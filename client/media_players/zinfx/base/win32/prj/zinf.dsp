# Microsoft Developer Studio Project File - Name="zinf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=zinf - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zinf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zinf.mak" CFG="zinf - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zinf - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "zinf - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "zinf - Win32 NASM Release" (based on "Win32 (x86) Application")
!MESSAGE "zinf - Win32 NASM Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "zinf - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\..\base\aps" /I "..\..\..\lib\zlib\include" /I "..\..\..\lib\xml\include" /I "..\..\..\lib\gdbm" /I "..\include" /I "..\..\include" /I "..\..\..\config" /I "..\..\..\ui\win32Test\include" /I "..\..\..\ui\win32Test\res" /I "..\..\..\io\include" /I "..\..\..\ui\include" /I "..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 musicrainz.lib musicbrainz.lib fabaselib.lib zlib.lib gdbm.lib version.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wininet.lib /nologo /subsystem:windows /machine:I386 /out:"zinf.exe" /libpath:"..\..\..\lib\gdbm" /libpath:"..\..\..\lib\zlib" /libpath:"..\\" /libpath:"..\..\..\..\lib\musicbrainz"

!ELSEIF  "$(CFG)" == "zinf - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\base\aps" /I "..\..\..\lib\zlib\include" /I "..\..\..\lib\xml\include" /I "..\..\..\lib\gdbm" /I "..\include" /I "..\..\include" /I "..\..\..\config" /I "..\..\..\ui\win32Test\include" /I "..\..\..\ui\win32Test\res" /I "..\..\..\io\include" /I "..\..\..\ui\include" /I "..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 musicbrainz.lib fabaselib.lib zlib.lib gdbm.lib version.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wininet.lib /nologo /subsystem:windows /debug /machine:I386 /out:"zinf.exe" /pdbtype:sept /libpath:"..\..\..\lib\gdbm" /libpath:"..\..\..\lib\zlib" /libpath:"..\\" /libpath:"..\..\..\lib\musicbrainz"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zinf_"
# PROP BASE Intermediate_Dir "zinf_"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\include" /I "..\..\include" /I "..\..\..\config" /I "..\..\..\ui\win32Test\include" /I "..\..\..\ui\win32Test\res" /I "..\..\..\io\include" /I "..\..\..\ui\include" /I "..\..\..\lmc\include" /I "..\..\..\ui\dummy\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# SUBTRACT BASE CPP /X
# ADD CPP /nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\..\base\aps" /I "..\..\..\lib\zlib\include" /I "..\..\..\lib\xml\include" /I "..\..\..\lib\gdbm" /I "..\include" /I "..\..\include" /I "..\..\..\config" /I "..\..\..\ui\win32Test\include" /I "..\..\..\ui\win32Test\res" /I "..\..\..\io\include" /I "..\..\..\ui\include" /I "..\..\..\lmc\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /FR /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"zinf.exe"
# ADD LINK32 musicbrainz.lib fabaselib.lib zlib.lib gdbm.lib version.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wininet.lib /nologo /subsystem:windows /machine:I386 /out:"zinf.exe" /libpath:"..\..\..\lib\gdbm" /libpath:"..\..\..\lib\zlib" /libpath:"..\\" /libpath:"..\..\..\..\lib\musicbrainz" /libpath:"..\..\..\lib\musicbrainz\\"

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zinf0"
# PROP BASE Intermediate_Dir "zinf0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\ui\dummy\include" /I "..\include" /I "..\..\include" /I "..\..\..\config" /I "..\..\..\ui\win32Test\include" /I "..\..\..\ui\win32Test\res" /I "..\..\..\io\include" /I "..\..\..\ui\include" /I "..\..\..\lmc\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# SUBTRACT BASE CPP /X
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\base\aps" /I "..\..\..\lib\zlib\include" /I "..\..\..\lib\xml\include" /I "..\..\..\lib\gdbm" /I "..\include" /I "..\..\include" /I "..\..\..\config" /I "..\..\..\ui\win32Test\include" /I "..\..\..\ui\win32Test\res" /I "..\..\..\io\include" /I "..\..\..\ui\include" /I "..\..\..\lmc\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /FR /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"zinf.exe" /pdbtype:sept
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 musicbrainz.lib fabaselib.lib zlib.lib gdbm.lib version.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wininet.lib /nologo /subsystem:windows /debug /machine:I386 /out:"zinf.exe" /pdbtype:sept /libpath:"..\..\..\lib\gdbm" /libpath:"..\..\..\lib\zlib" /libpath:"..\\" /libpath:"..\..\..\lib\musicbrainz"
# SUBTRACT LINK32 /profile

