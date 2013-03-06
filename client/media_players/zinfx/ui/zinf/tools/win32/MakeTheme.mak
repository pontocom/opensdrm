# Microsoft Developer Studio Generated NMAKE File, Based on MakeTheme.dsp
!IF "$(CFG)" == ""
CFG=MakeTheme - Win32 Release
!MESSAGE No configuration specified. Defaulting to MakeTheme - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "MakeTheme - Win32 Release" && "$(CFG)" != "MakeTheme - Win32 Debug" && "$(CFG)" != "MakeTheme - Win32 NASM Debug" && "$(CFG)" != "MakeTheme - Win32 NASM Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MakeTheme.mak" CFG="MakeTheme - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MakeTheme - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "MakeTheme - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "MakeTheme - Win32 NASM Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "MakeTheme - Win32 NASM Release" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MakeTheme - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\MakeTheme.exe"

!ELSE 

ALL : "zlib - Win32 Release" "unzip - Win32 Release" "..\..\..\..\config\config.h" ".\MakeTheme.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"unzip - Win32 ReleaseCLEAN" "zlib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\MakeTheme.obj"
	-@erase "$(INTDIR)\MakeTheme.res"
	-@erase "$(INTDIR)\ThemeZip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\MakeTheme.exe"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "../../include" /I "../../../../lib/zlib/include" /I "../../../../lib/unzip/include" /I "../../../../config" /I "../../../../base/include" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\MakeTheme.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\MakeTheme.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MakeTheme.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=zlib.lib unzip.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\MakeTheme.pdb" /machine:I386 /out:"MakeTheme.exe" /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\lib\unzip" 
LINK32_OBJS= \
	"$(INTDIR)\MakeTheme.obj" \
	"$(INTDIR)\ThemeZip.obj" \
	"$(INTDIR)\MakeTheme.res" \
	"..\..\..\..\lib\unzip\unzip.lib" \
	"..\..\..\..\lib\zlib\zlib.lib"

".\MakeTheme.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "zlib - Win32 Release" "unzip - Win32 Release" "..\..\..\..\config\config.h" ".\MakeTheme.exe"
   IF NOT EXIST ..\..\..\..\base\win32\prj\tools mkdir                                                    ..\..\..\..\base\win32\prj\tools
	copy MakeTheme.exe     ..\..\..\..\base\win32\prj\tools
	copy ..\..\howto\ThemeHowTo.txt     ..\..\..\..\base\win32\prj\tools
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\MakeTheme.exe"

!ELSE 

ALL : "zlib - Win32 Debug" "unzip - Win32 Debug" "..\..\..\..\config\config.h" ".\MakeTheme.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"unzip - Win32 DebugCLEAN" "zlib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\MakeTheme.obj"
	-@erase "$(INTDIR)\MakeTheme.res"
	-@erase "$(INTDIR)\ThemeZip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MakeTheme.pdb"
	-@erase ".\MakeTheme.exe"
	-@erase ".\MakeTheme.ilk"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../../../lib/zlib/include" /I "../../../../lib/unzip/include" /I "../../../../config" /I "../../../../base/include" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\MakeTheme.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\MakeTheme.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MakeTheme.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wsock32.lib zlib.lib unzip.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\MakeTheme.pdb" /debug /machine:I386 /out:"MakeTheme.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\lib\unzip" 
LINK32_OBJS= \
	"$(INTDIR)\MakeTheme.obj" \
	"$(INTDIR)\ThemeZip.obj" \
	"$(INTDIR)\MakeTheme.res" \
	"..\..\..\..\lib\unzip\unzip.lib" \
	"..\..\..\..\lib\zlib\zlib.lib"

".\MakeTheme.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "zlib - Win32 Debug" "unzip - Win32 Debug" "..\..\..\..\config\config.h" ".\MakeTheme.exe"
   IF NOT EXIST ..\..\..\..\base\win32\prj\tools mkdir                                                    ..\..\..\..\base\win32\prj\tools
	copy MakeTheme.exe     ..\..\..\..\base\win32\prj\tools
	copy ..\..\howto\ThemeHowTo.txt     ..\..\..\..\base\win32\prj\tools
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : ".\MakeTheme.exe"

