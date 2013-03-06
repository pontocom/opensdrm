/*____________________________________________________________________________
        
   FreeAmp - The Free MP3 Player

   Copyright (C) 1999 EMusic.com
   Portions Copyright (C) 1999 Valters Vingolds

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
        
   $Id: ZinfTheme.cpp,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786) 
#endif

#include <stdio.h> 
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#define _stat stat
#ifndef _S_IFDIR
#define _S_IFDIR S_IFDIR
#endif
#endif
#if defined(WIN32)
#define STRICT
#endif
#include "config.h"
#include "downloadmanager.h"

#ifdef __QNX__
#include <strings.h>
#endif

#ifdef HAVE_GTK
#include "GTKUtility.h"
#include "GTKPreferenceWindow.h"
#elif defined(WIN32)
#include "Win32Window.h"
#include "Win32PreferenceWindow.h"
//#include "win32updatemanager.h"
#include "resource.h"
extern HINSTANCE g_hinst;
#elif defined(__BEOS__)
#include "BeOSPreferenceWindow.h"
#include "win32impl.h"
#endif

#include "ZinfTheme.h"
#include "MessageDialog.h"
#include "eventdata.h"
#include "event.h"
#include "debug.h"
#include "PreferenceWindow.h"
#include "playlist.h"
#include "player.h"
#include "help.h"
#include "properties.h"
#include "missingfile.h"
#include "utility.h"

void WorkerThreadStart(void* arg);

#define DB Debug_v("%s:%d\n", __FILE__, __LINE__);

const char *szWelcomeMsg = "Welcome to "the_BRANDING"!";
const char *szParseError = "Parsing the Theme description failed. Cause: ";
const char *szCantFindHelpError = "Cannot find the help files. Please make "
                                  "sure that the help files are properly "
                                  "installed, and you are not running "
                                  the_BRANDING" from the build directory.";
const char *szKeepThemeMessage = "Would you like to keep this theme?";

struct OptionsArgs
{
   FreeAmpTheme *pThis;
   uint32        uDefaultPage;
}; 

extern    "C"
{
   UserInterface *Initialize(FAContext * context)
   {
      return new FreeAmpTheme(context);
   }
}

FreeAmpTheme::FreeAmpTheme(FAContext * context)
             :Theme(context)
{
   char   szTemp[255];
   uint32 iLen = 255;
   
   m_pContext = context;
   m_iCurrentSeconds = 0;
   m_iTotalSeconds = -1;
   m_iSeekSeconds = 0;
   m_bSeekInProgress = false;
   m_bVolumeChangeInProgress = false;
   m_iVolume = -1;
   m_iBalance = -1;
   m_iSeekPos = -1;
   m_iMuteVolume = -1;
   m_bPlayShown = true;
   m_oTitle = string("");
   m_eTimeDisplayState = kNormal;
   m_eTitleDisplayState = kNameArtist;
   m_oStreamInfo = string("");
//   m_pUpdateMan = NULL;
//   m_pUpdateThread = NULL;
   m_pOptionsThread = NULL;
   m_bInOptions = false;
   m_bShowBuffers = m_bPaused = m_bBufferingUp = false;
   m_iFramesSinceSeek = 2;
   m_fSecondsPerFrame = 0;

   m_eq = new Equalizer(context);
   m_eq->LoadSettings();

#if 0
#if defined( WIN32 )
    m_pUpdateMan = new Win32UpdateManager(m_pContext);
    m_pUpdateMan->DetermineLocalVersions();
    m_pUpdateMan->SetPlatform(string("WIN32"));
#if defined( _M_ALPHA )
     m_pUpdateMan->SetArchitecture(string("ALPHA"));
#else
     m_pUpdateMan->SetArchitecture(string("X86"));
#endif // _M_ALPHA

#endif // WIN32
#endif

   m_pContext->prefs->GetPrefString(kWindowModePref, szTemp, &iLen);
   if (iLen > 0)
      m_oCurrentWindow = string(szTemp);
   else   
      m_oCurrentWindow = string("MainWindow");

   LoadFreeAmpTheme();

}

FreeAmpTheme::~FreeAmpTheme()
{

    if (m_eq)
    {
        m_eq->SaveSettings();
        delete m_eq;
        m_eq = NULL;
    }
    if (m_pOptionsThread)
    {  
        delete m_pOptionsThread;
        m_pOptionsThread = NULL;
    }
//#if defined( WIN32 )
//    delete m_pUpdateMan;
//#endif // WIN32
}

Error FreeAmpTheme::Init(int32 startup_type)
{
    assert(this);

    m_iStartupType = startup_type;

    m_uiThread = Thread::CreateThread();
    m_uiThread->Create(WorkerThreadStart, this);

    return kError_NoErr;
}

void FreeAmpTheme::WorkerThread(void)
{
    char   szTemp[255];
    uint32 iLen = 255;
    Error  eRet;
    int32  iValue;

    m_pContext->prefs->GetPrefInt32(kTimeDisplayPref, &iValue);
    if (iValue)
       m_eTimeDisplayState = kTimeRemaining;
    iValue = -1;
    m_pContext->prefs->GetPrefInt32(kMetadataDisplayPref, &iValue);
    if (iValue < 0)
       iValue = 1;
    m_eTitleDisplayState = (TitleDisplayState)iValue;
    
    m_pContext->prefs->GetPrefString(kMainWindowPosPref, szTemp, &iLen);
    sscanf(szTemp, " %d , %d", &m_oWindowPos.x, &m_oWindowPos.y);

#if 0 //WIN32

    bool checkForUpdates = false;

    m_pContext->prefs->GetPrefBoolean(kCheckForUpdatesPref, &checkForUpdates);

    if(checkForUpdates)
    {
        m_pUpdateThread = Thread::CreateThread();
        m_pUpdateThread->Create(update_thread, this);
    }

#endif

    eRet = Theme::Run(m_oWindowPos);
    if (!IsError(eRet))
    {
       sprintf(szTemp, "%d,%d", m_oWindowPos.x, m_oWindowPos.y);
       m_pContext->prefs->SetPrefString(kMainWindowPosPref, szTemp);
       m_pContext->prefs->SetPrefString(kWindowModePref, m_oCurrentWindow.c_str());
    }
    else     
       m_pContext->target->AcceptEvent(new Event(CMD_QuitPlayer));
   
    m_pContext->prefs->SetPrefInt32(kTimeDisplayPref, 
                                    m_eTimeDisplayState == kTimeRemaining);
    m_pContext->prefs->SetPrefInt32(kMetadataDisplayPref, 
                                    (int)m_eTitleDisplayState);
}

void WorkerThreadStart(void* arg)
{
    FreeAmpTheme* ui = (FreeAmpTheme*)arg;
    ui->WorkerThread();
}

void FreeAmpTheme::LoadFreeAmpTheme(void)
{
   char    *szTemp;
   uint32   iLen = 255;
   string   oThemePath("");
   Error    eRet;
   struct  _stat buf;

   szTemp = new char[iLen];
   m_pContext->prefs->GetPrefString(kThemePathPref, szTemp, &iLen);

    if (strlen(szTemp) < 1) 
        strcpy(szTemp, BRANDING_DEFAULT_THEME);
 
   if (_stat(szTemp, &buf) < 0)
   {
      // If the theme doesn't exist, let's try to prepend the install/theme dir
      char   *dir;
      uint32  len = _MAX_PATH;

      dir = new char[_MAX_PATH];
   
      m_pContext->prefs->GetPrefString(kInstallDirPref, dir, &len);
      oThemePath = string(dir);
#if defined(unix)
      oThemePath += string(BRANDING_SHARE_PATH);
#endif
      oThemePath += string(DIR_MARKER_STR);    
      oThemePath += string("themes");
      oThemePath += string(DIR_MARKER_STR);    

      delete [] dir;
   }
   oThemePath += szTemp;
  
   // Save the theme.
   m_themeCache = oThemePath;

   eRet = LoadTheme(oThemePath, m_oCurrentWindow);
   
   if (IsError(eRet) && eRet != kError_InvalidParam)					   
   {
       MessageDialog oBox(m_pContext);
       string        oErr, oMessage(szParseError);

       GetErrorString(oErr);
       oMessage += oErr;
       oBox.Show(oMessage.c_str(), string(BRANDING), kMessageOk);
       m_pContext->target->AcceptEvent(new Event(CMD_QuitPlayer));
   }
   else if (eRet == kError_InvalidParam)
   {
       m_pContext->target->AcceptEvent(new Event(CMD_QuitPlayer));
   }
   else
      m_pContext->prefs->SetPrefString(kThemePathPref, oThemePath.c_str());

   iLen = 255; 
   m_pContext->prefs->GetPrefString(kThemeDefaultFontPref, szTemp, &iLen);
   SetDefaultFont(string(szTemp));
   
   delete [] szTemp;    
}


Error FreeAmpTheme::Close(void)
{
    Theme::Close();
    
    if (!m_uiThread)
       return kError_NoErr;
    
    m_uiThread->Join();
    delete m_uiThread;
    m_uiThread = NULL;

    return kError_NoErr;
}

Error FreeAmpTheme::AcceptEvent(Event * e)
{
   if (m_pWindow && m_pWindow->DisallowOutsideMessages())
      return kError_NoErr;

   switch (e->Type())
   {
      case INFO_ErrorMessage:
      {
         MessageDialog      oBox(m_pContext);
         ErrorMessageEvent *pEvent = (ErrorMessageEvent *)e;
         
         string oDesc(pEvent->GetErrorMessage());
  
         oBox.Show(oDesc.c_str(), string(BRANDING), kMessageOk);
         break;
      }
      case INFO_StatusMessage:
      {
         StatusMessageEvent *pEvent = (StatusMessageEvent *)e;
         
         string oDesc(pEvent->GetStatusMessage());
         m_pWindow->ControlStringValue("Info", true, oDesc);
         break;
      }
      case CMD_Cleanup:
      {
         string oName("MainWindow");
      	 Close();
         m_pContext->target->AcceptEvent(new Event(INFO_ReadyToDieUI));
         
         break;
      }   
      case INFO_Playing:
      {
         bool bEnable;
         int  iState;
         
         iState = 1;
         m_pWindow->ControlIntValue(string("PlayPause"), true, iState);
         iState = 1;
         m_pWindow->ControlIntValue(string("PlayStop"), true, iState);
         iState = 0;
         m_pWindow->ControlIntValue(string("MPause"), true, iState);
         bEnable = false;
         m_pWindow->ControlEnable(string("Play"), true, bEnable);
         bEnable = true;
         m_pWindow->ControlEnable(string("Pause"), true, bEnable);
         bEnable = true;
         m_pWindow->ControlEnable(string("Stop"), true, bEnable);
   
         bEnable = true;
         m_pWindow->ControlEnable(string("Volume"), true, bEnable);
         m_pWindow->ControlEnable(string("Mute"), true, bEnable);
         m_pWindow->ControlEnable(string("Balance"), true, bEnable);
         
         bEnable = false;
         m_pWindow->ControlEnable(string("StopIndicator"), true, bEnable);
         m_pWindow->ControlEnable(string("PauseIndicator"), true, bEnable);
         bEnable = true;
         m_pWindow->ControlEnable(string("PlayIndicator"), true, bEnable);

         m_bPlayShown = false;
         m_bPaused = false;
         
         break;
      }   
      case INFO_Paused:
      case INFO_Stopped:
      {
         int iState = 0;
         bool bEnable = true;
         string oEmpty("");
         
         iState = 0;
         m_pWindow->ControlIntValue(string("PlayPause"), true, iState);
         m_bPaused = e->Type() == INFO_Paused;
         iState = e->Type() == INFO_Paused ? 1 : 0;
         m_pWindow->ControlIntValue(string("PlayStop"), true, iState);
         iState = e->Type() == INFO_Paused ? 1 : 0;
         m_pWindow->ControlIntValue(string("MPause"), true, iState);
         bEnable = true;
         m_pWindow->ControlEnable(string("Play"), true, bEnable);
         
         
         bEnable = false;
         m_pWindow->ControlEnable(string("Pause"), true, bEnable);
         bEnable = false;
         m_pWindow->ControlEnable(string("Stop"), true, bEnable);
         m_bPlayShown = true;
         m_pWindow->ControlStringValue("BufferInfo", true, oEmpty);
         m_oStreamInfo = "";
         m_pWindow->ControlStringValue("StreamInfo", true, oEmpty);
         
         if (e->Type() == INFO_Stopped)
         {
            m_iSeekPos = 0;
            m_iTotalSeconds = -1;
            m_iCurrentSeconds = -1;
            m_pWindow->ControlIntValue(string("Seek"), true, m_iSeekPos);
            UpdateTimeDisplay(m_iCurrentSeconds);
            
            if ((int32)m_pContext->plm->GetCurrentIndex() < 0)
            {
                m_oTitle = szWelcomeMsg;
                
                m_pWindow->ControlStringValue("Title", true, m_oTitle);
                
                string title = BRANDING;
                m_pWindow->SetTitle(title);
            }    
            
            bEnable = false;
            m_pWindow->ControlEnable(string("Volume"), true, bEnable);
            m_pWindow->ControlEnable(string("Balance"), true, bEnable);
            m_pWindow->ControlEnable(string("Mute"), true, bEnable);
            m_pWindow->ControlEnable(string("StereoIndicator"), true, bEnable);
            m_pWindow->ControlEnable(string("MonoIndicator"), true, bEnable);

            m_pWindow->ControlStringValue("SampleRate", true, oEmpty);
            m_pWindow->ControlStringValue("BitRate", true, oEmpty);

            bEnable = false;
            m_pWindow->ControlEnable(string("PlayIndicator"), true, bEnable);
            m_pWindow->ControlEnable(string("PauseIndicator"), true, bEnable);
            bEnable = true;
            m_pWindow->ControlEnable(string("StopIndicator"), true, bEnable);
         }
         else {
            bEnable = false;
            m_pWindow->ControlEnable(string("PlayIndicator"), true, bEnable);
            m_pWindow->ControlEnable(string("StopIndicator"), true, bEnable);
            bEnable = true;
            m_pWindow->ControlEnable(string("PauseIndicator"), true, bEnable);
         }
         
         break;
      }   
      case INFO_DoneOutputting:
      {
         m_iSeekPos = 0;
         m_iTotalSeconds = -1;
         m_iCurrentSeconds = -1;
         m_pWindow->ControlIntValue(string("Seek"), true, m_iSeekPos);
         UpdateTimeDisplay(m_iCurrentSeconds);
         
         break;
      }   
      case INFO_MediaInfo:
      {
         MediaInfoEvent *info = (MediaInfoEvent *) e;
         string oName("Title"), oText;
         char  *pFoo = strrchr(info->m_filename, '/');
         bool   bSet = true;

         if (m_oTitle.length() == 0)
         {
             pFoo = (pFoo ? ++pFoo : info->m_filename);
             m_oTitle = string(pFoo);
             m_pWindow->ControlStringValue(oName, true, m_oTitle);
             oText = string(BRANDING": ") + string(pFoo);
             m_pWindow->SetTitle(oText);
         }

		 // Enable/disable the seek slider
         m_iTotalSeconds = (int32) info->m_totalSeconds;
         bSet = (m_iTotalSeconds < 0) ? false : true;
         oName = string("Seek");
         m_pWindow->ControlEnable(oName, true, bSet);

         m_pContext->target->AcceptEvent(new Event(CMD_GetVolume));
         break;
      }
 
      case INFO_PlaylistItemsUpdated:
      {
         PlaylistItemsUpdatedEvent *pInfo = 
            (PlaylistItemsUpdatedEvent *)e;

         vector<PlaylistItem*>::const_iterator i = pInfo->Items()->begin();

         for(; i != pInfo->Items()->end(); i++)
         {
            if((*i) == m_pContext->plm->GetCurrentItem())
            {
                UpdateMetaData(*i);
                break;
            }
         }
         
         break;
      }
      case INFO_PlaylistCurrentItemInfo:
      {
         if (m_pContext->plm->GetCurrentIndex() != kInvalidIndex)
             UpdateMetaData(m_pContext->plm->GetCurrentItem());
         break;
      }

      case INFO_MusicCatalogTrackRemoved:
      {
         if ((int32)m_pContext->plm->GetCurrentIndex() < 0)
         {
             m_oTitle = szWelcomeMsg;
             m_pWindow->ControlStringValue("Title", true, m_oTitle);
             string title = BRANDING;
             m_pWindow->SetTitle(title);
         }    
         break;   
      }
      case INFO_StreamInfo:
      {
         char *szTitle;
         string oName("Title"), oText;

         StreamInfoEvent *pInfo = (StreamInfoEvent *) e;

         szTitle = new char[100];
         pInfo->GetTitle(szTitle, 100);
         m_oTitle = string(szTitle);
         m_pWindow->ControlStringValue(oName, true, m_oTitle);
         oText = string(BRANDING": ") + string(szTitle);
         m_pWindow->SetTitle(oText);
         delete [] szTitle;

         break;
      }
      case INFO_HeadlineText:
      {
         string oName("HeadlineInfo"), oText;

         HeadlineMessageEvent *pInfo = (HeadlineMessageEvent *) e;

         oText = string(pInfo->GetHeadlineMessage());
         m_oHeadlineUrl = string(pInfo->GetHeadlineURL());
         m_pWindow->ControlStringValue(oName, true, oText);
         oName = string("HeadlineStreamInfo");
         m_pWindow->ControlStringValue(oName, true, oText);

         break;
      }
      case INFO_BufferStatus:
      {
         StreamBufferEvent *info = (StreamBufferEvent *) e;
         char *szTemp;
         string oName("BufferInfo"), oText;

         if (info->IsBufferingUp())
         {
             string oControl("Info"), oDesc;
             char   szText[64];
             
             sprintf(szText, "Buffering up [%d%%]", 
                min((info->GetInputPercent()*3)/2, (int32)100));
             oDesc = string(szText);
             m_pWindow->ControlStringValue(oControl, true, oDesc);
             m_bBufferingUp = true;
         }   
         if (!info->IsBufferingUp() && m_bBufferingUp == true)
         {
             string oControl("Info"), oDesc("Playing stream...");
             m_pWindow->ControlStringValue(oControl, true, oDesc);
             m_bBufferingUp = false;
         }   

         szTemp = new char[100];
         sprintf(szTemp, "I: %3ld O: %3ld %c", 
                 (long int)info->GetInputPercent(),
                 (long int)info->GetOutputPercent(),
                 info->IsBufferingUp() ? '^' : ' ');
         oText = string(szTemp);
         
         if (m_bShowBuffers)
             oName = string("StreamInfo");

         m_pWindow->ControlStringValue(oName, true, oText);
         delete [] szTemp;

         break;
      }
         
      case INFO_MediaTimeInfo:
      {
         MediaTimeInfoEvent *info = (MediaTimeInfoEvent *) e;
         int                 iSeconds;
         string              oName("Time"), oText;

         iSeconds = (info->m_hours * 3600) + (info->m_minutes * 60) + 
                     info->m_seconds;
         if ((iSeconds == m_iCurrentSeconds && iSeconds > 0) || 
             m_bSeekInProgress)
             break;

         if (m_iFramesSinceSeek++ < 1)
             break;

         m_iCurrentSeconds = iSeconds;            
         UpdateTimeDisplay(m_iCurrentSeconds);

         if (m_iTotalSeconds > 0)
         {
            m_iSeekPos = (iSeconds * 100) / m_iTotalSeconds;
            oName = string("Seek");
            m_pWindow->ControlIntValue(oName, true, m_iSeekPos);
         }   

         break;
      }
      case INFO_MPEGInfo:
      {
         MpegInfoEvent *info = (MpegInfoEvent *) e;
         char          *text;

         text = new char[100];
         m_fSecondsPerFrame = info->GetSecondsPerFrame();
         if (info->GetWAV())
              sprintf(text, "WAV Audio");
         else if (info->GetBitRate() == 1411200)
              sprintf(text, "CD Audio");
         else if (info->GetBitRate() == 0)
              sprintf(text, "VBR %ldkHz %s", 
                   (long int)(info->GetSampleRate() / 1000), 
                   info->GetChannels() ? "Stereo" : "Mono");
         else
              sprintf(text, "%ldkbps %ldkHz %s", 
                   (long int)(info->GetBitRate() / 1000),
                   (long int)(info->GetSampleRate() / 1000), 
                   info->GetChannels() ? "Stereo" : "Mono");

         m_oStreamInfo = text;
         m_pWindow->ControlStringValue("StreamInfo", true, m_oStreamInfo);

         sprintf(text, "%d", info->GetBitRate() / 1000);    
         string tempstr = text;
         m_pWindow->ControlStringValue("BitRate", true, tempstr);
         
         sprintf(text, "%d", info->GetSampleRate() / 1000);
         tempstr = text;
         m_pWindow->ControlStringValue("SampleRate", true, tempstr);

         bool bEnable = (info->GetChannels() != 0);
         m_pWindow->ControlEnable(string("StereoIndicator"), true, bEnable);
         bEnable = !bEnable;
         m_pWindow->ControlEnable(string("MonoIndicator"), true, bEnable);
         delete [] text;
      
         break;
      }
      case INFO_VorbisInfo:
      {
         VorbisInfoEvent *info = (VorbisInfoEvent *) e;
         char            *text;

         text = new char[100];
         m_fSecondsPerFrame = info->GetSecondsPerFrame();
              sprintf(text, "%ldkbps %ldkHz %s Vorbis", 
                   (long int)(info->GetBitRate() / 1000),
                   (long int)(info->GetSampleRate() / 1000), 
                   info->GetChannels() ? "Stereo" : "Mono");

         m_oStreamInfo = text;
         m_pWindow->ControlStringValue("StreamInfo", true, m_oStreamInfo);

         sprintf(text, "%d", info->GetBitRate() / 1000);    
         string tempstr = text;
         m_pWindow->ControlStringValue("BitRate", true, tempstr);
         
         sprintf(text, "%d", info->GetSampleRate() / 1000);
         tempstr = text;
         m_pWindow->ControlStringValue("SampleRate", true, tempstr);

         bool bEnable = (info->GetChannels() != 0);
         m_pWindow->ControlEnable(string("StereoIndicator"), true, bEnable);
         bEnable = !bEnable;
         m_pWindow->ControlEnable(string("MonoIndicator"), true, bEnable);
         delete [] text;
      
         break;
      }

      case INFO_PlaylistRepeat:
      {
         PlaylistRepeatEvent *plre = (PlaylistRepeatEvent *) e;
         int iState;

         switch (plre->GetRepeatMode())
         {
             case kPlaylistMode_RepeatNone:
                iState = 0;
                break;
             case kPlaylistMode_RepeatOne:
                iState = 1;
                break;
             case kPlaylistMode_RepeatAll:
                iState = 2;
                break;
             default:
                break;
         }
         m_pWindow->ControlIntValue(string("Repeat"), true, iState);
         break;
      }
      case INFO_PlaylistShuffle:
      {
         PlaylistShuffleEvent *plse = (PlaylistShuffleEvent *) e;
         int iState;

         iState = plse->GetShuffleMode() ? 1 : 0;
         m_pWindow->ControlIntValue(string("Shuffle"), true, iState);
         break;
      }
      case INFO_VolumeInfo:
      {
         int iLeft, iRight;
         
         iLeft = ((VolumeEvent *) e)->GetLeftVolume();
         iRight = ((VolumeEvent *) e)->GetRightVolume();

         if (iLeft > iRight)
         {
             m_iVolume = iLeft;
             m_iBalance = iRight * 50 / iLeft ;
         }
         else
         {
             m_iVolume = iRight;
			 if (iRight == 0)
				 m_iBalance = 50;
			 else
                 m_iBalance = 50 + (50 - (iLeft * 50 / iRight)); 
         }

         m_pWindow->ControlIntValue(string("Volume"), true, m_iVolume);
         m_pWindow->ControlIntValue(string("Balance"), true, m_iBalance);

         if (iLeft > 0 || iRight > 0)
         {
   	       int iState = 0;
             m_pWindow->ControlIntValue(string("Mute"), false, iState);
         }

         char VolumeText[255];
         sprintf(VolumeText, "%d%%", m_iVolume);
         string oVol = string(VolumeText);
         m_pWindow->ControlStringValue(string("VolumeText"), true, oVol);

         break;
      }

      case INFO_PrefsChanged:
      {
		 /*
		  * Make sure that the theme has changed before we
		  * bother to reload it.
		  */
	     char    *szTemp;
		 uint32   iLen = 255;
		 string   oThemePath("");
		 struct  _stat buf;
	     bool bValue;

		 szTemp = new char[iLen];
		 m_pContext->prefs->GetPrefString(kThemePathPref, szTemp, &iLen);

	    if (strlen(szTemp) < 1) 
		    strcpy(szTemp, BRANDING_DEFAULT_THEME);
 
		if (_stat(szTemp, &buf) < 0)
		{
			// If the theme doesn't exist, let's try to prepend the install/theme dir
			char   *dir;
			uint32  len = _MAX_PATH;

			dir = new char[_MAX_PATH];
   
			m_pContext->prefs->GetPrefString(kInstallDirPref, dir, &len);
			oThemePath = string(dir);
#if defined(unix)
		    oThemePath += string(BRANDING_SHARE_PATH);
#endif
			oThemePath += string(DIR_MARKER_STR);    
			oThemePath += string("themes");
	        oThemePath += string(DIR_MARKER_STR);    

			delete [] dir;
		}
		oThemePath += szTemp;

		//
		// Perform the reload - if necessary.
		//
        if ( strcmp( oThemePath.c_str(), m_themeCache.c_str() ) != 0 )
		{
			ReloadTheme();
			m_themeCache = oThemePath;
		}

        m_pContext->prefs->GetPrefBoolean(kStayOnTopPref, &bValue);
        m_pWindow->SetStayOnTop(bValue);
        m_pContext->prefs->GetPrefBoolean(kLiveInTrayPref, &bValue);
        m_pWindow->SetLiveInToolbar(bValue);
         
      	break;
      }
      case CMD_ShowPreferences:
      {
          ShowPreferencesEvent* prefsEvent = (ShowPreferencesEvent*)e;
      	 ShowOptions(prefsEvent->GetDefaultPage());
      	 break;
      }

      case CMD_LoadTheme:
      {
          char         *szSavedTheme, *szNewTheme;
          uint32        iLen = _MAX_PATH;
          string        oThemePath;
          MessageDialog oBox(m_pContext);
          string        oMessage(szKeepThemeMessage);

          szSavedTheme = new char[iLen];
          szNewTheme = new char[iLen];

          LoadThemeEvent *pInfo = (LoadThemeEvent *)e;
          strncpy(szNewTheme, pInfo->URL(), _MAX_PATH);
          szNewTheme[_MAX_PATH - 1] = 0;
          strncpy(szSavedTheme, pInfo->SavedTheme(), _MAX_PATH);
          szSavedTheme[_MAX_PATH - 1] = 0;

          m_pContext->prefs->SetPrefString(kThemePathPref, szNewTheme);
          ReloadTheme();
  
          if (oBox.Show(oMessage.c_str(), string(BRANDING), kMessageYesNo) == 
              kMessageReturnYes)
          {
              ThemeManager *pMan;
              string        oThemePath(szNewTheme);
          
              pMan = new ThemeManager(m_pContext);
              if (IsntError(pMan->AddTheme(oThemePath, true))) {
                  m_pContext->prefs->SetPrefString(kThemePathPref, oThemePath.c_str());
                  ReloadTheme();
              }
              delete pMan;    
          }
          else
          {
              m_pContext->prefs->SetPrefString(kThemePathPref, szSavedTheme);
              ReloadTheme();
          }
         
          unlink(szNewTheme);
          delete [] szSavedTheme;
          delete [] szNewTheme;
          
          break;
      }
      
      default:
         break;
   }
   return kError_NoErr;
}

