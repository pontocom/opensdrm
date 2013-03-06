# Microsoft Developer Studio Project File - Name="fabaselib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=fabaselib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fabaselib.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fabaselib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\lib\http\include" /I "..\..\..\lib\zlib\include" /I "..\..\..\lib\xml\include" /I "..\..\..\lib\gdbm" /I "..\include" /I "..\..\include" /I "..\..\..\config" /I "..\..\..\ui\win32Test\include" /I "..\..\..\ui\win32Test\res" /I "..\..\..\io\include" /I "..\..\..\ui\include" /I "..\..\..\lmc\include" /I "..\..\aps" /I "..\..\..\lib\musicbrainz\lib" /I "..\..\..\lib\musicbrainz\expat\xmlparse" /I "..\..\..\lib\musicbrainz\expat\xmltok" /I "..\..\..\lib\musicbrainz" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\fabaselib.lib"

!ELSEIF  "$(CFG)" == "fabaselib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "..\..\..\lib\http\include" /I "..\..\..\lib\zlib\include" /I "..\..\..\lib\xml\include" /I "..\..\..\lib\gdbm" /I "..\include" /I "..\..\include" /I "..\..\..\config" /I "..\..\..\ui\win32Test\include" /I "..\..\..\ui\win32Test\res" /I "..\..\..\io\include" /I "..\..\..\ui\include" /I "..\..\..\lmc\include" /I "..\..\aps" /I "..\..\..\lib\musicbrainz\lib" /I "..\..\..\lib\musicbrainz\expat\xmlparse" /I "..\..\..\lib\musicbrainz\expat\xmltok" /I "..\..\..\lib\musicbrainz" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\fabaselib.lib"

!ELSEIF  "$(CFG)" == "fabaselib - Win32 NASM Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "fabaseli"
# PROP BASE Intermediate_Dir "fabaseli"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /Z7 /Od /I "..\..\..\lib\zlib\include" /I "..\..\..\lib\xml\include" /I "..\..\..\lib\gdbm" /I "..\include" /I "..\..\include" /I "..\..\..\config" /I "..\..\..\ui\win32Test\include" /I "..\..\..\ui\win32Test\res" /I "..\..\..\io\include" /I "..\..\..\ui\include" /I "..\..\..\lmc\include" /I "..\..\aps" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "..\..\..\lib\http\include" /I "..\..\..\lib\zlib\include" /I "..\..\..\lib\xml\include" /I "..\..\..\lib\gdbm" /I "..\include" /I "..\..\include" /I "..\..\..\config" /I "..\..\..\ui\win32Test\include" /I "..\..\..\ui\win32Test\res" /I "..\..\..\io\include" /I "..\..\..\ui\include" /I "..\..\..\lmc\include" /I "..\..\aps" /I "..\..\..\lib\musicbrainz\lib" /I "..\..\..\lib\musicbrainz\expat\xmlparse" /I "..\..\..\lib\musicbrainz\expat\xmltok" /I "..\..\..\lib\musicbrainz" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"fabaselib.lib"
# ADD LIB32 /nologo /out:"..\fabaselib.lib"

!ELSEIF  "$(CFG)" == "fabaselib - Win32 NASM Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "fabasel0"
# PROP BASE Intermediate_Dir "fabasel0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\lib\zlib\include" /I "..\..\..\lib\xml\include" /I "..\..\..\lib\gdbm" /I "..\include" /I "..\..\include" /I "..\..\..\config" /I "..\..\..\ui\win32Test\include" /I "..\..\..\ui\win32Test\res" /I "..\..\..\io\include" /I "..\..\..\ui\include" /I "..\..\..\lmc\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\lib\http\include" /I "..\..\..\lib\zlib\include" /I "..\..\..\lib\xml\include" /I "..\..\..\lib\gdbm" /I "..\include" /I "..\..\include" /I "..\..\..\config" /I "..\..\..\ui\win32Test\include" /I "..\..\..\ui\win32Test\res" /I "..\..\..\io\include" /I "..\..\..\ui\include" /I "..\..\..\lmc\include" /I "..\..\aps" /I "..\..\..\lib\musicbrainz\lib" /I "..\..\..\lib\musicbrainz\expat\xmlparse" /I "..\..\..\lib\musicbrainz\expat\xmltok" /I "..\..\..\lib\musicbrainz" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"fabaselib.lib"
# ADD LIB32 /nologo /out:"..\fabaselib.lib"

!ENDIF 

# Begin Target

# Name "fabaselib - Win32 Release"
# Name "fabaselib - Win32 Debug"
# Name "fabaselib - Win32 NASM Debug"
# Name "fabaselib - Win32 NASM Release"
# Begin Group "src"

