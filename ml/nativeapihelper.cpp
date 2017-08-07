#include "mylibrary.h"

#pragma warning(disable:4245)
#pragma warning(disable:4750)

_ML_C_HEAD_

/************************************************************************
  strings
************************************************************************/
ULONG
InternalCopyUnicodeString(
    PUNICODE_STRING Unicode,
    PWCHAR          Buffer,
    ULONG_PTR       BufferCount,
    BOOL            IsDirectory = FALSE
)
{
    ULONG_PTR Length;

    Length = MY_MIN(Unicode->Length, BufferCount * sizeof(WCHAR));
    CopyMemory(Buffer, Unicode->Buffer, Length);
    Length /= sizeof(WCHAR);

    if (IsDirectory && Length < BufferCount && Buffer[Length - 1] != '\\')
        Buffer[Length++] = '\\';

    if (Length < BufferCount)
        Buffer[Length] = 0;

    return Length;
}

NTSTATUS
FASTCALL
Nt_AnsiToUnicode(
    PWCHAR      UnicodeBuffer,
    ULONG_PTR   BufferCount,
    LPCSTR      AnsiString,
    LONG_PTR    AnsiLength,
    PULONG_PTR  BytesInUnicode
)
{
    return ml::Native::Nls::AnsiToUnicode(UnicodeBuffer, BufferCount, AnsiString, AnsiLength, BytesInUnicode);
}

NTSTATUS
Nt_UnicodeToAnsi(
    PCHAR       AnsiBuffer,
    ULONG_PTR   BufferCount,
    LPCWSTR     UnicodeString,
    LONG_PTR    UnicodeLength,
    PULONG_PTR  BytesInAnsi
)
{
    return ml::Native::Nls::UnicodeToAnsi(AnsiBuffer, BufferCount, UnicodeString, UnicodeLength, BytesInAnsi);
}

NTSTATUS
Nt_AnsiToUnicodeString(
    PUNICODE_STRING Unicode,
    PCHAR           AnsiString,
    LONG_PTR        AnsiLength,
    BOOL            AllocateDestinationString
)
{
    return ml::Native::Nls::AnsiToUnicodeString(Unicode, AnsiString, AnsiLength, AllocateDestinationString);
}

NTSTATUS
Nt_UnicodeToAnsiString(
    PANSI_STRING    Ansi,
    LPCWSTR         UnicodeString,
    LONG_PTR        UnicodeLength,
    BOOL            AllocateDestinationString
)
{
    return ml::Native::Nls::UnicodeToAnsiString(Ansi, UnicodeString, UnicodeLength, AllocateDestinationString);
}


/************************************************************************
  process api
************************************************************************/

BOOL
Nt_IsWow64Process(
    HANDLE Process /* = NtCurrentProcess() */
)
{
    PVOID       Wow64Information;
    NTSTATUS    Status;

    Status = ZwQueryInformationProcess(
                Process,
                ProcessWow64Information,
                &Wow64Information,
                sizeof(Wow64Information),
                NULL
             );

    return NT_SUCCESS(Status) && Wow64Information != NULL;
}

ULONG Nt_GetErrorMode()
{
    ULONG HardErrorMode;

    if (!NT_SUCCESS(NtQueryInformationProcess(NtCurrentProcess(), ProcessDefaultHardErrorMode, &HardErrorMode, sizeof(HardErrorMode), NULL)))
        HardErrorMode = 0;

    return HardErrorMode;
}

ULONG Nt_SetErrorMode(ULONG Mode)
{
    ULONG HardErrorMode, PreviousHardErrorMode;
    NTSTATUS Status;

    PreviousHardErrorMode = Nt_GetErrorMode();
    HardErrorMode = PreviousHardErrorMode | Mode;
    NtSetInformationProcess(NtCurrentProcess(), ProcessDefaultHardErrorMode, &HardErrorMode, sizeof(HardErrorMode));

    return PreviousHardErrorMode;
}

/************************************************************************
  common file io
************************************************************************/

NTSTATUS Nt_DeleteFile(PCWSTR FileName)
{
    return Io::DeleteFile(FileName);
}

/************************************************************************
  common path
************************************************************************/

ULONG_PTR
Nt_QueryDosDevice(
    PCWSTR      DeviceName,
    PWSTR       TargetPath,
    ULONG_PTR   PathSize
)
{
    NTSTATUS        Status;
    UNICODE_STRING  Target;

    Status = Io::QueryDosDevice(DeviceName, &Target);
    if (NT_FAILED(Status))
        return 0;

    PathSize = InternalCopyUnicodeString(&Target, TargetPath, PathSize);
    RtlFreeUnicodeString(&Target);

    return PathSize;
}

/************************************************************************
  common query
************************************************************************/

POBJECT_TYPES_INFORMATION QuerySystemObjectTypes()
{
    ULONG                       Size, Length;
    NTSTATUS                    Status;
    POBJECT_TYPES_INFORMATION   ObjectTypes, StackPointer;

    Size = sizeof(*ObjectTypes);
    ObjectTypes = (POBJECT_TYPES_INFORMATION)AllocStack(Size);
    StackPointer = ObjectTypes;

    LOOP_FOREVER
    {

#if ML_KERNEL_MODE

        Status = ZwQueryObject(NULL, ObjectTypesInformation, ObjectTypes, Size, &Length);
        if (Status != STATUS_INFO_LENGTH_MISMATCH)
            break;

        if (ObjectTypes == StackPointer)
            ObjectTypes = NULL;

        FreeMemoryP(ObjectTypes);
        ObjectTypes = (POBJECT_TYPES_INFORMATION)AllocateMemoryP(Length);

#else
        Status = NtQueryObject(NULL, ObjectTypesInformation, ObjectTypes, Size, &Length);
        if (Status != STATUS_INFO_LENGTH_MISMATCH)
            break;

        if (ObjectTypes == StackPointer)
            ObjectTypes = NULL;

        ObjectTypes = (POBJECT_TYPES_INFORMATION)ReAllocateMemoryP(ObjectTypes, Length);

#endif

        if (ObjectTypes == NULL)
            return NULL;

        Size = Length;
    }

    if (!NT_SUCCESS(Status))
    {
        FreeMemoryP(ObjectTypes);
        ObjectTypes = NULL;
    }

    return ObjectTypes;
}

