#include "mylibrary.h"

_ML_C_HEAD_

PChar FASTCALL findexta(PCChar lpFileName)
{
    Char ch;
    PChar pDot, pSlash, pszFileName;

    pDot = 0;
    pSlash = 0;
    pszFileName = (PChar)lpFileName;
    LOOP_ALWAYS
    {
        ch = *pszFileName++;
        if (ch == 0)
            break;
        if (ch < 0)
        {
            if (*pszFileName == 0)
                break;
            ++pszFileName;
        }
        else if (ch == '/' || ch == '\\')
            pSlash = pszFileName - 1;
        else if (ch == '.')
            pDot = pszFileName - 1;
    }

    if (pDot == NULL)
        pDot = pszFileName - 1;
    else if (pDot < pSlash)
        pDot = pszFileName - 1;

    return pDot;
}

PWSTR FASTCALL findextw(PCWChar lpFileName)
{
    WChar ch;
    PWSTR pDot, pSlash, pszFileName;

    pDot = 0;
    pSlash = 0;
    pszFileName = (PWSTR)lpFileName;
    LOOP_ALWAYS
    {
        ch = *pszFileName++;
        if (ch == 0)
            break;
        if (ch == '/' || ch == '\\')
            pSlash = pszFileName - 1;
        else if (ch == '.')
            pDot = pszFileName - 1;
    }

    if (pDot == NULL)
        pDot = pszFileName - 1;
    else if (pDot < pSlash)
        pDot = pszFileName - 1;

    return pDot;
}

PChar FASTCALL findnamea(PCChar lpFileName)
{
    Char ch;
    PChar pBackSlash, pSlash, pszFileName;

    pBackSlash = 0;
    pSlash = 0;
    pszFileName = (PChar)lpFileName;
    LOOP_ALWAYS
    {
        ch = *pszFileName++;
        if (ch == 0)
            break;
        if (ch < 0)
        {
            if (*pszFileName == 0)
                break;
            ++pszFileName;
        }
        else if (ch == '/')
            pSlash = pszFileName - 1;
        else if (ch == '\\')
            pBackSlash = pszFileName - 1;
    }

    pBackSlash = MY_MAX(pBackSlash, pSlash);
    if (pBackSlash == 0)
        pBackSlash = (PChar)lpFileName;
    else
        ++pBackSlash;

    return pBackSlash;
}

PWSTR FASTCALL findnamew(PCWChar lpFileName, SizeT Length /* = -1 */)
{
    WChar ch;
    PWSTR pBackSlash, pSlash, pszFileName;

    if (lpFileName == NULL)
        return NULL;

    pBackSlash = 0;
    pSlash = 0;
    pszFileName = (PWSTR)lpFileName;

    for (; Length != 0; --Length)
    {
        ch = *pszFileName++;
        if (ch == 0)
            break;
        if (ch == '/')
            pSlash = pszFileName - 1;
        else if (ch == '\\')
            pBackSlash = pszFileName - 1;
    }

    pBackSlash = MY_MAX(pBackSlash, pSlash);
    if (pBackSlash == 0)
        pBackSlash = (PWSTR)lpFileName;
    else
        ++pBackSlash;

    return pBackSlash;
}

PChar StringLowerA(PChar pszString, ULong Length)
{
    PChar pszText;
    for (pszText = pszString; *pszText && Length != 0; ++pszText, --Length)
    {
        Char ch = *pszText;
        if (ch < 0)
        {
            if (*++pszText == 0 || --Length == 0)
                break;

            continue;
        }

        if (IN_RANGE('A', ch, 'Z'))
            *pszText = ch | 0x20;
    }

    return pszString;
}

PWChar StringLowerW(PWChar String, ULong Length)
{
    PWChar Text;

    if (Length == -1)
        Length = StrLengthW(String);

    for (Text = String; Length != 0; ++Text, --Length)
    {
        *Text = CHAR_LOWER(*Text);
    }

    return String;
}

PChar StringUpperA(PChar pszString, ULong Length)
{
    PChar pszText;
    for (pszText = pszString; *pszText && Length != 0; ++pszText, --Length)
    {
        Char ch = *pszText;
        if (ch < 0)
        {
            if (*++pszText == 0 || --Length == 0)
                break;

            continue;
        }

        if (IN_RANGE('a', ch, 'z'))
            *pszText = ch & 0xDF;
    }

    return pszString;
}

PWChar StringUpperW(PWChar String, ULong Length)
{
    PWChar Text;

    if (Length == -1)
        Length = StrLengthW(String);

    for (Text = String; Length != 0; ++Text, --Length)
    {
        *Text = CHAR_UPPER(*Text);
    }

    return String;
}

Long_Ptr FASTCALL StrLengthA(PCChar pString)
{
#if 0

    PCChar s;
    ULong quad, v1, flag;

    if (pString == NULL)
        return 0;

    s = pString;
    while ((Long_Ptr)(s) & 3)
    {
        if (*s++ == 0)
        {
            --s;
            return s - pString;
        }
    }

    do
    {
        quad = *(PULong)s;
        s += sizeof(quad);

        quad = ((quad + 0xFEFEFEFF) & ~quad) & 0x80808080;

    } while (quad == 0);

    v1 = quad >> 16;
    flag = quad & 0x8080;

    quad = flag ? quad: (v1);
    s += flag ? 0 : 2;

    s -= 3 + ((quad >> 7) & 1);

    return s - pString;

#else

    Long ch;
    Long_Ptr SizeOfUnit;
    PCChar pBuffer = pString;

    if (pString == NULL)
        return 0;

    SizeOfUnit = sizeof(Int32);
    while ((Long_Ptr)(pBuffer) & 3)
    {
        if (*pBuffer++ == 0)
        {
            --pBuffer;
            goto end_of_calc;
        }
    }

    LOOP_ALWAYS
    {
        Long temp;
        ch = *(PLong)pBuffer;
        pBuffer += SizeOfUnit;

        temp = (0x7EFEFEFF + ch) ^ (ch ^ -1);
        if ((temp & 0x81010100) == 0)
            continue;

        if (LoByte(ch) == 0)
        {
            pBuffer -= SizeOfUnit;
            break;
        }

        if ((ch & 0xFF00) == 0)
        {
            pBuffer -= SizeOfUnit - 1;
            break;
        }

        if ((ch & 0x00FF0000) == 0)
        {
            pBuffer -= SizeOfUnit - 2;
            break;
        }

        if ((ch & 0xFF000000) == 0)
        {
            pBuffer -= SizeOfUnit - 3;
            break;
        }
    }

end_of_calc:
    return pBuffer - pString;

#endif
}

