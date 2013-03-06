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

        $Id: Dialog.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
____________________________________________________________________________*/

#include <windows.h>
#include <windowsx.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>

#include "config.h"
#include "utility.h"
#include "resource.h"
#include "Win32MusicBrowser.h"
#include "eventdata.h"
#include "help.h"
#include "preferences.h"
#include "player.h"

#define UWM_EMPTYDBCHECK WM_USER + 69
 

TBBUTTON tbButtons[] = {
    { 0, ID_FILE_NEWPLAYLIST, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, -1},
    { 1, ID_FILE_SAVEPLAYLIST, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, -1},
	{ 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, -1},
    { 2, ID_FILE_IMPORT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, -1},
	{ 3, ID_EDIT_REMOVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, -1},
	{ 4, ID_EDIT_EDITINFO, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, -1},
    { 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, -1},
    { 5, ID_EDIT_ADDTRACK, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, -1},
    { 6, ID_EDIT_ADDFILE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, -1},
    { 7, ID_EDIT_MOVEUP, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, -1},
    { 8, ID_EDIT_MOVEDOWN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, -1} 
};

#define kNumPanes       2
#define kControlsWidth  104
#define kTimeWidth      70
int32   panes[kNumPanes]= {0, /*0,*/ -1};

static BOOL CALLBACK MainDlgProc(HWND hwnd, UINT msg, 
                                 WPARAM wParam, LPARAM lParam )
{
    MusicBrowserUI* ui = (MusicBrowserUI*)GetWindowLong(hwnd, GWL_USERDATA);
    switch (msg)
    {
        case WM_INITDIALOG:
        {
            ui = (MusicBrowserUI*)lParam;
            assert(ui != NULL);
            SetWindowLong(hwnd, GWL_USERDATA, (LONG)ui);
            
        	ui->InitDialog(hwnd);
        	ui->SetMinMaxInfo();
            return 1;
        }
        default:
            return ui->DialogProc(hwnd, msg, wParam, lParam);
    }
}        

BOOL MusicBrowserUI::DialogProc(HWND hwnd, UINT msg, 
                                WPARAM wParam, LPARAM lParam )
{
    bool filesAreURLs = false;

    switch (msg)
    {
        case WM_DESTROY:
        {
            Destroy();
            break;
        }

		case WM_CLOSE:
        {
			Close();
            return 1;
        }

        case WM_ENDSESSION:
        {
            SaveCurrentPlaylist();
            break;
        }

        case UWM_DROPURLS:
            filesAreURLs = true;
        case WM_DROPFILES:
        {
            DropFiles((HDROP)wParam, filesAreURLs);
            return 0;
        }

        case WM_SYSCOLORCHANGE:
        {
            SendMessage(m_hMusicView, WM_SYSCOLORCHANGE, 0, 0);
            SendMessage(m_hPlaylistView, WM_SYSCOLORCHANGE, 0, 0);
            SendMessage(m_hRebar, WM_SYSCOLORCHANGE, 0, 0);

             // update REBARBANDINFO for all rebar bands
            REBARBANDINFO rbb;

	        rbb.cbSize = sizeof(REBARBANDINFO);
	        rbb.fMask = RBBIM_COLORS;
	        rbb.clrFore = GetSysColor(COLOR_BTNTEXT);
	        rbb.clrBack = GetSysColor(COLOR_BTNFACE );	       

            SendMessage(m_hRebar, RB_SETBANDINFO, 0, (LPARAM)&rbb);
            SendMessage(m_hToolbar, WM_SYSCOLORCHANGE, 0, 0);
            break;
        }

		case WM_SIZE:
        {
            SizeWindow(wParam, LOWORD(lParam), HIWORD(lParam));
            return 1;
        } 
		case WM_NOTIFY:
            return Notify(wParam, (LPNMHDR)lParam);
            
		case WM_GETMINMAXINFO:
            GetMinMaxInfo((MINMAXINFO *)lParam);
            break;

        case WM_SETCURSOR:
            return SetCursor(LOWORD(lParam), HIWORD(lParam));
            break;

        case WM_MOUSEMOVE:
        {
            POINT sPoint;   
            sPoint.x = LOWORD(lParam);
            sPoint.y = HIWORD(lParam);  
            MouseMove((uint32)wParam, sPoint);
            return 1;
        }

        case WM_LBUTTONDOWN:
            MouseButtonDown(wParam, LOWORD(lParam), HIWORD(lParam));
            return 1;

        case WM_LBUTTONUP:
            MouseButtonUp(wParam, LOWORD(lParam), HIWORD(lParam));
            return 1;
            
        case UWM_EMPTYDBCHECK:
            EmptyDBCheck();
            return 1;

        case WM_HELP:
            ShowHelp(m_context, Music_Browser);
            return 1;

        case WM_COMMAND:
        {
        	switch(LOWORD(wParam))
            {
                case 2: // what is this? we get it when we press ESC.
                    if(m_bSearchInProgress)
                    {
                        StartStopMusicSearch();  
                    }
                    return 1;

                case ID_POPUP_RENAME:
                    RenameEvent();
                    return 1;

                case ID_POPUP_ADDTRACK_PLAY:
                    AddTrackAndPlayEvent();
                    return 1;

                case ID_POPUP_PLAY:
                    PlayNowEvent();
                    return 1;

				case ID_POPUP_TIP:
					TipEvent();
					return 1;

                case ID_FILE_NEWPLAYLIST:
                    NewPlaylist();
                    return 1;

                case ID_FILE_OPENPLAYLIST:
                    OpenPlaylist();
                    return 1;

                case ID_FILE_SAVEPLAYLIST:
                    SavePlaylist();
                    return 1;

                case ID_FILE_SAVEASPLAYLIST:
                    SavePlaylistAs();
                    return 1;

                case ID_FILE_IMPORT:
                    ImportTracksAndPlaylists();
                    return 1;

                case ID_FILE_EXPORTPLAYLIST:
                    ExportPlaylistEvent();
                    return 1;
                
                case ID_FILE_SEARCHFORMUSIC:
                    StartStopMusicSearch();
                    return 1;
                
                case ID_FILE_CLOSEPLAYLIST:
                    Close();
                    return 1;

                case ID_EDIT_UNDO_ACTION:
                    m_plm->Undo();
                    return 1;

                case ID_EDIT_REDO_ACTION:
                    m_plm->Redo();
                    return 1;

                case ID_POPUP_VISITWEB:
                    VisitWebEvent();
                    return 1;

                case ID_POPUP_ADDTRACK:
                case ID_EDIT_ADDTRACK:
                    AddTrackEvent();
                    return 1;

                case ID_EDIT_ADDFILE:
                    AddFileEvent(m_hWnd);
                    return 1;

                case ID_POPUP_REMOVE:
                case ID_EDIT_REMOVE:
                    RemoveEvent();
                    return 1;

                case ID_POPUP_MOVEUP:
                case ID_EDIT_MOVEUP:
                    MoveUpEvent();
                    return 1;

                case ID_POPUP_MOVEDOWN:
                case ID_EDIT_MOVEDOWN:
                    MoveDownEvent();
                    return 1;

                case ID_EDIT_CLEARPLAYLIST:
                    ClearPlaylistEvent();
                    return 1;

                case ID_POPUP_EDITPLAYLIST:
                case ID_EDIT_EDITPLAYLIST:
                    EditPlaylistEvent();
                    return 1;

                case ID_POPUP_EDITINFO:
                case ID_EDIT_EDITINFO:
                    EditInfoEvent();
                    return 1;

                case ID_POPUP_FAVORITE:
                    AddFavoriteEvent();
                    return 1;

                case ID_POPUP_REMOVEFAVORITE:
                    RemoveFavoriteEvent();
                    return 1;

                case ID_POPUP_EDITSTREAMINFO:
                    EditStreamInfoEvent();
                    return 1;

                case ID_CONTROLS_EJECT:
                case ID_POPUP_EJECT:
                    EjectCDEvent();
                    return 1;
                
				case ID_POPUP_REFRESH_CD:
					m_cdId = -1;
					m_context->player->UpdateCDNow();
					return 1;

                case ID_VIEW_MUSICCATALOG:
                    ExpandCollapseEvent();
                    return 1;

                case ID_VIEW_OPTIONS:
                    m_context->target->AcceptEvent(new ShowPreferencesEvent());
                    return 1;

                case ID_VIEW_LOCATECURRENTITEM:
                    DisplayCurrentItem();
                    return 1;
                    

                case ID_CONTROLS_PLAYPAUSE:
                case ID_CONTROLS_STOP:
                case ID_CONTROLS_PREVIOUSSONG:
                case ID_CONTROLS_NEXTSONG:
                case ID_CONTROLS_NORMALORDER:
                case ID_CONTROLS_SHUFFLE:
                case ID_CONTROLS_REPEATNONE:
                case ID_CONTROLS_REPEATONE:
                case ID_CONTROLS_REPEATALL:
                    PlayerControlsEvent(LOWORD(wParam));
                    return 1;


                case ID_SORT_ARTIST:
                case ID_SORT_ALBUM:
                case ID_SORT_TITLE:
                case ID_SORT_LENGTH:
                case ID_SORT_YEAR:
                case ID_SORT_TRACK:
                case ID_SORT_GENRE:
                case ID_SORT_LOCATION:
                case ID_SORT_FILENAME:
                case ID_SORT_RANDOMIZE:
                case IDC_RANDOMIZE:
                    SortEvent(LOWORD(wParam));
                    return 1;


                case ID_HELP_CONTENTS:
                    ShowHelp(m_context, Music_Browser);
                    return 1;

                case ID_HELP_FREEAMPWEBSITE:
                    ShellExecute(   hwnd, 
                                    "open", 
                                    "http://www.zinf.org/", 
                                    NULL, 
                                    NULL, 
                                    SW_SHOWNORMAL);
                    return 1;

                case ID_HELP_ABOUT:
                    m_context->target->AcceptEvent(new ShowPreferencesEvent(9));
                    return 1;
            }    
        }     

        case WM_DRAWITEM:
        {
            return DrawItem(wParam, (DRAWITEMSTRUCT*) lParam);

            break;
        }
        
        case WM_MEASUREITEM:
        {
            MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*) lParam;
            TEXTMETRIC tm;
            HDC hdc;
	        HFONT hFont;
	        HWND hwndLV;


            // Make sure the control is the listview control
            if (mis->CtlType != ODT_LISTVIEW || mis->CtlID != IDC_PLAYLISTBOX)
                return FALSE;

	        // Get the handle of the ListView control we're using
	        hwndLV = GetDlgItem(hwnd, mis->CtlID);

	        // Get the font the control is currently using
	        hFont = (HFONT)(DWORD) SendMessage(hwndLV, WM_GETFONT, 0, 0L);

	        // Set the font of the DC to the same font the control is using
            hdc = GetDC(hwndLV);
	        SelectObject(hdc, hFont);

            // Get the height of the font used by the control
            if (!GetTextMetrics(hdc, &tm))
                return FALSE;

            // Add a little extra space between items
            mis->itemHeight = tm.tmHeight + 1;

            // Make sure there is enough room for the images which are CY_SMICON high
            if (mis->itemHeight < 17)
    	        mis->itemHeight = 17;

	        // Clean up
	        ReleaseDC(hwndLV, hdc);

            return TRUE;

            break;
        }

        case WM_INITMENU:
        {
            UpdateMenuStates();
            break;
        }

        case WM_DEVICECHANGE:
        {   
            DeviceChanged(wParam, (PDEV_BROADCAST_HDR)lParam);
            break;
        }

    }
    
    return 0;
}

