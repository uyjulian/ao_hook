#ifndef _MEMORYALLOCATOR_H_bdd5f64b_fcd1_41d9_8f33_e4eeae67f3c8
#define _MEMORYALLOCATOR_H_bdd5f64b_fcd1_41d9_8f33_e4eeae67f3c8


#if CPP_DEFINED

#include <new>

#define MM_NOT_FREE     0x80000000

class NOVTABLE MemoryAllocator
{
protected:

#if ML_KERNEL_MODE

public:
    static const ULONG m_kMemTag = TAG4('MLKN');

#else

           HANDLE       m_hHeap;
    static HANDLE       m_hHeapGlobal;
    static ULONG_PTR    m_ObjectCount;

#endif  // MY_NT_DDK

protected:

#if !ML_KERNEL_MODE

    NoInline static HANDLE CreateGlobalHeapInternal(DWORD flOptions = 0, SIZE_T dwInitialSize = 0, SIZE_T dwMaximumSize = 0)
    {
        if (m_hHeapGlobal == NULL)
            m_hHeapGlobal = CreateHeapInternal(flOptions, dwInitialSize, dwMaximumSize);

        return m_hHeapGlobal;
    }

    ForceInline static ULONG_PTR AddRef()
    {
        return ++m_ObjectCount;
        //return Interlocked_Increment((PLong)&m_ObjectCount);
    }

    ForceInline static ULONG_PTR Release()
    {
        return --m_ObjectCount;
        //return Interlocked_Decrement((PLong)&m_ObjectCount);
    }

    NoInline Bool DestroyGlobal()
    {
        if (m_hHeap == NULL)
            return False;

        return DestroyGlobalHeap();
    }

    ForceInline Bool IsHeapPrivate()
    {
        return m_hHeap != NULL && m_hHeap != m_hHeapGlobal;
    }

#endif  // MY_NT_DDK

public:
    NoInline MemoryAllocator(HANDLE hHeap = NULL)
    {
#if ML_KERNEL_MODE
        UNREFERENCED_PARAMETER(hHeap);
#else
        if (hHeap != NULL)
        {
//            m_hHeapPrivate = hHeap;
        }
        else
        {
            if (AddRef() == 1)
            {
                hHeap = CreateGlobalHeapInternal();
                if (hHeap == NULL)
                    Release();
            }
            else
            {
                hHeap = m_hHeapGlobal;
            }

//            m_hHeapPrivate = NULL;
        }

        m_hHeap = hHeap;

#endif  // MY_NT_DDK
    }

    MemoryAllocator(const MemoryAllocator &mem)
    {
        *this = mem;
    }

    NoInline ~MemoryAllocator()
    {
#if !ML_KERNEL_MODE

        if (IsHeapPrivate())
        {
            if (IsNotProcessHeap())
                DestroyHeapInternal(m_hHeap);
        }
        else
        {
            DestroyGlobal();
        }

#endif  // MY_NT_DDK
    }

#if !ML_KERNEL_MODE

    HANDLE GetHeap() const
    {
        return m_hHeap;
    }

    static HANDLE CreateGlobalHeap(ULONG Options = 0)
    {
        HANDLE hHeap = CreateGlobalHeapInternal(Options);
        if (hHeap != NULL)
            AddRef();
        return hHeap;
    }

    static Bool DestroyGlobalHeap()
    {
        if (m_hHeapGlobal != NULL && Release() == 0)
        {
            if (DestroyHeapInternal(m_hHeapGlobal))
            {
                m_hHeapGlobal = NULL;
                return True;
            }
        }

        return False;
    }

    static HANDLE GetGlobalHeap()
    {
        return m_hHeapGlobal;
    }

    static PVOID GetAddressOfGlobalHeap()
    {
        return &m_hHeapGlobal;
    }

    static PVOID AllocateMemory(ULONG_PTR Size, ULONG Flags = 0)
    {
        return AllocateHeapInternal(GetGlobalHeap(), Flags, Size);
    }

    static PVOID ReAllocateMemory(PVOID Memory, ULONG_PTR Size, ULONG Flags = 0)
    {
        PVOID Block = ReAllocateHeapInternal(GetGlobalHeap(), Flags, Memory, Size);

        if (Block == NULL && Memory != NULL)
        {
            FreeMemory(Memory);
        }

        return Block;
    }