!ENDIF 

# Begin Target

# Name "zinf - Win32 Release"
# Name "zinf - Win32 Debug"
# Name "zinf - Win32 NASM Release"
# Name "zinf - Win32 NASM Debug"
# Begin Group "source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\config\config.win32

!IF  "$(CFG)" == "zinf - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\config\config.win32

"..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\config\config.win32 ..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\config\config.win32

"..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\config\config.win32 ..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\config\config.win32

"..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\config\config.win32 ..\..\..\config\config.h

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\config\config.win32

"..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\config\config.win32 ..\..\..\config\config.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\xml\src\Parse.cpp
# End Source File
# End Group
# Begin Group "text files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\AUTHORS

!IF  "$(CFG)" == "zinf - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\AUTHORS

"..\..\..\Authors.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\AUTHORS ..\..\..\Authors.txt

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\AUTHORS

"..\..\..\Authors.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\AUTHORS ..\..\..\Authors.txt

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\AUTHORS

"..\..\..\Authors.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\AUTHORS ..\..\..\Authors.txt

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\AUTHORS

"..\..\..\Authors.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\AUTHORS ..\..\..\Authors.txt

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\CHANGES

!IF  "$(CFG)" == "zinf - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\CHANGES

"..\..\..\Changes.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\CHANGES ..\..\..\Changes.txt

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\CHANGES

"..\..\..\Changes.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\CHANGES ..\..\..\Changes.txt

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\CHANGES

"..\..\..\Changes.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\CHANGES ..\..\..\Changes.txt

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\CHANGES

"..\..\..\Changes.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\CHANGES ..\..\..\Changes.txt

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\COPYING

!IF  "$(CFG)" == "zinf - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\COPYING

"..\..\..\Copying.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\COPYING ..\..\..\Copying.txt

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\COPYING

"..\..\..\Copying.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\COPYING ..\..\..\Copying.txt

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\COPYING

"..\..\..\Copying.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\COPYING ..\..\..\Copying.txt

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\COPYING

"..\..\..\Copying.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\COPYING ..\..\..\Copying.txt

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\README

!IF  "$(CFG)" == "zinf - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\README

"..\..\..\ReadMe.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\README ..\..\..\ReadMe.txt

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\README

"..\..\..\ReadMe.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\README ..\..\..\ReadMe.txt

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\README

"..\..\..\ReadMe.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\README ..\..\..\ReadMe.txt

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\README

"..\..\..\ReadMe.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\README ..\..\..\ReadMe.txt

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=..\res\music1.ico
# End Source File
# Begin Source File

SOURCE=..\res\music10.ico
# End Source File
# Begin Source File

SOURCE=..\res\music11.ico
# End Source File
# Begin Source File

SOURCE=..\res\music12.ico
# End Source File
# Begin Source File

SOURCE=..\res\music13.ico
# End Source File
# Begin Source File

SOURCE=..\res\music14.ico
# End Source File
# Begin Source File

SOURCE=..\res\music15.ico
# End Source File
# Begin Source File

SOURCE=..\res\music16.ico
# End Source File
# Begin Source File

SOURCE=..\res\music17.ico
# End Source File
# Begin Source File

SOURCE=..\res\music18.ico
# End Source File
# Begin Source File

SOURCE=..\res\music19.ico
# End Source File
# Begin Source File

SOURCE=..\res\music2.ico
# End Source File
# Begin Source File

SOURCE=..\res\music3.ico
# End Source File
# Begin Source File

SOURCE=..\res\music4.ico
# End Source File
# Begin Source File

SOURCE=..\res\music5.ico
# End Source File
# Begin Source File

SOURCE=..\res\music6.ico
# End Source File
# Begin Source File

SOURCE=..\res\music7.ico
# End Source File
# Begin Source File

SOURCE=..\res\music8.ico
# End Source File
# Begin Source File

SOURCE=..\res\music9.ico
# End Source File
# Begin Source File

SOURCE=..\res\warn.ico
# End Source File
# Begin Source File

SOURCE=..\res\zinf.rc
# End Source File
# End Group
# Begin Group "themes"

# PROP Default_Filter "fat"
# Begin Source File

SOURCE=..\..\..\themes\Aquatica.fat

!IF  "$(CFG)" == "zinf - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\themes\Aquatica.fat
InputName=Aquatica

".\themes\$(InputName).fat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	IF NOT EXIST themes mkdir themes 
	copy $(InputPath) themes 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\themes\Aquatica.fat
InputName=Aquatica

