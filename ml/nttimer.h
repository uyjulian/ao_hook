#ifndef _NTTIMER_H_5e54d33a_0623_48d6_be4c_a11f0207d94a_
#define _NTTIMER_H_5e54d33a_0623_48d6_be4c_a11f0207d94a_


NATIVE_API
NTSTATUS
NTAPI
NtQueryTimerResolution(
    OUT PULONG MaximumTime,
    OUT PULONG MinimumTime,
    OUT PULONG CurrentTime
);

NATIVE_API
NTSTATUS
NTAPI
NtSetTimerResolution(
    IN  ULONG   DesiredResolution,
    IN  BOOLEAN SetResolution,
    OUT PULONG  CurrentResolution
);

/*++

DesiredAccess
    Access mask for TimerHandle. Can be set of (from <WinNT.h>):

    TIMER_QUERY_STATE
    TIMER_MODIFY_STATE
    TIMER_ALL_ACCESS

--*/

NATIVE_API
NTSTATUS
NTAPI
NtCreateTimer(
    OUT PHANDLE             TimerHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  TIMER_TYPE          TimerType
);

typedef
VOID
(NTAPI
*PTIMER_APC_ROUTINE)(
    IN PVOID    TimerContext,
    IN ULONG    TimerLowValue,
    IN LONG     TimerHighValue
);


/*++

TimerHandle
    HANDLE to Timer Object opened with TIMER_MODIFY_STATE access.

DueTime
    Time when timer should be set, in 100ns units. If it is negative value, it means relative time.

TimerApcRoutine
    User's APC routine, defined as follows:

TimerContext
    Optional parameter to TimerApcRoutine.

ResumeTimer
    If set, Power Management restores system to normal mode when timer is signaled.

Period
    If zero, timer is set only once. Else will be set periodic in time intervals defined in Period value (in 100ms units).

PreviousState
    Optional pointer to value receiving state of Timer Object before NtSetTimer call.

--*/

NATIVE_API
NTSTATUS
NTAPI
NtSetTimer(
    IN  HANDLE              TimerHandle,
    IN  PLARGE_INTEGER      DueTime,
    IN  PTIMER_APC_ROUTINE  TimerApcRoutine OPTIONAL,
    IN  PVOID               TimerContext OPTIONAL,
    IN  BOOLEAN             ResumeTimer,
    IN  LONG                Period OPTIONAL,
    OUT PBOOLEAN            PreviousState OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryTimer(
    IN  HANDLE                  TimerHandle,
    IN  TIMER_INFORMATION_CLASS TimerInformationClass,
    OUT PVOID                   TimerInformation,
    IN  ULONG                   TimerInformationLength,
    OUT PULONG                  ReturnLength OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtCancelTimer(
    IN  HANDLE      TimerHandle,
    OUT PBOOLEAN    CurrentState OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
RtlCreateTimer(
    HANDLE                  TimerQueue,
    PHANDLE                 NewTimer,
    WAITORTIMERCALLBACKFUNC Callback,
    PVOID                   Parameter,
    ULONG                   DueTime,
    ULONG                   Period,
    ULONG                   Flags
);

#endif // _NTTIMER_H_5e54d33a_0623_48d6_be4c_a11f0207d94a_