#ifndef _NATIVEAPIHELPER_H_146120be_f089_49ef_bfd3_82c455e90ba3
#define _NATIVEAPIHELPER_H_146120be_f089_49ef_bfd3_82c455e90ba3

#include "ml/nativeapi.h"
//#include "ml/undocumentedapi.h"

_ML_C_HEAD_


/************************************************************************
  strings
************************************************************************/

NTSTATUS
FASTCALL
Nt_AnsiToUnicode(
    PWSTR       UnicodeBuffer,
    ULONG_PTR   BufferCount,
    PCSTR       AnsiString,
    LONG_PTR    DEF_VAL(AnsiLength, -1),
    PULONG_PTR  DEF_VAL(BytesInUnicode, NULL)
);

NTSTATUS
Nt_UnicodeToAnsi(
    PCHAR       AnsiBuffer,
    ULONG_PTR   BufferCount,
    LPCWSTR     UnicodeString,
    LONG_PTR    DEF_VAL(UnicodeLength, -1),
    PULONG_PTR  DEF_VAL(BytesInAnsi, NULL)
);

NTSTATUS
Nt_AnsiToUnicodeString(
    PUNICODE_STRING Unicode,
    PCHAR           AnsiString,
    LONG_PTR        DEF_VAL(AnsiLength, -1),
    BOOL            DEF_VAL(AllocateDestinationString, TRUE)
);

NTSTATUS
Nt_UnicodeToAnsiString(
    PANSI_STRING    Ansi,
    LPCWSTR         UnicodeString,
    LONG_PTR        DEF_VAL(UnicodeLength, -1),
    BOOL            DEF_VAL(AllocateDestinationString, TRUE)
);

BOOL Nt_IsPathExists(LPCWSTR pszPath);

PLDR_MODULE
Nt_FindLdrModuleByName(
    PUNICODE_STRING ModuleName
);

PLDR_MODULE
Nt_FindLdrModuleByHandle(
    PVOID BaseAddress
);

PTEB_ACTIVE_FRAME
Nt_FindThreadFrameByContext(
    ULONG_PTR Context
);

ForceInline
ANSI_STRING
CharToAnsiString(
    PSTR  Buffer,
    ULONG Length
)
{
    ANSI_STRING Ansi;

    Ansi.Buffer          = Buffer;
    Ansi.Length          = (USHORT)(Length * sizeof(*Buffer));
    Ansi.MaximumLength   = Ansi.Length;

    return Ansi;
}

ForceInline
UNICODE_STRING
WCharToUnicodeString(
    PWSTR Buffer,
    ULONG Length
)
{
    UNICODE_STRING Unicode;

    Unicode.Buffer          = Buffer;
    Unicode.Length          = (USHORT)(Length * sizeof(*Buffer));
    Unicode.MaximumLength   = Unicode.Length;

    return Unicode;
}

ForceInline
UNICODE_STRING64
WCharToUnicodeString64(
    PWSTR Buffer,
    ULONG Length
)
{
    UNICODE_STRING64 Unicode;

    Unicode.Dummy           = NULL;
    Unicode.Buffer          = Buffer;
    Unicode.Length          = (USHORT)(Length * sizeof(*Buffer));
    Unicode.MaximumLength   = Unicode.Length;

    return Unicode;
}

#pragma warning(disable:4238)

#define EMPTYUS         WCharToUnicodeString((PWSTR)NULL, 0)
#define USTR(_str)      WCharToUnicodeString(_str, CONST_STRLEN((_str)))
#define IUSTR(x)        {CONST_STRLEN(x) * sizeof(x[0]), CONST_STRLEN(x) * sizeof(x[0]), x}
#define VUSTR(_str)      ((*(volatile UNICODE_STRING *)&WCharToUnicodeString(_str, CONST_STRLEN((_str)))))
#define PUSTR(_str)     (PUNICODE_STRING)&VUSTR(_str)
#define USTR64(_str)    WCharToUnicodeString64(_str, CONST_STRLEN((_str)))
#define ASTR(_str)      CharToAnsiString(_str, CONST_STRLEN((_str)))

#define WCS2US          USTR
#define CS2AS           ASTR

