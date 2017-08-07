#ifndef _CONFIG_H_2a678057_bcdf_4132_9bc3_d6048ac24434
#define _CONFIG_H_2a678057_bcdf_4132_9bc3_d6048ac24434

#if (defined(_MSC_VER) && _MSC_VER >= 1300)
    #pragma once
#endif

#if defined(__INTEL_COMPILER)
    #define ML_COMPILER_INTEL __INTEL_COMPILER
    #define ML_COMPILER_MSC 1700
    #define MY_COMPILER_MSC ML_COMPILER_MSC

#elif defined(_MSC_VER)
    #define ML_COMPILER_MSC _MSC_VER
    #define MY_COMPILER_MSC ML_COMPILER_MSC

#endif // compiler

#if ML_COMPILER_MSC >= 1500
    #define MY_MSC_1500 1
    #define SUPPORT_VA_ARGS_MACRO 1
#endif  // MY_COMPILER_MSC >= 1500

#if ML_COMPILER_MSC >= 1700
    #define ML_SUPPORT_LAMBDA 1
#endif // ML_SUPPORT_LAMBDA

#if defined(UNICODE) || defined(_UNICODE)
    #define MY_UNICODE_ENABLE 1
#else
    #define MY_UNICODE_ENABLE 0
#endif // set unicode flag

#if !defined(WINCE)
    #define MY_OS_WIN32 1
#elif defined(_WIN32_WCE)
    #define MY_OS_WINCE 1
#endif // MY_OS


#if MY_OS_WIN32

#if !defined(_WIN32_WINNT)
    #define _WIN32_WINNT 0x0503
#endif /* _WIN32_WINNT */

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && defined(_M_IX86)
    #define _X86_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && defined(_M_AMD64)
    #define _AMD64_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && defined(_M_M68K)
    #define _68K_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && defined(_M_MPPC)
    #define _MPPC_
#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_M_IX86) && !defined(_AMD64_) && defined(_M_IA64)
#if !defined(_IA64_)
    #define _IA64_
#endif /* !_IA64_ */
#endif

#ifndef _MAC
#if defined(_68K_) || defined(_MPPC_)
    #define _MAC
#endif
#endif /* _MAC */

#if defined(_AMD64_)

    #define MY_X64 1
    #define ML_AMD64    1

#elif defined(_X86_)

    #define ML_X86 1
    #define ML_I386 1

#endif

#endif // MY_OS_WIN32

/************************************************************************/
/*  compiler settings                                                   */
/************************************************************************/
#pragma warning(disable:4530 4100 4101 4731 4740 4146 4214 4201 4750 4457 4458 4477 4459 4456)

#if MY_MSC_1500
    #pragma warning(suppress:4530)
#endif  // MY_MSC_1500

#if defined(ML_COMPILER_INTEL)
    #pragma warning(disable:7 157 170 181 186 869 1899 2557)
#endif

#if MY_COMPILER_MSC < 1300
    #define for if(1) for
#endif /* fix vc6 for bug */

#ifndef CPP_DEFINED
    #if defined(__cplusplus)
        #define CPP_DEFINED 1
    #else
        #define CPP_DEFINED 0
    #endif
#endif // CPP_DEFINED

#if defined(__cplusplus_cli)
    #define CPP_CLI_DEFINED 1
#else
    #define CPP_CLI_DEFINED 0
#endif // cpp cli

#if !ML_DISABLE_THIRD_LIB

    #define INCLUDE_LIB(_lib) comment(lib, #_lib)

#else

    #define INCLUDE_LIB(_lib) once

#endif // ML_DISABLE_THIRD_LIB

#define USE_STACK_FRAME_POINTER   "y", off
#define UNUSE_STACK_FRAME_POINTER "y", on

#if !defined(IF_EXIST)
    #define IF_EXIST __if_exists
#endif /* IF_EXIST */

#if !defined(IF_NOT_EXIST)
    #define IF_NOT_EXIST __if_not_exists
#endif /* IF_NOT_EXIST */

#if !defined(typedef_s)
    #define typedef_s(s, d) IF_NOT_EXIST(d) { typedef s d; }

    #if SUPPORT_VA_ARGS_MACRO
        #define typedef_s2(s, ...) IF_NOT_EXIST(d) { typedef s __VA_ARGS__; }
    #endif  // SUPPORT_VA_ARGS_MACRO

#endif /* typedef_s */

#ifndef EXTCPP
    #if CPP_DEFINED
        #define EXTCPP extern "C++"
    #else
        #define EXTCPP
    #endif
#endif /* EXTCPP */

#ifndef NOVTABLE
    #define NOVTABLE __declspec(novtable)
#endif /* NOVTABLE */

#if ML_AMD64

#ifndef ASM
    #define ASM
#endif /* ASM */

#ifndef NAKED
    #define NAKED
#endif /* ASM */

#if !defined(INLINE_ASM)
    #if defined(ML_COMPILER_INTEL)
        #define INLINE_ASM __asm
    #else
        #define INLINE_ASM
    #endif
#endif /* INLINE_ASM */

