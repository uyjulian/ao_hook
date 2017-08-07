#ifndef _MISC_H_093f7fa1_751a_4627_83f8_df8509b9926e
#define _MISC_H_093f7fa1_751a_4627_83f8_df8509b9926e


_ML_C_HEAD_

#pragma pack(1)

#define SHA224_DIGEST_SIZE ( 224 / 8)
#define SHA256_DIGEST_SIZE ( 256 / 8)
#define SHA384_DIGEST_SIZE ( 384 / 8)
#define SHA512_DIGEST_SIZE ( 512 / 8)

#define SHA256_BLOCK_SIZE  ( 512 / 8)
#define SHA512_BLOCK_SIZE  (1024 / 8)
#define SHA384_BLOCK_SIZE  SHA512_BLOCK_SIZE
#define SHA224_BLOCK_SIZE  SHA256_BLOCK_SIZE

typedef struct
{
    ULONG   tot_len;
    ULONG   len;
    BYTE    block[2 * SHA256_BLOCK_SIZE];
    ULONG   h[8];
} sha256_ctx;

#pragma pack()

VOID STDCALL sha256_init(sha256_ctx *ctx);
VOID STDCALL sha256_update(sha256_ctx *ctx, PVOID message, ULONG len);
VOID STDCALL sha256_final(sha256_ctx *ctx, PVOID digest);
VOID STDCALL sha256(PVOID message, ULONG len, PVOID digest);

inline VOID sha256to128(PVOID message, ULONG len, PVOID digest)
{
    ULONG Hash[8];

    sha256(message, len, Hash);
    ((PULONG)digest)[0] = Hash[0] ^ Hash[4];
    ((PULONG)digest)[1] = Hash[1] ^ Hash[5];
    ((PULONG)digest)[2] = Hash[2] ^ Hash[6];
    ((PULONG)digest)[3] = Hash[3] ^ Hash[7];
}

#if !defined(ReadTimeStampCounter)

ForceInline UInt64 ReadTimeStampCounter()
{
    __asm rdtsc;
}

#endif // ReadTimeStampCounter

ForceInline UInt64 GetRandom64()
{
    return ReadTimeStampCounter();
}

ForceInline UInt32 GetRandom32()
{
    return (UInt32)GetRandom64();
}

#if CPP_DEFINED

namespace PointerOperationType
{
    enum PointerOperationClass
    {
        POINTER_OP_ADD,
        POINTER_OP_SUB,
        POINTER_OP_AND,
        POINTER_OP_OR,
        POINTER_OP_XOR,
        POINTER_OP_MOD,
    };
}

EXTCPP
template<PointerOperationType::PointerOperationClass OP, class PtrType1, class PtrType2>
ForceInline
PtrType1 PtrOperator(PtrType1 Ptr1, PtrType2 Ptr2)
{
#pragma warning(push, 0)
    struct
    {
        union
        {
            PtrType1        _Ptr1;
            LARGE_INTEGER   Value1;
        };

        union
        {
            PtrType2        _Ptr2;
            LARGE_INTEGER   Value2;
        };
    } u;

#pragma warning(push)
#pragma warning(disable:4702)

    if (MY_MAX(sizeof(Ptr1), sizeof(Ptr2)) == sizeof(u.Value1.QuadPart))
    {
        u.Value1.QuadPart = 0;
        u.Value2.QuadPart = 0;
    }
    else
    {
        u.Value1.LowPart = NULL;
        u.Value2.LowPart = NULL;
    }

#pragma warning(pop)

    u._Ptr1 = Ptr1;
    u._Ptr2 = Ptr2;

    if (MY_MAX(sizeof(Ptr1), sizeof(Ptr2)) == sizeof(u.Value1.QuadPart))
    {
        switch (OP)
        {
            case PointerOperationType::POINTER_OP_ADD:
                u.Value1.QuadPart += u.Value2.QuadPart;
                break;

            case PointerOperationType::POINTER_OP_SUB:
                u.Value1.QuadPart -= u.Value2.QuadPart;
                break;

            case PointerOperationType::POINTER_OP_AND:
                u.Value1.QuadPart &= u.Value2.QuadPart;
                break;

            case PointerOperationType::POINTER_OP_OR:
                u.Value1.QuadPart |= u.Value2.QuadPart;
                break;

            case PointerOperationType::POINTER_OP_XOR:
                u.Value1.QuadPart ^= u.Value2.QuadPart;
                break;

            case PointerOperationType::POINTER_OP_MOD:
                u.Value1.QuadPart %= u.Value2.QuadPart;
                break;
        }
    }
    else
    {
        switch (OP)
        {
            case PointerOperationType::POINTER_OP_ADD:
                u.Value1.LowPart += u.Value2.LowPart;
                break;

            case PointerOperationType::POINTER_OP_SUB:
                u.Value1.LowPart -= u.Value2.LowPart;
                break;

            case PointerOperationType::POINTER_OP_AND:
                u.Value1.LowPart &= u.Value2.LowPart;
                break;

            case PointerOperationType::POINTER_OP_OR:
                u.Value1.LowPart |= u.Value2.LowPart;
                break;

            case PointerOperationType::POINTER_OP_XOR:
                u.Value1.LowPart ^= u.Value2.LowPart;
                break;

            case PointerOperationType::POINTER_OP_MOD:
                u.Value1.LowPart %= u.Value2.LowPart;
                break;
        }
    }

#pragma warning(pop)

    return u._Ptr1;
}