inline PLARGE_INTEGER FormatTimeOut(PLARGE_INTEGER TimeOut, ULONG Milliseconds)
{
    if (Milliseconds == INFINITE)
    {
        //TimeOut->LowPart = 0;
        //TimeOut->HighPart = 0x80000000;
        TimeOut->QuadPart = DELAY_QUAD_INFINITE;
    }
    else
    {
        TimeOut->QuadPart = DELAY_ONE_MILLISECOND * (LONG64)Milliseconds;
    }

    return TimeOut;
}

unsigned __int64 __readfsqword(unsigned long Offset);

inline ULONG_PTR ReadFsPtr(ULONG_PTR Offset)
{
#if ML_AMD64
    return (ULONG_PTR)__readgsqword((ULONG)Offset);
#else // x86
    return (ULONG_PTR)__readfsdword(Offset);
#endif
}

#pragma warning(disable:4733)

inline VOID WriteFsPtr(ULONG_PTR Offset, ULONG_PTR Data)
{
#if ML_AMD64
    __writegsqword((ULONG)Offset, Data);
#else // x86
    __writefsdword(Offset, Data);
#endif
}

#pragma warning(default:4733)

#define TEB_OFFSET FIELD_OFFSET(TEB_BASE, NtTib.Self)
#define PEB_OFFSET FIELD_OFFSET(TEB_BASE, ProcessEnvironmentBlock)

ForceInline PTEB_BASE Nt_CurrentTeb()
{
#if ML_AMD64
    return (PTEB_BASE)ReadFsPtr(TEB_OFFSET);
#else

#ifdef NtCurrentTeb
    return (PTEB_BASE)NtCurrentTeb();
#else
    return (PTEB_BASE)ReadFsPtr(TEB_OFFSET);
#endif // no macro

#endif
}

#if ML_KERNEL_MODE
inline
#else
ForceInline
#endif
PPEB_BASE Nt_CurrentPeb()
{

#if ML_KERNEL_MODE

    NTSTATUS    Status;
    PROCESS_BASIC_INFORMATION BasicInfo;

    Status = ZwQueryInformationProcess(NtCurrentProcess(), ProcessBasicInformation, &BasicInfo, sizeof(BasicInfo), NULL);
    if (!NT_SUCCESS(Status))
        return NULL;

    return (PPEB_BASE)BasicInfo.PebBaseAddress;

#else // r3

    return (PPEB_BASE)(ULONG_PTR)ReadFsPtr(PEB_OFFSET);

#endif // rx
}

ForceInline HANDLE RtlGetProcessHeap()
{
    return Nt_CurrentTeb()->ProcessEnvironmentBlock->ProcessHeap;
}

ForceInline PUSHORT Nt_GetDefaultCodePageBase()
{
    return Nt_CurrentPeb()->AnsiCodePageData;
}

inline PLDR_MODULE GetKernel32Ldr()
{
    LDR_MODULE *Ldr, *FirstLdr;

    Ldr = FIELD_BASE(Nt_CurrentPeb()->Ldr->InInitializationOrderModuleList.Flink, LDR_MODULE, InInitializationOrderLinks);
    FirstLdr = Ldr;

    do
    {
        Ldr = FIELD_BASE(Ldr->InInitializationOrderLinks.Flink, LDR_MODULE, InInitializationOrderLinks);
        if (Ldr->BaseDllName.Buffer == NULL)
            continue;

        if (CHAR_UPPER4W(*(PULONG64)(Ldr->BaseDllName.Buffer + 0)) != TAG4W('KERN') ||
            CHAR_UPPER4W(*(PULONG64)(Ldr->BaseDllName.Buffer + 4)) != CHAR_UPPER4W(TAG4W('EL32')) ||
            Ldr->BaseDllName.Buffer[8] != '.')
        {
            continue;
        }

        return Ldr;

    } while (FirstLdr != Ldr);

    return NULL;
}

inline PVOID GetKernel32Handle()
{
    return GetKernel32Ldr()->DllBase;
}

ForceInline PLDR_MODULE GetNtdllLdrModule()
{
    return FIELD_BASE(Nt_CurrentPeb()->Ldr->InInitializationOrderModuleList.Flink, LDR_MODULE, InInitializationOrderLinks);
}

ForceInline PVOID GetNtdllHandle()
{
    return GetNtdllLdrModule()->DllBase;
}

inline HANDLE Nt_GetCurrentDirectoryHandle()
{
    return Nt_CurrentPeb()->ProcessParameters->CurrentDirectory.Handle;
}