Error FreeAmpTheme::HandleControlMessage(string &oControlName, 
                                         ControlMessageEnum eMesg)
{
   if (oControlName == string("WindowStatus")) 
   {
       bool bEnable;

       if (eMesg == CM_WindowEnter) 
           bEnable = true;
       else if (eMesg == CM_WindowLeave)
           bEnable = false;
       else
           return kError_NoErr;

       m_pWindow->ControlEnable(oControlName, true, bEnable);
       return kError_NoErr;
   } 

   if (eMesg == CM_MouseEnter)
   {
       string oName("Info"), oDesc("");

       if (oControlName == string("Time") ||
           oControlName == string("TimeRemaining"))
       {
           if (m_eTimeDisplayState == kTimeRemaining && 
               m_pWindow->DoesControlExist("TimeRemaining"))
               m_pWindow->ControlGetDesc("TimeRemaining", oDesc);
           else
               m_pWindow->ControlGetDesc("Time", oDesc);
       }
       else
           m_pWindow->ControlGetDesc(oControlName, oDesc);

       m_pWindow->ControlStringValue(oName, true, oDesc);
                                
       return kError_NoErr;
   }    
   if (eMesg == CM_MouseLeave)
   {
       string oName("Info"), oEmpty("");
      
       m_pWindow->ControlStringValue(oName, true, oEmpty);
                                
       return kError_NoErr;
   }
   if (eMesg == CM_TogglePanel)
   {
       m_pWindow->PanelStateChanged();
       return kError_NoErr;
   }
   if (oControlName == string("Minimize") && eMesg == CM_Pressed)
   {
       m_pWindow->Minimize();
       
       return kError_NoErr;
   }
   if (oControlName == string("Volume") && 
       (eMesg == CM_ValueChanged || eMesg == CM_SliderUpdate))
   {
       int iVol;

       if (eMesg == CM_SliderUpdate)
           m_bVolumeChangeInProgress = true;
           
       if (eMesg == CM_ValueChanged)
           m_bVolumeChangeInProgress = false;

       m_pWindow->ControlIntValue(oControlName, false, iVol);
       SetVolume(iVol, m_iBalance);
           
       return kError_NoErr;
   }    
   if (oControlName == string("Balance") && 
       (eMesg == CM_ValueChanged || eMesg == CM_SliderUpdate))
   {
       int iBal;

       m_pWindow->ControlIntValue(oControlName, false, iBal);
       SetVolume(m_iVolume, iBal);
           
       return kError_NoErr;
   }    
   if (oControlName == string("Mute") && eMesg == CM_Pressed)
   {
       int iState;

       m_pWindow->ControlIntValue(oControlName, false, iState);
       if (iState == 0)
       {
           m_iMuteVolume = m_iVolume;
           m_iVolume = 0;
           iState = 1;
       }
       else
       {
           m_iVolume = m_iMuteVolume;
           m_iMuteVolume = -1;
           iState = 0;
       }
       SetVolume(m_iVolume, m_iBalance);
       m_pWindow->ControlIntValue(oControlName, true, iState);
       return kError_NoErr;
   }
   
   if (eMesg == CM_SliderUpdate && strncmp(oControlName.c_str(), "Eq", 2) == 0)
   {
       string oName("Info"), oDesc;
       char   szText[20];
       int    iPos;

       m_pWindow->ControlIntValue(oControlName, false, iPos);
       m_eq->ChangeValue(atoi(oControlName.c_str() + 2), iPos); 
       sprintf(szText, "%d db", ((iPos - 50) * 2) / 5);
       oDesc = string(szText);
       m_pWindow->ControlStringValue(oName, true, oDesc);
                                
       return kError_NoErr;
   }    
   if (oControlName == string("Seek") && eMesg == CM_ValueChanged)
   {
       string oName("Info"), oEmpty("");
       int    iValue, iFrame;
       
       m_pWindow->ControlIntValue(oControlName, false, iValue);
       
       iFrame = (int)(((float)iValue * (float)m_iTotalSeconds) / 
                      ((float)100 * m_fSecondsPerFrame));

       m_bSeekInProgress = false;
      
       m_pContext->target->AcceptEvent(new 
              VolumeEvent(CMD_ChangePosition, iFrame + 1));
       m_iFramesSinceSeek = 0;
       
       m_pWindow->ControlStringValue(oName, true, oEmpty);
              
       return kError_NoErr;
   }    
   if (oControlName == string("Seek") && eMesg == CM_SliderUpdate)
   {
       string oName("Time"), oText("");
       int    iValue, iTime;
       
       m_pWindow->ControlIntValue(oControlName, false, iValue);
       
       iTime = (iValue * m_iTotalSeconds) / 100;
       UpdateTimeDisplay(iTime);
       m_bSeekInProgress = true;
              
       return kError_NoErr;
   }    
   if (oControlName == string("Preamp") && eMesg == CM_SliderUpdate)
   {
       string oName("Info"), oDesc;
       char   szText[20];
       int    iPos;

       m_pWindow->ControlIntValue(oControlName, false, iPos);
       m_eq->ChangePreamp(iPos); 
       sprintf(szText, "%d db", ((iPos - 50) * 10) / 25);
       oDesc = string(szText);
       m_pWindow->ControlStringValue(oName, true, oDesc);
                                
       return kError_NoErr;
   }    
   if (oControlName == string("PlayPause") && eMesg == CM_Pressed)
   {
       int iState = 0;

       if (m_pContext->plm->CountItems() == 0)
       {
           m_pContext->target->AcceptEvent(new Event(CMD_ToggleMusicBrowserUI));
           return kError_NoErr;
       }
       m_pWindow->ControlIntValue(oControlName, false, iState);
       if (iState == 0)
           m_pContext->target->AcceptEvent(new Event(CMD_Play));
       else
           m_pContext->target->AcceptEvent(new Event(CMD_Pause));
       return kError_NoErr;
   }
   
   if (oControlName == string("PlayStop") && eMesg == CM_Pressed)
   {
       int iState = 0;

       if (m_pContext->plm->CountItems() == 0)
       {
           m_pContext->target->AcceptEvent(new Event(CMD_ToggleMusicBrowserUI));
           return kError_NoErr;
       }
       m_pWindow->ControlIntValue(oControlName, false, iState);
       if (iState > 1)
          iState = 1;
       
       if (iState == 0)
           m_pContext->target->AcceptEvent(new Event(CMD_Play));
	   else
           m_pContext->target->AcceptEvent(new Event(CMD_Stop));
       return kError_NoErr;
   }
  
   if (oControlName == string("Play") && eMesg == CM_Pressed)
   {
       if (m_pContext->plm->CountItems() == 0)
       {
           m_pContext->target->AcceptEvent(new Event(CMD_ToggleMusicBrowserUI));
           return kError_NoErr;
       }
       m_pContext->target->AcceptEvent(new Event(CMD_Play));
       return kError_NoErr;
   }
   if (oControlName == string("MPause") && eMesg == CM_Pressed)
   {
   	 int iState = 0;
       m_pWindow->ControlIntValue(oControlName, false, iState);
       if (iState == 0)
           m_pContext->target->AcceptEvent(new Event(CMD_Pause));
	   else
           m_pContext->target->AcceptEvent(new Event(CMD_Play));
       return kError_NoErr;
   }
   if (oControlName == string("Pause") && eMesg == CM_Pressed)
   {
       m_pContext->target->AcceptEvent(new Event(CMD_Pause));
       return kError_NoErr;
   }
   if (oControlName == string("Stop") && eMesg == CM_Pressed)
   {
       m_pContext->target->AcceptEvent(new Event(CMD_Stop));
       return kError_NoErr;
   }
   if (oControlName == string("Next") && eMesg == CM_Pressed)
   {
       m_pContext->target->AcceptEvent(new Event(CMD_NextMediaPiece));
       return kError_NoErr;
   }
   if (oControlName == string("Prev") && eMesg == CM_Pressed)
   {
       m_pContext->target->AcceptEvent(new Event(CMD_PrevMediaPiece));
       return kError_NoErr;
   }
   if (oControlName == string("MyMusic") && eMesg == CM_Pressed)
   {
       m_pContext->target->AcceptEvent(new Event(CMD_ToggleMusicBrowserUI));
       return kError_NoErr;
   }
   if (oControlName == string("Files") && eMesg == CM_Pressed)
   {
       m_pContext->target->AcceptEvent(new Event(CMD_AddFiles));
       return kError_NoErr;
   }
   if (oControlName == string("Download") && eMesg == CM_Pressed)
   {
       m_pContext->target->AcceptEvent(new Event(CMD_ToggleDownloadUI));
       return kError_NoErr;
   }
   if (oControlName == string("Options") && eMesg == CM_Pressed)
   {
       m_pContext->target->AcceptEvent(new ShowPreferencesEvent(0));
       return kError_NoErr;
   }
   if (oControlName == string("Quit") && eMesg == CM_Pressed)
   {
       m_pContext->target->AcceptEvent(new Event(CMD_QuitPlayer));
       return kError_NoErr;
   }
   if (eMesg == CM_ChangeWindow)
   {
       m_pWindow->ControlStringValue(oControlName, false, m_oCurrentWindow);

       // This is cheating, but I don't know how else to do it. :-(
       m_pWindow->DecUsageRef();
       m_pWindow->DecUsageRef();
       SwitchWindow(m_oCurrentWindow);
       m_pWindow->IncUsageRef();
       m_pWindow->IncUsageRef();
   
       return kError_NoErr;
   }
   if (oControlName == string("ReloadTheme") && eMesg == CM_Pressed)
   {
       m_pWindow->DecUsageRef();
       m_pWindow->DecUsageRef();
       ReloadTheme();
       m_pWindow->IncUsageRef();
       m_pWindow->IncUsageRef();
       return kError_NoErr;
   }
   if (oControlName == string("Shuffle") && eMesg == CM_Pressed)
   {
       int iState = 0;
       string     oName("Info"), oDesc("");

       m_pWindow->ControlIntValue(oControlName, false, iState);
       iState = (iState + 1) % 2;
       m_pContext->plm->SetShuffleMode(iState == 1);
       m_pWindow->ControlIntValue(oControlName, true, iState);
       
       m_pWindow->ControlGetDesc(oControlName, oDesc);
       m_pWindow->ControlStringValue(oName, true, oDesc);
       
       return kError_NoErr;
   }    
   if (oControlName == string("Repeat") && eMesg == CM_Pressed)
   {
       int        iState = 0;
       RepeatMode eMode = kPlaylistMode_RepeatNone;
       string     oName("Info"), oDesc("");

       m_pWindow->ControlIntValue(oControlName, false, iState);
       iState = (iState + 1) % 3;
       m_pWindow->ControlIntValue(oControlName, true, iState);
       
       switch(iState)
       {
          case 0:  
              eMode = kPlaylistMode_RepeatNone;
              break;
          case 1:  
              eMode = kPlaylistMode_RepeatOne;
              break;
          case 2:  
              eMode = kPlaylistMode_RepeatAll;
              break;
       }
       m_pContext->plm->SetRepeatMode(eMode);
       
       m_pWindow->ControlGetDesc(oControlName, oDesc);
       m_pWindow->ControlStringValue(oName, true, oDesc);
       
       return kError_NoErr;
   }    
   if (oControlName == string("Time") && eMesg == CM_Pressed)
   {
       string oName("Info"), oDesc("");
       
       if (m_iTotalSeconds < 0)
           return kError_NoErr;       
          
       if (m_eTimeDisplayState == kNormal)
       {
           m_eTimeDisplayState = kTimeRemaining;
           if (m_pWindow->DoesControlExist("TimeRemaining"))
               m_pWindow->ControlStringValue("Time", true, oDesc);
       }    
       else     
       { 
           m_eTimeDisplayState = kNormal;
           if (m_pWindow->DoesControlExist("TimeRemaining"))
               m_pWindow->ControlStringValue("TimeRemaining", true, oDesc);
       }    
       
       m_pWindow->ControlStringValue("Info", true, oDesc);
       UpdateTimeDisplay(m_iCurrentSeconds);                      
   }
   if (oControlName == string("Logo") && eMesg == CM_Pressed)
   {
       string oUrl;
       
       m_pWindow->ControlStringValue("Logo", false, oUrl);
       if (oUrl.length() == 0)
          oUrl = BRANDING_URL;
       
#ifdef WIN32   
       Int32PropValue *pProp;
       HWND            hWnd;
       if (IsError(m_pContext->props->GetProperty("MainWindow", 
                   (PropValue **)&pProp)))
          hWnd = NULL;
       else
          hWnd = (HWND)pProp->GetInt32();
             
       ShellExecute(hWnd, "open", oUrl.c_str(),
                    NULL, NULL, SW_SHOWNORMAL);
#else
       LaunchBrowser((char *)oUrl.c_str());
#endif
       return kError_NoErr;
   }
   
   if (oControlName == string("HeadlineInfo") && eMesg == CM_Pressed)
   {
       if (m_oHeadlineUrl.length() == 0)
          return kError_NoErr;
          
#ifdef WIN32   
       Int32PropValue *pProp;
       HWND            hWnd;
       if (IsError(m_pContext->props->GetProperty("MainWindow", 
                   (PropValue **)&pProp)))
          hWnd = NULL;
       else
          hWnd = (HWND)pProp->GetInt32();
             
       ShellExecute(hWnd, "open", m_oHeadlineUrl.c_str(),
                    NULL, NULL, SW_SHOWNORMAL);
#else
       LaunchBrowser((char *)m_oHeadlineUrl.c_str());
#endif
       return kError_NoErr;
   }
   if (oControlName == string("HeadlineStreamInfo") && eMesg == CM_Pressed)
   {
       bool bPlay;

       if (m_oHeadlineUrl.length() == 0)
          return kError_NoErr;

       m_pContext->prefs->GetPrefBoolean(kPlayImmediatelyPref, &bPlay);
       if (bPlay)
       {
           m_pContext->target->AcceptEvent(new Event(CMD_Stop));
           m_pContext->plm->RemoveAll();
           m_pContext->plm->AddItem(m_oHeadlineUrl);
           m_pContext->target->AcceptEvent(new Event(CMD_Play));
       }
       else
           m_pContext->plm->AddItem(m_oHeadlineUrl);

       return kError_NoErr;
   }

   if (oControlName == string("Help") && eMesg == CM_Pressed)
   {
       ShowHelp();
       return kError_NoErr;
   }
   if (oControlName == string("Credits") && eMesg == CM_Pressed)
   {
       ShowThemeCredits();
       return kError_NoErr;
   }

   if (strncmp(oControlName.c_str(), "Eq", 2) == 0 && eMesg == CM_ValueChanged)
   {
       int iValue;

       m_pWindow->ControlIntValue(oControlName, false, iValue);
       m_eq->ChangeValue(atoi(oControlName.c_str() + 2), iValue); 
       return kError_NoErr;
   }
   if (oControlName == string("Preamp") && eMesg == CM_ValueChanged)
   {
       int iValue;

       m_pWindow->ControlIntValue(oControlName, false, iValue);
       m_eq->ChangePreamp(iValue); 
       return kError_NoErr;
   }
   if (oControlName == string("EqEnable") && eMesg == CM_Pressed)
   {
       int iState;

       m_pWindow->ControlIntValue(oControlName, false, iState);
       if (iState == 1)
       {
           m_eq->Enable(true);
           iState = 0;
       }
       else
       {
           m_eq->Enable(false);
           iState = 1;
       }
       m_pWindow->ControlIntValue(oControlName, true, iState);
       return kError_NoErr;
   }
   if (oControlName == string("Title") && eMesg == CM_MouseDoubleClick)
   {
       m_pContext->target->AcceptEvent(new 
                            Event(CMD_EditCurrentPlaylistItemInfo));
       return kError_NoErr;
   } 
  
   return kError_NoErr;
}

