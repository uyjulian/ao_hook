#include "mylibrary.h"

ML_NAMESPACE

ML_NAMESPACE_BEGIN(Native)


ML_NAMESPACE_BEGIN(Exp);

NTSTATUS
ExceptionBox(
    PCWSTR      Text,
    PCWSTR      Title,
    PULONG_PTR  Response,
    ULONG_PTR   Type
)
{
    UNICODE_STRING  HardErrorTitle, HardErrorText;
    ULONG_PTR       HardErrorParameters[3];
    ULONG           LocalResponse;
    NTSTATUS        Status;

    RtlInitUnicodeString(&HardErrorTitle, Title);
    RtlInitUnicodeString(&HardErrorText, Text);

    HardErrorParameters[0] = (ULONG_PTR)&HardErrorText;
    HardErrorParameters[1] = (ULONG_PTR)&HardErrorTitle;
    HardErrorParameters[2] = Type;

#if ML_USER_MODE

    Status = NtRaiseHardError(STATUS_SERVICE_NOTIFICATION, countof(HardErrorParameters), 1 | 2, HardErrorParameters, 0, &LocalResponse);

#elif ML_KERNEL_MODE

    Status = ExRaiseHardError(STATUS_SERVICE_NOTIFICATION, countof(HardErrorParameters), 1 | 2, HardErrorParameters, 0, &LocalResponse);

#endif

    if (NT_SUCCESS(Status) && Response != NULL)
        *Response = LocalResponse;

    return Status;
}

ML_NAMESPACE_END_(Exp);


ML_NAMESPACE_END_(Native)

ML_NAMESPACE_END
