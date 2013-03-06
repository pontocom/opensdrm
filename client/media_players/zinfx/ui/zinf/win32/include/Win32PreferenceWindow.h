/*____________________________________________________________________________

   FreeAmp - The Free MP3 Player

   Copyright (C) 1999 EMusic

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

   $Id: Win32PreferenceWindow.h,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_WIN32PREFERENCEWINDOW_H__
#define INCLUDED_WIN32PREFERENCEWINDOW_H__

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <map>
#include <set>

using namespace std;

#include "config.h"
#include "PreferenceWindow.h"
//#include "win32updatemanager.h"
#include "preferences.h"
#include "log.h"
#include "registrar.h"
#include "resource.h"

typedef struct PrefsStruct 
{
    // page 1
    string defaultUI;
    string defaultPMO;
    int32 inputBufferSize;
    int32 outputBufferSize;
    int32 preBufferLength;
    int32 decoderThreadPriority;
    bool stayOnTop;
    bool liveInTray;
    bool useTextLabels;
    bool useImages;
    bool savePlaylistOnExit;
    bool playImmediately;
    bool reclaimFiletypes;
    bool askReclaimFiletypes;
    bool convertUnderscores;

    // page 2
    bool saveStreams;
    string saveStreamsDirectory;
    bool useProxyServer;
    string proxyServer;
    bool useAlternateIP;
    string alternateIP;

    // page 3
    string saveMusicDirectory;
    bool watchForNewMusic;
    set<string> watchDirectories;
    
    // page 4
    bool enableLogging;
    bool logMain;
    bool logInput;
    bool logOutput;
    bool logDecoder;
    bool logPerformance;

    //page 5

	// page 6
    string defaultFont;
    string currentTheme;

	// page 8
	bool updateCDAutomatically;
    string MBServerURL;

    // Browser page
    string playlistHeaderColumns;
    
    bool operator == (const struct PrefsStruct& pref)
    {
        return (
            playlistHeaderColumns == pref.playlistHeaderColumns &&
            defaultUI == pref.defaultUI &&
            defaultPMO == pref.defaultPMO &&
            inputBufferSize == pref.inputBufferSize &&
            outputBufferSize == pref.outputBufferSize &&
            preBufferLength == pref.preBufferLength &&
            decoderThreadPriority == pref.decoderThreadPriority &&
            stayOnTop == pref.stayOnTop &&
            liveInTray == pref.liveInTray &&
            saveStreams == pref.saveStreams &&
            saveStreamsDirectory == pref.saveStreamsDirectory &&
            useProxyServer == pref.useProxyServer &&
            proxyServer == pref.proxyServer &&
            useAlternateIP == pref.useAlternateIP &&
            alternateIP == pref.alternateIP &&
            enableLogging == pref.enableLogging &&
            logMain == pref.logMain &&
            logInput == pref.logInput &&
            logOutput == pref.logOutput &&
            logDecoder == pref.logDecoder &&
            logPerformance == pref.logPerformance &&
            
            defaultFont == pref.defaultFont &&
            currentTheme == pref.currentTheme &&

            reclaimFiletypes == pref.reclaimFiletypes &&
            askReclaimFiletypes == pref.askReclaimFiletypes &&
            saveMusicDirectory == pref.saveMusicDirectory &&

            useTextLabels == pref.useTextLabels &&
            useImages == pref.useImages &&

            savePlaylistOnExit == pref.savePlaylistOnExit &&
            playImmediately == pref.playImmediately &&
            convertUnderscores == pref.convertUnderscores &&

            watchForNewMusic == pref.watchForNewMusic &&
            watchDirectories == pref.watchDirectories &&

			updateCDAutomatically == pref.updateCDAutomatically &&
            MBServerURL == pref.MBServerURL &&
            true
        );
    }

    bool operator != (const struct PrefsStruct& pref)
    {
        return ! (*this == pref);
    }

} PrefsStruct;

typedef struct PrefPage {    
	HINSTANCE hInstance;
	LPCSTR pszTemplate;
	DLGPROC pfnDlgProc;
	LPARAM lParam;
	bool bChild;
    
    HWND hwnd;
} PrefPage;


class Win32PreferenceWindow : public PreferenceWindow
{
    public:

               Win32PreferenceWindow(FAContext *context,
                                     ThemeManager *pThemeMan,
                                     uint32 defaultPage = 0);
      virtual ~Win32PreferenceWindow(void); 
      

      virtual  bool Show(Window *pParent);

               void GetPrefsValues(PrefsStruct* values);

               void SavePrefsValues(PrefsStruct* values);

               bool MainProc( HWND hwnd, 
                              UINT msg, 
                              WPARAM wParam, 
                              LPARAM lParam);

               bool PrefGeneralProc(HWND hwnd, 
                                  UINT msg, 
                                  WPARAM wParam, 
                                  LPARAM lParam);
               bool PrefThemeProc(HWND hwnd, 
                                  UINT msg, 
                                  WPARAM wParam, 
                                  LPARAM lParam);
               bool PrefStreamingProc(HWND hwnd, 
                                  UINT msg, 
                                  WPARAM wParam, 
                                  LPARAM lParam);
               bool PrefPluginsProc(HWND hwnd, 
                                  UINT msg, 
                                  WPARAM wParam, 
                                  LPARAM lParam);
               bool PrefBrowserProc(HWND hwnd,
                                  UINT msg,
                                  WPARAM wParam,
                                  LPARAM lParam);
               bool PrefAdvancedProc(HWND hwnd, 
                                  UINT msg, 
                                  WPARAM wParam, 
                                  LPARAM lParam);
               bool PrefAboutProc(HWND hwnd, 
                                  UINT msg, 
                                  WPARAM wParam, 
                                  LPARAM lParam);
               bool PrefDirectoryProc(HWND hwnd, 
                                  UINT msg, 
                                  WPARAM wParam, 
                                  LPARAM lParam);
               bool PrefDebugProc(HWND hwnd, 
                                  UINT msg, 
                                  WPARAM wParam, 
                                  LPARAM lParam);
               bool PrefCDAudioProc(HWND hwnd,
                                  UINT msg,
                                  WPARAM wParam,
                                  LPARAM lParam);


	protected:
    
               bool DisplayPreferences(HWND hwndParent);
               void LoadThemeListBox(HWND hwnd);
               void InitializePrefDialog();
               void ShowPrefPage(PrefPage* page, bool show);


      PrefsStruct  m_originalValues;
      PrefsStruct  m_currentValues;
      PrefsStruct  m_proposedValues;
      uint32       m_defaultPage;

      Preferences*   m_prefs;      

      map<string, string> m_oThemeList;

      vector<PrefPage> m_pages;
      HWND m_hwndPref;
      string m_caption;
      uint32 m_startPage;
      PrefPage* m_currentPage;
      
};

#endif