void FreeAmpTheme::InitControls(void)
{
    bool   bSet, bEnable;
    int    iState;
    string oWelcome(szWelcomeMsg);
    
    assert(m_pWindow);
    
    // Set the volume control
    if (m_iVolume >= 0)
    {  
       bSet = true;
       m_pWindow->ControlIntValue(string("Volume"), true, m_iVolume);
       m_pWindow->ControlIntValue(string("Balance"), true, m_iBalance);
    }
    else 
    {
       int iTemp = 50;

       m_pWindow->ControlIntValue(string("Balance"), true, iTemp);
       bSet = false;   
    }   

    m_pWindow->ControlEnable(string("Volume"), true, bSet);
    m_pWindow->ControlEnable(string("Balance"), true, bSet);

    // Set the seek control
    bSet = (m_iTotalSeconds < 0) ? false : true;
    if (bSet)
       m_pWindow->ControlIntValue(string("Seek"), true, m_iSeekPos);
    m_pWindow->ControlEnable(string("Seek"), true, bSet);
    
    // Set the Play/Pause buttons
    iState = m_bPlayShown ? 0 : 1;
    m_pWindow->ControlIntValue(string("PlayPause"), true, iState);
    m_pWindow->ControlIntValue(string("PlayStop"), true, iState);
    
    iState = m_bPaused ? 1 : 0;
    m_pWindow->ControlIntValue(string("MPause"), true, iState);
    bEnable = m_bPlayShown;
    m_pWindow->ControlEnable(string("Play"), true, bEnable);
    bEnable = !m_bPlayShown;
    m_pWindow->ControlEnable(string("Pause"), true, bEnable);
    m_pWindow->ControlEnable(string("Stop"), true, bEnable);

    // Set the title text field to the current meta data, or if no
    // is available, show the welcome message
    if (m_oTitle.length() == 0)
        m_pWindow->ControlStringValue(string("Title"), true, oWelcome);
    else    
        m_pWindow->ControlStringValue(string("Title"), true, m_oTitle);

    // Ask the playlist manager what the current repeat mode is
    // and set the controls appropriately.    
    switch (m_pContext->plm->GetRepeatMode())
    {
        case kPlaylistMode_RepeatNone:
           iState = 0;
           break;
        case kPlaylistMode_RepeatOne:
           iState = 1;
           break;
        case kPlaylistMode_RepeatAll:
           iState = 2;
           break;
        default:
           break;
    }
    m_pWindow->ControlIntValue(string("Repeat"), true, iState);

    iState = m_pContext->plm->GetShuffleMode() ? 1 : 0;
    m_pWindow->ControlIntValue(string("Shuffle"), true, iState);

    m_pWindow->ControlStringValue("StreamInfo", true, m_oStreamInfo);

    if ((m_pWindow->DoesControlExist("HeadlineInfo") ||
         m_pWindow->DoesControlExist("HeadlineStreamInfo")) && m_pHeadlines)
    {
        if (m_pHeadlineGrabber)
        {
           m_pHeadlineGrabber->SetInfo(*m_pHeadlines);
           m_pHeadlineGrabber->Resume();
        }   
        else
        {
           m_pHeadlineGrabber = new Headlines(m_pContext);   
           m_pHeadlineGrabber->SetInfo(*m_pHeadlines);
        }   
    }
    else
    {
        if (m_pHeadlineGrabber)
           m_pHeadlineGrabber->Pause();
    }

    bEnable = false;
    m_pWindow->ControlEnable(string("StereoIndicator"), true, bEnable);
    m_pWindow->ControlEnable(string("MonoIndicator"), true, bEnable);
    m_pWindow->ControlEnable(string("PlayIndicator"), true, bEnable);
    m_pWindow->ControlEnable(string("PauseIndicator"), true, bEnable);
    bEnable = true;
    m_pWindow->ControlEnable(string("StopIndicator"), true, bEnable);

    iState = m_eq->IsEnabled() == false; 
    m_pWindow->ControlIntValue(string("EqEnable"), true, iState);

    iState = m_iMuteVolume != -1; 
    m_pWindow->ControlIntValue(string("Mute"), true, iState);

    m_eq->InitControls(m_pWindow);
}

