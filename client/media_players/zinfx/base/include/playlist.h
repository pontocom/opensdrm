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
	
	$Id: playlist.h,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PLAYLIST_H_
#define INCLUDED_PLAYLIST_H_

#include <assert.h>
#include <string>
#include <vector>
#include <functional>

using namespace std;

#include "config.h"
#include "facontext.h"

#include "errors.h"
#include "mutex.h"
#include "thread.h"
#include "metadata.h"
#include "playlistformat.h"
#include "portabledevice.h"
#include "registry.h"
#include "plmevent.h"
#include "undomanager.h"

typedef enum {
    kPlaylistSortKey_FirstKey,
    kPlaylistSortKey_Artist = kPlaylistSortKey_FirstKey,
    kPlaylistSortKey_Album,
    kPlaylistSortKey_Title,
    kPlaylistSortKey_Year,
    kPlaylistSortKey_Track,
    kPlaylistSortKey_Genre,
    kPlaylistSortKey_Time,
    kPlaylistSortKey_Location,
    kPlaylistSortKey_FileName,
    kPlaylistSortKey_Comment,
    kPlaylistSortKey_LastKey,
    kPlaylistSortKey_Random // not used with normal sort function

} PlaylistSortKey;

typedef enum {
    kPlaylistSortType_Ascending,
    kPlaylistSortType_Descending
} PlaylistSortType;

typedef enum {
    kPlaylistKey_FirstKey,
    kPlaylistKey_MasterPlaylist = kPlaylistKey_FirstKey,
    kPlaylistKey_ExternalPlaylist,
    kPlaylistKey_PortablePlaylist,
    kPlaylistKey_LastKey

} PlaylistKey;

typedef enum {
    kPlaylistMode_RepeatNone = 0,
    kPlaylistMode_RepeatOne  = 1,
    kPlaylistMode_RepeatAll  = 2

} RepeatMode;

#define kInvalidIndex 0xFFFFFFFF

typedef enum {
    kPlaylistItemState_Normal,
    kPlaylistItemState_RetrievingMetaData,
    kPlaylistItemState_Delete

} PlaylistItemState;

class PlaylistItem {

 public:
    PlaylistItem():m_state(kPlaylistItemState_Normal){}

    PlaylistItem(const char* url, const MetaData* metadata = NULL)
    {
        assert(url);

        if(url)
            SetURL(url);

        if(metadata)
            SetMetaData(metadata);

        m_state = kPlaylistItemState_Normal;
    }

    virtual ~PlaylistItem() {}

    Error SetMetaData(const MetaData* metadata)
    {
        Error result = kError_InvalidParam;
        assert(metadata);

        if(metadata)
        {
            result = kError_NoErr;

            m_metadata = *metadata;
        }

        return result;
    }

    const MetaData& GetMetaData() const { return m_metadata; }

    Error SetURL(const string &url) { m_url = url; return kError_NoErr;}
    Error GetURL(char* buf, uint32* len) { return SetBuffer(buf, m_url.c_str(), len); }
    const string& URL() const { return m_url; }

    Error SetWebURL(const string &webUrl) { m_webUrl = webUrl; return kError_NoErr;}
    Error GetWebURL(char* buf, uint32* len) { return SetBuffer(buf, m_webUrl.c_str(), len); }
    const string& WebURL() const { return m_webUrl; }

    void SetState(PlaylistItemState state) { m_state = state; }
    PlaylistItemState GetState() const { return m_state; }

    bool operator==(const PlaylistItem& item) const
    {
        bool result = false;
        
        result = ( m_metadata == item.m_metadata &&
                   m_url == item.m_url);

        return result;
    }

    bool operator!=(const PlaylistItem& item) const
    {
	    return !(*this == item);
    }

 protected:
    Error SetBuffer(char* dest, const char* src, uint32* len)
    {
        Error result = kError_InvalidParam;

        assert(dest);
        assert(src);
        assert(len);

        if(dest && src)
        {
            uint32 srclen = strlen(src) + 1;

            if(*len >= srclen)
            {
                strcpy(dest, src);
                result = kError_NoErr;
            }
            else
            {
                result = kError_BufferTooSmall;
            }

            *len = srclen;
        }

        return result;
    }

