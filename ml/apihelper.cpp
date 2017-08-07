#include "mylibrary.h"


#pragma warning(default:4245)
#pragma warning(default:4750)

_ML_C_HEAD_

#if ML_KERNEL_MODE

NTSTATUS
ProbeForReadSafe(
    PVOID   Address,
    SIZE_T  Length,
    ULONG   Alignment
)
{
    NTSTATUS Status = STATUS_SUCCESS;

    SEH_TRY
    {
        ProbeForRead(Address, Length, Alignment);
    }
    SEH_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        Status = GetExceptionCode();
    }

    return Status;
}

NTSTATUS
ProbeForWriteSafe(
    PVOID   Address,
    SIZE_T  Length,
    ULONG   Alignment
)
{
    NTSTATUS Status = STATUS_SUCCESS;

    SEH_TRY
    {
        ProbeForWrite(Address, Length, Alignment);
    }
    SEH_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        Status = GetExceptionCode();
    }

    return Status;
}

NTSTATUS
MmProbeAndLockPagesSafe(
    PMDL            MemoryDescriptorList,
    KPROCESSOR_MODE AccessMode,
    LOCK_OPERATION  Operation
)
{
    NTSTATUS Status = STATUS_SUCCESS;

    SEH_TRY
    {
        MmProbeAndLockPages(MemoryDescriptorList, AccessMode, Operation);
    }
    SEH_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        Status = GetExceptionCode();
    }

    return Status;
}

#else // r3

#pragma comment(lib, "dbghelp.lib")
#include <DbgHelp.h>

NTSTATUS CreateMiniDump(PEXCEPTION_POINTERS ExceptionPointers)
{
    WCHAR           MiniDumpFile[MAX_NTPATH];
    NtFileDisk      File;
    BOOL            Success;
    NTSTATUS        Status;
    PROCESS_TIMES   Times;

    MINIDUMP_EXCEPTION_INFORMATION ExceptionInformation;

    NtQueryInformationProcess(NtCurrentProcess(), ProcessTimes, &Times, sizeof(Times), NULL);

    Nt_GetModuleFileName(NULL, MiniDumpFile, countof(MiniDumpFile));
    swprintf(findextw(MiniDumpFile), L".%I64X.crash.dmp", Times.CreationTime.QuadPart);

    Status = File.Create(MiniDumpFile);
    FAIL_RETURN(Status);

    ExceptionInformation.ClientPointers     = FALSE;
    ExceptionInformation.ExceptionPointers  = ExceptionPointers;
    ExceptionInformation.ThreadId           = Ps::CurrentTid();

    Success = MiniDumpWriteDump(
                NtCurrentProcess(),
                (ULONG_PTR)Ps::CurrentTeb()->ClientId.UniqueProcess,
                File,
                MiniDumpNormal,
                &ExceptionInformation,
                NULL,
                NULL
              );

    return Success ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}

/************************************************************************
  directory
************************************************************************/
BOOL IsPathExistsW(LPCWSTR pszPath)
{
    if (GetFileAttributesW(pszPath) != -1)
        return TRUE;

    switch (GetLastError())
    {
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
        case ERROR_INVALID_NAME:
            return FALSE;
    }

    return TRUE;
}

BOOL IsPathExistsA(LPCSTR pszPath)
{
    WCHAR szPath[MAX_PATH];

    MultiByteToWideChar(CP_ACP, 0, pszPath, -1, szPath, countof(szPath));

    return IsPathExistsW(szPath);
}

BOOL CreateDirectoryWorker(LPCWSTR PathName)
{
#if USE_NT_VER
    NTSTATUS    Status;
    HANDLE      DirectoryHandle;

    Status = NtFileDisk::CreateDirectory(&DirectoryHandle, PathName);
    if (!NT_SUCCESS(Status))
        return FALSE;

    NtClose(DirectoryHandle);
    return TRUE;

#else
    return CreateDirectoryW(PathName, NULL);
#endif // USE_NT_VER
}

BOOL CreateDirectoryRecursiveW(LPCWSTR lpPathName)
{
    WCHAR  ch;
    LPWSTR lpPath = (LPWSTR)lpPathName;

    if (lpPath == NULL || *lpPath == 0)
        return FALSE;

#if USE_NT_VER
    if (Nt_IsPathExists(lpPathName))
        return TRUE;
#else
    if (IsPathExistsW(lpPathName))
        return TRUE;
#endif // USE_NT_VER

    do
    {
        if (*lpPath == '\\' || *lpPath == '/')
        {
            if (*(lpPath + 1) == 0)
            {
                break;
            }
            ch = *lpPath;
            *lpPath = 0;
            CreateDirectoryWorker(lpPathName);
            *lpPath = ch;
        }
        ++lpPath;
    } while (*lpPath);

    return CreateDirectoryWorker(lpPathName);
}

