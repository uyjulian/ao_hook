#ifndef _NTFILEIO_H_a683f7a2_b20c_4e58_b9c8_e58a305b1315_
#define _NTFILEIO_H_a683f7a2_b20c_4e58_b9c8_e58a305b1315_


#ifndef FILE_BEGIN
    #define FILE_BEGIN      0
    #define FILE_CURRENT    1
    #define FILE_END        2
#endif /* FILE_BEGIN */

#if !defined(INVALID_FILE_ATTRIBUTES)
    #define INVALID_FILE_ATTRIBUTES ((ULONG)-1)
#endif // INVALID_FILE_ATTRIBUTES

#if !defined(FSCTL_GET_REPARSE_POINT)

#include <WinIoctl.h>

#endif // FSCTL_GET_REPARSE_POINT

typedef struct _FILE_NAME_INFORMATION2
{
    ULONG FileNameLength;
    WCHAR FileName[MAX_NTPATH];

} FILE_NAME_INFORMATION2, *PFILE_NAME_INFORMATION2;

#if !defined(_NTIFS_)

#define SYMLINK_FLAG_RELATIVE   1

typedef struct _REPARSE_DATA_BUFFER
{
    ULONG  ReparseTag;
    USHORT ReparseDataLength;
    USHORT Reserved;
    union
    {
        struct
        {
            USHORT  SubstituteNameOffset;
            USHORT  SubstituteNameLength;
            USHORT  PrintNameOffset;
            USHORT  PrintNameLength;
            ULONG   Flags;
            WCHAR   PathBuffer[1];
        } SymbolicLinkReparseBuffer;
        struct
        {
            USHORT  SubstituteNameOffset;
            USHORT  SubstituteNameLength;
            USHORT  PrintNameOffset;
            USHORT  PrintNameLength;
            WCHAR   PathBuffer[1];
        } MountPointReparseBuffer;
        struct
        {
            UCHAR   DataBuffer[1];
        } GenericReparseBuffer;
    } DUMMYUNIONNAME;

} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;

typedef struct _FILE_ALLOCATION_INFORMATION
{
    LARGE_INTEGER AllocationSize;

} FILE_ALLOCATION_INFORMATION, *PFILE_ALLOCATION_INFORMATION;

#endif // _NTIFS_

#if !defined(_WDMDDK_)

typedef struct _FILE_EA_INFORMATION
{
    ULONG EaSize;

} FILE_EA_INFORMATION, *PFILE_EA_INFORMATION;

typedef struct
{
/* 0x000 */ ULONG   NextEntryOffset;
/* 0x004 */ UCHAR   Flags;
/* 0x005 */ UCHAR   EaNameLength;
/* 0x006 */ USHORT  EaValueLength;
/* 0x008 */ CHAR    EaName[1];

} FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;

typedef struct _FILE_RENAME_INFORMATION
{
    BOOL    ReplaceIfExists;
    HANDLE  RootDirectory;
    ULONG   FileNameLength;
    WCHAR   FileName[1];

} FILE_RENAME_INFORMATION, *PFILE_RENAME_INFORMATION;

typedef struct _FILE_BOTH_DIR_INFORMATION
{
    ULONG           NextEntryOffset;
    ULONG           FileIndex;
    LARGE_INTEGER   CreationTime;
    LARGE_INTEGER   LastAccessTime;
    LARGE_INTEGER   LastWriteTime;
    LARGE_INTEGER   ChangeTime;
    LARGE_INTEGER   EndOfFile;
    LARGE_INTEGER   AllocationSize;
    ULONG           FileAttributes;
    ULONG           FileNameLength;
    ULONG           EaSize;
    SHORT           ShortNameLength;
    WCHAR           ShortName[12];
    WCHAR           FileName[1];

} FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;

#endif // _NTIFS_

#if ML_USER_MODE

typedef struct _FILE_NETWORK_OPEN_INFORMATION
{
    LARGE_INTEGER   CreationTime;
    LARGE_INTEGER   LastAccessTime;
    LARGE_INTEGER   LastWriteTime;
    LARGE_INTEGER   ChangeTime;
    LARGE_INTEGER   AllocationSize;
    LARGE_INTEGER   EndOfFile;
    ULONG           FileAttributes;

} FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION;


