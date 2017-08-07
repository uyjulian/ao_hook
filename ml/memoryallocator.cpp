#include "mylibrary.h"
_ML_C_HEAD_

#if ML_KERNEL_MODE

PVOID AllocateMemory(ULONG_PTR Size, POOL_TYPE PoolType /* = PagedPool */)
{
    return MemoryAllocator::AllocateMemory(Size, PoolType);
}

PVOID AllocateMemoryP(ULONG_PTR Size, POOL_TYPE PoolType)
{
    return AllocateMemory(Size, PoolType);
}

BOOL FreeMemory(PVOID Memory, ULONG Flags)
{
    return Memory != NULL ? MemoryAllocator::FreeMemory(Memory) : FALSE;
}

BOOL FreeMemoryP(PVOID Memory, ULONG Flags)
{
    return FreeMemory(Memory, Flags);
}

#else // user mode

PVOID AllocateMemoryP(ULONG_PTR Size, ULONG Flags)
{
    return MemoryAllocator::AllocateMemory(Size, Flags);
}

PVOID ReAllocateMemoryP(PVOID Memory, ULONG_PTR Size, ULONG Flags)
{
    return MemoryAllocator::ReAllocateMemory(Memory, Size, Flags);
}

PVOID AllocateMemory(ULONG_PTR Size, ULONG Flags)
{
#if USE_CRT_VER
    return malloc(Size);
#elif USE_NT_VER
    return RtlAllocateHeap(CurrentPeb()->ProcessHeap, Flags, Size);
#else
    return HeapAlloc(GetProcessHeap(), Flags, Size);
#endif
}

PVOID ReAllocateMemory(PVOID Memory, ULONG_PTR Size, ULONG Flags)
{
    PVOID Block;

    if (Memory == NULL)
        return AllocateMemory(Size, Flags);

#if USE_CRT_VER
    Block = realloc(Memory, Size);
#elif USE_NT_VER
    Block = RtlReAllocateHeap(CurrentPeb()->ProcessHeap, Flags, Memory, Size);
#else
    Block = HeapReAlloc(GetProcessHeap(), Flags, Memory, Size);
#endif

    if (Block == nullptr)
    {
        FreeMemory(Memory);
        return nullptr;
    }

    return Block;
}

BOOL FreeMemory(PVOID Memory, ULONG Flags)
{
#if USE_CRT_VER
    free(Memory);
    return TRUE;
#elif USE_NT_VER
    return Memory != NULL ? RtlFreeHeap(CurrentPeb()->ProcessHeap, Flags, Memory) : FALSE;
#else
    return Memory != NULL ? HeapFree(GetProcessHeap(), Flags, Memory) : FALSE;
#endif
}

BOOL FreeMemoryP(PVOID Memory, ULONG Flags)
{
    return MemoryAllocator::FreeMemory(Memory, Flags);
}

PVOID AllocateVirtualMemory(ULONG_PTR Size, ULONG Protect, HANDLE ProcessHandle)
{
#if USE_NT_VER
    PVOID BaseAddress = NULL;
    NTSTATUS Status;
    Status = Nt_AllocateMemory(ProcessHandle, &BaseAddress, Size, Protect);
    return NT_SUCCESS(Status) ? BaseAddress : NULL;
#else
    return VirtualAllocEx(ProcessHandle, NULL, Size, MEM_RESERVE | MEM_COMMIT, Protect);
#endif
}

BOOL FreeVirtualMemory(PVOID Memory, HANDLE ProcessHandle)
{
#if USE_NT_VER
    return NT_SUCCESS(Nt_FreeMemory(ProcessHandle, Memory));
#else
    return VirtualFreeEx(ProcessHandle, Memory, 0, MEM_RELEASE);
#endif
}

#endif  // ML_KERNEL_MODE


_ML_C_TAIL_


