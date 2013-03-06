# Microsoft Developer Studio Generated NMAKE File, Based on musicbrowser.dsp
!IF "$(CFG)" == ""
CFG=musicbrowser - Win32 NASM Debug MS STL
!MESSAGE No configuration specified. Defaulting to musicbrowser - Win32 NASM Debug MS STL.
!ENDIF 

!IF "$(CFG)" != "musicbrowser - Win32 Release" && "$(CFG)" != "musicbrowser - Win32 Debug" && "$(CFG)" != "musicbrowser - Win32 NASM Debug" && "$(CFG)" != "musicbrowser - Win32 NASM Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "musicbrowser.mak" CFG="musicbrowser - Win32 NASM Debug MS STL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "musicbrowser - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "musicbrowser - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "musicbrowser - Win32 NASM Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "musicbrowser - Win32 NASM Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "musicbrowser - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\musicbrowser.ui"

!ELSE 

ALL : "gdbm - Win32 Release" "fabaselib - Win32 Release" "..\..\..\..\config\config.h" ".\musicbrowser.ui"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 ReleaseCLEAN" "gdbm - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\DataIndex.obj"
	-@erase "$(INTDIR)\Dialog.obj"
	-@erase "$(INTDIR)\DropObject.obj"
	-@erase "$(INTDIR)\DropSource.obj"
	-@erase "$(INTDIR)\DropTarget.obj"
	-@erase "$(INTDIR)\EditTrackInfoDialog.obj"
	-@erase "$(INTDIR)\EnumFormatEtc.obj"
	-@erase "$(INTDIR)\Event.obj"
	-@erase "$(INTDIR)\FavoriteDialog.obj"
	-@erase "$(INTDIR)\FreeAmpStreams.obj"
	-@erase "$(INTDIR)\IntroductionWizard.obj"
	-@erase "$(INTDIR)\MissingFileDialog.obj"
	-@erase "$(INTDIR)\MultiSelectTreeView.obj"
	-@erase "$(INTDIR)\musicbrowser.res"
	-@erase "$(INTDIR)\MusicSearchDialog.obj"
	-@erase "$(INTDIR)\MusicTree.obj"
	-@erase "$(INTDIR)\OpenSavePlaylists.obj"
	-@erase "$(INTDIR)\PerformDBCheckDialog.obj"
	-@erase "$(INTDIR)\PlaylistView.obj"
	-@erase "$(INTDIR)\RemoveTracksDialog.obj"
	-@erase "$(INTDIR)\SavePlaylistDialog.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\Win32MusicBrowser.obj"
	-@erase "$(OUTDIR)\musicbrowser.exp"
	-@erase "$(OUTDIR)\musicbrowser.lib"
	-@erase ".\musicbrowser.ui"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\base\aps" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\res" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\include" /I "..\..\..\..\lib\http\include" /I "..\..\..\..\lib\xml\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\musicbrowser.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\musicbrowser.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\musicbrowser.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib fabaselib.lib gdbm.lib comctl32.lib version.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x10900000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\musicbrowser.pdb" /machine:I386 /def:".\musicbrowser.def" /out:"musicbrowser.ui" /implib:"$(OUTDIR)\musicbrowser.lib" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\musicbrainz" 
DEF_FILE= \
	".\musicbrowser.def"
LINK32_OBJS= \
	"$(INTDIR)\DataIndex.obj" \
	"$(INTDIR)\Dialog.obj" \
	"$(INTDIR)\DropObject.obj" \
	"$(INTDIR)\DropSource.obj" \
	"$(INTDIR)\DropTarget.obj" \
	"$(INTDIR)\EditTrackInfoDialog.obj" \
	"$(INTDIR)\EnumFormatEtc.obj" \
	"$(INTDIR)\Event.obj" \
	"$(INTDIR)\FavoriteDialog.obj" \
	"$(INTDIR)\FreeAmpStreams.obj" \
	"$(INTDIR)\IntroductionWizard.obj" \
	"$(INTDIR)\MissingFileDialog.obj" \
	"$(INTDIR)\MultiSelectTreeView.obj" \
	"$(INTDIR)\MusicSearchDialog.obj" \
	"$(INTDIR)\MusicTree.obj" \
	"$(INTDIR)\OpenSavePlaylists.obj" \
	"$(INTDIR)\PerformDBCheckDialog.obj" \
	"$(INTDIR)\PlaylistView.obj" \
	"$(INTDIR)\RemoveTracksDialog.obj" \
	"$(INTDIR)\SavePlaylistDialog.obj" \
	"$(INTDIR)\Win32MusicBrowser.obj" \
	"$(INTDIR)\musicbrowser.res" \
	"..\..\..\..\base\win32\fabaselib.lib" \
	"..\..\..\..\lib\gdbm\gdbm.lib"