Long_Ptr FASTCALL StrLengthW(PCWChar pString)
{
    Long ch;
    PCWChar pBuffer;

    if (pString == NULL)
        return 0;

    pBuffer = pString;
    while ((Int_Ptr)pBuffer & 3)
    {
        if (*pBuffer++ == 0)
        {
            --pBuffer;
            goto end_of_strlen;
        }
    }

    LOOP_ALWAYS
    {
        ch = *(PInt)pBuffer;
        if ((ch & 0xFFFF) == 0)
        {
            break;
        }
        else if ((ch & 0xFFFF0000) == 0)
        {
            pBuffer = (PCWChar)((PByte)pBuffer + sizeof(*pBuffer));
            break;
        }

        pBuffer = (PCWChar)((PByte)pBuffer + sizeof(ch));
    }

end_of_strlen:

    return pBuffer - pString;
}

PChar FASTCALL StrCopyA(PChar DestinationString, PCChar SourceString)
{
    UInt32 ch, SizeOfUnit;
    PChar Buffer = DestinationString;

    if (Buffer == NULL || SourceString == NULL)
        return DestinationString;

    SizeOfUnit = sizeof(*(PInt)Buffer);
    while ((ULong_Ptr)(SourceString) & 3)
    {
        if ((*Buffer++ = *SourceString++) == 0)
            return DestinationString;
    }

    LOOP_ALWAYS
    {
        UInt32 temp;
        ch = *(PInt)SourceString;

        temp = (0x7EFEFEFF + ch) ^ (ch ^ -1);
        if ((temp & 0x81010100) == 0)
        {
            *(PULong)Buffer = ch;
            Buffer += SizeOfUnit;
            SourceString += SizeOfUnit;
            continue;
        }

        if (LoByte(ch) == 0)
        {
            *Buffer = (Char)ch;
            break;
        }

        if ((ch & 0xFF00) == 0)
        {
            *(PUInt16)Buffer = (UInt16)ch;
            break;
        }

        if ((ch & 0x00FF0000) == 0)
        {
            *(PUInt16)Buffer = (UInt16)ch;
            Buffer[2] = LoByte(HiWord(ch));
            break;
        }

        *(PULong)Buffer = ch;

        if ((ch & 0xFF000000) == 0)
            break;

        Buffer += SizeOfUnit;
        SourceString += SizeOfUnit;
    }

    return DestinationString;
}

PWChar FASTCALL StrCopyW(PWChar DestinationString, PCWChar SourceString)
{
    PWChar Destination;
    ULong  ch;

    if (DestinationString == NULL || SourceString == NULL)
        return DestinationString;

    Destination = DestinationString;

    while ((ULong_Ptr)SourceString & 3)
    {
        if ((*DestinationString++ = *SourceString++) == 0)
            return Destination;
    }

    do
    {
        ch = *(PULong)SourceString;

        if (TEST_BITS(ch, 0x0000FFFF) == 0)
        {
            *DestinationString = (WChar)ch;
            break;
        }

        *(PULong)DestinationString = ch;
        DestinationString += 2;
        SourceString += 2;
    } while (TEST_BITS(ch, 0xFFFF0000) != 0);

    return Destination;
}

PChar FASTCALL StrConcatenateA(PChar DestinationString, PCChar SourceString)
{
    return StrCopyA(DestinationString + StrLengthA(DestinationString), SourceString);
}

PWChar FASTCALL StrConcatenateW(PWChar DestinationString, PCWChar SourceString)
{
    return StrCopyW(DestinationString + StrLengthW(DestinationString), SourceString);
}

Long_Ptr FASTCALL StrNICompareA(PCChar pString1, PCChar pString2, SizeT LengthToCompare, LongPtr CaseConvertTo)
{
    Long ch1, ch2, temp;

    if (LengthToCompare == 0)
        return 0;

    if (pString1 == NULL || pString2 == NULL)
        return (Long_Ptr)pString1 - (Long_Ptr)pString2;

    do
    {
        ch1 = *pString1;
        ch2 = *pString2;
        temp = ch1 ^ ch2;
        if ((ch1 | ch2) < 0)
        {
            if (temp != 0 )
                break;

            if (--LengthToCompare == 0)
                break;

            ch1 = *++pString1;
            ch2 = *++pString2;
            if (ch1 != ch2)
                break;
        }
        else
        {
            if (TEST_BITS(temp, 0xDF))
                break;
        }

        if (ch1 == 0)
            break;

        ++pString1;
        ++pString2;

    } while (--LengthToCompare != 0);

    switch (CaseConvertTo)
    {
        case StrCmp_Keep:
            break;

        case StrCmp_ToLower:
            ch1 = CHAR_LOWER(ch1);
            ch2 = CHAR_LOWER(ch2);
            break;

        case StrCmp_ToUpper:
            ch1 = CHAR_UPPER(ch1);
            ch2 = CHAR_UPPER(ch2);
            break;
    }

    return LengthToCompare == 0 ? 0 : (ULong)ch1 < (ULong)ch2 ? -1 : (ULong)ch1 > (ULong)ch2 ? 1 : 0;
}

Long_Ptr FASTCALL StrNICompareW(PCWChar pString1, PCWChar pString2, SizeT LengthToCompare, LongPtr CaseConvertTo)
{
    ULong ch1, ch2;

    if (LengthToCompare == 0)
        return 0;

    if (pString1 == NULL || pString2 == NULL)
        return 1;

    do
    {
        ch1 = *(PInt16)pString1;
        ch2 = *(PInt16)pString2;

        if ((ch1 | ch2) & 0xFF80)
        {
            if (ch1 != ch2)
                break;
        }
        else if (((ch1 ^ ch2) & 0xFFDF) != 0)
        {
            break;
        }


#if 0
        ch1 >>= 16;
        ch2 >>= 16;
        if ((ch1 | ch2) & 0xFF80)
        {
            if (ch1 != ch2)
                break;
        }
        else if (((ch1 ^ ch2) & 0xFFDF) != 0)
            break;

        pString1 = (PCWChar)((PByte)pString1 + sizeof(ch1));
        pString2 = (PCWChar)((PByte)pString2 + sizeof(ch1));
#endif
        ++pString1;
        ++pString2;
    } while (--LengthToCompare != 0 && ch1);

    switch (CaseConvertTo)
    {
        case StrCmp_Keep:
            break;

        case StrCmp_ToLower:
            ch1 = CHAR_LOWER(ch1);
            ch2 = CHAR_LOWER(ch2);
            break;

        case StrCmp_ToUpper:
            ch1 = CHAR_UPPER(ch1);
            ch2 = CHAR_UPPER(ch2);
            break;
    }

    // return LengthToCompare == 0 ? 0 : (Long_Ptr)(Int16)(ch1 - ch2);
    return LengthToCompare == 0 ? 0 : (ULong)ch1 < (ULong)ch2 ? -1 : (ULong)ch1 > (ULong)ch2 ? 1 : 0;
}

