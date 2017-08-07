#ifndef _COMPRESSION_H_65414ec9_55d6_4212_9ce3_9b53394b394a
#define _COMPRESSION_H_65414ec9_55d6_4212_9ce3_9b53394b394a


_ML_C_HEAD_

Bool
FASTCALL
UCL_NRV2E_Compress(
    PVoid   Input,
    ULong   InputSize,
    PVoid   Output,
    PULong  OutputSize,
    Long    DEF_VAL(Level, 10)
);

Bool
FASTCALL
UCL_NRV2E_Decompress(
    PVoid   Input,
    ULong   InputSize,
    PVoid   Output,
    PULong  OutputSize
);

ULong_Ptr
FASTCALL
UCL_NRV2E_DecompressASMFast32(
    PVoid Input,
    PVoid Output
);

_ML_C_TAIL_

#endif // _COMPRESSION_H_65414ec9_55d6_4212_9ce3_9b53394b394a