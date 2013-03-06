# Microsoft Developer Studio Generated NMAKE File, Based on cddb.dsp
!IF "$(CFG)" == ""
CFG=cddb - Win32 Release
!MESSAGE No configuration specified. Defaulting to cddb - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "cddb - Win32 Release" && "$(CFG)" != "cddb - Win32 Debug" && "$(CFG)" != "cddb - Win32 NASM Release" && "$(CFG)" != "cddb - Win32 NASM Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cddb.mak" CFG="cddb - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cddb - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cddb - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cddb - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cddb - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "cddb - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : ".\cddb.mdf"


CLEAN :
	-@erase "$(INTDIR)\cddb.obj"
	-@erase "$(INTDIR)\cddb.res"
	-@erase "$(INTDIR)\cddb_proto.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\cddb.exp"
	-@erase "$(OUTDIR)\cddb.lib"
	-@erase ".\cddb.mdf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\base\aps" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\cd\win32\include" /I "..\..\..\..\lib\gdbm" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\cddb.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cddb.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cddb.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib fabaselib.lib gdbm.lib version.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x10500000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\cddb.pdb" /machine:I386 /def:".\cddb.def" /out:"cddb.mdf" /implib:"$(OUTDIR)\cddb.lib" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\lib\musicbrainz" 
DEF_FILE= \
	".\cddb.def"
LINK32_OBJS= \
	"$(INTDIR)\cddb.obj" \
	"$(INTDIR)\cddb_proto.obj" \
	"$(INTDIR)\cddb.res"

".\cddb.mdf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : ".\cddb.mdf"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                               ..\..\..\..\base\win32\prj\plugins
	copy cddb.mdf      ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "cddb - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : ".\cddb.mdf"


CLEAN :
	-@erase "$(INTDIR)\cddb.obj"
	-@erase "$(INTDIR)\cddb.res"
	-@erase "$(INTDIR)\cddb_proto.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\cddb.exp"
	-@erase "$(OUTDIR)\cddb.lib"
	-@erase "$(OUTDIR)\cddb.pdb"
	-@erase ".\cddb.ilk"
	-@erase ".\cddb.mdf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\base\aps" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\cd\win32\include" /I "..\..\..\..\lib\gdbm" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\cddb.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cddb.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cddb.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib fabaselib.lib gdbm.lib version.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x10500000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\cddb.pdb" /debug /machine:I386 /def:".\cddb.def" /out:"cddb.mdf" /implib:"$(OUTDIR)\cddb.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\lib\musicbrainz" 
DEF_FILE= \
	".\cddb.def"
LINK32_OBJS= \
	"$(INTDIR)\cddb.obj" \
	"$(INTDIR)\cddb_proto.obj" \
	"$(INTDIR)\cddb.res"

".\cddb.mdf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : ".\cddb.mdf"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                               ..\..\..\..\base\win32\prj\plugins
	copy cddb.mdf      ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "cddb - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : ".\cddb.mdf"


CLEAN :
	-@erase "$(INTDIR)\cddb.obj"
	-@erase "$(INTDIR)\cddb.res"
	-@erase "$(INTDIR)\cddb_proto.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\cddb.exp"
	-@erase "$(OUTDIR)\cddb.lib"
	-@erase ".\cddb.mdf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\base\aps" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\cd\win32\include" /I "..\..\..\..\lib\gdbm" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\cddb.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cddb.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cddb.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib fabaselib.lib gdbm.lib version.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x10500000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\cddb.pdb" /machine:I386 /def:".\cddb.def" /out:"cddb.mdf" /implib:"$(OUTDIR)\cddb.lib" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\lib\musicbrainz" 
DEF_FILE= \
	".\cddb.def"
LINK32_OBJS= \
	"$(INTDIR)\cddb.obj" \
	"$(INTDIR)\cddb_proto.obj" \
	"$(INTDIR)\cddb.res"

".\cddb.mdf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : ".\cddb.mdf"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                               ..\..\..\..\base\win32\prj\plugins
	copy cddb.mdf      ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "cddb - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : ".\cddb.mdf"


CLEAN :
	-@erase "$(INTDIR)\cddb.obj"
	-@erase "$(INTDIR)\cddb.res"
	-@erase "$(INTDIR)\cddb_proto.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\cddb.exp"
	-@erase "$(OUTDIR)\cddb.lib"
	-@erase "$(OUTDIR)\cddb.pdb"
	-@erase ".\cddb.ilk"
	-@erase ".\cddb.mdf"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\base\aps" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\cd\win32\include" /I "..\..\..\..\lib\gdbm" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\cddb.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cddb.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cddb.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib gdbm.lib version.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x10500000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\cddb.pdb" /debug /machine:I386 /def:".\cddb.def" /out:"cddb.mdf" /implib:"$(OUTDIR)\cddb.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\lib\musicbrainz" 
DEF_FILE= \
	".\cddb.def"
LINK32_OBJS= \
	"$(INTDIR)\cddb.obj" \
	"$(INTDIR)\cddb_proto.obj" \
	"$(INTDIR)\cddb.res"

".\cddb.mdf" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : ".\cddb.mdf"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                               ..\..\..\..\base\win32\prj\plugins
	copy cddb.mdf      ..\..\..\..\base\win32\prj\plugins
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
!IF EXISTS("cddb.dep")
!INCLUDE "cddb.dep"
!ELSE 
!MESSAGE Warning: cannot find "cddb.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "cddb - Win32 Release" || "$(CFG)" == "cddb - Win32 Debug" || "$(CFG)" == "cddb - Win32 NASM Release" || "$(CFG)" == "cddb - Win32 NASM Debug"
SOURCE=..\cddb.cpp

"$(INTDIR)\cddb.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\cddb_proto.cpp

"$(INTDIR)\cddb_proto.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\res\cddb.rc

!IF  "$(CFG)" == "cddb - Win32 Release"


"$(INTDIR)\cddb.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\cddb.res" /i "\Local\src\freeamp\plm\metadata\cddb\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "cddb - Win32 Debug"


"$(INTDIR)\cddb.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\cddb.res" /i "\Local\src\freeamp\plm\metadata\cddb\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "cddb - Win32 NASM Release"


"$(INTDIR)\cddb.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\cddb.res" /i "\Local\src\freeamp\plm\metadata\cddb\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "cddb - Win32 NASM Debug"


"$(INTDIR)\cddb.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\cddb.res" /i "\Local\src\freeamp\plm\metadata\cddb\res" /d "_DEBUG" $(SOURCE)


!ENDIF 


!ENDIF 

