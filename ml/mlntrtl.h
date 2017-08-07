#ifndef _MLNTRTL_H_8ca66ccf_34b3_42f8_8dae_1a65beec3402_
#define _MLNTRTL_H_8ca66ccf_34b3_42f8_8dae_1a65beec3402_


ML_NAMESPACE_BEGIN(Rtl);

inline PWSTR QueryCommandLine()
{
    return Ps::CurrentPeb()->ProcessParameters->CommandLine.Buffer;
}

NTSTATUS
DosPathNameToNtPathName(
    IN  PCWSTR                  DosName,
    OUT PUNICODE_STRING         NtName,
    OUT PCWSTR*                 DEF_VAL(DosFilePath, NULL) OPTIONAL,
    OUT PRTL_RELATIVE_NAME_U    DEF_VAL(FileName, NULL) OPTIONAL
);

NTSTATUS
NtPathNameToDosPathName(
    IN  PUNICODE_STRING DosPath,
    OUT PUNICODE_STRING NtPath
);

NTSTATUS
GetSystemDirectory(
    PUNICODE_STRING Buffer,
    BOOL            Wow64NoRedirect = FALSE
);

NTSTATUS
GetModuleDirectory(
    ml::String& Path,
    PVOID       ModuleBase = nullptr
);

NTSTATUS
GetWorkingDirectory(
    PUNICODE_STRING Buffer
);

NTSTATUS
SetWorkingDirectory(
    PCWSTR PathName
);

NTSTATUS
SetWorkingDirectory(
    PUNICODE_STRING PathName
);

NTSTATUS
SetExeDirectoryAsCurrent(
    VOID
);

BOOL
IsNameInExpression(
    IN PUNICODE_STRING  Expression,
    IN PUNICODE_STRING  Name,
    IN BOOL             DEF_VAL(IgnoreCase, TRUE),
    IN PWSTR            DEF_VAL(UpcaseTable, NULL) OPTIONAL
);

NTSTATUS
EnvironmentAppend(
    PUNICODE_STRING Key,
    PUNICODE_STRING Value
);

inline
BOOL
IsNameInExpression(
    IN PCWSTR   Expression,
    IN PCWSTR   Name,
    IN BOOL     DEF_VAL(IgnoreCase, TRUE),
    IN PWSTR    DEF_VAL(UpcaseTable, NULL) OPTIONAL
)
{
    UNICODE_STRING ExpressionString, NameString;

    RtlInitUnicodeString(&ExpressionString, Expression);
    RtlInitUnicodeString(&NameString, Name);

    return IsNameInExpression(&ExpressionString, &NameString, IgnoreCase, UpcaseTable);
}

ML_NAMESPACE_END_(Rtl);

#endif // _MLNTRTL_H_8ca66ccf_34b3_42f8_8dae_1a65beec3402_