BOOL FASTCALL MyCreateDirectoryA(LPCSTR lpPathName)
{
    WCHAR szPath[MAX_PATH * 2];

    MultiByteToWideChar(CP_ACP, 0, lpPathName, -1, szPath, countof(szPath));
    return CreateDirectoryRecursiveW(szPath);
}

ULONG GetExeDirectoryW(LPWSTR pszPath, ULONG BufferCount)
{
    WCHAR szPath[MAX_PATH];
    ULONG i;

    if (pszPath == NULL)
    {
        pszPath = szPath;
        BufferCount = countof(szPath);
    }

    i = GetModuleFileNameW(NULL, pszPath, BufferCount);
    if (i)
    {
        while (pszPath[--i] != '\\');
        pszPath[++i] = 0;
    }

    return i;
}

ULONG GetExeDirectoryA(LPSTR pszPath, ULONG BufferCount)
{
    WCHAR  szPath[MAX_PATH];
    ULONG i;

    i = GetExeDirectoryW(szPath, countof(szPath));
    if (i)
    {
        i = WideCharToMultiByte(CP_ACP, 0, szPath, i + 1, pszPath, BufferCount, NULL, NULL) - 1;
    }

    return i;
}


inline BOOL __IsParentDirectory(LPWSTR pszPath)
{
    if (*(PULONG)pszPath == '.')
        return TRUE;
    else if ((*(PULONG64)pszPath & 0x0000FFFFFFFFFFFF) == TAG2W('..'))
        return TRUE;

    return FALSE;
}

LONG ProcessFindResult(ENUM_DIRECTORY_INFO *pFindInfo, LPWSTR lpPath, LPWSTR lpFileName)
{
    LONG  Result;
    PVOID lpBuffer;

    StrCopyW(lpFileName, pFindInfo->wfd.cFileName);
    StrCopyW(pFindInfo->wfd.cFileName, lpPath);

    lpBuffer = pFindInfo->lpOutBuffer;
    if (lpBuffer != NULL)
    {
        if (pFindInfo->ElemCount.QuadPart >= pFindInfo->MaxCount.QuadPart)
        {
            pFindInfo->MaxCount.QuadPart *= 2;
#if USE_NT_VER
            lpBuffer = RtlReAllocateHeap(
                            pFindInfo->hHeap,
                            HEAP_ZERO_MEMORY,
                            pFindInfo->lpBuffer,
                            (ULONG_PTR)pFindInfo->MaxCount.QuadPart * pFindInfo->ElemSize
                       );
#else
            lpBuffer = HeapReAlloc(
                            pFindInfo->hHeap,
                            HEAP_ZERO_MEMORY,
                            pFindInfo->lpBuffer,
                            (ULONG_PTR)pFindInfo->MaxCount.QuadPart * pFindInfo->ElemSize
                       );
#endif // USE_NT_VER

            if (lpBuffer == NULL)
                return -1;

            pFindInfo->lpBuffer = lpBuffer;
            lpBuffer = (PBYTE)lpBuffer + pFindInfo->ElemCount.QuadPart * pFindInfo->ElemSize;
            pFindInfo->lpOutBuffer = lpBuffer;
        }

        lpBuffer = pFindInfo->lpOutBuffer;
    }

    Result = pFindInfo->CallBack(lpBuffer, &pFindInfo->wfd, pFindInfo->Context);
    if (Result != 0)
        pFindInfo->ElemCount.QuadPart += Result;

    if (lpBuffer != NULL)
        pFindInfo->lpOutBuffer = (PBYTE)lpBuffer + pFindInfo->ElemSize;

    return Result;
}

IF_NOT_EXIST(_wfinddata64_t)
{
    typedef struct
    {
        unsigned    attrib;
        __int64  time_create;    /* -1 for FAT file systems */
        __int64  time_access;    /* -1 for FAT file systems */
        __int64  time_write;
        __int64     size;
        wchar_t     name[260];
    } _wfinddata64_t;
}

