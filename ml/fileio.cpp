
#include "mylibrary.h"

#pragma warning(disable:4750 4995)

NtFileDisk::NtFileDisk(const NtFileDisk &file)
{
    *this = file;
}

NtFileDisk::operator HANDLE() const
{
    return m_FileHandle;
}

NtFileDisk& NtFileDisk::operator=(const NtFileDisk &file)
{
    if (this == &file)
        return *this;

    Close();

    ZwDuplicateObject(
        NtCurrentProcess(),
        file,
        NtCurrentProcess(),
        &m_FileHandle,
        0,
        0,
        DUPLICATE_SAME_ACCESS
    );

    return *this;
}

NtFileDisk& NtFileDisk::operator=(HANDLE Handle)
{
    if (m_FileHandle == Handle)
        return *this;

    Close();

    ZwDuplicateObject(
        NtCurrentProcess(),
        Handle,
        NtCurrentProcess(),
        &m_FileHandle,
        0,
        0,
        DUPLICATE_SAME_ACCESS
    );

    return *this;
}

NTSTATUS NtFileDisk::QueryFullNtPath(PCWSTR FileName, PUNICODE_STRING NtFilePath, ULONG_PTR Flags)
{
#if ML_KERNEL_MODE

    return RtlCreateUnicodeString(NtFilePath, FileName) ? STATUS_SUCCESS : STATUS_OBJECT_PATH_NOT_FOUND;

#else // r3

    WCHAR ExpandBuffer[MAX_NTPATH];
    UNICODE_STRING Path, Expand;

    static UNICODE_STRING DosDevicesPrefix = RTL_CONSTANT_STRING(L"\\??\\");

    if (FLAG_ON(Flags, NFD_EXPAND_ENVVAR))
    {
        RtlInitUnicodeString(&Path, FileName);

        RtlInitEmptyString(&Expand, ExpandBuffer, sizeof(ExpandBuffer));
        RtlExpandEnvironmentStrings_U(nullptr, &Path, &Expand, nullptr);
    }
    else
    {
        RtlInitUnicodeString(&Expand, (PWSTR)FileName);
    }

    if (
        Expand.Length >= DosDevicesPrefix.Length &&
        RtlCompareMemory(Expand.Buffer, DosDevicesPrefix.Buffer, DosDevicesPrefix.Length) == DosDevicesPrefix.Length
       )
    {
        return RtlDuplicateUnicodeString(RTL_DUPSTR_ADD_NULL, &Expand, NtFilePath);
    }

    return RtlDosPathNameToNtPathName_U(Expand.Buffer, NtFilePath, nullptr, nullptr) ? STATUS_SUCCESS : STATUS_OBJECT_PATH_NOT_FOUND;

#endif // MY_NT_DDK
}

NTSTATUS
NtFileDisk::
CreateFileInternalWithFullPath(
    PHANDLE         FileHandle,
    PUNICODE_STRING FileName,
    ULONG_PTR       Flags,
    ULONG_PTR       ShareMode,
    ULONG_PTR       Access,
    ULONG_PTR       CreationDisposition,
    ULONG_PTR       Attributes,
    ULONG_PTR       CreateOptions /* = 0 */
)
{
    NTSTATUS            Status;
    OBJECT_ATTRIBUTES   ObjectAttributes;
    IO_STATUS_BLOCK     IoStatus;

    if (!FLAG_ON(Attributes, 0x40000000))
    {
        CreateOptions |= FILE_SYNCHRONOUS_IO_NONALERT;
    }

    CLEAR_FLAG(Attributes, 0x40000000);

/*
    if (FLAG_ON(Attributes, FILE_ATTRIBUTE_DIRECTORY))
    {
        CLEAR_FLAG(Attributes, FILE_ATTRIBUTE_DIRECTORY);
        SET_FLAG(CreateOptions, FILE_DIRECTORY_FILE);
    }

    if (FLAG_ON(Attributes, FILE_FLAG_OPEN_REPARSE_POINT))
    {
        CLEAR_FLAG(Attributes, FILE_FLAG_OPEN_REPARSE_POINT);
        SET_FLAG(CreateOptions, FILE_OPEN_REPARSE_POINT);
    }

    if (FLAG_ON(Attributes, FILE_FLAG_BACKUP_SEMANTICS))
    {
        CLEAR_FLAG(Attributes, FILE_FLAG_BACKUP_SEMANTICS);
        SET_FLAG(CreateOptions, FILE_OPEN_FOR_BACKUP_INTENT);
    }
*/
/*
    if(Access & GENERIC_ALL)
    {
        CreateOptions |= FILE_OPEN_FOR_BACKUP_INTENT; // | FILE_OPEN_REMOTE_INSTANCE;
    }
    else
    {
        if(Access & GENERIC_READ)
            CreateOptions |= FILE_OPEN_FOR_BACKUP_INTENT;

//            if(Access & GENERIC_WRITE)
//                CreateOptions |= FILE_OPEN_REMOTE_INSTANCE;
    }
*/
    InitializeObjectAttributes(&ObjectAttributes, FileName, OBJ_CASE_INSENSITIVE, nullptr, nullptr);

    API_POINTER(ZwCreateFile) XCreateFile;

#if ML_KERNEL_MODE

    XCreateFile = ZwCreateFile;

#else

    XCreateFile = NtCreateFile;

#endif

    Status = XCreateFile(
                FileHandle,
                Access | SYNCHRONIZE | FILE_READ_ATTRIBUTES,
                &ObjectAttributes,
                &IoStatus,
                nullptr,
                Attributes,
                ShareMode,
                CreationDisposition,
                CreateOptions,
                nullptr,
                0
             );

    return Status;
}

