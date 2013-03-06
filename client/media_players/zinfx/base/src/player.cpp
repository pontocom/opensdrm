/*____________________________________________________________________________
        
        FreeAmp - The Free MP3 Player

        Portions Copyright (C) 1998-2000 EMusic.com

        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 2 of the License, or
        (at your option) any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this program; if not, Write to the Free Software
        Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
        
        $Id: player.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h> 
#ifdef WIN32
#include <direct.h>
#define MKDIR(z) mkdir(z)
#else
#define MKDIR(z) mkdir(z, 0755)
#endif

#ifdef __QNX__
#include <strings.h>
#endif

#include <set>

#include "config.h"
#include "event.h"
#include "lmc.h"
#include "player.h"
#include "thread.h"
#include "debug.h"
#include "ui.h"
#include "queue.h"
#include "semaphore.h"
#include "eventdata.h"
#include "registrar.h"
#include "preferences.h"
#include "properties.h"
#include "facontext.h"
#include "log.h"
#include "pmo.h"
#include "utility.h"
#include "downloadmanager.h"
#include "timer.h"

#define DB Debug_v("%s:%d\n", __FILE__, __LINE__);

Player   *Player::m_thePlayer = NULL;

const char *szPlaylistExt = ".M3U";
const char *themeExtension = "fat";

#define SEND_NORMAL_EVENT(e) { Event *ev = new Event(e); GetUIManipLock();    \
                               SendToUI(ev); ReleaseUIManipLock(); delete ev; \
                             }

Player *
Player::
GetPlayer(FAContext *context)
{
    if (m_thePlayer == NULL)
        m_thePlayer = new Player(context);
    return m_thePlayer;
}

Player::
Player(FAContext *context) : EventQueue()
{
    m_context = context;
    m_context->player = this;

    bool useProxy = false;
    m_context->prefs->GetPrefBoolean(kUseProxyPref, &useProxy);
    if (useProxy)
    {
       uint32 size = 1024;
       char *buffer = (char *)malloc(size);

       if (kError_BufferTooSmall == m_context->prefs->GetPrefString(
                                                 kProxyHostPref, buffer, &size))
       {
           buffer = (char *)realloc(buffer, size);
           m_context->prefs->GetPrefString(kProxyHostPref, buffer, &size);
       }
   
       char *port = strrchr(buffer, ':');
       if (port) {
           *port = '\0';
           port++;
       }

       string proxyAddr = string(buffer, strlen(buffer) + 1);
       int nPort = 80;
       if (port && *port)
           nPort = atoi(port);

       free(buffer);
    }

    // cout << "Creating player..." << endl;
    m_eventSem = new Semaphore();
    m_eventQueue = new Queue < Event * >();
    // cout << "Created queue" << endl;
    m_eventServiceThread = NULL;
    // cout << "Started event thread" << endl;
    m_uiList = new vector < UserInterface * >;
    // cout << "Created Lists" << endl;
    m_uiManipLock = new Mutex();
    m_lmcMutex = new Mutex();
    m_pmiMutex = new Mutex();
    m_pmoMutex = new Mutex();
    m_uiMutex = new Mutex();
    m_sigStopMutex = new Mutex();
    // cout << "Created mutex" << endl;
    m_imQuitting = 0;
    m_quitWaitingFor = 0;
    m_plm = new PlaylistManager(m_context);
    m_playerState = PlayerState_Stopped;

    m_lmcRegistry = NULL;
    m_pmiRegistry = NULL;
    m_pmoRegistry = NULL;
    m_uiRegistry = NULL;

    m_lmcExtensions = NULL;
    m_pmiProtocols = NULL;

    m_browserUI = NULL;
    m_downloadUI = NULL;

    m_sigspmo = NULL;
    m_pmo = NULL;
    m_lmc = NULL;
    m_ui = NULL;

    m_argUIList = new vector < char *>();

    m_argc = 0;
    m_argv = NULL;
    m_pTermSem = NULL;

    m_didUsage = false;
    m_autoplay = true;

    m_props.RegisterPropertyWatcher("pcm_volume", (PropertyWatcher *) this);

    m_context->plm = m_plm;
    m_context->props = &m_props;
    m_context->target = (EventQueue *) this;
    m_context->timerManager = new TimerManager();

    // add timer for checking CDs
    // this should be used only if there is no way to get notifications
    // from the OS since polling is inefficient

#ifndef WIN32
    m_cdTimer = NULL;

    bool pollCD = false;
    m_context->prefs->GetPrefBoolean(kCheckCDAutomaticallyPref, &pollCD);

    if (pollCD)
        m_context->timerManager->StartTimer(&m_cdTimer, cd_timer, 5, this);

    m_cdTimerActive = pollCD;

#endif
    // make sure the db dir exists so we have a place to store our 
    // stuff

    char* tempDir = new char[_MAX_PATH];
    uint32 length = _MAX_PATH;
    struct stat st;

    m_context->prefs->GetPrefString(kDatabaseDirPref, tempDir, &length);

    if(-1 == stat(tempDir, &st))
    {
        MKDIR(tempDir);
    }

    m_musicCatalog = new MusicCatalog(m_context);
    m_context->catalog = m_musicCatalog;

    string freeampdir = tempDir;
    freeampdir += DIR_MARKER_STR;
    freeampdir += "metadatabase";
    m_musicCatalog->SetDatabase(freeampdir.c_str());

    // make sure the music dir exists so we have a place to store our 
    // stuff

    length = _MAX_PATH;
    m_context->prefs->GetPrefString(kSaveMusicDirPref, tempDir, &length);

    if(-1 == stat(tempDir, &st))
    {
        MKDIR(tempDir);
    }

    delete [] tempDir;

    // Check to see if the version changed since the last time we ran.
    // If so, show the introwizard again...
    length = 20;
    char ver[20];
    m_context->prefs->GetPrefString(kLastFreeAmpVersionPref, ver, &length);
    if (strcmp(ver, ZINF_VERSION))
    {
        m_context->prefs->SetPrefBoolean(kWelcomePref, true);
        m_context->prefs->SetPrefString(kLastFreeAmpVersionPref, 
                                        ZINF_VERSION);
    }

    m_dlm = new DownloadManager(m_context);
    m_context->downloadManager = m_dlm;

    m_eqEnabled = false;
    memset(m_eqValues, 0, sizeof(m_eqValues));
}

#define TYPICAL_DELETE(x) /*printf("deleting...\n");*/ if (x) { delete x; x = NULL; }

