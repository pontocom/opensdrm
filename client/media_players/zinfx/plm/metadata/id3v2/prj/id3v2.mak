# Microsoft Developer Studio Generated NMAKE File, Based on id3v2.dsp
!IF "$(CFG)" == ""
CFG=id3v2 - Win32 Release
!MESSAGE No configuration specified. Defaulting to id3v2 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "id3v2 - Win32 Release" && "$(CFG)" != "id3v2 - Win32 Debug" && "$(CFG)" != "id3v2 - Win32 NASM Release" && "$(CFG)" != "id3v2 - Win32 NASM Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "id3v2.mak" CFG="id3v2 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "id3v2 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "id3v2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "id3v2 - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "id3v2 - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "id3v2 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : ".\id3v2.mdf"

!ELSE 

ALL : "fabaselib - Win32 Release" ".\id3v2.mdf"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\c_wrapper.obj"
	-@erase "$(INTDIR)\field.obj"
	-@erase "$(INTDIR)\field_binary.obj"
	-@erase "$(INTDIR)\field_integer.obj"
	-@erase "$(INTDIR)\field_string_ascii.obj"
	-@erase "$(INTDIR)\field_string_unicode.obj"
	-@erase "$(INTDIR)\frame.obj"
	-@erase "$(INTDIR)\frame_impl.obj"
	-@erase "$(INTDIR)\frame_parse.obj"
	-@erase "$(INTDIR)\frame_render.obj"
	-@erase "$(INTDIR)\header.obj"
	-@erase "$(INTDIR)\header_frame.obj"
	-@erase "$(INTDIR)\header_tag.obj"
	-@erase "$(INTDIR)\helpers.obj"
	-@erase "$(INTDIR)\id3v2.obj"
	-@erase "$(INTDIR)\id3v2.res"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\io_decorators.obj"
	-@erase "$(INTDIR)\io_helpers.obj"
	-@erase "$(INTDIR)\misc_support.obj"
	-@erase "$(INTDIR)\readers.obj"
	-@erase "$(INTDIR)\spec.obj"
	-@erase "$(INTDIR)\tag.obj"
	-@erase "$(INTDIR)\tag_file.obj"
	-@erase "$(INTDIR)\tag_find.obj"
	-@erase "$(INTDIR)\tag_impl.obj"
	-@erase "$(INTDIR)\tag_parse.obj"
	-@erase "$(INTDIR)\tag_parse_lyrics3.obj"
	-@erase "$(INTDIR)\tag_parse_musicmatch.obj"
	-@erase "$(INTDIR)\tag_parse_v1.obj"
	-@erase "$(INTDIR)\tag_render.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\writers.obj"
	-@erase "$(OUTDIR)\id3v2.exp"
	-@erase "$(OUTDIR)\id3v2.lib"
	-@erase ".\id3v2.mdf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\id3v2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\id3v2.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\id3v2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10600000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\id3v2.pdb" /machine:I386 /def:".\id3v2.def" /out:"id3v2.mdf" /implib:"$(OUTDIR)\id3v2.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\id3v2.def"
LINK32_OBJS= \
	"$(INTDIR)\c_wrapper.obj" \
	"$(INTDIR)\field.obj" \
	"$(INTDIR)\field_binary.obj" \
	"$(INTDIR)\field_integer.obj" \
	"$(INTDIR)\field_string_ascii.obj" \
	"$(INTDIR)\field_string_unicode.obj" \
	"$(INTDIR)\frame.obj" \
	"$(INTDIR)\frame_impl.obj" \
	"$(INTDIR)\frame_parse.obj" \
	"$(INTDIR)\frame_render.obj" \
	"$(INTDIR)\header.obj" \
	"$(INTDIR)\header_frame.obj" \
	"$(INTDIR)\header_tag.obj" \
	"$(INTDIR)\helpers.obj" \
	"$(INTDIR)\id3v2.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\io_decorators.obj" \
	"$(INTDIR)\io_helpers.obj" \
	"$(INTDIR)\misc_support.obj" \
	"$(INTDIR)\readers.obj" \
	"$(INTDIR)\spec.obj" \
	"$(INTDIR)\tag.obj" \
	"$(INTDIR)\tag_file.obj" \
	"$(INTDIR)\tag_find.obj" \
	"$(INTDIR)\tag_impl.obj" \
	"$(INTDIR)\tag_parse.obj" \
	"$(INTDIR)\tag_parse_lyrics3.obj" \
	"$(INTDIR)\tag_parse_musicmatch.obj" \
	"$(INTDIR)\tag_parse_v1.obj" \
	"$(INTDIR)\tag_render.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\writers.obj" \
	"$(INTDIR)\id3v2.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\id3v2.mdf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Release" ".\id3v2.mdf"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                ..\..\..\..\base\win32\prj\plugins
	copy id3v2.mdf       ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "id3v2 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : ".\id3v2.mdf"

