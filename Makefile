
export WINEDEBUG=-all

FILES := Battle.o edao.o Global.o Sound.o SoundArc.o  ml/fileio.o ml/ldasm.o ml/mainlib.o ml/memoryallocator.o ml/mlntfileio.o ml/mlntldr.o ml/mlntmemory.o ml/mlntnls.o ml/mlntprocess.o ml/mlntregistry.o ml/mlntrtl.o ml/nativeapihelper.o ml/ntmemory.o ml/patchmemory.o ml/peimage.o ml/stringhelper.o

.PHONY: clean

EDAO.dll: $(FILES)
	link /LIBPATH:c:\lib delayimp.lib ntdll.lib kernel32.lib user32.lib gdiplus.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /OPT:REF /delayload:gdiplus.dll /delayload:USER32.dll /nologo /dll /machine:I386 /ltcg /out:$@ $^

%.o: %.cpp
	cl /nologo /Gr /MD /W4 /GR- /O2 /Ob1 /D "WIN32" /D "NDEBUG" /D USE_NT_VER=1 /GS- /MP /arch:SSE /GL /c /Fo$@ /Tp$< /I. /utf-8

clean:
	rm -f $(FILES) EDAO.dll EDAO.lib EDAO.exp
