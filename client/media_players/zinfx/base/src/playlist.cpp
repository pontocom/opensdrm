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
	
	$Id: playlist.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786) 
#endif

#include <assert.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>

using namespace std;

#ifdef __QNX__
#include <strings.h>
#endif

#include "config.h"

#include "playlist.h"
#include "errors.h"
#include "mutex.h"
#include "thread.h"
#include "metadata.h"
#include "registrar.h"
#include "event.h"
#include "eventdata.h"
#include "musiccatalog.h"

class UndoAdd : public UndoItem {
 public:
            UndoAdd(PlaylistManager* plm, const string& url, uint32 index);
    virtual ~UndoAdd();

    virtual void Undo();
    virtual void Redo();
 private:
    PlaylistManager* m_plm;
    string m_url;
    uint32 m_index;
};

class UndoAddMulti : public UndoItem {
 public:
            UndoAddMulti(PlaylistManager* plm, vector<string>& urls, uint32 index);
    virtual ~UndoAddMulti();

    virtual void Undo();
    virtual void Redo();
 private:
    PlaylistManager* m_plm;
    vector<string> m_urls;
    uint32 m_index;
};

class UndoRemove : public UndoItem {
 public:
            UndoRemove(PlaylistManager* plm, const string& url, uint32 index);
    virtual ~UndoRemove();

    virtual void Undo();
    virtual void Redo();
 private:
    PlaylistManager* m_plm;
    string m_url;
    uint32 m_index;
};

class UndoMove : public UndoItem {
 public:
            UndoMove(PlaylistManager* plm, uint32 oldIndex, uint32 newIndex);
    virtual ~UndoMove();

    virtual void Undo();
    virtual void Redo();
 private:
    PlaylistManager* m_plm;
    uint32 m_newIndex, m_oldIndex;
};

#ifndef lstrcmpi
#define lstrcmpi strcasecmp
#endif

// Function object used for sorting PlaylistItems in PlaylistManager
bool PlaylistItemSort::operator() (PlaylistItem* item1, 
                                   PlaylistItem* item2) const
{
    bool result = true;

    assert(item1);
    assert(item2);

    if(item1 && item2)
    {
        MetaData m1 = item1->GetMetaData();
        MetaData m2 = item2->GetMetaData();

        switch(m_sortKey)
        {
            case kPlaylistSortKey_Artist:
            {
                //result = m1.Artist() < m2.Artist();
                result = (lstrcmpi(m1.Artist().c_str(), m2.Artist().c_str()) < 0);
                //result = (lstrcmpi(m1.Comment().c_str(), m2.Comment().c_str()) < 0);
                break;
            }

            case kPlaylistSortKey_Album:
            {
                //result = m1.Album() < m2.Album();
                result = (lstrcmpi(m1.Album().c_str(), m2.Album().c_str()) < 0);
                break;
            }

            case kPlaylistSortKey_Title:
            {
                //result = m1.Title() < m2.Title();
                result = (lstrcmpi(m1.Title().c_str(), m2.Title().c_str()) < 0);
                break;
            }

            case kPlaylistSortKey_Comment:
            {
                result = (lstrcmpi(m1.Comment().c_str(), m2.Comment().c_str()) < 0);
                break;
            }

            case kPlaylistSortKey_Year:
            {
                result = m1.Year() < m2.Year();
                break;
            }

            case kPlaylistSortKey_Track:
            {
                result = m1.Track() < m2.Track();
                break;
            }

            case kPlaylistSortKey_Genre:
            {
                //result = m1.Genre() < m2.Genre();
                result = (lstrcmpi(m1.Genre().c_str(), m2.Genre().c_str()) < 0);
                break;
            }

            case kPlaylistSortKey_Time:
            {
                result = m1.Time() < m2.Time();
                break;
            }

            case kPlaylistSortKey_Location:
            {
                result = item1->URL() < item2->URL();
                break;
            }

            case kPlaylistSortKey_FileName:
            {
                string file1, file2;
                string::size_type pos;

                pos = item1->URL().find_last_of('/');
                file1 = item1->URL().substr(pos + 1);

                pos = item2->URL().find_last_of('/');
                file2 = item2->URL().substr(pos + 1);

                result = (lstrcmpi(file1.c_str(), file2.c_str()) < 0);
                break;
            }

            default:
            {
                cerr << "Whoa! Why are we here?" << endl;
                break;
            }

        }
    }

    /*if(m_sortType == kPlaylistSortType_Descending)
    {
        result = !result;
    }*/

    return result;
}

// Function object used for ordering MetaDataFormats in PlaylistManager
bool MetaDataSort::operator() (MetaDataFormat* item1, 
                               MetaDataFormat* item2) const
{
    bool result = true;

    assert(item1);
    assert(item2);

    if(item1 && item2)
    {
        result = item1->Order() < item2->Order();
    }

    return result;
}


// Implementation of the PlaylistManager Class
PlaylistManager::PlaylistManager(FAContext* context)
{
    m_context = context;
    m_activeList = &m_masterList;
    m_playlistKey = kPlaylistKey_MasterPlaylist;
    m_current = kInvalidIndex;
    m_shuffle = false;
    m_repeatMode = kPlaylistMode_RepeatNone;
    m_sortKey = kPlaylistSortKey_Random;
    m_sortType = kPlaylistSortType_Ascending;
    m_time = 0;

    m_context->prefs->GetPrefBoolean(kPlaylistShufflePref, &m_shuffle);
    m_context->prefs->GetPrefInt32(kPlaylistRepeatPref, (int32*)&m_repeatMode);

    Registrar registrar;

    registrar.SetSubDir("plugins");
    registrar.SetSearchString("*.plf");
    registrar.InitializeRegistry(&m_playlistRegistry, context->prefs);

    registrar.SetSearchString("*.mdf");
    registrar.InitializeRegistry(&m_metadataRegistry, context->prefs);

    registrar.SetSearchString("*.ppp");
    registrar.InitializeRegistry(&m_portableRegistry, context->prefs);

    RegistryItem* module = NULL;
    MetaDataFormat* mdf = NULL;
    int32 i = 0;

    while((module = m_metadataRegistry.GetItem(i++)))
    {
        mdf = (MetaDataFormat*) module->InitFunction()(m_context);

        if(mdf)
        {
            m_metadataFormats.push_back(mdf);
        }
    }

    sort(m_metadataFormats.begin(), m_metadataFormats.end(), MetaDataSort());

    i = 0;
    PlaylistFormat* plf = NULL;

    while((module = m_playlistRegistry.GetItem(i++)))
    {
        plf = (PlaylistFormat*) module->InitFunction()(m_context);

        if(plf)
        {
            PlaylistFormatInfo plfi;

            uint32 index = 0;

            // error != kError_NoMoreFormats
            while(IsntError(plf->GetSupportedFormats(&plfi, index++)))
            {
                plfi.SetRef(plf);
                m_playlistFormats.push_back(new PlaylistFormatInfo(plfi));
            }
        }
    }

    i = 0;
    PortableDevice* pd = NULL;

    while((module = m_portableRegistry.GetItem(i++)))
    {
        pd = (PortableDevice*) module->InitFunction()(m_context);

        if(pd)
        {
            DeviceInfo di;

            uint32 index = 0;
            // error != kError_NoMoreDevices
            while(IsntError(pd->GetSupportedDevices(&di, index++)))
            {
                di.SetRef(pd);
                di.SetPluginName(module->Name());
                m_portablePlayers.push_back(new DeviceInfo(di));
            }
        }
    }
}