NTSTATUS
NtFileDisk::
CreateFileInternal(
    PHANDLE     FileHandle,
    LPCWSTR     FileName,
    ULONG_PTR   Flags,
    ULONG_PTR   ShareMode,
    ULONG_PTR   Access,
    ULONG_PTR   CreationDisposition,
    ULONG_PTR   Attributes,
    ULONG_PTR   CreateOptions /* = 0 */
)
{
    NTSTATUS            Status;
    UNICODE_STRING      NtFilePath;

    if (FLAG_ON(Flags, NFD_NOT_RESOLVE_PATH))
    {
        RtlInitUnicodeString(&NtFilePath, FileName);
    }
    else
    {
        Status = QueryFullNtPath(FileName, &NtFilePath, Flags);
        if (!NT_SUCCESS(Status))
            return Status;
    }

    Status = CreateFileInternalWithFullPath(
                FileHandle,
                &NtFilePath,
                Flags,
                ShareMode,
                Access,
                CreationDisposition,
                Attributes,
                CreateOptions
             );

    if (!FLAG_ON(Flags, NFD_NOT_RESOLVE_PATH))
        RtlFreeUnicodeString(&NtFilePath);

    return Status;
}

NTSTATUS NtFileDisk::GetSizeInternal(HANDLE FileHandle, PLARGE_INTEGER pFileSize)
{
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatus;
    FILE_STANDARD_INFORMATION FileStandard;
    API_POINTER(ZwQueryInformationFile) XQueryInformationFile;

#if ML_KERNEL_MODE

    XQueryInformationFile = ZwQueryInformationFile;

#else

    XQueryInformationFile = NtQueryInformationFile;

#endif

    if (pFileSize == nullptr)
        return STATUS_INVALID_PARAMETER;

    pFileSize->QuadPart = 0;

    Status = XQueryInformationFile(
                FileHandle,
                &IoStatus,
                &FileStandard,
                sizeof(FileStandard),
                FileStandardInformation
              );

    if (!NT_SUCCESS(Status))
        return Status;

    pFileSize->QuadPart = FileStandard.EndOfFile.QuadPart;

    return Status;
}

