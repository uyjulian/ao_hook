#ifndef _APIHELPER_H_ff88b0e9_19eb_4ea3_8c2c_99541c492e06
#define _APIHELPER_H_ff88b0e9_19eb_4ea3_8c2c_99541c492e06

#include "ml/nativeapihelper.h"
#include "ml/memoryallocator.h"

#pragma warning(disable:4127)

_ML_C_HEAD_

/************************************************************************
  misc
************************************************************************/

ForceInline ULONG HashAPI(PCChar pszName)
{
    ULONG Hash = 0;

    while (*(PByte)pszName)
    {
        Hash = _rotl(Hash, 0x0D) ^ *(PByte)pszName++;
    }

    return Hash;
}

ForceInline ULONG HashAPILower(PCChar pszName)
{
    ULONG Hash = 0;

    while (*(PByte)pszName)
    {
        BYTE ch = *(PByte)pszName++;
        Hash = _rotl(Hash, 0x0D) ^ CHAR_LOWER(ch);
    }

    return Hash;
}

ForceInline ULONG HashAPI2(PCChar pszName)
{
    ULONG Hash = 0;

    while (*(PByte)pszName)
    {
        Hash = _rotr(Hash, 9) + (ULONG)(*pszName++) + 0x9E370001;
    }

    return Hash;
}

ForceInline ULONG HashAPIUpper(PCChar pszName)
{
    ULONG ch, Hash = 0;

    while (*(PByte)pszName)
    {
        ch = *(PByte)pszName++;
        Hash = _rotl(Hash, 0x0D) ^ CHAR_UPPER(ch);
    }

    return Hash;
}

ForceInline ULONG HashAPI2Upper(PCChar pszName)
{
    ULONG ch, Hash = 0;

    while (*(PByte)pszName)
    {
        ch = *(PByte)pszName++;
        Hash = _rotr(Hash, 9) + CHAR_UPPER(ch) + 0x9E370001;
    }

    return Hash;
}

inline ULONG HashUnicodeUpper(PCWSTR Unicode, ULONG DEF_VAL(Hash, 0))
{
    WCHAR ch;

    do
    {
        ch = *Unicode++;
        Hash = Hash * 0x1003F + CHAR_UPPER(ch);
    } while (ch != 0);

    return Hash;
}

inline ULONG HashUnicodeLower(PCWSTR Unicode, ULONG DEF_VAL(Hash, 0))
{
    ULONG ch;

    ch = *Unicode++;

    while (ch)
    {
        Hash = Hash * 0x1003F + CHAR_LOWER(ch);
        ch = *Unicode++;
    }

    return Hash;
}

inline ULONG HashAnsiLower(PCSTR Ansi, ULONG DEF_VAL(Hash, 0))
{
    ULONG ch;

    ch = *(PByte)Ansi++;

    while (ch)
    {
        Hash = Hash * 0x1003F + CHAR_LOWER(ch);
        ch = *(PByte)Ansi++;
    }

    return Hash;
}

#if CPP_DEFINED

EXTCPP
template<class StrType>
inline ULONG_PTR HashStringLowerT(StrType *String, ULONG_PTR Hash = 0)
{
    StrType ch;

    ch = *String++;

    while (ch)
    {
        Hash = Hash * 0x1003F + CHAR_LOWER(ch);
        ch = *String++;
    }

    return Hash;
}

EXTCPP
template<class StrType>
inline ULONG_PTR HashStringT(StrType *String, ULONG_PTR Hash = 0)
{
    StrType ch = *String++;

    while (ch)
    {
        Hash = Hash * 0x1003F + ch;

        switch (sizeof(ch))
        {
            case sizeof(CHAR):
                *(PCHAR)&ch = *String++;
                break;

            case sizeof(WCHAR):
            *(PWSTR)&ch = *String++;
        }
    }

    return Hash;
}

#endif // CPP_DEFINED

#if ML_KERNEL_MODE

NTSTATUS
ProbeForReadSafe(
    PVOID   Address,
    SIZE_T  Length,
    ULONG   Alignment
);

NTSTATUS
ProbeForWriteSafe(
    PVOID   Address,
    SIZE_T  Length,
    ULONG   Alignment
);

NTSTATUS
MmProbeAndLockPagesSafe(
    PMDL            MemoryDescriptorList,
    KPROCESSOR_MODE AccessMode,
    LOCK_OPERATION  Operation
);

#else   // else if !MY_NT_DDK

/************************************************************************
  user mode
************************************************************************/

NTSTATUS CreateMiniDump(PEXCEPTION_POINTERS ExceptionPointers);

/************************************************************************
  directory
************************************************************************/

BOOL    IsPathExistsW(LPCWSTR Path);
BOOL    IsPathExistsA(LPCSTR Path);
BOOL    CreateDirectoryRecursiveA(LPCSTR PathName);
BOOL    CreateDirectoryRecursiveW(LPCWSTR PathName);
ULONG   GetExeDirectoryW(LPWSTR Path, ULONG BufferCount);
ULONG   GetExeDirectoryA(LPSTR Path, ULONG BufferCount);


