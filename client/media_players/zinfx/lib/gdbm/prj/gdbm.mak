# Microsoft Developer Studio Generated NMAKE File, Based on gdbm.dsp
!IF "$(CFG)" == ""
CFG=gdbm - Win32 NASM Debug MS STL
!MESSAGE No configuration specified. Defaulting to gdbm - Win32 NASM Debug MS STL.
!ENDIF 

!IF "$(CFG)" != "gdbm - Win32 Release" && "$(CFG)" != "gdbm - Win32 Debug" && "$(CFG)" != "gdbm - Win32 NASM Release" && "$(CFG)" != "gdbm - Win32 NASM Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gdbm.mak" CFG="gdbm - Win32 NASM Debug MS STL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gdbm - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gdbm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "gdbm - Win32 NASM Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gdbm - Win32 NASM Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "gdbm - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\config\config.h" "..\gdbm.lib"


CLEAN :
	-@erase "$(INTDIR)\bucket.obj"
	-@erase "$(INTDIR)\falloc.obj"
	-@erase "$(INTDIR)\findkey.obj"
	-@erase "$(INTDIR)\gdbmclose.obj"
	-@erase "$(INTDIR)\gdbmdelete.obj"
	-@erase "$(INTDIR)\gdbmerrno.obj"
	-@erase "$(INTDIR)\gdbmexists.obj"
	-@erase "$(INTDIR)\gdbmfdesc.obj"
	-@erase "$(INTDIR)\gdbmfetch.obj"
	-@erase "$(INTDIR)\gdbmopen.obj"
	-@erase "$(INTDIR)\gdbmreorg.obj"
	-@erase "$(INTDIR)\gdbmseq.obj"
	-@erase "$(INTDIR)\gdbmsetopt.obj"
	-@erase "$(INTDIR)\gdbmstore.obj"
	-@erase "$(INTDIR)\gdbmsync.obj"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\hash.obj"
	-@erase "$(INTDIR)\update.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "..\gdbm.lib"
	-@erase "..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\config" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\gdbm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gdbm.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\gdbm.lib" 
LIB32_OBJS= \
	"$(INTDIR)\bucket.obj" \
	"$(INTDIR)\falloc.obj" \
	"$(INTDIR)\findkey.obj" \
	"$(INTDIR)\gdbmclose.obj" \
	"$(INTDIR)\gdbmdelete.obj" \
	"$(INTDIR)\gdbmerrno.obj" \
	"$(INTDIR)\gdbmexists.obj" \
	"$(INTDIR)\gdbmfdesc.obj" \
	"$(INTDIR)\gdbmfetch.obj" \
	"$(INTDIR)\gdbmopen.obj" \
	"$(INTDIR)\gdbmreorg.obj" \
	"$(INTDIR)\gdbmseq.obj" \
	"$(INTDIR)\gdbmsetopt.obj" \
	"$(INTDIR)\gdbmstore.obj" \
	"$(INTDIR)\gdbmsync.obj" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\hash.obj" \
	"$(INTDIR)\update.obj" \
	"$(INTDIR)\version.obj"

"..\gdbm.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gdbm - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\config\config.h" "..\gdbm.lib"


CLEAN :
	-@erase "$(INTDIR)\bucket.obj"
	-@erase "$(INTDIR)\falloc.obj"
	-@erase "$(INTDIR)\findkey.obj"
	-@erase "$(INTDIR)\gdbmclose.obj"
	-@erase "$(INTDIR)\gdbmdelete.obj"
	-@erase "$(INTDIR)\gdbmerrno.obj"
	-@erase "$(INTDIR)\gdbmexists.obj"
	-@erase "$(INTDIR)\gdbmfdesc.obj"
	-@erase "$(INTDIR)\gdbmfetch.obj"
	-@erase "$(INTDIR)\gdbmopen.obj"
	-@erase "$(INTDIR)\gdbmreorg.obj"
	-@erase "$(INTDIR)\gdbmseq.obj"
	-@erase "$(INTDIR)\gdbmsetopt.obj"
	-@erase "$(INTDIR)\gdbmstore.obj"
	-@erase "$(INTDIR)\gdbmsync.obj"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\hash.obj"
	-@erase "$(INTDIR)\update.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "..\gdbm.lib"
	-@erase "..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\config" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\gdbm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gdbm.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\gdbm.lib" 
