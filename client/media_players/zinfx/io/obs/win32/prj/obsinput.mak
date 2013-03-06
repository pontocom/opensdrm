# Microsoft Developer Studio Generated NMAKE File, Based on obsinput.dsp
!IF "$(CFG)" == ""
CFG=obsinput - Win32 NASM Debug MS STL
!MESSAGE No configuration specified. Defaulting to obsinput - Win32 NASM Debug MS STL.
!ENDIF 

!IF "$(CFG)" != "obsinput - Win32 Release" && "$(CFG)" != "obsinput - Win32 Debug" && "$(CFG)" != "obsinput - Win32 NASM Debug" && "$(CFG)" != "obsinput - Win32 NASM Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "obsinput.mak" CFG="obsinput - Win32 NASM Debug MS STL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "obsinput - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "obsinput - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "obsinput - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "obsinput - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "obsinput - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\obsinput.pmi"

!ELSE 

ALL : "fabaselib - Win32 Release" "..\..\..\..\config\config.h" ".\obsinput.pmi"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\obs.res"
	-@erase "$(INTDIR)\obsinput.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmi.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\tstream.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\obsinput.exp"
	-@erase "$(OUTDIR)\obsinput.lib"
	-@erase ".\obsinput.pmi"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\obsinput.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\obs.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\obsinput.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11000000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\obsinput.pdb" /machine:I386 /def:".\obsinput.def" /out:"obsinput.pmi" /implib:"$(OUTDIR)\obsinput.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\obsinput.def"
LINK32_OBJS= \
	"$(INTDIR)\obsinput.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmi.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\tstream.obj" \
	"$(INTDIR)\obs.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\obsinput.pmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Release" "..\..\..\..\config\config.h" ".\obsinput.pmi"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                         ..\..\..\..\base\win32\prj\plugins
	copy obsinput.pmi                    ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "obsinput - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\obsinput.pmi"

!ELSE 

ALL : "fabaselib - Win32 Debug" "..\..\..\..\config\config.h" ".\obsinput.pmi"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\obs.res"
	-@erase "$(INTDIR)\obsinput.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmi.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\tstream.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\obsinput.exp"
	-@erase "$(OUTDIR)\obsinput.lib"
	-@erase "$(OUTDIR)\obsinput.pdb"
	-@erase ".\obsinput.ilk"
	-@erase ".\obsinput.pmi"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\obsinput.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\obs.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\obsinput.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11000000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\obsinput.pdb" /debug /machine:I386 /def:".\obsinput.def" /out:"obsinput.pmi" /implib:"$(OUTDIR)\obsinput.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\obsinput.def"
LINK32_OBJS= \
	"$(INTDIR)\obsinput.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmi.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\tstream.obj" \
	"$(INTDIR)\obs.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\obsinput.pmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Debug" "..\..\..\..\config\config.h" ".\obsinput.pmi"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                         ..\..\..\..\base\win32\prj\plugins
	copy obsinput.pmi                    ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "obsinput - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\obsinput.pmi"

!ELSE 

ALL : "fabaselib - Win32 NASM Debug" "..\..\..\..\config\config.h" ".\obsinput.pmi"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\obs.res"
	-@erase "$(INTDIR)\obsinput.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmi.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\tstream.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\obsinput.exp"
	-@erase "$(OUTDIR)\obsinput.lib"
	-@erase "$(OUTDIR)\obsinput.pdb"
	-@erase ".\obsinput.ilk"
	-@erase ".\obsinput.pmi"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\obsinput.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\obs.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\obsinput.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11000000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\obsinput.pdb" /debug /machine:I386 /def:".\obsinput.def" /out:"obsinput.pmi" /implib:"$(OUTDIR)\obsinput.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\obsinput.def"
LINK32_OBJS= \
	"$(INTDIR)\obsinput.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmi.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\tstream.obj" \
	"$(INTDIR)\obs.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\obsinput.pmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Debug" "..\..\..\..\config\config.h" ".\obsinput.pmi"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                         ..\..\..\..\base\win32\prj\plugins
	copy obsinput.pmi                    ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "obsinput - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\obsinput.pmi"

