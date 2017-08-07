#ifndef _NATIVETYPES_COMMON_H_e6f6abc9_91e7_452a_b418_671874fe30b3
#define _NATIVETYPES_COMMON_H_e6f6abc9_91e7_452a_b418_671874fe30b3

#pragma warning(disable:4103)



//#pragma warning(disable:4201)

_ML_C_HEAD_

#if !defined(FAIL_RETURN)
    #define FAIL_RETURN(Status) { NTSTATUS __Status__;  __Status__ = (Status); if (!NT_SUCCESS(__Status__)) return __Status__; }
    #define FAIL_CONTINUE(Status) if (!NT_SUCCESS(Status)) continue
    #define FAIL_BREAK(Status) if (!NT_SUCCESS(Status)) break
    #define SUCCESS_RETURN(Status) { NTSTATUS __Status__;  __Status__ = (Status); if (NT_SUCCESS(__Status__)) return __Status__; }
#endif  // FAIL_RETURN

#ifndef NT_FAILED
    #define NT_FAILED !NT_SUCCESS
#endif // NT_FAILED


#ifndef FACILITY_NTWIN32
    #define FACILITY_NTWIN32                 0x7
#endif // FACILITY_NTWIN32

#define DEFINE_NTSTATUS(Severity, Code) \
            (((ULONG)(Severity) << 30) | (1 << 29) | (FACILITY_NTWIN32 << 16) | (USHORT)(Code))




/************************************************************************
  base
************************************************************************/

typedef struct _CLIENT_ID64
{
    ULONG64  UniqueProcess;
    ULONG64  UniqueThread;

} CLIENT_ID64, *PCLIENT_ID64;

/************************************************************************
  strings
************************************************************************/

typedef struct _RTL_BUFFER
{
/* 0x000 */ PUCHAR                         Buffer;
/* 0x004 */ PUCHAR                         StaticBuffer;
/* 0x008 */ ULONG                          Size;
/* 0x00C */ ULONG                          StaticSize;
/* 0x010 */ ULONG                          ReservedForAllocatedSize;
/* 0x014 */ PVOID                          ReservedForIMalloc;

} RTL_BUFFER, *PRTL_BUFFER;

typedef struct _RTL_UNICODE_STRING_BUFFER
{
/* 0x000 */ UNICODE_STRING                 String;
/* 0x008 */ RTL_BUFFER                     ByteBuffer;
/* 0x020 */ UCHAR                          MinimumStaticBufferForTerminalNul[2];

} RTL_UNICODE_STRING_BUFFER, *PRTL_UNICODE_STRING_BUFFER;

//
// These must be converted to LUIDs before use.
//

#if ML_USER_MODE

enum
{
    SE_MIN_WELL_KNOWN_PRIVILEGE         = 2L,

    SE_CREATE_TOKEN_PRIVILEGE           = 2L,
    SE_ASSIGNPRIMARYTOKEN_PRIVILEGE     = 3L,
    SE_LOCK_MEMORY_PRIVILEGE            = 4L,
    SE_INCREASE_QUOTA_PRIVILEGE         = 5L,

    SE_MACHINE_ACCOUNT_PRIVILEGE        = 6L,
    SE_TCB_PRIVILEGE                    = 7L,
    SE_SECURITY_PRIVILEGE               = 8L,
    SE_TAKE_OWNERSHIP_PRIVILEGE         = 9L,
    SE_LOAD_DRIVER_PRIVILEGE            = 10L,
    SE_SYSTEM_PROFILE_PRIVILEGE         = 11L,
    SE_SYSTEMTIME_PRIVILEGE             = 12L,
    SE_PROF_SINGLE_PROCESS_PRIVILEGE    = 13L,
    SE_INC_BASE_PRIORITY_PRIVILEGE      = 14L,
    SE_CREATE_PAGEFILE_PRIVILEGE        = 15L,
    SE_CREATE_PERMANENT_PRIVILEGE       = 16L,
    SE_BACKUP_PRIVILEGE                 = 17L,
    SE_RESTORE_PRIVILEGE                = 18L,
    SE_SHUTDOWN_PRIVILEGE               = 19L,
    SE_DEBUG_PRIVILEGE                  = 20L,
    SE_AUDIT_PRIVILEGE                  = 21L,
    SE_SYSTEM_ENVIRONMENT_PRIVILEGE     = 22L,
    SE_CHANGE_NOTIFY_PRIVILEGE          = 23L,
    SE_REMOTE_SHUTDOWN_PRIVILEGE        = 24L,
    SE_UNDOCK_PRIVILEGE                 = 25L,
    SE_SYNC_AGENT_PRIVILEGE             = 26L,
    SE_ENABLE_DELEGATION_PRIVILEGE      = 27L,
    SE_MANAGE_VOLUME_PRIVILEGE          = 28L,
    SE_IMPERSONATE_PRIVILEGE            = 29L,
    SE_CREATE_GLOBAL_PRIVILEGE          = 30L,
    SE_TRUSTED_CREDMAN_ACCESS_PRIVILEGE = 31L,
    SE_RELABEL_PRIVILEGE                = 32L,
    SE_INC_WORKING_SET_PRIVILEGE        = 33L,
    SE_TIME_ZONE_PRIVILEGE              = 34L,
    SE_CREATE_SYMBOLIC_LINK_PRIVILEGE   = 35L,