// This function gets called after the window object is created,
// but before the window itself gets created
void FreeAmpTheme::InitWindow(void)
{        
	bool   bValue;
    
    m_pContext->prefs->GetPrefBoolean(kStayOnTopPref, &bValue);
    m_pWindow->SetStayOnTop(bValue);

    m_pContext->prefs->GetPrefBoolean(kLiveInTrayPref, &bValue);
    m_pWindow->SetLiveInToolbar(bValue);
}

void FreeAmpTheme::ReloadTheme(void)
{
    char    *szTemp;
    uint32   iLen = _MAX_PATH;
    string   oThemePath(""), oThemeFile("theme.xml");
    Error    eRet;
    struct  _stat buf;

    szTemp = new char[iLen];

    m_pContext->prefs->GetPrefString(kThemePathPref, szTemp, &iLen);
    
    if (_stat(szTemp, &buf) < 0 && strlen(szTemp) < 1)
    {
       // If the theme doesn't exist, let's try to prepend the install/theme dir
       char   *dir;
       uint32  len = _MAX_PATH;

       dir = new char[_MAX_PATH];
    
       m_pContext->prefs->GetPrefString(kInstallDirPref, dir, &len);
       oThemePath = string(dir);
#if defined(unix)
       oThemePath += string(BRANDING_SHARE_PATH);
#endif
       oThemePath += string(DIR_MARKER_STR);    
       oThemePath += string("themes");
       oThemePath += string(DIR_MARKER_STR);    
   
       delete [] dir;
    }
    oThemePath += szTemp;

    iLen = 255;
    m_pContext->prefs->GetPrefString(kThemeDefaultFontPref, szTemp, &iLen);
    SetDefaultFont(string(szTemp));

    eRet = LoadTheme(oThemePath, m_oCurrentWindow);
    if (IsError(eRet))					   
    {
        MessageDialog oBox(m_pContext);
	    string        oErr, oMessage(szParseError);
  
        GetErrorString(oErr);
        oMessage += oErr;
        oBox.Show(oMessage.c_str(), string(BRANDING), kMessageOk);
    }	
    
    delete [] szTemp;
}

