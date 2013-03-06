# Microsoft Developer Studio Generated NMAKE File, Based on simple.dsp
!IF "$(CFG)" == ""
CFG=simple - Win32 NASM Debug MS STL
!MESSAGE No configuration specified. Defaulting to simple - Win32 NASM Debug MS STL.
!ENDIF 

!IF "$(CFG)" != "simple - Win32 Release" && "$(CFG)" != "simple - Win32 Debug" && "$(CFG)" != "simple - Win32 NASM Debug" && "$(CFG)" != "simple - Win32 NASM Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "simple.mak" CFG="simple - Win32 NASM Debug MS STL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "simple - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "simple - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "simple - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "simple - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "simple - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\simple.ui"

!ELSE 

ALL : "xing - Win32 Release" "soundcard - Win32 Release" "fileinput - Win32 Release" "..\..\..\..\config\config.h" ".\simple.ui"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fileinput - Win32 ReleaseCLEAN" "soundcard - Win32 ReleaseCLEAN" "xing - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\about.obj"
	-@erase "$(INTDIR)\mutex.obj"
	-@erase "$(INTDIR)\playlist.obj"
	-@erase "$(INTDIR)\prefdialog.obj"
	-@erase "$(INTDIR)\preferences.obj"
	-@erase "$(INTDIR)\registrar.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\semaphore.obj"
	-@erase "$(INTDIR)\simple.res"
	-@erase "$(INTDIR)\simpleui.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\win32prefs.obj"
	-@erase "$(INTDIR)\win32thread.obj"
	-@erase "$(OUTDIR)\simple.exp"
	-@erase "$(OUTDIR)\simple.lib"
	-@erase ".\simple.ui"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\res" /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /I "..\..\..\..\lib\gdbm" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\simple.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\simple.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\simple.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=gdbm.lib fabaselib.lib version.lib wsock32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11400000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\simple.pdb" /machine:I386 /def:".\simpleui.def" /out:"simple.ui" /implib:"$(OUTDIR)\simple.lib" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\simpleui.def"
LINK32_OBJS= \
	"$(INTDIR)\about.obj" \
	"$(INTDIR)\mutex.obj" \
	"$(INTDIR)\playlist.obj" \
	"$(INTDIR)\prefdialog.obj" \
	"$(INTDIR)\preferences.obj" \
	"$(INTDIR)\registrar.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\semaphore.obj" \
	"$(INTDIR)\simpleui.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\win32prefs.obj" \
	"$(INTDIR)\win32thread.obj" \
	"$(INTDIR)\simple.res" \
	"..\..\..\..\io\local\win32\prj\Release\fileinput.lib" \
	"..\..\..\..\io\soundcard\win32\prj\Release\soundcard.lib" \
	"..\..\..\..\lmc\xingmp3\win32\prj\Release\xing.lib"

".\simple.ui" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "xing - Win32 Release" "soundcard - Win32 Release" "fileinput - Win32 Release" "..\..\..\..\config\config.h" ".\simple.ui"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                               ..\..\..\..\base\win32\prj\plugins
	copy simple.ui                 ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "simple - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\simple.ui"

!ELSE 

ALL : "xing - Win32 Debug" "soundcard - Win32 Debug" "fileinput - Win32 Debug" "..\..\..\..\config\config.h" ".\simple.ui"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fileinput - Win32 DebugCLEAN" "soundcard - Win32 DebugCLEAN" "xing - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\about.obj"
	-@erase "$(INTDIR)\mutex.obj"
	-@erase "$(INTDIR)\playlist.obj"
	-@erase "$(INTDIR)\prefdialog.obj"
	-@erase "$(INTDIR)\preferences.obj"
	-@erase "$(INTDIR)\registrar.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\semaphore.obj"
	-@erase "$(INTDIR)\simple.res"
	-@erase "$(INTDIR)\simpleui.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\win32prefs.obj"
	-@erase "$(INTDIR)\win32thread.obj"
	-@erase "$(OUTDIR)\simple.exp"
	-@erase "$(OUTDIR)\simple.lib"
	-@erase "$(OUTDIR)\simple.pdb"
	-@erase ".\simple.ilk"
	-@erase ".\simple.ui"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\res" /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /I "..\..\..\..\lib\gdbm" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\simple.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\simple.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\simple.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=gdbm.lib fabaselib.lib version.lib wsock32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11400000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\simple.pdb" /debug /machine:I386 /def:".\simpleui.def" /out:"simple.ui" /implib:"$(OUTDIR)\simple.lib" /pdbtype:sept /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\simpleui.def"