!ELSE 

ALL : "zlib - Win32 NASM Debug" "unzip - Win32 NASM Debug" ".\MakeTheme.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"unzip - Win32 NASM DebugCLEAN" "zlib - Win32 NASM DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\MakeTheme.obj"
	-@erase "$(INTDIR)\MakeTheme.res"
	-@erase "$(INTDIR)\ThemeZip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MakeTheme.pdb"
	-@erase ".\MakeTheme.exe"
	-@erase ".\MakeTheme.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../../../lib/zlib/include" /I "../../../../lib/unzip/include" /I "../../../../config" /I "../../../../base/include" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\MakeTheme.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\MakeTheme.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MakeTheme.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wsock32.lib zlib.lib unzip.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\MakeTheme.pdb" /debug /machine:I386 /out:"MakeTheme.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\lib\unzip" 
LINK32_OBJS= \
	"$(INTDIR)\MakeTheme.obj" \
	"$(INTDIR)\ThemeZip.obj" \
	"$(INTDIR)\MakeTheme.res" \
	"..\..\..\..\lib\unzip\unzip.lib" \
	"..\..\..\..\lib\zlib\zlib.lib"

".\MakeTheme.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "zlib - Win32 NASM Debug" "unzip - Win32 NASM Debug" ".\MakeTheme.exe"
   IF NOT EXIST ..\..\..\..\base\win32\prj\tools mkdir                                                    ..\..\..\..\base\win32\prj\tools
	copy MakeTheme.exe     ..\..\..\..\base\win32\prj\tools
	copy ..\..\howto\ThemeHowTo.txt     ..\..\..\..\base\win32\prj\tools
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 NASM Release"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\MakeTheme.exe"

!ELSE 

ALL : "zlib - Win32 NASM Release" "unzip - Win32 NASM Release" "..\..\..\..\config\config.h" ".\MakeTheme.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"unzip - Win32 NASM ReleaseCLEAN" "zlib - Win32 NASM ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\MakeTheme.obj"
	-@erase "$(INTDIR)\MakeTheme.res"
	-@erase "$(INTDIR)\ThemeZip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MakeTheme.pdb"
	-@erase ".\MakeTheme.exe"
	-@erase ".\MakeTheme.ilk"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../../../lib/zlib/include" /I "../../../../lib/unzip/include" /I "../../../../config" /I "../../../../base/include" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "NOMINMAX" /D "_DEBUG" /Fp"$(INTDIR)\MakeTheme.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\MakeTheme.res" /d "NDEBUG" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MakeTheme.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=zlib.lib unzip.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  wsock32.lib zlib.lib unzip.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\MakeTheme.pdb" /debug /machine:I386 /out:"MakeTheme.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\lib\unzip" 
LINK32_OBJS= \
	"$(INTDIR)\MakeTheme.obj" \
	"$(INTDIR)\ThemeZip.obj" \
	"$(INTDIR)\MakeTheme.res" \
	"..\..\..\..\lib\unzip\unzip.lib" \
	"..\..\..\..\lib\zlib\zlib.lib"

".\MakeTheme.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "zlib - Win32 NASM Release" "unzip - Win32 NASM Release" "..\..\..\..\config\config.h" ".\MakeTheme.exe"
   IF NOT EXIST ..\..\..\..\base\win32\prj\tools mkdir                                                    ..\..\..\..\base\win32\prj\tools
	copy MakeTheme.exe     ..\..\..\..\base\win32\prj\tools
	copy ..\..\howto\ThemeHowTo.txt     ..\..\..\..\base\win32\prj\tools
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
!IF EXISTS("MakeTheme.dep")
!INCLUDE "MakeTheme.dep"
!ELSE 
!MESSAGE Warning: cannot find "MakeTheme.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MakeTheme - Win32 Release" || "$(CFG)" == "MakeTheme - Win32 Debug" || "$(CFG)" == "MakeTheme - Win32 NASM Debug" || "$(CFG)" == "MakeTheme - Win32 NASM Release"
SOURCE=..\..\..\..\config\config.win32

