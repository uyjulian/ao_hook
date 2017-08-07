#ifndef _NTTOKEN_H_f4dd851c_2e83_4d40_9dbf_e2adf1753014_
#define _NTTOKEN_H_f4dd851c_2e83_4d40_9dbf_e2adf1753014_


#if ML_KERNEL_MODE

#if !defined(_NTIFS_)

/*
typedef enum _TOKEN_ELEVATION_TYPE {
    TokenElevationTypeDefault = 1,
    TokenElevationTypeFull,
    TokenElevationTypeLimited,
} TOKEN_ELEVATION_TYPE, *PTOKEN_ELEVATION_TYPE;
*/

typedef enum _TOKEN_INFORMATION_CLASS
{
    TokenUser                              = 1,
    TokenGroups                            = 2,
    TokenPrivileges                        = 3,
    TokenOwner                             = 4,
    TokenPrimaryGroup                      = 5,
    TokenDefaultDacl                       = 6,
    TokenSource                            = 7,
    TokenType                              = 8,
    TokenImpersonationLevel                = 9,
    TokenStatistics                        = 10,
    TokenRestrictedSids                    = 11,
    TokenSessionId                         = 12,
    TokenGroupsAndPrivileges               = 13,
    TokenSessionReference                  = 14,
    TokenSandBoxInert                      = 15,
    TokenAuditPolicy                       = 16,
    TokenOrigin                            = 17,
    TokenElevationType                     = 18,
    TokenLinkedToken                       = 19,
    TokenElevation                         = 20,
    TokenHasRestrictions                   = 21,
    TokenAccessInformation                 = 22,
    TokenVirtualizationAllowed             = 23,
    TokenVirtualizationEnabled             = 24,
    TokenIntegrityLevel                    = 25,
    TokenUIAccess                          = 26,
    TokenMandatoryPolicy                   = 27,
    TokenLogonSid                          = 28,
    TokenIsAppContainer                    = 29,
    TokenCapabilities                      = 30,
    TokenAppContainerSid                   = 31,
    TokenAppContainerNumber                = 32,
    TokenUserClaimAttributes               = 33,
    TokenDeviceClaimAttributes             = 34,
    TokenRestrictedUserClaimAttributes     = 35,
    TokenRestrictedDeviceClaimAttributes   = 36,
    TokenDeviceGroups                      = 37,
    TokenRestrictedDeviceGroups            = 38,
    MaxTokenInfoClass                      = 39,

} TOKEN_INFORMATION_CLASS, *PTOKEN_INFORMATION_CLASS;

#endif // _NTIFS_

#else // r3

NATIVE_API
NTSTATUS
NTAPI
RtlAdjustPrivilege(
    ULONG       Privilege,
    BOOLEAN     Enable,
    BOOLEAN     CurrentThread,
    PBOOLEAN    Enabled
);

NATIVE_API
BOOLEAN
NTAPI
RtlEqualSid (
    IN PSID Sid1,
    IN PSID Sid2
);

#endif // ML_KERNEL_MODE

NATIVE_API
NTSTATUS
NTAPI
NtCreateToken(
    OUT PHANDLE                 TokenHandle,
    IN  ACCESS_MASK             DesiredAccess,
    IN  POBJECT_ATTRIBUTES      ObjectAttributes OPTIONAL,
    IN  TOKEN_TYPE              TokenType,
    IN  PLUID                   AuthenticationId,
    IN  PLARGE_INTEGER          ExpirationTime,
    IN  PTOKEN_USER             User,
    IN  PTOKEN_GROUPS           Groups,
    IN  PTOKEN_PRIVILEGES       Privileges,
    IN  PTOKEN_OWNER            Owner OPTIONAL,
    IN  PTOKEN_PRIMARY_GROUP    PrimaryGroup,
    IN  PTOKEN_DEFAULT_DACL     DefaultDacl OPTIONAL,
    IN  PTOKEN_SOURCE           TokenSource
);

NATIVE_API
NTSTATUS
NTAPI
NtOpenProcessToken(
    IN  HANDLE  ProcessHandle,
    IN  ULONG   DesiredAccess,
    OUT PHANDLE TokenHandle
);

NATIVE_API
NTSTATUS
NTAPI
ZwOpenProcessToken(
    IN  HANDLE  ProcessHandle,
    IN  ULONG   DesiredAccess,
    OUT PHANDLE TokenHandle
);

NATIVE_API
NTSTATUS
NTAPI
ZwOpenProcessToken(
    IN  HANDLE  ProcessHandle,
    IN  ULONG   DesiredAccess,
    OUT PHANDLE TokenHandle
);

NATIVE_API
NTSTATUS
NTAPI
NtOpenThreadToken(
    IN  HANDLE      ThreadHandle,
    IN  ACCESS_MASK DesiredAccess,
    IN  BOOLEAN     OpenAsSelf,
    OUT PHANDLE     TokenHandle
);

NATIVE_API
NTSTATUS
NTAPI
NtDuplicateToken(
    IN  HANDLE              ExistingTokenHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    IN  BOOLEAN             EffectiveOnly,
    IN  TOKEN_TYPE          TokenType,
    OUT PHANDLE             NewTokenHandle
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryInformationToken(
    IN  HANDLE                  TokenHandle,
    IN  TOKEN_INFORMATION_CLASS TokenInformationClass,
    IN  PVOID                   TokenInformation,
    IN  ULONG                   TokenInformationLength,
    OUT PULONG                  ReturnLength
);

NATIVE_API
NTSTATUS
NTAPI
NtSetInformationToken(
    IN  HANDLE TokenHandle,
    IN  TOKEN_INFORMATION_CLASS TokenInformationClass,
    IN  PVOID TokenInformation,
    IN  ULONG TokenInformationLength
);

NATIVE_API
NTSTATUS
NTAPI
NtAdjustPrivilegesToken(
    IN  HANDLE              TokenHandle,
    IN  BOOLEAN             DisableAllPrivileges,
    IN  PTOKEN_PRIVILEGES   NewState OPTIONAL,
    IN  ULONG               BufferLength,
    OUT PTOKEN_PRIVILEGES   PreviousState,
    OUT PULONG              ReturnLength
);

NATIVE_API
NTSTATUS
NTAPI
ZwAdjustPrivilegesToken(
    IN  HANDLE              TokenHandle,
    IN  BOOLEAN             DisableAllPrivileges,
    IN  PTOKEN_PRIVILEGES   NewState OPTIONAL,
    IN  ULONG               BufferLength,
    OUT PTOKEN_PRIVILEGES   PreviousState,
    OUT PULONG              ReturnLength
);

#endif // _NTTOKEN_H_f4dd851c_2e83_4d40_9dbf_e2adf1753014_