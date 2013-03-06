# Microsoft Developer Studio Generated NMAKE File, Based on soundcard.dsp
!IF "$(CFG)" == ""
CFG=soundcard - Win32 NASM Debug MS STL
!MESSAGE No configuration specified. Defaulting to soundcard - Win32 NASM Debug MS STL.
!ENDIF 

!IF "$(CFG)" != "soundcard - Win32 Release" && "$(CFG)" != "soundcard - Win32 Debug" && "$(CFG)" != "soundcard - Win32 NASM Debug" && "$(CFG)" != "soundcard - Win32 NASM Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "soundcard.mak" CFG="soundcard - Win32 NASM Debug MS STL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "soundcard - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "soundcard - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "soundcard - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "soundcard - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "soundcard - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\soundcard.pmo"

!ELSE 

ALL : "fabaselib - Win32 Release" "..\..\..\..\config\config.h" ".\soundcard.pmo"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\eventbuffer.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmo.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\soundcard.res"
	-@erase "$(INTDIR)\soundcardpmo.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\win32volume.obj"
	-@erase "$(OUTDIR)\soundcard.exp"
	-@erase "$(OUTDIR)\soundcard.lib"
	-@erase ".\soundcard.pmo"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\soundcard.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\soundcard.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\soundcard.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11500000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\soundcard.pdb" /machine:I386 /def:".\soundcard.def" /out:"soundcard.pmo" /implib:"$(OUTDIR)\soundcard.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\soundcard.def"
LINK32_OBJS= \
	"$(INTDIR)\eventbuffer.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmo.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\soundcardpmo.obj" \
	"$(INTDIR)\win32volume.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\soundcard.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\soundcard.pmo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Release" "..\..\..\..\config\config.h" ".\soundcard.pmo"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                       ..\..\..\..\base\win32\prj\plugins
	copy soundcard.pmo                                  ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "soundcard - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\soundcard.pmo"

!ELSE 

ALL : "fabaselib - Win32 Debug" "..\..\..\..\config\config.h" ".\soundcard.pmo"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\eventbuffer.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmo.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\soundcard.res"
	-@erase "$(INTDIR)\soundcardpmo.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\win32volume.obj"
	-@erase "$(OUTDIR)\soundcard.exp"
	-@erase "$(OUTDIR)\soundcard.lib"
	-@erase "$(OUTDIR)\soundcard.pdb"
	-@erase ".\soundcard.ilk"
	-@erase ".\soundcard.pmo"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\soundcard.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\soundcard.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\soundcard.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11500000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\soundcard.pdb" /debug /machine:I386 /def:".\soundcard.def" /out:"soundcard.pmo" /implib:"$(OUTDIR)\soundcard.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\soundcard.def"
LINK32_OBJS= \
	"$(INTDIR)\eventbuffer.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmo.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\soundcardpmo.obj" \
	"$(INTDIR)\win32volume.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\soundcard.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\soundcard.pmo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Debug" "..\..\..\..\config\config.h" ".\soundcard.pmo"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                       ..\..\..\..\base\win32\prj\plugins
	copy soundcard.pmo                                  ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "soundcard - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\soundcard.pmo"

!ELSE 

ALL : "fabaselib - Win32 NASM Debug" "..\..\..\..\config\config.h" ".\soundcard.pmo"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\eventbuffer.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmo.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\soundcard.res"
	-@erase "$(INTDIR)\soundcardpmo.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\win32volume.obj"
	-@erase "$(OUTDIR)\soundcard.exp"
	-@erase "$(OUTDIR)\soundcard.lib"
	-@erase "$(OUTDIR)\soundcard.pdb"
	-@erase ".\soundcard.ilk"
	-@erase ".\soundcard.pmo"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\soundcard.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\soundcard.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\soundcard.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11500000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\soundcard.pdb" /debug /machine:I386 /def:".\soundcard.def" /out:"soundcard.pmo" /implib:"$(OUTDIR)\soundcard.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\soundcard.def"
LINK32_OBJS= \
	"$(INTDIR)\eventbuffer.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmo.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\soundcardpmo.obj" \
	"$(INTDIR)\win32volume.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\soundcard.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\soundcard.pmo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Debug" "..\..\..\..\config\config.h" ".\soundcard.pmo"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                       ..\..\..\..\base\win32\prj\plugins
	copy soundcard.pmo                                  ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "soundcard - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\soundcard.pmo"

