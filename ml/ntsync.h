#ifndef _NTSYNC_H_a657b25f_ed76_4374_b4b4_c0281e592892_
#define _NTSYNC_H_a657b25f_ed76_4374_b4b4_c0281e592892_


#ifndef EVENT_QUERY_STATE
    #define EVENT_QUERY_STATE       0x0001
#endif // EVENT_QUERY_STATE

#ifndef EVENT_MODIFY_STATE
    #define EVENT_MODIFY_STATE       0x0002
#endif // EVENT_MODIFY_STATE

//
// Event type
//

typedef enum _EVENT_INFORMATION_CLASS
{
    EventBasicInformation,

} EVENT_INFORMATION_CLASS;

typedef struct _EVENT_BASIC_INFORMATION
{
    EVENT_TYPE  EventType;
    LONG        EventState;
} EVENT_BASIC_INFORMATION, *PEVENT_BASIC_INFORMATION;

typedef enum _MUTANT_INFORMATION_CLASS
{
    MutantBasicInformation,
    MutantOwnerInformation,

} MUTANT_INFORMATION_CLASS;

typedef struct _MUTANT_BASIC_INFORMATION
{
    LONG    CurrentCount;
    BOOLEAN OwnedByCaller;
    BOOLEAN AbandonedState;

} MUTANT_BASIC_INFORMATION, *PMUTANT_BASIC_INFORMATION;

typedef struct _MUTANT_OWNER_INFORMATION
{
    CLIENT_ID ClientId;
} MUTANT_OWNER_INFORMATION, *PMUTANT_OWNER_INFORMATION;


typedef enum _SEMAPHORE_INFORMATION_CLASS
{
    SemaphoreBasicInformation
} SEMAPHORE_INFORMATION_CLASS;

typedef struct _SEMAPHORE_BASIC_INFORMATION
{
    LONG CurrentCount;
    LONG MaximumCount;

} SEMAPHORE_BASIC_INFORMATION, *PSEMAPHORE_BASIC_INFORMATION;

NATIVE_API
NTSTATUS
NTAPI
NtCreateKeyedEvent(
    OUT PHANDLE             KeyedEventHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  ULONG               Reserved
);

