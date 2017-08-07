#ifndef _MLSTRING_H_252d9413_55ca_4d44_976f_c0dcecd5afd4_
#define _MLSTRING_H_252d9413_55ca_4d44_976f_c0dcecd5afd4_

#define STRING_DEBUG 0
#define USE_TEMPLATE 1

#pragma warning(push)
#pragma warning(disable:4172)

template<typename STRING_LENGTH_TYPE = USHORT, typename LARGE_LENGTH_TYPE = ULONG>
class StringImplementT
{
protected:

    typedef typename STRING_LENGTH_TYPE STRING_LENGTH_TYPE;
    typedef typename LARGE_LENGTH_TYPE  LARGE_LENGTH_TYPE;

    typedef WCHAR           STRING_CHAR_TYPE;
    typedef PWSTR           STRING_POINTER_TYPE;
    typedef PCWSTR          STRING_CONST_POINTER_TYPE;

#if USE_TEMPLATE


    typedef struct STRING_TYPE
    {
        STRING_LENGTH_TYPE  Length;
        STRING_LENGTH_TYPE  MaximumLength;
        STRING_POINTER_TYPE Buffer;

    } STRING_TYPE, *PSTRING_TYPE;

    typedef const STRING_TYPE* PCSTRING_TYPE;


#else

    typedef UNICODE_STRING      STRING_TYPE;
    typedef PUNICODE_STRING     PSTRING_TYPE;
    typedef PCUNICODE_STRING    PCSTRING_TYPE;

#endif

    static const LARGE_LENGTH_TYPE  kMaxNumberValue = (LARGE_LENGTH_TYPE)-1ll;
    static const LARGE_LENGTH_TYPE  kCharSize       = sizeof(STRING_CHAR_TYPE);
    static const LARGE_LENGTH_TYPE  kMaxLength      = ((LARGE_LENGTH_TYPE)1 << bitsof(STRING_LENGTH_TYPE)) - 1 - kCharSize;

public:
    static const LARGE_LENGTH_TYPE  kInvalidIndex   = kMaxNumberValue;

protected:

#if USE_TEMPLATE
    template<typename, typename>
#endif

    friend class StringT;

protected:

    STRING_TYPE         String;
    STRING_CHAR_TYPE    Buffer[4];

protected:
    StringImplementT()
    {
        Reset();
    }

    VOID Reset()
    {
        RtlInitEmptyString(&this->String);
    }

    operator PCSTRING_TYPE()
    {
        return &this->String;
    }

    operator PSTRING_TYPE()
    {
        return &this->String;
    }

    LONG Compare(StringImplementT *Impl, BOOL CaseInSensitive = FALSE)
    {
        return Compare(*Impl, CaseInSensitive);
    }

    LONG Compare(STRING_CONST_POINTER_TYPE Str, LARGE_LENGTH_TYPE Count, BOOL CaseInSensitive = FALSE)
    {
        STRING_TYPE String;

        String.Buffer           = (STRING_POINTER_TYPE)Str;
        String.Length           = (STRING_LENGTH_TYPE)CountToLength(Count);
        String.MaximumLength    = String.Length;

        return Compare(&String, CaseInSensitive);
    }

    LONG Compare(PCSTRING_TYPE Str, BOOL CaseInSensitive = FALSE)
    {
        UNICODE_STRING Str1, Str2;

        Str1.Length         = (USHORT)GetLength();
        Str1.MaximumLength  = (USHORT)Str1.Length;
        Str1.Buffer         = GetBuffer();

        Str2.Length         = (USHORT)Str->Length;
        Str2.MaximumLength  = (USHORT)Str->MaximumLength;
        Str2.Buffer         = Str->Buffer;

        return RtlCompareUnicodeString(&Str1, &Str2, (BOOLEAN)CaseInSensitive);
    }

    NoInline NTSTATUS Concat(STRING_CONST_POINTER_TYPE Str, LARGE_LENGTH_TYPE Count)
    {
        FAIL_RETURN(VerifyBufferLength(GetCount() + Count));
        CopyString(Str, Count, TRUE);
        return STATUS_SUCCESS;
    }

    NoInline NTSTATUS CopyFrom(STRING_CONST_POINTER_TYPE Str, LARGE_LENGTH_TYPE Count)
    {
        FAIL_RETURN(VerifyBufferLength(Count));
        CopyString(Str, Count, FALSE);
        return STATUS_SUCCESS;
    }

    LARGE_LENGTH_TYPE FormatCountV(STRING_CONST_POINTER_TYPE Format, va_list Arguments)
    {
        return _vscwprintf(Format, Arguments);
    }

    LARGE_LENGTH_TYPE FormatV(STRING_CONST_POINTER_TYPE Format, va_list Arguments)
    {
        LARGE_LENGTH_TYPE Count;

        Count = _vsnwprintf(GetBuffer(), (size_t)LengthToCount(GetMaxLength()), Format, Arguments);
        this->String.Length = (STRING_LENGTH_TYPE)CountToLength(Count);

        return Count;
    }

    LARGE_LENGTH_TYPE GetLength()
    {
        return this->String.Length;
    }

    VOID SetLength(LARGE_LENGTH_TYPE Length)
    {
        this->String.Length = ML_MIN(this->String.Length, Length);
    }

    LARGE_LENGTH_TYPE GetMaxLength()
    {
        return this->String.MaximumLength;
    }

    LARGE_LENGTH_TYPE GetCount()
    {
        return GetLength() / kCharSize;
    }

    STRING_POINTER_TYPE GetBuffer()
    {
        return this->String.Buffer;
    }

    NoInline LARGE_LENGTH_TYPE IndexOf(STRING_CONST_POINTER_TYPE Str, LARGE_LENGTH_TYPE StartIndex = 0)
    {
        if (StartIndex >= GetCount())
            return kInvalidIndex;

        STRING_CONST_POINTER_TYPE Begin, End, Found;

        Begin = GetBuffer();
        End = PtrAdd(GetBuffer(), GetLength());

        if (Str[0] != 0)
        {
            Found = wcsstr(Begin + StartIndex, Str);
        }
        else
        {
            Found = Begin + StartIndex;
            while (Found <= End && Found[0] != 0)
                ++Found;

            Found = Found < End ? Found : nullptr;
        }

        return Found == nullptr ? kInvalidIndex : Found - Begin;
    }