Long_Ptr FASTCALL StrNCompareA(PCChar pString1, PCChar pString2, SizeT LengthToCompare)
{
    Long ch1, ch2, xor;

    if (LengthToCompare == 0)
        return 0;

    if (pString1 == NULL || pString2 == NULL)
        return pString1 - pString2;

    while (((Long_Ptr)pString1 & (Long_Ptr)pString2) & 3)
    {
        ch1 = *pString1;
        ch2 = *pString2;
        if (ch1 ^ ch2)
            return (Long_Ptr)(Char)(ch1 - ch2);

        *pString1++;
        *pString2++;
        --LengthToCompare;
    }

    ch1 = 0;
    ch2 = 0;
    xor = 0;
    if (LengthToCompare > 3) do
    {
        ch1 = *(PLong)pString1;
        ch2 = *(PLong)pString2;

        xor = ch1 ^ ch2;
        if (TEST_BITS(ch1, 0xFF) == 0)
        {
            ch1 &= 0xFF;
            ch2 &= 0xFF;
            xor &= 0xFF;
            LengthToCompare = 0;
            break;
        }
        else if (TEST_BITS(ch1, 0xFF00) == 0)
        {
            ch1 &= 0xFFFF;
            ch2 &= 0xFFFF;
            xor &= 0xFFFF;
            LengthToCompare = 0;
            break;
        }
        else if (TEST_BITS(ch1, 0xFF0000) == 0)
        {
            ch1 &= 0xFFFFFF;
            ch2 &= 0xFFFFFF;
            xor &= 0xFFFFFF;
            LengthToCompare = 0;
            break;
        }
        else if (TEST_BITS(ch1, 0xFF000000) == 0)
        {
            LengthToCompare = 0;
            break;
        }

        if (xor)
        {
            return ch1 < ch2 ? -1 : 1;
        }

        pString1 = (PCChar)((PByte)pString1 + sizeof(ch1));
        pString2 = (PCChar)((PByte)pString2 + sizeof(ch2));

        LengthToCompare -= 4;
    } while (LengthToCompare >= 4);

    while (LengthToCompare--)
    {
        ch1 = *pString1++;
        ch2 = *pString2++;
        xor = ch1 ^ ch2;
        if (xor != 0)
            break;
    }

    if (xor == 0)
        return 0;

    return ch1 < ch2 ? -1 : 1;
}

Long_Ptr FASTCALL StrNCompareW(PCWChar pString1, PCWChar pString2, SizeT LengthToCompare)
{
    Long ch1, ch2;
//    WChar ch1, ch2;

    if (LengthToCompare == 0)
        return 0;

    if (pString1 == NULL || pString2 == NULL)
        return pString1 - pString2;

    while (((Long_Ptr)pString1 & (Long_Ptr)pString2) & 3)
    {
        ch1 = *pString1;
        ch2 = *pString2;
        if (ch1 != ch2)
        {
            ch1 = (Long)(Int16)(ch1 - ch2);
            return ch1 < 0 ? -1 : ch1 > 0 ? 1 : 0;
//            return (Int)(Int16)(ch1 - ch2);
        }

        if (ch1 == 0)
            return 0;

        *pString1++;
        *pString2++;
    }

    do
    {
#if  0
        ch1 = pString1[0];
        if (ch1 != pString2[0])
            return ch1 - pString2[0];
        if (ch1 == 0)
            return 0;

        ch1 = pString1[1];
        if (ch1 != pString2[1])
            return ch1 - pString2[1];
        if (ch1 == 0)
            return 0;
#else
        ch1 = *(PInt32)pString1;
        ch2 = *(PInt32)pString2;
        if (LoWord(ch1) != LoWord(ch2))
        {
            ch1 = (Int)(Int16)((ch1 - ch2) & 0xFFFF);
            return ch1 < 0 ? -1 : ch1 > 0 ? 1 : 0;
//            return (Int)(Int16)(ch1 - ch2);
        }

        if (LoWord(ch1) == 0)
            return 0;

        ch1 >>= 16;
        ch2 >>= 16;
        if (ch1 != ch2)
        {
            ch1 = (Int)(Int16)(ch1 - ch2);
            return ch1 < 0 ? -1 : ch1 > 0 ? 1 : 0;
//            return (Int)(Int16)(ch1 - ch2);
        }

        if (ch1 == 0)
            return 0;
#endif
        pString1 = (PCWChar)((PByte)pString1 + sizeof(ch1));
        pString2 = (PCWChar)((PByte)pString2 + sizeof(ch2));

    } while (--LengthToCompare != 0);

    return 0;
}

PWSTR StrFindCharW(PCWChar lpString, WChar CharMatch)
{
    WChar ch;

    if (lpString == NULL)
        return NULL;

    if (CharMatch == 0)
        return (PWSTR)lpString + StrLengthW(lpString);

    do
    {
        ch = *lpString++;

    } while (ch && ch != CharMatch);

    return ch ? (PWSTR)lpString - 1 : NULL;
}

PWSTR PeekLineW(PVoid pBuffer, PVoid pEndOfBuffer, PVoid pOutBuffer, PSizeT pcbOutBuffer)
{
    WChar  ch;
    PWSTR pStart, pEnd, pOutput;
    SSizeT BufferSize;
    SizeT  cbOutSize;

    if (pBuffer == NULL)
        return NULL;

    if (pcbOutBuffer == NULL)
        return NULL;

    if (pcbOutBuffer != NULL)
    {
        BufferSize = *pcbOutBuffer;
        *pcbOutBuffer = 0;
    }
    else
    {
        BufferSize = 0;
    }

    pStart = (PWSTR)pBuffer;
    pEnd   = pEndOfBuffer != NULL ? (PWSTR)pEndOfBuffer : pStart + StrLengthW(pStart);

    if (pStart >= pEnd)
        return NULL;

    pOutput = (PWSTR)pOutBuffer;
    if (*pStart == BOM_UTF16_LE)
        ++pStart;

    cbOutSize = 0;
    while (pStart < pEnd)
    {
        ch = *pStart++;
        if (ch == '\n')
            break;

        if (BufferSize > 0 && ch != '\r')
        {
            if (pOutput != NULL)
                *pOutput++ = ch;

            cbOutSize  += sizeof(ch);
            BufferSize -= sizeof(ch);
        }
    }

    if (pOutput != NULL && BufferSize > 0)
        *pOutput = 0;

    *pcbOutBuffer = cbOutSize;

    if (cbOutSize == 0)
        return pStart < pEnd ? pStart : NULL;

    return pStart;
}

#define DECL_NUMBER_TABLE32(name) \
        UInt32 name[] = \
        { \
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, \
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, \
            0x03020100, 0x07060504, 0xFFFF0908, 0xFFFFFFFF, 0x0C0B0AFF, 0xFF0F0E0D, \
        }

