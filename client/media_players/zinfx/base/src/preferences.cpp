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
        
        $Id: preferences.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#include <string.h>
#include <stdlib.h>

#ifdef __QNX__
#include <strings.h>
#endif

#include "preferences.h"


// preferences
const char* kInstallDirPref = "InstallDirectory";
const char* kLibraryPathPref = "LibraryPath";
const char* kUIPref = "UI";
const char* kTextUIPref = "TextUI";
const char* kMusicBrowserUIPref = "MusicBrowserUI";
const char* kDownloadManagerUIPref = "DownloadManagerUI";
const char* kToolbarUIPref = "ToolbarUI";
const char* kSecondaryUIPref = "SecondaryUI";
const char* kPMOPref = "PMO";
const char* kALSADevicePref = "ALSADevice";
const char* kESOUNDHostPref = "EsounDHost";
const char* kOpenSaveDirPref = "OpenSaveDirectory";
const char* kStayOnTopPref = "StayOnTop";
const char* kLiveInTrayPref = "LiveInTray";
const char* kInputBufferSizePref = "InputBufferSize";
const char* kOutputBufferSizePref = "OutputBufferSize";
const char* kStreamBufferIntervalPref = "StreamBufferInterval";
const char* kDecoderThreadPriorityPref = "DecoderThreadPriority";
const char* kSaveStreamsPref = "SaveStreams";
const char* kSaveStreamsDirPref = "SaveStreamsDirectory";
const char* kUseProxyPref = "UseProxy";
const char* kProxyHostPref = "ProxyHost";  
const char* kPreBufferPref = "PreBuffer";  
const char* kUseAlternateNICPref = "UseAlternateNIC";  
const char* kAlternateNICAddressPref = "AlternateNICAddress";  
const char* kUseTitleStreamingPref = "UseTitleStreaming";  
const char* kThemePathPref = "ThemePath";  
const char* kMainWindowPosPref = "MainWindowPos";  
const char* kThemeDefaultFontPref = "ThemeDefaultFont";
const char* kSaveMusicDirPref = "SaveMusicDirectory";
const char* kDatabaseDirPref = "DatabaseDirectory";
const char* kCheckForUpdatesPref = "CheckForUpdates";
const char* kPlaylistRepeatPref = "PlaylistRepeat";
const char* kPlaylistShufflePref = "PlaylistShuffle";
const char* kTimeDisplayPref = "TimeDisplay";
const char* kVolumePref = "Volume";
const char* kUserNamePref = "UserName";
const char* kReclaimFiletypesPref = "ReclaimFiletypes";
const char* kAskToReclaimFiletypesPref = "AskToReclaimFiletypes";
const char* kUsersPortablePlayersPref = "UsersPortablePlayers";
const char* kShowToolbarTextLabelsPref = "ShowToolbarTextLabels";
const char* kShowToolbarImagesPref = "ShowToolbarImages";
const char* kSaveCurrentPlaylistOnExitPref = "SaveCurrentPlaylistOnExit";
const char* kViewMusicBrowserPref = "ViewMusicBrowser";
const char* kWindowModePref = "WindowMode";
const char* kWelcomePref = "Welcome";
const char* kPlayImmediatelyPref = "PlayImmediately";
const char* kSetLastResumePref = "SetLastResume";
const char* kNumberOfURLsToRememberPref = "NumberOfURLsToRemember";
const char* kCDDevicePathPref = "CDDevice";
const char* kCheckCDAutomaticallyPref = "AutoUpdateCD";
const char* kMBServerPref = "MBServer";
const char* kConvertUnderscoresToSpacesPref = "ConvertUnderscoresToSpaces";
const char* kAllowMultipleInstancesPref = "AllowMultipleInstances";
const char* kAudioCDLengthPref = "AudioCDLength";
const char* kWatchThisDirectoryPref = "WatchThisDirectory";
const char* kWatchThisDirTimeoutPref = "WatchThisDirectoryTimeout";
const char* kWriteID3v1Pref = "WriteID3v1Tags";
const char* kWriteID3v2Pref = "WriteID3v2Tags";
const char* kEqualizerSettingsPref = "EqualizerSettings";
const char* kSavedPlaylistPositionPref = "SavedPlaylistPosition";
const char* kMusicBrowserPositionPref = "MusicBrowserPosition";
const char* kMusicBrowserHeaderWidthsPref = "MusicBrowserHeaderWidths";
const char* kCloseDLMOnCompletePref = "CloseDLMOnComplete";
const char* kPerformDBCheckPref = "PerformDBCheck";
const char* kPlaylistHeaderColumnsPref = "PlaylistHeaderColumns";
const char* kAdvancedRelatablePref = "UseAdvancedRelatableFeatures";
const char* kMetadataDisplayPref = "DisplayMetadata";
const char* kEnableMusicBrainzPref = "ContributeMB";
const char* kEnableBitziPref = "ContributeBitzi";
const char* kLastFreeAmpVersionPref = "LastFreeAmpVersion";

