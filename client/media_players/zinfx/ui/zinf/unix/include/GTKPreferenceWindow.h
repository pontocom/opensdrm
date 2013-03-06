/*____________________________________________________________________________

   FreeAmp - The Free MP3 Player

   Copyright (C) 1999-2000 EMusic

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

   $Id: GTKPreferenceWindow.h,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_GTKPREFERENCEWINDOW_H__
#define INCLUDED_GTKPREFERENCEWINDOW_H__

#include <map>
#include <set>
using namespace std;

#include <gtk/gtk.h>

#include "config.h"
#include "PreferenceWindow.h"
#include "ThemeManager.h"
#include "preferences.h"
#include "log.h"
#include "registrar.h"

typedef set<string> PortableSet;

typedef struct PrefsStruct 
{
    Preferences* prefs;

    // page 1
    string defaultPMO;
    int32 inputBufferSize;
    int32 outputBufferSize;
    int32 preBufferLength;
    bool  reclaimFiletypes;
    bool  askReclaimFiletypes;

    // page 2
    bool saveStreams;
    string saveStreamsDirectory;
    bool useProxyServer;
    string proxyServer; 
    bool useAlternateIP;
    string alternateIP;
    
    // page 3
    bool enableLogging;
    bool logMain;
    bool logInput;
    bool logOutput;
    bool logDecoder;
    bool logPerformance;
    bool convertUnderscores;

    // page 5
    string defaultFont;
    string currentTheme;
    int    listboxIndex;

    bool useTextLabels;
    bool useImages; 
    bool savePlaylistOnExit;
    bool playImmediately;
    bool setLastResume;
    bool allowMultipleInstances;
 
    string saveMusicDirectory;
    PortableSet portablePlayers;

    string alsaOutput;

    string watchThisDirectory;
    int32  watchThisDirTimeout;

    bool pollCD;
    string CDDevicePath;
    string MBServer;

    string playlistHeaders;
    
    bool operator == (const struct PrefsStruct& pref)
    {
        return (
            playlistHeaders == pref.playlistHeaders &&
            CDDevicePath == pref.CDDevicePath &&
            MBServer == pref.MBServer &&
            pollCD == pref.pollCD &&
            watchThisDirTimeout == pref.watchThisDirTimeout &&
            watchThisDirectory == pref.watchThisDirectory &&
            convertUnderscores == pref.convertUnderscores &&
            allowMultipleInstances == pref.allowMultipleInstances &&
            playImmediately == pref.playImmediately &&
	    setLastResume == pref.setLastResume &&
            useTextLabels == pref.useTextLabels &&
            useImages == pref.useImages &&
            savePlaylistOnExit == pref.savePlaylistOnExit &&
            defaultPMO == pref.defaultPMO &&
            inputBufferSize == pref.inputBufferSize &&
            outputBufferSize == pref.outputBufferSize &&
            preBufferLength == pref.preBufferLength &&
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
            saveMusicDirectory == pref.saveMusicDirectory &&
            portablePlayers == pref.portablePlayers &&
            listboxIndex == pref.listboxIndex &&
            alsaOutput == pref.alsaOutput &&
            reclaimFiletypes == pref.reclaimFiletypes &&
            askReclaimFiletypes == pref.askReclaimFiletypes &&
            true
        );
    }

    bool operator != (const struct PrefsStruct& pref)
    {
        return ! (*this == pref);
    }
} PrefsStruct;

class OptionsPane {
  public:
    OptionsPane(string label, string desc, uint32 page, GtkWidget *pane)
    {
        m_label = label; m_description = desc; m_pageno = page; 
        m_pane = pane;
    }
    ~OptionsPane() { }

    string     m_label;
    string     m_description;
    uint32     m_pageno;
    GtkWidget *m_pane;
};

class GTKPreferenceWindow : public PreferenceWindow
{
  public:

               GTKPreferenceWindow(FAContext *context,
                                   ThemeManager *pThemeMan,
                                   uint32 defaultPage);
      virtual ~GTKPreferenceWindow(void); 
      
      virtual  bool Show(Window *pParent);

               void GetPrefsValues(Preferences* prefs, 
                                   PrefsStruct* values);

               void SavePrefsValues(Preferences* prefs, 
                                    PrefsStruct* values);

  protected:

      PrefsStruct  currentValues;
      PrefsStruct  originalValues;
      PrefsStruct  proposedValues;

      map<string, string> m_oThemeList;

  private:
      bool   eventLoop;
      uint32 startPage;

      GtkWidget *CreateGeneral(void);
      GtkWidget *CreateStreaming(void);
      GtkWidget *CreatePlugins(void);
      GtkWidget *CreateAbout(void);
      GtkWidget *CreateThemes(void);
      GtkWidget *CreateAdvanced(void);
      GtkWidget *CreateDirectories(void);
      GtkWidget *CreateCD(void);
      GtkWidget *CreatePlaylistHeaders(void);
      
      GtkWidget *saveMusicBox;

      GtkWidget *applyButton;

      GtkWidget *saveStreamLabel;
      GtkWidget *saveStreamBox;
      GtkWidget *saveBrowseBox;

      GtkWidget *proxyAddyLabel;
      GtkWidget *proxyAddyBox;
      GtkWidget *proxyColon;
      GtkWidget *proxyPortLabel;
      GtkWidget *proxyPortBox;

      GtkWidget *ipLabel;
      GtkWidget *ipOneBox;
      GtkWidget *ipPeriod1;
      GtkWidget *ipTwoBox;
      GtkWidget *ipPeriod2;
      GtkWidget *ipThreeBox;
      GtkWidget *ipPeriod3;
      GtkWidget *ipFourBox;

      GtkWidget *pmoMenu;
      GtkWidget *alsaOneBox;
      GtkWidget *alsaTwoBox;

      GtkWidget *themeList;

      GtkWidget *fontDialog;

      GtkWidget *paneLabel;
      GtkStyle  *paneStyle;

      GtkWidget *paneVbox;
      GtkWidget *visiblePane;
  
      GtkWidget *watchDirBox;

      GtkCTree *prefTree;

      GtkWidget *profileEnable;
      GtkWidget *profileListFrame;
      GtkWidget *profileTextFrame;
      GtkWidget *profileList;
      GtkWidget *profileEntry;
      GtkWidget *profileAdd;
      GtkWidget *profileDelete;
      
      bool enableProfiles;
     
      GtkWidget *pollCD;
      GtkWidget *cdPath;
      GtkWidget *mbServer;
      GtkWidget *enableMB;
      GtkWidget *enableBitzi;
      
      vector<OptionsPane *> *paneList;
      void AddPane(OptionsPane *pane);      

      bool firsttime;

      void ApplyProfiles(void);
      vector<string> *m_PMOnames;
      set<uint32>     m_profileSelection;

      GtkWidget *plAvailableList;
      GtkWidget *plShownList;

  public:
      void SetPane(OptionsPane *pane);
      void SetPane(uint32 panenum);

      GtkWidget *mainWindow;
      GtkWidget *pmoOptionMenu;

      bool  done;

      void UpdateThemeList(void);
      void AddThemeEvent(const char *newpath);
      void DeleteThemeEvent(void);
      void ChooseFont(void);
      void SetFont(void);

      void ApplyInfo(void);
      void CancelInfo(void);
      
      int  SetInputBufferSize(int newvalue);
      void SetOutputBufferSize(int newvalue);
      void SetPreBufferLength(int newvalue);

      void SaveLocalToggle(int active);
      void SaveLocalSet(char *newpath, bool set);
      void ProxyAddySet(void);
      void ProxyToggle(int active);
      void AltIPSet(void);
      void AltIPToggle(int active);

      void SelectTheme(int number);

      void SetPMO(int newsel);
      void AlsaSet(void);

      void ShowHelp(void);

      void SaveMusicSet(char *newpath, bool set);
      void SetToolbar(bool text, bool pics);
      void SaveOnExitToggle(int active);
      void PlayImmediatelyToggle(int active);
      void SetLastResumeToggle(int active);
      void AllowMultipleToggle(int active);
      void ConvertUnderscoresToggle(int active);
      void ReclaimTypesToggle(int active);
      void AskReclaimToggle(int active);

      void WatchDirSet(char *newpath, bool set);
      void SetWatchTimeout(int32 timeout);

      void PollCDToggle(int active);
      void CDPathSet(char *newpath, bool set);
      void MBServerSet(char *newpath, bool set);

      void UpdatePLHeaders(void);     
      void AddPLSelection(void);
      void RemovePLSelection(void);

      FAContext *GetContext(void) { return m_pContext; }
};

#endif
