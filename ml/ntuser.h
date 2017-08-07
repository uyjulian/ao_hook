#ifndef _NTUSER_H_4670045a_2124_41ce_9a20_68327cef99f9_
#define _NTUSER_H_4670045a_2124_41ce_9a20_68327cef99f9_


#define WIN32K_API NATIVE_API

#if ML_KERNEL_MODE

    DECLARE_HANDLE            (HWND);

#endif // r0

#if !ML_KERNEL_MODE

/************************************************************************
  user32 undoc api
************************************************************************/

#define RST_DONTATTACHQUEUE     0x00000001
#define RST_DONTJOURNALATTACH   0x00000002

WIN32K_API
VOID
NTAPI
RegisterSystemThread(
    ULONG Flags,
    ULONG Reserve       // must be zero
);

#define SNDMSG_FLAG_ANSICALL    0x00000001

inline
LRESULT
NTAPI
SendMessageWorker(
    HWND    Window,
    UINT    Message,
    WPARAM  wParam,
    LPARAM  lParam,
    ULONG   Flags
)
{
    return 0;
}

inline
HWND
NTAPI
NtUserCreateWindowEx_Win7(
    ULONG                   ExStyle,    // 0x80000000 == CreateWindowExA
    PLARGE_UNICODE_STRING   ClassName,
    PLARGE_UNICODE_STRING   ClassVersion,
    PLARGE_UNICODE_STRING   WindowName,
    ULONG                   Style,
    LONG                    X,
    LONG                    Y,
    LONG                    Width,
    LONG                    Height,
    HWND                    ParentWnd,
    HMENU                   Menu,
    PVOID                   Instance,
    LPVOID                  Param,
    ULONG                   ShowMode,
    ULONG                   Unknown
)
{
    return 0;
}

inline
HWND
NTAPI
NtUserCreateWindowEx_Win8(
    ULONG                   ExStyle,    // 0x80000000 == CreateWindowExA
    PLARGE_UNICODE_STRING   ClassName,
    PLARGE_UNICODE_STRING   ClassVersion,
    PLARGE_UNICODE_STRING   WindowName,
    ULONG                   Style,
    LONG                    X,
    LONG                    Y,
    LONG                    Width,
    LONG                    Height,
    HWND                    ParentWnd,
    HMENU                   Menu,
    PVOID                   Instance,
    LPVOID                  Param,
    ULONG                   ShowMode,
    ULONG                   Unknown,
    ULONG                   Unknown2
)
{
    return 0;
}

inline
LRESULT
NTAPI
NtUserMessageCall(
    IN HWND         hWnd,
    IN UINT         Message,
    IN WPARAM       wParam,
    IN LPARAM       lParam,
    IN ULONG_PTR    xParam,
    IN DWORD        xpfnProc,
    IN BOOL         Ansi
)
{
    return 0;
}

inline
BOOL
NTAPI
NtUserDefSetText(
    HWND                    hWnd,
    PLARGE_UNICODE_STRING   Text
)
{
    return 0;
}

#endif // !ML_KERNEL_MODE

typedef enum _WINDOWINFOCLASS
{
    WindowProcess   = 0,
    WindowThread    = 2,
    WindowIsHung    = 5,

} WINDOWINFOCLASS;

WIN32K_API
HANDLE
NTAPI
NtUserQueryWindow(
    IN HWND             hWnd,
    IN WINDOWINFOCLASS  WindowInfo
);

#endif // _NTUSER_H_4670045a_2124_41ce_9a20_68327cef99f9_