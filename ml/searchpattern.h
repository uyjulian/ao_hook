#ifndef _SEARCHPATTERN_H_214862b7_e205_40fb_ba78_ccfbe9d04321_
#define _SEARCHPATTERN_H_214862b7_e205_40fb_ba78_ccfbe9d04321_


#if CPP_DEFINED

_ML_CPP_HEAD_


typedef struct
{
    PVOID       Pattern;
    ULONG_PTR   Size;
    ULONG_PTR   Flags;
    LONG_PTR    HeadOffsetToNext;
    LONG_PTR    OffsetToPrevHead;
    ULONG_PTR   Extra;

} SEARCH_PATTERN_DATA, *PSEARCH_PATTERN_DATA;

#define PATTERN_AUTO_SEARCH     (1u << 0)
#define PATTERN_SEARCH_RANGE    (1u << 1)

#define ADD_PATTERN_(_bytes, _size, ...) { _bytes, _size, __VA_ARGS__ }

#define ADD_PATTERN(_bytes_arr, ...) ADD_PATTERN_((_bytes_arr), sizeof(_bytes_arr), __VA_ARGS__)
#define ADD_PATTERN_F(_bytes_arr, _flags, _offset_to_next, _offset_to_prev) ADD_PATTERN_((_bytes_arr), sizeof(_bytes_arr), _flags, _offset_to_next, _offset_to_prev)

template<typename CallbackRoutine, typename ContextType>
inline
PVOID
SearchPatternT(
    PSEARCH_PATTERN_DATA    Patterns,
    ULONG_PTR               PatternCount,
    PVOID                   Begin,
    LONG_PTR                Length,
    CallbackRoutine         Callback,
    ContextType             Context
)
{
    LONG_PTR                RemainSize;
    ULONG_PTR               Offset;
    PVOID                   Found;
    PBYTE                   Buffer, End;
    NTSTATUS                Status;
    PSEARCH_PATTERN_DATA    Data;

    Buffer  = (PBYTE)Begin;
    End     = Buffer + Length;
    while (Length > 0)
    {
        ULONG_PTR Count;

        Data        = Patterns;
        RemainSize  = Length;

        Found = Sunday(Buffer, RemainSize, Data->Pattern, Data->Size);
        if (Found == nullptr)
            return Found;

        Buffer = (PBYTE)Found + Data->Size;

        if (Data->HeadOffsetToNext != 0)
        {
            Buffer += Data->HeadOffsetToNext - Data->Size;
        }

        RemainSize = PtrOffset(End, Buffer);

        if (RemainSize < 0)
            return nullptr;

        if (PatternCount != 1)
        {
            for (Count = PatternCount - 1; Count != 0; --Count)
            {
                ++Data;

                if (Data->OffsetToPrevHead != 0)
                {
                    Offset = Data->OffsetToPrevHead - Data[-1].Size;
                    Buffer += Offset;
                    RemainSize -= Offset;
                }

                if ((ULONG_PTR)RemainSize < Data->Size)
                    return nullptr;

                if (FLAG_ON(Data->Flags, PATTERN_AUTO_SEARCH))
                {
                    PVOID AutoSearch;

                    Offset = FLAG_ON(Data->Flags, PATTERN_SEARCH_RANGE) ? Data->Extra : RemainSize;
                    AutoSearch = Sunday(Buffer, Offset, Data->Pattern, Data->Size);
                    if (AutoSearch == nullptr)
                        return nullptr;

                    Buffer = (PBYTE)AutoSearch + Data->Size;

                    if (Data->HeadOffsetToNext != 0)
                    {
                        Buffer += Data->HeadOffsetToNext - Data->Size;
                    }

                    RemainSize = PtrOffset(End, Buffer);

                    continue;
                }

                if (RtlCompareMemory(Buffer, Data->Pattern, Data->Size) != Data->Size)
                    break;

                Buffer += Data->Size;

                if (Data->HeadOffsetToNext != 0)
                {
                    Buffer += Data->HeadOffsetToNext - Data->Size;
                }

                RemainSize  = PtrOffset(End, Buffer);

                if (RemainSize < 0)
                    return nullptr;
            }

            if (Count != 0)
            {
                Buffer = (PBYTE)Found + Patterns[0].Size + Patterns[0].HeadOffsetToNext;

                if (Patterns[0].HeadOffsetToNext != 0)
                {
                    Buffer += Patterns[0].HeadOffsetToNext - Patterns[0].Size;
                }

                Length = PtrOffset(End, Buffer);
                continue;
            }
        }
        else
        {
            ;
        }

        Status = Callback(Found, Begin, RemainSize, Context);
        if (NT_SUCCESS(Status))
            return Found;

        Buffer = (PBYTE)Found + Patterns[0].Size + Patterns[0].HeadOffsetToNext;
        Length = PtrOffset(End, Buffer);
    }

    return nullptr;
}

