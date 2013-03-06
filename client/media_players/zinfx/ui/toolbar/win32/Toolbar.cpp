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
	
	$Id: Toolbar.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
____________________________________________________________________________*/

/* system headers */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include <shellapi.h>
#include <stdio.h>
#include <string.h>

#include <string>

using namespace std;

#include "event.h"
#include "eventdata.h"
#include "Toolbar.h"
#include "resource.h"
#include "debug.h"

#define UWM_TRAY  WM_USER + 666

HINSTANCE g_hinst = NULL;

extern "C" ToolbarUI *Initialize(FAContext *context)
{
    return new ToolbarUI(context);
}

INT 
WINAPI 
DllMain (   HINSTANCE hInstance,
            ULONG ul_reason_being_called,
	        LPVOID lpReserved)
{
	switch (ul_reason_being_called)
	{
		case DLL_PROCESS_ATTACH:
			g_hinst = hInstance;
	    	break;

		case DLL_THREAD_ATTACH:
		    break;

		case DLL_THREAD_DETACH:
		    break;

		case DLL_PROCESS_DETACH:
		    break;
	}

    return 1;                 
}

static
LRESULT 
WINAPI
MainWndProc(HWND hwnd, 
            UINT msg, 
            WPARAM wParam, 
            LPARAM lParam )
{
    LRESULT result = 0;
    ToolbarUI* ui = (ToolbarUI*)GetWindowLong(hwnd, GWL_USERDATA);

    switch (msg)
    {
        case WM_CREATE:
        {
            // When we create the window we pass in a pointer to our
            // UserInterface object...
            // Tuck away the pointer in a safe place
            
            ui = (ToolbarUI*)((LPCREATESTRUCT)lParam)->lpCreateParams;

            assert(ui != NULL);
          
            result = SetWindowLong(hwnd, GWL_USERDATA, (LONG)ui);
            
            break;
        }
        case UWM_TRAY:
        {
            ui->TrayNotify(lParam);
            break;
        }

        default:
            result = DefWindowProc( hwnd, msg, wParam, lParam );
            break;
    }

    return result;
}

ToolbarUI::
ToolbarUI(FAContext *context):
UserInterface()
{
    m_context       = context;
    m_hWnd          = NULL;
    m_trayIcon      = NULL;
	m_state         = UIState_Stopped, 
    *m_trayTooltip  = 0x00;
    m_uiThread      = NULL;
}

ToolbarUI::
~ToolbarUI()
{
   delete m_uiThread;    
}

Error ToolbarUI::Init(int32 startup_type)
{
   assert(this);

   m_uiThread = Thread::CreateThread();
   m_uiThread->Create(ui_thread_function, this);

   return kError_NoErr;
}

void 
ToolbarUI::
ui_thread_function(void* arg)
{
    ToolbarUI* ui = (ToolbarUI*)arg;

    ui->UIThreadFunction();
}

void
ToolbarUI::
UIThreadFunction()
{
    WNDCLASS wc;
    MSG msg;
    
    memset(&wc, 0x00, sizeof(WNDCLASS));

    wc.style = CS_OWNDC|CS_DBLCLKS ;
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = g_hinst;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hIcon = LoadIcon( g_hinst, MAKEINTRESOURCE(IDI_EXE_ICON) );
    wc.hbrBackground = NULL;//(HBRUSH)( COLOR_WINDOW + 1 );
    wc.lpszClassName = "ToolbarUI";

    m_trayIcon = LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_EXE_ICON));

    if( RegisterClass( &wc ) )    
    {
        m_hWnd = CreateWindow(   "ToolbarUI", 
                        "ToolbarUI Hidden Window",
                        WS_POPUP | WS_SYSMENU, 
                        0, 
                        0, 
                        0, 
                        0,
                        NULL, 
                        NULL, 
                        g_hinst, 
                        this );
                        
        SetupToolIcon();
                        
        if( m_hWnd )
        {
            while( GetMessage( &msg, NULL, 0, 0 ) )
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }
        RemoveTrayIcon();
        DestroyWindow(m_hWnd);
        m_hWnd = NULL;
    }    
}


