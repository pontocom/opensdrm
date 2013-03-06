# Microsoft Developer Studio Project File - Name="musicbrowser" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=musicbrowser - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "musicbrowser.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "musicbrowser.mak" CFG="musicbrowser - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "musicbrowser - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "musicbrowser - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "musicbrowser - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "musicbrowser - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "musicbrowser - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\base\aps" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\res" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\include" /I "..\..\..\..\lib\http\include" /I "..\..\..\..\lib\xml\include" /I "..\..\..\..\lib\musicbrainz" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 winmm.lib fabaselib.lib gdbm.lib comctl32.lib version.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x10900000" /subsystem:windows /dll /machine:I386 /out:"musicbrowser.ui" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\musicbrainz"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                                                                     ..\..\..\..\base\win32\prj\plugins	copy musicbrowser.ui                                                                                       ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\base\aps" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\res" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\include" /I "..\..\..\..\lib\http\include" /I "..\..\..\..\lib\xml\include" /I "..\..\..\..\lib\musicbrainz" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib fabaselib.lib gdbm.lib comctl32.lib version.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x10900000" /subsystem:windows /dll /debug /machine:I386 /def:".\musicbrowser.def" /out:"musicbrowser.ui" /pdbtype:sept /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\musicbrainz"
# SUBTRACT LINK32 /pdb:none /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                                                                     ..\..\..\..\base\win32\prj\plugins	copy musicbrowser.ui                                                                                       ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 NASM Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "musicbro"
# PROP BASE Intermediate_Dir "musicbro"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\res" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"Debug/musicbrowser.pch" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\base\aps" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\res" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\include" /I "..\..\..\..\lib\http\include" /I "..\..\..\..\lib\xml\include" /I "..\..\..\..\lib\musicbrainz" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib gdbm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\musicbrowser.def" /out:"musicbrowser.ui" /pdbtype:sept /libpath:"..\..\..\..\lib\gdbm"
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 winmm.lib fabaselib.lib gdbm.lib comctl32.lib version.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x10900000" /subsystem:windows /dll /debug /machine:I386 /def:".\musicbrowser.def" /out:"musicbrowser.ui" /pdbtype:sept /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\musicbrainz"
# SUBTRACT LINK32 /pdb:none /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                                                                     ..\..\..\..\base\win32\prj\plugins	copy musicbrowser.ui                                                                                       ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 NASM Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "musicbr0"
# PROP BASE Intermediate_Dir "musicbr0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\res" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\base\aps" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\res" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\include" /I "..\..\..\..\lib\http\include" /I "..\..\..\..\lib\xml\include" /I "..\..\..\..\lib\musicbrainz" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 gdbm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"musicbrowser.ui" /libpath:"..\..\..\..\lib\gdbm"
# ADD LINK32 winmm.lib fabaselib.lib gdbm.lib comctl32.lib version.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x10900000" /subsystem:windows /dll /machine:I386 /out:"musicbrowser.ui" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\musicbrainz"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                                                                     ..\..\..\..\base\win32\prj\plugins	copy musicbrowser.ui                                                                                       ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "musicbrowser - Win32 Release"
# Name "musicbrowser - Win32 Debug"
# Name "musicbrowser - Win32 NASM Debug"
# Name "musicbrowser - Win32 NASM Release"
# Begin Group "source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\config\config.win32

!IF  "$(CFG)" == "musicbrowser - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\DataIndex.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Dialog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\DropObject.cpp
# End Source File
# Begin Source File

SOURCE=..\src\DropSource.cpp
# End Source File
# Begin Source File

SOURCE=..\src\DropTarget.cpp
# End Source File
# Begin Source File

SOURCE=..\src\EditTrackInfoDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\EnumFormatEtc.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Event.cpp
# End Source File
# Begin Source File

SOURCE=..\src\FavoriteDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\IntroductionWizard.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MissingFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MultiSelectTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\musicbrowser.def

!IF  "$(CFG)" == "musicbrowser - Win32 Release"

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 NASM Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 NASM Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\MusicSearchDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MusicTree.cpp
# End Source File
# Begin Source File

SOURCE=..\src\OpenSavePlaylists.cpp
# End Source File
# Begin Source File

SOURCE=..\src\PerformDBCheckDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\PlaylistView.cpp
# End Source File
# Begin Source File