NATIVE_API
NTSTATUS
NTAPI
NtOpenKeyedEvent(
    OUT PHANDLE             KeyedEventHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtReleaseKeyedEvent(
    IN HANDLE           KeyedEventHandle,
    IN PVOID            Key,
    IN BOOLEAN          Alertable,
    IN PLARGE_INTEGER   Timeout OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtWaitForKeyedEvent(
    IN HANDLE           KeyedEventHandle,
    IN PVOID            Key,
    IN BOOLEAN          Alertable,
    IN PLARGE_INTEGER   Timeout OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtOpenEvent(
    OUT PHANDLE             EventHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes
);

NATIVE_API
NTSTATUS
NTAPI
ZwOpenEvent(
    OUT PHANDLE             EventHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes
);

NATIVE_API
NTSTATUS
NTAPI
NtCreateEvent(
    OUT PHANDLE             EventHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  EVENT_TYPE          EventType,
    IN  BOOLEAN             InitialState
);

NATIVE_API
NTSTATUS
NTAPI
ZwCreateEvent(
    OUT PHANDLE             EventHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  EVENT_TYPE          EventType,
    IN  BOOLEAN             InitialState
);

NATIVE_API
NTSTATUS
NTAPI
ZwSetEvent(
    HANDLE  EventHandle,
    PLONG   PreviousState
);

NATIVE_API
NTSTATUS
NTAPI
NtSetEvent(
    HANDLE  EventHandle,
    PLONG   PreviousState
);

NATIVE_API
NTSTATUS
NTAPI
NtSetEventBoostPriority(
    IN  HANDLE EventHandle
);

NATIVE_API
NTSTATUS
NTAPI
NtClearEvent(
    IN  HANDLE EventHandle
);

NATIVE_API
NTSTATUS
NTAPI
NtResetEvent(
    IN  HANDLE EventHandle,
    OUT PLONG PreviousState OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtPulseEvent(
    IN  HANDLE EventHandle,
    OUT PLONG PreviousState OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryEvent(
    IN  HANDLE                  EventHandle,
    IN  EVENT_INFORMATION_CLASS EventInformationClass,
    OUT PVOID                   EventInformation,
    IN  ULONG                   EventInformationLength,
    OUT PULONG                  ReturnLength OPTIONAL
);

#define EVENT_PAIR_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE)

NATIVE_API
NTSTATUS
NTAPI
NtCreateEventPair(
    OUT PHANDLE             EventPairHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
ZwCreateEventPair(
    OUT PHANDLE             EventPairHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtOpenEventPair(
    OUT PHANDLE             EventPairHandle,
    IN ACCESS_MASK          DesiredAccess,
    IN POBJECT_ATTRIBUTES   ObjectAttributes
);

NATIVE_API
NTSTATUS
NTAPI
ZwOpenEventPair(
    OUT PHANDLE             EventPairHandle,
    IN ACCESS_MASK          DesiredAccess,
    IN POBJECT_ATTRIBUTES   ObjectAttributes
);

NATIVE_API
NTSTATUS
NTAPI
NtSetLowEventPair(
    IN HANDLE EventPairHandle
);

NATIVE_API
NTSTATUS
NTAPI
ZwSetLowEventPair(
    IN HANDLE EventPairHandle
);

NATIVE_API
NTSTATUS
NTAPI
NtSetHighEventPair(
    IN HANDLE EventPairHandle
);

NATIVE_API
NTSTATUS
NTAPI
ZwSetHighEventPair(
    IN HANDLE EventPairHandle
);

NATIVE_API
NTSTATUS
NTAPI
NtWaitLowEventPair(
    IN HANDLE EventPairHandle
);

NATIVE_API
NTSTATUS
NTAPI
ZwWaitLowEventPair(
    IN HANDLE EventPairHandle
);

NATIVE_API
NTSTATUS
NTAPI
NtWaitHighEventPair(
    IN HANDLE EventPairHandle
);

NATIVE_API
NTSTATUS
NTAPI
ZwWaitHighEventPair(
    IN HANDLE EventPairHandle
);

NATIVE_API
NTSTATUS
NTAPI
NtSetLowWaitHighEventPair(
    IN HANDLE EventPairHandle
);

NATIVE_API
NTSTATUS
NTAPI
ZwSetLowWaitHighEventPair(
    IN HANDLE EventPairHandle
);

NATIVE_API
NTSTATUS
NTAPI
NtSetHighWaitLowEventPair(
    IN HANDLE EventPairHandle
);

NATIVE_API
NTSTATUS
NTAPI
ZwSetHighWaitLowEventPair(
    IN HANDLE EventPairHandle
);

NATIVE_API
NTSTATUS
NTAPI
NtCreateMutant(
    OUT PHANDLE             MutantHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  BOOLEAN             InitialOwner
);

NATIVE_API
NTSTATUS
NTAPI
NtOpenMutant(
    OUT PHANDLE             MutantHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes
);

NATIVE_API
NTSTATUS
NTAPI
NtReleaseMutant(
    IN  HANDLE  MutantHandle,
    OUT PLONG   PreviousCount OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryMutant(
    IN  HANDLE                      MutantHandle,
    IN  MUTANT_INFORMATION_CLASS    MutantInformationClass,
    OUT PVOID                       MutantInformation,
    IN  ULONG                       MutantInformationLength,
    OUT PULONG                      ReturnLength OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtCreateSemaphore(
    OUT PHANDLE             SemaphoreHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  LONG                InitialCount,
    IN  LONG                MaximumCount
);

NATIVE_API
NTSTATUS
NTAPI
NtOpenSemaphore(
    OUT PHANDLE             SemaphoreHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes
);

NATIVE_API
NTSTATUS
NTAPI
NtReleaseSemaphore(
    IN  HANDLE  SemaphoreHandle,
    IN  LONG    ReleaseCount,
    OUT PLONG   PreviousCount OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtQuerySemaphore(
    IN  HANDLE                      SemaphoreHandle,
    IN  SEMAPHORE_INFORMATION_CLASS SemaphoreInformationClass,
    OUT PVOID                       SemaphoreInformation,
    IN  ULONG                       SemaphoreInformationLength,
    OUT PULONG                      ReturnLength OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtDelayExecution(
    IN BOOLEAN          Alertable,
    IN PLARGE_INTEGER   Interval
);

NATIVE_API
NTSTATUS
NTAPI
ZwDelayExecution(
    IN BOOLEAN          Alertable,
    IN PLARGE_INTEGER   Interval
);

NATIVE_API
NTSTATUS
NTAPI
NtWaitForSingleObject(
    IN HANDLE         Handle,
    IN BOOLEAN        Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
ZwWaitForSingleObject(
    IN HANDLE         Handle,
    IN BOOLEAN        Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
);

typedef enum _OBJECT_WAIT_TYPE
{
    WaitAllObject,
    WaitAnyObject

} OBJECT_WAIT_TYPE, *POBJECT_WAIT_TYPE;

NATIVE_API
NTSTATUS
NTAPI
NtWaitForMultipleObjects(
    IN ULONG            ObjectCount,
    IN PHANDLE          ObjectsArray,
    IN OBJECT_WAIT_TYPE WaitType,
    IN BOOLEAN          Alertable,
    IN PLARGE_INTEGER   TimeOut OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtWaitForMultipleObjects32(
    IN ULONG            ObjectCount,
    IN PHANDLE          ObjectsArray,
    IN OBJECT_WAIT_TYPE WaitType,
    IN BOOLEAN          Alertable,
    IN PLARGE_INTEGER   TimeOut OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtTestAlert(
    VOID
);

#if !ML_KERNEL_MODE

NATIVE_API
VOID
NTAPI
RtlInitializeCriticalSectionAndSpinCount(
    LPCRITICAL_SECTION  CriticalSection,
    ULONG               SpinCount
);

NATIVE_API
VOID
NTAPI
RtlDeleteCriticalSection(
    LPCRITICAL_SECTION CriticalSection
);

NATIVE_API
VOID
NTAPI
RtlEnterCriticalSection(
    LPCRITICAL_SECTION CriticalSection
);

NATIVE_API
VOID
NTAPI
RtlLeaveCriticalSection(
    LPCRITICAL_SECTION CriticalSection
);

#endif // !ML_KERNEL_MODE

#endif // _NTSYNC_H_a657b25f_ed76_4374_b4b4_c0281e592892_