    static BOOL FreeMemory(PVOID Memory, ULONG Flags = 0)
    {
        return FreeHeapInternal(GetGlobalHeap(), Flags, Memory);
    }

    HANDLE CreateHeap(DWORD flOptions = 0, SIZE_T dwInitialSize = 0, SIZE_T dwMaximumSize = 0)
    {
        if (IsHeapPrivate())
            DestroyHeapInternal(m_hHeap);
        else
            DestroyGlobal();

        m_hHeap = CreateHeapInternal(flOptions, dwInitialSize, dwMaximumSize);

        return m_hHeap;
    }

    Bool DestroyHeap()
    {
        Bool Result = True;

        if (IsHeapPrivate() && IsNotProcessHeap())
        {
            Result = DestroyHeapInternal(m_hHeap);
            if (Result)
                m_hHeap = NULL;
        }

        return Result;
    }

#else // r0

    static POOL_TYPE GetGlobalHeap()
    {
        return NonPagedPool;
    }

    static PVOID AllocateMemory(ULONG_PTR Size, POOL_TYPE PoolType = NonPagedPool, ULONG Tag = m_kMemTag)
    {
        return AllocateHeapInternal(Size, PoolType, Tag);
    }

    static BOOL FreeMemory(PVOID Memory, ULONG Tag = m_kMemTag)
    {
        return FreeHeapInternal(Memory, Tag);
    }

#endif  // ML_KERNEL_MODE

#if ML_KERNEL_MODE

    PVoid Alloc(SizeT Size, POOL_TYPE PoolType = NonPagedPool, ULONG Tag = m_kMemTag)
    {
        return AllocateHeapInternal(Size, PoolType, Tag);
    }

    Bool Free(PVoid pBuffer, ULONG Tag = m_kMemTag)
    {
        return FreeHeapInternal(pBuffer, Tag);
    }

    Bool SafeFree(PVoid pBuffer, ULONG Tag = m_kMemTag)
    {
        LPVoid **pt = (LPVoid **)pBuffer;
        if (*pt == NULL)
            return False;

        FreeHeapInternal(*pt, Tag);
        *pt = NULL;

        return True;
    }

#else

    PVoid Alloc(SizeT Size, ULONG_PTR Flags = 0)
    {
        return AllocateHeapInternal(m_hHeap, Flags, Size);
    }

    PVoid ReAlloc(PVoid pBuffer, SizeT Size, ULONG_PTR Flags = 0)
    {
        PVoid pRealloc;

        if (pBuffer == NULL)
            return Alloc(Size, Flags);

        pRealloc = ReAllocateHeapInternal(m_hHeap, Flags, pBuffer, Size);
        if (!FLAG_ON(Flags, MM_NOT_FREE))
        {
            if (pRealloc == NULL)
            {
                Free(pBuffer);
            }
        }

        return pRealloc;
    }

    Bool Free(PVoid pBuffer, ULONG_PTR Flags = 0)
    {
        return pBuffer == NULL ? False : FreeHeapInternal(m_hHeap, Flags, pBuffer);
    }

    Bool SafeFree(LPVoid pBuffer, ULONG_PTR Flags = 0)
    {
        LPVoid **pt = (LPVoid **)pBuffer;
        if (*pt == NULL)
            return False;

        Bool Result = FreeHeapInternal(m_hHeap, Flags, *pt);
        if (Result)
            *pt = NULL;

        return Result;
    }

#endif

private:

#if !ML_KERNEL_MODE

#if USE_NT_VER

    BOOL IsNotProcessHeap()
    {
        return m_hHeap != Nt_CurrentPeb()->ProcessHeap;
    }

    static HANDLE CreateHeapInternal(ULONG Flags = 0, SIZE_T CommitSize = 0, SIZE_T ReserveSize = 0)
    {
        if (Flags == 0)
            Flags = HEAP_GROWABLE;
        return RtlCreateHeap(Flags, NULL, ReserveSize, CommitSize, NULL, NULL);
    }

