#ifndef _MLNTPROCESS_H_ea409cd9_d592_4611_ba82_ef00242f7ead_
#define _MLNTPROCESS_H_ea409cd9_d592_4611_ba82_ef00242f7ead_


ML_NAMESPACE_BEGIN(Ps);

static const HANDLE CurrentProcess = NtCurrentProcess();
static const HANDLE CurrentThread = NtCurrentThread();

#define INVALID_SESSION_ID  ULONG_PTR_MAX
#define INVALID_CLIENT_ID   ULONG_PTR_MAX

ForceInline ULONG_PTR CurrentPid()
{
    return (ULONG_PTR)Nt_CurrentTeb()->ClientId.UniqueProcess;
}

ForceInline ULONG_PTR CurrentTid()
{
    return (ULONG_PTR)Nt_CurrentTeb()->ClientId.UniqueThread;
}

ForceInline PPEB_BASE CurrentPeb()
{
    return Nt_CurrentPeb();
}

ForceInline PTEB_BASE CurrentTeb()
{
    return Nt_CurrentTeb();
}

ULONG_PTR
HandleToProcessId(
    HANDLE Process
);

ULONG_PTR
HandleToThreadId(
    HANDLE Thread
);

HANDLE
ProcessIdToHandle(
    ULONG_PTR ProcessId,
    ULONG_PTR Access = PROCESS_ALL_ACCESS
);

NTSTATUS
ProcessIdToHandleEx(
    PHANDLE     ProcessHandle,
    ULONG_PTR   ProcessId,
    ULONG_PTR   Access = PROCESS_ALL_ACCESS
);

HANDLE
ThreadIdToHandle(
    ULONG_PTR ThreadId,
    ULONG_PTR Access = THREAD_ALL_ACCESS
);

NTSTATUS
ThreadIdToHandleEx(
    PHANDLE     ThreadHandle,
    ULONG_PTR   ThreadId,
    ULONG_PTR   Access = THREAD_ALL_ACCESS
);

ForceInline ULONG_PTR HandleToPid(HANDLE Process)
{
    return HandleToProcessId(Process);
}

ForceInline ULONG_PTR HandleToTid(HANDLE Process)
{
    return HandleToThreadId(Process);
}

ULONG_PTR
HandleToThreadId(
    HANDLE Thread
);

template<class PidType>
ForceInline NTSTATUS PidToHandleEx(PHANDLE ProcessHandle, PidType ProcessId, ULONG_PTR Access = PROCESS_ALL_ACCESS)
{
    return ProcessIdToHandleEx(ProcessHandle, (ULONG_PTR)ProcessId, Access);
}

ForceInline HANDLE ProcessIdToHandle(HANDLE ProcessId, ULONG_PTR Access = PROCESS_ALL_ACCESS)
{
    return ProcessIdToHandle((ULONG_PTR)ProcessId, Access);
}

template<class PidType>
ForceInline HANDLE PidToHandle(PidType ProcessId, ULONG_PTR Access = PROCESS_ALL_ACCESS)
{
    return ProcessIdToHandle((ULONG_PTR)ProcessId, Access);
}

ForceInline HANDLE ThreadIdToHandle(HANDLE ThreadId, ULONG_PTR Access = THREAD_ALL_ACCESS)
{
    return ThreadIdToHandle((ULONG_PTR)ThreadId, Access);
}

template<class TidType>
ForceInline HANDLE TidToHandle(TidType ThreadId, ULONG_PTR Access = THREAD_ALL_ACCESS)
{
    return ThreadIdToHandle((ULONG_PTR)ThreadId, Access);
}

ULONG_PTR
GetSessionId(
    HANDLE Process
);

ULONG_PTR
GetSessionId(
    ULONG_PTR ProcessId
);

inline ULONG_PTR GetCurrentSessionId()
{
    return GetSessionId(CurrentProcess);
}

BOOL
IsWow64Process(
    HANDLE Process = CurrentProcess
);

PSYSTEM_PROCESS_INFORMATION QuerySystemProcesses();

typedef struct ML_THREAD_CONTEXT
{
    union
    {
        struct
        {
            BOOLEAN Rdi : 1;
            BOOLEAN Rsi : 1;
            BOOLEAN Rbp : 1;
            BOOLEAN Rsp : 1;
            BOOLEAN Rbx : 1;
            BOOLEAN Rdx : 1;
            BOOLEAN Rcx : 1;
            BOOLEAN Rax : 1;
        };

        ULONG_PTR Value;

    } Flags;

    ULONG_PTR Rdi;
    ULONG_PTR Rsi;
    ULONG_PTR Rbp;
    ULONG_PTR Rsp;
    ULONG_PTR Rbx;
    ULONG_PTR Rdx;
    ULONG_PTR Rcx;
    ULONG_PTR Rax;

    ML_THREAD_CONTEXT()
    {
        Flags.Value = 0;
    }

} ML_THREAD_CONTEXT, *PML_THREAD_CONTEXT;

