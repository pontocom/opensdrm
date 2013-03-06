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
	
	$Id: Toolbar.cpp,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/

/* system headers */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include <shellapi.h>
#include <stdio.h>
#include <string.h>

using namespace std;

#include "resource.h"

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
        case WM_INITMENUPOPUP:
            ui->InitMenuPopup(  (HMENU) wParam,
                                (uint32)LOWORD(lParam),
                                (HIWORD(lParam) != 0));
            break;

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

    *m_trayTooltip = 0x00;
    m_liveInTray = false;
}

ToolbarUI::
~ToolbarUI()
{
    RemoveTrayIcon();

    if(m_trayIcon)
        DeleteObject(m_trayIcon);
}

void 
ToolbarUI::
Create()
{
    assert( m_hwnd );

    m_trayIcon = LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_EXE_ICON));
}

void 
ToolbarUI::
InitMenuPopup(HMENU menuHandle, 
              uint32 position, 
              bool systemMenu)
{
    /*if(systemMenu)
    {
        MENUITEMINFO mii;
        uint32 state = 0;
        Preferences prefs;
        bool stayOnTop;
        char szStayOnTop[] = "Stay On Top";
        char buffer[256];

        prefs.GetPrefBoolean(kStayOnTopPref, &stayOnTop);

        GetMenuString(  menuHandle, 
                        0, 
                        buffer, 
                        sizeof(buffer), 
                        MF_BYPOSITION);

        if(strcmp(buffer, szStayOnTop))
        {
            mii.cbSize = sizeof(MENUITEMINFO);
            mii.fMask = MIIM_TYPE;
            mii.fType = MFT_SEPARATOR;

            InsertMenuItem( menuHandle, 
                            0, 
                            TRUE, 
                            &mii);

            mii.cbSize = sizeof(MENUITEMINFO);
            mii.fMask = MIIM_TYPE|MIIM_STATE|MIIM_ID ;
            mii.fType = MFT_STRING;
            mii.fState = (stayOnTop ? MFS_CHECKED : MFS_UNCHECKED);
            mii.dwTypeData = szStayOnTop;
            mii.cch = strlen(szStayOnTop);
            mii.wID = kMenuStayOnTop;

            InsertMenuItem( menuHandle, 
                            0, 
                            TRUE,
                            &mii);
        }

        mii.cbSize = sizeof(MENUITEMINFO);
        mii.fMask = MIIM_STATE;
        mii.fState = (stayOnTop ? MFS_CHECKED : MFS_UNCHECKED);

        SetMenuItemInfo(menuHandle, 
                        0, 
                        TRUE, 
                        &mii);
    }*/
}