".\musicbrowser.ui" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "gdbm - Win32 Release" "fabaselib - Win32 Release" "..\..\..\..\config\config.h" ".\musicbrowser.ui"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                                                                     ..\..\..\..\base\win32\prj\plugins
	copy musicbrowser.ui                                                                                       ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\musicbrowser.ui"

!ELSE 

ALL : "gdbm - Win32 Debug" "fabaselib - Win32 Debug" "..\..\..\..\config\config.h" ".\musicbrowser.ui"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 DebugCLEAN" "gdbm - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\DataIndex.obj"
	-@erase "$(INTDIR)\Dialog.obj"
	-@erase "$(INTDIR)\DropObject.obj"
	-@erase "$(INTDIR)\DropSource.obj"
	-@erase "$(INTDIR)\DropTarget.obj"
	-@erase "$(INTDIR)\EditTrackInfoDialog.obj"
	-@erase "$(INTDIR)\EnumFormatEtc.obj"
	-@erase "$(INTDIR)\Event.obj"
	-@erase "$(INTDIR)\FavoriteDialog.obj"
	-@erase "$(INTDIR)\FreeAmpStreams.obj"
	-@erase "$(INTDIR)\IntroductionWizard.obj"
	-@erase "$(INTDIR)\MissingFileDialog.obj"
	-@erase "$(INTDIR)\MultiSelectTreeView.obj"
	-@erase "$(INTDIR)\musicbrowser.res"
	-@erase "$(INTDIR)\MusicSearchDialog.obj"
	-@erase "$(INTDIR)\MusicTree.obj"
	-@erase "$(INTDIR)\OpenSavePlaylists.obj"
	-@erase "$(INTDIR)\PerformDBCheckDialog.obj"
	-@erase "$(INTDIR)\PlaylistView.obj"
	-@erase "$(INTDIR)\RemoveTracksDialog.obj"
	-@erase "$(INTDIR)\SavePlaylistDialog.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\Win32MusicBrowser.obj"
	-@erase "$(OUTDIR)\musicbrowser.exp"
	-@erase "$(OUTDIR)\musicbrowser.lib"
	-@erase "$(OUTDIR)\musicbrowser.pdb"
	-@erase ".\musicbrowser.ilk"
	-@erase ".\musicbrowser.ui"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\base\aps" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\res" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\include" /I "..\..\..\..\lib\http\include" /I "..\..\..\..\lib\xml\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\musicbrowser.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\musicbrowser.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\musicbrowser.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib fabaselib.lib gdbm.lib comctl32.lib version.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x10900000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\musicbrowser.pdb" /debug /machine:I386 /def:".\musicbrowser.def" /out:"musicbrowser.ui" /implib:"$(OUTDIR)\musicbrowser.lib" /pdbtype:sept /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\musicbrainz" 
LINK32_OBJS= \
	"$(INTDIR)\DataIndex.obj" \
	"$(INTDIR)\Dialog.obj" \
	"$(INTDIR)\DropObject.obj" \
	"$(INTDIR)\DropSource.obj" \
	"$(INTDIR)\DropTarget.obj" \
	"$(INTDIR)\EditTrackInfoDialog.obj" \
	"$(INTDIR)\EnumFormatEtc.obj" \
	"$(INTDIR)\Event.obj" \
	"$(INTDIR)\FavoriteDialog.obj" \
	"$(INTDIR)\FreeAmpStreams.obj" \
	"$(INTDIR)\IntroductionWizard.obj" \
	"$(INTDIR)\MissingFileDialog.obj" \
	"$(INTDIR)\MultiSelectTreeView.obj" \
	"$(INTDIR)\MusicSearchDialog.obj" \
	"$(INTDIR)\MusicTree.obj" \
	"$(INTDIR)\OpenSavePlaylists.obj" \
	"$(INTDIR)\PerformDBCheckDialog.obj" \
	"$(INTDIR)\PlaylistView.obj" \
	"$(INTDIR)\RemoveTracksDialog.obj" \
	"$(INTDIR)\SavePlaylistDialog.obj" \
	"$(INTDIR)\Win32MusicBrowser.obj" \
	"$(INTDIR)\musicbrowser.res" \
	"..\..\..\..\base\win32\fabaselib.lib" \
	"..\..\..\..\lib\gdbm\gdbm.lib"

