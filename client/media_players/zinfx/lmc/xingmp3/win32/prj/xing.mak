# Microsoft Developer Studio Generated NMAKE File, Based on xing.dsp
!IF "$(CFG)" == ""
CFG=xing - Win32 NASM Debug MS STL
!MESSAGE No configuration specified. Defaulting to xing - Win32 NASM Debug MS STL.
!ENDIF 

!IF "$(CFG)" != "xing - Win32 Release" && "$(CFG)" != "xing - Win32 Debug" && "$(CFG)" != "xing - Win32 NASM Release" && "$(CFG)" != "xing - Win32 NASM Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xing.mak" CFG="xing - Win32 NASM Debug MS STL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xing - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xing - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xing - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xing - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "xing - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\xing.lmc"

!ELSE 

ALL : "fabaselib - Win32 Release" "..\..\..\..\config\config.h" ".\xing.lmc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cdct.obj"
	-@erase "$(INTDIR)\csbt.obj"
	-@erase "$(INTDIR)\cup.obj"
	-@erase "$(INTDIR)\cupl3.obj"
	-@erase "$(INTDIR)\cwinm.obj"
	-@erase "$(INTDIR)\dec8.obj"
	-@erase "$(INTDIR)\eventbuffer.obj"
	-@erase "$(INTDIR)\hwin.obj"
	-@erase "$(INTDIR)\icdct.obj"
	-@erase "$(INTDIR)\isbt.obj"
	-@erase "$(INTDIR)\iup.obj"
	-@erase "$(INTDIR)\iwinm.obj"
	-@erase "$(INTDIR)\l3dq.obj"
	-@erase "$(INTDIR)\l3init.obj"
	-@erase "$(INTDIR)\mdct.obj"
	-@erase "$(INTDIR)\mhead.obj"
	-@erase "$(INTDIR)\msis.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\uph.obj"
	-@erase "$(INTDIR)\upsf.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wavep.obj"
	-@erase "$(INTDIR)\xing.res"
	-@erase "$(INTDIR)\xinglmc.obj"
	-@erase "$(OUTDIR)\xing.exp"
	-@erase "$(OUTDIR)\xing.lib"
	-@erase ".\xing.lmc"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\xing.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\xing.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xing.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11800000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\xing.pdb" /machine:I386 /def:".\xing.def" /out:"xing.lmc" /implib:"$(OUTDIR)\xing.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\xing.def"
LINK32_OBJS= \
	"$(INTDIR)\eventbuffer.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\cdct.obj" \
	"$(INTDIR)\csbt.obj" \
	"$(INTDIR)\cup.obj" \
	"$(INTDIR)\cupl3.obj" \
	"$(INTDIR)\cwinm.obj" \
	"$(INTDIR)\dec8.obj" \
	"$(INTDIR)\hwin.obj" \
	"$(INTDIR)\icdct.obj" \
	"$(INTDIR)\isbt.obj" \
	"$(INTDIR)\iup.obj" \
	"$(INTDIR)\iwinm.obj" \
	"$(INTDIR)\l3dq.obj" \
	"$(INTDIR)\l3init.obj" \
	"$(INTDIR)\mdct.obj" \
	"$(INTDIR)\mhead.obj" \
	"$(INTDIR)\msis.obj" \
	"$(INTDIR)\uph.obj" \
	"$(INTDIR)\upsf.obj" \
	"$(INTDIR)\wavep.obj" \
	"$(INTDIR)\xinglmc.obj" \
	"$(INTDIR)\xing.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\xing.lmc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Release" "..\..\..\..\config\config.h" ".\xing.lmc"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                    ..\..\..\..\base\win32\prj\plugins
	copy xing.lmc                            ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "xing - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\xing.lmc"

!ELSE 

