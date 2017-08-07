#ifndef _MLNTFILEIO_H_03daa20f_84f6_4731_b996_cf4155164012_
#define _MLNTFILEIO_H_03daa20f_84f6_4731_b996_cf4155164012_


ML_NAMESPACE_BEGIN(Io);

#if !defined(FILE_ATTRIBUTE_NO_SCRUB_DATA)
    #define FILE_ATTRIBUTE_NO_SCRUB_DATA        0x00020000
#endif

#if ML_KERNEL_MODE

//
// Typedefs
//

typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;


#endif // r0

typedef struct _ML_FIND_DATA {
    ULONG           FileAttributes;
    LARGE_INTEGER   CreationTime;
    LARGE_INTEGER   LastAccessTime;
    LARGE_INTEGER   LastWriteTime;
    LARGE_INTEGER   FileSize;
    ULONG           Reserved0;
    ULONG           Reserved1;
    WCHAR           FileName[MAX_NTPATH];
    WCHAR           AlternateFileName[14];

} ML_FIND_DATA, *PML_FIND_DATA;

NTSTATUS
QueryFirstFile(
    PHANDLE         Handle,
    PCWSTR          FileName,
    PML_FIND_DATA   FindFileData
);

NTSTATUS
QueryNextFile(
    HANDLE          FindFileHandle,
    PML_FIND_DATA   FindFileData
);

inline
NTSTATUS
QueryClose(
    HANDLE FindFileHandle
)
{
#if ML_KERNEL_MODE
    return ZwClose(FindFileHandle);
#elif ML_USER_MODE
    return NtClose(FindFileHandle);
#endif
}

ULONG_PTR
QueryFileAttributes(
    PCWSTR FileName
);

NTSTATUS
QueryFileAttributesEx(
    PCWSTR      FileName,
    PULONG_PTR  FileAttributes
);

NTSTATUS
ApplyFileAttributes(
    PCWSTR      FileName,
    ULONG_PTR   Attributes
);

NTSTATUS
MoveFile(
    PCWSTR  ExistingFileName,
    PCWSTR  NewFileName,
    BOOL    DEF_VAL(ReplaceIfExists, FALSE)
);

NTSTATUS
CopyFile(
    PCWSTR  ExistingFileName,
    PCWSTR  NewFileName,
    BOOL    FailIfExists
);

NTSTATUS
DeleteFile(
    PCWSTR FileName
);

NTSTATUS
DeleteDirectory(
    PCWSTR DirectoryName
);

BOOL
IsPathExists(
    PCWSTR Path
);

NTSTATUS
QueryDosLetterFromVolumeDeviceName(
    OUT PUNICODE_STRING DosLetter,
    IN  PUNICODE_STRING NtDeviceName
);

NTSTATUS
QueryDosPathFromHandle(
    OUT PUNICODE_STRING DosPath,
    IN  HANDLE          FileHandle
);

NTSTATUS
QueryDosDevice(
    PCWSTR          DeviceName,
    PUNICODE_STRING TargetPath
);

NTSTATUS
QueryDosPathFromNtDeviceName(
    OUT PUNICODE_STRING DosPath,
    IN  PUNICODE_STRING NtDeviceName
);

NTSTATUS
CreateNamedPipe(
    OUT PHANDLE                 ReadPipe,
    OUT PHANDLE                 WritePipe,
    IN  PUNICODE_STRING         DEF_VAL(PipeName, nullptr) OPTIONAL,
    IN  PSECURITY_ATTRIBUTES    DEF_VAL(PipeAttributes, nullptr) OPTIONAL,
    IN  ULONG_PTR               DEF_VAL(BufferSize, 0) OPTIONAL
);

template<typename F, typename P>
NTSTATUS
SetAsyncCall(
    CONST F&    Callback,
    ULONG64     DueTime,
    LONG        Period = 0,
    CONST P&    Context = nullptr
)
{
    NTSTATUS        Status;
    HANDLE          Timer;
    LARGE_INTEGER   _DueTime;

    typedef struct PARAMETER
    {
        ml::Function<BOOL(P)> func;
        P Context;
        LONG Period;
        HANDLE Timer;

        PARAMETER(const F &Callback, const P &Context, const LONG &Period) : func(Callback), Context(Context)
        {
            this->Period = Period;
            this->Timer = nullptr;
        }

        ~PARAMETER()
        {
            if (this->Timer != nullptr)
            {
                NtCancelTimer(this->Timer, nullptr);
                NtClose(this->Timer);
            }
        }

    } PARAMETER;

    PARAMETER *func = new PARAMETER(Callback, Context, Period);
    if (func == nullptr)
        return STATUS_NO_MEMORY;

    Status = NtCreateTimer(&Timer, TIMER_ALL_ACCESS, nullptr, SynchronizationTimer);
    if (NT_FAILED(Status))
    {
        delete func;
        return Status;
    }

    func->Timer = Timer;
    FormatTimeOut(&_DueTime, DueTime);

    Status = NtSetTimer(Timer, &_DueTime,
                [](PVOID p, ULONG, LONG)
                {
                    PARAMETER *param = (PARAMETER *)p;
                    BOOL cancel = param->func(param->Context);
                    if (param->Period == 0 || cancel)
                    {
                        delete param;
                    }
                },
                func, FALSE, Period, nullptr
            );

    if (NT_FAILED(Status))
    {
        delete func;
    }

    return Status;
}

template<typename F>
NTSTATUS
SetAsyncCall(
    CONST F&    Callback,
    ULONG64     DueTime,
    LONG        Period = 0
)
{
    NTSTATUS        Status;
    HANDLE          Timer;
    LARGE_INTEGER   _DueTime;

    typedef struct PARAMETER
    {
        ml::Function<BOOL()> func;
        LONG Period;
        HANDLE Timer;

        PARAMETER(const F &Callback, const LONG &Period) : func(Callback)
        {
            this->Period = Period;
            this->Timer = nullptr;
        }

        ~PARAMETER()
        {
            if (this->Timer != nullptr)
            {
                NtCancelTimer(this->Timer, nullptr);
                NtClose(this->Timer);
            }
        }

    } PARAMETER;

    PARAMETER *func = new PARAMETER(Callback, Period);
    if (func == nullptr)
        return STATUS_NO_MEMORY;

    Status = NtCreateTimer(&Timer, TIMER_ALL_ACCESS, nullptr, SynchronizationTimer);
    if (NT_FAILED(Status))
    {
        delete func;
        return Status;
    }

    func->Timer = Timer;
    FormatTimeOut(&_DueTime, DueTime);

    Status = NtSetTimer(Timer, &_DueTime,
                [](PVOID p, ULONG, LONG)
                {
                    PARAMETER *param = (PARAMETER *)p;
                    BOOL cancel = param->func();
                    if (param->Period == 0 || cancel)
                    {
                        delete param;
                    }
                },
                func, FALSE, Period, nullptr
            );

    if (NT_FAILED(Status))
    {
        delete func;
    }

    return Status;
}

ML_NAMESPACE_END_(Io);

#endif // _MLNTFILEIO_H_03daa20f_84f6_4731_b996_cf4155164012_