PSYSTEM_HANDLE_INFORMATION_EX QuerySystemHandles()
{
    ULONG                           Size, Length;
    NTSTATUS                        Status;
    PSYSTEM_HANDLE_INFORMATION_EX   SystemHandles;

    Size = sizeof(*SystemHandles) * 2;

    SystemHandles = (PSYSTEM_HANDLE_INFORMATION_EX)AllocateMemoryP(Size);
    if (SystemHandles == nullptr)
        return nullptr;

    LOOP_FOREVER
    {

#if ML_KERNEL_MODE

        Status = ZwQuerySystemInformation(SystemExtendedHandleInformation, SystemHandles, Size, &Length);
        if (Status != STATUS_INFO_LENGTH_MISMATCH)
            break;

        FreeMemoryP(SystemHandles);
        SystemHandles = (PSYSTEM_HANDLE_INFORMATION_EX)AllocateMemoryP(Length);

#else

        Status = NtQuerySystemInformation(SystemExtendedHandleInformation, SystemHandles, Size, &Length);
        if (Status != STATUS_INFO_LENGTH_MISMATCH)
            break;

        SystemHandles = (PSYSTEM_HANDLE_INFORMATION_EX)ReAllocateMemoryP(SystemHandles, Length);

#endif

        if (SystemHandles == nullptr)
            return nullptr;

        Size = Length;
    }

    if (!NT_SUCCESS(Status))
    {
        FreeMemoryP(SystemHandles);
        SystemHandles = nullptr;
    }

    return SystemHandles;
}

BOOL ReleaseSystemInformation(PVOID Processes)
{
    return FreeMemoryP(Processes);
}

HANDLE QueryCsrssProcessId(ULONG_PTR Session)
{
    return NULL;
}

/************************************************************************
  common process
************************************************************************/

/************************************************************************
  common token
************************************************************************/

ULONG_PTR
Nt_GetSessionId(
    HANDLE Process
)
{
    return ml::Native::Ps::GetSessionId(Process);
}

#if ML_KERNEL_MODE

/************************************************************************
  kernel mode
************************************************************************/
PLDR_MODULE LookupPsLoadedModuleList(PLDR_MODULE LdrModule, PVOID CallDriverEntry)
{
    PLDR_MODULE PsLoadedModuleList;
    UNICODE_STRING NtName;

    RTL_CONST_STRING(NtName, L"ntoskrnl.exe");

    PsLoadedModuleList = LdrModule;
    do
    {
        PsLoadedModuleList = PsLoadedModuleList->NextLoadOrder();
/*
        if (PsLoadedModuleList->SizeOfImage != 0 && (PsLoadedModuleList->DllBase != NULL || PsLoadedModuleList->EntryPoint != NULL))
        {
            continue;
        }
*/
        if (!IN_RANGEEX(PsLoadedModuleList->DllBase, CallDriverEntry, PtrAdd(PsLoadedModuleList->DllBase, PsLoadedModuleList->SizeOfImage)))
            continue;

        if (!RtlEqualUnicodeString(&PsLoadedModuleList->BaseDllName, &NtName, TRUE))
            continue;

        return PsLoadedModuleList->PrevLoadOrder();

    } while (LdrModule != PsLoadedModuleList);

    return NULL;
}

NTSTATUS QuerySystemModuleByHandle(PVOID ImageBase, PRTL_PROCESS_MODULE_INFORMATION Module)
{
    NTSTATUS                        Status;
    ULONG                           ReturnedSize;
    ULONG_PTR                       Size, NumberOfModules;
    PRTL_PROCESS_MODULE_INFORMATION SystemModule;
    PRTL_PROCESS_MODULES            ModuleInfo;

    Size = 0;
    ModuleInfo = NULL;

    LOOP_FOREVER
    {
        Status = ZwQuerySystemInformation(SystemModuleInformation, ModuleInfo, Size, &ReturnedSize);
        if (Status != STATUS_INFO_LENGTH_MISMATCH)
            break;

        FreeMemory(ModuleInfo);
        ModuleInfo = (PRTL_PROCESS_MODULES)AllocateMemory(ReturnedSize - Size);
        Size = ReturnedSize;
    }

    SCOPE_EXIT
    {
        FreeMemory(ModuleInfo);
    }
    SCOPE_EXIT_END;

    if (!NT_SUCCESS(Status))
        return Status;

    if (ImageBase == NULL)
    {
        *Module = ModuleInfo->Modules[0];
        return STATUS_SUCCESS;
    }

    SystemModule = ModuleInfo->Modules;
    for (NumberOfModules = ModuleInfo->NumberOfModules; NumberOfModules != 0; ++SystemModule, --NumberOfModules)
    {
    	if (IN_RANGE((ULONG_PTR)SystemModule->ImageBase, (ULONG_PTR)ImageBase, (ULONG_PTR)SystemModule->ImageBase + SystemModule->ImageSize))
        {
            *Module = *SystemModule;
            return STATUS_SUCCESS;
        }
    }

    return STATUS_DLL_NOT_FOUND;
}

NTSTATUS QueryModuleNameByHandle(PVOID ImageBase, PUNICODE_STRING ModuleName)
{
    NTSTATUS                        Status;
    ANSI_STRING                     ModuleAnsiName;
    RTL_PROCESS_MODULE_INFORMATION  SystemModule;

    Status = QuerySystemModuleByHandle(ImageBase, &SystemModule);
    if (!NT_SUCCESS(Status))
        return Status;

    RtlInitAnsiString(&ModuleAnsiName, (PSTR)SystemModule.FullPathName);
    return RtlAnsiStringToUnicodeString(ModuleName, &ModuleAnsiName, TRUE);
}

NTSTATUS
KiQueueUserApc(
    PETHREAD            Thread,
    PKNORMAL_ROUTINE    ApcRoutine,
    PVOID               ApcRoutineContext,
    PVOID               Argument1,
    PVOID               Argument2
)
{
    PKAPC       Apc;
    NTSTATUS    Status;

    Apc = (PKAPC)AllocateMemoryP(sizeof(*Apc));
    if (Apc == NULL)
        return STATUS_NO_MEMORY;

    KeInitializeApc(
        Apc,
        Thread,
        OriginalApcEnvironment,
        [] (PKAPC Apc, PKNORMAL_ROUTINE *NormalRoutine, PVOID *NormalContext, PVOID *SystemArgument1, PVOID *SystemArgument2)
        {
            FreeMemoryP(Apc);
        },
        NULL,
        ApcRoutine,
        UserMode,
        ApcRoutineContext
    );

    Status = KeInsertQueueApc(Apc, Argument1, Argument2, 0);

    return Status ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}

#else   // r3

/************************************************************************
  user mode
************************************************************************/

/************************************************************************
  user token
************************************************************************/

NTSTATUS
Nt_GetTokenInfo(
    HANDLE                  TokenHandle,
    TOKEN_INFORMATION_CLASS TokenInformationClass,
    PVOID                   Information,
    PULONG_PTR              Size
)
{
    NTSTATUS Status;
    PVOID TokenInformation;
    ULONG Length;

    TokenInformation = NULL;
    Length = 0;

    LOOP_FOREVER
    {
        Status = NtQueryInformationToken(TokenHandle, TokenInformationClass, TokenInformation, Length, &Length);
        if (Status != STATUS_BUFFER_TOO_SMALL)
            break;

        TokenInformation = ReAllocateMemory(TokenInformation, Length);
    }

    if (NT_FAILED(Status))
    {
        FreeMemory(TokenInformation);
        TokenInformation = NULL;
    }
    else
    {
        *(PVOID *)Information = TokenInformation;
        if (Size != NULL)
            *Size=  Length;
    }

    return Status;
}

