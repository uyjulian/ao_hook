#ifndef _SUNDAY_H_067e925d_ef1c_4241_b685_578bcbcf26d4_
#define _SUNDAY_H_067e925d_ef1c_4241_b685_578bcbcf26d4_


inline PVoid Sunday(PCVoid Buffer, LongPtr BufferSize, PCVoid Pattern, LongPtr PatternSize)
{
    LongPtr occ[256];
    LongPtr Index, SearchLength;

    if (PatternSize > BufferSize)
        return nullptr;

    FillMemory(occ, sizeof(occ), -1);
    for (Index = 0; Index != PatternSize; ++Index)
    {
        occ[((PByte)Pattern)[Index]] = Index;
    }

    for (LongPtr Begin = 0; Begin != BufferSize - PatternSize;)
    {
        LongPtr i = Begin, j = 0;

        for (; j < PatternSize && i < BufferSize && ((PByte)Buffer)[i] == ((PByte)Pattern)[j]; ++i, ++j);

        if (j == PatternSize)
            return &((PByte)Buffer)[Begin];

        if (Begin + PatternSize >= BufferSize)
            return nullptr;

        Begin += PatternSize - occ[((PByte)Buffer)[Begin + PatternSize]];
    }

    return nullptr;
}

#endif // _SUNDAY_H_067e925d_ef1c_4241_b685_578bcbcf26d4_