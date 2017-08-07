#include "mylibrary.h"

ML_NAMESPACE

ML_NAMESPACE_BEGIN(Native)


#include <mountmgr.h>

ML_NAMESPACE_BEGIN(Io);

VOID IopCopyFindData(PML_FIND_DATA FindFileData, PFILE_BOTH_DIR_INFORMATION FileInfo)
{
    ULONG Length;

    FindFileData->FileAttributes = FileInfo->FileAttributes;

    FindFileData->CreationTime      = FileInfo->CreationTime;
    FindFileData->LastAccessTime    = FileInfo->LastAccessTime;
    FindFileData->LastWriteTime     = FileInfo->LastWriteTime;
    FindFileData->FileSize          = FileInfo->EndOfFile;

    if (FLAG_ON(FileInfo->FileAttributes, FILE_ATTRIBUTE_REPARSE_POINT))
        FindFileData->Reserved0 = FileInfo->EaSize;

    Length = ML_MIN(FileInfo->FileNameLength, sizeof(FindFileData->FileName));
    CopyMemory(FindFileData->FileName, FileInfo->FileName, Length);
    if (Length < sizeof(FindFileData->FileName))
        FindFileData->FileName[Length / sizeof(WCHAR)] = 0;

    Length = ML_MIN(FileInfo->ShortNameLength, sizeof(FindFileData->AlternateFileName));
    CopyMemory(FindFileData->AlternateFileName, FileInfo->ShortName, Length);
    if (Length < sizeof(FindFileData->AlternateFileName))
        FindFileData->AlternateFileName[Length / sizeof(WCHAR)] = 0;
}

NTSTATUS
IopQueryFileBothDirectoryInformation(
    HANDLE                      DirectoryHandle,
    BOOL                        RestartScan,
    PUNICODE_STRING             FileName,
    PFILE_BOTH_DIR_INFORMATION* FileBothInformation
)
{
    NTSTATUS                    Status;
    ULONG_PTR                   FileInfoSize;
    IO_STATUS_BLOCK             IoStatus;
    PFILE_BOTH_DIR_INFORMATION  LocalFileBothInformation;

    API_POINTER(ZwQueryDirectoryFile) XZwQueryDirectoryFile;

    FileInfoSize = MAX_NTPATH * sizeof(WCHAR) + sizeof(*LocalFileBothInformation);
    LocalFileBothInformation = (PFILE_BOTH_DIR_INFORMATION)AllocateMemory(FileInfoSize);
    if (LocalFileBothInformation == NULL)
        return STATUS_NO_MEMORY;

#if ML_KERNEL_MODE

    XZwQueryDirectoryFile = ZwQueryDirectoryFile;

#elif ML_USER_MODE

    XZwQueryDirectoryFile = NtQueryDirectoryFile;

#endif

    LOOP_FOREVER
    {
        Status = XZwQueryDirectoryFile(
                    DirectoryHandle,
                    NULL,
                    NULL,
                    NULL,
                    &IoStatus,
                    LocalFileBothInformation,
                    FileInfoSize,
                    FileBothDirectoryInformation,
                    TRUE,
                    FileName,
                    (BOOLEAN)RestartScan
                 );

        if (Status != STATUS_BUFFER_OVERFLOW)
            break;

        FreeMemory(LocalFileBothInformation);
        FileInfoSize *= 2;
        LocalFileBothInformation = (PFILE_BOTH_DIR_INFORMATION)AllocateMemory(FileInfoSize);

        if (LocalFileBothInformation == NULL)
        {
            Status = STATUS_NO_MEMORY;
            break;
        }
    }

    if (NT_SUCCESS(Status))
    {
        *FileBothInformation = LocalFileBothInformation;
    }
    else
    {
        FreeMemory(LocalFileBothInformation);
    }

    return Status;
}

