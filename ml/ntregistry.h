#ifndef _NTREGISTRY_H_0aecc908_7e96_445f_a102_94f7cf820671_
#define _NTREGISTRY_H_0aecc908_7e96_445f_a102_94f7cf820671_


#if ML_USER_MODE

typedef enum
{
    KeyBasicInformation             = 0,
    KeyNodeInformation              = 1,
    KeyFullInformation              = 2,
    KeyNameInformation              = 3,
    KeyCachedInformation            = 4,
    KeyFlagsInformation             = 5,
    KeyVirtualizationInformation    = 6,
    KeyHandleTagsInformation        = 7,

    MaxKeyInfoClass,

} KEY_INFORMATION_CLASS;

typedef struct _KEY_NAME_INFORMATION {
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
} KEY_NAME_INFORMATION, *PKEY_NAME_INFORMATION;

typedef struct _KEY_WRITE_TIME_INFORMATION {
    LARGE_INTEGER LastWriteTime;
} KEY_WRITE_TIME_INFORMATION, *PKEY_WRITE_TIME_INFORMATION;

typedef struct _KEY_WOW64_FLAGS_INFORMATION {
    ULONG   UserFlags;
} KEY_WOW64_FLAGS_INFORMATION, *PKEY_WOW64_FLAGS_INFORMATION;

typedef struct _KEY_HANDLE_TAGS_INFORMATION {
    ULONG   HandleTags;
} KEY_HANDLE_TAGS_INFORMATION, *PKEY_HANDLE_TAGS_INFORMATION;

typedef struct _KEY_CONTROL_FLAGS_INFORMATION {
    ULONG   ControlFlags;
} KEY_CONTROL_FLAGS_INFORMATION, *PKEY_CONTROL_FLAGS_INFORMATION;



typedef enum _KEY_SET_INFORMATION_CLASS {
    KeyWriteTimeInformation,
    KeyWow64FlagsInformation,
    KeyControlFlagsInformation,
    KeySetVirtualizationInformation,
    KeySetDebugInformation,
    KeySetHandleTagsInformation,
    MaxKeySetInfoClass  // MaxKeySetInfoClass should always be the last enum
} KEY_SET_INFORMATION_CLASS;

typedef struct _KEY_SET_VIRTUALIZATION_INFORMATION {
    ULONG   VirtualTarget           : 1; // Tells if the key is a virtual target key.
    ULONG   VirtualStore            : 1; // Tells if the key is a virtual store key.
    ULONG   VirtualSource           : 1; // Tells if the key has been virtualized at least one (virtual hint)
    ULONG   Reserved                : 29;
} KEY_SET_VIRTUALIZATION_INFORMATION, *PKEY_SET_VIRTUALIZATION_INFORMATION;


//
// Value entry query structures
//

typedef enum _KEY_VALUE_INFORMATION_CLASS
{
    KeyValueBasicInformation,
    KeyValueFullInformation,
    KeyValuePartialInformation,
    KeyValueFullInformationAlign64,
    KeyValuePartialInformationAlign64,
    MaxKeyValueInfoClass  // MaxKeyValueInfoClass should always be the last enum

} KEY_VALUE_INFORMATION_CLASS;

typedef struct _KEY_VALUE_BASIC_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable size
} KEY_VALUE_BASIC_INFORMATION, *PKEY_VALUE_BASIC_INFORMATION;

typedef struct _KEY_VALUE_FULL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataOffset;
    ULONG   DataLength;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable size
//          Data[1];            // Variable size data not declared
} KEY_VALUE_FULL_INFORMATION, *PKEY_VALUE_FULL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataLength;
    UCHAR   Data[1];            // Variable size
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION_ALIGN64 {
    ULONG   Type;
    ULONG   DataLength;
    UCHAR   Data[1];            // Variable size
} KEY_VALUE_PARTIAL_INFORMATION_ALIGN64, *PKEY_VALUE_PARTIAL_INFORMATION_ALIGN64;

typedef struct _KEY_VALUE_ENTRY
{
    PUNICODE_STRING ValueName;
    ULONG           DataLength;
    ULONG           DataOffset;
    ULONG           Type;
} KEY_VALUE_ENTRY, *PKEY_VALUE_ENTRY;

typedef struct _KEY_BASIC_INFORMATION
{
    LARGE_INTEGER   LastWriteTime;
    ULONG           TitleIndex;
    ULONG           NameLength;
    WCHAR           Name[1];            // Variable length string

} KEY_BASIC_INFORMATION, *PKEY_BASIC_INFORMATION;

#endif // r3

NATIVE_API
NTSTATUS
NTAPI
RtlOpenCurrentUser(
    IN  ACCESS_MASK DesiredAccess,
    OUT PHANDLE     CurrentUserKey
);