void FreeAmpTheme::SetVolume(int iVolume, int iBalance)
{       
    string oVol("Volume: ");
    char   szPercent[40];
    int    iLeft, iRight, iLastBal = m_iBalance;

    if (iBalance < 50) // more focus on left channel
    {
        iLeft = iVolume;
        iRight = iVolume - (iVolume * (50 - iBalance) * 2 / 100);
    }
    else
    {
        iLeft = iVolume - (iVolume * (iBalance - 50) * 2 / 100);
        iRight = iVolume;
    }

    if (m_iMuteVolume != -1 && iVolume != 0)
    {
        int iState = 0;
        m_iMuteVolume = -1;
        m_pWindow->ControlIntValue(string("Mute"), true, iState);
    }
    m_iVolume = iVolume;
    m_iBalance = iBalance;

    m_pContext->target->AcceptEvent(new 
        VolumeEvent(CMD_SetVolume, iLeft, iRight));

    m_pWindow->ControlIntValue(string("Volume"), true, m_iVolume);
    m_pWindow->ControlIntValue(string("Balance"), true, m_iBalance);

    if (m_iBalance == iLastBal)
    {
        sprintf(szPercent, "%d%%", iVolume);
        oVol += string(szPercent);
        m_pWindow->ControlStringValue(string("Info"), true, oVol);
        oVol = string(szPercent);
        m_pWindow->ControlStringValue(string("VolumeText"), true, oVol);
    }
    else
    {
        if (m_iBalance == 50)
           strcpy(szPercent, "Equal balance");
        else
        if (m_iBalance < 50)
           sprintf(szPercent, "%d%% Left", 100 - m_iBalance * 2);
        else
           sprintf(szPercent, "%d%% Right", (m_iBalance - 50) * 2);
        oVol = string("Balance: ") + string(szPercent);
        m_pWindow->ControlStringValue(string("Info"), true, oVol);
    }
}    

