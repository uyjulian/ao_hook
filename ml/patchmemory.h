#ifndef _PATCHMEMORY_H_99edde6e_777d_4444_9611_1ae8abbe5e02_
#define _PATCHMEMORY_H_99edde6e_777d_4444_9611_1ae8abbe5e02_


#if CPP_DEFINED

#define NOP  0x90
#define CALL 0xE8
#define JUMP 0xE9
#define PUSH 0x68
#define REXW 0x49

#pragma push_macro("MP_INLINE")

#define MP_INLINE ForceInline
#define MP_CALL FASTCALL


ML_NAMESPACE_BEGIN(Mp);

#if ML_X86
    #define TRAMPOLINE_SIZE     0x40
#elif ML_AMD64
    #define TRAMPOLINE_SIZE     0x100
#endif

ML_NAMESPACE_BEGIN(PatchMemoryTypes);

enum
{
    MemoryPatch,
    FunctionPatch,
};

ML_NAMESPACE_END_(PatchMemoryTypes);

enum
{
    VirtualAddress      = 0x00000001,

    // patch
    BackupData          = 0x00000002,
    DataIsBuffer        = 0x00000004,

    // function
    DoNotDisassemble    = 0x00000002,
    NakedTrampoline     = 0x00000004,
    KeepRawTrampoline   = 0x00000008,
    ExecuteTrampoline   = 0x00000010,

    OpMask              = 0xF0000000,
    OpJump              = 0x00000000,
    OpCall              = 0x10000000,
    OpPush              = 0x20000000,
    OpJRax              = 0x30000000,
    OpJRcx              = 0x40000000,
    OpJRdx              = 0x50000000,
    OpJRbx              = 0x60000000,
    OpJRbp              = 0x70000000,
    OpJRsi              = 0x80000000,
    OpJRdi              = 0x90000000,
    OpJR10              = 0xA0000000,
};

typedef struct
{

#if ML_X86

    ULONG_PTR EFlags;
    ULONG_PTR Rdi;
    ULONG_PTR Rsi;
    ULONG_PTR Rbp;
    ULONG_PTR Rbx;
    ULONG_PTR Rdx;
    ULONG_PTR Rcx;
    ULONG_PTR Rax;
    ULONG_PTR Rsp;

#elif ML_AMD64

    ULONG_PTR EFlags;
    ULONG_PTR Rax;
    ULONG_PTR Rcx;
    ULONG_PTR Rdx;
    ULONG_PTR Rbx;
    //ULONG_PTR Rsp;
    ULONG_PTR Rbp;
    ULONG_PTR Rsi;
    ULONG_PTR Rdi;

    ULONG_PTR R8;
    ULONG_PTR R9;
    ULONG_PTR R10;
    ULONG_PTR R11;
    ULONG_PTR R12;
    ULONG_PTR R13;
    ULONG_PTR R14;
    ULONG_PTR R15;

#endif // arch

    ULONG_PTR ReturnAddress;

#if ML_X86

    PVOID GetArgument(LONG_PTR Index)
    {
        return *(PVOID *)(this->Rsp + (Index + 1) * sizeof(this->Rsp));
    }

    template<class T>
    VOID SetArgument(LONG_PTR Index, T Value)
    {
        *(T *)(this->Rsp + (Index + 1) * sizeof(this->Rsp)) = Value;;
    }

#endif

} TRAMPOLINE_NAKED_CONTEXT, *PTRAMPOLINE_NAKED_CONTEXT;

typedef struct
{
    ULONG_PTR PatchType;

    union
    {
        struct
        {
            union
            {
                ULONG Flags;
                struct
                {
                    BOOLEAN VirtualAddress      : 1;
                    BOOLEAN BackupData          : 1;
                    BOOLEAN DataIsBuffer        : 1;
                };
            } Options;

            ULONG64     Data;
            ULONG_PTR   Size;
            ULONG_PTR   Address;
            ULONG64     Backup;

        } Memory;

        struct
        {
            union
            {
                ULONG Flags;
                struct
                {
                    BOOLEAN VirtualAddress      : 1;
                    BOOLEAN DoNotDisassemble    : 1;
                    BOOLEAN NakedTrampoline     : 1;
                    BOOLEAN KeepRawTrampoline   : 1;
                    BOOLEAN ExecuteTrampoline   : 1;
                };
            } Options;

            ULONG_PTR   HookOp;
            ULONG_PTR   Source;
            PVOID       Target;
            PVOID*      Trampoline;
            ULONG_PTR   NopBytes;

        } Function;
    };

} PATCH_MEMORY_DATA, *PPATCH_MEMORY_DATA;

