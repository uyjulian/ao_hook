#ifndef _FILEIO_H_322e14b1_8a90_410b_bea8_1b39baddbd22
#define _FILEIO_H_322e14b1_8a90_410b_bea8_1b39baddbd22


//#include "MyLibrary.h"

#if ML_USER_MODE


_ML_C_HEAD_

inline Long fsize(FILE *fp)
{
    fseek(fp, 0, SEEK_CUR);
    return _filelength(_fileno(fp));
}

inline Int64 fsize64(FILE *fp)
{
    fseek(fp, 0, SEEK_CUR);
    return _filelengthi64(_fileno(fp));
}

inline Int64 my_fseek64(FILE *fp, Int64 offset, Int origin)
{
    Int64 ret;

    ret = fseek(fp, 0, SEEK_CUR);
    ret = _lseeki64(_fileno(fp), offset, origin);

    return ret == -1 ? 1 : 0;
}

inline Int64 my_ftell64(FILE *fp)
{
    int ret;
    fpos_t pos;

    ret = fseek(fp, 0, SEEK_CUR);
    ret = fgetpos(fp, &pos);

    return ret ? ret : pos;
}

_ML_C_TAIL_

#endif // ring3


#if CPP_DEFINED

#define NFD_NOT_RESOLVE_PATH    (0x00000001u)
#define NFD_EXPAND_ENVVAR       (0x00000002u)

/************************************************************************
  file disk
************************************************************************/
class NtFileDisk
{
protected:
    HANDLE          m_FileHandle;
    LARGE_INTEGER   m_FileSize, m_Position;

public:

    NtFileDisk()
    {
        m_FileHandle = nullptr;
    }

    ForceInline NtFileDisk(const NtFileDisk &file);
    ForceInline operator HANDLE() const;
    NtFileDisk& operator=(const NtFileDisk &file);
    NtFileDisk& operator=(HANDLE Handle);

    ~NtFileDisk()
    {
        Close();
    }

protected:
    static
    NTSTATUS
    CreateFileInternal(
        PHANDLE     FileHandle,
        PCWSTR      FileName,
        ULONG_PTR   Flags,
        ULONG_PTR   ShareMode,
        ULONG_PTR   Access,
        ULONG_PTR   CreationDisposition,
        ULONG_PTR   Attributes,
        ULONG_PTR   CreateOptions   = 0
    );

    static
    NTSTATUS
    CreateFileInternalWithFullPath(
        PHANDLE         FileHandle,
        PUNICODE_STRING FileName,
        ULONG_PTR           Flags,
        ULONG_PTR           ShareMode,
        ULONG_PTR           Access,
        ULONG_PTR           CreationDisposition,
        ULONG_PTR           Attributes,
        ULONG_PTR           CreateOptions   = 0
    );

    static
    NTSTATUS
    GetSizeInternal(
        HANDLE          FileHandle,
        PLARGE_INTEGER  FileSize
    );

    static
    NTSTATUS
    SeekInternal(
        HANDLE          FileHandle,
        LARGE_INTEGER   Offset,
        ULONG_PTR       MoveMethod,
        PLARGE_INTEGER  NewPosition = nullptr
    );

    static
    NTSTATUS
    ReadInternal(
        HANDLE          FileHandle,
        PVOID           Buffer,
        ULONG_PTR           Size,
        PLARGE_INTEGER  pBytesRead  = nullptr,
        PLARGE_INTEGER  Offset      = nullptr
    );

    static
    NTSTATUS
    WriteInternal(
        HANDLE          FileHandle,
        PVOID           Buffer,
        ULONG_PTR           Size,
        PLARGE_INTEGER  pBytesWritten   = nullptr,
        PLARGE_INTEGER  Offset          = nullptr
    );

    static
    NTSTATUS
    DeleteInternal(
        HANDLE FileHandle
    );

    static
    NTSTATUS
    SetEndOfFileInternal(
        HANDLE FileHandle,
        LARGE_INTEGER EndPosition
    );

    static
    NTSTATUS
    QuerySymbolicTargetInternal(
        HANDLE                  FileHandle,
        PREPARSE_DATA_BUFFER    ReparseBuffer,
        ULONG_PTR               BufferSize,
        PULONG_PTR              BytesReturned = nullptr
    );

    static
    NTSTATUS
    QuerySymbolicTargetInternal(
        HANDLE      FileHandle,
        PWSTR       PrintNameBuffer,
        PULONG_PTR  PrintNameBufferCount,
        PWSTR       SubstituteNameBuffer        = nullptr,
        PULONG_PTR  SubstituteNameBufferCount   = nullptr
    );

public:
    NTSTATUS Close();