".\themes\$(InputName).fat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	IF NOT EXIST themes mkdir themes 
	copy $(InputPath) themes 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\themes\Aquatica.fat
InputName=Aquatica

".\themes\$(InputName).fat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	IF NOT EXIST themes mkdir themes 
	copy $(InputPath) themes 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\themes\Aquatica.fat
InputName=Aquatica

".\themes\$(InputName).fat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	IF NOT EXIST themes mkdir themes 
	copy $(InputPath) themes 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\themes\Zinf.fat

!IF  "$(CFG)" == "zinf - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\themes\Zinf.fat
InputName=Zinf

".\themes\$(InputName).fat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	IF NOT EXIST themes mkdir themes 
	copy $(InputPath) themes 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\themes\Zinf.fat
InputName=Zinf

".\themes\$(InputName).fat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	IF NOT EXIST themes mkdir themes 
	copy $(InputPath) themes 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\themes\Zinf.fat
InputName=Zinf

".\themes\$(InputName).fat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	IF NOT EXIST themes mkdir themes 
	copy $(InputPath) themes 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\themes\Zinf.fat
InputName=Zinf

".\themes\$(InputName).fat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	IF NOT EXIST themes mkdir themes 
	copy $(InputPath) themes 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\themes\ZinfClassic.fat

!IF  "$(CFG)" == "zinf - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\themes\ZinfClassic.fat
InputName=ZinfClassic

".\themes\$(InputName).fat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	IF NOT EXIST themes mkdir themes 
	copy $(InputPath) themes 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\themes\ZinfClassic.fat
InputName=ZinfClassic

".\themes\$(InputName).fat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	IF NOT EXIST themes mkdir themes 
	copy $(InputPath) themes 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\themes\ZinfClassic.fat
InputName=ZinfClassic

".\themes\$(InputName).fat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	IF NOT EXIST themes mkdir themes 
	copy $(InputPath) themes 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\themes\ZinfClassic.fat
InputName=ZinfClassic

".\themes\$(InputName).fat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	IF NOT EXIST themes mkdir themes 
	copy $(InputPath) themes 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "includes"

# PROP Default_Filter ""
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

SOURCE=..\..\include\errors.h
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

SOURCE=..\..\..\lib\gdbm\gdbm_fa.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lmc\include\lmc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\log.h
# End Source File
# Begin Source File

SOURCE=..\..\include\metadata.h
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

SOURCE=..\..\include\queue.h
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

SOURCE=..\..\include\thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\ui\include\ui.h
# End Source File
# Begin Source File

SOURCE=..\..\include\undomanager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\utility.h
# End Source File
# Begin Source File

SOURCE=..\include\win32prefs.h
# End Source File
# End Group
# Begin Group "dlls"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib\musicbrainz\musicbrainz.dll

!IF  "$(CFG)" == "zinf - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\lib\musicbrainz\musicbrainz.dll
InputName=musicbrainz

".\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\lib\musicbrainz\musicbrainz.dll
InputName=musicbrainz

".\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\lib\musicbrainz\musicbrainz.dll
InputName=musicbrainz

".\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\lib\musicbrainz\musicbrainz.dll
InputName=musicbrainz

".\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\vorbis\ogg.dll

!IF  "$(CFG)" == "zinf - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\lib\vorbis\ogg.dll
InputName=ogg

".\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\lib\vorbis\ogg.dll
InputName=ogg

".\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\lib\vorbis\ogg.dll
InputName=ogg

".\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\lib\vorbis\ogg.dll
InputName=ogg

".\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\vorbis\vorbis.dll

!IF  "$(CFG)" == "zinf - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\lib\vorbis\vorbis.dll
InputName=vorbis

".\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\lib\vorbis\vorbis.dll
InputName=vorbis

".\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\lib\vorbis\vorbis.dll
InputName=vorbis

".\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\lib\vorbis\vorbis.dll
InputName=vorbis

".\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\vorbis\vorbisfile.dll

!IF  "$(CFG)" == "zinf - Win32 Release"

# Begin Custom Build
InputPath=..\..\..\lib\vorbis\vorbisfile.dll
InputName=vorbisfile

".\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\lib\vorbis\vorbisfile.dll
InputName=vorbisfile

".\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Release"

# Begin Custom Build
InputPath=..\..\..\lib\vorbis\vorbisfile.dll
InputName=vorbisfile

".\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ELSEIF  "$(CFG)" == "zinf - Win32 NASM Debug"

# Begin Custom Build
InputPath=..\..\..\lib\vorbis\vorbisfile.dll
InputName=vorbisfile

".\$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