NTSTATUS QueryFirstFile(PHANDLE Handle, PCWSTR FileName, PML_FIND_DATA FindFileData)
{
    NTSTATUS                    Status;
    ULONG_PTR                   Length, FirstChar;
    HANDLE                      DirectoryHandle;
    PWSTR                       LocalFileName, FileNameBuffer;
    UNICODE_STRING              NtPath, BaseFileName;
    IO_STATUS_BLOCK             IoStatus;
    PFILE_BOTH_DIR_INFORMATION  FileBothInformation;

    Length = StrLengthW(FileName);
    Length -= FileName[Length - 1] == '\\';

    FileNameBuffer = (PWSTR)AllocateMemory(Length * sizeof(WCHAR) + sizeof(WCHAR));
    if (FileNameBuffer == NULL)
        return STATUS_NO_MEMORY;

    CopyMemory(FileNameBuffer, FileName, Length * sizeof(WCHAR));
    FileNameBuffer[Length] = 0;

    Status = DosPathNameToNtPathName(FileNameBuffer, &NtPath, (PCWSTR *)&BaseFileName.Buffer);
    FreeMemory(FileNameBuffer);
    FAIL_RETURN(Status);

    LocalFileName = BaseFileName.Buffer;
    if (LocalFileName == NULL)
    {
        LocalFileName = PtrAdd(NtPath.Buffer, NtPath.Length);
        BaseFileName.Buffer = LocalFileName;
    }

    BaseFileName.Length = (USHORT)(NtPath.Length - (LocalFileName - NtPath.Buffer) * sizeof(WCHAR));
    BaseFileName.MaximumLength = BaseFileName.Length;

    FirstChar = *LocalFileName;
    *LocalFileName = 0;

    Status = NtFileDisk::OpenDirectory(&DirectoryHandle, NtPath.Buffer, NFD_NOT_RESOLVE_PATH);
    if (NT_FAILED(Status))
    {
        RtlFreeUnicodeString(&NtPath);
        return Status;
    }

    *LocalFileName = (WCHAR)FirstChar;
    if (BaseFileName.Length == 6 && *(PULONG64)BaseFileName.Buffer == TAG3W('*.*'))
    {
        BaseFileName.Length = 2;
    }
    else
    {
        for (ULONG Length = BaseFileName.Length / sizeof(WCHAR); Length; --Length)
        {
            switch (LocalFileName[0])
            {
                case '?':
                    LocalFileName[0] = DOS_QM;
                    break;

                case '*':
                    if (LocalFileName[1] != '.')
                        break;

                    LocalFileName[0] = DOS_STAR;
                    break;

                case '.':
                    switch (LocalFileName[1])
                    {
                        case '?':
                        case '*':
                            LocalFileName[0] = DOS_DOT;
                            break;
                    }

                    break;
            }
            ++LocalFileName;
        }
    }

    Status = IopQueryFileBothDirectoryInformation(DirectoryHandle, TRUE, &BaseFileName, &FileBothInformation);

    RtlFreeUnicodeString(&NtPath);

    if (NT_FAILED(Status))
    {
        QueryClose(DirectoryHandle);
        return Status;
    }

    IopCopyFindData(FindFileData, FileBothInformation);
    FreeMemory(FileBothInformation);

    *Handle = DirectoryHandle;

    return Status;
}

NTSTATUS QueryNextFile(HANDLE DirectoryHandle, PML_FIND_DATA FindFileData)
{
    NTSTATUS                    Status;
    PFILE_BOTH_DIR_INFORMATION  FileBothInformation;

    Status = IopQueryFileBothDirectoryInformation(DirectoryHandle, FALSE, NULL, &FileBothInformation);

    if (NT_SUCCESS(Status))
    {
        IopCopyFindData(FindFileData, FileBothInformation);
        FreeMemory(FileBothInformation);
    }

    return Status;
}

ULONG_PTR QueryFileAttributes(PCWSTR FileName)
{
    NTSTATUS    Status;
    ULONG_PTR   FileAttributes;

    Status = QueryFileAttributesEx(FileName, &FileAttributes);

    return NT_SUCCESS(Status) ? FileAttributes : INVALID_FILE_ATTRIBUTES;
}