/************************************************************************
  common file io
************************************************************************/

NTSTATUS
Nt_DeleteFile(
    PCWSTR FileName
);

/************************************************************************
  common path
************************************************************************/

ULONG_PTR
Nt_QueryDosDevice(
    PCWSTR      DeviceName,
    PWSTR       TargetPath,
    ULONG_PTR   PathSize
);


/************************************************************************
  common query api
************************************************************************/

#define FOR_EACH_PROCESS_INFO(_Info) for (ULONG_PTR NextOffset = ULONG_PTR_MAX; NextOffset != 0; NextOffset = (_Info)->NextEntryOffset, (_Info) = PtrAdd((_Info), NextOffset))
#define FOR_EACH_PROCESS_THREAD(_Process, _Thread) for (ULONG_PTR ThreadCount = (((_Thread) = (_Process)->Threads), (_Process)->NumberOfThreads); ThreadCount; ++(_Thread), --ThreadCount)

POBJECT_TYPES_INFORMATION       QuerySystemObjectTypes();
PSYSTEM_HANDLE_INFORMATION_EX   QuerySystemHandles();

BOOL ReleaseSystemInformation(PVOID Processes);

HANDLE QueryCsrssProcessId(ULONG_PTR DEF_VAL(Session, 1));

/************************************************************************
  common callback
************************************************************************/

typedef
VOID
(NTAPI
*PCREATE_PROCESS_NOTIFY)(
    PVOID       Context,
    ULONG_PTR   ProcessId,
    BOOL        Create
);

/************************************************************************
  common process
************************************************************************/

NTSTATUS
Nt_GetProcessUserName(
    PUNICODE_STRING UserName,
    HANDLE          DEF_VAL(Process, NtCurrentProcess())
);

/************************************************************************
  common token
************************************************************************/

NTSTATUS
Nt_GetTokenInfo(
    HANDLE                  TokenHandle,
    TOKEN_INFORMATION_CLASS TokenInformationClass,
    PVOID                   Information,
    PULONG_PTR              DEF_VAL(Size, NULL)
);

VOID
ReleaseTokenInfo(
    PVOID TokenInfo
);

NTSTATUS
Nt_SetProcessThreadToken(
    HANDLE Process,
    HANDLE Thread,
    HANDLE Token
);

ULONG_PTR
Nt_GetSessionId(
    HANDLE DEF_VAL(Process, NtCurrentProcess())
);

NTSTATUS
Nt_AdjustPrivilege(
    ULONG_PTR   Privilege,
    BOOL        DEF_VAL(Enable, TRUE),
    BOOL        DEF_VAL(CurrentThread, FALSE)
);

#if ML_KERNEL_MODE

PLDR_MODULE
LookupPsLoadedModuleList(
    PLDR_MODULE LdrModule,
    PVOID       CallDriverEntry
);

NTSTATUS
QuerySystemModuleByHandle(
    PVOID                           ImageBase,
    PRTL_PROCESS_MODULE_INFORMATION Module
);

NTSTATUS
QueryModuleNameByHandle(
    PVOID           ImageBase,
    PUNICODE_STRING ModuleName
);

NTSTATUS
Nt_WaitForDebugEvent(
    PDBGUI_WAIT_STATE_CHANGE WaitState,
    ULONG Timeout,
    HANDLE DebugObject
);

NTSTATUS
KiQueueUserApc(
    PETHREAD            Thread,
    PKNORMAL_ROUTINE    ApcRoutine,
    PVOID               DEF_VAL(ApcRoutineContext, NULL),
    PVOID               DEF_VAL(Argument1, NULL),
    PVOID               DEF_VAL(Argument2, NULL)
);

#else   // r3

/************************************************************************
  user mode
************************************************************************/

/************************************************************************
  debug api
************************************************************************/

NTSTATUS Nt_DebugActiveProcess(ULONG_PTR ProcessId);
NTSTATUS Nt_DebugActiveProcessStop(ULONG_PTR ProcessId);

NTSTATUS
Nt_WaitForDebugEvent(
    PDBGUI_WAIT_STATE_CHANGE WaitState,
    ULONG Timeout,
    HANDLE DEF_VAL(DebugObject, DbgUiGetThreadDebugObject())
);