ALL : "fabaselib - Win32 Debug" "..\..\..\..\config\config.h" ".\xing.lmc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cdct.obj"
	-@erase "$(INTDIR)\csbt.obj"
	-@erase "$(INTDIR)\cup.obj"
	-@erase "$(INTDIR)\cupl3.obj"
	-@erase "$(INTDIR)\cwinm.obj"
	-@erase "$(INTDIR)\dec8.obj"
	-@erase "$(INTDIR)\eventbuffer.obj"
	-@erase "$(INTDIR)\hwin.obj"
	-@erase "$(INTDIR)\icdct.obj"
	-@erase "$(INTDIR)\isbt.obj"
	-@erase "$(INTDIR)\iup.obj"
	-@erase "$(INTDIR)\iwinm.obj"
	-@erase "$(INTDIR)\l3dq.obj"
	-@erase "$(INTDIR)\l3init.obj"
	-@erase "$(INTDIR)\mdct.obj"
	-@erase "$(INTDIR)\mhead.obj"
	-@erase "$(INTDIR)\msis.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\uph.obj"
	-@erase "$(INTDIR)\upsf.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wavep.obj"
	-@erase "$(INTDIR)\xing.res"
	-@erase "$(INTDIR)\xinglmc.obj"
	-@erase "$(OUTDIR)\xing.exp"
	-@erase "$(OUTDIR)\xing.lib"
	-@erase "$(OUTDIR)\xing.pdb"
	-@erase ".\xing.ilk"
	-@erase ".\xing.lmc"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\xing.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\xing.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xing.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11800000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\xing.pdb" /debug /machine:I386 /def:".\xing.def" /out:"xing.lmc" /implib:"$(OUTDIR)\xing.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\xing.def"
LINK32_OBJS= \
	"$(INTDIR)\eventbuffer.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\cdct.obj" \
	"$(INTDIR)\csbt.obj" \
	"$(INTDIR)\cup.obj" \
	"$(INTDIR)\cupl3.obj" \
	"$(INTDIR)\cwinm.obj" \
	"$(INTDIR)\dec8.obj" \
	"$(INTDIR)\hwin.obj" \
	"$(INTDIR)\icdct.obj" \
	"$(INTDIR)\isbt.obj" \
	"$(INTDIR)\iup.obj" \
	"$(INTDIR)\iwinm.obj" \
	"$(INTDIR)\l3dq.obj" \
	"$(INTDIR)\l3init.obj" \
	"$(INTDIR)\mdct.obj" \
	"$(INTDIR)\mhead.obj" \
	"$(INTDIR)\msis.obj" \
	"$(INTDIR)\uph.obj" \
	"$(INTDIR)\upsf.obj" \
	"$(INTDIR)\wavep.obj" \
	"$(INTDIR)\xinglmc.obj" \
	"$(INTDIR)\xing.res" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\xing.lmc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 Debug" "..\..\..\..\config\config.h" ".\xing.lmc"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                    ..\..\..\..\base\win32\prj\plugins
	copy xing.lmc                            ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\xing.lmc"

!ELSE 

ALL : "fabaselib - Win32 NASM Release" "..\..\..\..\config\config.h" ".\xing.lmc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cdct.obj"
	-@erase "$(INTDIR)\csbt.obj"
	-@erase "$(INTDIR)\cup.obj"
	-@erase "$(INTDIR)\cupl3.obj"
	-@erase "$(INTDIR)\cwinm.obj"
	-@erase "$(INTDIR)\dec8.obj"
	-@erase "$(INTDIR)\eventbuffer.obj"
	-@erase "$(INTDIR)\hwin.obj"
	-@erase "$(INTDIR)\icdct.obj"
	-@erase "$(INTDIR)\isbt.obj"
	-@erase "$(INTDIR)\iup.obj"
	-@erase "$(INTDIR)\iwinm.obj"
	-@erase "$(INTDIR)\l3dq.obj"
	-@erase "$(INTDIR)\l3init.obj"
	-@erase "$(INTDIR)\mdct.obj"
	-@erase "$(INTDIR)\mhead.obj"
	-@erase "$(INTDIR)\msis.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\uph.obj"
	-@erase "$(INTDIR)\upsf.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wavep.obj"
	-@erase "$(INTDIR)\xing.res"
	-@erase "$(INTDIR)\xinglmc.obj"
	-@erase "$(OUTDIR)\xing.exp"
	-@erase "$(OUTDIR)\xing.lib"
	-@erase ".\xing.lmc"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /D "NDEBUG" /D "_WINDOWS" /D "ASM_X86" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\xing.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\xing.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xing.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11800000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\xing.pdb" /machine:I386 /def:".\xing.def" /out:"xing.lmc" /implib:"$(OUTDIR)\xing.lib" /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\xing.def"
