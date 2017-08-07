#ifndef _TYPES_H_54e4879c_024a_491f_b990_859f407442fe
#define _TYPES_H_54e4879c_024a_491f_b990_859f407442fe

#include "ml/include.h"

//#pragma warning(disable:4201)

/************************************************************************/
/*  绫诲瀷                                                                */
/************************************************************************/

_ML_C_HEAD_

#if !defined(_WCHAR_T_DEFINED)
    typedef unsigned short wchar_t;
    #define _WCHAR_T_DEFINED
#endif

#if !CPP_DEFINED

#define nullptr NULL

#endif // cpp

typedef float               Float, *PFloat, *LPFloat;
typedef double              Double, *PDouble, *LPDouble;

typedef char                Char, s8, Int8, *ps8, *PInt8, *PChar;
typedef const char         *PCChar;
typedef wchar_t             WChar, *PWChar;
typedef const wchar_t      *PCWChar;
typedef unsigned char       u8, UChar, UInt8, UInt24, Byte, *pu8, *PByte, *PUInt8, *PUChar;
typedef short               Short, s16, Int16, *ps16, *PInt16, *PShort, *LPShort;
typedef unsigned short      UShort, Word, u16, UInt16, *pu16, *PUInt16, *PWord, *LPWord, *PUShort, *LPUShort;
typedef long                Long, *PLong, *LPLong;
typedef long long           Long64, LongLong, *PLong64, *PLongLong;
typedef unsigned long       Dword, ULong, *PULong, *LPULong, *PDword, *LPDword;
typedef unsigned long long  ULong64, ULongLong, *PULong64, *PULongLong;
typedef void                Void, *PVoid, *LPVoid;
typedef const void         *LPCVoid, *PCVoid;

typedef int                 Bool, Int, s32, Int32, *PInt, *ps32, *PInt32;
typedef unsigned int        UInt, u32, UInt32, *PUInt, *pu32, *PUInt32;

typedef __int64             s64, Int64, *ps64, *PInt64;
typedef unsigned __int64    u64, UInt64, *pu64, *PUInt64;

typedef int (__cdecl *QSORT_COMPARE_ROUTINE)(const void *,const void *);

#if CPP_DEFINED

EXTCPP
template<typename T>
class LambdaCastHelper
{
public:

    typedef T FUNC;
};

#endif // CPP_DEFINED

#define QSortCallbackM(...) (QSORT_COMPARE_ROUTINE)(LambdaCastHelper<int(CDECL *)(__VA_ARGS__)>::FUNC)[] (__VA_ARGS__) -> int


#pragma pack(push, 1)

typedef struct
{
    UShort Offset   : 12;
    UShort Type     : 4;

} IMAGE_RELOCATION_ADDRESS_ENTRY, *PIMAGE_RELOCATION_ADDRESS_ENTRY;

typedef struct
{
    ULong   VirtualAddress;
    ULong   SizeOfBlock;
    IMAGE_RELOCATION_ADDRESS_ENTRY TypeOffset[1];

} *PIMAGE_BASE_RELOCATION2;

#pragma pack(pop)

#if 0 && CPP_DEFINED

class UInt24
{
protected:
    UInt32 m_Value;

public:
    template<class Type>
    UInt24& operator=(Type Value)
    {
        m_Value = (UInt32)Value & 0x00FFFFFF;
        return m_Value;
    }

    template<class Type>
    UInt24& operator+(Type Value)
    {
        m_Value = (m_Value + Value) & 0x00FFFFFF;
    }
};

#endif // CPP_DEFINED


#if defined(MY_X64)
    typedef __int64 Int_Ptr, *PInt_Ptr;
    typedef unsigned __int64 UInt_Ptr, *PUInt_Ptr;

    typedef __int64 Long_Ptr, *PLong_Ptr, LongPtr, *PLongPtr;
    typedef unsigned __int64 ULong_Ptr, *PULong_Ptr, ULongPtr, *PULongPtr;
#else
    typedef int MY_W64 Int_Ptr, *PInt_Ptr;
    typedef unsigned int MY_W64 UInt_Ptr, *PUInt_Ptr;

    typedef long MY_W64 Long_Ptr, *PLong_Ptr, LongPtr, *PLongPtr;
    typedef unsigned long MY_W64 ULong_Ptr, *PULong_Ptr, ULongPtr, *PULongPtr;
#endif

typedef ULong_Ptr SizeT, *PSizeT;
typedef Long_Ptr  SSizeT, *PSSizeT;

#define MAX_SHORT  (Short) (0x7FFF)
#define MAX_USHORT (UShort)(0xFFFF)
#define MAX_INT    (Int)   (0x7FFFFFFF)
#define MAX_UINT   (UInt)  (0xFFFFFFFF)
#define MAX_INT64  (Int64) (0x7FFFFFFFFFFFFFFF)
#define MAX_UINT64 (UInt64)(0xFFFFFFFFFFFFFFFF)
#define MAX_NTPATH  0x220

#if CPP_DEFINED

IF_NOT_EXIST(_PVFV)
{
    typedef void (CDECL *_PVFV)(void);
    typedef int  (CDECL *_PIFV)(void);
    typedef void (CDECL *_PVFI)(int);
}

