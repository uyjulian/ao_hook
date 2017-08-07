#ifndef _NTLPC_H_add4da86_44d6_42e5_801f_23fe1de78ffe_
#define _NTLPC_H_add4da86_44d6_42e5_801f_23fe1de78ffe_


#if defined(USE_LPC6432)

    #define LPC_CLIENT_ID CLIENT_ID64
    #define LPC_SIZE_T ULONGLONG
    #define LPC_PVOID ULONGLONG
    #define LPC_HANDLE ULONGLONG

    typedef CLIENT_ID64 LPC_CLIENT_ID;
    typedef ULONGLONG   LPC_SIZE_T, LPC_SIZE;
    typedef ULONGLONG   LPC_PVOID;
    typedef ULONGLONG   LPC_HANDLE;

#else

    #define LPC_CLIENT_ID CLIENT_ID
    #define LPC_SIZE_T SIZE_T
    #define LPC_PVOID PVOID
    #define LPC_HANDLE HANDLE

    typedef CLIENT_ID   LPC_CLIENT_ID;
    typedef ULONG_PTR   LPC_SIZE_T, LPC_SIZE;
    typedef PVOID       LPC_PVOID;
    typedef HANDLE      LPC_HANDLE;

#endif


typedef NTSTATUS LPC_STATUS;


//
// Valid return values for the PORT_MESSAGE Type file
//

enum LPC_REQUEST_TYPE
{
    LPC_NEW_MESSAGE         =  0,
    LPC_REQUEST             =  1,
    LPC_REPLY               =  2,
    LPC_DATAGRAM            =  3,
    LPC_LOST_REPLY          =  4,
    LPC_PORT_CLOSED         =  5,
    LPC_CLIENT_DIED         =  6,
    LPC_EXCEPTION           =  7,
    LPC_DEBUG_EVENT         =  8,
    LPC_ERROR_EVENT         =  9,
    LPC_CONNECTION_REQUEST  = 10,
};

typedef struct
{
    USHORT          DataLength;             // Length of data following header (bytes)
    USHORT          TotalLength;            // Length of data + sizeof(PORT_MESSAGE)
    USHORT          Type;                   // Type of the message (LPC_TYPE)
    USHORT          DataInfoOffset;         // Offset of array of virtual address ranges
    LPC_CLIENT_ID   ClientId;               // Client identifier of the message sender
    ULONG           MessageId;              // Identifier of the particular message instance
    union
    {
        ULONG ClientViewSize;               // Size, in bytes, of section created by the sender
        ULONG CallbackId;
    };

    // UCHAR Data[PORT_MAXIMUM_MESSAGE_LENGTH];

} PORT_MESSAGE, *PPORT_MESSAGE;

typedef struct _PORT_DATA_ENTRY
{
    LPC_PVOID   Base;
    ULONG       Size;

} PORT_DATA_ENTRY, *PPORT_DATA_ENTRY;

typedef struct _PORT_DATA_INFORMATION
{
    ULONG           CountDataEntries;
    PORT_DATA_ENTRY DataEntries[1];

} PORT_DATA_INFORMATION, *PPORT_DATA_INFORMATION;

// begin_ntifs

//
// The following bit may be placed in the Type field of a message
// prior calling NtRequestPort or NtRequestWaitReplyPort.  If the
// previous mode is KernelMode, the bit it left as is and passed
// to the receiver of the message.  Otherwise the bit is clear.
//

#define LPC_KERNELMODE_MESSAGE  (SHORT)0x8000

// end_ntifs

#define LPC_NO_IMPERSONATE  (SHORT)0x4000

#define PORT_VALID_OBJECT_ATTRIBUTES (OBJ_CASE_INSENSITIVE)

// begin_ntddk begin_wdm
#ifdef _WIN64
#define PORT_MAXIMUM_MESSAGE_LENGTH 512
#else
#define PORT_MAXIMUM_MESSAGE_LENGTH 256
#endif

#define LPC_MAX_CONNECTION_INFO_SIZE (16 * sizeof(ULONG_PTR))

#define PORT_TOTAL_MAXIMUM_MESSAGE_LENGTH ((PORT_MAXIMUM_MESSAGE_LENGTH + sizeof (PORT_MESSAGE) + LPC_MAX_CONNECTION_INFO_SIZE + 15) & ~15)

#define PORT_CREATE_MAXIMUM_MESSAGE_LENGTH          0x148
#define PORT_CREATE_MAXIMUM_CONNECTION_INFO_SIZE    0x104

// end_ntddk end_wdm

#if defined(USE_LPC6432)
    #undef PORT_MAXIMUM_MESSAGE_LENGTH
    #define PORT_MAXIMUM_MESSAGE_LENGTH 512