    NTSTATUS
    Open(
        PCSTR       FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ,
        ULONG_PTR   Access              = GENERIC_READ,
        ULONG_PTR   CreationDisposition = FILE_OPEN,
        ULONG_PTR   Attributes          = FILE_ATTRIBUTE_NORMAL,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT
    )
    {
        NTSTATUS        Status;
        UNICODE_STRING  FileNameU;
        ANSI_STRING     FileNameA;

        RtlInitAnsiString(&FileNameA, FileName);
        RtlAnsiStringToUnicodeString(&FileNameU, &FileNameA, TRUE);

        Status = Open(FileNameU.Buffer, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);

        RtlFreeUnicodeString(&FileNameU);

        return Status;
    }

    ForceInline
    NTSTATUS
    Open(
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ,
        ULONG_PTR   Access              = GENERIC_READ,
        ULONG_PTR   CreationDisposition = FILE_OPEN,
        ULONG_PTR   Attributes          = FILE_ATTRIBUTE_NORMAL,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT
    )
    {
        return Create(FileName, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);
    }

    ForceInline
    NTSTATUS
    CreateIfNotExist(
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ,
        ULONG_PTR   Access              = GENERIC_READ | GENERIC_WRITE,
        ULONG_PTR   CreationDisposition = FILE_OPEN_IF,
        ULONG_PTR   Attributes          = FILE_ATTRIBUTE_NORMAL,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT
    )
    {
        return Create(FileName, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);
    }

    NTSTATUS
    Modify(
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ,
        ULONG_PTR   Access              = GENERIC_READ | GENERIC_WRITE,
        ULONG_PTR   CreationDisposition = FILE_OPEN_IF,
        ULONG_PTR   Attributes          = FILE_ATTRIBUTE_NORMAL,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT
    )
    {
        return Create(FileName, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);
    }

    NoInline
    NTSTATUS
    Append(
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ,
        ULONG_PTR   Access              = GENERIC_READ | GENERIC_WRITE,
        ULONG_PTR   CreationDisposition = FILE_OPEN_IF,
        ULONG_PTR   Attributes          = FILE_ATTRIBUTE_NORMAL,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT
    )
    {
        NTSTATUS Status;

        Status = Create(FileName, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);

        if (NT_SUCCESS(Status))
            Status = Seek(0ll, FILE_END);

        return Status;
    }

    NTSTATUS
    Create(
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ,
        ULONG_PTR   Access              = GENERIC_WRITE,
        ULONG_PTR   CreationDisposition = FILE_OVERWRITE_IF,
        ULONG_PTR   Attributes          = FILE_ATTRIBUTE_NORMAL,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT
    );

    ForceInline
    NTSTATUS
    OpenDirectory(
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ,
        ULONG_PTR   Access              = GENERIC_READ,
        ULONG_PTR   CreationDisposition = FILE_OPEN,
        ULONG_PTR   Attributes          = FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_DIRECTORY,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT | FILE_DIRECTORY_FILE
    )
    {
        return CreateDirectory(FileName, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);
    }

    ForceInline
    NTSTATUS
    CreateDirectory(
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ | FILE_SHARE_WRITE,
        ULONG_PTR   Access              = GENERIC_ALL,
        ULONG_PTR   CreationDisposition = FILE_CREATE,
        ULONG_PTR   Attributes          = FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_DIRECTORY,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT | FILE_DIRECTORY_FILE
    )
    {
        Close();
        return CreateFileInternal(
                    &m_FileHandle,
                    FileName,
                    Flags,
                    ShareMode,
                    Access,
                    CreationDisposition,
                    Attributes,
                    CreateOptions
               );
    }

    NTSTATUS
    OpenVolume(
        ULONG_PTR   Drive,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        ULONG_PTR   Access              = GENERIC_READ,
        ULONG_PTR   CreationDisposition = FILE_OPEN,
        ULONG_PTR   Attributes          = 0,
        ULONG_PTR   CreateOptions       = FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
    )
    {
        WCHAR Volume[0x10];

        Volume[0] = '\\';
        Volume[1] = '\\';
        Volume[2] = '.';
        Volume[3] = '\\';
        Volume[4] = (WCHAR)Drive;
        Volume[5] = ':';
        Volume[6] = 0;

        return OpenDirectory(Volume, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);
    }

    NTSTATUS
    OpenDevice(
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ,
        ULONG_PTR   Access              = FILE_READ_ATTRIBUTES | SYNCHRONIZE,
        ULONG_PTR   CreationDisposition = FILE_OPEN,
        ULONG_PTR   Attributes          = 0,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT
    );

    NTSTATUS
    DeviceIoControl(
        ULONG_PTR   IoControlCode,
        PVOID       InputBuffer,
        ULONG_PTR   InputBufferLength,
        PVOID       OutputBuffer,
        ULONG_PTR   OutputBufferLength,
        PULONG_PTR  BytesReturned = nullptr
    );