VOID
CDECL
CaptureRegisters(
    PML_THREAD_CONTEXT Context
);

VOID
CDECL
SetRegisters(
    PML_THREAD_CONTEXT Context
);

enum
{
    ThreadNoAlertable   = FALSE,
    ThreadAlertable     = TRUE,
};

VOID
Sleep(
    ULONG_PTR   Milliseconds,
    BOOL        Alertable = ThreadNoAlertable
);

#if ML_USER_MODE

PTEB_ACTIVE_FRAME
FindThreadFrame(
    ULONG_PTR Context
);

PTEB_ACTIVE_FRAME
FindThreadFrameEx(
    ULONG_PTR Context,
    ULONG_PTR Data
);

NTSTATUS
CreateProcess(
    PCWSTR                  ApplicationName,
    PWSTR                   CommandLine         = NULL,
    PCWSTR                  CurrentDirectory    = NULL,
    ULONG                   CreationFlags       = 0,
    LPSTARTUPINFOW          StartupInfo         = NULL,
    LPPROCESS_INFORMATION   ProcessInformation  = NULL,
    LPSECURITY_ATTRIBUTES   ProcessAttributes   = NULL,
    LPSECURITY_ATTRIBUTES   ThreadAttributes    = NULL,
    PVOID                   Environment         = NULL,
    HANDLE                  Token               = NULL,
    BOOL                    InheritHandles      = FALSE
);


#define CPWD_NORMAL                 0
#define CPWD_BEFORE_KERNEL32        (1 << 0)
#define CPWD_BEFORE_ENTRY_POINT     (1 << 1)
#define CPWD_BEFORE_TLS_CALLBACK    (1 << 2)

typedef struct ML_PROCESS_INFORMATION : public PROCESS_INFORMATION
{
    PVOID FirstCallLdrLoadDll;

} ML_PROCESS_INFORMATION, *PML_PROCESS_INFORMATION;

NTSTATUS
CreateProcessWithDll(
    ULONG_PTR               Flags,
    PCWSTR                  DllPath,
    PCWSTR                  ApplicationName,
    PWSTR                   CommandLine         = NULL,
    PCWSTR                  CurrentDirectory    = NULL,
    ULONG                   CreationFlags       = 0,
    LPSTARTUPINFOW          StartupInfo         = NULL,
    PML_PROCESS_INFORMATION ProcessInformation  = NULL,
    LPSECURITY_ATTRIBUTES   ProcessAttributes   = NULL,
    LPSECURITY_ATTRIBUTES   ThreadAttributes    = NULL,
    PVOID                   Environment         = NULL,
    HANDLE                  Token               = NULL
);

NTSTATUS
CreateThread(
    PVOID                   StartAddress,
    PVOID                   StartParameter      = NULL,
    BOOL                    CreateSuspended     = FALSE,
    HANDLE                  ProcessHandle       = Ps::CurrentProcess,
    PHANDLE                 ThreadHandle        = NULL,
    PCLIENT_ID              ClientID            = NULL,
    ULONG                   ZeroBits            = 0,
    ULONG_PTR               MaximumStackSize    = 0,
    ULONG_PTR               CommittedStackSize  = 0,
    PSECURITY_DESCRIPTOR    SecurityDescriptor  = NULL
);

DECL_NORETURN
VOID
ExitProcess(
    NTSTATUS ExitStatus
);

inline PWSTR GetCommandLine()
{
    return CurrentPeb()->ProcessParameters->CommandLine.Buffer;
}

#define ThreadCallbackM(...) (PTHREAD_START_ROUTINE)(LambdaCastHelper<ULONG(NTAPI *)(__VA_ARGS__)>::FUNC)[] (__VA_ARGS__) -> ULONG

#define ThreadLambdaType(_type, ...) [] (_type __VA_ARGS__) -> ULONG
#define ThreadLambdaType_(_type, ...)  (PTHREAD_START_ROUTINE)ThreadLambdaType(_type, __VA_ARGS__)
#define ThreadLambda(...) ThreadLambdaType(ULONG_PTR, __VA_ARGS__)