LINK32_OBJS= \
	"$(INTDIR)\about.obj" \
	"$(INTDIR)\mutex.obj" \
	"$(INTDIR)\playlist.obj" \
	"$(INTDIR)\prefdialog.obj" \
	"$(INTDIR)\preferences.obj" \
	"$(INTDIR)\registrar.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\semaphore.obj" \
	"$(INTDIR)\simpleui.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\win32prefs.obj" \
	"$(INTDIR)\win32thread.obj" \
	"$(INTDIR)\simple.res" \
	"..\..\..\..\io\local\win32\prj\Debug\fileinput.lib" \
	"..\..\..\..\io\soundcard\win32\prj\Debug\soundcard.lib" \
	"..\..\..\..\lmc\xingmp3\win32\prj\Debug\xing.lib"

".\simple.ui" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "xing - Win32 Debug" "soundcard - Win32 Debug" "fileinput - Win32 Debug" "..\..\..\..\config\config.h" ".\simple.ui"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                               ..\..\..\..\base\win32\prj\plugins
	copy simple.ui                 ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "simple - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\simple.ui"

!ELSE 

ALL : "xing - Win32 NASM Debug" "soundcard - Win32 NASM Debug" "fileinput - Win32 NASM Debug" "..\..\..\..\config\config.h" ".\simple.ui"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fileinput - Win32 NASM DebugCLEAN" "soundcard - Win32 NASM DebugCLEAN" "xing - Win32 NASM DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\about.obj"
	-@erase "$(INTDIR)\mutex.obj"
	-@erase "$(INTDIR)\playlist.obj"
	-@erase "$(INTDIR)\prefdialog.obj"
	-@erase "$(INTDIR)\preferences.obj"
	-@erase "$(INTDIR)\registrar.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\semaphore.obj"
	-@erase "$(INTDIR)\simple.res"
	-@erase "$(INTDIR)\simpleui.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\win32prefs.obj"
	-@erase "$(INTDIR)\win32thread.obj"
	-@erase "$(OUTDIR)\simple.exp"
	-@erase "$(OUTDIR)\simple.lib"
	-@erase "$(OUTDIR)\simple.pdb"
	-@erase ".\simple.ilk"
	-@erase ".\simple.ui"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\res" /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /I "..\..\..\..\lib\gdbm" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\simple.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\simple.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\simple.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=gdbm.lib fabaselib.lib version.lib wsock32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11400000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\simple.pdb" /debug /machine:I386 /def:".\simpleui.def" /out:"simple.ui" /implib:"$(OUTDIR)\simple.lib" /pdbtype:sept /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\simpleui.def"
LINK32_OBJS= \
	"$(INTDIR)\about.obj" \
	"$(INTDIR)\mutex.obj" \
	"$(INTDIR)\playlist.obj" \
	"$(INTDIR)\prefdialog.obj" \
	"$(INTDIR)\preferences.obj" \
	"$(INTDIR)\registrar.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\semaphore.obj" \
	"$(INTDIR)\simpleui.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\win32prefs.obj" \
	"$(INTDIR)\win32thread.obj" \
	"$(INTDIR)\simple.res" \
	"..\..\..\..\io\local\win32\prj\Debug\fileinput.lib" \
	"..\..\..\..\io\soundcard\win32\prj\Debug\soundcard.lib" \
	"..\..\..\..\lmc\xingmp3\win32\prj\Debug\xing.lib"

".\simple.ui" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "xing - Win32 NASM Debug" "soundcard - Win32 NASM Debug" "fileinput - Win32 NASM Debug" "..\..\..\..\config\config.h" ".\simple.ui"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                               ..\..\..\..\base\win32\prj\plugins
	copy simple.ui                 ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "simple - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\simple.ui"

!ELSE 

ALL : "xing - Win32 NASM Release" "soundcard - Win32 NASM Release" "fileinput - Win32 NASM Release" "..\..\..\..\config\config.h" ".\simple.ui"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fileinput - Win32 NASM ReleaseCLEAN" "soundcard - Win32 NASM ReleaseCLEAN" "xing - Win32 NASM ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\about.obj"
	-@erase "$(INTDIR)\mutex.obj"
	-@erase "$(INTDIR)\playlist.obj"
	-@erase "$(INTDIR)\prefdialog.obj"
	-@erase "$(INTDIR)\preferences.obj"
	-@erase "$(INTDIR)\registrar.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\semaphore.obj"
	-@erase "$(INTDIR)\simple.res"
	-@erase "$(INTDIR)\simpleui.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\win32prefs.obj"
	-@erase "$(INTDIR)\win32thread.obj"
	-@erase "$(OUTDIR)\simple.exp"
	-@erase "$(OUTDIR)\simple.lib"
	-@erase ".\simple.ui"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\res" /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /I "..\..\..\..\lib\gdbm" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\simple.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\simple.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\simple.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=gdbm.lib fabaselib.lib version.lib wsock32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11400000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\simple.pdb" /machine:I386 /def:".\simpleui.def" /out:"simple.ui" /implib:"$(OUTDIR)\simple.lib" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\simpleui.def"