    ForceInline
    NTSTATUS
    OpenSymbolicLink(
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ,
        ULONG_PTR   Access              = GENERIC_READ,
        ULONG_PTR   CreationDisposition = FILE_OPEN,
        ULONG_PTR   Attributes          = 0,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT | FILE_OPEN_REPARSE_POINT | FILE_SYNCHRONOUS_IO_NONALERT
    )
    {
        return CreateSymbolicLink(FileName, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);
    }

    ForceInline
    NTSTATUS
    CreateSymbolicLink(
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ | FILE_SHARE_WRITE,
        ULONG_PTR   Access              = GENERIC_ALL,
        ULONG_PTR   CreationDisposition = FILE_CREATE,
        ULONG_PTR   Attributes          = 0,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT | FILE_OPEN_REPARSE_POINT | FILE_SYNCHRONOUS_IO_NONALERT
    )
    {
        Close();

        return CreateFileInternal(
                    &m_FileHandle,
                    FileName,
                    Flags,
                    ShareMode,
                    Access,
                    CreationDisposition,
                    Attributes,
                    CreateOptions
               );
    }

    ForceInline
    NTSTATUS
    QuerySymbolicTarget(
        PREPARSE_DATA_BUFFER    ReparseBuffer,
        ULONG_PTR               BufferSize,
        PULONG_PTR              BytesReturned = nullptr
    )
    {
        return QuerySymbolicTargetInternal(
                    m_FileHandle,
                    ReparseBuffer,
                    BufferSize,
                    BytesReturned
               );
    }

    ForceInline
    NTSTATUS
    QuerySymbolicTarget(
        PWSTR                   PrintNameBuffer,
        PULONG_PTR              PrintNameBufferCount,
        PWSTR                   SubstituteNameBuffer        = nullptr,
        PULONG_PTR              SubstituteNameBufferCount   = nullptr
    )
    {
        return QuerySymbolicTargetInternal(
                    m_FileHandle,
                    PrintNameBuffer,
                    PrintNameBufferCount,
                    SubstituteNameBuffer,
                    SubstituteNameBufferCount
               );
    }

    NTSTATUS
    GetSize(
        PLARGE_INTEGER FileSize
    );

    NTSTATUS
    GetPosition(
        PLARGE_INTEGER Position
    );

    ForceInline
    NTSTATUS
    Seek(
        LONG64          Offset,
        ULONG_PTR       MoveMethod = FILE_BEGIN,
        PLARGE_INTEGER  NewPosition = nullptr
    )
    {
        LARGE_INTEGER NewOffset;

        NewOffset.QuadPart = Offset;
        return Seek(NewOffset, MoveMethod, NewPosition);
    }

    NTSTATUS
    Seek(
        LARGE_INTEGER   Offset,
        ULONG_PTR       MoveMethod = FILE_BEGIN,
        PLARGE_INTEGER  NewPosition = nullptr
    );

    NTSTATUS
    Read(
        PVOID           Buffer,
        ULONG_PTR       Size = 0,
        PLARGE_INTEGER  BytesRead = nullptr
    );

    NTSTATUS
    Write(
        PVOID           Buffer,
        ULONG_PTR       Size,
        PLARGE_INTEGER  BytesWritten = nullptr
    );

    NTSTATUS
    Print(
        PLARGE_INTEGER  BytesWritten,
        PWSTR           Format,
        ...
    );

    NTSTATUS
    MapView(
        PVOID*  BaseAddress,
        HANDLE  ProcessHandle = NtCurrentProcess()
    );

    NTSTATUS
    UnMapView(
        PVOID   BaseAddress,
        HANDLE  ProcessHandle = NtCurrentProcess()
    );

    NTSTATUS SetEndOfFile();
    NTSTATUS SetEndOfFile(LARGE_INTEGER EndPosition);

    NTSTATUS SetEndOfFile(ULONG64 EndPosition)
    {
        LARGE_INTEGER LargeEndPosition;
        LargeEndPosition.QuadPart = EndPosition;
        return SetEndOfFile(LargeEndPosition);
    }

    NTSTATUS Delete()
    {
        return DeleteInternal(m_FileHandle);
    }

    ULONG_PTR GetSize32()
    {
        return (ULONG_PTR)GetSize64();
    }

    ULONG_PTR GetCurrentPos()
    {
        return (ULONG_PTR)GetCurrentPos64();
    }

    ULONG64 GetCurrentPos64()
    {
        return m_Position.QuadPart;
    }

    NTSTATUS Rewind()
    {
        return Seek(0ll, FILE_BEGIN);
    }

    HANDLE GetHandle() const
    {
        return m_FileHandle;
    }

    LONG64 GetSize64()
    {
        return m_FileSize.QuadPart;
    }

    /************************************************************************
      static ver
    ************************************************************************/

    ForceInline
    static
    NTSTATUS
    Close(
        HANDLE FileHandle
    )
    {
#if ML_KERNEL_MODE
        return ZwClose(FileHandle);
#else
        return NtClose(FileHandle);
#endif
    }

