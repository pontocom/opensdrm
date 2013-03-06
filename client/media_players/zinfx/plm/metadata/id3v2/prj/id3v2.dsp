# Microsoft Developer Studio Project File - Name="id3v2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=id3v2 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "id3v2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "id3v2.mak" CFG="id3v2 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "id3v2 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "id3v2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "id3v2 - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "id3v2 - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "id3v2 - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10600000" /subsystem:windows /dll /machine:I386 /out:"id3v2.mdf" /libpath:"..\..\..\..\base\win32"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                ..\..\..\..\base\win32\prj\plugins	copy id3v2.mdf       ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "id3v2 - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10600000" /subsystem:windows /dll /debug /machine:I386 /out:"id3v2.mdf" /pdbtype:sept /libpath:"..\..\..\..\base\win32"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                ..\..\..\..\base\win32\prj\plugins	copy id3v2.mdf       ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "id3v2 - Win32 NASM Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "id3v2__0"
# PROP BASE Intermediate_Dir "id3v2__0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"id3v2.mdf"
# ADD LINK32 fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10600000" /subsystem:windows /dll /machine:I386 /out:"id3v2.mdf" /libpath:"..\..\..\..\base\win32"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                ..\..\..\..\base\win32\prj\plugins	copy id3v2.mdf       ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "id3v2 - Win32 NASM Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /I "..\..\..\..\lib\id3\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "NOMINMAX" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10600000" /subsystem:windows /dll /debug /machine:I386 /out:"id3v2.mdf" /pdbtype:sept /libpath:"..\..\..\..\base\win32"
# ADD LINK32 fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib fabaselib.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x10600000" /subsystem:windows /dll /debug /machine:I386 /out:"id3v2.mdf" /pdbtype:sept /libpath:"..\..\..\..\base\win32"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                ..\..\..\..\base\win32\prj\plugins	copy id3v2.mdf       ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "id3v2 - Win32 Release"
# Name "id3v2 - Win32 Debug"
# Name "id3v2 - Win32 NASM Release"
# Name "id3v2 - Win32 NASM Debug"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\c_wrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\field.cpp
# End Source File
# Begin Source File

SOURCE=..\field_binary.cpp
# End Source File
# Begin Source File

SOURCE=..\field_integer.cpp
# End Source File
# Begin Source File

SOURCE=..\field_string_ascii.cpp
# End Source File
# Begin Source File

SOURCE=..\field_string_unicode.cpp
# End Source File
# Begin Source File

SOURCE=..\frame.cpp
# End Source File
# Begin Source File

SOURCE=..\frame_impl.cpp
# End Source File
# Begin Source File

SOURCE=..\frame_parse.cpp
# End Source File
# Begin Source File

SOURCE=..\frame_render.cpp
# End Source File
# Begin Source File

SOURCE=..\header.cpp
# End Source File
# Begin Source File

SOURCE=..\header_frame.cpp
# End Source File
# Begin Source File

SOURCE=..\header_tag.cpp
# End Source File
# Begin Source File

SOURCE=..\helpers.cpp
# End Source File
# Begin Source File

SOURCE=..\id3v2.cpp
# End Source File
# Begin Source File

SOURCE=.\id3v2.def
# End Source File
# Begin Source File

SOURCE=..\io.cpp
# End Source File
# Begin Source File

SOURCE=..\io_decorators.cpp
# End Source File
# Begin Source File

SOURCE=..\io_helpers.cpp
# End Source File
# Begin Source File

SOURCE=..\misc_support.cpp
# End Source File
# Begin Source File

SOURCE=..\readers.cpp
# End Source File
# Begin Source File

SOURCE=..\spec.cpp
# End Source File
# Begin Source File

SOURCE=..\tag.cpp
# End Source File
# Begin Source File

SOURCE=..\tag_file.cpp
# End Source File
# Begin Source File

SOURCE=..\tag_find.cpp
# End Source File
# Begin Source File

SOURCE=..\tag_impl.cpp
# End Source File
# Begin Source File

SOURCE=..\tag_parse.cpp
# End Source File
# Begin Source File

SOURCE=..\tag_parse_lyrics3.cpp
# End Source File
# Begin Source File

SOURCE=..\tag_parse_musicmatch.cpp
# End Source File
# Begin Source File

SOURCE=..\tag_parse_v1.cpp
# End Source File
# Begin Source File

SOURCE=..\tag_render.cpp
# End Source File
# Begin Source File

SOURCE=..\utils.cpp
# End Source File
# Begin Source File

SOURCE=..\writers.cpp
# End Source File
# End Group
# Begin Group "res"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\res\id3v2.rc
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

SOURCE=..\..\..\..\base\include\facontext.h
# End Source File
# Begin Source File

SOURCE=..\field.h
# End Source File
# Begin Source File

SOURCE=..\field_def.h
# End Source File
# Begin Source File

SOURCE=..\field_impl.h
# End Source File
# Begin Source File

SOURCE=..\flags.h
# End Source File
# Begin Source File

SOURCE=..\frame_def.h
# End Source File
# Begin Source File

SOURCE=..\frame_impl.h
# End Source File
# Begin Source File

SOURCE=..\globals.h
# End Source File
# Begin Source File

SOURCE=..\header_frame.h
# End Source File
# Begin Source File

SOURCE=..\header_tag.h
# End Source File
# Begin Source File

SOURCE=..\helpers.h
# End Source File
# Begin Source File

SOURCE=..\id3.h
# End Source File
# Begin Source File

SOURCE=..\id3config.h
# End Source File
# Begin Source File

SOURCE=..\id3v2.h
# End Source File
# Begin Source File

SOURCE=..\io_decorators.h
# End Source File
# Begin Source File

SOURCE=..\io_helpers.h
# End Source File
# Begin Source File

SOURCE=..\io_strings.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\log.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\metadata.h
# End Source File
# Begin Source File

SOURCE=..\misc_support.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\win32\include\mutex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\preferences.h
# End Source File
# Begin Source File

SOURCE=..\reader.h
# End Source File
# Begin Source File

SOURCE=..\readers.h
# End Source File
# Begin Source File

SOURCE=..\sized_types.h
# End Source File
# Begin Source File

SOURCE=..\spec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\include\stl_config.h
# End Source File
# Begin Source File

SOURCE=..\strings.h
# End Source File
# Begin Source File

SOURCE=..\tag.h
# End Source File
# Begin Source File

SOURCE=..\tag_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\include\type_traits.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\base\include\utility.h
# End Source File
# Begin Source File

SOURCE=..\utils.h
# End Source File
# Begin Source File

SOURCE=..\writer.h
# End Source File
# Begin Source File

SOURCE=..\writers.h
# End Source File
# End Group
# End Target
# End Project
