# Microsoft Developer Studio Generated NMAKE File, Based on rmp.dsp
!IF "$(CFG)" == ""
CFG=rmp - Win32 NASM Debug MS STL
!MESSAGE No configuration specified. Defaulting to rmp - Win32 NASM Debug MS STL.
!ENDIF 

!IF "$(CFG)" != "rmp - Win32 Release" && "$(CFG)" != "rmp - Win32 Debug" && "$(CFG)" != "rmp - Win32 NASM Release" && "$(CFG)" != "rmp - Win32 NASM Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rmp.mak" CFG="rmp - Win32 NASM Debug MS STL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rmp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "rmp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "rmp - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "rmp - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "rmp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : ".\rmp.dlf"

!ELSE 

ALL : "fabaselib - Win32 Release" ".\rmp.dlf"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\Parse.obj"
	-@erase "$(INTDIR)\rmp.obj"
	-@erase "$(INTDIR)\rmp.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\rmp.exp"
	-@erase "$(OUTDIR)\rmp.lib"
	-@erase ".\rmp.dlf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\lmc\include" /I "..\\" /I "..\..\include" /I "..\include" /I "..\..\..\io\include" /I "..\..\..\base\include" /I "..\..\..\base\win32\include" /I "..\..\..\config" /I "..\..\..\ui\include" /I "..\..\..\lib\xml\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\rmp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\rmp.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\rmp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11300000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\rmp.pdb" /machine:I386 /def:".\rmp.def" /out:"rmp.dlf" /implib:"$(OUTDIR)\rmp.lib" /libpath:"..\..\..\base\win32" 
DEF_FILE= \
	".\rmp.def"
LINK32_OBJS= \
	"$(INTDIR)\Parse.obj" \
	"$(INTDIR)\rmp.obj" \
	"$(INTDIR)\rmp.res" \
	"..\..\..\base\win32\fabaselib.lib"

".\rmp.dlf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Release" ".\rmp.dlf"
   IF NOT EXIST ..\..\..\base\win32\prj\plugins mkdir                                  ..\..\..\base\win32\prj\plugins
	copy rmp.dlf  ..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "rmp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : ".\rmp.dlf"

!ELSE 

ALL : "fabaselib - Win32 Debug" ".\rmp.dlf"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\Parse.obj"
	-@erase "$(INTDIR)\rmp.obj"
	-@erase "$(INTDIR)\rmp.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\rmp.exp"
	-@erase "$(OUTDIR)\rmp.lib"
	-@erase "$(OUTDIR)\rmp.pdb"
	-@erase ".\rmp.dlf"
	-@erase ".\rmp.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\lmc\include" /I "..\\" /I "..\..\include" /I "..\include" /I "..\..\..\io\include" /I "..\..\..\base\include" /I "..\..\..\base\win32\include" /I "..\..\..\config" /I "..\..\..\ui\include" /I "..\..\..\lib\xml\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\rmp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\rmp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\rmp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11300000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\rmp.pdb" /debug /machine:I386 /def:".\rmp.def" /out:"rmp.dlf" /implib:"$(OUTDIR)\rmp.lib" /pdbtype:sept /libpath:"..\..\..\base\win32" 
DEF_FILE= \
	".\rmp.def"
LINK32_OBJS= \
	"$(INTDIR)\Parse.obj" \
	"$(INTDIR)\rmp.obj" \
	"$(INTDIR)\rmp.res" \
	"..\..\..\base\win32\fabaselib.lib"

".\rmp.dlf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Debug" ".\rmp.dlf"
   IF NOT EXIST ..\..\..\base\win32\prj\plugins mkdir                                  ..\..\..\base\win32\prj\plugins
	copy rmp.dlf  ..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "rmp - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : ".\rmp.dlf"

!ELSE 

ALL : "fabaselib - Win32 NASM Release" ".\rmp.dlf"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\Parse.obj"
	-@erase "$(INTDIR)\rmp.obj"
	-@erase "$(INTDIR)\rmp.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\rmp.exp"
	-@erase "$(OUTDIR)\rmp.lib"
	-@erase ".\rmp.dlf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\lmc\include" /I "..\\" /I "..\..\include" /I "..\include" /I "..\..\..\io\include" /I "..\..\..\base\include" /I "..\..\..\base\win32\include" /I "..\..\..\config" /I "..\..\..\ui\include" /I "..\..\..\lib\xml\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\rmp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\rmp.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\rmp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11300000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\rmp.pdb" /machine:I386 /def:".\rmp.def" /out:"rmp.dlf" /implib:"$(OUTDIR)\rmp.lib" /libpath:"..\..\..\base\win32" 
