#include "mylibrary.h"

ML_NAMESPACE

ML_NAMESPACE_BEGIN(Native)


ML_NAMESPACE_BEGIN(Str)

/************************************************************************
  UNICODE_STRING helper class
************************************************************************/

UnicodeString::UnicodeString()
{
    RtlInitEmptyUnicodeString(this, NULL, 0);
}

UnicodeString::~UnicodeString()
{
    RtlFreeUnicodeString(this);
}

NTSTATUS UnicodeString::Create(PCWSTR Buffer, ULONG_PTR Length)
{
    return RtlSetUnicodeString(this, Buffer, Length);
}

NTSTATUS UnicodeString::Create(PCUNICODE_STRING Buffer)
{
    return Create(Buffer->Buffer, Buffer->Length / sizeof(WCHAR));
}

BOOL UnicodeString::operator==(PCWSTR Buffer) const
{
    UNICODE_STRING String;

    RtlInitUnicodeString(&String, Buffer);

    return RtlEqualUnicodeString(this, &String, FALSE);
}

BOOL UnicodeString::operator==(PCUNICODE_STRING String) const
{
    return RtlEqualUnicodeString(this, String, FALSE);
}

BOOL UnicodeString::operator==(const UnicodeString &String) const
{
    return RtlEqualUnicodeString(this, &String, FALSE);
}

UnicodeString& UnicodeString::operator=(PCWSTR Buffer)
{
    if (*this == Buffer)
        return *this;

    Create(Buffer);

    return *this;
}

UnicodeString& UnicodeString::operator=(PCUNICODE_STRING Buffer)
{
    if (*this == Buffer)
        return *this;

    Create(Buffer);

    return *this;
}

/************************************************************************
  UNICODE_STRING helper class end
************************************************************************/

ML_NAMESPACE_END_(Str);


ML_NAMESPACE_END_(Native)

ML_NAMESPACE_END
