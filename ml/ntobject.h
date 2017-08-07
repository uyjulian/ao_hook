#ifndef _NTOBJECT_H_44725294_0381_41e9_872d_92a0dd9f3ac0_
#define _NTOBJECT_H_44725294_0381_41e9_872d_92a0dd9f3ac0_


typedef struct
{
    ULONG           Attributes;
    ACCESS_MASK     DesiredAccess;
    ULONG           HandleCount;
    ULONG           ReferenceCount;
    ULONG           PagedPoolUsage;
    ULONG           NonPagedPoolUsage;
    ULONG           Reserved[3];
    ULONG           NameInformationLength;
    ULONG           TypeInformationLength;
    ULONG           SecurityDescriptorLength;
    LARGE_INTEGER   CreationTime;

} OBJECT_BASIC_INFORMATION, *POBJECT_BASIC_INFORMATION;

typedef struct
{
    UNICODE_STRING  Name;
    WCHAR           Buffer[MAX_NTPATH];

} OBJECT_NAME_INFORMATION2, *POBJECT_NAME_INFORMATION2;

typedef struct _OBJECT_TYPE_INFORMATION
{
    UNICODE_STRING      TypeName;
    ULONG               TotalNumberOfObjects;
    ULONG               TotalNumberOfHandles;
    ULONG               TotalPagedPoolUsage;
    ULONG               TotalNonPagedPoolUsage;
    ULONG               TotalNamePoolUsage;
    ULONG               TotalHandleTableUsage;
    ULONG               HighWaterNumberOfObjects;
    ULONG               HighWaterNumberOfHandles;
    ULONG               HighWaterPagedPoolUsage;
    ULONG               HighWaterNonPagedPoolUsage;
    ULONG               HighWaterNamePoolUsage;
    ULONG               HighWaterHandleTableUsage;
    ULONG               InvalidAttributes;
    GENERIC_MAPPING     GenericMapping;
    ULONG               ValidAccessMask;
    BOOLEAN             SecurityRequired;
    BOOLEAN             MaintainHandleCount;
    ULONG               PoolType;
    ULONG               DefaultPagedPoolCharge;
    ULONG               DefaultNonPagedPoolCharge;

} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

#if CPP_DEFINED

typedef struct OBJECT_TYPE_INFORMATION2 : public OBJECT_TYPE_INFORMATION
{
    OBJECT_TYPE_INFORMATION Type;
    WCHAR TypeNameBuffer[MAX_NTPATH];

} OBJECT_TYPE_INFORMATION2, *POBJECT_TYPE_INFORMATION2;

#endif // c++

typedef struct
{
    ULONG                   NumberOfObjectsTypes;
    OBJECT_TYPE_INFORMATION TypeInformation[1];

} OBJECT_TYPES_INFORMATION, *POBJECT_TYPES_INFORMATION;

typedef struct
{
    BOOLEAN InheritHandle;
    BOOLEAN ProtectFromClose;

} OBJECT_HANDLE_FLAG_INFORMATION, *POBJECT_HANDLE_FLAG_INFORMATION;

#if ML_USER_MODE
    typedef struct _OBJECT_TYPE *POBJECT_TYPE;
#endif // r3

typedef struct _OBJECT_TYPE_INITIALIZER *POBJECT_TYPE_INITIALIZER;

NATIVE_API
NTSTATUS
NTAPI
NtSetInformationObject(
    IN  HANDLE                      ObjectHandle,
    IN  OBJECT_INFORMATION_CLASS    ObjectInformationClass,
    IN  PVOID                       ObjectInformation,
    IN  ULONG                       Length
);

NATIVE_API
NTSTATUS
NTAPI
NtDuplicateObject(
    IN  HANDLE      SourceProcessHandle,
    IN  HANDLE      SourceHandle,
    IN  HANDLE      TargetProcessHandle OPTIONAL,
    OUT PHANDLE     TargetHandle OPTIONAL,
    IN  ACCESS_MASK DesiredAccess,
    IN  ULONG       HandleAttributes,
    IN  ULONG       Options
);