    BOOL IsNullOrEmpty()
    {
        return GetCount() == 0;
    }

    LARGE_LENGTH_TYPE LastIndexOf(STRING_CONST_POINTER_TYPE Str, LARGE_LENGTH_TYPE StartIndex = 0)
    {
        if (StartIndex >= GetCount())
            return kInvalidIndex;

        STRING_CONST_POINTER_TYPE Begin, End, Found, Prev;

        Begin = GetBuffer();
        End = PtrAdd(GetBuffer(), GetLength());

        Prev = nullptr;
        Begin += StartIndex;
        while (Begin < End)
        {
            Found = wcsstr(Begin, Str);
            if (Found == nullptr)
                break;

            Prev = Found;
            Begin = Found + 1;
        }

        return Prev == nullptr ? kInvalidIndex : Prev - GetBuffer();
    }

    BOOL MatchExpression(PCSTRING_TYPE Expression, BOOL IgnoreCase)
    {
        UNICODE_STRING Expr, Name;

        Expr.Length         = Expression->Length;
        Expr.MaximumLength  = Expr.Length;
        Expr.Buffer         = Expression->Buffer;

        Name.Length         = GetLength();
        Name.MaximumLength  = Name.Length;
        Name.Buffer         = GetBuffer();

        return Rtl::IsNameInExpression(&Expr, &Name, IgnoreCase);
    }

    VOID ToLower()
    {
        StringLowerW(GetBuffer(), GetCount());
    }

    VOID ToUpper()
    {
        StringUpperW(GetBuffer(), GetCount());
    }


    /************************************************************************
      internal
    ************************************************************************/

    NTSTATUS VerifyBufferLength(LARGE_LENGTH_TYPE Count)
    {
        LARGE_LENGTH_TYPE Length;

        Length = CountToLength(Count);

        if (Length > kMaxLength)
            return STATUS_NAME_TOO_LONG;

        if (Length >= this->String.MaximumLength)
            return STATUS_BUFFER_TOO_SMALL;

        return STATUS_SUCCESS;
    }

    NoInline VOID CopyString(STRING_CONST_POINTER_TYPE Str, LARGE_LENGTH_TYPE Count, BOOL Concat)
    {
        LARGE_LENGTH_TYPE Length;

        if (!Concat)
        {
            this->String.Length = 0;
            AddNull();
        }

        if (Count == 0)
            return;

        Length = CountToLength(Count);
        CopyMemory(GetBuffer() + GetCount(), Str, Length);
        this->String.Length += (STRING_LENGTH_TYPE)Length;
        AddNull();
    }

    VOID SetBuffer(STRING_POINTER_TYPE Buffer, LARGE_LENGTH_TYPE MaximumLength)
    {
        this->String.Buffer         = Buffer;
        this->String.Length         = 0;
        this->String.MaximumLength  = (STRING_LENGTH_TYPE)MaximumLength;

        GetBuffer()[0] = 0;
    }

    VOID AddNull()
    {
        *PtrAdd(GetBuffer(), GetLength()) = 0;
    }

    VOID CopyBuffer(StringImplementT *Impl)
    {
        CopyString(Impl->GetBuffer(), Impl->GetCount(), FALSE);
    }


    static ForceInline LARGE_LENGTH_TYPE CountToLength(LARGE_LENGTH_TYPE Count)
    {
        return Count * kCharSize;
    }

    static ForceInline LARGE_LENGTH_TYPE CountToLengthAddNull(LARGE_LENGTH_TYPE Count)
    {
        return Count * kCharSize + kCharSize;
    }

    static ForceInline LARGE_LENGTH_TYPE LengthToCount(LARGE_LENGTH_TYPE Length)
    {
        return Length / kCharSize;
    }

    static LARGE_LENGTH_TYPE GetStringCount(STRING_CONST_POINTER_TYPE Str)
    {
        return StrLengthW(Str);
    }

    static VOID InitString(PSTRING_TYPE String, STRING_CONST_POINTER_TYPE Buffer, LARGE_LENGTH_TYPE Count = kMaxNumberValue)
    {
        if (Count == kMaxNumberValue)
            Count = GetStringCount(Buffer);

        String->Buffer          = (STRING_POINTER_TYPE)Buffer;
        String->Length          = CountToLength(Count);
        String->MaximumLength   = String->Length;
    }
};


//typedef StringImplementT<> StringImplement;

#if USE_TEMPLATE
template<typename STRING_LENGTH_TYPE = USHORT, typename LARGE_LENGTH_TYPE = ULONG>
#endif

class StringT
{
public:

    typedef GrowableArray<StringT>  StringArray;
    typedef GrowableArray<BYTE>     ByteArray;

    enum
    {
        WithBOM  = 1 << 0,
    };

    class AnsiString : public ByteArray
    {
    public:
        operator bool() const
        {
            return this->GetSize() != 0;
        }

        operator PCSTR() const
        {
            return (PCSTR)this->GetData();
        }

        operator PSTR() const
        {
            return (PSTR)this->GetData();
        }

        operator PVOID() const
        {
            return (PVOID)this->GetData();
        }

        operator LPCVOID() const
        {
            return (LPCVOID)this->GetData();
        }

        operator LPCBYTE() const
        {
            return (LPCBYTE)this->GetData();
        }

        operator PBYTE() const
        {
            return (PBYTE)this->GetData();
        }
    };

protected:

#if USE_TEMPLATE
    typedef StringImplementT<STRING_LENGTH_TYPE, LARGE_LENGTH_TYPE> StringImplement;
#else
    typedef StringImplementT<> StringImplement;
#endif

    typedef typename StringImplement::STRING_LENGTH_TYPE             STRING_LENGTH_TYPE;
    typedef typename StringImplement::LARGE_LENGTH_TYPE              LARGE_LENGTH_TYPE;

    typedef typename StringImplement::STRING_CHAR_TYPE               STRING_CHAR_TYPE;
    typedef typename StringImplement::STRING_TYPE                    STRING_TYPE;
    typedef typename StringImplement::PSTRING_TYPE                   PSTRING_TYPE;
    typedef typename StringImplement::PCSTRING_TYPE                  PCSTRING_TYPE;
    typedef typename StringImplement::STRING_POINTER_TYPE            STRING_POINTER_TYPE;
    typedef typename StringImplement::STRING_CONST_POINTER_TYPE      STRING_CONST_POINTER_TYPE;

