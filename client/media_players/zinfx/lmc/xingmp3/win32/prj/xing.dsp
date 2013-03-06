# Microsoft Developer Studio Project File - Name="xing" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=xing - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xing.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xing.mak" CFG="xing - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xing - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xing - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xing - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xing - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xing - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\..\..\Crypto++" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /base:"0x11800000" /subsystem:windows /dll /machine:I386 /out:"xing.lmc" /libpath:"..\..\..\..\base\win32"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                    ..\..\..\..\base\win32\prj\plugins	copy xing.lmc                            ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "xing - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\..\..\Crypto++" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /base:"0x11800000" /subsystem:windows /dll /debug /machine:I386 /out:"xing.lmc" /pdbtype:sept /libpath:"..\..\..\..\base\win32"
# SUBTRACT LINK32 /profile /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                    ..\..\..\..\base\win32\prj\plugins	copy xing.lmc                            ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "xing___W"
# PROP BASE Intermediate_Dir "xing___W"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\..\..\Crypto++" /D "NDEBUG" /D "_WINDOWS" /D "ASM_X86" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"xing.lmc"
# ADD LINK32 fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /base:"0x11800000" /subsystem:windows /dll /machine:I386 /out:"xing.lmc" /libpath:"..\..\..\..\base\win32"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                    ..\..\..\..\base\win32\prj\plugins	copy xing.lmc                            ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "xing___0"
# PROP BASE Intermediate_Dir "xing___0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\..\..\Crypto++" /D "_DEBUG" /D "_WINDOWS" /D "ASM_X86" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"xing.lmc" /pdbtype:sept
# SUBTRACT BASE LINK32 /profile /nodefaultlib
# ADD LINK32 fabaselib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /base:"0x11800000" /subsystem:windows /dll /debug /machine:I386 /out:"xing.lmc" /pdbtype:sept /libpath:"..\..\..\..\base\win32"
# SUBTRACT LINK32 /profile /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                    ..\..\..\..\base\win32\prj\plugins	copy xing.lmc                            ..\..\..\..\base\win32\prj\plugins
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "xing - Win32 Release"
# Name "xing - Win32 Debug"
# Name "xing - Win32 NASM Release"
# Name "xing - Win32 NASM Debug"
# Begin Group "general source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\config\config.win32

!IF  "$(CFG)" == "xing - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "xing - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Debug"

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

SOURCE=..\..\..\..\io\src\pipeline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\io\src\pullbuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\xing.def
# End Source File
# Begin Source File

SOURCE=..\res\xing.rc
# End Source File
# End Group
# Begin Group "decoder"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\cdct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\csbt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cup.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cupl3.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cwinm.c
# End Source File
# Begin Source File

SOURCE=..\..\src\dec8.c
# End Source File
# Begin Source File

SOURCE=..\..\src\dorm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\hwin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\icdct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\isbt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\iup.c
# End Source File
# Begin Source File

SOURCE=..\..\src\iwinm.c
# End Source File
# Begin Source File

SOURCE=..\..\src\l3dq.c
# End Source File
# Begin Source File

SOURCE=..\..\src\l3init.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mdct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mhead.c
# End Source File
# Begin Source File

SOURCE=..\..\src\msis.c
# End Source File
# Begin Source File

SOURCE=..\..\src\uph.c
# End Source File
# Begin Source File

SOURCE=..\..\src\upsf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\wavep.c
# End Source File
# Begin Source File

SOURCE=..\..\src\xinglmc.cpp
# End Source File
# End Group
# Begin Group "Assembly Files"

# PROP Default_Filter "asm"
# Begin Source File

SOURCE=..\..\src\cdctasm.asm

!IF  "$(CFG)" == "xing - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xing - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Release"

# Begin Custom Build
IntDir=.\Release
InputPath=..\..\src\cdctasm.asm
InputName=cdctasm

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Debug"

# Begin Custom Build
IntDir=.\Debug
InputPath=..\..\src\cdctasm.asm
InputName=cdctasm

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\cwin8asm.asm

!IF  "$(CFG)" == "xing - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xing - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Release"

# Begin Custom Build
IntDir=.\Release
InputPath=..\..\src\cwin8asm.asm
InputName=cwin8asm

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Debug"

# Begin Custom Build
IntDir=.\Debug
InputPath=..\..\src\cwin8asm.asm
InputName=cwin8asm

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\cwinasm.asm

!IF  "$(CFG)" == "xing - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xing - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Release"

# Begin Custom Build
IntDir=.\Release
InputPath=..\..\src\cwinasm.asm
InputName=cwinasm

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Debug"

# Begin Custom Build
IntDir=.\Debug
InputPath=..\..\src\cwinasm.asm
InputName=cwinasm

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\mdctasm.asm

!IF  "$(CFG)" == "xing - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xing - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Release"

# Begin Custom Build
IntDir=.\Release
InputPath=..\..\src\mdctasm.asm
InputName=mdctasm

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Debug"

# Begin Custom Build
IntDir=.\Debug
InputPath=..\..\src\mdctasm.asm
InputName=mdctasm

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\msisasm.asm

!IF  "$(CFG)" == "xing - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xing - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Release"

# Begin Custom Build
IntDir=.\Release
InputPath=..\..\src\msisasm.asm
InputName=msisasm

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "xing - Win32 NASM Debug"

# Begin Custom Build
IntDir=.\Debug
InputPath=..\..\src\msisasm.asm
InputName=msisasm

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasmw -f win32 -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