NTSTATUS
NtFileDisk::
SeekInternal(
    HANDLE          FileHandle,
    LARGE_INTEGER   Offset,
    ULONG_PTR           MoveMethod,
    PLARGE_INTEGER  NewPosition /* = NULL */
)
{
    NTSTATUS                    Status;
    IO_STATUS_BLOCK             IoStatus;
    FILE_POSITION_INFORMATION   FilePosition;
    FILE_STANDARD_INFORMATION   FileStandard;
    API_POINTER(ZwQueryInformationFile) XQueryInformationFile;
    API_POINTER(ZwSetInformationFile)   XSetInformationFile;

#if ML_KERNEL_MODE

    XQueryInformationFile = ZwQueryInformationFile;
    XSetInformationFile = ZwSetInformationFile;

#else

    XQueryInformationFile = NtQueryInformationFile;
    XSetInformationFile = NtSetInformationFile;

#endif


    switch(MoveMethod)
    {
        case FILE_CURRENT:
            XQueryInformationFile(
                FileHandle,
                &IoStatus,
                &FilePosition,
                sizeof(FilePosition),
                FilePositionInformation);

            FilePosition.CurrentByteOffset.QuadPart += Offset.QuadPart;
            break;

        case FILE_END:
            XQueryInformationFile(
                FileHandle,
                &IoStatus,
                &FileStandard,
                sizeof(FileStandard),
                FileStandardInformation);

            FilePosition.CurrentByteOffset.QuadPart = FileStandard.EndOfFile.QuadPart + Offset.QuadPart;
            break;

        case FILE_BEGIN:
            FilePosition.CurrentByteOffset.QuadPart = Offset.QuadPart;
            break;

        default:
            return STATUS_INVALID_PARAMETER_2;
    }

    if(FilePosition.CurrentByteOffset.QuadPart < 0)
        return STATUS_INVALID_PARAMETER_1;

    Status = XSetInformationFile(
                FileHandle,
                &IoStatus,
                &FilePosition,
                sizeof(FILE_POSITION_INFORMATION),
                FilePositionInformation);

    if (!NT_SUCCESS(Status))
        return Status;

    if (NewPosition != nullptr)
    {
        NewPosition->QuadPart = FilePosition.CurrentByteOffset.QuadPart;
    }

    return Status;
}

NTSTATUS
NtFileDisk::
ReadInternal(
    HANDLE          FileHandle,
    PVOID           Buffer,
    ULONG_PTR           Size,
    PLARGE_INTEGER  BytesRead /* = NULL */,
    PLARGE_INTEGER  Offset     /* = NULL */
)
{
    NTSTATUS        Status;
    IO_STATUS_BLOCK IoStatus;
    API_POINTER(ZwReadFile) XReadFile;

    if (BytesRead != nullptr)
        BytesRead->QuadPart = 0;

#if ML_KERNEL_MODE

    XReadFile = ZwReadFile;

#else

    XReadFile = NtReadFile;

#endif

    Status = XReadFile(
                FileHandle,
                nullptr,
                nullptr,
                nullptr,
                &IoStatus,
                Buffer,
                Size,
                Offset,
                nullptr
             );
/*
    if (Status == STATUS_PENDING)
    {
        Status = NtWaitForSingleObject(FileHandle, FALSE, NULL);
        if (NT_SUCCESS(Status))
            Status = IoStatus.Status;
    }
*/
    if (!NT_SUCCESS(Status) || Status == STATUS_PENDING)
        return Status;

    if (BytesRead != nullptr)
        BytesRead->QuadPart = IoStatus.Information;

    return Status;
}

NTSTATUS
NtFileDisk::
WriteInternal(
    HANDLE          FileHandle,
    PVOID           Buffer,
    ULONG_PTR           Size,
    PLARGE_INTEGER  pBytesWritten  /* = NULL */,
    PLARGE_INTEGER  Offset         /* = NULL */
)
{
    NTSTATUS        Status;
    IO_STATUS_BLOCK IoStatus;

    API_POINTER(ZwWriteFile)    XWriteFile;

    if (pBytesWritten != nullptr)
        pBytesWritten->QuadPart = 0;

#if ML_KERNEL_MODE

    XWriteFile = ZwWriteFile;

#else

    XWriteFile = NtWriteFile;

#endif

    Status = XWriteFile(
                FileHandle,
                nullptr,
                nullptr,
                nullptr,
                &IoStatus,
                Buffer,
                Size,
                Offset,
                nullptr
             );
/*
    if (Status == STATUS_PENDING)
    {
        Status = NtWaitForSingleObject(FileHandle, FALSE, NULL);
        if (NT_SUCCESS(Status))
            Status = IoStatus.Status;
    }
*/
    if (!NT_SUCCESS(Status))
        return Status;

    if (pBytesWritten != nullptr)
        pBytesWritten->QuadPart = IoStatus.Information;

    return Status;
}