VOID CopyCrtFindDataToWin32FindData(PWIN32_FIND_DATAW FindFileData, _wfinddata64_t *finddata)
{
    LARGE_INTEGER  LargeInt;
    ULARGE_INTEGER LargeUInt;

    FindFileData->dwFileAttributes                  = finddata->attrib;

    LargeInt.QuadPart                               = finddata->size;
    FindFileData->nFileSizeLow                      = LargeInt.LowPart;
    FindFileData->nFileSizeHigh                     = LargeInt.HighPart;

    LargeUInt.QuadPart                              = finddata->time_create;
    FindFileData->ftCreationTime.dwLowDateTime      = LargeUInt.LowPart;
    FindFileData->ftCreationTime.dwHighDateTime     = LargeUInt.HighPart;

    LargeUInt.QuadPart                              = finddata->time_access;
    FindFileData->ftLastAccessTime.dwLowDateTime    = LargeUInt.LowPart;
    FindFileData->ftLastAccessTime.dwHighDateTime   = LargeUInt.HighPart;

    LargeUInt.QuadPart                              = finddata->time_write;
    FindFileData->ftLastWriteTime.dwLowDateTime     = LargeUInt.LowPart;
    FindFileData->ftLastWriteTime.dwHighDateTime    = LargeUInt.HighPart;

    FindFileData->cAlternateFileName[0]             = 0;
    CopyStruct(FindFileData->cFileName, finddata->name, MY_MIN(sizeof(finddata->name), sizeof(FindFileData->cFileName)));
}

HANDLE FindFirstFileWorker(LPCWSTR FileName, PWIN32_FIND_DATAW FindFileData)
{
#if USE_CRT_VER
    intptr_t handle;
    _wfinddata64_t finddata;

    handle = _wfindfirst64(FileName, &finddata);
    if (handle == -1)
        return INVALID_HANDLE_VALUE;

    CopyCrtFindDataToWin32FindData(FindFileData, &finddata);
    return (HANDLE)handle;

#elif USE_NT_VER
    return Nt_FindFirstFile(FileName, FindFileData);
#else
    return FindFirstFileW(FileName, FindFileData);
#endif
}

BOOL FindNextFileWorker(HANDLE FindFileHandle, PWIN32_FIND_DATAW FindFileData)
{
#if USE_CRT_VER
    _wfinddata64_t finddata;
    if (_wfindnext64((intptr_t)FindFileHandle, &finddata) == -1)
        return FALSE;

    CopyCrtFindDataToWin32FindData(FindFileData, &finddata);
    return TRUE;

#elif USE_NT_VER
    return Nt_FindNextFile(FindFileHandle, FindFileData);
#else
    return FindNextFileW(FindFileHandle, FindFileData);
#endif
}

BOOL FindCloseWorker(HANDLE FindFileHandle)
{
#if USE_CRT_VER
    return _findclose((intptr_t)FindFileHandle) == 0;
#elif USE_NT_VER
    return Nt_FindClose(FindFileHandle);
#else
    return FindClose(FindFileHandle);
#endif
}

BOOL IsCircleSymbolicLink(ENUM_DIRECTORY_INFO *pFindInfo, LPWSTR lpPath, LPWSTR lpFileName)
{
    if (!FLAG_ON(pFindInfo->wfd.dwFileAttributes, FILE_ATTRIBUTE_REPARSE_POINT))
        return FALSE;

    ULONG_PTR   Length, CurrentPathLength;
    PWCHAR      ShorterPath, LinkedPath;
    HANDLE      SymbolicLink;
    NTSTATUS    Status;

    StrCopyW(lpFileName, pFindInfo->wfd.cFileName);
    if (!NT_SUCCESS(NtFileDisk::OpenSymbolicLink(&SymbolicLink, lpPath)))
        return FALSE;

    LinkedPath  = pFindInfo->SymbolicLinkPath;
    Length      = countof(pFindInfo->SymbolicLinkPath);
    Status      = NtFileDisk::QuerySymbolicTarget(SymbolicLink, LinkedPath, &Length);

    NtFileDisk::Close(SymbolicLink);
    if (!NT_SUCCESS(Status))
        return FALSE;

    if (LinkedPath[Length - 1] != '\\')
        LinkedPath[Length++] = '\\';

    CurrentPathLength = StrLengthW(lpPath);
    if (CurrentPathLength < Length)
    {
        ShorterPath = lpPath;
        Length = CurrentPathLength;
    }
    else
    {
        ShorterPath = LinkedPath;
    }

    if (lpPath[Length - 1] != '\\' || LinkedPath[Length - 1] != '\\')
        return FALSE;

    return !StrNICompareW(lpPath, LinkedPath, Length);
}