#else // x86

#ifndef ASM
    #define ASM __declspec(naked)
#endif /* ASM */

#ifndef NAKED
    #define NAKED __declspec(naked)
#endif /* ASM */

#if !defined(INLINE_ASM)
    #define INLINE_ASM __asm
#endif /* INLINE_ASM */

#endif // x64 x86

#ifndef MY_DLL_EXPORT
    #define MY_DLL_EXPORT __declspec(dllexport)
#endif  /* MY_DLL_EXPORT */

#ifdef _MY_NTLIB_DLL_
    #define MY_NTLIB_DLL_EXPORT __declspec(dllexport)
#else
    #define MY_NTLIB_DLL_EXPORT
#endif  /* _MY_NTLIB_DLL_ */

#ifndef MY_DLL_IMPORT
    #define MY_DLL_IMPORT __declspec(dllimport)
#endif /* MY_DLL_IMPORT */

#if !defined(MY_CRT_IMPORT)
    #if defined(_DLL)
        #define MY_IMPORT MY_DLL_IMPORT
    #else
        #define MY_IMPORT
    #endif
#endif /* MY_CRT_IMPORT */

#define MY_LIB_NAME MY_LIB

/* C++ 下的函数修饰 */
#if  CPP_DEFINED
    #define _ML_C_HEAD_     extern "C" {
    #define _ML_C_TAIL_     }
    #define _ML_CPP_HEAD_     extern "C++" {
    #define _ML_CPP_TAIL_     }
    #define ML_NAMESPACE_BEGIN(name) namespace name {
    #define ML_NAMESPACE_END }
    #define ML_NAMESPACE_END_(name) }
    #define DEFAULT_VALUE(type, var, value) type var = value
#else
    #define _ML_C_HEAD_
    #define _ML_C_TAIL_
    #define _ML_CPP_HEAD_
    #define _ML_CPP_TAIL_
    #define ML_NAMESPACE_BEGIN(name)
    #define ML_NAMESPACE_END
    #define DEFAULT_VALUE(type, var, value) type var
#endif /* _ML_C_HEAD_ */

#define ML_NAMESPACE ML_NAMESPACE_BEGIN(ml)

#define MY_NAMESPACE_BEGIN  ML_NAMESPACE_BEGIN
#define MY_NAMESPACE_END    ML_NAMESPACE_END

#ifndef ForceInline
    #if (MY_COMPILER_MSC >= 1200)
        #define ForceInline __forceinline
    #else
        #define ForceInline __inline
    #endif
#endif /* ForceInline */

#ifndef NoInline
    #if (MY_COMPILER_MSC >= 1300)
        #define NoInline __declspec(noinline)
    #else
        #define NoInline
    #endif
#endif /* NoInline */

#if (defined(_M_IX86) || defined(_M_IA64) || defined(_M_AMD64)) && !defined(MIDL_PASS)
    #define DECL_IMPORT __declspec(dllimport)
    #define DECL_EXPORT __declspec(dllexport)
#else
    #define DECL_IMPORT
    #define DECL_EXPORT
#endif

#if !defined(DISABLE_SEH)

    #define SEH_TRY     __try
    #define SEH_FINALLY __finally
    #define SEH_LEAVE   __leave

    #if SUPPORT_VA_ARGS_MACRO
        #define SEH_EXCEPT(...)  __except(__VA_ARGS__)
    #else
        #define SEH_EXCEPT  __except
    #endif  // SUPPORT_VA_ARGS_MACRO

#else

    #define SEH_TRY
    #define SEH_FINALLY
    #define SEH_LEAVE

    #if SUPPORT_VA_ARGS_MACRO
        #define SEH_EXCEPT(...) for (int __c_false = 0; __c_false; )
    #else
        #define SEH_EXCEPT()
    #endif  // SUPPORT_VA_ARGS_MACRO

#endif // DISABLE_SEH

#ifndef EXTC
    #if CPP_DEFINED
        #define EXTC extern "C"
        #define EXTC_IMPORT EXTC __declspec(dllimport)
        #define EXTC_EXPORT EXTC DECL_EXPORT
    #else
        #define EXTC
        #define EXTC_IMPORT __declspec(dllimport)
    #endif
#endif /* EXTC */

#ifndef DECL_NORETURN
    #if (MY_COMPILER_MSC >= 1200) && !defined(MIDL_PASS)
        #define DECL_NORETURN   __declspec(noreturn)
    #else
        #define DECL_NORETURN
    #endif
#endif

#ifndef DECL_NOTHROW
    #if (MY_COMPILER_MSC >= 1200) && !defined(MIDL_PASS)
        #define DECL_NOTHROW   __declspec(nothrow)
    #else
        #define DECL_NOTHROW
    #endif
#endif

#ifndef DECL_ALIGN
    #if (MY_COMPILER_MSC >= 1300) && !defined(MIDL_PASS)
        #define DECL_ALIGN(x)   __declspec(align(x))
    #else
        #define DECL_ALIGN(x)
    #endif
#endif

