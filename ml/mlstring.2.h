#ifndef _MLSTRING_H_8ceda0f8_e680_4166_a056_6300386e156d_
#define _MLSTRING_H_8ceda0f8_e680_4166_a056_6300386e156d_


ML_NAMESPACE_BEGIN(Str);

class UnicodeString : public UNICODE_STRING
{
protected:

public:
    UnicodeString();
    ~UnicodeString();

    NTSTATUS Create(PCWSTR Buffer, ULONG_PTR Length = -1);
    NTSTATUS Create(PCUNICODE_STRING Buffer);

    BOOL operator==(PCWSTR Buffer) const;
    BOOL operator==(PCUNICODE_STRING String) const;
    BOOL operator==(const UnicodeString &String) const;

    UnicodeString& operator=(PCWSTR Buffer);
    UnicodeString& operator=(PCUNICODE_STRING Buffer);
};

ML_NAMESPACE_END_(Str);

#endif // _MLSTRING_H_8ceda0f8_e680_4166_a056_6300386e156d_