void FreeAmpTheme::HandleKeystroke(unsigned char cKey)
{
    switch(cKey)
    {
     case 'p':
     case 'P':
        m_pContext->target->AcceptEvent(new Event(CMD_Play));
        break;

     case 'S':
     case 's':
        m_pContext->target->AcceptEvent(new Event(CMD_Stop));
        break;

     case 'u':
     case 'U':
        m_pContext->target->AcceptEvent(new Event(CMD_Pause));
        break;

     case 'M':
     case 'm':
        m_pContext->target->AcceptEvent(new Event(CMD_ToggleMusicBrowserUI));
        break;

     case 'n':
     case 'N':
        m_pContext->target->AcceptEvent(new Event(CMD_NextMediaPiece));
        break;

     case 'r':
     case 'R':
        m_pContext->target->AcceptEvent(new Event(CMD_PrevMediaPiece));
        break;

     case 'd':
     case 'D':
        m_pContext->target->AcceptEvent(new Event(CMD_ToggleDownloadUI));
        break;
        
     case 'o':
     case 'O':
        m_pContext->target->AcceptEvent(new ShowPreferencesEvent(0));
        break;

     case 'c':
     case 'C':
        ShowThemeCredits();
        break;
 
     case 't':
     case 'T':
        ReloadTheme();
        break;
      
     case 'i':
     case 'I':
     {  
        string oText;

        m_eTitleDisplayState = 
          (m_eTitleDisplayState == kName) ? kNameArtist :
          (m_eTitleDisplayState == kNameArtist) ? kNameArtistAlbum : kName;

        switch(m_eTitleDisplayState)
        {
            case kName: 
              oText = string("Display track name only.");
              break;
            case kNameArtist: 
              oText = string("Display track and artist name.");
              break;
            case kNameArtistAlbum: 
              oText = string("Display track, artist and album name.");
              break;
        }
        m_pWindow->ControlStringValue("Info", true, oText);
              
        break;
     }
     case 'h':
     case 'H':
        ShowHelp();
        break;

     case '@':
     {
        string oText("-23:59:59");
        if (m_pWindow->DoesControlExist("TimeRemaining"))
           m_pWindow->ControlStringValue("TimeRemaining", true, oText);
        else
           m_pWindow->ControlStringValue("Time", true, oText);

        break;
     }    

     case '!':
     {
        m_bShowBuffers = !m_bShowBuffers;
        break;
     }   
   }
}