!IF  "$(CFG)" == "MakeTheme - Win32 Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 NASM Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 NASM Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ENDIF 

SOURCE=..\src\MakeTheme.cpp

"$(INTDIR)\MakeTheme.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\MakeTheme.rc

"$(INTDIR)\MakeTheme.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=..\..\src\ThemeZip.cpp

"$(INTDIR)\ThemeZip.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!IF  "$(CFG)" == "MakeTheme - Win32 Release"

"unzip - Win32 Release" : 
   cd "\Local\src\freeamp\lib\unzip\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\unzip.mak CFG="unzip - Win32 Release" 
   cd "..\..\..\ui\freeamp\tools\win32"

"unzip - Win32 ReleaseCLEAN" : 
   cd "\Local\src\freeamp\lib\unzip\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\unzip.mak CFG="unzip - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\ui\freeamp\tools\win32"

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 Debug"

"unzip - Win32 Debug" : 
   cd "\Local\src\freeamp\lib\unzip\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\unzip.mak CFG="unzip - Win32 Debug" 
   cd "..\..\..\ui\freeamp\tools\win32"

"unzip - Win32 DebugCLEAN" : 
   cd "\Local\src\freeamp\lib\unzip\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\unzip.mak CFG="unzip - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\ui\freeamp\tools\win32"

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 NASM Debug"

"unzip - Win32 NASM Debug" : 
   cd "\Local\src\freeamp\lib\unzip\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\unzip.mak CFG="unzip - Win32 NASM Debug" 
   cd "..\..\..\ui\freeamp\tools\win32"

"unzip - Win32 NASM DebugCLEAN" : 
   cd "\Local\src\freeamp\lib\unzip\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\unzip.mak CFG="unzip - Win32 NASM Debug" RECURSE=1 CLEAN 
   cd "..\..\..\ui\freeamp\tools\win32"

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 NASM Release"

"unzip - Win32 NASM Release" : 
   cd "\Local\src\freeamp\lib\unzip\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\unzip.mak CFG="unzip - Win32 NASM Release" 
   cd "..\..\..\ui\freeamp\tools\win32"

"unzip - Win32 NASM ReleaseCLEAN" : 
   cd "\Local\src\freeamp\lib\unzip\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\unzip.mak CFG="unzip - Win32 NASM Release" RECURSE=1 CLEAN 
   cd "..\..\..\ui\freeamp\tools\win32"

!ENDIF 

!IF  "$(CFG)" == "MakeTheme - Win32 Release"

"zlib - Win32 Release" : 
   cd "\Local\src\freeamp\lib\zlib\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 Release" 
   cd "..\..\..\ui\freeamp\tools\win32"

"zlib - Win32 ReleaseCLEAN" : 
   cd "\Local\src\freeamp\lib\zlib\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\ui\freeamp\tools\win32"

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 Debug"

"zlib - Win32 Debug" : 
   cd "\Local\src\freeamp\lib\zlib\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 Debug" 
   cd "..\..\..\ui\freeamp\tools\win32"

"zlib - Win32 DebugCLEAN" : 
   cd "\Local\src\freeamp\lib\zlib\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\ui\freeamp\tools\win32"

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 NASM Debug"

"zlib - Win32 NASM Debug" : 
   cd "\Local\src\freeamp\lib\zlib\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 NASM Debug" 
   cd "..\..\..\ui\freeamp\tools\win32"

"zlib - Win32 NASM DebugCLEAN" : 
   cd "\Local\src\freeamp\lib\zlib\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 NASM Debug" RECURSE=1 CLEAN 
   cd "..\..\..\ui\freeamp\tools\win32"

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 NASM Release"

"zlib - Win32 NASM Release" : 
   cd "\Local\src\freeamp\lib\zlib\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 NASM Release" 
   cd "..\..\..\ui\freeamp\tools\win32"

"zlib - Win32 NASM ReleaseCLEAN" : 
   cd "\Local\src\freeamp\lib\zlib\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\zlib.mak CFG="zlib - Win32 NASM Release" RECURSE=1 CLEAN 
   cd "..\..\..\ui\freeamp\tools\win32"

!ENDIF 


!ENDIF 

