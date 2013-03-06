# Microsoft Developer Studio Generated NMAKE File, Based on unzip.dsp
!IF "$(CFG)" == ""
CFG=unzip - Win32 NASM Debug MS STL
!MESSAGE No configuration specified. Defaulting to unzip - Win32 NASM Debug MS STL.
!ENDIF 

!IF "$(CFG)" != "unzip - Win32 Release" && "$(CFG)" != "unzip - Win32 Debug" && "$(CFG)" != "unzip - Win32 NASM Debug" && "$(CFG)" != "unzip - Win32 NASM Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "unzip.mak" CFG="unzip - Win32 NASM Debug MS STL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "unzip - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "unzip - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "unzip - Win32 NASM Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "unzip - Win32 NASM Release" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "unzip - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\unzip.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\zip_header.obj"
	-@erase "$(INTDIR)\zip_inflate.obj"
	-@erase "$(INTDIR)\zip_testcrc.obj"
	-@erase "$(INTDIR)\zip_utils.obj"
	-@erase "$(INTDIR)\zip_wrapper.obj"
	-@erase "..\unzip.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\zlib\include" /I "..\..\..\config" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\unzip.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\unzip.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\unzip.lib" 
LIB32_OBJS= \
	"$(INTDIR)\zip_header.obj" \
	"$(INTDIR)\zip_inflate.obj" \
	"$(INTDIR)\zip_testcrc.obj" \
	"$(INTDIR)\zip_utils.obj" \
	"$(INTDIR)\zip_wrapper.obj"

"..\unzip.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "unzip - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\unzip.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\zip_header.obj"
	-@erase "$(INTDIR)\zip_inflate.obj"
	-@erase "$(INTDIR)\zip_testcrc.obj"
	-@erase "$(INTDIR)\zip_utils.obj"
	-@erase "$(INTDIR)\zip_wrapper.obj"
	-@erase "..\unzip.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "..\include" /I "..\..\zlib\include" /I "..\..\..\config" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\unzip.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\unzip.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\unzip.lib" 
LIB32_OBJS= \
	"$(INTDIR)\zip_header.obj" \
	"$(INTDIR)\zip_inflate.obj" \
	"$(INTDIR)\zip_testcrc.obj" \
	"$(INTDIR)\zip_utils.obj" \
	"$(INTDIR)\zip_wrapper.obj"

"..\unzip.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "unzip - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\unzip.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\zip_header.obj"
	-@erase "$(INTDIR)\zip_inflate.obj"
	-@erase "$(INTDIR)\zip_testcrc.obj"
	-@erase "$(INTDIR)\zip_utils.obj"
	-@erase "$(INTDIR)\zip_wrapper.obj"
	-@erase "..\unzip.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "..\include" /I "..\..\zlib\include" /I "..\..\..\config" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\unzip.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\unzip.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\unzip.lib" 
LIB32_OBJS= \
	"$(INTDIR)\zip_header.obj" \
	"$(INTDIR)\zip_inflate.obj" \
	"$(INTDIR)\zip_testcrc.obj" \
	"$(INTDIR)\zip_utils.obj" \
	"$(INTDIR)\zip_wrapper.obj"

"..\unzip.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "unzip - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\unzip.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\zip_header.obj"
	-@erase "$(INTDIR)\zip_inflate.obj"
	-@erase "$(INTDIR)\zip_testcrc.obj"
	-@erase "$(INTDIR)\zip_utils.obj"
	-@erase "$(INTDIR)\zip_wrapper.obj"
	-@erase "..\unzip.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\zlib\include" /I "..\..\..\config" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\unzip.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\unzip.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\unzip.lib" 
LIB32_OBJS= \
	"$(INTDIR)\zip_header.obj" \
	"$(INTDIR)\zip_inflate.obj" \
	"$(INTDIR)\zip_testcrc.obj" \
	"$(INTDIR)\zip_utils.obj" \
	"$(INTDIR)\zip_wrapper.obj"

"..\unzip.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

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
!IF EXISTS("unzip.dep")
!INCLUDE "unzip.dep"
!ELSE 
!MESSAGE Warning: cannot find "unzip.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "unzip - Win32 Release" || "$(CFG)" == "unzip - Win32 Debug" || "$(CFG)" == "unzip - Win32 NASM Debug" || "$(CFG)" == "unzip - Win32 NASM Release"
SOURCE=..\src\zip_header.cpp

"$(INTDIR)\zip_header.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\zip_inflate.cpp

"$(INTDIR)\zip_inflate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\zip_testcrc.cpp

"$(INTDIR)\zip_testcrc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\zip_utils.cpp

"$(INTDIR)\zip_utils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\zip_wrapper.cpp

"$(INTDIR)\zip_wrapper.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

