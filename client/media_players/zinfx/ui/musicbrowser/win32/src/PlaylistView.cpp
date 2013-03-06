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

        $Id: PlaylistView.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786) 
#endif

#include <windows.h>
#include <windowsx.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>

#include <algorithm>

using namespace std;

#include "config.h"
#include "utility.h"
#include "resource.h"
#include "Win32MusicBrowser.h"
#include "DropSource.h"
#include "DropObject.h"

#define kPrePadding 5

BOOL MusicBrowserUI::DrawItem(int32 controlId, DRAWITEMSTRUCT* dis)
{
    BOOL result = TRUE;

    switch(controlId)
    {
        case IDC_PLAYLISTBOX:
        {
            uint32 uiFlags = ILD_TRANSPARENT;
            RECT rcClip;
            HIMAGELIST himl;
            HFONT boldFont = NULL, oldFont = NULL;
            int32 cxImage = 0, cyImage = 0;

            // Get Image List
            himl = ListView_GetImageList(dis->hwndItem, LVSIL_SMALL);
            ImageList_GetIconSize(himl, &cxImage, &cyImage);

            rcClip = dis->rcItem;
        
            HWND hwndList = GetDlgItem(m_hWnd, IDC_PLAYLISTBOX);
            PlaylistItem* item;
            LV_ITEM lv_item;
            string displayString;

            lv_item.mask = LVIF_PARAM;
            lv_item.iItem = dis->itemID;
            lv_item.iSubItem = 0;
            lv_item.lParam = NULL;

            ListView_GetItem(hwndList, &lv_item);

            //item = (PlaylistItem*) lv_item.lParam;
            item = m_plm->ItemAt(dis->itemID);

            if(item == NULL)
                return FALSE;

            // is this the current index? if so make it bold ...
            // btw, we only do this if it is the primary browser
            if(dis->itemID == m_plm->GetCurrentIndex() && !m_pParent)
            {
                LOGFONT lf;

                GetObject(GetWindowFont(hwndList), sizeof(LOGFONT), &lf);

                lf.lfWeight = FW_BOLD;

                boldFont = CreateFontIndirect(&lf);

                oldFont = (HFONT)SelectObject(dis->hDC, boldFont);
            }

            // Item index
            char buf[64];
            sprintf(buf, "%d", dis->itemID + 1);
            displayString = buf;
            
            CalcStringEllipsis(dis->hDC, 
                               displayString, 
                               ListView_GetColumnWidth(hwndList, 0) /*- (cxImage + 1)*/);

			UINT oldAlign;

            oldAlign = SetTextAlign(dis->hDC, TA_CENTER | TA_TOP );

            RECT indexRect = rcClip;

            indexRect.right = indexRect.left + ListView_GetColumnWidth(hwndList, 0) - 1;

            UINT left = indexRect.left + (ListView_GetColumnWidth(hwndList, 0)/2);

            COLORREF scrollColor = GetSysColor(COLOR_SCROLLBAR);
            COLORREF winColor = GetSysColor(COLOR_WINDOW);

            if(scrollColor == winColor)
            {
                int r = GetRValue(scrollColor);
                int g = GetGValue(scrollColor);
                int b = GetBValue(scrollColor);

                if(( r + g + b)/3 < 128)
                {
                    r -= 25;
                    g -= 25;
                    b -= 25;

                    if(r < 0)
                        r = 0;
                    if(g < 0)
                        g = 0;
                    if(b < 0)
                        b = 0;
                }
                else
                {
                    r += 25;
                    g += 25;
                    b += 25;

                    if(r > 255)
                        r = 255;
                    if(g > 255)
                        g = 255;
                    if(b > 255)
                        b = 255;
                }

                SetBkColor(dis->hDC, RGB(r, g, b));
            }
            else
            {
                SetBkColor(dis->hDC, GetSysColor(COLOR_SCROLLBAR)); //COLOR_INFOBK ));
            }
            
            SetTextColor(dis->hDC, GetSysColor(COLOR_WINDOWTEXT)); //COLOR_INFOTEXT));
            

            ExtTextOut( dis->hDC, 
                        left, indexRect.top + 1,
                        ETO_CLIPPED | ETO_OPAQUE,
                        &indexRect, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);

            SetTextAlign(dis->hDC, oldAlign);

            
            // Move over to the next column
            rcClip.left = indexRect.right; //ListView_GetColumnWidth(hwndList, 0);

            // Check to see if this item is selected
            if(dis->itemState & ODS_SELECTED && GetFocus() == hwndList)
            {
                // Set the text background and foreground colors
                SetTextColor(dis->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
                SetBkColor(dis->hDC, GetSysColor(COLOR_HIGHLIGHT));

		        // Also add the ILD_BLEND50 so the images come out selected
		        //uiFlags |= ILD_BLEND50;
            }
            else
            {
                // Set the text background and foreground colors to the
                // standard window colors
                SetTextColor(dis->hDC, GetSysColor(COLOR_WINDOWTEXT));
                SetBkColor(dis->hDC, GetSysColor(COLOR_WINDOW));
            }


            // 
            // Iterate over all the columns that are currently visible,
            // finding the text that should be displayed in that particular
            // slot.
            // Paint the text, and update the paint rect.
            //

            HWND header = ListView_GetHeader(hwndList);
            int nCols   = Header_GetItemCount(header);

            for (int i = 1; i < nCols; i++)
            {
                const char *columnText = GetColumnText(i);
                if (!columnText)
                    break;
 
                if (stricmp(columnText, ARTIST_COLUMN) == 0)
                {
                    displayString = item->GetMetaData().Artist();
                }
                else if ( stricmp( columnText, TITLE_COLUMN ) == 0 )
                {
                    displayString = item->GetMetaData().Title();
                }
                else if ( stricmp( columnText, ALBUM_COLUMN ) == 0 )
                {
                    displayString = item->GetMetaData().Album();
                }
                else if ( stricmp( columnText, LOCATION_COLUMN ) == 0 )
                {

                    char path[_MAX_PATH];
                    uint32 length = sizeof(path);
                    URLToFilePath(item->URL().c_str(), path, &length);
                    displayString = path;
                }
                else if ( stricmp( columnText, TIME_COLUMN ) == 0 )
                {
                    char buf[16];
                    if (item->GetMetaData().Time() != 0)
                    {
                        int32 seconds = item->GetMetaData().Time();
                        int32 hours = seconds / 3600;
                        int32 minutes = seconds / 60 - hours * 60;
                        seconds = seconds - minutes * 60 - hours * 3600;
                           
                        if (hours)
                            sprintf(buf, "%d:%02d:%02d", hours, minutes, seconds);
                        else
                            sprintf(buf, "%d:%02d", minutes, seconds);

                        displayString = buf;
                    }
                }
                else if ( stricmp( columnText, GENRE_COLUMN ) == 0 )
                {
                    displayString = item->GetMetaData().Genre();
                }
                else if ( stricmp( columnText, COMMENT_COLUMN ) == 0 )
                {
                    displayString = item->GetMetaData().Comment();
                }
                else if ( stricmp( columnText, YEAR_COLUMN ) == 0 )
                {
                    char buf[16];
                    int32 year = item->GetMetaData().Year();
                    if (year)
                        sprintf(buf, "%d", year );
                    else
                        sprintf(buf, "Unknown");
                    displayString = buf;
                }
                else if ( stricmp( columnText, TRACK_COLUMN ) == 0 )
                {
                    char buf[16];
                    int32 track = item->GetMetaData().Track();
                    if(track)
                        sprintf(buf, "%d", track );
                    else
                        sprintf(buf, "Unknown");
                    displayString = buf;
                }
                else
                {
                    displayString = "Unknown";
                }
 
                CalcStringEllipsis(dis->hDC,               
                                   displayString, 
                                   ListView_GetColumnWidth(hwndList, i) - kPrePadding);

                ExtTextOut( dis->hDC, 
                            rcClip.left + kPrePadding, rcClip.top + 1, 
                            ETO_CLIPPED | ETO_OPAQUE,
                            &rcClip, 
                            displayString.c_str(),
                            displayString.size(),
                            NULL);            

                // Move over to the next column
                rcClip.left += ListView_GetColumnWidth(hwndList, i);
            }

            break;
        }


    }

    return result;
}

#define LENGTH_COLUMN_WIDTH 60
#define INDEX_COLUMN_WIDTH 25
#define FIXED_COLUMN_WIDTH (LENGTH_COLUMN_WIDTH + INDEX_COLUMN_WIDTH)

void MusicBrowserUI::InitList(void)
{
    m_hPlaylistHeader = FindWindowEx(m_hPlaylistView, NULL, WC_HEADER, NULL);

    if(m_hPlaylistHeader)
    {
        HD_ITEM hd_item;
    
        hd_item.mask = HDI_FORMAT;
        hd_item.fmt = HDF_OWNERDRAW;

        Header_SetItem(m_hPlaylistHeader, 0, &hd_item);
    }

    // First column is always inserted
    InsertColumn("#", 0);

    //
    // Insert the columns that are specified by the user.
    // 

    uint32 size = 100;
    char *buffer = new char[size];
    if (kError_BufferTooSmall == m_context->prefs->GetPrefString(
                                                     kPlaylistHeaderColumnsPref,
                                                     buffer, &size))
    {
        int bufferSize = size;
        delete [] buffer;
        buffer = new char[bufferSize];
        m_context->prefs->GetPrefString(kPlaylistHeaderColumnsPref, buffer, &size);
    }

    int columnN = 1;
    char *token = strtok(buffer, "|");
    while (token != NULL)
    {
        InsertColumn(token, columnN);
        token = strtok(NULL, "|");
        columnN++;
    }

    delete [] buffer;

    if(m_itemsAddedBeforeWeWereCreated)
    {
        for(uint32 i = 0; i < m_itemsAddedBeforeWeWereCreated; i++)
        {
            LV_ITEM lv_item;

            lv_item.mask = 0;
            lv_item.iSubItem = 0;
            lv_item.iItem = 0;

            ListView_InsertItem(m_hPlaylistView, &lv_item);
        }

        m_itemsAddedBeforeWeWereCreated = 0;
    }

    ResizeColumns();
}

//
//  The purpose of this function is to insert a new column
// into the playlist view - making sure that the column information
// that we maintain is updated at the same time.
//
void  MusicBrowserUI::InsertColumn( const char *title, int position )
{
    //
    //  I've totally fudged the column width issue here, as the
    // original implementation was breaking, and the new columns
    // were being inserted off-screen.  Steve 
    //
    LV_COLUMN  lvc;
       
    lvc.mask = LVCF_FMT | LVCF_TEXT;
    lvc.fmt = LVCFMT_LEFT; // left align column
    lvc.pszText = (char *)title;
    lvc.cchTextMax = strlen(lvc.pszText);
    lvc.iSubItem = position;
       
    // First column is special.
    if (0 == position)
    {
        lvc.cx = INDEX_COLUMN_WIDTH;
        lvc.mask |= LVCF_WIDTH;
    }

    // Add to our list.
    m_columnInfo.AddColumn( (char *)title, position );

    // Actually perform the insertion.
    ListView_InsertColumn(m_hPlaylistView, position, &lvc);
} 

//
// Resizes the widths of the columns to appropratie sizes.
//
void MusicBrowserUI::ResizeColumns()
{
    //
    // Disable repaints.
    //

    SendMessage(m_hPlaylistView, WM_SETREDRAW, FALSE, 0);
    ShowWindow(m_hPlaylistView, SW_HIDE);

    HWND hwndList = GetDlgItem(m_hWnd, IDC_PLAYLISTBOX);
    HWND header = ListView_GetHeader(hwndList);
    int nCols   = Header_GetItemCount(header);
    for (int col = 1; col < nCols ; col++) 
    {
        ResizeHeader(hwndList, col);
    }
      
    //  Enable repaints.
    //
    ShowWindow( m_hPlaylistView, SW_SHOW);
    SendMessage(m_hPlaylistView, WM_SETREDRAW, TRUE, 0);
}

void MusicBrowserUI::RemoveAllColumns( )
{
    HWND hwndList = GetDlgItem(m_hWnd, IDC_PLAYLISTBOX);

    for( int i = 0; i < m_columnInfo.GetNColumns(); i++ )
    { 
        ListView_DeleteColumn( hwndList, 0);
    }
    m_columnInfo.Erase();

    ListView_SetColumnWidth( hwndList, 0 , 0 );
}

void MusicBrowserUI::PlaylistListItemMoved(const PlaylistItem* item, 
                                           uint32 oldIndex, 
                                           uint32 newIndex)
{
    HWND    hwnd = GetDlgItem(m_hWnd, IDC_PLAYLISTBOX);
    uint32  index = m_plm->IndexOf(item);

    if(index != kInvalidIndex)
    {
        m_bListChanged = true;
        UpdateButtonStates();

        //char buf[256];
        //sprintf(buf, "oldIndex: %d\tnewIndex: %d\r\n", oldIndex, newIndex);

        //OutputDebugString(buf);

        //LV_ITEM lv_item;

        uint32 state = ListView_GetItemState(m_hPlaylistView, 
                                             oldIndex, 
                                             LVIS_SELECTED|LVIS_FOCUSED);

        /*lv_item.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
        lv_item.iSubItem = 0;
        lv_item.iItem = index;
        lv_item.lParam = (LPARAM)item;
        lv_item.iImage = 0;
        lv_item.stateMask = LVIS_FOCUSED|LVIS_SELECTED;
        lv_item.state = state;*/

        ListView_SetItemState(m_hPlaylistView, 
                              oldIndex, 
                              0,
                              LVIS_SELECTED|LVIS_FOCUSED);

        ListView_SetItemState(m_hPlaylistView, 
                              newIndex, 
                              state,
                              LVIS_SELECTED|LVIS_FOCUSED);

		int lowerBound = min(oldIndex, newIndex);
		int upperBound = max(oldIndex, newIndex);

		for (int i = lowerBound; i <= upperBound; i++)
            ListView_RedrawItems(m_hPlaylistView, i, i);

        //HMENU menu = GetSubMenu(GetMenu(m_hWnd), 1);

        //EnableMenuItem(menu, ID_EDIT_UNDO_ACTION, (m_plm->CanUndo() ? MF_ENABLED : MF_GRAYED));
        //EnableMenuItem(menu, ID_EDIT_REDO_ACTION, (m_plm->CanRedo() ? MF_ENABLED : MF_GRAYED));
    }
}

void MusicBrowserUI::PlaylistListItemRemoved(const vector<PlaylistItem*>* itemList, 
                                             const vector<uint32>* oldIndexList)
{
    // item has already been deleted when we get this 
    // msg so don't access it. only use it for comparison


    vector<uint32>::const_iterator i = oldIndexList->begin();

    for(;i != oldIndexList->end(); i++)
    {
        uint32 oldIndex = *i;

        if(oldIndex != kInvalidIndex)
        {
            LV_ITEM lv_item;
        
            lv_item.mask = LVIF_PARAM|LVIF_STATE;
            lv_item.iItem = oldIndex;
            lv_item.iSubItem = 0;
            lv_item.lParam = 0;
            lv_item.stateMask = LVIS_SELECTED|LVIS_FOCUSED;

            ListView_GetItem(m_hPlaylistView, &lv_item);
        
            ListView_DeleteItem(m_hPlaylistView, oldIndex);

            if(oldIndex >= ListView_GetItemCount(m_hPlaylistView))
                oldIndex = ListView_GetItemCount(m_hPlaylistView) - 1;

            ListView_SetItemState(m_hPlaylistView, oldIndex, lv_item.state, LVIS_SELECTED|LVIS_FOCUSED);

            /*if(lv_item.state & LVIS_SELECTED)
            {
                ListView_SetItemState(m_hPlaylistView, oldIndex, LVIS_SELECTED, LVIS_SELECTED);
            }

            if(lv_item.state & LVIS_FOCUSED)
            {
                ListView_SetItemState(m_hPlaylistView, oldIndex, LVIS_FOCUSED, LVIS_FOCUSED);
            }*/

            m_bListChanged = true;
        }
    }
    
    SetFocus(m_hPlaylistView);

    UpdateTotalTime();
    //UpdateButtonStates();
    

    //HMENU menu = GetSubMenu(GetMenu(m_hWnd), 1);

    //EnableMenuItem(menu, ID_EDIT_UNDO_ACTION, (m_plm->CanUndo() ? MF_ENABLED : MF_GRAYED));
    //EnableMenuItem(menu, ID_EDIT_REDO_ACTION, (m_plm->CanRedo() ? MF_ENABLED : MF_GRAYED));

    ListView_RedrawItems(m_hPlaylistView, ListView_GetTopIndex(m_hPlaylistView), ListView_GetItemCount(m_hPlaylistView) - 1);
}

void MusicBrowserUI::PlaylistListSorted(void)
{
    ListView_RedrawItems(m_hPlaylistView, 0, ListView_GetItemCount(m_hPlaylistView) - 1);
    m_bListChanged = true;
    UpdateButtonStates();

    //HMENU menu = GetSubMenu(GetMenu(m_hWnd), 1);

    //EnableMenuItem(menu, ID_EDIT_UNDO_ACTION, (m_plm->CanUndo() ? MF_ENABLED : MF_GRAYED));
    //EnableMenuItem(menu, ID_EDIT_REDO_ACTION, (m_plm->CanRedo() ? MF_ENABLED : MF_GRAYED));
}

void MusicBrowserUI::PlaylistListItemsUpdated(const vector<PlaylistItem*>* items)
{
    HWND   hwnd = GetDlgItem(m_hWnd, IDC_PLAYLISTBOX);

    vector<PlaylistItem*>::const_iterator i = items->begin();

    for(; i != items->end(); i++)
    {
        uint32 index = m_plm->IndexOf(*i);

        if(index != kInvalidIndex)
        {
            ListView_RedrawItems(hwnd, index, index);
            
            //HMENU menu = GetSubMenu(GetMenu(m_hWnd), 1);

            //EnableMenuItem(menu, ID_EDIT_UNDO_ACTION, (m_plm->CanUndo() ? MF_ENABLED : MF_GRAYED));
            //EnableMenuItem(menu, ID_EDIT_REDO_ACTION, (m_plm->CanRedo() ? MF_ENABLED : MF_GRAYED));
        }
        else
        {
            vector<PlaylistItem*>::iterator cd;

            cd = find(m_cdTracks->begin(), m_cdTracks->end(), *i);

            if(cd != m_cdTracks->end())
            {
                if((*cd)->URL() == (*i)->URL())
                {
                    MetaData metadata = (*i)->GetMetaData();
                    (*cd)->SetMetaData(&metadata);

                    TV_ITEM tv_item;
                    BOOL success;

                    tv_item.hItem = TreeView_GetChild(m_hMusicView, m_hCDItem);
                    tv_item.mask = TVIF_PARAM;

                    do
                    {
                        success = TreeView_GetItem(m_hMusicView, &tv_item);

                        if(success)
                        {
                            TreeData* treedata = (TreeData*)tv_item.lParam;

                            if(treedata && (*i)->URL() == treedata->m_pTrack->URL())
                            {
                                if(metadata.Title().size())
                                {
                                    tv_item.mask = TVIF_TEXT;
                                    tv_item.pszText = (char*)(metadata.Title().c_str());
                                    tv_item.cchTextMax = strlen(tv_item.pszText);

                                    TreeView_SetItem(m_hMusicView, &tv_item);
                                }
                                break;
                            }
                        }

                    }while(success && 
                           (tv_item.hItem = TreeView_GetNextSibling(m_hMusicView, 
                                                                    tv_item.hItem)));
                    string CD;

                    CD = (metadata.Album().size() ? metadata.Album() : "Unknown Album");
                    CD += " by ";
                    CD += (metadata.Artist().size() ? metadata.Artist() : "Unknown Artist");

                    tv_item.hItem = m_hCDItem;
                    tv_item.pszText = (char*)(CD.c_str());
                    tv_item.cchTextMax = strlen(tv_item.pszText);
                    TreeView_SetItem(m_hMusicView, &tv_item);
                }
            }
        }
    }
    
    UpdateTotalTime();
}

void MusicBrowserUI::PlaylistListItemAdded(const PlaylistItem* item)
{
    LV_ITEM       lv_item;
    uint32        index = m_plm->IndexOf(item);

    if(index != kInvalidIndex)
    {
        if(m_hPlaylistView)
        {
            lv_item.mask = 0;
            lv_item.iSubItem = 0;
            lv_item.iItem = 0;

            if(!ListView_GetItemCount(m_hPlaylistView) && 
               !m_pParent && 
               !m_autoPlayHack)
                m_context->target->AcceptEvent(new Event(CMD_Play));

            ListView_InsertItem(m_hPlaylistView, &lv_item);

            // this skips change notification
            // for initial loading of list for
            // editing. a hack pretty much but
            // i can't think of a better way
            if(m_initialCount)
                m_initialCount--;
            else
            {
                m_bListChanged = true;
                UpdateButtonStates();
            }

            UpdateTotalTime();
        }
        else
        {
            m_itemsAddedBeforeWeWereCreated++;
        }

        //HMENU menu = GetSubMenu(GetMenu(m_hWnd), 1);

        //EnableMenuItem(menu, ID_EDIT_UNDO_ACTION, (m_plm->CanUndo() ? MF_ENABLED : MF_GRAYED));
        //EnableMenuItem(menu, ID_EDIT_REDO_ACTION, (m_plm->CanRedo() ? MF_ENABLED : MF_GRAYED));
    }
}

void MusicBrowserUI::PlaylistListItemsAdded(const vector<PlaylistItem*>* items)
{
    uint32 count = ListView_GetItemCount(m_hPlaylistView);

    if(m_plm->CountItems() != count)
    {
        uint32 newSize = count + items->size();
        ListView_SetItemCount(m_hPlaylistView, newSize);
    }

    if(m_hPlaylistView)
    {
        LV_ITEM lv_item;

        lv_item.mask = 0;
        lv_item.iSubItem = 0;
        lv_item.iItem = 0;

        if(!count && 
           !m_pParent && 
           !m_autoPlayHack)
            m_context->target->AcceptEvent(new Event(CMD_Play));

        // this speed up adding a lot of files by preventing
        // the list from updating 
        SendMessage(m_hPlaylistView, WM_SETREDRAW, FALSE, 0);

        uint32 itemcount = items->size();

        for(uint32 i = 0; i < itemcount; i++)
            ListView_InsertItem(m_hPlaylistView, &lv_item);

        SendMessage(m_hPlaylistView, WM_SETREDRAW, TRUE, 0);
        ListView_RedrawItems(m_hPlaylistView, count, count + itemcount);

        // this skips change notification
        // for initial loading of list for
        // editing. a hack pretty much but
        // i can't think of a better way
        if(m_initialCount)
            m_initialCount--;
        else
        {
            m_bListChanged = true;
            UpdateButtonStates();
        }

        UpdateTotalTime();
    }
    else
    {
        m_itemsAddedBeforeWeWereCreated += items->size();
    }

    if(count > 999)
    {
        //ListView_SetColumnWidth(m_hPlaylistView, 0, INDEX_COLUMN_WIDTH + 6);
        //ListView_SetColumnWidth(m_hPlaylistView, 1, ListView_GetColumnWidth(m_hPlaylistView, 1) - 2);
        //ListView_SetColumnWidth(m_hPlaylistView, 2, ListView_GetColumnWidth(m_hPlaylistView, 2) - 2);
        //ListView_SetColumnWidth(m_hPlaylistView, 3, ListView_GetColumnWidth(m_hPlaylistView, 3) - 2);
    }
}

void MusicBrowserUI::DisplayCurrentItem()
{
    ListView_EnsureVisible(m_hPlaylistView, 
                           m_plm->GetCurrentIndex(), 
                           FALSE);
}

void MusicBrowserUI::GetSelectedPlaylistItems(vector<PlaylistItem*>* items)
{
    uint32 selected = ListView_GetSelectedCount(m_hPlaylistView);
    uint32 count = ListView_GetItemCount(m_hPlaylistView);
    uint32 index = 0;
    uint32 found = 0;

    for(index = 0, found = 0; found < selected && index < count; index++)
    {
        uint32 state = ListView_GetItemState(m_hPlaylistView, 
                                             index, 
                                             LVIS_SELECTED);
        if(state & LVIS_SELECTED)
        {
            PlaylistItem* item = m_plm->ItemAt(index);

            items->push_back(item);
            found++;
        }
    }
}

void MusicBrowserUI::LVBeginDrag(HWND hwnd, NM_LISTVIEW* nmlv)
{
    vector<string>* urls = new vector<string>;
    vector<PlaylistItem*> items;

    m_playlistDropTarget->TargetIsSource(true);

    GetSelectedPlaylistItems(&items);

    vector<PlaylistItem*>::iterator i;

    for(i = items.begin(); i != items.end(); i++)
    {
        urls->push_back((*i)->URL().c_str());
    }

    HIMAGELIST himl;
    RECT rcItem;
    POINT hotspot;
    
    himl = ListView_CreateDragImage(hwnd, nmlv->iItem, &hotspot);

    ListView_GetItemRect(hwnd, nmlv->iItem, &rcItem, LVIR_ICON); 

    hotspot.x = 0;
    hotspot.y = (rcItem.bottom - rcItem.top)/2;

    DataObject* data = new DataObject(CFSTR_FREEAMP_PLAYLISTITEM, urls);
    DropSource* src = new DropSource(hwnd, himl, hotspot, nmlv->ptAction);
    DWORD dwEffect = 0;

    DoDragDrop(data, 
               src, 
               DROPEFFECT_COPY|DROPEFFECT_SCROLL|DROPEFFECT_MOVE, 
               &dwEffect); 

    if(dwEffect == DROPEFFECT_MOVE)
    {
        vector<PlaylistItem*>::iterator i;

        for(i = items.begin(); i != items.end(); i++)
        {
            m_plm->RemoveItem(*i);         
        }
    }

    /*if(dwEffect != DROPEFFECT_NONE)
    {
        for(index = 0, found = 0; found < selected && index < count; index++)
        {
            uint32 state = ListView_GetItemState(hwnd, 
                                                 index, 
                                                 LVIS_SELECTED);
            if(state & LVIS_SELECTED)
            {
                ListView_SetItemState(hwnd, index, 0, LVIS_SELECTED);
                found++;
            }
        }
    }

    if(dwEffect == DROPEFFECT_MOVE)
    {
        vector<PlaylistItem*>::iterator i;

        for(i = list.begin(); i != list.end(); i++)
        {
            m_plm->RemoveItem(*i);         
        }
    }*/

    data->Release();
    src->Release();

    m_playlistDropTarget->TargetIsSource(false);
}

void MusicBrowserUI::UpdateTotalTime()
{
    uint32 count = ListView_GetItemCount(m_hPlaylistView);
    uint32 index = 0;
    uint32 total = 0;
    bool approximate = false;

    /*
    for(index = 0; index < count; index++)
    {
        PlaylistItem* item = m_plm->ItemAt(index);

        if(item)
        {
            uint32 time = item->GetMetaData().Time();

            if(!time)
                approximate = true;

            total += time;
        }
    }   
    */

    total = m_plm->Time();

    char buf[32] = "~";
    char* time = buf;

    // this will place a twiddle in front of the time
    // if any of the times are zero and thus assumed 
    // to be unknown...
    if(approximate)
        time=buf + 1;

    uint32 hours = total / 3600;
    uint32 minutes = total / 60 - hours * 60;
    uint32 seconds = total - minutes * 60 - hours * 3600;

    if(hours)
        sprintf(time, "%d:%02d:%02d", hours, minutes, seconds);
    else
        sprintf(time, "%d:%02d", minutes, seconds);

    SendMessage(m_hStatus, SB_SETTEXT, 1, (LPARAM) time);
}

LRESULT WINAPI 
ListViewWndProc(HWND hwnd, 
                UINT msg, 
                WPARAM wParam, 
                LPARAM lParam)
{
    MusicBrowserUI* ui = (MusicBrowserUI*)GetProp(hwnd, "this" );

    return ui->ListViewWndProc(hwnd, msg, wParam, lParam);
}

void MusicBrowserUI::ResizeHeader(HWND hwnd, uint32 column)
{
//    if(ListView_GetItemCount(hwnd))
//    {
        PlaylistItem* item = NULL;
        uint32 textLength = 0;
        int32 columnWidth = ListView_GetColumnWidth(hwnd,column);
        uint32 i = 0;
        string text;
        HDC hdc = GetDC(hwnd);

        while(item = m_plm->ItemAt(i++))
        {
            MetaData metadata = item->GetMetaData();
      
            const char *columnText = GetColumnText(column);

            if ( stricmp( columnText, ARTIST_COLUMN ) == 0 )
            {
                text = metadata.Artist();
            }
            else if ( stricmp( columnText, TITLE_COLUMN ) == 0 )
            {
                text = metadata.Title();
            }
            else if ( stricmp( columnText, ALBUM_COLUMN ) == 0 )
            {
                text = metadata.Album();
            }
            else if ( stricmp( columnText, LOCATION_COLUMN ) == 0 )
            {
                char path[_MAX_PATH];
                uint32 length = sizeof(path);
                URLToFilePath(item->URL().c_str(), path, &length);
                text = path;
            }
            else if ( stricmp( columnText, TIME_COLUMN ) == 0 )
            {
                char buf[16];
                if(metadata.Time() != 0)
                {
                    int32 seconds = metadata.Time();
                    int32 hours = seconds / 3600;
                    int32 minutes = seconds / 60 - hours * 60;
                    seconds = seconds - minutes * 60 - hours * 3600;

                    if(hours)
                       sprintf(buf, "%d:%02d:%02d", hours, minutes, seconds);
                    else
                       sprintf(buf, "%d:%02d", minutes, seconds);

                    text = buf;
                }
            }
            else if ( stricmp( columnText, GENRE_COLUMN ) == 0 )
            {
                text = metadata.Genre();
            }
            else if ( stricmp( columnText, COMMENT_COLUMN ) == 0 )
            {
                text = metadata.Comment();
            }
            else if ( stricmp( columnText, YEAR_COLUMN ) == 0 )
            {
                char buf[16];
                int32 year = metadata.Year();
                if(year)
                    sprintf(buf, "%d", year );
                else
                    sprintf(buf, "Unknown");

                text = buf;
            }
            else if ( stricmp( columnText, TRACK_COLUMN) == 0 )
            {
                char buf[16];
                int32 track = metadata.Track();
                if (track)
                    sprintf(buf, "%d", track );
                else
                    sprintf(buf, "Unknown");

                text = buf;
            }
            else
            {
                text = "Unknown";
            }

			if (strlen(text.c_str()) < strlen(columnText)) 
				text = columnText;

            SIZE size;

            GetTextExtentPoint32(hdc, text.c_str(), text.size(), &size); 

            if(size.cx > textLength)
                textLength = size.cx;
        }
		if (textLength <= 2)
        {
			text = GetColumnText(column);

			SIZE size;
			GetTextExtentPoint32(hdc, text.c_str(), text.size(), &size);

			textLength = size.cx;
		}

        ReleaseDC(hwnd, hdc);

        textLength += 5;

        if(column < m_columnInfo.GetNColumns()) 
        {
            int32 nextColumnWidth = ListView_GetColumnWidth(hwnd,column + 1);

            ListView_SetColumnWidth(hwnd,column, textLength);

            int32 delta = columnWidth - textLength;

            nextColumnWidth += delta;

            ListView_SetColumnWidth(hwnd,column + 1, nextColumnWidth);
        }
        else
        {
            int32 nextColumnWidth = ListView_GetColumnWidth(hwnd,column - 1);

            ListView_SetColumnWidth(hwnd,column, textLength);

            int32 delta = columnWidth - textLength;

            nextColumnWidth += delta;

            ListView_SetColumnWidth(hwnd,column - 1, nextColumnWidth);
        }
//  }
}

LRESULT MusicBrowserUI::ListViewWndProc(HWND hwnd, 
                                        UINT msg, 
                                        WPARAM wParam, 
                                        LPARAM lParam)
{
    WNDPROC lpOldProc = (WNDPROC)GetProp( hwnd, "oldproc" );
    bool filesAreURLs = false;

	switch(msg)
	{
		case WM_DESTROY:   
		{
			//  Put back old window proc and
			SetWindowLong( hwnd, GWL_WNDPROC, (DWORD)lpOldProc );

			// remove window property
			RemoveProp( hwnd, "oldproc" ); 
            RemoveProp( hwnd, "this" ); 

			break;
		}

        case WM_SETFOCUS:
        case WM_KILLFOCUS:
            UpdateButtonStates();
            break;
        
        case UWM_MOVEITEMS:
        {
            LV_HITTESTINFO hti;
            RECT itemRect;

            hti.pt = *((POINT*)lParam);
            int32 insertIndex = ListView_HitTest(hwnd, &hti);

            if(insertIndex < 0)
            {
                insertIndex = ListView_GetItemCount(hwnd) - 1;

				if (insertIndex > 0)
					insertIndex++;
            }
            else
            {   
                int32 middle;

                ListView_GetItemRect(hwnd, hti.iItem, &itemRect, LVIR_BOUNDS);

                middle = itemRect.top + (itemRect.bottom - itemRect.top)/2;

                if(hti.pt.y >= middle)
                {                    
                    insertIndex++; 
                }
			}

            char buf[256];
            sprintf(buf, "insert: %d\r\n", insertIndex);

            OutputDebugString(buf);

            vector<uint32> items;
            uint32 selected = ListView_GetSelectedCount(hwnd);
            uint32 count = ListView_GetItemCount(hwnd);
            uint32 index = 0;
            uint32 found = 0;

            for(index = 0, found = 0; found < selected && index < count; index++)
            {
                uint32 state = ListView_GetItemState(hwnd, 
                                                     index, 
                                                     LVIS_SELECTED);
                if(state & LVIS_SELECTED)
                {
                    items.push_back(index);
                    found++;
                }
            }

            m_plm->MoveItems(&items, insertIndex);

            break;
        }

        case UWM_DROPURLS:
            filesAreURLs = true;
        case WM_DROPFILES:
        {
            HDROP dropHandle = (HDROP)wParam;
            uint32 count;
            char url[1024];
            char path[MAX_PATH];
            vector<string> fileList;

            count = DragQueryFile(  dropHandle,
                                    -1L,
                                    url,
                                    sizeof(url));

            for(uint32 i = 0; i < count; i++)
            {
                DragQueryFile(  dropHandle,
                                i,
                                url,
                                sizeof(url));

                if(!filesAreURLs)
                {
                    uint32 length = sizeof(url);
                    strcpy(path, url);

                    // if this is a file drop it could be
                    // a couple things: a link, a dir, or 
                    // an mp3

                    char* extension = NULL;

                    extension = strrchr(path, '.');

                    if(extension && strcasecmp(extension, ".lnk") == 0)
                    { 
                        string link = path;

                        ResolveLink(link);

                        strcpy(path, link.c_str());
                    }

                    struct stat st;

                    stat(path, &st);

                    if(st.st_mode & _S_IFDIR)
                    {
                        vector<string> query;

                        query.push_back("*.mp1");
                        query.push_back("*.mp2");
                        query.push_back("*.mp3");
                        query.push_back("*.ogg");

                        ::SetCursor(LoadCursor(NULL, IDC_WAIT));
                        FindMusicFiles(path, fileList, query);
                        ::SetCursor(LoadCursor(NULL, IDC_ARROW));

                        continue;
                    }
                    else
                    {
                        FilePathToURL(path, url, &length);
                    }
                }

                fileList.push_back(url);
            }

            // we know that we are gonna be adding a 
            // bunch of items so let windows know.
            // it will make the adds more efficient
            uint32 newSize = ListView_GetItemCount(hwnd);
            newSize += fileList.size();
            ListView_SetItemCount(hwnd, newSize);

            LV_HITTESTINFO hti;
            RECT itemRect;

            DragQueryPoint(dropHandle, &hti.pt);
            int32 index = ListView_HitTest(hwnd, &hti);

            if(index < 0)
            {
                m_plm->AddItems(fileList);
            }
            else
            {   
                int32 middle;

                ListView_GetItemRect(hwnd, hti.iItem, &itemRect, LVIR_BOUNDS);

                middle = itemRect.top + (itemRect.bottom - itemRect.top)/2;

                if(hti.pt.y >= middle)
                    index++; 

                m_plm->AddItems(fileList, index);
            }

            SetFocus(hwnd);

            //char buf[256];
            //sprintf(buf, "x: %d   y: %d\r\n", pt.x, pt.y);
            //OutputDebugString(buf);
   
            break;
        }

        case WM_DRAWITEM:
        {
            DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*) lParam;

            if(dis->CtlType == ODT_HEADER)
            {
                RECT rcClip = rcClip = dis->rcItem;
                UINT oldAlign;

                oldAlign = SetTextAlign(dis->hDC, TA_CENTER | TA_TOP );

                UINT left = rcClip.left + ((rcClip.right - rcClip.left)/2);
                UINT top = rcClip.top + 2; // + ((rcClip.bottom - rcClip.top)/2);

                ExtTextOut( dis->hDC,left, top, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        "#",
                        strlen("#"),
                        NULL);

                SetTextAlign(dis->hDC, oldAlign);
            }

            break;
        }

        case WM_ERASEBKGND:
        {
            HDC hdc = (HDC) wParam;

            SCROLLINFO si;
            uint32 columnWidth = ListView_GetColumnWidth(hwnd, 0);
            
            RECT headerRect;
            GetClientRect(m_hPlaylistHeader, &headerRect);

            uint32 headerHeight = headerRect.bottom - headerRect.top;

            si.cbSize = sizeof(SCROLLINFO);
            si.fMask = SIF_ALL;

            GetScrollInfo(hwnd, SB_HORZ, &si);
            
            RECT rectClient, rectColumn;

            GetClientRect(hwnd, &rectClient);
            
            rectClient.top += headerHeight;

            if(si.nPos < columnWidth)
            {
                rectColumn = rectClient;
                rectColumn.right = rectColumn.left + columnWidth - si.nPos - 1;
                rectClient.left = rectColumn.right;
                
                COLORREF scrollColor = GetSysColor(COLOR_SCROLLBAR);
                COLORREF winColor = GetSysColor(COLOR_WINDOW);

                if(scrollColor == winColor)
                {
                    HBRUSH brush;
                    int r = GetRValue(scrollColor);
                    int g = GetGValue(scrollColor);
                    int b = GetBValue(scrollColor);

                    if(( r + g + b)/3 < 128)
                    {
                        r -= 25;
                        g -= 25;
                        b -= 25;

                        if(r < 0)
                            r = 0;
                        if(g < 0)
                            g = 0;
                        if(b < 0)
                            b = 0;
                    }
                    else
                    {
                        r += 25;
                        g += 25;
                        b += 25;

                        if(r > 255)
                            r = 255;
                        if(g > 255)
                            g = 255;
                        if(b > 255)
                            b = 255;
                    }

                    brush = CreateSolidBrush(RGB(r, g, b));

                    FillRect(hdc, &rectColumn, brush);
                    DeleteObject(brush);
                }
                else
                {
                    FillRect(hdc, &rectColumn, (HBRUSH)(COLOR_SCROLLBAR + 1)); //(COLOR_INFOBK + 1));
                }
            }

            FillRect(hdc, &rectClient, (HBRUSH)GetClassLong(hwnd, GCL_HBRBACKGROUND));

            return TRUE;
            break;
        }

        case WM_KEYDOWN:
        {
            // Ctrl + "+"
            if(wParam == VK_ADD && (GetKeyState(VK_CONTROL) < 0))
            {
                //  Disable repaints.
                SendMessage(m_hPlaylistView, WM_SETREDRAW, FALSE, 0);
                ShowWindow( m_hPlaylistView, SW_HIDE );

                //  Resize all columns
                HWND hwndList = GetDlgItem(m_hWnd, IDC_PLAYLISTBOX);
                HWND header = ListView_GetHeader(hwndList);
                int nCols   = Header_GetItemCount(header);
                for( int i = 1; i < nCols; i++ )
                {
                     ResizeHeader(hwnd, i);
                }
                               
                //  Enable repaints.
                ShowWindow( m_hPlaylistView, SW_SHOW);
                SendMessage(m_hPlaylistView, WM_SETREDRAW, TRUE, 0);
                return 0;
            }
            break;
        }

        case WM_NOTIFY:
        {
            int idCtrl = wParam; 
            HD_NOTIFY* hdn = (HD_NOTIFY*) lParam; 
            static int32 itemTrack = -1;
            static int32 oldWidth = 0;

            if(hdn->hdr.code == HDN_BEGINTRACKW)
            {
                if(hdn->iItem == 0 /*|| hdn->iItem == 4*/)
                    return TRUE; 

                //oldWidth = ListView_GetColumnWidth(hwnd, hdn->iItem);

                //itemTrack = hdn->iItem;
            }
            /*else if(hdn->hdr.code == HDN_ITEMCHANGINGW)
            {
                if(hdn->pitem->mask & HDI_WIDTH)                    
                {
                    if(hdn->iItem == itemTrack)
                    {
                        int32 currentWidth = ListView_GetColumnWidth(hwnd, hdn->iItem);
                        int32 nextHeaderWidth = ListView_GetColumnWidth(hwnd, hdn->iItem + 1);

                        int32 headerResizeAmount = hdn->pitem->cxy - currentWidth;

                        if(nextHeaderWidth - headerResizeAmount < 1)
                        {
                            return TRUE;
                        }
                    }
                }
            }
            else if(hdn->hdr.code == HDN_ITEMCHANGEDW)
            {
                if(hdn->iItem == itemTrack)
                {
                    int32 newWidth = ListView_GetColumnWidth(hwnd, hdn->iItem);
                    
                    int32 headerResizeAmount = newWidth - oldWidth;

                    int32 nextHeaderWidth = ListView_GetColumnWidth(hwnd, hdn->iItem + 1);

                    nextHeaderWidth -= headerResizeAmount;

                    ListView_SetColumnWidth(hwnd, hdn->iItem + 1, nextHeaderWidth);

                    oldWidth = newWidth;
                }
            }
            else if(hdn->hdr.code == HDN_ENDTRACKW)
            {
                itemTrack = -1;   
                oldWidth = 0;
            }*/
            else if(hdn->hdr.code == HDN_DIVIDERDBLCLICKW)
            {
                ResizeHeader(hwnd, hdn->iItem);

                return TRUE;
            }

            break;
        }
    } 
	
	//  Pass all non-custom messages to old window proc
	return CallWindowProc(lpOldProc, hwnd, msg, wParam, lParam );
}

int MusicBrowserUI::GetColumnIndex(const char *columnTitle)
{
    return (m_columnInfo.FindColumn((char  *)columnTitle));
}

const char *MusicBrowserUI::GetColumnText(int column)
{
    return (m_columnInfo.FindTitle(column));
}