void FreeAmpTheme::ShowHelp(void)
{
    if (!::ShowHelp(m_pContext, FreeAmp_Main_Window))
    {
        MessageDialog oBox(m_pContext);
        string oMessage("Cannot find the help files. Please make sure that the "
			            "help files are properly installed, and you are not "
						"running "the_BRANDING" from the build directory.");
        oBox.Show(oMessage.c_str(), string(BRANDING), kMessageOk, true);  
    }
}
bool FreeAmpTheme::HandleMenuCommand(uint32 uCommand)
{
    switch(uCommand)
    {
        case kMCMyMusic:
            m_pContext->target->AcceptEvent(
                       new Event(CMD_ToggleMusicBrowserUI));
            return true;           

        case kMCPlay:
            m_pContext->target->AcceptEvent(
                       new Event(CMD_Play));
            return true;           

        case kMCStop:
            m_pContext->target->AcceptEvent(
                       new Event(CMD_Stop));
            return true;           

        case kMCPause:
            m_pContext->target->AcceptEvent(
                       new Event(CMD_Pause));
            return true;           

        case kMCNext:
            m_pContext->target->AcceptEvent(
                       new Event(CMD_NextMediaPiece));
            return true;           

        case kMCPrev:
            m_pContext->target->AcceptEvent(
                       new Event(CMD_PrevMediaPiece));
            return true;           

        case kMCExit:
            m_pContext->target->AcceptEvent(
                       new Event(CMD_QuitPlayer));
            return true;           
    }
    
    return false;
}

void FreeAmpTheme::HandleMouseWheelChange(int iSteps)
{
	string oControlName("Volume");
	int    iVol;
	bool   bEnabled;

    m_pWindow->ControlEnable(oControlName, false, bEnabled);
	if (bEnabled)
	{
       m_pWindow->ControlIntValue(oControlName, false, iVol);
       iVol += iSteps * 5;
	   iVol = max(min(iVol, 100), 0);
       SetVolume(iVol, m_iBalance);
	}
}

void FreeAmpTheme::HandleMouseWheelClick(void)
{
    int    iState;
	string oControlName("Mute");

    if (m_iMuteVolume < 0)
    {
        m_iMuteVolume = m_iVolume;
        m_iVolume = 0;
        iState = 1;
    }
    else
    {
        m_iVolume = m_iMuteVolume;
        m_iMuteVolume = -1;
        iState = 0;
    }
    SetVolume(m_iVolume, m_iBalance);
    m_pWindow->ControlIntValue(oControlName, true, iState);
}

void FreeAmpTheme::VolumeChanged(void)
{
    if (!m_bVolumeChangeInProgress)
       m_pContext->target->AcceptEvent(new Event(CMD_GetVolume));
}

void FreeAmpTheme::UpdateTimeDisplay(int iCurrentSeconds)
{
    string oText;
    char   szText[20];
    int    iSeconds;
    
    if (m_eTimeDisplayState == kTimeRemaining && m_iTotalSeconds >= 0)
    {    
        iSeconds = m_iTotalSeconds - iCurrentSeconds - 1;
        if (iSeconds > 3600)
            sprintf(szText, "-%d:%02d:%02d", 
                    iSeconds / 3600,
                    (iSeconds % 3600) / 60,
                    iSeconds % 60);
        else                
        if (iSeconds <= 0)
            sprintf(szText, "0:00");
        else
            sprintf(szText, "-%d:%02d", 
                    (iSeconds % 3600) / 60,
                    iSeconds % 60);
    }    
    else    
    if (iCurrentSeconds >= 0)
    {
        if (iCurrentSeconds > 3600)
           sprintf(szText, "%d:%02d:%02d", 
                   iCurrentSeconds / 3600,
                   (iCurrentSeconds % 3600) / 60,
                   iCurrentSeconds % 60);
        else           
           sprintf(szText, "%d:%02d", 
                   (iCurrentSeconds % 3600) / 60,
                   iCurrentSeconds % 60);
    }
    else    
        sprintf(szText, "0:00");

    oText = string(szText);
    if (m_eTimeDisplayState == kTimeRemaining && 
        m_pWindow->DoesControlExist("TimeRemaining") &&
        m_iTotalSeconds >= 0)
        m_pWindow->ControlStringValue("TimeRemaining", true, oText);
    else
        m_pWindow->ControlStringValue("Time", true, oText);
}

