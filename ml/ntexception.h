#ifndef _NTEXCEPTION_H_a57b7443_d72e_4c1e_a4b5_15c7e4c037dd_
#define _NTEXCEPTION_H_a57b7443_d72e_4c1e_a4b5_15c7e4c037dd_


BOOLEAN
NTAPI
RtlDispatchException(
    PEXCEPTION_RECORD   ExceptionRecord,
    PCONTEXT            Context
);

#if ML_KERNEL_MODE

typedef struct _KTRAP_FRAME32
{
/* 0x000 */ ULONG                          DbgEbp;
/* 0x004 */ ULONG                          DbgEip;
/* 0x008 */ ULONG                          DbgArgMark;
/* 0x00C */ ULONG                          DbgArgPointer;
/* 0x010 */ USHORT                         TempSegCs;
/* 0x012 */ UCHAR                          Logging;
/* 0x013 */ UCHAR                          Reserved;
/* 0x014 */ ULONG                          TempEsp;
/* 0x018 */ ULONG                          Dr0;
/* 0x01C */ ULONG                          Dr1;
/* 0x020 */ ULONG                          Dr2;
/* 0x024 */ ULONG                          Dr3;
/* 0x028 */ ULONG                          Dr6;
/* 0x02C */ ULONG                          Dr7;
/* 0x030 */ ULONG                          SegGs;
/* 0x034 */ ULONG                          SegEs;
/* 0x038 */ ULONG                          SegDs;
/* 0x03C */ ULONG                          Edx;
/* 0x040 */ ULONG                          Ecx;
/* 0x044 */ ULONG                          Eax;
/* 0x048 */ ULONG                          PreviousPreviousMode;
/* 0x04C */ PEXCEPTION_REGISTRATION_RECORD ExceptionList;
/* 0x050 */ ULONG                          SegFs;
/* 0x054 */ ULONG                          Edi;
/* 0x058 */ ULONG                          Esi;
/* 0x05C */ ULONG                          Ebx;
/* 0x060 */ ULONG                          Ebp;
/* 0x064 */ ULONG                          ErrCode;
/* 0x068 */ ULONG                          Eip;
/* 0x06C */ ULONG                          SegCs;
/* 0x070 */ ULONG                          EFlags;
/* 0x074 */ ULONG                          HardwareEsp;
/* 0x078 */ ULONG                          HardwareSegSs;
/* 0x07C */ ULONG                          V86Es;
/* 0x080 */ ULONG                          V86Ds;
/* 0x084 */ ULONG                          V86Fs;
/* 0x088 */ ULONG                          V86Gs;

} KTRAP_FRAME32, *PKTRAP_FRAME32;

#if ML_AMD64
#else // 32

typedef PKTRAP_FRAME32 PKTRAP_FRAME;

struct KEXCEPTION_FRAME;

typedef KEXCEPTION_FRAME *PKEXCEPTION_FRAME;

#endif // x86

VOID
NTAPI
KiDispatchException(
    IN      PEXCEPTION_RECORD    ExceptionRecord,
    IN      PKEXCEPTION_FRAME    ExceptionFrame,
    IN OUT  PKTRAP_FRAME         TrapFrame,
    IN      KPROCESSOR_MODE      PreviousMode,
    IN      BOOLEAN              FirstChance
);

#else // r3

NATIVE_API
VOID
NTAPI
KiUserExceptionDispatcher(
    PEXCEPTION_RECORD   ExceptionRecord,
    PCONTEXT            Context
);

NATIVE_API
NTSTATUS
NTAPI
RtlRaiseException(
    PEXCEPTION_RECORD ExceptionRecord
);

NATIVE_API
VOID
NTAPI
RtlSetUnhandledExceptionFilter(
    LPTOP_LEVEL_EXCEPTION_FILTER TopLevelExceptionFilter
);

NATIVE_API
LONG
NTAPI
RtlKnownExceptionFilter(
    PEXCEPTION_POINTERS ExceptionPointers
);

NATIVE_API
PVOID
NTAPI
RtlAddVectoredExceptionHandler(
    IN LONG                         FirstHandler,
    IN PVECTORED_EXCEPTION_HANDLER  Handler
);

NATIVE_API
ULONG
NTAPI
RtlRemoveVectoredExceptionHandler(
    IN PVOID Handler
);

NATIVE_API
PVOID
NTAPI
RtlAddVectoredContinueHandler(
    IN LONG                         FirstHandler,
    IN PVECTORED_EXCEPTION_HANDLER  Handler
);

NATIVE_API
ULONG
NTAPI
RtlRemoveVectoredContinueHandler(
    IN PVOID Handler
);

#endif // !ML_KERNEL_MODE

typedef enum _HARDERROR_RESPONSE_OPTION {
    OptionAbortRetryIgnore      = 0,
    OptionOk                    = 1,
    OptionOkCancel              = 2,
    OptionRetryCancel           = 3,
    OptionYesNo                 = 4,
    OptionYesNoCancel           = 5,
    OptionShutdownSystem        = 6,
    OptionOkNoWait              = 7,
    OptionCancelTryContinue     = 8,

} HARDERROR_RESPONSE_OPTION;

typedef enum _HARDERROR_RESPONSE
{
    ResponseReturnToCaller      = 0,
    ResponseNotHandled          = 1,
    ResponseAbort               = 2,
    ResponseCancel              = 3,
    ResponseIgnore              = 4,
    ResponseNo                  = 5,
    ResponseOk                  = 6,
    ResponseRetry               = 7,
    ResponseYes                 = 8,
    ResponseTryAgain            = 9,
    ResponseContinue            = 10,

} HARDERROR_RESPONSE;

#define HARDERROR_PARAMETERS_FLAGSPOS   4
#define HARDERROR_FLAGS_DEFDESKTOPONLY  0x00020000

#define MAXIMUM_HARDERROR_PARAMETERS    5
#define HARDERROR_OVERRIDE_ERRORMODE    0x10000000

NATIVE_API
NTSTATUS
NTAPI
NtRaiseHardError(
    IN  NTSTATUS    ErrorStatus,
    IN  ULONG       NumberOfParameters,
    IN  ULONG       UnicodeStringParameterMask,
    IN  PULONG_PTR  Parameters,
    IN  ULONG       ValidResponseOptions,
    OUT PULONG      Response
);

NATIVE_API
NTSTATUS
NTAPI
ZwRaiseHardError(
    IN  NTSTATUS    ErrorStatus,
    IN  ULONG       NumberOfParameters,
    IN  ULONG       UnicodeStringParameterMask,
    IN  PULONG_PTR  Parameters,
    IN  ULONG       ValidResponseOptions,
    OUT PULONG      Response
);

NATIVE_API
NTSTATUS
NTAPI
NtRaiseException(
    IN  PEXCEPTION_RECORD   ExceptionRecord,
    IN  PCONTEXT            ContextRecord,
    IN  BOOLEAN             FirstChance
);

NATIVE_API
NTSTATUS
NTAPI
ZwRaiseException(
    IN  PEXCEPTION_RECORD   ExceptionRecord,
    IN  PCONTEXT            ContextRecord,
    IN  BOOLEAN             FirstChance
);

NATIVE_API
NTSTATUS
NTAPI
ExRaiseHardError(
    IN  NTSTATUS    ErrorStatus,
    IN  ULONG       NumberOfParameters,
    IN  ULONG       UnicodeStringParameterMask,
    IN  PULONG_PTR  Parameters,
    IN  ULONG       ValidResponseOptions,
    OUT PULONG      Response
);

#endif // _NTEXCEPTION_H_a57b7443_d72e_4c1e_a4b5_15c7e4c037dd_