enum
{
    EDF_SUBDIR      = 0x00000001,
    EDF_BEFORE      = 0x00000002,
    EDF_AFTER       = 0x00000004,
    EDF_PROCDIR     = 0x00000008,
};

typedef
LONG
(STDCALL *EnumDirectoryFilesCallBackRoutine)(
    PVOID               Buffer,
    PWIN32_FIND_DATAW   FindData,
    ULONG_PTR           Context
);

#define EnumFilesM(Buffer, FindData, Context) [] (PVOID Buffer, PWIN32_FIND_DATAW FindData, ULONG_PTR Context) -> LONG

typedef EnumDirectoryFilesCallBackRoutine FEnumDirectoryFilesCallBack;

typedef struct
{
    ULONG                               Flags;
    HANDLE                              hHeap;
    PVOID                               lpBuffer;
    PVOID                               lpOutBuffer;
    LPCWSTR                             pszFilter;
    ULONG                               ElemSize;
    LARGE_INTEGER                       ElemCount;
    LARGE_INTEGER                       MaxCount;
    EnumDirectoryFilesCallBackRoutine   CallBack;
    ULONG_PTR                           Context;
    WIN32_FIND_DATAW                    wfd;
    WCHAR                               SymbolicLinkPath[MAX_NTPATH];
} ENUM_DIRECTORY_INFO;

BOOL EnumDirectoryFilesWorker(LPWSTR lpPath, ENUM_DIRECTORY_INFO *pFindInfo);
BOOL EnumDirectoryFilesFree(PVOID lpBuffer);

BOOL
EnumDirectoryFiles(
    PVOID                              *lpFilesBuffer,
    LPCWSTR                             pszFilter,
    ULONG                               ElemSize,
    LPCWSTR                             pszPath,
    PLARGE_INTEGER                      pElemCount,
    EnumDirectoryFilesCallBackRoutine   CallBack,
    ULONG_PTR                           Context,
    ULONG                               Flags = 0
);

/************************************************************************
  command line
************************************************************************/

typedef struct
{
    int newmode;
} __my_startupinfo;

EXTC_IMPORT void CDECL __getmainargs (int *argc, char ***argv, char ***envp, int, __my_startupinfo *);
EXTC_IMPORT void CDECL __wgetmainargs(int *argc, wchar_t ***argv, wchar_t ***envp, int, __my_startupinfo *);

#ifndef UNICODE
    #define getmainargs __getmainargs
#else
    #define getmainargs __wgetmainargs
#endif /* UNICODE */

/*
    argret = __wgetmainargs(&argc, &argv, &envp,
                            _dowildcard, &startinfo);
#else
    argret = __getmainargs(&argc, &argv, &envp,
                           _dowildcard, &startinfo);
*/

#if USE_NT_VER

#define getargsW(pargc, pargv) (*(pargv)) = CmdLineToArgvW(Nt_GetCommandLine(), (pargc))

#else // !USE_NT_VER

#define getargsA(pargc, pargv) \
            { \
                Char **__envp__;int __dowildcard = 0;__my_startupinfo _my_startupinfo;\
                *(pargc) = 0; \
                __getmainargs((int *)(pargc), (pargv), (&__envp__), __dowildcard, &_my_startupinfo); \
            }

#define getargsW(pargc, pargv) \
            { \
                WChar **__envp__;int __dowildcard = 0;__my_startupinfo _my_startupinfo; \
                *(pargc) = 0; \
                __wgetmainargs((int *)(pargc), (pargv), (&__envp__), __dowildcard, &_my_startupinfo); \
            }

#endif  // USE_NT_VER

#if MY_UNICODE_ENABLE
    #define getargs getargsW
#else
    #define getargs getargsA
#endif

#if defined(_MT) && !defined(_DLL)
    #undef getargs
    #undef getargsA
    #undef getargsW
    #define getargs(a, b)
    #define getargsA getargs
    #define getargsW getargs
#endif

LONG_PTR    FASTCALL CmdLineToArgvWorkerA(LPSTR pszCmdLine, LPSTR *pArgv, PLONG_PTR pCmdLineLength);
PSTR*       FASTCALL CmdLineToArgvA(LPSTR pszCmdLine, PLONG_PTR pArgc);
LONG_PTR    FASTCALL CmdLineToArgvWorkerW(LPWSTR pszCmdLine, LPWSTR *pArgv, PLONG_PTR pCmdLineLength);
PWSTR*      FASTCALL CmdLineToArgvW(LPWSTR pszCmdLine, PLONG_PTR pArgc);

inline VOID ReleaseArgv(PVOID Argv)
{
    FreeMemory(Argv, 0);
}

ForceInline PTChar* FASTCALL CmdLineToArgv(PTChar pszCmdLine, PLONG_PTR pArgc)
{
    if (sizeof(*pszCmdLine) == sizeof(Char))
        return (PTChar *)CmdLineToArgvA((LPSTR)pszCmdLine, pArgc);
    else
        return (PTChar *)CmdLineToArgvW((LPWSTR)pszCmdLine, pArgc);
}

#endif  // MY_NT_DDK

_ML_C_TAIL_

#pragma warning(default:4127)

#endif // _APIHELPER_H_ff88b0e9_19eb_4ea3_8c2c_99541c492e06