bool MusicBrowserUI::CreateMainDialog()
{
    MSG   msg;

	m_hWnd = CreateDialogParam( g_hinst, 
                    MAKEINTRESOURCE(IDD_MUSICBROWSER),
                    NULL,
                    (DLGPROC)MainDlgProc, 
                    (LPARAM)this);

    if(m_hWnd)
    {
        if(m_pParent)
        {
            RECT sRect;
            
            GetWindowRect(m_hParent, &sRect);
            SetWindowPos(m_hWnd, NULL, sRect.left + 20, sRect.top + 20,
                         0, 0, SWP_NOZORDER|SWP_NOSIZE);
            ShowWindow(m_hWnd, SW_NORMAL);
            UpdateWindow(m_hWnd);
        }
        
        while(GetMessage(&msg,NULL,0,0))
        {
            if(!IsDialogMessage(m_hWnd, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    
    DestroyWindow(m_hWnd);
    m_hWnd = NULL;

    return false;
}

void MusicBrowserUI::UIThreadFunc(void* arg)
{
    MusicBrowserUI* ui = (MusicBrowserUI*)arg;

    if (ui->CreateMainDialog())
       delete ui;
}

Error MusicBrowserUI::CloseMainDialog()
{ 
    if(m_pParent)
    {
        if(m_bListChanged)
        {
            ShowWindow(m_hWnd, SW_RESTORE);
            SetForegroundWindow(m_hWnd);
            if (MessageBox(m_hWnd, "Would you like to save this playlist?",
                  BRANDING, MB_YESNO) == IDYES)
            {
                if(!m_portableDevice)
                    SavePlaylist();
                else
                    SavePortablePlaylist();
            }
        }    
    }

    m_bListChanged = false;   
    PostMessage(m_hWnd, WM_QUIT, 0, 0);
   
    return kError_NoErr;
}

void MusicBrowserUI::ShowBrowser(bool bShowExpanded)
{
    ShowWindow(m_hWnd, SW_RESTORE);
	ShowWindow(m_hWnd, SW_SHOW);
    SetForegroundWindow(m_hWnd);
    PostMessage(m_hWnd, UWM_EMPTYDBCHECK, 0, 0);
	isVisible = true;
}

void MusicBrowserUI::HideBrowser()
{
	isVisible = false;
	ShowWindow(m_hWnd, SW_HIDE);
}

void MusicBrowserUI::Close()
{
    if(m_pParent == NULL)
       HideBrowser();
    else
       CloseMainDialog();
}

void MusicBrowserUI::Destroy()
{
    RECT  controlRect;
    char buf[256];
    int32 h,w, s;
    int32 screenX, screenY;
    WINDOWPLACEMENT wp;

    screenX = GetSystemMetrics(SM_CXSCREEN);
    screenY = GetSystemMetrics(SM_CYSCREEN);

    wp.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(m_hWnd, &wp);
    
    //GetWindowRect(m_hWnd, &rect);
    GetWindowRect(m_hMusicView, &controlRect);

    h = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
    w = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
    s = controlRect.right - controlRect.left;

    // some sanity checking
    if(wp.rcNormalPosition.left < 0)
        wp.rcNormalPosition.left = 0;
    
    if(wp.rcNormalPosition.top < 0)
        wp.rcNormalPosition.top = 0;
    
    if(s >= w)
        s = w/3;

    sprintf(buf, "%d,%d,%d,%d,%d", wp.rcNormalPosition.left, wp.rcNormalPosition.top, w, h, s);
    m_context->prefs->SetPrefString(kMusicBrowserPositionPref, buf);

    int a,b,c,d,e,f,g,i;

    a = ListView_GetColumnWidth(m_hPlaylistView, 1);
    b = ListView_GetColumnWidth(m_hPlaylistView, 2);
    c = ListView_GetColumnWidth(m_hPlaylistView, 3);
    d = ListView_GetColumnWidth(m_hPlaylistView, 4);
    e = ListView_GetColumnWidth(m_hPlaylistView, 5);
    f = ListView_GetColumnWidth(m_hPlaylistView, 6);
    g = ListView_GetColumnWidth(m_hPlaylistView, 7);
    i = ListView_GetColumnWidth(m_hPlaylistView, 8);

    sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d", a,b,c,d,e,f,g,i);
    m_context->prefs->SetPrefString(kMusicBrowserHeaderWidthsPref, buf);

    RevokeDragDrop(m_hPlaylistView);
    OleUninitialize(); 

    ImageList_Destroy(TreeView_SetImageList(
                        m_hMusicView,
                        NULL, TVSIL_NORMAL)); 
    ImageList_Destroy(ListView_SetImageList(
                        m_hPlaylistView,
                        NULL, LVSIL_SMALL)); 

    m_playlistDropTarget->Release();

    if(m_pParent)
    {
        m_pParent->RemoveMusicBrowserWindow(this);
        delete this;
    }
}

void MusicBrowserUI::ExpandCollapseEvent()
{
    HMENU        hMenu;
    MENUITEMINFO sItem;
    RECT catalogRect, playlistRect, titleRect;

    GetWindowRect(m_hMusicView, &catalogRect);
    GetWindowRect(m_hPlaylistView, &playlistRect);
    GetWindowRect(m_hPlaylistTitle, &titleRect);
    MapWindowPoints(NULL, m_hWnd, (LPPOINT)&catalogRect, 2);
    MapWindowPoints(NULL, m_hWnd, (LPPOINT)&playlistRect, 2);
    MapWindowPoints(NULL, m_hWnd, (LPPOINT)&titleRect, 2);

    if (m_state == STATE_COLLAPSED)
    {
       m_state = STATE_EXPANDED;
       SetWindowText(m_hWnd,  "My Music - " BRANDING);
       sItem.dwTypeData = "View &Playlist Only";

       ShowWindow(m_hMusicView, SW_SHOW);
       ShowWindow(m_hMusicViewTitle, SW_SHOW);

       MoveWindow(m_hPlaylistView, 
                  catalogRect.left + m_iCollapseMoveAmount, 
                  catalogRect.top, 
        	      (playlistRect.right - playlistRect.left) - m_iCollapseMoveAmount,
                  playlistRect.bottom - playlistRect.top,
                  TRUE);

       MoveWindow(m_hPlaylistTitle, 
                  titleRect.left + m_iCollapseMoveAmount, 
                  titleRect.top, 
        	      (titleRect.right - titleRect.left) - m_iCollapseMoveAmount,
                  titleRect.bottom - titleRect.top,
                  TRUE);
    }            
    else
    {                
       m_state = STATE_COLLAPSED;
       SetWindowText(m_hWnd, "Playlist - " BRANDING);
       sItem.dwTypeData = "View &My Music";

       ShowWindow(m_hMusicView, SW_HIDE);
       ShowWindow(m_hMusicViewTitle, SW_HIDE);

       m_iCollapseMoveAmount = playlistRect.left - catalogRect.left;
       
       MoveWindow(m_hPlaylistView, 
                  catalogRect.left, 
                  catalogRect.top, 
        	      (playlistRect.right - playlistRect.left) + m_iCollapseMoveAmount,
                  playlistRect.bottom - playlistRect.top,
                  TRUE);

       MoveWindow(m_hPlaylistTitle, 
                  titleRect.left - m_iCollapseMoveAmount, 
                  titleRect.top, 
        	      (titleRect.right - titleRect.left) + m_iCollapseMoveAmount,
                  titleRect.bottom - titleRect.top,
                  TRUE);
    }

    hMenu = GetMenu(m_hWnd);
    hMenu = GetSubMenu(hMenu, 2);
    sItem.cbSize = sizeof(MENUITEMINFO);
    sItem.fMask = MIIM_TYPE;
    sItem.fType = MFT_STRING;
    sItem.cch = strlen(sItem.dwTypeData);
    SetMenuItemInfo(hMenu, ID_VIEW_MUSICCATALOG, false, &sItem);
    
    SendMessage(m_hStatus, SB_SETTEXT, 0, (LPARAM)"");
    InvalidateRect(m_hWnd, NULL, true);
    UpdateWindow(m_hWnd);
}

void MusicBrowserUI::SizeWindow(int iType, int iWidth, int iHeight)
{
    RECT windowRect;
    RECT clientRect;
    RECT controlRect;
    uint32 windowHeight, windowWidth;
    uint32 clientHeight, clientWidth;
    uint32 controlHeight, controlWidth;
    uint32 statusbarHeight;
    RECT oldListViewRect;

    GetWindowRect(m_hWnd, &windowRect);
    windowHeight = windowRect.bottom - windowRect.top;
    windowWidth = windowRect.right - windowRect.left;

    GetClientRect(m_hWnd, &clientRect);
    clientHeight = clientRect.bottom - clientRect.top;
    clientWidth = clientRect.right - clientRect.left;

    HDWP hdwp = BeginDeferWindowPos(5);
    
    // Status Bar
    GetWindowRect(m_hStatus, &controlRect); 
    controlHeight = controlRect.bottom - controlRect.top;
    controlWidth = controlRect.right - controlRect.left;

    statusbarHeight = controlHeight;
    hdwp = DeferWindowPos(hdwp, m_hStatus, NULL,
                           windowRect.left, 
                           windowRect.bottom - controlHeight,
                           windowWidth,
                           controlHeight,
                           SWP_NOZORDER);

    panes[0] = clientRect.right - clientRect.left /*- kControlsWidth*/ - kTimeWidth;
    //panes[1] = panes[0] + kControlsWidth;
    SendMessage(m_hStatus, SB_SETPARTS, kNumPanes, (LPARAM) panes);

    // Tool Bar
    GetWindowRect(m_hRebar, &controlRect); 
    controlHeight = controlRect.bottom - controlRect.top;
    controlWidth = controlRect.right - controlRect.left;

    hdwp = DeferWindowPos(hdwp, m_hRebar, NULL,
                           0, 
                           0,
                           clientWidth,
                           controlHeight,
                           SWP_NOZORDER);

    // Music Catalog View
    GetWindowRect(m_hMusicView, &controlRect); 
    MapWindowPoints(NULL, m_hWnd, (LPPOINT)&controlRect, 2);
    controlHeight = controlRect.bottom - controlRect.top;
    controlWidth = controlRect.right - controlRect.left;

    hdwp = DeferWindowPos(hdwp, m_hMusicView, NULL,
                           controlRect.left,
                           controlRect.top,
                           controlWidth,
                           clientRect.bottom - controlRect.top - statusbarHeight,
                           SWP_NOZORDER);


    // Current Playlist Title
    GetWindowRect(m_hPlaylistTitle, &controlRect);
    MapWindowPoints(NULL, m_hWnd, (LPPOINT)&controlRect, 2);
    controlHeight = controlRect.bottom - controlRect.top;
    controlWidth = clientRect.right - controlRect.left;

    hdwp = DeferWindowPos(hdwp, m_hPlaylistTitle, NULL,
                           controlRect.left,
                           controlRect.top,
                           controlWidth,
                           controlHeight,
                           SWP_NOZORDER);

    // Playlist View
    GetClientRect(m_hPlaylistView, &oldListViewRect);
    GetWindowRect(m_hPlaylistView, &controlRect);
    MapWindowPoints(NULL, m_hWnd, (LPPOINT)&controlRect, 2);
    controlHeight = clientRect.bottom - controlRect.top;
    controlWidth = clientRect.right - controlRect.left;

    hdwp = DeferWindowPos(hdwp, m_hPlaylistView, NULL,
                           controlRect.left,
                           controlRect.top,
                           controlWidth,
                           controlHeight - statusbarHeight,
                           SWP_NOZORDER);

    /*int delta = controlRect.right - controlRect.left - oldListViewRect.right - oldListViewRect.left;

    controlWidth -= delta;

    if(controlWidth < oldListViewRect.right - oldListViewRect.left)
    {
        int32 oldWidth = 0;
        HWND hwnd = m_hPlaylistView;

        oldWidth += ListView_GetColumnWidth(hwnd, 0);
        oldWidth += ListView_GetColumnWidth(hwnd, 1);
        oldWidth += ListView_GetColumnWidth(hwnd, 2);
        oldWidth += ListView_GetColumnWidth(hwnd, 3);
        oldWidth += ListView_GetColumnWidth(hwnd, 4);

        int32 headerResizeAmount = controlWidth - oldWidth;

        int32 eachHeaderAmount = headerResizeAmount/3;
        int32 titleExtraAmount = headerResizeAmount%3;
        int32 width;

        if(eachHeaderAmount)
        {
            width = ListView_GetColumnWidth(m_hPlaylistView, 1);
            width += eachHeaderAmount;    
            ListView_SetColumnWidth(m_hPlaylistView, 1, width);

            width = ListView_GetColumnWidth(m_hPlaylistView, 2);
            width += eachHeaderAmount;
            ListView_SetColumnWidth(m_hPlaylistView, 2, width);

            width = ListView_GetColumnWidth(m_hPlaylistView, 3);
            width += eachHeaderAmount;
            ListView_SetColumnWidth(m_hPlaylistView, 3, width);
        }

        if(titleExtraAmount)
        {
            static uint32 lastColumn = 1;

            while(titleExtraAmount)
            {
                width = ListView_GetColumnWidth(m_hPlaylistView, lastColumn);
   
                if(titleExtraAmount > 0)
                {
                    width += 1;
                    titleExtraAmount--;
                }
                else
                {
                    width -= 1;
                    titleExtraAmount++;
                }

                ListView_SetColumnWidth(m_hPlaylistView, lastColumn, width);

                if(++lastColumn > 3)
                    lastColumn = 1;
            }
        }        
    }*/
                           
    EndDeferWindowPos(hdwp);

    /*GetClientRect(m_hPlaylistView, &controlRect);

    controlWidth = controlRect.right - controlRect.left;

    if(controlWidth > oldListViewRect.right - oldListViewRect.left)
    {
        int32 oldWidth = 0;
        HWND hwnd = m_hPlaylistView;

        oldWidth += ListView_GetColumnWidth(hwnd, 0);
        oldWidth += ListView_GetColumnWidth(hwnd, 1);
        oldWidth += ListView_GetColumnWidth(hwnd, 2);
        oldWidth += ListView_GetColumnWidth(hwnd, 3);
        oldWidth += ListView_GetColumnWidth(hwnd, 4);

        int32 headerResizeAmount = controlWidth - oldWidth;

        int32 eachHeaderAmount = headerResizeAmount/3;
        int32 titleExtraAmount = headerResizeAmount%3;
        int32 width;

        if(eachHeaderAmount)
        {
            width = ListView_GetColumnWidth(m_hPlaylistView, 1);
            width += eachHeaderAmount;    
            ListView_SetColumnWidth(m_hPlaylistView, 1, width);

            width = ListView_GetColumnWidth(m_hPlaylistView, 2);
            width += eachHeaderAmount;
            ListView_SetColumnWidth(m_hPlaylistView, 2, width);

            width = ListView_GetColumnWidth(m_hPlaylistView, 3);
            width += eachHeaderAmount;
            ListView_SetColumnWidth(m_hPlaylistView, 3, width);
        }

        if(titleExtraAmount)
        {
            static uint32 lastColumn = 1;

            while(titleExtraAmount)
            {
                width = ListView_GetColumnWidth(m_hPlaylistView, lastColumn);
   
                if(titleExtraAmount > 0)
                {
                    width += 1;
                    titleExtraAmount--;
                }
                else
                {
                    width -= 1;
                    titleExtraAmount++;
                }

                ListView_SetColumnWidth(m_hPlaylistView, lastColumn, width);

                if(++lastColumn > 3)
                    lastColumn = 1;
            }
        }        
    }*/
}

void MusicBrowserUI::GetMinMaxInfo(MINMAXINFO *pInfo)
{
    pInfo->ptMinTrackSize = m_sMinSize;
}

void MusicBrowserUI::SetMinMaxInfo()
{
	RECT  sLoc, sLoc2;
    
	GetWindowRect(m_hMusicView, &sLoc);
	GetWindowRect(m_hPlaylistView, &sLoc2);
    //m_iCollapseMoveAmount = sLoc2.left - sLoc.left;
}

void MusicBrowserUI::InitDialog(HWND hWnd)
{
    HIMAGELIST      hList;
    

    HINSTANCE hinst = (HINSTANCE)GetWindowLong(m_hWnd, GWL_HINSTANCE);
    HICON appIcon = LoadIcon(hinst, MAKEINTRESOURCE(IDI_EXE_ICON));

    SetClassLong(m_hWnd, GCL_HICON, (LONG)appIcon);

    m_hWnd = hWnd;
    m_hMusicView = GetDlgItem(m_hWnd, IDC_MUSICTREE);
    m_hPlaylistView = GetDlgItem(m_hWnd, IDC_PLAYLISTBOX);
    m_hPlaylistTitle = GetDlgItem(m_hWnd, IDC_PLAYLISTTITLE);
    m_hMusicViewTitle = GetDlgItem(m_hWnd, IDC_MUSICCATALOGTEXT);

    HBITMAP bmp;
    
    hList = ImageList_Create(16, 16, ILC_COLOR24|ILC_MASK, 10, 0);

    bmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_CATALOG));
    ImageList_AddMasked(hList, bmp, RGB(255,255,0));
    DeleteObject(bmp);
    bmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_PLAYLIST));
    ImageList_AddMasked(hList, bmp, RGB(255,0,0));
    DeleteObject(bmp);
    bmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_ARTIST));
    ImageList_AddMasked(hList, bmp, RGB(255,0,0));
    DeleteObject(bmp);
    bmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_ALBUM));
    ImageList_AddMasked(hList, bmp, RGB(255,0,0));
    DeleteObject(bmp);
    bmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_TRACK));
    ImageList_AddMasked(hList, bmp, RGB(255,0,0));
    DeleteObject(bmp);
    bmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_ALL));
    ImageList_AddMasked(hList, bmp, RGB(255,0,0));
    DeleteObject(bmp);
    bmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_UNCATAGORIZED));
    ImageList_AddMasked(hList, bmp, RGB(255,0,0));
    DeleteObject(bmp);
    bmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_PORTABLE));
    ImageList_AddMasked(hList, bmp, RGB(255,0,0));
    DeleteObject(bmp);
    bmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_STREAMS));
    ImageList_AddMasked(hList, bmp, RGB(255,0,0));
    DeleteObject(bmp);
    bmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_WIREDPLANET));
    ImageList_AddMasked(hList, bmp, RGB(255,0,0));
    DeleteObject(bmp);
    bmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_ICECAST));
    ImageList_AddMasked(hList, bmp, RGB(255,0,0));
    DeleteObject(bmp);
    bmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_SHOUTCAST));
    ImageList_AddMasked(hList, bmp, RGB(255,0,0));
    DeleteObject(bmp);
    bmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_BUSY));
    ImageList_AddMasked(hList, bmp, RGB(255,0,0));
    DeleteObject(bmp);
    bmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_CD));
    ImageList_AddMasked(hList, bmp, RGB(255,0,0));
    DeleteObject(bmp);
    bmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_FAVORITES));
    ImageList_AddMasked(hList, bmp, RGB(255,0,0));
    DeleteObject(bmp);
    

    TreeView_SetImageList(m_hMusicView, hList, TVSIL_NORMAL); 
    
    hList = ImageList_Create(16, 16, ILC_COLOR24|ILC_MASK, 1, 0);

    bmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_PLAYING));
    ImageList_AddMasked(hList, bmp, RGB(255,0,0));
    DeleteObject(bmp);
 
    ListView_SetImageList(m_hPlaylistView, hList, LVSIL_SMALL); 

    SetTitles();
    CreateToolbar();

    InitTree();
    InitList();

    // Subclass the listview

    // Set the proc address as a property 
	// of the window so it can get it
	SetProp(m_hPlaylistView,
			"oldproc",
            (HANDLE)GetWindowLong(m_hPlaylistView, GWL_WNDPROC));

    SetProp(m_hPlaylistView,
        "this",
        (HANDLE)this);
	
	// Subclass the window so we can draw it
	SetWindowLong(m_hPlaylistView,
                  GWL_WNDPROC,
                  (DWORD)::ListViewWndProc);

    // Subclass the treeview

    // Set the proc address as a property 
	// of the window so it can get it
	SetProp(m_hMusicView, 
            "oldproc",
            (HANDLE)GetWindowLong(m_hMusicView, GWL_WNDPROC));

    SetProp(m_hMusicView, 
            "this",
            (HANDLE)this);
	
	// Subclass the window so we can handle multi-select
	SetWindowLong(m_hMusicView, 
			      GWL_WNDPROC, 
                  (DWORD)::TreeViewWndProc );  

    // register our OLE drag and drop crap
    OleInitialize(NULL);

    int result;
    m_playlistDropTarget = new DropTarget(m_hPlaylistView);
    //CoLockObjectExternal ((IUnknown*)m_playlistDropTarget, TRUE, TRUE);
    result = RegisterDragDrop(m_hPlaylistView, m_playlistDropTarget);
    
    m_hStatus= CreateStatusWindow(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                                  "", m_hWnd, IDC_STATUS);

    RECT rect;

    GetClientRect(m_hWnd, &rect);

    panes[0] = rect.right - rect.left /*- kControlsWidth*/ - kTimeWidth;
    //panes[1] = panes[0] + kControlsWidth;
    SendMessage(m_hStatus, SB_SETPARTS, kNumPanes, (LPARAM) panes);
    //SendMessage(m_hStatus, SB_SETTEXT, 1|SBT_NOBORDERS , (LPARAM) "");
    SendMessage(m_hStatus, SB_SETTEXT, kNumPanes - 1, (LPARAM) "0:00");

    GetClientRect(m_hStatus, &rect);

    /*CreateWindow("BUTTON",
                 "button",
                 WS_VISIBLE|WS_CHILD,
                 panes[0] + 2,
                 rect.top + 1,
                 kControlsWidth - 2,
                 rect.bottom - rect.top,
                 m_hStatus,
                 NULL,
                 g_hinst,
                 NULL);*/

    UpdateButtonStates();
    m_context->target->AcceptEvent(new Event(CMD_QueryPlayerState));

    m_sMinSize.x = 500;
    m_sMinSize.y = 300;

    if(m_pParent == NULL)
    {
        m_plm->SetActivePlaylist(kPlaylistKey_MasterPlaylist);
    }   
    else if(m_portableDevice)
    {
        m_plm->SetActivePlaylist(kPlaylistKey_PortablePlaylist);
        if(m_plm->IsPortableAvailable(m_portableDevice))
        {
            vector<PlaylistItem*> items;
            m_plm->ReadPortablePlaylist(m_portableDevice, &items);

            m_initialCount = items.size();

            if(m_initialCount)
                m_plm->AddItems(&items);
        }
    }
    else
    {
        m_plm->SetActivePlaylist(kPlaylistKey_ExternalPlaylist);

        if(m_currentListName.size())
            LoadPlaylist(m_currentListName.c_str());
    }       

    UpdateTotalTime();

    // resize window
    char buf[256];
    uint32 size = sizeof(buf);
    int32 x,y,h,w, s;

    m_context->prefs->GetPrefString(kMusicBrowserPositionPref, buf, &size);
    sscanf(buf, " %d , %d , %d , %d, %d", &x, &y, &w, &h, &s);

    if(x >= 0 && y >= 0 && h >= 0  && w >= 0 && s >= 0 && s < w)
    {
        int32 screenX, screenY;

        screenX = GetSystemMetrics(SM_CXSCREEN);
        screenY = GetSystemMetrics(SM_CYSCREEN);

        if(x > screenX)
            x = 0;
        if(y > screenY)
            y = 0;

        MoveWindow(hWnd, x, y, w, h, TRUE);

        /*RECT controlRect;
        int32 controlHeight, controlWidth;

        GetWindowRect(m_hMusicView, &controlRect);
        MapWindowPoints(NULL, m_hWnd, (LPPOINT)&controlRect, 2);
        controlHeight = controlRect.bottom - controlRect.top;
        controlWidth = controlRect.right - controlRect.left;

        MoveWindow(m_hMusicView, controlRect.left, 
                    controlRect.top, s, controlHeight, TRUE);
        */

        RECT catalogRect, playlistRect, titleRect;
        int32 delta;

        GetWindowRect(m_hMusicView, &catalogRect);
        GetWindowRect(m_hPlaylistView, &playlistRect);
        GetWindowRect(m_hPlaylistTitle, &titleRect);

        delta = s - (catalogRect.right - catalogRect.left);
        catalogRect.right = catalogRect.left + s;
        playlistRect.left = playlistRect.left + delta;
        titleRect.left += delta;

        MapWindowPoints(NULL, m_hWnd, (LPPOINT)&catalogRect, 2);
        MapWindowPoints(NULL, m_hWnd, (LPPOINT)&playlistRect, 2);
        MapWindowPoints(NULL, m_hWnd, (LPPOINT)&titleRect, 2);

        
        MoveWindow(m_hMusicView, 
                  catalogRect.left, 
                  catalogRect.top, 
        	      (catalogRect.right - catalogRect.left),
                  catalogRect.bottom - catalogRect.top,
                  TRUE);

        MoveWindow(m_hPlaylistView, 
                  playlistRect.left,
                  playlistRect.top, 
        	      (playlistRect.right - playlistRect.left),
                  playlistRect.bottom - playlistRect.top,
                  TRUE);

        MoveWindow(m_hPlaylistTitle, 
                  titleRect.left,
                  titleRect.top, 
        	      (titleRect.right - titleRect.left),
                  titleRect.bottom - titleRect.top,
                  TRUE);

        m_splitterRect.top = playlistRect.top;
        m_splitterRect.bottom = playlistRect.bottom;
        m_splitterRect.left = catalogRect.right + 1;
        m_splitterRect.right = playlistRect.left - 1;

        // resize headers... only wanna do this if we resized the window
        int32 a,b,c,d,e,f,g,h;

        size = sizeof(buf);
        m_context->prefs->GetPrefString(kMusicBrowserHeaderWidthsPref, buf, &size);
        if (sscanf(buf, " %d , %d , %d , %d , %d , %d , %d , %d", &a, &b, &c, &d,
			                                                      &e, &f, &g, &h)
             != 8)
        {
		    sscanf(buf, " %d , %d , %d , %d", &a, &b, &c, &d);

            if(a >= 1 && b >= 5 && c >= 5 && d >= 5)
			{
                ListView_SetColumnWidth(m_hPlaylistView, 1, a);
                ListView_SetColumnWidth(m_hPlaylistView, 2, b);
                ListView_SetColumnWidth(m_hPlaylistView, 3, c);
                ListView_SetColumnWidth(m_hPlaylistView, 4, d);
			}
		}
		else
		{
            if(a >= 1 && b >= 5 && c >= 5 && d >= 5)
			{
                ListView_SetColumnWidth(m_hPlaylistView, 1, a);
                ListView_SetColumnWidth(m_hPlaylistView, 2, b);
                ListView_SetColumnWidth(m_hPlaylistView, 3, c);
                ListView_SetColumnWidth(m_hPlaylistView, 4, d);
                ListView_SetColumnWidth(m_hPlaylistView, 5, e);
                ListView_SetColumnWidth(m_hPlaylistView, 6, f);
                ListView_SetColumnWidth(m_hPlaylistView, 7, g);
                ListView_SetColumnWidth(m_hPlaylistView, 8, h);
			}
		}
    }

    if(m_pParent)
    {
        ShowWindow(m_hWnd, SW_SHOW);
        SetForegroundWindow(m_hWnd);
    }
    else
    {
        bool viewMusicBrowser = true;

        m_context->prefs->GetPrefBoolean(kViewMusicBrowserPref, &viewMusicBrowser);

        if(!viewMusicBrowser)
        {
            ExpandCollapseEvent();  
        }
    }

	bool check;
	m_context->prefs->GetPrefBoolean(kCheckCDAutomaticallyPref, &check);
	if (check)
       CheckForCD();

    m_initialized = true;
	
	// UGLY hack to fix threading issue with win98 osr2 creating blank my music tree
    m_context->target->AcceptEvent(new Event(INFO_MusicCatalogCleared));

}