//logging
const char* kUseDebugLogPref = "UseDebugLog";
const char* kLogMainPref = "LogMain";
const char* kLogDecodePref = "LogDecode";
const char* kLogInputPref = "LogInput";
const char* kLogOutputPref = "LogOutput";
const char* kLogPerformancePref = "LogPerformance";

// default values
const int32 kDefaultInputBufferSize = 64;
const int32 kDefaultOutputBufferSize = 512;
const int32 kDefaultStreamBufferInterval = 3;
const int32 kDefaultDecoderThreadPriority = 5;
const bool  kDefaultLogging = false;
const bool  kDefaultSaveStreams = false;
const char *kDefaultSaveStreamsDir = ".";
const int32 kDefaultPreBuffer = 0;
const char *kDefaultProxyHost = "http://proxy.yourdomain.com:8080";
const bool  kDefaultUseProxy = false;
const bool  kDefaultUseNIC = false;
const char *kDefaultNICAddress = "192.168.1.1";
const bool  kDefaultUseTitleStreaming = true;
const char *kDefaultThemePath = "";  
const char *kDefaultMainWindowPos = "-1,-1";  
const char *kDefaultWindowMode = "MainWindow";
#ifdef WIN32
const char *kDefaultThemeDefaultFont = "Arial";
const char *kDefaultCDDevicePath = "cdaudio";
const bool  kDefaultCheckCDAutomatically = true;
#else
const char *kDefaultThemeDefaultFont = "Helvetica";
const char *kDefaultCDDevicePath = "/dev/cdrom";
const bool  kDefaultCheckCDAutomatically = false;
#endif
const char *kDefaultDownloadManagerUI = "download.ui";
const char *kDefaultMusicBrowserUI = "musicbrowser.ui";
const char *kDefaultToolbarUI = "toolbar.ui";
const char *kDefaultSecondaryUI = "";
const bool kDefaultCheckForUpdates = false;
const int32 kDefaultPlaylistRepeat = 0;
const bool kDefaultPlaylistShuffle = false;
const int32 kDefaultTimeDisplay = 0;
const int32 kDefaultVolume = 85;
const char* kDefaultUserName = "";
const bool kDefaultReclaimFiletypes = true;
const bool kDefaultAskToReclaimFiletypesPref = true;
const char* kDefaultUsersPortablePlayers = "";
const bool kDefaultShowToolbarTextLabels = true;
const bool kDefaultShowToolbarImages = true;
const bool kDefaultSaveCurrentPlaylistOnExit = false;
const bool kDefaultViewMusicBrowser = true;
const bool kDefaultWelcome = true;
const bool kDefaultPlayImmediately = true;
const bool kDefaultSetLastResume = true;
const int32 kDefaultNumberOfURLsToRemember = 10;
const char* kDefaultMBServer = "http://www.musicbrainz.org:80";
const bool kDefaultConvertUnderscoresToSpaces = true;
const bool kDefaultAllowMultipleInstances = false;
const int32 kDefaultAudioCDLength = 4440; // 74 minutes
const int32 kDefaultWatchThisDirTimeout = 600000;
const bool  kDefaultWriteID3v1 = true;
const bool  kDefaultWriteID3v2 = true;
const char* kDefaultEqualizerSettings = "0,50,50,50,50,50,50,50,50,50,50";
const uint32 kDefaultSavedPlaylistPosition = 0;
const char* kDefaultMusicBrowserPosition = "-1,-1,-1,-1,-1";
const char* kDefaultMusicBrowserHeaderWidths = "-1,-1,-1.-1";
const bool  kDefaultCloseDLMOnComplete = false;
const bool kDefaultPerformDBCheck = true;
const char* kDefaultPlaylistHeaderColumns = "Title|Artist|Album|Time|Genre";
const bool kDefaultAdvancedRelatable = false;
const bool kDefaultMetadataDisplay = 1;
const bool kDefaultEnableMusicBrainz = false;
const bool kDefaultEnableBitzi = false;
const char *kDefaultLastFreeAmpVersion = "0.0.0";

