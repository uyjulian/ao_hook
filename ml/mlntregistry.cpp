#include "mylibrary.h"

ML_NAMESPACE

ML_NAMESPACE_BEGIN(Native)


ML_NAMESPACE_BEGIN(Reg)

PVOID AllocateKeyInfo(ULONG_PTR Size)
{
    return AllocateMemoryP(Size);
}

VOID FreeKeyInfo(PVOID Info)
{
    FreeMemoryP(Info);
}

NTSTATUS OpenPredefinedKeyHandle(PHANDLE KeyHandle, HANDLE PredefinedKey, ACCESS_MASK DesiredAccess)
{
    ULONG_PTR           Length;
    NTSTATUS            Status;
    OBJECT_ATTRIBUTES   ObjectAttributes;
    UNICODE_STRING      KeyPath;
    PWSTR               LocalKeyBuffer;

    API_POINTER(ZwOpenKey)  XOpenKey;

    static WCHAR LocalMachineSuffix[] = L"_Classes";
    static WCHAR LocalSettingsSuffix[] = L"\\Local Settings";

    if ((LONG)(LONG_PTR)PredefinedKey >= 0)
        return STATUS_INVALID_HANDLE;

#if ML_KERNEL_MODE

    XOpenKey = ZwOpenKey;

#else

    XOpenKey = NtOpenKey;

#endif


#pragma push_macro("KEY_INDEX")
#define KEY_INDEX(_Key) ((ULONG_PTR)(_Key) & ~(1 << (bitsof(LONG) - 1)))

    Status = STATUS_INVALID_HANDLE;
    switch (KEY_INDEX(PredefinedKey))
    {
        case KEY_INDEX(HKEY_CLASSES_ROOT):
            Status = RtlFormatCurrentUserKeyPath(&KeyPath);
            FAIL_BREAK(Status);

            Length = KeyPath.Length;
            LocalKeyBuffer = (PWSTR)AllocStack(KeyPath.Length + sizeof(LocalMachineSuffix));
            CopyMemory(LocalKeyBuffer, KeyPath.Buffer, Length);
            CopyMemory(PtrAdd(LocalKeyBuffer, Length), LocalMachineSuffix, sizeof(LocalMachineSuffix));

            RtlFreeUnicodeString(&KeyPath);

            KeyPath.Buffer          = LocalKeyBuffer;
            KeyPath.Length          = (USHORT)(Length + CONST_STRLEN(LocalMachineSuffix) * sizeof(WCHAR));
            KeyPath.MaximumLength   = KeyPath.Length;

            InitializeObjectAttributes(&ObjectAttributes, &KeyPath, OBJ_CASE_INSENSITIVE, NULL, NULL);
            Status = XOpenKey(KeyHandle, DesiredAccess, &ObjectAttributes);

            if (NT_SUCCESS(Status))
                break;

            NO_BREAK;

        case KEY_INDEX(HKEY_MACHINE_CLASS):
            InitializeObjectAttributes(&ObjectAttributes, PUSTR(L"\\Registry\\Machine\\Software\\Classes"), OBJ_CASE_INSENSITIVE, NULL, NULL);
            Status = XOpenKey(KeyHandle, DesiredAccess, &ObjectAttributes);
            break;

#if ML_USER_MODE

        case KEY_INDEX(HKEY_CURRENT_USER):
            Status = RtlOpenCurrentUser(DesiredAccess, KeyHandle);
            break;

#endif // r3

        case KEY_INDEX(HKEY_LOCAL_MACHINE):
            InitializeObjectAttributes(&ObjectAttributes, PUSTR(L"\\Registry\\Machine"), OBJ_CASE_INSENSITIVE, NULL, NULL);
            Status = XOpenKey(KeyHandle, DesiredAccess, &ObjectAttributes);
            break;

        case KEY_INDEX(HKEY_USERS):
            InitializeObjectAttributes(&ObjectAttributes, PUSTR(L"\\Registry\\User"), OBJ_CASE_INSENSITIVE, NULL, NULL);
            Status = XOpenKey(KeyHandle, DesiredAccess, &ObjectAttributes);
            break;

        case KEY_INDEX(HKEY_PERFORMANCE_DATA):
        case KEY_INDEX(HKEY_PERFORMANCE_TEXT):
        case KEY_INDEX(HKEY_PERFORMANCE_NLSTEXT):
            *KeyHandle = (HANDLE)PredefinedKey;
            Status = STATUS_SUCCESS;
            break;

        case KEY_INDEX(HKEY_CURRENT_CONFIG):
            InitializeObjectAttributes(&ObjectAttributes, PUSTR(L"\\Registry\\Machine\\System\\CurrentControlSet\\Hardware Profiles\\Current"), OBJ_CASE_INSENSITIVE, NULL, NULL);
            Status = XOpenKey(KeyHandle, DesiredAccess, &ObjectAttributes);
            break;

        case KEY_INDEX(HKEY_CURRENT_USER_LOCAL_SETTINGS):
            Status = RtlFormatCurrentUserKeyPath(&KeyPath);
            FAIL_BREAK(Status);

            Length = KeyPath.Length;
            LocalKeyBuffer = (PWSTR)AllocStack(KeyPath.Length + sizeof(LocalMachineSuffix) + sizeof(LocalSettingsSuffix));
            CopyMemory(LocalKeyBuffer, KeyPath.Buffer, Length);
            CopyMemory(PtrAdd(LocalKeyBuffer, Length), LocalMachineSuffix, CONST_STRLEN(LocalMachineSuffix) * sizeof(WCHAR));
            Length += CONST_STRLEN(LocalMachineSuffix) * sizeof(WCHAR);
            CopyMemory(PtrAdd(LocalKeyBuffer, Length), LocalSettingsSuffix, sizeof(LocalSettingsSuffix));
            Length += CONST_STRLEN(LocalSettingsSuffix) * sizeof(WCHAR);

            RtlFreeUnicodeString(&KeyPath);

            KeyPath.Buffer          = LocalKeyBuffer;
            KeyPath.Length          = (USHORT)Length;
            KeyPath.MaximumLength   = KeyPath.Length;

            InitializeObjectAttributes(&ObjectAttributes, &KeyPath, OBJ_CASE_INSENSITIVE, NULL, NULL);
            Status = XOpenKey(KeyHandle, DesiredAccess, &ObjectAttributes);
            break;

        case KEY_INDEX(HKEY_DYN_DATA):
            Status = STATUS_NOT_IMPLEMENTED;
            break;
    }

#pragma pop_macro("KEY_INDEX")

    return Status;
}