void MusicBrowserUI::AddToolbarButtons(bool textLabels, bool images)
{
    if(!textLabels && !images)
    {
        textLabels = true;
        images = true;
    }

    SendMessage(m_hRebar, RB_DELETEBAND, 0, 0);

    if(images && textLabels)
        m_hToolbar = m_hBothToolbar;
    else if(!images)
        m_hToolbar = m_hTextToolbar;
    else
        m_hToolbar = m_hImageToolbar;

    RECT windowRect, clientRect, buttonRect, rebarRect, titleRect;
    uint32 top = 0;

    GetWindowRect(m_hRebar, &windowRect);
    GetClientRect(m_hRebar, &clientRect);
    SendMessage(m_hToolbar, TB_GETITEMRECT , (WPARAM) 0, (LPARAM) &buttonRect); 

    REBARBANDINFO rbb;

    // Initialize REBARBANDINFO for all rebar bands
	rbb.cbSize = sizeof(REBARBANDINFO);
	rbb.fMask = RBBIM_COLORS |	    // clrFore and clrBack are valid
		RBBIM_CHILD |				// hwndChild is valid
		RBBIM_CHILDSIZE |			// cxMinChild and cyMinChild are valid
		RBBIM_STYLE |				// fStyle is valid
		RBBIM_ID;					// wID is valid
	rbb.clrFore = GetSysColor(COLOR_BTNTEXT);
	rbb.clrBack = GetSysColor(COLOR_BTNFACE );
	rbb.fStyle = RBBS_NOVERT |	// do not display in vertical orientation
		            RBBS_CHILDEDGE;
	rbb.hbmBack = NULL;
	rbb.hwndChild = m_hToolbar;
	rbb.wID = IDC_TOOLBAR;
	rbb.cxMinChild = clientRect.right - clientRect.left;
	rbb.cyMinChild = buttonRect.bottom - buttonRect.top;

	// Insert band into rebar
	SendMessage(m_hRebar, RB_INSERTBAND, (UINT) -1, (LPARAM) &rbb);

    // resize rebar
    SetWindowPos(m_hRebar, NULL, 
                 0, 0,
                 windowRect.right - windowRect.left, 
                 buttonRect.bottom - buttonRect.top + 4, 
                 SWP_NOMOVE|SWP_NOZORDER );

    GetWindowRect(m_hRebar, &rebarRect);
    MapWindowPoints(NULL, m_hWnd, (LPPOINT)&rebarRect, 2);

    top = rebarRect.bottom + 2;

    HDWP hdwp = BeginDeferWindowPos(4);

    // move music catalog and playlist views
    GetWindowRect(m_hMusicViewTitle, &windowRect);
    MapWindowPoints(NULL, m_hWnd, (LPPOINT)&windowRect, 2);

    hdwp = DeferWindowPos(hdwp, m_hMusicViewTitle, NULL, 
                 windowRect.left, 
                 top,
                 0, 
                 0, 
                 SWP_NOZORDER| SWP_NOSIZE );

    GetWindowRect(m_hPlaylistTitle, &windowRect);
    MapWindowPoints(NULL, m_hWnd, (LPPOINT)&windowRect, 2);

    hdwp = DeferWindowPos(hdwp, m_hPlaylistTitle, NULL, 
                 windowRect.left, 
                 top,
                 0, 
                 0, 
                 SWP_NOZORDER| SWP_NOSIZE );

    GetWindowRect(m_hMusicViewTitle, &titleRect);
    MapWindowPoints(NULL, m_hWnd, (LPPOINT)&titleRect, 2);

    top += titleRect.bottom - titleRect.top + 3;

    GetWindowRect(m_hMusicView, &windowRect);
    MapWindowPoints(NULL, m_hWnd, (LPPOINT)&windowRect, 2);

    hdwp = DeferWindowPos(hdwp, m_hMusicView, NULL, 
                 windowRect.left, 
                 top,
                 windowRect.right - windowRect.left, 
                 windowRect.bottom - windowRect.top + (windowRect.top - top),
                 SWP_NOZORDER );

    GetWindowRect(m_hPlaylistTitle, &titleRect);
    MapWindowPoints(NULL, m_hWnd, (LPPOINT)&titleRect, 2);

    GetWindowRect(m_hPlaylistView, &windowRect);
    MapWindowPoints(NULL, m_hWnd, (LPPOINT)&windowRect, 2);

    hdwp = DeferWindowPos(hdwp, m_hPlaylistView, NULL, 
                 windowRect.left, 
                 top,
                 windowRect.right - windowRect.left, 
                 windowRect.bottom - windowRect.top + (windowRect.top - top),
                 SWP_NOZORDER );

    EndDeferWindowPos(hdwp);
}

