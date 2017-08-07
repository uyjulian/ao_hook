#include "mylibrary.h"

ML_NAMESPACE

ML_NAMESPACE_BEGIN(Native)



ML_NAMESPACE_BEGIN(Ps);

using namespace Mm;
using namespace Ldr;

ULONG_PTR HandleToProcessId(HANDLE Process)
{
#if ML_USER_MODE

    NTSTATUS Status;
    PROCESS_BASIC_INFORMATION Basic;

    Status = NtQueryInformationProcess(Process, ProcessBasicInformation, &Basic, sizeof(Basic), nullptr);

    return NT_SUCCESS(Status) ? Basic.UniqueProcessId : INVALID_CLIENT_ID;

#else // r0
#endif
}

ULONG_PTR HandleToThreadId(HANDLE Thread)
{
#if ML_USER_MODE

    NTSTATUS Status;
    THREAD_BASIC_INFORMATION Basic;

    Status = NtQueryInformationThread(Thread, ThreadBasicInformation, &Basic, sizeof(Basic), nullptr);

    return NT_SUCCESS(Status) ? (ULONG_PTR)Basic.ClientId.UniqueThread : INVALID_CLIENT_ID;

#else // r0
#endif
}

NTSTATUS ProcessIdToHandleEx(PHANDLE ProcessHandle, ULONG_PTR ProcessId, ULONG_PTR Access /* = PROCESS_ALL_ACCESS */)
{
#if ML_USER_MODE

    NTSTATUS            Status;
    OBJECT_ATTRIBUTES   ObjectAttributes;
    CLIENT_ID           CliendID;

    CliendID.UniqueThread   = nullptr;
    CliendID.UniqueProcess  = (HANDLE)ProcessId;
    InitializeObjectAttributes(&ObjectAttributes, nullptr, 0, nullptr, nullptr);

    Status = NtOpenProcess(ProcessHandle, Access, &ObjectAttributes, &CliendID);

    return Status;

#else // r0

    PEPROCESS   Process;
    NTSTATUS    Status;

    Status = PsLookupProcessByProcessId((HANDLE)ProcessId, &Process);
    FAIL_RETURN(Status);

    Status = ObOpenObjectByPointer(Process, 0, nullptr, Access, *PsProcessType, KernelMode, ProcessHandle);
    ObDereferenceObject(Process);

    return Status;

#endif
}

HANDLE ProcessIdToHandle(ULONG_PTR ProcessId, ULONG_PTR Access /* = PROCESS_ALL_ACCESS */)
{
    NTSTATUS    Status;
    HANDLE      ProcessHandle;

    Status = ProcessIdToHandleEx(&ProcessHandle, ProcessId, Access);
    return NT_SUCCESS(Status) ? ProcessHandle : nullptr;
}

NTSTATUS ThreadIdToHandleEx(PHANDLE ThreadHandle, ULONG_PTR ThreadId, ULONG_PTR Access)
{
#if ML_USER_MODE

    NTSTATUS            Status;
    OBJECT_ATTRIBUTES   ObjectAttributes;
    CLIENT_ID           CliendID;

    CliendID.UniqueProcess  = NULL;
    CliendID.UniqueThread   = (HANDLE)ThreadId;
    InitializeObjectAttributes(&ObjectAttributes, NULL, NULL, NULL, NULL);

    return NtOpenThread(ThreadHandle, Access, &ObjectAttributes, &CliendID);

#else // r0

    PETHREAD    Thread;
    HANDLE      Handle;
    NTSTATUS    Status;

    Status = PsLookupThreadByThreadId((HANDLE)ThreadId, &Thread);
    FAIL_RETURN(Status);

    Status = ObOpenObjectByPointer(Thread, 0, nullptr, Access, *PsThreadType, KernelMode, ThreadHandle);
    ObDereferenceObject(Thread);

    return Status;

#endif
}