    SE_MAX_WELL_KNOWN_PRIVILEGE
};

#endif


/************************************************************************
  afd
************************************************************************/

#if ML_USER_MODE

typedef struct _MDL
{
    struct _MDL *Next;
    SHORT Size;
    SHORT MdlFlags;

    struct _EPROCESS *Process;
    PVOID MappedSystemVa;   /* see creators for field size annotations. */
    PVOID StartVa;   /* see creators for validity; could be address 0.  */
    ULONG ByteCount;
    ULONG ByteOffset;

} MDL, *PMDL;

#endif // r3

typedef struct _AFD_MAPBUF
{
     PVOID BufferAddress;
     PMDL  Mdl;

} AFD_MAPBUF, *PAFD_MAPBUF;

typedef struct _AFD_WSABUF
{
     ULONG  Length;
     PCHAR  Buffer;

} AFD_WSABUF, *PAFD_WSABUF;

typedef struct  _AFD_RECV_INFO
{
     PAFD_WSABUF    BufferArray;
     ULONG          BufferCount;
     ULONG          AfdFlags;
     ULONG          TdiFlags;

} AFD_RECV_INFO , *PAFD_RECV_INFO;

typedef struct _AFD_RECV_INFO_UDP
{
    PAFD_WSABUF     BufferArray;
    ULONG           BufferCount;
    ULONG           AfdFlags;
    ULONG           TdiFlags;
    PVOID           Address;
    PLONG           AddressLength;

} AFD_RECV_INFO_UDP, *PAFD_RECV_INFO_UDP;

typedef struct  _AFD_SEND_INFO
{
    PAFD_WSABUF BufferArray;
    ULONG       BufferCount;
    ULONG       AfdFlags;
    ULONG       TdiFlags;

} AFD_SEND_INFO , *PAFD_SEND_INFO;

typedef struct _AFD_SEND_INFO_UDP
{
    PAFD_WSABUF BufferArray;
    ULONG       BufferCount;
    ULONG       AfdFlags;
    ULONG       Padding[9];
    ULONG       SizeOfRemoteAddress;
    PVOID       RemoteAddress;

} AFD_SEND_INFO_UDP, *PAFD_SEND_INFO_UDP;

/* IOCTL Generation */
#define FSCTL_AFD_BASE                      FILE_DEVICE_NETWORK
#define _AFD_CONTROL_CODE(Operation, Method) ((FSCTL_AFD_BASE)<<12 | (Operation<<2) | Method)

/* AFD Commands */
#define AFD_BIND                        0
#define AFD_CONNECT                     1
#define AFD_START_LISTEN                2
#define AFD_WAIT_FOR_LISTEN             3
#define AFD_ACCEPT                      4
#define AFD_RECV                        5
#define AFD_RECV_DATAGRAM               6
#define AFD_SEND                        7
#define AFD_SEND_DATAGRAM               8
#define AFD_SELECT                      9
#define AFD_DISCONNECT                  10
#define AFD_GET_SOCK_NAME               11
#define AFD_GET_PEER_NAME               12
#define AFD_GET_TDI_HANDLES             13
#define AFD_SET_INFO                    14
#define AFD_GET_CONTEXT_SIZE            15
#define AFD_GET_CONTEXT                 16
#define AFD_SET_CONTEXT                 17
#define AFD_SET_CONNECT_DATA            18
#define AFD_SET_CONNECT_OPTIONS         19
#define AFD_SET_DISCONNECT_DATA         20
#define AFD_SET_DISCONNECT_OPTIONS      21
#define AFD_GET_CONNECT_DATA            22
#define AFD_GET_CONNECT_OPTIONS         23
#define AFD_GET_DISCONNECT_DATA         24
#define AFD_GET_DISCONNECT_OPTIONS      25
#define AFD_SET_CONNECT_DATA_SIZE       26
#define AFD_SET_CONNECT_OPTIONS_SIZE    27
#define AFD_SET_DISCONNECT_DATA_SIZE    28
#define AFD_SET_DISCONNECT_OPTIONS_SIZE 29
#define AFD_GET_INFO                    30
#define AFD_EVENT_SELECT                33
#define AFD_ENUM_NETWORK_EVENTS         34
#define AFD_DEFER_ACCEPT                35
#define AFD_GET_PENDING_CONNECT_DATA    41
#define AFD_VALIDATE_GROUP              42