EXTC _PVFV __xi_a, __xi_z, __xc_a, __xc_z;

inline void my_initterm(_PVFV *pfbegin, _PVFV *pfend)
{
    /*
    * walk the table of function pointers from the bottom up, until
    * the end is encountered.  Do not skip the first entry.  The initial
    * value of pfbegin points to the first valid entry.  Do not try to
    * execute what pfend points to.  Only entries before pfend are valid.
    */
    while ( pfbegin < pfend )
    {
        /*
        * if current table entry is non-NULL, call thru it.
        */
        if ( *pfbegin != NULL )
            (**pfbegin)();
        ++pfbegin;
    }
}

#endif // CPP_DEFINED

#define DR7_LEN_1_BYTE  1
#define DR7_LEN_2_BYTE  2
#define DR7_LEN_4_BYTE  3
#define DR7_RW_EXECUTE  0
#define DR7_RW_WRITE    1
#define DR7_RW_ACCESS   3

#pragma pack(1)

typedef union
{
    ULong Dr6;
    struct
    {
        Byte B0         : 1;
        Byte B1         : 1;
        Byte B2         : 1;
        Byte B3         : 1;
        Byte Reserve1   : 8;
        Byte Reserve2   : 2;
        Byte BD         : 1;
        Byte BS         : 1;
        Byte BT         : 1;
    };

} DR6_INFO, *PDR6_INFO;

typedef union
{
    ULong Dr7;
    struct
    {
        Byte L0         : 1;    // 0x01
        Byte G0         : 1;    // 0x02
        Byte L1         : 1;    // 0x03
        Byte G1         : 1;    // 0x04
        Byte L2         : 1;    // 0x05
        Byte G2         : 1;    // 0x06
        Byte L3         : 1;    // 0x07
        Byte G3         : 1;    // 0x08
        Byte LE         : 1;    // 0x09
        Byte GE         : 1;    // 0x0A
        Byte Reserve    : 3;    // 0x0B
        Byte GD         : 1;    // 0x0E
        Byte Reserve2   : 2;    // 0x0F
        Byte RW0        : 2;    // 0x11
        Byte LEN0       : 2;    // 0x13
        Byte RW1        : 2;    // 0x15
        Byte LEN1       : 2;    // 0x17
        Byte RW2        : 2;    // 0x19
        Byte LEN2       : 2;    // 0x1B
        Byte RW3        : 2;    // 0x1D
        Byte LEN3       : 2;    // 0x1F
    };
} DR7_INFO, *PDR7_INFO;

typedef struct
{
    ULong_Ptr Rdi;
    ULong_Ptr Rsi;
    ULong_Ptr Rbp;
    ULong_Ptr Rsp;
    ULong_Ptr Rbx;
    ULong_Ptr Rdx;
    ULong_Ptr Rcx;
    ULong_Ptr Rax;

} PUSHAD_REGISTER, *PPUSHAD_REGISTER;

typedef union
{
    struct
    {
        ULong LowPart;
        Long  HighPart;
    };
    Long64 QuadPart;
} Large_Integer, *PLarge_Integer, *LPLarge_Integer;

typedef union
{
    struct
    {
        ULong LowPart;
        ULong HighPart;
    };
    ULong64 QuadPart;
} ULarge_Integer, *PULarge_Integer, *LPULarge_Integer;

typedef union
{
    struct
    {
        UInt16 w1;
        UInt16 w2;
        UInt16 w3;
        UInt16 w4;
        UInt16 w5;
        UInt16 w6;
        UInt16 w7;
        Int16  w8;
    };

    struct
    {
        UInt32 d1;
        UInt32 d2;
        UInt32 d3;
        Int32  d4;
    };

    struct
    {
        ULarge_Integer q1;
        Large_Integer  q2;
    };
} MMXRegister;

typedef union
{
    LPVoid  lpBuffer;
    PByte   pbBuffer;
    PUInt16 pBuffer16;
    PUInt32 pBuffer32;
    PUInt64 pBuffer64;
    PLarge_Integer  pLi;
    PULarge_Integer pULi;
} MultiTypePointer;

#pragma pack()

#if !CPP_DEFINED
    typedef unsigned char bool, *pbool;
    #define inline __inline
#else
    typedef bool *pbool;
#endif /* __cplusplus */

#if MY_UNICODE_ENABLE
    typedef WChar TChar;
#else
    typedef Char TChar;
#endif /* UNICODE */

typedef TChar *PTChar;
typedef const TChar *PCTChar;

#if MY_OS_WIN32 && !ML_AMD64

ForceInline Void FASTCALL DebugBreakPoint()
{
    __debugbreak();
}

ForceInline Int CDECL _purecall2()
{
    DebugBreakPoint();
    return 0;
}

ForceInline int CDECL _purecall()
{
    DebugBreakPoint();
    return 0;
}

#endif

// override global operator new/delete

#if !defined(MYAPI)
    #define MYAPI(funcname) funcname
#endif

