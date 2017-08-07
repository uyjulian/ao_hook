#ifndef _NTDEBUG_H_c22191c1_3c8c_4044_84cc_7520d0bd5829_
#define _NTDEBUG_H_c22191c1_3c8c_4044_84cc_7520d0bd5829_


/************************************************************************
  debug
************************************************************************/
//
// Debug States
//
typedef enum _DBG_STATE
{
    DbgIdle                         = 0,
    DbgReplyPending                 = 1,
    DbgCreateThreadStateChange      = 2,
    DbgCreateProcessStateChange     = 3,
    DbgExitThreadStateChange        = 4,
    DbgExitProcessStateChange       = 5,
    DbgExceptionStateChange         = 6,
    DbgBreakpointStateChange        = 7,
    DbgSingleStepStateChange        = 8,
    DbgLoadDllStateChange           = 9,
    DbgUnloadDllStateChange         = 10,

} DBG_STATE, *PDBG_STATE;

//
// Debug Object Access Masks
//
#define DEBUG_OBJECT_WAIT_STATE_CHANGE      0x0001
#define DEBUG_OBJECT_ADD_REMOVE_PROCESS     0x0002
#define DEBUG_OBJECT_SET_INFORMATION        0x0004
#define DEBUG_OBJECT_ALL_ACCESS             (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x0F)

#define DBGK_KILL_PROCESS_ON_EXIT           1

//
// Debug Message Structures
//
typedef struct _DBGKM_EXCEPTION
{
    EXCEPTION_RECORD ExceptionRecord;
    ULONG FirstChance;
} DBGKM_EXCEPTION, *PDBGKM_EXCEPTION;

typedef struct _DBGKM_CREATE_THREAD
{
    ULONG SubSystemKey;
    PVOID StartAddress;
} DBGKM_CREATE_THREAD, *PDBGKM_CREATE_THREAD;

typedef struct _DBGKM_CREATE_PROCESS
{
    ULONG       SubSystemKey;
    HANDLE      FileHandle;
    PVOID       BaseOfImage;
    ULONG_PTR   DebugInfoFileOffset;
    ULONG_PTR   DebugInfoSize;
    DBGKM_CREATE_THREAD InitialThread;
} DBGKM_CREATE_PROCESS, *PDBGKM_CREATE_PROCESS;

typedef struct _DBGKM_EXIT_THREAD
{
    NTSTATUS ExitStatus;
} DBGKM_EXIT_THREAD, *PDBGKM_EXIT_THREAD;

typedef struct _DBGKM_EXIT_PROCESS
{
    NTSTATUS ExitStatus;
} DBGKM_EXIT_PROCESS, *PDBGKM_EXIT_PROCESS;

typedef struct _DBGKM_LOAD_DLL
{
    HANDLE  FileHandle;
    PVOID   BaseOfDll;
    ULONG   DebugInfoFileOffset;
    ULONG   DebugInfoSize;
    PVOID   NamePointer;
} DBGKM_LOAD_DLL, *PDBGKM_LOAD_DLL;

typedef struct _DBGKM_UNLOAD_DLL
{
    PVOID BaseAddress;
} DBGKM_UNLOAD_DLL, *PDBGKM_UNLOAD_DLL;

typedef struct _DBGUI_CREATE_THREAD {
    HANDLE HandleToThread;
    DBGKM_CREATE_THREAD NewThread;
} DBGUI_CREATE_THREAD, *PDBGUI_CREATE_THREAD;

typedef struct _DBGUI_CREATE_PROCESS {
    HANDLE HandleToProcess;
    HANDLE HandleToThread;
    DBGKM_CREATE_PROCESS NewProcess;
} DBGUI_CREATE_PROCESS, *PDBGUI_CREATE_PROCESS;

//
// User-Mode Debug State Change Structure
//
typedef struct _DBGUI_WAIT_STATE_CHANGE
{
    DBG_STATE NewState;
    CLIENT_ID AppClientId;
    union
    {
        DBGKM_EXCEPTION         Exception;
        DBGUI_CREATE_THREAD     CreateThread;
        DBGUI_CREATE_PROCESS    CreateProcessInfo;
        DBGKM_EXIT_THREAD       ExitThread;
        DBGKM_EXIT_PROCESS      ExitProcess;
        DBGKM_LOAD_DLL          LoadDll;
        DBGKM_UNLOAD_DLL        UnloadDll;
    } StateInfo;

} DBGUI_WAIT_STATE_CHANGE, *PDBGUI_WAIT_STATE_CHANGE;


#if ML_USER_MODE

NATIVE_API
VOID
NTAPI
DbgBreakPoint(
    VOID
);

NATIVE_API
ULONG
CDECL
DbgPrint(
    PCSTR Format,
    ...
);

#endif // ML_USER_MODE

NATIVE_API
NTSTATUS
NTAPI
DbgUiConnectToDbg(
    VOID
);

NATIVE_API
HANDLE
NTAPI
DbgUiGetThreadDebugObject(
    VOID
);

NATIVE_API
NTSTATUS
NTAPI
DbgUiIssueRemoteBreakin(
    HANDLE Process
);

NATIVE_API
NTSTATUS
NTAPI
DbgUiContinue(
    IN PCLIENT_ID   ClientId,
    IN NTSTATUS     ContinueStatus
);

NATIVE_API
NTSTATUS
NTAPI
DbgUiDebugActiveProcess(
    IN HANDLE Process
);

NATIVE_API
NTSTATUS
NTAPI
DbgUiStopDebugging(
    IN HANDLE Process
);

NATIVE_API
NTSTATUS
NTAPI
DbgUiWaitStateChange(
    IN PDBGUI_WAIT_STATE_CHANGE StateChange,
    IN PLARGE_INTEGER           TimeOut
);

NATIVE_API
NTSTATUS
NTAPI
DbgUiConvertStateChangeStructure(
    IN PDBGUI_WAIT_STATE_CHANGE WaitStateChange,
    IN DEBUG_EVENT             *DebugEvent
);

NATIVE_API
BOOL
NTAPI
DbgUiRemoteBreakin(
    PVOID Parameter
);

NATIVE_API
NTSTATUS
NTAPI
NtCreateDebugObject(
    OUT PHANDLE             DebugObject,
    IN  ACCESS_MASK         DebugObjectAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    IN  ULONG               Flags
);

NATIVE_API
NTSTATUS
NTAPI
NtDebugActiveProcess(
    IN HANDLE DebuggeeProcess,
    IN HANDLE DebugObject
);

NATIVE_API
NTSTATUS
NTAPI
NtRemoveProcessDebug(
    IN HANDLE DebuggeeProcess,
    IN HANDLE DebugObject
);

NATIVE_API
NTSTATUS
NTAPI
NtDebugContinue(
    IN HANDLE       DebugObjectHandle,
    IN PCLIENT_ID   ClientId,
    IN NTSTATUS     ContinueStatus
);

NATIVE_API
NTSTATUS
NTAPI
NtSetInformationDebugObject(
    IN  HANDLE  DebugObject,
    IN  ULONG   InformationClass,
    IN  PVOID   Information,
    IN  ULONG   InformationLength,
    OUT PULONG  ReturnLength OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtWaitForDebugEvent(
    IN  HANDLE                      DebugObject,
    IN  BOOLEAN                     Alertable,
    IN  PLARGE_INTEGER              Timeout OPTIONAL,
    OUT PDBGUI_WAIT_STATE_CHANGE    StateChange
);

#endif // _NTDEBUG_H_c22191c1_3c8c_4044_84cc_7520d0bd5829_