# Microsoft Developer Studio Generated NMAKE File, Based on pls.dsp
!IF "$(CFG)" == ""
CFG=pls - Win32 NASM Debug MS STL
!MESSAGE No configuration specified. Defaulting to pls - Win32 NASM Debug MS STL.
!ENDIF 

!IF "$(CFG)" != "pls - Win32 Release" && "$(CFG)" != "pls - Win32 Debug" && "$(CFG)" != "pls - Win32 NASM Release" && "$(CFG)" != "pls - Win32 NASM Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pls.mak" CFG="pls - Win32 NASM Debug MS STL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pls - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pls - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pls - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pls - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "pls - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : ".\pls.plf"


CLEAN :
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\pls.obj"
	-@erase "$(INTDIR)\pls.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\pls.exp"
	-@erase "$(OUTDIR)\pls.lib"
	-@erase ".\pls.plf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\pls.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\pls.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pls.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11100000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\pls.pdb" /machine:I386 /def:".\pls.def" /out:"pls.plf" /implib:"$(OUTDIR)\pls.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\pls.def"
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\pls.obj" \
	"$(INTDIR)\pls.res"

".\pls.plf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : ".\pls.plf"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                 ..\..\..\..\base\win32\prj\plugins
	copy pls.plf        ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "pls - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : ".\pls.plf"


CLEAN :
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\pls.obj"
	-@erase "$(INTDIR)\pls.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\pls.exp"
	-@erase "$(OUTDIR)\pls.lib"
	-@erase "$(OUTDIR)\pls.pdb"
	-@erase ".\pls.ilk"
	-@erase ".\pls.plf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\pls.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\pls.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pls.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11100000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\pls.pdb" /debug /machine:I386 /def:".\pls.def" /out:"pls.plf" /implib:"$(OUTDIR)\pls.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\pls.def"
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\pls.obj" \
	"$(INTDIR)\pls.res"

".\pls.plf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : ".\pls.plf"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                 ..\..\..\..\base\win32\prj\plugins
	copy pls.plf        ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "pls - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : ".\pls.plf"


CLEAN :
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\pls.obj"
	-@erase "$(INTDIR)\pls.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\pls.exp"
	-@erase "$(OUTDIR)\pls.lib"
	-@erase ".\pls.plf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\pls.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\pls.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pls.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11100000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\pls.pdb" /machine:I386 /def:".\pls.def" /out:"pls.plf" /implib:"$(OUTDIR)\pls.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\pls.def"
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\pls.obj" \
	"$(INTDIR)\pls.res"

".\pls.plf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : ".\pls.plf"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                 ..\..\..\..\base\win32\prj\plugins
	copy pls.plf        ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "pls - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : ".\pls.plf"


CLEAN :
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\pls.obj"
	-@erase "$(INTDIR)\pls.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\pls.exp"
	-@erase "$(OUTDIR)\pls.lib"
	-@erase "$(OUTDIR)\pls.pdb"
	-@erase ".\pls.ilk"
	-@erase ".\pls.plf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\pls.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\pls.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pls.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11100000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\pls.pdb" /debug /machine:I386 /def:".\pls.def" /out:"pls.plf" /implib:"$(OUTDIR)\pls.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\pls.def"
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\pls.obj" \
	"$(INTDIR)\pls.res"

".\pls.plf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : ".\pls.plf"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                 ..\..\..\..\base\win32\prj\plugins
	copy pls.plf        ..\..\..\..\base\win32\prj\plugins
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
!IF EXISTS("pls.dep")
!INCLUDE "pls.dep"
!ELSE 
!MESSAGE Warning: cannot find "pls.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "pls - Win32 Release" || "$(CFG)" == "pls - Win32 Debug" || "$(CFG)" == "pls - Win32 NASM Release" || "$(CFG)" == "pls - Win32 NASM Debug"
SOURCE=..\..\..\..\base\src\debug.cpp

"$(INTDIR)\debug.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\pls.cpp

"$(INTDIR)\pls.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\res\pls.rc

!IF  "$(CFG)" == "pls - Win32 Release"


"$(INTDIR)\pls.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\pls.res" /i "\Local\src\freeamp\plm\playlist\pls\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "pls - Win32 Debug"


"$(INTDIR)\pls.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\pls.res" /i "\Local\src\freeamp\plm\playlist\pls\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "pls - Win32 NASM Release"


"$(INTDIR)\pls.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\pls.res" /i "\Local\src\freeamp\plm\playlist\pls\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "pls - Win32 NASM Debug"


"$(INTDIR)\pls.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\pls.res" /i "\Local\src\freeamp\plm\playlist\pls\res" /d "_DEBUG" $(SOURCE)


!ENDIF 


!ENDIF 