NTSTATUS QueryFileAttributesEx(PCWSTR FileName, PULONG_PTR FileAttributes)
{
#if ML_USER_MODE

    NTSTATUS                Status;
    UNICODE_STRING          NtPath;
    OBJECT_ATTRIBUTES       ObjectAttributes;
    FILE_BASIC_INFORMATION  FileBasic;

    Status = NtFileDisk::QueryFullNtPath(FileName, &NtPath);
    FAIL_RETURN(Status);

    InitializeObjectAttributes(&ObjectAttributes, &NtPath, OBJ_CASE_INSENSITIVE, NULL, 0);
    Status = NtQueryAttributesFile(&ObjectAttributes, &FileBasic);
    RtlFreeUnicodeString(&NtPath);
    FAIL_RETURN(Status);

    *FileAttributes = FileBasic.FileAttributes;

    return Status;

#elif ML_KERNEL_MODE

    return STATUS_NOT_IMPLEMENTED;

#endif

}

NTSTATUS ApplyFileAttributes(PCWSTR FileName, ULONG_PTR Attributes)
{
    ULONG                   Mask;
    NTSTATUS                Status;
    HANDLE                  FileHandle;
    IO_STATUS_BLOCK         IoStatus;
    FILE_BASIC_INFORMATION  FileInfo;
    OBJECT_ATTRIBUTES       ObjectAttributes;
    UNICODE_STRING          FilePath;

    Status = NtFileDisk::QueryFullNtPath(FileName, &FilePath);
    FAIL_RETURN(Status);

    InitializeObjectAttributes(&ObjectAttributes, &FilePath, OBJ_CASE_INSENSITIVE, nullptr, nullptr);
    Status = NtOpenFile(
                &FileHandle,
                SYNCHRONIZE | FILE_WRITE_ATTRIBUTES,
                &ObjectAttributes,
                &IoStatus,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                FILE_OPEN_REPARSE_POINT | FILE_OPEN_FOR_BACKUP_INTENT | FILE_SYNCHRONOUS_IO_NONALERT
            );

    RtlFreeUnicodeString(&FilePath);

    if (NT_FAILED(Status))
        return Status;

    Mask =  FILE_ATTRIBUTE_NO_SCRUB_DATA |
            FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED |
            FILE_ATTRIBUTE_TEMPORARY |
            FILE_ATTRIBUTE_ARCHIVE |
            FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;

    ZeroMemory(&FileInfo, sizeof(FileInfo));
    FileInfo.FileAttributes = (Attributes & Mask) | FILE_ATTRIBUTE_NORMAL;

    Status = NtSetInformationFile(FileHandle, &IoStatus, &FileInfo, sizeof(FileInfo), FileBasicInformation);
    NtClose(FileHandle);

    return Status;
}

NTSTATUS MoveFile(PCWSTR ExistingFileName, PCWSTR NewFileName, BOOL ReplaceIfExists)
{
    NTSTATUS                    Status;
    HANDLE                      FileHandle;
    UNICODE_STRING              New;
    PFILE_RENAME_INFORMATION    FileRename;
    IO_STATUS_BLOCK             IoStatus;

    Status = NtFileDisk::Open(
                &FileHandle,
                ExistingFileName,
                0,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                SYNCHRONIZE | DELETE | FILE_READ_ATTRIBUTES
             );
    if (NT_FAILED(Status))
        return Status;

    Status = NtFileDisk::QueryFullNtPath(NewFileName, &New);
    if (NT_FAILED(Status))
    {
        NtClose(FileHandle);
        return Status;
    }

    FileRename = (PFILE_RENAME_INFORMATION)AllocStack(New.Length + sizeof(*FileRename));
    FileRename->ReplaceIfExists = ReplaceIfExists;
    FileRename->FileNameLength  = New.Length;
    FileRename->RootDirectory   = NULL;
    CopyMemory(FileRename->FileName, New.Buffer, New.Length);
    RtlFreeUnicodeString(&New);

    Status = NtSetInformationFile(
                FileHandle,
                &IoStatus,
                FileRename,
                FileRename->FileNameLength + sizeof(*FileRename),
                FileRenameInformation
             );
    NtClose(FileHandle);

    return Status;
}