void MusicBrowserUI::CreateToolbar()
{
    INITCOMMONCONTROLSEX icex;
    RECT rect;
    RECT dummyRect;
    HWND hwndDummy = GetDlgItem(m_hWnd, IDC_DUMMYCONTROL);

    GetWindowRect(hwndDummy, &dummyRect);
    int32 toolbarHeight = dummyRect.bottom - dummyRect.top;

    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_COOL_CLASSES ;	// load the rebar
	InitCommonControlsEx(&icex);

    GetClientRect(m_hWnd, &rect);

    // Create the REBAR control
	m_hRebar = CreateWindowEx( 
				0L,
				REBARCLASSNAME,
				NULL,
                WS_VISIBLE | WS_BORDER | WS_CHILD | 0 |
                  WS_CLIPSIBLINGS | CCS_NODIVIDER | RBS_VARHEIGHT |
                  RBS_BANDBORDERS | CCS_NOPARENTALIGN,
				0, 0, rect.right - rect.left, toolbarHeight,
				m_hWnd,
				(HMENU)IDC_REBAR,
				g_hinst,
				NULL );

    m_hImageToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, (LPSTR) NULL,
                     WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT |
                     TBSTYLE_TOOLTIPS | WS_CLIPCHILDREN | 
                     WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE, 
                     0, 0, 0, 0, m_hRebar, (HMENU) IDC_TOOLBAR, g_hinst, NULL);

    m_hTextToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, (LPSTR) NULL,
                     WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT |
                     TBSTYLE_TOOLTIPS | WS_CLIPCHILDREN | 
                     WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE, 
                     0, 0, 0, 0, m_hRebar, (HMENU) IDC_TOOLBAR + 1, g_hinst, NULL);

    m_hBothToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, (LPSTR) NULL,
                     WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT |
                     TBSTYLE_TOOLTIPS | WS_CLIPCHILDREN | 
                     WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE, 
                     0, 0, 0, 0, m_hRebar, (HMENU) IDC_TOOLBAR + 2, g_hinst, NULL);

    // Send the TB_BUTTONSTRUCTSIZE message, which is required for 
    // backward compatibility. 
    SendMessage(m_hImageToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);
    SendMessage(m_hTextToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);
    SendMessage(m_hBothToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);

    SendMessage(m_hImageToolbar, TB_SETBITMAPSIZE, 0, MAKELPARAM(22, 18)); 
    SendMessage(m_hTextToolbar, TB_SETBITMAPSIZE, 0, MAKELPARAM(0, 0)); 
    SendMessage(m_hBothToolbar, TB_SETBITMAPSIZE, 0, MAKELPARAM(22, 18)); 

    TBADDBITMAP tbab; 

    // Add the bitmap containing button images to the toolbar.
    tbab.hInst = g_hinst; 
    tbab.nID   = IDB_TOOLBAR; 
    SendMessage(m_hImageToolbar, TB_ADDBITMAP, (WPARAM) 10, (WPARAM) &tbab);
    SendMessage(m_hBothToolbar, TB_ADDBITMAP, (WPARAM) 10, (WPARAM) &tbab);

    SendMessage(m_hImageToolbar, TB_ADDBUTTONS, (WPARAM) 11, (LPARAM) &tbButtons);
    SendMessage(m_hImageToolbar, TB_AUTOSIZE, (WPARAM) 0, (LPARAM) 0);

    tbButtons[0].iString = SendMessage(m_hBothToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"New Playlist");
    tbButtons[1].iString = SendMessage(m_hBothToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"Save Playlist");
    tbButtons[3].iString = SendMessage(m_hBothToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"Import Items");
    tbButtons[4].iString = SendMessage(m_hBothToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"Remove Items");
    tbButtons[5].iString = SendMessage(m_hBothToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"Edit Info");
    tbButtons[7].iString = SendMessage(m_hBothToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"Add Items");
    tbButtons[8].iString = SendMessage(m_hBothToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"Add Files");
    tbButtons[9].iString = SendMessage(m_hBothToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"Move Up");
    tbButtons[10].iString = SendMessage(m_hBothToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"Move Down");
    SendMessage(m_hBothToolbar, TB_ADDBUTTONS, (WPARAM) 11, (LPARAM) &tbButtons);
    SendMessage(m_hBothToolbar, TB_AUTOSIZE, (WPARAM) 0, (LPARAM) 0);

    tbButtons[0].iString = SendMessage(m_hTextToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"New Playlist");
    tbButtons[1].iString = SendMessage(m_hTextToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"Save Playlist");
    tbButtons[3].iString = SendMessage(m_hTextToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"Import Items");
    tbButtons[4].iString = SendMessage(m_hTextToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"Remove Items");
    tbButtons[5].iString = SendMessage(m_hTextToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"Edit Info");
    tbButtons[7].iString = SendMessage(m_hTextToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"Add Items");
    tbButtons[8].iString = SendMessage(m_hTextToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"Add Files");
    tbButtons[9].iString = SendMessage(m_hTextToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"Move Up");
    tbButtons[10].iString = SendMessage(m_hTextToolbar, TB_ADDSTRING, (WPARAM) 0, (LPARAM)"Move Down");
    SendMessage(m_hTextToolbar, TB_ADDBUTTONS, (WPARAM) 11, (LPARAM) &tbButtons);
    SendMessage(m_hTextToolbar, TB_AUTOSIZE, (WPARAM) 0, (LPARAM) 0);
   
    RECT buttonRect;
    SIZE sizeString;
    char temp[] = "HowBigAmI?";
    HDC hdc = GetDC(m_hTextToolbar);

    SendMessage(m_hTextToolbar, TB_GETITEMRECT , (WPARAM) 0, (LPARAM) &buttonRect); 

    GetTextExtentPoint32(hdc, temp, strlen(temp), &sizeString); 
    
    ReleaseDC(m_hTextToolbar, hdc);

    SendMessage(m_hTextToolbar, TB_SETBUTTONSIZE , 0, 
        MAKELPARAM(buttonRect.right - buttonRect.left, sizeString.cy + 6));

    bool useTextLabels, useImages;
    m_context->prefs->GetPrefBoolean(kShowToolbarTextLabelsPref, &useTextLabels);
    m_context->prefs->GetPrefBoolean(kShowToolbarImagesPref, &useImages);

    AddToolbarButtons(useTextLabels, useImages);
}