VOID ReleaseTokenInfo(PVOID TokenInfo)
{
    FreeMemory(TokenInfo);
}

NTSTATUS
Nt_SetProcessThreadToken(
    HANDLE Process,
    HANDLE Thread,
    HANDLE Token
)
{
    BOOLEAN                 Enabled;
    NTSTATUS                Status;
    OBJECT_ATTRIBUTES       ObjectAttributes;
    PROCESS_ACCESS_TOKEN    ProcessToken;

    InitializeObjectAttributes(&ObjectAttributes, NULL, 0, NULL, NULL);

    Status = NtDuplicateToken(Token, 0, &ObjectAttributes, FALSE, TokenPrimary, &ProcessToken.Token);
    FAIL_RETURN(Status);

    if (NT_FAILED(RtlAdjustPrivilege(SE_ASSIGNPRIMARYTOKEN_PRIVILEGE, TRUE, TRUE, &Enabled)))
        Enabled = FALSE;

    ProcessToken.Thread = Thread;
    Status = NtSetInformationProcess(Process, ProcessAccessToken, &ProcessToken, sizeof(ProcessToken));

    if (Enabled)
        RtlAdjustPrivilege(SE_ASSIGNPRIMARYTOKEN_PRIVILEGE, FALSE, TRUE, &Enabled);

    NtClose(ProcessToken.Token);

    return Status;
}

NTSTATUS
Nt_AdjustPrivilege(
    ULONG_PTR   Privilege,
    BOOL        Enable,
    BOOL        CurrentThread
)
{
    BOOLEAN Enabled;
    return RtlAdjustPrivilege(Privilege, (BOOLEAN)Enable, (BOOLEAN)CurrentThread, &Enabled);
}

/************************************************************************
  debug api
************************************************************************/

NTSTATUS Nt_DebugActiveProcess(ULONG_PTR ProcessId)
{
    HANDLE      ProcessHandle;
    NTSTATUS    Status;

    Status = DbgUiConnectToDbg();
    if (!NT_SUCCESS(Status))
        return Status;

    ProcessHandle = ml::Native::Ps::ProcessIdToHandle(ProcessId);
    if (ProcessHandle == NULL)
        return STATUS_UNSUCCESSFUL;

    Status = DbgUiDebugActiveProcess(ProcessHandle);
    NtClose(ProcessHandle);

    return Status;
}

NTSTATUS Nt_DebugActiveProcessStop(ULONG_PTR ProcessId)
{
    HANDLE      Process;
    NTSTATUS    Status;

    Process = ml::Native::Ps::ProcessIdToHandle(ProcessId);
    if (Process == NULL)
        return STATUS_UNSUCCESSFUL;

    Status = DbgUiStopDebugging(Process);
    NtClose(Process);

    return Status;
}

NTSTATUS Nt_WaitForDebugEvent(PDBGUI_WAIT_STATE_CHANGE WaitState, ULONG Timeout, HANDLE DebugObject)
{
    NTSTATUS        Status;
    LARGE_INTEGER   TimeOut;

    FormatTimeOut(&TimeOut, Timeout);
    LOOP_FOREVER
    {
        Status = NtWaitForDebugEvent(DebugObject, TRUE, &TimeOut, WaitState);
        switch (Status)
        {
            case STATUS_USER_APC:
            case STATUS_ALERTED:
                continue;
        }

        if (!NT_SUCCESS(Status) || Status == STATUS_TIMEOUT)
            return Status;

        break;
    }

    return Status;
}

/************************************************************************
  list entry
************************************************************************/

VOID
InitializeListHead(
    PLIST_ENTRY ListHead
)
{
    ListHead->Flink = ListHead->Blink = ListHead;
}

BOOL
IsListEmpty(
    LIST_ENTRY *ListHead
)
{
    return (BOOL)(ListHead->Flink == ListHead);
}

BOOL
RemoveEntryList(
    PLIST_ENTRY Entry
)
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Flink;

    Flink = Entry->Flink;
    Blink = Entry->Blink;
    Blink->Flink = Flink;
    Flink->Blink = Blink;
    return (BOOL)(Flink == Blink);
}

PLIST_ENTRY
RemoveHeadList(
    PLIST_ENTRY ListHead
)
{
    PLIST_ENTRY Flink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Flink;
    Flink = Entry->Flink;
    ListHead->Flink = Flink;
    Flink->Blink = ListHead;
    return Entry;
}

PLIST_ENTRY
RemoveTailList(
    PLIST_ENTRY ListHead
)
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Blink;
    Blink = Entry->Blink;
    ListHead->Blink = Blink;
    Blink->Flink = ListHead;
    return Entry;
}

VOID
InsertTailList(
    PLIST_ENTRY ListHead,
    PLIST_ENTRY Entry
)
{
    PLIST_ENTRY Blink;

    Blink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = Blink;
    Blink->Flink = Entry;
    ListHead->Blink = Entry;
}

VOID
InsertHeadList(
    PLIST_ENTRY ListHead,
    PLIST_ENTRY Entry
)
{
    PLIST_ENTRY Flink;

    Flink = ListHead->Flink;
    Entry->Flink = Flink;
    Entry->Blink = ListHead;
    Flink->Blink = Entry;
    ListHead->Flink = Entry;
}

VOID
AppendTailList(
    PLIST_ENTRY ListHead,
    PLIST_ENTRY ListToAppend
)
{
    PLIST_ENTRY ListEnd = ListHead->Blink;

    ListHead->Blink->Flink = ListToAppend;
    ListHead->Blink = ListToAppend->Blink;
    ListToAppend->Blink->Flink = ListHead;
    ListToAppend->Blink = ListEnd;
}

PSINGLE_LIST_ENTRY
PopEntryList(
    PSINGLE_LIST_ENTRY ListHead
)
{
    PSINGLE_LIST_ENTRY FirstEntry;
    FirstEntry = ListHead->Next;
    if (FirstEntry != NULL) {
        ListHead->Next = FirstEntry->Next;
    }

    return FirstEntry;
}

VOID
PushEntryList(
    PSINGLE_LIST_ENTRY ListHead,
    PSINGLE_LIST_ENTRY Entry
)
{
    Entry->Next = ListHead->Next;
    ListHead->Next = Entry;
}

VOID Nt_ListLock(PCRITICAL_SECTION Lock)
{
    RtlEnterCriticalSection(Lock);
}

VOID Nt_ListUnlock(PCRITICAL_SECTION Lock)
{
    RtlLeaveCriticalSection(Lock);
}

PLIST_ENTRY
ExInterlockedInsertHeadList(
    PLIST_ENTRY         ListHead,
    PLIST_ENTRY         Entry,
    PCRITICAL_SECTION   Lock
)
{
    Nt_ListLock(Lock);
    InsertHeadList(ListHead, Entry);
    Nt_ListUnlock(Lock);
    return ListHead;
}