LINK32_OBJS= \
	"$(INTDIR)\about.obj" \
	"$(INTDIR)\mutex.obj" \
	"$(INTDIR)\playlist.obj" \
	"$(INTDIR)\prefdialog.obj" \
	"$(INTDIR)\preferences.obj" \
	"$(INTDIR)\registrar.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\semaphore.obj" \
	"$(INTDIR)\simpleui.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\win32prefs.obj" \
	"$(INTDIR)\win32thread.obj" \
	"$(INTDIR)\simple.res" \
	"..\..\..\..\io\local\win32\prj\Release\fileinput.lib" \
	"..\..\..\..\io\soundcard\win32\prj\Release\soundcard.lib" \
	"..\..\..\..\lmc\xingmp3\win32\prj\Release\xing.lib"

".\simple.ui" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "xing - Win32 NASM Release" "soundcard - Win32 NASM Release" "fileinput - Win32 NASM Release" "..\..\..\..\config\config.h" ".\simple.ui"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                               ..\..\..\..\base\win32\prj\plugins
	copy simple.ui                 ..\..\..\..\base\win32\prj\plugins
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
!IF EXISTS("simple.dep")
!INCLUDE "simple.dep"
!ELSE 
!MESSAGE Warning: cannot find "simple.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "simple - Win32 Release" || "$(CFG)" == "simple - Win32 Debug" || "$(CFG)" == "simple - Win32 NASM Debug" || "$(CFG)" == "simple - Win32 NASM Release"
SOURCE=..\src\about.cpp

"$(INTDIR)\about.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\config\config.win32

!IF  "$(CFG)" == "simple - Win32 Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "simple - Win32 Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "simple - Win32 NASM Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "simple - Win32 NASM Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ENDIF 

SOURCE=..\..\..\..\base\win32\src\mutex.cpp

"$(INTDIR)\mutex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\base\src\playlist.cpp

"$(INTDIR)\playlist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\prefdialog.cpp

"$(INTDIR)\prefdialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\base\src\preferences.cpp

"$(INTDIR)\preferences.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\base\src\registrar.cpp

"$(INTDIR)\registrar.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\base\src\registry.cpp

"$(INTDIR)\registry.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\base\win32\src\semaphore.cpp

"$(INTDIR)\semaphore.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\simpleui.cpp

"$(INTDIR)\simpleui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\base\src\thread.cpp

"$(INTDIR)\thread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\base\win32\src\win32prefs.cpp

"$(INTDIR)\win32prefs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\base\win32\src\win32thread.cpp

"$(INTDIR)\win32thread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\res\simple.rc

!IF  "$(CFG)" == "simple - Win32 Release"


"$(INTDIR)\simple.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\simple.res" /i "\Local\src\freeamp\ui\simple\win32\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "simple - Win32 Debug"


"$(INTDIR)\simple.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\simple.res" /i "\Local\src\freeamp\ui\simple\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "simple - Win32 NASM Debug"


"$(INTDIR)\simple.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\simple.res" /i "\Local\src\freeamp\ui\simple\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "simple - Win32 NASM Release"


"$(INTDIR)\simple.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\simple.res" /i "\Local\src\freeamp\ui\simple\win32\res" /d "NDEBUG" $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "simple - Win32 Release"

"fileinput - Win32 Release" : 
   cd "\Local\src\freeamp\io\local\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fileinput.mak CFG="fileinput - Win32 Release" 
   cd "..\..\..\..\ui\simple\win32\prj"

"fileinput - Win32 ReleaseCLEAN" : 
   cd "\Local\src\freeamp\io\local\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fileinput.mak CFG="fileinput - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\ui\simple\win32\prj"

!ELSEIF  "$(CFG)" == "simple - Win32 Debug"

"fileinput - Win32 Debug" : 
   cd "\Local\src\freeamp\io\local\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fileinput.mak CFG="fileinput - Win32 Debug" 
   cd "..\..\..\..\ui\simple\win32\prj"

"fileinput - Win32 DebugCLEAN" : 
   cd "\Local\src\freeamp\io\local\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fileinput.mak CFG="fileinput - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\ui\simple\win32\prj"

!ELSEIF  "$(CFG)" == "simple - Win32 NASM Debug"

"fileinput - Win32 NASM Debug" : 
   cd "\Local\src\freeamp\io\local\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fileinput.mak CFG="fileinput - Win32 NASM Debug" 
   cd "..\..\..\..\ui\simple\win32\prj"

"fileinput - Win32 NASM DebugCLEAN" : 
   cd "\Local\src\freeamp\io\local\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fileinput.mak CFG="fileinput - Win32 NASM Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\ui\simple\win32\prj"