typedef struct _TRAMPOLINE_DATA
{
    BYTE                Trampoline[TRAMPOLINE_SIZE];
    BYTE                OriginalCode[TRAMPOLINE_SIZE];
    ULONG               TrampolineSize;
    ULONG               OriginSize;
    PVOID               JumpBackAddress;
    PATCH_MEMORY_DATA   PatchData;

} TRAMPOLINE_DATA, *PTRAMPOLINE_DATA;



template<class TYPE>
MP_INLINE PVOID __AnyToPtr__(const TYPE &Val)
{
    union
    {
        TYPE Val;
        PVOID Ptr;
    } u;

    u.Ptr = nullptr;
    u.Val = Val;

    return u.Ptr;
}

/************************************************************************
  MemoryPatch
************************************************************************/

MP_INLINE PATCH_MEMORY_DATA MemoryPatch(ULONG64 Data, ULONG_PTR Size, ULONG_PTR Address, ULONG Flags)
{
    PATCH_MEMORY_DATA PatchData;

    PatchData.PatchType = PatchMemoryTypes::MemoryPatch;
    PatchData.Memory.Options.Flags = Flags;

    PatchData.Memory.Data       = Data;
    PatchData.Memory.Size       = Size;
    PatchData.Memory.Address    = Address;

    PatchData.Memory.Backup = 0;

    return PatchData;
}

MP_INLINE PATCH_MEMORY_DATA MemoryPatchRva(ULONG64 Data, ULONG_PTR Size, ULONG_PTR RVA, ULONG Flags = 0)
{
    return MemoryPatch((ULONG64)Data, Size, (ULONG_PTR)RVA, Flags);
}

MP_INLINE PATCH_MEMORY_DATA MemoryPatchRva(PVOID Data, ULONG_PTR Size, ULONG_PTR RVA, ULONG Flags = 0)
{
    return MemoryPatch((ULONG64)Data, Size, (ULONG_PTR)RVA, Flags | DataIsBuffer);
}

template<class VA_TYPE>
MP_INLINE PATCH_MEMORY_DATA MemoryPatchVa(ULONG64 Data, ULONG_PTR Size, VA_TYPE Address, ULONG Flags = 0)
{
    return MemoryPatch((ULONG64)Data, Size, (ULONG_PTR)__AnyToPtr__(Address), Flags | VirtualAddress);
}

template<class VA_TYPE>
MP_INLINE PATCH_MEMORY_DATA MemoryPatchVa(PVOID Data, ULONG_PTR Size, VA_TYPE Address, ULONG Flags = 0)
{
    return MemoryPatchVa((ULONG64)Data, Size, Address, Flags | VirtualAddress | DataIsBuffer);
}

/************************************************************************
  FunctionPatch
************************************************************************/

MP_INLINE PATCH_MEMORY_DATA FunctionPatch(PVOID Source, PVOID Target, PVOID Trampoline, ULONG Flags)
{
    PATCH_MEMORY_DATA PatchData;

    PatchData.PatchType                 = PatchMemoryTypes::FunctionPatch;
    PatchData.Function.Options.Flags    = Flags;
    PatchData.Function.HookOp           = Flags & OpMask;

    PatchData.Function.Source           = (ULONG_PTR)Source;
    PatchData.Function.Target           = Target;
    PatchData.Function.Trampoline       = (PVOID *)Trampoline;
    PatchData.Function.NopBytes         = 0;

    return PatchData;
}

template<class SOURCE_TYPE, class TARGET_TYPE>
MP_INLINE PATCH_MEMORY_DATA FunctionJumpVa(SOURCE_TYPE Source, TARGET_TYPE Target, PVOID Trampoline = nullptr, ULONG Flags = OpJump)
{
    return FunctionPatch(__AnyToPtr__(Source), __AnyToPtr__((SOURCE_TYPE)Target), Trampoline, Flags | VirtualAddress);
}