LIB32_OBJS= \
	"$(INTDIR)\bucket.obj" \
	"$(INTDIR)\falloc.obj" \
	"$(INTDIR)\findkey.obj" \
	"$(INTDIR)\gdbmclose.obj" \
	"$(INTDIR)\gdbmdelete.obj" \
	"$(INTDIR)\gdbmerrno.obj" \
	"$(INTDIR)\gdbmexists.obj" \
	"$(INTDIR)\gdbmfdesc.obj" \
	"$(INTDIR)\gdbmfetch.obj" \
	"$(INTDIR)\gdbmopen.obj" \
	"$(INTDIR)\gdbmreorg.obj" \
	"$(INTDIR)\gdbmseq.obj" \
	"$(INTDIR)\gdbmsetopt.obj" \
	"$(INTDIR)\gdbmstore.obj" \
	"$(INTDIR)\gdbmsync.obj" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\hash.obj" \
	"$(INTDIR)\update.obj" \
	"$(INTDIR)\version.obj"

"..\gdbm.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gdbm - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\config\config.h" "..\gdbm.lib"


CLEAN :
	-@erase "$(INTDIR)\bucket.obj"
	-@erase "$(INTDIR)\falloc.obj"
	-@erase "$(INTDIR)\findkey.obj"
	-@erase "$(INTDIR)\gdbmclose.obj"
	-@erase "$(INTDIR)\gdbmdelete.obj"
	-@erase "$(INTDIR)\gdbmerrno.obj"
	-@erase "$(INTDIR)\gdbmexists.obj"
	-@erase "$(INTDIR)\gdbmfdesc.obj"
	-@erase "$(INTDIR)\gdbmfetch.obj"
	-@erase "$(INTDIR)\gdbmopen.obj"
	-@erase "$(INTDIR)\gdbmreorg.obj"
	-@erase "$(INTDIR)\gdbmseq.obj"
	-@erase "$(INTDIR)\gdbmsetopt.obj"
	-@erase "$(INTDIR)\gdbmstore.obj"
	-@erase "$(INTDIR)\gdbmsync.obj"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\hash.obj"
	-@erase "$(INTDIR)\update.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "..\gdbm.lib"
	-@erase "..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\config" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\gdbm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gdbm.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\gdbm.lib" 
LIB32_OBJS= \
	"$(INTDIR)\bucket.obj" \
	"$(INTDIR)\falloc.obj" \
	"$(INTDIR)\findkey.obj" \
	"$(INTDIR)\gdbmclose.obj" \
	"$(INTDIR)\gdbmdelete.obj" \
	"$(INTDIR)\gdbmerrno.obj" \
	"$(INTDIR)\gdbmexists.obj" \
	"$(INTDIR)\gdbmfdesc.obj" \
	"$(INTDIR)\gdbmfetch.obj" \
	"$(INTDIR)\gdbmopen.obj" \
	"$(INTDIR)\gdbmreorg.obj" \
	"$(INTDIR)\gdbmseq.obj" \
	"$(INTDIR)\gdbmsetopt.obj" \
	"$(INTDIR)\gdbmstore.obj" \
	"$(INTDIR)\gdbmsync.obj" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\hash.obj" \
	"$(INTDIR)\update.obj" \
	"$(INTDIR)\version.obj"

"..\gdbm.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gdbm - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\config\config.h" "..\gdbm.lib"