/************************************************************************
  process api
************************************************************************/

BOOL
Nt_IsWow64Process(
    HANDLE DEF_VAL(Process, NtCurrentProcess())
);

ULONG Nt_GetErrorMode();
ULONG Nt_SetErrorMode(ULONG Mode);

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
);

NTSTATUS
Nt_ReadMemory(
    HANDLE      ProcessHandle,
    PVOID       BaseAddress,
    PVOID       Buffer,
    ULONG_PTR   Size,
    PULONG_PTR  DEF_VAL(BytesRead, NULL)
);

NTSTATUS
Nt_WriteMemory(
    HANDLE      ProcessHandle,
    PVOID       BaseAddress,
    PVOID       Buffer,
    ULONG_PTR   Size,
    PULONG_PTR  DEF_VAL(BytesWritten, NULL)
);

NTSTATUS
Nt_WriteProtectMemory(
    HANDLE      ProcessHandle,
    PVOID       BaseAddress,
    PVOID       Buffer,
    ULONG_PTR   Size,
    PULONG_PTR  DEF_VAL(BytesWritten, NULL)
);


/************************************************************************
  file io
************************************************************************/

NTSTATUS
Nt_MoveFile(
    PCWSTR ExistingFileName,
    PCWSTR NewFileName
);


NTSTATUS
Nt_CopyFile(
    PCWSTR  ExistingFileName,
    PCWSTR  NewFileName,
    BOOL    DEF_VAL(FailIfExists, FALSE)
);

ULONG_PTR
Nt_GetDosPathFromNtDeviceName(
    PWSTR           DosPath,
    ULONG_PTR       DosPathLength,
    PUNICODE_STRING NtDeviceName
);

NTSTATUS
Nt_GetDosLetterFromVolumeDeviceName(
    PUNICODE_STRING DosLetter,
    PUNICODE_STRING NtDeviceName
);

NTSTATUS
Nt_GetDosPathFromHandle(
    PUNICODE_STRING DosPath,
    HANDLE          FileHandle
);

NTSTATUS
Nt_NtPathNameToDosPathName(
    PUNICODE_STRING DosPath,
    PUNICODE_STRING NtPath
);

ULONG
Nt_GetFileAttributes(
    LPCWSTR FileName
);

PVOID
Nt_GetModuleHandle(
    LPCWSTR lpModuleName
);

ULONG_PTR
Nt_GetModulePath(
    PVOID       ModuleBase,
    PWSTR       Path,
    ULONG_PTR   BufferCount
);

ULONG_PTR
Nt_GetModuleFileName(
    PVOID       ModuleBase,
    LPWSTR      lpFilename,
    ULONG_PTR   nSize
);

BOOL Nt_SetExeDirectoryAsCurrent();

BOOL
Nt_FindNextFile(
    HANDLE              FindFileHandle,
    PWIN32_FIND_DATAW   FindFileData
);

BOOL
Nt_FindClose(
    HANDLE FindFileHandle
);

HANDLE
Nt_FindFirstFile(
    LPCWSTR             FileName,
    PWIN32_FIND_DATAW   FindFileData
);

/************************************************************************
  registry api
************************************************************************/

NTSTATUS
Nt_OpenPredefinedKeyHandle(
    PHANDLE     KeyHandle,
    HKEY        PredefinedKey,
    ACCESS_MASK DEF_VAL(DesiredAccess, KEY_ALL_ACCESS)
);

NTSTATUS
Nt_RegGetValue(
    HANDLE                      hKey,
    PCWSTR                      SubKey,
    PCWSTR                      ValueName,
    KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    PVOID                       KeyValueInformation,
    ULONG                       Length,
    PULONG                      DEF_VAL(ResultLength, NULL),
    ULONG                       DEF_VAL(Flags, 0)
);

NTSTATUS
Nt_RegSetValue(
    HANDLE  hKey,
    PCWSTR  SubKey,
    PCWSTR  ValueName,
    ULONG   ValueType,
    LPCVOID ValueData,
    DWORD   ValueDataLength
);

NTSTATUS
Nt_RegDeleteValue(
    HANDLE  hKey,
    PCWSTR  SubKey,
    PCWSTR  ValueName
);


/************************************************************************
  dll api
************************************************************************/

PVOID
Nt_LoadLibrary(
    PWSTR ModuleFileName
);

