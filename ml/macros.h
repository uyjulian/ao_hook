#ifndef _MACROS_H_6cd488df_3b68_4112_a937_9fcfa63eb723
#define _MACROS_H_6cd488df_3b68_4112_a937_9fcfa63eb723

#include "ml/types.h"

#if ML_DEBUG_KERNEL

#if ML_X86

    #define DEBUG_BREAK_POINT     DbgBreakPoint

#elif ML_AMD64

    #define DEBUG_BREAK_POINT     DbgBreakPoint

#endif // arch

    #if SUPPORT_VA_ARGS_MACRO
        #define DebugPrint(...)         DbgPrint(__VA_ARGS__)
    #else
        #define DebugPrint  DbgPrint
    #endif

#else

    #define DEBUG_BREAK_POINT()

    #if SUPPORT_VA_ARGS_MACRO
        #define DebugPrint(...)
    #else
        #define DebugPrint()
    #endif

#endif /* ML_DEBUG_KERNEL */

#define BP DEBUG_BREAK_POINT

#define RTL_CONST_STRING(_str, _string) \
            (_str).Length = sizeof(_string) - sizeof((_str).Buffer[0]); \
            (_str).MaximumLength = sizeof(_string); \
            (_str).Buffer = (_string);

enum // ECodePage
{
    CP_SHIFTJIS = 932,
    CP_GBK      = 936,
    CP_GB2312   = CP_GBK,
    CP_BIG5     = 950,
    CP_UTF16_LE = 1200,
    CP_UTF16_BE = 1201,
};

#define BOM_UTF8        (UInt32)(0xBFBBEF)
#define BOM_UTF16_LE    (UInt16)(0xFEFF)
#define BOM_UTF16_BE    (UInt16)(0xFFFE)

#define __MAKE_WSTRING(str) L##str
#define MAKE_WSTRING(str) __MAKE_WSTRING(str)

#define ASM_UNIQUE() INLINE_ASM mov eax, __LINE__

#define ASM_DUMMY(Bytes) ASM_DUMMY_##Bytes

#define ASM_DUMMY_1 INLINE_ASM nop
#define ASM_DUMMY_2 INLINE_ASM mov eax, eax

// lea eax, [eax+0];
#define ASM_DUMMY_3 INLINE_ASM __emit 0x8D INLINE_ASM __emit 0x40 INLINE_ASM __emit 0x00

// // lea esi, [esi]
#define ASM_DUMMY_4 INLINE_ASM __emit 0x8D \
                    INLINE_ASM __emit 0x74 \
                    INLINE_ASM __emit 0x26 \
                    INLINE_ASM __emit 0x00

#define ASM_DUMMY_5 INLINE_ASM mov eax, 1
#define ASM_DUMMY_6 INLINE_ASM __emit 0x8D INLINE_ASM __emit 0x80 INLINE_ASM __emit 0x00 INLINE_ASM __emit 0x00 INLINE_ASM __emit 0x00 INLINE_ASM __emit 0x00

#define ASM_DUMMY_7 INLINE_ASM __emit 0x8D \
                    INLINE_ASM __emit 0xB4 \
                    INLINE_ASM __emit 0x26 \
                    INLINE_ASM __emit 0x00 \
                    INLINE_ASM __emit 0x00 \
                    INLINE_ASM __emit 0x00 \
                    INLINE_ASM __emit 0x00

#define ASM_DUMMY_AUTO() INLINE_ASM mov eax, 1 INLINE_ASM mov ecx, 1 INLINE_ASM mov edx, 1 ASM_UNIQUE() INLINE_ASM ret

#if !defined(_M_IA64)
    #define MEMORY_PAGE_SIZE (4 * 1024)
#else
    #define MEMORY_PAGE_SIZE (8 * 1024)
#endif

#define NO_BREAK

#define NT6_LIB(lib) "D:/Dev/Windows Kits/8.0/Lib/win8/um/x86/" #lib