ForceInline
PVOID
SearchPattern(
    PSEARCH_PATTERN_DATA    Patterns,
    ULONG_PTR               PatternCount,
    PVOID                   Begin,
    LONG_PTR                Length
)
{
    auto AutoPatternCheck = [] (PVOID, PVOID, ULONG_PTR, PVOID) { return STATUS_SUCCESS; };

    return SearchPatternT(Patterns, PatternCount, Begin, Length, AutoPatternCheck, (PVOID)0);
}

inline
PVOID
SearchPatternSafe(
    PSEARCH_PATTERN_DATA    Patterns,
    ULONG_PTR               PatternCount,
    PVOID                   Begin,
    LONG_PTR                Length
)
{
    SEH_TRY
    {
        return SearchPattern(Patterns, PatternCount, Begin, Length);
    }
    SEH_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        return nullptr;
    }
}

template<class T = VOID>
inline PVOID SearchPatternSafe(const ml::String& Pattern, PVOID Begin, LONG_PTR Length)
{
    GrowableArray<SEARCH_PATTERN_DATA>  Patterns;
    GrowableArray<ml::String::ByteArray *>  BytesArray;
    ml::String::ByteArray*                  CurrentBytes;
    SEARCH_PATTERN_DATA                 CurrentPattern;
    ULONG_PTR                           GapBytes;

    SCOPE_EXIT
    {
        for (auto &p : BytesArray)
            delete p;
    }
    SCOPE_EXIT_END;

    CurrentBytes = nullptr;
    GapBytes = 0;
    ZeroMemory(&CurrentPattern, sizeof(CurrentPattern));

    for (ml::String &p : Pattern.Split(' '))
    {
        if (!p)
            continue;

        if (p.GetCount() != 2)
            return nullptr;

        if (p[0] == '?' && p[1] == '?')
        {
            ++GapBytes;
        }
        else
        {
            ULONG Hex;

            if (GapBytes != 0)
            {
                CurrentPattern.Pattern = CurrentBytes->GetData();
                CurrentPattern.Size = CurrentBytes->GetSize();
                CurrentPattern.HeadOffsetToNext = CurrentPattern.Size + GapBytes;
                Patterns.Add(CurrentPattern);

                ZeroMemory(&CurrentPattern, sizeof(CurrentPattern));

                GapBytes = 0;
                BytesArray.Add(CurrentBytes);
                CurrentBytes = nullptr;
            }

            if (CurrentBytes == nullptr)
                CurrentBytes = new ml::String::ByteArray;

            Hex = p.ToHex();
            CurrentBytes->Add(Hex);
        }
    }

    if (CurrentBytes != nullptr)
    {
        BytesArray.Add(CurrentBytes);

        CurrentPattern.Pattern = CurrentBytes->GetData();
        CurrentPattern.Size = CurrentBytes->GetSize();
        Patterns.Add(CurrentPattern);
    }

    return SearchPatternSafe(Patterns.GetData(), Patterns.GetSize(), Begin, Length);
}

template<class T = VOID>
inline PVOID SearchPatternSafe(PCSTR Pattern, PVOID Begin, LONG_PTR Length)
{
    return SearchPatternSafe(ml::String::Decode((PVOID)Pattern, StrLengthA(Pattern), CP_ACP), Begin, Length);
}

inline PVOID ReverseSearchFunctionHeader(PVOID Start, ULONG_PTR Length)
{
    PBYTE Buffer;

    Buffer = (PBYTE)Start;

    for (; Length != 0; --Buffer, --Length)
    {
        switch (Buffer[0])
        {
            case CALL:
                // push    local_var_size
                // mov     eax, exception_handler
                // call    _SEH_prolog

                if (Buffer[-5] != 0xB8)
                    continue;

                if (Buffer[-7] == 0x6A)
                {
                    Buffer -= 7;
                }
                else if (Buffer[-10] == 0x68)
                {
                    Buffer -= 10;
                }
                else
                {
                    continue;
                }

                break;

            case 0x55:
                if (Buffer[1] != 0x8B || Buffer[2] != 0xEC)
                    continue;

                // push ebp
                // mov ebp, esp

                break;

            default:
                continue;
        }

        return Buffer;
    }

    return nullptr;
}

inline PVOID SearchStringReference(PLDR_MODULE Module, PVOID String, ULONG_PTR SizeInBytes, ULONG_PTR BeginOffset = 0)
{
    PVOID StringValue, StringReference;

    SEARCH_PATTERN_DATA Str[] =
    {
        ADD_PATTERN_(String, SizeInBytes),
    };

    StringValue = SearchPattern(Str, countof(Str), Module->DllBase, Module->SizeOfImage);
    if (StringValue == nullptr)
        return nullptr;

    SEARCH_PATTERN_DATA Stub[] =
    {
        ADD_PATTERN(&StringValue),
    };

    StringReference = SearchPattern(Stub, countof(Stub), PtrAdd(Module->DllBase, BeginOffset), PtrSub(Module->SizeOfImage, BeginOffset));
    if (StringReference == nullptr)
        return nullptr;

    return StringReference;
}


_ML_CPP_TAIL_

#endif // CPP_DEFINED

#endif // _SEARCHPATTERN_H_214862b7_e205_40fb_ba78_ccfbe9d04321_