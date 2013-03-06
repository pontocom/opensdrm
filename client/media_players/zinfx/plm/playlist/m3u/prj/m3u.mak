# Microsoft Developer Studio Generated NMAKE File, Based on m3u.dsp
!IF "$(CFG)" == ""
CFG=m3u - Win32 NASM Debug MS STL
!MESSAGE No configuration specified. Defaulting to m3u - Win32 NASM Debug MS STL.
!ENDIF 

!IF "$(CFG)" != "m3u - Win32 Release" && "$(CFG)" != "m3u - Win32 Debug" && "$(CFG)" != "m3u - Win32 NASM Release" && "$(CFG)" != "m3u - Win32 NASM Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "m3u.mak" CFG="m3u - Win32 NASM Debug MS STL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "m3u - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "m3u - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "m3u - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "m3u - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "m3u - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : ".\m3u.plf"

!ELSE 

ALL : "fabaselib - Win32 Release" ".\m3u.plf"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\m3u.obj"
	-@erase "$(INTDIR)\m3u.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\m3u.exp"
	-@erase "$(OUTDIR)\m3u.lib"
	-@erase ".\m3u.plf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\m3u.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\m3u.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\m3u.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10700000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\m3u.pdb" /machine:I386 /def:".\m3u.def" /out:"m3u.plf" /implib:"$(OUTDIR)\m3u.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\m3u.def"
LINK32_OBJS= \
	"$(INTDIR)\m3u.obj" \
	"$(INTDIR)\m3u.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\m3u.plf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Release" ".\m3u.plf"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                ..\..\..\..\base\win32\prj\plugins
	copy m3u.plf       ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "m3u - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : ".\m3u.plf"

!ELSE 

ALL : "fabaselib - Win32 Debug" ".\m3u.plf"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\m3u.obj"
	-@erase "$(INTDIR)\m3u.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\m3u.exp"
	-@erase "$(OUTDIR)\m3u.lib"
	-@erase "$(OUTDIR)\m3u.pdb"
	-@erase ".\m3u.ilk"
	-@erase ".\m3u.plf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\m3u.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\m3u.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\m3u.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10700000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\m3u.pdb" /debug /machine:I386 /def:".\m3u.def" /out:"m3u.plf" /implib:"$(OUTDIR)\m3u.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\m3u.def"
LINK32_OBJS= \
	"$(INTDIR)\m3u.obj" \
	"$(INTDIR)\m3u.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\m3u.plf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Debug" ".\m3u.plf"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                ..\..\..\..\base\win32\prj\plugins
	copy m3u.plf       ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "m3u - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : ".\m3u.plf"

!ELSE 

ALL : "fabaselib - Win32 NASM Release" ".\m3u.plf"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\m3u.obj"
	-@erase "$(INTDIR)\m3u.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\m3u.exp"
	-@erase "$(OUTDIR)\m3u.lib"
	-@erase ".\m3u.plf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\m3u.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\m3u.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\m3u.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10700000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\m3u.pdb" /machine:I386 /def:".\m3u.def" /out:"m3u.plf" /implib:"$(OUTDIR)\m3u.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\m3u.def"
LINK32_OBJS= \
	"$(INTDIR)\m3u.obj" \
	"$(INTDIR)\m3u.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\m3u.plf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Release" ".\m3u.plf"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                ..\..\..\..\base\win32\prj\plugins
	copy m3u.plf       ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "m3u - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : ".\m3u.plf"

!ELSE 

ALL : "fabaselib - Win32 NASM Debug" ".\m3u.plf"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\m3u.obj"
	-@erase "$(INTDIR)\m3u.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\m3u.exp"
	-@erase "$(OUTDIR)\m3u.lib"
	-@erase "$(OUTDIR)\m3u.pdb"
	-@erase ".\m3u.ilk"
	-@erase ".\m3u.plf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\m3u.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\m3u.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\m3u.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10700000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\m3u.pdb" /debug /machine:I386 /def:".\m3u.def" /out:"m3u.plf" /implib:"$(OUTDIR)\m3u.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\m3u.def"
LINK32_OBJS= \
	"$(INTDIR)\m3u.obj" \
	"$(INTDIR)\m3u.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\m3u.plf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Debug" ".\m3u.plf"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                ..\..\..\..\base\win32\prj\plugins
	copy m3u.plf       ..\..\..\..\base\win32\prj\plugins
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
!IF EXISTS("m3u.dep")
!INCLUDE "m3u.dep"
!ELSE 
!MESSAGE Warning: cannot find "m3u.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "m3u - Win32 Release" || "$(CFG)" == "m3u - Win32 Debug" || "$(CFG)" == "m3u - Win32 NASM Release" || "$(CFG)" == "m3u - Win32 NASM Debug"
SOURCE=..\m3u.cpp

"$(INTDIR)\m3u.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\res\m3u.rc

!IF  "$(CFG)" == "m3u - Win32 Release"


"$(INTDIR)\m3u.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\m3u.res" /i "\Local\src\freeamp\plm\playlist\m3u\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "m3u - Win32 Debug"


"$(INTDIR)\m3u.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\m3u.res" /i "\Local\src\freeamp\plm\playlist\m3u\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "m3u - Win32 NASM Release"


"$(INTDIR)\m3u.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\m3u.res" /i "\Local\src\freeamp\plm\playlist\m3u\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "m3u - Win32 NASM Debug"


"$(INTDIR)\m3u.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\m3u.res" /i "\Local\src\freeamp\plm\playlist\m3u\res" /d "_DEBUG" $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "m3u - Win32 Release"

"fabaselib - Win32 Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" 
   cd "..\..\..\plm\playlist\m3u\prj"

"fabaselib - Win32 ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\plm\playlist\m3u\prj"

!ELSEIF  "$(CFG)" == "m3u - Win32 Debug"

"fabaselib - Win32 Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" 
   cd "..\..\..\plm\playlist\m3u\prj"

"fabaselib - Win32 DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\plm\playlist\m3u\prj"

!ELSEIF  "$(CFG)" == "m3u - Win32 NASM Release"

"fabaselib - Win32 NASM Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" 
   cd "..\..\..\plm\playlist\m3u\prj"

"fabaselib - Win32 NASM ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" RECURSE=1 CLEAN 
   cd "..\..\..\plm\playlist\m3u\prj"

!ELSEIF  "$(CFG)" == "m3u - Win32 NASM Debug"

"fabaselib - Win32 NASM Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" 
   cd "..\..\..\plm\playlist\m3u\prj"

"fabaselib - Win32 NASM DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" RECURSE=1 CLEAN 
   cd "..\..\..\plm\playlist\m3u\prj"

!ENDIF 


!ENDIF 

