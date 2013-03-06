# Microsoft Developer Studio Project File - Name="vorbis" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=vorbis - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vorbis.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vorbis.mak" CFG="vorbis - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vorbis - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vorbis - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vorbis - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "vorbis - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vorbis - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lib\vorbis" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 fabaselib.lib version.lib gdbm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ole32.lib shell32.lib /nologo /base:"0x11800000" /subsystem:windows /dll /machine:I386 /out:"vorbis.lmc" /libpath:"..\..\..\lib\gdbm" /libpath:"..\..\..\lib\zlib" /libpath:"..\\" /libpath:"..\..\..\lib\musicbrainz" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lmc\vorbis\win32" /libpath:"..\..\..\..\lib\musicbrainz"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                    ..\..\..\..\base\win32\prj\plugins	copy vorbis.lmc                            ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "vorbis - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lib\vorbis" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 fabaselib.lib version.lib gdbm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ole32.lib shell32.lib /nologo /base:"0x11800000" /subsystem:windows /dll /debug /machine:I386 /out:"vorbis.lmc" /pdbtype:sept /libpath:"..\..\..\lib\gdbm" /libpath:"..\..\..\lib\zlib" /libpath:"..\\" /libpath:"..\..\..\lib\musicbrainz" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lmc\vorbis\win32" /libpath:"..\..\..\..\lib\musicbrainz"
# SUBTRACT LINK32 /profile /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                    ..\..\..\..\base\win32\prj\plugins	copy vorbis.lmc                            ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "vorbis - Win32 NASM Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vorbis___W"
# PROP BASE Intermediate_Dir "vorbis___W"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lib\vorbis" /D "NDEBUG" /D "_WINDOWS" /D "ASM_X86" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"vorbis.lmc"
# ADD LINK32 shell32.lib fabaselib.lib version.lib gdbm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ole32.lib /nologo /base:"0x11800000" /subsystem:windows /dll /machine:I386 /out:"vorbis.lmc" /libpath:"..\..\..\lib\gdbm" /libpath:"..\..\..\lib\zlib" /libpath:"..\\" /libpath:"..\..\..\lib\musicbrainz" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lmc\vorbis\win32" /libpath:"..\..\..\..\lib\musicbrainz"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                    ..\..\..\..\base\win32\prj\plugins	copy vorbis.lmc                            ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "vorbis - Win32 NASM Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vorbis___0"
# PROP BASE Intermediate_Dir "vorbis___0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lib\vorbis" /D "_DEBUG" /D "_WINDOWS" /D "ASM_X86" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"vorbis.lmc" /pdbtype:sept
# SUBTRACT BASE LINK32 /profile /nodefaultlib
# ADD LINK32 fabaselib.lib version.lib gdbm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ole32.lib shell32.lib /nologo /base:"0x11800000" /subsystem:windows /dll /debug /machine:I386 /out:"vorbis.lmc" /pdbtype:sept /libpath:"..\..\..\..\lib\musicbrainz" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lmc\vorbis\win32"
# SUBTRACT LINK32 /profile /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                    ..\..\..\..\base\win32\prj\plugins	copy vorbis.lmc                            ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "vorbis - Win32 Release"
# Name "vorbis - Win32 Debug"
# Name "vorbis - Win32 NASM Release"
# Name "vorbis - Win32 NASM Debug"
# Begin Group "general source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\config\config.win32

!IF  "$(CFG)" == "vorbis - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "vorbis - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "vorbis - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "vorbis - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\io\src\eventbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\src\log.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\io\src\pipeline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\io\src\pullbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\win32\src\semaphore.cpp
# End Source File
# Begin Source File

SOURCE=.\vorbis.def
# End Source File
# Begin Source File

SOURCE=..\res\vorbis.rc
# End Source File
# Begin Source File

SOURCE=..\..\src\vorbislmc.cpp
# ADD CPP /I "..\include\vorbis"
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\lib\vorbis\vorbisfile.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\vorbis\vorbis.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\vorbis\ogg.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\musicbrainz\musicbrainz.lib
# End Source File
# End Target
# End Project
