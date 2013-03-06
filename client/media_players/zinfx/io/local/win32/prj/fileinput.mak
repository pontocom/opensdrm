# Microsoft Developer Studio Generated NMAKE File, Based on fileinput.dsp
!IF "$(CFG)" == ""
CFG=fileinput - Win32 Release
!MESSAGE No configuration specified. Defaulting to fileinput - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "fileinput - Win32 Release" && "$(CFG)" != "fileinput - Win32 Debug" && "$(CFG)" != "fileinput - Win32 NASM Debug" && "$(CFG)" != "fileinput - Win32 NASM Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fileinput.mak" CFG="fileinput - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fileinput - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fileinput - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fileinput - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fileinput - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "fileinput - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\fileinput.pmi"

!ELSE 

ALL : "fabaselib - Win32 Release" "..\..\..\..\config\config.h" ".\fileinput.pmi"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\local.res"
	-@erase "$(INTDIR)\localfileinput.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmi.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\fileinput.exp"
	-@erase "$(OUTDIR)\fileinput.lib"
	-@erase ".\fileinput.pmi"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\fileinput.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\local.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\fileinput.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /base:"0x11900000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\fileinput.pdb" /machine:I386 /def:".\fileinput.def" /out:"fileinput.pmi" /implib:"$(OUTDIR)\fileinput.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\fileinput.def"
LINK32_OBJS= \
	"$(INTDIR)\localfileinput.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmi.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\local.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\fileinput.pmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Release" "..\..\..\..\config\config.h" ".\fileinput.pmi"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                          ..\..\..\..\base\win32\prj\plugins
	copy fileinput.pmi                     ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "fileinput - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\fileinput.pmi"

!ELSE 

ALL : "fabaselib - Win32 Debug" "..\..\..\..\config\config.h" ".\fileinput.pmi"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\local.res"
	-@erase "$(INTDIR)\localfileinput.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmi.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\fileinput.exp"
	-@erase "$(OUTDIR)\fileinput.lib"
	-@erase "$(OUTDIR)\fileinput.pdb"
	-@erase ".\fileinput.ilk"
	-@erase ".\fileinput.pmi"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\fileinput.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\local.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\fileinput.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11900000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\fileinput.pdb" /debug /machine:I386 /def:".\fileinput.def" /out:"fileinput.pmi" /implib:"$(OUTDIR)\fileinput.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\fileinput.def"
LINK32_OBJS= \
	"$(INTDIR)\localfileinput.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmi.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\local.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\fileinput.pmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Debug" "..\..\..\..\config\config.h" ".\fileinput.pmi"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                          ..\..\..\..\base\win32\prj\plugins
	copy fileinput.pmi                     ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "fileinput - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\fileinput.pmi"

!ELSE 

ALL : "fabaselib - Win32 NASM Debug" "..\..\..\..\config\config.h" ".\fileinput.pmi"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\local.res"
	-@erase "$(INTDIR)\localfileinput.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmi.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\fileinput.exp"
	-@erase "$(OUTDIR)\fileinput.lib"
	-@erase "$(OUTDIR)\fileinput.pdb"
	-@erase ".\fileinput.ilk"
	-@erase ".\fileinput.pmi"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\fileinput.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\local.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\fileinput.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11900000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\fileinput.pdb" /debug /machine:I386 /def:".\fileinput.def" /out:"fileinput.pmi" /implib:"$(OUTDIR)\fileinput.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\fileinput.def"
LINK32_OBJS= \
	"$(INTDIR)\localfileinput.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmi.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\local.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\fileinput.pmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Debug" "..\..\..\..\config\config.h" ".\fileinput.pmi"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                          ..\..\..\..\base\win32\prj\plugins
	copy fileinput.pmi                     ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "fileinput - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\fileinput.pmi"

!ELSE 

ALL : "fabaselib - Win32 NASM Release" "..\..\..\..\config\config.h" ".\fileinput.pmi"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\local.res"
	-@erase "$(INTDIR)\localfileinput.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmi.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\fileinput.exp"
	-@erase "$(OUTDIR)\fileinput.lib"
	-@erase ".\fileinput.pmi"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\fileinput.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\local.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\fileinput.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11900000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\fileinput.pdb" /machine:I386 /def:".\fileinput.def" /out:"fileinput.pmi" /implib:"$(OUTDIR)\fileinput.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\fileinput.def"
LINK32_OBJS= \
	"$(INTDIR)\localfileinput.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmi.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\local.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\fileinput.pmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Release" "..\..\..\..\config\config.h" ".\fileinput.pmi"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                          ..\..\..\..\base\win32\prj\plugins
	copy fileinput.pmi                     ..\..\..\..\base\win32\prj\plugins
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
!IF EXISTS("fileinput.dep")
!INCLUDE "fileinput.dep"
!ELSE 
!MESSAGE Warning: cannot find "fileinput.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "fileinput - Win32 Release" || "$(CFG)" == "fileinput - Win32 Debug" || "$(CFG)" == "fileinput - Win32 NASM Debug" || "$(CFG)" == "fileinput - Win32 NASM Release"

!IF  "$(CFG)" == "fileinput - Win32 Release"

"fabaselib - Win32 Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" 
   cd "..\..\..\io\local\win32\prj"

"fabaselib - Win32 ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\io\local\win32\prj"

!ELSEIF  "$(CFG)" == "fileinput - Win32 Debug"

"fabaselib - Win32 Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" 
   cd "..\..\..\io\local\win32\prj"

"fabaselib - Win32 DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\io\local\win32\prj"

!ELSEIF  "$(CFG)" == "fileinput - Win32 NASM Debug"

"fabaselib - Win32 NASM Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" 
   cd "..\..\..\io\local\win32\prj"

"fabaselib - Win32 NASM DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" RECURSE=1 CLEAN 
   cd "..\..\..\io\local\win32\prj"

!ELSEIF  "$(CFG)" == "fileinput - Win32 NASM Release"

"fabaselib - Win32 NASM Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" 
   cd "..\..\..\io\local\win32\prj"

"fabaselib - Win32 NASM ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" RECURSE=1 CLEAN 
   cd "..\..\..\io\local\win32\prj"

!ENDIF 

SOURCE=..\..\..\..\config\config.win32

!IF  "$(CFG)" == "fileinput - Win32 Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "fileinput - Win32 Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "fileinput - Win32 NASM Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "fileinput - Win32 NASM Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ENDIF 

SOURCE=..\res\local.rc

!IF  "$(CFG)" == "fileinput - Win32 Release"


"$(INTDIR)\local.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\local.res" /i "\Local\src\freeamp\io\local\win32\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "fileinput - Win32 Debug"


"$(INTDIR)\local.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\local.res" /i "\Local\src\freeamp\io\local\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "fileinput - Win32 NASM Debug"


"$(INTDIR)\local.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\local.res" /i "\Local\src\freeamp\io\local\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "fileinput - Win32 NASM Release"


"$(INTDIR)\local.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\local.res" /i "\Local\src\freeamp\io\local\win32\res" /d "NDEBUG" $(SOURCE)


!ENDIF 

SOURCE=..\..\localfileinput.cpp

"$(INTDIR)\localfileinput.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\src\pipeline.cpp

"$(INTDIR)\pipeline.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\src\pmi.cpp

"$(INTDIR)\pmi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\src\pullbuffer.cpp

"$(INTDIR)\pullbuffer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

