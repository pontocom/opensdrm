# Microsoft Developer Studio Generated NMAKE File, Based on dsoundcard.dsp
!IF "$(CFG)" == ""
CFG=dsoundcard - Win32 Release
!MESSAGE No configuration specified. Defaulting to dsoundcard - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "dsoundcard - Win32 Release" && "$(CFG)" != "dsoundcard - Win32 Debug" && "$(CFG)" != "dsoundcard - Win32 NASM Debug" && "$(CFG)" != "dsoundcard - Win32 NASM Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dsoundcard.mak" CFG="dsoundcard - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dsoundcard - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dsoundcard - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dsoundcard - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dsoundcard - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "dsoundcard - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\dsoundcard.pmo"

!ELSE 

ALL : "fabaselib - Win32 Release" "..\..\..\..\config\config.h" ".\dsoundcard.pmo"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dsound.res"
	-@erase "$(INTDIR)\dsoundcardpmo.obj"
	-@erase "$(INTDIR)\eventbuffer.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmo.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dsoundcard.exp"
	-@erase "$(OUTDIR)\dsoundcard.lib"
	-@erase ".\dsoundcard.pmo"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Ob2 /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\dsoundcard.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL" 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dsound.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dsoundcard.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib dsound.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10100000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\dsoundcard.pdb" /machine:I386 /def:".\dsoundcard.def" /out:"dsoundcard.pmo" /implib:"$(OUTDIR)\dsoundcard.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\dsoundcard.def"
LINK32_OBJS= \
	"$(INTDIR)\dsoundcardpmo.obj" \
	"$(INTDIR)\eventbuffer.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmo.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\dsound.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\dsoundcard.pmo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Release" "..\..\..\..\config\config.h" ".\dsoundcard.pmo"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                    ..\..\..\..\base\win32\prj\plugins
	copy dsoundcard.pmo                               ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "dsoundcard - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\dsoundcard.pmo"

!ELSE 

ALL : "fabaselib - Win32 Debug" "..\..\..\..\config\config.h" ".\dsoundcard.pmo"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dsound.res"
	-@erase "$(INTDIR)\dsoundcardpmo.obj"
	-@erase "$(INTDIR)\eventbuffer.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmo.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dsoundcard.exp"
	-@erase "$(OUTDIR)\dsoundcard.lib"
	-@erase "$(OUTDIR)\dsoundcard.pdb"
	-@erase ".\dsoundcard.ilk"
	-@erase ".\dsoundcard.pmo"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /Od /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\dsoundcard.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL" 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dsound.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dsoundcard.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib dsound.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10100000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\dsoundcard.pdb" /debug /machine:I386 /def:".\dsoundcard.def" /out:"dsoundcard.pmo" /implib:"$(OUTDIR)\dsoundcard.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\dsoundcard.def"
LINK32_OBJS= \
	"$(INTDIR)\dsoundcardpmo.obj" \
	"$(INTDIR)\eventbuffer.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmo.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\dsound.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\dsoundcard.pmo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Debug" "..\..\..\..\config\config.h" ".\dsoundcard.pmo"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                        ..\..\..\..\base\win32\prj\plugins
	copy dsoundcard.pmo               ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "dsoundcard - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\dsoundcard.pmo"

!ELSE 

ALL : "fabaselib - Win32 NASM Debug" "..\..\..\..\config\config.h" ".\dsoundcard.pmo"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dsound.res"
	-@erase "$(INTDIR)\dsoundcardpmo.obj"
	-@erase "$(INTDIR)\eventbuffer.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmo.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dsoundcard.exp"
	-@erase "$(OUTDIR)\dsoundcard.lib"
	-@erase "$(OUTDIR)\dsoundcard.pdb"
	-@erase ".\dsoundcard.ilk"
	-@erase ".\dsoundcard.pmo"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /Od /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\dsoundcard.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL" 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dsound.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dsoundcard.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib dsound.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10100000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\dsoundcard.pdb" /debug /machine:I386 /def:".\dsoundcard.def" /out:"dsoundcard.pmo" /implib:"$(OUTDIR)\dsoundcard.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\dsoundcard.def"