PlaylistManager::~PlaylistManager()
{
    uint32 index = 0;
    uint32 size = 0;
    PlaylistItem* item = NULL;
    vector<MetaDataFormat *>::iterator i;
    vector<PlaylistFormatInfo *>::iterator j;
    //uint32 count = 0;

    m_mutex.Acquire();

    size = m_masterList.size();

    for(index = 0; index < size; index++)
    {
        item = m_masterList[index];

        if(item)
        {
            // if the metadata thread is still accessing this item
            // we don't wanna delete the item  out from under it.
            // instead we set a flag and let the metadata thread
            // clean up when it returns.
            if(item->GetState() == kPlaylistItemState_RetrievingMetaData)
            {
                item->SetState(kPlaylistItemState_Delete);  
            }
            else
            {
                delete item;
            }
        }
    }
    
    size = m_externalList.size();

    for(index = 0; index < size; index++)
    {
        item = m_externalList[index];

        if(item)
        {
            // if the metadata thread is still accessing this item
            // we don't wanna delete the item  out from under it.
            // instead we set a flag and let the metadata thread
            // clean up when it returns.
            if(item->GetState() == kPlaylistItemState_RetrievingMetaData)
            {
                item->SetState(kPlaylistItemState_Delete);  
            }
            else
            {
                delete item;
            }
        }
    }

    size = m_portableList.size();

    for(index = 0; index < size; index++)
    {
        item = m_portableList[index];

        if(item)
        {
            // if the metadata thread is still accessing this item
            // we don't wanna delete the item  out from under it.
            // instead we set a flag and let the metadata thread
            // clean up when it returns.
            if(item->GetState() == kPlaylistItemState_RetrievingMetaData)
            {
                item->SetState(kPlaylistItemState_Delete);  
            }
            else
            {
                delete item;
            }
        }
    }

    for(j = m_playlistFormats.begin(); j != m_playlistFormats.end(); j++)
        delete (*j);

    size = m_portablePlayers.size();

    for(index = 0; index < size; index++)
    {
        delete m_portablePlayers[index]->GetRef();
        delete m_portablePlayers[index];
    }

    for(i = m_metadataFormats.begin(); i != m_metadataFormats.end(); i++)
        delete (*i);

    m_mutex.Release();

    m_context->prefs->SetPrefBoolean(kPlaylistShufflePref, m_shuffle);
    m_context->prefs->SetPrefInt32(kPlaylistRepeatPref, m_repeatMode);
}

void PlaylistManager::ShuffleIt(vector<PlaylistItem *> *toBeShuffled)
{
    if (!toBeShuffled)
        toBeShuffled = &m_shuffleList;

    int max = toBeShuffled->size();
    vector<PlaylistItem *> tempShuffled;

    srand((unsigned int)time(NULL));
    
    int i;
    vector<bool> usedList;
    for (i = 0; i < max; i++)
    {
        usedList.push_back(false);
    }

    bool used = true;
    int  index = 0;
    int  lastindex = 0;

    for (i = 0; i < max; i++) 
    {
        while (used)
        {
            index = (int)((double)rand() / (RAND_MAX + 1.0) * max);
            if (usedList[index] == false)
                used = false;
            if (max - i > 50 && abs(index - lastindex) < 10)
                used = true;
        } 
        usedList[index] = true;
        PlaylistItem *dupe = (*toBeShuffled)[index];
        tempShuffled.push_back(dupe);
        used = true;
        lastindex = index;
    }

    toBeShuffled->erase(toBeShuffled->begin(), toBeShuffled->end());
    vector<PlaylistItem *>::iterator iter = tempShuffled.begin();
    for (; iter != tempShuffled.end(); iter++) 
        toBeShuffled->push_back(*iter);
}

Error PlaylistManager::SetCurrentItem(PlaylistItem* item)
{
    return SetCurrentIndex(IndexOf(item));
}

PlaylistItem*  PlaylistManager::GetCurrentItem()
{
    PlaylistItem* result = NULL;
    m_mutex.Acquire();
    
    if(m_masterList.size())
    {
        if(m_shuffle)
            result = m_shuffleList[m_current];
        else
            result = m_masterList[m_current];
    }
   
    m_mutex.Release();
    return result;
}

Error PlaylistManager::SetCurrentIndex(uint32 index)
{
   m_mutex.Acquire();
   index = CheckIndex(index);
   if(m_shuffle)
      index = InternalIndexOf(&m_shuffleList, ItemAt(index));
   InternalSetCurrentIndex(index);
   m_mutex.Release();
   return kError_NoErr;
}

void PlaylistManager::InternalSetCurrentIndex(uint32 index)
{
    m_current = index;
    if (index != kInvalidIndex) 
        m_context->target->AcceptEvent(new PlaylistCurrentItemInfoEvent(GetCurrentItem(), this));
}

uint32 PlaylistManager::GetCurrentIndex()
{
    uint32 result = kInvalidIndex;

    if(m_masterList.size())
    {
        if(m_shuffle)
            result = IndexOf(m_shuffleList[m_current]);
        else
            result = m_current;
    }

    return result;
}


Error PlaylistManager::GotoNextItem(bool userAction)
{
    m_mutex.Acquire();

    Error result = kError_NoErr;
    uint32 count = m_masterList.size();
    uint32 index = m_current;
    
    if(count)
    {
        if(!(kPlaylistMode_RepeatOne == m_repeatMode) || userAction)
        {
            index++;

            if( (index >= count) && 
                (m_repeatMode == kPlaylistMode_RepeatAll || userAction))
            {
                index = 0;

                if(m_shuffle)
                {
                    ShuffleIt();
                }
            }
            else if(index >= count)
            {
                index = m_current;
            }
        }

        InternalSetCurrentIndex(index);
    }
    else
    {
        m_current = kInvalidIndex;
    }

    m_mutex.Release();
    return result;
}

Error PlaylistManager::GotoPreviousItem(bool userAction)
{
    m_mutex.Acquire();

    Error result = kError_NoErr;
    uint32 count = m_masterList.size();
    uint32 index = m_current;
    
    if(count)
    {
        if(!(kPlaylistMode_RepeatOne == m_repeatMode) || userAction)
        {
            if( (index == 0) && 
                (m_repeatMode == kPlaylistMode_RepeatAll || userAction))
            {
                index = count - 1;

                if(m_shuffle)
                {
                    ShuffleIt();
                }
            }
            else if(index != 0)
            {
                index--;
            }
        }

        InternalSetCurrentIndex(index);
    }
    else
    {
        m_current = kInvalidIndex;
    }

    m_mutex.Release();

    return result;
}

bool PlaylistManager::HasAnotherItem()
{
    m_mutex.Acquire();
    bool result = false;

    if(m_repeatMode == kPlaylistMode_RepeatOne || 
        m_repeatMode == kPlaylistMode_RepeatAll)
    {
        result = true;
    }
    else
    {
        result = (m_current < m_masterList.size() - 1);
    }

    m_mutex.Release();
    return result;
}

Error PlaylistManager::SetShuffleMode(bool shuffle)
{
    m_mutex.Acquire();

    const PlaylistItem* currentItem = GetCurrentItem();

    m_shuffle = shuffle;

    if(shuffle)
    {
         ShuffleIt();
        if(currentItem)
            m_current = InternalIndexOf(&m_shuffleList, currentItem);
    }
    else
    {
        if(currentItem)
            m_current = IndexOf(currentItem);
    }

    m_context->target->AcceptEvent(new PlaylistShuffleEvent(m_shuffle, this));

    m_mutex.Release();

    return kError_NoErr;
}

Error PlaylistManager::SetRepeatMode(RepeatMode mode)
{
    m_repeatMode = mode;
    m_context->target->AcceptEvent(new PlaylistRepeatEvent(m_repeatMode, this));
    return kError_NoErr;
}