    ForceInline
    static
    NTSTATUS
    Open(
        PHANDLE     FileHandle,
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ,
        ULONG_PTR   Access              = GENERIC_READ,
        ULONG_PTR   CreationDisposition = FILE_OPEN,
        ULONG_PTR   Attributes          = FILE_ATTRIBUTE_NORMAL,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT
    )
    {
        return Create(FileHandle, FileName, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);
    }

    ForceInline
    static
    NTSTATUS
    Create(
        PHANDLE     FileHandle,
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ,
        ULONG_PTR   Access              = GENERIC_WRITE,
        ULONG_PTR   CreationDisposition = FILE_OVERWRITE_IF,
        ULONG_PTR   Attributes          = FILE_ATTRIBUTE_NORMAL,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT
    )
    {
        return CreateFileInternal(FileHandle, FileName, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);
    }

    ForceInline
    static
    NTSTATUS
    CreateDirectory(
        PHANDLE     FileHandle,
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ | FILE_SHARE_WRITE,
        ULONG_PTR   Access              = GENERIC_ALL,
        ULONG_PTR   CreationDisposition = FILE_CREATE,
        ULONG_PTR   Attributes          = FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_DIRECTORY,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT | FILE_DIRECTORY_FILE
    )
    {
        return Create(
                    FileHandle,
                    FileName,
                    Flags,
                    ShareMode,
                    Access,
                    CreationDisposition,
                    Attributes,
                    CreateOptions
               );
    }

    ForceInline
    static
    NTSTATUS
    OpenDirectory(
        PHANDLE     FileHandle,
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ,
        ULONG_PTR   Access              = GENERIC_READ,
        ULONG_PTR   CreationDisposition = FILE_OPEN,
        ULONG_PTR   Attributes          = FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_DIRECTORY,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT | FILE_DIRECTORY_FILE
    )
    {
        return CreateDirectory(FileHandle, FileName, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);
    }

    ForceInline
    static
    NTSTATUS
    OpenVolume(
        PHANDLE     FileHandle,
        ULONG_PTR   Drive,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        ULONG_PTR   Access              = GENERIC_READ,
        ULONG_PTR   CreationDisposition = FILE_OPEN,
        ULONG_PTR   Attributes          = 0,
        ULONG_PTR   CreateOptions       = FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
    )
    {
        WCHAR Volume[0x10];

        Volume[0] = '\\';
        Volume[1] = '\\';
        Volume[2] = '.';
        Volume[3] = '\\';
        Volume[4] = (WCHAR)Drive;
        Volume[5] = ':';
        Volume[6] = 0;

        return OpenDirectory(FileHandle, Volume, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);
    }

    static
    NTSTATUS
    OpenDevice(
        PHANDLE     FileHandle,
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ,
        ULONG_PTR   Access              = FILE_READ_ATTRIBUTES | SYNCHRONIZE,
        ULONG_PTR   CreationDisposition = FILE_OPEN,
        ULONG_PTR   Attributes          = 0,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT
    )
    {
        NTSTATUS        Status;
        UNICODE_STRING  DeviceName;

        RtlInitUnicodeString(&DeviceName, FileName);
        Status = CreateFileInternalWithFullPath(FileHandle, &DeviceName, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);
        return Status;
    }

    ForceInline
    static
    NTSTATUS
    OpenSymbolicLink(
        PHANDLE     FileHandle,
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ,
        ULONG_PTR   Access              = GENERIC_READ,
        ULONG_PTR   CreationDisposition = FILE_OPEN,
        ULONG_PTR   Attributes          = 0,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT | FILE_OPEN_REPARSE_POINT | FILE_SYNCHRONOUS_IO_NONALERT
    )
    {
        return CreateSymbolicLink(FileHandle, FileName, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);
    }

    ForceInline
    static
    NTSTATUS
    CreateSymbolicLink(
        PHANDLE     FileHandle,
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ | FILE_SHARE_WRITE,
        ULONG_PTR   Access              = GENERIC_ALL,
        ULONG_PTR   CreationDisposition = FILE_CREATE,
        ULONG_PTR   Attributes          = 0,
        ULONG_PTR   CreateOptions       = FILE_OPEN_FOR_BACKUP_INTENT | FILE_OPEN_REPARSE_POINT | FILE_SYNCHRONOUS_IO_NONALERT
    )
    {
        return CreateFileInternal(
                    FileHandle,
                    FileName,
                    Flags,
                    ShareMode,
                    Access,
                    CreationDisposition,
                    Attributes,
                    CreateOptions
               );
    }

