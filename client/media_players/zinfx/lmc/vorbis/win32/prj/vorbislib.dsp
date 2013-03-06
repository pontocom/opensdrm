# Microsoft Developer Studio Project File - Name="vorbislib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vorbislib - Win32 NASM Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vorbislib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vorbislib.mak" CFG="vorbislib - Win32 NASM Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vorbislib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vorbislib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vorbislib - Win32 NASM Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vorbislib - Win32 NASM Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vorbislib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vorbislib___Win32_Release"
# PROP BASE Intermediate_Dir "vorbislib___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vorbislib___Win32_Release"
# PROP Intermediate_Dir "vorbislib___Win32_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\..\vorbis\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\vorbislib.lib"

!ELSEIF  "$(CFG)" == "vorbislib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vorbislib___Win32_Debug"
# PROP BASE Intermediate_Dir "vorbislib___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vorbislib___Win32_Debug"
# PROP Intermediate_Dir "vorbislib___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\..\vorbis\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\vorbislib.lib"

!ELSEIF  "$(CFG)" == "vorbislib - Win32 NASM Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vorbislib___Win32_NASM_Release"
# PROP BASE Intermediate_Dir "vorbislib___Win32_NASM_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vorbislib___Win32_NASM_Release"
# PROP Intermediate_Dir "vorbislib___Win32_NASM_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\..\vorbis\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\..\vorbis\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\vorbislib.lib"
# ADD LIB32 /nologo /out:"..\vorbislib.lib"

!ELSEIF  "$(CFG)" == "vorbislib - Win32 NASM Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vorbislib___Win32_NASM_Debug"
# PROP BASE Intermediate_Dir "vorbislib___Win32_NASM_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vorbislib___Win32_NASM_Debug"
# PROP Intermediate_Dir "vorbislib___Win32_NASM_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\..\vorbis\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /I "..\..\..\include" /I "..\..\include" /I "..\..\..\..\io\include" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\..\..\..\..\vorbis\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\vorbislib.lib"
# ADD LIB32 /nologo /out:"..\vorbislib.lib"

!ENDIF 

# Begin Target

# Name "vorbislib - Win32 Release"
# Name "vorbislib - Win32 Debug"
# Name "vorbislib - Win32 NASM Release"
# Name "vorbislib - Win32 NASM Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\lib\analysis.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\barkmel.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\bitwise.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\block.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\codebook.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\envelope.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\floor0.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\framing.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\iir.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\info.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\lpc.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\lsp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\mapping0.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\mdct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\psy.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\registry.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\res0.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\sharedbook.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\smallft.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\synthesis.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\time0.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\vorbisfile.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\window.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