PLIST_ENTRY
ExInterlockedInsertTailList(
    PLIST_ENTRY         ListHead,
    PLIST_ENTRY         Entry,
    PCRITICAL_SECTION   Lock
)
{
    PLIST_ENTRY TailList;

    Nt_ListLock(Lock);
    TailList = ListHead->Blink;
    InsertTailList(ListHead, Entry);
    Nt_ListUnlock(Lock);

    return TailList;
}

PLIST_ENTRY
ExInterlockedRemoveHeadList(
    PLIST_ENTRY         ListHead,
    PCRITICAL_SECTION   Lock
)
{
    Nt_ListLock(Lock);
    ListHead = RemoveHeadList(ListHead);
    Nt_ListUnlock(Lock);
    return ListHead;
}

PSINGLE_LIST_ENTRY
ExInterlockedPopEntryList(
    PSINGLE_LIST_ENTRY  ListHead,
    PCRITICAL_SECTION   Lock
)
{
    Nt_ListLock(Lock);
    ListHead = PopEntryList(ListHead);
    Nt_ListUnlock(Lock);
    return ListHead;
}

PSINGLE_LIST_ENTRY
ExInterlockedPushEntryList(
    PSINGLE_LIST_ENTRY  ListHead,
    PSINGLE_LIST_ENTRY  Entry,
    PCRITICAL_SECTION   Lock
)
{
    Nt_ListLock(Lock);
    PushEntryList(ListHead, Entry);
    Nt_ListUnlock(Lock);
    return ListHead;
}

VOID
Nt_InitializeSListHead(
    PSLIST_HEADER SListHead
)
{
    RtlZeroMemory(SListHead, sizeof(SLIST_HEADER));
}

/************************************************************************
  registry api
************************************************************************/

#if !defined(HKEY_CURRENT_USER_LOCAL_SETTINGS)
    #define HKEY_CURRENT_USER_LOCAL_SETTINGS    (( HKEY ) (ULONG_PTR)((LONG)0x80000007) )
#endif

NTSTATUS Nt_OpenPredefinedKeyHandle(PHANDLE KeyHandle, HKEY PredefinedKey, ACCESS_MASK DesiredAccess)
{
    return Reg::OpenPredefinedKeyHandle(KeyHandle, PredefinedKey, DesiredAccess);
}

NTSTATUS
Nt_RegGetValue(
    HANDLE                      hKey,
    PCWSTR                      SubKey,
    PCWSTR                      ValueName,
    KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    PVOID                       KeyValueInformation,
    ULONG                       Length,
    PULONG                      ResultLength,
    ULONG                       Flags
)
{
    return Reg::GetKeyValue(hKey, SubKey, ValueName, KeyValueInformationClass, KeyValueInformation, Length, ResultLength, Flags);
}

NTSTATUS
Nt_RegSetValue(
    HANDLE  hKey,
    PCWSTR  SubKey,
    PCWSTR  ValueName,
    ULONG   ValueType,
    LPCVOID ValueData,
    ULONG   ValueDataLength
)
{
    return Reg::SetKeyValue(hKey, SubKey, ValueName, ValueType, ValueData, ValueDataLength);
}

NTSTATUS
Nt_RegDeleteValue(
    HANDLE  hKey,
    PCWSTR  SubKey,
    PCWSTR  ValueName
)
{
    return Reg::DeleteKeyValue(hKey, SubKey, ValueName);
}

/************************************************************************
  memory
************************************************************************/

NTSTATUS
Nt_ProtectMemory(
    HANDLE      ProcessHandle,
    PVOID       BaseAddress,
    ULONG_PTR   Size,
    ULONG       NewProtect,
    PULONG      OldProtect
)
{
    return ZwProtectVirtualMemory(ProcessHandle, &BaseAddress, &Size, NewProtect, OldProtect);
}

NTSTATUS
Nt_ReadMemory(
    HANDLE      ProcessHandle,
    PVOID       BaseAddress,
    PVOID       Buffer,
    ULONG_PTR   Size,
    PULONG_PTR  BytesRead /* = NULL */
)
{
    return ZwReadVirtualMemory(ProcessHandle, BaseAddress, Buffer, Size, BytesRead);
}

NTSTATUS
Nt_WriteMemory(
    HANDLE      ProcessHandle,
    PVOID       BaseAddress,
    PVOID       Buffer,
    ULONG_PTR   Size,
    PULONG_PTR  BytesWritten /* = NULL */
)
{
    return ZwWriteVirtualMemory(ProcessHandle, BaseAddress, Buffer, Size, BytesWritten);
}

NTSTATUS
Nt_WriteProtectMemory(
    HANDLE      ProcessHandle,
    PVOID       BaseAddress,
    PVOID       Buffer,
    ULONG_PTR   Size,
    PULONG_PTR  BytesWritten /* = NULL */
)
{
    ULONG       Protect;
    NTSTATUS    Status;

    Status = Nt_ProtectMemory(ProcessHandle, BaseAddress, Size, PAGE_EXECUTE_READWRITE, &Protect);
    FAIL_RETURN(Status);

    Status = Nt_WriteMemory(ProcessHandle, BaseAddress, Buffer, Size, BytesWritten);

    if (Protect != PAGE_EXECUTE_READWRITE)
        Nt_ProtectMemory(ProcessHandle, BaseAddress, Size, Protect, &Protect);

    return Status;
}

/************************************************************************
  process
************************************************************************/
/*
NTSTATUS
Nt_GetProcessUserName(
    PUNICODE_STRING UserName,
    HANDLE          Process
)
{
    HANDLE      Token;
    NTSTATUS    Status;
    PTOKEN_USER User;
    ULONG       Length, DomainLength;
    WCHAR       Name[MAX_NTPATH], Domain[MAX_NTPATH];
    SID_NAME_USE SidType;

    Status = NtOpenProcessToken(Process, TOKEN_QUERY, &Token);
    FAIL_RETURN(Status);

    Status = Nt_GetTokenInfo(Token, TokenUser, &User);
    if (NT_FAILED(Status))
    {
        NtClose(Token);
        return Status;
    }

    Length = countof(Name);
    DomainLength = countof(Domain);
    Status = LookupAccountSidW(NULL, User->User.Sid, Name, &Length, Domain, &DomainLength, &SidType);

    NtClose(Token);
    ReleaseTokenInfo(User);

    if (!Status)
        return STATUS_UNSUCCESSFUL;

    return RtlCreateUnicodeString(UserName, Name) ? STATUS_SUCCESS : STATUS_NO_MEMORY;
}
*/
#if ML_AMD64

NTSTATUS
InjectDllToRemoteProcess(
    HANDLE          ProcessHandle,
    HANDLE          ThreadHandle,
    PUNICODE_STRING DllFullPath,
    ULONG_PTR       Flags,
    PVOID*          InjectBuffer,
    ULONG_PTR       Timeout
)
{
    return STATUS_NOT_IMPLEMENTED;
}