void MusicBrowserUI::SetTitles()
{
    if (m_pParent == NULL)
    {
       SetWindowText(m_hPlaylistTitle, 
                     "Currently Listening To:");
       SetWindowText(m_hWnd, 
                     "My Music - " BRANDING);
    }   
    else
    {
       string oTitle, oName;
       
       if (m_currentListName.length() == 0)
          oName = "New Playlist";
       else
       {
          char szBase[MAX_PATH];
       
          _splitpath(m_currentListName.c_str(), NULL, NULL, szBase, NULL);
          oName = szBase;
       }
       
       oTitle = string("Editing playlist: ") + oName;
       SetWindowText(m_hPlaylistTitle, 
                     oTitle.c_str());
       oTitle = "Editing " + oName + " - " + BRANDING;
       SetWindowText(m_hWnd, oTitle.c_str());
    }   
}

void MusicBrowserUI::DropFiles(HDROP dropHandle, bool filesAreURLs)
{
    uint32 count;
    char url[1024];

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

        //MessageBox(NULL, url, "url", MB_OK);
    }
}

BOOL MusicBrowserUI::SetCursor(int hitTest, int mouseMsg)
{
    BOOL result = FALSE;

    if(hitTest == HTCLIENT)
    {
        result = TRUE;
    }
    return result;
}