Error
Preferences::
SetDefaults()
{
    int32 dummyInt;
    bool  dummyBool;
    char  dummyString[255];
    uint32 size = 255;

    // set default for input buffer size
    if (GetPrefInt32(kInputBufferSizePref, &dummyInt) == kError_NoPrefValue)
        SetPrefInt32(kInputBufferSizePref, kDefaultInputBufferSize);

    // set default for output buffer size
    if (GetPrefInt32(kOutputBufferSizePref, &dummyInt) == kError_NoPrefValue)
        SetPrefInt32(kOutputBufferSizePref, kDefaultOutputBufferSize);

    // set default for streaming buffer interval
    if (GetPrefInt32(kStreamBufferIntervalPref, &dummyInt)
        == kError_NoPrefValue)
        SetPrefInt32(kStreamBufferIntervalPref, 
                     kDefaultStreamBufferInterval);

    // set default for decoder thread priority
    if (GetPrefInt32(kDecoderThreadPriorityPref, &dummyInt)
        == kError_NoPrefValue)
        SetPrefInt32(kDecoderThreadPriorityPref, 
                     kDefaultDecoderThreadPriority);

    // set defaults for logging
    if (GetPrefBoolean(kUseDebugLogPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kUseDebugLogPref, kDefaultLogging);
    if (GetPrefBoolean(kLogMainPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kLogMainPref, kDefaultLogging);
    if (GetPrefBoolean(kLogDecodePref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kLogDecodePref, kDefaultLogging);
    if (GetPrefBoolean(kLogInputPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kLogInputPref, kDefaultLogging);
    if (GetPrefBoolean(kLogOutputPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kLogOutputPref, kDefaultLogging);
    if (GetPrefBoolean(kLogPerformancePref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kLogPerformancePref, kDefaultLogging);

    if (GetPrefBoolean(kSaveStreamsPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kSaveStreamsPref, kDefaultSaveStreams);

    if (GetPrefString(kSaveStreamsDirPref, dummyString, &size) == kError_NoPrefValue)
        SetPrefString(kSaveStreamsDirPref, kDefaultSaveStreamsDir);

    if (GetPrefInt32(kPreBufferPref, &dummyInt) == kError_NoPrefValue)
        SetPrefInt32(kPreBufferPref, kDefaultPreBuffer);

    if (GetPrefBoolean(kUseProxyPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kUseProxyPref, kDefaultUseProxy);

    dummyInt = 255;
    if (GetPrefString(kProxyHostPref, dummyString, 
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kProxyHostPref, kDefaultProxyHost);

    dummyInt = 255;
    if (GetPrefString(kMBServerPref, dummyString, (uint32 *)&dummyInt) == 
        kError_NoPrefValue)
        SetPrefString(kMBServerPref, kDefaultMBServer);

    if (GetPrefBoolean(kUseAlternateNICPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kUseAlternateNICPref, kDefaultUseNIC);

    dummyInt = 255;
    if (GetPrefString(kAlternateNICAddressPref, dummyString, 
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kAlternateNICAddressPref, kDefaultNICAddress);

    if (GetPrefBoolean(kUseTitleStreamingPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kUseTitleStreamingPref, kDefaultUseTitleStreaming);

    dummyInt = 255;
    if (GetPrefString(kThemePathPref, dummyString, 
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kThemePathPref, kDefaultThemePath);

    dummyInt = 255;
    if (GetPrefString(kMainWindowPosPref, dummyString, 
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kMainWindowPosPref, kDefaultMainWindowPos);

    dummyInt = 255;
    if (GetPrefString(kWindowModePref, dummyString, 
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kWindowModePref, kDefaultWindowMode);

    dummyInt = 255;
    if (GetPrefString(kThemeDefaultFontPref, dummyString, 
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kThemeDefaultFontPref, kDefaultThemeDefaultFont);

    dummyInt = 255;
    if (GetPrefString(kMusicBrowserUIPref, dummyString,
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kMusicBrowserUIPref, kDefaultMusicBrowserUI);

    dummyInt = 255;
    if (GetPrefString(kDownloadManagerUIPref, dummyString,
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kDownloadManagerUIPref, kDefaultDownloadManagerUI);

    dummyInt = 255;
    if (GetPrefString(kToolbarUIPref, dummyString,
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kToolbarUIPref, kDefaultToolbarUI);

    dummyInt = 255;
    if (GetPrefString(kSecondaryUIPref, dummyString,
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kSecondaryUIPref, kDefaultSecondaryUI);

    if (GetPrefBoolean(kCheckForUpdatesPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kCheckForUpdatesPref, kDefaultCheckForUpdates);

    if (GetPrefInt32(kPlaylistRepeatPref, &dummyInt) == kError_NoPrefValue)
        SetPrefInt32(kPlaylistRepeatPref, kDefaultPlaylistRepeat);

    if (GetPrefBoolean(kPlaylistShufflePref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kPlaylistShufflePref, kDefaultPlaylistShuffle);

    if (GetPrefInt32(kTimeDisplayPref, &dummyInt) == kError_NoPrefValue)
        SetPrefInt32(kTimeDisplayPref, kDefaultTimeDisplay);

    if (GetPrefInt32(kVolumePref, &dummyInt) == kError_NoPrefValue)
        SetPrefInt32(kVolumePref, kDefaultVolume);

    dummyInt = 255;
    if (GetPrefString(kUserNamePref, dummyString, 
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kUserNamePref, kDefaultUserName);

    dummyInt = 255;
    if (GetPrefString(kCDDevicePathPref, dummyString,
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kCDDevicePathPref, kDefaultCDDevicePath);

    if (GetPrefBoolean(kReclaimFiletypesPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kReclaimFiletypesPref, kDefaultReclaimFiletypes);

    if (GetPrefBoolean(kAskToReclaimFiletypesPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kAskToReclaimFiletypesPref, kDefaultAskToReclaimFiletypesPref);

    dummyInt = 255;
    if (GetPrefString(kUsersPortablePlayersPref, dummyString, 
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kUsersPortablePlayersPref, kDefaultUsersPortablePlayers);

    if (GetPrefBoolean(kShowToolbarTextLabelsPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kShowToolbarTextLabelsPref, kDefaultShowToolbarTextLabels);

    if (GetPrefBoolean(kShowToolbarImagesPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kShowToolbarImagesPref, kDefaultShowToolbarImages);

    if (GetPrefBoolean(kSaveCurrentPlaylistOnExitPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kSaveCurrentPlaylistOnExitPref, kDefaultSaveCurrentPlaylistOnExit);

    if (GetPrefBoolean(kViewMusicBrowserPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kViewMusicBrowserPref, kDefaultViewMusicBrowser);

    if (GetPrefBoolean(kWelcomePref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kWelcomePref, kDefaultWelcome);

    if (GetPrefBoolean(kPlayImmediatelyPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kPlayImmediatelyPref, kDefaultPlayImmediately);
	
    if (GetPrefBoolean(kSetLastResumePref, &dummyBool) == kError_NoPrefValue)
	SetPrefBoolean(kSetLastResumePref, kDefaultSetLastResume);

    if (GetPrefInt32(kNumberOfURLsToRememberPref, &dummyInt) == kError_NoPrefValue)
        SetPrefInt32(kNumberOfURLsToRememberPref, kDefaultNumberOfURLsToRemember);

    if (GetPrefBoolean(kConvertUnderscoresToSpacesPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kConvertUnderscoresToSpacesPref, kDefaultConvertUnderscoresToSpaces);

    if (GetPrefBoolean(kAllowMultipleInstancesPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kAllowMultipleInstancesPref, kDefaultAllowMultipleInstances);

    if (GetPrefInt32(kAudioCDLengthPref, &dummyInt) == kError_NoPrefValue)
        SetPrefInt32(kAudioCDLengthPref, kDefaultAudioCDLength);

    if (GetPrefInt32(kWatchThisDirTimeoutPref, &dummyInt) == kError_NoPrefValue)
        SetPrefInt32(kWatchThisDirTimeoutPref, kDefaultWatchThisDirTimeout);

    if (GetPrefBoolean(kWriteID3v1Pref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kWriteID3v1Pref, kDefaultWriteID3v1);

    if (GetPrefBoolean(kWriteID3v2Pref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kWriteID3v2Pref, kDefaultWriteID3v2);

    if (GetPrefBoolean(kCloseDLMOnCompletePref, &dummyBool) == 
        kError_NoPrefValue)
        SetPrefBoolean(kCloseDLMOnCompletePref, kDefaultCloseDLMOnComplete);

	dummyInt = 255;
    if (GetPrefString(kEqualizerSettingsPref, dummyString, 
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kEqualizerSettingsPref, kDefaultEqualizerSettings);

    if (GetPrefInt32(kSavedPlaylistPositionPref, &dummyInt) == kError_NoPrefValue)
        SetPrefInt32(kSavedPlaylistPositionPref, kDefaultSavedPlaylistPosition);

    dummyInt = 255;
    if (GetPrefString(kMusicBrowserPositionPref, dummyString, 
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kMusicBrowserPositionPref, kDefaultMusicBrowserPosition);

    dummyInt = 255;
    if (GetPrefString(kMusicBrowserHeaderWidthsPref, dummyString, 
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kMusicBrowserHeaderWidthsPref, kDefaultMusicBrowserHeaderWidths);

    if (GetPrefBoolean(kPerformDBCheckPref, &dummyBool) == 
        kError_NoPrefValue)
        SetPrefBoolean(kPerformDBCheckPref, kDefaultPerformDBCheck);

    if (GetPrefBoolean(kCheckCDAutomaticallyPref, &dummyBool) == 
        kError_NoPrefValue)
        SetPrefBoolean(kCheckCDAutomaticallyPref, kDefaultCheckCDAutomatically);

    dummyInt = 255;
    if (GetPrefString(kPlaylistHeaderColumnsPref, dummyString,
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kPlaylistHeaderColumnsPref, kDefaultPlaylistHeaderColumns);

    if (GetPrefBoolean(kAdvancedRelatablePref, &dummyBool) == 
        kError_NoPrefValue)
        SetPrefBoolean(kAdvancedRelatablePref, kDefaultAdvancedRelatable);

    if (GetPrefInt32(kMetadataDisplayPref, &dummyInt) == kError_NoPrefValue)
        SetPrefInt32(kMetadataDisplayPref, kDefaultMetadataDisplay);

    if (GetPrefBoolean(kEnableMusicBrainzPref, &dummyBool) == 
        kError_NoPrefValue)
        SetPrefBoolean(kEnableMusicBrainzPref, kDefaultEnableMusicBrainz);

    if (GetPrefBoolean(kEnableBitziPref, &dummyBool) == kError_NoPrefValue)
        SetPrefBoolean(kEnableBitziPref, kDefaultEnableBitzi);

    dummyInt = 255;
    if (GetPrefString(kLastFreeAmpVersionPref, dummyString,
        (uint32 *)&dummyInt) == kError_NoPrefValue)
        SetPrefString(kLastFreeAmpVersionPref, kDefaultLastFreeAmpVersion);

    return kError_NoErr;
}

Error
Preferences::
GetPrefBoolean(const char* pref, bool* value)
{
    Error error;
    char temp[32];
    uint32 size = sizeof(temp);

    *value = false;
    error = GetPrefString(pref, temp, &size);
    if (IsntError(error))
    {
        if (0 == strcasecmp(temp, "true"))
            *value = true;
        else if (0 != strcasecmp(temp, "false"))
        {
            // In the case of a syntax error, pretend the preference doesn't
            // exist, so that it'll be replaced by a default value.
            error = kError_NoPrefValue;
        }
    }
    return error;

}

Error
Preferences::
SetPrefBoolean(const char* pref, bool value)
{
    return SetPrefString(pref, value ? "true" : "false");
}

Error
Preferences::
GetPrefInt32(const char* pref, int32* value)
{
    Error error;
    char temp[32];
    char* endp;
    uint32 size = sizeof(temp);

    *value = -1;
    error = GetPrefString(pref, temp, &size);
    if (IsntError(error))
    {
        *value = strtol(temp, &endp, 10);
        // In the case of a syntax error, pretend the preference doesn't
        // exist, so that it'll be replaced by a default value.
        if (*endp != '\0')
            error = kError_NoPrefValue;
    }
    return error;
}

Error
Preferences::
SetPrefInt32(const char* pref, int32 value)
{
    Error error;
    bool isNeg = 0;
    char temp[32];
    char* p = temp + sizeof(temp);

    if (value < 0)
    {
        value = -value;
        isNeg = 1;
    }

    *--p = '\0';

    do
    {
        *--p = '0' + (value % 10);
        value /= 10;
    } while (value > 0);

    if (isNeg)
        *--p = '-';

    error = SetPrefString(pref, p);

    return error;
}


LibDirFindHandle *
Preferences::
GetFirstLibDir(char *path, uint32 *len)
{
    Error error;
    
    error = GetPrefString(kInstallDirPref, path, len);
    return IsError(error) ? 0 : (LibDirFindHandle *) 1;
}

Error
Preferences::
GetNextLibDir(LibDirFindHandle *hLibDirFind, char *path, uint32 *len)
{
    path[0] = '\0';
    return kError_NoMoreLibDirs;
}

Error
Preferences::
GetLibDirClose(LibDirFindHandle *hLibDirFind)
{
    return kError_NoErr;
}

const char *
Preferences::
GetLibDirs()
{
    return NULL;
}