HANDLE ThreadIdToHandle(ULONG_PTR ThreadId, ULONG_PTR Access)
{
    NTSTATUS Status;
    HANDLE Handle;

    Status = ThreadIdToHandleEx(&Handle, ThreadId, Access);
    return NT_SUCCESS(Status) ? Handle : nullptr;
}

ULONG_PTR GetSessionId(HANDLE Process)
{
    NTSTATUS Status;
    PROCESS_SESSION_INFORMATION Session;

#if ML_KERNEL_MODE

    Status = ZwQueryInformationProcess(Process, ProcessSessionInformation, &Session, sizeof(Session), NULL);

#else

    Status = NtQueryInformationProcess(Process, ProcessSessionInformation, &Session, sizeof(Session), NULL);

#endif

    return NT_FAILED(Status) ? INVALID_SESSION_ID : Session.SessionId;
}

ULONG_PTR GetSessionId(ULONG_PTR ProcessId)
{
    ULONG_PTR   SessionId;
    HANDLE      Process;

    Process = PidToHandle(ProcessId);
    if (Process == NULL)
        return INVALID_SESSION_ID;

    SessionId = GetSessionId(Process);

#if ML_KERNEL_MODE

    ZwClose(Process);

#else

    NtClose(Process);

#endif

    return SessionId;
}

BOOL IsWow64Process(HANDLE Process)
{
    PVOID       Wow64Information;
    NTSTATUS    Status;

    API_POINTER(ZwQueryInformationProcess)  XQueryInformationProcess;

#if ML_KERNEL_MODE

    XQueryInformationProcess = ZwQueryInformationProcess;

#else

    XQueryInformationProcess = NtQueryInformationProcess;

#endif

    Status = XQueryInformationProcess(
                Process,
                ProcessWow64Information,
                &Wow64Information,
                sizeof(Wow64Information),
                NULL
             );

    return NT_SUCCESS(Status) && Wow64Information != NULL;
}

PSYSTEM_PROCESS_INFORMATION QuerySystemProcesses()
{
    ULONG                       Size, Length;
    NTSTATUS                    Status;
    PSYSTEM_PROCESS_INFORMATION ProcessInfo;

    ProcessInfo = NULL;
    Size = 0;

    LOOP_FOREVER
    {
#if ML_KERNEL_MODE

        Status = ZwQuerySystemInformation(SystemProcessInformation, ProcessInfo, Size, &Length);
        if (Status != STATUS_INFO_LENGTH_MISMATCH)
            break;

        FreeMemoryP(ProcessInfo);
        ProcessInfo = (PSYSTEM_PROCESS_INFORMATION)AllocateMemoryP(Length);

#else
        Status = NtQuerySystemInformation(SystemProcessInformation, ProcessInfo, Size, &Length);
        if (Status != STATUS_INFO_LENGTH_MISMATCH)
            break;

        ProcessInfo = (PSYSTEM_PROCESS_INFORMATION)ReAllocateMemoryP(ProcessInfo, Length);

#endif

        if (ProcessInfo == NULL)
            return NULL;

        Size = Length;
    }

    if (!NT_SUCCESS(Status))
    {
        FreeMemoryP(ProcessInfo);
        ProcessInfo = NULL;
    }

    return ProcessInfo;
}

#if ML_X86

NAKED VOID CDECL CaptureRegisters(PML_THREAD_CONTEXT Context)
{
    INLINE_ASM
    {
        push    ebx;
        push    ecx;
        mov     ebx, [esp + 0Ch];
        mov     ecx, [ebx]ML_THREAD_CONTEXT.Flags;
        pushad;

        mov     edx, [esp + 020h];
        mov     eax, [esp + 024h];
        mov     [esp]PUSHAD_REGISTER.Rcx, edx;
        mov     [esp]PUSHAD_REGISTER.Rbx, eax;

        lea     edi, [ebx]ML_THREAD_CONTEXT.Rdi;
        lea     esi, [esp];

        and     ecx, (1 << 8) - 1;

        jecxz   LOOP_END;

_LOOP:
        shr     ecx, 1;
        jnc     FLAG_NOT_SET;

        mov     eax, [esi];
        mov     [edi], eax;

FLAG_NOT_SET:
        lea     esi, [esi + 04h];
        lea     edi, [edi + 04h];
        jnz     _LOOP;

LOOP_END:

        add     [ebx]ML_THREAD_CONTEXT.Rsp, 010h;

        popad;
        pop     ecx;
        pop     ebx;
        ret;
    }
}