!ELSE 

ALL : "fabaselib - Win32 Debug" ".\id3v2.mdf"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\c_wrapper.obj"
	-@erase "$(INTDIR)\field.obj"
	-@erase "$(INTDIR)\field_binary.obj"
	-@erase "$(INTDIR)\field_integer.obj"
	-@erase "$(INTDIR)\field_string_ascii.obj"
	-@erase "$(INTDIR)\field_string_unicode.obj"
	-@erase "$(INTDIR)\frame.obj"
	-@erase "$(INTDIR)\frame_impl.obj"
	-@erase "$(INTDIR)\frame_parse.obj"
	-@erase "$(INTDIR)\frame_render.obj"
	-@erase "$(INTDIR)\header.obj"
	-@erase "$(INTDIR)\header_frame.obj"
	-@erase "$(INTDIR)\header_tag.obj"
	-@erase "$(INTDIR)\helpers.obj"
	-@erase "$(INTDIR)\id3v2.obj"
	-@erase "$(INTDIR)\id3v2.res"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\io_decorators.obj"
	-@erase "$(INTDIR)\io_helpers.obj"
	-@erase "$(INTDIR)\misc_support.obj"
	-@erase "$(INTDIR)\readers.obj"
	-@erase "$(INTDIR)\spec.obj"
	-@erase "$(INTDIR)\tag.obj"
	-@erase "$(INTDIR)\tag_file.obj"
	-@erase "$(INTDIR)\tag_find.obj"
	-@erase "$(INTDIR)\tag_impl.obj"
	-@erase "$(INTDIR)\tag_parse.obj"
	-@erase "$(INTDIR)\tag_parse_lyrics3.obj"
	-@erase "$(INTDIR)\tag_parse_musicmatch.obj"
	-@erase "$(INTDIR)\tag_parse_v1.obj"
	-@erase "$(INTDIR)\tag_render.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\writers.obj"
	-@erase "$(OUTDIR)\id3v2.exp"
	-@erase "$(OUTDIR)\id3v2.lib"
	-@erase "$(OUTDIR)\id3v2.pdb"
	-@erase ".\id3v2.ilk"
	-@erase ".\id3v2.mdf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\id3v2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\id3v2.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\id3v2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10600000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\id3v2.pdb" /debug /machine:I386 /def:".\id3v2.def" /out:"id3v2.mdf" /implib:"$(OUTDIR)\id3v2.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\id3v2.def"
LINK32_OBJS= \
	"$(INTDIR)\c_wrapper.obj" \
	"$(INTDIR)\field.obj" \
	"$(INTDIR)\field_binary.obj" \
	"$(INTDIR)\field_integer.obj" \
	"$(INTDIR)\field_string_ascii.obj" \
	"$(INTDIR)\field_string_unicode.obj" \
	"$(INTDIR)\frame.obj" \
	"$(INTDIR)\frame_impl.obj" \
	"$(INTDIR)\frame_parse.obj" \
	"$(INTDIR)\frame_render.obj" \
	"$(INTDIR)\header.obj" \
	"$(INTDIR)\header_frame.obj" \
	"$(INTDIR)\header_tag.obj" \
	"$(INTDIR)\helpers.obj" \
	"$(INTDIR)\id3v2.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\io_decorators.obj" \
	"$(INTDIR)\io_helpers.obj" \
	"$(INTDIR)\misc_support.obj" \
	"$(INTDIR)\readers.obj" \
	"$(INTDIR)\spec.obj" \
	"$(INTDIR)\tag.obj" \
	"$(INTDIR)\tag_file.obj" \
	"$(INTDIR)\tag_find.obj" \
	"$(INTDIR)\tag_impl.obj" \
	"$(INTDIR)\tag_parse.obj" \
	"$(INTDIR)\tag_parse_lyrics3.obj" \
	"$(INTDIR)\tag_parse_musicmatch.obj" \
	"$(INTDIR)\tag_parse_v1.obj" \
	"$(INTDIR)\tag_render.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\writers.obj" \
	"$(INTDIR)\id3v2.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\id3v2.mdf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Debug" ".\id3v2.mdf"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                ..\..\..\..\base\win32\prj\plugins
	copy id3v2.mdf       ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "id3v2 - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : ".\id3v2.mdf"