/* AFD IOCTLs */

#define IOCTL_AFD_BIND                          _AFD_CONTROL_CODE(AFD_BIND,                         METHOD_NEITHER)     //  0x12003
#define IOCTL_AFD_CONNECT                       _AFD_CONTROL_CODE(AFD_CONNECT,                      METHOD_NEITHER)     //  0x12007
#define IOCTL_AFD_START_LISTEN                  _AFD_CONTROL_CODE(AFD_START_LISTEN,                 METHOD_NEITHER)     //  0x1200B
#define IOCTL_AFD_WAIT_FOR_LISTEN               _AFD_CONTROL_CODE(AFD_WAIT_FOR_LISTEN,              METHOD_BUFFERED)    //  0x1200C
#define IOCTL_AFD_ACCEPT                        _AFD_CONTROL_CODE(AFD_ACCEPT,                       METHOD_BUFFERED)    //  0x12010
#define IOCTL_AFD_RECV                          _AFD_CONTROL_CODE(AFD_RECV,                         METHOD_NEITHER)     //  0x12017
#define IOCTL_AFD_RECV_DATAGRAM                 _AFD_CONTROL_CODE(AFD_RECV_DATAGRAM,                METHOD_NEITHER)     //  0x1201B
#define IOCTL_AFD_SEND                          _AFD_CONTROL_CODE(AFD_SEND,                         METHOD_NEITHER)     //  0x1201F
#define IOCTL_AFD_SEND_DATAGRAM                 _AFD_CONTROL_CODE(AFD_SEND_DATAGRAM,                METHOD_NEITHER)     //  0x12023
#define IOCTL_AFD_SELECT                        _AFD_CONTROL_CODE(AFD_SELECT,                       METHOD_BUFFERED)    //  0x12024
#define IOCTL_AFD_DISCONNECT                    _AFD_CONTROL_CODE(AFD_DISCONNECT,                   METHOD_NEITHER)     //  0x1202B
#define IOCTL_AFD_GET_SOCK_NAME                 _AFD_CONTROL_CODE(AFD_GET_SOCK_NAME,                METHOD_NEITHER)     //  0x1202F
#define IOCTL_AFD_GET_PEER_NAME                 _AFD_CONTROL_CODE(AFD_GET_PEER_NAME,                METHOD_NEITHER)     //  0x12033
#define IOCTL_AFD_GET_TDI_HANDLES               _AFD_CONTROL_CODE(AFD_GET_TDI_HANDLES,              METHOD_NEITHER)     //  0x12037
#define IOCTL_AFD_SET_INFO                      _AFD_CONTROL_CODE(AFD_SET_INFO,                     METHOD_NEITHER)     //  0x1203B
#define IOCTL_AFD_GET_CONTEXT_SIZE              _AFD_CONTROL_CODE(AFD_GET_CONTEXT_SIZE,             METHOD_NEITHER)     //  0x1203F
#define IOCTL_AFD_GET_CONTEXT                   _AFD_CONTROL_CODE(AFD_GET_CONTEXT,                  METHOD_NEITHER)     //  0x12043
#define IOCTL_AFD_SET_CONTEXT                   _AFD_CONTROL_CODE(AFD_SET_CONTEXT,                  METHOD_NEITHER)     //  0x12047
#define IOCTL_AFD_SET_CONNECT_DATA              _AFD_CONTROL_CODE(AFD_SET_CONNECT_DATA,             METHOD_NEITHER)     //  0x1204B
#define IOCTL_AFD_SET_CONNECT_OPTIONS           _AFD_CONTROL_CODE(AFD_SET_CONNECT_OPTIONS,          METHOD_NEITHER)     //  0x1204F
#define IOCTL_AFD_SET_DISCONNECT_DATA           _AFD_CONTROL_CODE(AFD_SET_DISCONNECT_DATA,          METHOD_NEITHER)     //  0x12053
#define IOCTL_AFD_SET_DISCONNECT_OPTIONS        _AFD_CONTROL_CODE(AFD_SET_DISCONNECT_OPTIONS,       METHOD_NEITHER)     //  0x12057
#define IOCTL_AFD_GET_CONNECT_DATA              _AFD_CONTROL_CODE(AFD_GET_CONNECT_DATA,             METHOD_NEITHER)     //  0x1205B
#define IOCTL_AFD_GET_CONNECT_OPTIONS           _AFD_CONTROL_CODE(AFD_GET_CONNECT_OPTIONS,          METHOD_NEITHER)     //  0x1205F
#define IOCTL_AFD_GET_DISCONNECT_DATA           _AFD_CONTROL_CODE(AFD_GET_DISCONNECT_DATA,          METHOD_NEITHER)     //  0x12063
#define IOCTL_AFD_GET_DISCONNECT_OPTIONS        _AFD_CONTROL_CODE(AFD_GET_DISCONNECT_OPTIONS,       METHOD_NEITHER)     //  0x12067
#define IOCTL_AFD_SET_CONNECT_DATA_SIZE         _AFD_CONTROL_CODE(AFD_SET_CONNECT_DATA_SIZE,        METHOD_NEITHER)     //  0x1206B
#define IOCTL_AFD_SET_CONNECT_OPTIONS_SIZE      _AFD_CONTROL_CODE(AFD_SET_CONNECT_OPTIONS_SIZE,     METHOD_NEITHER)     //  0x1206F
#define IOCTL_AFD_SET_DISCONNECT_DATA_SIZE      _AFD_CONTROL_CODE(AFD_SET_DISCONNECT_DATA_SIZE,     METHOD_NEITHER)     //  0x12073
#define IOCTL_AFD_SET_DISCONNECT_OPTIONS_SIZE   _AFD_CONTROL_CODE(AFD_SET_DISCONNECT_OPTIONS_SIZE,  METHOD_NEITHER)     //  0x12077
#define IOCTL_AFD_GET_INFO                      _AFD_CONTROL_CODE(AFD_GET_INFO,                     METHOD_NEITHER)     //  0x1207B
#define IOCTL_AFD_EVENT_SELECT                  _AFD_CONTROL_CODE(AFD_EVENT_SELECT,                 METHOD_NEITHER)     //  0x12087
#define IOCTL_AFD_DEFER_ACCEPT                  _AFD_CONTROL_CODE(AFD_DEFER_ACCEPT,                 METHOD_NEITHER)     //  0x1208F
#define IOCTL_AFD_GET_PENDING_CONNECT_DATA      _AFD_CONTROL_CODE(AFD_GET_PENDING_CONNECT_DATA,     METHOD_NEITHER)     //  0x120A7
#define IOCTL_AFD_ENUM_NETWORK_EVENTS           _AFD_CONTROL_CODE(AFD_ENUM_NETWORK_EVENTS,          METHOD_NEITHER)     //  0x1208B
#define IOCTL_AFD_VALIDATE_GROUP                _AFD_CONTROL_CODE(AFD_VALIDATE_GROUP,               METHOD_NEITHER)     //  0x120AB