CLEAN :
	-@erase "$(INTDIR)\bucket.obj"
	-@erase "$(INTDIR)\falloc.obj"
	-@erase "$(INTDIR)\findkey.obj"
	-@erase "$(INTDIR)\gdbmclose.obj"
	-@erase "$(INTDIR)\gdbmdelete.obj"
	-@erase "$(INTDIR)\gdbmerrno.obj"
	-@erase "$(INTDIR)\gdbmexists.obj"
	-@erase "$(INTDIR)\gdbmfdesc.obj"
	-@erase "$(INTDIR)\gdbmfetch.obj"
	-@erase "$(INTDIR)\gdbmopen.obj"
	-@erase "$(INTDIR)\gdbmreorg.obj"
	-@erase "$(INTDIR)\gdbmseq.obj"
	-@erase "$(INTDIR)\gdbmsetopt.obj"
	-@erase "$(INTDIR)\gdbmstore.obj"
	-@erase "$(INTDIR)\gdbmsync.obj"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\hash.obj"
	-@erase "$(INTDIR)\update.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "..\gdbm.lib"
	-@erase "..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "..\config" /I ".\\" /I "..\\" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\gdbm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gdbm.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\gdbm.lib" 
LIB32_OBJS= \
	"$(INTDIR)\bucket.obj" \
	"$(INTDIR)\falloc.obj" \
	"$(INTDIR)\findkey.obj" \
	"$(INTDIR)\gdbmclose.obj" \
	"$(INTDIR)\gdbmdelete.obj" \
	"$(INTDIR)\gdbmerrno.obj" \
	"$(INTDIR)\gdbmexists.obj" \
	"$(INTDIR)\gdbmfdesc.obj" \
	"$(INTDIR)\gdbmfetch.obj" \
	"$(INTDIR)\gdbmopen.obj" \
	"$(INTDIR)\gdbmreorg.obj" \
	"$(INTDIR)\gdbmseq.obj" \
	"$(INTDIR)\gdbmsetopt.obj" \
	"$(INTDIR)\gdbmstore.obj" \
	"$(INTDIR)\gdbmsync.obj" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\hash.obj" \
	"$(INTDIR)\update.obj" \
	"$(INTDIR)\version.obj"

"..\gdbm.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

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
!IF EXISTS("gdbm.dep")
!INCLUDE "gdbm.dep"
!ELSE 
!MESSAGE Warning: cannot find "gdbm.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "gdbm - Win32 Release" || "$(CFG)" == "gdbm - Win32 Debug" || "$(CFG)" == "gdbm - Win32 NASM Release" || "$(CFG)" == "gdbm - Win32 NASM Debug"
SOURCE=..\bucket.c

"$(INTDIR)\bucket.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\config\config.win32

!IF  "$(CFG)" == "gdbm - Win32 Release"

InputPath=..\config\config.win32

"..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if not exist ..\config\config.h copy ..\config\config.win32 ..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "gdbm - Win32 Debug"

InputPath=..\config\config.win32

"..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if not exist ..\config\config.h copy ..\config\config.win32 ..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "gdbm - Win32 NASM Release"

InputPath=..\config\config.win32

"..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if not exist ..\config\config.h copy ..\config\config.win32 ..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "gdbm - Win32 NASM Debug"

InputPath=..\config\config.win32

"..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if not exist ..\config\config.h copy ..\config\config.win32 ..\config\config.h
<< 
	

!ENDIF 

SOURCE=..\falloc.c

"$(INTDIR)\falloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\findkey.c

"$(INTDIR)\findkey.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\gdbmclose.c

"$(INTDIR)\gdbmclose.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\gdbmdelete.c

"$(INTDIR)\gdbmdelete.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\gdbmerrno.c

"$(INTDIR)\gdbmerrno.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\gdbmexists.c

"$(INTDIR)\gdbmexists.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\gdbmfdesc.c

"$(INTDIR)\gdbmfdesc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\gdbmfetch.c

"$(INTDIR)\gdbmfetch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\gdbmopen.c

"$(INTDIR)\gdbmopen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\gdbmreorg.c

"$(INTDIR)\gdbmreorg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\gdbmseq.c

"$(INTDIR)\gdbmseq.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\gdbmsetopt.c

"$(INTDIR)\gdbmsetopt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\gdbmstore.c

"$(INTDIR)\gdbmstore.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\gdbmsync.c

"$(INTDIR)\gdbmsync.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\global.c

"$(INTDIR)\global.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\hash.c

"$(INTDIR)\hash.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\update.c

"$(INTDIR)\update.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\version.c

"$(INTDIR)\version.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

