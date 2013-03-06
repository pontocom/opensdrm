# Microsoft Developer Studio Project File - Name="gdbm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=gdbm - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gdbm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gdbm.mak" CFG="gdbm - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gdbm - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gdbm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "gdbm - Win32 NASM Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gdbm - Win32 NASM Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gdbm - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\config" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\gdbm.lib"

!ELSEIF  "$(CFG)" == "gdbm - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\config" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\gdbm.lib"

!ELSEIF  "$(CFG)" == "gdbm - Win32 NASM Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gdbm___W"
# PROP BASE Intermediate_Dir "gdbm___W"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\config" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\gdbm.lib"
# ADD LIB32 /nologo /out:"..\gdbm.lib"

!ELSEIF  "$(CFG)" == "gdbm - Win32 NASM Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "gdbm___0"
# PROP BASE Intermediate_Dir "gdbm___0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "..\config" /I ".\\" /I "..\\" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\gdbm.lib"
# ADD LIB32 /nologo /out:"..\gdbm.lib"

!ENDIF 

# Begin Target

# Name "gdbm - Win32 Release"
# Name "gdbm - Win32 Debug"
# Name "gdbm - Win32 NASM Release"
# Name "gdbm - Win32 NASM Debug"
# Begin Source File

SOURCE=..\bucket.c
# End Source File
# Begin Source File

SOURCE=..\config\config.win32

!IF  "$(CFG)" == "gdbm - Win32 Release"

# Begin Custom Build
InputPath=..\config\config.win32

"..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist ..\config\config.h copy ..\config\config.win32 ..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "gdbm - Win32 Debug"

# Begin Custom Build
InputPath=..\config\config.win32

"..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist ..\config\config.h copy ..\config\config.win32 ..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "gdbm - Win32 NASM Release"

# Begin Custom Build
InputPath=..\config\config.win32

"..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist ..\config\config.h copy ..\config\config.win32 ..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "gdbm - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\config\config.win32

"..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist ..\config\config.h copy ..\config\config.win32 ..\config\config.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\falloc.c
# End Source File
# Begin Source File

SOURCE=..\findkey.c
# End Source File
# Begin Source File

SOURCE=..\gdbmclose.c
# End Source File
# Begin Source File

SOURCE=..\gdbmdelete.c
# End Source File
# Begin Source File

SOURCE=..\gdbmerrno.c
# End Source File
# Begin Source File

SOURCE=..\gdbmexists.c
# End Source File
# Begin Source File

SOURCE=..\gdbmfdesc.c
# End Source File
# Begin Source File

SOURCE=..\gdbmfetch.c
# End Source File
# Begin Source File

SOURCE=..\gdbmopen.c
# End Source File
# Begin Source File

SOURCE=..\gdbmreorg.c
# End Source File
# Begin Source File

SOURCE=..\gdbmseq.c
# End Source File
# Begin Source File

SOURCE=..\gdbmsetopt.c
# End Source File
# Begin Source File

SOURCE=..\gdbmstore.c
# End Source File
# Begin Source File

SOURCE=..\gdbmsync.c
# End Source File
# Begin Source File

SOURCE=..\global.c
# End Source File
# Begin Source File

SOURCE=..\hash.c
# End Source File
# Begin Source File

SOURCE=..\update.c
# End Source File
# Begin Source File

SOURCE=..\version.c
# End Source File
# End Target
# End Project