EXTCPP
template<PointerOperationType::PointerOperationClass OP, class PtrType1>
ForceInline
PtrType1 PtrOperator(PtrType1 Ptr1, unsigned short Ptr2)
{
    return PtrOperator<OP>(Ptr1, (ULONG_PTR)Ptr2);
}

EXTCPP
template<PointerOperationType::PointerOperationClass OP, class PtrType1>
ForceInline
PtrType1 PtrOperator(PtrType1 Ptr1, short Ptr2)
{
    return PtrOperator<OP>(Ptr1, (ULONG_PTR)Ptr2);
}

EXTCPP
template<PointerOperationType::PointerOperationClass OP, class PtrType1>
ForceInline
PtrType1 PtrOperator(PtrType1 Ptr1, unsigned char Ptr2)
{
    return PtrOperator<OP>(Ptr1, (ULONG_PTR)Ptr2);
}

EXTCPP
template<PointerOperationType::PointerOperationClass OP, class PtrType1>
ForceInline
PtrType1 PtrOperator(PtrType1 Ptr1, char Ptr2)
{
    return PtrOperator<OP>(Ptr1, (ULONG_PTR)Ptr2);
}

EXTCPP
template<class PtrType1, class PtrType2>
ForceInline
ULONG_PTR PtrOffset(PtrType1 Ptr, PtrType2 Offset)
{
#pragma warning(push, 0)
    union
    {
        PtrType1    _Ptr1;
        ULONG_PTR   Value;
    };
#pragma warning(pop)

    _Ptr1 = Ptr;

    return PtrOperator<PointerOperationType::POINTER_OP_SUB>(Value, Offset);
}

EXTCPP
template<class PtrType1, class PtrType2>
ForceInline
PtrType1 PtrAdd(PtrType1 Ptr, PtrType2 Offset)
{
    return PtrOperator<PointerOperationType::POINTER_OP_ADD>(Ptr, Offset);
}

EXTCPP
template<class PtrType1, class PtrType2>
ForceInline
PtrType1 PtrSub(PtrType1 Ptr, PtrType2 Offset)
{
    return PtrOperator<PointerOperationType::POINTER_OP_SUB>(Ptr, Offset);
}

EXTCPP
template<class PtrType1, class PtrType2>
ForceInline
PtrType1 PtrAnd(PtrType1 Ptr, PtrType2 Offset)
{
    return PtrOperator<PointerOperationType::POINTER_OP_AND>(Ptr, Offset);
}

EXTCPP
template<class PtrType1, class PtrType2>
ForceInline
PtrType1 PtrOr(PtrType1 Ptr, PtrType2 Offset)
{
    return PtrOperator<PointerOperationType::POINTER_OP_OR>(Ptr, Offset);
}

EXTCPP
template<class PtrType1, class PtrType2>
ForceInline
PtrType1 PtrXor(PtrType1 Ptr, PtrType2 Offset)
{
    return PtrOperator<PointerOperationType::POINTER_OP_XOR>(Ptr, Offset);
}

EXTCPP
template<class PtrType1, class PtrType2>
ForceInline
PtrType1 PtrMod(PtrType1 Ptr, PtrType2 Offset)
{
    return PtrOperator<PointerOperationType::POINTER_OP_MOD>(Ptr, Offset);
}

EXTCPP template<class T> ForceInline Void FASTCALL Swap(T &t1, T &t2)
{
    T t;
    t = t1;
    t1 = t2;
    t2 = t;
}

ForceInline UInt32 GetRandom32Range(UInt32 Min, UInt32 Max)
{
    UInt32 Result = GetRandom32();

    if (Max < Min)
        Swap(Min, Max);

    return (Result % (Max + 1 - Min) + Min);
}

_ML_CPP_HEAD_

template<typename T> T Bswap(T);

#define DECL_BSWAP_2(type) template<> inline type Bswap(type v) { return _byteswap_ushort(v); }
#define DECL_BSWAP_4(type) template<> inline type Bswap(type v) { return _byteswap_ulong(v); }
#define DECL_BSWAP_8(type) template<> inline type Bswap(type v) { return _byteswap_uint64(v); }

DECL_BSWAP_2(Int16);
DECL_BSWAP_2(UInt16);
DECL_BSWAP_2(WChar);

DECL_BSWAP_4(Int32);
DECL_BSWAP_4(UInt32);

DECL_BSWAP_8(Int64);
DECL_BSWAP_8(UInt64);

DECL_BSWAP_4(Long);
DECL_BSWAP_4(ULong);

//DECL_BSWAP_8(Long64);
//DECL_BSWAP_8(ULong64);

_ML_CPP_TAIL_

#endif // CPP_DEFINED

#if ML_X86

ForceInline UInt32 FASTCALL GetEfFlags()
{
    INLINE_ASM
    {
        pushfd;
        pop eax;
    }
}

ForceInline UInt32 FASTCALL SetEfFlags(UInt32 EfFlags)
{
    INLINE_ASM
    {
        push EfFlags;
        popfd;
    }
}

ForceInline Void SetDirectionFlag()
{
    INLINE_ASM std;
}

ForceInline Void ClearDirectionFlag()
{
    INLINE_ASM cld;
}

inline Bool IsCPUIDValid()
{
    __asm
    {
        pushfd;
        pop eax;
        or eax, 1 << 21;
        push eax;
        popfd;
        pushfd;
        pop eax;
        shr eax, 21;
        and eax, 1;
    }
}

#endif // x86

_ML_C_TAIL_

#endif // _MISC_H_093f7fa1_751a_4627_83f8_df8509b9926e