Error PlaylistManager::ToggleRepeatMode()
{
    Error result = kError_NoErr;

    if(m_repeatMode == kPlaylistMode_RepeatNone)
        result = SetRepeatMode(kPlaylistMode_RepeatOne);
    else if(m_repeatMode == kPlaylistMode_RepeatOne)
        result = SetRepeatMode(kPlaylistMode_RepeatAll);
    else if(m_repeatMode == kPlaylistMode_RepeatAll)
        result = SetRepeatMode(kPlaylistMode_RepeatNone);

    return result;
}

Error PlaylistManager::ToggleShuffleMode()
{
    return SetShuffleMode(!m_shuffle);
}


// Functions for adding items to playlist       
Error PlaylistManager::AddItem(const char* url)
{
    Error result = kError_InvalidParam;

    assert(url);

    if(url)
    {
        result = ReadPlaylist(url);

        if(IsError(result))
        {
            result = kError_OutOfMemory;

            PlaylistItem* item = new PlaylistItem(url);

            if(item)
            {
                result = AddItem(item, true);
            }
        }
    }

    return result;
}

Error PlaylistManager::AddItem(const char* url, uint32 index)
{
    Error result = kError_InvalidParam;

    assert(url);

    if(url)
    {
        result = ReadPlaylist(url);

        if(IsError(result))
        {
            result = kError_OutOfMemory;

            PlaylistItem* item = new PlaylistItem(url);

            if(item)
            {
                result = AddItem(item, index, true);
            }
        }
    }

    return result;
}

Error PlaylistManager::AddItem(const string& url)
{
    Error result = kError_InvalidParam;

    result = AddItem(url.c_str());

    return result;
}

Error PlaylistManager::AddItem(const string& url, uint32 index)
{
    Error result = kError_InvalidParam;

    result = AddItem(url.c_str(), index);

    return result;
}

Error PlaylistManager::AddItems(const vector<string>& urls)
{
    Error result = kError_InvalidParam;
    vector<string>::const_iterator i;
    vector<PlaylistItem*> list;

    for(i = urls.begin(); i != urls.end(); i++)
    {
        result = ReadPlaylist((*i).c_str());

        if(IsError(result))
        {
            result = kError_OutOfMemory;

            PlaylistItem* item = new PlaylistItem((*i).c_str());

            if(item)
            {
                list.push_back(item);
            }
        }
    }

    result = AddItems(&list);

    return result;
}

Error PlaylistManager::AddItems(const vector<string>& urls, uint32 index)
{
    Error result = kError_InvalidParam;
    vector<string>::const_iterator i;
    vector<PlaylistItem*> list;

    for(i = urls.begin(); i != urls.end(); i++)
    {
        result = ReadPlaylist((*i).c_str());

        if(IsError(result))
        {
            result = kError_OutOfMemory;

            PlaylistItem* item = new PlaylistItem((*i).c_str());

            if(item)
            {
                list.push_back(item);
            }
        }
    }

    result = AddItems(&list, index);

    return result;
}

Error PlaylistManager::AddItem(PlaylistItem* item, bool queryForMetaData)
{
    Error result = kError_InvalidParam;
    m_mutex.Acquire();

    assert(item);

    if(item)
    {
        m_activeList->push_back(item);

        if(kPlaylistKey_MasterPlaylist == GetActivePlaylist())
        {
            AddItemToShuffleList(item);

            if(m_current == kInvalidIndex && m_activeList->size())
                InternalSetCurrentIndex(0);
        }

        if(queryForMetaData)
            RetrieveMetaData(item);

        /*UndoAdd* undoItem = new UndoAdd(this, item->URL(), m_activeList->size() - 1);

        m_undo.AddItem(undoItem);*/
     
        m_context->target->AcceptEvent(new PlaylistItemAddedEvent(item, this));

        result = kError_NoErr;
    }

    m_mutex.Release();
    return result;
}

Error PlaylistManager::AddItem(PlaylistItem* item, uint32 index, bool queryForMetaData)
{
    Error result = kError_InvalidParam;
    m_mutex.Acquire();

    assert(item);

    if(index > m_activeList->size())
        index = m_activeList->size();

    //index = CheckIndex(index);

    if(item && index != kInvalidIndex)
    {
		const PlaylistItem* currentItem = GetCurrentItem();

        m_activeList->insert(m_activeList->begin() + index,item);

        if(kPlaylistKey_MasterPlaylist == GetActivePlaylist())
        {
            AddItemToShuffleList(item);

            if(m_current == kInvalidIndex && m_activeList->size())
                InternalSetCurrentIndex(0);
            else if(index <= m_current)
            {
				SetCurrentIndex(IndexOf(currentItem));
                //SetCurrentIndex(m_current + 1);
            }
        }

        if(queryForMetaData)
            RetrieveMetaData(item);

        /*UndoAdd* undoItem = new UndoAdd(this, item->URL(), index);

        m_undo.AddItem(undoItem);*/

        m_context->target->AcceptEvent(new PlaylistItemAddedEvent(item, this));

        result = kError_NoErr;
    }

    m_mutex.Release();
    return result;
}

Error PlaylistManager::AddItems(vector<PlaylistItem*>* list, bool queryForMetaData)
{
    Error result = kError_InvalidParam;
    m_mutex.Acquire();

    assert(list);

    if(list)
    {
        //uint32 index = m_activeList->size();

        m_activeList->insert(m_activeList->end(),
                             list->begin(), 
                             list->end());

        if(kPlaylistKey_MasterPlaylist == GetActivePlaylist())
        {
            AddItemsToShuffleList(list);

            if(m_current == kInvalidIndex && m_activeList->size())
                InternalSetCurrentIndex(0);
        }

        // we need our own vector so user can delete theirs
        vector<PlaylistItem*>* items = new vector<PlaylistItem*>;

        if(items)
        {
            items->insert(items->end(), list->begin(), list->end());

            if(queryForMetaData)
                RetrieveMetaData(items);
        }
        
        /*vector<PlaylistItem *>::iterator i = list->begin();
        vector<string> urls;

        for(; i != list->end(); i++)
            urls.push_back((*i)->URL());

        UndoAddMulti* undoItem = new UndoAddMulti(this, urls, index);
        m_undo.AddItem(undoItem);*/

        //vector<PlaylistItem *>::iterator i = list->begin();
        //for(; i != list->end(); i++)
            //m_context->target->AcceptEvent(new PlaylistItemAddedEvent(*i, this));

        m_context->target->AcceptEvent(new PlaylistItemsAddedEvent(list, this));

        result = kError_NoErr;
    }

    m_mutex.Release();

    return result;
}



Error PlaylistManager::AddItems(vector<PlaylistItem*>* list, uint32 index, bool queryForMetaData)
{
    Error result = kError_InvalidParam;
    m_mutex.Acquire();

    assert(list);

    if(index > m_activeList->size())
        index = m_activeList->size();

    //index = CheckIndex(index);

    if(list && index != kInvalidIndex)
    {
		const PlaylistItem* currentItem = GetCurrentItem();

        m_activeList->insert(m_activeList->begin() + index,
                             list->begin(), 
                             list->end());

        if(kPlaylistKey_MasterPlaylist == GetActivePlaylist())
        {
            AddItemsToShuffleList(list);

            if(m_current == kInvalidIndex && m_activeList->size())
                InternalSetCurrentIndex(0);
            else if(index <= m_current)
            {
				SetCurrentIndex(IndexOf(currentItem));
                //SetCurrentIndex(m_current + list->size());
            }
        }

        // we need our own vector so user can delete theirs
        vector<PlaylistItem*>* items = new vector<PlaylistItem*>;

        if(items)
        {
            items->insert(items->end(), list->begin(), list->end());

            if(queryForMetaData)
                RetrieveMetaData(items);
        }

        /*vector<PlaylistItem *>::iterator i = list->begin();
        vector<string> urls;

        for(; i != list->end(); i++)
            urls.push_back((*i)->URL());

        UndoAddMulti* undoItem = new UndoAddMulti(this, urls, index);
        m_undo.AddItem(undoItem);*/

        //vector<PlaylistItem *>::iterator i = list->begin();
        //for(; i != list->end(); i++)
        //    m_context->target->AcceptEvent(new PlaylistItemAddedEvent(*i, this));

        m_context->target->AcceptEvent(new PlaylistItemsAddedEvent(list, this));

        result = kError_NoErr;
    }

    m_mutex.Release();

    return result;
}


