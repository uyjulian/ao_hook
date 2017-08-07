#include "mylibrary.h"

ML_NAMESPACE

ML_NAMESPACE_BEGIN(Native)


ML_NAMESPACE_BEGIN(Mm)

NTSTATUS
AllocVirtualMemory(
    PVOID*      BaseAddress,
    ULONG_PTR   Size,
    ULONG       Protect,        /* = PAGE_EXECUTE_READWRITE */
    ULONG       AllocationType, /* = MEM_RESERVE | MEM_COMMIT */
    HANDLE      ProcessHandle   /* = Ps::CurrentProcess */
)
{
    return NtAllocateVirtualMemory(ProcessHandle, BaseAddress, 0, &Size, AllocationType, Protect);
}

NTSTATUS
AllocVirtualMemoryEx(
    HANDLE      ProcessHandle,
    PVOID*      BaseAddress,
    ULONG_PTR   Size,
    ULONG       Protect,
    ULONG       AllocationType
)
{
    return AllocVirtualMemory(BaseAddress, Size, Protect, AllocationType, ProcessHandle);
}

NTSTATUS
FreeVirtualMemory(
    PVOID   BaseAddress,
    HANDLE  ProcessHandle,  /* = Ps::CurrentProcess */
    ULONG   FreeType        /* = MEM_RELEASE */
)
{
    SIZE_T Size = 0;

    return NtFreeVirtualMemory(ProcessHandle, &BaseAddress, &Size, MEM_RELEASE);
}

#if ML_USER_MODE

NTSTATUS
ProtectVirtualMemory(
    PVOID       BaseAddress,
    ULONG_PTR   Size,
    ULONG       NewProtect,
    PULONG      OldProtect,
    HANDLE      ProcessHandle   /* = Ps::CurrentProcess */
)
{
    ULONG _OldProtect;
    NTSTATUS Status;

    Status = NtProtectVirtualMemory(ProcessHandle, &BaseAddress, &Size, NewProtect, &_OldProtect);
    if (NT_SUCCESS(Status) && OldProtect != NULL)
        *OldProtect = _OldProtect;

    return Status;
}

NTSTATUS
ProtectMemory(
    HANDLE      ProcessHandle,
    PVOID       BaseAddress,
    ULONG_PTR   Size,
    ULONG       NewProtect,
    PULONG      OldProtect
)
{
    return ProtectVirtualMemory(BaseAddress, Size, NewProtect, OldProtect, ProcessHandle);
}

NTSTATUS
ReadMemory(
    HANDLE      ProcessHandle,
    PVOID       BaseAddress,
    PVOID       Buffer,
    ULONG_PTR   Size,
    PULONG_PTR  BytesRead /* = NULL */
)
{
    return NtReadVirtualMemory(ProcessHandle, BaseAddress, Buffer, Size, BytesRead);
}

NTSTATUS
WriteMemory(
    HANDLE      ProcessHandle,
    PVOID       BaseAddress,
    PVOID       Buffer,
    ULONG_PTR   Size,
    PULONG_PTR  BytesWritten /* = NULL */
)
{
    return NtWriteVirtualMemory(ProcessHandle, BaseAddress, Buffer, Size, BytesWritten);
}

NTSTATUS
WriteProtectMemory(
    HANDLE      ProcessHandle,
    PVOID       BaseAddress,
    PVOID       Buffer,
    ULONG_PTR   Size,
    PULONG_PTR  BytesWritten /* = NULL */
)
{
    ULONG       Protect;
    NTSTATUS    Status;

    //Status = ProtectVirtualMemory(BaseAddress, Size, PAGE_WRITECOPY, &Protect, ProcessHandle);
    //FAIL_RETURN(Status);

    Status = ProtectVirtualMemory(BaseAddress, Size, PAGE_EXECUTE_READWRITE, &Protect, ProcessHandle);
    FAIL_RETURN(Status);

    Status = WriteMemory(ProcessHandle, BaseAddress, Buffer, Size, BytesWritten);

    if (Protect != PAGE_EXECUTE_READWRITE)
        ProtectVirtualMemory(BaseAddress, Size, Protect, NULL, ProcessHandle);

    return Status;
}

NTSTATUS QueryMappedImageName(HANDLE ProcessHandle, PVOID ImageBase, String& ImageName)
{
    NTSTATUS                            Status;
    ULONG_PTR                           Size;
    UNICODE_STRING                      DosPath;
    PMEMORY_MAPPED_FILENAME_INFORMATION MappedFileName;
    BYTE                                LocalBuffer[sizeof(*MappedFileName)];

    MappedFileName = (PMEMORY_MAPPED_FILENAME_INFORMATION)LocalBuffer;
    Size = sizeof(LocalBuffer);

    Status = NtQueryVirtualMemory(ProcessHandle, ImageBase, MemoryMappedFilenameInformation, MappedFileName, Size, &Size);
    if (Status != STATUS_BUFFER_OVERFLOW && Status != STATUS_INFO_LENGTH_MISMATCH)
        return Status;

    MappedFileName = (PMEMORY_MAPPED_FILENAME_INFORMATION)AllocStack(Size);
    Status = NtQueryVirtualMemory(ProcessHandle, ImageBase, MemoryMappedFilenameInformation, MappedFileName, Size, &Size);
    FAIL_RETURN(Status);

    Status = Io::QueryDosPathFromNtDeviceName(&DosPath, &MappedFileName->Name);
    FAIL_RETURN(Status);

    ImageName = DosPath;
    RtlFreeUnicodeString(&DosPath);

    return Status;
}

#endif // r3

ML_NAMESPACE_END_(Mm)


ML_NAMESPACE_END_(Native)

ML_NAMESPACE_END
