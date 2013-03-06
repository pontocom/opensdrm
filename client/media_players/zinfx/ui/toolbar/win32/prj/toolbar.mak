# Microsoft Developer Studio Generated NMAKE File, Based on toolbar.dsp
!IF "$(CFG)" == ""
CFG=toolbar - Win32 NASM Debug MS STL
!MESSAGE No configuration specified. Defaulting to toolbar - Win32 NASM Debug MS STL.
!ENDIF 

!IF "$(CFG)" != "toolbar - Win32 Release" && "$(CFG)" != "toolbar - Win32 Debug" && "$(CFG)" != "toolbar - Win32 NASM Debug" && "$(CFG)" != "toolbar - Win32 NASM Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "toolbar.mak" CFG="toolbar - Win32 NASM Debug MS STL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "toolbar - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "toolbar - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "toolbar - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "toolbar - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "toolbar - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : ".\toolbar.ui"

!ELSE 

ALL : "fabaselib - Win32 Release" ".\toolbar.ui"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\Toolbar.obj"
	-@erase "$(INTDIR)\toolbar.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\toolbar.exp"
	-@erase "$(OUTDIR)\toolbar.lib"
	-@erase ".\toolbar.ui"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lib\xml\include" /I "..\res" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\toolbar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\toolbar.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\toolbar.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11600000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\toolbar.pdb" /machine:I386 /def:".\toolbar.def" /out:"toolbar.ui" /implib:"$(OUTDIR)\toolbar.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\toolbar.def"
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\Toolbar.obj" \
	"$(INTDIR)\toolbar.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\toolbar.ui" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Release" ".\toolbar.ui"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                        ..\..\..\..\base\win32\prj\plugins
	copy toolbar.ui                                          ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "toolbar - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : ".\toolbar.ui"

!ELSE 

ALL : "fabaselib - Win32 Debug" ".\toolbar.ui"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\Toolbar.obj"
	-@erase "$(INTDIR)\toolbar.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\toolbar.exp"
	-@erase "$(OUTDIR)\toolbar.lib"
	-@erase "$(OUTDIR)\toolbar.pdb"
	-@erase ".\toolbar.ilk"
	-@erase ".\toolbar.ui"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lib\xml\include" /I "..\res" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\toolbar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\toolbar.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\toolbar.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11600000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\toolbar.pdb" /debug /machine:I386 /def:".\toolbar.def" /out:"toolbar.ui" /implib:"$(OUTDIR)\toolbar.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\toolbar.def"
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\Toolbar.obj" \
	"$(INTDIR)\toolbar.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\toolbar.ui" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Debug" ".\toolbar.ui"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                        ..\..\..\..\base\win32\prj\plugins
	copy toolbar.ui                                          ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "toolbar - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : ".\toolbar.ui"

!ELSE 

ALL : "fabaselib - Win32 NASM Debug" ".\toolbar.ui"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\Toolbar.obj"
	-@erase "$(INTDIR)\toolbar.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\toolbar.exp"
	-@erase "$(OUTDIR)\toolbar.lib"
	-@erase "$(OUTDIR)\toolbar.pdb"
	-@erase ".\toolbar.ilk"
	-@erase ".\toolbar.ui"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lib\xml\include" /I "..\res" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\toolbar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\toolbar.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\toolbar.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11600000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\toolbar.pdb" /debug /machine:I386 /def:".\toolbar.def" /out:"toolbar.ui" /implib:"$(OUTDIR)\toolbar.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\toolbar.def"
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\Toolbar.obj" \
	"$(INTDIR)\toolbar.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\toolbar.ui" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Debug" ".\toolbar.ui"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                        ..\..\..\..\base\win32\prj\plugins
	copy toolbar.ui                                          ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "toolbar - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : ".\toolbar.ui"

!ELSE 

ALL : "fabaselib - Win32 NASM Release" ".\toolbar.ui"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\Toolbar.obj"
	-@erase "$(INTDIR)\toolbar.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\toolbar.exp"
	-@erase "$(OUTDIR)\toolbar.lib"
	-@erase ".\toolbar.ui"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lib\xml\include" /I "..\res" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\toolbar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\toolbar.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\toolbar.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11600000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\toolbar.pdb" /machine:I386 /def:".\toolbar.def" /out:"toolbar.ui" /implib:"$(OUTDIR)\toolbar.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\toolbar.def"
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\Toolbar.obj" \
	"$(INTDIR)\toolbar.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\toolbar.ui" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Release" ".\toolbar.ui"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                        ..\..\..\..\base\win32\prj\plugins
	copy toolbar.ui                                          ..\..\..\..\base\win32\prj\plugins
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
!IF EXISTS("toolbar.dep")
!INCLUDE "toolbar.dep"
!ELSE 
!MESSAGE Warning: cannot find "toolbar.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "toolbar - Win32 Release" || "$(CFG)" == "toolbar - Win32 Debug" || "$(CFG)" == "toolbar - Win32 NASM Debug" || "$(CFG)" == "toolbar - Win32 NASM Release"
SOURCE=..\..\..\..\base\src\debug.cpp

"$(INTDIR)\debug.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Toolbar.cpp

"$(INTDIR)\Toolbar.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\toolbar.rc

!IF  "$(CFG)" == "toolbar - Win32 Release"


"$(INTDIR)\toolbar.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\toolbar.res" /i "\Local\src\freeamp\ui\toolbar\win32" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "toolbar - Win32 Debug"


"$(INTDIR)\toolbar.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\toolbar.res" /i "\Local\src\freeamp\ui\toolbar\win32" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "toolbar - Win32 NASM Debug"


"$(INTDIR)\toolbar.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\toolbar.res" /i "\Local\src\freeamp\ui\toolbar\win32" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "toolbar - Win32 NASM Release"


"$(INTDIR)\toolbar.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\toolbar.res" /i "\Local\src\freeamp\ui\toolbar\win32" /d "NDEBUG" $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "toolbar - Win32 Release"

"fabaselib - Win32 Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" 
   cd "..\..\..\ui\toolbar\win32\prj"

"fabaselib - Win32 ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\ui\toolbar\win32\prj"

!ELSEIF  "$(CFG)" == "toolbar - Win32 Debug"

"fabaselib - Win32 Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" 
   cd "..\..\..\ui\toolbar\win32\prj"

"fabaselib - Win32 DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\ui\toolbar\win32\prj"

!ELSEIF  "$(CFG)" == "toolbar - Win32 NASM Debug"

"fabaselib - Win32 NASM Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" 
   cd "..\..\..\ui\toolbar\win32\prj"

"fabaselib - Win32 NASM DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" RECURSE=1 CLEAN 
   cd "..\..\..\ui\toolbar\win32\prj"

!ELSEIF  "$(CFG)" == "toolbar - Win32 NASM Release"

"fabaselib - Win32 NASM Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" 
   cd "..\..\..\ui\toolbar\win32\prj"

"fabaselib - Win32 NASM ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" RECURSE=1 CLEAN 
   cd "..\..\..\ui\toolbar\win32\prj"

!ENDIF 


!ENDIF 