SOURCE=..\src\RemoveTracksDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\SavePlaylistDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Win32MusicBrowser.cpp
# End Source File
# End Group
# Begin Group "resource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\res\active.ico
# End Source File
# Begin Source File

SOURCE=..\res\album.bmp
# End Source File
# Begin Source File

SOURCE=..\res\all.bmp
# End Source File
# Begin Source File

SOURCE=..\res\arrow.ico
# End Source File
# Begin Source File

SOURCE=..\res\artist.bmp
# End Source File
# Begin Source File

SOURCE=..\res\blank.ico
# End Source File
# Begin Source File

SOURCE=..\res\busy.bmp
# End Source File
# Begin Source File

SOURCE=..\res\catalog.bmp
# End Source File
# Begin Source File

SOURCE=..\res\cd.bmp
# End Source File
# Begin Source File

SOURCE=..\res\closed.ico
# End Source File
# Begin Source File

SOURCE=..\res\drag.cur
# End Source File
# Begin Source File

SOURCE=..\res\favorites.bmp
# End Source File
# Begin Source File

SOURCE=..\res\icecast.bmp
# End Source File
# Begin Source File

SOURCE=..\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=..\res\musicbrowser.rc
# End Source File
# Begin Source File

SOURCE=..\res\nodrop.cur
# End Source File
# Begin Source File

SOURCE=..\res\playing.bmp
# End Source File
# Begin Source File

SOURCE=..\res\playlist.bmp
# End Source File
# Begin Source File

SOURCE=..\res\portable.bmp
# End Source File
# Begin Source File

SOURCE=..\res\relatable.bmp
# End Source File
# Begin Source File

SOURCE=..\res\shoutcast.bmp
# End Source File
# Begin Source File

SOURCE=..\res\splitter.cur
# End Source File
# Begin Source File

SOURCE=..\res\streams.bmp
# End Source File
# Begin Source File

SOURCE=..\res\toolbar.bmp
# End Source File
# Begin Source File

SOURCE=..\res\track.bmp
# End Source File
# Begin Source File

SOURCE=..\res\uncatagorized.bmp
# End Source File
# Begin Source File

SOURCE=..\res\warn.ico
# End Source File
# Begin Source File

SOURCE=..\res\wiredplanet.bmp
# End Source File
# Begin Source File

SOURCE=..\res\wizard.bmp
# End Source File
# Begin Source File

SOURCE=..\res\wizard_small.bmp
# End Source File
# End Group
# Begin Group "includes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\..\include\algobase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\aps\aps.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\aps\apsmetadata.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\aps\apsplaylist.h
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# Begin Source File

SOURCE=..\include\ColumnInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\config\config.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\database.h
# End Source File
# Begin Source File

SOURCE=..\include\DataIndex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\downloadformat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\downloadmanager.h
# End Source File
# Begin Source File

SOURCE=..\include\DropObject.h
# End Source File
# Begin Source File

SOURCE=..\include\DropSource.h
# End Source File
# Begin Source File

SOURCE=..\include\DropTarget.h
# End Source File
# Begin Source File

SOURCE=..\include\EditTrackInfoDialog.h
# End Source File
# Begin Source File

SOURCE=..\include\EnumFormatEtc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\errors.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\event.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\eventdata.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\facontext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\aps\FAMetaUnit.h
# End Source File
# Begin Source File

SOURCE=..\include\FavoriteDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\include\FreeAmpStreams.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\gdbm\gdbm_fa.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\include\hashtable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\include\heap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\help.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\win32\include\help.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\http\include\Http.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Icecast.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lmc\include\lmc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\log.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\metadata.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\missingfile.h
# End Source File
# Begin Source File

SOURCE=..\include\MissingFileDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\musiccatalog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\win32\include\mutex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\xml\include\Parse.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\io\include\pipeline.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\player.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\playlist.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\playlistformat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\plmevent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\io\include\pmi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\io\include\pmo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\portabledevice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\preferences.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\properties.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\propimpl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\io\include\pullbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\registrar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\registry.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\win32\include\semaphore.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\include\stl_config.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\include\tempbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\timer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\include\tree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\include\type_traits.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ui.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\undomanager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\utility.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\aps\uuid.h
# End Source File
# Begin Source File

SOURCE=..\include\Win32MusicBrowser.h
# End Source File
# End Group
# End Target
# End Project