".\musicbrowser.ui" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "gdbm - Win32 Debug" "fabaselib - Win32 Debug" "..\..\..\..\config\config.h" ".\musicbrowser.ui"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                                                                     ..\..\..\..\base\win32\prj\plugins
	copy musicbrowser.ui                                                                                       ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 NASM Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\musicbrowser.ui"

!ELSE 

ALL : "gdbm - Win32 NASM Debug" "fabaselib - Win32 NASM Debug" "..\..\..\..\config\config.h" ".\musicbrowser.ui"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM DebugCLEAN" "gdbm - Win32 NASM DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\DataIndex.obj"
	-@erase "$(INTDIR)\Dialog.obj"
	-@erase "$(INTDIR)\DropObject.obj"
	-@erase "$(INTDIR)\DropSource.obj"
	-@erase "$(INTDIR)\DropTarget.obj"
	-@erase "$(INTDIR)\EditTrackInfoDialog.obj"
	-@erase "$(INTDIR)\EnumFormatEtc.obj"
	-@erase "$(INTDIR)\Event.obj"
	-@erase "$(INTDIR)\FavoriteDialog.obj"
	-@erase "$(INTDIR)\FreeAmpStreams.obj"
	-@erase "$(INTDIR)\IntroductionWizard.obj"
	-@erase "$(INTDIR)\MissingFileDialog.obj"
	-@erase "$(INTDIR)\MultiSelectTreeView.obj"
	-@erase "$(INTDIR)\musicbrowser.res"
	-@erase "$(INTDIR)\MusicSearchDialog.obj"
	-@erase "$(INTDIR)\MusicTree.obj"
	-@erase "$(INTDIR)\OpenSavePlaylists.obj"
	-@erase "$(INTDIR)\PerformDBCheckDialog.obj"
	-@erase "$(INTDIR)\PlaylistView.obj"
	-@erase "$(INTDIR)\RemoveTracksDialog.obj"
	-@erase "$(INTDIR)\SavePlaylistDialog.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\Win32MusicBrowser.obj"
	-@erase "$(OUTDIR)\musicbrowser.exp"
	-@erase "$(OUTDIR)\musicbrowser.lib"
	-@erase "$(OUTDIR)\musicbrowser.pdb"
	-@erase ".\musicbrowser.ilk"
	-@erase ".\musicbrowser.ui"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\base\aps" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\res" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\include" /I "..\..\..\..\lib\http\include" /I "..\..\..\..\lib\xml\include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\musicbrowser.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\musicbrowser.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\musicbrowser.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib fabaselib.lib gdbm.lib comctl32.lib version.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x10900000" /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\musicbrowser.pdb" /debug /machine:I386 /def:".\musicbrowser.def" /out:"musicbrowser.ui" /implib:"$(OUTDIR)\musicbrowser.lib" /pdbtype:sept /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\musicbrainz" 
LINK32_OBJS= \
	"$(INTDIR)\DataIndex.obj" \
	"$(INTDIR)\Dialog.obj" \
	"$(INTDIR)\DropObject.obj" \
	"$(INTDIR)\DropSource.obj" \
	"$(INTDIR)\DropTarget.obj" \
	"$(INTDIR)\EditTrackInfoDialog.obj" \
	"$(INTDIR)\EnumFormatEtc.obj" \
	"$(INTDIR)\Event.obj" \
	"$(INTDIR)\FavoriteDialog.obj" \
	"$(INTDIR)\FreeAmpStreams.obj" \
	"$(INTDIR)\IntroductionWizard.obj" \
	"$(INTDIR)\MissingFileDialog.obj" \
	"$(INTDIR)\MultiSelectTreeView.obj" \
	"$(INTDIR)\MusicSearchDialog.obj" \
	"$(INTDIR)\MusicTree.obj" \
	"$(INTDIR)\OpenSavePlaylists.obj" \
	"$(INTDIR)\PerformDBCheckDialog.obj" \
	"$(INTDIR)\PlaylistView.obj" \
	"$(INTDIR)\RemoveTracksDialog.obj" \
	"$(INTDIR)\SavePlaylistDialog.obj" \
	"$(INTDIR)\Win32MusicBrowser.obj" \
	"$(INTDIR)\musicbrowser.res" \
	"..\..\..\..\base\win32\fabaselib.lib" \
	"..\..\..\..\lib\gdbm\gdbm.lib"