    ForceInline
    static
    NTSTATUS
    QuerySymbolicTarget(
        HANDLE                  FileHandle,
        PREPARSE_DATA_BUFFER    ReparseBuffer,
        ULONG_PTR               BufferSize,
        PULONG_PTR              BytesReturned = nullptr
    )
    {
        return QuerySymbolicTargetInternal(
                    FileHandle,
                    ReparseBuffer,
                    BufferSize,
                    BytesReturned
               );
    }

    ForceInline
    static
    NTSTATUS
    QuerySymbolicTarget(
        HANDLE                  FileHandle,
        PWSTR                   PrintNameBuffer,
        PULONG_PTR              PrintNameBufferCount,
        PWSTR                   SubstituteNameBuffer        = nullptr,
        PULONG_PTR              SubstituteNameBufferCount   = nullptr
    )
    {
        return QuerySymbolicTargetInternal(
                    FileHandle,
                    PrintNameBuffer,
                    PrintNameBufferCount,
                    SubstituteNameBuffer,
                    SubstituteNameBufferCount
               );
    }

    ForceInline
    static
    NTSTATUS
    GetSize(
        HANDLE          FileHandle,
        PLARGE_INTEGER  FileSize
    )
    {
        return GetSizeInternal(FileHandle, FileSize);
    }

    ForceInline
    static
    NTSTATUS
    Read(
        HANDLE          FileHandle,
        PVOID           Buffer,
        ULONG_PTR       Size,
        PLARGE_INTEGER  BytesRead = nullptr,
        PLARGE_INTEGER  Offset = nullptr
    )
    {
        return ReadInternal(FileHandle, Buffer, Size, BytesRead, Offset);
    }

    ForceInline
    static
    NTSTATUS
    Write(
        HANDLE          FileHandle,
        PVOID           Buffer,
        ULONG_PTR       Size,
        PLARGE_INTEGER  BytesWritten = nullptr,
        PLARGE_INTEGER  Offset = nullptr
    )
    {
        return WriteInternal(FileHandle, Buffer, Size, BytesWritten, Offset);
    }

    ForceInline
    static
    NTSTATUS
    Seek(
        HANDLE          FileHandle,
        LONG64          Offset,
        ULONG_PTR       MoveMethod = FILE_BEGIN,
        PLARGE_INTEGER  NewPosition = nullptr
    )
    {
        LARGE_INTEGER NewOffset;

        NewOffset.QuadPart = Offset;
        return Seek(FileHandle, NewOffset, MoveMethod, NewPosition);
    }

    ForceInline
    static
    NTSTATUS
    Seek(
        HANDLE          FileHandle,
        LARGE_INTEGER   Offset,
        ULONG_PTR       MoveMethod = FILE_BEGIN,
        PLARGE_INTEGER  NewPosition = nullptr
    )
    {
        return SeekInternal(FileHandle, Offset, MoveMethod, NewPosition);
    }

    ForceInline
    static
    NTSTATUS
    SetEndOfFile(
        HANDLE FileHandle
    );

    ForceInline
    static
    NTSTATUS
    Delete(
        HANDLE FileHandle
    )
    {
        return DeleteInternal(FileHandle);
    }

    static
    NTSTATUS
    QueryFullNtPath(
        PCWSTR          FileName,
        PUNICODE_STRING NtFilePath,
        ULONG_PTR       Flags = 0
    );

#if ML_USER_MODE

