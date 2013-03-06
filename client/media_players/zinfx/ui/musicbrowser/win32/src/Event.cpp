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

        $Id: Event.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
____________________________________________________________________________*/

#include <windows.h>
#include <windowsx.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <math.h>

#include <sstream>

using namespace std;

#include "config.h"
#include "utility.h"
#include "resource.h"
#include "Win32MusicBrowser.h"
#include "EditTrackInfoDialog.h"
#include "eventdata.h"
#include "debug.h"
#include "FavoriteDialog.h"
#include "player.h"

void MusicBrowserUI::TipEvent(PlaylistItem *item)
{
   vector<PlaylistItem*> items;
   PlaylistItem *tip = NULL;
   if (item)
       tip = item;

   if (!tip) {
	   GetSelectedPlaylistItems(&items);
       tip = *(items.begin());
   }
 
   if (!tip)
	   return;

   string artistname = tip->GetMetaData().Artist();

   if (artistname.size() == 0 || artistname == "Unknown")
       return;

   string encoded;
   ReplaceSpaces(artistname, encoded);
   string url = string("http://www.fairtunes.com/servlet/ArtistLookupServlet?redirectPage=http://www.fairtunes.com/search.jsp&searchTerms=") + encoded;

   ShellExecute(m_hWnd, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void MusicBrowserUI::ClearPlaylistEvent(void)
{
    m_context->target->AcceptEvent(new Event(CMD_Stop));

    m_plm->RemoveAll();
}

void MusicBrowserUI::RenameEvent(void)
{
    HWND hwnd = m_hMusicView;

    HTREEITEM item;
    //tv_hitTESTINFO hti;

    //GetCursorPos(&hti.pt);
    //ScreenToClient(hwnd, &hti.pt);
    //item = TreeView_HitTest(hwnd, &hti);  

    item = TreeView_GetSelection(hwnd);

    if(item /*&& (hti.flags & TVHT_ONITEM)*/ &&
       item != m_hMyMusicItem &&
       item != m_hPlaylistItem &&
       item != m_hAllItem &&
       item != m_hUncatItem &&
       item != m_hNewPlaylistItem && 
       item != m_hPortableItem &&
       TreeView_GetParent(m_hMusicView, item) != m_hPortableItem)
    {
        EditItemLabel(hwnd, item);
    }
}

const char kErrorMsg[] = "Cannot delete %s: Access is denied.\r\n\r\n"
                        "Make sure the file is not currently in use.";

bool MusicBrowserUI::DeleteFromDrive(const char* url)
{
    bool result = true;
    char path[MAX_PATH];
    uint32 length = sizeof(path);
    BOOL success = FALSE;

    URLToFilePath(url, path, &length);

    do
    {
        success = DeleteFile(path);

        if(!success)
        {
            int ret;
            char msg[MAX_PATH + sizeof(kErrorMsg) + 1];
            char* cp;

            cp = strrchr(path, '\\');

            if(cp)
                cp++;
            else
                cp = path;

            sprintf(msg, kErrorMsg, cp);

            ret = MessageBox(m_hWnd, 
                              msg,
                              "Unable To Delete File",
                              MB_ABORTRETRYIGNORE|MB_ICONSTOP);

            switch(ret)
            {
                case IDABORT:
                    result = false;
                    success = TRUE;
                    break;

                case IDRETRY:
                    result = true;
                    success = FALSE;
                    break;

                case IDIGNORE:
                    result = true;
                    success = TRUE;
                    break;
            }
        }

    }while(!success);

    return result;
}

void MusicBrowserUI::RemoveEvent(void)
{
    // first figure out which control has focus
    HWND hwndFocus = GetFocus();

    if(hwndFocus == m_hPlaylistView)
    {
        uint32 count = ListView_GetSelectedCount(m_hPlaylistView);
        uint32 found = 0;
        uint32 index = ListView_GetItemCount(m_hPlaylistView) - 1;
        bool isCurrent = false;
        vector<PlaylistItem*> items;

        while(found < count)
        {
            uint32 state = ListView_GetItemState(m_hPlaylistView, 
                                                 index, 
                                                 LVIS_SELECTED);
            if(state & LVIS_SELECTED)
            {
                found++;

                if(m_plm->GetCurrentIndex() == index)
                    isCurrent = true;

                items.push_back(m_plm->ItemAt(index));
            }

            index--;
        }    

        bool needToStopPlay = isCurrent && m_playerState == PLAYERSTATE_PLAYING;
        
        if(needToStopPlay)
            m_context->target->AcceptEvent(new Event(CMD_Stop));

        m_plm->RemoveItems(&items);

        if(needToStopPlay)
            m_context->target->AcceptEvent(new Event(CMD_Play));
    }
    else if(hwndFocus == m_hMusicView)
    {
        bool deleteFromDrive = false;

        if(0 < DialogBoxParam(g_hinst, 
                              MAKEINTRESOURCE(IDD_REMOVETRACKS),
                              m_hWnd, 
                              ::RemoveTracksDlgProc, 
                              (LPARAM)&deleteFromDrive))
        {       
            vector<PlaylistItem*> items;
            GetSelectedMusicTreeItems(&items); 
            bool keepGoing = true;

            vector<PlaylistItem*>::iterator i;

            for(i = items.begin(); i != items.end(); i++)
            {
                m_context->catalog->RemoveSong((*i)->URL().c_str());

                if(deleteFromDrive)
                {
                    keepGoing = DeleteFromDrive((*i)->URL().c_str());

                    if(!keepGoing)
                        break;
                }
            }

            if(keepGoing)
            {
                vector<string> urls;            
                GetSelectedPlaylistItems(&urls);

                vector<string>::iterator j;
            
                for(j = urls.begin(); j != urls.end(); j++)
                {
                    m_context->catalog->RemovePlaylist((*j).c_str());

                    if(deleteFromDrive)
                    {
                        keepGoing = DeleteFromDrive((*j).c_str());

                        if(!keepGoing)
                            break;
                    }
                }
            }
        }
    }
}

void MusicBrowserUI::MoveUpEvent(void)
{
    uint32 count = ListView_GetSelectedCount(m_hPlaylistView);
    uint32 found = 0;
    uint32 index = 0;

    while(found < count)
    {
        uint32 state = ListView_GetItemState(m_hPlaylistView, 
                                             index, 
                                             LVIS_SELECTED);

        if(state & LVIS_SELECTED)
        {
            found++;

            if(index == 0)
                break;

            m_plm->MoveItem(index, index - 1);
        }

        index++;
    }    
}

void MusicBrowserUI::MoveDownEvent(void)
{
    uint32 count = ListView_GetSelectedCount(m_hPlaylistView);
    uint32 found = 0;
    uint32 index = ListView_GetItemCount(m_hPlaylistView) - 1;;

    while(found < count)
    {
        uint32 state = ListView_GetItemState(m_hPlaylistView, 
                                             index, 
                                             LVIS_SELECTED);

        if(state & LVIS_SELECTED)
        {
            found++;

            if(index == m_plm->CountItems() - 1)
                break;

            m_plm->MoveItem(index, index + 1);
        }

        index--;
    }    
}

void MusicBrowserUI::MoveItemEvent(int source, int dest)
{
    m_plm->MoveItem(source, dest);
}

void MusicBrowserUI::StartStopMusicSearch(bool useWizard)
{  
    HMENU           hMenu;
    MENUITEMINFO    sItem;
    bool            doSearch = false;

    if (m_bSearchInProgress)
    {
        m_bSearchInProgress = false;
        m_context->catalog->StopSearchMusic();
        return;
    }

    m_searchPathList.clear();

    if(useWizard)
        doSearch = IntroductionWizard(&m_searchPathList, m_context->aps);
    else
        doSearch = (0 < DialogBoxParam(g_hinst, 
                          MAKEINTRESOURCE(IDD_MUSICSEARCH),
                          m_hWnd, 
                          ::MusicSearchDlgProc, 
                          (LPARAM )this));

    if(doSearch)
    {
        
        m_context->catalog->SearchMusic(m_searchPathList);

        m_bSearchInProgress = true;
        hMenu = GetMenu(m_hWnd);
        hMenu = GetSubMenu(hMenu, 0);
        sItem.cbSize = sizeof(MENUITEMINFO);
        sItem.fMask = MIIM_TYPE;
        sItem.fType = MFT_STRING;
        sItem.dwTypeData = "Stop &Music Search";
        sItem.cch = strlen(sItem.dwTypeData);
        SetMenuItemInfo(hMenu, ID_FILE_SEARCHFORMUSIC, false, &sItem);
    }
}

void MusicBrowserUI::ExportPlaylistEvent()
{
    TV_ITEM tv_item;

    // get the first playlist item
    tv_item.hItem = TreeView_GetChild(m_hMusicView, m_hPlaylistItem);
    tv_item.mask = TVIF_STATE|TVIF_PARAM;
    tv_item.stateMask = TVIS_SELECTED;
    tv_item.state = 0;

    // skip the "Create New Playlist..." item
    tv_item.hItem = TreeView_GetNextSibling(m_hMusicView, tv_item.hItem);


    if(tv_item.hItem)
    {
        BOOL result = FALSE;

        // iterate the items and see if any are selected.
        // for now we only grab the first one on an export
        // but i need to figure out a good way to allow a
        // user to export multiple items since we allow
        // multi-select in the treeview.
        do
        {
            result = TreeView_GetItem(m_hMusicView, &tv_item);

            if(result && (tv_item.state & TVIS_SELECTED))
            {
                string playlistPath;

                TreeData* treedata = (TreeData*)tv_item.lParam;

                if(treedata)
                {
                    playlistPath = treedata->m_oPlaylistPath;

                    ExportPlaylist(playlistPath);
                }
                break;
            }
            
        }while(result && 
               (tv_item.hItem = TreeView_GetNextSibling(m_hMusicView, 
                                                        tv_item.hItem)));
    }
}

const char* kMultipleArtists =  "<Multiple Artists Selected>";
const char* kMultipleAlbums =   "<Multiple Albums Selected>";
const char* kMultipleGenres =   "<Multiple Genres Selected>";
const char* kMultipleTracks =   "<Multiple Tracks Selected>";
const char* kMultipleComments = "<Enter a new comment for all selected tracks.>";

void MusicBrowserUI::EditInfoEvent()
{
    vector<PlaylistItem*> items;

    if(m_hPlaylistView == GetFocus())
        GetSelectedPlaylistItems(&items); 
    else if(m_hMusicView == GetFocus())
        GetSelectedMusicTreeItems(&items); 

    if(items.size())
    {
        EditInfo(items);
    }
}

void MusicBrowserUI::EditInfo(vector<PlaylistItem*>& items, HWND hwnd)
{
    MetaData metadata;

    metadata = items[0]->GetMetaData();

    bool sameArtist = true;
    bool sameAlbum = true;
    bool sameGenre = true;
    bool sameYear = true;
    bool samePlayCount = true;
    vector<PlaylistItem*>::iterator track;

    for(track = items.begin(); track != items.end(); track++)
    {
        MetaData metadataCheck = (*track)->GetMetaData();

        if(metadata.Artist() != metadataCheck.Artist())
            sameArtist = false;

        if(metadata.Album() != metadataCheck.Album())
            sameAlbum = false;

        if(metadata.Genre() != metadataCheck.Genre())
            sameGenre = false;

        if(metadata.Year() != metadataCheck.Year())
            sameYear = false;

        if(metadata.PlayCount() != metadataCheck.PlayCount())
            samePlayCount = false;
    }

    if(!sameArtist)
        metadata.SetArtist(kMultipleArtists);

    if(!sameAlbum)
        metadata.SetAlbum(kMultipleAlbums);

    if(!sameYear)
        metadata.SetYear(-1);

    if(!sameGenre)
        metadata.SetGenre(kMultipleGenres);

    if(!samePlayCount)
        metadata.SetPlayCount(-1);

    char location[MAX_PATH];

    if(items.size() > 1)
    {
        metadata.SetTitle(kMultipleTracks);
        metadata.SetTrack(-1);
        metadata.SetComment(kMultipleComments);
        strcpy(location, kMultipleTracks);
    }
    else
    {
        uint32 size = sizeof(location);
        if(IsError(URLToFilePath(items[0]->URL().c_str(), location, &size)))
            strcpy(location, items[0]->URL().c_str());

    }

    EditTrackInfoDialog editTrackInfo(m_context,
                                      (HINSTANCE)GetWindowLong(m_hWnd, GWL_HINSTANCE),  
                                      (hwnd ? hwnd : m_hWnd),
                                      m_context->catalog->GetMusicList(),
                                      &metadata,
                                      location);
    if(editTrackInfo.Show())
    {
        for(track = items.begin(); track != items.end(); track++)
        {
            MetaData oldMetaData, newMetaData;

            oldMetaData = newMetaData = (*track)->GetMetaData();

            if(metadata.Artist() != kMultipleArtists)
                newMetaData.SetArtist(metadata.Artist().c_str());

            if(metadata.Album() != kMultipleAlbums)
                newMetaData.SetAlbum(metadata.Album().c_str());

            if(metadata.Title() != kMultipleTracks)
                newMetaData.SetTitle(metadata.Title().c_str());

            if(metadata.Genre() != kMultipleGenres)
                newMetaData.SetGenre(metadata.Genre().c_str());

            if(metadata.Comment() != kMultipleComments)
                newMetaData.SetComment(metadata.Comment().c_str());

            if(metadata.Year() != -1)
               newMetaData.SetYear(metadata.Year());

            if(metadata.Track() != -1)
                newMetaData.SetTrack(metadata.Track());

            if(metadata.PlayCount() != -1)
                newMetaData.SetPlayCount(metadata.PlayCount());

            if(newMetaData != oldMetaData)
            {
                (*track)->SetMetaData(&newMetaData);

                m_context->catalog->UpdateSong(*track);

                m_plm->UpdateTrackMetaData(*track, true);
            }
        } 
    }
}

void MusicBrowserUI::EjectCDEvent()
{
    char mciCommand[128];
    char mciReturn[128];

    sprintf(mciCommand, "sysinfo cdaudio quantity wait");

    mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);
	
    if (atoi(mciReturn) <= 0)
        return;

    sprintf(mciCommand, "open cdaudio shareable alias cd1 wait");
    mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);

    sprintf(mciCommand, "set cd1 door open");
    mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);

    sprintf(mciCommand, "close cd1 wait");
    mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);
}