!ELSE 

ALL : "fabaselib - Win32 NASM Release" "..\..\..\..\config\config.h" ".\soundcard.pmo"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\eventbuffer.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmo.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\soundcard.res"
	-@erase "$(INTDIR)\soundcardpmo.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\win32volume.obj"
	-@erase "$(OUTDIR)\soundcard.exp"
	-@erase "$(OUTDIR)\soundcard.lib"
	-@erase ".\soundcard.pmo"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\soundcard.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\soundcard.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\soundcard.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11500000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\soundcard.pdb" /machine:I386 /def:".\soundcard.def" /out:"soundcard.pmo" /implib:"$(OUTDIR)\soundcard.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\soundcard.def"
LINK32_OBJS= \
	"$(INTDIR)\eventbuffer.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmo.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\soundcardpmo.obj" \
	"$(INTDIR)\win32volume.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\soundcard.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\soundcard.pmo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Release" "..\..\..\..\config\config.h" ".\soundcard.pmo"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                       ..\..\..\..\base\win32\prj\plugins
	copy soundcard.pmo                                  ..\..\..\..\base\win32\prj\plugins
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
!IF EXISTS("soundcard.dep")
!INCLUDE "soundcard.dep"
!ELSE 
!MESSAGE Warning: cannot find "soundcard.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "soundcard - Win32 Release" || "$(CFG)" == "soundcard - Win32 Debug" || "$(CFG)" == "soundcard - Win32 NASM Debug" || "$(CFG)" == "soundcard - Win32 NASM Release"
SOURCE=..\..\..\..\config\config.win32

!IF  "$(CFG)" == "soundcard - Win32 Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "soundcard - Win32 Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "soundcard - Win32 NASM Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "soundcard - Win32 NASM Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ENDIF 

SOURCE=..\..\..\src\eventbuffer.cpp

"$(INTDIR)\eventbuffer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\src\pipeline.cpp

"$(INTDIR)\pipeline.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\src\pmo.cpp

"$(INTDIR)\pmo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\src\pullbuffer.cpp

"$(INTDIR)\pullbuffer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\soundcardpmo.cpp

"$(INTDIR)\soundcardpmo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\src\win32volume.cpp

"$(INTDIR)\win32volume.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\res\soundcard.rc

!IF  "$(CFG)" == "soundcard - Win32 Release"


"$(INTDIR)\soundcard.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\soundcard.res" /i "\Local\src\freeamp\io\soundcard\win32\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "soundcard - Win32 Debug"


"$(INTDIR)\soundcard.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\soundcard.res" /i "\Local\src\freeamp\io\soundcard\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "soundcard - Win32 NASM Debug"


"$(INTDIR)\soundcard.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\soundcard.res" /i "\Local\src\freeamp\io\soundcard\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "soundcard - Win32 NASM Release"


"$(INTDIR)\soundcard.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\soundcard.res" /i "\Local\src\freeamp\io\soundcard\win32\res" /d "NDEBUG" $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "soundcard - Win32 Release"

"fabaselib - Win32 Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" 
   cd "..\..\..\io\soundcard\win32\prj"

"fabaselib - Win32 ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\io\soundcard\win32\prj"

!ELSEIF  "$(CFG)" == "soundcard - Win32 Debug"

"fabaselib - Win32 Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" 
   cd "..\..\..\io\soundcard\win32\prj"

"fabaselib - Win32 DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\io\soundcard\win32\prj"

!ELSEIF  "$(CFG)" == "soundcard - Win32 NASM Debug"

"fabaselib - Win32 NASM Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" 
   cd "..\..\..\io\soundcard\win32\prj"

"fabaselib - Win32 NASM DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" RECURSE=1 CLEAN 
   cd "..\..\..\io\soundcard\win32\prj"

!ELSEIF  "$(CFG)" == "soundcard - Win32 NASM Release"

"fabaselib - Win32 NASM Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" 
   cd "..\..\..\io\soundcard\win32\prj"

"fabaselib - Win32 NASM ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" RECURSE=1 CLEAN 
   cd "..\..\..\io\soundcard\win32\prj"

!ENDIF 

SOURCE=..\..\..\..\base\src\debug.cpp

"$(INTDIR)\debug.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

