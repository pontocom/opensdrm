/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player

	Portions Copyright (C) 1998-1999 EMusic.com

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
	
	$Id: preferences.h,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PREFERENCES_H
#define INCLUDED_PREFERENCES_H

#include "config.h"
#include "errors.h"

// preferences
extern const char* kInstallDirPref;
extern const char* kLibraryPathPref;
extern const char* kUIPref;
extern const char* kTextUIPref;
extern const char* kMusicBrowserUIPref;
extern const char* kDownloadManagerUIPref;
extern const char* kToolbarUIPref;
extern const char* kSecondaryUIPref;
extern const char* kPMOPref;
extern const char* kALSADevicePref;
extern const char* kESOUNDHostPref;
extern const char* kOpenSaveDirPref;
extern const char* kStayOnTopPref;
extern const char* kLiveInTrayPref;
extern const char* kInputBufferSizePref;
extern const char* kOutputBufferSizePref;
extern const char* kStreamBufferIntervalPref;
extern const char* kDecoderThreadPriorityPref;
extern const char* kUseDebugLogPref;
extern const char* kLogMainPref;
extern const char* kLogDecodePref;
extern const char* kLogInputPref;
extern const char* kLogOutputPref;
extern const char* kLogPerformancePref;
extern const char* kSaveStreamsPref;
extern const char* kSaveStreamsDirPref;
extern const char* kUseProxyPref;
extern const char* kProxyHostPref;
extern const char* kPreBufferPref;
extern const char* kUseAlternateNICPref;
extern const char* kAlternateNICAddressPref;
extern const char* kUseTitleStreamingPref;  
extern const char* kThemePathPref;
extern const char* kMainWindowPosPref;
extern const char* kWindowModePref;
extern const char* kThemeDefaultFontPref;
extern const char* kSaveMusicDirPref;
extern const char* kDatabaseDirPref;
extern const char* kCheckForUpdatesPref;
extern const char* kPlaylistRepeatPref;
extern const char* kPlaylistShufflePref;
extern const char* kTimeDisplayPref;
extern const char* kVolumePref;
extern const char* kUserNamePref;
extern const char* kReclaimFiletypesPref;
extern const char* kAskToReclaimFiletypesPref;
extern const char* kUsersPortablePlayersPref;
extern const char* kShowToolbarTextLabelsPref;
extern const char* kShowToolbarImagesPref;
extern const char* kSaveCurrentPlaylistOnExitPref;
extern const char* kViewMusicBrowserPref;
extern const char* kWelcomePref;
extern const char* kPlayImmediatelyPref;
extern const char* kSetLastResumePref;
extern const char* kNumberOfURLsToRememberPref;
extern const char* kCDDevicePathPref;
extern const char* kCheckCDAutomaticallyPref;
extern const char* kMBServerPref;
extern const char* kConvertUnderscoresToSpacesPref;
extern const char* kAllowMultipleInstancesPref;
extern const char* kAudioCDLengthPref;
extern const char* kWatchThisDirectoryPref;
extern const char* kWatchThisDirTimeoutPref;
extern const char* kWriteID3v1Pref;
extern const char* kWriteID3v2Pref;
extern const char* kEqualizerSettingsPref;
extern const char* kSavedPlaylistPositionPref;
extern const char* kMusicBrowserPositionPref;
extern const char* kMusicBrowserHeaderWidthsPref;
extern const char* kCloseDLMOnCompletePref;
extern const char* kPerformDBCheckPref;
extern const char* kPlaylistHeaderColumnsPref;
extern const char* kAdvancedRelatablePref;
extern const char* kMetadataDisplayPref;
extern const char* kEnableMusicBrainzPref;
extern const char* kEnableBitziPref;
extern const char* kLastFreeAmpVersionPref;

extern const int32 kDefaultWatchThisDirTimeout;

class LibDirFindHandle;

class Preferences {
  public:
    virtual ~Preferences() { }

    virtual Error SetDefaults();
    virtual Error Save() = 0;

    virtual Preferences *ComponentPrefs(const char *componentName) = 0;

    // foundation preference functions
    virtual Error GetPrefString(const char* pref, char* buf, uint32* len) = 0;
    virtual Error SetPrefString(const char* pref, const char* buf) = 0;

    virtual Error GetPrefBoolean(const char* pref, bool* value);
    virtual Error SetPrefBoolean(const char* pref, bool value);

    virtual Error GetPrefInt32(const char* pref, int32* value);
    virtual Error SetPrefInt32(const char* pref, int32 value);

    // Iterators for the freeamp library path
    virtual LibDirFindHandle *GetFirstLibDir(char *path, uint32 *len);
    virtual Error GetNextLibDir(LibDirFindHandle *hLibDirFind,
				char *path, uint32 *len);
    virtual Error GetLibDirClose(LibDirFindHandle *hLibDirFind);

    // XXX: This is a non-portable hack, and should only be used for user
    //      messages.  Normally, you should use the iterators above instead.
    virtual const char *GetLibDirs() = 0;    
};

#endif /* _PREFERENCES_H */