void MusicBrowserUI::NewFavoriteEvent()
{
    FavoriteDialog favoriteDialog(m_context,
                                  m_hWnd,
                                  NULL);

    favoriteDialog.Show();
}

void MusicBrowserUI::AddFavoriteEvent()
{
    HWND hwndFocus = GetFocus();

    if(hwndFocus == m_hMusicView)
    {
        vector<PlaylistItem*> items;

        GetSelectedStreamItems(&items);

        if(items.size())
        {
            vector<PlaylistItem*>::iterator i = items.begin();
        
            for(; i != items.end(); i++)
            {
                m_context->catalog->WriteMetaDataToDatabase((*i)->URL().c_str(),
                                                            (*i)->GetMetaData(),
                                                            kTypeStream);

                m_context->catalog->AddStream((*i)->URL().c_str());
            }
        }
    }
    else if(hwndFocus == m_hPlaylistView)
    {
        uint32 count = ListView_GetItemCount(m_hPlaylistView);
        uint32 selected = ListView_GetSelectedCount(m_hPlaylistView);

        uint32 index = 0;
        uint32 found = 0;

        for(index = 0; index < count, found < selected; index++)
        {
            uint32 state = ListView_GetItemState(m_hPlaylistView, 
                                                 index, 
                                                 LVIS_SELECTED);
            if(state & LVIS_SELECTED)
            {
                found++;

                PlaylistItem* item = m_plm->ItemAt(index);

                if(!item->URL().compare(0, 4, "http"))
                {
                    m_context->catalog->WriteMetaDataToDatabase(item->URL().c_str(),
                                                                item->GetMetaData(),
                                                                kTypeStream);

                    m_context->catalog->AddStream(item->URL().c_str()); 
                }
            }
        }
    }
}