#else // x86

ASM PVOID InjectRemote_LoadDll()
{
    INLINE_ASM
    {
        __emit  0xCC;

        push eax;           // ret addr
        pushad;
        pushfd;
        call SELF_LOCALIZATION;
SELF_LOCALIZATION:
        pop  esi;
        and  esi, 0FFFFF000h;
        lodsd;
        mov  [esp+0x24], eax;   // ret addr
        lodsd;
        xor  ecx, ecx;
        push ecx;
        push esp;               // pModuleHandle
        push esi;               // ModuleFileName
        push ecx;               // Flags
        push ecx;               // PathToFile
        call eax;               // LdrLoadDll
        pop  eax;               // pop ModuleHandle
        xchg eax, esi;
        and  eax, 0FFFFF000h;
        and  dword ptr [eax], 0;
        popfd;
        popad;
        ret;
    }
}

static ASM VOID InjectRemote_LoadDllEnd() { ASM_UNIQUE() }

ASM VOID InjectRemote_IssueRemoteBreakin()
{
    INLINE_ASM
    {
        call    SELF_LOCATE;
SELF_LOCATE:
        pop     ecx;
        lea     ecx, [ecx + 14];
        push    ecx;
        xor     eax, eax;
        push    dword ptr fs:[eax];
        mov     dword ptr fs:[eax], esp;
        __emit  0xCC;

        call    SELF_LOCATE2;
SELF_LOCATE2:
        pop     esi;
        and     esi, not (MEMORY_PAGE_SIZE - 1);
        lodsd;
        mov     ebx, eax;
        lodsd;
        mov     ecx, [esi];
        jecxz   ISSUE_BREAK_IN_ONLY;

        xor     ecx, ecx;
        push    eax;    // sub esp, 4
        push    esp;    // pModuleHandle
        push    esi;    // ModuleFileName
        push    ecx;    // Flags
        push    ecx;    // PathToFile
        call    eax;    // LdrLoadDll
//        pop     eax;    // add esp, 4;

ISSUE_BREAK_IN_ONLY:

        xchg eax, esi;
        and  eax, 0FFFFF000h;
        and  dword ptr [eax], 0;

EXIT_THREAD:
//        push    0;
        push    -2;
        call    ebx;
        jmp     EXIT_THREAD;
    }
}

static ASM VOID InjectRemote_IssueRemoteBreakinEnd() { ASM_UNIQUE() }

NTSTATUS
InjectDllToRemoteProcess(
    HANDLE          ProcessHandle,
    HANDLE          ThreadHandle,
    PUNICODE_STRING DllFullPath,
    ULONG_PTR       Flags,
    PVOID*          InjectBuffer,
    ULONG_PTR       Timeout
)
{
    NTSTATUS        Status;
    PVOID           Buffer, InjectContext, ShellCode;
    ULONG_PTR       DllPathLength, InjectContextSize, ShellCodeSize;
    CONTEXT         ThreadContext;
    LARGE_INTEGER   TimeOut;

    typedef struct
    {
        ULONG           ReturnAddr;
        PVOID           xLdrLoadDll;
        UNICODE_STRING  DllPath;
    } INJECT_REMOTE_LOAD_DLL;

    typedef struct
    {
        PVOID           xNtTerminateThread;
        PVOID           xLdrLoadDll;
        UNICODE_STRING  DllPath;
    //    WCHAR           Buffer[1];
    } INJECT_REMOTE_ISSUE_BREAK_IN;

    union
    {
        INJECT_REMOTE_LOAD_DLL          LoadDll;
        INJECT_REMOTE_ISSUE_BREAK_IN    Breakin;
    };

    Buffer = NULL;
    if (FLAG_ON(Flags, INJECT_PREALLOC_BUFFER))
    {
        if (InjectBuffer == NULL)
            return STATUS_INVALID_PARAMETER;

        Buffer = *InjectBuffer;
        if (Buffer == NULL)
            return STATUS_NO_MEMORY;
    }
    else if (InjectBuffer != NULL)
    {
        *InjectBuffer = NULL;
    }

    ThreadContext.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
    Status = NtGetContextThread(ThreadHandle, &ThreadContext);
    if (!NT_SUCCESS(Status))
        return Status;

    if (!FLAG_ON(Flags, INJECT_PREALLOC_BUFFER))
    {
        Status = Mm::AllocVirtualMemoryEx(ProcessHandle, &Buffer, MEMORY_PAGE_SIZE);
        if (!NT_SUCCESS(Status))
            return Status;
    }

    if (FLAG_OFF(Flags, INJECT_ISSUE_BREAKIN_ONLY) && DllFullPath != nullptr)
    {
        DllPathLength = DllFullPath->Length;
    }
    else
    {
        DllPathLength = 0;
    }

    if (FLAG_ON(Flags, INJECT_ISSUE_REMOTE_BREAKIN | INJECT_ISSUE_BREAKIN_ONLY))
    {
        PVOID ntdll;

        InjectContext       = &Breakin;
        InjectContextSize   = sizeof(Breakin);
        ShellCode           = InjectRemote_IssueRemoteBreakin;
        ShellCodeSize       = PtrOffset(InjectRemote_IssueRemoteBreakinEnd, InjectRemote_IssueRemoteBreakin);

        ntdll = GetNtdllHandle();

        Breakin.xLdrLoadDll             = EATLookupRoutineByHashPNoFix(ntdll, NTDLL_LdrLoadDll);
        Breakin.xNtTerminateThread      = EATLookupRoutineByHashPNoFix(ntdll, NTDLL_NtTerminateThread);

        if (FLAG_ON(Flags, INJECT_ISSUE_BREAKIN_ONLY))
        {
            RtlInitEmptyUnicodeString(&Breakin.DllPath, NULL, 0);
        }
        else
        {
            Breakin.DllPath.Length          = (USHORT)DllPathLength;
            Breakin.DllPath.MaximumLength   = (USHORT)DllPathLength;
            Breakin.DllPath.Buffer          = (PWSTR)((ULONG_PTR)Buffer + sizeof(Breakin));
        }
    }
    else
    {
        PVOID ntdll;

        InjectContext       = &LoadDll;
        InjectContextSize   = sizeof(LoadDll);
        ShellCode           = PtrAdd(InjectRemote_LoadDll, FLAG_OFF(Flags, INJECT_ISSUE_BREAKIN));
        ShellCodeSize       = PtrOffset(InjectRemote_LoadDllEnd, InjectRemote_LoadDll);

        ntdll = GetNtdllHandle();

        LoadDll.xLdrLoadDll             = EATLookupRoutineByHashPNoFix(ntdll, NTDLL_LdrLoadDll);
        LoadDll.ReturnAddr              = ThreadContext.Eip;
        LoadDll.DllPath.Length          = (USHORT)DllPathLength;
        LoadDll.DllPath.MaximumLength   = (USHORT)DllPathLength;
        LoadDll.DllPath.Buffer          = (PWSTR)((ULONG_PTR)Buffer + sizeof(LoadDll));
    }

    Status = STATUS_UNSUCCESSFUL;
    LOOP_ONCE
    {
        Status = WriteMemory(ProcessHandle, Buffer, InjectContext, InjectContextSize);
        if (!NT_SUCCESS(Status))
            break;

        if (DllFullPath != nullptr && FLAG_OFF(Flags, INJECT_ISSUE_BREAKIN_ONLY))
        {
            Status = WriteMemory(
                        ProcessHandle,
                        (PVOID)((ULONG_PTR)Buffer + InjectContextSize),
                        DllFullPath->Buffer,
                        DllPathLength
                        );
            if (!NT_SUCCESS(Status))
                break;
        }

        ThreadContext.Eip = ROUND_UP((ULONG_PTR)Buffer + InjectContextSize + DllPathLength, 16);
        Status = WriteMemory(
                    ProcessHandle,
                    (PVOID)ThreadContext.Eip,
                    ShellCode,
                    ShellCodeSize
                 );
        if (!NT_SUCCESS(Status))
            break;

        NtFlushInstructionCache(ProcessHandle, (PVOID)ThreadContext.Eip, ShellCodeSize);

        if (FLAG_OFF(Flags, INJECT_DONT_CHANGE_IP))
        {
            Status = NtSetContextThread(ThreadHandle, &ThreadContext);
            if (!NT_SUCCESS(Status))
                break;

            Status = NtGetContextThread(ThreadHandle, &ThreadContext);
        }

        if (FLAG_ON(Flags, INJECT_THREAD_SUSPENDED))
        {
            if (InjectBuffer != NULL)
            {
                *InjectBuffer = FLAG_ON(Flags, INJECT_POINT_TO_SHELL_CODE) ? (PVOID)ThreadContext.Eip : Buffer;
            }

            return Status;
        }

        Status = NtResumeThread(ThreadHandle, NULL);
        if (!NT_SUCCESS(Status))
            break;

        FormatTimeOut(&TimeOut, Timeout);
        for (ULONG TryTimes = 30; TryTimes; --TryTimes)
        {
            ULONG Val;
            Status = ReadMemory(ProcessHandle, Buffer, &Val, sizeof(Val));
            if (!NT_SUCCESS(Status))
                break;

            if (Val != 0)
            {
                NtDelayExecution(FALSE, &TimeOut);
                continue;
            }

            break;
        }

        if (!NT_SUCCESS(Status))
            break;

        NtDelayExecution(FALSE, &TimeOut);
        Status = NtGetContextThread(ThreadHandle, &ThreadContext);
        if (!NT_SUCCESS(Status))
            break;

        if ((ULONG_PTR)ThreadContext.Eip < (ULONG_PTR)Buffer ||
            (ULONG_PTR)ThreadContext.Eip > (ULONG_PTR)Buffer + MEMORY_PAGE_SIZE)
        {
            Status = STATUS_SUCCESS;
        }
        else
        {
            Status = STATUS_UNSUCCESSFUL;
        }
    }

    if (FLAG_OFF(Flags, INJECT_PREALLOC_BUFFER))
        Mm::FreeVirtualMemory(Buffer, ProcessHandle);

    return Status;
}

