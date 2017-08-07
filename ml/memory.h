#ifndef _MEMORY_H_38900f05_7a34_48ee_8cd6_84300c9b8f1f
#define _MEMORY_H_38900f05_7a34_48ee_8cd6_84300c9b8f1f

#pragma warning(disable:4127)
#pragma warning(disable:4047)
#pragma warning(disable:4706)

#include "ml/intrinsic.h"

_ML_C_HEAD_

#define CopyStruct(dest, src, size) \
    { \
        typedef DECL_ALIGN(1) struct { ::Byte b[size]; } _SDummy; \
        *(_SDummy *)(dest) = *(_SDummy *)(src); \
    }
#if 0
    else \
    { \
        memcpy(dest, src, size); \
    }
#endif

ForceInline void* AllocStack(size_t Size)
{
#if _DEBUG
    return malloc(ROUND_UP(Size, 16));
#else
    return _alloca(ROUND_UP(Size, 16));
#endif
}

inline PVoid XorMemory(PVoid Destination, Byte Value, size_t Size)
{
    UInt32 c1;
    PByte Buffer = (PByte)Destination;

    c1 = Value | (UInt32)(Value << 8) | (UInt32)(Value << 16) | (UInt32)(Value << 24);

    for (; Size != 0 && (Int_Ptr)Buffer & 3; --Size)
        *Buffer++ ^= c1;

    for (; Size >= 4; Buffer += 4, Size -= 4)
        *(PUInt32)Buffer ^= c1;

    for (; Size; --Size)
        *Buffer++ ^= c1;

    return Destination;
}

inline PVoid XorMemory4(PVoid Destination, UInt32 Value, size_t Size)
{
    PByte Buffer = (PByte)Destination;

    while (Size)
        *(PUInt32)&Buffer[Size -= 4] ^= Value;

    return Destination;
}

inline PVoid AndMemory(PVoid Destination, Byte Value, size_t Size)
{
    UInt32 c1;
    PByte  Buffer = (PByte)Destination;

    c1 = Value | (UInt32)(Value << 8) | (UInt32)(Value << 16) | (UInt32)(Value << 24);

    for (; Size != 0 && (Int_Ptr)Buffer & 3; --Size)
        *Buffer++ &= c1;

    for (; Size >= 4; Buffer += 4, Size -= 4)
        *(PUInt32)Buffer &= c1;

    for (; Size; --Size)
        *Buffer++ &= c1;

    return Destination;
}

inline PVoid OrMemory(PVoid Destination, Byte Value, size_t Size)
{
    UInt32 c1;
    PByte  Buffer = (PByte)Destination;

    c1 = Value | (UInt32)(Value << 8) | (UInt32)(Value << 16) | (UInt32)(Value << 24);

    for (; Size != 0 && (Int_Ptr)Buffer & 3; --Size)
        *Buffer++ |= c1;

    for (; Size >= 4; Buffer += 4, Size -= 4)
        *(PUInt32)Buffer |= c1;

    for (; Size; --Size)
        *Buffer++ |= c1;

    return Destination;
}

#if MY_OS_WIN32

#if !defined(MY_X64)

ForceInline LPVoid CDECL my_memcpy_inline(void *dst, const void *src, size_t count)
{
#if !MY_MSC_1500 || defined(MY_COMPILER_INTEL)
    __asm
    {
        mov  edi, dst;
        mov  esi, src;
        mov  ecx, count;
        mov  edx, ecx;
        shr  ecx, 2;
        rep  movsd;
        mov  ecx, edx;
        and  ecx, 3;
        rep  movsb;
    }
#else
    size_t n = count & ~3;
    __movsd((PULONG_PTR)dst, (const ULONG_PTR *)src, count >> 2);
    __movsb((PByte)dst + n, (PByte)src + n, count & 3);
#endif

    return dst;
}

ForceInline LPVoid CDECL my_memset_inline(void* dest, Byte c, size_t count)
{
    UInt32 c1;

    c1 = c | (UInt32)(c << 8);
    c1 |= (c1 << 16);
#if defined(MY_COMPILER_INTEL) || !MY_MSC_1500
    __asm
    {
        mov  edi, dest;
        mov  eax, c1;
        mov  ecx, count;
        mov  edx, ecx;
        shr  ecx, 2;
        rep  stosd;
        mov  ecx, edx;
        and  ecx, 3;
        rep  stosb;
    }
#else
    size_t n = count & ~3;
    __stosd((PULONG_PTR)dest, c1, count >> 2);
    __stosb((PByte)dest + n, (Byte)c1, count & 3);
#endif

	return dest;
}