Player::
~Player()
{
#ifndef WIN32
    m_context->timerManager->StopTimer(m_cdTimer);
#endif

    TYPICAL_DELETE(m_dlm);

    TYPICAL_DELETE(m_pTermSem);

    if(m_argUIList)
    {
        vector<char*>::iterator i = m_argUIList->begin();

        for (; i != m_argUIList->end(); i++)
            delete [] *i; 

        delete m_argUIList;

        m_argUIList = NULL;
    }

    if(m_eventServiceThread)
    {
        m_eventServiceThread->Join();
        delete    m_eventServiceThread;

        m_eventServiceThread = NULL;
    }

    if(m_pmo)
    {
        m_pmo->Pause();
        delete    m_pmo;

        m_pmo = NULL;
    }

    TYPICAL_DELETE(m_eventSem);
    TYPICAL_DELETE(m_eventQueue);

    // Delete CIOs
    if(m_uiList)
    {
        vector<UserInterface *>::iterator i = m_uiList->begin();

        for (; i != m_uiList->end(); i++)
            delete *i; 
        delete m_uiList;

        m_uiList = NULL;
    }

    TYPICAL_DELETE(m_musicCatalog);
    TYPICAL_DELETE(m_uiManipLock);
    TYPICAL_DELETE(m_lmcMutex);
    TYPICAL_DELETE(m_pmiMutex);
    TYPICAL_DELETE(m_pmoMutex);
    TYPICAL_DELETE(m_uiMutex);

    TYPICAL_DELETE(m_lmcRegistry);
    TYPICAL_DELETE(m_pmiRegistry);
    TYPICAL_DELETE(m_pmoRegistry);
    TYPICAL_DELETE(m_uiRegistry);

    map<string, RegistryItem *>::iterator i = m_lmcExtensions->begin();
    for (; i != m_lmcExtensions->end(); i++)
        delete (*i).second;
    TYPICAL_DELETE(m_lmcExtensions);

    map<string, RegistryItem *>::iterator j = m_pmiProtocols->begin();
    for (; j != m_pmiProtocols->end(); j++)
        delete (*j).second;
    TYPICAL_DELETE(m_pmiProtocols);

    TYPICAL_DELETE(m_context->timerManager);
    TYPICAL_DELETE(m_sigStopMutex);
    TYPICAL_DELETE(m_plm);
}

void      
Player::
SetTerminationSemaphore(Semaphore * pSem)
{
    m_pTermSem = pSem;
}

/*
    return true if parsing was successful, false otherwise. 
*/

typedef char *pchar;

bool      
Player::
SetArgs(int32 argc, char **argv)
{
    bool autoplay = false;

    // remember these guys so we can use them later and elsewhere
    m_argc = argc;
    m_argv = argv;

    m_context->argv = m_argv;
    m_context->argc = m_argc;

    // now parse them and pull out any args we know about
    for (int32 i = 1; i < argc; i++)
    {
        char* arg = argv[i];

        // is this an option?
        if(arg[0] == '-' 
#ifdef WIN32
        	|| arg[0] == '/'
#endif   
			)
        {
            switch(arg[1])
            {
                // print help
                case 'h':
                case 'H':
                case '-':
                    if(!strcasecmp(arg + 1, "help") || !strcasecmp(arg + 2, "help"))
                    {
                        Usage(argv[0]);
                        AcceptEvent(new Event(CMD_QuitPlayer));
                        return true;
                    }
                    break;

                // autoplay
                case 'p':
                case 'P':
                    if(!strcasecmp(arg + 1, "play"))
                        autoplay = true;

                // save streams
                // shuffle
                case 's':
                case 'S':
                    if(!strcasecmp(arg + 1, "save"))
		                m_context->argFlags |= FAC_ARGFLAGS_SAVE_STREAMS;
                    else if(!strcasecmp(arg + 1, "shuffle"))
                        m_plm->SetShuffleMode(true);
                    break;

                case 'i':
                case 'I':
                    if(!strcasecmp(arg + 1, "import"))
                    {
                        uint32 length = _MAX_PATH;
                        char   url[_MAX_PATH];
                        
                        i++;
                        FilePathToURL(argv[i], url, &length);
                        m_context->catalog->AddSong(url);
                        m_plm->AddItem(url);
                    }
                    break;

                // set UIs
                case 'u':
                case 'U':
                {
                    if(arg[2] == 'i' || arg[2] == 'I')
                    {
                        char* argUI = NULL;
                        
                        i++;
                        if(i >= argc)
                        {
                            Usage(argv[0]);
                            AcceptEvent(new Event(CMD_QuitPlayer));
                            return false;
                        }

                        arg = argv[i];

                        argUI = new char[strlen(arg) + 1];

                        strcpy(argUI, arg);
                        
                        m_argUIList->push_back(argUI);
                    }

                    break;
                }

                default:
                    break;

            }
        }
        else
        {
            HandleSingleArg(arg);
        }
    }
    
    if(m_autoplay)
    {
        AcceptEvent(new Event(CMD_Play));
    }  

    return true;
}

void Player::HandleSingleArg(char *arg)
{
    char* path = new char[_MAX_PATH];
    char* url = new char[_MAX_PATH];
    char *proto = NULL;

    // is this a URL we know how to handle
    proto = GetProtocol(arg);
    if(IsSupportedProtocol(proto)) 
    {
        m_plm->AddItem(arg);
        delete [] proto;
    }
    else
    {
#ifdef WIN32
        strcpy(path, arg);

        HANDLE handle;
        WIN32_FIND_DATA data;

        handle = FindFirstFile(arg, &data);

        // find long filename for item and
        // expand wildcards...
        if(handle != INVALID_HANDLE_VALUE)
        {
            do
            {
                char* cp = NULL;
            
                cp = strrchr(path, '\\');

                if(cp)
                    cp++;
                else 
                    cp = path;

                strcpy(cp, data.cFileName);


                // make sure we have an absolute path
                ResolvePath(&path);

                // format this path as a URL
                uint32 length = _MAX_PATH;
                FilePathToURL(path, url, &length);

                // who needs to get this, plm or dlm?
                bool giveToDLM = false;
                bool giveToTheme = false;
                char* extension = NULL;

                extension = strrchr(url, '.');
                if(extension)
                {
                    DownloadFormatInfo dlfi;
                    uint32 i = 0;

                    extension++;

                    while(IsntError(m_dlm->GetSupportedDownloadFormats(&dlfi, i++)))
                    {
                        if(!strcasecmp(extension, dlfi.GetExtension()))
                        {
                            giveToDLM = true;
                            break;
                        }
                    }
                    if (!strcasecmp(extension, themeExtension))
                        giveToTheme = true; 
                }

                if (giveToDLM) 
                    m_dlm->ReadDownloadFile(url);
                else 
                {
                    if (giveToTheme)
                        AddTheme(url);
                    else 
                        m_plm->AddItem(url); 
                }	

            }while(FindNextFile(handle, &data));
           
            FindClose(handle);
        }

#else
        strcpy(path, arg);
        
        //printf("Path: %s\r\n", path);
       
        // make sure we have an absolute path
        ResolvePath(&path);
       
        //printf("Resolved: %s\r\n", path);

        // format this path as a URL
        uint32 length = _MAX_PATH;
        FilePathToURL(path, url, &length);
    
        //printf("URL: %s\r\n", url);

        // who needs to get this, plm or dlm?
        bool giveToDLM = false;
        bool giveToTheme = false;
        char* extension = NULL;

        extension = strrchr(url, '.');

        if(extension)
        {
            DownloadFormatInfo dlfi;
            uint32 i = 0;

            extension++;

            while(IsntError(m_dlm->GetSupportedDownloadFormats(&dlfi, i++)))
            {
                if(!strcasecmp(extension, dlfi.GetExtension()))
                {
                    giveToDLM = true;
                    break;
                }
            }
            if (!strcasecmp(extension, themeExtension))
                giveToTheme = true; 
        }

        if (giveToDLM) 
            m_dlm->ReadDownloadFile(url);
        else 
        {
            if (giveToTheme)
                AddTheme(url);
            else {
                struct stat st;
                if (stat(path, &st) == 0) 
                    if (!(st.st_mode & S_IFDIR))
                        m_plm->AddItem(url); 
            }
        }	
#endif
    }
    
    delete [] path;
    delete [] url;
}