void MusicBrowserUI::MouseMove(uint32 uFlags, POINT &sPoint)
{
    if(m_trackSplitter)
    {
        HDC hdc = GetDC(NULL);
        POINT pt = sPoint;

        MapWindowPoints(m_hWnd, NULL, (LPPOINT)&pt, 1);

        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, m_splitterBrush);

        PatBlt(hdc,
               m_splitterRect.left, 
               m_splitterRect.top, 
               m_splitterRect.right - m_splitterRect.left, 
               m_splitterRect.bottom - m_splitterRect.top, 
               PATINVERT);

        OffsetRect(&m_splitterRect, (pt.x - m_splitterRect.left), 0);

        PatBlt(hdc,
               m_splitterRect.left, 
               m_splitterRect.top, 
               m_splitterRect.right - m_splitterRect.left, 
               m_splitterRect.bottom - m_splitterRect.top, 
               PATINVERT);

        SelectObject(hdc, oldBrush);

        ReleaseDC(NULL, hdc);
    }
    else
    {
        /*RECT catalogRect,playlistRect;
        
        GetWindowRect(m_hMusicView, &catalogRect);
        GetWindowRect(m_hPlaylistView, &playlistRect);
        MapWindowPoints(NULL, m_hWnd, (LPPOINT)&catalogRect, 2);
        MapWindowPoints(NULL, m_hWnd, (LPPOINT)&playlistRect, 2);

        m_splitterRect.top = playlistRect.top;
        m_splitterRect.bottom = playlistRect.bottom;
        m_splitterRect.left = catalogRect.right + 1;
        m_splitterRect.right = playlistRect.left - 1;*/

        if(PtInRect(&m_splitterRect, sPoint))
        {
            //m_overSplitter = true;
            ::SetCursor(m_hSplitterCursor);
        }
        else
        {
            //m_overSplitter = false;
            ::SetCursor(LoadCursor(NULL, IDC_ARROW));
        }
    }
} 

void MusicBrowserUI::MouseButtonDown(int keys, int x, int y)
{
    RECT catalogRect,playlistRect;
    RECT clientRect;
    POINT pt;

    pt.x = x;
    pt.y = y;

    GetClientRect(m_hWnd, &clientRect);
    //InflateRect(&clientRect, -150, 0);
    clientRect.left += 125;
    clientRect.right -= 175;
    GetWindowRect(m_hMusicView, &catalogRect);
    GetWindowRect(m_hPlaylistView, &playlistRect);
    MapWindowPoints(m_hWnd, NULL, (LPPOINT)&pt, 1);
    MapWindowPoints(m_hWnd, NULL, (LPPOINT)&clientRect, 2);

    m_splitterRect.top = playlistRect.top;
    m_splitterRect.bottom = playlistRect.bottom;
    m_splitterRect.left = catalogRect.right + 1;
    m_splitterRect.right = playlistRect.left - 1;

    if(PtInRect(&m_splitterRect, pt))
    {
        m_trackSplitter = true;
        SetCapture(m_hWnd);
        ClipCursor(&clientRect);
        m_hSavedCursor = ::SetCursor(m_hSplitterCursor);

        HDC hdc = GetDC(NULL);

        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, m_splitterBrush);

        PatBlt(hdc,
               m_splitterRect.left, 
               m_splitterRect.top, 
               m_splitterRect.right - m_splitterRect.left, 
               m_splitterRect.bottom - m_splitterRect.top, 
               PATINVERT);

        SelectObject(hdc, oldBrush);

        ReleaseDC(NULL, hdc);
    }
}

void MusicBrowserUI::MouseButtonUp(int keys, int x, int y)
{
    POINT pt;

    pt.x = x;
    pt.y = y;

    if(m_trackSplitter)
    {
        m_trackSplitter = false;
        ReleaseCapture();
        ClipCursor(NULL);
        ::SetCursor(m_hSavedCursor);

        HDC hdc = GetDC(NULL);

        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, m_splitterBrush);

        PatBlt(hdc,
               m_splitterRect.left, 
               m_splitterRect.top, 
               m_splitterRect.right - m_splitterRect.left, 
               m_splitterRect.bottom - m_splitterRect.top, 
               PATINVERT);

        SelectObject(hdc, oldBrush);

        ReleaseDC(NULL, hdc);


        RECT catalogRect, playlistRect, titleRect;
        int32 delta;

        GetWindowRect(m_hMusicView, &catalogRect);
        GetWindowRect(m_hPlaylistView, &playlistRect);
        GetWindowRect(m_hPlaylistTitle, &titleRect);

        delta = (m_splitterRect.right + 1) - playlistRect.left;
        catalogRect.right = m_splitterRect.left - 1;
        playlistRect.left = m_splitterRect.right + 1;
        titleRect.left += delta;

        MapWindowPoints(NULL, m_hWnd, (LPPOINT)&catalogRect, 2);
        MapWindowPoints(NULL, m_hWnd, (LPPOINT)&playlistRect, 2);
        MapWindowPoints(NULL, m_hWnd, (LPPOINT)&titleRect, 2);

        
        MoveWindow(m_hMusicView, 
                  catalogRect.left, 
                  catalogRect.top, 
        	      (catalogRect.right - catalogRect.left),
                  catalogRect.bottom - catalogRect.top,
                  TRUE);

        MoveWindow(m_hPlaylistView, 
                  playlistRect.left,
                  playlistRect.top, 
        	      (playlistRect.right - playlistRect.left),
                  playlistRect.bottom - playlistRect.top,
                  TRUE);

        MoveWindow(m_hPlaylistTitle, 
                  titleRect.left,
                  titleRect.top, 
        	      (titleRect.right - titleRect.left),
                  titleRect.bottom - titleRect.top,
                  TRUE);

        m_splitterRect.top = playlistRect.top;
        m_splitterRect.bottom = playlistRect.bottom;
        m_splitterRect.left = catalogRect.right + 1;
        m_splitterRect.right = playlistRect.left - 1;
    }
}

void MusicBrowserUI::UpdateButtonStates()
{
    // start off disabling everything, then enable only what 
    // should be.

    SendMessage(m_hToolbar, TB_ENABLEBUTTON,
                ID_EDIT_REMOVE, MAKELPARAM(FALSE, 0));

    SendMessage(m_hToolbar, TB_ENABLEBUTTON,
                ID_EDIT_EDITINFO, MAKELPARAM(FALSE, 0));

    SendMessage(m_hToolbar, TB_ENABLEBUTTON,
                ID_EDIT_ADDTRACK, MAKELPARAM(FALSE, 0));    

    SendMessage(m_hToolbar, TB_ENABLEBUTTON,
                ID_EDIT_MOVEUP, MAKELPARAM(FALSE, 0));

    SendMessage(m_hToolbar, TB_ENABLEBUTTON,
                ID_EDIT_MOVEDOWN, MAKELPARAM(FALSE, 0));

    // File Menu
    SendMessage(m_hToolbar, TB_ENABLEBUTTON, 
                    ID_FILE_SAVEPLAYLIST, MAKELPARAM(m_bListChanged, 0)); 

    if(m_hPlaylistView == GetFocus())
    {
        // Can we move items up and down?
        uint32 index = ListView_GetItemCount(m_hPlaylistView);
        uint32 count = ListView_GetSelectedCount(m_hPlaylistView);
        uint32 state;

        state = ListView_GetItemState(m_hPlaylistView, 
                                      index - 1, 
                                      LVIS_SELECTED);

        SendMessage(m_hToolbar, TB_ENABLEBUTTON, 
                    ID_EDIT_MOVEDOWN, MAKELPARAM(!(state & LVIS_SELECTED || !count), 0)); 

        state = ListView_GetItemState(m_hPlaylistView, 
                                      0, 
                                      LVIS_SELECTED);

        SendMessage(m_hToolbar, TB_ENABLEBUTTON, 
                    ID_EDIT_MOVEUP, MAKELPARAM(!(state & LVIS_SELECTED || !count), 0)); 
        
        SendMessage(m_hToolbar, TB_ENABLEBUTTON, 
                    ID_EDIT_REMOVE, MAKELPARAM(count!=0, 0)); 

        SendMessage(m_hToolbar, TB_ENABLEBUTTON, 
                    ID_EDIT_EDITINFO, MAKELPARAM(count!=0, 0)); 

        SendMessage(m_hToolbar, TB_ENABLEBUTTON, 
            ID_EDIT_ADDTRACK, 0); 
    }

    // what about items which deal with the treeview?

    //HTREEITEM treeSelect = TreeView_GetSelection(m_hMusicView);

    if(m_hMusicView == GetFocus())
    {
        SendMessage(m_hToolbar, TB_ENABLEBUTTON, 
            ID_EDIT_ADDTRACK, 0); 

        uint32 trackCount = 0;
        uint32 playlistCount = 0;

        trackCount = GetSelectedTrackCount();
        playlistCount = GetSelectedPlaylistCount();

        if(trackCount + playlistCount)
        {
            if(!IsItemSelected(m_hNewPlaylistItem) &&
               !IsItemSelected(m_hNewPlaylistItem))
            {
                SendMessage(m_hToolbar, TB_ENABLEBUTTON, 
                            ID_EDIT_ADDTRACK, MAKELPARAM(TRUE, 0)); 
            }

            if(!IsItemSelected(m_hPlaylistItem) &&
               !IsItemSelected(m_hNewPlaylistItem) && 
               !IsItemSelected(m_hPortableItem) &&
               !IsItemSelected(m_hNewPortableItem) &&
			   !IsItemSelected(m_hMyMusicItem) &&
               !IsItemSelected(m_hAllItem))
            {
                SendMessage(m_hToolbar, TB_ENABLEBUTTON, 
                        ID_EDIT_REMOVE, MAKELPARAM(TRUE, 0)); 
            }
 
            if(!IsItemSelected(m_hPlaylistItem) &&
               !IsItemSelected(m_hNewPlaylistItem) && 
               !IsItemSelected(m_hPortableItem) &&
               !IsItemSelected(m_hNewPortableItem) &&
               trackCount)
            {
                SendMessage(m_hToolbar, TB_ENABLEBUTTON, 
                        ID_EDIT_EDITINFO, MAKELPARAM(TRUE, 0));
            }
 
            /*if(!IsItemSelected(m_hNewPlaylistItem) && !IsItemSelected(m_hAllItem) &&
               !IsItemSelected(m_hMyMusicItem) && !IsItemSelected(m_hUncatItem) &&
               !IsItemSelected(m_hPlaylistItem) && !IsItemSelected(m_hPortableItem) &&
               (playlistCount && !trackCount) &&
               !IsItemSelected(m_hMyMusicItem) &&
               !IsItemSelected(m_hUncatItem) &&
               !IsItemSelected(m_hAllItem))
            {
                SendMessage(m_hToolbar, TB_ENABLEBUTTON, 
                        ID_EDIT_EDITINFO, MAKELPARAM(TRUE, 0)); 
            }*/       
        }
        else
        {
            // Edit Menu
            SendMessage(m_hToolbar, TB_ENABLEBUTTON, ID_EDIT_REMOVE, 0); 
            SendMessage(m_hToolbar, TB_ENABLEBUTTON, ID_EDIT_EDITINFO, 0);  
        }
 
    }
    
    //SendMessage(m_hToolbar, TB_ENABLEBUTTON, ID_EDIT_EDIT, 
    //            MAKELONG(oType.length() != 0, 0));    
    //SendMessage(m_hToolbar, TB_ENABLEBUTTON, ID_EDIT_REMOVE, 
    ///            MAKELONG(oType.length() != 0, 0)); 
}