Error 
ToolbarUI::
AcceptEvent(Event* event)
{
    Error result = kError_UnknownErr;

    if (event) 
    {
        result = kError_NoErr;

        switch (event->Type()) 
        {
            case INFO_Playing: 
            {   
                m_state = UIState_Playing;
	            break; 
            }

            case INFO_Paused: 
            {
                m_state = UIState_Paused;
	            break; 
            }

            case INFO_Stopped: 
            {
                m_state = UIState_Stopped;
	            break; 
            }

            case INFO_MediaInfo: 
            {
				MediaInfoEvent *info = (MediaInfoEvent*)event;

				char *foo = strrchr(info->m_filename,'\\');

                char title[256] = BRANDING" - ";
                if (foo)
                    strcat(title, foo);
                else
                    strcat(title, info->m_filename);    

                SetTrayTooltip(title);

				break; 
            }

            case INFO_PlaylistItemsUpdated:
            {
               PlaylistItemsUpdatedEvent *pInfo =
                  (PlaylistItemsUpdatedEvent *)event;
      
               vector<PlaylistItem*>::const_iterator i = 
                          pInfo->Items()->begin();
               for(; i != pInfo->Items()->end(); i++)
               {
                  if((*i) == m_context->plm->GetCurrentItem())
                  {
                      UpdateMetaData(*i);
                      break;
                  }
               }
            
               break;
            }
            case INFO_PlaylistCurrentItemInfo:
            {
               if (m_context->plm->GetCurrentIndex() != kInvalidIndex)
                   UpdateMetaData(m_context->plm->GetCurrentItem());
                     break;
            }

            case INFO_StreamInfo:
            { 
                StreamInfoEvent *info = (StreamInfoEvent*)event;

                char title[256] = BRANDING" - ";
                
                info->GetTitle(title + strlen(title), 256 - strlen(title));
                SetTrayTooltip(title);

                break;
            }

	        case CMD_Cleanup: 
            {
                if (m_hWnd)
                   PostMessage(m_hWnd, WM_QUIT, 0, 0);
                m_uiThread->Join();
	            m_context->target->AcceptEvent(new Event(INFO_ReadyToDieUI));
	            break; 
            }	       
            
            case INFO_PrefsChanged:
            {
                SetupToolIcon();
                break;
            }
	    }
    } 

    return result;
}

void ToolbarUI::UpdateMetaData(const PlaylistItem *pItem)
{
    string oTitle("");

    if (pItem->GetMetaData().Title().length() > 0 ||
        pItem->GetMetaData().Artist().length() > 0)
    {
        oTitle = pItem->GetMetaData().Title();
        if (pItem->GetMetaData().Artist().length() > 0)
           oTitle += string(" - ") + pItem->GetMetaData().Artist();
    }   
    else
    {
        string url;
        char path[MAX_PATH];
        uint32 len = MAX_PATH;

        url = pItem->URL();
        URLToFilePath(url.c_str(), path, &len);
        oTitle = string(path);
    }

    oTitle = string(BRANDING": ") + oTitle;
    SetTrayTooltip((char*)oTitle.c_str());
}

void ToolbarUI::SetupToolIcon(void)
{
    bool bShowIcon;
    
    m_context->prefs->GetPrefBoolean(kLiveInTrayPref, &bShowIcon);
    if (bShowIcon)
       AddTrayIcon();
    else
       RemoveTrayIcon();
}