void Player::AddTheme(char *url)
{
    char     szSavedTheme[_MAX_PATH], szNewTheme[_MAX_PATH];
    char    *pTempFile;
    uint32   iLen = _MAX_PATH;   

    m_context->prefs->GetPrefString(kThemePathPref, 
                                    szSavedTheme, &iLen);
    iLen = _MAX_PATH;   
    URLToFilePath(url, szNewTheme, &iLen); 
    pTempFile = tmpnam(NULL);

    if (CopyFile(szNewTheme, pTempFile, false))
        AcceptEvent(new LoadThemeEvent(pTempFile, szSavedTheme));
}

void      
Player::
Usage(const char *progname)
{
    if(m_didUsage)
        return;

    printf(The_BRANDING " version " ZINF_VERSION " -- Usage:\n\n");
    printf("%s [-save] [-ui <UI plugin name>] <MP3 file/stream> "
      "[MP3 file/stream] ...\n\n", progname);
    printf("Example command line:\n\n");
    printf("   %s -ui zinf.ui mysong1.mp3 mysong2.mp3\n\n", progname);

    m_didUsage = true;
}

int32     
Player::
CompareNames(const char *p1, const char *p2)
{
// windows plugins and unix plugins are named differently...
#if defined( WIN32 ) || defined ( __BEOS__ )
    return strcasecmp(p1, p2);
#else
    // ut << "Comparing: " << p1 << " to " << p2 << endl;
    if (strcmp(p1, p2))
    {
        // no direct match, try w/ .ui appended...
        char      foo[512];

        sprintf(foo, "%s.ui", p2);
        // ut << "Comparing: " << p1 << " to " << foo << endl;
        if (strcmp(p1, foo))
        {
            // no plugin.ui match, try  plugin-arch.ui
            char      foo[512];

            sprintf(foo, "%s.ui", p2);
            // cout << "Comparing: " << p1 << " to " << foo << endl;
            if (strcmp(p1, foo))
            {
                // no match
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
#endif
}

void      
Player::
Run()
{
   uint32    uiListIndex = 0;
   char     *name = NULL;
   uint32    len = 256;
   Error     error = kError_NoErr;
   int32     uisActivated = 0;
   bool      bValue;

   m_context->prefs->GetPrefBoolean(kUseDebugLogPref, &bValue);
   if (bValue)
      m_context->log->Open();
   
   m_context->prefs->GetPrefBoolean(kLogInputPref, &bValue);
   if (bValue)
      m_context->log->AddLogLevel(LogInput);
   
   m_context->prefs->GetPrefBoolean(kLogOutputPref, &bValue);
   if (bValue)
      m_context->log->AddLogLevel(LogOutput);
   
   m_context->prefs->GetPrefBoolean(kLogDecodePref, &bValue);
   if (bValue)
      m_context->log->AddLogLevel(LogDecode);
   
   m_context->prefs->GetPrefBoolean(kLogPerformancePref, &bValue);
   if (bValue)
      m_context->log->AddLogLevel(LogPerf);

   bool loadSecondaryUIs = true;

   // which ui should we instantiate first??
   if (m_argUIList->size() == 0)
   {
      const char *pref = kUIPref;
      name = new char[len];

#ifdef unix
      if (!getenv("DISPLAY")) {
	  pref = kTextUIPref;
          loadSecondaryUIs = false;
      }
#endif
      while ((error = m_context->prefs->GetPrefString(pref, name, &len)) ==
             kError_BufferTooSmall)
      {
         delete[] name;
         len++;

         name = new char[len];
      }
#ifdef unix
#ifndef HAVE_GTK
      if (!strcmp("zinf.ui", name)) {
          pref = kTextUIPref;

          while ((error = m_context->prefs->GetPrefString(pref, name, &len)) ==
                 kError_BufferTooSmall)
          {
              delete [] name;
              len++;

              name = new char[len];
          }
      }
#endif
#endif
   }
   else
   {
      char *orig = (*m_argUIList)[uiListIndex++];
	  // RAK: This pointer is later re-used when the size of the
      // contents may have changed. See comment below.
      name = new char[strlen(orig) + 1];

      strcpy(name, orig);
   }

    // at this point add in any extra UIs that might be wanted
    len = 255;
    char* secondaries = new char[len];
    while  ((error = m_context->prefs->GetPrefString(kSecondaryUIPref,
                                                     secondaries, &len)) ==
           kError_BufferTooSmall)
    {
       delete[] secondaries;
       len++;

       secondaries = new char[len];
    }

    // We use a set to make sure that only one of each is invoked
    set<string> secondaryUIList;
    char* cp = secondaries;
    char* ui = cp;

    while((cp = strchr(cp, ';')))
    {
        *cp = 0x00;
        secondaryUIList.insert(string(ui));
        //MessageBox(NULL, name, "name", MB_OK);

        cp++;
        ui = cp;
    }

    if(*ui)
    {
        secondaryUIList.insert(string(ui));
        //MessageBox(NULL, name, "name", MB_OK);
    }

    set<string>::const_iterator i;

    for(i = secondaryUIList.begin(); i != secondaryUIList.end(); i++)
    {
        char* ui = new char[(*i).size() + 1];

        strcpy(ui, (*i).c_str());

        m_argUIList->push_back(ui);
    }

    delete [] secondaries;

#ifdef HAVE_GTK
   if (strcmp("zinf.ui", name))
       loadSecondaryUIs = false;
#endif

   len = 255;
   char *downloadName = new char[len];
   while  ((error = m_context->prefs->GetPrefString(kDownloadManagerUIPref,
                                                    downloadName, &len)) ==
           kError_BufferTooSmall)
   {
       delete[] downloadName;
       len++;
     
       downloadName = new char[len];
   }

   len = 255;
   char *musicBrowserName = new char[len];
   while  ((error = m_context->prefs->GetPrefString(kMusicBrowserUIPref, 
                                                    musicBrowserName, &len)) ==
           kError_BufferTooSmall)
   {
       delete[] musicBrowserName;
       len++;

       musicBrowserName = new char[len];
   }

#ifdef WIN32
   len = 255;
   char *toolbarName = new char[len];
   while  ((error = m_context->prefs->GetPrefString(kToolbarUIPref,
                                                    toolbarName, &len)) ==
           kError_BufferTooSmall)
   {
       delete[] toolbarName;
       len++;
     
       toolbarName = new char[len];
   }
#endif

   if (IsntError(error))
   {
      while (*name)
      {
         RegistryItem *item = NULL;
//       UserInterface *ui = NULL;
         int32     i = 0;

         while (NULL != (item = m_uiRegistry->GetItem(i++)))
         {
            if (!CompareNames(item->Name(), downloadName) && loadSecondaryUIs)
            {
               m_ui = (UserInterface *) item->InitFunction()(m_context);

               Error er = m_ui->Init(SECONDARY_UI_STARTUP);
               if (IsntError(er)) 
               {
                   RegisterActiveUI(m_ui);
                   m_downloadUI = m_ui;
               }
               else 
               {
                   delete m_ui;
                   m_ui = NULL;
               }
            }
            else if (!CompareNames(item->Name(), musicBrowserName) && 
                     loadSecondaryUIs)
            {
               m_ui = (UserInterface *) item->InitFunction()(m_context);
              
               Error er = m_ui->Init(SECONDARY_UI_STARTUP);
               if (IsntError(er))
               {
                   RegisterActiveUI(m_ui);
                   m_browserUI = m_ui;
               }
               else
               {
                   delete m_ui;
                   m_ui = NULL;
               }
            }
#ifdef WIN32 
            else
            if (!CompareNames(item->Name(), toolbarName))
            {
               m_ui = (UserInterface *) item->InitFunction()(m_context);

               Error er = m_ui->Init(SECONDARY_UI_STARTUP);
               if (IsntError(er)) 
                   RegisterActiveUI(m_ui);
               else 
               {
                   delete m_ui;
                   m_ui = NULL;
               }
            }
#endif            
            else if (!CompareNames(item->Name(), name))
            {
               m_ui = (UserInterface *) item->InitFunction()(m_context);

               //m_ui->SetPropManager((Properties *) this);
               //m_ui->SetPlaylistManager(m_plm);
               //m_ui->SetArgs(m_argc, m_argv);
               Error     er = m_ui->Init((uisActivated == 0) ? PRIMARY_UI
					 : SECONDARY_UI_STARTUP);
               if (IsntError(er))
               {
                  RegisterActiveUI(m_ui);
                  uisActivated++;
               }
               else
               {
                  delete    m_ui;

                  m_ui = NULL;
               }
           //    break;  Don't think this'll work now...
            }
         }

         if(uiListIndex < m_argUIList->size())
         {
            char *p = (*m_argUIList)[uiListIndex++];
            
            // RAK: Boundschecker was pissed about this. This copy
            // may be larger than the space allocated on line 524
            // So, delete the old pointer and create a new one.
            delete name;
            name = new char[strlen(p) + 1];
            
            strcpy(name, p);
         }
         else
         {
            *name = '\0';
         }
      }

      if (!uisActivated)
      {
#ifdef WIN32
         MessageBox(NULL, The_BRANDING" cannot find a valid user interface module.\r\n"
                          "Please make sure that " the_BRANDING" is installed correctly.\r\n"
                          "You may wish to remove and reinstall " the_BRANDING" to fix this problem",
                          BRANDING" Error", MB_OK);
#else
         const char *thePath = getenv(ZINF_PATH_ENV);

         if (thePath == NULL)
          thePath = m_context->prefs->GetLibDirs();
         cerr << "No UI plugin in '" << thePath << "' matched 'plugins/" << name << "' or 'plugins/" << name << ".ui.'" << endl;
         cerr << The_BRANDING << " will quit." << endl;
#endif
         Event    *e = new Event(CMD_QuitPlayer);

         AcceptEvent(e);
         e = new Event(INFO_ReadyToDieUI);
         AcceptEvent(e);
      }
   }
   m_eventServiceThread = Thread::CreateThread();
   m_eventServiceThread->Create(Player::EventServiceThreadFunc, this);

   m_context->catalog->StartTimer();

   delete[] name;
   delete[] musicBrowserName;
   delete[] downloadName;
#ifdef WIN32
   delete[] toolbarName;
#endif
}

void 
Player::
EventServiceThreadFunc(void *pPlayer)
{
   Player   *pP = (Player *) pPlayer;
   Event    *pC;
   int32     rtnVal = 0x0;

   while (rtnVal == 0)
   {                            // serviceEvent will return 1 if error or time
      if (pP->m_eventQueue->Peek() == NULL)
          pP->m_eventSem->Wait();

      pC = pP->m_eventQueue->Read();
      if (pC)
      {
         rtnVal = pP->ServiceEvent(pC);
      }
   }
}

int32     
Player::
RegisterActiveUI(UserInterface * ui)
{
   GetUIManipLock();
   if (m_uiList && ui)
   {
      m_uiList->push_back(ui);
      ReleaseUIManipLock();
      return 0;
   }
   else
   {
      ReleaseUIManipLock();
      return 255;
   }
}

int32     
Player::
RegisterLMCs(Registry * registry)
{
   int32     result = 0;

   m_lmcMutex->Acquire();

   if (m_lmcRegistry)
   {
      Registrar::CleanupRegistry(m_lmcRegistry);
      delete    m_lmcRegistry;
   }

   if (m_lmcExtensions) {
      map<string, RegistryItem *>::iterator iter = m_lmcExtensions->begin();
      for (; iter != m_lmcExtensions->end(); iter++)
          delete (*iter).second;
      delete m_lmcExtensions;
   }

   m_lmcExtensions = new map<string, RegistryItem *>;

   m_lmcRegistry = registry;

   RegistryItem *lmc_item;
   LogicalMediaConverter *lmc;
   int iItems = registry->CountItems();

   for (int iLoop = 0; iLoop < iItems; iLoop++)
   {
      RegistryItem* temp = registry->GetItem(iLoop);

      lmc = (LogicalMediaConverter *)temp->InitFunction()(m_context);
      vector<string> *extList = lmc->GetExtensions();
      vector<string>::iterator i;

      for (i = extList->begin(); i != extList->end(); i++)
      {
          lmc_item = new RegistryItem(*temp);
          (*m_lmcExtensions)[(*i)] = lmc_item;
      }

      delete extList;
      delete lmc;
   }

   m_lmcMutex->Release();

   return result;
}

int32     
Player::
RegisterPMIs(Registry * registry)
{
   int32     result = 0;

   m_pmiMutex->Acquire();

   if (m_pmiRegistry)
   {
      Registrar::CleanupRegistry(m_pmiRegistry);
      delete    m_pmiRegistry;
   }

   if (m_pmiProtocols) {
       map<string, RegistryItem *>::iterator iter = m_pmiProtocols->begin();
       for (; iter != m_pmiProtocols->end(); iter++)
           delete (*iter).second;
       delete m_pmiProtocols;
   }

   m_pmiProtocols = new map<string, RegistryItem *>;
   m_pmiRegistry = registry;

   RegistryItem *pmi_item;
   PhysicalMediaInput *pmi;
   int iItems = registry->CountItems();

   for (int iLoop = 0; iLoop < iItems; iLoop++)
   {
      RegistryItem *temp = registry->GetItem(iLoop);

      pmi = (PhysicalMediaInput *)temp->InitFunction() (m_context);
      vector<string> *protoList = pmi->GetProtocols();
      vector<string>::iterator i;

      for (i = protoList->begin(); i != protoList->end(); i++)
      {
         pmi_item = new RegistryItem(*temp);
         (*m_pmiProtocols)[(*i)] = pmi_item;
      }

      delete protoList;
      delete pmi;
   }
   
   m_pmiMutex->Release();

   return result;
}

int32     
Player::
RegisterPMOs(Registry * registry)
{
   int32     result = 0;

   m_pmoMutex->Acquire();

   if (m_pmoRegistry)
   {
    Registrar::CleanupRegistry(m_pmoRegistry);
      delete    m_pmoRegistry;
   }

   m_pmoRegistry = registry;

   m_pmoMutex->Release();

   return result;
}

int32     
Player::
RegisterUIs(Registry * registry)
{
   int32     result = 0;

   m_uiMutex->Acquire();

   if (m_uiRegistry)
   {
    Registrar::CleanupRegistry(m_uiRegistry);
      delete    m_uiRegistry;
   }

   m_uiRegistry = registry;

   m_uiMutex->Release();

   return result;
}

Registry* 
Player::
GetLMCRegistry() const
{
    return m_lmcRegistry;
}

Registry* 
Player::
GetPMIRegistry() const
{
    return m_pmiRegistry;
}

Registry* 
Player::
GetPMORegistry() const
{
    return m_pmoRegistry;
}

Registry*  
Player::
GetUIRegistry() const
{
    return m_uiRegistry;
}

void 
Player::
GetUIManipLock()
{
   m_uiManipLock->Acquire();
}

void 
Player::
ReleaseUIManipLock()
{
   m_uiManipLock->Release();
}

Error 
Player::
AcceptEvent(Event * e)
{
   if (!m_eventQueue)
       return kError_YouScrewedUp;

   m_eventQueue->Write(e);
   m_eventSem->Signal();
   return kError_NoErr;
}

bool 
Player::
SetState(PlayerState ps)
{
#if 0
   printf("Player state: ");
   switch(ps)
   {
       case PlayerState_Stopped:
          printf("Stopped.\n");
          break;
       case PlayerState_Paused:
          printf("Paused.\n");
          break;
       case PlayerState_Playing:
          printf("Playing.\n");
          break;
       default:
          printf("Unknown.\n");
          break;
   }
#endif
   if (ps == m_playerState)
      return false;
   m_playerState = ps;
   return true;
}

char *
Player::
GetProtocol(const char *title)
{
   char *temp_proto;
   char *proto_return = NULL;

   temp_proto = strstr(title, "://");
   if(temp_proto)
   {
     int proto_len = temp_proto - title;
     proto_return = new char[ proto_len + 1];
     strncpy(proto_return, title, proto_len);
     proto_return[proto_len] = 0;
   }

   return proto_return;
}

char *
Player::
GetExtension(const char *title)
{
   char *temp_ext;
   char *ext_return = NULL;
   char *proto = NULL;

   temp_ext = strrchr(title, '.');
   if (temp_ext)
   {
      temp_ext = temp_ext + 1;
      ext_return = new char [strlen(temp_ext) + 1];
      strcpy(ext_return, temp_ext);
      char *p = ext_return;
      while (*p) {
         *p = toupper(*p);
         p++;
      }
   }

   if (!IsSupportedExtension(ext_return) && 
       !(m_plm->IsSupportedPlaylistFormat(ext_return)))
   {
       delete [] ext_return;
       ext_return = NULL;
       proto = GetProtocol(title);
 
       if (IsSupportedProtocol(proto) && (strncasecmp(proto, "file", 4) != 0))
       {
           ext_return = new char[4];
           strcpy(ext_return, "MP3");
       }
       if (proto)
           delete [] proto;
   }

   return ext_return;
}

bool
Player::
IsSupportedExtension(const char *ext)
{
   if (ext == NULL)
       return false;
   if (m_lmcExtensions->find(ext) != m_lmcExtensions->end())
       return true;
   return false;
}

bool
Player::
IsSupportedProtocol(const char *proto)
{
   if (proto == NULL)
       return false;
   if (m_pmiProtocols->find(proto) != m_pmiProtocols->end())
       return true;
   return false;
}

RegistryItem *
Player::
ChooseLMC(const char *szUrl)
{
   RegistryItem *lmc_item = NULL;
   char     *iExt;

   iExt = GetExtension(szUrl);
   if (!iExt)
      return NULL;

   if (m_lmcExtensions->find(iExt) != m_lmcExtensions->end()) 
       lmc_item = (*m_lmcExtensions)[iExt];

   delete iExt;

   return lmc_item;
}

RegistryItem *
Player::
ChoosePMI(const char *szUrl, char *szTitle)
{
   PhysicalMediaInput *pmi;
   RegistryItem *pmi_item, *ret = NULL;
   int       iLoop;
   char     *szNewUrl = NULL;

   if (strstr(szUrl, "://") == NULL)
   {
      szNewUrl = new char[strlen(szUrl) + strlen("file:// ") + 1];

      sprintf(szNewUrl, "file://%s", szUrl);

      szUrl = szNewUrl;
   }

   for (iLoop = 0; iLoop < m_pmiRegistry->CountItems(); iLoop++)
   {
      pmi_item = m_pmiRegistry->GetItem(iLoop);

      pmi = (PhysicalMediaInput *) pmi_item->InitFunction()(m_context);
      if (pmi->CanHandle(szUrl, szTitle))
      {
         ret = pmi_item;
         delete pmi;

         break;
      }
      delete pmi;
   }

   if (szNewUrl)
      delete szNewUrl;

   return ret;
}

void 
Player::
CreatePMO(const PlaylistItem * pc, Event * pC)
{
   Error     error = kError_NoErr;
   Event    *e;
   PhysicalMediaOutput *pmo = NULL;
   PhysicalMediaInput *pmi = NULL;
   LogicalMediaConverter *lmc = NULL;
   RegistryItem *pmi_item = NULL;
   RegistryItem *lmc_item = NULL;
   RegistryItem *item = NULL;

   if (!pc)
   {
      m_plm->SetCurrentIndex(0);
      if (m_pmo)
      {
         m_pmo->Pause();
         delete m_pmo;
         m_pmo = NULL;

         m_lmc = NULL;
      }
      if (SetState(PlayerState_Stopped))
      {
         SEND_NORMAL_EVENT(INFO_Stopped);
      }
      GetUIManipLock();

      e = new Event(INFO_PlaylistDonePlay);
      SendToUI(e);

      ReleaseUIManipLock();

      delete e;
   }

   if (m_pmo)
   {
      m_pmo->Pause();
      delete    m_pmo;
      m_pmo = NULL;
      m_lmc = NULL;
   }

   pmi_item = ChoosePMI(pc->URL().c_str());
   if (!pmi_item)
   {
      char szErr[1024];

      sprintf(szErr, "Cannot determine what pmi to use for %s\n", pc->URL().c_str());
      m_context->log->Error(szErr);
      AcceptEvent(new ErrorMessageEvent(szErr));

      return;
   }

   lmc_item = ChooseLMC(pc->URL().c_str());
   if (!lmc_item) 
   {
   // FIXME: Should probably have a user definable default LMC
      lmc_item = ChooseLMC("blah.mp3");
   }

   if (pmi_item)
   {
      pmi = (PhysicalMediaInput *) pmi_item->InitFunction()(m_context);
      pmi->SetPropManager((Properties *) this);
   }

   char *extension = GetExtension(pc->URL().c_str());
   if (extension) {
       if (!strncasecmp("CDA", extension, 3)) {
           int32 i = 0;

           while (NULL != (item = m_pmoRegistry->GetItem(i++)))
           {
               if (!strcmp("cd.pmo", item->Name()))
               {
                   break;
               }
           }
       }
       delete [] extension;
   }

   if (!item) {
       char defaultPMO[256];
       uint32 size = sizeof(defaultPMO);

       m_context->prefs->GetPrefString(kPMOPref, defaultPMO, &size);

       int32 i = 0;

       while (NULL != (item = m_pmoRegistry->GetItem(i++)))
       {
            if(!strcmp(defaultPMO, item->Name()))
            {
                break;
            }
       }

	   // if the default isn't around then just use first one 
	   // is there a better way?
	   if(!item)
		  item = m_pmoRegistry->GetItem(0);

	   if (item)
	   {
		  pmo = (PhysicalMediaOutput *) item->InitFunction()(m_context);
		  pmo->SetPropManager((Properties *) this);
	   }

	   error = kError_NoErr;
	   if (lmc_item)
	   {
		  lmc = (LogicalMediaConverter *) lmc_item->InitFunction()(m_context);

		  lmc->SetPropManager((Properties *) this);
	   }

	   lmc->SetPMI(pmi);
	   lmc->SetPMO(pmo);

	   pmo->SetPMI(pmi);
	   pmo->SetLMC(lmc);

	   lmc->SetEQData(m_eqValues, m_eqPreamp);
	   lmc->SetEQData(m_eqEnabled);
   }
   else
   {
	   // We're using the CD PMO, do no need to set up an LMC or PMI
	   pmo = (PhysicalMediaOutput *) item->InitFunction()(m_context);
	   pmo->SetPropManager((Properties *) this);
   }

   pmi = NULL;
   m_lmc = lmc;
   lmc = NULL;

   error = pmo->SetTo(pc->URL().c_str());
   // If this is a missing file, then just bail out -- the 
   // pmo that was created will have sent a missing file event
   // that will notify the user
   if (error == kError_FileNotFound)
   {
      AcceptEvent(new MissingFileEvent((PlaylistItem *)
                                       m_plm->GetCurrentItem()));
      //m_plm->GotoNextItem();
      AcceptEvent(new Event(CMD_Play));
      goto epilogue;
   }

   if (IsError(error))
   {
      char szErr[1024];

      sprintf(szErr, "Cannot setup the audio decode process: %d\n", error);
      m_context->log->Error(szErr);

      goto epilogue;
   }

   m_pmo = pmo;
   pmo = NULL;

   epilogue:

   if (pmo)
   {
       m_pmo = NULL;
       delete pmo;
   }

   if (pmi)
   {
       delete pmi;
   }

   if (lmc)
   {
       m_lmc = NULL;
       delete lmc;
   }
}

void 
Player::
DoneOutputting(Event *pEvent)
{
   // LMC or PMO sends this when its done
   // outputting whatever.  Now, go on to next
   // piece in playlist

   if (m_pmo)
   {
      delete m_pmo;
      m_pmo = NULL;
      m_lmc = NULL;
   }

#if 0
   PlaylistItem *item = m_plm->GetCurrentItem();
   if (item)
   {
      MetaData mdata = (MetaData)item->GetMetaData();
      if (mdata.GUID().size() == 16) {
          mdata.AddPlayCount();
          item->SetMetaData(&mdata);
          m_musicCatalog->UpdateSong(item);
          m_APSInterface->WriteToLog(mdata.GUID());   
      }
   }
#endif
   if (SetState(PlayerState_Stopped))
   {
       SEND_NORMAL_EVENT(INFO_Stopped);
   }

   SEND_NORMAL_EVENT(INFO_DoneOutputting);

   if (pEvent->Type() == INFO_DoneOutputtingDueToError &&
       (m_plm->GetRepeatMode() == kPlaylistMode_RepeatOne || 
        (m_plm->GetRepeatMode() == kPlaylistMode_RepeatAll && 
         m_plm->CountItems() == 1))) 
   {
      m_plm->SetCurrentIndex(0);
      SEND_NORMAL_EVENT(INFO_PlaylistDonePlay);
      delete pEvent;
      
      return;
   }

   if (m_plm->HasAnotherItem()) 
   {
      //AcceptEvent(new Event(CMD_NextMediaPiece));
      m_plm->GotoNextItem(false);

      if (m_playerState == PlayerState_Paused)
      {
         AcceptEvent(new Event(CMD_PlayPaused));
      }
      else
      {
         AcceptEvent(new Event(CMD_Play));
      }
   }
   else
   {
      m_plm->SetCurrentIndex(0);
      SEND_NORMAL_EVENT(INFO_PlaylistDonePlay);
   }
   delete pEvent;
}


void 
Player::
Stop(Event *pEvent)
{
    if (m_pmo)
    {
       m_pmo->Pause();
       delete m_pmo;
       m_pmo = NULL;
       m_lmc = NULL;
    }

    if (SetState(PlayerState_Stopped))
    {
       SEND_NORMAL_EVENT(INFO_Stopped);
    }

    delete pEvent;
}

void
Player::
GetVolume(Event *pEvent)
{
    int32 left = -1, right = -1;

    delete pEvent;
    if (m_pmo) 
    {
       m_pmo->GetVolume(left, right);
       Event *e = new VolumeEvent(INFO_VolumeInfo, left, right);
       SendToUI(e);
       delete e;
    }   
}

void
Player::
SetVolume(Event *pEvent)
{
    int32 left=((VolumeEvent *) pEvent)->GetLeftVolume();
    int32 right=((VolumeEvent *) pEvent)->GetRightVolume();
    if (m_pmo) 
        m_pmo->SetVolume(left, right);
    delete pEvent;
}

void 
Player::
ChangePosition(Event *pEvent)
{
    if (m_pmo)
       m_pmo->ChangePosition(((ChangePositionEvent *) pEvent)->GetPosition());

    delete pEvent;
}

void 
Player::
HandleQueryState()
{
    if (m_playerState == PlayerState_Playing)
    {
        SEND_NORMAL_EVENT(INFO_Playing);
    }
    else if (m_playerState == PlayerState_Stopped)
    {
        SEND_NORMAL_EVENT(INFO_Stopped);
    }
    else if (m_playerState == PlayerState_Paused)
    {
        SEND_NORMAL_EVENT(INFO_Paused);
    }
}

void 
Player::
Play(Event *pEvent)
{
    const PlaylistItem               *pItem;
	
	if ((m_playerState == PlayerState_Playing) && (pEvent->Type() == PlayerState_Playing)) 
	{	// ignore double Playing event
		delete pEvent;
		return;
	}
    
	if (m_playerState == PlayerState_Playing)
    {
       delete m_pmo;
       m_pmo = NULL;
       m_lmc = NULL;

       if (SetState(PlayerState_Stopped))
       {
           SEND_NORMAL_EVENT(INFO_Stopped);
       }
    }

    if (!m_pmo)
    {
       pItem = m_plm->GetCurrentItem();
       if (pItem)
       {
            CreatePMO(pItem, pEvent);
       }   

       if (!m_pmo)  {
          delete pEvent;
          return;
       }
    }

    if (pEvent->Type() == CMD_PlayPaused)
    {
        if (SetState(PlayerState_Paused))
        {
           SEND_NORMAL_EVENT(INFO_Playing); 
           SEND_NORMAL_EVENT(INFO_Paused);
        }
    }
    else
    {
        m_pmo->Resume();
        if (SetState(PlayerState_Playing))
        {
           SEND_NORMAL_EVENT(INFO_Playing);
        }
    }

    delete pEvent;
}

void 
Player::
Next(Event *pEvent)
{
   if (m_playerState != PlayerState_Stopped)
   {
#if 0
       PlaylistItem *item = m_plm->GetCurrentItem();
       if (item)
       {
          MetaData mdata = (MetaData)item->GetMetaData();
          m_APSInterface->WriteToLog(mdata.GUID(), APSInterface::SongSkip);
       }
#endif
      AcceptEvent(new Event(CMD_Stop));
   }

   m_plm->GotoNextItem(true);

   if (m_playerState != PlayerState_Stopped)
   {
       if (m_playerState == PlayerState_Paused)
       {
          AcceptEvent(new Event(CMD_PlayPaused));
       }   
       else
       { 
          AcceptEvent(new Event(CMD_Play));
       }   
   }

   delete pEvent;
}

void 
Player::
Previous(Event *pEvent)
{
   if (m_playerState != PlayerState_Stopped)
   {
#if 0
       PlaylistItem *item = m_plm->GetCurrentItem();
       if (item)
       {
          MetaData mdata = (MetaData)item->GetMetaData();
          m_APSInterface->WriteToLog(mdata.GUID(), APSInterface::SongSkip);
       }
#endif
      AcceptEvent(new Event(CMD_Stop));
   }

   m_plm->GotoPreviousItem(true);

   if (m_playerState != PlayerState_Stopped)
   {
       if (m_playerState == PlayerState_Paused)
          AcceptEvent(new Event(CMD_PlayPaused));
       else
          AcceptEvent(new Event(CMD_Play));
   }

   delete pEvent;
}

void 
Player::
Pause(Event *pEvent)
{
   if (m_pmo)
   {
      m_pmo->Pause();
      if (SetState(PlayerState_Paused))
         SEND_NORMAL_EVENT(INFO_Paused);
   }
   delete pEvent;
}

void 
Player::
UnPause(Event *pEvent)
{
   if (m_pmo)
   {
      m_pmo->Resume();
      if (SetState(PlayerState_Playing))
         SEND_NORMAL_EVENT(INFO_Playing);
   }
   delete pEvent;
}

void 
Player::
TogglePause(Event *pEvent)
{
    if (m_pmo)
    {
       if (m_playerState == PlayerState_Playing)
           Pause(NULL);
       else
       if (m_playerState == PlayerState_Paused)
           UnPause(NULL);
    }

    delete pEvent;
}

int 
Player::
Quit(Event *pEvent)
{
   Event *pe;

   Stop(NULL);
   // 1) Set "I'm already quitting flag" (or exit if its already Set)
   m_imQuitting = 1;
   // 2) Get CIO/COO manipulation lock

   GetUIManipLock();
   // 3) Count CIO/COO, put into m_quitWaitingFor.
   m_quitWaitingFor = m_uiList->size();
   // 4) Send CMD_Cleanup event to all CIO/COOs

   pe = new Event(CMD_Cleanup);

   SendToUI(pe);
   delete    pe;

   delete  pEvent;

   if (m_quitWaitingFor == 0)
   {
      if (m_pTermSem)
      {
         m_pTermSem->Signal();
      }
      return 1;
   }
   else
   {
      ReleaseUIManipLock();
      return 0;
   }
}

int 
Player::
ReadyToDieUI(Event *pEvent)
{
   delete pEvent;

   if (!m_imQuitting)
       return 0;

   m_quitWaitingFor--;
   if (m_quitWaitingFor > 0)
       return 0;

   GetUIManipLock();
   if (m_pTermSem)
      m_pTermSem->Signal();

   return 1;
}

void 
Player::
HandleMediaInfo(Event *pEvent)
{
   MediaInfoEvent *pmvi;
   Event          *pe = NULL;
   
   GetUIManipLock();

   pmvi = (MediaInfoEvent *)pEvent;
   pmvi->m_indexOfSong = m_plm->GetCurrentIndex() + 1;         // zero based
   pmvi->m_totalSongs = m_plm->CountItems();

   SendToUI(pEvent);

   for (uint32 foobar = 0; foobar < pmvi->m_childEvents->size(); foobar++)
   {
      pe = (*pmvi->m_childEvents)[foobar];
      SendToUI(pe);
   }

   ReleaseUIManipLock();

   delete pEvent;
}

void 
Player::
HandleMediaTimeInfo(Event *pEvent)
{
   if (m_playerState == PlayerState_Playing)
   {
      GetUIManipLock();
      SendToUI(pEvent);
      ReleaseUIManipLock();
   }

   delete pEvent;
}

void 
Player::
SendEventToUI(Event *pEvent)
{
   GetUIManipLock();
   SendToUI(pEvent);
   ReleaseUIManipLock();
}

void
Player::
SendEventToCatalog(Event *pEvent)
{
    if (m_musicCatalog)
        m_musicCatalog->AcceptEvent(pEvent);
}

void 
Player::
SetEQData(Event *pEvent)
{
   if (((SetEqualizerDataEvent *) pEvent)->IsEQData())
   {
       memcpy(m_eqValues, ((SetEqualizerDataEvent *) pEvent)->GetEQData(), 
              sizeof(float) * 32);
       m_eqPreamp = ((SetEqualizerDataEvent *) pEvent)->GetPreamp(); 
   }
   else
       m_eqEnabled = ((SetEqualizerDataEvent *) pEvent)->GetEnableState();

   if (m_lmc && m_pmo)
   {
       if (((SetEqualizerDataEvent *) pEvent)->IsEQData())
       {
           m_eqPreamp = ((SetEqualizerDataEvent *) pEvent)->GetPreamp(); 
           m_lmc->SetEQData(m_eqValues, m_eqPreamp);
       }
       else
           m_lmc->SetEQData(m_eqEnabled);
   }
   delete pEvent;
}

#define _VISUAL_ENABLE_
#ifdef  _VISUAL_ENABLE_
void 
Player::
SendVisBuf(Event *pEvent)
{
   if (m_playerState == PlayerState_Playing)
   {
      GetUIManipLock();
      SendToUI(pEvent);
      ReleaseUIManipLock();
   }
   delete pEvent;
}
#endif // _VISUAL_ENABLE_
#undef  _VISUAL_ENABLE_

void
Player::
ToggleUI(Event *pEvent)
{
   switch (pEvent->Type()) 
   {
      case CMD_ToggleDownloadUI: {
           if (!m_downloadUI) {
              delete pEvent;
              return;
           }
           m_downloadUI->AcceptEvent(pEvent);
           break; }
      case CMD_TogglePlaylistUI: 
      case CMD_ToggleMusicBrowserUI: {
           if (!m_browserUI) {
              delete pEvent;
              return;
           }
           m_browserUI->AcceptEvent(pEvent);
           break; }
      default:
           break;
   }
   delete pEvent;
} 

void
Player::
HandlePrefsChanged(Event *pEvent)
{
#ifndef WIN32
    bool pollCD = false;
    m_context->prefs->GetPrefBoolean(kCheckCDAutomaticallyPref, &pollCD);

    if (pollCD && !m_cdTimerActive)
        m_context->timerManager->StartTimer(&m_cdTimer, cd_timer, 5, this);
    else if (!pollCD && m_cdTimerActive)
        m_context->timerManager->StopTimer(m_cdTimer);
 
    m_cdTimerActive = pollCD;
#endif

    bool useProxy = false;
    m_context->prefs->GetPrefBoolean(kUseProxyPref, &useProxy);
    if (useProxy)
    {
       uint32 size = 1024;
       char *buffer = (char *)malloc(size);

       if (kError_BufferTooSmall == m_context->prefs->GetPrefString(
                                                 kProxyHostPref, buffer, &size))
       {
           buffer = (char *)realloc(buffer, size);
           m_context->prefs->GetPrefString(kProxyHostPref, buffer, &size);
       }

       char *port = strrchr(buffer, ':');
       if (port) {
           *port = '\0';
           port++;
       }

       string proxyAddr = string(buffer, strlen(buffer) + 1);
       int nPort = 80;
       if (port && *port)
           nPort = atoi(port);

       free(buffer);
    }


    SendEventToUI(pEvent);
    SendEventToCatalog(pEvent);

    delete pEvent;
}

int32 
Player::
ServiceEvent(Event * pC)
{
    if (!pC)
    {
      return 255;
    }

    //printf("Got event %d\n", pC->Type());
    switch (pC->Type())
    {
        case INFO_DoneOutputtingDueToError:
        case INFO_DoneOutputting:
            DoneOutputting(pC);
            break;

        case CMD_Stop:
            Stop(pC);
            break;

        case CMD_GetVolume:
            GetVolume(pC);
            break;

        case CMD_SetVolume:
            SetVolume(pC);
            break;

        case CMD_ChangePosition:
            ChangePosition(pC);
            break;

        case CMD_PlayPaused:
            Play(pC);
            break;

        case CMD_Play:
            Play(pC);
            break;

        case CMD_NextMediaPiece:
            Next(pC); 
            break;

        case CMD_PrevMediaPiece:
            Previous(pC);
            break;

        case CMD_Pause:
            Pause(pC);
            break;

        case CMD_UnPause:
            UnPause(pC);
            break;

        case CMD_TogglePause:
            TogglePause(pC);
            break;

        case CMD_QuitPlayer:
            return Quit(pC);

        case CMD_QueryPlayerState:
            HandleQueryState();
            break;

        case INFO_ReadyToDieUI:
            return ReadyToDieUI(pC);

        case INFO_MediaInfo:
            HandleMediaInfo(pC);
            break;

        case INFO_MediaTimeInfo:
            HandleMediaTimeInfo(pC);
            break;

        case INFO_PrefsChanged:
            HandlePrefsChanged(pC);
            break;

        case INFO_PlaylistItemsUpdated:
            SendEventToUI(pC);
            SendEventToCatalog(pC);
            delete pC;
            break;

        case INFO_UserMessage:
        case INFO_StatusMessage:
        case INFO_BrowserMessage:
        case INFO_ErrorMessage:
        case INFO_FileNotFound:
        case INFO_StreamInfo:
        case INFO_PlaylistShuffle:
        case INFO_PlaylistRepeat:
        case INFO_PlaylistUpdated:
        case INFO_PlaylistItemAdded:
        case INFO_PlaylistItemsAdded:
        case INFO_PlaylistItemRemoved:
        case INFO_PlaylistItemMoved:
        case INFO_PlaylistSorted:
        case INFO_PlaylistCurrentItemInfo:
        case INFO_BufferStatus:
        case INFO_HeadlineText:
        case INFO_SearchMusicDone:
        case INFO_DownloadItemAdded:
        case INFO_DownloadItemRemoved:
        case INFO_DownloadItemNewState:
        case INFO_DownloadItemProgress:
        case INFO_MusicCatalogTrackAdded:
        case INFO_MusicCatalogTrackRemoved:
        case INFO_MusicCatalogStreamAdded:
        case INFO_MusicCatalogStreamRemoved:
        case INFO_MusicCatalogPlaylistAdded:
        case INFO_MusicCatalogPlaylistRemoved:
        case INFO_MusicCatalogTrackChanged:
        case INFO_MusicCatalogCleared:
        case INFO_MusicCatalogRegenerating:
        case INFO_MusicCatalogDoneRegenerating:
        case INFO_CDDiscStatus:
        case INFO_VorbisInfo:
        case CMD_AddFiles:
        case CMD_LoadTheme:
        case CMD_ShowPreferences:
        case CMD_EditCurrentPlaylistItemInfo:
        case INFO_CDNotFound:
        case INFO_DatabaseUpgraded:
            SendEventToUI(pC);
            delete pC;
            break;

        case CMD_ToggleDownloadUI:
        case CMD_TogglePlaylistUI:
        case CMD_ToggleMusicBrowserUI:
            ToggleUI(pC);
            break;

        case CMD_SetEQData:
            SetEQData(pC);
            break;

#define _VISUAL_ENABLE_
#ifdef  _VISUAL_ENABLE_
        case CMD_SendVisBuf:
            SendVisBuf(pC);
            break;
#endif // _VISUAL_ENABLE_
#undef  _VISUAL_ENABLE_

        default:
            m_context->log->Error("serviceEvent: Unknown event: %d\n",
                                  pC->Type());
            delete  pC;
            break;
    }

    return 0;
}

Error     
Player::
PropertyChange(const char *pProp, PropValue * ppv)
{
   Error     rtn = kError_UnknownErr;

   return rtn;
}

void      
Player::
SendToUI(Event * pe)
{
   uint32     i;

   for (i = 0; i < m_uiList->size(); i++)
   {
      (*m_uiList)[i]->AcceptEvent(pe);
   }
}

Error     
Player::
GetProperty(const char *pProp, PropValue ** ppVal)
{
   return m_props.GetProperty(pProp, ppVal);
}

Error     
Player::
SetProperty(const char *pProp, PropValue * pVal)
{
   return m_props.SetProperty(pProp, pVal);
}

Error     
Player::
RegisterPropertyWatcher(const char *pProp, PropertyWatcher * pPropWatch)
{
   return m_props.RegisterPropertyWatcher(pProp, pPropWatch);
}

Error     
Player::
RemovePropertyWatcher(const char *pProp, PropertyWatcher * pPropWatch)
{
   return m_props.RemovePropertyWatcher(pProp, pPropWatch);
}

void Player::cd_timer(void* arg)
{
    Player* player = (Player*) arg;

    player->CDTimer();
}

void Player::UpdateCDNow(void)
{
    CDTimer();
}

void Player::CDTimer()
{
    Registry *pmoRegistry = m_context->player->GetPMORegistry();
    RegistryItem *pmo_item = NULL;
    int32 i = 0;

    if (!pmoRegistry)
        return;

    while(NULL != (pmo_item = pmoRegistry->GetItem(i++))) 
    {
        if(!strcmp("cd.pmo", pmo_item->Name())) 
        {
            break;
        }
    }

    if(!pmo_item)
        return;

    PhysicalMediaOutput *pmo;
    pmo = (PhysicalMediaOutput *)pmo_item->InitFunction()(m_context);
    pmo->SetPropManager(m_context->props);

    if(IsError(pmo->Init(NULL))) 
    {
        CDInfoEvent *cie = new CDInfoEvent(0, 0, "");
        AcceptEvent(cie);
    }

    delete pmo;
}