// Functions for removing items from playlist
Error PlaylistManager::RemoveItem(PlaylistItem* item)
{
    Error result = kError_InvalidParam;
    m_mutex.Acquire();

    assert(item);

    if(item)
    {
        result = RemoveItem(IndexOf(item));
    }

    m_mutex.Release();
    return result;
}

Error PlaylistManager::RemoveItem(uint32 index)
{
    Error result = kError_InvalidParam;
    m_mutex.Acquire();

    index = CheckIndex(index);

    if(index != kInvalidIndex)
    {
        const PlaylistItem* currentItem = GetCurrentItem();

        PlaylistItem* item = (*m_activeList)[index];

        m_activeList->erase(m_activeList->begin() + index);

        if(kPlaylistKey_MasterPlaylist == GetActivePlaylist())
        {
            int32 shuffleIndex = InternalIndexOf(&m_shuffleList, item);

            m_shuffleList.erase(m_shuffleList.begin() + shuffleIndex);

            if(!m_activeList->size())
            {
               m_current = kInvalidIndex;
            }
        }

        /*UndoRemove* undoItem = new UndoRemove(this, item->URL(), index);

        m_undo.AddItem(undoItem);*/

        // if the metadata thread is still accessing this item
        // we don't wanna delete the item  out from under it.
        // instead we set a flag and let the metadata thread
        // clean up when it returns.
        if(item->GetState() == kPlaylistItemState_RetrievingMetaData)
        {
            item->SetState(kPlaylistItemState_Delete);  
        }
        else
        {
            delete item;
        }

        if(kPlaylistKey_MasterPlaylist == GetActivePlaylist())
        {
            uint32 newIndex = IndexOf(currentItem);
            
            if(newIndex != kInvalidIndex)
            {
                SetCurrentIndex(newIndex);
            }
            else if(m_current != kInvalidIndex && m_current >= m_activeList->size())
            {
                SetCurrentIndex(m_activeList->size() - 1);
            }
            else
            {
                SetCurrentIndex(m_current);
            }
        }

        vector<PlaylistItem*> itemList;
        vector<uint32> indexList;

        itemList.push_back(item);
        indexList.push_back(index);

        m_context->target->AcceptEvent(new PlaylistItemRemovedEvent(&itemList, &indexList, this));

        result = kError_NoErr;
    }

    m_mutex.Release();
    return result;
}

Error PlaylistManager::RemoveItems(uint32 index, uint32 count)
{
    Error result = kError_InvalidParam;
    m_mutex.Acquire();

    index = CheckIndex(index);

    if(index != kInvalidIndex)
    {
        uint32 i;
        const PlaylistItem* currentItem = GetCurrentItem();
        vector<PlaylistItem*> itemList;
        vector<uint32> indexList;

        /*UndoMultiItem* multiItem= new UndoMultiItem();

        for(i = 0; i < count; i++)
        {
            PlaylistItem* item = (*m_activeList)[index + i];

            UndoRemove* undoItem = new UndoRemove(this, item->URL(), index + i);

            multiItem->AddItem(undoItem);
        }

        m_undo.AddItem(multiItem);*/

        for(i = 0; i < count; i++)
        {
            PlaylistItem* item = (*m_activeList)[index + i];

            if(kPlaylistKey_MasterPlaylist == GetActivePlaylist())
            {
                int32 shuffleIndex = InternalIndexOf(&m_shuffleList, item);

                m_shuffleList.erase(m_shuffleList.begin() + shuffleIndex);

                if(!m_activeList->size())
                {
                   m_current = kInvalidIndex;
                }
            }            

            // if the metadata thread is still accessing this item
            // we don't wanna delete the item  out from under it.
            // instead we set a flag and let the metadata thread
            // clean up when it returns.
            if(item->GetState() == kPlaylistItemState_RetrievingMetaData)
            {
                item->SetState(kPlaylistItemState_Delete);  
            }
            else
            {
                delete item;
            }

            itemList.push_back(item);
            indexList.push_back(index + i);
        }

        m_context->target->AcceptEvent(new PlaylistItemRemovedEvent(&itemList, &indexList, this));

        m_activeList->erase(m_activeList->begin() + index, 
                            m_activeList->begin() + index + count);

        if(kPlaylistKey_MasterPlaylist == GetActivePlaylist())
        {
            uint32 newIndex = IndexOf(currentItem);
            
            if(newIndex != kInvalidIndex)
            {
                SetCurrentIndex(newIndex);
            }
            else if(m_current != kInvalidIndex && m_current >= m_activeList->size())
            {
                SetCurrentIndex(m_activeList->size() - 1);
            }
            else
            {
                SetCurrentIndex(m_current);
            }
        }

        result = kError_NoErr;
    }

    m_mutex.Release();
    return result;
}

Error PlaylistManager::RemoveItems(vector<PlaylistItem*>* items)
{
    Error result = kError_InvalidParam;
    m_mutex.Acquire();

    assert(items);

    if(items)
    {
        uint32 index = 0;
        uint32 size = 0;
        PlaylistItem* item = NULL;
        uint32 oldIndex;
        const PlaylistItem* currentItem = GetCurrentItem();
        vector<PlaylistItem*> itemList;
        vector<uint32> indexList;

        size = items->size();

        /*vector<PlaylistItem *>::iterator i = items->begin();

        UndoMultiItem* multiItem= new UndoMultiItem();

        for(; i != items->end(); i++)
        {
            UndoRemove* undoItem = new UndoRemove(this, (*i)->URL(), IndexOf(*i));

            multiItem->AddItem(undoItem);
        }

        m_undo.AddItem(multiItem);*/

        for(index = 0; index < size; index++)
        {
            item = (*items)[index];

            if(item)
            {
                oldIndex = IndexOf(item);

                m_activeList->erase(m_activeList->begin() + oldIndex);

                if(kPlaylistKey_MasterPlaylist == GetActivePlaylist())
                {
                    int32 shuffleIndex = InternalIndexOf(&m_shuffleList, item);

                    m_shuffleList.erase(m_shuffleList.begin() + shuffleIndex);
                }

                // if the metadata thread is still accessing this item
                // we don't wanna delete the item  out from under it.
                // instead we set a flag and let the metadata thread
                // clean up when it returns.
                if(item->GetState() == kPlaylistItemState_RetrievingMetaData)
                {
                    item->SetState(kPlaylistItemState_Delete);  
                }
                else
                {
                    delete item;
                }

                itemList.push_back(item);
                indexList.push_back(oldIndex);                

                result = kError_NoErr;
            }
        }  

        m_context->target->AcceptEvent(new PlaylistItemRemovedEvent(&itemList, &indexList, this));

        if(kPlaylistKey_MasterPlaylist == GetActivePlaylist())
        {
            uint32 newIndex = IndexOf(currentItem);
            
            if(newIndex != kInvalidIndex)
            {
                SetCurrentIndex(newIndex);
            }
            else if(m_current != kInvalidIndex && m_current >= m_activeList->size())
            {
                SetCurrentIndex(m_activeList->size() - 1);
            }
            else
            {
                SetCurrentIndex(m_current);
            }
        }
    }

    m_mutex.Release();
    return result;
}

