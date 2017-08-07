#ifndef _NTPO_H_87cbac49_8d82_445e_8ac6_19ad5037eeb4_
#define _NTPO_H_87cbac49_8d82_445e_8ac6_19ad5037eeb4_


#if ML_KERNEL_MODE

#define ES_SYSTEM_REQUIRED      ((ULONG)0x00000001)
#define ES_DISPLAY_REQUIRED     ((ULONG)0x00000002)
#define ES_USER_PRESENT         ((ULONG)0x00000004)
#define ES_AWAYMODE_REQUIRED    ((ULONG)0x00000004)
#define ES_CONTINUOUS           ((ULONG)0x80000000)

typedef ULONG EXECUTION_STATE;

#endif // r0

NTSTATUS
NTAPI
NtSetThreadExecutionState(
    IN  EXECUTION_STATE     Flags,
    OUT EXECUTION_STATE*    PreviousFlags
);

#endif // _NTPO_H_87cbac49_8d82_445e_8ac6_19ad5037eeb4_