NTSTATUS NtFileDisk::DeleteInternal(HANDLE FileHandle)
{
    NTSTATUS Status;
    IO_STATUS_BLOCK  IoStatus;
    FILE_DISPOSITION_INFORMATION FileInformation;
    API_POINTER(ZwSetInformationFile)   XSetInformationFile;

#if ML_KERNEL_MODE

    XSetInformationFile = ZwSetInformationFile;

#else

    XSetInformationFile = NtSetInformationFile;

#endif

    FileInformation.DeleteFile = TRUE;
    Status = XSetInformationFile(
                 FileHandle,
                 &IoStatus,
                 &FileInformation,
                 sizeof(FileInformation),
                 FileDispositionInformation);

    return Status;
}

NTSTATUS NtFileDisk::SetEndOfFileInternal(HANDLE FileHandle, LARGE_INTEGER EndPosition)
{
    NTSTATUS        Status;
    IO_STATUS_BLOCK IoStatus;

    union
    {
        FILE_END_OF_FILE_INFORMATION    EndOfFile;
        FILE_ALLOCATION_INFORMATION     FileAllocation;
    };

    API_POINTER(ZwSetInformationFile)   XSetInformationFile;

#if ML_KERNEL_MODE

    XSetInformationFile = ZwSetInformationFile;

#else

    XSetInformationFile = NtSetInformationFile;

#endif


    EndOfFile.EndOfFile.QuadPart = EndPosition.QuadPart;

    Status = XSetInformationFile(
                FileHandle,
                &IoStatus,
                &EndOfFile,
                sizeof(EndOfFile),
                FileEndOfFileInformation
             );

    if (!NT_SUCCESS(Status))
        return Status;

    FileAllocation.AllocationSize.QuadPart = EndPosition.QuadPart;
    Status = XSetInformationFile(
                FileHandle,
                &IoStatus,
                &FileAllocation,
                sizeof(FileAllocation),
                FileAllocationInformation
             );

    return Status;
}

NTSTATUS
NtFileDisk::
QuerySymbolicTargetInternal(
    HANDLE                  FileHandle,
    PREPARSE_DATA_BUFFER    ReparseBuffer,
    ULONG_PTR               BufferSize,
    PULONG_PTR              BytesReturned /* = NULL */
)
{
    NTSTATUS        Status;
    IO_STATUS_BLOCK IoStatus;

    API_POINTER(ZwDeviceIoControlFile) IoControlRoutine;

#if ML_KERNEL_MODE

    IoControlRoutine = ZwFsControlFile;

#else

    IoControlRoutine = NtFsControlFile;

#endif

    Status = IoControlRoutine(
                FileHandle,
                nullptr,
                nullptr,
                nullptr,
                &IoStatus,
                FSCTL_GET_REPARSE_POINT,
                nullptr,
                0,
                ReparseBuffer,
                (ULONG_PTR)BufferSize
             );

    if (BytesReturned != nullptr)
        *BytesReturned = IoStatus.Information;

    return Status;
}