    static
    HANDLE
    SimulateCreateFile(
        PCWSTR lpFileName,
        ULONG dwDesiredAccess,
        ULONG dwShareMode,
        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        ULONG dwCreationDisposition,
        ULONG dwFlagsAndAttributes,
        HANDLE hTemplateFile
    )
    {
        OBJECT_ATTRIBUTES ObjectAttributes;
        IO_STATUS_BLOCK IoStatusBlock;
        UNICODE_STRING NtPathU;
        HANDLE FileHandle;
        NTSTATUS Status;
        ULONG FileAttributes, Flags = 0;
        PVOID EaBuffer = nullptr;
        ULONG EaLength = 0;

        if (lpFileName == nullptr || lpFileName[0] == 0)
        {
            SetLastNTError(STATUS_OBJECT_PATH_NOT_FOUND);
            return INVALID_HANDLE_VALUE;
        }

        /* validate & translate the creation disposition */
        switch (dwCreationDisposition)
        {
            case CREATE_NEW:
                dwCreationDisposition = FILE_CREATE;
                break;

            case CREATE_ALWAYS:
                dwCreationDisposition = FILE_OVERWRITE_IF;
                break;

            case OPEN_EXISTING:
                dwCreationDisposition = FILE_OPEN;
                break;

            case OPEN_ALWAYS:
                dwCreationDisposition = FILE_OPEN_IF;
                break;

            case TRUNCATE_EXISTING:
                dwCreationDisposition = FILE_OVERWRITE;
                break;

            default:
                SetLastNTError(STATUS_INVALID_PARAMETER);
                return (INVALID_HANDLE_VALUE);
        }

        /* check for console input/output */
#if 0
        if (0 == _wcsicmp(L"CONOUT$", lpFileName)
            || 0 == _wcsicmp(L"CONIN$", lpFileName))
        {
            return OpenConsoleW(lpFileName,
                dwDesiredAccess,
                lpSecurityAttributes ? lpSecurityAttributes->bInheritHandle : FALSE,
                FILE_SHARE_READ | FILE_SHARE_WRITE);
        }
#endif
        /* validate & translate the flags */

        /* translate the flags that need no validation */
        if (!(dwFlagsAndAttributes & FILE_FLAG_OVERLAPPED))
        {
        /* yes, nonalert is correct! apc's are not delivered
            while waiting for file io to complete */
            Flags |= FILE_SYNCHRONOUS_IO_NONALERT;
        }

        if(dwFlagsAndAttributes & FILE_FLAG_WRITE_THROUGH)
            Flags |= FILE_WRITE_THROUGH;

        if(dwFlagsAndAttributes & FILE_FLAG_NO_BUFFERING)
            Flags |= FILE_NO_INTERMEDIATE_BUFFERING;

        if(dwFlagsAndAttributes & FILE_FLAG_RANDOM_ACCESS)
            Flags |= FILE_RANDOM_ACCESS;

        if(dwFlagsAndAttributes & FILE_FLAG_SEQUENTIAL_SCAN)
            Flags |= FILE_SEQUENTIAL_ONLY;

        if(dwFlagsAndAttributes & FILE_FLAG_DELETE_ON_CLOSE)
            Flags |= FILE_DELETE_ON_CLOSE;

        if(dwFlagsAndAttributes & FILE_FLAG_BACKUP_SEMANTICS)
        {
            if(dwDesiredAccess & GENERIC_ALL)
                Flags |= FILE_OPEN_FOR_BACKUP_INTENT | FILE_OPEN_REMOTE_INSTANCE;
            else
            {
                if(dwDesiredAccess & GENERIC_READ)
                    Flags |= FILE_OPEN_FOR_BACKUP_INTENT;

                if(dwDesiredAccess & GENERIC_WRITE)
                    Flags |= FILE_OPEN_REMOTE_INSTANCE;
            }
        }
        else
            Flags |= FILE_NON_DIRECTORY_FILE;

        if(dwFlagsAndAttributes & FILE_FLAG_OPEN_REPARSE_POINT)
            Flags |= FILE_OPEN_REPARSE_POINT;

        if(dwFlagsAndAttributes & FILE_FLAG_OPEN_NO_RECALL)
            Flags |= FILE_OPEN_NO_RECALL;

        FileAttributes = (dwFlagsAndAttributes & (FILE_ATTRIBUTE_VALID_FLAGS & ~FILE_ATTRIBUTE_DIRECTORY));

        /* handle may allways be waited on and querying attributes are allways allowed */
        dwDesiredAccess |= SYNCHRONIZE | FILE_READ_ATTRIBUTES;

        /* FILE_FLAG_POSIX_SEMANTICS is handled later */

        /* validate & translate the filename */
        if (!RtlDosPathNameToNtPathName_U(
                lpFileName,
                &NtPathU,
                nullptr,
                nullptr))
        {
            RtlSetLastWin32Error(ERROR_PATH_NOT_FOUND);
            return INVALID_HANDLE_VALUE;
        }

        if (hTemplateFile != nullptr)
        {
            FILE_EA_INFORMATION EaInformation;

        /* try to get the size of the extended attributes, if we fail just continue
            creating the file without copying the attributes! */
            Status = ZwQueryInformationFile(
                        hTemplateFile,
                        &IoStatusBlock,
                        &EaInformation,
                        sizeof(FILE_EA_INFORMATION),
                        FileEaInformation
                     );
            if (NT_SUCCESS(Status) && (EaInformation.EaSize != 0))
            {
                /* there's extended attributes to read, let's give it a try */
    /*
                EaBuffer = RtlAllocateHeap(RtlGetProcessHeap(),
                    0,
                    EaInformation.EaSize);
    */
                EaBuffer = AllocStack(EaInformation.EaSize);
                Status = ZwQueryEaFile(hTemplateFile,
                            &IoStatusBlock,
                            EaBuffer,
                            EaInformation.EaSize,
                            FALSE,
                            nullptr,
                            0,
                            nullptr,
                            TRUE
                         );

                if (NT_SUCCESS(Status))
                {
                /* we successfully read the extended attributes, break the loop
                    and continue */
                    EaLength = EaInformation.EaSize;
                }
            }
        }

        /* build the object attributes */
        InitializeObjectAttributes(&ObjectAttributes, &NtPathU, 0, nullptr, nullptr);

        if (lpSecurityAttributes)
        {
            if(lpSecurityAttributes->bInheritHandle)
                ObjectAttributes.Attributes |= OBJ_INHERIT;

            ObjectAttributes.SecurityDescriptor = lpSecurityAttributes->lpSecurityDescriptor;
        }

        if(!(dwFlagsAndAttributes & FILE_FLAG_POSIX_SEMANTICS))
            ObjectAttributes.Attributes |= OBJ_CASE_INSENSITIVE;

        /* perform the call */
        Status = ZwCreateFile(&FileHandle,
                    dwDesiredAccess,
                    &ObjectAttributes,
                    &IoStatusBlock,
                    nullptr,
                    FileAttributes,
                    dwShareMode,
                    dwCreationDisposition,
                    Flags,
                    EaBuffer,
                    EaLength
                );

        RtlFreeUnicodeString(&NtPathU);

#if 0
        /* free the extended attributes buffer if allocated */
        if (EaBuffer != NULL)
        {
            RtlFreeHeap(RtlGetProcessHeap(), 0, EaBuffer);
        }
#endif

        /* error */
        if (!NT_SUCCESS(Status))
        {
        /* In the case file creation was rejected due to CREATE_NEW flag
        * was specified and file with that name already exists, correct
        * last error is ERROR_FILE_EXISTS and not ERROR_ALREADY_EXISTS.
        * Note: RtlNtStatusToDosError is not the subject to blame here.
            */
            if (Status == STATUS_OBJECT_NAME_COLLISION &&
                dwCreationDisposition == FILE_CREATE)
            {
                RtlSetLastWin32Error( ERROR_FILE_EXISTS );
            }
            else
            {
                SetLastNTError(Status);
            }

            return INVALID_HANDLE_VALUE;
        }

        /*
        create with OPEN_ALWAYS (FILE_OPEN_IF) returns info = FILE_OPENED or FILE_CREATED
        create with CREATE_ALWAYS (FILE_OVERWRITE_IF) returns info = FILE_OVERWRITTEN or FILE_CREATED
        */
        if (dwCreationDisposition == FILE_OPEN_IF)
        {
            RtlSetLastWin32Error(IoStatusBlock.Information == FILE_OPENED ? ERROR_ALREADY_EXISTS : 0);
        }
        else if (dwCreationDisposition == FILE_OVERWRITE_IF)
        {
            RtlSetLastWin32Error(IoStatusBlock.Information == FILE_OVERWRITTEN ? ERROR_ALREADY_EXISTS : 0);
        }

        return FileHandle;
    }

#endif // r3

};