// Named pipe values

// NamedPipeType for NtCreateNamedPipeFile
#define FILE_PIPE_BYTE_STREAM_TYPE 0x00000000
#define FILE_PIPE_MESSAGE_TYPE 0x00000001

// CompletionMode for NtCreateNamedPipeFile
#define FILE_PIPE_QUEUE_OPERATION 0x00000000
#define FILE_PIPE_COMPLETE_OPERATION 0x00000001

// ReadMode for NtCreateNamedPipeFile
#define FILE_PIPE_BYTE_STREAM_MODE 0x00000000
#define FILE_PIPE_MESSAGE_MODE 0x00000001

// NamedPipeConfiguration for NtQueryInformationFile
#define FILE_PIPE_INBOUND 0x00000000
#define FILE_PIPE_OUTBOUND 0x00000001
#define FILE_PIPE_FULL_DUPLEX 0x00000002

// NamedPipeState for NtQueryInformationFile
#define FILE_PIPE_DISCONNECTED_STATE 0x00000001
#define FILE_PIPE_LISTENING_STATE 0x00000002
#define FILE_PIPE_CONNECTED_STATE 0x00000003
#define FILE_PIPE_CLOSING_STATE 0x00000004

// NamedPipeEnd for NtQueryInformationFile
#define FILE_PIPE_CLIENT_END 0x00000000
#define FILE_PIPE_SERVER_END 0x00000001

#endif // r3 only


NATIVE_API
NTSTATUS
NTAPI
NtOpenDirectoryObject(
    OUT PHANDLE             DirectoryHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes
);

NATIVE_API
NTSTATUS
NTAPI
ZwOpenDirectoryObject(
    OUT PHANDLE             DirectoryHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes
);

NATIVE_API
NTSTATUS
NTAPI
ZwOpenFile(
    PHANDLE             FileHandle,
    ACCESS_MASK         DesiredAccess,
    POBJECT_ATTRIBUTES  ObjectAttributes,
    PIO_STATUS_BLOCK    IoStatusBlock,
    ULONG               ShareAccess,
    ULONG               OpenOptions
);

NATIVE_API
NTSTATUS
NTAPI
NtOpenFile(
    PHANDLE             FileHandle,
    ACCESS_MASK         DesiredAccess,
    POBJECT_ATTRIBUTES  ObjectAttributes,
    PIO_STATUS_BLOCK    IoStatusBlock,
    ULONG               ShareAccess,
    ULONG               OpenOptions
);

NATIVE_API
NTSTATUS
NTAPI
NtCreateFile(
    OUT PHANDLE             FileHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    OUT PLARGE_INTEGER      AllocationSize OPTIONAL,
    IN  ULONG               FileAttributes,
    IN  ULONG               ShareAccess,
    IN  ULONG               CreateDisposition,
    IN  ULONG               CreateOptions,
    IN  PVOID               EaBuffer,
    IN  ULONG               EaLength
);

NATIVE_API
NTSTATUS
NTAPI
ZwCreateFile(
    OUT PHANDLE             FileHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    OUT PLARGE_INTEGER      AllocationSize OPTIONAL,
    IN  ULONG               FileAttributes,
    IN  ULONG               ShareAccess,
    IN  ULONG               CreateDisposition,
    IN  ULONG               CreateOptions,
    IN  PVOID               EaBuffer,
    IN  ULONG               EaLength
);