ForceInline NTSTATUS CloseKeyHandle(HANDLE KeyHandle)
{
    if (KeyHandle == nullptr)
        return STATUS_INVALID_HANDLE;

#if ML_KERNEL_MODE
    return ZwClose(KeyHandle);
#else
    return NtClose(KeyHandle);
#endif
}

ForceInline NTSTATUS CloseMappedPredefinedKeyHandle(HANDLE KeyHandle)
{
    return CloseKeyHandle(KeyHandle);
}

NTSTATUS
OpenKey(
    PHANDLE     KeyHandle,
    HANDLE      hKey,
    ACCESS_MASK DesiredAccess,
    PCWSTR      SubKey
)
{
    BOOL                Retry;
    NTSTATUS            Status;
    HANDLE              RootKey;
    UNICODE_STRING      SubKeyString;
    OBJECT_ATTRIBUTES   ObjectAttributes;

    API_POINTER(ZwOpenKey)  XOpenKey;

#if ML_KERNEL_MODE

    XOpenKey = ZwOpenKey;

#else

    XOpenKey = NtOpenKey;

#endif

    RtlInitUnicodeString(&SubKeyString, SubKey);
    Retry = FALSE;

    LOOP_FOREVER
    {
        Status = OpenPredefinedKeyHandle(&RootKey, hKey, DesiredAccess);
        FAIL_RETURN(Status);

        InitializeObjectAttributes(&ObjectAttributes, &SubKeyString, OBJ_CASE_INSENSITIVE, RootKey, nullptr);
        Status = NtOpenKey(KeyHandle, DesiredAccess, &ObjectAttributes);

        CloseKeyHandle(RootKey);

        if (Retry == FALSE && Status == STATUS_OBJECT_NAME_NOT_FOUND && hKey == HKEY_CLASSES_ROOT)
        {
            Retry = TRUE;
            hKey = HKEY_MACHINE_CLASS;
            continue;
        }

        break;
    }

    return Status;
}

