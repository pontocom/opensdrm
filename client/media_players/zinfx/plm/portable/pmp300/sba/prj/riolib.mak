# Microsoft Developer Studio Generated NMAKE File, Based on riolib.dsp
!IF "$(CFG)" == ""
CFG=riolib - Win32 NASM Debug MS STL
!MESSAGE No configuration specified. Defaulting to riolib - Win32 NASM Debug MS STL.
!ENDIF 

!IF "$(CFG)" != "riolib - Win32 Release" && "$(CFG)" != "riolib - Win32 Debug" && "$(CFG)" != "riolib - Win32 NASM Release" && "$(CFG)" != "riolib - Win32 NASM Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "riolib.mak" CFG="riolib - Win32 NASM Debug MS STL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "riolib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "riolib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "riolib - Win32 NASM Release" (based on "Win32 (x86) Static Library")
!MESSAGE "riolib - Win32 NASM Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "riolib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\riolib.lib"


CLEAN :
	-@erase "$(INTDIR)\rio.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\riolib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\riolib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\riolib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\riolib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\rio.obj"

"..\riolib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "riolib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\riolib.lib"


CLEAN :
	-@erase "$(INTDIR)\rio.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\riolib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\riolib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\riolib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\riolib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\rio.obj"

"..\riolib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "riolib - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\riolib.lib"


CLEAN :
	-@erase "$(INTDIR)\rio.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\riolib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\riolib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\riolib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\riolib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\rio.obj"

"..\riolib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "riolib - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\riolib.lib"


CLEAN :
	-@erase "$(INTDIR)\rio.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\riolib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\riolib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\riolib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\riolib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\rio.obj"

"..\riolib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("riolib.dep")
!INCLUDE "riolib.dep"
!ELSE 
!MESSAGE Warning: cannot find "riolib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "riolib - Win32 Release" || "$(CFG)" == "riolib - Win32 Debug" || "$(CFG)" == "riolib - Win32 NASM Release" || "$(CFG)" == "riolib - Win32 NASM Debug"
SOURCE=..\rio.cpp

"$(INTDIR)\rio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

