#ifndef FT_EXPORT

#ifdef __cplusplus
#define FT_EXPORT( x )  extern "C"  x CDECL
#else
#define FT_EXPORT( x )  extern  x CDECL
#endif

#endif /* !FT_EXPORT */


#include "ft2build.h"
#include FT_FREETYPE_H

namespace NFreeType
{
    FT_Error CDECL FT_New_Face_Ex( FT_Library   library,
        const char*  filepathname,
        FT_Long      face_index,
        FT_Face     *aface )
    {
        TYPE_OF(&FT_Open_Face) Stub_FT_Open_face = (TYPE_OF(&FT_Open_Face))0x067577D;
        FT_Open_Args  args;
        FT_Error Result;

        if (!filepathname)
            return FT_Err_Invalid_Argument;

        WCHAR	szwPath[MAX_PATH];
		Nt_AnsiToUnicode(szwPath, countof(szwPath), (PCHAR)filepathname, -1);
        NtFileDisk file;
        NTSTATUS Status = file.Open(szwPath);
        if (!NT_SUCCESS(Status))
        {
            return FT_Err_Cannot_Open_Resource;
		}

#if CONSOLE_DEBUG
        PrintConsoleA("load font once:%s\n", filepathname);
#endif
        ULONG fileSize = file.GetSize32();
        if (fileSize > (64 << 20))
        {
            args.flags    = FT_OPEN_PATHNAME;
            args.pathname = (char*)filepathname;
            args.stream   = NULL;

            return Stub_FT_Open_face( library, &args, face_index, aface );
        }

        PBYTE p = new BYTE[fileSize];
        file.Read(p, fileSize);
		file.Close();

        args.flags       = FT_OPEN_MEMORY;
        args.memory_base = p;
        args.memory_size = fileSize;
        args.stream      = NULL;

        Result = Stub_FT_Open_face( library, &args, face_index, aface );
        if (Result)
        {
            delete p;
        }
        return Result;
    }
}