    static BOOL DestroyHeapInternal(HANDLE hHeap)
    {
        return !RtlDestroyHeap(hHeap);
    }

#else

    BOOL IsNotProcessHeap()
    {
        return m_hHeap != GetProcessHeap();
    }

    static HANDLE CreateHeapInternal(ULONG Flags = 0, SIZE_T CommitSize = 0, SIZE_T ReserveSize = 0)
    {
        return HeapCreate(Flags, CommitSize, ReserveSize);
    }

    static BOOL DestroyHeapInternal(HANDLE hHeap)
    {
        return HeapDestroy(hHeap);
    }

#endif

#endif  // MY_NT_DDK

    static LONG_PTR ModifyAllocCount(LONG_PTR Increment)
    {
        static LONG_PTR AllocCount;

        AllocCount += Increment;
        return AllocCount;
    }

#if ML_KERNEL_MODE

    static PVOID AllocateHeapInternal(SIZE_T Size, POOL_TYPE PoolType = NonPagedPool, ULONG Tag = m_kMemTag)
    {
        PVOID Memory = ExAllocatePoolWithTag(PoolType, Size, Tag);

    #if ML_MEMORY_DEBUG

        ModifyAllocCount(Memory != nullptr);

    #endif

        return Memory;
    }

    static BOOL FreeHeapInternal(PVOID Memory, ULONG Tag = m_kMemTag)
    {
        if (Memory == NULL)
            return TRUE;

    #if ML_MEMORY_DEBUG

        ModifyAllocCount(-1);

    #endif

        ExFreePoolWithTag(Memory, Tag);

        return TRUE;
    }

#elif USE_NT_VER

    static PVOID AllocateHeapInternal(HANDLE Heap, ULONG_PTR Flags, SIZE_T Size)
    {
        PVOID Memory = RtlAllocateHeap(Heap, (ULONG)Flags, Size);

    #if ML_MEMORY_DEBUG

        ModifyAllocCount(Memory != nullptr);

    #endif

        return Memory;
    }

    static PVOID ReAllocateHeapInternal(HANDLE Heap, ULONG_PTR Flags, PVOID Memory, SIZE_T Size)
    {
        return Memory == NULL ? AllocateHeapInternal(Heap, Flags, Size) : RtlReAllocateHeap(Heap, (ULONG)Flags, Memory, Size);
    }

    static BOOL FreeHeapInternal(HANDLE Heap, ULONG_PTR Flags, PVOID Memory)
    {
        if (Memory == NULL)
            return TRUE;

    #if ML_MEMORY_DEBUG

        ModifyAllocCount(-1);

    #endif

        return RtlFreeHeap(Heap, (ULONG)Flags, Memory);
    }

#elif 0
    static PVOID AllocateHeapInternal(HANDLE hHeap, ULONG Flags, SIZE_T Size)
    {
        return HeapAlloc(hHeap, Flags, Size);
    }

    static PVOID ReAllocateHeapInternal(HANDLE hHeap, ULONG Flags, PVOID pvMemory, SIZE_T Size)
    {
        return pvMemory == NULL ? AllocateHeapInternal(hHeap, Flags, Size) : HeapReAlloc(hHeap, Flags, pvMemory, Size);
    }

    static BOOL FreeHeapInternal(HANDLE hHeap, ULONG Flags, PVOID pvMemory)
    {
        if (pvMemory == NULL)
            return TRUE;

        return HeapFree(hHeap, Flags, pvMemory);
    }

#endif  // MY_NT_DDK
};

#if !ML_KERNEL_MODE

DECL_SELECTANY HANDLE       MemoryAllocator::m_hHeapGlobal = NULL;
DECL_SELECTANY ULONG_PTR    MemoryAllocator::m_ObjectCount = 0;

#endif // ML_KERNEL_MODE

// typedef MemoryAllocator CMem;

#if ML_KERNEL_MODE