NTSTATUS CopyFile(PCWSTR ExistingFileName, PCWSTR NewFileName, BOOL FailIfExists)
{
    if (FailIfExists && IsPathExists(NewFileName))
        return STATUS_OBJECT_NAME_EXISTS;

    PVOID                   Buffer;
    ULONG                   BufferSize;
    LARGE_INTEGER           BytesTransfered;
    NTSTATUS                Status;
    LARGE_INTEGER           FileSize;
    FILE_BASIC_INFORMATION  FileInformation;
    IO_STATUS_BLOCK         IoStatus;
    NtFileDisk             SourceFile, DestinationFile;

    Status = SourceFile.Open(ExistingFileName);
    if (NT_FAILED(Status))
        return Status;

    Status = DestinationFile.Create(NewFileName);
    if (NT_FAILED(Status))
        return Status;

    Status = ZwQueryInformationFile(SourceFile, &IoStatus, &FileInformation, sizeof(FileInformation), FileBasicInformation);
    if (NT_FAILED(Status))
    {
        DestinationFile.Delete();
        return Status;
    }

    Status = ZwSetInformationFile(DestinationFile, &IoStatus, &FileInformation, sizeof(FileInformation), FileBasicInformation);
    if (NT_FAILED(Status))
    {
        DestinationFile.Delete();
        return Status;
    }

    BufferSize = 0x1000;
    Buffer = AllocStack(BufferSize);
    SourceFile.GetSize(&FileSize);
    for (; FileSize.QuadPart; FileSize.QuadPart -= BytesTransfered.QuadPart)
    {
        BytesTransfered.LowPart = MY_MIN(FileSize.LowPart, BufferSize);
        Status = SourceFile.Read(Buffer, BytesTransfered.LowPart, &BytesTransfered);
        if (NT_FAILED(Status))
            break;

        Status = DestinationFile.Write(Buffer, BytesTransfered.LowPart, &BytesTransfered);
        if (NT_FAILED(Status))
            break;
    }

    if (NT_FAILED(Status))
        DestinationFile.Delete();

    return Status;
}

NTSTATUS DeleteFile(PCWSTR FileName)
{
    HANDLE      FileHandle;
    NTSTATUS    Status;

    Status = NtFileDisk::Open(
                &FileHandle,
                FileName,
                0,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                DELETE,
                FILE_OPEN,
                FILE_ATTRIBUTE_NORMAL,
                FILE_NON_DIRECTORY_FILE | FILE_OPEN_FOR_BACKUP_INTENT | FILE_OPEN_REPARSE_POINT
             );
    if (NT_FAILED(Status))
        return Status;

    Status = NtFileDisk::Delete(FileHandle);
    NtFileDisk::Close(FileHandle);

    return Status;
}

NTSTATUS DeleteDirectory(PCWSTR DirectoryName)
{
    HANDLE      FileHandle;
    NTSTATUS    Status;

    Status = NtFileDisk::Open(
                &FileHandle,
                DirectoryName,
                0,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                DELETE,
                FILE_OPEN,
                FILE_ATTRIBUTE_NORMAL,
                FILE_DIRECTORY_FILE | FILE_OPEN_FOR_BACKUP_INTENT | FILE_OPEN_REPARSE_POINT
             );
    if (NT_FAILED(Status))
        return Status;

    Status = NtFileDisk::Delete(FileHandle);
    NtFileDisk::Close(FileHandle);

    return Status;
}

BOOL IsPathExists(PCWSTR Path)
{
    return QueryFileAttributes(Path) != INVALID_FILE_ATTRIBUTES;
}