    static const LARGE_LENGTH_TYPE  kMaxNumberValue = StringImplement::kMaxNumberValue;
    static const LARGE_LENGTH_TYPE  kMaxLength      = StringImplement::kMaxLength;
    static const LARGE_LENGTH_TYPE  kCharSize       = StringImplement::kCharSize;
    static const ULONG_PTR          kStringImplSize = sizeof(StringImplement) - FIELD_SIZE(StringImplement, Buffer);

    typename StringImplement::STRING_POINTER_TYPE Buffer;

public:
    static const LARGE_LENGTH_TYPE  kInvalidIndex   = StringImplement::kInvalidIndex;

public:
    StringT()
    {
        Reset();
        *this = L"";
    }

    StringT(const StringT& Str)
    {
        Reset();
        *this = Str;
    }

    StringT(STRING_CONST_POINTER_TYPE Str)
    {
        Reset();
        *this = Str;
    }

    StringT(const UNICODE_STRING &Str)
    {
        Reset();
        *this = Str;
    }

    StringT(STRING_CHAR_TYPE Chr)
    {
        STRING_CHAR_TYPE Buffer[2];

        Reset();

        Buffer[0] = Chr;
        Buffer[1] = 0;

        *this = Buffer;
    }

    NoInline ~StringT()
    {
        ReleaseBuffer();

#if STRING_DEBUG
        this->Buffer = (STRING_POINTER_TYPE)~0x112233;
#endif

    }

    LARGE_LENGTH_TYPE GetCount() const
    {
        return GetImplement()->GetCount();
    }

    LARGE_LENGTH_TYPE GetSize() const
    {
        return GetImplement()->GetLength();
    }

    STRING_POINTER_TYPE GetBuffer() const
    {
        return GetImplement()->GetBuffer();
    }

    operator STRING_POINTER_TYPE() const
    {
        return GetBuffer();
    }

    operator STRING_CONST_POINTER_TYPE() const
    {
        return GetBuffer();
    }
/*
    operator PSTRING_TYPE()
    {
        return GetImplement()->operator PSTRING_TYPE();
    }

    operator PCSTRING_TYPE() const
    {
        return GetImplement()->operator PCSTRING_TYPE();
    }
*/
    operator UNICODE_STRING() const
    {
        UNICODE_STRING str;
        StringImplement *Impl = GetImplement();

        if (Impl->GetLength() > (TYPE_OF(str.Length))-1)
        {
            RtlInitEmptyString(&str);
            return str;
        }

        str.Length = (USHORT)Impl->GetLength();
        str.MaximumLength = str.Length;
        str.Buffer = Impl->GetBuffer();

        return str;
    }

    ForceInline operator PCUNICODE_STRING() const
    {
        return (PCUNICODE_STRING)&((UNICODE_STRING)(*this));
    }

    ForceInline operator PUNICODE_STRING() const
    {
        return &((UNICODE_STRING)(*this));
    }

    operator bool() const
    {
        return this->GetCount() != 0;
    }

    bool operator!() const
    {
        return this->GetCount() == 0;
    }

    NoInline StringT& operator=(STRING_CONST_POINTER_TYPE Str)
    {
        CopyFrom(Str);
        return *this;
    }

    NoInline StringT& operator=(const StringT& Str)
    {
        if (this == &Str)
            return *this;

        CopyFrom(Str.Buffer, Str.GetCount());

        return *this;
    }

    NoInline StringT& operator=(PCUNICODE_STRING Str)
    {
        CopyFrom(Str->Buffer, Str->Length / sizeof(Str->Buffer[0]));
        return *this;
    }

    StringT& operator=(const UNICODE_STRING &Str)
    {
        return operator=(&Str);
    }

    NoInline BOOL operator!=(STRING_CONST_POINTER_TYPE Str) const
    {
        return Compare(Str) != 0;
    }

    NoInline BOOL operator!=(const StringT& Str) const
    {
        return Compare(Str.GetBuffer(), Str.GetCount()) != 0;
    }

    NoInline BOOL operator==(STRING_CONST_POINTER_TYPE Str) const
    {
        return Compare(Str) == 0;
    }

    NoInline BOOL operator==(const StringT& Str) const
    {
        return Compare(Str.GetBuffer(), Str.GetCount()) == 0;
    }

    NoInline StringT operator+(STRING_CONST_POINTER_TYPE Str) const
    {
        return StringT(*this).Concat(Str);
    }

    NoInline StringT operator+(const StringT& Str) const
    {
        return StringT(*this).Concat(Str);
    }

    NoInline StringT& operator+=(STRING_CHAR_TYPE Chr)
    {
        STRING_CHAR_TYPE Buffer[2];

        Buffer[0] = Chr;
        Buffer[1] = 0;

        return Concat(Buffer, 1);
    }

    NoInline StringT& operator+=(STRING_CONST_POINTER_TYPE Str)
    {
        return Concat(Str);
    }

    NoInline StringT& operator+=(const StringT& Str)
    {
        return Concat(Str);
    }

    NoInline StringT& operator+=(const UNICODE_STRING& Str)
    {
        return Concat(Str.Buffer, Str.Length / sizeof(Str.Buffer[0]));
    }

    STRING_CHAR_TYPE& operator[](INT Index) const
    {
        return GetBuffer()[Index < 0 ? GetCount() + Index : Index];
    }

    STRING_CONST_POINTER_TYPE begin()
    {
        return &(*this)[0];
    }

    STRING_CONST_POINTER_TYPE end()
    {
        return &(*this)[-1] + 1;
    }

    NoInline NTSTATUS Assign(LARGE_LENGTH_TYPE Count)
    {
        return ResizeBuffer(Count);
    }

    LONG Compare(STRING_CONST_POINTER_TYPE Str, BOOL CaseInSensitive = FALSE) const
    {
        return GetImplement()->Compare(Str, StringImplement::GetStringCount(Str), CaseInSensitive);
    }

    LONG Compare(STRING_CONST_POINTER_TYPE Str, LARGE_LENGTH_TYPE Count, BOOL CaseInSensitive = FALSE) const
    {
        return GetImplement()->Compare(Str, Count, CaseInSensitive);
    }

    StringT& Concat(STRING_CONST_POINTER_TYPE Str)
    {
        return Concat(Str, StringImplement::GetStringCount(Str));
    }

