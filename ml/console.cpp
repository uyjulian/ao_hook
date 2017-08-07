#include "mylibrary.h"

_ML_C_HEAD_

#if ML_USER_MODE

ULONG_PTR PrintConsoleA(PCSTR Format, ...)
{
    BOOL        Result;
    ULONG       Length;
    CHAR        Buffer[0xF00 / 2];
    va_list     Args;
    HANDLE      StdHandle;

    va_start(Args, Format);
    Length = _vsnprintf(Buffer, countof(Buffer) - 1, Format, Args);
    if (Length == -1)
        return Length;

    StdHandle = Nt_CurrentPeb()->ProcessParameters->StandardOutput;

    if (PtrAnd(StdHandle, 0x10000003) == (HANDLE)3)
    {
        Result = WriteConsoleA(
                    StdHandle,
                    Buffer,
                    Length,
                    &Length,
                    NULL
                 );
    }
    else
    {
        LARGE_INTEGER BytesWritten;

        Result = NT_SUCCESS(NtFileDisk::Write(StdHandle, Buffer, Length, &BytesWritten));
        Length = Result ? (ULONG_PTR)BytesWritten.QuadPart : 0;
    }

    return Result ? Length : 0;
}

ULONG_PTR PrintConsoleW(PCWSTR Format, ...)
{
    return PrintConsole(Format);
}

ULONG_PTR PrintConsole(PCWSTR Format, ...)
{
    BOOL        Success, IsConsole;
    ULONG       Length;
    WCHAR       Buffer[0xF00 / 2];
    va_list     Args;
    HANDLE      StdOutput;

    va_start(Args, Format);
    Length = _vsnwprintf(Buffer, countof(Buffer) - 1, Format, Args);
    if (Length == -1)
        return Length;

    StdOutput = CurrentPeb()->ProcessParameters->StandardOutput;

    IsConsole = TRUE;

    LOOP_ONCE
    {
        IO_STATUS_BLOCK             IoStatus;
        FILE_FS_DEVICE_INFORMATION  VolumeInfo;

        if (PtrAnd(StdOutput, 0x10000003) == (HANDLE)3)
            break;

        IoStatus.Status = NtQueryVolumeInformationFile(
                            StdOutput,
                            &IoStatus,
                            &VolumeInfo,
                            sizeof(VolumeInfo),
                            FileFsDeviceInformation
                        );
        if (NT_FAILED(IoStatus.Status))
            break;

        IsConsole = VolumeInfo.DeviceType == FILE_DEVICE_CONSOLE;
    }

    // if (PtrAnd(StdHandle, (1 << (bitsof(ULONG_PTR) - 4)) | 3) == (HANDLE)3)
    if (IsConsole)
    {
        Success = WriteConsoleW(
                    StdOutput,
                    Buffer,
                    Length,
                    &Length,
                    NULL
                 );
    }
    else
    {
        LARGE_INTEGER BytesWritten;

        Success = NT_SUCCESS(NtFileDisk::Write(StdOutput, Buffer, Length * sizeof(WCHAR), &BytesWritten));
        Length = Success ? ((ULONG_PTR)BytesWritten.QuadPart / sizeof(WCHAR)) : 0;
    }

    return Success ? Length : 0;
}

CHAR ConsoleReadChar()
{
    HANDLE hConsole;
    INPUT_RECORD ir;
    DWORD dwRead;

    hConsole = CreateFileW(
                    L"CONIN$",
                    GENERIC_READ|GENERIC_WRITE,
                    FILE_SHARE_READ|FILE_SHARE_WRITE,
                    NULL,
                    OPEN_EXISTING,
                    0,
                    NULL
                );
    LOOP_FOREVER
    {
        ReadConsoleInputW(hConsole, &ir, 1, &dwRead);
        if (dwRead == 0)
            break;

        if (ir.EventType == KEY_EVENT &&
            ir.Event.KeyEvent.bKeyDown == TRUE &&
            ir.Event.KeyEvent.uChar.UnicodeChar)
        {
            break;
        }
    }

    CloseHandle(hConsole);

    return ir.Event.KeyEvent.uChar.AsciiChar;
}

VOID ClearConsoleScreen()
{
    ULONG                       BytesWritten;
    COORD                       Position;
    HANDLE                      StdOutputHandle;
    CONSOLE_SCREEN_BUFFER_INFO  ScreenBufferInfo;

    StdOutputHandle = Nt_CurrentPeb()->ProcessParameters->StandardOutput;

    GetConsoleScreenBufferInfo(StdOutputHandle, &ScreenBufferInfo);

    Position.X = 0;
    Position.Y = 0;
    FillConsoleOutputCharacterW(StdOutputHandle, ' ', ScreenBufferInfo.dwSize.X * ScreenBufferInfo.dwSize.Y, Position, &BytesWritten);

    SetConsoleCursorPosition(StdOutputHandle, Position);
}

VOID PauseConsole(PCWSTR PauseText)
{
    if (PauseText != NULL)
        PrintConsole(L"%s\n", PauseText);
    ConsoleReadChar();
}

#endif // ML_USER_MODE

_ML_C_TAIL_