!ELSE 

ALL : "fabaselib - Win32 NASM Release" "..\..\..\..\config\config.h" ".\obsinput.pmi"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\obs.res"
	-@erase "$(INTDIR)\obsinput.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pmi.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\tstream.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\obsinput.exp"
	-@erase "$(OUTDIR)\obsinput.lib"
	-@erase ".\obsinput.pmi"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\obsinput.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\obs.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\obsinput.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11000000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\obsinput.pdb" /machine:I386 /def:".\obsinput.def" /out:"obsinput.pmi" /implib:"$(OUTDIR)\obsinput.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\obsinput.def"
LINK32_OBJS= \
	"$(INTDIR)\obsinput.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pmi.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\tstream.obj" \
	"$(INTDIR)\obs.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\obsinput.pmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Release" "..\..\..\..\config\config.h" ".\obsinput.pmi"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                         ..\..\..\..\base\win32\prj\plugins
	copy obsinput.pmi                    ..\..\..\..\base\win32\prj\plugins
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
!IF EXISTS("obsinput.dep")
!INCLUDE "obsinput.dep"
!ELSE 
!MESSAGE Warning: cannot find "obsinput.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "obsinput - Win32 Release" || "$(CFG)" == "obsinput - Win32 Debug" || "$(CFG)" == "obsinput - Win32 NASM Debug" || "$(CFG)" == "obsinput - Win32 NASM Release"

!IF  "$(CFG)" == "obsinput - Win32 Release"

"fabaselib - Win32 Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" 
   cd "..\..\..\io\obs\win32\prj"

"fabaselib - Win32 ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\io\obs\win32\prj"

!ELSEIF  "$(CFG)" == "obsinput - Win32 Debug"

"fabaselib - Win32 Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" 
   cd "..\..\..\io\obs\win32\prj"

"fabaselib - Win32 DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\io\obs\win32\prj"

!ELSEIF  "$(CFG)" == "obsinput - Win32 NASM Debug"

"fabaselib - Win32 NASM Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" 
   cd "..\..\..\io\obs\win32\prj"

"fabaselib - Win32 NASM DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" RECURSE=1 CLEAN 
   cd "..\..\..\io\obs\win32\prj"

!ELSEIF  "$(CFG)" == "obsinput - Win32 NASM Release"

"fabaselib - Win32 NASM Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" 
   cd "..\..\..\io\obs\win32\prj"

"fabaselib - Win32 NASM ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" RECURSE=1 CLEAN 
   cd "..\..\..\io\obs\win32\prj"

!ENDIF 

SOURCE=..\..\..\..\config\config.win32

!IF  "$(CFG)" == "obsinput - Win32 Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "obsinput - Win32 Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "obsinput - Win32 NASM Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "obsinput - Win32 NASM Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ENDIF 

SOURCE=..\res\obs.rc

!IF  "$(CFG)" == "obsinput - Win32 Release"


"$(INTDIR)\obs.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\obs.res" /i "\Local\src\freeamp\io\obs\win32\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "obsinput - Win32 Debug"


"$(INTDIR)\obs.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\obs.res" /i "\Local\src\freeamp\io\obs\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "obsinput - Win32 NASM Debug"


"$(INTDIR)\obs.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\obs.res" /i "\Local\src\freeamp\io\obs\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "obsinput - Win32 NASM Release"


"$(INTDIR)\obs.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\obs.res" /i "\Local\src\freeamp\io\obs\win32\res" /d "NDEBUG" $(SOURCE)


!ENDIF 

SOURCE=..\..\obsinput.cpp

"$(INTDIR)\obsinput.obj" : $(SOURCE) "$(INTDIR)"
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


SOURCE=..\..\..\src\tstream.cpp

"$(INTDIR)\tstream.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