#if ML_KERNEL_MODE
/*
    inline PVOID _KiNewAllocate(HANDLE HeapBase, ULONG Flags, SIZE_T Bytes)
    {
        ULONG_PTR PoolType = (ULONG_PTR)HeapBase;
        return AllocateMemory(Bytes, PoolType);
    }

    BOOLEAN _KiDelete(HANDLE HeapBase, ULONG Flags, LPVOID Memory)
    {
        FreeMemory(Memory);
    }
*/
    #define NEW_ALLOC_API(PoolType, Flags, Bytes) AllocateMemory(Bytes, (POOL_TYPE)(ULONG_PTR)(PoolType))
    #define NEW_FREE_API(HeapBase, Flags, Memory) FreeMemory(Memory)

#else // r3

    #if USE_CRT_ALLOC
        #define MY_OPNEW_ALLOC(n) malloc(n)
        #define MY_OPNEW_FREE(p)  free(p)
    #else
        #define MY_OPNEW_ALLOC(n) HeapAlloc(GetProcessHeap(), 0, n)
        #define MY_OPNEW_FREE(p)  HeapFree(GetProcessHeap(), 0, p)
    #endif

    #if USE_NT_VER
        #define NEW_ALLOC_API   RtlAllocateHeap
        #define NEW_FREE_API    RtlFreeHeap
    #else
        #define NEW_ALLOC_API   HeapAlloc
        #define NEW_FREE_API    HeapFree
    #endif // USE_NT_VER

#endif // r0

#ifndef SAFE_DELETE
    #define SAFE_DELETE(p) if (p) \
                           { \
                               delete (p); \
                               (p) = NULL; \
                           }
#endif // SAFE_DELETE

#ifndef SAFE_FREE
    #define SAFE_FREE(p) if (p) \
                           { \
                               MY_OPNEW_FREE(p); \
                               (p) = NULL; \
                           }
#endif // SAFE_FREE

#if CPP_DEFINED

// && (defined(_INC_MALLOC) || defined(_WINDOWS_))

#define OVERLOAD_OP_NEW  \
    inline Void* CDECL operator new[](size_t size)\
    {\
        return size ? MY_OPNEW_ALLOC(size) : NULL;\
    }\
    \
    inline Void* CDECL operator new(size_t size)\
    {\
        return (Void *)(new char[size]);\
    }\
    \
    inline Void CDECL operator delete(Void *p)\
    {\
        if (p != NULL) \
            MY_OPNEW_FREE(p);\
    }\
    \
    inline Void CDECL operator delete[](Void *p)\
    {\
        delete(p);\
    }

#define OVERLOAD_CPP_NEW_WITH_HEAP(NewHeap) \
    inline PVoid CDECL operator new[](size_t Size) \
    { \
        return NEW_ALLOC_API((NewHeap), 0, Size); \
    } \
    inline PVoid CDECL operator new(size_t Size) \
    { \
        return operator new[](Size); \
    } \
    inline Void CDECL operator delete(PVoid pBuffer) \
    { \
        if (pBuffer != NULL) \
            NEW_FREE_API((NewHeap), 0, pBuffer); \
    } \
    inline Void CDECL operator delete[](PVoid pBuffer) \
    { \
        delete pBuffer; \
    }

#define OVERLOAD_CPP_METHOD_NEW_WITH_HEAP(NewHeap) \
    inline PVoid CDECL operator new[](size_t Size) \
    { \
        return NEW_ALLOC_API((NewHeap), 0, Size); \
    } \
    inline PVoid CDECL operator new(size_t Size) \
    { \
        return operator new[](Size); \
    } \
    inline Void CDECL operator delete(PVoid pBuffer) \
    { \
        if (pBuffer != NULL) \
            NEW_FREE_API((NewHeap), 0, pBuffer); \
    } \
    inline Void CDECL operator delete[](PVoid pBuffer) \
    { \
        delete pBuffer; \
    }

#else

    #define OVERLOAD_OP_NEW
    #define OVERLOAD_CPP_NEW_WITH_HEAP(Heap)
    #define OVERLOAD_CPP_METHOD_NEW_WITH_HEAP(Heap)

#endif /* __cplusplus */


#define ML_OVERLOAD_NEW \
            inline PVoid CDECL operator new[](size_t Size) \
            { \
                return AllocateMemoryP(Size); \
            } \
            inline PVoid CDECL operator new(size_t Size) \
            { \
                return operator new[](Size); \
            } \
            inline Void CDECL operator delete(PVoid Buffer) \
            { \
                if (Buffer != NULL) \
                    FreeMemoryP(Buffer); \
            } \
            inline Void CDECL operator delete(PVoid Buffer, UInt_Ptr) \
            { \
                if (Buffer != NULL) \
                    FreeMemoryP(Buffer); \
            } \
            inline Void CDECL operator delete[](PVoid Buffer) \
            { \
                delete Buffer; \
            } \
            inline Void CDECL operator delete[](PVoid Buffer, UInt_Ptr) \
            { \
                delete Buffer; \
            }

_ML_C_TAIL_

//#pragma warning(default:4201)

#endif // _TYPES_H_54e4879c_024a_491f_b990_859f407442fe