#ifndef SYSTEM_CACHE_ALIGNMENT_SIZE
    #if defined(_AMD64_) || defined(_X86_)
        #define SYSTEM_CACHE_ALIGNMENT_SIZE 64
    #else
        #define SYSTEM_CACHE_ALIGNMENT_SIZE 128
    #endif
#endif

#ifndef DECL_CACHEALIGN
    #define DECL_CACHEALIGN DECL_ALIGN(SYSTEM_CACHE_ALIGNMENT_SIZE)
#endif

#ifndef DECL_UUID
    #if (MY_COMPILER_MSC >= 1100) && CPP_DEFINED
        #define DECL_UUID(x)    __declspec(uuid(x))
    #else
        #define DECL_UUID(x)
    #endif
#endif

#ifndef DECL_NOVTABLE
    #if (MY_COMPILER_MSC >= 1100) && CPP_DEFINED
        #define DECL_NOVTABLE   __declspec(novtable)
    #else
        #define DECL_NOVTABLE
    #endif
#endif

#ifndef DECL_SELECTANY
    #if (MY_COMPILER_MSC >= 1100)
        #define DECL_SELECTANY  __declspec(selectany)
    #else
        #define DECL_SELECTANY
    #endif
#endif

#ifndef NOP_FUNCTION
    #if (MY_COMPILER_MSC >= 1210)
        #define NOP_FUNCTION __noop
    #else
        #define NOP_FUNCTION (void)0
    #endif
#endif

#ifndef DECL_ADDRSAFE
    #if (MY_COMPILER_MSC >= 1200) && (defined(_M_ALPHA) || defined(_M_AXP64))
        #define DECL_ADDRSAFE  __declspec(address_safe)
    #else
        #define DECL_ADDRSAFE
    #endif
#endif

#ifndef DECL_DEPRECATED
    #if (MY_COMPILER_MSC >= 1300) && !defined(MIDL_PASS)
        #define DECL_DEPRECATED   __declspec(deprecated)
        #define DEPRECATE_SUPPORTED
    #else
        #define DECL_DEPRECATED
        #undef  DEPRECATE_SUPPORTED
    #endif
#endif

#if !defined(DECL_THREAD) && (defined(MY_COMPILER_MSC) || defined(__INTEL_COMPILER))
    #define DECL_THREAD __declspec(thread)
#endif // DECL_THREAD

#ifndef UNUSED_VARIABLE
    #define UNUSED_VARIABLE(Var) IF_EXIST(Var) { (Var); }
#endif // UNUSED_VARIABLE

#ifndef True
    #define True 1l
#endif /* True */

#ifndef False
    #define False 0l
#endif /* False */

#ifndef NULL
    #define NULL 0l
#endif /* NULL */

#ifdef CDECL
    #undef CDECL
#endif /* CDECL */
#define CDECL __cdecl

#ifdef STDCALL
    #undef STDCALL
#endif /* STDCALL */
#define STDCALL __stdcall

#ifdef FASTCALL
    #undef FASTCALL
#endif /* STDCALL */

#if ML_AMD64 || CPP_CLI_DEFINED
    #define FASTCALL
#else
    #define FASTCALL __fastcall
#endif

#ifdef THISCALL
    #undef THISCALL
#endif

#ifdef ML_COMPILER_MSC
    #define THISCALL __thiscall
#else
    #define THISCALL
#endif

#ifndef OPTIONAL
    #define OPTIONAL
#endif /* OPTIONAL */

#ifndef RESERVED
    #define RESERVED
#endif /* RESERVED */

#if ML_USER_MODE

#if !defined(ARGUMENT_PRESENT)
    #define ARGUMENT_PRESENT(ArgumentPointer) ((CHAR *)((ULONG_PTR)(ArgumentPointer)) != (CHAR *)(NULL) )
    #define ARGUMENT_NOT_PRESENT(ArgumentPointer) (!ARGUMENT_PRESENT(ArgumentPointer))
#endif

#endif


#if ML_COMPILER_MSC

#define PROPERTY(type, name) \
    __declspec(property(put = __property_set_##name, get = __property_get_##name ) ) type name; \
    typedef type __property_type_##name

#define READONLY_PROPERTY(type, name) \
    __declspec(property(get = __property_get_##name) ) type name; \
    typedef type __property_type_##name; \
    GET(name)

#define WRITEONLY_PROPERTY(type, name) \
    __declspec(property(put = __property_set_##name) ) type name; \
    typedef type property__tmp_type_##name; \
    SET(name)

#define GET(name) __property_type_##name __property_get_##name()
#define SET(name) void __property_set_##name(const __property_type_##name & value)

#endif // msc


#if !defined(INTEL_STATIC)
    #if defined(__INTEL_COMPILER)
        #define INTEL_STATIC static
    #else
        #define INTEL_STATIC
    #endif
#endif /* INTEL_STATIC */

#if !defined(MY_W64)
    #if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && (_MSC_VER >= 1300)
        #define MY_W64 __w64
    #else
        #define MY_W64
    #endif
#endif

#endif // _CONFIG_H_2a678057_bcdf_4132_9bc3_d6048ac24434