void 
ToolbarUI::
TrayNotify(int32 notifyMessage)
{
    switch(notifyMessage)
    {
        case WM_LBUTTONUP:
        {
            ShowWindow( m_hwnd, SW_NORMAL);
			SetForegroundWindow(m_hwnd);
			break;
        }

        case WM_RBUTTONUP:
        {
            HMENU menuHandle, popupHandle;
            HINSTANCE hinst = (HINSTANCE)GetWindowLong(m_hwnd, GWL_HINSTANCE);
            POINT pt;
            int32 command;

            // need mouse coordinates relative to screen
            GetCursorPos(&pt);
          
            // load the menu and retrieve its popup
            menuHandle = LoadMenu(hinst, MAKEINTRESOURCE(IDM_TRAY));
            popupHandle = GetSubMenu(menuHandle, 0);

            if(m_state != UIState_Stopped)
            {
                MENUITEMINFO mii;

                memset(&mii, 0x00, sizeof(MENUITEMINFO));

                mii.cbSize = sizeof(MENUITEMINFO);
                mii.fMask = MIIM_TYPE|MIIM_ID ;
                mii.fType = MFT_STRING;
                mii.dwTypeData = "Stop";
                mii.cch = strlen("Stop");
                mii.wID = IDC_STOP;

                SetMenuItemInfo(popupHandle, 
                                5,
                                TRUE,
                                &mii);
            }

            if(m_state == UIState_Paused)
            {
                MENUITEMINFO mii;

                memset(&mii, 0x00, sizeof(MENUITEMINFO));

                mii.cbSize = sizeof(MENUITEMINFO);
                mii.fMask = MIIM_STATE;
                mii.fState = MFS_CHECKED;

                SetMenuItemInfo(popupHandle, 
                                6,
                                TRUE,
                                &mii);
            }

            SetForegroundWindow(m_hwnd);

            // display the popup
            command = TrackPopupMenu(   popupHandle,			
					                    TPM_RETURNCMD | TPM_RIGHTBUTTON |
                                        TPM_NONOTIFY,
					                    pt.x, 
                                        pt.y,       
					                    0,  
					                    m_hwnd,
					                    NULL);

            SetForegroundWindow(m_hwnd);

            switch(command)
            {
                case IDC_ABOUT:
                    SendMessage(m_hwnd, WM_COMMAND, kAboutControl, 0);
                    break;

                case IDC_OPEN:
                    SendMessage(m_hwnd, WM_COMMAND, kOpenControl, 0);
                    break;

                case IDC_SAVE:
                    SendMessage(m_hwnd, WM_COMMAND, kSaveControl, 0);
                    break;

                case IDC_PREF:
                    SendMessage(m_hwnd, WM_COMMAND, kPrefControl, 0);
                    break;

                case IDC_PLAY:
                    SendMessage(m_hwnd, WM_COMMAND, kPlayControl, 0);
                    break;

                case IDC_STOP:
                    SendMessage(m_hwnd, WM_COMMAND, kStopControl, 0);
                    break;

                case IDC_PAUSE:
                    SendMessage(m_hwnd, WM_COMMAND, kPauseControl, 0);
                    break;

                case IDC_NEXTSONG:
                    SendMessage(m_hwnd, WM_COMMAND, kNextControl, 0);
                    break;

                case IDC_LASTSONG:
                    SendMessage(m_hwnd, WM_COMMAND, kLastControl, 0);
                    break;

                case IDC_EXIT:
                    SendMessage(m_hwnd, WM_COMMAND, kCloseControl, 0);
                    break;
            }

			break;
        }

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
                m_stopView->Show();
                m_playView->Hide();
                m_pauseView->SetState(Unpressed);
	            break; 
            }

            case INFO_Paused: 
            {
                m_pauseView->SetState(Pressed);
                m_state = UIState_Paused;
	            break; 
            }

            case INFO_Stopped: 
            {
                m_state = UIState_Stopped;
                m_playView->Show();
                m_stopView->Hide();
                m_pauseView->SetState(Unpressed);
                m_timeView->SetCurrentTime(0, 0, 0);
	            break; 
            }

            case INFO_MediaInfo: 
            {
				MediaInfoEvent *info = (MediaInfoEvent*)event;

				char *foo = strrchr(info->m_filename,'\\');

                char title[256] = BRANDING" - ";

fix me
!
                SetTrayTooltip(m_songTitleView->Text());

				break; 
            }

            case INFO_StreamInfo:
            { 
                StreamInfoEvent *info = (StreamInfoEvent*)event;

                char title[256] = BRANDING" - ";


                SetTrayTooltip(m_songTitleView->Text());

                break;
            }

	        case CMD_Cleanup: 
            {
	            m_target->AcceptEvent(new Event(INFO_ReadyToDieUI));
	            break; 
            }	       
	    }
    } 

    return result;
}


void 
ToolbarUI::
AddTrayIcon()
{
	NOTIFYICONDATA nid;
	int rc;

	// Fill out NOTIFYICONDATA structure
	nid.cbSize = sizeof(NOTIFYICONDATA);	// sanitycheck
	nid.hWnd = m_hwnd;					    // window to receive notifications
	nid.uID = 1;							// application defined identifier for icon
	nid.uFlags = NIF_MESSAGE |				// uCallbackMessage is valid, use it
				 NIF_ICON |					// hIcon is valid, use it
				 NIF_TIP;					// there is tooltip specified
	nid.uCallbackMessage = UWM_TRAY;        // that's what we will receive in wndProc
	nid.hIcon = m_trayIcon;

	strcpy(nid.szTip, m_trayTooltip);

	rc = Shell_NotifyIcon(NIM_ADD, &nid);	// this adds the icon
}

void
ToolbarUI::
RemoveTrayIcon()
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = m_hwnd;
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
	nid.hWnd = m_hwnd;
	nid.uID = 1;
	nid.uFlags = NIF_TIP;				// just change tip
	strcpy(nid.szTip, m_trayTooltip);
	Shell_NotifyIcon(NIM_MODIFY, &nid); // now, modify our tooltip
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
    wc.lpszClassName = BRANDING;

    if( RegisterClass( &wc ) )    
    {
        CreateWindow(   BRANDING, 
                        BRANDING,
                        WS_POPUP | WS_VISIBLE | WS_SYSMENU, 
                        0, 
                        0, 
                        0, 
                        0,
                        NULL, 
                        NULL, 
                        g_hinst, 
                        this );
        if( m_hwnd )
        {
            ShowWindow( m_hwnd, SW_NORMAL );
            UpdateWindow( m_hwnd );

            m_uiSemaphore->Signal();

            while( GetMessage( &msg, NULL, 0, 0 ) )
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }
    }    

    m_target->AcceptEvent(new Event(CMD_QuitPlayer));
}

void 
ToolbarUI::
ui_thread_function(void* arg)
{
    ToolbarUI* ui = (ToolbarUI*)arg;

    ui->UIThreadFunction();
}
