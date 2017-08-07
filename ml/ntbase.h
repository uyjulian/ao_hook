#ifndef _NTBASE_H_bc4f0ce4_29e3_43c6_a3a3_98415b2adbab_
#define _NTBASE_H_bc4f0ce4_29e3_43c6_a3a3_98415b2adbab_

#include "ml/nativetypes.h"

#define NATIVE_API  EXTC_IMPORT
#define NTKRNLAPI   EXTC_IMPORT

ForceInline NTSTATUS ML_NTSTATUS_FROM_WIN32(long x) { return x <= 0 ? (NTSTATUS)x : (NTSTATUS) (((x) & 0x0000FFFF) | (FACILITY_NTWIN32 << 16) | ERROR_SEVERITY_ERROR);}

#endif // _NTBASE_H_bc4f0ce4_29e3_43c6_a3a3_98415b2adbab_