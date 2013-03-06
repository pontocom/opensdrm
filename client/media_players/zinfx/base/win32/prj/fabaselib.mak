# Microsoft Developer Studio Generated NMAKE File, Based on fabaselib.dsp
!IF "$(CFG)" == ""
CFG=fabaselib - Win32 Release
!MESSAGE No configuration specified. Defaulting to fabaselib - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "fabaselib - Win32 Release" && "$(CFG)" != "fabaselib - Win32 Debug" && "$(CFG)" != "fabaselib - Win32 NASM Debug" && "$(CFG)" != "fabaselib - Win32 NASM Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fabaselib.mak" CFG="fabaselib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fabaselib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fabaselib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "fabaselib - Win32 NASM Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "fabaselib - Win32 NASM Release" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "fabaselib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\config\config.h" "..\fabaselib.lib"


CLEAN :
	-@erase "$(INTDIR)\apsconvert.obj"
	-@erase "$(INTDIR)\apsinterface.obj"
	-@erase "$(INTDIR)\apsplaylist.obj"
	-@erase "$(INTDIR)\comhttpsocket.obj"
	-@erase "$(INTDIR)\database.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\downloadmanager.obj"
	-@erase "$(INTDIR)\errors.obj"
	-@erase "$(INTDIR)\FAMetaUnit.obj"
	-@erase "$(INTDIR)\gencrc.obj"
	-@erase "$(INTDIR)\hosttonet.obj"
	-@erase "$(INTDIR)\Http.obj"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\missingfile.obj"
	-@erase "$(INTDIR)\musiccatalog.obj"
	-@erase "$(INTDIR)\mutex.obj"
	-@erase "$(INTDIR)\nettohost.obj"
	-@erase "$(INTDIR)\Parse.obj"
	-@erase "$(INTDIR)\player.obj"
	-@erase "$(INTDIR)\playlist.obj"
	-@erase "$(INTDIR)\preferences.obj"
	-@erase "$(INTDIR)\propimpl.obj"
	-@erase "$(INTDIR)\registrar.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\semaphore.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\timer.obj"
	-@erase "$(INTDIR)\undomanager.obj"
	-@erase "$(INTDIR)\updatemanager.obj"
	-@erase "$(INTDIR)\utility.obj"
	-@erase "$(INTDIR)\uuid.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\win32prefs.obj"
	-@erase "$(INTDIR)\win32thread.obj"
	-@erase "$(INTDIR)\win32updatemanager.obj"
	-@erase "$(INTDIR)\wincomsocket.obj"
	-@erase "$(INTDIR)\ypclient.obj"
	-@erase "..\fabaselib.lib"
	-@erase "..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\lib\http\include" /I "..\..\..\lib\zlib\include" /I "..\..\..\lib\xml\include" /I "..\..\..\lib\gdbm" /I "..\include" /I "..\..\include" /I "..\..\..\config" /I "..\..\..\ui\win32Test\include" /I "..\..\..\ui\win32Test\res" /I "..\..\..\io\include" /I "..\..\..\ui\include" /I "..\..\..\lmc\include" /I "..\..\aps" /I "..\..\..\lib\musicbrainz\lib" /I "..\..\..\lib\musicbrainz\expat\xmlparse" /I "..\..\..\lib\musicbrainz\expat\xmltok" /I "..\..\..\lib\musicbrainz" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\fabaselib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\fabaselib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\fabaselib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\apsconvert.obj" \
	"$(INTDIR)\apsinterface.obj" \
	"$(INTDIR)\apsplaylist.obj" \
	"$(INTDIR)\comhttpsocket.obj" \
	"$(INTDIR)\database.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\downloadmanager.obj" \
	"$(INTDIR)\errors.obj" \
	"$(INTDIR)\FAMetaUnit.obj" \
	"$(INTDIR)\gencrc.obj" \
	"$(INTDIR)\hosttonet.obj" \
	"$(INTDIR)\Http.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\missingfile.obj" \
	"$(INTDIR)\musiccatalog.obj" \
	"$(INTDIR)\mutex.obj" \
	"$(INTDIR)\nettohost.obj" \
	"$(INTDIR)\Parse.obj" \
	"$(INTDIR)\player.obj" \
	"$(INTDIR)\playlist.obj" \
	"$(INTDIR)\preferences.obj" \
	"$(INTDIR)\propimpl.obj" \
	"$(INTDIR)\registrar.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\semaphore.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\timer.obj" \
	"$(INTDIR)\undomanager.obj" \
	"$(INTDIR)\updatemanager.obj" \
	"$(INTDIR)\utility.obj" \
	"$(INTDIR)\uuid.obj" \
	"$(INTDIR)\win32prefs.obj" \
	"$(INTDIR)\win32thread.obj" \
	"$(INTDIR)\win32updatemanager.obj" \
	"$(INTDIR)\wincomsocket.obj" \
	"$(INTDIR)\ypclient.obj"