Error PlaylistManager::RemoveAll()
{
    m_mutex.Acquire();

    Error result = kError_InvalidParam;
    uint32 index = m_activeList->size() - 1;
    //uint32 size = m_activeList->size();
    PlaylistItem* item = NULL;
    vector<PlaylistItem*> itemList;
    vector<uint32> indexList;

    vector<PlaylistItem*>::reverse_iterator i;

    for(i = m_activeList->rbegin(); i !=  m_activeList->rend(); i++, index--)
    {
        item = (*i);

        if(item)
        {            
            /*UndoRemove* undoItem = new UndoRemove(this, item->URL(), IndexOf(item));

            m_undo.AddItem(undoItem);*/

            // if the metadata thread is still accessing this item
            // we don't wanna delete the item  out from under it.
            // instead we set a flag and let the metadata thread
            // clean up when it returns.
            if(item->GetState() == kPlaylistItemState_RetrievingMetaData)
            {
                item->SetState(kPlaylistItemState_Delete);  
            }
            else
            {
                delete item;
            }          
            
            itemList.push_back(item);
            indexList.push_back(index);
        }
    }  

    m_context->target->AcceptEvent(new PlaylistItemRemovedEvent(&itemList, &indexList, this));

    m_activeList->clear();

    if(kPlaylistKey_MasterPlaylist == GetActivePlaylist())
    {
        m_shuffleList.clear();
        m_current = kInvalidIndex;
    }

    result = kError_NoErr;

    m_mutex.Release();
    return result;
}


// Functions for moving items around
Error PlaylistManager::SwapItems(PlaylistItem* item1, PlaylistItem* item2)
{
    return SwapItems(IndexOf(item1), IndexOf(item2));
}

Error PlaylistManager::SwapItems(uint32 index1, uint32 index2)
{
    Error result = kError_InvalidParam;
    m_mutex.Acquire();

    index1 = CheckIndex(index1);
    index2 = CheckIndex(index2);

    if(index1 != kInvalidIndex && index2 != kInvalidIndex)
    {
        PlaylistItem* temp;

        /*UndoMove* undoItem1 = new UndoMove(this, index2, index1);
        m_undo.AddItem(undoItem1);

        UndoMove* undoItem2 = new UndoMove(this, index1, index2);
        m_undo.AddItem(undoItem2);*/

        temp = (*m_activeList)[index1];
        (*m_activeList)[index1] = (*m_activeList)[index2];
        (*m_activeList)[index2] = temp;

        m_context->target->AcceptEvent(
                new PlaylistItemMovedEvent(index1, index2, (*m_activeList)[index2], this));

        // we add 1 to index2 bc it was shifted down by the above move
        m_context->target->AcceptEvent(
                new PlaylistItemMovedEvent(index2 + 1, index1, (*m_activeList)[index1], this));

        if(kPlaylistKey_MasterPlaylist == GetActivePlaylist())
        {
            if(m_current == index1)
                InternalSetCurrentIndex(index2);
            else if(m_current == index2)
                InternalSetCurrentIndex(index1);
        }        

        result = kError_NoErr;
    }

    m_mutex.Release();
    return result;
}

Error PlaylistManager::MoveItem(PlaylistItem* item, uint32 index)
{
    return MoveItem(IndexOf(item), index);
}

Error PlaylistManager::MoveItem(uint32 oldIndex, uint32 newIndex)
{
    Error result = kError_InvalidParam;
    m_mutex.Acquire();

    oldIndex = CheckIndex(oldIndex);
    newIndex = CheckIndex(newIndex);

    if(oldIndex != kInvalidIndex && newIndex != kInvalidIndex)
    {
        PlaylistItem* item = (*m_activeList)[oldIndex];

        /*UndoMove* undoItem = new UndoMove(this, newIndex, oldIndex);
        m_undo.AddItem(undoItem);*/

        m_activeList->erase(m_activeList->begin() + oldIndex);

        if(newIndex > m_activeList->size())
            newIndex = m_activeList->size();

        m_activeList->insert(m_activeList->begin() + newIndex,item);

        if(kPlaylistKey_MasterPlaylist == GetActivePlaylist())
        {
            if(oldIndex == m_current)
                InternalSetCurrentIndex(newIndex);
            else if(newIndex == m_current)
            {
                if(oldIndex > m_current)
                    InternalSetCurrentIndex(m_current + 1);
                else
                    InternalSetCurrentIndex(m_current - 1);
            }
        }

        m_context->target->AcceptEvent(
                new PlaylistItemMovedEvent(oldIndex, newIndex, item, this));

        result = kError_NoErr;
    }

    m_mutex.Release();
    return result;
}

Error PlaylistManager::MoveItems(vector<PlaylistItem*>* items, uint32 index)
{
    Error result = kError_InvalidParam;
    m_mutex.Acquire();

    assert(items);

    index = CheckIndex(index);

    const PlaylistItem* currentItem = GetCurrentItem();
    vector<uint32> oldIndices;

    if(items)
    {
        uint32 size = 0;
        uint32 i = 0;
        PlaylistItem* item = NULL;

        size = items->size();

        for(i = 0; i < size; i++)
        {
            item = (*items)[i];

            if(item)
            {
                uint32 oldIndex = IndexOf(item);

                if(oldIndex < index)
                    index--;

                oldIndices.push_back(oldIndex);
            }
        }

        for(i = 0; i < size; i++)
        {
            item = (*items)[i];

            if(item)
            {
                m_activeList->erase(m_activeList->begin() + IndexOf(item));
            }
        }  

        if(index >= m_activeList->size())
            index = m_activeList->size();

        m_activeList->insert(m_activeList->begin() + index,
                             items->begin(), 
                             items->end());

        if(kPlaylistKey_MasterPlaylist == GetActivePlaylist())
            SetCurrentIndex(IndexOf(currentItem));

        /*UndoMultiItem* multiItem= new UndoMultiItem();

        for(i = 0; i < size; i++)
        {
            UndoMove* undoItem = new UndoMove(this, index + i, oldIndices[i]);
            multiItem.AddItem(undoItem);
        }  

        m_undo.AddItem(multiItem);*/

        for(i = 0; i < size; i++)
        {
            item = (*items)[i];

            if(item)
            {
                m_context->target->AcceptEvent(
                    new PlaylistItemMovedEvent(oldIndices[i], index + i, item, this));
            }
        }

        result = kError_NoErr;
    }

    m_mutex.Release();
    return result;
}

Error PlaylistManager::MoveItems(vector<uint32>* items, uint32 index)
{
    Error result = kError_InvalidParam;
    m_mutex.Acquire();

    assert(items);

    if(items)
    {
        vector<PlaylistItem*> pl_items;

        vector<uint32>::iterator i;

        for(i = items->begin(); i != items->end(); i++)
        {
            pl_items.push_back(ItemAt(*i));
        }

        result = MoveItems(&pl_items, index);
    }

    m_mutex.Release();
    return result;
}

typedef struct WriteToDiskStruct {
    PlaylistItem item;
    vector<MetaDataFormat*> metadataFormats;
    Thread* thread;
}WriteToDiskStruct;

static void write_to_disk(void* arg)
{
    WriteToDiskStruct* wtd = (WriteToDiskStruct*)arg;

    MetaData metadata = wtd->item.GetMetaData();

    MetaDataFormat* mdf = NULL;
    uint32 numFormats;

    numFormats = wtd->metadataFormats.size();

    for(uint32 i = 0; i < numFormats; i++)
    {
        mdf = wtd->metadataFormats[i];

        if(mdf)
        {
            mdf->WriteMetaData(wtd->item.URL().c_str(), metadata);
        }
    }

    delete wtd->thread;
    delete wtd;
}