LINK32_OBJS= \
	"$(INTDIR)\dsoundcardpmo.obj" \
	"$(INTDIR)\eventbuffer.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmo.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\dsound.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\dsoundcard.pmo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Debug" "..\..\..\..\config\config.h" ".\dsoundcard.pmo"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                        ..\..\..\..\base\win32\prj\plugins
	copy dsoundcard.pmo               ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "dsoundcard - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\dsoundcard.pmo"

!ELSE 

ALL : "fabaselib - Win32 NASM Release" "..\..\..\..\config\config.h" ".\dsoundcard.pmo"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dsound.res"
	-@erase "$(INTDIR)\dsoundcardpmo.obj"
	-@erase "$(INTDIR)\eventbuffer.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmo.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dsoundcard.exp"
	-@erase "$(OUTDIR)\dsoundcard.lib"
	-@erase ".\dsoundcard.pmo"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Ob2 /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\dsoundcard.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL" 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dsound.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dsoundcard.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib dsound.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10100000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\dsoundcard.pdb" /machine:I386 /def:".\dsoundcard.def" /out:"dsoundcard.pmo" /implib:"$(OUTDIR)\dsoundcard.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\dsoundcard.def"
LINK32_OBJS= \
	"$(INTDIR)\dsoundcardpmo.obj" \
	"$(INTDIR)\eventbuffer.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmo.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\dsound.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\dsoundcard.pmo" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Release" "..\..\..\..\config\config.h" ".\dsoundcard.pmo"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                    ..\..\..\..\base\win32\prj\plugins
	copy dsoundcard.pmo                               ..\..\..\..\base\win32\prj\plugins
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
!IF EXISTS("dsoundcard.dep")
!INCLUDE "dsoundcard.dep"
!ELSE 
!MESSAGE Warning: cannot find "dsoundcard.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dsoundcard - Win32 Release" || "$(CFG)" == "dsoundcard - Win32 Debug" || "$(CFG)" == "dsoundcard - Win32 NASM Debug" || "$(CFG)" == "dsoundcard - Win32 NASM Release"

!IF  "$(CFG)" == "dsoundcard - Win32 Release"

"fabaselib - Win32 Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" 
   cd "..\..\..\io\dsound\win32\prj"

"fabaselib - Win32 ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\io\dsound\win32\prj"

!ELSEIF  "$(CFG)" == "dsoundcard - Win32 Debug"

"fabaselib - Win32 Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" 
   cd "..\..\..\io\dsound\win32\prj"

"fabaselib - Win32 DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\io\dsound\win32\prj"

!ELSEIF  "$(CFG)" == "dsoundcard - Win32 NASM Debug"

"fabaselib - Win32 NASM Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" 
   cd "..\..\..\io\dsound\win32\prj"

"fabaselib - Win32 NASM DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" RECURSE=1 CLEAN 
   cd "..\..\..\io\dsound\win32\prj"

!ELSEIF  "$(CFG)" == "dsoundcard - Win32 NASM Release"

"fabaselib - Win32 NASM Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" 
   cd "..\..\..\io\dsound\win32\prj"

"fabaselib - Win32 NASM ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" RECURSE=1 CLEAN 
   cd "..\..\..\io\dsound\win32\prj"

!ENDIF 

SOURCE=..\..\..\..\config\config.win32

!IF  "$(CFG)" == "dsoundcard - Win32 Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "dsoundcard - Win32 Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "dsoundcard - Win32 NASM Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "dsoundcard - Win32 NASM Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ENDIF 

SOURCE=..\res\dsound.rc

!IF  "$(CFG)" == "dsoundcard - Win32 Release"


"$(INTDIR)\dsound.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\dsound.res" /i "\Local\src\freeamp\io\dsound\win32\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "dsoundcard - Win32 Debug"


"$(INTDIR)\dsound.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\dsound.res" /i "\Local\src\freeamp\io\dsound\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "dsoundcard - Win32 NASM Debug"


"$(INTDIR)\dsound.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\dsound.res" /i "\Local\src\freeamp\io\dsound\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "dsoundcard - Win32 NASM Release"


"$(INTDIR)\dsound.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\dsound.res" /i "\Local\src\freeamp\io\dsound\win32\res" /d "NDEBUG" $(SOURCE)


!ENDIF 

SOURCE=..\src\dsoundcardpmo.cpp

"$(INTDIR)\dsoundcardpmo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


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



!ENDIF 