ForceInline LPVoid CDECL memset4(void* dest, ULong_Ptr c, size_t count)
{
#if defined(MY_COMPILER_INTEL) || !defined(MY_COMPILER_MSC)
    __asm
    {
        mov  edi, dest;
        mov  eax, c;
        mov  ecx, count;
        mov  edx, ecx;
        shr  ecx, 2;
        rep  stosd;
        mov  ecx, edx;
        and  ecx, 3;
        rep  stosb;
    }
    return dest;
#else
    __stosd((PULONG_PTR)dest, (ULong)c, count / sizeof(c));
    return dest;
#endif
}

ForceInline LPVoid CDECL memset2(void* dest, UInt16 c, size_t count)
{
    UInt32 c1 = c | (c << 16);
    __asm
    {
        mov  edi, dest;
        mov  eax, c1;
        mov  ecx, count;
        mov  edx, ecx;
        shr  ecx, 2;
        rep  stosd;
        mov  ecx, edx;
        and  ecx, 3;
        rep  stosb;
    }

    return dest;
}

#endif // MY_X64

#endif // MY_OS_WIN32

// EXTC_IMPORT void* __cdecl memset(void * _Dst, int _Val, size_t _Size);
// EXTC_IMPORT void* __cdecl memcpy(void *dest, const void *src, size_t count);

#pragma intrinsic(memcmp, memcpy, memset)

#if !defined(NOT_USE_CUSTOM_MEMFUNC) && MY_OS_WIN32

#undef RtlMoveMemory

#if ML_AMD64

    #define CompareMemory(Buffer1, Buffer2, SizeInBytes) memcmp((void *)(Buffer1), (void *)(Buffer2), (size_t)(SizeInBytes))

#if ML_KERNEL_MODE

    #define CopyMemory(Destination, Source, Length) RtlCopyMemory((void*)(Destination), (void*)(Source), (size_t)(Length))
    #define FillMemory(Destination, Length, Fill) memset(Destination, Fill, Length)
    #define FillMemory4(Destination, Length, Fill) memset4(Destination, Fill, Length)
    #define ZeroMemory(Destination, Length) FillMemory(Destination, Length, 0)

#endif

#else //x86

#ifdef memcpy
    #undef memcpy
#endif /* memcpy */

#ifdef memset
    #undef memset
#endif /* memset */

#ifdef ZeroMemory
    #undef ZeroMemory
#endif /* ZeroMemory */

#ifdef CopyMemory
    #undef CopyMemory
#endif /* CopyMemory */

#ifdef RtlCopyMemory
    #undef RtlCopyMemory
#endif /* RtlCopyMemory */

#ifdef FillMemory
    #undef FillMemory
#endif // FillMemory

    #define memcpy my_memcpy_inline
    #define memset my_memset_inline
    #define CompareMemory(Buffer1, Buffer2, SizeInBytes) memcmp((void *)(Buffer1), (void *)(Buffer2), (size_t)(SizeInBytes))
    #define FillMemory(Destination, Length, Fill) memset(Destination, (Byte)Fill, Length)
    #define FillMemory4(Destination, Length, Fill) memset4(Destination, Fill, Length)
    #define ZeroMemory(Destination, Length) FillMemory(Destination, Length, 0)
    #define CopyMemory(Destination, Source, Length) memcpy((void *)(Destination), (void *)(Source), (size_t)(Length))
    #define RtlCopyMemory CopyMemory

#endif // arch

//    #define memcpy my_memcpy_detect
#endif // NOT_USE_CUSTOM_MEMFUNC

EXTC_IMPORT Void STDCALL RtlMoveMemory(PVoid Destination, PVoid Source, SizeT Length);

_ML_C_TAIL_



#pragma warning(default:4047)
#pragma warning(default:4127)
#pragma warning(default:4706)

#endif // _MEMORY_H_38900f05_7a34_48ee_8cd6_84300c9b8f1f