VOID CDECL RegistersContext(PML_THREAD_CONTEXT Context)
{
    ;
}

#endif // x86

VOID Sleep(ULONG_PTR Milliseconds, BOOL Alertable)
{
    NTSTATUS Status;
    LARGE_INTEGER Timeout;

    FormatTimeOut(&Timeout, Milliseconds);

    LOOP_FOREVER
    {

#if ML_USER_MODE
        Status = NtDelayExecution((BOOLEAN)Alertable, &Timeout);
#else
        Status = KeDelayExecutionThread(KernelMode, Alertable, &Timeout);
#endif

        if (Milliseconds != INFINITE)
            break;

        if (Alertable == ThreadNoAlertable)
            break;

        if (Status != STATUS_USER_APC && Status != STATUS_ALERTED)
            break;
    }
}

#if ML_USER_MODE

PTEB_ACTIVE_FRAME
FindThreadFrame(
    ULONG_PTR Context
)
{
    PTEB_ACTIVE_FRAME Frame;

    Frame = RtlGetFrame();
    while (Frame != NULL && Frame->Context != Context)
        Frame = Frame->Previous;

    return Frame;
}

PTEB_ACTIVE_FRAME
FindThreadFrameEx(
    ULONG_PTR Context,
    ULONG_PTR Data
)
{
    PTEB_ACTIVE_FRAME Frame;

    Frame = RtlGetFrame();
    while (Frame != nullptr && Frame->Context != Context && Frame->Data != Data)
        Frame = Frame->Previous;

    return Frame;
}

NTSTATUS
CreateProcess(
    PCWSTR                  ApplicationName,
    PWSTR                   CommandLine,
    PCWSTR                  CurrentDirectory,
    ULONG                   CreationFlags,
    LPSTARTUPINFOW          StartupInfo,
    LPPROCESS_INFORMATION   ProcessInformation,
    LPSECURITY_ATTRIBUTES   ProcessAttributes,
    LPSECURITY_ATTRIBUTES   ThreadAttributes,
    PVOID                   Environment,
    HANDLE                  Token,
    BOOL                    InheritHandles
)
{
    // BOOL                Result;
    // STARTUPINFOW        DefaultStartupInfo;
    // PROCESS_INFORMATION DefaultProcessInformation;

    // if (StartupInfo == NULL)
    // {
    //     ZeroMemory(&DefaultStartupInfo, sizeof(DefaultStartupInfo));
    //     DefaultStartupInfo.cb = sizeof(DefaultStartupInfo);
    //     StartupInfo = &DefaultStartupInfo;
    // }

    // if (ProcessInformation == NULL)
    //     ProcessInformation = &DefaultProcessInformation;

    // Result = CreateProcessInternalW(
    //                 Token,
    //                 ApplicationName,
    //                 CommandLine,
    //                 ProcessAttributes,
    //                 ThreadAttributes,
    //                 InheritHandles,
    //                 CreationFlags,
    //                 Environment,
    //                 CurrentDirectory,
    //                 StartupInfo,
    //                 ProcessInformation,
    //                 NULL
    //          );

    // if (!Result)
    //     return ML_NTSTATUS_FROM_WIN32(RtlGetLastWin32Error());

    // if (ProcessInformation == &DefaultProcessInformation)
    // {
    //     NtClose(DefaultProcessInformation.hProcess);
    //     NtClose(DefaultProcessInformation.hThread);
    // }

    return STATUS_SUCCESS;
}