NTSTATUS
GetKeyValue(
    HANDLE                          hKey,
    PCWSTR                          SubKey,
    PCWSTR                          ValueName,
    PKEY_VALUE_PARTIAL_INFORMATION* Value
)
{
    NTSTATUS    Status;
    ULONG       Length;
    PKEY_VALUE_PARTIAL_INFORMATION Buffer;

    Buffer = NULL;
    Length = 0;

    LOOP_FOREVER
    {
        Status = GetKeyValue(hKey, SubKey, ValueName, KeyValuePartialInformation, Buffer, Length, &Length, 0);
        if (Status != STATUS_BUFFER_TOO_SMALL)
            break;

        FreeKeyInfo(Buffer);
        Buffer = (PKEY_VALUE_PARTIAL_INFORMATION)AllocateKeyInfo(Length + sizeof(WCHAR));
        if (Buffer == NULL)
            return STATUS_NO_MEMORY;

        *(PWSTR)PtrAdd(Buffer, Length) = 0;
    }

    if (NT_FAILED(Status))
    {
        FreeKeyInfo(Buffer);
    }
    else
    {
        *Value = Buffer;
    }

    return Status;
}

NTSTATUS
GetKeyValue(
    HANDLE                      hKey,
    PCWSTR                      SubKey,
    PCWSTR                      ValueName,
    KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    PVOID                       KeyValueInformation,
    ULONG                       Length,
    PULONG                      ResultLength,
    ULONG                       Flags
)
{
    ULONG               ReturnLength;
    HANDLE              KeyHandle;
    NTSTATUS            Status;
    UNICODE_STRING      SubKeyString, ValueNameString;

    API_POINTER(ZwCreateKey)        XCreateKey;
    API_POINTER(ZwOpenKey)          XOpenKey;
    API_POINTER(ZwQueryValueKey)    XQueryValueKey;

#if ML_KERNEL_MODE

    XCreateKey      = ZwCreateKey;
    XOpenKey        = ZwOpenKey;
    XQueryValueKey  = ZwQueryValueKey;

#else

    XCreateKey      = NtCreateKey;
    XOpenKey        = NtOpenKey;
    XQueryValueKey  = NtQueryValueKey;

#endif

    Status = OpenKey(&KeyHandle, hKey, KEY_QUERY_VALUE, SubKey);
    FAIL_RETURN(Status);

    ReturnLength = 0;
    RtlInitUnicodeString(&ValueNameString, ValueName);
    Status = XQueryValueKey(
                KeyHandle,
                &ValueNameString,
                KeyValueInformationClass,
                KeyValueInformation,
                Length,
                &ReturnLength
             );

    if (KeyHandle != (HANDLE)hKey)
        CloseKeyHandle(KeyHandle);

    if ((NT_SUCCESS(Status) || Status == STATUS_BUFFER_TOO_SMALL) && ResultLength != NULL)
        *ResultLength = ReturnLength;

    return Status;
}

NTSTATUS
SetKeyValue(
    HANDLE      hKey,
    PCWSTR      SubKey,
    PCWSTR      ValueName,
    ULONG       ValueType,
    LPCVOID     ValueData,
    ULONG       ValueDataLength,
    ULONG_PTR   Flags
)
{
    HANDLE              KeyHandle, RootKey;
    NTSTATUS            Status;
    UNICODE_STRING      SubKeyString, ValueNameString;
    OBJECT_ATTRIBUTES   ObjectAttributes;

    API_POINTER(ZwCreateKey)    XCreateKey;
    API_POINTER(ZwSetValueKey)  XSetValueKey;

#if ML_KERNEL_MODE
    XCreateKey = ZwCreateKey;
    XSetValueKey = ZwSetValueKey;
#else
    XCreateKey = NtCreateKey;
    XSetValueKey = NtSetValueKey;
#endif

    RootKey = NULL;

    if (SubKey == NULL || SubKey[0] == 0)
    {
        KeyHandle = (HANDLE)hKey;
    }
    else
    {
        Status = OpenPredefinedKeyHandle(&RootKey, (HKEY)hKey);
        if (!NT_SUCCESS(Status))
            return Status;

        RtlInitUnicodeString(&SubKeyString, SubKey);
        InitializeObjectAttributes(
            &ObjectAttributes,
            &SubKeyString,
            OBJ_CASE_INSENSITIVE,
            RootKey,
            NULL
        );

        Status = XCreateKey(
                    &KeyHandle,
                    KEY_SET_VALUE | Flags,
                    &ObjectAttributes,
                    0,
                    NULL,
                    0,
                    NULL
                 );

        if (Status == STATUS_OBJECT_NAME_NOT_FOUND)
        {
            PWSTR Name, End;

            Name = ObjectAttributes.ObjectName->Buffer;
            End  = PtrAdd(Name, ObjectAttributes.ObjectName->Length);

            while (Name < End)
            {
                if (*Name != '\\')
                {
                    ++Name;
                    continue;
                }

                ObjectAttributes.ObjectName->Length = (USHORT)PtrOffset(Name, ObjectAttributes.ObjectName->Buffer);
                ++Name;

                Status = XCreateKey(
                            &KeyHandle,
                            0x020000000 | Flags,    // MAXIMUM_ALLOWED ?
                            &ObjectAttributes,
                            0,
                            NULL,
                            0,
                            NULL
                         );

                if (!NT_SUCCESS(Status))
                    goto _EXIT;

                CloseKeyHandle(KeyHandle);
            }

            ObjectAttributes.ObjectName->Length = (USHORT)PtrOffset(End, ObjectAttributes.ObjectName->Buffer);
            Status = XCreateKey(
                        &KeyHandle,
                        KEY_SET_VALUE | Flags,
                        &ObjectAttributes,
                        0,
                        NULL,
                        0,
                        NULL
                     );
        }

        if (!NT_SUCCESS(Status))
            goto _EXIT;
    }

    RtlInitUnicodeString(&ValueNameString, ValueName);

    Status = XSetValueKey(
                KeyHandle,
                &ValueNameString,
                0,
                ValueType,
                (PVOID)ValueData,
                ValueDataLength
             );

    if (KeyHandle != (HANDLE)hKey)
        CloseKeyHandle(KeyHandle);

_EXIT:

    if (RootKey != NULL)
        CloseMappedPredefinedKeyHandle(RootKey);

    return Status;
}

