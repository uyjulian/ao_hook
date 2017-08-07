#ifndef _MLNTLPC_H_a6aabfd9_718b_4fea_938c_b62ea34136b8_
#define _MLNTLPC_H_a6aabfd9_718b_4fea_938c_b62ea34136b8_


ML_NAMESPACE_BEGIN(Lpc);

typedef NTSTATUS IPC_STATUS;

enum
{
    IPCType_None,
    IPCType_ServerListen,
    IPCType_ServerConnected,
    IPCType_Client,
};

class InterProcessBase
{
protected:
    ULONG_PTR IpcType;

public:
    InterProcessBase()
    {
        IpcType = IPCType_None;
    }

    ULONG_PTR GetIpcType()
    {
        return IpcType;
    }

    IPC_STATUS ReleaseAll()
    {
        return STATUS_NOT_IMPLEMENTED;
    }
};

class InterProcessServerBase : public InterProcessBase
{
public:
    InterProcessServerBase()
    {
    }
};

class InterProcessClientBase : public InterProcessBase
{
public:
    InterProcessClientBase()
    {
        IpcType = IPCType_Client;
    }
};

enum IPC_MESSAGE_TYPE
{
    IpcMessageType_ConnectionRequest,
    IpcMessageType_SmallData,
};

typedef struct IPC_MESSAGE : public PORT_MESSAGE
{
    PVOID GetMessageData()
    {
        return PtrAdd(this, sizeof(PORT_MESSAGE));
    }

    ULONG   MessageType;
    ULONG64 MessageDataLength;

    static const ULONG_PTR kOverhead = sizeof(ULONG) + sizeof(ULONG64);
    static const ULONG_PTR kMaxSmallDataLength = PORT_CREATE_MAXIMUM_MESSAGE_LENGTH - sizeof(PORT_MESSAGE) - kOverhead;

    union
    {
        struct
        {
            ULONG64 MaxMessageLength;

        } ConnectionRequest;

        struct
        {
            BYTE Buffer[kMaxSmallDataLength];

        } SmallData;

    } MessageData;

} IPC_MESSAGE, *PIPC_MESSAGE;


/************************************************************************
  InterProcessLpcServer
************************************************************************/

class InterProcessLpcServer : public InterProcessServerBase
{
protected:
    union
    {
        LPC_HANDLE          PortHandle;
        LPC_HANDLE          LpcPort;
        LPC_HANDLE          ConnectedPort;
    };

    union
    {
        struct
        {
            ULONG_PTR MaxConnectionInfoLength;
            ULONG_PTR MaxMessageLength;
            ULONG_PTR MaxSmallDataLength;

        } Listening;

        struct
        {
            PORT_VIEW           ServerView;
            REMOTE_PORT_VIEW    ClientView;
            PVOID               Context;

        } Connected;
    };

public:
    InterProcessLpcServer();
    ~InterProcessLpcServer();

    VOID Reset();
    IPC_STATUS ReleaseAll();

    operator LPC_HANDLE()
    {
        return PortHandle;
    }

    /************************************************************************
      listen
    ************************************************************************/

    IPC_STATUS
    Create(
        IN PWSTR Name,
        IN ULONG MaxConnectionInfoLength    = sizeof(IPC_MESSAGE),
        IN ULONG MaxMessageLength           = PORT_CREATE_MAXIMUM_MESSAGE_LENGTH
    );

    IPC_STATUS
    Listen(
        IN PIPC_MESSAGE ConnectionRequest,
        IN ULONG        Timeout = INFINITE
    );

    IPC_STATUS
    Listen(
        IN PIPC_MESSAGE     ConnectionRequest,
        IN PLARGE_INTEGER   Timeout = NULL
    );

    IPC_STATUS
    Accept(
        OUT InterProcessLpcServer&  ConnectedPort,
        IN  PIPC_MESSAGE            ConnectionRequest,
        IN  BOOL                    AcceptConnection    = TRUE,
        IN  PVOID                   Context             = NULL OPTIONAL
    );

    /************************************************************************
      connected
    ************************************************************************/

    PVOID GetDataBuffer(PIPC_MESSAGE Message)
    {
        return Message->MessageDataLength > Message->kMaxSmallDataLength ?
                    this->Connected.ClientView.ViewBase :
                    Message->MessageData.SmallData.Buffer;
    }

    IPC_STATUS
    Receive(
        OUT PVOID           Buffer,
        IN  ULONG_PTR       Size,
        OUT PULONG_PTR      BytesTransfered = NULL,
        OUT PVOID*          Context = NULL OPTIONAL,
        IN  ULONG           Timeout = INFINITE OPTIONAL
    );

    IPC_STATUS
    Receive(
        OUT PVOID           Buffer,
        IN  ULONG_PTR       Size,
        OUT PULONG_PTR      BytesTransfered = NULL,
        OUT PVOID*          Context = NULL OPTIONAL,
        IN  PLARGE_INTEGER  Timeout = NULL OPTIONAL
    );
};


/************************************************************************
  InterProcessLpcClient
************************************************************************/

class InterProcessLpcClient : public InterProcessClientBase
{
protected:
    LPC_HANDLE          ConnetionPort;
    PORT_VIEW           ClientView;
    REMOTE_PORT_VIEW    ServerView;
    PVOID               Context;
    ULONG_PTR           MaxSmallDataLength;

public:
    InterProcessLpcClient();
    ~InterProcessLpcClient();

    VOID Reset();
    IPC_STATUS ReleaseAll();

    LPC_STATUS
    Connect(
        IN PWSTR Name,
        IN ULONG MaxMessageLength = 0
    );

    LPC_STATUS
    Send(
        PVOID       Buffer,
        ULONG_PTR   Size,
        PULONG_PTR  BytesTransfered = NULL
    );
};

ML_NAMESPACE_END_(Lpc);

#endif // _MLNTLPC_H_a6aabfd9_718b_4fea_938c_b62ea34136b8_