#if !ML_AMD64

NTSTATUS InjectDllBeforeKernel32Loaded(PML_PROCESS_INFORMATION ProcInfo, PCWSTR DllPath, ULONG CreationFlags)
{
    NTSTATUS                Status;
    ULONG_PTR               DllLoadCount, Offset, UndefinedInstruction, LdrLoadDllFirstDword;
    PLDR_MODULE             Ntdll;
    PVOID                   InjectBuffer, ReloadedNtdll, LdrLoadDll;
    HANDLE                  Debuggee;
    DBGUI_WAIT_STATE_CHANGE DbgState;
    PEXCEPTION_RECORD       ExceptionRecord;

    Ntdll = GetNtdllLdrModule();
    Status = LoadPeImage(Ntdll->FullDllName.Buffer, &ReloadedNtdll, NULL, LOAD_PE_IGNORE_RELOC);
    FAIL_RETURN(Status);

    LdrLoadDll = PtrAdd(EATLookupRoutineByHashPNoFix(ReloadedNtdll, NTDLL_LdrLoadDll), PtrOffset(Ntdll->DllBase, ReloadedNtdll));

    UnloadPeImage(ReloadedNtdll);

    Debuggee = ProcInfo->hProcess;

    Status = ReadMemory(Debuggee, LdrLoadDll, &LdrLoadDllFirstDword, sizeof(LdrLoadDllFirstDword));
    FAIL_RETURN(Status);

    UndefinedInstruction = 0x0B0F;   // ud2
    Status = WriteProtectMemory(Debuggee, LdrLoadDll, &UndefinedInstruction, 2);
    FAIL_RETURN(Status);

    if (FLAG_ON(CreationFlags, CREATE_SUSPENDED))
        NtResumeProcess(Debuggee);

    Status = DbgUiWaitStateChange(&DbgState, NULL);
    FAIL_RETURN(Status);

    DllLoadCount = 0;

    auto DbgX = [&] () -> NTSTATUS
    {
        NTSTATUS Status;

        Status = DbgUiContinue(&DbgState.AppClientId, DBG_CONTINUE);
        FAIL_RETURN(Status);

        Status = DbgUiWaitStateChange(&DbgState, NULL);
        return Status;
    };

    for (; ; Status = DbgX())
    {
        FAIL_RETURN(Status);

        switch (DbgState.NewState)
        {
            case DbgExceptionStateChange:
                break;

            case DbgCreateThreadStateChange:
                if (DbgState.StateInfo.CreateThread.HandleToThread != NULL)
                    NtClose(DbgState.StateInfo.CreateThread.HandleToThread);

                continue;

            case DbgCreateProcessStateChange:
                if (DbgState.StateInfo.CreateProcessInfo.HandleToProcess != NULL)
                    NtClose(DbgState.StateInfo.CreateProcessInfo.HandleToProcess);

                if (DbgState.StateInfo.CreateProcessInfo.HandleToThread != NULL)
                    NtClose(DbgState.StateInfo.CreateProcessInfo.HandleToThread);

                if (DbgState.StateInfo.CreateProcessInfo.NewProcess.FileHandle != NULL)
                    NtClose(DbgState.StateInfo.CreateProcessInfo.NewProcess.FileHandle);

                continue;

            case DbgLoadDllStateChange:
            LOOP_ONCE
            {
                if (DbgState.StateInfo.LoadDll.FileHandle != NULL)
                    NtClose(DbgState.StateInfo.LoadDll.FileHandle);

                if (DllLoadCount != 0)
                    break;

                ++DllLoadCount;

                BOOLEAN                 BeingDebugged;
                PPEB_BASE               Peb;
                PVOID                   FsBase;
                CONTEXT                 Context;
                HANDLE                  Thread;
                DESCRIPTOR_TABLE_ENTRY  Descriptor;

                Thread = TidToHandle(DbgState.AppClientId.UniqueThread);
                if (Thread == NULL)
                    break;

                Context.ContextFlags = CONTEXT_SEGMENTS;
                Status = NtGetContextThread(Thread, &Context);
                if (NT_FAILED(Status))
                {
                    NtClose(Thread);
                    break;
                }

                Descriptor.Selector = Context.SegFs;
                Status = NtQueryInformationThread(Thread, ThreadDescriptorTableEntry, &Descriptor, sizeof(Descriptor), NULL);
                NtClose(Thread);
                FAIL_BREAK(Status);

                FsBase = (PVOID)((Descriptor.Descriptor.HighWord.Bits.BaseHi << 24)  |
                                (Descriptor.Descriptor.HighWord.Bits.BaseMid << 16) |
                                (Descriptor.Descriptor.BaseLow));

                if (FsBase == NULL)
                    break;

                Peb = (PPEB_BASE)PtrAdd(FsBase, PEB_OFFSET);

                Status = ReadMemory(Debuggee, Peb, &Peb, sizeof(Peb));
                FAIL_BREAK(Status);

                // ReadMemory(Debuggee, &Peb->BeingDebugged, &BeingDebugged, sizeof(BeingDebugged));

                BeingDebugged = FALSE;
                WriteMemory(Debuggee, &Peb->BeingDebugged, &BeingDebugged, sizeof(BeingDebugged));
            }
            continue;

            default:
                continue;
        }

        ExceptionRecord = &DbgState.StateInfo.Exception.ExceptionRecord;
        if (ExceptionRecord->ExceptionCode != STATUS_ILLEGAL_INSTRUCTION)
            continue;

        if (ExceptionRecord->ExceptionAddress != LdrLoadDll)
            continue;

        break;
    }

    Status = WriteProtectMemory(Debuggee, LdrLoadDll, &LdrLoadDllFirstDword, sizeof(LdrLoadDllFirstDword));
    FAIL_RETURN(Status);

    PVOID           CallIp, CallNextIp;
    CONTEXT         Context;
    UNICODE_STRING  Dll;
    BYTE            CallOp;

    Context.ContextFlags = CONTEXT_CONTROL;
    Status = NtGetContextThread(ProcInfo->hThread, &Context);
    FAIL_RETURN(Status);


#if ML_X86

    Status = ReadMemory(Debuggee, (PVOID)Context.Esp, &CallNextIp, sizeof(ProcInfo->FirstCallLdrLoadDll));

#elif ML_AMD64

    Status = ReadMemory(Debuggee, (PVOID)Context.Rsp, &CallNextIp, sizeof(ProcInfo->FirstCallLdrLoadDll));

#endif

    FAIL_RETURN(Status);

    CallIp = PtrSub(CallNextIp, 5);
    Status = ReadMemory(Debuggee, CallIp, &CallOp, sizeof(CallOp));
    FAIL_RETURN(Status);

    if (CallOp != 0xE8)
        return STATUS_NOT_SUPPORTED;

    ProcInfo->FirstCallLdrLoadDll = CallIp;

    RtlInitUnicodeString(&Dll, DllPath);

    Status = InjectDllToRemoteProcess(
                Debuggee,
                ProcInfo->hThread,
                &Dll,
                INJECT_THREAD_SUSPENDED,
                &InjectBuffer
             );

    FAIL_RETURN(Status);

    Status = DbgUiContinue(&DbgState.AppClientId, DBG_CONTINUE);
    FAIL_RETURN(Status);

    if (FLAG_ON(CreationFlags, CREATE_SUSPENDED))
        NtSuspendProcess(Debuggee);

    Status = DbgUiStopDebugging(Debuggee);

    return Status;
}