    StringT& Concat(const StringT& Str)
    {
        StringImplement *Impl = Str.GetImplement();
        return Concat(Impl->GetBuffer(), Impl->GetCount());
    }

    StringT& Concat(STRING_CONST_POINTER_TYPE Str, LARGE_LENGTH_TYPE Count)
    {
        NTSTATUS Status;

        Status = ResizeBuffer(GetCount() + Count);
        if (NT_FAILED(Status))
            return *this;

        GetImplement()->Concat(Str, Count);

        return *this;
    }

    NTSTATUS CopyFrom(STRING_CONST_POINTER_TYPE Str)
    {
        return CopyFrom(Str, StringImplement::GetStringCount(Str));
    }

    NoInline NTSTATUS CopyFrom(STRING_CONST_POINTER_TYPE Str, LARGE_LENGTH_TYPE Count)
    {
        NTSTATUS Status;

        Status = ResizeBuffer(Count);
        FAIL_RETURN(Status);

        return GetImplement()->CopyFrom(Str, Count);
    }

    NoInline StringT Copy() const
    {
        return StringT(*this);
    }

    NoInline static StringT Format(STRING_CONST_POINTER_TYPE format, ...)
    {
        va_list Arguments;
        va_start(Arguments, format);
        return FormatV(format, Arguments);
    }

    NoInline static StringT FormatV(STRING_CONST_POINTER_TYPE Format, va_list Arguments)
    {
        NTSTATUS    Status;
        StringT      NewString;

        Status = NewString.ResizeBuffer(NewString.GetImplement()->FormatCountV(Format, Arguments));
        if (NT_FAILED(Status))
            return NewString;

        NewString.GetImplement()->FormatV(Format, Arguments);
        return NewString;
    }

    NoInline LARGE_LENGTH_TYPE IndexOf(STRING_CHAR_TYPE Chr, LARGE_LENGTH_TYPE StartIndex = 0) const
    {
        STRING_CHAR_TYPE Buffer[2];

        Buffer[0] = Chr;
        Buffer[1] = 0;

        return IndexOf(Buffer, StartIndex);
    }

    NoInline LARGE_LENGTH_TYPE IndexOf(STRING_CONST_POINTER_TYPE Str, LARGE_LENGTH_TYPE StartIndex = 0) const
    {
        return GetImplement()->IndexOf(Str, StartIndex);
    }

    NoInline BOOL IsNullOrEmpty() const
    {
        return GetImplement() == nullptr ? TRUE : GetImplement()->IsNullOrEmpty();
    }

    NoInline LARGE_LENGTH_TYPE LastIndexOf(STRING_CHAR_TYPE Chr, LARGE_LENGTH_TYPE StartIndex = 0) const
    {
        STRING_CHAR_TYPE Buffer[2];

        Buffer[0] = Chr;
        Buffer[1] = 0;

        return LastIndexOf(Buffer, StartIndex);
    }

    NoInline LARGE_LENGTH_TYPE LastIndexOf(STRING_CONST_POINTER_TYPE Str, LARGE_LENGTH_TYPE StartIndex = 0) const
    {
        return GetImplement()->LastIndexOf(Str, StartIndex);
    }

    NoInline BOOL MatchExpression(STRING_CONST_POINTER_TYPE Expression, BOOL IgnoreCase = TRUE) const
    {
        STRING_TYPE Expr;
        StringImplement::InitString(&Expr, Expression);
        return GetImplement()->MatchExpression(&Expr, IgnoreCase);
    }

    NoInline BOOL MatchExpression(const UNICODE_STRING& Expression, BOOL IgnoreCase = TRUE) const
    {
        STRING_TYPE Expr;
        Expr.Length = Expression.Length;
        Expr.MaximumLength = Expression.MaximumLength;
        Expr.Buffer = Expression.Buffer;
        return GetImplement()->MatchExpression(&Expr, IgnoreCase);
    }

    NoInline BOOL MatchExpression(StringT Expression, BOOL IgnoreCase = TRUE) const
    {
        return GetImplement()->MatchExpression(&Expression.GetImplement()->String, IgnoreCase);
    }

    NoInline StringT PadLeft(LARGE_LENGTH_TYPE TotalWidth, STRING_CHAR_TYPE PaddingChar = ' ') const
    {
        NTSTATUS            Status;
        StringT              Padded = *this;
        LARGE_LENGTH_TYPE   LengthToPad;

        if (TotalWidth <= Padded.GetCount())
            return Padded;

        LengthToPad = TotalWidth - Padded.GetCount();
        Status = Padded.ResizeBuffer(TotalWidth);
        if (NT_FAILED(Status))
            return Padded;

        Padded = L"";

        for (; LengthToPad; --LengthToPad)
            Padded += PaddingChar;

        Padded += *this;

        return Padded;
    }

    NoInline StringT PadRight(LARGE_LENGTH_TYPE TotalWidth, STRING_CHAR_TYPE PaddingChar = ' ') const
    {
        NTSTATUS            Status;
        StringT             Padded = *this;
        LARGE_LENGTH_TYPE   LengthToPad;

        if (TotalWidth <= Padded.GetCount())
            return Padded;

        LengthToPad = TotalWidth - Padded.GetCount();
        Status = Padded.ResizeBuffer(TotalWidth);
        if (NT_FAILED(Status))
            return Padded;

        for (; LengthToPad; --LengthToPad)
            Padded += PaddingChar;

        return Padded;
    }

    NoInline StringT Remove(LARGE_LENGTH_TYPE StartIndex, LARGE_LENGTH_TYPE Count = kMaxNumberValue) const
    {
        NTSTATUS            Status;
        StringT              NewString;
        StringImplement*    Impl;
        LARGE_LENGTH_TYPE   TailLength;

        if (StartIndex >= GetCount() || StartIndex == 0)
            return NewString;

        if (Count == kMaxNumberValue || StartIndex + Count >= GetCount())
            Count = GetCount() - StartIndex;

        TailLength = GetCount() - (StartIndex + Count);

        Status = NewString.ResizeBuffer(StartIndex + TailLength);
        if (NT_FAILED(Status))
            return NewString;

        Impl = NewString.GetImplement();
        Impl->CopyFrom(GetBuffer(), StartIndex);
        Impl->Concat(GetBuffer() + StartIndex + Count, TailLength);

        return NewString;
    }