Int32 StringToInt32A(PCChar pszString)
{
/*
    Char NumberTable[] =
    {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,  0,  1,
         2,  3,  4,  5,  6,  7,  8,  9, -1, -1,
        -1, -1, -1, -1, -1, 10, 11, 12, 13, 14,
        15,
    };
*/
    if (pszString == NULL)
        return 0;

    DECL_NUMBER_TABLE32(NumberTable32);
    PChar NumberTable = (PChar)NumberTable32;

    Int32 Result, Temp, Sign;
    Char ch;

    if (*pszString == '-')
    {
        Sign = 1;
        ++pszString;
    }
    else
    {
        Sign = 0;
    }

    Result = 0;
    LOOP_ALWAYS
    {
        ch = *pszString++;
        if (ch == 0)
            break;
        if (IN_RANGE('a', ch, 'z'))
            ch &= 0xDF;
        if ((UInt32)ch > sizeof(NumberTable32))
            return 0;

        Temp = NumberTable[ch];
        if ((Char)Temp == (Char)-1)
            break;

        Result = Result * 10 + Temp;
    }

    if (Sign)
        Result = -Result;

    return Result;
}

Int32 StringToInt32W(PCWChar pszString)
{
/*
    Char NumberTable[] =
    {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,  0,  1,
         2,  3,  4,  5,  6,  7,  8,  9, -1, -1,
        -1, -1, -1, -1, -1, 10, 11, 12, 13, 14,
        15,
    };
*/
    if (pszString == NULL)
        return 0;

    DECL_NUMBER_TABLE32(NumberTable32);
    PChar NumberTable = (PChar)NumberTable32;

    Int32 Result, Temp, Sign;
    WChar ch;

    if (*pszString == '-')
    {
        Sign = 1;
        ++pszString;
    }
    else
    {
        Sign = 0;
    }

    Result = 0;
    LOOP_ALWAYS
    {
        ch = *pszString++;
        if (ch == 0)
            break;
        if (IN_RANGE('a', ch, 'z'))
            ch &= 0xFFDF;
        if ((UInt32)ch > sizeof(NumberTable32))
            break;

        Temp = NumberTable[ch];
        if ((Char)Temp == (Char)-1)
            break;

        Result = Result * 10 + Temp;
    }

    if (Sign)
        Result = -Result;

    return Result;
}

Int32 StringToInt32HexW(PCWSTR pszString)
{
    Int32 Result;
    WChar ch;

    Result = 0;
    LOOP_ALWAYS
    {
        ch = *pszString++;
        if (ch == 0)
            break;
        if (IN_RANGE('0', ch, '9'))
        {
            ch -= '0';
        }
        else
        {
            ch &= 0xFFDF;
            if (IN_RANGE('A', ch, 'F'))
                ch = ch - 'A' + 10;
            else
                break;
        }

        Result = (Result << 4) | ch;
    }

    return Result;
}

Int64 StringToInt64HexW(PCWSTR pszString)
{
    Int64 Result;
    WChar ch;

    Result = 0;
    LOOP_ALWAYS
    {
        ch = *pszString++;
        if (ch == 0)
            break;
        if (IN_RANGE('0', ch, '9'))
        {
            ch -= '0';
        }
        else
        {
            ch &= 0xFFDF;
            if (IN_RANGE('A', ch, 'F'))
                ch = ch - 'A' + 10;
            else
                break;
        }

        Result = (Result << 4) | ch;
    }

    return Result;
}

Int64 StringToInt64A(PChar pszString)
{
/*
    Char NumberTable[] =
    {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,  0,  1,
         2,  3,  4,  5,  6,  7,  8,  9, -1, -1,
        -1, -1, -1, -1, -1, 10, 11, 12, 13, 14,
        15,
    };
*/
    if (pszString == NULL)
        return 0;

    DECL_NUMBER_TABLE32(NumberTable32);
    PChar NumberTable = (PChar)NumberTable32;

    Int64 Result, Temp, Sign;
    Char ch;

    if (*pszString == '-')
    {
        Sign = 1;
        ++pszString;
    }
    else
    {
        Sign = 0;
    }

    Result = 0;
    LOOP_ALWAYS
    {
        ch = *pszString++;
        if (ch == 0)
            break;
        if (IN_RANGE('a', ch, 'z'))
            ch &= 0xDF;
        if ((UInt32)ch > sizeof(NumberTable32))
            return 0;

        Temp = NumberTable[ch];
        if ((Char)Temp == (Char)-1)
            break;

        Result = Result * 10 + Temp;
    }

    if (Sign)
        Result = -Result;

    return Result;
}

Int64 StringToInt64W(PCWChar pszString)
{
/*
    Char NumberTable[] =
    {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,  0,  1,
         2,  3,  4,  5,  6,  7,  8,  9, -1, -1,
        -1, -1, -1, -1, -1, 10, 11, 12, 13, 14,
        15,
    };
*/
    if (pszString == NULL)
        return 0;

    DECL_NUMBER_TABLE32(NumberTable32);
    PChar NumberTable = (PChar)NumberTable32;

    Int64 Result, Temp, Sign;
    WChar ch;

    if (*pszString == '-')
    {
        Sign = 1;
        ++pszString;
    }
    else
    {
        Sign = 0;
    }

    Result = 0;
    LOOP_ALWAYS
    {
        ch = *pszString++;
        if (ch == 0)
            break;
        if (IN_RANGE('a', ch, 'z'))
            ch &= 0xFFDF;
        if ((UInt32)ch > sizeof(NumberTable32))
            break;

        Temp = NumberTable[ch];
        if ((Char)Temp == (Char)-1)
            break;

        Result = Result * 10 + Temp;
    }

    if (Sign)
        Result = -Result;

    return Result;
}

Bool IsShiftJISChar(SizeT uChar)
{
    Byte LowByte, HighByte;

    LowByte = LOBYTE(uChar);
    HighByte = HIBYTE(uChar);
    if ((HighByte >= 0x80 && HighByte <= 0x9F) ||
        (HighByte >= 0xE0 && HighByte <= 0xFC))
    {
        if ((LowByte >= 0x40 && LowByte <= 0x7E) ||
            (LowByte >= 0x80 && LowByte <= 0xFC))
        {
            return True;
        }
    }
/*
    else if (HighByte >= 0xA1 && HighByte <= 0xDF)
        return True;
*/
    return False;
}

Bool IsShiftJISString(PCChar pString, SizeT Length)
{
    Bool   bMBChar;
    PCChar p;

    if (pString == NULL)
        return False;

    bMBChar = False;
    p = pString;
    for (; Length; ++p, --Length)
    {
        Char  c;
        Int32 ch;

        c = *p;
        CONTINUE_IF(c > 0);
        BREAK_IF(c == 0 || --Length == 0);

        ch = *(PUInt16)p++;
        if (!IsShiftJISChar(SWAPCHAR(ch)))
            return False;
        else
            bMBChar = True;
    }

    return bMBChar;
}

Bool
StrMatchExpression(
    IN PWChar   Expression,
    IN PWChar   Name,
    IN Bool     IgnoreCase,
    IN PWChar   UpcaseTable OPTIONAL
)
{
    return Rtl::IsNameInExpression(Expression, Name, IgnoreCase, UpcaseTable);
}

/***********************************************************************
 *           WPRINTF_ParseFormatA
 *
 * Parse a format specification. A format specification has the form:
 *
 * [-][#][0][width][.precision]type
 *
 * Return value is the length of the format specification in characters.
 ***********************************************************************/