// Functions for updating
// This function searches the items in the playlist
// and updates the metadata if the tracks are the
// same (matched based on URL)
Error PlaylistManager::UpdateTrackMetaData(PlaylistItem* updatedTrack, bool writeToDisk)
{
    Error result = kError_InvalidParam;
    //m_mutex.Acquire();

    vector<PlaylistItem*>::iterator i = m_activeList->begin();

    MetaData metadata = updatedTrack->GetMetaData();

    vector<PlaylistItem*> pl_items;

    for(; i != m_activeList->end(); i++)
    {
        if((*i)->URL() == updatedTrack->URL())
        {
            (*i)->SetMetaData(&metadata);
            
            pl_items.push_back(*i);
        }
    }

    if(pl_items.size())
    {
        m_context->target->AcceptEvent(new PlaylistItemsUpdatedEvent(&pl_items, this));
    }

    if(writeToDisk)
    {
        WriteToDiskStruct* wtd = new WriteToDiskStruct;

        wtd->item = *updatedTrack;
        wtd->metadataFormats = m_metadataFormats;

        wtd->thread = Thread::CreateThread();
        wtd->thread->Create(write_to_disk, wtd, true);
    }

    //m_mutex.Release();
    return result;
}


// Functions for sorting
Error PlaylistManager::Sort(PlaylistSortKey key, PlaylistSortType type)
{
    Error result = kError_InvalidParam;
    m_mutex.Acquire();

    const PlaylistItem* currentItem = GetCurrentItem();

    if(key >= kPlaylistSortKey_FirstKey && key < kPlaylistSortKey_LastKey)
    {
        if(type == kPlaylistSortType_Ascending)
            stable_sort(m_activeList->begin(), m_activeList->end(), PlaylistItemSort(key));
        else
            stable_sort(m_activeList->begin(), m_activeList->end(), not2(PlaylistItemSort(key)));
    
        m_sortKey = key;
        m_sortType = type;

        result = kError_NoErr;
    }
    else if(key == kPlaylistSortKey_Random)
    {
        ShuffleIt(m_activeList);
        
        m_sortKey = key;
        m_sortType = type;

        result = kError_NoErr;
    }

    if(IsntError(result))
    {
        if(kPlaylistKey_MasterPlaylist == GetActivePlaylist() && currentItem)
            SetCurrentIndex(IndexOf(currentItem));
           
        m_context->target->AcceptEvent(new PlaylistSortedEvent(key, this));

        //m_undo.Clear();
    }

    m_mutex.Release();
    return result;
}

PlaylistSortKey PlaylistManager::GetPlaylistSortKey() const
{
    return m_sortKey;
}

PlaylistSortType PlaylistManager::GetPlaylistSortType() const
{
    return m_sortType;
}

// Which playlist are we dealing with for purposes of editing:
// 1) Master Playlist - list of songs to play
// 2) Secondary Playlist - a playlist that we want to edit
//      - External playlist
//      - Portable playlist

Error PlaylistManager::SetActivePlaylist(PlaylistKey key)
{
    Error result = kError_InvalidParam;
    m_mutex.Acquire();

    if(key >= kPlaylistKey_FirstKey && key < kPlaylistKey_LastKey)
    {
        m_playlistKey = key;
        result = kError_NoErr;

        switch(key)
        {
            case kPlaylistKey_MasterPlaylist:
            {
                m_activeList = &m_masterList;
                break;
            }

            case kPlaylistKey_ExternalPlaylist:
            {
                m_activeList = &m_externalList;
                break;
            }
    

            case kPlaylistKey_PortablePlaylist:
            {
                //m_activeList = &m_portableList;
                m_activeList = &m_externalList;
                break;
            }

            default:
            {
                result = kError_InvalidParam;
            }
        }
    }

    m_mutex.Release();
    return result;
}

PlaylistKey PlaylistManager::GetActivePlaylist() const
{
    return m_playlistKey;  
}

Error PlaylistManager::SetExternalPlaylist(char* url)
{
    Error result = kError_InvalidParam;
    m_mutex.Acquire();

    assert(url);

    if(url)
    {
        // first delete old playlist
        uint32 index = 0;
        uint32 numItems = 0;
        PlaylistItem* item = NULL;

        numItems = m_externalList.size();

        for(index = 0; index < numItems; index++)
        {
            item = m_externalList[index];

            if(item)
            {
                // if the metadata thread is still accessing this item
                // we don't wanna delete the item  out from under it.
                // instead we set a flag and let the metadata thread
                // clean up when it returns.
                if(item->GetState() == kPlaylistItemState_RetrievingMetaData)
                {
                    item->SetState(kPlaylistItemState_Delete);  
                }
                else
                {
                    delete item;
                }
            }
        }

        m_externalList.clear();

        result = ReadPlaylist(url, &m_externalList);

        vector<PlaylistItem*>* items = new vector<PlaylistItem*>;

        items->insert(items->end(), 
                      m_externalList.begin(), 
                      m_externalList.end());

        RetrieveMetaData(items);

        m_externalPlaylist = string(url);
    }

    m_mutex.Release();
    return result;
}

Error PlaylistManager::GetExternalPlaylist(char* url, uint32* length)
{
    Error result = kError_InvalidParam;
    m_mutex.Acquire();

    assert(url);
    assert(length);

    if(url && length)
    {
        if(*length >= m_externalPlaylist.size() + 1)
        {
            strcpy(url, m_externalPlaylist.c_str());
            result = kError_NoErr;
        }
        else
        {
            result = kError_BufferTooSmall;
        }

        *length = m_externalPlaylist.size() + 1;
    }

    m_mutex.Release();
    return result;
}

Error PlaylistManager::SetPortablePlaylist(DeviceInfo* device, 
                                           PLMCallBackFunction function,
                                           void* cookie)
{
    Error result = kError_InvalidParam;

    assert(device);

    if(device)
    {
        result = ReadPortablePlaylist(device, NULL, function, cookie);
        m_portableDevice = *device;
    }

    return result;
}

Error PlaylistManager::GetPortablePlaylist(DeviceInfo* device)
{
    Error result = kError_InvalidParam;

    assert(device);

    if(device)
    {
        result = kError_NoErr;
        *device = m_portableDevice;
    }

    return result;
}


// External playlist support
bool PlaylistManager::IsSupportedPlaylistFormat(const char *extension)
{
    bool retvalue = false;

    m_mutex.Acquire();

    if (!extension)
    {
        m_mutex.Release();
        return false;
    }

    uint32 numFormats = m_playlistFormats.size();

    for(uint32 index = 0; index < numFormats; index++)
    {
        PlaylistFormatInfo* format = m_playlistFormats[index];

        if(!strcasecmp(extension, format->GetExtension()))
        {
            retvalue = true;
            break;
        }
    }

    m_mutex.Release();
  
    return retvalue;
}

Error PlaylistManager::GetSupportedPlaylistFormats(PlaylistFormatInfo* format, 
                                                   uint32 index)
{
    Error result = kError_InvalidParam;

    assert(format);

    if(format)
    {
        result = kError_NoMoreFormats;

        uint32 numFormats = m_playlistFormats.size();

        if(index < numFormats)
        {
            result = kError_NoErr;

            *format = *m_playlistFormats[index];
        }
    }

    return result;
}