void MusicBrowserUI::RemoveFavoriteEvent()
{
    vector<PlaylistItem*> items;

    GetSelectedFavoritesItems(&items);

    if(items.size())
    {
        vector<PlaylistItem*>::iterator i = items.begin();
        
        for(; i != items.end(); i++)
        {
            m_context->catalog->RemoveStream((*i)->URL().c_str());
        }
    }
}

void MusicBrowserUI::EditStreamInfoEvent()
{
    vector<PlaylistItem*> items;

    GetSelectedFavoritesItems(&items);

    if(items.size())
    {
        FavoriteDialog favoriteDialog(m_context,
                                      m_hWnd,
                                      items[0]);

        favoriteDialog.Show();  
    }
}

int32 MusicBrowserUI::Notify(WPARAM command, NMHDR *pHdr)
{
    int32 result = 0;

	TOOLTIPTEXT *pToolTipText;
    
    
    if (pHdr->idFrom == IDC_MUSICTREE)
    {
        NM_TREEVIEW* pTreeView = (NM_TREEVIEW*)pHdr;

        switch(pTreeView->hdr.code)
        {
            case TVN_DELETEITEM:
            {
                TreeData* treedata = (TreeData*)pTreeView->itemOld.lParam;

                if(treedata)
                    delete treedata; 

                break;
            }

            case TVN_BEGINDRAG:
            {
                TVBeginDrag(m_hMusicView, pTreeView);
                break;
            }

            case TVN_KEYDOWN:
            {
                TV_KEYDOWN* pnkd = (TV_KEYDOWN*)pHdr; 

                if(pnkd->wVKey == VK_DELETE)
                    RemoveEvent();
                else if(pnkd->wVKey == VK_F2)
                {
                    HTREEITEM item = TreeView_GetSelection(m_hMusicView);

                    if(item &&
                       item != m_hMyMusicItem &&
                       item != m_hPlaylistItem &&
                       item != m_hAllItem &&
                       item != m_hUncatItem &&
                       item != m_hNewPlaylistItem &&
                       item != m_hPortableItem &&
                       item != m_hStreamsItem &&
                       item != m_hCDItem &&
                       item != m_hFavoritesItem &&
                       item != m_hNewFavoritesItem &&
                       TreeView_GetParent(m_hMusicView, item) != m_hStreamsItem &&
                       TreeView_GetParent(m_hMusicView,TreeView_GetParent(m_hMusicView, item)) != m_hStreamsItem)
                        TreeView_EditLabel(m_hMusicView, item);
                }

                break;
            }

            case TVN_BEGINLABELEDIT:
            {
                TV_DISPINFO* info = (TV_DISPINFO*)pHdr;
                HTREEITEM item = info->item.hItem;

                if(item == m_hMyMusicItem ||
                   item == m_hPlaylistItem ||
                   item == m_hAllItem ||
                   item == m_hUncatItem ||
                   item == m_hNewPlaylistItem ||
                   item == m_hPortableItem ||
                   TreeView_GetParent(m_hMusicView, item) == m_hPortableItem)
                {
                    result = TRUE;
                }

                break;
            }

            case TVN_ENDLABELEDIT:
            {
                TV_DISPINFO* info = (TV_DISPINFO*)pHdr;
                TV_ITEM tv_item = info->item;
                TreeData* treedata = (TreeData*)tv_item.lParam;
                
                if(treedata)
                {
                    // was the operation cancelled?
                    if(tv_item.pszText)
                    {
                        if(treedata->IsTrack())
                        {
                            // just change the title for this song
                            UpdateTrackName(treedata->m_pTrack, 
                                            tv_item.pszText);
                        } 
                        else if(treedata->IsPlaylist())
                        {
                            // just change the title for this playlist
                            UpdatePlaylistName(treedata->m_oPlaylistPath, 
                                               tv_item.pszText);
                        }
                        else if(treedata->IsAlbum())
                        {
                            // need to change the album for all tracks in album
                            UpdateAlbumName(treedata->m_pAlbum, 
                                            tv_item.pszText);
                        }
                        else if(treedata->IsArtist())
                        {
                            // need to change the artist for all albums
                            // and tracks by this artist
                            UpdateArtistName(treedata->m_pArtist, 
                                             tv_item.pszText);
                        }
                        else if(treedata->IsUncatagorized())
                        {
                            // just change the title for this song
                            UpdateUncatagorizedTrackName(treedata->m_pTrack, 
                                            tv_item.pszText);
                        }                        
                    }

                    if(treedata->IsTrack())
                    {
                        ostringstream ost;
    
                        if(treedata->m_pTrack->GetMetaData().Track() == 0)
                            ost << "?. ";
                        else            
                            ost << treedata->m_pTrack->GetMetaData().Track() << ". ";

                        if(tv_item.pszText)
                            ost << tv_item.pszText;
                        else
                            ost << treedata->m_pTrack->GetMetaData().Title();

                        string title = ost.str();
                    
                        if(!tv_item.pszText &&
                           treedata->m_pTrack->GetMetaData().Title() == string(" ") ||
                           treedata->m_pTrack->GetMetaData().Title().length() == 0)
                            tv_item.pszText = "Unknown";
                        else    
                            tv_item.pszText = (char *)(title.c_str());
                    
                        tv_item.mask = TVIF_HANDLE | TVIF_TEXT;
                        tv_item.cchTextMax = strlen(tv_item.pszText);

                        TreeView_SetItem(m_hMusicView, &tv_item);
                        result = FALSE;
                    }
                }
                
                break;
            }

            case TVN_ITEMEXPANDING:
            {
                if(!TreeView_GetChild(m_hMusicView, 
                                      pTreeView->itemNew.hItem))
                {
                    TreeData* treedata = (TreeData*)pTreeView->itemNew.lParam;

                    if(pTreeView->itemNew.hItem == m_hMyMusicItem)
                    {
                        FillArtists();
                    }
                    else if(pTreeView->itemNew.hItem == m_hAllItem)
                    {
                        m_fillAllThread = Thread::CreateThread();
	                    m_fillAllThread->Create(MusicBrowserUI::fill_all_tracks, this);
                    }
                    else if(pTreeView->itemNew.hItem == m_hUncatItem)
                    {
                        FillUncatTracks();
                    }
                    else if(treedata && treedata->GetLevel() == 1)
                    {
                        FillAlbums(&pTreeView->itemNew);
                    }
                    else if(treedata && treedata->GetLevel() == 2)
                    {
                        FillTracks(&pTreeView->itemNew);
                    }
                    else if(pTreeView->itemNew.hItem == m_hPlaylistItem)
                    {
                        FillPlaylists();
                    }
                    else if(pTreeView->itemNew.hItem == m_hStreamsItem)
                    {
                        FillStreams();
                    }
                    else if(pTreeView->itemNew.hItem == m_hFavoritesItem)
                    {
                        FillFavorites();
                    }
                    else if(pTreeView->itemNew.hItem == m_hPortableItem)
                    {
                        FillPortables();
                    }
                }

                break;
            }

            case TVN_SELCHANGED:
            {
                UpdateButtonStates();
                break;
            }

            case NM_DBLCLK:
            {
                /*int32 lParam;
                lParam = GetCurrentItemFromMousePos();

                TV_ITEM tv_item;
                TV_HITTESTINFO tv_htinfo;


                GetCursorPos(&tv_htinfo.pt);
                ScreenToClient(m_hMusicView, &tv_htinfo.pt);

                if(TreeView_HitTest(m_hMusicView, &tv_htinfo))
                {
                    tv_item.hItem = TreeView_GetSelection(m_hMusicView); 
                    tv_item.mask = TVIF_PARAM | TVIF_HANDLE;
                    TreeView_GetItem(m_hMusicView, &tv_item);

                    TreeData* treedata = (TreeData*)tv_item.lParam;

                    if(treedata->IsTrack())
                    {
                        PlaylistItem *item;
                
                        item = new PlaylistItem(*treedata->m_pTrack);
                        m_plm->AddItem(item, false);
                    } 
                    else if(treedata->IsPlaylist())
                    {
                        m_plm->ReadPlaylist(treedata->m_oPlaylistPath.c_str());
                    }
                    else if(treedata->IsPortable())
                    {
                        EditPortablePlaylist(treedata->m_pPortable);
                    }
                    else if(tv_htinfo.hItem == m_hNewPlaylistItem)
                    {
                        NewPlaylist();
                    }
                    else if(tv_htinfo.hItem == m_hNewPortableItem)
                    {
                        m_context->target->AcceptEvent(new ShowPreferencesEvent(3));
                    }
                }*/
                break;
            }

            case NM_RCLICK:
            {
                HMENU mainMenu, cdMenu, streamMenu;
                HMENU subMenu;
                POINT sPoint;
                uint32 trackCount = 0;
                uint32 playlistCount = 0;

                trackCount = GetSelectedTrackCount();
                playlistCount = GetSelectedPlaylistCount();
            
                GetCursorPos(&sPoint);

                TV_HITTESTINFO tv_htinfo;

                tv_htinfo.pt = sPoint;

                ScreenToClient(m_hMusicView, &tv_htinfo.pt);
                TreeView_HitTest(m_hMusicView, &tv_htinfo);

                mainMenu = LoadMenu(g_hinst, MAKEINTRESOURCE(IDR_TVPOPUP));
                cdMenu = LoadMenu(g_hinst, MAKEINTRESOURCE(IDR_TVPOPUP1));
                streamMenu = LoadMenu(g_hinst, MAKEINTRESOURCE(IDR_TVPOPUP2));


                if(tv_htinfo.hItem == m_hStreamsItem || 
                   TreeView_GetParent(m_hMusicView, tv_htinfo.hItem) == m_hStreamsItem ||
                   TreeView_GetParent(m_hMusicView, TreeView_GetParent(m_hMusicView, tv_htinfo.hItem)) == m_hStreamsItem)
                {
                    subMenu = GetSubMenu(streamMenu, 0);

                    if(tv_htinfo.hItem == m_hStreamsItem)
                    {
                        EnableMenuItem(subMenu, ID_POPUP_ADDTRACK, MF_BYCOMMAND|MF_GRAYED);
                        EnableMenuItem(subMenu, ID_POPUP_ADDTRACK_PLAY, MF_BYCOMMAND|MF_GRAYED);
                        EnableMenuItem(subMenu, ID_POPUP_FAVORITE, MF_BYCOMMAND|MF_GRAYED);
                        EnableMenuItem(subMenu, ID_POPUP_VISITWEB, MF_BYCOMMAND|MF_GRAYED);
                    }

                    if(tv_htinfo.hItem == m_hFavoritesItem || 
                       TreeView_GetParent(m_hMusicView, tv_htinfo.hItem) == m_hFavoritesItem)
                    {
                        DeleteMenu(subMenu, 2, MF_BYPOSITION);
                        DeleteMenu(subMenu, 2, MF_BYPOSITION);
                        DeleteMenu(subMenu, ID_POPUP_VISITWEB, MF_BYCOMMAND);

                        if(tv_htinfo.hItem == m_hFavoritesItem ||
                           tv_htinfo.hItem == m_hNewFavoritesItem)
                        {
                            EnableMenuItem(subMenu, ID_POPUP_RENAME, MF_BYCOMMAND|MF_GRAYED);
                            EnableMenuItem(subMenu, ID_POPUP_REMOVEFAVORITE, MF_BYCOMMAND|MF_GRAYED);
                            EnableMenuItem(subMenu, ID_POPUP_EDITSTREAMINFO, MF_BYCOMMAND|MF_GRAYED);

                            if(tv_htinfo.hItem == m_hNewFavoritesItem)
                            {
                                EnableMenuItem(subMenu, ID_POPUP_ADDTRACK, MF_BYCOMMAND|MF_GRAYED);
                                EnableMenuItem(subMenu, ID_POPUP_ADDTRACK_PLAY, MF_BYCOMMAND|MF_GRAYED);
                            }
                        }
                    }
                    else
                    {
                        DeleteMenu(subMenu, 4, MF_BYPOSITION);
                        DeleteMenu(subMenu, 4, MF_BYPOSITION);
                        DeleteMenu(subMenu, 4, MF_BYPOSITION);
                        DeleteMenu(subMenu, 4, MF_BYPOSITION);
                        DeleteMenu(subMenu, 4, MF_BYPOSITION);
                    }
                }
                else if(tv_htinfo.hItem == m_hCDItem ||
                        TreeView_GetParent(m_hMusicView, tv_htinfo.hItem) == m_hCDItem)
                {
                    subMenu = GetSubMenu(cdMenu, 0);
                }
                else
                {
                    subMenu = GetSubMenu(mainMenu, 0);

                    if(m_pParent)
                    {
                        DeleteMenu(subMenu, ID_POPUP_ADDTRACK_PLAY, MF_BYCOMMAND);
                    }

                    if( IsItemSelected(m_hMyMusicItem) ||
                        IsItemSelected(m_hPlaylistItem) ||
                        IsItemSelected(m_hAllItem) ||
                        IsItemSelected(m_hUncatItem) ||
                        (IsItemSelected(m_hNewPlaylistItem) && !(playlistCount + trackCount)))
                    {
                        EnableMenuItem(subMenu,
                                       ID_POPUP_REMOVE,
                                       MF_BYCOMMAND|MF_GRAYED);
                        EnableMenuItem(subMenu,
                                       ID_BITZI_LOOKUP,
                                       MF_BYCOMMAND|MF_GRAYED);
                    }


                    if( playlistCount > 1 ||
                        IsItemSelected(m_hMyMusicItem) ||
                        IsItemSelected(m_hPlaylistItem) ||
                        IsItemSelected(m_hAllItem) ||
                        IsItemSelected(m_hUncatItem) ||
                        (IsItemSelected(m_hNewPlaylistItem) && !(playlistCount + trackCount)))
                    {
                        EnableMenuItem(subMenu,
                                       ID_POPUP_EDITINFO,
                                       MF_BYCOMMAND|MF_GRAYED);    
                        EnableMenuItem(subMenu,
                                       ID_BITZI_LOOKUP,
                                       MF_BYCOMMAND|MF_GRAYED);
                    }


                    if( trackCount > 1 ||
                        playlistCount > 1 || (trackCount && playlistCount) ||
                        IsItemSelected(m_hMyMusicItem) ||
                        IsItemSelected(m_hPlaylistItem) ||
                        IsItemSelected(m_hAllItem) ||
                        IsItemSelected(m_hUncatItem) ||
                        IsItemSelected(m_hNewPlaylistItem))
                    {
                        EnableMenuItem(subMenu,
                                       ID_POPUP_RENAME,
                                       MF_BYCOMMAND|MF_GRAYED);                         
                        EnableMenuItem(subMenu,
                                       ID_BITZI_LOOKUP,
                                       MF_BYCOMMAND|MF_GRAYED);
                    }

                    if(trackCount == 0)
                    {
                        EnableMenuItem(subMenu,
                                       ID_POPUP_EDITINFO,
                                       MF_BYCOMMAND|MF_GRAYED);
                        EnableMenuItem(subMenu,
                                       ID_BITZI_LOOKUP,
                                       MF_BYCOMMAND|MF_GRAYED);

                        if( IsItemSelected(m_hNewPlaylistItem) &&
                            playlistCount == 1)
                        {
                            EnableMenuItem(subMenu,
                                       ID_POPUP_ADDTRACK,
                                       MF_BYCOMMAND|MF_GRAYED);

                            EnableMenuItem(subMenu,
                                       ID_POPUP_ADDTRACK_PLAY,
                                       MF_BYCOMMAND|MF_GRAYED);

                            EnableMenuItem(subMenu,
                                       ID_POPUP_REMOVE,
                                       MF_BYCOMMAND|MF_GRAYED);

                            EnableMenuItem(subMenu,
                                       ID_POPUP_RENAME,
                                       MF_BYCOMMAND|MF_GRAYED);

                            EnableMenuItem(subMenu,
                                       ID_POPUP_EDITPLAYLIST,
                                       MF_BYCOMMAND|MF_GRAYED);
                        }
                    }

                    if(!playlistCount || trackCount)
                    {
                        DeleteMenu(subMenu, ID_POPUP_EDITPLAYLIST, MF_BYCOMMAND);
                    }
                }

                TrackPopupMenu(subMenu, 
                               TPM_LEFTALIGN|TPM_RIGHTBUTTON,
                               sPoint.x, sPoint.y, 
                               0, m_hWnd, NULL);

                DestroyMenu(mainMenu);
                DestroyMenu(cdMenu);
                DestroyMenu(streamMenu);

                break;
            }

            /*case ??:
            {
                TV_HITTESTINFO sHit;
                HTREEITEM      hItem;
                POINT          sPoint;
            
                sHit.flags = TVHT_ONITEM;
          
                GetCursorPos(&sPoint);
                ScreenToClient(m_hWnd, &sPoint);
                ClientToWindow(m_hWnd, &sPoint); 
                sHit.pt = sPoint;
                hItem = TreeView_HitTest(m_hMusicView, &sHit);
                if (hItem == m_hPlaylistItem)
                   SendMessage(m_hStatus, SB_SETTEXT, 0, 
                               (LPARAM)"This tree item contains all of your playlists.");
                else               
                if (hItem == m_hMyMusicItem)
                   SendMessage(m_hStatus, SB_SETTEXT, 0, 
                               (LPARAM)"This tree item contains all of your music.");
                else               
                if (hItem == m_hAllItem)
                   SendMessage(m_hStatus, SB_SETTEXT, 0, 
                               (LPARAM)"This tree item lists all of your music tracks.");
                else               
                if (hItem == m_hUncatItem)
                   SendMessage(m_hStatus, SB_SETTEXT, 0, 
                               (LPARAM)"This tree item lists all of your uncategorized music tracks.");
                else               
                   SendMessage(m_hStatus, SB_SETTEXT, 0, (LPARAM)"");

                break;
            }*/

        }
    }
    else if(pHdr->idFrom == IDC_PLAYLISTBOX)
    {
        NM_LISTVIEW* pListView = (NM_LISTVIEW*)pHdr;

        switch(pListView->hdr.code)
        {
            case LVN_BEGINDRAG:
            {
                LVBeginDrag(m_hPlaylistView, pListView);
                break;
            }

            case NM_RCLICK:
            {
                HMENU menu;
                HMENU subMenu;
                POINT sPoint;
            
                GetCursorPos(&sPoint);

                menu = LoadMenu(g_hinst, MAKEINTRESOURCE(IDR_LVPOPUP));
                subMenu = GetSubMenu(menu, 0);

                if(m_pParent)
                {
                    DeleteMenu(subMenu, ID_POPUP_PLAY, MF_BYCOMMAND);
                }

                // Can we move items up and down?
                uint32 count = ListView_GetItemCount(m_hPlaylistView);
                uint32 selected = ListView_GetSelectedCount(m_hPlaylistView);

                if(count)
                {
                    uint32 state;

                    EnableMenuItem(subMenu, ID_POPUP_PLAY, MF_BYCOMMAND|
                                   (selected > 1) ? MF_GRAYED : MF_ENABLED );

                    state = ListView_GetItemState(m_hPlaylistView,
                                                  count - 1, 
                                                  LVIS_SELECTED);

                    EnableMenuItem(subMenu, ID_POPUP_MOVEDOWN, MF_BYCOMMAND|
                                   (state & LVIS_SELECTED) ? MF_GRAYED : MF_ENABLED );

                    state = ListView_GetItemState(m_hPlaylistView, 
                                                  0, 
                                                  LVIS_SELECTED);

                    EnableMenuItem(subMenu, ID_POPUP_MOVEUP, MF_BYCOMMAND|
                                   (state & LVIS_SELECTED) ? MF_GRAYED : MF_ENABLED );

                    uint32 index = 0;
                    uint32 found = 0;


                    for(index = 0; index < count, found < selected; index++)
                    {
                        uint32 state = ListView_GetItemState(m_hPlaylistView, 
                                                             index, 
                                                             LVIS_SELECTED);
                        if(state & LVIS_SELECTED)
                        {
                            found++;

                            PlaylistItem* item = m_plm->ItemAt(index);

                            if(!item->URL().compare(0, 4, "http"))
                            {
                                MENUITEMINFO mii;

                                mii.cbSize = sizeof(MENUITEMINFO);
                                mii.fMask = MIIM_ID | MIIM_TYPE;
                                mii.wID = ID_POPUP_FAVORITE;
                                mii.fType = MFT_STRING;
                                mii.dwTypeData = "Add to Favorites";
                                mii.cch = strlen(mii.dwTypeData);

                                InsertMenuItem(subMenu, ID_POPUP_EDITINFO, FALSE, &mii);
                                
                                break;
                            }
                        }
                    }
                }
                else
                {
                    EnableMenuItem(subMenu, ID_POPUP_PLAY, MF_BYCOMMAND|MF_GRAYED);
                    EnableMenuItem(subMenu, ID_POPUP_MOVEUP, MF_BYCOMMAND|MF_GRAYED);
                    EnableMenuItem(subMenu, ID_POPUP_MOVEDOWN, MF_BYCOMMAND|MF_GRAYED);
                    EnableMenuItem(subMenu, ID_POPUP_REMOVE, MF_BYCOMMAND|MF_GRAYED);
                    EnableMenuItem(subMenu, ID_POPUP_EDITINFO, MF_BYCOMMAND|MF_GRAYED);
                }

                TrackPopupMenu(subMenu, 
                               TPM_LEFTALIGN, sPoint.x, sPoint.y, 
                               0, m_hWnd, NULL);

                DestroyMenu(menu);
                break;
            }

            case LVN_ITEMCHANGED:
            {
                // update only when focus is set
                //if(pListView->uNewState&LVIS_FOCUSED)
                    UpdateButtonStates();
                break;
            }

            case NM_DBLCLK:
            {
                // only do this for the root browser
                if(!m_pParent)
                {
                    m_playerEQ->AcceptEvent(new Event(CMD_Stop));
                    m_plm->SetCurrentIndex(pListView->iItem);
                    m_playerEQ->AcceptEvent(new Event(CMD_Play));
                }
                break;
            }

            case LVN_COLUMNCLICK:
            {
                int column = pListView->iSubItem;
                const char *columnText = GetColumnText(column);

                if (stricmp(columnText, ARTIST_COLUMN) == 0)
                {
                    SendMessage(m_hWnd, WM_COMMAND, ID_SORT_ARTIST, 0);
                }
                else if (stricmp(columnText, TITLE_COLUMN) == 0)
                {
                    SendMessage(m_hWnd, WM_COMMAND, ID_SORT_TITLE, 0);
                }
                else if (stricmp(columnText, TRACK_COLUMN) == 0)
                {
                    SendMessage(m_hWnd, WM_COMMAND, ID_SORT_TRACK, 0);
                }
                else if (stricmp(columnText, ALBUM_COLUMN) == 0)
                {
                    SendMessage(m_hWnd, WM_COMMAND, ID_SORT_ALBUM, 0);
                }
                else if (stricmp(columnText, TIME_COLUMN) == 0)
                {
                    SendMessage(m_hWnd, WM_COMMAND, ID_SORT_LENGTH, 0);
                }
                else if (stricmp(columnText, GENRE_COLUMN) == 0)
                {
                    SendMessage(m_hWnd, WM_COMMAND, ID_SORT_GENRE, 0);
                }
                else if (stricmp(columnText, COMMENT_COLUMN) == 0)
                {
                    SendMessage(m_hWnd, WM_COMMAND, ID_SORT_COMMENT, 0);
                }
                else if (stricmp(columnText, YEAR_COLUMN) == 0)
                {
                    SendMessage(m_hWnd, WM_COMMAND, ID_SORT_YEAR, 0);
                }
                else if (stricmp(columnText, LOCATION_COLUMN) == 0)
                {
                    SendMessage(m_hWnd, WM_COMMAND, ID_SORT_LOCATION, 0);
                }
                break;
            }

            case LVN_KEYDOWN:
            {
                LV_KEYDOWN* pnkd = (LV_KEYDOWN*)pHdr; 

                if(pnkd->wVKey == VK_DELETE)
                {
                    RemoveEvent();  
                }
                else if(pnkd->wVKey == 'A' && (GetKeyState(VK_CONTROL) < 0))
                {
                    uint32 count = ListView_GetItemCount(pListView->hdr.hwndFrom);
                
                    for(uint32 index = 0; index < count; index++)
                        ListView_SetItemState(pListView->hdr.hwndFrom,
                                              index,
                                              LVIS_SELECTED,
                                              LVIS_SELECTED);
                }

                break;
            }
        }
    }
    
    if(pHdr->code == TTN_NEEDTEXT)
    {
        pToolTipText = (LPTOOLTIPTEXT)pHdr;

        switch(pToolTipText->hdr.idFrom)
        {
            case ID_FILE_NEWPLAYLIST:
                pToolTipText->lpszText = "Create a new playlist";
                result = TRUE;
                break;
            case ID_FILE_SAVEPLAYLIST:
                pToolTipText->lpszText = "Save current playlist";
                result = TRUE;
                break;
            case ID_FILE_IMPORT:
                pToolTipText->lpszText = "Import tracks and playlists from disk";
                result = TRUE;
                break;
            case ID_EDIT_REMOVE:
                pToolTipText->lpszText = "Remove selected tracks and playlists";
                result = TRUE;
                break;
            case ID_EDIT_EDITINFO:
                pToolTipText->lpszText = "Edit track or playlist info";
                result = TRUE;
                break;
            case ID_EDIT_ADDTRACK:
                pToolTipText->lpszText = "Add selected tracks and playlists to playlist";
                result = TRUE;
                break;
            case ID_EDIT_ADDFILE:
                pToolTipText->lpszText = "Add files from disk to playlist";
                result = TRUE;
                break;
            case ID_EDIT_MOVEUP:
                pToolTipText->lpszText = "Move selected playlist items up";
                result = TRUE;
                break;
            case ID_EDIT_MOVEDOWN:
                pToolTipText->lpszText = "Move selected playlist items down";
                result = TRUE;
                break;
        }      
    }
 
	return result;
}