Int FormatString_ParseFormatA( PCChar format, FMTSTR_FORMAT *res )
{
    PCChar p = format;

    res->flags = 0;
    res->width = 0;
    res->precision = 0;
    if (*p == 'I' && p[1] == '6' && p[2] == '4') { res->flags |= FMTSTR_FLAG_I64; p += 3; }
    if (*p == '-') { res->flags |= FMTSTR_FLAG_LEFTALIGN; p++; }
    if (*p == '#') { res->flags |= FMTSTR_FLAG_PREFIX_HEX; p++; }
    if (*p == '0') { res->flags |= FMTSTR_FLAG_ZEROPAD; p++; }
    while ((*p >= '0') && (*p <= '9'))  /* width field */
    {
        res->width = res->width * 10 + *p - '0';
        p++;
    }
    if (*p == '.')  /* precision field */
    {
        p++;
        while ((*p >= '0') && (*p <= '9'))
        {
            res->precision = res->precision * 10 + *p - '0';
            p++;
        }
    }
    if (*p == 'l') { res->flags |= FMTSTR_FLAG_LONG; p++; }
    else if (*p == 'h') { res->flags |= FMTSTR_FLAG_SHORT; p++; }
    else if (*p == 'w') { res->flags |= FMTSTR_FLAG_WIDE; p++; }
    switch(*p)
    {
        case 'c':
            res->type = (res->flags & FMTSTR_FLAG_LONG) ? FMTSTR_FORMAT_WCHAR : FMTSTR_FORMAT_CHAR;
            break;

        case 'C':
            res->type = (res->flags & FMTSTR_FLAG_SHORT) ? FMTSTR_FORMAT_CHAR : FMTSTR_FORMAT_WCHAR;
            break;

        case 'u':
            if (!TEST_BITS(res->flags, FMTSTR_FLAG_FORCESIGNED))
            {
                res->type = FMTSTR_FORMAT_UNSIGNED;
                break;
            }
            NO_BREAK;
        case 'd':
        case 'i':
            res->type = FMTSTR_FORMAT_SIGNED;
            break;

        case 's':
            res->type = (res->flags & (FMTSTR_FLAG_LONG |FMTSTR_FLAG_WIDE)) ? FMTSTR_FORMAT_WSTRING : FMTSTR_FORMAT_STRING;
            break;

        case 'S':
            res->type = (res->flags & (FMTSTR_FLAG_SHORT|FMTSTR_FLAG_WIDE)) ? FMTSTR_FORMAT_STRING : FMTSTR_FORMAT_WSTRING;
            break;

        case 'X':
            res->flags |= FMTSTR_FLAG_UPPER_HEX;
            /* fall through */
        case 'x':
            res->type = FMTSTR_FORMAT_HEXA;
            break;

#if MY_OS_WIN32
        case 'b':
        case 'B':
            res->type = FMTSTR_FORMAT_BINARY;
            break;
#endif

        default: /* unknown format char */
            res->type = FMTSTR_FORMAT_UNKNOWN;
            p--;  /* print format as normal char */
            break;
    }
    return (Int)(p - format) + 1;
}


/***********************************************************************
 *           WPRINTF_ParseFormatW
 *
 * Parse a format specification. A format specification has the form:
 *
 * [-][#][0][width][.precision]type
 *
 * Return value is the length of the format specification in characters.
 */

Int FormatString_ParseFormatW(PCWChar format, FMTSTR_FORMAT *res)
{
    PCWChar p = format;

    res->flags = 0;
    res->width = 0;
    res->precision = 0;
    if (*p == 'I' && p[1] == '6' && p[2] == '4') { res->flags |= FMTSTR_FLAG_I64; p += 3; }
    if (*p == '+') { res->flags |= FMTSTR_FLAG_FORCESIGNED; p++; }
    if (*p == '-') { res->flags |= FMTSTR_FLAG_LEFTALIGN; p++; }
    if (*p == '#') { res->flags |= FMTSTR_FLAG_PREFIX_HEX; p++; }
    if (*p == '0') { res->flags |= FMTSTR_FLAG_ZEROPAD; p++; }
    while ((*p >= '0') && (*p <= '9'))  /* width field */
    {
        res->width = res->width * 10 + *p - '0';
        p++;
    }
    if (*p == '.')  /* precision field */
    {
        p++;
        while ((*p >= '0') && (*p <= '9'))
        {
            res->precision = res->precision * 10 + *p - '0';
            p++;
        }
    }
    if (*p == 'l') { res->flags |= FMTSTR_FLAG_LONG; p++; }
    else if (*p == 'h') { res->flags |= FMTSTR_FLAG_SHORT; p++; }
    else if (*p == 'w') { res->flags |= FMTSTR_FLAG_WIDE; p++; }
    switch((Char)*p)
    {
        case 'c':
            res->type = (res->flags & FMTSTR_FLAG_SHORT) ? FMTSTR_FORMAT_CHAR : FMTSTR_FORMAT_WCHAR;
            break;

        case 'C':
            res->type = (res->flags & FMTSTR_FLAG_LONG) ? FMTSTR_FORMAT_WCHAR : FMTSTR_FORMAT_CHAR;
            break;

        case 'u':
            if (!TEST_BITS(res->flags, FMTSTR_FLAG_FORCESIGNED))
            {
                res->type = FMTSTR_FORMAT_UNSIGNED;
                break;
            }
            NO_BREAK;
        case 'd':
        case 'i':
            res->type = FMTSTR_FORMAT_SIGNED;
            break;

        case 's':
            res->type = ((res->flags & FMTSTR_FLAG_SHORT) && !(res->flags & FMTSTR_FLAG_WIDE)) ? FMTSTR_FORMAT_STRING : FMTSTR_FORMAT_WSTRING;
            break;

        case 'S':
            res->type = (res->flags & (FMTSTR_FLAG_LONG|FMTSTR_FLAG_WIDE)) ? FMTSTR_FORMAT_WSTRING : FMTSTR_FORMAT_STRING;
            break;

        case 'p':
            res->width = 8;
            res->flags |= FMTSTR_FLAG_ZEROPAD;
            /* fall through */
        case 'X':
            res->flags |= FMTSTR_FLAG_UPPER_HEX;
            /* fall through */
        case 'x':
            res->type = FMTSTR_FORMAT_HEXA;
            break;

#if MY_OS_WIN32
        case 'b':
        case 'B':
            res->type = FMTSTR_FORMAT_BINARY;
            break;
#endif

        default:
            res->type = FMTSTR_FORMAT_UNKNOWN;
            p--;  /* print format as normal char */
            break;
    }

    return (Int)(p - format) + 1;
}