NTSTATUS
CreateProcessWithDll(
    ULONG_PTR               Flags,
    PCWSTR                  DllPath,
    PCWSTR                  ApplicationName,
    PWSTR                   CommandLine,
    PCWSTR                  CurrentDirectory,
    ULONG                   CreationFlags,
    LPSTARTUPINFOW          StartupInfo,
    PML_PROCESS_INFORMATION ProcessInformation,
    LPSECURITY_ATTRIBUTES   ProcessAttributes,
    LPSECURITY_ATTRIBUTES   ThreadAttributes,
    PVOID                   Environment,
    HANDLE                  Token
)
{
    if (Flags == CPWD_NORMAL)
        return CreateProcess(ApplicationName, CommandLine, CurrentDirectory, CreationFlags, StartupInfo, ProcessInformation, ProcessAttributes, ThreadAttributes, Environment, Token);

    NTSTATUS                Status;
    ML_PROCESS_INFORMATION  ProcInfo;

    Status = STATUS_SUCCESS;

    switch (Flags)
    {
        case CPWD_BEFORE_KERNEL32:
            Status = CreateProcess(ApplicationName, CommandLine, CurrentDirectory, CreationFlags | DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS, StartupInfo, &ProcInfo, ProcessAttributes, ThreadAttributes, Environment, Token);
            FAIL_RETURN(Status);

            Status = InjectDllBeforeKernel32Loaded(&ProcInfo, DllPath, CreationFlags);
            break;

        default:
            return STATUS_INVALID_PARAMETER;
    }

    if (NT_FAILED(Status))
    {
        NtTerminateProcess(ProcInfo.hProcess, Status);
        NtClose(ProcInfo.hProcess);
        NtClose(ProcInfo.hThread);
    }
    else if (ProcessInformation == NULL)
    {
        NtClose(ProcInfo.hProcess);
        NtClose(ProcInfo.hThread);
    }
    else
    {
        *ProcessInformation = ProcInfo;
    }

    return Status;
}