    NoInline StringT Replace(const StringT& OldValue, const StringT& NewValue) const
    {
        return ReplaceWorker(OldValue, OldValue.GetCount(), NewValue, NewValue.GetCount());
    }

    NoInline StringT Replace(STRING_CONST_POINTER_TYPE OldValue, STRING_CONST_POINTER_TYPE NewValue) const
    {
        return ReplaceWorker(OldValue, StringImplement::GetStringCount(OldValue), NewValue, StringImplement::GetStringCount(NewValue));
    }

    NoInline StringT Replace(STRING_CHAR_TYPE OldValue, STRING_CHAR_TYPE NewValue) const
    {
        STRING_CHAR_TYPE Old[2], New[2];

        Old[0] = OldValue;
        Old[1] = 0;
        New[0] = NewValue;
        New[1] = 0;

        return ReplaceWorker(Old, 1, New, 1);
    }

    StringArray SplitLines(STRING_CONST_POINTER_TYPE Separator = L"\n") const
    {
        return Replace(L"\r\n", L"\n").Replace(L"\r", L"\n").Split(Separator);
    }

    StringArray Split(const STRING_CHAR_TYPE Separator, LARGE_LENGTH_TYPE MaxSplit = kMaxNumberValue) const
    {
        STRING_CHAR_TYPE Buffer[2];

        Buffer[0] = Separator;
        Buffer[1] = 0;

        return SplitWorker(Buffer, 1, MaxSplit);
    }

    NoInline StringArray Split(STRING_CONST_POINTER_TYPE Separator, LARGE_LENGTH_TYPE MaxSplit = kMaxNumberValue) const
    {
        return SplitWorker(Separator, StringImplement::GetStringCount(Separator), MaxSplit);
    }

    StringT Join(const StringArray& Values) const
    {
        return Join(*this, Values);
    }

    NoInline static StringT Join(const StringT& Separator, const StringArray& Values)
    {
        StringT Str;
        StringImplement* Impl;

        for (auto &value : Values)
        {
            Str += value + Separator;
        }

        Impl = Str.GetImplement();
        Impl->SetLength(Impl->GetLength() - Separator.GetSize());
        Impl->AddNull();

        return Str;
    }

    NoInline BOOL StartsWith(STRING_CONST_POINTER_TYPE Starts, BOOL CaseInSensitive = FALSE) const
    {
        return StartsWithWorker(Starts, StringImplement::GetStringCount(Starts), CaseInSensitive);
    }

    NoInline BOOL StartsWith(const StringT& Starts, BOOL CaseInSensitive = FALSE) const
    {
        return &Starts == this ? TRUE : StartsWithWorker(Starts, Starts.GetCount(), CaseInSensitive);
    }

    NoInline BOOL EndsWith(STRING_CONST_POINTER_TYPE Ends, BOOL CaseInSensitive = FALSE) const
    {
        return EndsWithWorker(Ends, StringImplement::GetStringCount(Ends), CaseInSensitive);
    }

    NoInline BOOL EndsWith(StringT& Ends, BOOL CaseInSensitive = FALSE) const
    {
        return &Ends == this ? TRUE : EndsWithWorker(Ends, Ends.GetCount(), CaseInSensitive);
    }

    NoInline StringT SubString(LARGE_LENGTH_TYPE StartIndex, LARGE_LENGTH_TYPE Count = kMaxNumberValue) const
    {
        StringT NewString;

        if (StartIndex >= GetCount())
            return NewString;

        if (Count == kMaxNumberValue || StartIndex + Count >= GetCount())
            Count = GetCount() - StartIndex;

        NewString.CopyFrom(GetBuffer() + StartIndex, Count);

        return NewString;
    }

    NoInline StringT ToLower() const
    {
        StringT NewString = *this;
        NewString.GetImplement()->ToLower();
        return NewString;
    }

    NoInline StringT ToUpper() const
    {
        StringT NewString = *this;
        NewString.GetImplement()->ToUpper();
        return NewString;
    }

    NoInline StringT TrimEnd(StringT TrimChars) const
    {
        STRING_POINTER_TYPE Begin, End;

        Begin = GetBuffer();
        End = Begin + GetCount();

        while (End > Begin)
        {
            if (TrimChars.IndexOf(End[-1]) == kInvalidIndex)
                break;

            --End;
        }

        if (End <= Begin)
            return StringT();

        return SubString(0, End - Begin);
    }

    NoInline StringT TrimStart(StringT TrimChars) const
    {
        STRING_POINTER_TYPE Begin, End;

        Begin = GetBuffer();
        End = Begin + GetCount();

        while (Begin != End)
        {
            if (TrimChars.IndexOf(Begin[0]) == kInvalidIndex)
                break;

            ++Begin;
        }

        if (Begin == End)
            return StringT();

        return SubString(Begin - GetBuffer());
    }

    StringT Strip() const
    {
        return Trim(L" \t");
    }

    NoInline StringT Trim(StringT TrimChars) const
    {
        return TrimStart(TrimChars).TrimEnd(TrimChars);
    }

    NoInline ULONG64 ToHex() const
    {
        return StringToInt64HexW(*this);
    }

    NoInline ULONG64 ToInteger() const
    {
        return StringToInt64W(*this);
    }

    AnsiString Encode(ULONG_PTR Encoding = CP_ACP, ULONG_PTR Flags = 0) const
    {
        return Encode(*this, Encoding, Flags);
    }