inline
UInt
FormatString_GetLen(
    FMTSTR_FORMAT   *format,
    FMTSTR_DATA     *arg,
    PChar            number,
    UInt             maxlen
)
{
    UInt len = 0, UpperFlag;

    if (format->flags & FMTSTR_FLAG_LEFTALIGN)
        format->flags &= ~FMTSTR_FLAG_ZEROPAD;

    if (format->width > maxlen)
        format->width = maxlen;

    switch(format->type)
    {
        case FMTSTR_FORMAT_CHAR:
        case FMTSTR_FORMAT_WCHAR:
            return (format->precision = 1);

        case FMTSTR_FORMAT_STRING:
            if (arg->lpcstr_view)
            {
                len = StrLengthA(arg->lpcstr_view);
                if (format->precision != 0 && format->precision > len)
                    len = format->precision;
/*
                for (len = 0; format->precision == 0 || (len < format->precision); len++)
                {
                    Char ch = arg->lpcstr_view[len];
                    if (ch == 0)
                        break;
                }
*/
            }
            else
            {
                len = format->precision == NULL ? 6 : ML_MIN(6, format->precision);
            }

            if (len > maxlen)
                len = maxlen;

            return (format->precision = len);

        case FMTSTR_FORMAT_WSTRING:
            if (arg->lpcwstr_view)
            {
                len = StrLengthW(arg->lpcwstr_view);
                if (format->precision != 0 && format->precision > len)
                    len = format->precision;
/*
                for (len = 0; !format->precision || (len < format->precision); len++)
                    if (arg->lpcwstr_view[len] == 0)
                        break;
*/
            }
            else
            {
                len = format->precision == NULL ? 6 : ML_MIN(6, format->precision);
            }

            if (len > maxlen)
                len = maxlen;
/*
            if (!arg->lpcwstr_view) arg->lpcwstr_view = NULL;
            for (len = 0; !format->precision || (len < format->precision); len++)
                if (!*(arg->lpcwstr_view + len)) break;
            if (len > maxlen) len = maxlen;
*/
            return (format->precision = len);
/*
        case FMTSTR_FORMAT_SIGNED:
            if ((format->flags & WPRINTF_I64) == 0)
            {
                arg->int64_view = arg->int_view;
            }

            if ((INT64)arg->int64_view < 0)
            {
                *number++ = '-';
                arg->int64_view = -(INT64)arg->int64_view;
                ++len;
            }
        case FMTSTR_FORMAT_UNSIGNED:
            for (UINT64 i = arg->int64_view; i; i /= 10)
            {
                UINT64 v = i % 10;
                ++number;
            }
            for (UINT64 i = arg->int64_view; i; i /= 10)
            {
                UINT64 v = i % 10;
                *--number = v + '0';
                ++len;
            }
            break;
*/
        case FMTSTR_FORMAT_SIGNED:
            if (format->flags & FMTSTR_FLAG_I64)
            {
                if ((Int64)arg->int64_view < 0)
                {
                    *number++ = '-';
                    arg->int64_view = -(Int64)arg->int64_view;
                    ++len;
                }
            }
            else
            {
                if ((Int)arg->int_view < 0)
                {
                    *number++ = '-';
                    arg->int_view = -(Int)arg->int_view;
                    ++len;
                }
            }
            NO_BREAK;

        case FMTSTR_FORMAT_UNSIGNED:
            if (format->flags & FMTSTR_FLAG_I64)
            {
                if (arg->int64_view == 0)
                {
                    *number++ = '0';
                    ++len;
                    break;
                }

                for (UInt64 i = arg->int64_view; i; i /= 10)
                {
                    ++number;
                }
                for (UInt64 i = arg->int64_view; i; i /= 10)
                {
                    UInt64 v = i % 10;
                    *--number = (Char)(v + '0');
                    ++len;
                }
            }
            else
            {
                if (arg->int_view == 0)
                {
                    *number++ = '0';
                    ++len;
                    break;
                }

                for (UInt i = arg->int_view; i; i /= 10)
                {
                    ++number;
                }
                for (UInt i = arg->int_view; i; i /= 10)
                {
                    UInt v = i % 10;
                    *--number = (Char)(v + '0');
                    ++len;
                }
            }
            break;

        case FMTSTR_FORMAT_HEXA:
            UpperFlag = (format->flags & FMTSTR_FLAG_UPPER_HEX) ? 0xDF : 0xFF;

            if (format->flags & FMTSTR_FLAG_I64)
            {
                for (UInt64 i = arg->int64_view; i; i >>= 4)
                    ++number;

                for (UInt64 i = arg->int64_view; i; i >>= 4)
                {
                    UInt64 v = i & 0xF;
                    *--number = (Char)(v < 10 ? v + '0' : (v - 10 + ('a' & UpperFlag)));
                    ++len;
                }
            }
            else
            {
                for (UInt i = arg->int_view; i; i >>= 4)
                    ++number;

                for (UInt i = arg->int_view; i; i >>= 4)
                {
                    UInt v = i & 0xF;
                    *--number = (Char)(v < 10 ? v + '0' : (v - 10 + ('a' & UpperFlag)));
                    ++len;
                }
            }
            break;

#if MY_OS_WIN32

        case FMTSTR_FORMAT_BINARY:
            {
                ULong bit;
                Large_Integer val;

                val.QuadPart = arg->int64_view;
                if ((format->flags & FMTSTR_FLAG_I64) && val.HighPart != 0)
                {
                    _BitScanReverse(&bit, val.HighPart);
                    bit += 32;
                }
                else if (val.LowPart != 0)
                {
                    _BitScanReverse(&bit, val.LowPart);
                }
                else
                {
                    bit = 0;
                }

                number += bit;
                ++bit;
                do
                {
                    *number-- = (Char)(val.LowPart & 1) + '0';
                    ++len;
                } while (--bit);
            }
            break;

#endif

        default:
            return 0;
    }

    if (len > maxlen) len = maxlen;
    if (format->precision < len) format->precision = len;
    if (format->precision > maxlen) format->precision = maxlen;
    if ((format->flags & FMTSTR_FLAG_ZEROPAD) && (format->width > format->precision))
        format->precision = format->width;
    if (format->flags & FMTSTR_FLAG_PREFIX_HEX) len += 2;
    return len;
}

/***********************************************************************
 *           wvnsprintfA   (SHLWAPI.@)
 *
 * Print formatted output to a string, up to a maximum number of chars.
 *
 * PARAMS
 * buffer [O] Destination for output string
 * maxlen [I] Maximum number of characters to write
 * spec   [I] Format string
 *
 * RETURNS
 *  Success: The number of characters written.
 *  Failure: -1.
 */