!ELSE 

ALL : "fabaselib - Win32 NASM Release" ".\id3v2.mdf"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\c_wrapper.obj"
	-@erase "$(INTDIR)\field.obj"
	-@erase "$(INTDIR)\field_binary.obj"
	-@erase "$(INTDIR)\field_integer.obj"
	-@erase "$(INTDIR)\field_string_ascii.obj"
	-@erase "$(INTDIR)\field_string_unicode.obj"
	-@erase "$(INTDIR)\frame.obj"
	-@erase "$(INTDIR)\frame_impl.obj"
	-@erase "$(INTDIR)\frame_parse.obj"
	-@erase "$(INTDIR)\frame_render.obj"
	-@erase "$(INTDIR)\header.obj"
	-@erase "$(INTDIR)\header_frame.obj"
	-@erase "$(INTDIR)\header_tag.obj"
	-@erase "$(INTDIR)\helpers.obj"
	-@erase "$(INTDIR)\id3v2.obj"
	-@erase "$(INTDIR)\id3v2.res"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\io_decorators.obj"
	-@erase "$(INTDIR)\io_helpers.obj"
	-@erase "$(INTDIR)\misc_support.obj"
	-@erase "$(INTDIR)\readers.obj"
	-@erase "$(INTDIR)\spec.obj"
	-@erase "$(INTDIR)\tag.obj"
	-@erase "$(INTDIR)\tag_file.obj"
	-@erase "$(INTDIR)\tag_find.obj"
	-@erase "$(INTDIR)\tag_impl.obj"
	-@erase "$(INTDIR)\tag_parse.obj"
	-@erase "$(INTDIR)\tag_parse_lyrics3.obj"
	-@erase "$(INTDIR)\tag_parse_musicmatch.obj"
	-@erase "$(INTDIR)\tag_parse_v1.obj"
	-@erase "$(INTDIR)\tag_render.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\writers.obj"
	-@erase "$(OUTDIR)\id3v2.exp"
	-@erase "$(OUTDIR)\id3v2.lib"
	-@erase ".\id3v2.mdf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\id3v2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\id3v2.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\id3v2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10600000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\id3v2.pdb" /machine:I386 /def:".\id3v2.def" /out:"id3v2.mdf" /implib:"$(OUTDIR)\id3v2.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\id3v2.def"
LINK32_OBJS= \
	"$(INTDIR)\c_wrapper.obj" \
	"$(INTDIR)\field.obj" \
	"$(INTDIR)\field_binary.obj" \
	"$(INTDIR)\field_integer.obj" \
	"$(INTDIR)\field_string_ascii.obj" \
	"$(INTDIR)\field_string_unicode.obj" \
	"$(INTDIR)\frame.obj" \
	"$(INTDIR)\frame_impl.obj" \
	"$(INTDIR)\frame_parse.obj" \
	"$(INTDIR)\frame_render.obj" \
	"$(INTDIR)\header.obj" \
	"$(INTDIR)\header_frame.obj" \
	"$(INTDIR)\header_tag.obj" \
	"$(INTDIR)\helpers.obj" \
	"$(INTDIR)\id3v2.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\io_decorators.obj" \
	"$(INTDIR)\io_helpers.obj" \
	"$(INTDIR)\misc_support.obj" \
	"$(INTDIR)\readers.obj" \
	"$(INTDIR)\spec.obj" \
	"$(INTDIR)\tag.obj" \
	"$(INTDIR)\tag_file.obj" \
	"$(INTDIR)\tag_find.obj" \
	"$(INTDIR)\tag_impl.obj" \
	"$(INTDIR)\tag_parse.obj" \
	"$(INTDIR)\tag_parse_lyrics3.obj" \
	"$(INTDIR)\tag_parse_musicmatch.obj" \
	"$(INTDIR)\tag_parse_v1.obj" \
	"$(INTDIR)\tag_render.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\writers.obj" \
	"$(INTDIR)\id3v2.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\id3v2.mdf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Release" ".\id3v2.mdf"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                ..\..\..\..\base\win32\prj\plugins
	copy id3v2.mdf       ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "id3v2 - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : ".\id3v2.mdf"