#define LOOP_ALWAYS for (;;)
#define LOOP_FOREVER LOOP_ALWAYS
#define LOOP_ONCE   for (Bool __condition_ = True; __condition_; __condition_ = False)

#if !defined(BREAK_IF)
    #define BREAK_IF(c) if (c) break;
#endif /* BREAK_IF */

#if !defined(CONTINUE_IF)
    #define CONTINUE_IF(c) if (c) continue;
#endif /* CONTINUE_IF */

#if !defined(RETURN_IF)
    #define RETURN_IF(c, r) if (c) return r
#endif /* RETURN_IF */

#if !defined(countof)
    #define countof(x) (sizeof((x)) / sizeof(*(x)))
#endif /* countof */

#if !defined(CONST_STRLEN)
    #define CONST_STRLEN(str) (countof(str) - 1)
    #define CONST_STRSIZE(str) (CONST_STRLEN(str) * sizeof(str[0]))
#endif // CONST_STRLEN

#if !defined(bitsof)
    #define bitsof(x) (sizeof(x) * 8)
#endif /* bitsof */

#define FIELD_BASE(address, type, field) (type *)((ULONG_PTR)address - (ULONG_PTR)&((type *)0)->field)
#define FIELD_TYPE(_Type, _Field)  TYPE_OF(((_Type*)0)->_Field)

#ifndef FIELD_SIZE
    #define FIELD_SIZE(type, field) (sizeof(((type *)0)->field))
#endif // FIELD_SIZE

#define SET_FLAG(_V, _F)    ((_V) |= (_F))
#define CLEAR_FLAG(_V, _F)  ((_V) &= ~(_F))
#define FLAG_ON(_V, _F)     (!!((_V) & (_F)))
#define FLAG_OFF(_V, _F)     (!FLAG_ON(_V, _F))

//TODO: figure out what this actually is
#define FLAG_ON_ALL FLAG_ON


#if !defined(TEST_BIT)
    #define TEST_BIT(value, bit) ((value) & (1 << bit))
#endif /* TEST_BIT */

#if !defined(TEST_BITS)
    #define TEST_BITS(value, bits) (Bool)(!!((value) & (bits)))
#endif /* TEST_BITS */

#if !defined(ROUND_DOWN)
    #define ROUND_DOWN(Value, Multiple) ((Value) / (Multiple) * (Multiple))
#endif /* ROUND_DOWN */

#if !defined(ROUND_UP)
    #define ROUND_UP(Value, Multiple) (ROUND_DOWN((Value) + (Multiple) - 1, (Multiple)))
#endif /* ROUND_UP */

#if !defined(IN_RANGE)
    #define IN_RANGE(low, value, high) (((low) <= (value)) && (value) <= (high))
    #define IN_RANGEEX(low, value, high) (((low) <= (value)) && (value) < (high))
#endif

#if !defined(MEMORY_PAGE_ADDRESS)
    #define MEMORY_PAGE_ADDRESS(Address) (ROUND_DOWN((ULongPtr)(Address), MEMORY_PAGE_SIZE))
#endif /* MEMORY_PAGE_ADDRESS */

#if defined(DEFAULT_VALUE)
    #undef DEFAULT_VALUE
#endif // DEFAULT_VALUE

#if CPP_DEFINED
    #define DEFAULT_VALUE(type, var, value) type var = value
    #define DEF_VAL(var, value)             var = value
#else
    #define DEFAULT_VALUE(type, var, value) type var
    #define DEF_VAL(var, value)             var
#endif //CPP_DEFINED

#if defined(__INTEL_COMPILER)
    #pragma warning(disable:1479)
#endif

#define MAKE_UNIQUE_NAME2(n) _##n
#define MAKE_UNIQUE_NAME(n) MAKE_UNIQUE_NAME2(n)
#define DUMMY_STRUCT(n) DECL_ALIGN(1) struct { char _[n]; } MAKE_UNIQUE_NAME(__LINE__)
// #define DUMMY_STRUCT(size) DECL_ALIGN(1) struct { Byte __dummy[size]; }