Int FormatStringvnA(PChar pszBuffer, UInt cchLimitIn, PCChar pszFormat, va_list args)
{
    FMTSTR_FORMAT format;
    PChar p = pszBuffer;
    UInt i, len, sign;
    Char number[65];
    FMTSTR_DATA argData;

    if (pszBuffer == NULL)
        cchLimitIn = (UInt)-1;

    while (*pszFormat && (cchLimitIn > 1))
    {
        if (*pszFormat != '%')
        {
            if (pszBuffer != NULL)
                *p = *pszFormat;

            ++p;
            ++pszFormat;
            cchLimitIn--;
            continue;
        }

        pszFormat++;
        if (*pszFormat == '%')
        {
            if (pszBuffer != NULL)
                *p = *pszFormat;

            ++p;
            ++pszFormat;
            cchLimitIn--;
            continue;
        }

        pszFormat += FormatString_ParseFormatA( pszFormat, &format );

        switch(format.type)
        {
            case FMTSTR_FORMAT_WCHAR:
                argData.wchar_view = (WChar)va_arg( args, int );
                break;

            case FMTSTR_FORMAT_CHAR:
                argData.char_view = (Char)va_arg( args, int );
                break;

            case FMTSTR_FORMAT_STRING:
                argData.lpcstr_view = va_arg( args, PCChar );
                break;

            case FMTSTR_FORMAT_WSTRING:
                argData.lpcwstr_view = va_arg( args, PCWChar );
                break;

#if MY_OS_WIN32
            case FMTSTR_FORMAT_BINARY:
#endif
            case FMTSTR_FORMAT_HEXA:
            case FMTSTR_FORMAT_SIGNED:
            case FMTSTR_FORMAT_UNSIGNED:
                if (format.flags & FMTSTR_FLAG_I64)
                    argData.int64_view = va_arg( args, Int64 );
                else
                    argData.int_view = va_arg( args, Int );
                break;

            default:
                argData.wchar_view = 0;
                break;
        }

        len = FormatString_GetLen( &format, &argData, number, cchLimitIn - 1 );
        sign = 0;
        if (!(format.flags & FMTSTR_FLAG_LEFTALIGN) && format.precision < format.width)
        {
            i = format.width - format.precision;
            if (i)
            {
                if (pszBuffer != NULL)
                    memset(p, ' ', i);
                p += i;
                cchLimitIn -= i;
            }
        }
/*
        if (!(format.flags & FMTSTR_FLAG_LEFTALIGN))
            for (i = format.precision; i < format.width; i++, cchLimitIn--)
                *p++ = ' ';
*/
        switch(format.type)
        {
            case FMTSTR_FORMAT_WCHAR:
                if (pszBuffer != NULL)
                    *p = (Char)argData.wchar_view;
                ++p;
                break;

            case FMTSTR_FORMAT_CHAR:
                if (pszBuffer != NULL)
                    *p = argData.char_view;
                ++p;
                break;

            case FMTSTR_FORMAT_STRING:
            case FMTSTR_FORMAT_WSTRING:
                if (len == 0)
                    break;

                if (format.type == FMTSTR_FORMAT_STRING)
                {
                    if (argData.lpcstr_view)
                    {
                        if (pszBuffer != NULL)
                            CopyMemory(p, argData.lpcstr_view, len);
                        p += len;
                        break;
                    }
                }
                else if (argData.lpcwstr_view)
                {
                    Int MaxLength;
                    PCWChar ptr = argData.lpcwstr_view;

                    MaxLength = pszBuffer == NULL ? 0 : cchLimitIn - (p - pszBuffer);
#if MY_NT_DDK
                    UNICODE_STRING  Unicode;
                    ANSI_STRING     Ansi;

                    Unicode.Buffer = (PWChar)ptr;
                    Unicode.Length = (UShort)(len * sizeof(*ptr));
                    Unicode.MaximumLength = Unicode.Length;

                    Ansi.Buffer = p;
                    Ansi.MaximumLength = Ansi.Length;

                    RtlUnicodeStringToAnsiString(&Ansi, &Unicode, FALSE);
                    len = Ansi.Length;
#else
                    len = WideCharToMultiByte(CP_ACP, 0, ptr, len, p, MaxLength, NULL, NULL);
#endif
                    p += len;
                    break;
                }

                switch (len)
                {
                    default:
                        p[5] = ')';
                    case 5:
                        p[4] = 'l';
                    case 4:
                        p[3] = 'l';
                    case 3:
                        p[2] = 'u';
                    case 2:
                        p[1] = 'n';
                    case 1:
                        p[0] = '(';
                        break;
                }
                p += ML_MIN(6, len);
                break;

            case FMTSTR_FORMAT_HEXA:
                if ((format.flags & FMTSTR_FLAG_PREFIX_HEX) && (cchLimitIn > 3))
                {
                    *p++ = '0';
                    *p++ = (format.flags & FMTSTR_FLAG_UPPER_HEX) ? 'X' : 'x';
                    cchLimitIn -= 2;
                    len -= 2;
                }
                /* fall through */
            case FMTSTR_FORMAT_SIGNED:
                /* Transfer the sign now, just in case it will be zero-padded*/
                if (number[0] == '-')
                {
                    *p++ = '-';
                    sign = 1;
                }
                /* fall through */
            case FMTSTR_FORMAT_UNSIGNED:
//                for (i = len; i < format.precision; i++, cchLimitIn--) *p++ = '0';
                if (len < format.precision)
                {
                    i = format.precision - len;
                    if (i)
                    {
                        if (pszBuffer != NULL)
                            memset(p, '0', i);
                        p += i;
                        cchLimitIn -= i;
                    }
                }
                /* fall through */

#if MY_OS_WIN32

            case FMTSTR_FORMAT_BINARY:
                i = len - sign;
                if (i)
                {
                    if (pszBuffer != NULL)
                        CopyMemory(p, number + sign, i);
                    p += i;
                }
                break;
#endif

            case FMTSTR_FORMAT_UNKNOWN:
                continue;
        }

        if ((format.flags & FMTSTR_FLAG_LEFTALIGN) && format.precision < format.width)
        {
            i = format.width - format.precision;
            if (i)
            {
                if (pszBuffer != NULL)
                    memset(p, ' ', i);
                p += i;
                cchLimitIn -= i;
            }
        }
/*
        if (format.flags & FMTSTR_FLAG_LEFTALIGN)
            for (i = format.precision; i < format.width; i++, cchLimitIn--)
                *p++ = ' ';
*/
        cchLimitIn -= len;
    }

    if (pszBuffer != NULL)
        *p = 0;

    return (cchLimitIn >= 1) ? (Int)(p - pszBuffer) : -1;
}

/***********************************************************************
 *           wvnsprintfW   (SHLWAPI.@)
 *
 * See wvnsprintfA.
 */