void MusicBrowserUI::UpdateMenuStates()
{
    HMENU        hMenuRoot, hMenu;
    MENUITEMINFO sMenuItem;

    hMenuRoot = GetMenu(m_hWnd);


    // File Menu
    hMenu = GetSubMenu(hMenuRoot, 0); 

    EnableMenuItem(hMenu, ID_FILE_SAVEPLAYLIST, 
                   m_bListChanged ? MF_ENABLED : MF_GRAYED );

    // start off disabled... might enable below
    EnableMenuItem(hMenu, ID_FILE_EXPORTPLAYLIST, MF_GRAYED);

    // Edit Menu
    hMenu = GetSubMenu(hMenuRoot, 1);

    // start off disabled... might enable below
    EnableMenuItem(hMenu, ID_EDIT_REMOVE, MF_GRAYED);
    EnableMenuItem(hMenu, ID_EDIT_EDITINFO, MF_GRAYED);

    sMenuItem.cbSize = sizeof(MENUITEMINFO);
    sMenuItem.fMask =  MIIM_DATA|MIIM_TYPE;
    sMenuItem.fType = MFT_STRING;
    sMenuItem.dwTypeData = "&Remove Items...";
    sMenuItem.cch = strlen(sMenuItem.dwTypeData);

    SetMenuItemInfo(hMenu, ID_EDIT_REMOVE, false, &sMenuItem);

    hMenu = GetSubMenu(hMenuRoot, 1);

    // Can we move items up and down?
    uint32 index = ListView_GetItemCount(m_hPlaylistView);
    uint32 count = ListView_GetSelectedCount(m_hPlaylistView);

    if(count && m_hPlaylistView == GetFocus())
    {
        uint32 state;

        state = ListView_GetItemState(m_hPlaylistView, 
                                      index - 1, 
                                      LVIS_SELECTED);

        EnableMenuItem(hMenu, ID_EDIT_MOVEDOWN, 
                       (state & LVIS_SELECTED) ? MF_GRAYED : MF_ENABLED );

        state = ListView_GetItemState(m_hPlaylistView, 
                                      0, 
                                      LVIS_SELECTED);

        EnableMenuItem(hMenu, ID_EDIT_MOVEUP, 
                       (state & LVIS_SELECTED) ? MF_GRAYED : MF_ENABLED );

        EnableMenuItem(hMenu, ID_EDIT_REMOVE, MF_ENABLED);

        EnableMenuItem(hMenu, ID_EDIT_EDITINFO, MF_ENABLED);

        sMenuItem.cbSize = sizeof(MENUITEMINFO);
        sMenuItem.fMask = MIIM_DATA|MIIM_TYPE;
        sMenuItem.fType = MFT_STRING;
        sMenuItem.dwTypeData = "&Remove Tracks from Playlist";
        sMenuItem.cch = strlen(sMenuItem.dwTypeData);

        SetMenuItemInfo(hMenu, ID_EDIT_REMOVE, false, &sMenuItem);
    }
    else
    {    
        EnableMenuItem(hMenu, ID_EDIT_MOVEUP, MF_GRAYED);
        EnableMenuItem(hMenu, ID_EDIT_MOVEDOWN, MF_GRAYED);
    }

    // what about items which deal with the treeview?

    EnableMenuItem(hMenu, ID_EDIT_ADDTRACK, MF_GRAYED);

    //HTREEITEM treeSelect = TreeView_GetSelection(m_hMusicView);

    uint32 trackCount = 0;
    uint32 playlistCount = 0;

    trackCount = GetSelectedTrackCount();
    playlistCount = GetSelectedPlaylistCount();

    if((trackCount + playlistCount) && m_hMusicView == GetFocus())
    {
        if(!IsItemSelected(m_hNewPlaylistItem) &&
            !IsItemSelected(m_hNewPlaylistItem))
        {
            EnableMenuItem(hMenu, ID_EDIT_ADDTRACK, MF_ENABLED );
        }

        if(!IsItemSelected(m_hPlaylistItem) &&
           !IsItemSelected(m_hNewPlaylistItem) && 
           !IsItemSelected(m_hPortableItem) &&
           !IsItemSelected(m_hNewPortableItem) &&
		   !IsItemSelected(m_hMyMusicItem) &&
           !IsItemSelected(m_hAllItem))
        {
            EnableMenuItem(hMenu, ID_EDIT_REMOVE, MF_ENABLED);
        }

        if(!IsItemSelected(m_hPlaylistItem) &&
           !IsItemSelected(m_hNewPlaylistItem) && 
           !IsItemSelected(m_hPortableItem) &&
           !IsItemSelected(m_hNewPortableItem) &&
           trackCount)
        {
            EnableMenuItem(hMenu, ID_EDIT_EDITINFO, MF_ENABLED);
        }

        /*if(!IsItemSelected(m_hNewPlaylistItem) && !IsItemSelected(m_hAllItem) &&
           !IsItemSelected(m_hMyMusicItem) && !IsItemSelected(m_hUncatItem) &&
           !IsItemSelected(m_hPlaylistItem) && !IsItemSelected(m_hPortableItem) &&
           (playlistCount && !trackCount) &&
           !IsItemSelected(m_hMyMusicItem) &&
           !IsItemSelected(m_hUncatItem) &&
           !IsItemSelected(m_hAllItem))
        {
            EnableMenuItem(hMenu, ID_EDIT_EDITINFO, MF_ENABLED);
            SendMessage(m_hToolbar, TB_ENABLEBUTTON, 
                    ID_EDIT_EDITINFO, MAKELPARAM(TRUE, 0)); 
        }*/       

        sMenuItem.cbSize = sizeof(MENUITEMINFO);
        sMenuItem.fMask =  MIIM_DATA|MIIM_TYPE;
        sMenuItem.fType = MFT_STRING;
        sMenuItem.dwTypeData = "&Remove Items from Catalog...";
        sMenuItem.cch = strlen(sMenuItem.dwTypeData);

        SetMenuItemInfo(hMenu, ID_EDIT_REMOVE, false, &sMenuItem);

        TV_ITEM tv_item;
        bool enableEditPlaylist = false;

        if(m_hNewPlaylistItem)
        {
            // get the first playlist item
            tv_item.hItem = TreeView_GetChild(m_hMusicView, m_hPlaylistItem);
            tv_item.mask = TVIF_STATE|TVIF_PARAM;
            tv_item.stateMask = TVIS_SELECTED;
            tv_item.state = 0;

            // skip the "Create New Playlist..." item
            if(tv_item.hItem)
                tv_item.hItem = TreeView_GetNextSibling(m_hMusicView, tv_item.hItem);

            if(tv_item.hItem)
            {
                BOOL result = FALSE;

                do
                {
                    result = TreeView_GetItem(m_hMusicView, &tv_item);

                    if(result && (tv_item.state & TVIS_SELECTED))
                    {
                        enableEditPlaylist = true;               
                        break;
                    }
            
                }while(result &&
                       (tv_item.hItem = TreeView_GetNextSibling(m_hMusicView, 
                                                                tv_item.hItem)));
            }
        }

        EnableMenuItem(hMenu, ID_EDIT_EDITPLAYLIST, 
                        enableEditPlaylist ? MF_ENABLED : MF_GRAYED);

        EnableMenuItem(GetSubMenu(hMenuRoot, 0), ID_FILE_EXPORTPLAYLIST, 
                       enableEditPlaylist ? MF_ENABLED : MF_GRAYED);
    }
    else
    {
        EnableMenuItem(hMenu, ID_EDIT_EDITPLAYLIST, MF_GRAYED);
    }

    sMenuItem.cbSize = sizeof(MENUITEMINFO);
    sMenuItem.fMask =  MIIM_DATA;
    sMenuItem.fType = MFT_STRING;
    //sMenuItem.fState = (oType.length() == 0) ? MFS_GRAYED : MFS_ENABLED;

    //oText = string("&Edit") + oType + string("...");
    //sMenuItem.dwTypeData = (char *)oText.c_str();
    //sMenuItem.cch = strlen(sMenuItem.dwTypeData);
    //SetMenuItemInfo(hMenu, ID_EDIT_EDIT, false, &sMenuItem);

    //oText = string("&Remove") + oType + string(" from Catalog...");
    //sMenuItem.dwTypeData = (char *)oText.c_str();
    //sMenuItem.cch = strlen(sMenuItem.dwTypeData);
    //SetMenuItemInfo(hMenu, ID_EDIT_REMOVE, false, &sMenuItem);
    
    //oText = string("Remove") + oType + string(" from &Disk...");
    //sMenuItem.dwTypeData = (char *)oText.c_str();
    //sMenuItem.cch = strlen(sMenuItem.dwTypeData);
    //SetMenuItemInfo(hMenu, ID_EDIT_REMOVEFROMDISK, false, &sMenuItem);
    
    //SendMessage(m_hToolbar, TB_ENABLEBUTTON, ID_EDIT_EDIT, 
    //            MAKELONG(oType.length() != 0, 0));    
    //SendMessage(m_hToolbar, TB_ENABLEBUTTON, ID_EDIT_REMOVE, 
    ///            MAKELONG(oType.length() != 0, 0)); 
    
    // View Menu
    hMenu = GetSubMenu(hMenuRoot, 2);

    if(m_pParent)
    {
        EnableMenuItem(hMenu, ID_VIEW_LOCATECURRENTITEM, MF_GRAYED);  
    }

    // Controls Menu
    hMenu = GetSubMenu(hMenuRoot, 3);

    sMenuItem.fMask = MIIM_STATE;

	switch(m_plm->GetRepeatMode()) 
    {
		case kPlaylistMode_RepeatNone:
            sMenuItem.fState = MFS_CHECKED;
            SetMenuItemInfo(hMenu, ID_CONTROLS_REPEATNONE, false, &sMenuItem);
            sMenuItem.fState = MFS_UNCHECKED;
            SetMenuItemInfo(hMenu, ID_CONTROLS_REPEATONE, false, &sMenuItem);
            sMenuItem.fState = MFS_UNCHECKED;
            SetMenuItemInfo(hMenu, ID_CONTROLS_REPEATALL, false, &sMenuItem);
			break;

		case kPlaylistMode_RepeatOne:
            sMenuItem.fState = MFS_UNCHECKED;
            SetMenuItemInfo(hMenu, ID_CONTROLS_REPEATNONE, false, &sMenuItem);
            sMenuItem.fState = MFS_CHECKED;
            SetMenuItemInfo(hMenu, ID_CONTROLS_REPEATONE, false, &sMenuItem);
            sMenuItem.fState = MFS_UNCHECKED;
            SetMenuItemInfo(hMenu, ID_CONTROLS_REPEATALL, false, &sMenuItem);
			break;

		case kPlaylistMode_RepeatAll:
            sMenuItem.fState = MFS_UNCHECKED;
            SetMenuItemInfo(hMenu, ID_CONTROLS_REPEATNONE, false, &sMenuItem);
            sMenuItem.fState = MFS_UNCHECKED;
            SetMenuItemInfo(hMenu, ID_CONTROLS_REPEATONE, false, &sMenuItem);
            sMenuItem.fState = MFS_CHECKED;
            SetMenuItemInfo(hMenu, ID_CONTROLS_REPEATALL, false, &sMenuItem);
            break;
	}
    
    sMenuItem.fState = m_plm->GetShuffleMode() ? MFS_CHECKED:MFS_UNCHECKED;
    SetMenuItemInfo(hMenu, ID_CONTROLS_SHUFFLE, false, &sMenuItem);
 
    sMenuItem.fState = m_plm->GetShuffleMode() ? MFS_UNCHECKED: MFS_CHECKED;
    SetMenuItemInfo(hMenu, ID_CONTROLS_NORMALORDER, false, &sMenuItem);

}