# PROP Default_Filter "c;cpp"
# Begin Source File

SOURCE=..\..\..\config\config.win32

!IF  "$(CFG)" == "fabaselib - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\config\config.win32

"..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\config\config.win32 ..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "fabaselib - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\config\config.win32

"..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\config\config.win32 ..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "fabaselib - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\config\config.win32

"..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\config\config.win32 ..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "fabaselib - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\config\config.win32

"..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\config\config.win32 ..\..\..\config\config.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\database.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\downloadmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\errors.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\http\src\Http.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\log.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\missingfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\musiccatalog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\mutex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\xml\src\Parse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\player.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\playlist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\preferences.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\propimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\registrar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\registry.cpp
# End Source File
# Begin Source File

SOURCE=..\src\semaphore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\thread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\timer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\undomanager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utility.cpp
# End Source File
# Begin Source File

SOURCE=..\src\win32prefs.cpp
# End Source File
# Begin Source File

SOURCE=..\src\win32thread.cpp
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\..\..\..\..\include\algobase.h
# End Source File
# Begin Source File

SOURCE=..\..\aps\aps.h
# End Source File
# Begin Source File

SOURCE=..\..\aps\apsconvert.h
# End Source File
# Begin Source File

SOURCE=..\..\aps\apsmetadata.h
# End Source File
# Begin Source File

SOURCE=..\..\aps\apsplaylist.h
# End Source File
# Begin Source File

SOURCE=..\..\aps\apsutility.h
# End Source File
# Begin Source File

SOURCE=..\..\aps\automutex.h
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\musicbrainz\musicbrainz\bitprint.h
# End Source File
# Begin Source File

SOURCE=..\..\aps\comhttpsocket.h
# End Source File
# Begin Source File

SOURCE=..\..\..\config\config.h
# End Source File
# Begin Source File

SOURCE=..\..\include\database.h
# End Source File
# Begin Source File

SOURCE=..\..\include\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\include\downloadformat.h
# End Source File
# Begin Source File

SOURCE=..\..\include\downloadmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\event.h
# End Source File
# Begin Source File

SOURCE=..\..\include\eventdata.h
# End Source File
# Begin Source File

SOURCE=..\..\include\facontext.h
# End Source File
# Begin Source File

SOURCE=..\..\aps\FAMetaUnit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\gdbm\gdbm_fa.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\include\hashtable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\include\heap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\http\include\Http.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lmc\include\lmc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\log.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\musicbrainz\musicbrainz\mb_c.h
# End Source File
# Begin Source File

SOURCE=..\..\include\metadata.h
# End Source File
# Begin Source File

SOURCE=..\..\include\missingfile.h
# End Source File
# Begin Source File

SOURCE=..\..\include\musiccatalog.h
# End Source File
# Begin Source File

SOURCE=..\include\mutex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\xml\include\Parse.h
# End Source File
# Begin Source File

SOURCE=..\..\..\io\include\pipeline.h
# End Source File
# Begin Source File

SOURCE=..\..\include\player.h
# End Source File
# Begin Source File

SOURCE=..\..\include\playlist.h
# End Source File
# Begin Source File

SOURCE=..\..\include\playlistformat.h
# End Source File
# Begin Source File

SOURCE=..\..\include\plmevent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\io\include\pmi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\io\include\pmo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\portabledevice.h
# End Source File
# Begin Source File

SOURCE=..\..\include\preferences.h
# End Source File
# Begin Source File

SOURCE=..\..\include\properties.h
# End Source File
# Begin Source File

SOURCE=..\..\include\propimpl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\io\include\pullbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\musicbrainz\musicbrainz\queries.h
# End Source File
# Begin Source File

SOURCE=..\..\include\registrar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\registry.h
# End Source File
# Begin Source File

SOURCE=..\include\semaphore.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\include\stl_config.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\include\tempbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\include\thread.h
# End Source File
# Begin Source File

SOURCE=..\..\include\timer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\include\tree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\include\type_traits.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ui\include\ui.h
# End Source File
# Begin Source File

SOURCE=..\..\include\undomanager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\updatemanager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\utility.h
# End Source File
# Begin Source File

SOURCE=..\..\aps\uuid.h
# End Source File
# Begin Source File

SOURCE=..\include\win32prefs.h
# End Source File
# Begin Source File

SOURCE=..\include\win32thread.h
# End Source File
# Begin Source File

SOURCE=..\include\win32updatemanager.h
# End Source File
# Begin Source File

SOURCE=..\..\aps\wincomsocket.h
# End Source File
# Begin Source File

SOURCE=..\..\aps\YPClient.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\zlib\include\zconf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\zlib\include\zlib.h
# End Source File
# End Group
# End Target
# End Project