"..\fabaselib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "fabaselib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\fabaselib.lib"


CLEAN :
	-@erase "$(INTDIR)\apsconvert.obj"
	-@erase "$(INTDIR)\apsinterface.obj"
	-@erase "$(INTDIR)\apsplaylist.obj"
	-@erase "$(INTDIR)\comhttpsocket.obj"
	-@erase "$(INTDIR)\database.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\downloadmanager.obj"
	-@erase "$(INTDIR)\errors.obj"
	-@erase "$(INTDIR)\FAMetaUnit.obj"
	-@erase "$(INTDIR)\gencrc.obj"
	-@erase "$(INTDIR)\hosttonet.obj"
	-@erase "$(INTDIR)\Http.obj"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\missingfile.obj"
	-@erase "$(INTDIR)\musiccatalog.obj"
	-@erase "$(INTDIR)\mutex.obj"
	-@erase "$(INTDIR)\nettohost.obj"
	-@erase "$(INTDIR)\Parse.obj"
	-@erase "$(INTDIR)\player.obj"
	-@erase "$(INTDIR)\playlist.obj"
	-@erase "$(INTDIR)\preferences.obj"
	-@erase "$(INTDIR)\propimpl.obj"
	-@erase "$(INTDIR)\registrar.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\semaphore.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\timer.obj"
	-@erase "$(INTDIR)\undomanager.obj"
	-@erase "$(INTDIR)\updatemanager.obj"
	-@erase "$(INTDIR)\utility.obj"
	-@erase "$(INTDIR)\uuid.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\win32prefs.obj"
	-@erase "$(INTDIR)\win32thread.obj"
	-@erase "$(INTDIR)\win32updatemanager.obj"
	-@erase "$(INTDIR)\wincomsocket.obj"
	-@erase "$(INTDIR)\ypclient.obj"
	-@erase "..\fabaselib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "..\..\..\lib\http\include" /I "..\..\..\lib\zlib\include" /I "..\..\..\lib\xml\include" /I "..\..\..\lib\gdbm" /I "..\include" /I "..\..\include" /I "..\..\..\config" /I "..\..\..\ui\win32Test\include" /I "..\..\..\ui\win32Test\res" /I "..\..\..\io\include" /I "..\..\..\ui\include" /I "..\..\..\lmc\include" /I "..\..\aps" /I "..\..\..\lib\musicbrainz\lib" /I "..\..\..\lib\musicbrainz\expat\xmlparse" /I "..\..\..\lib\musicbrainz\expat\xmltok" /I "..\..\..\lib\musicbrainz" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\fabaselib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\fabaselib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\fabaselib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\apsconvert.obj" \
	"$(INTDIR)\apsinterface.obj" \
	"$(INTDIR)\apsplaylist.obj" \
	"$(INTDIR)\comhttpsocket.obj" \
	"$(INTDIR)\database.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\downloadmanager.obj" \
	"$(INTDIR)\errors.obj" \
	"$(INTDIR)\FAMetaUnit.obj" \
	"$(INTDIR)\gencrc.obj" \
	"$(INTDIR)\hosttonet.obj" \
	"$(INTDIR)\Http.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\missingfile.obj" \
	"$(INTDIR)\musiccatalog.obj" \
	"$(INTDIR)\mutex.obj" \
	"$(INTDIR)\nettohost.obj" \
	"$(INTDIR)\Parse.obj" \
	"$(INTDIR)\player.obj" \
	"$(INTDIR)\playlist.obj" \
	"$(INTDIR)\preferences.obj" \
	"$(INTDIR)\propimpl.obj" \
	"$(INTDIR)\registrar.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\semaphore.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\timer.obj" \
	"$(INTDIR)\undomanager.obj" \
	"$(INTDIR)\updatemanager.obj" \
	"$(INTDIR)\utility.obj" \
	"$(INTDIR)\uuid.obj" \
	"$(INTDIR)\win32prefs.obj" \
	"$(INTDIR)\win32thread.obj" \
	"$(INTDIR)\win32updatemanager.obj" \
	"$(INTDIR)\wincomsocket.obj" \
	"$(INTDIR)\ypclient.obj"

