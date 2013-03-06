/*____________________________________________________________________________

        FreeAmp - The Free MP3 Player

        Portions Copyright (C) 1999 EMusic.com

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

        $Id: musiccatalog.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786) 
#endif

#include "config.h"

#include <sys/stat.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include "win32impl.h"
#endif

#include <string>
#include <algorithm>

using namespace std;

#include "musiccatalog.h"
#include "player.h"
#include "utility.h"
#include "pmo.h"
#include "debug.h"

#define METADATABASE_VERSION 1
#define DATABASE_SUB_VERSION 3

MusicCatalog::MusicCatalog(FAContext *context, char *databasepath)
{
    m_database = NULL;
    m_context = context;
    m_plm = context->plm;
    m_mutex = new Mutex();
    m_catMutex = new Mutex();
    m_timerMutex = new Mutex();
    m_MBLookupLock = new Mutex();
    m_inUpdateSong = false;
    m_addImmediately = false;
    m_bSurpressAddMessages = false;
    m_trackCount = 0;
    m_artistList = new vector<ArtistList *>;
    m_unsorted = new vector<PlaylistItem *>;
    m_playlists = new vector<string>;
    m_streams = new vector<PlaylistItem *>;
    m_MBRequests = new vector<pair<string, string> >;
    m_watchTimer = NULL;

    m_killGUIDs = false;

    m_killMBThread = false;
    m_MBLookupThreadActive = false;
    m_pendingMBLookups = 0;
    m_timeout = 0;
    context->prefs->GetPrefInt32(kWatchThisDirTimeoutPref, &m_timeout);

    if (databasepath)
        SetDatabase(databasepath);
}

void MusicCatalog::StartTimer(void)
{
    if (m_timeout > 0) {
        WatchTimer();
        m_context->timerManager->StartTimer(&m_watchTimer, watch_timer,
                                            m_timeout, this);
    }
}

MusicCatalog::~MusicCatalog()
{
    m_killMBThread = true;
    while (m_MBLookupThreadActive)
        usleep(50);

    ClearCatalog();
    StopSearchMusic();

    m_mutex->Acquire();
    m_catMutex->Acquire();
    m_timerMutex->Acquire();

    if(m_watchTimer)
        m_context->timerManager->StopTimer(m_watchTimer);

    delete m_artistList;
    delete m_unsorted;
    delete m_streams;

    if (m_database)
        delete m_database;

    delete m_playlists;
    delete m_mutex;
    delete m_catMutex;
    delete m_timerMutex;
}

class comp_catalog {
  public:
      bool operator()(PlaylistItem *a, PlaylistItem *b)
      {
          if (a->GetMetaData().Track() == b->GetMetaData().Track())
             // sort alphabetically...
             return (a->GetMetaData().Title() < b->GetMetaData().Title());
          return (a->GetMetaData().Track() < b->GetMetaData().Track());
      }
      bool operator()(AlbumList *a, AlbumList *b)
      {
          return (a->name < b->name);
      }
      bool operator()(ArtistList *a, ArtistList *b)
      {
          return (a->name < b->name);
      }
};

void MusicCatalog::Sort(void)
{
    m_catMutex->Acquire();
    // Sort the playlists...
    sort(m_playlists->begin(), m_playlists->end());

    // Sort the uncategorized tracks
    sort(m_unsorted->begin(), m_unsorted->end(), comp_catalog());

    // Sort the rest o the junk
    vector<ArtistList *>::iterator i = m_artistList->begin();
    for (; i != m_artistList->end(); i++) {
        vector<AlbumList *>::iterator j = (*i)->m_albumList->begin();
        for (; j != (*i)->m_albumList->end(); j++) {
            sort((*j)->m_trackList->begin(), (*j)->m_trackList->end(), 
                 comp_catalog());
        }
        sort((*i)->m_albumList->begin(), (*i)->m_albumList->end(), 
             comp_catalog());
    }
    sort(m_artistList->begin(), m_artistList->end(), comp_catalog());

    // Sort the streams
    sort(m_streams->begin(), m_streams->end(), comp_catalog());

    m_catMutex->Release();
}

Error MusicCatalog::RemovePlaylist(const char *url)
{
    assert(url);

    if (!m_database->Working())
        return kError_DatabaseNotWorking;

    char *data = m_database->Value(url);

    if (!data)
        return kError_ItemNotFound;

    delete [] data;
    m_database->Remove(url);

    m_catMutex->Acquire();

    vector<string>::iterator i = m_playlists->begin();
    for (; i != m_playlists->end(); i++)
         if ((*i) == url) {
             string tempstr = url;
             if (tempstr.find("currentlist.m3u") >= tempstr.length()) {
                 string sUrl = url; 
                 m_context->target->AcceptEvent(new MusicCatalogPlaylistRemovedEvent(sUrl));
                 m_playlists->erase(i);
             }
             m_catMutex->Release();
             return kError_NoErr;
         }   

    m_catMutex->Release();
    return kError_ItemNotFound;
}


Error MusicCatalog::AddPlaylist(const char *url)
{
    assert(url);

    if (!m_database->Working())
        return kError_DatabaseNotWorking;
    
    char *data = m_database->Value(url);
    if (!data) 
        m_database->Insert(url, "P");
    else 
        delete [] data;

    bool found = false;
    m_catMutex->Acquire();
    vector<string>::iterator i = m_playlists->begin();
    for (; i != m_playlists->end(); i++)
         if ((*i) == url)
             found = true;
    
    if (!found) {
        string tempstr = url;
        if (tempstr.find("currentlist.m3u") < tempstr.length()) 
            m_playlists->insert(m_playlists->begin(), url);
        else
            m_playlists->push_back(url);

        string sUrl = url;
        m_context->target->AcceptEvent(new MusicCatalogPlaylistAddedEvent(sUrl));
        m_catMutex->Release();
        return kError_NoErr;
    }

    m_catMutex->Release();
    return kError_DuplicateItem;
}

Error MusicCatalog::RemoveStream(const char *url)
{
    assert(url);

    if (!m_database->Working())
        return kError_DatabaseNotWorking;

    MetaData *meta = ReadMetaDataFromDatabase(url);
    if (!meta)
        return kError_ItemNotFound;

    string strGUID = meta->GUID();

    m_database->Remove(url);

    m_catMutex->Acquire();
    
    vector<PlaylistItem *>::iterator i = m_streams->begin();
    for (; i != m_streams->end(); i++)
         if ((*i)->URL() == url) {
             m_context->target->AcceptEvent(new MusicCatalogStreamRemovedEvent(*i));
             delete (*i);
             m_streams->erase(i);
             break;
         }

    m_catMutex->Release();

    delete meta;
    return kError_NoErr;
}

PlaylistItem *MusicCatalog::GetPlaylistItemFromURL(const char *url)
{
    assert(url);
    PlaylistItem *retitem = NULL;

    if (!m_database->Working())
        return retitem;

    MetaData *meta = ReadMetaDataFromDatabase(url);
    if (!meta)
        return retitem;

    m_catMutex->Acquire();
    if ((meta->Artist().size() == 0) || (meta->Artist() == " ")) {
        vector<PlaylistItem *>::iterator i = m_unsorted->begin();
        for (; i != m_unsorted->end(); i++)
            if ((*i)->URL() == url)
            {
                retitem = *i;
                break;
            }
    }
    else 
    {
        vector<ArtistList *>::iterator    i;
        vector<AlbumList *>              *alList;
        vector<AlbumList *>::iterator     j;
        vector<PlaylistItem *>           *trList;
        vector<PlaylistItem *>::iterator  k;
        bool                              found = false;

        i = m_artistList->begin();
        for (; i != m_artistList->end() && !found; i++)
        {
            if (CaseCompare(meta->Artist(),(*i)->name))
            {
                alList = (*i)->m_albumList;
                j = alList->begin();
                for (; j != alList->end() && !found; j++)
                {
                    if (CaseCompare(meta->Album(),(*j)->name))
                    {
                        trList = (*j)->m_trackList;
                        k = trList->begin();
                        for (; k != trList->end() && !found; k++)
                            if (url == (*k)->URL())
                            {
                                retitem = *k;
                                found = true;
                                break;
                            }

                     } 
                }
            }
        }
    }
 
    delete meta;

    m_catMutex->Release();
    return retitem;
}

Error MusicCatalog::RemoveSong(const char *url)
{
    assert(url);

    if (!m_database->Working())
        return kError_DatabaseNotWorking;

    MetaData *meta = ReadMetaDataFromDatabase(url);
    if (!meta)
        return kError_ItemNotFound;

    string strGUID = meta->GUID();

    m_database->Remove(url);

    m_catMutex->Acquire();
    if ((meta->Artist().size() == 0) || (meta->Artist() == " ")) {
        vector<PlaylistItem *>::iterator i = m_unsorted->begin();
        for (; i != m_unsorted->end(); i++)
            if ((*i)->URL() == url)
            {
                AcceptEvent(new MusicCatalogTrackRemovedEvent(*i, NULL, NULL));
                m_unsorted->erase(i);
                m_trackCount--;
                break;
            }
    }
    else 
    {
        vector<ArtistList *>::iterator    i;
        vector<AlbumList *>              *alList;
        vector<AlbumList *>::iterator     j;
        vector<PlaylistItem *>           *trList;
        vector<PlaylistItem *>::iterator  k;
        bool                              found = false;
        
        i = m_artistList->begin();
        for (; i != m_artistList->end() && !found; i++) 
        {
            if (CaseCompare(meta->Artist(),(*i)->name)) 
            {
                alList = (*i)->m_albumList;
                j = alList->begin();
                for (; j != alList->end() && !found; j++) 
                {
                    if (CaseCompare(meta->Album(),(*j)->name)) 
                    {
                        trList = (*j)->m_trackList;
                        k = trList->begin();
                        for (; k != trList->end() && !found; k++)
                            if (url == (*k)->URL())
                            {
                                AcceptEvent(new MusicCatalogTrackRemovedEvent(*k, *i, *j));
                                trList->erase(k);
                                found = true;
                                m_trackCount--;
                                break;
                            }    

                        if (trList->size() == 0) {
                            alList->erase(j);
                        }
                     }
                }

                if (alList->size() == 0) {
                    m_artistList->erase(i);
                }
            }
        }
    }

    delete meta;

    m_catMutex->Release();
    return kError_NoErr;
}

Error MusicCatalog::AddStream(const char *url)
{
    assert(url);
  
    if (!m_database->Working())
        return kError_DatabaseNotWorking;

    PlaylistItem *newstream;

    MetaData *meta = ReadMetaDataFromDatabase(url);

    if (!meta)
        return kError_DatabaseNotWorking;

    newstream = new PlaylistItem(url, meta);

    m_catMutex->Acquire();

    vector<PlaylistItem *>::iterator i = m_streams->begin();
    for (; i != m_streams->end(); i++) 
        if ((*i)->URL() == url)
            return kError_DuplicateItem;

    m_streams->push_back(newstream);
    m_context->target->AcceptEvent(new MusicCatalogStreamAddedEvent(newstream));

    m_catMutex->Release();

    return kError_NoErr;
}

bool MusicCatalog::CaseCompare(const string &s1, const string &s2)
{
    if (s1.length() == s2.length()) {
        for (uint32 i = 0; i < s1.length(); i++) {
            if (toupper(s1[i]) != toupper(s2[i]))
                return false;
        }
        return true;
    }
    return false;
}

Error MusicCatalog::AddSong(const char *url)
{
    assert(url);
    bool generated = false;

    if (!m_database->Working())
        return kError_DatabaseNotWorking;

    PlaylistItem *newtrack;

    MetaData *meta = ReadMetaDataFromDatabase(url);

    if (!meta) {
        newtrack = new PlaylistItem(url);
        m_context->plm->RetrieveMetaDataNow(newtrack);

        MetaData tempdata = (MetaData)(newtrack->GetMetaData());
        WriteMetaDataToDatabase(url, tempdata);

        delete newtrack;

        meta = ReadMetaDataFromDatabase(url);
    }

    if (!meta)
        return kError_DatabaseNotWorking;

    if (m_killGUIDs)
    { 
        meta->SetGUID("");
        WriteMetaDataToDatabase(url, *meta);
    }

    newtrack = new PlaylistItem(url, meta);

    m_catMutex->Acquire();

    if ((meta->Artist().size() == 0) || (meta->Artist() == " ")) {
        vector<PlaylistItem *>::iterator i = m_unsorted->begin();
        for (; i != m_unsorted->end(); i++)
             if ((*i)->URL() == url) {
                 m_catMutex->Release();
                 delete meta;
                 delete newtrack;
                 return kError_DuplicateItem;
             }

        m_unsorted->push_back(newtrack);
        AcceptEvent(new MusicCatalogTrackAddedEvent(newtrack, NULL, NULL));
        m_trackCount++;
    }
    else {
        bool changed = false;
        if (meta->Album() == " " || meta->Album().size() == 0) {
            string unknownstr = string("Unknown");
            meta->SetAlbum(unknownstr);
            changed = true;
        }
        if (meta->Title() == " " || meta->Title().size() == 0) {
            string unknownstr = string("Unknown");
            meta->SetTitle(unknownstr);
            changed = true;
        }
  
        if (changed)
            WriteMetaDataToDatabase(url, *meta);

        bool found_artist = false;
        vector<ArtistList *>::iterator i = m_artistList->begin();
        for (; i != m_artistList->end(); i++) {
            if (CaseCompare(meta->Artist(),(*i)->name)) {
                bool found_album = false;
                found_artist = true;
                vector<AlbumList *> *alList = (*i)->m_albumList;
                vector<AlbumList *>::iterator j = alList->begin();
                for (; j != alList->end(); j++) {
                    if (CaseCompare(meta->Album(),(*j)->name)) {
                        found_album = true;
                       
                        vector<PlaylistItem *> *trList = (*j)->m_trackList;
                        vector<PlaylistItem *>::iterator k = trList->begin();
                        for (; k != trList->end(); k++) {
                            if ((*k)->URL() == url) {
                                delete newtrack;
                                delete meta;  
                                m_catMutex->Release();
                                return kError_DuplicateItem;
                            }
                        }
                        (*j)->m_trackList->push_back(newtrack);
                        AcceptEvent(new MusicCatalogTrackAddedEvent(newtrack, 
                                                                    *i, *j));
                        m_trackCount++;
                        break;
                     }
                }
                if (!found_album) {
                    AlbumList *newalbum = new AlbumList;
                    newalbum->name = meta->Album();
                    newalbum->m_trackList->push_back(newtrack);
                    alList->push_back(newalbum);
                    AcceptEvent(new MusicCatalogTrackAddedEvent(newtrack, (*i), newalbum));
                    break;
                }
            }
        }
        if (!found_artist) {
            ArtistList *newartist = new ArtistList;
            newartist->name = meta->Artist();
            AlbumList *newalbum = new AlbumList;
            newalbum->name = meta->Album();
            newalbum->m_trackList->push_back(newtrack);
            newartist->m_albumList->push_back(newalbum);
            m_artistList->push_back(newartist);
            AcceptEvent(new MusicCatalogTrackAddedEvent(newtrack, newartist, newalbum));
        }
    }
    delete meta;
    m_catMutex->Release();

    return kError_NoErr;
}

Error MusicCatalog::UpdateSong(PlaylistItem *item)
{
    assert(item);

    if (!m_database->Working())
        return kError_DatabaseNotWorking;

    m_inUpdateSong = true;

    Error err = RemoveSong(item->URL().c_str());    

    if (IsError(err))
        return err;
   
    WriteMetaDataToDatabase(item->URL().c_str(), item->GetMetaData());

    m_database->Sync();

    err = AddSong(item->URL().c_str());

    if (IsError(err))
        return err;

    m_inUpdateSong = false;

    m_context->target->AcceptEvent(new MusicCatalogTrackChangedEvent(m_oldItem, m_newItem, m_oldArtist, m_newArtist, m_oldAlbum, m_newAlbum));

    return kError_NoErr;
}

Error MusicCatalog::Remove(const char *url)
{
    assert(url);

    if (!m_database->Working())
        return kError_DatabaseNotWorking;

    char *data = m_database->Value(url);
    if (!data)
        return kError_ItemNotFound;

    Error retvalue = kError_YouScrewedUp;

    if (*data == 'M')
        retvalue = RemoveSong(url);
    else if (*data == 'P')
        retvalue = RemovePlaylist(url);
    else if (*data == 'S')
        retvalue = RemoveStream(url);

    delete [] data;

    return retvalue;
}

Error MusicCatalog::Add(const char *url)
{
    assert(url);

    if (!m_database->Working())
        return kError_DatabaseNotWorking;

    char *data = m_database->Value(url);
    if (!data)
        return kError_ItemNotFound;

    Error retvalue = kError_YouScrewedUp;

    if (*data == 'M')
        retvalue = AddSong(url);
    else if (*data == 'P')
        retvalue = AddPlaylist(url);
    else if (*data == 'S')
        retvalue = AddStream(url);

    delete [] data;

    return retvalue;
}

void MusicCatalog::ClearCatalog()
{
    m_catMutex->Acquire();

    vector<ArtistList *>::iterator a;
    vector<PlaylistItem *>::iterator p;

    for(a = m_artistList->begin(); a != m_artistList->end(); a++)
       delete (*a);
    delete m_artistList;

    for(p = m_unsorted->begin(); p != m_unsorted->end(); p++)
       delete (*p);
    delete m_unsorted;

    delete m_playlists;

    for(p = m_streams->begin(); p != m_streams->end(); p++)
       delete (*p);
    delete m_streams;

    m_trackCount = 0;

    m_artistList = new vector<ArtistList *>;
    m_unsorted = new vector<PlaylistItem *>;
    m_playlists = new vector<string>;
    m_streams = new vector<PlaylistItem *>;
    m_catMutex->Release();
}

Error MusicCatalog::RePopulateFromDatabase()
{
    if (!m_database->Working())
        return kError_DatabaseNotWorking;

    ClearCatalog();
    m_context->target->AcceptEvent(new Event(INFO_MusicCatalogCleared));

    int subversion = m_database->GetSubVersion();
    if (subversion < DATABASE_SUB_VERSION)
    {
        m_killGUIDs = true;
    }

    m_catMutex->Acquire(); 
    m_bSurpressAddMessages = true;
    char *key = m_database->NextKey(NULL);
    Error err = kError_NoErr;

    while (key) {
        err = Add(key);

        if (IsError(err)) { 
             //m_context->target->AcceptEvent(new ErrorMessageEvent("There was an internal error during generation of the Music Catalog"));
             //m_catMutex->Release();
             //return kError_YouScrewedUp;
        }
            
        key = m_database->NextKey(key);
    }
 
    if (subversion < DATABASE_SUB_VERSION)
        m_killGUIDs = false;
    m_database->StoreSubVersion(DATABASE_SUB_VERSION);

    m_catMutex->Release();
    m_bSurpressAddMessages = false;
    return kError_NoErr;
}

void MusicCatalog::SetDatabase(const char *path)
{
    if (m_database)
        delete m_database;

    m_database = new Database(path, METADATABASE_VERSION);

    if (!m_database->Working()) {
        delete m_database;
        m_database = NULL;
    }

    if (m_database) {
        //if (m_database->IsUpgraded())
        //    m_context->target->AcceptEvent(new Event(INFO_DatabaseUpgraded));
        RePopulateFromDatabase();
        if (m_timeout == 0)
            PruneDatabase(true, true);
        Sort();
    }
}

typedef struct PruneThreadStruct {
    MusicCatalog *mc;
    Thread *thread;
    bool sendmessages;
} PruneThreadStruct;

void MusicCatalog::PruneDatabase(bool sendmessages, bool spawn)
{
    if (spawn) {
        Thread *thread = Thread::CreateThread();

        if (thread) {
            PruneThreadStruct *pts = new PruneThreadStruct;

            pts->mc = this;
            pts->sendmessages = sendmessages;
            pts->thread = thread;

            thread->Create(prune_thread_function, pts, true);
        }
    }
    else {
        PruneThread(sendmessages);
    }
}

void MusicCatalog::prune_thread_function(void *arg)
{
    PruneThreadStruct *pts = (PruneThreadStruct *)arg;

    pts->mc->PruneThread(pts->sendmessages);

    delete pts->thread;
    delete pts;
}

void MusicCatalog::PruneThread(bool sendmessages)
{
    m_catMutex->Acquire();
    char *key = m_database->NextKey(NULL);
    struct stat st;

    while (key) {
        if (!strncmp("file://", key, 7)) {
            uint32 length = strlen(key) + 1;
            char *filename = new char[length];

            if (IsntError(URLToFilePath(key, filename, &length))) {
                if (-1 == stat(filename, &st)) {
                    if (sendmessages) {
                        Remove(key);
                        key = NULL;
                    }
                    else {
                        m_database->Remove(key);
                        m_trackCount--;
                        key = NULL;
                    }
                }
            }
            delete [] filename;
        }
        key = m_database->NextKey(key);
    }
    m_catMutex->Release();
}

void MusicCatalog::PruneDirectory(string &directory)
{
    m_catMutex->Acquire();

    char *key = m_database->NextKey(NULL);
    struct stat st;
 
    while (key) {
        if (!strncmp("file://", key, 7)) {
            uint32 length = strlen(key) + 1;
            char *filename = new char[length];
            
            if (IsntError(URLToFilePath(key, filename, &length))) {
                if (!strncmp(directory.c_str(), filename, directory.size())) {
                    if (-1 == stat(filename, &st)) {
                        Remove(key);
                        key = NULL;
                    }
                }
            }
            delete [] filename;
        }
        key = m_database->NextKey(key);
    }
    m_catMutex->Release();
}

typedef struct MusicSearchThreadStruct {
    MusicCatalog *mc;
    vector<string> pathList;
    bool bSendMessages;
    Thread *thread;
} MusicSearchThreadStruct;

void MusicCatalog::SearchMusic(vector<string> &pathList, bool bBrowserMessages)
{
    if (!m_database->Working())
        return;

    Thread *thread = Thread::CreateThread();

    if (thread) {
        MusicSearchThreadStruct *mst = new MusicSearchThreadStruct;

        mst->mc = this;
        mst->pathList = pathList;
        mst->thread = thread;
        mst->bSendMessages = bBrowserMessages;

        thread->Create(musicsearch_thread_function, mst, true);
    }
}

void MusicCatalog::StopSearchMusic(void)
{
    m_exit = true;
}

void MusicCatalog::musicsearch_thread_function(void *arg)
{
    MusicSearchThreadStruct *mst = (MusicSearchThreadStruct *)arg;

    mst->mc->m_mutex->Acquire();

    mst->mc->m_exit = false;
    mst->mc->DoSearchPaths(mst->pathList, mst->bSendMessages);

    if (mst->bSendMessages)
        mst->mc->AcceptEvent(new Event(INFO_SearchMusicDone));

    mst->mc->m_mutex->Release();

    delete mst->thread;
    delete mst;
}

void MusicCatalog::DoSearchPaths(vector<string> &pathList, bool bSendMessages)
{
    vector<string>::iterator i;
    
    if (!bSendMessages)
        m_addImmediately = true;

    for(i = pathList.begin(); i != pathList.end(); i++)
        DoSearchMusic((char *)(*i).c_str(), bSendMessages);

    m_addImmediately = false;
}

void MusicCatalog::DoSearchMusic(char *path, bool bSendMessages)
{
    WIN32_FIND_DATA find;
    HANDLE handle;
    string search = path;

#ifndef WIN32
    if (!strcmp(path, "/dev") || !strcmp(path, "/proc"))
        return;
#endif
 
    if (bSendMessages) 
    {
        string info = string("Searching: ") + search;
        m_context->player->AcceptEvent(new BrowserMessageEvent(info));
    }
   
    if (search[search.size() - 1] != DIR_MARKER)
       search.append(DIR_MARKER_STR);

    search.append("*");
#ifdef WIN32
    search.append(".*");
#endif

    handle = FindFirstFile((char *)search.c_str(), &find);

    if (handle != INVALID_HANDLE_VALUE)
    {
        do
        {
            char *fileExt;
#ifndef WIN32
            if (find.dwFileAttributes & FILE_ATTRIBUTE_SYMLINK)
                continue;
#endif
            if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (!(!strcmp("." , find.cFileName) || !strcmp("..", find.cFileName)))
                {
                    string newDir = path; 
                    if (path[strlen(path) - 1] != DIR_MARKER)
                        newDir.append(DIR_MARKER_STR);
                    newDir.append(find.cFileName);

                    DoSearchMusic((char *)newDir.c_str(), bSendMessages);
                }
            }
            else 
            {
#ifdef WIN32
                string dirtest = path;
                if (path[strlen(path) - 1] != DIR_MARKER)
                    dirtest.append(DIR_MARKER_STR);
                dirtest.append(find.cFileName);

                struct stat stdir;

                stat(dirtest.c_str(), &stdir);
                if (stdir.st_mode & _S_IFDIR) {
                    DoSearchMusic((char *)dirtest.c_str(), bSendMessages);
                    continue;
                }
#endif          
                fileExt = m_context->player->GetExtension(find.cFileName);
                if (fileExt && m_plm->IsSupportedPlaylistFormat(fileExt) && 
                    strcmp("currentlist.m3u", find.cFileName))
                {
                    string file = path;
                    file.append(DIR_MARKER_STR);
                    file.append(find.cFileName);

                    uint32 urlLength = strlen(file.c_str()) + 10;
                    char *url = new char[urlLength];

                    if (IsntError(FilePathToURL(file.c_str(), url, &urlLength)))
                        m_database->Insert(url, "P");

                    if (m_addImmediately)
                        AddPlaylist(url);

                    delete [] url;
                }
                else if (fileExt && 
                         m_context->player->IsSupportedExtension(fileExt))
                {
                    string file = path;
                    file.append(DIR_MARKER_STR);
                    file.append(find.cFileName);

                    char *tempurl = new char[file.length() + 10];
                    uint32 length = file.length() + 10;

                    if (IsError(FilePathToURL(file.c_str(), tempurl, &length)))
                    {
                        delete [] tempurl;
                        delete [] fileExt;

                        continue;
                    }
                   
                    if (!bSendMessages) {
                        MetaData *tdata = ReadMetaDataFromDatabase(tempurl);
                        if (tdata) {
                            delete tdata;
                            delete [] tempurl;
                            delete [] fileExt;

                            continue;
                        }
                    }

                    PlaylistItem *plist = new PlaylistItem(tempurl);
                    m_plm->RetrieveMetaDataNow(plist);

                    MetaData *tempdata = ReadMetaDataFromDatabase(tempurl);
                    if (!tempdata || !m_addImmediately) {
                        MetaData newmdata = (MetaData)plist->GetMetaData();
                        if (tempdata)
                            newmdata.SetGUID(tempdata->GUID());

                        WriteMetaDataToDatabase(tempurl, newmdata);
                    }
                    if (m_addImmediately)
                        AddSong(tempurl);

                    if (tempdata)
                        delete tempdata;
                    delete plist;
                    delete [] tempurl;
                }
                if (fileExt)
                    delete [] fileExt;
            }
        }
        while (FindNextFile(handle, &find) && !m_exit);
        FindClose(handle);
    }
}

void MusicCatalog::WriteMetaDataToDatabase(const char *url, 
                                           const MetaData metadata,
                                           MetadataStorageType type)
{
    if (!m_database->Working())
        return;

    string ost;
    char *num = new char[4096];
    char *temp = new char[1024];
    const char *kDatabaseDelimiter = " ";

    if (type == kTypeStream)
        ost.append("S");
    else
        ost.append("M");
    ost.append(kDatabaseDelimiter);
    ost.append("11");
    ost.append(kDatabaseDelimiter);

    sprintf(num, "%ld%s", (long int)metadata.Artist().size(), kDatabaseDelimiter);
    ost.append(num);
    sprintf(num, "%ld%s", (long int)metadata.Album().size(), kDatabaseDelimiter);
    ost.append(num);
    sprintf(num, "%ld%s", (long int)metadata.Title().size(), kDatabaseDelimiter);
    ost.append(num);
    sprintf(num, "%ld%s", (long int)metadata.Comment().size(), kDatabaseDelimiter);
    ost.append(num);
    sprintf(num, "%ld%s", (long int)metadata.Genre().size(), kDatabaseDelimiter);
    ost.append(num);
    sprintf(temp, "%ld", (long int)metadata.Year());
    sprintf(num, "%ld%s", (long int)strlen(temp), kDatabaseDelimiter);
    ost.append(num);
    sprintf(temp, "%ld", (long int)metadata.Track());
    sprintf(num, "%ld%s", (long int)strlen(temp), kDatabaseDelimiter);
    ost.append(num);
    sprintf(temp, "%ld", (long int)metadata.Time());
    sprintf(num, "%ld%s", (long int)strlen(temp), kDatabaseDelimiter);
    ost.append(num);
    sprintf(temp, "%ld", (long int)metadata.Size());
    sprintf(num, "%ld%s", (long int)strlen(temp), kDatabaseDelimiter);
    ost.append(num);
    sprintf(temp, "%ld", (long int)metadata.PlayCount());
    sprintf(num, "%ld%s", (long int)strlen(temp), kDatabaseDelimiter);
    ost.append(num);
    sprintf(num, "%ld%s", (long int)metadata.GUID().size(), kDatabaseDelimiter);
    ost.append(num);

    ost.append(metadata.Artist());
    ost.append(metadata.Album());
    ost.append(metadata.Title());
    ost.append(metadata.Comment());
    ost.append(metadata.Genre());
    sprintf(temp, "%ld", (long int)metadata.Year());
    ost.append(temp);
    sprintf(temp, "%ld", (long int)metadata.Track());
    ost.append(temp);
    sprintf(temp, "%ld", (long int)metadata.Time());
    ost.append(temp);
    sprintf(temp, "%ld", (long int)metadata.Size());
    ost.append(temp);
    sprintf(temp, "%ld", (long int)metadata.PlayCount());
    ost.append(temp);
    ost.append(metadata.GUID());
    ost.append("\0");

    m_database->Insert(url, (char *)ost.c_str());

    delete [] num;
    delete [] temp;
}

MetaData *MusicCatalog::ReadMetaDataFromDatabase(const char *url)
{
    if (!m_database->Working())
        return NULL;

    char *dbasedata = m_database->Value(url);

    if (!dbasedata)
        return NULL;

    MetaData *metadata = new MetaData();
    char *value = dbasedata + 2;

    uint32 numFields = 0;
    int offset = 0;

    sscanf(value, "%lu%n", (long unsigned int *)&numFields, &offset);
    uint32* fieldLength =  new uint32[numFields];

    int temp;

    for(uint32 i = 0; i < numFields; i++)
    {
        sscanf(value + offset, " %lu %n", (long unsigned int *)&fieldLength[i],
	                                  &temp);

        if (i == numFields - 1) {
            char intholder[10];
            sprintf(intholder, "%lu", (long unsigned int)fieldLength[i]);
            offset += strlen(intholder) + 1;
        }
        else
            offset += temp;
    }

    string data = value;
    data.erase(0, offset);

    uint32 count = 0;
    string substring;

    for(uint32 j = 0; j < numFields; j++)
    {
        if (fieldLength[j] == 0) 
            continue;

        substring = data.substr(count, fieldLength[j]);

        switch(j)
        {
            case 0:
                metadata->SetArtist(substring);
                break;
            case 1:
                metadata->SetAlbum(substring);
                break;
            case 2:
                metadata->SetTitle(substring);
                break;
            case 3:
                metadata->SetComment(substring);
                break;
            case 4:
                metadata->SetGenre(substring);
                break;
            case 5:
                metadata->SetYear(atoi(substring.c_str()));
                break;
            case 6:
                metadata->SetTrack(atoi(substring.c_str()));
                break;
            case 7:
                metadata->SetTime(atoi(substring.c_str()));
                break;
            case 8:
                metadata->SetSize(atoi(substring.c_str()));
                break;
            case 9:
                metadata->SetPlayCount(atoi(substring.c_str()));
                break;
            case 10: 
                metadata->SetGUID(substring);
                break;
            default:
                break;

        }

        count += fieldLength[j];
    }

    delete [] fieldLength;
    delete [] dbasedata;

    return metadata;
}

Error MusicCatalog::AcceptEvent(Event *e)
{
    switch (e->Type()) {
        case INFO_MusicCatalogTrackRemoved: {
            if (m_inUpdateSong) {
                MusicCatalogTrackRemovedEvent *mctr = 
                                             (MusicCatalogTrackRemovedEvent *)e;
                m_oldItem = (PlaylistItem *)mctr->Item();
                m_oldArtist = (ArtistList *)mctr->Artist();
                m_oldAlbum = (AlbumList *)mctr->Album();
     
                delete e;
            }
            else 
                m_context->target->AcceptEvent(e);
            break; }
        case INFO_MusicCatalogTrackAdded: {
            if (m_inUpdateSong) {
                MusicCatalogTrackAddedEvent *mcta =
                                             (MusicCatalogTrackAddedEvent *)e;
                m_newItem = (PlaylistItem *)mcta->Item();
                m_newArtist = (ArtistList *)mcta->Artist();
                m_newAlbum = (AlbumList *)mcta->Album();

                delete e;
            }
            else if (!m_bSurpressAddMessages)
                m_context->target->AcceptEvent(e);
            else
                delete e;
            break; }
        case INFO_SearchMusicDone: {
            m_context->target->AcceptEvent(new Event(INFO_MusicCatalogRegenerating));
            m_database->Sync();
            string info = "Pruning the Music Catalog Database...";
            m_context->target->AcceptEvent(new BrowserMessageEvent(info));
            PruneDatabase();
            info = "Regenerating the Music Catalog Database...";
            m_context->target->AcceptEvent(new BrowserMessageEvent(info));
            RePopulateFromDatabase();
            info = "Sorting the Music Catalog Database...";
            m_context->target->AcceptEvent(new BrowserMessageEvent(info));
            Sort();
            m_context->target->AcceptEvent(new Event(INFO_SearchMusicDone));
            m_context->target->AcceptEvent(new Event(INFO_MusicCatalogDoneRegenerating));
            delete e;
            break;
        }
        case INFO_PrefsChanged: {
            int32 watchtimeout = 0;
            m_context->prefs->GetPrefInt32(kWatchThisDirTimeoutPref, &watchtimeout);
            if (m_timeout != watchtimeout) {
                m_timeout = watchtimeout;
                if (m_timeout != 0) 
                    m_context->timerManager->SetTimer(m_watchTimer, m_timeout);
                else 
                    m_context->timerManager->StopTimer(m_watchTimer);
            }
            break;
        }
    }
    return kError_NoErr; 
}

void MusicCatalog::watch_timer(void *arg) 
{
    MusicCatalog *cat = (MusicCatalog*)arg;
    cat->WatchTimer();
}

void MusicCatalog::WatchTimer(void)
{
    bool welcome;
    m_context->prefs->GetPrefBoolean(kWelcomePref, &welcome);
    if (welcome)
        return;

#ifndef DEBUG_MUTEXES
    if (!m_timerMutex->Acquire(0)) {
#else
    if (!m_timerMutex->__Acquire(__FILE__, __LINE__, 0)) {
#endif
        return;
    }
    char *watchDir = new char[_MAX_PATH];
    uint32 length = _MAX_PATH;

    m_context->prefs->GetPrefString(kWatchThisDirectoryPref, watchDir, &length);

    vector<string> searchPaths;

    char *temp = strtok(watchDir, ";");
    do {
        string dir = temp;
        searchPaths.push_back(dir);
        PruneDirectory(dir);
    } while ((temp = strtok(NULL, ";")));

    SearchMusic(searchPaths, false);

    delete [] watchDir;

    m_timerMutex->Release();
}