NATIVE_API
NTSTATUS
NTAPI
RtlFormatCurrentUserKeyPath(
    OUT PUNICODE_STRING CurrentUserKeyPath
);

NATIVE_API
NTSTATUS
NTAPI
NtOpenKey(
    OUT PHANDLE             KeyHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes
);

NATIVE_API
NTSTATUS
NTAPI
ZwOpenKey(
    OUT PHANDLE             KeyHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes
);

NATIVE_API
NTSTATUS
NTAPI
NtOpenKeyEx(
    OUT PHANDLE             KeyHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    IN  ULONG               OpenOptions
);

NATIVE_API
NTSTATUS
NTAPI
NtCreateKey(
    OUT PHANDLE             KeyHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    IN  ULONG               TitleIndex,
    IN  PUNICODE_STRING     Class OPTIONAL,
    IN  ULONG               CreateOptions,
    OUT PULONG              Disposition OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
ZwCreateKey(
    OUT PHANDLE             KeyHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    IN  ULONG               TitleIndex,
    IN  PUNICODE_STRING     Class OPTIONAL,
    IN  ULONG               CreateOptions,
    OUT PULONG              Disposition OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtCreateKeyTransacted(
    OUT PHANDLE             KeyHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    IN  ULONG               TitleIndex OPTIONAL,
    IN  PUNICODE_STRING     Class OPTIONAL,
    IN  ULONG               CreateOptions,
    IN  HANDLE              TransactionHandle,
    OUT PULONG              Disposition OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryKey(
    IN  HANDLE                  KeyHandle,
    IN  KEY_INFORMATION_CLASS   KeyInformationClass,
    OUT PVOID                   KeyInformation,
    IN  ULONG                   Length,
    OUT PULONG                  ResultLength
);

NATIVE_API
NTSTATUS
NTAPI
ZwQueryKey(
    IN  HANDLE                  KeyHandle,
    IN  KEY_INFORMATION_CLASS   KeyInformationClass,
    OUT PVOID                   KeyInformation,
    IN  ULONG                   Length,
    OUT PULONG                  ResultLength
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryValueKey(
    HANDLE                      KeyHandle,
    PUNICODE_STRING             ValueName,
    KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    PVOID                       KeyValueInformation,
    ULONG                       Length,
    PULONG                      ResultLength
);

NATIVE_API
NTSTATUS
NTAPI
ZwQueryValueKey(
    HANDLE                      KeyHandle,
    PUNICODE_STRING             ValueName,
    KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    PVOID                       KeyValueInformation,
    ULONG                       Length,
    PULONG                      ResultLength
);

NATIVE_API
NTSTATUS
NTAPI
NtSetValueKey(
    IN  HANDLE          KeyHandle,
    IN  PUNICODE_STRING ValueName,
    IN  ULONG           TitleIndex OPTIONAL,
    IN  ULONG           ValueType,
    IN  PVOID           ValueData OPTIONAL,
    IN  ULONG           ValueDataSize
);

NATIVE_API
NTSTATUS
NTAPI
ZwSetValueKey(
    IN  HANDLE          KeyHandle,
    IN  PUNICODE_STRING ValueName,
    IN  ULONG           TitleIndex OPTIONAL,
    IN  ULONG           ValueType,
    IN  PVOID           ValueData OPTIONAL,
    IN  ULONG           ValueDataSize
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryMultipleValueKey(
    IN      HANDLE              KeyHandle,
    IN OUT  PKEY_VALUE_ENTRY    ValueEntries,
    IN      ULONG               EntryCount,
    OUT     PVOID               ValueBuffer,
    IN OUT  PULONG              BufferLength,
    OUT     PULONG              RequiredBufferLength OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
ZwQueryMultipleValueKey(
    IN      HANDLE              KeyHandle,
    IN OUT  PKEY_VALUE_ENTRY    ValueEntries,
    IN      ULONG               EntryCount,
    OUT     PVOID               ValueBuffer,
    IN OUT  PULONG              BufferLength,
    OUT     PULONG              RequiredBufferLength OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtDeleteKey(
    IN HANDLE KeyHandle
);

NATIVE_API
NTSTATUS
NTAPI
ZwDeleteKey(
    IN HANDLE KeyHandle
);

NATIVE_API
NTSTATUS
NTAPI
NtDeleteValueKey(
    IN  HANDLE          KeyHandle,
    IN  PUNICODE_STRING ValueName
);

NATIVE_API
NTSTATUS
NTAPI
ZwDeleteValueKey(
    IN  HANDLE          KeyHandle,
    IN  PUNICODE_STRING ValueName
);

#endif // _NTREGISTRY_H_0aecc908_7e96_445f_a102_94f7cf820671_