LINK32_OBJS= \
	"$(INTDIR)\eventbuffer.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\cdct.obj" \
	"$(INTDIR)\csbt.obj" \
	"$(INTDIR)\cup.obj" \
	"$(INTDIR)\cupl3.obj" \
	"$(INTDIR)\cwinm.obj" \
	"$(INTDIR)\dec8.obj" \
	"$(INTDIR)\hwin.obj" \
	"$(INTDIR)\icdct.obj" \
	"$(INTDIR)\isbt.obj" \
	"$(INTDIR)\iup.obj" \
	"$(INTDIR)\iwinm.obj" \
	"$(INTDIR)\l3dq.obj" \
	"$(INTDIR)\l3init.obj" \
	"$(INTDIR)\mdct.obj" \
	"$(INTDIR)\mhead.obj" \
	"$(INTDIR)\msis.obj" \
	"$(INTDIR)\uph.obj" \
	"$(INTDIR)\upsf.obj" \
	"$(INTDIR)\wavep.obj" \
	"$(INTDIR)\xinglmc.obj" \
	"$(INTDIR)\xing.res" \
	"$(INTDIR)\cdctasm.obj" \
	"$(INTDIR)\cwin8asm.obj" \
	"$(INTDIR)\cwinasm.obj" \
	"$(INTDIR)\mdctasm.obj" \
	"$(INTDIR)\msisasm.obj" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\xing.lmc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Release" "..\..\..\..\config\config.h" ".\xing.lmc"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                    ..\..\..\..\base\win32\prj\plugins
	copy xing.lmc                            ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\xing.lmc"

!ELSE 

ALL : "fabaselib - Win32 NASM Debug" "..\..\..\..\config\config.h" ".\xing.lmc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cdct.obj"
	-@erase "$(INTDIR)\csbt.obj"
	-@erase "$(INTDIR)\cup.obj"
	-@erase "$(INTDIR)\cupl3.obj"
	-@erase "$(INTDIR)\cwinm.obj"
	-@erase "$(INTDIR)\dec8.obj"
	-@erase "$(INTDIR)\eventbuffer.obj"
	-@erase "$(INTDIR)\hwin.obj"
	-@erase "$(INTDIR)\icdct.obj"
	-@erase "$(INTDIR)\isbt.obj"
	-@erase "$(INTDIR)\iup.obj"
	-@erase "$(INTDIR)\iwinm.obj"
	-@erase "$(INTDIR)\l3dq.obj"
	-@erase "$(INTDIR)\l3init.obj"
	-@erase "$(INTDIR)\mdct.obj"
	-@erase "$(INTDIR)\mhead.obj"
	-@erase "$(INTDIR)\msis.obj"
	-@erase "$(INTDIR)\pipeline.obj"
	-@erase "$(INTDIR)\pullbuffer.obj"
	-@erase "$(INTDIR)\uph.obj"
	-@erase "$(INTDIR)\upsf.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wavep.obj"
	-@erase "$(INTDIR)\xing.res"
	-@erase "$(INTDIR)\xinglmc.obj"
	-@erase "$(OUTDIR)\xing.exp"
	-@erase "$(OUTDIR)\xing.lib"
	-@erase "$(OUTDIR)\xing.pdb"
	-@erase ".\xing.ilk"
	-@erase ".\xing.lmc"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /D "_DEBUG" /D "_WINDOWS" /D "ASM_X86" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\xing.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\xing.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xing.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x11800000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\xing.pdb" /debug /machine:I386 /def:".\xing.def" /out:"xing.lmc" /implib:"$(OUTDIR)\xing.lib" /pdbtype:sept /libpath:"..\..\..\..\base\win32" 
DEF_FILE= \
	".\xing.def"