".\musicbrowser.ui" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "gdbm - Win32 NASM Debug" "fabaselib - Win32 NASM Debug" "..\..\..\..\config\config.h" ".\musicbrowser.ui"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                                                                     ..\..\..\..\base\win32\prj\plugins
	copy musicbrowser.ui                                                                                       ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 NASM Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\config\config.h" ".\musicbrowser.ui"

!ELSE 

ALL : "gdbm - Win32 NASM Release" "fabaselib - Win32 NASM Release" "..\..\..\..\config\config.h" ".\musicbrowser.ui"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"fabaselib - Win32 NASM ReleaseCLEAN" "gdbm - Win32 NASM ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\DataIndex.obj"
	-@erase "$(INTDIR)\Dialog.obj"
	-@erase "$(INTDIR)\DropObject.obj"
	-@erase "$(INTDIR)\DropSource.obj"
	-@erase "$(INTDIR)\DropTarget.obj"
	-@erase "$(INTDIR)\EditTrackInfoDialog.obj"
	-@erase "$(INTDIR)\EnumFormatEtc.obj"
	-@erase "$(INTDIR)\Event.obj"
	-@erase "$(INTDIR)\FavoriteDialog.obj"
	-@erase "$(INTDIR)\FreeAmpStreams.obj"
	-@erase "$(INTDIR)\IntroductionWizard.obj"
	-@erase "$(INTDIR)\MissingFileDialog.obj"
	-@erase "$(INTDIR)\MultiSelectTreeView.obj"
	-@erase "$(INTDIR)\musicbrowser.res"
	-@erase "$(INTDIR)\MusicSearchDialog.obj"
	-@erase "$(INTDIR)\MusicTree.obj"
	-@erase "$(INTDIR)\OpenSavePlaylists.obj"
	-@erase "$(INTDIR)\PerformDBCheckDialog.obj"
	-@erase "$(INTDIR)\PlaylistView.obj"
	-@erase "$(INTDIR)\RemoveTracksDialog.obj"
	-@erase "$(INTDIR)\SavePlaylistDialog.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\Win32MusicBrowser.obj"
	-@erase "$(OUTDIR)\musicbrowser.exp"
	-@erase "$(OUTDIR)\musicbrowser.lib"
	-@erase ".\musicbrowser.ui"
	-@erase "..\..\..\..\config\config.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\include" /I "..\..\..\include" /I "..\..\..\..\base\aps" /I "..\..\..\..\base\include" /I "..\..\..\..\base\win32\include" /I "..\..\..\..\config" /I "..\..\..\..\ui\include" /I "..\res" /I "..\..\..\..\lib\gdbm" /I "..\..\..\..\lmc\include" /I "..\..\..\..\io\include" /I "..\..\..\..\lib\http\include" /I "..\..\..\..\lib\xml\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "NOMINMAX" /Fp"$(INTDIR)\musicbrowser.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\musicbrowser.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\musicbrowser.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib fabaselib.lib gdbm.lib comctl32.lib version.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib musicbrainz.lib /nologo /base:"0x10900000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\musicbrowser.pdb" /machine:I386 /def:".\musicbrowser.def" /out:"musicbrowser.ui" /implib:"$(OUTDIR)\musicbrowser.lib" /libpath:"..\..\..\..\lib\gdbm" /libpath:"..\..\..\..\base\win32" /libpath:"..\..\..\..\lib\musicbrainz" 
DEF_FILE= \
	".\musicbrowser.def"
