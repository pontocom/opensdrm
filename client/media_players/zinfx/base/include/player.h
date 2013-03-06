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
        
        $Id: player.h,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PLAYER_H_
#define INCLUDED_PLAYER_H_

#include <vector>
#include <map>
using namespace std;

#include "config.h"
#include "thread.h"
#include "event.h"
#include "queue.h"
#include "ui.h"
#include "mutex.h"
#include "playlist.h"
#include "semaphore.h"
#include "registry.h"
#include "preferences.h"
#include "properties.h"
#include "propimpl.h"
#include "musiccatalog.h"
//#include "hashtable.h"
#include "downloadmanager.h"
#include "timer.h"
#include "lmc.h"

typedef enum
{
   PlayerState_Paused = 1,
   PlayerState_Playing,
   PlayerState_Stopped
}
PlayerState;

class FAContext;

class Player : public EventQueue, Properties, PropertyWatcher
{

 public:
    static Player *GetPlayer(FAContext *context);
    virtual  ~Player();

    int32     RegisterActiveUI(UserInterface * ui);
    int32     RegisterLMCs(Registry * registry);
    int32     RegisterPMIs(Registry * registry);
    int32     RegisterPMOs(Registry * registry);
    int32     RegisterUIs(Registry * registry);

    Registry* GetLMCRegistry() const;
    Registry* GetPMIRegistry() const;
    Registry* GetPMORegistry() const;
    Registry*  GetUIRegistry() const;

    void      Run();
    bool      SetArgs(int32 argc, char **argv);
    void      HandleSingleArg(char *arg);
    void      AddTheme(char *url);
    void      SetTerminationSemaphore(Semaphore *);
    void      testQueue();
    static void EventServiceThreadFunc(void *);
    virtual Error AcceptEvent(Event *);
    virtual RegistryItem *ChooseLMC(const char *szUrl);
    virtual RegistryItem *ChoosePMI(const char *szUrl, char *szTitle = NULL);

    bool    IsSupportedExtension(const char *ext);
    char *GetExtension(const char *title);
 
    bool  IsSupportedProtocol(const char *proto);
    char *GetProtocol(const char *title);
    
    // Properties
    virtual Error GetProperty(const char *, PropValue **);
    virtual Error SetProperty(const char *, PropValue *);
    virtual Error RegisterPropertyWatcher(const char *, PropertyWatcher *);
    virtual Error RemovePropertyWatcher(const char *, PropertyWatcher *);

    virtual Error PropertyChange(const char *, PropValue *);

    PlayerState  State() const { return m_playerState; }

    void UpdateCDNow();
    
 protected:
              Player(FAContext *context);
    void      GetUIManipLock();
    void      ReleaseUIManipLock();
    int32     CompareNames(const char *, const char *);
    void      SendToUI(Event *);
    void      Usage(const char *);

    bool         SetState(PlayerState);

    int32     ServiceEvent(Event *);
    void      CreatePMO(const PlaylistItem * pc, Event * pC);

    static void cd_timer(void* arg);
    void        CDTimer();

    FAContext *m_context;

 private:
    // These are event loop handling functions
    void DoneOutputting(Event *pEvent) ;
    void Stop(Event *pEvent);
    void GetVolume(Event *pEvent);
    void SetVolume(Event *pEvent);
    void ChangePosition(Event *pEvent);
    void GetMediaInfo(Event *pEvent) ;
    void Play(Event *pEvent);
    void Next(Event *pEvent);
    void Previous(Event *pEvent);
    void Pause(Event *pEvent);
    void UnPause(Event *pEvent);
    void TogglePause(Event *pEvent);
    int  Quit(Event *pEvent);
    int  ReadyToDieUI(Event *pEvent);
    void HandleMediaInfo(Event *pEvent);
    void HandleMediaTimeInfo(Event *pEvent);
    void SendEventToUI(Event *pEvent);
    void GetMediaTitle(Event *pEvent);
    void ToggleUI(Event *pEvent);
    void HandleQueryState();
    void SendEventToCatalog(Event *pEvent);   
    void HandlePrefsChanged(Event *pEvent);
    
    #define _EQUALIZER_ENABLE_
    #ifdef  _EQUALIZER_ENABLE_
    void Player::SetEQData(Event *pEvent);
    #endif // _EQUALIZER_ENABLE_
    #undef  _EQUALIZER_ENABLE_    

    #define _VISUAL_ENABLE_
    #ifdef  _VISUAL_ENABLE_
    void Player::SendVisBuf(Event *pEvent); 
    #endif // _VISUAL_ENABLE_
    #undef  _VISUAL_ENABLE_  

    PropertiesImpl m_props;
    bool      m_didUsage;
    bool      m_autoplay, m_bTrackChanging;
    Semaphore *m_pTermSem;
    static Player *m_thePlayer;
    Semaphore *m_eventSem;
    PlayerState m_playerState;
    Queue < Event * >*m_eventQueue;
    Thread   *m_eventServiceThread;
    int32     m_quitWaitingFor;  // keeps track of how many CIO's 
    // and COO's haven't sent in 
    // their "Ready To Die" infos.

    Thread   *m_signatureThread;
    
    int32     m_imQuitting;
    vector < UserInterface * >*m_uiList;

    Mutex    *m_uiManipLock;
    Mutex    *m_lmcMutex;
    Mutex    *m_pmiMutex;
    Mutex    *m_pmoMutex;
    Mutex    *m_uiMutex;
    Mutex    *m_sigStopMutex;

    Semaphore *m_signatureSem;

    PlaylistManager *m_plm;

    PhysicalMediaOutput *m_sigspmo;
 
    PhysicalMediaOutput *m_pmo;
    LogicalMediaConverter *m_lmc;
    UserInterface *m_ui;

    Registry *m_lmcRegistry;
    Registry *m_pmiRegistry;
    Registry *m_pmoRegistry;
    Registry *m_uiRegistry;

    vector < char *>* m_argUIList;

    int32     m_argc;
    char    **m_argv;

    map<string, RegistryItem *> *m_lmcExtensions;
    map<string, RegistryItem *> *m_pmiProtocols;
    
    MusicCatalog *m_musicCatalog;
    UserInterface *m_browserUI;

    DownloadManager *m_dlm;
    UserInterface *m_downloadUI;

    TimerRef m_cdTimer;
    bool     m_cdTimerActive;
    float    m_eqValues[32], m_eqPreamp;
    bool     m_eqEnabled;
};

#endif // _PLAYER_H_