Error PlaylistManager::ReadPlaylist(const char* url, 
                                    vector<PlaylistItem*>* items,
                                    PLMCallBackFunction function,
                                    void* cookie)
{
    Error result = kError_InvalidParam;

    assert(url);

    if(url)
    {
        // find a suitable plugin
        result = kError_FormatNotSupported;
        char* extension = strrchr(url, '.');

        if(extension)
        {
            extension++;

            uint32 numFormats = m_playlistFormats.size();

            for(uint32 index = 0; index < numFormats; index++)
            {
                PlaylistFormatInfo* format = m_playlistFormats[index];
                
                if(!strcasecmp(extension, format->GetExtension()))
                {
                    bool addToActiveList = false;

                    if(!items)
                    {
                        items = new vector<PlaylistItem*>;
                        addToActiveList = true;
                    }

                    result = format->GetRef()->ReadPlaylist(url, 
                                                            items, 
                                                            function, 
                                                            cookie);

                    if(addToActiveList)
                    {
                        AddItems(items);
                        delete items;
                    }

                    break;
                }
            }
        }
    }

    return result;
}

Error PlaylistManager::WritePlaylist(const char* url, 
                                     PlaylistFormatInfo* format, 
                                     vector<PlaylistItem*>* items,
                                     PLMCallBackFunction function,
                                     void* cookie)
{
    Error result = kError_InvalidParam;

    assert(url);
    assert(format);

    if(url && format)
    {
        if(!items)
        {
            items = m_activeList;
        }
        result = format->GetRef()->WritePlaylist(url, 
                                                 format, 
                                                 items, 
                                                 function, 
                                                 cookie);
    }

    return result;
}

Error PlaylistManager::WritePlaylist(const char* url, 
                                     vector<PlaylistItem*>* items,
                                     PLMCallBackFunction function,
                                     void* cookie)
{
    Error result = kError_InvalidParam;

    assert(url);

    if(url)
    {
        // find a suitable plugin
        result = kError_FormatNotSupported;
        char* extension = strrchr(url, '.');

        if(extension)
        {
            extension++;

            uint32 numFormats = m_playlistFormats.size();

            for(uint32 index = 0; index < numFormats; index++)
            {
                PlaylistFormatInfo* format = m_playlistFormats[index];
                
                if(!strcasecmp(extension, format->GetExtension()))
                {
                    if(!items)
                    {
                        items = m_activeList;
                    }

                    result = format->GetRef()->WritePlaylist(url, 
                                                             format, 
                                                             items, 
                                                             function, 
                                                             cookie);
                    break;
                }
            }
        }
    }

    return result;
}


// Portable player communication
Error PlaylistManager::GetSupportedPortables(DeviceInfo* device, uint32 index)
{
    Error result = kError_InvalidParam;

    assert(device);

    if(device)
    {
        result = kError_NoMoreDevices;

        uint32 numDevices = m_portablePlayers.size();

        if(index < numDevices)
        {
            result = kError_NoErr;

            *device = *m_portablePlayers[index];
        }
    }

    return result;
}

bool PlaylistManager::IsPortableAvailable(DeviceInfo* device)
{
    bool result = false;

    assert(device);

    if(device)
    {
        result = device->GetRef()->IsDeviceAvailable(device);
    }

    return result;
}

Error PlaylistManager::GetDeviceInfo(DeviceInfo* device)
{
   Error result = kError_InvalidParam;

    assert(device);

    if(device)
    {
        result = device->GetRef()->GetDeviceInfo(device);
    }

    return result;
}

Error PlaylistManager::InitializeDevice(DeviceInfo* device, 
                                        PLMCallBackFunction function,
                                        void* cookie)
{
    Error result = kError_InvalidParam;

    assert(device);

    if(device)
    {
        result = device->GetRef()->InitializeDevice(device, function, cookie);
    }

    return result;

}


Error PlaylistManager::ReadPortablePlaylist(DeviceInfo* device,
                                            vector<PlaylistItem*>* items,
                                            PLMCallBackFunction function,
                                            void* cookie)
{
    Error result = kError_InvalidParam;

    assert(device);

    if(device)
    {
        // first delete old playlist

        if(!items)
        {
            uint32 index = 0;
            uint32 numItems = 0;
            PlaylistItem* item = NULL;

            numItems = m_externalList.size();

            for(index = 0; index < numItems; index++)
            {
                item = m_externalList[index];

                if(item)
                {
                    // if the metadata thread is still accessing this item
                    // we don't wanna delete the item  out from under it.
                    // instead we set a flag and let the metadata thread
                    // clean up when it returns.
                    if(item->GetState() == kPlaylistItemState_RetrievingMetaData)
                    {
                        item->SetState(kPlaylistItemState_Delete);  
                    }
                    else
                    {
                        delete item;
                    }
                }
            }
        }

        bool addToActiveList = false;
      
        if(!items)
        {
            items = new vector<PlaylistItem*>;
            addToActiveList = true;
        }

        result = device->GetRef()->ReadPlaylist(device, 
                                                items,
                                                function, 
                                                cookie);

        if(addToActiveList)
        {
            AddItems(items);
            delete items;
        }
    }

    return result;
}

#if 0
Error PlaylistManager::ReadPortablePlaylist(DeviceInfo* device,
                                            PLMCallBackFunction function,
                                            void* cookie)
{
    Error result = kError_InvalidParam;

    assert(device);

    if(device)
    {
        // first delete old playlist
        uint32 index = 0;
        uint32 numItems = 0;
        PlaylistItem* item = NULL;

        numItems = m_portableList.size();

        for(index = 0; index < numItems; index++)
        {
            item = m_portableList[index];

            if(item)
            {
                // if the metadata thread is still accessing this item
                // we don't wanna delete the item  out from under it.
                // instead we set a flag and let the metadata thread
                // clean up when it returns.
                if(item->GetState() == kPlaylistItemState_RetrievingMetaData)
                {
                    item->SetState(kPlaylistItemState_Delete);  
                }
                else
                {
                    delete item;
                }
            }
        }

        m_portableList.clear();

        result = device->GetRef()->ReadPlaylist(device, 
                                                &m_portableList,
                                                function, 
                                                cookie);

        vector<PlaylistItem *>::iterator i = m_portableList.begin();
        for(; i != m_portableList.end(); i++)
            m_context->target->AcceptEvent(new PlaylistItemAddedEvent(*i, this));
    }

    return result;
}

#endif

Error PlaylistManager::SyncPortablePlaylist(DeviceInfo* device,
                                            PLMCallBackFunction function,
                                            void* cookie)
{
    Error result = kError_InvalidParam;

    assert(device);

    if(device)
    {
        result = device->GetRef()->WritePlaylist(device, 
                                                &m_externalList,
                                                function, 
                                                cookie);
    }

    return result;
}

Error PlaylistManager::DownloadItemFromPortable(DeviceInfo* device,
                                                PlaylistItem* item,
                                                const char* url,
                                                PLMCallBackFunction function,
                                                void* cookie)
{
    Error result = kError_InvalidParam;

    assert(device);
    assert(item);
    assert(url);

    if(device && item && url)
    {
        result = device->GetRef()->DownloadSong(device, 
                                                item,
                                                url,
                                                function, 
                                                cookie);
    }

    return result;
}

uint32 PlaylistManager::Time()
{
    uint32 time = 0;

    //m_mutex.Acquire();

    vector<PlaylistItem*>::iterator i = m_activeList->begin();

    for(; i != m_activeList->end();i++)
    {
       time += (*i)->GetMetaData().Time();
    }

    //m_mutex.Release();

    return time;
}

// Utility Functions
bool PlaylistManager::IsEmpty()
{
    bool result;
    m_mutex.Acquire();

    result = m_activeList->empty();

    m_mutex.Release();
    return result;
}

uint32 PlaylistManager::CountItems()
{
    uint32 result;
    m_mutex.Acquire();

    result = m_activeList->size();

    m_mutex.Release();
    return result;
}

PlaylistItem* PlaylistManager::ItemAt(uint32 index)
{
    PlaylistItem* result = NULL;
    m_mutex.Acquire();
    
    index = CheckIndex(index);

    if(index != kInvalidIndex)
    {
        result = (*m_activeList)[index];
    }
    
    m_mutex.Release();
    return result;
}