#endif // x64

VOID Nt_ExitProcess(ULONG ExitCode)
{
    return ml::Native::Ps::ExitProcess(ExitCode);
}

// NTSTATUS
// Nt_CreateProcess2(
//     CreateProcessInternal2Routine   Routine,
//     HANDLE                          Token,
//     PCWSTR                          ApplicationName,
//     PWSTR                           CommandLine,
//     PCWSTR                          CurrentDirectory,
//     ULONG                           CreationFlags,
//     LPSTARTUPINFOW                  StartupInfo,
//     LPPROCESS_INFORMATION           ProcessInformation,
//     LPSECURITY_ATTRIBUTES           ProcessAttributes,
//     LPSECURITY_ATTRIBUTES           ThreadAttributes,
//     PVOID                           Environment
// )
// {
//     BOOL                            Result;
//     STARTUPINFOW                    DefaultStartupInfo;
//     PROCESS_INFORMATION             DefaultProcessInformation;

//     SEH_TRY
//     {
//         if (StartupInfo == NULL)
//         {
//             ZeroMemory(&DefaultStartupInfo, sizeof(DefaultStartupInfo));
//             DefaultStartupInfo.cb = sizeof(DefaultStartupInfo);
//             StartupInfo = &DefaultStartupInfo;
//         }

//         if (ProcessInformation == NULL)
//             ProcessInformation = &DefaultProcessInformation;

//         CREATE_PROCESS_INTERNAL_PARAM Parameter =
//         {
//             Token,
//             ApplicationName,
//             CommandLine,
//             ProcessAttributes,
//             ThreadAttributes,
//             FALSE,
//             CreationFlags,
//             Environment,
//             CurrentDirectory,
//             StartupInfo,
//             ProcessInformation,
//             NULL,
//         };

//         Result = Routine(Parameter);
//         if (!Result)
//             return STATUS_UNSUCCESSFUL;

//         if (ProcessInformation == &DefaultProcessInformation)
//         {
//             NtClose(DefaultProcessInformation.hProcess);
//             NtClose(DefaultProcessInformation.hThread);
//         }

//         return STATUS_SUCCESS;
//     }
//     SEH_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
//     {
//         ;
//     }

//     return STATUS_UNSUCCESSFUL;
// }

NTSTATUS
Nt_CreateProcess(
    HANDLE                  Token,
    PCWSTR                  ApplicationName,
    PWSTR                   CommandLine,
    PCWSTR                  CurrentDirectory,
    ULONG                   CreationFlags,
    LPSTARTUPINFOW          StartupInfo,
    LPPROCESS_INFORMATION   ProcessInformation,
    LPSECURITY_ATTRIBUTES   ProcessAttributes,
    LPSECURITY_ATTRIBUTES   ThreadAttributes,
    PVOID                   Environment
)
{
//     BOOL                            Result;
//     STARTUPINFOW                    DefaultStartupInfo;
//     PROCESS_INFORMATION             DefaultProcessInformation;

//     if (StartupInfo == NULL)
//     {
//         ZeroMemory(&DefaultStartupInfo, sizeof(DefaultStartupInfo));
//         DefaultStartupInfo.cb = sizeof(DefaultStartupInfo);
//         StartupInfo = &DefaultStartupInfo;
//     }

//     if (ProcessInformation == NULL)
//         ProcessInformation = &DefaultProcessInformation;

// #if ML_DISABLE_THIRD_LIB

//     Result = CreateProcessAsUserW(
//                     Token,
//                     ApplicationName,
//                     (PWSTR)CommandLine,
//                     ProcessAttributes,
//                     ThreadAttributes,
//                     FALSE,
//                     CreationFlags,
//                     Environment,
//                     CurrentDirectory,
//                     StartupInfo,
//                     ProcessInformation
//              );

// #else

//     Result = CreateProcessInternalW(
//                     Token,
//                     ApplicationName,
//                     CommandLine,
//                     ProcessAttributes,
//                     ThreadAttributes,
//                     FALSE,
//                     CreationFlags,
//                     Environment,
//                     CurrentDirectory,
//                     StartupInfo,
//                     ProcessInformation,
//                     NULL
//              );

// #endif // ML_DISABLE_THIRD_LIB

//     if (!Result)
//         return STATUS_UNSUCCESSFUL;

//     if (ProcessInformation == &DefaultProcessInformation)
//     {
//         NtClose(DefaultProcessInformation.hProcess);
//         NtClose(DefaultProcessInformation.hThread);
//     }

    return STATUS_SUCCESS;
}

