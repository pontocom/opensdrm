# Microsoft Developer Studio Generated NMAKE File, Based on cdoutput.dsp
!IF "$(CFG)" == ""
CFG=cdoutput - Win32 Release
!MESSAGE No configuration specified. Defaulting to cdoutput - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "cdoutput - Win32 Release" && "$(CFG)" != "cdoutput - Win32 Debug" && "$(CFG)" != "cdoutput - Win32 NASM Debug" && "$(CFG)" != "cdoutput - Win32 NASM Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cdoutput.mak" CFG="cdoutput - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cdoutput - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cdoutput - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cdoutput - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cdoutput - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "cdoutput - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\config\config.h" ".\cd.pmo"


CLEAN :
	-@erase "$(INTDIR)\cdaudio.obj"
	-@erase "$(INTDIR)\cdoutput.res"
	-@erase "$(INTDIR)\cdpmo.obj"
	-@erase "$(INTDIR)\discids.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmo.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\win32volume.obj"
	-@erase "$(OUTDIR)\cd.exp"
	-@erase "$(OUTDIR)\cd.lib"
	-@erase ".\cd.pmo"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\cdoutput.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cdoutput.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cdoutput.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /base:"0x11900000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\cd.pdb" /machine:I386 /def:".\cdoutput.def" /out:"cd.pmo" /implib:"$(OUTDIR)\cd.lib" /libpath:"..\..\..\..\base\win32" 
LINK32_OBJS= \
	"$(INTDIR)\cdaudio.obj" \
	"$(INTDIR)\cdpmo.obj" \
	"$(INTDIR)\discids.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmo.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\win32volume.obj" \
	"$(INTDIR)\cdoutput.res"

".\cd.pmo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\..\..\config\config.h" ".\cd.pmo"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                          ..\..\..\..\base\win32\prj\plugins
	copy cd.pmo                 ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "cdoutput - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\config\config.h" ".\cd.pmo"


CLEAN :
	-@erase "$(INTDIR)\cdaudio.obj"
	-@erase "$(INTDIR)\cdoutput.res"
	-@erase "$(INTDIR)\cdpmo.obj"
	-@erase "$(INTDIR)\discids.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmo.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\win32volume.obj"
	-@erase "$(OUTDIR)\cd.exp"
	-@erase "$(OUTDIR)\cd.lib"
	-@erase "$(OUTDIR)\cd.pdb"
	-@erase ".\cd.ilk"
	-@erase ".\cd.pmo"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\cdoutput.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cdoutput.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cdoutput.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /base:"0x11900000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\cd.pdb" /debug /machine:I386 /def:".\cdoutput.def" /out:"cd.pmo" /implib:"$(OUTDIR)\cd.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
LINK32_OBJS= \
	"$(INTDIR)\cdaudio.obj" \
	"$(INTDIR)\cdpmo.obj" \
	"$(INTDIR)\discids.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmo.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\win32volume.obj" \
	"$(INTDIR)\cdoutput.res"

".\cd.pmo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\..\..\config\config.h" ".\cd.pmo"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                          ..\..\..\..\base\win32\prj\plugins
	copy cd.pmo                 ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "cdoutput - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\config\config.h" ".\cd.pmo"


CLEAN :
	-@erase "$(INTDIR)\cdaudio.obj"
	-@erase "$(INTDIR)\cdoutput.res"
	-@erase "$(INTDIR)\cdpmo.obj"
	-@erase "$(INTDIR)\discids.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmo.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\win32volume.obj"
	-@erase "$(OUTDIR)\cd.exp"
	-@erase "$(OUTDIR)\cd.lib"
	-@erase "$(OUTDIR)\cd.pdb"
	-@erase ".\cd.ilk"
	-@erase ".\cd.pmo"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\cdoutput.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cdoutput.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cdoutput.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /base:"0x11900000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\cd.pdb" /debug /machine:I386 /def:".\cdoutput.def" /out:"cd.pmo" /implib:"$(OUTDIR)\cd.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
LINK32_OBJS= \
	"$(INTDIR)\cdaudio.obj" \
	"$(INTDIR)\cdpmo.obj" \
	"$(INTDIR)\discids.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmo.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\win32volume.obj" \
	"$(INTDIR)\cdoutput.res"

".\cd.pmo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\..\..\config\config.h" ".\cd.pmo"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                          ..\..\..\..\base\win32\prj\plugins
	copy cd.pmo                 ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "cdoutput - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\config\config.h" ".\cd.pmo"


CLEAN :
	-@erase "$(INTDIR)\cdaudio.obj"
	-@erase "$(INTDIR)\cdoutput.res"
	-@erase "$(INTDIR)\cdpmo.obj"
	-@erase "$(INTDIR)\discids.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmo.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\win32volume.obj"
	-@erase "$(OUTDIR)\cd.exp"
	-@erase "$(OUTDIR)\cd.lib"
	-@erase ".\cd.pmo"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\cdoutput.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cdoutput.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cdoutput.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /base:"0x11900000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\cd.pdb" /machine:I386 /def:".\cdoutput.def" /out:"cd.pmo" /implib:"$(OUTDIR)\cd.lib" /libpath:"..\..\..\..\base\win32" 
LINK32_OBJS= \
	"$(INTDIR)\cdaudio.obj" \
	"$(INTDIR)\cdpmo.obj" \
	"$(INTDIR)\discids.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmo.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\win32volume.obj" \
	"$(INTDIR)\cdoutput.res"

".\cd.pmo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\..\..\config\config.h" ".\cd.pmo"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                          ..\..\..\..\base\win32\prj\plugins
	copy cd.pmo                 ..\..\..\..\base\win32\prj\plugins
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
!IF EXISTS("cdoutput.dep")
!INCLUDE "cdoutput.dep"
!ELSE 
!MESSAGE Warning: cannot find "cdoutput.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "cdoutput - Win32 Release" || "$(CFG)" == "cdoutput - Win32 Debug" || "$(CFG)" == "cdoutput - Win32 NASM Debug" || "$(CFG)" == "cdoutput - Win32 NASM Release"
SOURCE=..\src\cdaudio.cpp

"$(INTDIR)\cdaudio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\cdpmo.cpp

"$(INTDIR)\cdpmo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\config\config.win32

!IF  "$(CFG)" == "cdoutput - Win32 Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "cdoutput - Win32 Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "cdoutput - Win32 NASM Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "cdoutput - Win32 NASM Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ENDIF 

SOURCE=..\src\discids.cpp

"$(INTDIR)\discids.obj" : $(SOURCE) "$(INTDIR)"
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


SOURCE=..\..\..\src\win32volume.cpp

"$(INTDIR)\win32volume.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\res\cdoutput.rc

!IF  "$(CFG)" == "cdoutput - Win32 Release"


"$(INTDIR)\cdoutput.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\cdoutput.res" /i "\Local\src\freeamp\io\cd\win32\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "cdoutput - Win32 Debug"


"$(INTDIR)\cdoutput.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\cdoutput.res" /i "\Local\src\freeamp\io\cd\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "cdoutput - Win32 NASM Debug"


"$(INTDIR)\cdoutput.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\cdoutput.res" /i "\Local\src\freeamp\io\cd\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "cdoutput - Win32 NASM Release"


"$(INTDIR)\cdoutput.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\cdoutput.res" /i "\Local\src\freeamp\io\cd\win32\res" /d "NDEBUG" $(SOURCE)


!ENDIF 


!ENDIF 

