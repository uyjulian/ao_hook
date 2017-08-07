#ifndef _MLNTMEMORY_H_3b0738ba_74a9_48c5_9804_cd1307c21092_
#define _MLNTMEMORY_H_3b0738ba_74a9_48c5_9804_cd1307c21092_


ML_NAMESPACE_BEGIN(Mm);

#define SEC_IMAGE         0x1000000

NTSTATUS
AllocVirtualMemory(
    PVOID*      BaseAddress,
    ULONG_PTR   Size,
    ULONG       Protect         = PAGE_EXECUTE_READWRITE,
    ULONG       AllocationType  = MEM_RESERVE | MEM_COMMIT,
    HANDLE      ProcessHandle   = Ps::CurrentProcess
);

NTSTATUS
AllocVirtualMemoryEx(
    HANDLE      ProcessHandle,
    PVOID*      BaseAddress,
    ULONG_PTR   Size,
    ULONG       Protect         = PAGE_EXECUTE_READWRITE,
    ULONG       AllocationType  = MEM_RESERVE | MEM_COMMIT
);

NTSTATUS
ProtectVirtualMemory(
    PVOID       BaseAddress,
    ULONG_PTR   Size,
    ULONG       NewProtect,
    PULONG      OldProtect      = nullptr,
    HANDLE      ProcessHandle   = Ps::CurrentProcess
);

NTSTATUS
ProtectMemory(
    HANDLE      ProcessHandle,
    PVOID       BaseAddress,
    ULONG_PTR   Size,
    ULONG       NewProtect,
    PULONG      OldProtect = nullptr
);

NTSTATUS
FreeVirtualMemory(
    PVOID   BaseAddress,
    HANDLE  ProcessHandle   = Ps::CurrentProcess,
    ULONG   FreeType        = MEM_RELEASE
);

NTSTATUS
ReadMemory(
    HANDLE      ProcessHandle,
    PVOID       BaseAddress,
    PVOID       Buffer,
    ULONG_PTR   Size,
    PULONG_PTR  BytesRead = nullptr
);

NTSTATUS
WriteMemory(
    HANDLE      ProcessHandle,
    PVOID       BaseAddress,
    PVOID       Buffer,
    ULONG_PTR   Size,
    PULONG_PTR  BytesWritten = nullptr
);

NTSTATUS
WriteProtectMemory(
    HANDLE      ProcessHandle,
    PVOID       BaseAddress,
    PVOID       Buffer,
    ULONG_PTR   Size,
    PULONG_PTR  BytesWritten = nullptr
);

NTSTATUS
QueryMappedImageName(
    HANDLE  ProcessHandle,
    PVOID   ImageBase,
    String& ImageName
);

ML_NAMESPACE_END_(Mm);

#endif // _MLNTMEMORY_H_3b0738ba_74a9_48c5_9804_cd1307c21092_