template<typename F, typename P>
NTSTATUS
CreateThreadT(
    const F&                StartAddress,
    P                       StartParameter      = nullptr,
    BOOL                    CreateSuspended     = FALSE,
    HANDLE                  ProcessHandle       = Ps::CurrentProcess,
    PHANDLE                 ThreadHandle        = nullptr,
    PCLIENT_ID              ClientID            = nullptr,
    ULONG                   StackZeroBits       = 0,
    ULONG                   StackReserved       = 0,
    ULONG                   StackCommit         = 0,
    PSECURITY_DESCRIPTOR    SecurityDescriptor  = nullptr
)
{
    NTSTATUS Status;

    typedef struct StartParameterWrapper
    {
        ml::Function<ULONG(P)> func;
        P StartParameter;

        StartParameterWrapper(const F &StartAddress, const P &Parameter) : func(StartAddress), StartParameter(Parameter)
        {
        }

    } StartParameterWrapper;

    StartParameterWrapper *func;

    func = new StartParameterWrapper(StartAddress, StartParameter);
    if (func == NULL)
        return STATUS_NO_MEMORY;

    Status = CreateThread(PTHREAD_START_ROUTINE(
                [] (PVOID Function) -> ULONG
                {
                    ULONG RetValue;
                    StartParameterWrapper *func = (StartParameterWrapper *)Function;

                    RetValue = (func->func)(func->StartParameter);

                    delete func;

                    return RetValue;
                }),
                func,
                CreateSuspended, ProcessHandle, ThreadHandle, ClientID, StackZeroBits, StackReserved, StackCommit, SecurityDescriptor
            );

    if (NT_FAILED(Status))
    {
        delete func;
    }

    return Status;
}

template<typename F>
NTSTATUS QueueApcRoutine(const F &ApcFunction, HANDLE Thread = CurrentThread)
{
    NTSTATUS Status;
    ml::Function<void()> *func;

    func = new ml::Function<void()>(ApcFunction);
    if (func == NULL)
        return STATUS_NO_MEMORY;

    Status = NtQueueApcThread(Thread,
                [] (PVOID ApcFunction, PVOID, PVOID) -> VOID
                {
                    ml::Function<void()> *func = (ml::Function<void()> *)ApcFunction;

                    (*func)();

                    delete func;
                },
                func,
                NULL,
                NULL
            );

    if (NT_FAILED(Status))
    {
        delete func;
    }

    return Status;
}

#else // r0

#define LambdaKernelApc(Apc) [] (PKAPC Apc, PKNORMAL_ROUTINE* NormalRoutine, PVOID* NormalContext, PVOID* SystemArgument1, PVOID* SystemArgument2) -> VOID
#define LambdaNormalApc(NormalContext) [] (PVOID NormalContext, PVOID, PVOID) -> VOID
#define NormalApcCallbackM(...) (PKNORMAL_ROUTINE)(LambdaCastHelper<VOID(NTAPI *)(__VA_ARGS__, PVOID, PVOID)>::FUNC)[] (__VA_ARGS__, PVOID, PVOID) -> VOID

typedef struct
{
    KAPC                Apc;
    PKKERNEL_ROUTINE    Callback;
    PVOID               Argument1;
    PVOID               Argument2;

} ML_KERNEL_APC, *PML_KERNEL_APC;

inline
NTSTATUS
QueueNormalApc(
    PETHREAD            Thread,
    PKNORMAL_ROUTINE    NormalRoutine,
    PVOID               NormalContext = nullptr,
    KPRIORITY           Increment = IO_NO_INCREMENT
)
{
    PKAPC       Apc;
    NTSTATUS    Status;

    Apc = new KAPC;
    if (Apc == nullptr)
        return STATUS_NO_MEMORY;

    KeInitializeApc(Apc, Thread, OriginalApcEnvironment,
        LambdaKernelApc(Apc)
        {
            delete Apc;
        },
        nullptr,
        NormalRoutine,
        KernelMode,
        NormalContext
    );

    if (KeInsertQueueApc(Apc, Apc, nullptr, Increment) == FALSE)
    {
        delete Apc;
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

inline
NTSTATUS
QueueNormalApc(
    HANDLE              ThreadId,
    PKNORMAL_ROUTINE    NormalRoutine,
    PVOID               NormalContext = nullptr,
    KPRIORITY           Increment = IO_NO_INCREMENT
)
{
    NTSTATUS Status;
    PETHREAD Thread;

    Status = PsLookupThreadByThreadId(ThreadId, &Thread);
    FAIL_RETURN(Status);

    Status = QueueNormalApc(Thread, NormalRoutine, NormalContext, Increment);
    ObDereferenceObject(Thread);

    return Status;
}

inline PETHREAD TidToThreadObject(HANDLE ThreadId)
{
    PETHREAD Thread;

    return NT_FAILED(PsLookupThreadByThreadId(ThreadId, &Thread)) ? nullptr : Thread;
}

inline PEPROCESS PidToProcessObject(HANDLE ProcessId)
{
    PEPROCESS Process;

    return NT_FAILED(PsLookupProcessByProcessId(ProcessId, &Process)) ? nullptr : Process;
}

#endif // r0

ML_NAMESPACE_END_(Ps);

#endif // _MLNTPROCESS_H_ea409cd9_d592_4611_ba82_ef00242f7ead_