#endif

typedef struct _LPC_CLIENT_DIED_MSG
{
    PORT_MESSAGE    PortMsg;
    LARGE_INTEGER   CreateTime;

} LPC_CLIENT_DIED_MSG, *PLPC_CLIENT_DIED_MSG;

// begin_ntifs

typedef struct _PORT_VIEW
{
    ULONG       Length;             // Size of this structure
    LPC_HANDLE  SectionHandle;      // Handle to section object with
                                    // SECTION_MAP_WRITE and SECTION_MAP_READ
    ULONG       SectionOffset;      // The offset in the section to map a view for
                                    // the port data area. the offset must be aligned
                                    // with the allocation granularity of the system.
    LPC_SIZE_T  ViewSize;           // The size of the view (in bytes)
    LPC_PVOID   ViewBase;           // The base address of the view in the creator
    LPC_PVOID   ViewRemoteBase;     // The Base address of the view in the process
                                    // connected to the port.

} PORT_VIEW, *PPORT_VIEW;

typedef struct _REMOTE_PORT_VIEW
{
    ULONG       Length;         // Size of this structure
    LPC_SIZE_T  ViewSize;       // The size of the view (bytes)
    LPC_PVOID   ViewBase;       // Base address of the view

} REMOTE_PORT_VIEW, *PREMOTE_PORT_VIEW;

// end_ntifs



#ifndef InitializePortHeader

#define InitializeServerView(__ServerView, __ViewSize, __ViewBase) { \
            (__ServerView)->Length = sizeof(*(__ServerView)); \
            (__ServerView)->ViewSize = (__ViewSize); \
            (__ServerView)->ViewBase = (__ViewBase); }

#define InitializeClientView(__ClientView, __SectionHandle, __SectionOffset, __ViewSize, __ViewBase, __ViewRemoteBase) { \
            (__ClientView)->Length = sizeof(*(__ClientView)); \
            (__ClientView)->SectionHandle = (__SectionHandle); \
            (__ClientView)->SectionOffset = (__SectionOffset); \
            (__ClientView)->ViewSize = (__ViewSize); \
            (__ClientView)->ViewBase = (__ViewBase); \
            (__ClientView)->ViewRemoteBase = (__ViewRemoteBase); }

#define InitializeMessageHeader(ph, l, t) {                        \
    (ph)->TotalLength    = (USHORT) (l);                            \
    (ph)->DataLength    = (USHORT) (l - sizeof(PORT_MESSAGE));    \
    (ph)->Type            = (USHORT) (t);                            \
    (ph)->VirtualRangesOffset = 0;                                \
}

#define InitializeMessageData(msag, view, command, dataaddr, datalen) {        \
    InitializeMessageHeader(&((msag)->Header), 256, LPC_NEW_MESSAGE)        \
    (msag)->Command        = command;                                            \
    (msag)->DataLength  = datalen;                                            \
    if ((datalen) < 224)                                                    \
    {                                                                        \
        CopyMemory((PVOID)(msag)->MessageText,                                    \
               (PVOID)(dataaddr),                                            \
               (ULONG)(datalen));                                            \
    }                                                                        \
    else                                                                    \
    {                                                                        \
    CopyMemory((PVOID)(view)->ViewBase,                                        \
               (PVOID)(dataaddr),                                            \
               (ULONG)(datalen));                                            \
    }                                                                        \
}

#endif  // InitializePortHeader

/*++

    NtCreatePort
    ============

    Creates a LPC port object. The creator of the LPC port becomes a server
    of LPC communication

    PortHandle - Points to a variable that will receive the
        port object handle if the call is successful.

    ObjectAttributes - Points to a structure that specifies the object
        attributes. OBJ_KERNEL_HANDLE, OBJ_OPENLINK, OBJ_OPENIF, OBJ_EXCLUSIVE,
        OBJ_PERMANENT, and OBJ_INHERIT are not valid attributes for a port object.

    MaxConnectionInfoLength - The maximum size, in bytes, of data that can
        be sent through the port.

    MaxMessageLength - The maximum size, in bytes, of a message
        that can be sent through the port.

    MaxPoolUsage - Specifies the maximum amount of NonPaged pool that can be used for
        message storage. Zero means default value.

    ZwCreatePort verifies that (MaxDataSize <= 0x104) and (MaxMessageSize <= 0x148).

--*/

