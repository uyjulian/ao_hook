#ifndef _APIHELPER_H_ff88b0e9_19eb_4ea3_8c2c_99541c492e06
#define _APIHELPER_H_ff88b0e9_19eb_4ea3_8c2c_99541c492e06

#include "ml/nativeapihelper.h"
#include "ml/memoryallocator.h"

_ML_C_HEAD_

/************************************************************************
  misc
************************************************************************/

ForceInline ULONG HashAPI(PCChar pszName)
{
    ULONG Hash = 0;

    while (*(PByte)pszName)
    {
        Hash = _rotl(Hash, 0x0D) ^ *(PByte)pszName++;
    }

    return Hash;
}

_ML_C_TAIL_

#endif // _APIHELPER_H_ff88b0e9_19eb_4ea3_8c2c_99541c492e06