"..\fabaselib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "fabaselib - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\fabaselib.lib"


CLEAN :
	-@erase "$(INTDIR)\apsconvert.obj"
	-@erase "$(INTDIR)\apsinterface.obj"
	-@erase "$(INTDIR)\apsplaylist.obj"
	-@erase "$(INTDIR)\comhttpsocket.obj"
	-@erase "$(INTDIR)\database.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\downloadmanager.obj"
	-@erase "$(INTDIR)\errors.obj"
	-@erase "$(INTDIR)\FAMetaUnit.obj"
	-@erase "$(INTDIR)\gencrc.obj"
	-@erase "$(INTDIR)\hosttonet.obj"
	-@erase "$(INTDIR)\Http.obj"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\missingfile.obj"
	-@erase "$(INTDIR)\musiccatalog.obj"
	-@erase "$(INTDIR)\mutex.obj"
	-@erase "$(INTDIR)\nettohost.obj"
	-@erase "$(INTDIR)\Parse.obj"
	-@erase "$(INTDIR)\player.obj"
	-@erase "$(INTDIR)\playlist.obj"
	-@erase "$(INTDIR)\preferences.obj"
	-@erase "$(INTDIR)\propimpl.obj"
	-@erase "$(INTDIR)\registrar.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\semaphore.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\timer.obj"
	-@erase "$(INTDIR)\undomanager.obj"
	-@erase "$(INTDIR)\updatemanager.obj"
	-@erase "$(INTDIR)\utility.obj"
	-@erase "$(INTDIR)\uuid.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\win32prefs.obj"
	-@erase "$(INTDIR)\win32thread.obj"
	-@erase "$(INTDIR)\win32updatemanager.obj"
	-@erase "$(INTDIR)\wincomsocket.obj"
	-@erase "$(INTDIR)\ypclient.obj"
	-@erase "..\fabaselib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "..\..\..\lib\http\include" /I "..\..\..\lib\zlib\include" /I "..\..\..\lib\xml\include" /I "..\..\..\lib\gdbm" /I "..\include" /I "..\..\include" /I "..\..\..\config" /I "..\..\..\ui\win32Test\include" /I "..\..\..\ui\win32Test\res" /I "..\..\..\io\include" /I "..\..\..\ui\include" /I "..\..\..\lmc\include" /I "..\..\aps" /I "..\..\..\lib\musicbrainz\lib" /I "..\..\..\lib\musicbrainz\expat\xmlparse" /I "..\..\..\lib\musicbrainz\expat\xmltok" /I "..\..\..\lib\musicbrainz" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\fabaselib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\fabaselib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\fabaselib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\apsconvert.obj" \
	"$(INTDIR)\apsinterface.obj" \
	"$(INTDIR)\apsplaylist.obj" \
	"$(INTDIR)\comhttpsocket.obj" \
	"$(INTDIR)\database.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\downloadmanager.obj" \
	"$(INTDIR)\errors.obj" \
	"$(INTDIR)\FAMetaUnit.obj" \
	"$(INTDIR)\gencrc.obj" \
	"$(INTDIR)\hosttonet.obj" \
	"$(INTDIR)\Http.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\missingfile.obj" \
	"$(INTDIR)\musiccatalog.obj" \
	"$(INTDIR)\mutex.obj" \
	"$(INTDIR)\nettohost.obj" \
	"$(INTDIR)\Parse.obj" \
	"$(INTDIR)\player.obj" \
	"$(INTDIR)\playlist.obj" \
	"$(INTDIR)\preferences.obj" \
	"$(INTDIR)\propimpl.obj" \
	"$(INTDIR)\registrar.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\semaphore.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\timer.obj" \
	"$(INTDIR)\undomanager.obj" \
	"$(INTDIR)\updatemanager.obj" \
	"$(INTDIR)\utility.obj" \
	"$(INTDIR)\uuid.obj" \
	"$(INTDIR)\win32prefs.obj" \
	"$(INTDIR)\win32thread.obj" \
	"$(INTDIR)\win32updatemanager.obj" \
	"$(INTDIR)\wincomsocket.obj" \
	"$(INTDIR)\ypclient.obj"