    NoInline static StringT Decode(LPCVOID Bytes, ULONG_PTR Length, ULONG_PTR Encoding)
    {
        NTSTATUS    Status;
        PWSTR       UnicodeString;
        ULONG       UnicodeStringActualByteCount;
        StringT     Str;

        if (Encoding == CP_UTF16_LE)
        {
            if (*(PWSTR)Bytes == BOM_UTF16_LE)
            {
                Bytes = (PWSTR)Bytes + 1;
                Length -= sizeof(WCHAR);
            }

            Str.CopyFrom((PWSTR)Bytes, Length / sizeof(WCHAR));
            return Str;
        }
        else if (Encoding == CP_UTF8)
        {

#if ML_SUPPORT_XP

            UnicodeStringActualByteCount = MultiByteToWideChar(CP_UTF8, 0, (PCSTR)Bytes, Length, nullptr, 0) / sizeof(WCHAR);
            Status = UnicodeStringActualByteCount == 0 ? NTSTATUS_FROM_WIN32(RtlGetLastWin32Error()) : STATUS_SUCCESS;

#else
            Status = RtlUTF8ToUnicodeN(nullptr, 0, &UnicodeStringActualByteCount, (PCSTR)Bytes, Length);
#endif

            if (NT_FAILED(Status))
                return L"";

            UnicodeString = (PWSTR)AllocateMemory(UnicodeStringActualByteCount);
            if (UnicodeString == nullptr)
                return Str;


#if ML_SUPPORT_XP

            UnicodeStringActualByteCount = MultiByteToWideChar(CP_UTF8, 0, (PCSTR)Bytes, Length, nullptr, 0) / sizeof(WCHAR);
            Status = UnicodeStringActualByteCount == 0 ? NTSTATUS_FROM_WIN32(RtlGetLastWin32Error()) : STATUS_SUCCESS;

#else
            Status = RtlUTF8ToUnicodeN(UnicodeString, UnicodeStringActualByteCount, &UnicodeStringActualByteCount, (PCSTR)Bytes, Length);
#endif

            if (NT_SUCCESS(Status))
                Str.CopyFrom(UnicodeString, UnicodeStringActualByteCount / sizeof(UnicodeString[0]));

            FreeMemory(UnicodeString);

            return Str;
        }

        switch (Encoding)
        {
            case CP_ACP:
                Encoding = CurrentPeb()->AnsiCodePageData[1];
                break;

            case CP_OEMCP:
                Encoding = CurrentPeb()->OemCodePageData[1];
                break;
        }

        NtFileMemory NlsFile;
        CPTABLEINFO CodePageTable;

        Status = InitCodePageTable(&CodePageTable, NlsFile, Encoding);
        if (NT_FAILED(Status))
            return Str;

        CustomCPToUnicodeSize(&CodePageTable, &UnicodeStringActualByteCount, (PSTR)Bytes, Length);

        UnicodeString = (PWSTR)AllocateMemory(UnicodeStringActualByteCount);
        if (UnicodeString == nullptr)
            return Str;

        Status = RtlCustomCPToUnicodeN(&CodePageTable, UnicodeString, UnicodeStringActualByteCount, &UnicodeStringActualByteCount, (PCSTR)Bytes, Length);
        if (NT_SUCCESS(Status))
            Str.CopyFrom(UnicodeString, UnicodeStringActualByteCount / sizeof(UnicodeString[0]));

        FreeMemory(UnicodeString);

        return Str;
    }

    NoInline static AnsiString Encode(const StringT& Str, ULONG_PTR Encoding = CP_ACP, ULONG_PTR Flags = 0)
    {
        NTSTATUS    Status;
        ULONG_PTR   Length;
        AnsiString  Array;

        //if (Encoding == CP_THREAD_ACP)
        //{
        //    WCHAR Buffer[8];
        //    GetLocaleInfoW(CurrentTeb()->CurrentLocale, LOCALE_IDEFAULTANSICODEPAGE, Buffer, countof(Buffer));

        //    Encoding = StringToInt32W(Buffer);
        //    if (Encoding == CurrentPeb()->AnsiCodePageData[1])
        //        Encoding = CP_ACP;
        //}

        if (Encoding == CP_UTF16_LE)
        {
            PWSTR Buffer;

            Length = (ULONG_PTR)Str.GetSize() + sizeof(WCHAR) * (1 + FLAG_ON(Flags, WithBOM));
            Status = Array.SetSize(Length);
            if (NT_FAILED(Status))
                return Array;

            Buffer = (PWSTR)Array.GetData();

            if (FLAG_ON(Flags, WithBOM))
            {
                Buffer[0] = BOM_UTF16_LE;
                ++Buffer;
            }

            CopyMemory(Buffer, (PWSTR)Str, Str.GetSize());
            Buffer[Str.GetCount() + 1] = 0;

            Array.UpdateDataCount(Length);

            return Array;
        }
        else if (Encoding == CP_UTF8)
        {
            PSTR Buffer;
            ULONG UTF8StringActualByteCount;

#if ML_SUPPORT_XP

            UTF8StringActualByteCount = WideCharToMultiByte(CP_UTF8, 0, Str, Str.GetCount(), nullptr, 0, nullptr, nullptr);
            Status = UTF8StringActualByteCount == 0 ? NTSTATUS_FROM_WIN32(RtlGetLastWin32Error()) : STATUS_SUCCESS;

#else

            Status = RtlUnicodeToUTF8N(nullptr, 0, &UTF8StringActualByteCount, Str, (ULONG)Str.GetSize());
#endif

            if (NT_FAILED(Status))
                return Array;

            Length = UTF8StringActualByteCount + 1 + (FLAG_ON(Flags, WithBOM) ? 3 : 0);
            Status = Array.SetSize(Length);
            if (NT_FAILED(Status))
                return Array;

            Array.UpdateDataCount(Length);

            Buffer = (PSTR)Array.GetData();

            if (FLAG_ON(Flags, WithBOM))
            {
                *(PULONG)Buffer = BOM_UTF8;
                Buffer += 3;
                Length -= 3;
            }

#if ML_SUPPORT_XP

            UTF8StringActualByteCount = WideCharToMultiByte(CP_UTF8, 0, Str, Str.GetCount(), Buffer, Length, nullptr, nullptr);
            Status = UTF8StringActualByteCount == 0 ? NTSTATUS_FROM_WIN32(RtlGetLastWin32Error()) : STATUS_SUCCESS;

#else
            Status = RtlUnicodeToUTF8N(Buffer, Length, &UTF8StringActualByteCount, Str, (ULONG)Str.GetSize());
#endif

            if (NT_FAILED(Status))
            {
                Array.RemoveAll();
                return Array;
            }

            Buffer[UTF8StringActualByteCount] = 0;

            return Array;
        }
        else if (Encoding == CP_ACP)
        {
            PSTR Buffer;
            ULONG StringActualByteCount;

            Status = RtlUnicodeToMultiByteSize(&StringActualByteCount, Str, (ULONG)Str.GetSize());
            if (NT_FAILED(Status))
                return Array;

            Length = StringActualByteCount + 1;
            Status = Array.SetSize(Length);
            if (NT_FAILED(Status))
                return Array;

            Array.UpdateDataCount(Length);

            Buffer = (PSTR)Array.GetData();

            Status = RtlUnicodeToMultiByteN(Buffer, Length, &StringActualByteCount, Str, (ULONG)Str.GetSize());
            if (NT_FAILED(Status))
            {
                Array.RemoveAll();
                return Array;
            }

            Buffer[StringActualByteCount] = 0;

            return Array;
        }
        else if (Encoding == CP_OEMCP)
        {
            PSTR Buffer;
            ULONG StringActualByteCount;

            Status = RtlUnicodeToOemN(nullptr, 0, &StringActualByteCount, Str, (ULONG)Str.GetSize());
            if (NT_FAILED(Status))
                return Array;

            Length = StringActualByteCount + 1;
            Status = Array.SetSize(Length);
            if (NT_FAILED(Status))
                return Array;

            Array.UpdateDataCount(Length);

            Buffer = (PSTR)Array.GetData();

            Status = RtlUnicodeToOemN(Buffer, Length, &StringActualByteCount, Str, (ULONG)Str.GetSize());
            if (NT_FAILED(Status))
            {
                Array.RemoveAll();
                return Array;
            }

            Buffer[StringActualByteCount] = 0;

            return Array;
        }

        PSTR            Buffer;
        ULONG           BytesInMultiByteString;
        NtFileMemory    NlsFile;
        CPTABLEINFO     CodePageTable;

        Status = InitCodePageTable(&CodePageTable, NlsFile, Encoding);
        if (NT_FAILED(Status))
            return Array;

        UnicodeToCustomCPSize(&CodePageTable, &BytesInMultiByteString, Str, (ULONG)Str.GetSize());

        Status = Array.SetSize(BytesInMultiByteString);
        if (NT_FAILED(Status))
        {
            return Array;
        }

        Buffer = (PSTR)Array.GetData();
        Array.UpdateDataCount(BytesInMultiByteString);

        Status = RtlUnicodeToCustomCPN(&CodePageTable, Buffer, BytesInMultiByteString, &BytesInMultiByteString, Str, (ULONG)Str.GetSize());
        Array.Add(0);

        return Array;
    }

protected:
    NoInline static NTSTATUS InitCodePageTable(PCPTABLEINFO CodePageTable, NtFileMemory &NlsFile, ULONG_PTR Encoding)
    {
        NTSTATUS    Status;
        StringT     NlsFileName;

        Status = GetNlsFile(NlsFileName, Encoding);
        FAIL_RETURN(Status);

        Status = NlsFile.Open(StringT(L"\\SystemRoot\\system32\\") + NlsFileName, NFD_NOT_RESOLVE_PATH);
        FAIL_RETURN(Status);

        RtlInitCodePageTable((PUSHORT)NlsFile.GetBuffer(), CodePageTable);

        return STATUS_SUCCESS;
    }

