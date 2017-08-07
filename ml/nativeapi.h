#ifndef _NATIVEAPI_H_536f1070_45de_44d7_9e53_6c5db2ff00d3
#define _NATIVEAPI_H_536f1070_45de_44d7_9e53_6c5db2ff00d3

#pragma warning(disable:4103)

#if ML_AMD64
    #pragma pack(push, 8)
#else
    #pragma pack(push, 4)
#endif

#include "ml/ntbase.h"

_ML_C_HEAD_

#include "ml/ntdebug.h"
#include "ml/ntexception.h"
#include "ml/ntfileio.h"
#include "ml/ntlpc.h"
#include "ml/ntmemory.h"
#include "ml/ntmisc.h"
#include "ml/ntmodule.h"
#include "ml/ntregistry.h"
#include "ml/ntshimengine.h"
#include "ml/ntstring.h"
#include "ml/ntsync.h"
#include "ml/nttimer.h"
#include "ml/nttoken.h"
#include "ml/ntuser.h"
#include "ml/ntrtl.h"
#include "ml/ntnls.h"
#include "ml/ntgdi.h"
#include "ml/ntpo.h"
// Font Management Services

#include "ml/fms.h"

_ML_C_TAIL_


#endif // _NATIVEAPI_H_536f1070_45de_44d7_9e53_6c5db2ff00d3