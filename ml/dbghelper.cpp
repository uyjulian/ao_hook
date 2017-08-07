#include "mylibrary.h"


#pragma warning(default:4750)

#if ML_SUPPORT_LAMBDA


ML_NAMESPACE
ML_NAMESPACE_BEGIN(DebugHelper);

static API_POINTER(RtlAllocateHeap)    StubRtlAllocateHeap;
static API_POINTER(RtlReAllocateHeap)  StubRtlReAllocateHeap;
static API_POINTER(RtlFreeHeap)        StubRtlFreeHeap;
static API_POINTER(RtlSizeHeap)        StubRtlSizeHeap;

BOOL IsCallerSelf(PVOID ReturnAddress)
{
    PLDR_MODULE Module;

    Module = FindLdrModuleByHandle(&__ImageBase);

    return IN_RANGE((ULONG_PTR)Module->DllBase, (ULONG_PTR)ReturnAddress, (ULONG_PTR)PtrAdd(Module->DllBase, Module->SizeOfImage));
}

static PVOID NTAPI HCH_RtlAllocateHeap(HANDLE HeapBase, ULONG Flags, SIZE_T Bytes)
{
    ULONG_PTR Size;
    PVOID Memory, Second;

    if (!IsCallerSelf(_ReturnAddress()))
        return StubRtlAllocateHeap(HeapBase, Flags, Bytes);

    Size = ROUND_UP(Bytes, MEMORY_PAGE_SIZE) + MEMORY_PAGE_SIZE * 2;

    Memory = NULL;
    Nt_AllocateMemory(NtCurrentProcess(), &Memory, Size, PAGE_EXECUTE_READWRITE, MEM_RESERVE);
    Nt_AllocateMemory(NtCurrentProcess(), &Memory, Size - MEMORY_PAGE_SIZE, PAGE_EXECUTE_READWRITE, MEM_COMMIT);

    Second = PtrAdd(Memory, Size - MEMORY_PAGE_SIZE);
    Nt_AllocateMemory(NtCurrentProcess(), &Second, MEMORY_PAGE_SIZE, PAGE_READONLY, MEM_COMMIT);

    Memory = PtrSub(Second, Bytes);
    *(PULONG_PTR)PtrSub(Memory, sizeof(PVOID)) = Bytes;

    return Memory;
}

static BOOL NTAPI HCH_RtlFreeHeap(HANDLE HeapBase, ULONG Flags, LPVOID Memory)
{
    if (!IsCallerSelf(_ReturnAddress()))
        return StubRtlFreeHeap(HeapBase, Flags, Memory);

    Memory = (PVOID)ROUND_DOWN((ULONG_PTR)Memory, MEMORY_PAGE_SIZE);
    Nt_FreeMemory(NtCurrentProcess(), PtrSub(Memory, MEMORY_PAGE_SIZE));
    return TRUE;
}

static PVOID NTAPI HCH_RtlReAllocateHeap(HANDLE HeapBase, ULONG Flags, PVOID Memory, SIZE_T Bytes)
{
    PVOID NewMemory;

    if (!IsCallerSelf(_ReturnAddress()))
        return StubRtlReAllocateHeap(HeapBase, Flags, Memory, Bytes);

    NewMemory = HCH_RtlAllocateHeap(HeapBase, Flags, Bytes);

    Bytes = MY_MIN(((PULONG_PTR)Memory)[-1], Bytes);
    CopyMemory(NewMemory, Memory, Bytes);

    HCH_RtlFreeHeap(HeapBase, Flags, Memory);

    return NewMemory;
}

static ULONG_PTR NTAPI HCH_RtlSizeHeap(HANDLE HeapBase, ULONG Flags, PVOID Memory)
{
    if (!IsCallerSelf(_ReturnAddress()))
        return StubRtlSizeHeap(HeapBase, Flags, Memory);

    return ((PULONG_PTR)Memory)[-1];
}

ML_NAMESPACE_END_(DebugHelper);
ML_NAMESPACE_END; // ml

NTSTATUS EnableHeapCorruptionHelper()
{
    Mp::PATCH_MEMORY_DATA p[] =
    {
        Mp::FunctionJumpVa(RtlAllocateHeap,   ml::DebugHelper::HCH_RtlAllocateHeap,    &ml::DebugHelper::StubRtlAllocateHeap),
        Mp::FunctionJumpVa(RtlReAllocateHeap, ml::DebugHelper::HCH_RtlReAllocateHeap,  &ml::DebugHelper::StubRtlReAllocateHeap),
        Mp::FunctionJumpVa(RtlFreeHeap,       ml::DebugHelper::HCH_RtlFreeHeap,        &ml::DebugHelper::StubRtlFreeHeap),
        Mp::FunctionJumpVa(RtlSizeHeap,       ml::DebugHelper::HCH_RtlSizeHeap,        &ml::DebugHelper::StubRtlSizeHeap),
    };

    return Mp::PatchMemory(p, countof(p));
}

NTSTATUS DisableHeapCorruptionHelper()
{
    Mp::RestoreMemory(ml::DebugHelper::StubRtlAllocateHeap);
    Mp::RestoreMemory(ml::DebugHelper::StubRtlReAllocateHeap);
    Mp::RestoreMemory(ml::DebugHelper::StubRtlFreeHeap);
    Mp::RestoreMemory(ml::DebugHelper::StubRtlSizeHeap);

    return 0;
}

#endif // ML_SUPPORT_LAMBDA