NATIVE_API
NTSTATUS
NTAPI
NtCreatePort(
    OUT PHANDLE             PortHandle,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    IN  ULONG               MaxConnectionInfoLength,
    IN  ULONG               MaxMessageLength,
    IN  ULONG               MaxPoolUsage OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtCreateWaitablePort(
    OUT PHANDLE             PortHandle,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    IN  ULONG               MaxConnectionInfoLength,
    IN  ULONG               MaxMessageLength,
    IN  ULONG               MaxPoolUsage OPTIONAL
);

/*++

    NtConnectPort
    =============

    Creates a port connected to a named port (cliend side).

    PortHandle - A pointer to a variable that will receive the client
        communication port object handle value.

    PortName - Points to a structure that specifies the name
        of the port to connect to.

    SecurityQos - Points to a structure that specifies the level
        of impersonation available to the port listener.

    ClientView - Optionally points to a structure describing
        the shared memory region used to send large amounts of data
        to the listener; if the call is successful, this will be updated.

    ServerView - Optionally points to a caller-allocated buffer
        or variable that receives information on the shared memory region
        used by the listener to send large amounts of data to the
        caller.

    MaxMessageLength - Optionally points to a variable that receives the size,
        in bytes, of the largest message that can be sent through the port.

    ConnectionInformation - Optionally points to a caller-allocated
        buffer or variable that specifies connect data to send to the listener,
        and receives connect data sent by the listener.

    ConnectionInformationLength - Optionally points to a variable that
        specifies the size, in bytes, of the connect data to send
        to the listener, and receives the size of the connect data
        sent by the listener.

--*/

NATIVE_API
NTSTATUS
NTAPI
NtConnectPort(
    OUT     PHANDLE                         PortHandle,
    IN      PUNICODE_STRING                 PortName,
    IN      PSECURITY_QUALITY_OF_SERVICE    SecurityQos,
    IN OUT  PPORT_VIEW                      ClientView OPTIONAL,
    OUT     PREMOTE_PORT_VIEW               ServerView OPTIONAL,
    OUT     PULONG                          MaxMessageLength OPTIONAL,
    IN OUT  PVOID                           ConnectionInformation OPTIONAL,
    IN OUT  PULONG                          ConnectionInformationLength OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtSecureConnectPort(
    OUT     PHANDLE                         PortHandle,
    IN      PUNICODE_STRING                 PortName,
    IN      PSECURITY_QUALITY_OF_SERVICE    SecurityQos,
    IN OUT  PPORT_VIEW                      ClientView OPTIONAL,
    IN      PSID                            RequiredServerSid OPTIONAL,
    IN OUT  PREMOTE_PORT_VIEW               ServerView OPTIONAL,
    OUT     PULONG                          MaxMessageLength OPTIONAL,
    IN OUT  PVOID                           ConnectionInformation OPTIONAL,
    IN OUT  PULONG                          ConnectionInformationLength OPTIONAL
);

/*++

    NtListenPort
    ============

    Listens on a port for a connection request message on the server side.

    PortHandle - A handle to a port object. The handle doesn't need
        to grant any specific access.

    ConnectionRequest - Points to a caller-allocated buffer
        or variable that receives the connect message sent to
        the port.

--*/

NATIVE_API
NTSTATUS
NTAPI
NtListenPort(
    IN  HANDLE          PortHandle,
    OUT PPORT_MESSAGE   ConnectionRequest
);

/*++

    NtAcceptConnectPort
    ===================

    Accepts or rejects a connection request on the server side.

    PortHandle - Points to a variable that will receive the port object
        handle if the call is successful.

    PortContext - A numeric identifier to be associated with the port.

    ConnectionRequest - Points to a caller-allocated buffer or variable
        that identifies the connection request and contains any connect
        data that should be returned to requestor of the connection

    AcceptConnection - Specifies whether the connection should
        be accepted or not

    ServerView - Optionally points to a structure describing
        the shared memory region used to send large amounts of data to the
        requestor; if the call is successful, this will be updated

    ClientView - Optionally points to a caller-allocated buffer
        or variable that receives information on the shared memory
        region used by the requestor to send large amounts of data to the
        caller

--*/

NATIVE_API
NTSTATUS
NTAPI
NtAcceptConnectPort(
    OUT     PHANDLE             PortHandle,
    IN      PVOID               PortContext OPTIONAL,
    IN      PPORT_MESSAGE       ConnectionRequest,
    IN      BOOLEAN             AcceptConnection,
    IN OUT  PPORT_VIEW          ServerView OPTIONAL,
    OUT     PREMOTE_PORT_VIEW   ClientView OPTIONAL
);

/*++

    NtCompleteConnectPort
    =====================

    Completes the port connection process on the server side.

    PortHandle - A handle to a port object. The handle doesn't need
        to grant any specific access.

--*/

NATIVE_API
NTSTATUS
NTAPI
NtCompleteConnectPort(
    IN HANDLE PortHandle
);

/*++

    NtRequestPort
    =============

    Sends a request message to a port (client side)

    PortHandle - A handle to a port object. The handle doesn't need
        to grant any specific access.

    RequestMessage - Points to a caller-allocated buffer or variable
        that specifies the request message to send to the port.

--*/

NATIVE_API
NTSTATUS
NTAPI
NtRequestPort(
    IN HANDLE           PortHandle,
    IN PPORT_MESSAGE    RequestMessage
);

/*++

    NtRequestWaitReplyPort
    ======================

    Sends a request message to a port and waits for a reply (client side)

    PortHandle - A handle to a port object. The handle doesn't need
        to grant any specific access.

    RequestMessage - Points to a caller-allocated buffer or variable
        that specifies the request message to send to the port.

    ReplyMessage - Points to a caller-allocated buffer or variable
        that receives the reply message sent to the port.

--*/

NATIVE_API
NTSTATUS
NTAPI
NtRequestWaitReplyPort(
    IN  HANDLE          PortHandle,
    IN  PPORT_MESSAGE   RequestMessage,
    OUT PPORT_MESSAGE   ReplyMessage
);

/*++

    NtReplyPort
    ===========

    Sends a reply message to a port (Server side)

    PortHandle - A handle to a port object. The handle doesn't need
        to grant any specific access.

    ReplyMessage - Points to a caller-allocated buffer or variable
        that specifies the reply message to send to the port.

--*/

NATIVE_API
NTSTATUS
NTAPI
NtReplyPort(
    IN HANDLE           PortHandle,
    IN PPORT_MESSAGE    ReplyMessage
);

/*++

    NtReplyWaitReplyPort
    ====================

    Sends a reply message to a port and waits for a reply message

    PortHandle - A handle to a port object. The handle doesn't need
        to grant any specific access.

    ReplyMessage - Points to a caller-allocated buffer or variable
        that specifies the reply message to send to the port.

--*/

NATIVE_API
NTSTATUS
NTAPI
NtReplyWaitReplyPort(
    IN      HANDLE          PortHandle,
    IN OUT  PPORT_MESSAGE   ReplyMessage
);

/*++

    NtReplyWaitReceivePort
    ======================

    Optionally sends a reply message to a port and waits for a
    message

    PortHandle - A handle to a port object. The handle doesn't need
        to grant any specific access.

    PortContext - Optionally points to a variable that receives
        a numeric identifier associated with the port.

    ReplyMessage - Optionally points to a caller-allocated buffer
        or variable that specifies the reply message to send to the port.

    ReceiveMessage - Points to a caller-allocated buffer or variable
        that receives the message sent to the port.

--*/

NATIVE_API
NTSTATUS
NTAPI
NtReplyWaitReceivePort(
    IN  HANDLE          PortHandle,
    OUT PVOID*          PortContext OPTIONAL,
    IN  PPORT_MESSAGE   ReplyMessage OPTIONAL,
    OUT PPORT_MESSAGE   ReceiveMessage
);

NATIVE_API
NTSTATUS
NTAPI
NtReplyWaitReceivePortEx(
    IN  HANDLE          PortHandle,
    OUT PVOID*          PortContext OPTIONAL,
    IN  PPORT_MESSAGE   ReplyMessage OPTIONAL,
    OUT PPORT_MESSAGE   ReceiveMessage,
    IN  PLARGE_INTEGER  Timeout OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtImpersonateClientOfPort(
    IN HANDLE           PortHandle,
    IN PPORT_MESSAGE    Message
);

NATIVE_API
NTSTATUS
NTAPI
NtReadRequestData(
    IN  HANDLE          PortHandle,
    IN  PPORT_MESSAGE   Message,
    IN  ULONG           DataEntryIndex,
    OUT PVOID           Buffer,
    IN  ULONG_PTR       BufferSize,
    OUT PULONG_PTR      NumberOfBytesRead OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtWriteRequestData(
    IN  HANDLE          PortHandle,
    IN  PPORT_MESSAGE   Message,
    IN  ULONG           DataEntryIndex,
    OUT PVOID           Buffer,
    IN  ULONG_PTR       BufferSize,
    OUT PULONG_PTR      NumberOfBytesWritten OPTIONAL
);


typedef enum _PORT_INFORMATION_CLASS
{
    PortBasicInformation,
    PortDumpInformation

} PORT_INFORMATION_CLASS;

typedef struct _IO_COMPLETION_BASIC_INFORMATION
{
    LONG Depth;

} IO_COMPLETION_BASIC_INFORMATION, *PIO_COMPLETION_BASIC_INFORMATION;

NATIVE_API
NTSTATUS
NTAPI
NtQueryInformationPort(
    IN  HANDLE                  PortHandle,
    IN  PORT_INFORMATION_CLASS  PortInformationClass,
    IN  PVOID                   PortInformation,
    IN  ULONG                   Length,
    OUT PULONG                  ReturnLength OPTIONAL
);



// Asynchronous Local Inter-process Communication

// ALPC handles aren't NT object manager handles, and
// it seems traditional to use a typedef in these cases.
// rev
typedef PVOID ALPC_HANDLE, *PALPC_HANDLE;

#define ALPC_PORFLG_ALLOW_LPC_REQUESTS 0x20000 // rev
#define ALPC_PORFLG_WAITABLE_PORT 0x40000 // dbg
#define ALPC_PORFLG_SYSTEM_PROCESS 0x100000 // dbg

// symbols
typedef struct _ALPC_PORT_ATTRIBUTES
{
    ULONG                       Flags;
    SECURITY_QUALITY_OF_SERVICE SecurityQos;
    ULONG_PTR                   MaxMessageLength;
    ULONG_PTR                   MemoryBandwidth;
    ULONG_PTR                   MaxPoolUsage;
    ULONG_PTR                   MaxSectionSize;
    ULONG_PTR                   MaxViewSize;
    ULONG_PTR                   MaxTotalSectionSize;
    ULONG                       DupObjectTypes;
#ifdef _M_X64
    ULONG                       Reserved;
#endif
} ALPC_PORT_ATTRIBUTES, *PALPC_PORT_ATTRIBUTES;

// begin_rev
#define ALPC_MESSAGE_SECURITY_ATTRIBUTE 0x80000000
#define ALPC_MESSAGE_VIEW_ATTRIBUTE 0x40000000
#define ALPC_MESSAGE_CONTEXT_ATTRIBUTE 0x20000000
#define ALPC_MESSAGE_HANDLE_ATTRIBUTE 0x10000000
// end_rev

// symbols
typedef struct _ALPC_MESSAGE_ATTRIBUTES
{
    ULONG AllocatedAttributes;
    ULONG ValidAttributes;
} ALPC_MESSAGE_ATTRIBUTES, *PALPC_MESSAGE_ATTRIBUTES;

// symbols
typedef struct _ALPC_COMPLETION_LIST_STATE
{
    union
    {
        struct
        {
            ULONG64 Head : 24;
            ULONG64 Tail : 24;
            ULONG64 ActiveThreadCount : 16;
        } s1;
        ULONG64 Value;
    } u1;
} ALPC_COMPLETION_LIST_STATE, *PALPC_COMPLETION_LIST_STATE;

#define ALPC_COMPLETION_LIST_BUFFER_GRANULARITY_MASK 0x3f // dbg

#pragma warning(push)
#pragma warning(disable:4324)

#if ML_KERNEL_MODE

typedef struct _RTL_SRWLOCK {
    PVOID Ptr;
} RTL_SRWLOCK, *PRTL_SRWLOCK;

#endif // r0

// symbols
typedef struct DECL_ALIGN(128) _ALPC_COMPLETION_LIST_HEADER
{
    ULONG64 StartMagic;

    ULONG TotalSize;
    ULONG ListOffset;
    ULONG ListSize;
    ULONG BitmapOffset;
    ULONG BitmapSize;
    ULONG DataOffset;
    ULONG DataSize;
    ULONG AttributeFlags;
    ULONG AttributeSize;

    DECL_ALIGN(128) ALPC_COMPLETION_LIST_STATE State;
    ULONG LastMessageId;
    ULONG LastCallbackId;
    DECL_ALIGN(128) ULONG PostCount;
    DECL_ALIGN(128) ULONG ReturnCount;
    DECL_ALIGN(128) ULONG LogSequenceNumber;
    DECL_ALIGN(128) RTL_SRWLOCK UserLock;

    ULONG64 EndMagic;
} ALPC_COMPLETION_LIST_HEADER, *PALPC_COMPLETION_LIST_HEADER;

// private
typedef struct _ALPC_CONTEXT_ATTR
{
    PVOID PortContext;
    PVOID MessageContext;
    ULONG Sequence;
    ULONG MessageId;
    ULONG CallbackId;
} ALPC_CONTEXT_ATTR, *PALPC_CONTEXT_ATTR;

// begin_rev
#define ALPC_HANDLEFLG_DUPLICATE_SAME_ACCESS 0x10000
#define ALPC_HANDLEFLG_DUPLICATE_SAME_ATTRIBUTES 0x20000
#define ALPC_HANDLEFLG_DUPLICATE_INHERIT 0x80000
// end_rev

// private
typedef struct _ALPC_HANDLE_ATTR
{
    ULONG Flags;
    HANDLE Handle;
    ULONG ObjectType; // ObjectTypeCode, not ObjectTypeIndex
    ACCESS_MASK DesiredAccess;
} ALPC_HANDLE_ATTR, *PALPC_HANDLE_ATTR;

#define ALPC_SECFLG_CREATE_HANDLE 0x20000 // dbg

// name:private
// rev
typedef struct _ALPC_SECURITY_ATTR
{
    ULONG Flags;
    PSECURITY_QUALITY_OF_SERVICE SecurityQos;
    ALPC_HANDLE ContextHandle; // dbg
    ULONG Reserved1;
    ULONG Reserved2;
} ALPC_SECURITY_ATTR, *PALPC_SECURITY_ATTR;

// begin_rev
#define ALPC_VIEWFLG_NOT_SECURE 0x40000
// end_rev

// private
typedef struct _ALPC_DATA_VIEW_ATTR
{
    ULONG Flags;
    ALPC_HANDLE SectionHandle;
    PVOID ViewBase; // must be zero on input
    SIZE_T ViewSize;
} ALPC_DATA_VIEW_ATTR, *PALPC_DATA_VIEW_ATTR;


#pragma warning(pop)

// private

typedef enum _ALPC_PORT_INFORMATION_CLASS
{
    AlpcBasicInformation                                    = 0, // q: out ALPC_BASIC_INFORMATION
    AlpcPortInformation                                     = 1, // s: in ALPC_PORT_ATTRIBUTES
    AlpcAssociateCompletionPortInformation                  = 2, // s: in ALPC_PORT_ASSOCIATE_COMPLETION_PORT
    AlpcConnectedSIDInformation                             = 3, // q: in SID
    AlpcServerInformation                                   = 4, // q: inout ALPC_SERVER_INFORMATION
    AlpcMessageZoneInformation                              = 5, // s: in ALPC_PORT_MESSAGE_ZONE_INFORMATION
    AlpcRegisterCompletionListInformation                   = 6, // s: in ALPC_PORT_COMPLETION_LIST_INFORMATION
    AlpcUnregisterCompletionListInformation                 = 7, // s: VOID
    AlpcAdjustCompletionListConcurrencyCountInformation     = 8, // s: in ULONG
    AlpcRegisterCallback                                    = 9, // kernel-mode only // rev
    AlpcCompletionListRundownInformation                    = 10,// s: VOID // rev
    MaxAlpcPortInfoClass,

} ALPC_PORT_INFORMATION_CLASS;


// private
typedef struct _ALPC_BASIC_INFORMATION
{
    ULONG Flags;
    ULONG SequenceNo;
    PVOID PortContext;
} ALPC_BASIC_INFORMATION, *PALPC_BASIC_INFORMATION;

// private
typedef struct _ALPC_PORT_ASSOCIATE_COMPLETION_PORT
{
    PVOID CompletionKey;
    HANDLE CompletionPort;
} ALPC_PORT_ASSOCIATE_COMPLETION_PORT, *PALPC_PORT_ASSOCIATE_COMPLETION_PORT;

// private
typedef struct _ALPC_SERVER_INFORMATION
{
    union
    {
        struct
        {
            HANDLE ThreadHandle;
        } In;
        struct
        {
            BOOLEAN ThreadBlocked;
            HANDLE ConnectedProcessId;
            UNICODE_STRING ConnectionPortName;
        } Out;
    };
} ALPC_SERVER_INFORMATION, *PALPC_SERVER_INFORMATION;

// private
typedef struct _ALPC_PORT_MESSAGE_ZONE_INFORMATION
{
    PVOID Buffer;
    ULONG Size;
} ALPC_PORT_MESSAGE_ZONE_INFORMATION, *PALPC_PORT_MESSAGE_ZONE_INFORMATION;

// private
typedef struct _ALPC_PORT_COMPLETION_LIST_INFORMATION
{
    PVOID Buffer; // PALPC_COMPLETION_LIST_HEADER
    ULONG Size;
    ULONG ConcurrencyCount;
    ULONG AttributeFlags;
} ALPC_PORT_COMPLETION_LIST_INFORMATION, *PALPC_PORT_COMPLETION_LIST_INFORMATION;

// private
typedef enum _ALPC_MESSAGE_INFORMATION_CLASS
{
    AlpcMessageSidInformation, // q: out SID
    AlpcMessageTokenModifiedIdInformation,  // q: out LUID
    MaxAlpcMessageInfoClass
} ALPC_MESSAGE_INFORMATION_CLASS, *PALPC_MESSAGE_INFORMATION_CLASS;

// begin_private

// System calls

NATIVE_API
NTSTATUS
NTAPI
NtAlpcCreatePort(
    __out PHANDLE PortHandle,
    __in POBJECT_ATTRIBUTES ObjectAttributes,
    __in_opt PALPC_PORT_ATTRIBUTES PortAttributes
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcDisconnectPort(
    __in HANDLE PortHandle,
    __in ULONG Flags
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcQueryInformation(
    __in HANDLE PortHandle,
    __in ALPC_PORT_INFORMATION_CLASS PortInformationClass,
    __out_bcount(Length) PVOID PortInformation,
    __in ULONG Length,
    __out_opt PULONG ReturnLength
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcSetInformation(
    __in HANDLE PortHandle,
    __in ALPC_PORT_INFORMATION_CLASS PortInformationClass,
    __in_bcount(Length) PVOID PortInformation,
    __in ULONG Length
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcCreatePortSection(
    __in HANDLE PortHandle,
    __in ULONG Flags,
    __in_opt HANDLE SectionHandle,
    __in SIZE_T SectionSize,
    __out PALPC_HANDLE AlpcSectionHandle,
    __out PSIZE_T ActualSectionSize
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcDeletePortSection(
    __in HANDLE PortHandle,
    __reserved ULONG Flags,
    __in ALPC_HANDLE SectionHandle
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcCreateResourceReserve(
    __in HANDLE PortHandle,
    __reserved ULONG Flags,
    __in SIZE_T MessageSize,
    __out PALPC_HANDLE ResourceId
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcDeleteResourceReserve(
    __in HANDLE PortHandle,
    __reserved ULONG Flags,
    __in ALPC_HANDLE ResourceId
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcCreateSectionView(
    __in HANDLE PortHandle,
    __reserved ULONG Flags,
    __inout PALPC_DATA_VIEW_ATTR ViewAttributes
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcDeleteSectionView(
    __in HANDLE PortHandle,
    __reserved ULONG Flags,
    __in PVOID ViewBase
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcCreateSecurityContext(
    __in HANDLE PortHandle,
    __reserved ULONG Flags,
    __inout PALPC_SECURITY_ATTR SecurityAttribute
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcDeleteSecurityContext(
    __in HANDLE PortHandle,
    __reserved ULONG Flags,
    __in ALPC_HANDLE ContextHandle
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcRevokeSecurityContext(
    __in HANDLE PortHandle,
    __reserved ULONG Flags,
    __in ALPC_HANDLE ContextHandle
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcQueryInformationMessage(
    __in HANDLE PortHandle,
    __in PPORT_MESSAGE PortMessage,
    __in ALPC_MESSAGE_INFORMATION_CLASS MessageInformationClass,
    __out_bcount(Length) PVOID MessageInformation,
    __in ULONG Length,
    __out_opt PULONG ReturnLength
    );

#define ALPC_MSGFLG_REPLY_MESSAGE 0x1
#define ALPC_MSGFLG_LPC_MODE 0x2 // ?
#define ALPC_MSGFLG_RELEASE_MESSAGE 0x10000 // dbg
#define ALPC_MSGFLG_SYNC_REQUEST 0x20000 // dbg
#define ALPC_MSGFLG_WAIT_USER_MODE 0x100000
#define ALPC_MSGFLG_WAIT_ALERTABLE 0x200000
#define ALPC_MSGFLG_WOW64_CALL 0x80000000 // dbg

NATIVE_API
NTSTATUS
NTAPI
NtAlpcConnectPort(
    __out PHANDLE PortHandle,
    __in PUNICODE_STRING PortName,
    __in POBJECT_ATTRIBUTES ObjectAttributes,
    __in_opt PALPC_PORT_ATTRIBUTES PortAttributes,
    __in ULONG Flags,
    __in_opt PSID RequiredServerSid,
    __inout PPORT_MESSAGE ConnectionMessage,
    __inout_opt PULONG BufferLength,
    __inout_opt PALPC_MESSAGE_ATTRIBUTES OutMessageAttributes,
    __inout_opt PALPC_MESSAGE_ATTRIBUTES InMessageAttributes,
    __in_opt PLARGE_INTEGER Timeout
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcAcceptConnectPort(
    __out PHANDLE PortHandle,
    __in HANDLE ConnectionPortHandle,
    __in ULONG Flags,
    __in POBJECT_ATTRIBUTES ObjectAttributes,
    __in PALPC_PORT_ATTRIBUTES PortAttributes,
    __in_opt PVOID PortContext,
    __in PPORT_MESSAGE ConnectionRequest,
    __inout_opt PALPC_MESSAGE_ATTRIBUTES ConnectionMessageAttributes,
    __in BOOLEAN AcceptConnection
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcSendWaitReceivePort(
    __in HANDLE PortHandle,
    __in ULONG Flags,
    __in_opt PPORT_MESSAGE SendMessage,
    __in_opt PALPC_MESSAGE_ATTRIBUTES SendMessageAttributes,
    __inout_opt PPORT_MESSAGE ReceiveMessage,
    __inout_opt PULONG BufferLength,
    __inout_opt PALPC_MESSAGE_ATTRIBUTES ReceiveMessageAttributes,
    __in_opt PLARGE_INTEGER Timeout
    );

#define ALPC_CANCELFLG_TRY_CANCEL 0x1 // dbg
#define ALPC_CANCELFLG_NO_CONTEXT_CHECK 0x8
#define ALPC_CANCELFLGP_FLUSH 0x10000 // dbg

NATIVE_API
NTSTATUS
NTAPI
NtAlpcCancelMessage(
    __in HANDLE PortHandle,
    __in ULONG Flags,
    __in PALPC_CONTEXT_ATTR MessageContext
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcImpersonateClientOfPort(
    __in HANDLE PortHandle,
    __in PPORT_MESSAGE PortMessage,
    __reserved PVOID Reserved
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcOpenSenderProcess(
    __out PHANDLE ProcessHandle,
    __in HANDLE PortHandle,
    __in PPORT_MESSAGE PortMessage,
    __reserved ULONG Flags,
    __in ACCESS_MASK DesiredAccess,
    __in POBJECT_ATTRIBUTES ObjectAttributes
    );

NATIVE_API
NTSTATUS
NTAPI
NtAlpcOpenSenderThread(
    __out PHANDLE ThreadHandle,
    __in HANDLE PortHandle,
    __in PPORT_MESSAGE PortMessage,
    __reserved ULONG Flags,
    __in ACCESS_MASK DesiredAccess,
    __in POBJECT_ATTRIBUTES ObjectAttributes
    );

// Support functions

NATIVE_API
ULONG
NTAPI
AlpcMaxAllowedMessageLength(
    VOID
    );

NATIVE_API
ULONG
NTAPI
AlpcGetHeaderSize(
    __in ULONG Flags
    );

NATIVE_API
NTSTATUS
NTAPI
AlpcInitializeMessageAttribute(
    __in ULONG AttributeFlags,
    __out_opt PALPC_MESSAGE_ATTRIBUTES Buffer,
    __in ULONG BufferSize,
    __out PULONG RequiredBufferSize
    );

NATIVE_API
PVOID
NTAPI
AlpcGetMessageAttribute(
    __in PALPC_MESSAGE_ATTRIBUTES Buffer,
    __in ULONG AttributeFlag
    );

NATIVE_API
NTSTATUS
NTAPI
AlpcRegisterCompletionList(
    __in HANDLE PortHandle,
    __out PALPC_COMPLETION_LIST_HEADER Buffer,
    __in ULONG Size,
    __in ULONG ConcurrencyCount,
    __in ULONG AttributeFlags
    );

NATIVE_API
NTSTATUS
NTAPI
AlpcUnregisterCompletionList(
    __in HANDLE PortHandle
    );


NATIVE_API
NTSTATUS
NTAPI
AlpcRundownCompletionList(
    __in HANDLE PortHandle
);


NATIVE_API
NTSTATUS
NTAPI
AlpcAdjustCompletionListConcurrencyCount(
    __in HANDLE PortHandle,
    __in ULONG ConcurrencyCount
    );

NATIVE_API
BOOLEAN
NTAPI
AlpcRegisterCompletionListWorkerThread(
    __inout PVOID CompletionList
    );

NATIVE_API
BOOLEAN
NTAPI
AlpcUnregisterCompletionListWorkerThread(
    __inout PVOID CompletionList
    );

NATIVE_API
VOID
NTAPI
AlpcGetCompletionListLastMessageInformation(
    __in PVOID CompletionList,
    __out PULONG LastMessageId,
    __out PULONG LastCallbackId
    );

NATIVE_API
ULONG
NTAPI
AlpcGetOutstandingCompletionListMessageCount(
    __in PVOID CompletionList
    );

NATIVE_API
PPORT_MESSAGE
NTAPI
AlpcGetMessageFromCompletionList(
    __in PVOID CompletionList,
    __out_opt PALPC_MESSAGE_ATTRIBUTES *MessageAttributes
    );

NATIVE_API
VOID
NTAPI
AlpcFreeCompletionListMessage(
    __inout PVOID CompletionList,
    __in PPORT_MESSAGE Message
    );

NATIVE_API
PALPC_MESSAGE_ATTRIBUTES
NTAPI
AlpcGetCompletionListMessageAttributes(
    __in PVOID CompletionList,
    __in PPORT_MESSAGE Message
    );

// end_private


#endif // _NTLPC_H_add4da86_44d6_42e5_801f_23fe1de78ffe_