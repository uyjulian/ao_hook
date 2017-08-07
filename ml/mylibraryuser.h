#ifndef _MYLIBRARYUSER_H_e5b0d13b_823a_4f24_88ec_6515f58a7140_
#define _MYLIBRARYUSER_H_e5b0d13b_823a_4f24_88ec_6515f58a7140_

#define ML_USER_MODE  1

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_NONSTDC_NO_WARNINGS

#ifndef _WINSOCKAPI_
    #include <ws2spi.h>
#endif

#include <intsafe.h>

#include "ml/macros.h"
#include "ml/macrosplus.h"

#if ML_KERNEL_MODE

//    #include <ntddk.h>

#else

//    #include <winsock2.h>
    #include <Windows.h>
    #include <Wininet.h>

    #pragma warning(disable:4005)

    #include <ntstatus.h>

    #pragma warning(default:4005)

#endif // MY_NT_DDK

typedef void * POINTER_64 HANDLE64;
typedef HANDLE64 *PHANDLE64;

#include "ml/dllfunctionnamehash.h"
//#pragma comment(lib, "Shlwapi.lib")

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <direct.h>

#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <signal.h>
#include <locale.h>
#include <process.h>

#include <tchar.h>
#include <string.h>

#include <math.h>
#include <float.h>

#include <tchar.h>

#include <Shlwapi.h>

#include "ml/memory.h"

#include "ml/misc.h"

#include "ml/apihelper.h"
#include "ml/image.h"

#include "ml/peimage.h"
#include "ml/fileio.h"
#include "ml/ldasm.h"

// class

#if CPP_DEFINED


ML_NAMESPACE

#include "ml/traits.h"
#include "ml/pointer.h"

#include "ml/vector.hpp"
#include "ml/mlstring.h"
#include "ml/mlnativeapi.h"

ML_NAMESPACE_END;

#include "ml/patchmemory.h"
#include "ml/sunday.h"
#include "ml/acautomation.h"

#endif // CPP_DEFINED

#endif // _MYLIBRARYUSER_H_e5b0d13b_823a_4f24_88ec_6515f58a7140_