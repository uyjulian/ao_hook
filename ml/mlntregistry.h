#ifndef _MLNTREGISTRY_H_ffe728ea_b2d9_416c_bf5b_aea45e0fe4fc_
#define _MLNTREGISTRY_H_ffe728ea_b2d9_416c_bf5b_aea45e0fe4fc_


ML_NAMESPACE_BEGIN(Reg);

#if !defined(HKEY_CURRENT_USER_LOCAL_SETTINGS)
    #define HKEY_CURRENT_USER_LOCAL_SETTINGS    (( HKEY ) (ULONG_PTR)((LONG)0x80000007) )
#endif

#if ML_KERNEL_MODE

typedef HANDLE HKEY;

#define HKEY_CLASSES_ROOT                   (( HKEY ) (ULONG_PTR)((LONG)0x80000000) )
#define HKEY_CURRENT_USER                   (( HKEY ) (ULONG_PTR)((LONG)0x80000001) )
#define HKEY_LOCAL_MACHINE                  (( HKEY ) (ULONG_PTR)((LONG)0x80000002) )
#define HKEY_USERS                          (( HKEY ) (ULONG_PTR)((LONG)0x80000003) )
#define HKEY_PERFORMANCE_DATA               (( HKEY ) (ULONG_PTR)((LONG)0x80000004) )
#define HKEY_PERFORMANCE_TEXT               (( HKEY ) (ULONG_PTR)((LONG)0x80000050) )
#define HKEY_PERFORMANCE_NLSTEXT            (( HKEY ) (ULONG_PTR)((LONG)0x80000060) )
#define HKEY_CURRENT_CONFIG                 (( HKEY ) (ULONG_PTR)((LONG)0x80000005) )
#define HKEY_DYN_DATA                       (( HKEY ) (ULONG_PTR)((LONG)0x80000006) )
#define HKEY_CURRENT_USER_LOCAL_SETTINGS    (( HKEY ) (ULONG_PTR)((LONG)0x80000007) )

#endif // r0

#define HKEY_MACHINE_CLASS                  (( HKEY ) (ULONG_PTR)((LONG)0x80000020) )

PVOID
AllocateKeyInfo(
    ULONG_PTR Size
);

VOID
FreeKeyInfo(
    PVOID Info
);

NTSTATUS
OpenPredefinedKeyHandle(
    PHANDLE     KeyHandle,
    HANDLE      PredefinedKey,
    ACCESS_MASK DEF_VAL(DesiredAccess, KEY_ALL_ACCESS)
);

NTSTATUS
OpenKey(
    PHANDLE     KeyHandle,
    HANDLE      hKey,
    ACCESS_MASK DesiredAccess,
    PCWSTR      SubKey
);

NTSTATUS
CloseKeyHandle(
    HANDLE KeyHandle
);

NTSTATUS
GetKeyValue(
    HANDLE                      hKey,
    PCWSTR                      SubKey,
    PCWSTR                      ValueName,
    KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    PVOID                       KeyValueInformation,
    ULONG                       Length,
    PULONG                      DEF_VAL(ResultLength, NULL),
    ULONG                       DEF_VAL(Flags, 0)
);

NTSTATUS
GetKeyValue(
    HANDLE                          hKey,
    PCWSTR                          SubKey,
    PCWSTR                          ValueName,
    PKEY_VALUE_PARTIAL_INFORMATION* Value
);

NTSTATUS
SetKeyValue(
    HANDLE      hKey,
    PCWSTR      SubKey,
    PCWSTR      ValueName,
    ULONG       ValueType,
    LPCVOID     ValueData,
    DWORD       ValueDataLength,
    ULONG_PTR   Flags = 0
);

NTSTATUS
DeleteKey(
    HANDLE      hKey,
    PCWSTR      SubKey,
    ULONG_PTR   Flags = 0
);

NTSTATUS
DeleteKeyValue(
    HANDLE      hKey,
    PCWSTR      SubKey,
    PCWSTR      ValueName,
    ULONG_PTR   Flags = 0
);

ML_NAMESPACE_END_(Reg);

#endif // _MLNTREGISTRY_H_ffe728ea_b2d9_416c_bf5b_aea45e0fe4fc_