    NoInline
    static
    NTSTATUS
    CustomCPToUnicodeSize(
        PCPTABLEINFO    CustomCP,
        PULONG          BytesInUnicodeString,
        PCSTR           MultiByteString,
        ULONG           BytesInMultiByteString
    )
    {
        ULONG   cbUnicode = 0;
        PUSHORT NlsLeadByteInfo;

        if (CustomCP->DBCSCodePage)
        {
            NlsLeadByteInfo = CustomCP->DBCSOffsets;

            while (BytesInMultiByteString--)
            {
                if (NlsLeadByteInfo[*(PUCHAR)MultiByteString++])
                {
                    if (BytesInMultiByteString == 0)
                    {
                        cbUnicode += sizeof(WCHAR);
                        break;
                    }
                    else
                    {
                        BytesInMultiByteString--;
                        MultiByteString++;
                    }
                }

                cbUnicode += sizeof(WCHAR);
            }

            *BytesInUnicodeString = cbUnicode;
        }
        else
        {
            *BytesInUnicodeString = BytesInMultiByteString * sizeof(WCHAR);
        }

        return STATUS_SUCCESS;
    }

    NoInline
    static
    NTSTATUS
    UnicodeToCustomCPSize(
        PCPTABLEINFO    CustomCP,
        PULONG          BytesInMultiByteString,
        PCWSTR          UnicodeString,
        ULONG           BytesInUnicodeString
    )
    {
        ULONG cbMultiByte = 0;
        ULONG CharsInUnicodeString;
        PUSHORT NlsUnicodeToMbAnsiData;

        CharsInUnicodeString = BytesInUnicodeString / sizeof(WCHAR);

        if (CustomCP->DBCSCodePage)
        {
            USHORT MbChar;

            NlsUnicodeToMbAnsiData = (PUSHORT)CustomCP->WideCharTable;

            while (CharsInUnicodeString--)
            {
                MbChar = NlsUnicodeToMbAnsiData[*UnicodeString++];
                if (HIBYTE(MbChar) == 0)
                {
                    cbMultiByte++ ;
                }
                else
                {
                    cbMultiByte += 2;
                }
            }

            *BytesInMultiByteString = cbMultiByte;
        }
        else
        {
            *BytesInMultiByteString = CharsInUnicodeString;
        }

        return STATUS_SUCCESS;
    }

    NoInline static NTSTATUS GetNlsFile(StringT& NlsFile, ULONG NlsIndex, PCWSTR SubKey = L"System\\CurrentControlSet\\Control\\Nls\\CodePage")
    {
        BOOL        Success;
        WCHAR       NlsIndexBuffer[16];
        NTSTATUS    Status;
        PKEY_VALUE_PARTIAL_INFORMATION FileName;

        _snwprintf(NlsIndexBuffer, countof(NlsIndexBuffer), L"%d", NlsIndex);

        Status = GetKeyValue(HKEY_LOCAL_MACHINE, SubKey, NlsIndexBuffer, &FileName);
        FAIL_RETURN(Status);

        NlsFile.CopyFrom((PWSTR)FileName->Data, FileName->DataLength / sizeof(WCHAR));

        FreeKeyInfo(FileName);

        return STATUS_SUCCESS;
    }

    StringImplement* GetImplement() const
    {
        return this->Buffer == nullptr ? nullptr : FIELD_BASE(this->Buffer, StringImplement, Buffer);
    }

    VOID SetImplement(StringImplement *Impl)
    {
        this->Buffer = Impl->Buffer;
    }

