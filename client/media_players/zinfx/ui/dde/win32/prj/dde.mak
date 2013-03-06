# Microsoft Developer Studio Generated NMAKE File, Based on dde.dsp
!IF "$(CFG)" == ""
CFG=dde - Win32 Release
!MESSAGE No configuration specified. Defaulting to dde - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "dde - Win32 Release" && "$(CFG)" != "dde - Win32 Debug" && "$(CFG)" != "dde - Win32 NASM Debug" && "$(CFG)" != "dde - Win32 NASM Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dde.mak" CFG="dde - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dde - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dde - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dde - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dde - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "dde - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\config\config.h" ".\dde.ui"


CLEAN :
	-@erase "$(INTDIR)\dde.res"
	-@erase "$(INTDIR)\DDE_UI.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dde.exp"
	-@erase "$(OUTDIR)\dde.lib"
	-@erase ".\dde.ui"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\res" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\dde.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dde.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dde.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib fabaselib.lib version.lib gdbm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x10900000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\dde.pdb" /machine:I386 /def:".\dde.def" /out:"dde.ui" /implib:"$(OUTDIR)\dde.lib" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\musicbrainz" 
DEF_FILE= \
	".\dde.def"
LINK32_OBJS= \
	"$(INTDIR)\DDE_UI.obj" \
	"$(INTDIR)\dde.res"

".\dde.ui" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\..\..\config\config.h" ".\dde.ui"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                                                                     ..\..\..\..\base\win32\prj\plugins
	copy dde.ui                                                                                       ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "dde - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\config\config.h" ".\dde.ui"


CLEAN :
	-@erase "$(INTDIR)\dde.res"
	-@erase "$(INTDIR)\DDE_UI.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dde.exp"
	-@erase "$(OUTDIR)\dde.lib"
	-@erase "$(OUTDIR)\dde.pdb"
	-@erase ".\dde.ilk"
	-@erase ".\dde.ui"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\res" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\dde.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dde.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dde.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib fabaselib.lib version.lib gdbm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x10900000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\dde.pdb" /debug /machine:I386 /def:".\dde.def" /out:"dde.ui" /implib:"$(OUTDIR)\dde.lib" /pdbtype:sept /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\musicbrainz" 
LINK32_OBJS= \
	"$(INTDIR)\DDE_UI.obj" \
	"$(INTDIR)\dde.res"

".\dde.ui" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\..\..\config\config.h" ".\dde.ui"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                                                                     ..\..\..\..\base\win32\prj\plugins
	copy dde.ui                                                                                       ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "dde - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\config\config.h" ".\dde.ui"


CLEAN :
	-@erase "$(INTDIR)\dde.res"
	-@erase "$(INTDIR)\DDE_UI.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dde.exp"
	-@erase "$(OUTDIR)\dde.lib"
	-@erase "$(OUTDIR)\dde.pdb"
	-@erase ".\dde.ilk"
	-@erase ".\dde.ui"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\res" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\dde.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dde.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dde.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib fabaselib.lib version.lib gdbm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x10900000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\dde.pdb" /debug /machine:I386 /def:".\dde.def" /out:"dde.ui" /implib:"$(OUTDIR)\dde.lib" /pdbtype:sept /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\musicbrainz" 
LINK32_OBJS= \
	"$(INTDIR)\DDE_UI.obj" \
	"$(INTDIR)\dde.res"

".\dde.ui" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\..\..\config\config.h" ".\dde.ui"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                                                                     ..\..\..\..\base\win32\prj\plugins
	copy dde.ui                                                                                       ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "dde - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\config\config.h" ".\dde.ui"


CLEAN :
	-@erase "$(INTDIR)\dde.res"
	-@erase "$(INTDIR)\DDE_UI.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dde.exp"
	-@erase "$(OUTDIR)\dde.lib"
	-@erase ".\dde.ui"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\res" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\dde.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dde.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dde.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib fabaselib.lib version.lib gdbm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x10900000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\dde.pdb" /machine:I386 /def:".\dde.def" /out:"dde.ui" /implib:"$(OUTDIR)\dde.lib" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\musicbrainz" 
DEF_FILE= \
	".\dde.def"
LINK32_OBJS= \
	"$(INTDIR)\DDE_UI.obj" \
	"$(INTDIR)\dde.res"

".\dde.ui" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\..\..\config\config.h" ".\dde.ui"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                                                                     ..\..\..\..\base\win32\prj\plugins
	copy dde.ui                                                                                       ..\..\..\..\base\win32\prj\plugins
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
!IF EXISTS("dde.dep")
!INCLUDE "dde.dep"
!ELSE 
!MESSAGE Warning: cannot find "dde.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dde - Win32 Release" || "$(CFG)" == "dde - Win32 Debug" || "$(CFG)" == "dde - Win32 NASM Debug" || "$(CFG)" == "dde - Win32 NASM Release"
SOURCE=..\..\..\..\config\config.win32

!IF  "$(CFG)" == "dde - Win32 Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "dde - Win32 Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "dde - Win32 NASM Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "dde - Win32 NASM Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ENDIF 

SOURCE=..\src\DDE_UI.cpp

"$(INTDIR)\DDE_UI.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\res\dde.rc

!IF  "$(CFG)" == "dde - Win32 Release"


"$(INTDIR)\dde.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\dde.res" /i "\Local\src\freeamp\ui\dde\win32\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "dde - Win32 Debug"


"$(INTDIR)\dde.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\dde.res" /i "\Local\src\freeamp\ui\dde\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "dde - Win32 NASM Debug"


"$(INTDIR)\dde.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\dde.res" /i "\Local\src\freeamp\ui\dde\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "dde - Win32 NASM Release"


"$(INTDIR)\dde.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\dde.res" /i "\Local\src\freeamp\ui\dde\win32\res" /d "NDEBUG" $(SOURCE)


!ENDIF 


!ENDIF 