static
UINT 
APIENTRY 
OpenFileHookProc(   HWND hwnd, 
                    UINT msg, 
                    WPARAM wParam, 
                    LPARAM lParam)
{
    UINT result = 0;
    static OPENFILENAME* ofn = NULL;

    switch(msg)
    {
        case WM_INITDIALOG:
        {
            ofn = (OPENFILENAME*)lParam;

            HWND hwndURL = GetDlgItem(hwnd, IDC_URL);
            
            vector<PlaylistItem*>* url_list = (vector<PlaylistItem*>*)ofn->lCustData; 
            vector<PlaylistItem*>::iterator i = url_list->begin();

            for(i; i != url_list->end(); i++)
            {
                ComboBox_AddString(hwndURL, (*i)->URL().c_str());               
            }

            ofn->lCustData = 0;
            break;
        }

        case WM_COMMAND:
        {
            switch(wParam)
            {
                case IDC_OPEN_URL:
                {
                    char* url = new char[ofn->nMaxFile + 1];

                    if(GetDlgItemText(  hwnd,
                                        IDC_URL,
                                        url,
                                        ofn->nMaxFile))
                    {
                        *ofn->lpstrFile = 0x00;

                        if(!strstr(url, "://"))
                        {
                            strcpy(ofn->lpstrFile, "http://");
                        }

                        strcat(ofn->lpstrFile, url);

                        PostMessage(GetParent(hwnd), 
                                    WM_COMMAND, 
                                    IDCANCEL,
                                    0);

                        ofn->lCustData = 1;
                    }

                    delete url;
                    
                    result = 1;
                    break;
                }
            }

            break;
        }

        case WM_NOTIFY:
        {
            OFNOTIFY* notify = (OFNOTIFY*)lParam;

            switch(notify->hdr.code)
            {
                case CDN_FILEOK:
                {
                    
                    break;
                }                
            }

            break;
        }
    }

    return result;
}

bool
MusicBrowserUI::
FileOpenDialog(HWND hwnd, 
               const char* title,
               const char* filter,
               vector<string>* fileList,
               bool allowURL)
{
    bool result = false;
    OPENFILENAME ofn;
    char initialDir[MAX_PATH] = {0x00};
    uint32 initialDirSize = sizeof(initialDir);
    char installDir[MAX_PATH] = {0x00};
    uint32 installDirSize = sizeof(installDir);
    char urlPath[MAX_PATH + 7];
    uint32 urlPathSize = sizeof(urlPath);
    const int32 kBufferSize = MAX_PATH * 128;
    char* fileBuffer = new char[kBufferSize];
    vector<PlaylistItem*> url_list;

    *fileBuffer = 0x00;

    m_context->prefs->GetPrefString(kInstallDirPref,  installDir, &initialDirSize);

    if(*installDir)
    {
        strcat(installDir, "\\urls.m3u");

        if(IsntError(FilePathToURL(installDir, urlPath, &urlPathSize)))
        {
            m_plm->ReadPlaylist(urlPath, &url_list);
        }
    }

    m_context->prefs->GetPrefString(kOpenSaveDirPref,  initialDir, &initialDirSize);

    int hookFlags = 0;

    if(allowURL)
        hookFlags = OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;

    // Setup open file dialog box structure
    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = hwnd;
    ofn.hInstance         = g_hinst;
    ofn.lpstrFilter       = filter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0;
    ofn.nFilterIndex      = 1;
    ofn.lpstrFile         = fileBuffer;
    ofn.nMaxFile          = kBufferSize;
    ofn.lpstrFileTitle    = NULL;
    ofn.nMaxFileTitle     = 0;
    ofn.lpstrInitialDir   = initialDir;
    ofn.lpstrTitle        = title;
    ofn.Flags             = OFN_FILEMUSTEXIST | 
					        OFN_PATHMUSTEXIST |
  	     			        OFN_HIDEREADONLY | 
					        OFN_ALLOWMULTISELECT |
					        OFN_EXPLORER | 
                            hookFlags;
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = "MP3";
    ofn.lCustData         = (DWORD)(allowURL ? &url_list : NULL);
    ofn.lpfnHook          = (allowURL ? OpenFileHookProc : NULL);
    ofn.lpTemplateName    = (allowURL ? MAKEINTRESOURCE(IDD_OPENURL) : NULL);

    if(GetOpenFileName(&ofn) || ofn.lCustData)
    {
        char file[MAX_PATH];
        char* cp = NULL;

        if(ofn.lCustData) // URL
        {
            fileList->push_back(fileBuffer);

            if(*urlPath)
            {
                bool found = false;
                vector<PlaylistItem*>::iterator i = url_list.begin();

                for(i; i != url_list.end(); i++)
                {
                    if((*i)->URL() == fileBuffer)
                    {
                        PlaylistItem* item = *i;
                        url_list.erase(i);
                        url_list.insert(url_list.begin(), item);

                        found = true;
                        break;
                    }
                }

                if(!found)
                {
                    PlaylistItem* item = new PlaylistItem(fileBuffer);
                    url_list.insert(url_list.begin(), item);
                    int32 numToRemember = 10;

                    m_context->prefs->GetPrefInt32(kNumberOfURLsToRememberPref, &numToRemember);

                    if(url_list.size() > numToRemember)
                    {
                        i = url_list.begin() + numToRemember;

                        for(i; i != url_list.end(); i++)
                        {
                            delete *i;
                        }

                        url_list.erase(url_list.begin() + numToRemember, url_list.end());
                    }                    
                }

                m_plm->WritePlaylist(urlPath, &url_list);
            }
        }
        else // potential list of files
        {
            strncpy(file, fileBuffer, ofn.nFileOffset);

            if(*(fileBuffer + ofn.nFileOffset - 1) != DIR_MARKER)
                strcat(file, DIR_MARKER_STR);

            cp = fileBuffer + ofn.nFileOffset;

            while(*cp)
            {
                char url[MAX_PATH + 7];
                uint32 length = sizeof(url);

	            strcpy(file + ofn.nFileOffset, cp);

                FilePathToURL(file, url, &length);

                fileList->push_back(url);

	            cp += strlen(cp) + 1;
            }

            *(fileBuffer + ofn.nFileOffset - 1) = 0x00;

            m_context->prefs->SetPrefString(kOpenSaveDirPref, fileBuffer);
        }

        result = true;
    }

    /*if(CommDlgExtendedError() == CDERR_NOHINSTANCE)
    {   
        MessageBox(hwnd, "CDERR_NOHINSTANCE", 0, MB_OK); 
    }*/

    /*char buf[256];
    sprintf(buf, "%lx", CommDlgExtendedError());
    MessageBox(hwnd, buf, 0, MB_OK); */

    /*LPVOID lpMessageBuffer;

	FormatMessage(
	  FORMAT_MESSAGE_ALLOCATE_BUFFER |
	  FORMAT_MESSAGE_FROM_SYSTEM,
	  NULL,
	  CommDlgExtendedError(),
	  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	  (LPTSTR) &lpMessageBuffer,
	  0,
	  NULL );

	// now display this string
 	MessageBox(NULL, (char*)lpMessageBuffer, 0, MB_OK);

	// Free the buffer allocated by the system
	LocalFree( lpMessageBuffer );*/

    vector<PlaylistItem*>::iterator i = url_list.begin();

    for(i; i != url_list.end(); i++)
    {
        delete (*i);
    }

    delete [] fileBuffer;

    return result;
}

void ClientToWindow(HWND hWnd, POINT *Pt) 
{
    RECT wRect, cRect;
 
    GetWindowRect(hWnd, &wRect);
    GetClientRect(hWnd, &cRect);
    MapWindowPoints(hWnd, HWND_DESKTOP, (LPPOINT)&cRect, 2);
    Pt->x += (wRect.left - cRect.left);
    Pt->y += (wRect.top - cRect.top);
}

uint32 MusicBrowserUI::CalcStringEllipsis(HDC hdc, string& displayString, int32 columnWidth)
{
    const TCHAR szEllipsis[] = TEXT("...");
    SIZE   sizeString;
    SIZE   sizeEllipsis;
    string temp;
    
    // Adjust the column width to take into account the edges
    //columnWidth -= 4;

    temp = displayString;        

    GetTextExtentPoint32(hdc, temp.c_str(), temp.size(), &sizeString);       

    // If the width of the string is greater than the column width shave
    // the string and add the ellipsis
    if(sizeString.cx > columnWidth)
    {
        GetTextExtentPoint32(hdc, szEllipsis, strlen(szEllipsis), &sizeEllipsis);
       
        while(temp.size() > 1)
        {
            temp.erase(temp.size() - 1, 1);

            GetTextExtentPoint32(hdc, temp.c_str(), temp.size(), &sizeString);
            
            if ((uint32)(sizeString.cx + sizeEllipsis.cx) <= columnWidth)
            {
                // The string with the ellipsis finally fits                
                // Concatenate the two strings and break out of the loop
                temp += szEllipsis;
                displayString = temp;
                break;
            }
            else if(temp.size() == 1)
            {
                temp += szEllipsis;
                displayString = temp;
                break;
            }
        }
    }

    GetTextExtentPoint32(hdc, displayString.c_str(), displayString.size(), &sizeString);

    return sizeString.cx;
}