 private:
    MetaData m_metadata;
    string m_url, m_webUrl;
    PlaylistItemState m_state;
};

class PlaylistItemSort : public binary_function<PlaylistItem*, PlaylistItem*, bool> {

 public:
    PlaylistItemSort(PlaylistSortKey sortKey, 
                     PlaylistSortType sortType = kPlaylistSortType_Ascending) : 
                     m_sortKey(sortKey), m_sortType(sortType) { }

    bool operator() (PlaylistItem* item1, PlaylistItem* item2) const;
   
 private:
    PlaylistSortKey m_sortKey;
    PlaylistSortType m_sortType;
};

class MetaDataSort : public binary_function<PlaylistItem*, PlaylistItem*, bool> {

 public:
    MetaDataSort() { }

    bool operator() (MetaDataFormat* item1, MetaDataFormat* item2) const;
};

class PlaylistManager {

 public:
    PlaylistManager(FAContext* context);
    virtual ~PlaylistManager();

    Error SetCurrentItem(PlaylistItem* item);
    PlaylistItem* GetCurrentItem();
    
    Error SetCurrentIndex(uint32 index);
    uint32 GetCurrentIndex();

    Error GotoNextItem(bool userAction = false);
    Error GotoPreviousItem(bool userAction = false);

    bool HasAnotherItem();

    Error SetShuffleMode(bool shuffle);
    bool GetShuffleMode() const {return m_shuffle;}
    Error SetRepeatMode(RepeatMode mode);
    RepeatMode GetRepeatMode() const {return m_repeatMode;}
    Error ToggleRepeatMode();
    Error ToggleShuffleMode();

    // Functions for undoing your actions
    bool CanUndo();
    bool CanRedo();

    void Undo();
    void Redo();

    // Functions for adding items to playlist       
    Error AddItem(const char* url);
    Error AddItem(const char* url, uint32 index);
    Error AddItem(const string& url);
    Error AddItem(const string& url, uint32 index);
    Error AddItems(const vector<string>& urls);
    Error AddItems(const vector<string>& urls, uint32 index);
    Error AddItem(PlaylistItem* item, bool queryForMetaData = true);
    Error AddItem(PlaylistItem* item, uint32 index, bool queryForMetaData = true);
    Error AddItems(vector<PlaylistItem*>* list, bool queryForMetaData = true);
    Error AddItems(vector<PlaylistItem*>* list, uint32 index, bool queryForMetaData = true);

    // Functions for removing items from playlist
    Error RemoveItem(PlaylistItem* item);
    Error RemoveItem(uint32 index);
    Error RemoveItems(uint32 index, uint32 count);
    Error RemoveItems(vector<PlaylistItem*>* items);
    Error RemoveAll();

    // Functions for moving items around
    Error SwapItems(uint32 index1, uint32 index2);
    Error SwapItems(PlaylistItem* item1, PlaylistItem* item2);
    Error MoveItem(PlaylistItem* item, uint32 index);
    Error MoveItem(uint32 oldIndex, uint32 newIndex);
    Error MoveItems(vector<PlaylistItem*>* items, uint32 index);
    Error MoveItems(vector<uint32>* items, uint32 index);

    // Functions for updating
    // This function searches the items in the playlist
    // and updates the metadata if the tracks are the
    // same (matched based on URL)
    Error UpdateTrackMetaData(PlaylistItem* updatedTrack, bool writeToDisk = false);

    // Functions for sorting
    Error Sort(PlaylistSortKey key, PlaylistSortType type = kPlaylistSortType_Ascending);
    PlaylistSortKey GetPlaylistSortKey() const;
    PlaylistSortType GetPlaylistSortType() const;

    // Which playlist are we dealing with for purposes of editing:
    // 1) Master Playlist - list of songs to play
    // 2) Secondary Playlist - a playlist that we want to edit
    //      - External playlist
    //      - Portable playlist

    Error SetActivePlaylist(PlaylistKey key);
    PlaylistKey GetActivePlaylist() const;
    Error SetExternalPlaylist(char* url);
    Error GetExternalPlaylist(char* url, uint32* length);
    Error SetPortablePlaylist(DeviceInfo* device, 
                              PLMCallBackFunction function = NULL,
                              void* cookie = NULL);
    Error GetPortablePlaylist(DeviceInfo* device);

