#ifndef _INTRINSIC_H_725e6954_2a61_467e_9a57_c634deb44f80
#define _INTRINSIC_H_725e6954_2a61_467e_9a57_c634deb44f80

_ML_C_HEAD_

// #include <emmintrin.h>
#include <xmmintrin.h>
//#include <crtversion.h>



// #if _VC_CRT_BUILD_VERSION >= 23026
//     #include <intrin.h>
// #endif

void __cdecl _disable(void);
void __cdecl _enable(void);

void*   __cdecl _alloca(size_t _Size);
int     __cdecl memcmp(const void *buf1, const void *buf2, size_t count);
void*   __cdecl memcpy(void *dest, const void *src, size_t count);
void*   __cdecl memset(void *dest, int c, size_t count);

unsigned char   __cdecl _rotr8(unsigned char value, unsigned char shift);
unsigned short  __cdecl _rotr16(unsigned short value, unsigned char shift);
unsigned char   __cdecl _rotl8(unsigned char value, unsigned char shift);
unsigned short  __cdecl _rotl16(unsigned short value, unsigned char shift);

void            __cdecl __debugbreak();

void*           _AddressOfReturnAddress();
void*           _ReturnAddress();

#if CPP_DEFINED

IF_NOT_EXIST(__stosb)
{
    void            __cdecl __stosb(void* Dest, unsigned char Data, size_t Count);
    void            __cdecl __stosw(void* Dest, unsigned short Data, size_t Count);
    void            __cdecl __stosd(void* Dest, unsigned long Data, size_t Count);
}

IF_NOT_EXIST(__movsb)
{
    void            __cdecl __movsb(void *Destination, const void *Source, size_t Count);
    void            __cdecl __movsw(void *Dest, const void *Source, size_t Count);
    void            __cdecl __movsd(void *Dest, const void *Source, size_t Count);
}

#endif


/************************************************************************
  interlocked
************************************************************************/

long            __cdecl _InterlockedIncrement(long volatile * lpAddend);
long            __cdecl _InterlockedDecrement(long volatile * lpAddend);
long            __cdecl _InterlockedCompareExchange(long volatile * Destination, long Exchange, long Comperand);
long            __cdecl _InterlockedExchangeAdd(long volatile * Addend, long Value);
long            __cdecl _InterlockedExchange(volatile long * Target, long Value);
char            __cdecl _InterlockedExchange8(char volatile *Destination, char ExChange);

long            _InterlockedAnd(long volatile *Destination, long Value);
long            _InterlockedOr(long volatile *Destination, long Value);
long            _InterlockedXor(long volatile *Destination, long Value);

long            __cdecl _InterlockedIncrement_acq(long * lpAddend);
long            __cdecl _InterlockedIncrement_rel(long * lpAddend);
short           _InterlockedIncrement16(short volatile * lpAddend);
short           __cdecl _InterlockedIncrement16_acq(short * lpAddend);
short           __cdecl _InterlockedIncrement16_rel(short * lpAddend);
__int64         _InterlockedIncrement64(volatile __int64 * lpAddend);
__int64         __cdecl _InterlockedIncrement64_acq(volatile __int64 * lpAddend);
__int64         __cdecl _InterlockedIncrement64_rel(volatile __int64 * lpAddend);

long            __cdecl _InterlockedDecrement_acq(long * lpAddend);
long            __cdecl _InterlockedDecrement_acq(long * lpAddend);
short           _InterlockedDecrement16(short volatile * lpAddend);
short           __cdecl _InterlockedDecrement16_acq(short * lpAddend);
short           __cdecl _InterlockedDecrement16_rel(short * lpAddend);
__int64         _InterlockedDecrement64(volatile __int64 * lpAddend);
__int64         __cdecl _InterlockedDecrement64_acq(__int64 * lpAddend);
__int64         __cdecl _InterlockedDecrement64_rel(__int64 * lpAddend);

long            __cdecl _InterlockedExchange_acq(long * Target, long Value);
__int64         _InterlockedExchange64(volatile __int64 * Target, __int64 Value);
__int64         __cdecl _InterlockedExchange64_acq(__int64 * Target, __int64 Value);

long            __cdecl _InterlockedCompareExchange_acq(long volatile * Destination, long Exchange, long Comperand);
long            __cdecl _InterlockedCompareExchange_rel(long volatile * Destination, long Exchange, long Comperand);
short                   _InterlockedCompareExchange16(short volatile * Destination, short Exchange, short Comperand);
short           __cdecl _InterlockedCompareExchange16_acq(short volatile * Destination, short Exchange, short Comperand);
short           __cdecl _InterlockedCompareExchange16_rel(short volatile * Destination, short Exchange, short Comperand);
__int64         __cdecl _InterlockedCompareExchange64(__int64 volatile * Destination, __int64 Exchange, __int64 Comperand);
__int64         __cdecl _InterlockedCompareExchange64_acq(__int64 volatile * Destination, __int64 Exchange, __int64 Comperand);
__int64         __cdecl _InterlockedCompareExchange64_rel(__int64 volatile * Destination, __int64 Exchange, __int64 Comperand);
unsigned char   __cdecl _InterlockedCompareExchange128(__int64 volatile * Destination, __int64 ExchangeHigh, __int64 ExchangeLow, __int64 * Comparand);
unsigned char           _BitScanForward (unsigned long *Index, unsigned long Mask);