template<class TARGET_TYPE>
MP_INLINE PATCH_MEMORY_DATA FunctionJumpRva(ULONG_PTR SourceRva, TARGET_TYPE Target, PVOID Trampoline = nullptr, ULONG Flags = OpJump)
{
    return FunctionPatch((PVOID)SourceRva, __AnyToPtr__(Target), Trampoline, Flags);
}

template<class SOURCE_TYPE, class TARGET_TYPE>
MP_INLINE PATCH_MEMORY_DATA FunctionCallVa(SOURCE_TYPE Source, TARGET_TYPE Target, PVOID Trampoline = nullptr, ULONG Flags = OpCall)
{
    return FunctionPatch(__AnyToPtr__(Source), __AnyToPtr__((SOURCE_TYPE)Target), Trampoline, Flags | VirtualAddress);
}

template<class TARGET_TYPE>
MP_INLINE PATCH_MEMORY_DATA FunctionCallRva(ULONG_PTR SourceRva, TARGET_TYPE Target, PVOID Trampoline = nullptr, ULONG Flags = OpCall)
{
    return FunctionPatch((PVOID)SourceRva, __AnyToPtr__(Target), Trampoline, Flags);
}


NTSTATUS
MP_CALL
PatchMemory(
    PPATCH_MEMORY_DATA  PatchData,
    ULONG_PTR           PatchCount,
    PVOID               BaseAddress = nullptr
);

NTSTATUS
MP_CALL
RestoreMemory(
    PTRAMPOLINE_DATA TrampolineData
);

template<class TRAMPOLINE_PTR> inline NTSTATUS RestoreMemory(TRAMPOLINE_PTR& Trampoline)
{
    NTSTATUS Status;

    if (Trampoline == nullptr)
        return STATUS_SUCCESS;

    Status = RestoreMemory(FIELD_BASE(Trampoline, TRAMPOLINE_DATA, Trampoline));
    if (NT_SUCCESS(Status))
        Trampoline = nullptr;

    return Status;
}

inline NTSTATUS RestoreMemory(PPATCH_MEMORY_DATA PatchData, ULONG_PTR PatchCount)
{
    FOR_EACH(PatchData, PatchData, PatchCount)
    {
        switch (PatchData->PatchType)
        {
            case PatchMemoryTypes::MemoryPatch:
                break;

            case PatchMemoryTypes::FunctionPatch:
                if (PatchData->Function.Trampoline != nullptr)
                    RestoreMemory(*PatchData->Function.Trampoline);
                break;
        }
    }

    return STATUS_SUCCESS;
}

ML_NAMESPACE_END_(Mp);

/************************************************************************
  helper function
************************************************************************/

template<class PtrType>
MP_INLINE
PtrType GetCallDestination(PtrType Buffer)
{
    union
    {
        PtrType     Pointer;
        ULONG_PTR   Value;
    };

    Pointer = Buffer;
    Value   = *(PLONG)(Value + 1) + Value + sizeof(ULONG) + 1;

    return Pointer;
}

NTSTATUS
CopyOneOpCode(
    PVOID       Target,
    PVOID       Source,
    PULONG_PTR  DestinationOpLength,
    PULONG_PTR  SourceOpLength,
    ULONG_PTR   ForwardSize,
    ULONG_PTR   BackwardSize,
    PVOID       TargetIp = IMAGE_INVALID_VA
);

MP_INLINE ULONG_PTR GetOpCodeSize32(PVOID Buffer)
{
    return LdeGetOpCodeSize32(Buffer);
}

MP_INLINE ULONG_PTR GetOpCodeSize64(PVOID Buffer)
{
    return LdeGetOpCodeSize64(Buffer);
}

MP_INLINE ULONG_PTR GetOpCodeSize(PVOID Buffer)
{
#if ML_AMD64
    return GetOpCodeSize64(Buffer);
#elif ML_X86
    return GetOpCodeSize32(Buffer);
#endif
}

//#pragma pop_macro("MP_CALL")
#pragma pop_macro("MP_INLINE")

#endif // cpp


#endif // _PATCHMEMORY_H_99edde6e_777d_4444_9611_1ae8abbe5e02_