BOOL EnumDirectoryFilesWorker(LPWSTR lpPath, ENUM_DIRECTORY_INFO *pFindInfo)
{
    BOOL    bIsDirectory;
    LPWSTR  lpEnd, lpFileName;
    HANDLE  hFind;
    LONG    Result;

    lpEnd = lpPath + StrLengthW(lpPath);
    if (pFindInfo->wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        if (*(lpEnd - 1) != '\\')
            *lpEnd++ = '\\';

        lpFileName = lpEnd;
        StrCopyW(lpEnd, pFindInfo->pszFilter);
        bIsDirectory = TRUE;
    }
    else
    {
        lpFileName   = findnamew(lpPath);
        bIsDirectory = FALSE;
    }

    hFind = FindFirstFileWorker(lpPath, &pFindInfo->wfd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (FLAG_ON(pFindInfo->wfd.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
            {
                if (__IsParentDirectory(pFindInfo->wfd.cFileName))
                    continue;

                if (IsCircleSymbolicLink(pFindInfo, lpPath, lpFileName))
                    continue;

                if (FLAG_ON(pFindInfo->Flags, EDF_BEFORE))
                {
                    Result = ProcessFindResult(pFindInfo, lpPath, lpFileName);
                    if (Result == -1)
                    {
                        FindCloseWorker(hFind);
                        return FALSE;
                    }
                }

                if (!FLAG_ON(pFindInfo->Flags, EDF_SUBDIR))
                    continue;

                if (!FLAG_ON(pFindInfo->Flags, EDF_BEFORE))
                    StrCopyW(lpFileName, pFindInfo->wfd.cFileName);

                if (!EnumDirectoryFilesWorker(lpPath, pFindInfo))
                    break;
            }
            else
            {
                Result = ProcessFindResult(pFindInfo, lpPath, lpFileName);
                if (Result == -1)
                {
                    FindCloseWorker(hFind);
                    return FALSE;
                }
            }
        } while (FindNextFileWorker(hFind, &pFindInfo->wfd));

        FindCloseWorker(hFind);
    }

    if (bIsDirectory && FLAG_ON(pFindInfo->Flags, EDF_AFTER))
    {
        pFindInfo->wfd.cFileName[0] = 0;
        SET_FLAG(pFindInfo->wfd.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
        Result = ProcessFindResult(pFindInfo, lpPath, lpFileName);
        if (Result == -1)
            return FALSE;
    }

    *lpEnd = 0;

    return TRUE;
}

BOOL EnumDirectoryFilesFree(PVOID lpBuffer)
{
    return MemoryAllocator::FreeMemory(lpBuffer, 0);
}

BOOL
EnumDirectoryFiles(
    PVOID                      *lpFilesBuffer,
    LPCWSTR                     pszFilter,
    ULONG                       ElemSize,
    LPCWSTR                     pszPath,
    PLARGE_INTEGER              pElemCount,
    FEnumDirectoryFilesCallBack CallBack,
    ULONG_PTR                   Context,
    ULONG                       Flags
)
{
    WCHAR szPath[MAX_NTPATH];
    ENUM_DIRECTORY_INFO info;
    HANDLE hFind;

    if (CallBack == NULL)
        return FALSE;

    if (pElemCount != NULL)
        pElemCount->QuadPart = 0;

    if (pszPath != NULL)
    {
        hFind = FindFirstFileWorker(pszPath, &info.wfd);
        if (hFind == INVALID_HANDLE_VALUE)
            return FALSE;

        FindCloseWorker(hFind);
    }
    else
    {
        info.wfd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
    }

    info.ElemSize = ElemSize;
    info.MaxCount.QuadPart = 100;
    info.hHeap = MemoryAllocator::GetGlobalHeap();
    if (lpFilesBuffer != NULL)
    {
#if USE_NT_VER
        info.lpBuffer = RtlAllocateHeap(info.hHeap, HEAP_ZERO_MEMORY, (ULONG_PTR)info.MaxCount.QuadPart * info.ElemSize);
#else
        info.lpBuffer = HeapAlloc(info.hHeap, HEAP_ZERO_MEMORY, (ULONG_PTR)info.MaxCount.QuadPart * info.ElemSize);
#endif // USE_NT_VER
        if (info.lpBuffer == NULL)
            return FALSE;
    }
    else
    {
        info.lpBuffer = NULL;
    }

    info.lpOutBuffer = info.lpBuffer;
    info.CallBack = CallBack;
    info.Context = Context;
    info.ElemCount.QuadPart = 0;
    info.pszFilter = pszFilter;
    info.Flags = Flags;

    if (pszPath != NULL)
    {
        StrCopyW(szPath, pszPath);
    }
    else
    {
        *(PULONG)szPath = L'.';
    }

    if (!EnumDirectoryFilesWorker(szPath, &info))
    {
        EnumDirectoryFilesFree(info.lpBuffer);
        return FALSE;
    }

    if (pElemCount != NULL)
        *pElemCount = info.ElemCount;
    if (lpFilesBuffer != NULL)
        *lpFilesBuffer = info.lpBuffer;

    return TRUE;
}

/************************************************************************
  command line
************************************************************************/

LONG_PTR FASTCALL CmdLineToArgvWorkerA(LPSTR pszCmdLine, LPSTR *pArgv, PLONG_PTR pCmdLineLength)
{
    LONG_PTR  argc;
    Char separator, ch;
    LPSTR pCmdLine;

    argc = 0;
    pCmdLine = pszCmdLine;

    LOOP_ALWAYS
    {
        separator = *pszCmdLine++;
        if (separator == 0)
            break;
        if (separator != '"')
            separator = ' ';

        ++argc;
        if (pArgv)
            *pArgv++ = (LPSTR)pszCmdLine - 1 + (separator == '"');

        LOOP_ALWAYS
        {
            ch = *pszCmdLine;
            if (ch == separator)
            {
                if (pArgv)
                    *pszCmdLine = 0;
                ++pszCmdLine;
                break;
            }
            else if (ch == 0)
                break;

            ++pszCmdLine;
        }
/*
        if (pArgv)
            *(pszCmdLine - 1)= 0;
*/
        while (*pszCmdLine == '\t' || *pszCmdLine == ' ')
            ++pszCmdLine;
    }

    if (pCmdLineLength)
        *pCmdLineLength = pszCmdLine - pCmdLine + 0;

    return argc;
}

LPSTR* FASTCALL CmdLineToArgvA(LPSTR pszCmdLine, PLONG_PTR pArgc)
{
    LONG_PTR argc, CmdLineLength;
    LPSTR *argv;

    argc = CmdLineToArgvWorkerA(pszCmdLine, NULL, &CmdLineLength);
    argv = (LPSTR *)AllocateMemory(CmdLineLength * sizeof(*pszCmdLine) + (argc + 1) * 4);
    if (argv == NULL)
        return NULL;

    CopyMemory(&argv[argc + 1], pszCmdLine, CmdLineLength * sizeof(*pszCmdLine));
    argv[argc] = 0;
    pszCmdLine = (LPSTR)&argv[argc + 1];

    argc = CmdLineToArgvWorkerA(pszCmdLine, argv, NULL);
    if (pArgc)
        *pArgc = argc;

    return (LPSTR *)argv;
}

LONG_PTR FASTCALL CmdLineToArgvWorkerW(LPWSTR pszCmdLine, LPWSTR *pArgv, PLONG_PTR pCmdLineLength)
{
    LONG_PTR  argc;
    WCHAR separator, ch;
    LPWSTR pCmdLine;

    argc = 0;
    pCmdLine = pszCmdLine;

    LOOP_ALWAYS
    {
        separator = *pszCmdLine++;
        if (separator == 0)
            break;
        if (separator != '"')
            separator = ' ';

        ++argc;
        if (pArgv)
            *pArgv++ = (LPWSTR)pszCmdLine - 1 + (separator == '"');

        LOOP_ALWAYS
        {
            ch = *pszCmdLine;
            if (ch == separator)
            {
                if (pArgv)
                    *pszCmdLine = 0;
                ++pszCmdLine;
                break;
            }
            else if (ch == 0)
                break;

            ++pszCmdLine;
        }
/*
        if (pArgv && ch != 0)
            *(pszCmdLine - 1)= 0;
*/
        while (*pszCmdLine == '\t' || *pszCmdLine == ' ')
            ++pszCmdLine;
    }

    if (pCmdLineLength)
        *pCmdLineLength = pszCmdLine - pCmdLine + 0;

    return argc;
}

LPWSTR* FASTCALL CmdLineToArgvW(LPWSTR pszCmdLine, PLONG_PTR pArgc)
{
    LONG_PTR argc, CmdLineLength;
    LPWSTR *argv;

    argc = CmdLineToArgvWorkerW(pszCmdLine, NULL, &CmdLineLength);
    argv = (LPWSTR *)AllocateMemory(CmdLineLength * sizeof(*pszCmdLine) + (argc + 1) * 4);
    if (argv == NULL)
        return NULL;

    CopyMemory(&argv[argc + 1], pszCmdLine, CmdLineLength * sizeof(*pszCmdLine));
    argv[argc] = 0;
    pszCmdLine = (LPWSTR)&argv[argc + 1];

    argc = CmdLineToArgvWorkerW(pszCmdLine, argv, NULL);
    if (pArgc)
        *pArgc = argc;

    return (LPWSTR *)argv;
}

#endif  // ML_KERNEL_MODE

_ML_C_TAIL_