NTSTATUS
QueryDosLetterFromVolumeDeviceName(
    PUNICODE_STRING DosLetter,
    PUNICODE_STRING NtDeviceName
)
{
    NtFileDisk              MountPointManager;
    NTSTATUS                Status;
    PMOUNTMGR_VOLUME_PATHS  VolumePath;
    ULONG_PTR               DosVolLength, ReturnLength;
    PMOUNTMGR_TARGET_NAME   TargetName;

    Status = MountPointManager.OpenDevice(MOUNTMGR_DEVICE_NAME);
    FAIL_RETURN(Status);

    DosVolLength = MAX_NTPATH * sizeof(WCHAR) + sizeof(VolumePath->MultiSzLength);
    VolumePath = (PMOUNTMGR_VOLUME_PATHS)AllocStack(DosVolLength);

    TargetName = (PMOUNTMGR_TARGET_NAME)AllocStack(NtDeviceName->Length + sizeof(TargetName->DeviceNameLength));
    TargetName->DeviceNameLength = NtDeviceName->Length;
    CopyMemory(TargetName->DeviceName, NtDeviceName->Buffer, NtDeviceName->Length);

    LOOP_FOREVER
    {
        Status = MountPointManager.DeviceIoControl(
                        IOCTL_MOUNTMGR_QUERY_DOS_VOLUME_PATH,
                        TargetName,
                        TargetName->DeviceNameLength + sizeof(TargetName->DeviceNameLength),
                        VolumePath,
                        DosVolLength,
                        &ReturnLength
                 );
        if (Status != STATUS_MORE_PROCESSING_REQUIRED)
            break;

        VolumePath = (PMOUNTMGR_VOLUME_PATHS)AllocStack(ReturnLength - DosVolLength + sizeof(WCHAR));
        DosVolLength = ReturnLength;
    }

    FAIL_RETURN(Status);

    *PtrAdd(VolumePath->MultiSz, (ULONG_PTR)VolumePath->MultiSzLength) = 0;
    return RtlCreateUnicodeString(DosLetter, VolumePath->MultiSz) ? STATUS_SUCCESS : STATUS_NO_MEMORY;
}

NTSTATUS QueryDosPathFromHandle(PUNICODE_STRING DosPath, HANDLE FileHandle)
{
    NTSTATUS                    Status;
    OBJECT_NAME_INFORMATION2    ObjectName;
    FILE_NAME_INFORMATION2      FileName;
    IO_STATUS_BLOCK             IoStatus;
    UNICODE_STRING              DosLetter;
    ULONG_PTR                   Length;

    API_POINTER(ZwQueryObject)              XQueryObject;
    API_POINTER(ZwQueryInformationFile)     XQueryInformationFile;

#if ML_KERNEL_MODE

    XQueryObject            = ZwQueryObject;
    XQueryInformationFile   = ZwQueryInformationFile;

#else

    XQueryObject            = NtQueryObject;
    XQueryInformationFile   = NtQueryInformationFile;

#endif

    RtlInitEmptyUnicodeString(DosPath, NULL, 0);

    Status = XQueryObject(FileHandle, ObjectNameInformation, &ObjectName, sizeof(ObjectName), NULL);
    FAIL_RETURN(Status);

    Status = XQueryInformationFile(FileHandle, &IoStatus, &FileName, sizeof(FileName), FileNameInformation);
    FAIL_RETURN(Status);

    ObjectName.Name.Length -= (USHORT)FileName.FileNameLength;
    Status = QueryDosLetterFromVolumeDeviceName(&DosLetter, &ObjectName.Name);
    FAIL_RETURN(Status);

    Length = DosLetter.Length;
    CopyMemory(ObjectName.Buffer, DosLetter.Buffer, Length);

    CopyMemory(PtrAdd(ObjectName.Buffer, Length), FileName.FileName, FileName.FileNameLength);

    RtlFreeUnicodeString(&DosLetter);

    Length += FileName.FileNameLength;
    Length /= sizeof(WCHAR);
    ObjectName.Buffer[Length] = 0;

    if (FLAG_ON(QueryFileAttributes(ObjectName.Buffer), FILE_ATTRIBUTE_DIRECTORY))
    {
        if (ObjectName.Buffer[Length - 1] != '\\')
        {
            ObjectName.Buffer[Length++] = '\\';
        }
    }

    ObjectName.Name.Length = (USHORT)(Length * sizeof(WCHAR));

    return RtlDuplicateUnicodeString(RTL_DUPSTR_ADD_NULL, &ObjectName.Name, DosPath);
}