NTSTATUS
Nt_CreateThread(
    PVOID                   StartAddress,
    PVOID                   StartParameter,
    BOOL                    CreateSuspended,
    HANDLE                  ProcessHandle,
    PHANDLE                 ThreadHandle,
    PCLIENT_ID              ClientID,
    ULONG                   StackZeroBits,
    ULONG                   StackReserved,
    ULONG                   StackCommit,
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
                StackZeroBits,
                StackReserved,
                StackCommit,
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

NTSTATUS
Nt_SetThreadPriority(
    HANDLE  ThreadHandle,
    LONG    Priority
)
{
    NTSTATUS Status;

    switch (Priority)
    {
        case THREAD_PRIORITY_TIME_CRITICAL:
            Priority = (HIGH_PRIORITY + 1) / 2;
            break;

        case THREAD_PRIORITY_IDLE:
            Priority = -((HIGH_PRIORITY + 1) / 2);
            break;
    }

    Status = NtSetInformationThread(
                ThreadHandle,
                ThreadBasePriority,
                &Priority,
                sizeof(LONG)
             );

    return Status;
}

PLDR_MODULE Nt_FindLdrModuleByName(PUNICODE_STRING ModuleName)
{
    return ml::Native::Ldr::FindLdrModuleByName(ModuleName);
}

PLDR_MODULE Nt_FindLdrModuleByHandle(PVOID BaseAddress)
{
    return ml::Native::Ldr::FindLdrModuleByHandle(BaseAddress);
}

PTEB_ACTIVE_FRAME Nt_FindThreadFrameByContext(ULONG_PTR Context)
{
    return ml::Native::Ps::FindThreadFrame(Context);
}


/************************************************************************
  path
************************************************************************/

ULONG
Nt_GetSystemDirectory64(
    PWSTR Buffer,
    ULONG BufferCount
)
{
    ULONG                           Length, ResultLength;
    NTSTATUS                        Status;
    PKEY_VALUE_PARTIAL_INFORMATION  ValuePartial;

    Length = MAX_NTPATH * 2 + sizeof(*ValuePartial);
    ValuePartial = (PKEY_VALUE_PARTIAL_INFORMATION)AllocStack(Length);

    LOOP_FOREVER
    {
        Status = Nt_RegGetValue(
                    HKEY_LOCAL_MACHINE,
                    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Installer",
                    L"InstallerLocation",
                    KeyValuePartialInformation,
                    ValuePartial,
                    Length,
                    &ResultLength,
                    KEY_WOW64_64KEY
                 );
        if (Status != STATUS_BUFFER_TOO_SMALL || ResultLength <= Length)
            break;

        ValuePartial = (PKEY_VALUE_PARTIAL_INFORMATION)AllocStack(ResultLength - Length);
        Length = ResultLength;
    }

    if (!NT_SUCCESS(Status))
        return 0;

    Length = MY_MIN(ValuePartial->DataLength, BufferCount * sizeof(WCHAR));
    CopyMemory(Buffer, ValuePartial->Data, Length);

    Length /= sizeof(WCHAR);
    while (Buffer[Length - 1] == 0)
        --Length;

    if (Length < BufferCount)
    {
        if (Buffer[Length - 1] != '\\')
            Buffer[Length++] = '\\';
    }

    if (Length < BufferCount)
        Buffer[Length] = 0;

    return Length;
}

ULONG Nt_GetWindowsDirectory(PWCHAR Buffer, ULONG BufferCount)
{
    ULONG Length;
    PLDR_MODULE LdrModule;

    LdrModule = FIELD_BASE(Nt_CurrentPeb()->Ldr->InInitializationOrderModuleList.Flink, LDR_MODULE, InInitializationOrderLinks);
    Length = (LdrModule->FullDllName.Length - LdrModule->BaseDllName.Length) / sizeof(WCHAR) - 2;

    while (LdrModule->FullDllName.Buffer[Length] != '\\' && --Length);
    if (Length == 0)
        return 0;

    ++Length;
    Length *= sizeof(WCHAR);
    Length = MY_MIN(Length, BufferCount * sizeof(WCHAR));
    CopyMemory(Buffer, LdrModule->FullDllName.Buffer, Length);
    Length /= sizeof(WCHAR);
    if (Length < BufferCount)
        Buffer[Length] = 0;

    return Length;
}

ULONG Nt_GetSystemDirectory(PWCHAR Buffer, ULONG BufferCount)
{
    ULONG Length;
    PLDR_MODULE LdrModule;

    LdrModule = FIELD_BASE(Nt_CurrentPeb()->Ldr->InInitializationOrderModuleList.Flink, LDR_MODULE, InInitializationOrderLinks);
    Length = LdrModule->FullDllName.Length - LdrModule->BaseDllName.Length;

    if (Buffer == NULL)
        return Length / sizeof(WCHAR);

    Length = MY_MIN(Length, BufferCount * sizeof(WCHAR));
    CopyMemory(Buffer, LdrModule->FullDllName.Buffer, Length);
    Length /= sizeof(WCHAR);
    if (Length < BufferCount)
        Buffer[Length] = 0;

    return Length;
}

ULONG_PTR
Nt_GetDosPathFromNtDeviceName(
    PWSTR           DosPath,
    ULONG_PTR       DosPathLength,
    PUNICODE_STRING NtDeviceName
)
{
    ULONG_PTR       Length;
    NTSTATUS        Status;
    UNICODE_STRING  DosPathString;

    Status = Io::QueryDosPathFromNtDeviceName(&DosPathString, NtDeviceName);
    if (NT_FAILED(Status))
        return 0;

    DosPathLength = InternalCopyUnicodeString(&DosPathString, DosPath, DosPathLength);
    RtlFreeUnicodeString(&DosPathString);

    return DosPathLength;
}

NTSTATUS
Nt_GetDosLetterFromVolumeDeviceName(
    PUNICODE_STRING DosLetter,
    PUNICODE_STRING NtDeviceName
)
{
    return Io::QueryDosLetterFromVolumeDeviceName(DosLetter, NtDeviceName);
}

NTSTATUS Nt_GetDosPathFromHandle(PUNICODE_STRING DosPath, HANDLE FileHandle)
{
    return Io::QueryDosPathFromHandle(DosPath, FileHandle);
}

NTSTATUS Nt_NtPathNameToDosPathName(PUNICODE_STRING DosPath, PUNICODE_STRING NtPath)
{
    return Rtl::NtPathNameToDosPathName(DosPath, NtPath);
}

VOID ConvertFindData(PWIN32_FIND_DATAW W32, PML_FIND_DATA ML)
{
    CopyStruct(W32->cFileName, ML->FileName, sizeof(W32->cFileName));
    CopyStruct(W32->cAlternateFileName, ML->AlternateFileName, sizeof(W32->cAlternateFileName));

    W32->dwFileAttributes = ML->FileAttributes;
    W32->ftCreationTime.dwLowDateTime = ML->CreationTime.LowPart;
    W32->ftCreationTime.dwHighDateTime = ML->CreationTime.HighPart;

    W32->ftLastWriteTime.dwLowDateTime = ML->LastWriteTime.LowPart;
    W32->ftLastWriteTime.dwHighDateTime = ML->LastWriteTime.HighPart;

    W32->ftLastAccessTime.dwLowDateTime = ML->LastAccessTime.LowPart;
    W32->ftLastAccessTime.dwHighDateTime = ML->LastAccessTime.HighPart;

    W32->dwReserved0 = ML->Reserved0;
    W32->dwReserved1 = ML->Reserved1;

    W32->nFileSizeLow = ML->FileSize.LowPart;
    W32->nFileSizeHigh = ML->FileSize.HighPart;
}

BOOL Nt_FindClose(HANDLE FindFileHandle)
{
    return NT_SUCCESS(Io::QueryClose(FindFileHandle));
}

HANDLE Nt_FindFirstFile(LPCWSTR FileName, PWIN32_FIND_DATAW FindFileData)
{
    NTSTATUS        Status;
    HANDLE          DirectoryHandle;
    ML_FIND_DATA    FindData;

    Status = Io::QueryFirstFile(&DirectoryHandle, FileName, &FindData);
    if (NT_SUCCESS(Status))
        ConvertFindData(FindFileData, &FindData);

    return NT_SUCCESS(Status) ? DirectoryHandle : INVALID_HANDLE_VALUE;
}

BOOL Nt_FindNextFile(HANDLE FindFileHandle, PWIN32_FIND_DATAW FindFileData)
{
    NTSTATUS        Status;
    ML_FIND_DATA    FindData;

    Status = Io::QueryNextFile(FindFileHandle, &FindData);
    if (NT_SUCCESS(Status))
        ConvertFindData(FindFileData, &FindData);

    return NT_SUCCESS(Status);
}

ULONG Nt_GetFileAttributes(LPCWSTR FileName)
{
    return Io::QueryFileAttributes(FileName);
}

NTSTATUS Nt_MoveFile(LPCWSTR ExistingFileName, LPCWSTR NewFileName)
{
    return Io::MoveFile(ExistingFileName, NewFileName);
}

NTSTATUS Nt_CopyFile(PCWSTR ExistingFileName, PCWSTR NewFileName, BOOL FailIfExists)
{
    return Io::CopyFile(ExistingFileName, NewFileName, FailIfExists);
}

BOOL Nt_IsPathExists(LPCWSTR pszPath)
{
    return Io::IsPathExists(pszPath);
}

ULONG_PTR Nt_GetModulePath(PVOID ModuleBase, PWSTR Path, ULONG_PTR BufferCount)
{
    PLDR_MODULE LdrModule;
    ULONG_PTR   PathLength;

    LdrModule   = Nt_FindLdrModuleByHandle(ModuleBase);
    PathLength  = (LdrModule->FullDllName.Length - LdrModule->BaseDllName.Length) / sizeof(WCHAR);

    InternalCopyUnicodeString(&LdrModule->FullDllName, Path, PathLength);
    if (PathLength < BufferCount)
        Path[PathLength] = 0;

    return PathLength;
}

ULONG Nt_GetExeDirectory(PWCHAR Path, ULONG BufferCount)
{
    return Nt_GetModulePath(NULL, Path, BufferCount);
}

ULONG_PTR Nt_GetModuleFileName(PVOID ModuleBase, LPWSTR Filename, ULONG_PTR BufferCount)
{
    ULONG_PTR               Length;
    PEB_BASE               *Peb;
    PLDR_DATA_TABLE_ENTRY   LdrModule, FirstLdrModule;

    Peb = Nt_CurrentPeb();
    LdrModule = FIELD_BASE(Peb->Ldr->InLoadOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

    FirstLdrModule = LdrModule;

    if (ModuleBase == NULL)
        ModuleBase = Peb->ImageBaseAddress;

    LOOP_FOREVER
    {
        if ((ULONG_PTR)ModuleBase >= (ULONG_PTR)LdrModule->DllBase &&
            (ULONG_PTR)ModuleBase < (ULONG_PTR)LdrModule->DllBase + LdrModule->SizeOfImage)
        {
            break;
        }

        LdrModule = (PLDR_DATA_TABLE_ENTRY)LdrModule->InLoadOrderLinks.Flink;
        if (LdrModule == FirstLdrModule)
            return 0;
    }

    Length = InternalCopyUnicodeString(&LdrModule->FullDllName, Filename, BufferCount);
    if (LdrModule->FullDllName.Length / sizeof(WCHAR) < BufferCount)
        Filename[BufferCount] = 0;
/*
    Length = min(nSize * sizeof(*lpFilename), LdrModule->FullDllName.Length);
    CopyMemory(lpFilename, LdrModule->FullDllName.Buffer, Length);
    Length /= sizeof(*lpFilename);
    if (Length < nSize)
        lpFilename[Length] = 0;
*/
    return Length;
}

BOOL Nt_SetExeDirectoryAsCurrent()
{
    return NT_SUCCESS(Rtl::SetExeDirectoryAsCurrent());
}

/************************************************************************
  misc
************************************************************************/

PVOID Nt_LoadLibrary(PWSTR ModuleFileName)
{
    return ml::Native::Ldr::LoadDll(ModuleFileName);
}

PVOID FASTCALL Nt_GetProcAddress(PVOID ModuleBase, LPCSTR lpProcName)
{
    return Ldr::GetRoutineAddress(ModuleBase, lpProcName);
}

PVOID Nt_GetModuleHandle(LPCWSTR lpModuleName)
{
    UNICODE_STRING  ModuleFileName;
    PLDR_MODULE     Module;

    RtlInitUnicodeString(&ModuleFileName, lpModuleName);
    Module = Ldr::FindLdrModuleByName(&ModuleFileName);
    return Module == NULL ? NULL : Module->DllBase;
}

VOID Nt_Sleep(ULONG_PTR Milliseconds, BOOL Alertable)
{
    return Ps::Sleep(Milliseconds, Alertable);
}

#endif  // MY_NT_DDK

_ML_C_TAIL_