"..\fabaselib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "fabaselib - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\config\config.h" "..\fabaselib.lib"


CLEAN :
	-@erase "$(INTDIR)\apsconvert.obj"
	-@erase "$(INTDIR)\apsinterface.obj"
	-@erase "$(INTDIR)\apsplaylist.obj"
	-@erase "$(INTDIR)\comhttpsocket.obj"
	-@erase "$(INTDIR)\database.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\downloadmanager.obj"
	-@erase "$(INTDIR)\errors.obj"
	-@erase "$(INTDIR)\FAMetaUnit.obj"
	-@erase "$(INTDIR)\gencrc.obj"
	-@erase "$(INTDIR)\hosttonet.obj"
	-@erase "$(INTDIR)\Http.obj"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\missingfile.obj"
	-@erase "$(INTDIR)\musiccatalog.obj"
	-@erase "$(INTDIR)\mutex.obj"
	-@erase "$(INTDIR)\nettohost.obj"
	-@erase "$(INTDIR)\Parse.obj"
	-@erase "$(INTDIR)\player.obj"
	-@erase "$(INTDIR)\playlist.obj"
	-@erase "$(INTDIR)\preferences.obj"
	-@erase "$(INTDIR)\propimpl.obj"
	-@erase "$(INTDIR)\registrar.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\semaphore.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\timer.obj"
	-@erase "$(INTDIR)\undomanager.obj"
	-@erase "$(INTDIR)\updatemanager.obj"
	-@erase "$(INTDIR)\utility.obj"
	-@erase "$(INTDIR)\uuid.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\win32prefs.obj"
	-@erase "$(INTDIR)\win32thread.obj"
	-@erase "$(INTDIR)\win32updatemanager.obj"
	-@erase "$(INTDIR)\wincomsocket.obj"
	-@erase "$(INTDIR)\ypclient.obj"
	-@erase "..\fabaselib.lib"
	-@erase "..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\lib\http\include" /I "..\..\..\lib\zlib\include" /I "..\..\..\lib\xml\include" /I "..\..\..\lib\gdbm" /I "..\include" /I "..\..\include" /I "..\..\..\config" /I "..\..\..\ui\win32Test\include" /I "..\..\..\ui\win32Test\res" /I "..\..\..\io\include" /I "..\..\..\ui\include" /I "..\..\..\lmc\include" /I "..\..\aps" /I "..\..\..\lib\musicbrainz\lib" /I "..\..\..\lib\musicbrainz\expat\xmlparse" /I "..\..\..\lib\musicbrainz\expat\xmltok" /I "..\..\..\lib\musicbrainz" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\fabaselib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\fabaselib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\fabaselib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\apsconvert.obj" \
	"$(INTDIR)\apsinterface.obj" \
	"$(INTDIR)\apsplaylist.obj" \
	"$(INTDIR)\comhttpsocket.obj" \
	"$(INTDIR)\database.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\downloadmanager.obj" \
	"$(INTDIR)\errors.obj" \
	"$(INTDIR)\FAMetaUnit.obj" \
	"$(INTDIR)\gencrc.obj" \
	"$(INTDIR)\hosttonet.obj" \
	"$(INTDIR)\Http.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\missingfile.obj" \
	"$(INTDIR)\musiccatalog.obj" \
	"$(INTDIR)\mutex.obj" \
	"$(INTDIR)\nettohost.obj" \
	"$(INTDIR)\Parse.obj" \
	"$(INTDIR)\player.obj" \
	"$(INTDIR)\playlist.obj" \
	"$(INTDIR)\preferences.obj" \
	"$(INTDIR)\propimpl.obj" \
	"$(INTDIR)\registrar.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\semaphore.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\timer.obj" \
	"$(INTDIR)\undomanager.obj" \
	"$(INTDIR)\updatemanager.obj" \
	"$(INTDIR)\utility.obj" \
	"$(INTDIR)\uuid.obj" \
	"$(INTDIR)\win32prefs.obj" \
	"$(INTDIR)\win32thread.obj" \
	"$(INTDIR)\win32updatemanager.obj" \
	"$(INTDIR)\wincomsocket.obj" \
	"$(INTDIR)\ypclient.obj"