LINK32_OBJS= \
	"$(INTDIR)\DataIndex.obj" \
	"$(INTDIR)\Dialog.obj" \
	"$(INTDIR)\DropObject.obj" \
	"$(INTDIR)\DropSource.obj" \
	"$(INTDIR)\DropTarget.obj" \
	"$(INTDIR)\EditTrackInfoDialog.obj" \
	"$(INTDIR)\EnumFormatEtc.obj" \
	"$(INTDIR)\Event.obj" \
	"$(INTDIR)\FavoriteDialog.obj" \
	"$(INTDIR)\FreeAmpStreams.obj" \
	"$(INTDIR)\IntroductionWizard.obj" \
	"$(INTDIR)\MissingFileDialog.obj" \
	"$(INTDIR)\MultiSelectTreeView.obj" \
	"$(INTDIR)\MusicSearchDialog.obj" \
	"$(INTDIR)\MusicTree.obj" \
	"$(INTDIR)\OpenSavePlaylists.obj" \
	"$(INTDIR)\PerformDBCheckDialog.obj" \
	"$(INTDIR)\PlaylistView.obj" \
	"$(INTDIR)\RemoveTracksDialog.obj" \
	"$(INTDIR)\SavePlaylistDialog.obj" \
	"$(INTDIR)\Win32MusicBrowser.obj" \
	"$(INTDIR)\musicbrowser.res" \
	"..\..\..\..\base\win32\fabaselib.lib" \
	"..\..\..\..\lib\gdbm\gdbm.lib"

".\musicbrowser.ui" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "gdbm - Win32 NASM Release" "fabaselib - Win32 NASM Release" "..\..\..\..\config\config.h" ".\musicbrowser.ui"
   IF NOT EXIST ..\..\..\..\base\win32\prj\plugins mkdir                                                                                                     ..\..\..\..\base\win32\prj\plugins
	copy musicbrowser.ui                                                                                       ..\..\..\..\base\win32\prj\plugins
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("musicbrowser.dep")
!INCLUDE "musicbrowser.dep"
!ELSE 
!MESSAGE Warning: cannot find "musicbrowser.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "musicbrowser - Win32 Release" || "$(CFG)" == "musicbrowser - Win32 Debug" || "$(CFG)" == "musicbrowser - Win32 NASM Debug" || "$(CFG)" == "musicbrowser - Win32 NASM Release"
SOURCE=..\..\..\..\config\config.win32

!IF  "$(CFG)" == "musicbrowser - Win32 Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 NASM Debug"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 NASM Release"

InputPath=..\..\..\..\config\config.win32

"..\..\..\..\config\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy ..\..\..\..\config\config.win32 ..\..\..\..\config\config.h
<< 
	

!ENDIF 

SOURCE=..\src\DataIndex.cpp

"$(INTDIR)\DataIndex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\Dialog.cpp

"$(INTDIR)\Dialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\DropObject.cpp

"$(INTDIR)\DropObject.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\DropSource.cpp

"$(INTDIR)\DropSource.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\DropTarget.cpp

"$(INTDIR)\DropTarget.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\EditTrackInfoDialog.cpp

"$(INTDIR)\EditTrackInfoDialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\EnumFormatEtc.cpp

"$(INTDIR)\EnumFormatEtc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\Event.cpp

"$(INTDIR)\Event.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\FavoriteDialog.cpp

"$(INTDIR)\FavoriteDialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\FreeAmpStreams.cpp

"$(INTDIR)\FreeAmpStreams.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\IntroductionWizard.cpp

"$(INTDIR)\IntroductionWizard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\MissingFileDialog.cpp

"$(INTDIR)\MissingFileDialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\MultiSelectTreeView.cpp

"$(INTDIR)\MultiSelectTreeView.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\MusicSearchDialog.cpp

"$(INTDIR)\MusicSearchDialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\MusicTree.cpp

"$(INTDIR)\MusicTree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\OpenSavePlaylists.cpp

"$(INTDIR)\OpenSavePlaylists.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\PerformDBCheckDialog.cpp

"$(INTDIR)\PerformDBCheckDialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\PlaylistView.cpp

"$(INTDIR)\PlaylistView.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\RemoveTracksDialog.cpp

"$(INTDIR)\RemoveTracksDialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\SavePlaylistDialog.cpp

"$(INTDIR)\SavePlaylistDialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\Win32MusicBrowser.cpp

"$(INTDIR)\Win32MusicBrowser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\res\musicbrowser.rc

