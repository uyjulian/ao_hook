#ifndef _UNDOCUMENTEDAPI_H_b221130a_bdd5_4b40_9da0_d44c29e1d279
#define _UNDOCUMENTEDAPI_H_b221130a_bdd5_4b40_9da0_d44c29e1d279

#pragma INCLUDE_LIB(undoc_k32.lib)

#if !MY_NT_DDK


_ML_C_HEAD_

EXTC_IMPORT HANDLE WINAPI GetConsoleInputWaitHandle();

EXTC_IMPORT
BOOL
WINAPI
CreateProcessInternalA(
    HANDLE                  Token,
    LPCSTR                  ApplicationName,
    LPSTR                   CommandLine,
    LPSECURITY_ATTRIBUTES   ProcessAttributes,
    LPSECURITY_ATTRIBUTES   ThreadAttributes,
    BOOL                    InheritHandles,
    DWORD                   CreationFlags,
    LPVOID                  Environment,
    LPCSTR                  CurrentDirectory,
    LPSTARTUPINFOA          StartupInfo,
    LPPROCESS_INFORMATION   ProcessInformation,
    PHANDLE                 NewToken
);


typedef struct
{
    HANDLE                  Token;
    PCWSTR                  ApplicationName;
    PCWSTR                  CommandLine;
    LPSECURITY_ATTRIBUTES   ProcessAttributes;
    LPSECURITY_ATTRIBUTES   ThreadAttributes;
    BOOL                    InheritHandles;
    DWORD                   CreationFlags;
    LPVOID                  Environment;
    PCWSTR                  CurrentDirectory;
    LPSTARTUPINFOW          StartupInfo;
    LPPROCESS_INFORMATION   ProcessInformation;
    PHANDLE                 NewToken;

} CREATE_PROCESS_INTERNAL_PARAM, *PCREATE_PROCESS_INTERNAL_PARAM;

typedef BOOL (WINAPI *CreateProcessInternal2Routine)(CREATE_PROCESS_INTERNAL_PARAM Parameter);


EXTC_IMPORT
BOOL
WINAPI
CreateProcessInternalW(
    HANDLE                  Token,
    PCWSTR                  ApplicationName,
    PWSTR                   CommandLine,
    LPSECURITY_ATTRIBUTES   ProcessAttributes,
    LPSECURITY_ATTRIBUTES   ThreadAttributes,
    BOOL                    InheritHandles,
    DWORD                   CreationFlags,
    LPVOID                  Environment,
    PCWSTR                  CurrentDirectory,
    LPSTARTUPINFOW          StartupInfo,
    LPPROCESS_INFORMATION   ProcessInformation,
    PHANDLE                 NewToken
);

EXTC_IMPORT
INT
WINAPI
MessageBoxTimeoutA(
    IN HWND     hWnd,
    IN LPCSTR   Text,
    IN LPCSTR   Caption,
    IN UINT     Type,
    IN WORD     LanguageId,
    IN DWORD    Milliseconds
);

EXTC_IMPORT
INT
STDCALL
MessageBoxTimeoutW(
    IN HWND     hWnd,
    IN LPCWSTR  Text,
    IN LPCWSTR  Caption,
    IN UINT     Type,
    IN WORD     anguageId,
    IN DWORD    Milliseconds
);

#if MY_UNICODE_ENABLE
    #define MessageBoxTimeout MessageBoxTimeoutW
#else
    #define MessageBoxTimeout MessageBoxTimeoutA
#endif

_ML_C_TAIL_

#endif // MY_NT_DDK

#endif // _UNDOCUMENTEDAPI_H_b221130a_bdd5_4b40_9da0_d44c29e1d279