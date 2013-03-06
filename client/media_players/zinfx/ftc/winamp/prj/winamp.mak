# Microsoft Developer Studio Generated NMAKE File, Based on winamp.dsp
!IF "$(CFG)" == ""
CFG=winamp - Win32 NASM Debug MS STL
!MESSAGE No configuration specified. Defaulting to winamp - Win32 NASM Debug MS STL.
!ENDIF 

!IF "$(CFG)" != "winamp - Win32 Release" && "$(CFG)" != "winamp - Win32 Debug" && "$(CFG)" != "winamp - Win32 NASM Release" && "$(CFG)" != "winamp - Win32 NASM Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "winamp.mak" CFG="winamp - Win32 NASM Debug MS STL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "winamp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "winamp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "winamp - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "winamp - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "winamp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\base\win32\prj\plugins\winamp_theme.xml" ".\winamp.ftf"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\winamp.obj"
	-@erase "$(INTDIR)\winamp.res"
	-@erase "$(OUTDIR)\winamp.exp"
	-@erase "$(OUTDIR)\winamp.lib"
	-@erase ".\winamp.ftf"
	-@erase "..\..\..\base\win32\prj\plugins\winamp_theme.xml"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\lmc\include" /I "..\\" /I "..\..\include" /I "..\include" /I "..\..\..\io\include" /I "..\..\..\base\include" /I "..\..\..\base\win32\include" /I "..\..\..\config" /I "..\..\..\ui\include" /I "..\..\..\ui\freeamp\include" /I "..\..\..\lib\xml\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\winamp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\winamp.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\winamp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11300000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\winamp.pdb" /machine:I386 /def:".\winamp.def" /out:"winamp.ftf" /implib:"$(OUTDIR)\winamp.lib" /libpath:"..\..\..\base\win32" 
DEF_FILE= \
	".\winamp.def"
LINK32_OBJS= \
	"$(INTDIR)\winamp.obj" \
	"$(INTDIR)\winamp.res"

".\winamp.ftf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\..\base\win32\prj\plugins\winamp_theme.xml" ".\winamp.ftf"
   IF NOT EXIST ..\..\..\base\win32\prj\plugins mkdir                                  ..\..\..\base\win32\prj\plugins
	copy winamp.ftf  ..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "winamp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\base\win32\prj\plugins\winamp_theme.xml" ".\winamp.ftf"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\winamp.obj"
	-@erase "$(INTDIR)\winamp.res"
	-@erase "$(OUTDIR)\winamp.exp"
	-@erase "$(OUTDIR)\winamp.lib"
	-@erase "$(OUTDIR)\winamp.pdb"
	-@erase ".\winamp.ftf"
	-@erase ".\winamp.ilk"
	-@erase "..\..\..\base\win32\prj\plugins\winamp_theme.xml"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\lmc\include" /I "..\\" /I "..\..\include" /I "..\include" /I "..\..\..\io\include" /I "..\..\..\base\include" /I "..\..\..\base\win32\include" /I "..\..\..\config" /I "..\..\..\ui\include" /I "..\..\..\ui\freeamp\include" /I "..\..\..\lib\xml\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\winamp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\winamp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\winamp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11300000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\winamp.pdb" /debug /machine:I386 /def:".\winamp.def" /out:"winamp.ftf" /implib:"$(OUTDIR)\winamp.lib" /pdbtype:sept /libpath:"..\..\..\base\win32" 
DEF_FILE= \
	".\winamp.def"
LINK32_OBJS= \
	"$(INTDIR)\winamp.obj" \
	"$(INTDIR)\winamp.res"

".\winamp.ftf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\..\base\win32\prj\plugins\winamp_theme.xml" ".\winamp.ftf"
   IF NOT EXIST ..\..\..\base\win32\prj\plugins mkdir                                  ..\..\..\base\win32\prj\plugins
	copy winamp.ftf  ..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "winamp - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\base\win32\prj\plugins\winamp_theme.xml" ".\winamp.ftf"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\winamp.obj"
	-@erase "$(INTDIR)\winamp.res"
	-@erase "$(OUTDIR)\winamp.exp"
	-@erase "$(OUTDIR)\winamp.lib"
	-@erase ".\winamp.ftf"
	-@erase "..\..\..\base\win32\prj\plugins\winamp_theme.xml"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\lmc\include" /I "..\\" /I "..\..\include" /I "..\include" /I "..\..\..\io\include" /I "..\..\..\base\include" /I "..\..\..\base\win32\include" /I "..\..\..\config" /I "..\..\..\ui\include" /I "..\..\..\ui\freeamp\include" /I "..\..\..\lib\xml\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\winamp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\winamp.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\winamp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11300000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\winamp.pdb" /machine:I386 /def:".\winamp.def" /out:"winamp.ftf" /implib:"$(OUTDIR)\winamp.lib" /libpath:"..\..\..\base\win32" 