template<class Object, POOL_TYPE PoolType = NonPagedPool>
class FixedMemoryBlock
{
protected:
    union
    {
        NPAGED_LOOKASIDE_LIST   NonPaged;
        PAGED_LOOKASIDE_LIST    Paged;
    } LookasideList;

public:
    static const ULONG_PTR BlockSize = sizeof(Object);

public:
    NTSTATUS Initialize(
        PALLOCATE_FUNCTION  Allocate    = NULL,
        PFREE_FUNCTION      Free        = NULL,
        ULONG               Flags       = 0,
        ULONG               Tag         = MemoryAllocator::m_kMemTag,
        USHORT              Depth       = 0
    )
    {
        switch (PoolType)
        {
            case NonPagedPool:
                ExInitializeNPagedLookasideList(&LookasideList.NonPaged, Allocate, Free, Flags, BlockSize, Tag, Depth);
                break;

            case PagedPool:
                ExInitializePagedLookasideList(&LookasideList.Paged, Allocate, Free, Flags, BlockSize, Tag, Depth);
                break;

            default:
                return STATUS_INVALID_PARAMETER;
        }

        return STATUS_SUCCESS;
    }

    NTSTATUS UnInitialize()
    {
        switch (PoolType)
        {
            case NonPagedPool:
                ExDeleteNPagedLookasideList(&LookasideList.NonPaged);
                break;

            case PagedPool:
                ExDeletePagedLookasideList(&LookasideList.Paged);
                break;
        }

        return STATUS_SUCCESS;
    }

    Object* Allocate()
    {
        PVOID Block;

        switch (PoolType)
        {
            case NonPagedPool:
                Block = ExAllocateFromNPagedLookasideList(&LookasideList.NonPaged);
                break;

            case PagedPool:
                Block = ExAllocateFromPagedLookasideList(&LookasideList.Paged);
                break;

            default:
                Block = NULL;
                break;
        }

        if (Block != NULL)
            new (Block) Object;

        return (Object *)Block;
    }

    VOID Free(PVOID Block)
    {
        if (Block == NULL)
            return;

        switch (PoolType)
        {
            case NonPagedPool:
                ((Object *)Block)->~Object();
                ExFreeToNPagedLookasideList(&LookasideList.NonPaged, Block);
                break;

            case PagedPool:
                ((Object *)Block)->~Object();
                ExFreeToPagedLookasideList(&LookasideList.Paged, Block);
                break;
        }
    }
};

#endif // r0

#endif // CPP_DEFINED

_ML_C_HEAD_

BOOL
FreeMemoryP(
    PVOID Memory,
    ULONG DEF_VAL(Flags, 0)
);

#if ML_KERNEL_MODE

PVOID
AllocateMemoryP(
    ULONG_PTR Size,
    POOL_TYPE DEF_VAL(PoolType, NonPagedPool)
);

PVOID
AllocateMemory(
    ULONG_PTR Size,
    POOL_TYPE DEF_VAL(PoolType, NonPagedPool)
);

BOOL
FreeMemory(
    PVOID Memory,
    ULONG Flags = 0
);

#else // user mode

PVOID
AllocateMemoryP(
    ULONG_PTR   Size,
    ULONG       DEF_VAL(Flags, 0)
);

PVOID
ReAllocateMemoryP(
    PVOID       Memory,
    ULONG_PTR   Size,
    ULONG       DEF_VAL(Flags, 0)
);

PVOID
AllocateMemory(
    ULONG_PTR   Size,
    ULONG       DEF_VAL(Flags, 0)
);

PVOID
ReAllocateMemory(
    PVOID       Memory,
    ULONG_PTR   Size,
    ULONG       DEF_VAL(Flags, 0)
);

BOOL
FreeMemory(
    PVOID Memory,
    ULONG DEF_VAL(Flags, 0)
);

PVOID
AllocateVirtualMemory(
    ULONG_PTR   Size,
    ULONG       DEF_VAL(Protect, PAGE_EXECUTE_READWRITE),
    HANDLE      DEF_VAL(ProcessHandle, NtCurrentProcess())
);

BOOL
FreeVirtualMemory(
    PVOID   Memory,
    HANDLE  DEF_VAL(ProcessHandle, NtCurrentProcess())
);

#endif // ML_KERNEL_MODE

_ML_C_TAIL_

#endif // _MEMORYALLOCATOR_H_bdd5f64b_fcd1_41d9_8f33_e4eeae67f3c8