NATIVE_API
NTSTATUS
NTAPI
NtCreateNamedPipeFile(
    OUT PHANDLE             FileHandle,
    IN  ULONG               DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  ULONG               ShareAccess,
    IN  ULONG               CreateDisposition,
    IN  ULONG               CreateOptions,
    IN  ULONG               NamedPipeType,
    IN  ULONG               ReadMode,
    IN  ULONG               CompletionMode,
    IN  ULONG               MaximumInstances,
    IN  ULONG               InboundQuota,
    IN  ULONG               OutboundQuota,
    IN  PLARGE_INTEGER      DefaultTimeout OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
ZwCreateNamedPipeFile(
    OUT PHANDLE             FileHandle,
    IN  ULONG               DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  ULONG               ShareAccess,
    IN  ULONG               CreateDisposition,
    IN  ULONG               CreateOptions,
    IN  ULONG               NamedPipeType,
    IN  ULONG               ReadMode,
    IN  ULONG               CompletionMode,
    IN  ULONG               MaximumInstances,
    IN  ULONG               InboundQuota,
    IN  ULONG               OutboundQuota,
    IN  PLARGE_INTEGER      DefaultTimeout OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtReadFile(
    IN  HANDLE              FileHandle,
    IN  HANDLE              Event OPTIONAL,
    IN  PIO_APC_ROUTINE     ApcRoutine OPTIONAL,
    IN  PVOID               ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    OUT PVOID               Buffer,
    IN  ULONG               Length,
    IN  PLARGE_INTEGER      ByteOffset OPTIONAL,
    IN  PULONG              Key OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
ZwReadFile(
    IN  HANDLE              FileHandle,
    IN  HANDLE              Event OPTIONAL,
    IN  PIO_APC_ROUTINE     ApcRoutine OPTIONAL,
    IN  PVOID               ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    OUT PVOID               Buffer,
    IN  ULONG               Length,
    IN  PLARGE_INTEGER      ByteOffset OPTIONAL,
    IN  PULONG              Key OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtWriteFile(
    IN  HANDLE              FileHandle,
    IN  HANDLE              Event,
    IN  PIO_APC_ROUTINE     ApcRoutine OPTIONAL,
    IN  PVOID               ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  PVOID               Buffer,
    IN  ULONG               Length,
    IN  PLARGE_INTEGER      ByteOffset OPTIONAL,
    IN  PULONG              Key OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
ZwWriteFile(
    IN  HANDLE              FileHandle,
    IN  HANDLE              Event,
    IN  PIO_APC_ROUTINE     ApcRoutine OPTIONAL,
    IN  PVOID               ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  PVOID               Buffer,
    IN  ULONG               Length,
    IN  PLARGE_INTEGER      ByteOffset OPTIONAL,
    IN  PULONG              Key OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtFlushBuffersFile(
    IN  HANDLE              FileHandle,
    OUT PIO_STATUS_BLOCK    IoStatusBlock
);

NATIVE_API
NTSTATUS
NTAPI
ZwFlushBuffersFile(
    IN  HANDLE              FileHandle,
    OUT PIO_STATUS_BLOCK    IoStatusBlock
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryAttributesFile(
    IN  POBJECT_ATTRIBUTES      ObjectAttributes,
    OUT PFILE_BASIC_INFORMATION FileInformation
);

NATIVE_API
NTSTATUS
NTAPI
ZwQueryAttributesFile(
    IN  POBJECT_ATTRIBUTES      ObjectAttributes,
    OUT PFILE_BASIC_INFORMATION FileInformation
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryFullAttributesFile(
    IN  POBJECT_ATTRIBUTES              ObjectAttributes,
    OUT PFILE_NETWORK_OPEN_INFORMATION  FileInformation
);

NATIVE_API
NTSTATUS
NTAPI
ZwQueryFullAttributesFile(
    IN  POBJECT_ATTRIBUTES              ObjectAttributes,
    OUT PFILE_NETWORK_OPEN_INFORMATION  FileInformation
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryInformationFile(
    IN  HANDLE                  FileHandle,
    OUT PIO_STATUS_BLOCK        IoStatusBlock,
    OUT PVOID                   FileInformation,
    IN  ULONG                   Length,
    IN  FILE_INFORMATION_CLASS  FileInformationClass
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryVolumeInformationFile(
    IN  HANDLE                  FileHandle,
    OUT PIO_STATUS_BLOCK        IoStatusBlock,
    OUT PVOID                   FsInformation,
    IN  ULONG                   Length,
    IN  FS_INFORMATION_CLASS    FsInformationClass
);

NATIVE_API
NTSTATUS
NTAPI
ZwQueryVolumeInformationFile(
    IN  HANDLE                  FileHandle,
    OUT PIO_STATUS_BLOCK        IoStatusBlock,
    OUT PVOID                   FsInformation,
    IN  ULONG                   Length,
    IN  FS_INFORMATION_CLASS    FsInformationClass
);

NATIVE_API
NTSTATUS
NTAPI
ZwQueryInformationFile(
    IN  HANDLE                  FileHandle,
    OUT PIO_STATUS_BLOCK        IoStatusBlock,
    OUT PVOID                   FileInformation,
    IN  ULONG                   Length,
    IN  FILE_INFORMATION_CLASS  FileInformationClass
);

NATIVE_API
NTSTATUS
NTAPI
NtSetInformationFile(
    IN  HANDLE                  FileHandle,
    OUT PIO_STATUS_BLOCK        IoStatusBlock,
    IN  PVOID                   FileInformation,
    IN  ULONG                   Length,
    IN  FILE_INFORMATION_CLASS  FileInformationClass
);

NATIVE_API
NTSTATUS
NTAPI
ZwSetInformationFile(
    IN  HANDLE                  FileHandle,
    OUT PIO_STATUS_BLOCK        IoStatusBlock,
    IN  PVOID                   FileInformation,
    IN  ULONG                   Length,
    IN  FILE_INFORMATION_CLASS  FileInformationClass
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryDirectoryFile(
    IN  HANDLE                  FileHandle,
    IN  HANDLE                  Event OPTIONAL,
    IN  PIO_APC_ROUTINE         ApcRoutine OPTIONAL,
    IN  PVOID                   ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK        IoStatusBlock,
    OUT PVOID                   FileInformation,
    IN  ULONG                   Length,
    IN  FILE_INFORMATION_CLASS  FileInformationClass,
    IN  BOOLEAN                 ReturnSingleEntry,
    IN  PUNICODE_STRING         FileName OPTIONAL,
    IN  BOOLEAN                 RestartScan
);

NATIVE_API
NTSTATUS
NTAPI
ZwQueryDirectoryFile(
    IN  HANDLE                  FileHandle,
    IN  HANDLE                  Event OPTIONAL,
    IN  PIO_APC_ROUTINE         ApcRoutine OPTIONAL,
    IN  PVOID                   ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK        IoStatusBlock,
    OUT PVOID                   FileInformation,
    IN  ULONG                   Length,
    IN  FILE_INFORMATION_CLASS  FileInformationClass,
    IN  BOOLEAN                 ReturnSingleEntry,
    IN  PUNICODE_STRING         FileName OPTIONAL,
    IN  BOOLEAN                 RestartScan
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryEaFile(
    IN  HANDLE              FileHandle,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    OUT PVOID               Buffer,
    IN  ULONG               Length,
    IN  BOOLEAN             ReturnSingleEntry,
    IN  PVOID               EaList OPTIONAL,
    IN  ULONG               EaListLength,
    IN  PULONG              EaIndex OPTIONAL,
    IN  BOOLEAN             RestartScan
);

#if !defined(_NTIFS_)

NATIVE_API
NTSTATUS
NTAPI
ZwQueryEaFile(
    IN  HANDLE              FileHandle,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    OUT PVOID               Buffer,
    IN  ULONG               Length,
    IN  BOOLEAN             ReturnSingleEntry,
    IN  PVOID               EaList OPTIONAL,
    IN  ULONG               EaListLength,
    IN  PULONG              EaIndex OPTIONAL,
    IN  BOOLEAN             RestartScan
);

#endif // _NTIFS_

NATIVE_API
NTSTATUS
NTAPI
NtOpenSymbolicLinkObject(
    OUT PHANDLE             LinkHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes
);

NATIVE_API
NTSTATUS
NTAPI
ZwOpenSymbolicLinkObject(
    OUT PHANDLE             LinkHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes
);

NATIVE_API
NTSTATUS
NTAPI
NtQuerySymbolicLinkObject(
    IN      HANDLE          LinkHandle,
    IN OUT  PUNICODE_STRING LinkTarget,
    OUT     PULONG          ReturnedLength OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
ZwQuerySymbolicLinkObject(
    IN      HANDLE          LinkHandle,
    IN OUT  PUNICODE_STRING LinkTarget,
    OUT     PULONG          ReturnedLength OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtDeviceIoControlFile(
    HANDLE              FileHandle,
    HANDLE              Event,
    PIO_APC_ROUTINE     ApcRoutine,
    PVOID               ApcContext,
    PIO_STATUS_BLOCK    IoStatusBlock,
    ULONG               IoControlCode,
    PVOID               InputBuffer,
    ULONG               InputBufferLength,
    PVOID               OutputBuffer,
    ULONG               OutputBufferLength
);

NATIVE_API
NTSTATUS
NTAPI
ZwDeviceIoControlFile(
    HANDLE              FileHandle,
    HANDLE              Event,
    PIO_APC_ROUTINE     ApcRoutine,
    PVOID               ApcContext,
    PIO_STATUS_BLOCK    IoStatusBlock,
    ULONG               IoControlCode,
    PVOID               InputBuffer,
    ULONG               InputBufferLength,
    PVOID               OutputBuffer,
    ULONG               OutputBufferLength
);

NATIVE_API
NTSTATUS
NTAPI
NtFsControlFile(
    IN  HANDLE              FileHandle,
    IN  HANDLE              Event OPTIONAL,
    IN  PIO_APC_ROUTINE     ApcRoutine OPTIONAL,
    IN  PVOID               ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  ULONG               FsControlCode,
    IN  PVOID               InputBuffer OPTIONAL,
    IN  ULONG               InputBufferLength,
    OUT PVOID               OutputBuffer OPTIONAL,
    IN  ULONG               OutputBufferLength
);

NATIVE_API
NTSTATUS
NTAPI
ZwFsControlFile(
    IN  HANDLE              FileHandle,
    IN  HANDLE              Event OPTIONAL,
    IN  PIO_APC_ROUTINE     ApcRoutine OPTIONAL,
    IN  PVOID               ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  ULONG               FsControlCode,
    IN  PVOID               InputBuffer OPTIONAL,
    IN  ULONG               InputBufferLength,
    OUT PVOID               OutputBuffer OPTIONAL,
    IN  ULONG               OutputBufferLength
);

NATIVE_API
NTSTATUS
NTAPI
NtCreateIoCompletion(
    OUT PHANDLE             IoCompletionHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  ULONG               NumberOfConcurrentThreads
);

NATIVE_API
NTSTATUS
NTAPI
NtSetIoCompletion(
    IN HANDLE 	IoCompletionPortHandle,
    IN PVOID 	CompletionKey,
    IN PVOID 	CompletionContext,
    IN NTSTATUS CompletionStatus,
    IN ULONG 	CompletionInformation
);

NATIVE_API
NTSTATUS
NTAPI
NtRemoveIoCompletion(
    IN  HANDLE              IoCompletionHandle,
    OUT PULONG_PTR          CompletionKey,
    OUT PVOID              *CompletionValue,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  PLARGE_INTEGER      Timeout OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtCancelIoFile(
    IN  HANDLE              FileHandle,
    OUT PIO_STATUS_BLOCK    IoStatusBlock
);

NATIVE_API
NTSTATUS
NTAPI
NtCancelIoFileEx(
    IN  HANDLE              FileHandle,
    IN  LPOVERLAPPED        Overlapped,
    OUT PIO_STATUS_BLOCK    IoStatusBlock
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryIoCompletion(
    IN  HANDLE                          IoCompletionHandle,
    IN  IO_COMPLETION_INFORMATION_CLASS IoCompletionInformationClass,
    OUT PVOID                           IoCompletionInformation,
    IN  ULONG                           IoCompletionInformationLength,
    OUT PULONG                          ReturnLength OPTIONAL
);

#if !ML_KERNEL_MODE

NATIVE_API
NTSTATUS
NTAPI
RtlSetCurrentDirectory_U(
    PUNICODE_STRING Path
);

NATIVE_API
ULONG
NTAPI
RtlGetCurrentDirectory_U(
    ULONG   BytesInBuffer,
    PWCHAR  Buffer
);

NATIVE_API
NTSTATUS
NTAPI
RtlWow64EnableFsRedirection(
    BOOLEAN Enable
);

NATIVE_API
NTSTATUS
NTAPI
RtlWow64EnableFsRedirectionEx(
    BOOL    Disable,
    PBOOL   PreviousState
);

#endif // ML_KERNEL_MODE

#endif // _NTFILEIO_H_a683f7a2_b20c_4e58_b9c8_e58a305b1315_