LINK32_OBJS= \
	"$(INTDIR)\eventbuffer.obj" \
	"$(INTDIR)\pipeline.obj" \
	"$(INTDIR)\pullbuffer.obj" \
	"$(INTDIR)\cdct.obj" \
	"$(INTDIR)\csbt.obj" \
	"$(INTDIR)\cup.obj" \
	"$(INTDIR)\cupl3.obj" \
	"$(INTDIR)\cwinm.obj" \
	"$(INTDIR)\dec8.obj" \
	"$(INTDIR)\hwin.obj" \
	"$(INTDIR)\icdct.obj" \
	"$(INTDIR)\isbt.obj" \
	"$(INTDIR)\iup.obj" \
	"$(INTDIR)\iwinm.obj" \
	"$(INTDIR)\l3dq.obj" \
	"$(INTDIR)\l3init.obj" \
	"$(INTDIR)\mdct.obj" \
	"$(INTDIR)\mhead.obj" \
	"$(INTDIR)\msis.obj" \
	"$(INTDIR)\uph.obj" \
	"$(INTDIR)\upsf.obj" \
	"$(INTDIR)\wavep.obj" \
	"$(INTDIR)\xinglmc.obj" \
	"$(INTDIR)\xing.res" \
	"$(INTDIR)\cdctasm.obj" \
	"$(INTDIR)\cwin8asm.obj" \
	"$(INTDIR)\cwinasm.obj" \
	"$(INTDIR)\mdctasm.obj" \
	"$(INTDIR)\msisasm.obj" \
	"..\..\..\..\base\win32\fabaselib.lib"

".\xing.lmc" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "fabaselib - Win32 NASM Debug" "..\..\..\..\config\config.h" ".\xing.lmc"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                    ..\..\..\..\base\win32\prj\plugins
	copy xing.lmc                            ..\..\..\..\base\win32\prj\plugins
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
!IF EXISTS("xing.dep")
!INCLUDE "xing.dep"
!ELSE 
!MESSAGE Warning: cannot find "xing.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "xing - Win32 Release" || "$(CFG)" == "xing - Win32 Debug" || "$(CFG)" == "xing - Win32 NASM Release" || "$(CFG)" == "xing - Win32 NASM Debug"
SOURCE=..\..\..\..\config\config.win32

!IF  "$(CFG)" == "xing - Win32 Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "xing - Win32 Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ENDIF 

SOURCE=..\..\..\..\io\src\eventbuffer.cpp

"$(INTDIR)\eventbuffer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\io\src\pipeline.cpp

"$(INTDIR)\pipeline.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\..\io\src\pullbuffer.cpp

"$(INTDIR)\pullbuffer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\res\xing.rc

!IF  "$(CFG)" == "xing - Win32 Release"


"$(INTDIR)\xing.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\xing.res" /i "\Local\src\freeamp\lmc\xingmp3\win32\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "xing - Win32 Debug"


"$(INTDIR)\xing.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\xing.res" /i "\Local\src\freeamp\lmc\xingmp3\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "xing - Win32 NASM Release"


"$(INTDIR)\xing.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\xing.res" /i "\Local\src\freeamp\lmc\xingmp3\win32\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "xing - Win32 NASM Debug"


"$(INTDIR)\xing.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\xing.res" /i "\Local\src\freeamp\lmc\xingmp3\win32\res" /d "_DEBUG" $(SOURCE)


!ENDIF 

SOURCE=..\..\src\cdct.c

"$(INTDIR)\cdct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\csbt.c

"$(INTDIR)\csbt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\cup.c

"$(INTDIR)\cup.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\cupl3.c

"$(INTDIR)\cupl3.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\cwinm.c

"$(INTDIR)\cwinm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\dec8.c

"$(INTDIR)\dec8.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\hwin.c

"$(INTDIR)\hwin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\icdct.c

"$(INTDIR)\icdct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\isbt.c

"$(INTDIR)\isbt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\iup.c

"$(INTDIR)\iup.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\iwinm.c

"$(INTDIR)\iwinm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\l3dq.c

"$(INTDIR)\l3dq.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\l3init.c

"$(INTDIR)\l3init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\mdct.c

"$(INTDIR)\mdct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\mhead.c

"$(INTDIR)\mhead.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\msis.c

"$(INTDIR)\msis.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\uph.c