uint32 PlaylistManager::IndexOf(const PlaylistItem* item)
{
    return InternalIndexOf(m_activeList, item);
}

uint32 PlaylistManager::InternalIndexOf(vector<PlaylistItem*>* list,
                                        const PlaylistItem* item)
{
    uint32 result = kInvalidIndex;
    uint32 index = 0;
    uint32 size = 0;

    assert(list);
    assert(item);

    if(list && item)
    {
        vector<PlaylistItem*>::iterator i = list->begin();
        size = list->size();

        for(index = 0; i != list->end();i++,  index++)
        {
            if(item == (*i))
            {
                result = index;
                break;
            }
        }
    }
    
    return result;
}

bool PlaylistManager::HasItem(const PlaylistItem* item)
{
    return (IndexOf(item) != kInvalidIndex);
}


bool PlaylistManager::CanUndo()
{
    m_mutex.Acquire();
    
    bool result = m_undo.CanUndo();
    
    m_mutex.Release();

    return result;
}

bool PlaylistManager::CanRedo()
{
    m_mutex.Acquire();

    bool result = m_undo.CanRedo();
    
    m_mutex.Release();

    return result;    
}

void PlaylistManager::Undo()
{
    m_mutex.Acquire();
    
    m_undo.Undo();
    
    m_mutex.Release();
}

void PlaylistManager::Redo()
{
    m_mutex.Acquire();
    
    m_undo.Redo();
    
    m_mutex.Release();
}

// Internal functions

inline uint32 PlaylistManager::CheckIndex(uint32 index)
{
	// If we're dealing with a bogus index then set it to -1.
	if(index >= CountItems())
    {
		index = kInvalidIndex;
    }

	return index;
}

void PlaylistManager::AddItemToShuffleList(PlaylistItem* item)
{
    m_shuffleList.push_back(item);
}

void PlaylistManager::AddItemsToShuffleList(vector<PlaylistItem*>* list)
{
	ShuffleIt(list);

    m_shuffleList.insert(m_shuffleList.end(),
                         list->begin(), 
                         list->end());
}

void
PlaylistManager::
MetaDataThreadFunction(vector<PlaylistItem*>* list)
{
    assert(list);

    if(list)
    {
        uint32 index = 0;
        uint32 numItems = 0;
        PlaylistItem* item = NULL;

        numItems = list->size();

        vector<PlaylistItem*> pl_items;

        for(index = 0; index < numItems; index++)
        {
            item = (*list)[index];

            if(item)
            {
                MetaData metadata;
                MetaData* dbData = NULL;

                // is there any metadata stored for this item in the music db?
                dbData = m_context->catalog->ReadMetaDataFromDatabase(item->URL().c_str());

                if(dbData)
                {
                    metadata = *dbData;
                    delete dbData;
                }
                else // if not look to see if the file contains metadata
                {
                    metadata = item->GetMetaData();
                    MetaDataFormat* mdf = NULL;
                    uint32 numFormats;

                    numFormats = m_metadataFormats.size();

                    for(uint32 i = 0; i < numFormats; i++)
                    {
                        mdf = m_metadataFormats[i];

                        if(mdf)
                        {
                            mdf->ReadMetaData(item->URL().c_str(), &metadata);
                        }
                    }
                }                

                m_mutex.Acquire();

                if(item->GetState() == kPlaylistItemState_Delete)
                {
                    delete item;
                }
                else
                {
                    item->SetMetaData(&metadata);
                    item->SetState(kPlaylistItemState_Normal);

                    if(item == GetCurrentItem())
                        pl_items.insert(pl_items.begin(), item);
                    else
                        pl_items.push_back(item);
                }

                m_mutex.Release();
            }
        }     

        if(pl_items.size())
        {
            m_context->target->AcceptEvent(new PlaylistItemsUpdatedEvent(&pl_items, this));
        }

        delete list;
    }
}

typedef struct MetaDataThreadStruct{
    PlaylistManager* plm;
    vector<PlaylistItem*>* items;
    Thread* thread;
} MetaDataThreadStruct;

void 
PlaylistManager::
metadata_thread_function(void* arg)
{
    MetaDataThreadStruct* mts = (MetaDataThreadStruct*)arg;

    mts->plm->MetaDataThreadFunction(mts->items);

    delete mts->thread;
    delete mts;
}

void PlaylistManager::RetrieveMetaData(PlaylistItem* item)
{
    assert(item);

    if(item)
    {
        vector<PlaylistItem*>* items = new vector<PlaylistItem*>;

        if(items)
        {
            items->push_back(item);
    
            RetrieveMetaData(items);
        }
    }
}

void PlaylistManager::RetrieveMetaData(vector<PlaylistItem*>* list)
{
    uint32 index = 0;
    uint32 size = 0;
    PlaylistItem* item = NULL;

    assert(list);

    if(list)
    {
        size = list->size();

        for(index = 0; index < size; index++)
        {
            item = (*list)[index];

            if(item && item->GetState() == kPlaylistItemState_Normal)
            {
                item->SetState(kPlaylistItemState_RetrievingMetaData);
            }
        }

        Thread* thread = Thread::CreateThread();

        if(thread)
        {
            MetaDataThreadStruct* mts = new MetaDataThreadStruct;

            mts->plm = this;
            mts->items = list;
            mts->thread = thread;

            thread->Create(metadata_thread_function, mts, true);
        }
    }
}

void
PlaylistManager::
RetrieveMetaDataNow(PlaylistItem* item)
{
    assert(item);

    if(item)
    {
        MetaData metadata = item->GetMetaData();
        MetaDataFormat* mdf = NULL;
        uint32 numFormats;

        numFormats = m_metadataFormats.size();

        for (uint32 i = 0; i < numFormats; i++)
        {
            mdf = m_metadataFormats[i];

            if (mdf)
                mdf->ReadMetaData(item->URL().c_str(), &metadata);
        }

        item->SetMetaData(&metadata);
        item->SetState(kPlaylistItemState_Normal);
    }
}


UndoAdd::UndoAdd(PlaylistManager* plm, const string& url, uint32 index):
    m_plm(plm), m_url(url), m_index(index)
{
    
}

UndoAdd::~UndoAdd()
{

}

void UndoAdd::Undo()
{
    m_plm->RemoveItem(m_index);
}

void UndoAdd::Redo()
{
    m_plm->AddItem(m_url, m_index);
}

UndoAddMulti::UndoAddMulti(PlaylistManager* plm, vector<string>& urls, uint32 index):
    m_plm(plm), m_urls(urls), m_index(index)
{
    
}

UndoAddMulti::~UndoAddMulti()
{

}

void UndoAddMulti::Undo()
{
    m_plm->RemoveItems(m_index, m_urls.size());
}

void UndoAddMulti::Redo()
{
    m_plm->AddItems(m_urls, m_index);
}

UndoRemove::UndoRemove(PlaylistManager* plm, const string& url, uint32 index):
    m_plm(plm), m_url(url), m_index(index)
{

}

UndoRemove::~UndoRemove()
{

}

void UndoRemove::Undo()
{
    m_plm->AddItem(m_url, m_index);
}

void UndoRemove::Redo()
{
    m_plm->RemoveItem(m_index);
}

UndoMove::UndoMove(PlaylistManager* plm, uint32 newIndex, uint32 oldIndex):
    m_plm(plm), m_newIndex(newIndex), m_oldIndex(oldIndex)
{

}

UndoMove::~UndoMove()
{
    
}

void UndoMove::Undo()
{
    m_plm->MoveItem(m_newIndex, m_oldIndex);
}

void UndoMove::Redo()
{
    m_plm->MoveItem(m_oldIndex, m_newIndex);
}