NTSTATUS
DeleteKey(
    HANDLE      hKey,
    PCWSTR      SubKey,
    ULONG_PTR   Flags
)
{
    HANDLE              KeyHandle, RootKey;
    NTSTATUS            Status;
    UNICODE_STRING      SubKeyString;
    OBJECT_ATTRIBUTES   ObjectAttributes;

    RootKey = NULL;
    if (SubKey == NULL || SubKey[0] == 0)
    {
        return STATUS_INVALID_PARAMETER;
    }

    Status = OpenPredefinedKeyHandle(&RootKey, (HKEY)hKey);
    FAIL_RETURN(Status);

    LOOP_ONCE
    {
        RtlInitUnicodeString(&SubKeyString, SubKey);
        InitializeObjectAttributes(&ObjectAttributes, &SubKeyString, OBJ_CASE_INSENSITIVE, RootKey, NULL);
        Status = ZwOpenKey(&KeyHandle, KEY_ALL_ACCESS | Flags, &ObjectAttributes);
        if (Status == STATUS_OBJECT_NAME_NOT_FOUND && hKey == HKEY_CLASSES_ROOT)
        {
            Status = DeleteKey(HKEY_MACHINE_CLASS, SubKey);
            break;
        }
        else if (NT_FAILED(Status))
        {
            break;
        }

        Status = ZwDeleteKey(KeyHandle);

        if (KeyHandle != (HANDLE)hKey)
            CloseKeyHandle(KeyHandle);
    }

    if (RootKey != NULL)
        CloseMappedPredefinedKeyHandle(RootKey);

    return Status;
}

NTSTATUS
DeleteKeyValue(
    HANDLE      hKey,
    PCWSTR      SubKey,
    PCWSTR      ValueName,
    ULONG_PTR   Flags
)
{
    HANDLE              KeyHandle, RootKey;
    NTSTATUS            Status;
    UNICODE_STRING      SubKeyString;
    OBJECT_ATTRIBUTES   ObjectAttributes;

    RootKey = NULL;
    if (SubKey == NULL || SubKey[0] == 0)
    {
        KeyHandle = (HANDLE)hKey;
    }
    else
    {
        Status = OpenPredefinedKeyHandle(&RootKey, (HKEY)hKey);
        if (!NT_SUCCESS(Status))
            return Status;

        RtlInitUnicodeString(&SubKeyString, SubKey);
        InitializeObjectAttributes(&ObjectAttributes, &SubKeyString, OBJ_CASE_INSENSITIVE, RootKey, NULL);
        Status = ZwOpenKey(&KeyHandle, KEY_SET_VALUE | Flags, &ObjectAttributes);
        if (NT_FAILED(Status))
            goto _EXIT;
    }

    RtlInitUnicodeString(&SubKeyString, ValueName);
    Status = ZwDeleteValueKey(KeyHandle, &SubKeyString);

    if (KeyHandle != (HANDLE)hKey)
        CloseKeyHandle(KeyHandle);

_EXIT:

    if (RootKey != NULL)
        CloseMappedPredefinedKeyHandle(RootKey);

    return Status;
}

ML_NAMESPACE_END_(Reg);


ML_NAMESPACE_END_(Native)

ML_NAMESPACE_END