#if !ML_KERNEL_MODE

/************************************************************************
  memory file
************************************************************************/
class NtFileMemory
{
protected:
    PVOID           m_BufferBase, m_BufferCurrent, m_BufferEnd;
    LARGE_INTEGER   m_FileSize;
    MemoryAllocator m_Memory;

public:
    NtFileMemory()
    {
        m_BufferBase    = nullptr;
        m_BufferCurrent = nullptr;
        m_BufferEnd     = nullptr;
    }

    ~NtFileMemory()
    {
        Close();
    }

    NTSTATUS Close()
    {
        return ReleaseAll();
    }

    NTSTATUS Open(
        PCWSTR      FileName,
        ULONG_PTR   Flags               = 0,
        ULONG_PTR   ShareMode           = FILE_SHARE_READ,
        ULONG_PTR   Access              = GENERIC_READ,
        ULONG_PTR   CreationDisposition = FILE_OPEN,
        ULONG_PTR   Attributes          = FILE_ATTRIBUTE_NORMAL,
        ULONG_PTR   CreateOptions       = 0
    )
    {
        NtFileDisk  File;
        PVOID       Buffer;
        ULONG_PTR   Size;
        NTSTATUS    Status;

        ReleaseAll();

        Status = File.Open(FileName, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);
        FAIL_RETURN(Status);

        Size = File.GetSize32();
        m_BufferBase = m_Memory.Alloc(ROUND_UP(Size + sizeof(ULONG_PTR), 16));
        if (m_BufferBase == nullptr)
            return STATUS_NO_MEMORY;

        Status = File.Read(m_BufferBase, Size);
        if (NT_FAILED(Status))
        {
            ReleaseAll();
            return Status;
        }

        *(PULONG)PtrAdd(m_BufferBase, Size) = 0;

        m_FileSize.QuadPart = Size;
        m_BufferCurrent     = m_BufferBase;
        m_BufferEnd         = PtrAdd(m_BufferCurrent, Size);

        return Status;
    }

    NTSTATUS Open(PVOID Buffer, ULONG_PTR Size)
    {
        ReleaseAll();

        m_BufferBase = m_Memory.Alloc(ROUND_UP(Size + sizeof(ULONG_PTR), 16));
        if (m_BufferBase == nullptr)
            return STATUS_NO_MEMORY;

        CopyMemory(m_BufferBase, Buffer, Size);

        *(PULONG)PtrAdd(m_BufferBase, Size) = 0;

        m_FileSize.QuadPart = Size;
        m_BufferCurrent     = m_BufferBase;
        m_BufferEnd         = PtrAdd(m_BufferCurrent, Size);

        return STATUS_SUCCESS;
    }