#define MAKE_NAME_PREFIX_(prefix, n) prefix##n
#define MAKE_NAME_PREFIX(prefix, n) MAKE_NAME_PREFIX_(prefix, n)

#define GetStructMemberOffset(type, member_begin, member_end) \
    (PByte)&(*(type*)0).member_end - (PByte)&(*(type*)0).member_begin

#if CPP_DEFINED

template<class FunctionType, class FunctionAddress>
ForceInline
FunctionType GetRoutinePtr(FunctionType, FunctionAddress Address)
{
    union
    {
        FunctionType    _Addr1;
        FunctionAddress _Addr2;
    };

    _Addr2 = Address;

    return _Addr1;
}

#if SUPPORT_VA_ARGS_MACRO

#define CallFuncPtr(RoutineType, RoutineAddr, ...) ((TYPE_OF(RoutineType)*)(RoutineAddr))(__VA_ARGS__)

#endif // SUPPORT_VA_ARGS_MACRO

#endif

//#if CPP_DEFINED
/*
template<class Type>
ForceInline const Type& MY_MAX(const Type &Value1, const Type &Value2)
{
    return Value1 < Value2 ? Value2 : Value1;
}

template<class Type>
ForceInline const Type& MY_MIN(const Type &Value1, const Type &Value2)
{
    return Value2 < Value1 ? Value2 : Value1;
}
*/
//#else // CPP_DEFINED

#define ML_MAX(a, b) ((a) < (b) ? (b) : (a))
#define ML_MIN(a, b) ((a) < (b) ? (a) : (b))

#define MY_MAX ML_MAX
#define MY_MIN ML_MIN

//#endif // CPP_DEFINED

#define CHAR_UPPER(ch) (IN_RANGE('a', (ch), 'z') ? ((ch) & (Byte)0xDF) : ch)
#define CHAR_LOWER(ch) (IN_RANGE('A', (ch), 'Z') ? ((ch) | (Byte)~0xDF) : ch)

#define _CHAR_UPPER4W(ch) (UInt64)((ch) & 0xFFDFFFDFFFDFFFDF)
#define CHAR_UPPER4W(ch) _CHAR_UPPER4W((UInt64)(ch))
#define CHAR_UPPER3W(ch) (UInt64)(CHAR_UPPER4W(ch) & 0x0000FFFFFFFFFFFF)
#define CHAR_UPPER2W(ch) (UInt64)(CHAR_UPPER4W(ch) & 0x00000000FFFFFFFF)
#define CHAR_UPPER1W(ch) (UInt64)(CHAR_UPPER4W(ch) & 0x000000000000FFFF)

#define _CHAR_UPPER4(ch) (UInt32)((ch) & 0xDFDFDFDF)
#define CHAR_UPPER4(ch) (UInt32)_CHAR_UPPER4((UInt32)(ch))
#define CHAR_UPPER3(ch) (UInt32)(CHAR_UPPER4(ch) & 0x00FFFFFF)
#define CHAR_UPPER2(ch) (UInt32)(CHAR_UPPER4(ch) & 0x0000FFFF)
#define CHAR_UPPER1(ch) (UInt32)(CHAR_UPPER4(ch) & 0x000000FF)
#define CHAR_UPPER8(ch) ((UInt64)(ch) & 0xDFDFDFDFDFDFDFDF)

#define _TAG2(s) ((((s) << 8) | ((s) >> 8)) & 0xFFFF)
#define TAG2(s) _TAG2((u16)(s))

#define _TAG3(s) ( \
                (((s) >> 16) & 0xFF)       | \
                (((s)        & 0xFF00))    | \
                (((s) << 16) & 0x00FF0000) \
                )
#define TAG3(s) _TAG3((u32)(s))

