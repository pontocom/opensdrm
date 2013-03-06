# Microsoft Developer Studio Project File - Name="zinfui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=zinfui - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zinfui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zinfui.mak" CFG="zinfui - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zinfui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "zinfui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "zinfui - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "zinfui - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "zinfui - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\res" /I "..\include" /I "..\..\include" /I "..\..\..\..\base\aps" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lib\xml\include" /I "..\..\..\..\lib\zlib\include" /I "..\..\..\..\lib\unzip\include" /I "..\..\..\..\lib\http\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 fabaselib.lib zlib.lib unzip.lib gdbm.lib shlwapi.lib version.lib wsock32.lib winmm.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x12000000" /subsystem:windows /dll /machine:I386 /out:"zinf.ui" /libpath:"..\..\..\..\lib\unzip" /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\musicbrainz"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                          ..\..\..\..\base\win32\prj\plugins	copy zinf.ui                                         ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "zinfui - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\base\aps" /I "..\res" /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lib\xml\include" /I "..\..\..\..\lib\zlib\include" /I "..\..\..\..\lib\unzip\include" /I "..\..\..\..\lib\http\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 fabaselib.lib zlib.lib unzip.lib gdbm.lib shlwapi.lib version.lib wsock32.lib winmm.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x12000000" /subsystem:windows /dll /debug /machine:I386 /out:"zinf.ui" /pdbtype:sept /libpath:"..\..\..\..\lib\unzip" /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\musicbrainz"
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                          ..\..\..\..\base\win32\prj\plugins	copy zinf.ui                                         ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "zinfui - Win32 NASM Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "freeampu"
# PROP BASE Intermediate_Dir "freeampu"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\res" /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\base\aps" /I "..\res" /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lib\xml\include" /I "..\..\..\..\lib\zlib\include" /I "..\..\..\..\lib\unzip\include" /I "..\..\..\..\lib\http\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"freeamp.ui" /pdbtype:sept
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 fabaselib.lib zlib.lib unzip.lib gdbm.lib shlwapi.lib version.lib wsock32.lib winmm.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x12000000" /subsystem:windows /dll /debug /machine:I386 /out:"zinf.ui" /pdbtype:sept /libpath:"..\..\..\..\lib\unzip" /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\musicbrainz"
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                          ..\..\..\..\base\win32\prj\plugins	copy zinf.ui                                         ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "zinfui - Win32 NASM Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "freeamp0"
# PROP BASE Intermediate_Dir "freeamp0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\res" /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\..\..\base\aps" /I "..\res" /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lib\xml\include" /I "..\..\..\..\lib\zlib\include" /I "..\..\..\..\lib\unzip\include" /I "..\..\..\..\lib\http\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"freeamp.ui"
# ADD LINK32 fabaselib.lib zlib.lib unzip.lib gdbm.lib shlwapi.lib version.lib wsock32.lib winmm.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x12000000" /subsystem:windows /dll /machine:I386 /out:"zinf.ui" /libpath:"..\..\..\..\lib\unzip" /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\musicbrainz"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                          ..\..\..\..\base\win32\prj\plugins	copy zinf.ui                                         ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "zinfui - Win32 Release"
# Name "zinfui - Win32 Debug"
# Name "zinfui - Win32 NASM Debug"
# Name "zinfui - Win32 NASM Release"
# Begin Group "source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Bitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ButtonControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Canvas.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\config\config.win32

!IF  "$(CFG)" == "zinfui - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "zinfui - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "zinfui - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "zinfui - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\Control.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DialControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Equalizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Font.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ForeignTheme.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Headlines.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\http\src\Http.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Median.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\MultiStateControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Panel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\xml\src\Parse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\PixFontControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\PixSliderControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\PixTimeControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\SliderControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\TextControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Theme.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ThemeManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ThemeZip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\VSliderControl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Win32Bitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Win32Canvas.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Win32Font.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Win32MessageDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Win32PreferenceWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Win32Window.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Window.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ZinfTheme.cpp
# End Source File
# Begin Source File

SOURCE=.\zinfui.def
# End Source File
# End Group
# Begin Group "resource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\res\checked.bmp
# End Source File
# Begin Source File

SOURCE=..\res\folder.bmp
# End Source File
# Begin Source File

SOURCE=..\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=..\res\info.ico
# End Source File
# Begin Source File

SOURCE=..\res\unchecked.bmp
# End Source File
# Begin Source File

SOURCE=..\res\zinfui.rc
# End Source File
# End Group
# Begin Group "includes"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# Begin Source File

SOURCE=..\..\include\Bitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ButtonControl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Canvas.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\config\config.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Control.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\database.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\include\DialControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\downloadformat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\downloadmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Equalizer.h
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

SOURCE=..\..\include\Font.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ForeignTheme.h
# End Source File
# Begin Source File

SOURCE=..\..\include\FreeAmpTheme.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\gdbm\gdbm_fa.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Headlines.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\win32\include\help.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\http\include\Http.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lmc\include\lmc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\log.h
# End Source File
# Begin Source File

SOURCE=..\include\Median.h
# End Source File
# Begin Source File

SOURCE=..\..\include\MessageDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\metadata.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\missingfile.h
# End Source File
# Begin Source File

SOURCE=..\..\include\MultiStateControl.h
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

SOURCE=..\..\include\PixFontControl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\PixTimeControl.h
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

SOURCE=..\..\..\..\base\include\portabledevice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\preferences.h
# End Source File
# Begin Source File

SOURCE=..\..\include\PreferenceWindow.h
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

SOURCE=..\..\..\..\base\include\queue.h
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

SOURCE=..\..\include\SliderControl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\TextControl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Theme.h
# End Source File
# Begin Source File

SOURCE=..\..\include\themeformat.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ThemeManager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ThemeZip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\timer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ui.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\undomanager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\updatemanager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\utility.h
# End Source File
# Begin Source File

SOURCE=..\..\include\VSliderControl.h
# End Source File
# Begin Source File

SOURCE=..\include\Win32Bitmap.h
# End Source File
# Begin Source File

SOURCE=..\include\Win32Canvas.h
# End Source File
# Begin Source File

SOURCE=..\include\Win32Font.h
# End Source File
# Begin Source File

SOURCE=..\include\Win32PreferenceWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\win32\include\win32updatemanager.h
# End Source File
# Begin Source File

SOURCE=..\include\Win32Window.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Window.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\zlib\include\zconf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\unzip\include\zip_header.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\unzip\include\zip_inflate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\unzip\include\zip_testcrc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\unzip\include\zip_utils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\unzip\include\zip_wrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\zlib\include\zlib.h
# End Source File
# End Group
# End Target
# End Project