    NTSTATUS GetPosition(PLARGE_INTEGER Position)
    {
        if (Position == nullptr)
            return STATUS_INVALID_PARAMETER;

        Position->QuadPart = PtrOffset(m_BufferCurrent, m_BufferBase);

        return STATUS_SUCCESS;
    }

    ULONG_PTR GetPosition32()
    {
        return PtrOffset(m_BufferCurrent, m_BufferBase);
    }

    ULONG_PTR GetSize32()
    {
        return (ULONG_PTR)GetSize64();
    }

    LONG64 GetSize64()
    {
        return m_FileSize.QuadPart;
    }

    PVOID GetBuffer()
    {
        return m_BufferCurrent;
    }

    operator PVOID()
    {
        return this->GetBuffer();
    }

    operator ULONG_PTR()
    {
        return (ULONG_PTR)this->GetSize64();
    }

    inline
    NTSTATUS
    Seek(
        LONG64          Offset,
        ULONG_PTR       MoveMethod = FILE_BEGIN,
        PLARGE_INTEGER  NewPosition = nullptr
    )
    {
        LARGE_INTEGER NewOffset;

        NewOffset.QuadPart = Offset;
        return Seek(NewOffset, MoveMethod, NewPosition);
    }

    NoInline
    NTSTATUS
    Seek(
        LARGE_INTEGER   Offset,
        ULONG_PTR       MoveMethod = FILE_BEGIN,
        PLARGE_INTEGER  NewPosition = nullptr
    )
    {
        LARGE_INTEGER NewOffset;

        NewOffset.QuadPart = PtrOffset(m_BufferCurrent, m_BufferBase);

        switch(MoveMethod)
        {
            case FILE_CURRENT:
                NewOffset.QuadPart += Offset.QuadPart;
                break;

            case FILE_END:
                NewOffset.QuadPart = m_FileSize.QuadPart + Offset.QuadPart;
                break;

            case FILE_BEGIN:
                NewOffset.QuadPart = Offset.QuadPart;
                break;

            default:
                return STATUS_INVALID_PARAMETER_2;
        }

        if (NewOffset.QuadPart < 0)
            return STATUS_INVALID_PARAMETER_1;

        if (NewOffset.QuadPart > m_FileSize.QuadPart)
        {
            PVOID BufferBase;

            BufferBase = m_Memory.ReAlloc(m_BufferBase, (SizeT)NewOffset.QuadPart, HEAP_ZERO_MEMORY);
            if (BufferBase == nullptr)
                return STATUS_NO_MEMORY;

            m_FileSize.QuadPart = NewOffset.QuadPart;
            m_BufferBase        = BufferBase;
            m_BufferEnd         = PtrAdd(BufferBase, m_FileSize.QuadPart);
        }

        m_BufferCurrent = PtrAdd(m_BufferBase, NewOffset.QuadPart);

        if (NewPosition != nullptr)
            NewPosition->QuadPart = NewOffset.QuadPart;

        return STATUS_SUCCESS;
    }

    NoInline
    NTSTATUS
    Read(
        PVOID           Buffer,
        ULONG_PTR       Size,
        PLARGE_INTEGER  BytesRead = nullptr
    )
    {
        ULONG_PTR SizeRemain;

        SizeRemain = PtrOffset(m_BufferEnd, m_BufferCurrent);
        Size = ML_MIN(Size, SizeRemain);

        if (Size == 0)
            return STATUS_END_OF_FILE;

        if (Buffer == nullptr)
        {
            if (BytesRead != nullptr)
            {
                BytesRead->QuadPart = SizeRemain;
                return STATUS_SUCCESS;
            }

            return STATUS_INVALID_PARAMETER_1;
        }

        CopyMemory(Buffer, m_BufferCurrent, Size);

        if (BytesRead != nullptr)
            BytesRead->QuadPart = Size;

        m_BufferCurrent = PtrAdd(m_BufferCurrent, Size);

        return STATUS_SUCCESS;
    }

    NTSTATUS
    Write(
        PVOID           Buffer,
        ULONG_PTR       Size,
        PLARGE_INTEGER  pBytesWritten = nullptr
    );

protected:
    NTSTATUS ReleaseAll()
    {
        m_Memory.SafeFree(&m_BufferBase);
        m_BufferCurrent = nullptr;
        m_BufferEnd     = nullptr;

        return STATUS_SUCCESS;
    }
};

#endif // ML_KERNEL_MODE

#endif // CPP_DEFINED


#endif // _FILEIO_H_322e14b1_8a90_410b_bea8_1b39baddbd22