Int FormatStringvnW(PWChar pszBuffer, UInt cchLimitIn, PCWChar pszFormat, va_list args)
{
    FMTSTR_FORMAT format;
    PWChar p = pszBuffer;
    UInt i, len, sign;
    Char number[65];
    FMTSTR_DATA argData;

    if (pszBuffer == NULL)
        cchLimitIn = (UInt)-1;

    while (*pszFormat && (cchLimitIn > 1))
    {
        if (*pszFormat != '%')
        {
            if (pszBuffer != NULL)
                *p = *pszFormat;

            ++p;
            ++pszFormat;
            cchLimitIn--;
            continue;
        }

        pszFormat++;

        if (*pszFormat == '%')
        {
            if (pszBuffer != NULL)
                *p = *pszFormat;

            ++p;
            ++pszFormat;
            cchLimitIn--;
            continue;
        }

        pszFormat += FormatString_ParseFormatW( pszFormat, &format );

        switch(format.type)
        {
            case FMTSTR_FORMAT_WCHAR:
                argData.wchar_view = (WChar)va_arg( args, int );
                break;

            case FMTSTR_FORMAT_CHAR:
                argData.char_view = (Char)va_arg( args, int );
                break;

            case FMTSTR_FORMAT_STRING:
                argData.lpcstr_view = va_arg( args, PCChar );
                break;

            case FMTSTR_FORMAT_WSTRING:
                argData.lpcwstr_view = va_arg( args, PCWChar );
                break;

#if MY_OS_WIN32
            case FMTSTR_FORMAT_BINARY:
#endif
            case FMTSTR_FORMAT_HEXA:
            case FMTSTR_FORMAT_SIGNED:
            case FMTSTR_FORMAT_UNSIGNED:
                if (format.flags & FMTSTR_FLAG_I64)
                    argData.int64_view = va_arg( args, Int64 );
                else
                    argData.int_view = va_arg( args, Int );
                break;

            default:
                argData.wchar_view = 0;
                break;
        }

        len = FormatString_GetLen( &format, &argData, number, cchLimitIn - 1 );
        sign = 0;
        if (!(format.flags & FMTSTR_FLAG_LEFTALIGN))
        {
            if (pszBuffer != NULL)
            {
                for (i = format.precision; i < format.width; i++, cchLimitIn--)
                    *p++ = ' ';
            }
            else if (format.precision < format.width)
            {
                UInt l = format.width - format.precision;
                p += l;
                cchLimitIn -= l;
            }
        }

        switch(format.type)
        {
            case FMTSTR_FORMAT_WCHAR:
                if (pszBuffer != NULL)
                    *p = argData.wchar_view;
                ++p;
                break;

            case FMTSTR_FORMAT_CHAR:
                if (pszBuffer != NULL)
                    *p = argData.char_view;
                ++p;
                break;

            case FMTSTR_FORMAT_STRING:
            case FMTSTR_FORMAT_WSTRING:
                if (len == 0)
                    break;

                if (format.type == FMTSTR_FORMAT_STRING)
                {
                    if (argData.lpcstr_view)
                    {
                        PCChar ptr = argData.lpcstr_view;
                        UInt MaxLength = cchLimitIn - (p - pszBuffer);

                        MaxLength = (cchLimitIn - (p - pszBuffer));
                        if (MaxLength > len * 2)
                            MaxLength = len * 2;

#if MY_NT_DDK

                        UNICODE_STRING  Unicode;
                        ANSI_STRING     Ansi;

                        Unicode.Buffer = (PWChar)ptr;
                        Unicode.Length = (UShort)(len * sizeof(*ptr));
                        Unicode.MaximumLength = Unicode.Length;

                        Ansi.Buffer = (PChar)p;
                        Ansi.MaximumLength = Ansi.Length;

                        RtlUnicodeStringToAnsiString(&Ansi, &Unicode, FALSE);
                        len = Ansi.Length;
#else
                        len = MultiByteToWideChar(CP_ACP, 0, ptr, len, p, pszBuffer == NULL ? 0 : MaxLength);
#endif // MY_NT_DDK
                        p += len;
                        break;
                    }
                }
                else if (argData.lpcwstr_view != NULL)
                {
                    if (pszBuffer != NULL)
                        CopyMemory(p, argData.lpcwstr_view, len * sizeof(WChar));
                    p += len;
                    cchLimitIn -= len;
                    break;
                }

                if (pszBuffer != NULL)
                    switch (len)
                    {
                        default:
                            p[5] = ')';
                        case 5:
                            p[4] = 'l';
                        case 4:
                            p[3] = 'l';
                        case 3:
                            p[2] = 'u';
                        case 2:
                            p[1] = 'n';
                        case 1:
                            p[0] = '(';
                            break;
                    }

                len = ML_MIN(6, len);
                p += len;
                break;

            case FMTSTR_FORMAT_HEXA:
                if ((format.flags & FMTSTR_FLAG_PREFIX_HEX) && (cchLimitIn > 3))
                {
                    if (pszBuffer != NULL)
                    {
                        *p++ = '0';
                        *p++ = (format.flags & FMTSTR_FLAG_UPPER_HEX) ? 'X' : 'x';
                    }
                    else
                    {
                        p += 2;
                    }
                    cchLimitIn -= 2;
                    len -= 2;
                }
                /* fall through */
            case FMTSTR_FORMAT_SIGNED:
                /* Transfer the sign now, just in case it will be zero-padded*/
                if (number[0] == '-')
                {
                    if (pszBuffer != NULL)
                        *p = '-';
                    ++p;
                    sign = 1;
                }
                /* fall through */
            case FMTSTR_FORMAT_UNSIGNED:
                if (pszBuffer != NULL)
                {
                    for (i = len; i < format.precision; i++, cchLimitIn--)
                        *p++ = '0';
                }
                else if (len < format.precision)
                {
                    UInt l = format.precision - len;
                    p += l;
                    cchLimitIn -= l;
                }
                /* fall through */
#if MY_OS_WIN32
            case FMTSTR_FORMAT_BINARY:
#endif
                if (pszBuffer != NULL)
                {
                    for (i = sign; i < len; i++)
                        *p++ = (WChar)number[i];
                }
                else if (sign < len)
                {
                    UInt l = len - sign;
                    p += l;
                }

                break;

            case FMTSTR_FORMAT_UNKNOWN:
                continue;
        }
        if (format.flags & FMTSTR_FLAG_LEFTALIGN)
        {
            if (pszBuffer != NULL)
            {
                for (i = format.precision; i < format.width; i++, cchLimitIn--)
                    *p++ = ' ';
            }
            else if (format.precision < format.width)
            {
                UInt l = format.width - format.precision;
                p += l;
                cchLimitIn -= l;
            }
        }
        cchLimitIn -= len;
    }

    if (pszBuffer != NULL)
        *p = 0;

    return (cchLimitIn >= 1) ? (Int)(p - pszBuffer) : -1;
}

Int FormatStringvA(PChar pszBuffer, PCChar pszFormat, va_list args)
{
    return FormatStringvnA(pszBuffer, MAX_INT, pszFormat, args);
}

Int FormatStringvW(PWChar pszBuffer, PCWChar pszFormat, va_list args)
{
    return FormatStringvnW(pszBuffer, MAX_INT / 2, pszFormat, args);
}

Int FormatStringA(PChar pszBuffer, PCChar pszFormat, ...)
{
    va_list valist;

    va_start(valist, pszFormat);
    return FormatStringvA(pszBuffer, pszFormat, valist);
}

Int FormatStringW(PWChar pszBuffer, PCWChar pszFormat, ...)
{
    va_list valist;

    va_start(valist, pszFormat);
    return FormatStringvW(pszBuffer, pszFormat, valist);
}

_ML_C_TAIL_