NTSTATUS QueryDosDevice(PCWSTR DeviceName, PUNICODE_STRING TargetPath)
{
    ULONG               Length;
    NTSTATUS            Status;
    HANDLE              Directory, Link;
    OBJECT_ATTRIBUTES   ObjectAttributes;
    UNICODE_STRING      Device, Target;
    PWSTR               TargetBuffer;

#if ML_USER_MODE

    Device = USTR(L"\\??");

    InitializeObjectAttributes(&ObjectAttributes, &Device, OBJ_CASE_INSENSITIVE, NULL, NULL);
    Status = NtOpenDirectoryObject(&Directory, DIRECTORY_QUERY, &ObjectAttributes);
    FAIL_RETURN(Status);

    RtlInitUnicodeString(&Device, DeviceName);
    InitializeObjectAttributes(&ObjectAttributes, &Device, OBJ_CASE_INSENSITIVE, Directory, NULL);
    Status = NtOpenSymbolicLinkObject(&Link, SYMBOLIC_LINK_QUERY, &ObjectAttributes);
    NtClose(Directory);
    FAIL_RETURN(Status);

    RtlInitEmptyUnicodeString(&Target, NULL, 0);

    Status = NtQuerySymbolicLinkObject(Link, &Target, &Length);

    if (Status != STATUS_BUFFER_TOO_SMALL)
    {
        NtClose(Link);
        return Status;
    }

    Length += sizeof(WCHAR);
    TargetBuffer = (PWSTR)AllocStack(Length);
    RtlInitEmptyUnicodeString(&Target, TargetBuffer, Length);

    Status = NtQuerySymbolicLinkObject(Link, &Target, &Length);
    NtClose(Link);

    FAIL_RETURN(Status);

    Length = Target.Length / sizeof(WCHAR);
    if (Target.Buffer[Length - 1] != '\\')
    {
        Target.Buffer[Length++] = '\\';
        Target.Buffer[Length] = 0;
        Target.Length = (USHORT)(Length * sizeof(WCHAR));
    }

    return RtlDuplicateUnicodeString(RTL_DUPSTR_ADD_NULL, &Target, TargetPath);

#else

    Device = USTR(L"\\??");

    InitializeObjectAttributes(&ObjectAttributes, &Device, OBJ_CASE_INSENSITIVE, NULL, NULL);
    Status = ZwOpenDirectoryObject(&Directory, DIRECTORY_QUERY, &ObjectAttributes);
    FAIL_RETURN(Status);

    RtlInitUnicodeString(&Device, DeviceName);
    InitializeObjectAttributes(&ObjectAttributes, &Device, OBJ_CASE_INSENSITIVE, Directory, NULL);
    Status = ZwOpenSymbolicLinkObject(&Link, SYMBOLIC_LINK_QUERY, &ObjectAttributes);
    ZwClose(Directory);
    FAIL_RETURN(Status);

    RtlInitEmptyUnicodeString(&Target, NULL, 0);

    Status = ZwQuerySymbolicLinkObject(Link, &Target, &Length);

    if (Status != STATUS_BUFFER_TOO_SMALL)
    {
        ZwClose(Link);
        return Status;
    }

    Length += sizeof(WCHAR);
    TargetBuffer = (PWSTR)AllocStack(Length);
    RtlInitEmptyUnicodeString(&Target, TargetBuffer, Length);

    Status = ZwQuerySymbolicLinkObject(Link, &Target, &Length);
    ZwClose(Link);

    FAIL_RETURN(Status);

    Length = Target.Length / sizeof(WCHAR);
    if (Target.Buffer[Length - 1] != '\\')
    {
        Target.Buffer[Length++] = '\\';
        Target.Buffer[Length] = 0;
        Target.Length = (USHORT)(Length * sizeof(WCHAR));
    }

    return RtlDuplicateUnicodeString(RTL_DUPSTR_ADD_NULL, &Target, TargetPath);

#endif
}