!IF  "$(CFG)" == "musicbrowser - Win32 Release"


"$(INTDIR)\musicbrowser.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\musicbrowser.res" /i "\Local\src\freeamp\ui\musicbrowser\win32\res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "musicbrowser - Win32 Debug"


"$(INTDIR)\musicbrowser.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\musicbrowser.res" /i "\Local\src\freeamp\ui\musicbrowser\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "musicbrowser - Win32 NASM Debug"


"$(INTDIR)\musicbrowser.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\musicbrowser.res" /i "\Local\src\freeamp\ui\musicbrowser\win32\res" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "musicbrowser - Win32 NASM Release"


"$(INTDIR)\musicbrowser.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\musicbrowser.res" /i "\Local\src\freeamp\ui\musicbrowser\win32\res" /d "NDEBUG" $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "musicbrowser - Win32 Release"

"fabaselib - Win32 Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" 
   cd "..\..\..\ui\musicbrowser\win32\prj"

"fabaselib - Win32 ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\ui\musicbrowser\win32\prj"

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 Debug"

"fabaselib - Win32 Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" 
   cd "..\..\..\ui\musicbrowser\win32\prj"

"fabaselib - Win32 DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\ui\musicbrowser\win32\prj"

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 NASM Debug"

"fabaselib - Win32 NASM Debug" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" 
   cd "..\..\..\ui\musicbrowser\win32\prj"

"fabaselib - Win32 NASM DebugCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Debug" RECURSE=1 CLEAN 
   cd "..\..\..\ui\musicbrowser\win32\prj"

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 NASM Release"

"fabaselib - Win32 NASM Release" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" 
   cd "..\..\..\ui\musicbrowser\win32\prj"

"fabaselib - Win32 NASM ReleaseCLEAN" : 
   cd "\Local\src\freeamp\base\win32\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\fabaselib.mak CFG="fabaselib - Win32 NASM Release" RECURSE=1 CLEAN 
   cd "..\..\..\ui\musicbrowser\win32\prj"

!ENDIF 

!IF  "$(CFG)" == "musicbrowser - Win32 Release"

"gdbm - Win32 Release" : 
   cd "\Local\src\freeamp\lib\gdbm\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\gdbm.mak CFG="gdbm - Win32 Release" 
   cd "..\..\..\ui\musicbrowser\win32\prj"

"gdbm - Win32 ReleaseCLEAN" : 
   cd "\Local\src\freeamp\lib\gdbm\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\gdbm.mak CFG="gdbm - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\ui\musicbrowser\win32\prj"

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 Debug"

"gdbm - Win32 Debug" : 
   cd "\Local\src\freeamp\lib\gdbm\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\gdbm.mak CFG="gdbm - Win32 Debug" 
   cd "..\..\..\ui\musicbrowser\win32\prj"

"gdbm - Win32 DebugCLEAN" : 
   cd "\Local\src\freeamp\lib\gdbm\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\gdbm.mak CFG="gdbm - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\ui\musicbrowser\win32\prj"

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 NASM Debug"

"gdbm - Win32 NASM Debug" : 
   cd "\Local\src\freeamp\lib\gdbm\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\gdbm.mak CFG="gdbm - Win32 NASM Debug" 
   cd "..\..\..\ui\musicbrowser\win32\prj"

"gdbm - Win32 NASM DebugCLEAN" : 
   cd "\Local\src\freeamp\lib\gdbm\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\gdbm.mak CFG="gdbm - Win32 NASM Debug" RECURSE=1 CLEAN 
   cd "..\..\..\ui\musicbrowser\win32\prj"

!ELSEIF  "$(CFG)" == "musicbrowser - Win32 NASM Release"

"gdbm - Win32 NASM Release" : 
   cd "\Local\src\freeamp\lib\gdbm\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\gdbm.mak CFG="gdbm - Win32 NASM Release" 
   cd "..\..\..\ui\musicbrowser\win32\prj"

"gdbm - Win32 NASM ReleaseCLEAN" : 
   cd "\Local\src\freeamp\lib\gdbm\prj"
   $(MAKE) /$(MAKEFLAGS) /F .\gdbm.mak CFG="gdbm - Win32 NASM Release" RECURSE=1 CLEAN 
   cd "..\..\..\ui\musicbrowser\win32\prj"

!ENDIF 


!ENDIF 