void _ReadBarrier(void);
void _WriteBarrier(void);
void _ReadWriteBarrier(void);


/************************************************************************
  cpuid
************************************************************************/
void __cpuid(int CPUInfo[4], int InfoType);
void __cpuidex(int CPUInfo[4], int InfoType, int ECXValue);


/************************************************************************
  dr
************************************************************************/
UInt_Ptr __readdr(unsigned int DebugRegister);

#if ML_AMD64
    void __writedr(unsigned DebugRegister, unsigned __int64 DebugValue);
#elif ML_X86
    void __writedr(unsigned int DebugRegister, unsigned int DebugValue);
#endif

/************************************************************************
  crx
************************************************************************/

#if ML_AMD64

    void __writecr0(unsigned __int64 Data);
    void writecr3(unsigned __int64 Data);
    void writecr4(unsigned __int64 Data);
    void writecr8(unsigned __int64 Data);

#elif ML_X86
    void __writecr0(unsigned Data);
    void writecr3(unsigned Data);
    void writecr4(unsigned Data);
    void writecr8(unsigned Data);
#endif

ULongPtr __readcr0(void);


/************************************************************************
  msr
************************************************************************/

unsigned __int64 __readmsr(unsigned long register);
void __writemsr(unsigned long Register, unsigned __int64 Value);


/************************************************************************
  vmx
************************************************************************/

void __vmx_off();


/************************************************************************
  idt
************************************************************************/

void __lidt(void *);


#if ML_AMD64

unsigned char __vmx_on(unsigned __int64 *VmsSupportPhysicalAddress);
unsigned char __vmx_vmread(size_t Field, size_t *FieldValue);
unsigned char __vmx_vmwrite(size_t Field, size_t FieldValue);
unsigned char __vmx_vmclear(unsigned __int64 *VmcsPhysicalAddress);
unsigned char __vmx_vmlaunch(void);
unsigned char __vmx_vmptrld(unsigned __int64 *VmcsPhysicalAddress);
void          __vmx_vmptrst(unsigned __int64 *VmcsPhysicalAddress);

#endif

#if !ML_AMD64

    #define _InterlockedCompareExchangePointer(Target, Exchange, Comperand) (PVOID)_InterlockedCompareExchange((long *)Target, (long)Exchange, (long)Comperand)

#ifndef _InterlockedExchangePointer
    #define _InterlockedExchangePointer(Target, Value) (PVOID)_InterlockedExchange((long *)Target, (long)Value)
#endif

    #if ML_USER_MODE

    inline long long _InterlockedExchange64(long long volatile *Target, long long Value)
    {
        long long Old;

        do
        {
            Old = *Target;
        } while (_InterlockedCompareExchange64(Target, Value, Old) != Old);

        return Old;
    }

    #endif // r3

#else

    #define _InterlockedCompareExchangePointer(Target, Exchange, Comperand) (PVOID)_InterlockedCompareExchange64((__int64 *)Target, (__int64)Exchange, (__int64)Comperand)
    void * _InterlockedExchangePointer(void * volatile * Target, void * Value);
    void * _InterlockedExchangePointer_acq(void * volatile * Target, void * Value);

#endif

#if MY_OS_WIN32
    #define Interlocked_Increment _InterlockedIncrement
    #define Interlocked_Decrement _InterlockedDecrement
#else
    #define Interlocked_Increment InterlockedIncrement
    #define Interlocked_Decrement InterlockedDecrement
#endif // MY_OS_WIN32

ForceInline LongPtr _InterlockedIncrementPtr(PVoid Addend)
{
#if ML_AMD64
    return _InterlockedIncrement64((PLongPtr)Addend);
#else
    return _InterlockedIncrement((PLongPtr)Addend);
#endif
}

ForceInline LongPtr _InterlockedDecrementPtr(PVoid Addend)
{
#if ML_AMD64
    return _InterlockedDecrement64((PLongPtr)Addend);
#else
    return _InterlockedDecrement((PLongPtr)Addend);
#endif
}

ForceInline BOOLEAN MlBitScanForwardPtr(PULONG Index, ULONG_PTR Mask)
{
#if ML_AMD64

    return _BitScanForward64(Index, Mask);

#else

    return _BitScanForward(Index, Mask);

#endif
}

#pragma intrinsic(_BitScanForward)

_ML_C_TAIL_

#endif // _INTRINSIC_H_725e6954_2a61_467e_9a57_c634deb44f80