!ELSE 

ALL : "fabaselib - Win32 NASM Debug" ".\id3v2.mdf"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\c_wrapper.obj"
	-@erase "$(INTDIR)\field.obj"
	-@erase "$(INTDIR)\field_binary.obj"
	-@erase "$(INTDIR)\field_integer.obj"
	-@erase "$(INTDIR)\field_string_ascii.obj"
	-@erase "$(INTDIR)\field_string_unicode.obj"
	-@erase "$(INTDIR)\frame.obj"
	-@erase "$(INTDIR)\frame_impl.obj"
	-@erase "$(INTDIR)\frame_parse.obj"
	-@erase "$(INTDIR)\frame_render.obj"
	-@erase "$(INTDIR)\header.obj"
	-@erase "$(INTDIR)\header_frame.obj"
	-@erase "$(INTDIR)\header_tag.obj"
	-@erase "$(INTDIR)\helpers.obj"
	-@erase "$(INTDIR)\id3v2.obj"
	-@erase "$(INTDIR)\id3v2.res"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\io_decorators.obj"
	-@erase "$(INTDIR)\io_helpers.obj"
	-@erase "$(INTDIR)\misc_support.obj"
	-@erase "$(INTDIR)\readers.obj"
	-@erase "$(INTDIR)\spec.obj"
	-@erase "$(INTDIR)\tag.obj"
	-@erase "$(INTDIR)\tag_file.obj"
	-@erase "$(INTDIR)\tag_find.obj"
	-@erase "$(INTDIR)\tag_impl.obj"
	-@erase "$(INTDIR)\tag_parse.obj"
	-@erase "$(INTDIR)\tag_parse_lyrics3.obj"
	-@erase "$(INTDIR)\tag_parse_musicmatch.obj"
	-@erase "$(INTDIR)\tag_parse_v1.obj"
	-@erase "$(INTDIR)\tag_render.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\writers.obj"
	-@erase "$(OUTDIR)\id3v2.exp"
	-@erase "$(OUTDIR)\id3v2.lib"
	-@erase "$(OUTDIR)\id3v2.pdb"
	-@erase ".\id3v2.ilk"
	-@erase ".\id3v2.mdf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\id3v2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32  /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\id3v2.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\id3v2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10600000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\id3v2.pdb" /debug /machine:I386 /def:".\id3v2.def" /out:"id3v2.mdf" /implib:"$(OUTDIR)\id3v2.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\id3v2.def"
LINK32_OBJS= \
	"$(INTDIR)\c_wrapper.obj" \
	"$(INTDIR)\field.obj" \
	"$(INTDIR)\field_binary.obj" \
	"$(INTDIR)\field_integer.obj" \
	"$(INTDIR)\field_string_ascii.obj" \
	"$(INTDIR)\field_string_unicode.obj" \
	"$(INTDIR)\frame.obj" \
	"$(INTDIR)\frame_impl.obj" \
	"$(INTDIR)\frame_parse.obj" \
	"$(INTDIR)\frame_render.obj" \
	"$(INTDIR)\header.obj" \
	"$(INTDIR)\header_frame.obj" \
	"$(INTDIR)\header_tag.obj" \
	"$(INTDIR)\helpers.obj" \
	"$(INTDIR)\id3v2.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\io_decorators.obj" \
	"$(INTDIR)\io_helpers.obj" \
	"$(INTDIR)\misc_support.obj" \
	"$(INTDIR)\readers.obj" \
	"$(INTDIR)\spec.obj" \
	"$(INTDIR)\tag.obj" \
	"$(INTDIR)\tag_file.obj" \
	"$(INTDIR)\tag_find.obj" \
	"$(INTDIR)\tag_impl.obj" \
	"$(INTDIR)\tag_parse.obj" \
	"$(INTDIR)\tag_parse_lyrics3.obj" \
	"$(INTDIR)\tag_parse_musicmatch.obj" \
	"$(INTDIR)\tag_parse_v1.obj" \
	"$(INTDIR)\tag_render.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\writers.obj" \
	"$(INTDIR)\id3v2.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\id3v2.mdf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Debug" ".\id3v2.mdf"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                ..\..\..\..\base\win32\prj\plugins
	copy id3v2.mdf       ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("id3v2.dep")