NTSTATUS
NtFileDisk::
QuerySymbolicTargetInternal(
    HANDLE                  FileHandle,
    PWSTR                   PrintNameBuffer,
    PULONG_PTR              PrintNameBufferCount,
    PWSTR                   SubstituteNameBuffer       /* = NULL */,
    PULONG_PTR              SubstituteNameBufferCount  /* = NULL */
)
{
    PWSTR                   PrintName, SubstituteName;
    ULONG_PTR               PrintLength, SubstituteLength;
    ULONG_PTR               ReparseBufferSize, Returned;
    NTSTATUS                Status;
    PREPARSE_DATA_BUFFER    ReparseBuffer;

    ReparseBufferSize = sizeof(*ReparseBuffer) + MAX_NTPATH * sizeof(WCHAR);
    ReparseBuffer = (PREPARSE_DATA_BUFFER)AllocStack(ReparseBufferSize);

    LOOP_FOREVER
    {
        Status = QuerySymbolicTargetInternal(FileHandle, ReparseBuffer, ReparseBufferSize, &Returned);
        if (NT_SUCCESS(Status))
            break;
        if (Status != STATUS_BUFFER_TOO_SMALL)
            return Status;

        ReparseBufferSize += MAX_NTPATH * sizeof(WCHAR);
        ReparseBuffer = (PREPARSE_DATA_BUFFER)AllocStack(ReparseBufferSize);
    }

    if (PrintNameBuffer == nullptr || PrintNameBufferCount == nullptr)
    {
        if (PrintNameBufferCount != nullptr)
        {
            *PrintNameBufferCount = ReparseBuffer->SymbolicLinkReparseBuffer.PrintNameLength / sizeof(WCHAR);
        }
    }
    else
    {
        PrintName   = ReparseBuffer->SymbolicLinkReparseBuffer.PathBuffer + ReparseBuffer->SymbolicLinkReparseBuffer.PrintNameOffset / sizeof(WCHAR);
        PrintLength = ReparseBuffer->SymbolicLinkReparseBuffer.PrintNameLength;

        PrintLength = MY_MIN(PrintLength, *PrintNameBufferCount);
        CopyMemory(PrintNameBuffer, PrintName, PrintLength);
        PrintLength /= sizeof(WCHAR);
        if (PrintLength < *PrintNameBufferCount)
            PrintNameBuffer[PrintLength] = 0;

        *PrintNameBufferCount = PrintLength;
    }

    if (SubstituteNameBuffer == nullptr || SubstituteNameBufferCount == nullptr)
    {
        if (SubstituteNameBufferCount != nullptr)
        {
            *SubstituteNameBufferCount = ReparseBuffer->SymbolicLinkReparseBuffer.SubstituteNameLength / sizeof(WCHAR);
        }
    }
    else
    {
        SubstituteName  = ReparseBuffer->SymbolicLinkReparseBuffer.PathBuffer + ReparseBuffer->SymbolicLinkReparseBuffer.SubstituteNameOffset / sizeof(WCHAR);
        SubstituteLength= ReparseBuffer->SymbolicLinkReparseBuffer.SubstituteNameLength;

        SubstituteLength = MY_MIN(SubstituteLength, *SubstituteNameBufferCount);
        CopyMemory(SubstituteNameBuffer, SubstituteName, SubstituteLength);
        SubstituteLength /= sizeof(WCHAR);
        if (SubstituteLength < *SubstituteNameBufferCount)
            SubstituteNameBuffer[SubstituteLength] = 0;

        *SubstituteNameBufferCount = SubstituteLength;
    }

    return Status;
}

NTSTATUS NtFileDisk::Close()
{
    NTSTATUS Status;

    if (m_FileHandle == nullptr)
        return STATUS_SUCCESS;

#if ML_KERNEL_MODE

    Status = ZwClose(m_FileHandle);

#else

    Status = NtClose(m_FileHandle);

#endif

    if (!NT_SUCCESS(Status))
        return Status;

    m_FileHandle = nullptr;

    return Status;
}

NTSTATUS
NtFileDisk::
Create(
    LPCWSTR FileName,
    ULONG_PTR   Flags               /* = 0 */,
    ULONG_PTR   ShareMode           /* = FILE_SHARE_READ */,
    ULONG_PTR   Access              /* = GENERIC_WRITE */,
    ULONG_PTR   CreationDisposition /* = FILE_OVERWRITE_IF */,
    ULONG_PTR   Attributes          /* = FILE_ATTRIBUTE_NORMAL */,
    ULONG_PTR   CreateOptions       /* = 0 */
)
{
    NTSTATUS Status;

    Close();

    Status = CreateFileInternal(&m_FileHandle, FileName, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);
    if (!NT_SUCCESS(Status))
        return Status;

    if (CreationDisposition != FILE_OVERWRITE_IF)
    {
        Status = GetSizeInternal(m_FileHandle, &m_FileSize);
        if (!NT_SUCCESS(Status))
        {
            Close();
            return Status;
        }
    }
    else
    {
        m_FileSize.QuadPart = 0;
    }

    m_Position.QuadPart = 0;

    return Status;
}

NTSTATUS
NtFileDisk::
OpenDevice(
    PCWSTR      FileName,
    ULONG_PTR   Flags               /* = 0 */,
    ULONG_PTR   ShareMode           /* = FILE_SHARE_READ */,
    ULONG_PTR   Access              /* = GENERIC_READ */,
    ULONG_PTR   CreationDisposition /* = FILE_OPEN */,
    ULONG_PTR   Attributes          /* = FILE_ATTRIBUTE_NORMAL */,
    ULONG_PTR   CreateOptions       /* = 0 */
)
{
    NTSTATUS        Status;
    UNICODE_STRING  DeviceName;

    Close();

    RtlInitUnicodeString(&DeviceName, FileName);

    Status = CreateFileInternalWithFullPath(&m_FileHandle, &DeviceName, Flags, ShareMode, Access, CreationDisposition, Attributes, CreateOptions);
    if (!NT_SUCCESS(Status))
        return Status;

    return Status;
}