    VOID Reset()
    {
        this->Buffer = nullptr;
    }

    NTSTATUS ReleaseImplement(StringImplement *Impl)
    {
        return FreeMemory(Impl) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
    }

    NTSTATUS ReleaseBuffer()
    {
        StringImplement* Impl = GetImplement();

        if (Impl != nullptr)
        {
            ReleaseImplement(Impl);
        }

        Reset();

        return STATUS_SUCCESS;
    }

    NTSTATUS AllocateBuffer(LARGE_LENGTH_TYPE Count)
    {
        return ResizeBuffer(Count);
    }

    NTSTATUS ResizeBuffer(LARGE_LENGTH_TYPE Count)
    {
        LARGE_LENGTH_TYPE   Length, MaxLength;
        STRING_TYPE         Buffer;
        StringImplement     *Impl, *OldImpl;

        Length = StringImplement::CountToLength(Count);
        if (Length > kMaxLength)
            return STATUS_NAME_TOO_LONG;

        OldImpl = GetImplement();

        LOOP_ONCE
        {
            if (OldImpl == nullptr)
                break;

            if (OldImpl->GetMaxLength() > Length)
                return STATUS_SUCCESS;
        }

        Length = StringImplement::CountToLengthAddNull(Count);
        MaxLength = ROUND_UP(Length * 3 / 2, kCharSize);
        Impl = (StringImplement *)AllocateMemory((ULONG_PTR)(kStringImplSize + MaxLength));
        if (Impl == nullptr)
            return STATUS_NO_MEMORY;

        Impl->SetBuffer(Impl->Buffer, MaxLength);

        if (OldImpl != nullptr)
        {
            Impl->CopyBuffer(OldImpl);
            ReleaseImplement(OldImpl);
        }

        SetImplement(Impl);

        return STATUS_SUCCESS;
    }

    StringT
    ReplaceWorker(
        STRING_CONST_POINTER_TYPE   OldValue,
        LARGE_LENGTH_TYPE           OldCount,
        STRING_CONST_POINTER_TYPE   NewValue,
        LARGE_LENGTH_TYPE           NewCount
    ) const
    {
        StringT                     NewString;
        LARGE_LENGTH_TYPE           StartIndex, Sub;
        STRING_CONST_POINTER_TYPE   Base, Begin, End;
        StringImplement*            Impl;

        Impl = GetImplement();

        Base        = GetBuffer();
        Begin       = Base;
        End         = Begin + GetCount();
        StartIndex  = 0;

        while (Begin < End)
        {
            Sub = Impl->IndexOf(OldValue, StartIndex);
            if (Sub == kInvalidIndex)
                break;

            if (StartIndex != Sub)
                NewString.Concat(Base + StartIndex, Sub - StartIndex);

            NewString.Concat(NewValue, NewCount);

            StartIndex = Sub + OldCount;
            Begin = Base + StartIndex;
        }

        if (Begin < End)
            NewString.Concat(Begin, End - Begin);

        return NewString;
    }

    StringArray SplitWorker(STRING_CONST_POINTER_TYPE Separator, LARGE_LENGTH_TYPE Length, LARGE_LENGTH_TYPE MaxSplit) const
    {
        StringArray                 Array;
        LARGE_LENGTH_TYPE           StartIndex, Sep;
        STRING_CONST_POINTER_TYPE   Begin, End, Base;
        StringImplement*            Impl;

        Impl = GetImplement();

        Base        = GetBuffer();
        Begin       = Base;
        End         = Begin + GetCount();
        StartIndex  = 0;

        for (; MaxSplit; --MaxSplit)
        {
            Sep = Impl->IndexOf(Separator, StartIndex);
            if (Sep == kInvalidIndex)
                break;

            //if (Sep != StartIndex)
            {
                Array.Add(SubString(StartIndex, Sep - StartIndex));
            }
            //else
            {
                //Array.Add(L"");
            }

            StartIndex = Sep + Length;
            Begin = Base + StartIndex;
        }

        if (Begin < End)
        {
            Array.Add(SubString(Begin - Base));
        }

        return Array;
    }

    BOOL StartsWithWorker(STRING_CONST_POINTER_TYPE Starts, LARGE_LENGTH_TYPE Count, BOOL CaseInSensitive)
    {
        StringImplement* Impl;

        Impl = GetImplement();

        if (Count == 0 && Impl->GetCount() == 0)
            return TRUE;

        if (Count > Impl->GetCount())
            return FALSE;

        if (Count == Impl->GetCount())
            return Compare(Starts, Count, CaseInSensitive) == 0;

        return SubString(0, Count).Compare(Starts, Count, CaseInSensitive) == 0;
    }

    BOOL EndsWithWorker(STRING_CONST_POINTER_TYPE Ends, LARGE_LENGTH_TYPE Count, BOOL CaseInSensitive) const
    {
        StringImplement* Impl;

        Impl = GetImplement();

        if (Count == 0 && Impl->GetCount() == 0)
            return TRUE;

        if (Count > Impl->GetCount())
            return FALSE;

        if (Count == Impl->GetCount())
            return Compare(Ends, Count, CaseInSensitive) == 0;

        return SubString(Impl->GetCount() - Count).Compare(Ends, Count, CaseInSensitive) == 0;
    }

private:
    static PVOID AllocateMemory(ULONG_PTR Size)
    {

#if STRING_DEBUG
        ++DebugAllocCount;
#endif

        return AllocateMemoryP(Size);
    }

    static BOOL FreeMemory(PVOID Memory)
    {

#if STRING_DEBUG
        --DebugAllocCount;
        if (DebugAllocCount < 0)
            RtlRaiseException(0);
#endif

        return FreeMemoryP(Memory);
    }


#if STRING_DEBUG
    static LONG_PTR DebugAllocCount;
#endif
};

#if STRING_DEBUG

DECL_SELECTANY LONG_PTR StringT<>::DebugAllocCount = 0;

#endif

#if USE_TEMPLATE

    typedef StringT<ULONG, ULONG64> String;
    //typedef StringT<ULONG, ULONG64> LargeString;

#else

    typedef StringT String;

#endif

typedef String::StringArray StringArray;
typedef String::ByteArray ByteArray;


#pragma warning(pop)

#endif // _MLSTRING_H_252d9413_55ca_4d44_976f_c0dcecd5afd4_