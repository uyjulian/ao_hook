#include "mylibrary.h"


NTSTATUS
Nt_AllocateMemory(
    HANDLE      ProcessHandle,
    PVOID*      BaseAddress,
    ULONG_PTR   Size,
    ULONG       Protect,        /* = PAGE_EXECUTE_READWRITE */
    ULONG       AllocationType  /* = MEM_RESERVE | MEM_COMMIT */
)
{
    return ml::Native::Mm::AllocVirtualMemory(BaseAddress, Size, Protect, AllocationType, ProcessHandle);
}

NTSTATUS
Nt_FreeMemory(
    HANDLE  ProcessHandle,
    PVOID   BaseAddress
)
{
    return ml::Native::Mm::FreeVirtualMemory(BaseAddress, ProcessHandle);
}