void MusicBrowserUI::SortEvent(int id)
{
    PlaylistSortKey oldKey, newKey;
    PlaylistSortType type;
    
    if(!m_plm->CountItems())
        return;

    switch(id)
    {
        case ID_SORT_ARTIST:
            newKey = kPlaylistSortKey_Artist;
            break;     
        case ID_SORT_ALBUM:
            newKey = kPlaylistSortKey_Album;
            break;     
        case ID_SORT_TITLE:
            newKey = kPlaylistSortKey_Title;
            break;     
        case ID_SORT_LENGTH:
            newKey = kPlaylistSortKey_Time;
            break;     
        case ID_SORT_YEAR:
            newKey = kPlaylistSortKey_Year;
            break;     
        case ID_SORT_TRACK:
            newKey = kPlaylistSortKey_Track;
            break;     
        case ID_SORT_GENRE:
            newKey = kPlaylistSortKey_Genre;
            break;    
        case ID_SORT_COMMENT:
            newKey = kPlaylistSortKey_Comment;
            break; 
        case ID_SORT_LOCATION:
            newKey = kPlaylistSortKey_Location;
            break;
        case ID_SORT_FILENAME:
            newKey = kPlaylistSortKey_FileName;
            break;  
        case IDC_RANDOMIZE:
        case ID_SORT_RANDOMIZE:
            newKey = kPlaylistSortKey_Random;
            break;     
        default:
             return;
    }

    oldKey = m_plm->GetPlaylistSortKey();

    type = (oldKey == newKey && 
            m_plm->GetPlaylistSortType() == kPlaylistSortType_Ascending)
            ? kPlaylistSortType_Descending : kPlaylistSortType_Ascending;

    ::SetCursor(LoadCursor(NULL, IDC_WAIT));
    m_plm->Sort(newKey, type);
    ::SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void MusicBrowserUI::ToggleVisEvent(void)
{
    Event *e;

    if (m_state == STATE_COLLAPSED)
        e = new Event(CMD_TogglePlaylistUI);
    else
        e = new Event(CMD_ToggleMusicBrowserUI);

    AcceptEvent(e);
    
    delete e;
}

void MusicBrowserUI::PlayNowEvent(void)
{
    if(!m_pParent)
    {
        uint32 count = ListView_GetItemCount(m_hPlaylistView);
        uint32 index = 0;

        for(index = 0; index < count; index++)
        {
            uint32 state = ListView_GetItemState(m_hPlaylistView, 
                                                 index, 
                                                 LVIS_SELECTED);
            if(state & LVIS_SELECTED)
            {
                // only do this for the root browser
                m_playerEQ->AcceptEvent(new Event(CMD_Stop));
                m_plm->SetCurrentIndex(index);
                m_playerEQ->AcceptEvent(new Event(CMD_Play));
                break;
            }
        }
    }
}

void MusicBrowserUI::VisitWebEvent(void)
{
    vector<PlaylistItem*> items;

    GetSelectedStreamItems(&items);
	if (items.size() <= 0 || items[0]->WebURL().size() == 0)
		return;

	ShellExecute(m_hParent, "open", items[0]->WebURL().c_str(), 
		         NULL, NULL, SW_SHOWNORMAL);
}

void MusicBrowserUI::AddTrackEvent(void)
{
    vector<PlaylistItem*> items;
    vector<string> urls;

    GetSelectedMusicTreeItems(&items);   
    
    vector<PlaylistItem*>::iterator i;

    for(i = items.begin(); i != items.end(); i++)
    {
        urls.push_back((*i)->URL().c_str());
    }

    GetSelectedPlaylistItems(&urls);

    items.clear();

    GetSelectedFavoritesItems(&items);
    GetSelectedStreamItems(&items);

    for(i = items.begin(); i != items.end(); i++)
    {
        urls.push_back((*i)->URL().c_str());
    }

    GetSelectedCDItems(&urls);

    // we know that we are gonna be adding a 
    // bunch of items so let windows know.
    // it will make the adds more efficient
    uint32 newSize = ListView_GetItemCount(m_hPlaylistView);
    newSize += urls.size();
    ListView_SetItemCount(m_hPlaylistView, newSize);

    m_plm->AddItems(urls);
    
}

void MusicBrowserUI::AddTrackAndPlayEvent(void)
{
    uint32 count = m_plm->CountItems();

    AddTrackEvent();

    if(m_plm->CountItems())
    {
		m_context->target->AcceptEvent(new Event(CMD_Stop));
        m_plm->SetCurrentIndex(count);
        m_context->target->AcceptEvent(new Event(CMD_Play));
    }
}

void MusicBrowserUI::AddFileEvent(HWND hwndParent, bool playNow)
{
    PlaylistFormatInfo format;
    int32 i, iOffset = 0;

    char szFilter[1024] = "MPEG Audio Streams (.mp1;.mp2;.mp3;.mpp)\0"
                          "*.mp1;*.mp2;*.mp3;*.mpp\0"
                          "Ogg/Vorbis Streams (.ogg)\0"
                          "*.ogg\0"
                          "WAV Files (.wav)\0"
                          "*.wav\0";

    // we need a way to iterate LMCs...
    iOffset += strlen(szFilter) + 1; 
    iOffset += strlen(szFilter + iOffset) + 1;  
    iOffset += strlen(szFilter + iOffset) + 1;  
    iOffset += strlen(szFilter + iOffset) + 1;  

    for(i = 0; ; i++)
    {
       if (m_plm->GetSupportedPlaylistFormats(&format, i) != kError_NoErr)
          break;
    
       sprintf(szFilter + iOffset, "%s (.%s)", 
            format.GetDescription(),
            format.GetExtension());
       iOffset += strlen(szFilter + iOffset) + 1;     

       sprintf(szFilter + iOffset, "*.%s", 
            format.GetExtension());
       iOffset += strlen(szFilter + iOffset) + 1;     
    }
    
    strcpy(szFilter + iOffset, "All Files (*.*)\0");
    iOffset += strlen(szFilter + iOffset) + 1;     
    strcpy(szFilter + iOffset, "*.*\0");
    iOffset += strlen(szFilter + iOffset) + 1;     
    szFilter[iOffset] = 0;
    
    vector<string> oFileList;

    if (FileOpenDialog(hwndParent, 
                       "Add Tracks and Playlists",
                       szFilter, 
                       &oFileList,
                       true) )
    {
        // we know that we are gonna be adding a 
        // bunch of items so let windows know.
        // it will make the adds more efficient
        uint32 newSize = ListView_GetItemCount(m_hPlaylistView);
        newSize += oFileList.size();
        ListView_SetItemCount(m_hPlaylistView, newSize);

        uint32 initialCount = m_context->plm->CountItems();

        if(playNow)
        {
            m_context->target->AcceptEvent(new Event(CMD_Stop));
            m_context->plm->RemoveAll();
        }

        m_plm->AddItems(oFileList);

        if(playNow || !initialCount)
        {
            m_context->target->AcceptEvent(new Event(CMD_Play));
        }
    }
}

void MusicBrowserUI::EmptyDBCheck(void)
{    
    bool welcome = false;
    bool checkDB = true;

    m_context->prefs->GetPrefBoolean(kWelcomePref, &welcome);
    m_context->prefs->GetPrefBoolean(kPerformDBCheckPref, &checkDB);

    if ((m_context->catalog->GetPlaylists()->size() > 0 ||
        m_context->catalog->GetMusicList()->size() > 0 ||
        m_context->catalog->GetUnsortedMusic()->size() > 0) && 
        welcome == false)
        return;

    if(welcome)
    {
        m_context->prefs->SetPrefBoolean(kWelcomePref, false);
        StartStopMusicSearch(true); 
    }
    else if(checkDB)
    {
        if(0 < DialogBoxParam(g_hinst, 
                              MAKEINTRESOURCE(IDD_CHECKDB),
                              m_hWnd, 
                              ::PerformDBCheckDlgProc, 
                              (LPARAM)&checkDB))
        {     
            StartStopMusicSearch();
        }

        m_context->prefs->SetPrefBoolean(kPerformDBCheckPref, checkDB);
    }
}

void MusicBrowserUI::EditPlaylistEvent(void)
{
    TV_ITEM tv_item;

    // get the first playlist item
    tv_item.hItem = TreeView_GetChild(m_hMusicView, m_hPlaylistItem);
    tv_item.mask = TVIF_STATE|TVIF_PARAM;
    tv_item.stateMask = TVIS_SELECTED;
    tv_item.state = 0;

    // skip the "Create New Playlist..." item
    tv_item.hItem = TreeView_GetNextSibling(m_hMusicView, tv_item.hItem);

    if(tv_item.hItem)
    {
        BOOL result = FALSE;

        do
        {
            result = TreeView_GetItem(m_hMusicView, &tv_item);

            if(result && (tv_item.state & TVIS_SELECTED))
            {
                string playlistPath;

                TreeData* treedata = (TreeData*)tv_item.lParam;

                if(treedata)
                {
                    playlistPath = treedata->m_oPlaylistPath;

                    EditPlaylist(playlistPath);
                }
            }
            
        }while(result && 
               (tv_item.hItem = TreeView_GetNextSibling(m_hMusicView, 
                                                        tv_item.hItem)));
    }
}

void MusicBrowserUI::PlayerControlsEvent(int command)
{
    switch(command)
    {
        case ID_CONTROLS_PLAYPAUSE:

            if(m_playerState == PLAYERSTATE_PLAYING)
                m_context->target->AcceptEvent(new Event(CMD_Pause));
            else
                m_context->target->AcceptEvent(new Event(CMD_Play));

            break;

        case ID_CONTROLS_STOP:
            m_context->target->AcceptEvent(new Event(CMD_Stop));
            break;

        case ID_CONTROLS_PREVIOUSSONG:
            m_context->target->AcceptEvent(new Event(CMD_PrevMediaPiece));
            break;

        case ID_CONTROLS_NEXTSONG:
            m_context->target->AcceptEvent(new Event(CMD_NextMediaPiece));
            break;

        case ID_CONTROLS_NORMALORDER:
            m_plm->SetShuffleMode(false);
            break;

        case ID_CONTROLS_SHUFFLE:
            m_plm->SetShuffleMode(true);
            break;

        case ID_CONTROLS_REPEATNONE:
            m_plm->SetRepeatMode(kPlaylistMode_RepeatNone);
            break;
        case ID_CONTROLS_REPEATONE:
            m_plm->SetRepeatMode(kPlaylistMode_RepeatOne);
            break;
        case ID_CONTROLS_REPEATALL:
            m_plm->SetRepeatMode(kPlaylistMode_RepeatAll);
            break;
    }
}

void MusicBrowserUI::ChangePlayerState(int32 event)
{
    HMENU           menu;
    MENUITEMINFO    item;

    menu = GetMenu(m_hWnd);
    menu = GetSubMenu(menu, 3);
    item.cbSize = sizeof(MENUITEMINFO);
    item.fMask = MIIM_TYPE;
    item.fType = MFT_STRING;

    switch(event) 
    {
        case INFO_Playing: 
        {   
            m_playerState = PLAYERSTATE_PLAYING;
            
            item.dwTypeData = "Pa&use the Current Track";
            item.cch = strlen(item.dwTypeData);
            SetMenuItemInfo(menu, ID_CONTROLS_PLAYPAUSE, false, &item);
                   
	        break; 
        }

        case INFO_Paused: 
        case INFO_Stopped: 
        {
            if(event == INFO_Paused)
                m_playerState = PLAYERSTATE_PAUSED;
            else
                m_playerState = PLAYERSTATE_STOPPED;

            item.dwTypeData = "&Play the Current Track";
            item.cch = strlen(item.dwTypeData);
            SetMenuItemInfo(menu, ID_CONTROLS_PLAYPAUSE, false, &item);

	        break; 
        }
    }
}

void MusicBrowserUI::ChangeShuffleMode(bool shuffled)
{
    HMENU           menu;
    MENUITEMINFO    item;

    menu = GetMenu(m_hWnd);
    menu = GetSubMenu(menu, 3);
    item.cbSize = sizeof(MENUITEMINFO);
    item.fMask = MIIM_STATE;
    item.fState = shuffled ? MFS_CHECKED:MFS_UNCHECKED;
    SetMenuItemInfo(menu, ID_CONTROLS_SHUFFLE, false, &item);
    
    item.fState = shuffled ? MFS_UNCHECKED: MFS_CHECKED;
    SetMenuItemInfo(menu, ID_CONTROLS_NORMALORDER, false, &item);
}

void MusicBrowserUI::ChangeRepeatMode(RepeatMode mode)
{
    HMENU           menu;
    MENUITEMINFO    item;

    menu = GetMenu(m_hWnd);
    menu = GetSubMenu(menu, 3);
    item.fMask = MIIM_STATE;

    item.cbSize = sizeof(MENUITEMINFO);

	switch(mode) 
    {
		case kPlaylistMode_RepeatNone:
            item.fState = MFS_CHECKED;
            SetMenuItemInfo(menu, ID_CONTROLS_REPEATNONE, false, &item);
            item.fState = MFS_UNCHECKED;
            SetMenuItemInfo(menu, ID_CONTROLS_REPEATONE, false, &item);
            item.fState = MFS_UNCHECKED;
            SetMenuItemInfo(menu, ID_CONTROLS_REPEATALL, false, &item);
			break;

		case kPlaylistMode_RepeatOne:
            item.fState = MFS_UNCHECKED;
            SetMenuItemInfo(menu, ID_CONTROLS_REPEATNONE, false, &item);
            item.fState = MFS_CHECKED;
            SetMenuItemInfo(menu, ID_CONTROLS_REPEATONE, false, &item);
            item.fState = MFS_UNCHECKED;
            SetMenuItemInfo(menu, ID_CONTROLS_REPEATALL, false, &item);
			break;

		case kPlaylistMode_RepeatAll:
            item.fState = MFS_UNCHECKED;
            SetMenuItemInfo(menu, ID_CONTROLS_REPEATNONE, false, &item);
            item.fState = MFS_UNCHECKED;
            SetMenuItemInfo(menu, ID_CONTROLS_REPEATONE, false, &item);
            item.fState = MFS_CHECKED;
            SetMenuItemInfo(menu, ID_CONTROLS_REPEATALL, false, &item);
            break;
	}

}