    // External playlist support
    bool  IsSupportedPlaylistFormat(const char *extension);

    Error GetSupportedPlaylistFormats(PlaylistFormatInfo* format, uint32 index);
    Error ReadPlaylist(const char* url, 
                       vector<PlaylistItem*>* items = NULL, 
                       PLMCallBackFunction function = NULL,
                       void* cookie = NULL);

    Error WritePlaylist(const char* url, 
                        PlaylistFormatInfo* format, 
                        vector<PlaylistItem*>* items = NULL,
                        PLMCallBackFunction function = NULL,
                        void* cookie = NULL);

    Error WritePlaylist(const char* url,
                        vector<PlaylistItem*>* items = NULL,
                        PLMCallBackFunction function = NULL,
                        void* cookie = NULL);

    // Portable player communication
    Error GetSupportedPortables(DeviceInfo* device, uint32 index);
    bool IsPortableAvailable(DeviceInfo* device);

    Error GetDeviceInfo(DeviceInfo* device);

    Error InitializeDevice(DeviceInfo* device, 
                           PLMCallBackFunction function = NULL,
                           void* cookie = NULL);

    Error ReadPortablePlaylist(DeviceInfo* device, 
                               vector<PlaylistItem*>* items = NULL,
                               PLMCallBackFunction function = NULL,
                               void* cookie = NULL);
    Error SyncPortablePlaylist(DeviceInfo* device, 
                               PLMCallBackFunction function = NULL,
                               void* cookie = NULL);

    Error DownloadItemFromPortable(DeviceInfo* device,
                                   PlaylistItem* item,
                                   const char* url,
                                   PLMCallBackFunction function = NULL,
                                   void* cookie = NULL);

    uint32 Time();

    // Utility Functions
    bool            IsEmpty();
    uint32          CountItems();
    PlaylistItem*   ItemAt(uint32 index);
	uint32          IndexOf(const PlaylistItem* item);
    bool            HasItem(const PlaylistItem* item);

    void RetrieveMetaData(PlaylistItem* item);
    void RetrieveMetaData(vector<PlaylistItem*>* list);

    /* and for the truely selfish function that wants its data immediately,
     * w/o spawning a thread, w/o sending messages, and no dbase lookup. 
     */
    void RetrieveMetaDataNow(PlaylistItem* item);

 protected:
    inline uint32 CheckIndex(uint32 index);
    uint32 InternalIndexOf(vector<PlaylistItem*>* list, const PlaylistItem* item);

    void AddItemToShuffleList(PlaylistItem* item);
    void AddItemsToShuffleList(vector<PlaylistItem*>* list);

    static void metadata_thread_function(void* arg);
    void MetaDataThreadFunction(vector<PlaylistItem*>* list);

    void InternalSetCurrentIndex(uint32 index);


 private:
    void ShuffleIt(vector<PlaylistItem *> *toBeShuffled = NULL);
    
    FAContext* m_context;

    vector<PlaylistItem*>   m_masterList;
    vector<PlaylistItem*>   m_externalList;
    vector<PlaylistItem*>   m_portableList;
    vector<PlaylistItem*>*  m_activeList; 
    vector<PlaylistItem*>   m_shuffleList;

    uint32           m_current;
    bool             m_shuffle;
    RepeatMode       m_repeatMode;
    PlaylistKey      m_playlistKey;
    PlaylistSortKey  m_sortKey;
    PlaylistSortType m_sortType;

    string      m_externalPlaylist;
    DeviceInfo  m_portableDevice;

    Mutex       m_mutex;

    Registry m_playlistRegistry;
    Registry m_portableRegistry;
    Registry m_metadataRegistry;

    vector<MetaDataFormat*>         m_metadataFormats;
    vector<PlaylistFormatInfo*>     m_playlistFormats;
    vector<DeviceInfo*>             m_portablePlayers;

    UndoManager m_undo;

    uint32 m_time;

};

#endif // _PLAYLIST_H_