!ELSEIF  "$(CFG)" == "simple - Win32 NASM Release"

"fileinput - Win32 NASM Release" : 
   cd "\Local\src\freeamp\io\local\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fileinput.mak CFG="fileinput - Win32 NASM Release" 
   cd "..\..\..\..\ui\simple\win32\prj"

"fileinput - Win32 NASM ReleaseCLEAN" : 
   cd "\Local\src\freeamp\io\local\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fileinput.mak CFG="fileinput - Win32 NASM Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\ui\simple\win32\prj"

!ENDIF 

!IF  "$(CFG)" == "simple - Win32 Release"

"soundcard - Win32 Release" : 
   cd "\Local\src\freeamp\io\soundcard\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\soundcard.mak CFG="soundcard - Win32 Release" 
   cd "..\..\..\..\ui\simple\win32\prj"

"soundcard - Win32 ReleaseCLEAN" : 
   cd "\Local\src\freeamp\io\soundcard\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\soundcard.mak CFG="soundcard - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\ui\simple\win32\prj"

!ELSEIF  "$(CFG)" == "simple - Win32 Debug"

"soundcard - Win32 Debug" : 
   cd "\Local\src\freeamp\io\soundcard\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\soundcard.mak CFG="soundcard - Win32 Debug" 
   cd "..\..\..\..\ui\simple\win32\prj"

"soundcard - Win32 DebugCLEAN" : 
   cd "\Local\src\freeamp\io\soundcard\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\soundcard.mak CFG="soundcard - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\ui\simple\win32\prj"

!ELSEIF  "$(CFG)" == "simple - Win32 NASM Debug"

"soundcard - Win32 NASM Debug" : 
   cd "\Local\src\freeamp\io\soundcard\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\soundcard.mak CFG="soundcard - Win32 NASM Debug" 
   cd "..\..\..\..\ui\simple\win32\prj"

"soundcard - Win32 NASM DebugCLEAN" : 
   cd "\Local\src\freeamp\io\soundcard\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\soundcard.mak CFG="soundcard - Win32 NASM Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\ui\simple\win32\prj"

!ELSEIF  "$(CFG)" == "simple - Win32 NASM Release"

"soundcard - Win32 NASM Release" : 
   cd "\Local\src\freeamp\io\soundcard\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\soundcard.mak CFG="soundcard - Win32 NASM Release" 
   cd "..\..\..\..\ui\simple\win32\prj"

"soundcard - Win32 NASM ReleaseCLEAN" : 
   cd "\Local\src\freeamp\io\soundcard\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\soundcard.mak CFG="soundcard - Win32 NASM Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\ui\simple\win32\prj"

!ENDIF 

!IF  "$(CFG)" == "simple - Win32 Release"

"xing - Win32 Release" : 
   cd "\Local\src\freeamp\lmc\xingmp3\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\xing.mak CFG="xing - Win32 Release" 
   cd "..\..\..\..\ui\simple\win32\prj"

"xing - Win32 ReleaseCLEAN" : 
   cd "\Local\src\freeamp\lmc\xingmp3\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\xing.mak CFG="xing - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\ui\simple\win32\prj"

!ELSEIF  "$(CFG)" == "simple - Win32 Debug"

"xing - Win32 Debug" : 
   cd "\Local\src\freeamp\lmc\xingmp3\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\xing.mak CFG="xing - Win32 Debug" 
   cd "..\..\..\..\ui\simple\win32\prj"

"xing - Win32 DebugCLEAN" : 
   cd "\Local\src\freeamp\lmc\xingmp3\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\xing.mak CFG="xing - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\ui\simple\win32\prj"

!ELSEIF  "$(CFG)" == "simple - Win32 NASM Debug"

"xing - Win32 NASM Debug" : 
   cd "\Local\src\freeamp\lmc\xingmp3\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\xing.mak CFG="xing - Win32 NASM Debug" 
   cd "..\..\..\..\ui\simple\win32\prj"

"xing - Win32 NASM DebugCLEAN" : 
   cd "\Local\src\freeamp\lmc\xingmp3\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\xing.mak CFG="xing - Win32 NASM Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\ui\simple\win32\prj"

!ELSEIF  "$(CFG)" == "simple - Win32 NASM Release"

"xing - Win32 NASM Release" : 
   cd "\Local\src\freeamp\lmc\xingmp3\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\xing.mak CFG="xing - Win32 NASM Release" 
   cd "..\..\..\..\ui\simple\win32\prj"

"xing - Win32 NASM ReleaseCLEAN" : 
   cd "\Local\src\freeamp\lmc\xingmp3\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\xing.mak CFG="xing - Win32 NASM Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\ui\simple\win32\prj"

!ENDIF 


!ENDIF 