/************************************************************************
  iocp api
************************************************************************/

typedef enum _IO_COMPLETION_INFORMATION_CLASS
{
    IoCompletionBasicInformation,

} IO_COMPLETION_INFORMATION_CLASS;

/************************************************************************
  shim engine
************************************************************************/
typedef enum
{
    SE_PROCESS_ATTACH     = 1,
    SE_PROCESS_DETACH     = 2,
    SE_DLL_LOAD           = 3,
} SHIM_NOTIFY_REASON;

typedef struct
{
    PCSTR   ModuleName;
    PCSTR   FunctionName;
    PVOID   NewRoutine;
    PVOID   Unknown[3];
} SHIM_HOOK_API_INFO, *PSHIM_HOOK_API_INFO;

/************************************************************************
  timer api
************************************************************************/

typedef enum
{
    TimerBasicInformation   = 0,

} TIMER_INFORMATION_CLASS;

typedef struct _TIMER_BASIC_INFORMATION
{
    LARGE_INTEGER   RemainingTime;
    BOOLEAN         TimerState;

} TIMER_BASIC_INFORMATION, *PTIMER_BASIC_INFORMATION;

/************************************************************************
  misc
************************************************************************/

#define DELAY_ONE_MICROSECOND   (-10)
#define DELAY_ONE_MILLISECOND   (DELAY_ONE_MICROSECOND * 1000)
#define DELAY_ONE_SECOND        (DELAY_ONE_MILLISECOND * 1000)
#define DELAY_QUAD_INFINITE     0x8000000000000000ll

#if !ML_KERNEL_MODE

typedef enum
{
    NotificationTimer       = 0,
    SynchronizationTimer    = 1,
} TIMER_TYPE;

#endif // ML_KERNEL_MODE


_ML_C_TAIL_


#pragma warning(default:4103)

//#pragma warning(default:4201)

#endif // _NATIVETYPES_COMMON_H_e6f6abc9_91e7_452a_b418_671874fe30b3