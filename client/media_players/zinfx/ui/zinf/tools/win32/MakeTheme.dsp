# Microsoft Developer Studio Project File - Name="MakeTheme" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=MakeTheme - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MakeTheme.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MakeTheme.mak" CFG="MakeTheme - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MakeTheme - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "MakeTheme - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "MakeTheme - Win32 NASM Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "MakeTheme - Win32 NASM Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MakeTheme - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../include" /I "../../../../lib/zlib/include" /I "../../../../lib/unzip/include" /I "../../../../config" /I "../../../../base/include" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 zlib.lib unzip.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"MakeTheme.exe" /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\lib\unzip"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\tools mkdir                                                    ..\..\..\..\base\win32\prj\tools	copy MakeTheme.exe     ..\..\..\..\base\win32\prj\tools	copy ..\..\howto\ThemeHowTo.txt     ..\..\..\..\base\win32\prj\tools
# End Special Build Tool

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../../../lib/zlib/include" /I "../../../../lib/unzip/include" /I "../../../../config" /I "../../../../base/include" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib zlib.lib unzip.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"MakeTheme.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\lib\unzip"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\tools mkdir                                                    ..\..\..\..\base\win32\prj\tools	copy MakeTheme.exe     ..\..\..\..\base\win32\prj\tools	copy ..\..\howto\ThemeHowTo.txt     ..\..\..\..\base\win32\prj\tools
# End Special Build Tool

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 NASM Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MakeThem"
# PROP BASE Intermediate_Dir "MakeThem"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../include" /I "../../../../lib/zlib/include" /I "../../../../config" /I "../../../../base/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../../../lib/zlib/include" /I "../../../../lib/unzip/include" /I "../../../../config" /I "../../../../base/include" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib zlib.lib unzip.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"MakeTheme.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\lib\unzip"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\tools mkdir                                                    ..\..\..\..\base\win32\prj\tools	copy MakeTheme.exe     ..\..\..\..\base\win32\prj\tools	copy ..\..\howto\ThemeHowTo.txt     ..\..\..\..\base\win32\prj\tools
# End Special Build Tool

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 NASM Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../../../lib/zlib/include" /I "../../../../lib/unzip/include" /I "../../../../config" /I "../../../../base/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_DEBUG" /D "NOMINMAX" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../../../lib/zlib/include" /I "../../../../lib/unzip/include" /I "../../../../config" /I "../../../../base/include" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "NOMINMAX" /D "_DEBUG" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_DEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  wsock32.lib zlib.lib unzip.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"MakeTheme.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\lib\unzip"
# ADD LINK32 zlib.lib unzip.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  wsock32.lib zlib.lib unzip.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"MakeTheme.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\zlib" /libpath:"..\..\..\..\lib\unzip"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\tools mkdir                                                    ..\..\..\..\base\win32\prj\tools	copy MakeTheme.exe     ..\..\..\..\base\win32\prj\tools	copy ..\..\howto\ThemeHowTo.txt     ..\..\..\..\base\win32\prj\tools
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "MakeTheme - Win32 Release"
# Name "MakeTheme - Win32 Debug"
# Name "MakeTheme - Win32 NASM Debug"
# Name "MakeTheme - Win32 NASM Release"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\config\config.win32

!IF  "$(CFG)" == "MakeTheme - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "MakeTheme - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\MakeTheme.cpp
# End Source File
# Begin Source File

SOURCE=.\MakeTheme.rc
# End Source File
# Begin Source File

SOURCE=..\..\src\ThemeZip.cpp
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\..\..\..\..\..\include\algobase.h
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\config\config.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\include\stl_config.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ThemeZip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\include\type_traits.h
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
