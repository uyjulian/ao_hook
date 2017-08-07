#ifndef _KMP_H_56576ff0_9962_4584_aa21_3e68ef01d6ff_
#define _KMP_H_56576ff0_9962_4584_aa21_3e68ef01d6ff_


_ML_C_HEAD_

/************************************************************************/
/* KMP                                                                  */
/************************************************************************/
inline Long_Ptr InitNext(const Void *lpKeyWord, Long_Ptr KeyWordLen, PLong_Ptr piNext)
{
    Long_Ptr i, j;
    const Char *pDest;

    pDest = (const Char *)lpKeyWord;

    piNext[0] = -1;
    i = 0;
    j = (Long_Ptr)-1;

    while (i < KeyWordLen)
    {
        if (j == -1 || pDest[i] == pDest[j])
        {
            if (pDest[++i] != pDest[++j])
            {
                piNext[i] = j;
            }
            else
            {
                piNext[i] = piNext[j];
            }
        }
        else
        {
            j = piNext[j];
        }
    }

    return KeyWordLen;
}

inline PVoid KMP(PCVoid _Src, Long_Ptr SrcLength, PCVoid _KeyWord, Long_Ptr KeyWordLen)
{
    Long_Ptr i, j;
    PLong_Ptr  Next;//[100];
    PLong_Ptr pNext;
    const Char *pSrc, *pKeyWord;

    if (SrcLength == 0)
        return nullptr;

    Next = (PLong_Ptr)AllocStack((KeyWordLen + 1) * sizeof(*pNext));
    pNext  = Next;
    pSrc  = (const Char *)_Src;
    pKeyWord = (const Char *)_KeyWord;
    InitNext(_KeyWord, KeyWordLen, pNext);
    i = 0;
    j = 0;
    while (i != SrcLength && j != KeyWordLen)
    {
        if (j == -1 || pSrc[i] == pKeyWord[j])
        {
            ++i;
            ++j;
        }
        else
        {
            j = pNext[j];
        }
    }

    if (i == SrcLength && j != KeyWordLen)
        return nullptr;

    return (Void *)(pSrc + i - KeyWordLen);
}

_ML_C_TAIL_

#endif // _KMP_H_56576ff0_9962_4584_aa21_3e68ef01d6ff_