void FreeAmpTheme::UpdateMetaData(const PlaylistItem *pItem)
{
    bool bEnable;

    if (pItem->GetMetaData().Title().length() > 0 || 
        pItem->GetMetaData().Artist().length() > 0 ||
        pItem->GetMetaData().Album().length() > 0)
    {
        string oText;
        
        m_oTitle = pItem->GetMetaData().Title();
        if (pItem->GetMetaData().Artist().length() > 0 && 
            (m_eTitleDisplayState == kNameArtist ||
             m_eTitleDisplayState == kNameArtistAlbum))
           m_oTitle += string(" - ") + pItem->GetMetaData().Artist();

        if (pItem->GetMetaData().Album().length() > 0 && 
             m_eTitleDisplayState == kNameArtistAlbum)
           m_oTitle += string(" - ") + pItem->GetMetaData().Album() + 
                       string(" - ");;

        oText = string(BRANDING": ") + m_oTitle;
        m_pWindow->SetTitle(oText);
    }    
    else
        m_oTitle = "";
      
    if (pItem->GetMetaData().Title().length() > 0)
    {
        m_oTrackName = pItem->GetMetaData().Title();
    }
    else
        m_oTrackName = "Unknown";

    if (pItem->GetMetaData().Artist().length() > 0)
    {
        m_oArtist = pItem->GetMetaData().Artist();
    }
    else
        m_oArtist = "Unknown";

    if (pItem->GetMetaData().Album().length() > 0)
    {
        m_oAlbum = pItem->GetMetaData().Album();
    }
    else
        m_oAlbum = "Unknown";

    if (pItem->GetMetaData().Year() > 0)
    {
        char   szText[20];
        sprintf(szText,"%d",pItem->GetMetaData().Year());
        m_oYear = string(szText);
    }
    else
        m_oYear = "";

    if (pItem->GetMetaData().Track() > 0)
    {
        char   szText[20];
        sprintf(szText,"%d",pItem->GetMetaData().Track());
        m_oTrackNo = string(szText);
    }
    else
        m_oTrackNo = "";

    if (pItem->GetMetaData().Genre().length() > 0)
    {
        m_oGenre = pItem->GetMetaData().Genre();
    }
    else
        m_oGenre = "";

    if (pItem->GetMetaData().Comment().length() > 0)
    {
        m_oComment = pItem->GetMetaData().Comment();
    }
    else
        m_oComment = "";

    int dummy;
    if (strncmp(pItem->URL().c_str(), "http://", 7) == 0 ||
        strncmp(pItem->URL().c_str(), "rtp://", 7) == 0 ||
        sscanf(pItem->URL().c_str(), "file://%d.cda", &dummy) == 1)
    {
        bEnable = false;
        m_oFileName.erase();
    }
    else
    {
        bEnable = true;
        m_oFileName = pItem->URL();
    }
    m_pWindow->ControlEnable(string("BitziLookup"), true, bEnable);
    m_pWindow->ControlStringValue(string("Title"), true, m_oTitle);
    m_pWindow->ControlStringValue(string("TrackName"), true, m_oTrackName);
    m_pWindow->ControlStringValue(string("TrackNo"), true, m_oTrackNo);
    m_pWindow->ControlStringValue(string("Artist"), true, m_oArtist);
    m_pWindow->ControlStringValue(string("Album"), true, m_oAlbum);
    m_pWindow->ControlStringValue(string("Year"), true, m_oYear);
    m_pWindow->ControlStringValue(string("Genre"), true, m_oGenre);
    m_pWindow->ControlStringValue(string("Comment"), true, m_oComment);
}

void FreeAmpTheme::DropFiles(vector<string> *pFileList)
{
    char                    *ext;
    char                    *url;
    uint32                   length, countbefore;
    vector<string>::iterator i;
    bool                     bPlay;
    bool                     bEmptied = false;
    
    ext = new char[_MAX_PATH];
    url = new char[_MAX_PATH + 7];

    m_pContext->prefs->GetPrefBoolean(kPlayImmediatelyPref, &bPlay);
    
    countbefore = m_pContext->plm->CountItems();

    for(i = pFileList->begin(); i != pFileList->end(); i++)
    {
        char          *pExtension = NULL;
        vector<char*>  fileList;
        struct _stat   st;

        _stat((*i).c_str(), &st);
        if(st.st_mode & _S_IFDIR)
        {
            vector<string> oList, oQuery;

            oQuery.push_back(string("*.mp1"));
            oQuery.push_back(string("*.mp2"));
            oQuery.push_back(string("*.mp3")); 

            FindMusicFiles(i->c_str(), oList, oQuery); 
            if (oList.size() > 0)
            {
                if(bPlay && !bEmptied)
                {
                    m_pContext->target->AcceptEvent(new Event(CMD_Stop));
                    m_pContext->plm->RemoveAll();
                    bEmptied = true;
                }

                m_pContext->plm->AddItems(oList);
            }
        }
        else
        {
            PlaylistFormatInfo     oInfo;              
            char                   ext[_MAX_PATH];
            int                    j;
            Error                  eRet = kError_NoErr;
            
            pExtension = strrchr((*i).c_str(), '.');
            if (!pExtension)
                continue;

            strcpy(ext, pExtension + 1);
            ToUpper(ext);

            for(j = 0; ; j++)
            {
                eRet = m_pContext->plm->GetSupportedPlaylistFormats(&oInfo, j);
                if (IsError(eRet))
                    break;

                if (strcasecmp(oInfo.GetExtension(), ext) == 0)
                    break;   
            }

            if (!IsError(eRet))
            {
                length = _MAX_PATH + 7;
                FilePathToURL((*i).c_str(), url, &length);
                
                if (bPlay && !bEmptied)
                {
                    m_pContext->target->AcceptEvent(new Event(CMD_Stop));
                    m_pContext->plm->RemoveAll();
                    bEmptied = true;
                }

                m_pContext->plm->ReadPlaylist(url);
            }   
            else if (m_pContext->player->IsSupportedExtension(ext))
            {
                length = _MAX_PATH + 7;
                FilePathToURL((*i).c_str(), url, &length);
            
                if (bPlay && !bEmptied)
                {
                    m_pContext->target->AcceptEvent(new Event(CMD_Stop));
                    m_pContext->plm->RemoveAll();
                    bEmptied = true;
                }

                m_pContext->plm->AddItem(url);
                
            }
            else if (!strcasecmp("fat", ext) ||
                     !strcasecmp("zip", ext)) 
            {
                length = _MAX_PATH + 7;
                FilePathToURL((*i).c_str(), url, &length);

                m_pContext->player->AddTheme(url); 
            }   
        }
    }
    
    if (countbefore == 0 || bPlay)
        m_pContext->target->AcceptEvent(new Event(CMD_Play));
        
    delete [] ext;
    delete [] url;    
}

void FreeAmpTheme::PostWindowCreate(void)
{
#ifdef WIN32
    Int32PropValue *pProp;
    
    pProp = new Int32PropValue((int)((Win32Window *)m_pWindow)->GetWindowHandle());
    m_pContext->props->SetProperty("MainWindow", pProp);
#endif
    string winTitle = string(BRANDING);
    m_pWindow->SetTitle(winTitle);

}

#if 0
void FreeAmpTheme::update_thread(void* arg)
{
    FreeAmpTheme* _this = (FreeAmpTheme*)arg;

    _this->UpdateThread();
}

void FreeAmpTheme::UpdateThread()
{
#ifdef WIN32

    m_pUpdateMan->RetrieveLatestVersionInfo();

    if(m_pUpdateMan->IsUpdateAvailable())
    {
        if(0 < DialogBoxParam(g_hinst, 
                              MAKEINTRESOURCE(IDD_UPDATEAVAILABLE),
                              NULL, 
                              ::UpdateAvailableDlgProc, 
                              (LPARAM) 0))
        {
            ShowOptions(4);
        }
    }

#endif

    delete m_pUpdateThread;
}
#endif

void FreeAmpTheme::ShowOptions(uint32 defaultPage)
{
    OptionsArgs *oArgs = new OptionsArgs;
    
    if (m_bInOptions)
       return;
    m_bInOptions = true;
       
    if (m_pOptionsThread)
    {  
        delete m_pOptionsThread;
        m_pOptionsThread = NULL;
    }

    oArgs->pThis = this;
    oArgs->uDefaultPage = defaultPage;

    m_pOptionsThread = Thread::CreateThread();
    m_pOptionsThread->Create(options_thread, oArgs, true);
}

void FreeAmpTheme::options_thread(void* arg)
{
    OptionsArgs *pArgs = (OptionsArgs *)arg;
  
    pArgs->pThis->OptionsThread(pArgs->uDefaultPage);

    delete pArgs;
}

void FreeAmpTheme::OptionsThread(uint32 defaultPage)
{
    PreferenceWindow *pWindow;


#ifdef WIN32
//    pWindow = new Win32PreferenceWindow(m_pContext, m_pThemeMan, m_pUpdateMan, defaultPage);
    pWindow = new Win32PreferenceWindow(m_pContext, m_pThemeMan, defaultPage);
#elif defined(__BEOS__)
    pWindow = new BeOSPreferenceWindow(m_pContext, m_pThemeMan);
#else
    pWindow = new GTKPreferenceWindow(m_pContext, m_pThemeMan, defaultPage); 
#endif

    pWindow->Show(m_pWindow);

    delete pWindow;

    m_bInOptions = false;
}
 