PVOID
FASTCALL
Nt_GetProcAddress(
    PVOID ModuleBase,
    PCSTR ProcedureName
);

/************************************************************************
  process api
************************************************************************/

#define INJECT_THREAD_SUSPENDED     (1u << 0)
#define INJECT_ISSUE_REMOTE_BREAKIN (1u << 1)
#define INJECT_POINT_TO_SHELL_CODE  (1u << 2)
#define INJECT_ISSUE_BREAKIN        (1u << 3)
#define INJECT_PREALLOC_BUFFER      (1u << 4)
#define INJECT_DONT_CHANGE_IP       (1u << 5)
#define INJECT_ISSUE_BREAKIN_ONLY   (1u << 6)

NTSTATUS
InjectDllToRemoteProcess(
    HANDLE          ProcessHandle,
    HANDLE          ThreadHandle,
    PUNICODE_STRING DllFullPath,
    ULONG_PTR       Flags,
    PVOID*          DEF_VAL(InjectBuffer, NULL),
    ULONG_PTR       DEF_VAL(Timeout, 500)
);

NTSTATUS
Nt_CreateProcess(
    HANDLE                  Token,
    PCWSTR                  ApplicationName,
    PWSTR                   DEF_VAL(CommandLine, NULL),
    PCWSTR                  DEF_VAL(CurrentDirectory, NULL),
    ULONG                   DEF_VAL(CreationFlags, 0),
    LPSTARTUPINFOW          DEF_VAL(StartupInfo, NULL),
    LPPROCESS_INFORMATION   DEF_VAL(ProcessInformation, NULL),
    LPSECURITY_ATTRIBUTES   DEF_VAL(ProcessAttributes, NULL),
    LPSECURITY_ATTRIBUTES   DEF_VAL(ThreadAttributes, NULL),
    PVOID                   DEF_VAL(Environment, NULL)
);

// NTSTATUS
// Nt_CreateProcess2(
//     CreateProcessInternal2Routine   Routine,
//     HANDLE                          Token,
//     PCWSTR                          ApplicationName,
//     PWSTR                           DEF_VAL(CommandLine, NULL),
//     PCWSTR                          DEF_VAL(CurrentDirectory, NULL),
//     ULONG                           DEF_VAL(CreationFlags, 0),
//     LPSTARTUPINFOW                  DEF_VAL(StartupInfo, NULL),
//     LPPROCESS_INFORMATION           DEF_VAL(ProcessInformation, NULL),
//     LPSECURITY_ATTRIBUTES           DEF_VAL(ProcessAttributes, NULL),
//     LPSECURITY_ATTRIBUTES           DEF_VAL(ThreadAttributes, NULL),
//     PVOID                           DEF_VAL(Environment, NULL)
// );

ULONG
Nt_WaitForSingleObjectEx(
    IN HANDLE hHandle,
    IN ULONG  Milliseconds,
    IN BOOL   bAlertable
);

VOID MessageBoxErrorW(HWND hWnd, NTSTATUS Status, LPWSTR lpCaption, UINT uType);
VOID PrintError(NTSTATUS Status);
VOID Nt_Sleep(ULONG_PTR Milliseconds, BOOL DEF_VAL(Alertable, FALSE));
BOOL Nt_TerminateThread(IN HANDLE hThread, IN ULONG dwExitCode);

ForceInline PWCHAR Nt_GetCommandLine()
{
    return Nt_CurrentPeb()->ProcessParameters->CommandLine.Buffer;
}

ForceInline PVOID Nt_GetExeModuleHandle()
{
    return (PVOID)Nt_CurrentPeb()->ImageBaseAddress;
}

ForceInline ULONG_PTR Nt_GetCurrentProcessId()
{
    return (ULONG_PTR)Nt_CurrentTeb()->ClientId.UniqueProcess;
}

ForceInline ULONG_PTR Nt_GetCurrentThreadId()
{
    return (ULONG_PTR)Nt_CurrentTeb()->ClientId.UniqueThread;
}

ULONG
Nt_GetSystemDirectory64(
    PWSTR Buffer,
    ULONG BufferCount
);

ULONG
Nt_GetWindowsDirectory(
    PWSTR Buffer,
    ULONG BufferCount
);

ULONG
Nt_GetSystemDirectory(
    PWSTR Buffer,
    ULONG BufferCount
);