DEF_FILE= \
	".\rmp.def"
LINK32_OBJS= \
	"$(INTDIR)\Parse.obj" \
	"$(INTDIR)\rmp.obj" \
	"$(INTDIR)\rmp.res" \
	"..\..\..\base\win32\fabaselib.lib"

".\rmp.dlf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Release" ".\rmp.dlf"
   IF NOT EXIST ..\..\..\base\win32\prj\plugins mkdir                                  ..\..\..\base\win32\prj\plugins
	copy rmp.dlf  ..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "rmp - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : ".\rmp.dlf"

!ELSE 

ALL : "fabaselib - Win32 NASM Debug" ".\rmp.dlf"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\Parse.obj"
	-@erase "$(INTDIR)\rmp.obj"
	-@erase "$(INTDIR)\rmp.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\rmp.exp"
	-@erase "$(OUTDIR)\rmp.lib"
	-@erase "$(OUTDIR)\rmp.pdb"
	-@erase ".\rmp.dlf"
	-@erase ".\rmp.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include" /I "..\include" /I "..\..\..\io\include" /I "..\..\..\base\include" /I "..\..\..\base\win32\include" /I "..\..\..\config" /I "..\..\..\ui\include" /I "..\..\..\lib\xml\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\rmp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\rmp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\rmp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11300000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\rmp.pdb" /debug /machine:I386 /def:".\rmp.def" /out:"rmp.dlf" /implib:"$(OUTDIR)\rmp.lib" /pdbtype:sept /libpath:"..\..\..\base\win32" 
DEF_FILE= \
	".\rmp.def"
LINK32_OBJS= \
	"$(INTDIR)\Parse.obj" \
	"$(INTDIR)\rmp.obj" \
	"$(INTDIR)\rmp.res" \
	"..\..\..\base\win32\fabaselib.lib"

".\rmp.dlf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Debug" ".\rmp.dlf"
   IF NOT EXIST ..\..\..\base\win32\prj\plugins mkdir                                  ..\..\..\base\win32\prj\plugins
	copy rmp.dlf  ..\..\..\base\win32\prj\plugins
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
!IF EXISTS("rmp.dep")
!INCLUDE "rmp.dep"
!ELSE 
!MESSAGE Warning: cannot find "rmp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "rmp - Win32 Release" || "$(CFG)" == "rmp - Win32 Debug" || "$(CFG)" == "rmp - Win32 NASM Release" || "$(CFG)" == "rmp - Win32 NASM Debug"
SOURCE=..\..\..\lib\xml\src\Parse.cpp

"$(INTDIR)\Parse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\rmp.cpp

"$(INTDIR)\rmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\res\rmp.rc

!IF  "$(CFG)" == "rmp - Win32 Release"


"$(INTDIR)\rmp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\rmp.res" /i "\Local\src\freeamp\dlm\rmp\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "rmp - Win32 Debug"


"$(INTDIR)\rmp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\rmp.res" /i "\Local\src\freeamp\dlm\rmp\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "rmp - Win32 NASM Release"


"$(INTDIR)\rmp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\rmp.res" /i "\Local\src\freeamp\dlm\rmp\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "rmp - Win32 NASM Debug"


"$(INTDIR)\rmp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\rmp.res" /i "\Local\src\freeamp\dlm\rmp\res" /d "_DEBUG" $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "rmp - Win32 Release"

"fabaselib - Win32 Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" 
   cd "..\..\..\dlm\rmp\prj"

"fabaselib - Win32 ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\dlm\rmp\prj"

!ELSEIF  "$(CFG)" == "rmp - Win32 Debug"

"fabaselib - Win32 Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" 
   cd "..\..\..\dlm\rmp\prj"

"fabaselib - Win32 DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\dlm\rmp\prj"

!ELSEIF  "$(CFG)" == "rmp - Win32 NASM Release"

"fabaselib - Win32 NASM Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" 
   cd "..\..\..\dlm\rmp\prj"

"fabaselib - Win32 NASM ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" RECURSE=1 CLEAN 
   cd "..\..\..\dlm\rmp\prj"

!ELSEIF  "$(CFG)" == "rmp - Win32 NASM Debug"

"fabaselib - Win32 NASM Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" 
   cd "..\..\..\dlm\rmp\prj"

"fabaselib - Win32 NASM DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" RECURSE=1 CLEAN 
   cd "..\..\..\dlm\rmp\prj"

!ENDIF 


!ENDIF 

