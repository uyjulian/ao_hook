#include "mylibrary.h"

ML_NAMESPACE

ML_NAMESPACE_BEGIN(Native)


ML_NAMESPACE_BEGIN(Lpc);

#if ML_USER_MODE

/************************************************************************
  InterProcessLpcServer
************************************************************************/

#define VERIFY_IPC_TYPE(_type) if (this->IpcType != (_type)) return STATUS_OBJECT_TYPE_MISMATCH
#define VERIFY_IPC_TYPE_(_obj, _type) if ((_obj)->IpcType != (_type)) return STATUS_OBJECT_TYPE_MISMATCH

InterProcessLpcServer::InterProcessLpcServer()
{
    Reset();
}

InterProcessLpcServer::~InterProcessLpcServer()
{
    ReleaseAll();
}

VOID InterProcessLpcServer::Reset()
{
    IpcType = IPCType_None;
    PortHandle = nullptr;

    ZeroMemory(&Connected.ServerView, sizeof(Connected.ServerView));
    ZeroMemory(&Connected.ClientView, sizeof(Connected.ClientView));
}

IPC_STATUS InterProcessLpcServer::ReleaseAll()
{
    if (PortHandle != nullptr)
    {
        ZwClose(PortHandle);
    }

    if (IpcType == IPCType_ServerConnected && Connected.ServerView.SectionHandle != nullptr)
    {
        ZwClose(Connected.ServerView.SectionHandle);
    }

    Reset();

    return STATUS_SUCCESS;
}


/*++

  listening

--*/

IPC_STATUS
InterProcessLpcServer::
Create(
    PWSTR Name,
    ULONG MaxConnectionInfoLength,
    ULONG MaxMessageLength
)
{
    NTSTATUS            Status;
    OBJECT_ATTRIBUTES   ObjectAttributes;
    UNICODE_STRING      PortName;

    VERIFY_IPC_TYPE(IPCType_None);

    RtlInitUnicodeString(&PortName, Name);
    InitializeObjectAttributes(&ObjectAttributes, &PortName, 0, nullptr, nullptr);

    Status = NtCreatePort(&LpcPort, &ObjectAttributes, MaxConnectionInfoLength, MaxMessageLength, 0);
    FAIL_RETURN(Status);

    IpcType = IPCType_ServerListen;

    Listening.MaxConnectionInfoLength   = MaxConnectionInfoLength;
    Listening.MaxMessageLength          = MaxMessageLength;
    Listening.MaxSmallDataLength        = MaxMessageLength - sizeof(PORT_MESSAGE) - IPC_MESSAGE::kOverhead;

    return Status;
}

IPC_STATUS InterProcessLpcServer::Listen(PIPC_MESSAGE ConnectionRequest, ULONG Timeout)
{
    LARGE_INTEGER LocalTimeout;

    FormatTimeOut(&LocalTimeout, Timeout);
    return Listen(ConnectionRequest, Timeout == INFINITE ? nullptr : &LocalTimeout);
}

IPC_STATUS InterProcessLpcServer::Listen(PIPC_MESSAGE ConnectionRequest, PLARGE_INTEGER Timeout)
{
    NTSTATUS Status;

    VERIFY_IPC_TYPE(IPCType_ServerListen);

    ZeroMemory(ConnectionRequest, sizeof(*ConnectionRequest));

    LOOP_FOREVER
    {
        Status = NtReplyWaitReceivePortEx(LpcPort, nullptr, nullptr, ConnectionRequest, Timeout);
        if (Status != STATUS_SUCCESS ||
            (ConnectionRequest->Type & ~LPC_KERNELMODE_MESSAGE) == LPC_CONNECTION_REQUEST)
        {
            break;
        }
    }

    return Status;
}

IPC_STATUS
InterProcessLpcServer::
Accept(
    InterProcessLpcServer&  ConnectedPort,
    PIPC_MESSAGE            ConnectionRequest,
    BOOL                    AcceptConnection,
    PVOID                   Context
)
{
    NTSTATUS    Status;
    ULONG64     MaxMessageLength;

    VERIFY_IPC_TYPE(IPCType_ServerListen);
    VERIFY_IPC_TYPE_(&ConnectedPort, IPCType_None);

    if ((ConnectionRequest->Type & ~LPC_KERNELMODE_MESSAGE) != LPC_CONNECTION_REQUEST)
        return STATUS_UNSUCCESSFUL;

    MaxMessageLength = ConnectionRequest->MessageData.ConnectionRequest.MaxMessageLength;

    if (MaxMessageLength > Listening.MaxSmallDataLength)
    {
        OBJECT_ATTRIBUTES   ObjectAttributes;
        LARGE_INTEGER       MaximumSize;

        MaximumSize.QuadPart = MaxMessageLength;
        InitializeObjectAttributes(&ObjectAttributes, nullptr, 0, nullptr, nullptr);

        Status = ZwCreateSection(
                    &ConnectedPort.Connected.ServerView.SectionHandle,
                    SECTION_ALL_ACCESS,
                    &ObjectAttributes,
                    &MaximumSize,
                    PAGE_READWRITE,
                    SEC_COMMIT,
                    nullptr
                );

        if (NT_FAILED(Status))
            return Status;

        ConnectedPort.Connected.ServerView.Length = sizeof(Connected.ServerView);
        ConnectedPort.Connected.ServerView.SectionOffset = 0;
        ConnectedPort.Connected.ServerView.ViewSize = (LPC_SIZE_T)MaximumSize.QuadPart;

        ConnectedPort.Connected.ClientView.Length = sizeof(Connected.ClientView);

        Status = NtAcceptConnectPort(
                    &ConnectedPort.ConnectedPort,
                    &ConnectedPort,
                    ConnectionRequest,
                    (BOOLEAN)AcceptConnection,
                    &ConnectedPort.Connected.ServerView,
                    &ConnectedPort.Connected.ClientView
                );
    }
    else
    {
        Status = NtAcceptConnectPort(
                    &ConnectedPort.ConnectedPort,
                    &ConnectedPort,
                    ConnectionRequest,
                    (BOOLEAN)AcceptConnection,
                    nullptr,
                    nullptr
                );
    }

    if (NT_SUCCESS(Status))
        Status = NtCompleteConnectPort(ConnectedPort.ConnectedPort);

    if (NT_FAILED(Status))
    {
        ConnectedPort.ReleaseAll();

        return Status;
    }

    ConnectedPort.IpcType = IPCType_ServerConnected;
    ConnectedPort.Connected.Context = Context;

    return Status;
}


