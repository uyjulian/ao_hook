#ifndef _CONSOLE_H_82cc62c0_f9e1_426c_836e_059a18d387e9
#define _CONSOLE_H_82cc62c0_f9e1_426c_836e_059a18d387e9


#if !defined(FILE_DEVICE_CONSOLE)
    #define FILE_DEVICE_CONSOLE             0x00000050
#endif

_ML_C_HEAD_

CHAR        ConsoleReadChar();
VOID        ClearConsoleScreen();
VOID        PauseConsole(PCWSTR DEF_VAL(PauseText, nullptr));
ULONG_PTR   PrintConsoleA(PCSTR Format, ...);
ULONG_PTR   PrintConsoleW(PCWSTR Format, ...);
ULONG_PTR   PrintConsole(PCWSTR Format, ...);

_ML_C_TAIL_

#endif // _CONSOLE_H_82cc62c0_f9e1_426c_836e_059a18d387e9