#endif // AMD64

NTSTATUS
CreateThread(
    PVOID                   StartAddress,
    PVOID                   StartParameter,
    BOOL                    CreateSuspended,
    HANDLE                  ProcessHandle,
    PHANDLE                 ThreadHandle,
    PCLIENT_ID              ClientID,
    ULONG                   ZeroBits,
    ULONG_PTR               MaximumStackSize,
    ULONG_PTR               CommittedStackSize,
    PSECURITY_DESCRIPTOR    SecurityDescriptor
)
{
    NTSTATUS    Status;
    CLIENT_ID   ThreadId;
    HANDLE      NewThreadHandle;

    Status = RtlCreateUserThread(
                ProcessHandle,
                SecurityDescriptor,
                (BOOLEAN)CreateSuspended,
                ZeroBits,
                MaximumStackSize,
                CommittedStackSize,
                (PUSER_THREAD_START_ROUTINE)StartAddress,
                StartParameter,
                &NewThreadHandle,
                &ThreadId
             );
    if (!NT_SUCCESS(Status))
        return Status;

    if (ClientID != NULL)
        *ClientID = ThreadId;

    if (ThreadHandle != NULL)
    {
        *ThreadHandle = NewThreadHandle;
    }
    else
    {
        NtClose(NewThreadHandle);
    }

    return Status;
}

DECL_NORETURN
VOID
ExitProcess(
    NTSTATUS ExitStatus
)
{
    PVOID       LoaderLockCookie;
    NTSTATUS    Status;
    HANDLE      ProcessHeap = CurrentPeb()->ProcessHeap;

    Status = LdrLockLoaderLock(0, NULL, &LoaderLockCookie);
    RtlAcquirePebLock();
    RtlLockHeap(ProcessHeap);

    NtTerminateProcess(NULL, ExitStatus);

    RtlUnlockHeap(ProcessHeap);
    RtlReleasePebLock();

    if (NT_SUCCESS(Status))
        LdrUnlockLoaderLock(0, LoaderLockCookie);

    LdrShutdownProcess();
    NtTerminateProcess(CurrentProcess, ExitStatus);
}

#endif // r3

ML_NAMESPACE_END_(Ps);


ML_NAMESPACE_END_(Native)

ML_NAMESPACE_END