NTSTATUS
QueryDosPathFromNtDeviceName(
    PUNICODE_STRING DosPath,
    PUNICODE_STRING NtDeviceName
)
{
    NTSTATUS        Status;
    ULONG_PTR       Length;
    WCHAR           DosLetter[4];
    PWSTR           DosPathBuffer;
    UNICODE_STRING  DosDevice;

    *(PULONG)&DosLetter[0] = TAG2W('A:');
    *(PULONG)&DosLetter[2] = 0;

    for (ULONG_PTR Count = 'Z' - 'A' + 1; Count; ++DosLetter[0], --Count)
    {
        Status = QueryDosDevice(DosLetter, &DosDevice);
        FAIL_CONTINUE(Status);

        if (RtlCompareMemory(NtDeviceName->Buffer, DosDevice.Buffer, DosDevice.Length) !=  DosDevice.Length)
        {
            RtlFreeUnicodeString(&DosDevice);
            continue;
        }

        Length = NtDeviceName->Length - DosDevice.Length;
        DosPathBuffer = (PWSTR)AllocStack(Length + sizeof(DosLetter) + sizeof(WCHAR));
        *(PULONG)DosPathBuffer = *(PULONG)DosLetter;
        DosPathBuffer[2] = '\\';

        CopyMemory(DosPathBuffer + 3, PtrAdd(NtDeviceName->Buffer, DosDevice.Length), Length);
        *PtrAdd(DosPathBuffer, Length + 6) = 0;

        RtlFreeUnicodeString(&DosDevice);

        return RtlCreateUnicodeString(DosPath, DosPathBuffer) ? STATUS_SUCCESS : STATUS_NO_MEMORY;
    }

    return STATUS_OBJECT_PATH_NOT_FOUND;
}

NTSTATUS
CreateNamedPipe(
    PHANDLE                 ReadPipe,
    PHANDLE                 WritePipe,
    PUNICODE_STRING         PipeName,
    PSECURITY_ATTRIBUTES    PipeAttributes,
    ULONG_PTR               BufferSize
)
{
    NTSTATUS            Status;
    HANDLE              LocalReadPipe, LocalWritePipe;
    NtFileDisk          NamedPipe;
    OBJECT_ATTRIBUTES   ObjectAttributes;
    IO_STATUS_BLOCK     IoStatus;
    LARGE_INTEGER       DefaultTimeout;
    UNICODE_STRING      PipeObjectName;

    if (BufferSize == 0)
        BufferSize = 0x1000;

    Status = NamedPipe.OpenDevice(
                L"\\Device\\NamedPipe\\",
                0,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                GENERIC_READ | SYNCHRONIZE,
                FILE_OPEN,
                0,
                FILE_SYNCHRONOUS_IO_NONALERT
            );

    FAIL_RETURN(Status);

    if (PipeName != nullptr)
    {
        PipeObjectName = *PipeName;
    }
    else
    {
        RtlInitEmptyString(&PipeObjectName);
    }

    InitializeObjectAttributes(&ObjectAttributes, &PipeObjectName, OBJ_CASE_INSENSITIVE, NamedPipe, nullptr);
    if (PipeAttributes != nullptr)
    {
        ObjectAttributes.SecurityDescriptor = PipeAttributes->lpSecurityDescriptor;
        SET_FLAG(ObjectAttributes.Attributes, PipeAttributes->bInheritHandle ? OBJ_INHERIT : 0);
    }

    FormatTimeOut(&DefaultTimeout, 120000);

    Status = NtCreateNamedPipeFile(
                &LocalReadPipe,
                GENERIC_READ | SYNCHRONIZE | FILE_WRITE_ATTRIBUTES,
                &ObjectAttributes,
                &IoStatus,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                FILE_CREATE,
                FILE_SYNCHRONOUS_IO_NONALERT,
                FILE_PIPE_BYTE_STREAM_TYPE,
                FILE_PIPE_BYTE_STREAM_MODE,
                FILE_PIPE_QUEUE_OPERATION,
                1,
                BufferSize,
                BufferSize,
                &DefaultTimeout
            );
    FAIL_RETURN(Status);

    ObjectAttributes.RootDirectory = LocalReadPipe;

    Status = NtOpenFile(
                &LocalWritePipe,
                GENERIC_WRITE | SYNCHRONIZE | FILE_READ_ATTRIBUTES,
                &ObjectAttributes,
                &IoStatus,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE
            );

    if (NT_FAILED(Status))
    {
        NtClose(LocalReadPipe);
        return Status;
    }

    *ReadPipe = LocalReadPipe;
    *WritePipe = LocalWritePipe;

    return Status;
}

ML_NAMESPACE_END_(Io);


ML_NAMESPACE_END_(Native)

ML_NAMESPACE_END