"..\fabaselib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("fabaselib.dep")
!INCLUDE "fabaselib.dep"
!ELSE 
!MESSAGE Warning: cannot find "fabaselib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "fabaselib - Win32 Release" || "$(CFG)" == "fabaselib - Win32 Debug" || "$(CFG)" == "fabaselib - Win32 NASM Debug" || "$(CFG)" == "fabaselib - Win32 NASM Release"
SOURCE=..\..\aps\apsconvert.cpp

"$(INTDIR)\apsconvert.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\aps\apsinterface.cpp

"$(INTDIR)\apsinterface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\aps\apsplaylist.cpp

"$(INTDIR)\apsplaylist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\aps\comhttpsocket.cpp

"$(INTDIR)\comhttpsocket.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\config\config.win32

!IF  "$(CFG)" == "fabaselib - Win32 Release"

InputPath=..\..\..\config\config.win32

"..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\config\config.win32 ..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "fabaselib - Win32 Debug"

InputPath=..\..\..\config\config.win32

"..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\config\config.win32 ..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "fabaselib - Win32 NASM Debug"

InputPath=..\..\..\config\config.win32

"..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\config\config.win32 ..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "fabaselib - Win32 NASM Release"

InputPath=..\..\..\config\config.win32

"..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\config\config.win32 ..\..\..\config\config.h
<< 
	

!ENDIF 

SOURCE=..\..\src\database.cpp

"$(INTDIR)\database.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\debug.cpp

"$(INTDIR)\debug.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\downloadmanager.cpp

"$(INTDIR)\downloadmanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\errors.cpp

"$(INTDIR)\errors.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\aps\FAMetaUnit.cpp

"$(INTDIR)\FAMetaUnit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\aps\gencrc.cpp

"$(INTDIR)\gencrc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\aps\hosttonet.cpp

"$(INTDIR)\hosttonet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\http\src\Http.cpp

"$(INTDIR)\Http.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\log.cpp

"$(INTDIR)\log.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\missingfile.cpp

"$(INTDIR)\missingfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\musiccatalog.cpp

"$(INTDIR)\musiccatalog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\mutex.cpp

"$(INTDIR)\mutex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\aps\nettohost.cpp

"$(INTDIR)\nettohost.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\xml\src\Parse.cpp

"$(INTDIR)\Parse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\player.cpp

"$(INTDIR)\player.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\playlist.cpp

"$(INTDIR)\playlist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\preferences.cpp

"$(INTDIR)\preferences.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\propimpl.cpp

"$(INTDIR)\propimpl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\registrar.cpp

"$(INTDIR)\registrar.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\registry.cpp

"$(INTDIR)\registry.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\semaphore.cpp

"$(INTDIR)\semaphore.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\thread.cpp

"$(INTDIR)\thread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\timer.cpp

"$(INTDIR)\timer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\undomanager.cpp

"$(INTDIR)\undomanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\updatemanager.cpp

"$(INTDIR)\updatemanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\utility.cpp

"$(INTDIR)\utility.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\aps\uuid.cpp

"$(INTDIR)\uuid.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\win32prefs.cpp

"$(INTDIR)\win32prefs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\win32thread.cpp

"$(INTDIR)\win32thread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\win32updatemanager.cpp

"$(INTDIR)\win32updatemanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\aps\wincomsocket.cpp

"$(INTDIR)\wincomsocket.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\aps\ypclient.cpp

"$(INTDIR)\ypclient.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