void 
ToolbarUI::
TrayNotify(int32 notifyMessage)
{
    switch(notifyMessage)
    {
        case WM_LBUTTONDBLCLK:
        case WM_LBUTTONUP:
        {
            Int32PropValue *pProp;
            HWND            hWnd;
            if (IsError(m_context->props->GetProperty("MainWindow", 
                       (PropValue **)&pProp)))
               return;        
            else
               hWnd = (HWND)pProp->GetInt32();

            ShowWindow(hWnd, SW_RESTORE);
            SetForegroundWindow(hWnd);
            
			break;
        }

        case WM_RBUTTONUP:
        {
            HMENU menuHandle, popupHandle;
            HINSTANCE hinst = (HINSTANCE)GetWindowLong(m_hWnd, GWL_HINSTANCE);
            POINT pt;
            int32 command;

            // need mouse coordinates relative to screen
            GetCursorPos(&pt);
          
            // load the menu and retrieve its popup
            menuHandle = LoadMenu(hinst, MAKEINTRESOURCE(IDM_TRAY));
            popupHandle = GetSubMenu(menuHandle, 0);

            if(m_state == UIState_Playing)
            {
                MENUITEMINFO mii;

                memset(&mii, 0x00, sizeof(MENUITEMINFO));

                mii.cbSize = sizeof(MENUITEMINFO);
                mii.fMask = MIIM_TYPE|MIIM_ID ;
                mii.fType = MFT_STRING;
                mii.dwTypeData = "Pause";
                mii.cch = strlen("Pause");
                mii.wID = IDC_PAUSE;

                SetMenuItemInfo(popupHandle, 
                                2,
                                TRUE,
                                &mii);
            }

            SetForegroundWindow(m_hWnd);

            // display the popup
            command = TrackPopupMenu(   popupHandle,			
					                    TPM_RETURNCMD | TPM_RIGHTBUTTON |
                                        TPM_NONOTIFY,
					                    pt.x, 
                                        pt.y,       
					                    0,  
					                    m_hWnd,
					                    NULL);

            SetForegroundWindow(m_hWnd);

            switch(command)
            {
                case IDC_MYMUSIC:
                    m_context->target->AcceptEvent(
                               new Event(CMD_ToggleMusicBrowserUI));
                    break;

                case IDC_PLAY:
                    m_context->target->AcceptEvent(
                               new Event(CMD_Play));
                    break;

                case IDC_STOP:
                    m_context->target->AcceptEvent(
                               new Event(CMD_Stop));
                    break;

                case IDC_PAUSE:
                    m_context->target->AcceptEvent(
                               new Event(CMD_Pause));
                    break;

                case IDC_NEXTTRACK:
                    m_context->target->AcceptEvent(
                               new Event(CMD_NextMediaPiece));
                    break;

                case IDC_PREVIOUSTRACK:
                    m_context->target->AcceptEvent(
                               new Event(CMD_PrevMediaPiece));
                    break;

                case IDC_EXIT:
                    m_context->target->AcceptEvent(
                               new Event(CMD_QuitPlayer));
                    break;
            }

			break;
        }

    }
}



void 
ToolbarUI::
AddTrayIcon()
{
	NOTIFYICONDATA nid;
	int rc;

	// Fill out NOTIFYICONDATA structure
	nid.cbSize = sizeof(NOTIFYICONDATA);	// sanitycheck
	nid.hWnd = m_hWnd;					    // window to receive notifications
	nid.uID = 1;							// application defined identifier for icon
	nid.uFlags = NIF_MESSAGE |				// uCallbackMessage is valid, use it
				 NIF_ICON |					// hIcon is valid, use it
				 NIF_TIP;					// there is tooltip specified
	nid.uCallbackMessage = UWM_TRAY;        // that's what we will receive in wndProc
	nid.hIcon = m_trayIcon;
    
	strncpy(nid.szTip, m_trayTooltip, 63);
    nid.szTip[63] = 0;

	rc = Shell_NotifyIcon(NIM_ADD, &nid);	// this adds the icon
}

void
ToolbarUI::
RemoveTrayIcon()
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = m_hWnd;
	nid.uID = 1;
	nid.uFlags = NIF_ICON;
	nid.hIcon = m_trayIcon;

	Shell_NotifyIcon(NIM_DELETE, &nid);
}

void
ToolbarUI::
SetTrayTooltip(char *str)
{
	NOTIFYICONDATA nid;

    strncpy(m_trayTooltip, str, sizeof(m_trayTooltip));

	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = m_hWnd;
	nid.uID = 1;
	nid.uFlags = NIF_TIP;				// just change tip
	strncpy(nid.szTip, m_trayTooltip, 63);
    nid.szTip[63] = 0;
	Shell_NotifyIcon(NIM_MODIFY, &nid); // now, modify our tooltip
}