!INCLUDE "id3v2.dep"
!ELSE 
!MESSAGE Warning: cannot find "id3v2.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "id3v2 - Win32 Release" || "$(CFG)" == "id3v2 - Win32 Debug" || "$(CFG)" == "id3v2 - Win32 NASM Release" || "$(CFG)" == "id3v2 - Win32 NASM Debug"
SOURCE=..\c_wrapper.cpp

"$(INTDIR)\c_wrapper.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\field.cpp

"$(INTDIR)\field.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\field_binary.cpp

"$(INTDIR)\field_binary.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\field_integer.cpp

"$(INTDIR)\field_integer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\field_string_ascii.cpp

"$(INTDIR)\field_string_ascii.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\field_string_unicode.cpp

"$(INTDIR)\field_string_unicode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\frame.cpp

"$(INTDIR)\frame.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\frame_impl.cpp

"$(INTDIR)\frame_impl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\frame_parse.cpp

"$(INTDIR)\frame_parse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\frame_render.cpp

"$(INTDIR)\frame_render.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\header.cpp

"$(INTDIR)\header.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\header_frame.cpp

"$(INTDIR)\header_frame.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\header_tag.cpp

"$(INTDIR)\header_tag.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\helpers.cpp

"$(INTDIR)\helpers.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\id3v2.cpp

"$(INTDIR)\id3v2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\io.cpp

"$(INTDIR)\io.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\io_decorators.cpp

"$(INTDIR)\io_decorators.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\io_helpers.cpp

"$(INTDIR)\io_helpers.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\misc_support.cpp

"$(INTDIR)\misc_support.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\readers.cpp

"$(INTDIR)\readers.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\spec.cpp

"$(INTDIR)\spec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\tag.cpp

"$(INTDIR)\tag.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\tag_file.cpp

"$(INTDIR)\tag_file.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\tag_find.cpp

"$(INTDIR)\tag_find.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\tag_impl.cpp

"$(INTDIR)\tag_impl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\tag_parse.cpp

"$(INTDIR)\tag_parse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\tag_parse_lyrics3.cpp

"$(INTDIR)\tag_parse_lyrics3.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\tag_parse_musicmatch.cpp

"$(INTDIR)\tag_parse_musicmatch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\tag_parse_v1.cpp

"$(INTDIR)\tag_parse_v1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\tag_render.cpp

"$(INTDIR)\tag_render.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\utils.cpp

"$(INTDIR)\utils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\writers.cpp

"$(INTDIR)\writers.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\res\id3v2.rc

!IF  "$(CFG)" == "id3v2 - Win32 Release"


"$(INTDIR)\id3v2.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\id3v2.res" /i "\Local\src\freeamp\plm\metadata\id3v2\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "id3v2 - Win32 Debug"


"$(INTDIR)\id3v2.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\id3v2.res" /i "\Local\src\freeamp\plm\metadata\id3v2\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "id3v2 - Win32 NASM Release"


"$(INTDIR)\id3v2.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\id3v2.res" /i "\Local\src\freeamp\plm\metadata\id3v2\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "id3v2 - Win32 NASM Debug"


"$(INTDIR)\id3v2.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\id3v2.res" /i "\Local\src\freeamp\plm\metadata\id3v2\res" /d "_DEBUG" $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "id3v2 - Win32 Release"

"fabaselib - Win32 Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" 
   cd "..\..\..\plm\metadata\id3v2\prj"

"fabaselib - Win32 ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\plm\metadata\id3v2\prj"

!ELSEIF  "$(CFG)" == "id3v2 - Win32 Debug"

"fabaselib - Win32 Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" 
   cd "..\..\..\plm\metadata\id3v2\prj"

"fabaselib - Win32 DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\plm\metadata\id3v2\prj"

!ELSEIF  "$(CFG)" == "id3v2 - Win32 NASM Release"

"fabaselib - Win32 NASM Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" 
   cd "..\..\..\plm\metadata\id3v2\prj"

"fabaselib - Win32 NASM ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" RECURSE=1 CLEAN 
   cd "..\..\..\plm\metadata\id3v2\prj"

!ELSEIF  "$(CFG)" == "id3v2 - Win32 NASM Debug"

"fabaselib - Win32 NASM Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" 
   cd "..\..\..\plm\metadata\id3v2\prj"

"fabaselib - Win32 NASM DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" RECURSE=1 CLEAN 
   cd "..\..\..\plm\metadata\id3v2\prj"

!ENDIF 


!ENDIF 