NTSTATUS
NtFileDisk::
DeviceIoControl(
    ULONG_PTR   IoControlCode,
    PVOID       InputBuffer,
    ULONG_PTR   InputBufferLength,
    PVOID       OutputBuffer,
    ULONG_PTR   OutputBufferLength,
    PULONG_PTR  BytesReturned /* = NULL */
)
{
    NTSTATUS        Status;
    IO_STATUS_BLOCK IoStatus;
    TYPE_OF(ZwDeviceIoControlFile)* IoControlRoutine;

#if ML_KERNEL_MODE

    IoControlRoutine = DEVICE_TYPE_FROM_CTL_CODE(IoControlCode) == FILE_DEVICE_FILE_SYSTEM ? ZwFsControlFile : ZwDeviceIoControlFile;

#else

    IoControlRoutine = DEVICE_TYPE_FROM_CTL_CODE(IoControlCode) == FILE_DEVICE_FILE_SYSTEM ? NtFsControlFile : NtDeviceIoControlFile;

#endif

    Status = IoControlRoutine(
                m_FileHandle,
                nullptr,
                nullptr,
                nullptr,
                &IoStatus,
                IoControlCode,
                InputBuffer,
                InputBufferLength,
                OutputBuffer,
                OutputBufferLength
             );
    if (!NT_SUCCESS(Status))
        return Status;

    if (BytesReturned != nullptr)
        *BytesReturned = IoStatus.Information;

    return Status;
}

NTSTATUS NtFileDisk::GetSize(PLARGE_INTEGER FileSize)
{
    if (FileSize == nullptr)
        return STATUS_INVALID_PARAMETER;

    FileSize->QuadPart = m_FileSize.QuadPart;

    return STATUS_SUCCESS;
}

NTSTATUS NtFileDisk::GetPosition(PLARGE_INTEGER Position)
{
    if (Position == nullptr)
        return STATUS_INVALID_PARAMETER;

    Position->QuadPart = m_Position.QuadPart;

    return STATUS_SUCCESS;
}

NTSTATUS
NtFileDisk::
Seek(
    LARGE_INTEGER   Offset,
    ULONG_PTR       MoveMethod,
    PLARGE_INTEGER  NewPosition /* = NULL */
)
{
#if 1
    LARGE_INTEGER NewOffset = m_Position;

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
    {
        return STATUS_INVALID_PARAMETER_1;
    }

    m_Position = NewOffset;
    if (m_Position.QuadPart > m_FileSize.QuadPart)
        m_FileSize.QuadPart = m_Position.QuadPart;

    if (NewPosition != nullptr)
        NewPosition->QuadPart = m_Position.QuadPart;

    return STATUS_SUCCESS;
#else
    NTSTATUS Status;
    LARGE_INTEGER CurrentByteOffset;

    Status = SeekInternal(m_FileHandle, Offset, MoveMethod, &CurrentByteOffset);

    if (!NT_SUCCESS(Status))
        return Status;

    m_Position.QuadPart = CurrentByteOffset.QuadPart;
    if (m_Position.QuadPart > m_FileSize.QuadPart)
        m_FileSize.QuadPart = m_Position.QuadPart;

    if (NewPosition != nullptr)
        NewPosition->QuadPart = CurrentByteOffset.QuadPart;

    return Status;
#endif
}

NTSTATUS
NtFileDisk::
Read(
    PVOID           Buffer,
    ULONG_PTR       Size        /* = 0 */,
    PLARGE_INTEGER  BytesRead   /* = NULL */
)
{
    NTSTATUS        Status;
    LARGE_INTEGER   LocalBytesRead;

    if (Size == 0)
        Size = m_FileSize.LowPart;

    Status = ReadInternal(m_FileHandle, Buffer, Size, &LocalBytesRead, &m_Position);
    if (!NT_SUCCESS(Status) || Status == STATUS_PENDING)
        return Status;

    m_Position.QuadPart += LocalBytesRead.QuadPart;

    if (BytesRead != nullptr)
        BytesRead->QuadPart = LocalBytesRead.QuadPart;

    return Status;
}