ULONG
Nt_GetExeDirectory(
    PWSTR Path,
    ULONG BufferCount
);

inline
ULONG
Nt_GetCurrentDirectory(
    ULONG BufferCount,
    PWSTR Buffer
)
{
    return RtlGetCurrentDirectory_U(BufferCount * sizeof(WCHAR), Buffer) / sizeof(WCHAR);
}

inline
BOOL
Nt_SetCurrentDirectory(
    PCWSTR PathName
)
{
    UNICODE_STRING Path;

    RtlInitUnicodeString(&Path, PathName);
    return NT_SUCCESS(RtlSetCurrentDirectory_U(&Path));
}

VOID
Nt_ExitProcess(
    ULONG ExitCode
);

inline
VOID
SetLastNTError(
    NTSTATUS Status
)
{
    RtlSetLastWin32ErrorAndNtStatusFromNtStatus(Status);
}

ForceInline
VOID
BaseSetLastNTError(
    NTSTATUS Status
)
{
    SetLastNTError(Status);
}

NTSTATUS
Nt_CreateThread(
    PVOID                   StartAddress,
    PVOID                   DEF_VAL(StartParameter, NULL),
    BOOL                    DEF_VAL(CreateSuspended, FALSE),
    HANDLE                  DEF_VAL(ProcessHandle, NtCurrentProcess()),
    PHANDLE                 DEF_VAL(ThreadHandle, NULL),
    PCLIENT_ID              DEF_VAL(ClientID, NULL),
    ULONG                   DEF_VAL(StackZeroBits, 0),
    ULONG                   DEF_VAL(StackReserved, 0),
    ULONG                   DEF_VAL(StackCommit, 0),
    PSECURITY_DESCRIPTOR    DEF_VAL(SecurityDescriptor, NULL)
);

NTSTATUS
Nt_SetThreadPriority(
    HANDLE  ThreadHandle,
    LONG    Priority
);

VOID
InitializeListHead(
    PLIST_ENTRY ListHead
);

BOOL
IsListEmpty(
    LIST_ENTRY *ListHead
);

BOOL
RemoveEntryList(
    PLIST_ENTRY Entry
);

PLIST_ENTRY
RemoveHeadList(
    PLIST_ENTRY ListHead
);

PLIST_ENTRY
RemoveTailList(
    PLIST_ENTRY ListHead
);

VOID
InsertTailList(
    PLIST_ENTRY ListHead,
    PLIST_ENTRY Entry
);

VOID
InsertHeadList(
    PLIST_ENTRY ListHead,
    PLIST_ENTRY Entry
);

VOID
AppendTailList(
    PLIST_ENTRY ListHead,
    PLIST_ENTRY ListToAppend
);

PSINGLE_LIST_ENTRY
PopEntryList(
    PSINGLE_LIST_ENTRY ListHead
);

VOID
PushEntryList(
    PSINGLE_LIST_ENTRY ListHead,
    PSINGLE_LIST_ENTRY Entry
);

PLIST_ENTRY
ExInterlockedInsertHeadList(
    PLIST_ENTRY         ListHead,
    PLIST_ENTRY         Entry,
    PCRITICAL_SECTION   Lock
);

PLIST_ENTRY
ExInterlockedInsertTailList(
    PLIST_ENTRY         ListHead,
    PLIST_ENTRY         Entry,
    PCRITICAL_SECTION   Lock
);

PLIST_ENTRY
ExInterlockedRemoveHeadList(
    PLIST_ENTRY         ListHead,
    PCRITICAL_SECTION   Lock
);

PSINGLE_LIST_ENTRY
ExInterlockedPopEntryList(
    PSINGLE_LIST_ENTRY  ListHead,
    PCRITICAL_SECTION   Lock
);

PSINGLE_LIST_ENTRY
ExInterlockedPushEntryList(
    PSINGLE_LIST_ENTRY  ListHead,
    PSINGLE_LIST_ENTRY  Entry,
    PCRITICAL_SECTION   Lock
);

VOID
Nt_InitializeSListHead(
    PSLIST_HEADER SListHead
);

#endif  // MY_NT_DDK

_ML_C_TAIL_

#endif // _NATIVEAPIHELPER_H_146120be_f089_49ef_bfd3_82c455e90ba3