DEF_FILE= \
	".\winamp.def"
LINK32_OBJS= \
	"$(INTDIR)\winamp.obj" \
	"$(INTDIR)\winamp.res"

".\winamp.ftf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\..\base\win32\prj\plugins\winamp_theme.xml" ".\winamp.ftf"
   IF NOT EXIST ..\..\..\base\win32\prj\plugins mkdir                                  ..\..\..\base\win32\prj\plugins
	copy winamp.ftf  ..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "winamp - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\base\win32\prj\plugins\winamp_theme.xml" ".\winamp.ftf"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\winamp.obj"
	-@erase "$(INTDIR)\winamp.res"
	-@erase "$(OUTDIR)\winamp.exp"
	-@erase "$(OUTDIR)\winamp.lib"
	-@erase "$(OUTDIR)\winamp.pdb"
	-@erase ".\winamp.ftf"
	-@erase ".\winamp.ilk"
	-@erase "..\..\..\base\win32\prj\plugins\winamp_theme.xml"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\include" /I "..\include" /I "..\..\..\io\include" /I "..\..\..\base\include" /I "..\..\..\base\win32\include" /I "..\..\..\config" /I "..\..\..\ui\include" /I "..\..\..\ui\freeamp\include" /I "..\..\..\lib\xml\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\winamp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\winamp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\winamp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11300000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\winamp.pdb" /debug /machine:I386 /def:".\winamp.def" /out:"winamp.ftf" /implib:"$(OUTDIR)\winamp.lib" /pdbtype:sept /libpath:"..\..\..\base\win32" 
DEF_FILE= \
	".\winamp.def"
LINK32_OBJS= \
	"$(INTDIR)\winamp.obj" \
	"$(INTDIR)\winamp.res"

".\winamp.ftf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\..\base\win32\prj\plugins\winamp_theme.xml" ".\winamp.ftf"
   IF NOT EXIST ..\..\..\base\win32\prj\plugins mkdir                                  ..\..\..\base\win32\prj\plugins
	copy winamp.ftf  ..\..\..\base\win32\prj\plugins
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
!IF EXISTS("winamp.dep")
!INCLUDE "winamp.dep"
!ELSE 
!MESSAGE Warning: cannot find "winamp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "winamp - Win32 Release" || "$(CFG)" == "winamp - Win32 Debug" || "$(CFG)" == "winamp - Win32 NASM Release" || "$(CFG)" == "winamp - Win32 NASM Debug"
SOURCE=..\winamp.cpp

"$(INTDIR)\winamp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\res\winamp.rc

!IF  "$(CFG)" == "winamp - Win32 Release"


"$(INTDIR)\winamp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\winamp.res" /i "\Local\src\freeamp\ftc\winamp\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "winamp - Win32 Debug"


"$(INTDIR)\winamp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\winamp.res" /i "\Local\src\freeamp\ftc\winamp\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "winamp - Win32 NASM Release"


"$(INTDIR)\winamp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\winamp.res" /i "\Local\src\freeamp\ftc\winamp\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "winamp - Win32 NASM Debug"


"$(INTDIR)\winamp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\winamp.res" /i "\Local\src\freeamp\ftc\winamp\res" /d "_DEBUG" $(SOURCE)


!ENDIF 

SOURCE=..\winamp_theme.xml

!IF  "$(CFG)" == "winamp - Win32 Release"

InputPath=..\winamp_theme.xml

"..\..\..\base\win32\prj\plugins\winamp_theme.xml" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\winamp_theme.xml ..\..\..\base\win32\prj\plugins
<< 
	

!ELSEIF  "$(CFG)" == "winamp - Win32 Debug"

InputPath=..\winamp_theme.xml

"..\..\..\base\win32\prj\plugins\winamp_theme.xml" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\winamp_theme.xml ..\..\..\base\win32\prj\plugins
<< 
	

!ELSEIF  "$(CFG)" == "winamp - Win32 NASM Release"

InputPath=..\winamp_theme.xml

"..\..\..\base\win32\prj\plugins\winamp_theme.xml" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\winamp_theme.xml ..\..\..\base\win32\prj\plugins
<< 
	

!ELSEIF  "$(CFG)" == "winamp - Win32 NASM Debug"

InputPath=..\winamp_theme.xml

"..\..\..\base\win32\prj\plugins\winamp_theme.xml" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\winamp_theme.xml ..\..\..\base\win32\prj\plugins
<< 
	

!ENDIF 


!ENDIF 