NTSTATUS
NtFileDisk::
Write(
    PVOID           Buffer,
    ULONG_PTR           Size,
    PLARGE_INTEGER  pBytesWritten /* = NULL */
)
{
    NTSTATUS        Status;
    LARGE_INTEGER   BytesWritten;

    Status = WriteInternal(m_FileHandle, Buffer, Size, &BytesWritten, &m_Position);
    if (!NT_SUCCESS(Status) || Status == STATUS_PENDING)
        return Status;

    if (pBytesWritten != nullptr)
        pBytesWritten->QuadPart = BytesWritten.QuadPart;

    if (BytesWritten.QuadPart + m_Position.QuadPart > m_FileSize.QuadPart)
    {
        m_FileSize.QuadPart = BytesWritten.QuadPart + m_Position.QuadPart;
    }

    m_Position.QuadPart += BytesWritten.QuadPart;

    return Status;
}

NTSTATUS
NtFileDisk::
Print(
    PLARGE_INTEGER  BytesWritten,
    PWSTR           Format,
    ...
)
{
    ULONG_PTR   Length;
    WCHAR   Buffer[0xFF0 / 2];
    va_list Arg;

    va_start(Arg, Format);
    //Length = _vsnwprintf(Buffer, countof(Buffer) - 1, Format, Arg);
    Length = FormatStringvnW(Buffer, countof(Buffer) - 1, Format, Arg);
    if (Length == -1)
        return STATUS_BUFFER_TOO_SMALL;

    return Write(Buffer, Length * sizeof(WCHAR), BytesWritten);
}

NTSTATUS NtFileDisk::MapView(PVOID *BaseAddress, HANDLE ProcessHandle)
{
    NTSTATUS    Status;
    HANDLE      SectionHandle;
    ULONG_PTR   ViewSize;

    Status = NtCreateSection(
                &SectionHandle,
                GENERIC_READ,
                NULL,
                NULL,
                PAGE_READONLY,
                SEC_IMAGE,
                *this
             );
    FAIL_RETURN(Status);

    ViewSize = 0;
    Status = NtMapViewOfSection(
                SectionHandle,
                ProcessHandle,
                BaseAddress,
                0,
                0,
                NULL,
                &ViewSize,
                ViewShare,
                0,
                PAGE_READONLY
             );

    NtClose(SectionHandle);

    return Status;
}

NTSTATUS NtFileDisk::UnMapView(PVOID BaseAddress, HANDLE ProcessHandle)
{
    if (BaseAddress == nullptr)
        return STATUS_INVALID_PARAMETER;

#if ML_USER_MODE

    return NtUnmapViewOfSection(ProcessHandle, BaseAddress);

#else

    return STATUS_NOT_IMPLEMENTED;

#endif
}


NTSTATUS NtFileDisk::SetEndOfFile()
{
    NTSTATUS Status;

    Status = SetEndOfFileInternal(m_FileHandle, m_Position);
    if (NT_SUCCESS(Status))
    {
        m_FileSize.QuadPart = m_Position.QuadPart;
    }

    return Status;
}

NTSTATUS NtFileDisk::SetEndOfFile(LARGE_INTEGER EndPosition)
{
    NTSTATUS Status;

    Status = SetEndOfFileInternal(m_FileHandle, EndPosition);
    if (NT_SUCCESS(Status))
    {
        m_FileSize.QuadPart = EndPosition.QuadPart;
        m_Position.QuadPart = ML_MIN(m_Position.QuadPart, EndPosition.QuadPart);
    }

    return Status;
}

/************************************************************************
  static ver
************************************************************************/

NTSTATUS NtFileDisk::SetEndOfFile(HANDLE FileHandle)
{
    NTSTATUS                    Status;
    FILE_POSITION_INFORMATION   FilePosition;
    IO_STATUS_BLOCK             IoStatus;
    API_POINTER(ZwQueryInformationFile) XQueryInformationFile;

#if ML_KERNEL_MODE

    XQueryInformationFile = ZwQueryInformationFile;

#else

    XQueryInformationFile = NtQueryInformationFile;

#endif

    Status = XQueryInformationFile(
                FileHandle,
                &IoStatus,
                &FilePosition,
                sizeof(FilePosition),
                FilePositionInformation
            );
    if (!NT_SUCCESS(Status))
        return Status;

    return SetEndOfFileInternal(FileHandle, FilePosition.CurrentByteOffset);
}