/*++

  listening end

--*/


/*++

  connected

--*/

IPC_STATUS
InterProcessLpcServer::
Receive(
    OUT PVOID           Buffer,
    IN  ULONG_PTR       Size,
    OUT PULONG_PTR      BytesTransfered,
    OUT PVOID*          Context,
    IN  ULONG           Timeout
)
{
    LARGE_INTEGER LocalTimeout;

    FormatTimeOut(&LocalTimeout, Timeout);
    return Receive(Buffer, Size, BytesTransfered, Context, &LocalTimeout);
}

IPC_STATUS
InterProcessLpcServer::
Receive(
    OUT PVOID           Buffer,
    IN  ULONG_PTR       Size,
    OUT PULONG_PTR      BytesTransfered,
    OUT PVOID*          Context,
    IN  PLARGE_INTEGER  Timeout
)
{
    NTSTATUS                Status;
    PVOID                   DataBuffer;
    InterProcessLpcServer*  AssociatedPort;
    IPC_MESSAGE             ReceiveMessage;

    Status = NtReplyWaitReceivePortEx(ConnectedPort, (PVOID *)&AssociatedPort, nullptr, &ReceiveMessage, Timeout);
    FAIL_RETURN(Status);

    if (AssociatedPort != this)
    {
        // what the fuck
    }

    DataBuffer = AssociatedPort->GetDataBuffer(&ReceiveMessage);

    Size = (ULONG_PTR)ML_MIN(Size, ReceiveMessage.MessageDataLength);

    if (Buffer != nullptr)
        CopyMemory(Buffer, DataBuffer, Size);

    if (BytesTransfered != nullptr)
        *BytesTransfered = Size;

    if (Context != nullptr)
        *Context = AssociatedPort->Connected.Context;

    return Status;
}


/*++

  connected end

--*/


/************************************************************************
  InterProcessLpcClient
************************************************************************/

InterProcessLpcClient::InterProcessLpcClient()
{
    Reset();
}

InterProcessLpcClient::~InterProcessLpcClient()
{
    ReleaseAll();
}

VOID InterProcessLpcClient::Reset()
{
    IpcType = IPCType_None;
    ConnetionPort = nullptr;

    ZeroMemory(&ServerView, sizeof(ServerView));
    ZeroMemory(&ClientView, sizeof(ClientView));
}

IPC_STATUS InterProcessLpcClient::ReleaseAll()
{
    if (ConnetionPort != nullptr)
    {
        ZwClose(ConnetionPort);
    }

    Reset();

    return STATUS_SUCCESS;
}

LPC_STATUS
InterProcessLpcClient::
Connect(
    PWSTR Name,
    ULONG MaxMessageLength
)
{
    ULONG           ConnectionInformationLength;
    NTSTATUS        Status;
    UNICODE_STRING  PortName;
    IPC_MESSAGE     ConnectionMessage;

    VERIFY_IPC_TYPE(IPCType_None);

    RtlInitUnicodeString(&PortName, Name);

    ConnectionMessage.MessageType = IpcMessageType_ConnectionRequest;
    ConnectionMessage.MessageDataLength = sizeof(ConnectionMessage.MessageData.ConnectionRequest);
    ConnectionMessage.MessageData.ConnectionRequest.MaxMessageLength = MaxMessageLength;

    ConnectionInformationLength = (ULONG)ConnectionMessage.MessageDataLength;

    Status = NtConnectPort(
                &ConnetionPort,
                &PortName,
                nullptr,
                &ClientView,
                &ServerView,
                &MaxMessageLength,
                PtrAdd(&ConnectionMessage, sizeof(PORT_MESSAGE)),
                &ConnectionInformationLength
            );

    FAIL_RETURN(Status);

    IpcType = IPCType_Client;

    MaxSmallDataLength = MaxMessageLength - sizeof(PORT_MESSAGE) - IPC_MESSAGE::kOverhead;

    return Status;
}

#endif // r3

ML_NAMESPACE_END_(Lpc);

ML_NAMESPACE_END_(Native)

ML_NAMESPACE_END