"$(INTDIR)\uph.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\upsf.c

"$(INTDIR)\upsf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\wavep.c

"$(INTDIR)\wavep.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\xinglmc.cpp

"$(INTDIR)\xinglmc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\cdctasm.asm

!IF  "$(CFG)" == "xing - Win32 Release"

!ELSEIF  "$(CFG)" == "xing - Win32 Debug"

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Release"

IntDir=.\Release
InputPath=..\..\src\cdctasm.asm
InputName=cdctasm

"$(INTDIR)\cdctasm.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)
<< 
	

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Debug"

IntDir=.\Debug
InputPath=..\..\src\cdctasm.asm
InputName=cdctasm

"$(INTDIR)\cdctasm.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)
<< 
	

!ENDIF 

SOURCE=..\..\src\cwin8asm.asm

!IF  "$(CFG)" == "xing - Win32 Release"

!ELSEIF  "$(CFG)" == "xing - Win32 Debug"

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Release"

IntDir=.\Release
InputPath=..\..\src\cwin8asm.asm
InputName=cwin8asm

"$(INTDIR)\cwin8asm.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)
<< 
	

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Debug"

IntDir=.\Debug
InputPath=..\..\src\cwin8asm.asm
InputName=cwin8asm

"$(INTDIR)\cwin8asm.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)
<< 
	

!ENDIF 

SOURCE=..\..\src\cwinasm.asm

!IF  "$(CFG)" == "xing - Win32 Release"

!ELSEIF  "$(CFG)" == "xing - Win32 Debug"

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Release"

IntDir=.\Release
InputPath=..\..\src\cwinasm.asm
InputName=cwinasm

"$(INTDIR)\cwinasm.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)
<< 
	

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Debug"

IntDir=.\Debug
InputPath=..\..\src\cwinasm.asm
InputName=cwinasm

"$(INTDIR)\cwinasm.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)
<< 
	

!ENDIF 

SOURCE=..\..\src\mdctasm.asm

!IF  "$(CFG)" == "xing - Win32 Release"

!ELSEIF  "$(CFG)" == "xing - Win32 Debug"

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Release"

IntDir=.\Release
InputPath=..\..\src\mdctasm.asm
InputName=mdctasm

"$(INTDIR)\mdctasm.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)
<< 
	

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Debug"

IntDir=.\Debug
InputPath=..\..\src\mdctasm.asm
InputName=mdctasm

"$(INTDIR)\mdctasm.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)
<< 
	

!ENDIF 

SOURCE=..\..\src\msisasm.asm

!IF  "$(CFG)" == "xing - Win32 Release"

!ELSEIF  "$(CFG)" == "xing - Win32 Debug"

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Release"

IntDir=.\Release
InputPath=..\..\src\msisasm.asm
InputName=msisasm

"$(INTDIR)\msisasm.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)
<< 
	

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Debug"

IntDir=.\Debug
InputPath=..\..\src\msisasm.asm
InputName=msisasm

"$(INTDIR)\msisasm.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)
<< 
	

!ENDIF 

!IF  "$(CFG)" == "xing - Win32 Release"

"fabaselib - Win32 Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" 
   cd "..\..\..\lmc\xingmp3\win32\prj"

"fabaselib - Win32 ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\lmc\xingmp3\win32\prj"

!ELSEIF  "$(CFG)" == "xing - Win32 Debug"

"fabaselib - Win32 Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" 
   cd "..\..\..\lmc\xingmp3\win32\prj"

"fabaselib - Win32 DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\lmc\xingmp3\win32\prj"

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Release"

"fabaselib - Win32 NASM Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" 
   cd "..\..\..\lmc\xingmp3\win32\prj"

"fabaselib - Win32 NASM ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" RECURSE=1 CLEAN 
   cd "..\..\..\lmc\xingmp3\win32\prj"

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Debug"

"fabaselib - Win32 NASM Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" 
   cd "..\..\..\lmc\xingmp3\win32\prj"

"fabaselib - Win32 NASM DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" RECURSE=1 CLEAN 
   cd "..\..\..\lmc\xingmp3\win32\prj"

!ENDIF 


!ENDIF 