#define _TAG4(s) ( \
                (((s) >> 24) & 0xFF)       | \
                (((s) >> 8 ) & 0xFF00)     | \
                (((s) << 24) & 0xFF000000) | \
                (((s) << 8 ) & 0x00FF0000) \
                )
#define TAG4(s) _TAG4((u32)(s))

#define TAG8(left, right) (((UInt64)TAG4(right) << 32) | TAG4(left))

#define _TAG2W(x) (((x) & 0xFF) << 16 | ((x) & 0xFF00) >> 8)
#define TAG2W(x) (UInt32)_TAG2W((UInt32)(x))

#define _TAG3W(x) (TAG4W(x) >> 16)
#define TAG3W(x) (UInt64)_TAG3W((UInt64)(x))

#define _TAG4W(x) (((UInt64)TAG2W((x) & 0xFFFF) << 32) | ((UInt64)TAG2W((x) >> 16)))
#define TAG4W(x) (UInt64)_TAG4W((UInt64)(x))

#pragma warning(disable:4310)
#define SWAP2(v) (u16)(((u32)(v) << 8) | ((u16)(v) >> 8))
#define SWAPCHAR(v) ((u32)SWAP2(v))

#define LoByte(v)  (u8) ((v & 0xFF))
#define HiByte(v)  (u8) (((v) >> 8) & 0xFF)
#define LoWord(v)  (u16)((v) & 0xFFFF)
#define HiWord(v)  (u16)(((v) >> 16) & 0xFFFF)
#define LoDword(v) (u32)((v))
#define HiDword(v) (u32)(((v) >> 32))

#define MakeLong(l, h)   (long)((s32)(l) | ((s32)(h) << 16))
#define MakeLong64(l, h) (s64)((s64)(l) | (s64)(h) << 32)

#define MakeDword(l, h) (u32)((u32)(l) | ((u32)(h) << 16))
#define MakeQword(l, h) (u64)((u64)(l) | (u64)(h) << 32)

#define MAKEINTRESA(i) ((PChar)(Word)(i))
#define MAKEINTRESW(i) ((PWChar)(Word)(i))

#define STRTOULONG(x) (ULong_Ptr)(x)

#define ML_IP_ADDRESS(a1, a2, a3, a4) ((a1) | ((a2) << 8) | ((a3) << 16) | ((a4) << 24))
#define ML_PORT(_port) SWAP2(_port)

#define FOR_EACH(_it, _base, _n) for (auto _Count = ( ((_it) = (_base)), (_n)); _Count != 0; ++(_it), --_Count)
#define FOR_EACH_REVERSE(_it, _base, _n) for (auto _Count = ( ((_it) = (_base) + (_n) - 1), (_n)); _Count != 0; --(_it), --_Count)
#define FOR_EACH_ARRAY(_it, _arr) FOR_EACH(_it, _arr, countof(_arr))
#define FOR_EACH_S(_it, _base, _n, _size) for (auto _Count = ( ((_it) = (_base)), (_n)); _Count != 0; ((_it) = PtrAdd(_it, _size)), --_Count)
#define FOR_EACH_X(_it, _base, _n) for (auto _Count = ( ((_it) = (_base)), (_n); _Count != 0; ++(_it), --(_n), --_Count)

#define FOR_EACH_FORWARD(_it, _n) { (_it) += (_n); (_Count) += (_n); }
#define FOR_EACH_BACKWARD(_it, _n) { (_it) -= (_n); (_Count) -= (_n); }

#if MY_UNICODE_ENABLE
    #define MAKEINTRES MAKEINTRESW
#else
    #define MAKEINTRES MAKEINTRESA
#endif /* UNICODE */


#define DEFINED_HRESULT(failed, facility, code) \
            ((HRESULT)(((failed) << 31) | ((facility) << 16) | (code)))

#define SET_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTC const GUID DECLSPEC_SELECTANY name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

#define DECL_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTC const GUID FAR name

#endif // _MACROS_H_6cd488df_3b68_4112_a937_9fcfa63eb723