NATIVE_API
NTSTATUS
NTAPI
ZwDuplicateObject(
    IN  HANDLE      SourceProcessHandle,
    IN  HANDLE      SourceHandle,
    IN  HANDLE      TargetProcessHandle OPTIONAL,
    OUT PHANDLE     TargetHandle OPTIONAL,
    IN  ACCESS_MASK DesiredAccess,
    IN  ULONG       HandleAttributes,
    IN  ULONG       Options
);

NATIVE_API
NTSTATUS
NTAPI
ZwQueryObject(
    HANDLE                      Handle,
    OBJECT_INFORMATION_CLASS    ObjectInformationClass,
    PVOID                       ObjectInformation,
    ULONG                       ObjectInformationLength,
    PULONG                      ReturnLength
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryObject(
    HANDLE                      Handle,
    OBJECT_INFORMATION_CLASS    ObjectInformationClass,
    PVOID                       ObjectInformation,
    ULONG                       ObjectInformationLength,
    PULONG                      ReturnLength
);

NATIVE_API
NTSTATUS
NTAPI
NtClose(
    IN HANDLE Handle
);

NATIVE_API
NTSTATUS
NTAPI
ZwClose(
    IN HANDLE Handle
);

#if ML_KERNEL_MODE

NTKRNLAPI
NTSTATUS
ObCreateObjectType(
    IN  PUNICODE_STRING             TypeName,
    IN  POBJECT_TYPE_INITIALIZER    ObjectTypeInitializer,
    IN  PSECURITY_DESCRIPTOR        SecurityDescriptor OPTIONAL,
    OUT POBJECT_TYPE*               ObjectType
);

NTSTATUS
NTAPI
ObCreateObject(
    IN      KPROCESSOR_MODE     ProbeMode           OPTIONAL,
    IN      POBJECT_TYPE        ObjectType,
    IN      POBJECT_ATTRIBUTES  ObjectAttributes    OPTIONAL,
    IN      KPROCESSOR_MODE     OwnershipMode,
    IN OUT  PVOID               ParseContext        OPTIONAL,
    IN      ULONG               ObjectBodySize,
    IN      ULONG               PagedPoolCharge     OPTIONAL,
    IN      ULONG               NonPagedPoolCharge  OPTIONAL,
    OUT     PVOID*              Object
);

NTKERNELAPI
NTSTATUS
NTAPI
ObCloseHandle(
    IN HANDLE           Handle,
    IN KPROCESSOR_MODE  PreviousMode
);

NTKERNELAPI
POBJECT_TYPE
NTAPI
ObGetObjectType(
    IN PVOID Object
);

//
//  A function to enumerate through the handle table of a process using a
//  callback.
//

typedef struct HANDLE_TABLE *PHANDLE_TABLE;
typedef struct HANDLE_TABLE_ENTRY *PHANDLE_TABLE_ENTRY;

typedef
BOOLEAN
(NTAPI
*EX_ENUMERATE_HANDLE_ROUTINE)(
    IN PHANDLE_TABLE_ENTRY  HandleTableEntry,
    IN HANDLE               Handle,
    IN PVOID                EnumParameter
);

typedef
BOOLEAN
(NTAPI
*EX_ENUMERATE_HANDLE_ROUTINE_WIN8)(
    IN PHANDLE_TABLE        HandleTable,
    IN PHANDLE_TABLE_ENTRY  HandleTableEntry,
    IN HANDLE               Handle,
    IN PVOID                EnumParameter
);

NTKERNELAPI
BOOLEAN
NTAPI
ExEnumHandleTable(
    IN  PHANDLE_TABLE               HandleTable,
    IN  EX_ENUMERATE_HANDLE_ROUTINE EnumHandleProcedure,
    IN  PVOID                       EnumParameter,
    OUT PHANDLE                     Handle OPTIONAL
);

#endif // r0

#endif // _NTOBJECT_H_44725294_0381_41e9_872d_92a0dd9f3ac0_