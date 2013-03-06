/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player

	Portions Copyright (C) 1998 GoodNoise

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
	
	$Id: zinfui.cpp,v 1.1 2004/10/13 16:17:07 linha2 Exp $
____________________________________________________________________________*/

/* system headers */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <assert.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <mmsystem.h >
#include <sys/stat.h>

/* project headers */
#include "config.h"
#include "thread.h"
#include "freeampui.h"
#include "event.h"
#include "eventdata.h"
#include "playlist.h"
#include "utility.h"
#include "renderer.h"
#include "fontwidth.h"
#include "preferences.h"
#include "prefdialog.h"

#include "stringitem.h"

#include "resource.h"

#define TIMER_MOUSE_POSITION    0
#define TIMER_SEEK_POSITION     1
#define TIMER_VOLUME_POSITION   2
#define TIMER_SHOW_STATUS_INFO  3

#define UWM_TRAY  WM_USER + 666

const char* kAudioFileFilter =
            "MPEG Audio Streams (.mpg, .mp1, .mp2, .mp3, .mpp)\0"
            "*.mpg;*.mp1;*.mp2;*.mp3;*.mpp\0"
            "Playlists (.m3u)\0"
            "*.m3u\0"
            "All Files (*.*)\0"
            "*.*\0";

const char* kOpenAudioFileTitle = "Open Audio File";

const char* kPlaylistFileFilter =
            "Playlists (.m3u)\0"
            "*.m3u\0"
            "All Files (*.*)\0"
            "*.*\0";

const char* kOpenPlaylistFileTitle = "Open Playlist File";
const char* kSavePlaylistFileTitle = "Save Playlist File";

HINSTANCE g_hinst = NULL;

extern "C" FreeAmpUI *Initialize(FAContext *context)
{
    return new FreeAmpUI(context);
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
    FreeAmpUI* ui = (FreeAmpUI*)GetWindowLong(hwnd, GWL_USERDATA);

    switch (msg)
    {
        case WM_CREATE:
        {
            // When we create the window we pass in a pointer to our
            // UserInterface object...
            // Tuck away the pointer in a safe place
            
            ui = (FreeAmpUI*)((LPCREATESTRUCT)lParam)->lpCreateParams;

            assert(ui != NULL);
          
            result = SetWindowLong(hwnd, GWL_USERDATA, (LONG)ui);

            // remember what window we are associated with
            ui->SetWindowHandle(hwnd);

            // Now we can start using the object's methods
            ui->Create();
            break;
        }

        case WM_DESTROY:
            ui->Destroy();
            break;

        case WM_PAINT:
            result = ui->Paint();
            break;

        case WM_PALETTECHANGED:
            if(hwnd == (HWND)wParam)
                break;

        case WM_QUERYNEWPALETTE:
        {
            if(ui->Palette())
            {
                HDC hdc = GetDC(hwnd);
                int32 numEntries;

                SelectPalette(hdc, ui->Palette(), FALSE);
                numEntries = RealizePalette(hdc);

                ReleaseDC(hwnd, hdc);

                if(numEntries)
                    InvalidateRect(hwnd, NULL, TRUE);

                result = TRUE;
            }

            break;
        }

        case WM_COPYDATA:
        {
            COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;

            ui->FilesReceived((char*)pcds->lpData, pcds->dwData);
            break;
        }

        case WM_INITMENUPOPUP:
            ui->InitMenuPopup(  (HMENU) wParam,
                                (uint32)LOWORD(lParam),
                                (HIWORD(lParam) != 0));
            break;

        case WM_GETMINMAXINFO:
            ui->MinMaxInfo((MINMAXINFO*)lParam);
            break;

        case WM_SETCURSOR:
            SetCursor(ui->Cursor());
            result = 1;
            break;

        case WM_COMMAND:
            ui->Command(wParam, (View*)lParam);
            break;

        case WM_NOTIFY:
            ui->Notify(wParam, (LPNMHDR)lParam);
            break;

        case WM_DROPFILES:
            ui->DropFiles((HDROP) wParam);
			break;

        case WM_TIMER:
            ui->Timer(wParam);
            break;

        case WM_KILLFOCUS:
        case WM_CANCELMODE:
            ui->CancelMode();
            break;

        case WM_NCHITTEST:
        {
            POINT pt;

            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);

            ScreenToClient(hwnd, &pt);

            result = ui->HitTest(pt.x, pt.y);
            break;
        }

        case WM_MOUSEMOVE:
        {
            if(!ui->MouseCaptured())
            {
                SetTimer(hwnd, TIMER_MOUSE_POSITION, 500, NULL);
                ui->SetMouseCapture(true);
            }

            ui->MouseMove(LOWORD(lParam), HIWORD(lParam), wParam);

            break;
        }

        case WM_NCMOUSEMOVE:
        {
            POINT pt;

            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);

            ScreenToClient(hwnd, &pt);
            
            ui->MouseMove(pt.x, pt.y, 0);

            KillTimer(hwnd, TIMER_MOUSE_POSITION);
            ui->SetMouseCapture(false);
            break;
        }

        case WM_LBUTTONDOWN:
        {
            ui->LeftButtonDown(LOWORD(lParam), HIWORD(lParam), wParam);
            break;
        }

        case WM_LBUTTONUP:
        {
            ui->LeftButtonUp(LOWORD(lParam), HIWORD(lParam), wParam);
            break;
        }

        case WM_LBUTTONDBLCLK:
        {
            ui->LeftButtonDoubleClick(LOWORD(lParam), HIWORD(lParam), wParam);
            break;
        }

        case WM_RBUTTONDOWN:
        {
            ui->RightButtonDown(LOWORD(lParam), HIWORD(lParam), wParam);
            break;
        }

        case WM_NCRBUTTONDOWN:
        {
            POINT pt;

            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);

            ScreenToClient(hwnd, &pt);

            ui->RightButtonDown(pt.x, pt.y, wParam);
            break;
        }

        case WM_RBUTTONUP:
        {
            ui->RightButtonUp(LOWORD(lParam), HIWORD(lParam), wParam);
            break;
        }

        case WM_NCRBUTTONUP:
        {
            POINT pt;

            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);

            ScreenToClient(hwnd, &pt);

            ui->RightButtonUp(pt.x, pt.y, wParam);
            break;
        }

        case WM_KEYDOWN:
        {
            ui->KeyDown(wParam);
            break;
        }

        case WM_EXITSIZEMOVE:
        {
            ui->ExitSizeMove();
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

FreeAmpUI::
FreeAmpUI(FAContext *context):
UserInterface()
{
    m_context       = context;
    m_hwnd          = NULL;
    m_palette       = NULL;
    m_windowRegion  = NULL;
    m_playerRegion  = NULL;
    m_viewList      = NULL;
    m_trayIcon      = NULL;

    *m_trayTooltip = 0x00;

    m_mouseCaptured = false;

    m_log = false;
    m_onTop = false;
    m_liveInTray = false;

    m_playerCanvas  = NULL;
    m_bodyBitmap    = NULL;
    m_playBitmap    = NULL;
    m_stopBitmap    = NULL;
    m_pauseBitmap   = NULL;
    m_nextBitmap    = NULL;
    m_lastBitmap    = NULL;

    m_modeBitmap        = NULL;
    m_minimizeBitmap    = NULL;
    m_closeBitmap       = NULL;
    m_repeatBitmap      = NULL;
    m_shuffleBitmap     = NULL;
    m_openBitmap        = NULL;
    m_volumeDialBitmap  = NULL;
    m_seekDialBitmap    = NULL;

    m_shuffleIconBitmap     = NULL;
    m_repeatIconBitmap      = NULL;
    m_repeatAllIconBitmap   = NULL;

    m_smallFontBitmap       = NULL;
    m_largeFontBitmap       = NULL;

    m_timeBackgroundBitmap  = NULL;

    m_drawerBitmap      = NULL;
    m_drawerMaskBitmap  = NULL;

    m_panelBackingBitmap      = NULL;
    m_panelBackingMaskBitmap  = NULL;

    m_scrollbarBitmap   = NULL;

    m_addBitmap     = NULL;
    m_deleteBitmap  = NULL;
    m_saveBitmap    = NULL;
    m_loadBitmap    = NULL;

    m_captureView   = NULL;
    m_mouseView     = NULL;

    m_backgroundView = NULL;

    //m_resizeView    = NULL;

    m_playView      = NULL;
    m_stopView      = NULL;
    m_pauseView     = NULL;
    m_nextView      = NULL;
    m_lastView      = NULL;

    m_modeView      = NULL;
    m_minimizeView  = NULL;
    m_closeView     = NULL;
    m_repeatView    = NULL;
    m_shuffleView   = NULL;
    m_openView      = NULL;
    m_volumeView    = NULL;
    m_seekView      = NULL;

    m_shuffleIconView   = NULL;
    m_repeatIconView    = NULL;
    m_repeatAllIconView = NULL;

    m_songTitleView     = NULL;
    m_timeView          = NULL;
    m_volumeInfoView    = NULL;

    m_drawerView        = NULL;
    m_panelBackingView  = NULL;

    m_playlistView      = NULL;

    m_scrollbarView     = NULL;

    m_addView     = NULL;
    m_deleteView  = NULL;
    m_saveView    = NULL;
    m_loadView    = NULL;

	m_width			= 0;
	m_height		= 0;

	m_secondsPerFrame	= 0;
	m_totalFrames		= 0;
    m_currentFrame      = 0;
    m_seekFrame         = 0;

    m_prevSongInfoText = NULL;

    m_state = UIState_Stopped;

    for(int32 i = 0; i < kNumControls; i++)
    {
        m_controlRegions[i] = NULL;
    }
    
    m_cursor = LoadCursor(NULL, IDC_ARROW);

    m_uiSemaphore = new Semaphore();

    m_prefs = m_context->prefs;

    m_uiThread = Thread::CreateThread();
    m_uiThread->Create(ui_thread_function, this);

    m_uiSemaphore->Wait();
    delete m_uiSemaphore;
}

FreeAmpUI::
~FreeAmpUI()
{
    // Be good citizens and clean up after ourselves
    RemoveTrayIcon();

    DeleteBitmaps();
    DeleteRegions();

    if(m_viewList)
    {
        m_viewList->DeleteAll();
        delete m_viewList;
    }

    if(m_prevSongInfoText)
        delete [] m_prevSongInfoText;

    if(m_uiThread)
        delete m_uiThread;

    if(m_trayIcon)
        DeleteObject(m_trayIcon);
}

void 
FreeAmpUI::
Create()
{
    assert( m_hwnd );

    // We want people to be able to drop files on the player
    DragAcceptFiles(m_hwnd, TRUE);

    // Where are things located? What shape are they?
    InitializeRegions();

    // Center the window and make sure it is the right size
    assert( m_windowRegion );

    RECT windowRect;

    GetRgnBox(m_windowRegion, &windowRect);
    
    int32 left, top;

    m_prefs->GetWindowPosition( &left, 
                                &top,
                                &m_width, 
                                &m_height);

    if(left == 0 && top == 0 && m_width == 0 && m_height == 0)
    {
        m_width = windowRect.right - windowRect.left;
        m_height = windowRect.bottom - windowRect.top;
        left = (GetSystemMetrics (SM_CXFULLSCREEN) - m_width) / 2;
	    top = (GetSystemMetrics (SM_CYFULLSCREEN) - m_height) / 2;
    }
    		
	SetWindowPos (	m_hwnd,
					0, 
					left, 
					top, 
					m_width, 
					m_height, 
					SWP_NOZORDER);

    // copy window region so windows doesn't take our region
    HRGN windowRegion = CreateRectRgn(0,0,0,0);
    
    CombineRgn( windowRegion,
                windowRegion,
                m_windowRegion,
                RGN_OR);


    // Change window region
    SetWindowRgn(m_hwnd, windowRegion, TRUE);

    // What do we look like?
    LoadBitmaps();

    // Create Controls
    CreateControls();

    CreateTooltips();

    m_trayIcon = LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_EXE_ICON));

    ReadPreferences();

    // platform specific initialization
    OSVERSIONINFO osid;

	osid.dwOSVersionInfoSize = sizeof (osid);
	GetVersionEx (&osid);

	switch (osid.dwPlatformId) 
    {
        // Window 3.x
	    case VER_PLATFORM_WIN32s:
		    break;

	    // Windows 95
	    case VER_PLATFORM_WIN32_WINDOWS:
 		    break;

	    // Windows NT
	    case VER_PLATFORM_WIN32_NT:
        {
		    if(osid.dwMajorVersion >= 4)
            {
                // this will allow us port access thru
                // inp and outp so the rio stuff will
                // work.

                const char* driver = "portio";
                char path[MAX_PATH];
                uint32 size = sizeof(path);

                m_prefs->GetPrefString(kInstallDirPref, path, &size);

                strcat(path, "\\");
                strcat(path, driver);
                strcat(path, ".sys");

                LoadDriver(driver, path);
            }

            break;
        }
    }
}

void 
FreeAmpUI::
Destroy()
{
    // platform specific uninitialization
    OSVERSIONINFO osid;

	osid.dwOSVersionInfoSize = sizeof (osid);
	GetVersionEx (&osid);

	switch (osid.dwPlatformId) 
    {
        // Window 3.x
	    case VER_PLATFORM_WIN32s:
		    break;

	    // Windows 95
	    case VER_PLATFORM_WIN32_WINDOWS:
 		    break;

	    // Windows NT
	    case VER_PLATFORM_WIN32_NT:
        {
		    if(osid.dwMajorVersion >= 4)
            {
                UnloadDriver("portio");
            }

            break;
        }
    }

    // Tell windows msg loop we wanna die
    PostQuitMessage(0);
}

void
FreeAmpUI::
ExitSizeMove()
{
    // save window position
    RECT windowRect;

    GetWindowRect(m_hwnd, &windowRect);
    
    m_prefs->SetWindowPosition( windowRect.left, 
                                windowRect.top,
                                windowRect.right - windowRect.left, 
                                windowRect.bottom - windowRect.top);
}

void 
FreeAmpUI::
MinMaxInfo(MINMAXINFO* info)
{
    /*char buf[256];

    wsprintf(buf,   "max width = %d\r\n"
                    "max height = %d\r\n"
                    "min width = %d\r\n"
                    "min height = %d\r\n", 
                    info->ptMaxTrackSize.x, 
                    info->ptMaxTrackSize.y, 
                    info->ptMinTrackSize.x, 
                    info->ptMinTrackSize.y);

    OutputDebugString(buf);*/
}

int32 
FreeAmpUI::
HitTest(int32 xPos, 
        int32 yPos)
{
    int32 result = 0;

    if(PtInRegion(m_playerRegion, xPos, yPos))
    {
        result = HTCAPTION;
    }
    else if(PtInRegion(m_controlRegions[kResizeControl], xPos, yPos))
    {
        result = HTSIZE;
    }
    else
    {
        result = HTCLIENT;
    }

    return result;
}

void
FreeAmpUI::
KeyDown(int32 keyCode)
{
    switch(keyCode) 
    { 
        case VK_PRIOR:
        case VK_NEXT:
        case VK_UP: 
        case VK_DOWN: 
        case VK_HOME: 
        case VK_END: 
        case VK_DELETE: 
        case 'A':
            if(m_playlistView->Visible() && m_playlistView->Enabled())
                m_playlistView->KeyPressed(keyCode);
            break;

        case 'O':
        {
            short state = GetKeyState(VK_CONTROL);

            if(state>>4)
            {
                SendMessage(m_hwnd, WM_COMMAND, kOpenControl, 0);
            }

            break;
        }

        case 'P':
        {
            short state = GetKeyState(VK_CONTROL);

            if(state>>4)
            {
                SendMessage(m_hwnd, WM_COMMAND, kPlayControl, 0);
            }

            break;
        }

        case 'U':
        {
            short state = GetKeyState(VK_CONTROL);

            if(state>>4)
            {
                SendMessage(m_hwnd, WM_COMMAND, kPauseControl, 0);
            }

            break;
        }

        case 'S':
        {
            short state = GetKeyState(VK_CONTROL);

            if(state>>4)
            {
                SendMessage(m_hwnd, WM_COMMAND, kStopControl, 0);
            }

            break;
        }

        case 'N':
        {
            short state = GetKeyState(VK_CONTROL);

            if(state>>4)
            {
                SendMessage(m_hwnd, WM_COMMAND, kNextControl, 0);
            }

            break;
        }

        case 'L':
        {
            short state = GetKeyState(VK_CONTROL);

            if(state>>4)
            {
                SendMessage(m_hwnd, WM_COMMAND, kLastControl, 0);
            }

            break;
        }
    }

}

void 
FreeAmpUI::
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

static void ResolveLink(char* buf, int32 length)
{
    assert(buf);

    if(buf)
    {
        HRESULT hres = NULL;

        hres = CoInitialize(NULL);

        if(SUCCEEDED(hres))
        {
            IShellLink* psl = NULL;

            // Get a pointer to the IShellLink interface
            hres = CoCreateInstance(CLSID_ShellLink, 
                                    NULL, 
                                    CLSCTX_INPROC_SERVER, 
                                    IID_IShellLink, 
                                    (void**)&psl);

            if(SUCCEEDED(hres))
            {
                IPersistFile* ppf;

                // Get a pointer to the IPersistFile interface
                hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);

                if(SUCCEEDED(hres))
                {
                    WORD wsz[MAX_PATH];

                    // Ensure string is UNICODE
                    MultiByteToWideChar(CP_ACP, 
                                        0, 
                                        buf, 
                                        -1, 
                                        wsz, 
                                        MAX_PATH);

                    // Load Shortcut
                    hres = ppf->Load(wsz, STGM_READ);

                    if(SUCCEEDED(hres))
                    {
                        // Resolve the link
                        hres = psl->Resolve(NULL, SLR_ANY_MATCH|SLR_NO_UI);
                
                        if(SUCCEEDED(hres))
                        {
                            WIN32_FIND_DATA wfd;

                            // Resolve the link
                            hres = psl->GetPath(buf,length,&wfd, 0);
                        }
                    }

                    // Release the IPersist Interface
                    ppf->Release();
                }
        
                // Release the IShellLink Interface
                psl->Release();
            }

            CoUninitialize(); 
        }
    }
}

void 
FreeAmpUI::
DropFiles(HDROP dropHandle)
{
	int32 count;
    int32 originalCount = m_plm->CountItems();
	char file[MAX_PATH + 1];

	count = DragQueryFile(	dropHandle,
							-1L,
							file,
							sizeof(file));

	//m_target->AcceptEvent(new Event(CMD_Stop));

	for(int32 i = 0; i < count; i++)
	{
		DragQueryFile(	dropHandle,
						i,
						file,
						sizeof(file));

        char* pExtension = NULL;

        pExtension = strrchr(file, '.');

        if(pExtension && strcasecmp(pExtension, ".lnk") == 0)
        { 
            ResolveLink(file, sizeof(file));
        }

        List<char*> fileList;
        struct stat st;

        stat(file, &st);

        if(st.st_mode & _S_IFDIR)
        {
            HANDLE findFileHandle = NULL;
            WIN32_FIND_DATA findData;
            char findPath[MAX_PATH + 1];

            strcpy(findPath, file);
            strcat(findPath, "\\*.mp?");

            findFileHandle = FindFirstFile(findPath, &findData);

            if(findFileHandle != INVALID_HANDLE_VALUE)
            {
                do
                {
                    strcpy(findPath, file);
                    strcat(findPath, "\\");
                    strcat(findPath, findData.cFileName);
                    m_plm->AddItem(findPath,0);

                }while(FindNextFile(findFileHandle, &findData));

                FindClose(findFileHandle);
            }
        }
        else
        {
            pExtension = strrchr(file, '.');

            if(pExtension && strcasecmp(pExtension, ".m3u") == 0)
            { 
                Error error;

                error = m_plm->ExpandM3U(file, fileList);

                if(IsError(error))
                {
                    MessageBox(NULL, "Cannot open playlist file", file, MB_OK); 
                }
                else
                {
                    char* item = NULL;
                    int32 i = 0;

                    while(item = fileList.ItemAt(i++))
                    {
                        m_plm->AddItem(item,0);
                    }
                }
            }
            else
            {
                /*OutputDebugString(file);
                OutputDebugString("\r\n");*/
                m_plm->AddItem(file,0);
            }
        }
	}

	if(!originalCount) 
    {
		m_target->AcceptEvent(new Event(CMD_Play));
	}
}

void 
FreeAmpUI::
Command(int32 command, 
        View* source)
{
    switch(command)
    {
        case kPlayControl:
        {            
            m_target->AcceptEvent(new Event(CMD_Play));
            break;
        }

        case kStopControl:
        {            
            m_target->AcceptEvent(new Event(CMD_Stop));
            break;
        }

        case kPauseControl:
        {
            m_target->AcceptEvent(new Event(CMD_TogglePause));
            break;
        }

        case kLastControl:
        {
			m_target->AcceptEvent(new Event(CMD_PrevMediaPiece));
            break;
        }

        case kNextControl:
        {
			m_target->AcceptEvent(new Event(CMD_NextMediaPiece));
            break;
        }

        case kModeControl:
        {
            break;
        }

        case kMinimizeControl:
        {
            bool liveInTray;

            m_prefs->GetPrefBoolean(kLiveInTrayPref, &liveInTray);

            if(liveInTray)
                ShowWindow(m_hwnd, SW_HIDE);
            else
                ShowWindow(m_hwnd, SW_MINIMIZE);

            break;
        }

        case kCloseControl:
        {
            DestroyWindow( m_hwnd );
            break;
        }

        case kRepeatControl:
        {
            m_plm->ToggleRepeat();
            break;
        }

        case kShuffleControl:
        {
            m_plm->ToggleShuffle();
            break;
        }

        case kAddControl:
        {
            List<char*> fileList;
            

            if(FileOpenDialog(  m_hwnd, 
                                kOpenAudioFileTitle,
                                kAudioFileFilter,
                                &fileList,
                                m_prefs))
            {
                AddFileListToPlayList(&fileList);
            }

            break;
        }

        case kDeleteControl:
        {
            m_playlistView->DeleteSelection();
            break;
        }

        case kSaveControl:
        {
            char file[MAX_PATH + 1];
            
            uint32 size = sizeof(file);

            if(FileSaveDialog(  m_hwnd,
                                kSavePlaylistFileTitle,
                                kPlaylistFileFilter,
                                file, 
                                &size,
                                m_prefs))
            {
                if(strcmp(file, kSaveToRio))
                {
                    m_plm->ExportToM3U(file);
                }
                else
                {
                    m_plm->ExportToRio();
                }
            }
           
            break;
        }

        case kLoadControl:
        {
            List<char*> fileList;
           
            if(FileOpenDialog(  m_hwnd,
                                kOpenPlaylistFileTitle,
                                kPlaylistFileFilter,
                                &fileList,
                                m_prefs))
            {
                m_plm->MakeEmpty();

                AddFileListToPlayList(&fileList);

                m_plm->SetFirst();
            }
            break;
        }

        case kOpenControl:
		{
            List<char*> fileList;

			if(FileOpenDialog(  m_hwnd, 
                                kOpenAudioFileTitle,
                                kAudioFileFilter,
                                &fileList,
                                m_prefs))
			{
                m_plm->MakeEmpty();

                AddFileListToPlayList(&fileList);

                m_plm->SetFirst();

                if(m_state == UIState_Playing)
                {
                    m_target->AcceptEvent(new Event(CMD_Stop));
                    m_target->AcceptEvent(new Event(CMD_Play));
                }
                else
                {
					;
                }
			}

			break;        
		}

        case kPlaylistControl:
        {
            HRGN windowRegion = CreateRectRgn(0,0,0,0);
            RECT windowRect;

            GetWindowRgn(m_hwnd, windowRegion);

            if(EqualRgn(windowRegion, m_windowRegion))
            {
                // move region into place
                OffsetRgn(m_playListRegion, 0, 123);

                // copy window region so windows doesn't take our region    
                CombineRgn( windowRegion,
                            m_playListRegion,
                            m_windowRegion,
                            RGN_OR);

                GetRgnBox(windowRegion, &windowRect);
    
                m_width = windowRect.right - windowRect.left;
                m_height = windowRect.bottom - windowRect.top;
				            
	            SetWindowPos (	m_hwnd,
					            0, 
					            0, 
					            0, 
					            m_width, 
					            m_height, 
					            SWP_NOZORDER|SWP_NOMOVE);

                // Change window region
                SetWindowRgn(m_hwnd, windowRegion, TRUE);

                m_drawerView->OffsetBy(0, 123);
                m_playlistBackView->Show();
                m_panelBackingView->Show();
                m_playlistView->Show();
                m_scrollbarView->Show();
                m_addView->Show();
                m_deleteView->Show();
                m_loadView->Show();
                m_saveView->Show();
            }
            else
            {
                // move region into place
                OffsetRgn(m_playListRegion, 0, -123);

                // copy window region so windows doesn't take our region    
                CombineRgn( windowRegion,
                            m_windowRegion,
                            m_windowRegion,
                            RGN_OR);

                GetRgnBox(windowRegion, &windowRect);
    
                m_width = windowRect.right - windowRect.left;
                m_height = windowRect.bottom - windowRect.top;
				            
	            SetWindowPos (	m_hwnd,
					            0, 
					            0, 
					            0, 
					            m_width, 
					            m_height, 
					            SWP_NOZORDER|SWP_NOMOVE);

                // Change window region
                SetWindowRgn(m_hwnd, windowRegion, TRUE);

                m_drawerView->OffsetBy(0, -123);
                m_playlistBackView->Hide();
                m_panelBackingView->Hide();
                m_playlistView->Hide();
                m_scrollbarView->Hide();
                m_addView->Hide();
                m_deleteView->Hide();
                m_loadView->Hide();
                m_saveView->Hide();
            }

            DIB* canvas = new DIB();
            canvas->Create(m_width, m_height, m_playerCanvas->BitsPerPixel());

            canvas->SetPalette( m_bodyBitmap->Palette(), 
                                m_bodyBitmap->NumberOfPaletteEntries() );

            delete m_playerCanvas;

            m_playerCanvas = canvas;

            InvalidateRect(m_hwnd, &windowRect, FALSE);
            
            break;
        }

        case kSongInfoControl:
        {
            TimeView* view = (TimeView*)source;

            view->ToggleDisplay();
            break;
        }

        case kPrefControl:
        {
            if(DisplayPreferences(m_hwnd, m_prefs))
                ReadPreferences();
            break;
        }
    }
}

void 
FreeAmpUI::
Notify(int32 command, LPNMHDR notifyMsgHdr)
{
    if(notifyMsgHdr->code == TTN_NEEDTEXT)
    {
        int32 idCtrl = notifyMsgHdr->idFrom;
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) notifyMsgHdr; 

        POINT pt;

        GetCursorPos(&pt);

        ScreenToClient(m_hwnd, &pt);

        // tooltips only use rects... we have non-rectangular controls
        if(PtInRegion(m_controlRegions[idCtrl], pt.x, pt.y))
        {
            switch (idCtrl) 
            { 
                case kRepeatControl:
                    lpttt->lpszText = "Repeat"; 
                    break;
                case kShuffleControl:
                    lpttt->lpszText = "Shuffle"; 
                    break;
                case kOpenControl:
                    lpttt->lpszText = "Open..."; 
                    break;
                case kModeControl:
                    lpttt->lpszText = "Change Player Mode (disabled)"; 
                    break;
                case kMinimizeControl:
                    lpttt->lpszText = "Minimize"; 
                    break;
                case kCloseControl:
                    lpttt->lpszText = "Close"; 
                    break;
                case kPlayControl:
                    if(m_state == UIState_Stopped)
                        lpttt->lpszText = "Play"; 
                    else
                        lpttt->lpszText = "Stop";
                    break;
                case kPauseControl:
                    if(m_state == UIState_Paused)
                        lpttt->lpszText = "Continue Playing"; 
                    else
                        lpttt->lpszText = "Pause"; 
                    break;
                case kNextControl:
                    lpttt->lpszText = "Next Song"; 
                    break;
                case kLastControl:
                    lpttt->lpszText = "Previous Song"; 
                    break;
                case kPlaylistControl:
                    lpttt->lpszText = "Open Playlist"; 
                    break;
                case kSongInfoControl:
                    lpttt->lpszText = "Change Display Mode"; 
                    break;
                case kVolumeControl:
                    lpttt->lpszText = "Volume"; 
                    break;
                case kSeekControl:
                    lpttt->lpszText = "Seek"; 
                    break;
            }
        }
    }
    else
    {
        switch( command )
        {
            case kVolumeControl:
            {
                switch(notifyMsgHdr->code)
                {
                    case DIAL_BUTTON_DOWN:
                    {
                        m_target->AcceptEvent(new Event(CMD_GetVolume));
                        SetTimer(m_hwnd, TIMER_VOLUME_POSITION, 200, NULL);

                        m_timeView->Hide();
                        m_volumeInfoView->Show();
                       
                        break;
                    }

                    case DIAL_BUTTON_UP:
                    {
                        KillTimer(m_hwnd, TIMER_VOLUME_POSITION);
                        m_timeView->Show();
                        m_volumeInfoView->Hide();
                        m_volumeView->SetPosition(0);
                        break;
                    }

                    case DIAL_MOVE:
                    {
                        /*char buffer[256];
                        wsprintf(buffer,"position = %d\r\n", m_volumeView->Position());
                        OutputDebugString(buffer);*/

                        m_volumeAcceleration = m_volumeView->Position() / 5;

                        break;
                    }
                }

                break;
            }

            case kSeekControl:
            {
                switch(notifyMsgHdr->code)
                {
                    case DIAL_BUTTON_DOWN:
                    {
                        int32 hours = m_timeView->CurrentHours();
                        int32 minutes = m_timeView->CurrentMinutes();
                        int32 seconds = m_timeView->CurrentSeconds();

                        m_lastTimeDisplay = m_timeView->Display();

                        m_timeView->SetDisplay(DisplaySeekTime);
                        m_timeView->SetSeekTime(hours, minutes, seconds);

                        // oops broken with new buffering scheme
                        // let's try to calculate this...
                        // NOTE: won't work for VBR files...grrrr
                        //m_seekFrame = m_currentFrame;

                        int32 currentTimeInSeconds = hours * 3600;
			            currentTimeInSeconds += minutes * 60;
			            currentTimeInSeconds += seconds;

                        m_seekFrame = (int32)((float)currentTimeInSeconds/m_secondsPerFrame);

                        /*char buffer[256];
                        wsprintf(buffer,"m_seekFrame = %d\r\n", m_seekFrame);
                        OutputDebugString(buffer);*/

                        SetTimer(m_hwnd, TIMER_SEEK_POSITION, 100, NULL);
                        break;
                    }

                    case DIAL_BUTTON_UP:
                    {
                        KillTimer(m_hwnd, TIMER_SEEK_POSITION);

                        int32 seconds = (int32)ceil(m_secondsPerFrame * m_seekFrame);
			            int32 hours = seconds / 3600;
			            int32 minutes = seconds / 60 - hours * 60;
			            seconds = seconds - minutes * 60 - hours * 3600;

                        m_timeView->SetCurrentTime(hours, minutes, seconds);

                        m_timeView->SetDisplay(m_lastTimeDisplay);
                        m_seekView->SetPosition(0);

                        if(m_state == UIState_Playing)
                        {
                            m_target->AcceptEvent(new ChangePositionEvent(m_seekFrame));
                        }

                        break;
                    }

                    case DIAL_MOVE:
                    {
                        m_seekAcceleration = m_seekView->Position() / 5;

                        break;
                    }
                }

                break;
            }
        }
    }
}

void
FreeAmpUI::
CancelMode()
{
    if(m_captureView)
    {
        m_captureView = NULL;
        ReleaseCapture();
    }
    
    if(m_mouseView)
    {
        POINT pt;

        GetCursorPos(&pt);

        ScreenToClient(m_hwnd, &pt);

        if(!m_mouseView->PointInView(pt.x, pt.y))
        {
            m_mouseView->MouseLeft();
            m_mouseView = NULL;
        }
    }

}

void
FreeAmpUI::
Timer(int32 timerID)
{
    switch(timerID)
    {
        case TIMER_SHOW_STATUS_INFO:
        {
            if(m_prevSongInfoText)
                m_songTitleView->SetText(m_prevSongInfoText);

            KillTimer(m_hwnd, TIMER_SHOW_STATUS_INFO);
            break;
        }

        case TIMER_MOUSE_POSITION:
        {
            bool pointInWindow;
            POINT pt;

            GetCursorPos(&pt);

            ScreenToClient(m_hwnd, &pt);

            pointInWindow = MouseMove(pt.x, pt.y, 0);

            if(!pointInWindow)
            {
                KillTimer(m_hwnd, TIMER_MOUSE_POSITION);
                SetMouseCapture(false);
            }

            break;
        }

        case TIMER_SEEK_POSITION:
        {
            m_seekFrame += m_seekAcceleration*20*abs(m_seekAcceleration);

            if(m_seekFrame < 0)
                m_seekFrame = 0;
            else if(m_seekFrame > m_totalFrames)
                m_seekFrame = m_totalFrames;

            //sprintf(foo,"position: %d\r\n",position);
			//OutputDebugString(foo);

            int32 seconds = (int32)ceil(m_secondsPerFrame * m_seekFrame);
			int32 hours = seconds / 3600;
			int32 minutes = seconds / 60 - hours * 60;
			seconds = seconds - minutes * 60 - hours * 3600;

            //sprintf(foo,"seconds: %d\r\n",seconds);

            m_timeView->SetSeekTime(hours, minutes, seconds);
            break;
        }

        case TIMER_VOLUME_POSITION:
        {
            int32 volume = m_volumeInfoView->Volume();

            volume += m_volumeAcceleration;

            if(volume > 100)
                volume = 100;
            else if(volume < 0)
                volume = 0;

            m_volumeInfoView->SetPrefInt32(kVolumePref, volume);

            m_target->AcceptEvent(new VolumeEvent(CMD_SetVolume, volume));

            /*MMRESULT result;
            //float percent = (float)volume/(float)100;

			result = waveOutSetPrefInt32(kVolumePref,  (HWAVEOUT)WAVE_MAPPER, 
                                        MAKELPARAM( 0xFFFF*volume/100, 
                                                    0xFFFF*volume/100));*/

            break;
        }
    }
}

bool
FreeAmpUI::
MouseMove(int32 xPos, 
          int32 yPos, 
          int32 modifiers)
{
    bool result = false;

    Item<View*>* viewItem = m_viewList->LastItem();

    POINT pt;
    RECT rect;

    pt.x = xPos;
    pt.y = yPos;

    /*char buffer[256];
    wsprintf(buffer,"x = %d   y = %d \r\n", xPos, yPos);
    OutputDebugString(buffer);*/

    GetClientRect(m_hwnd, &rect);

    if(m_captureView)
    {
        m_captureView->MouseMove(xPos, yPos, modifiers);
    }
    else if(!PtInRect(&rect, pt))
    {
        if(m_mouseView)
            m_mouseView->MouseLeft();

        m_mouseView = NULL;
    }
    else
    {
        do
        {
            if( viewItem->Member()->PointInView(xPos, yPos) && 
                viewItem->Member()->Visible() &&
                viewItem->Member()->Enabled())

			{
				if(m_mouseView != viewItem->Member())
				{
					if(m_mouseView)
						m_mouseView->MouseLeft();

					m_mouseView = viewItem->Member();

					m_mouseView->MouseEntered();
				}

				viewItem->Member()->MouseMove(xPos, yPos, modifiers);

				break;
			}

        }while(viewItem = m_viewList->PriorItem(viewItem) );

        result = true;
    }

    return result;
}

bool
FreeAmpUI::
LeftButtonDown( int32 xPos, 
                int32 yPos, 
                int32 modifiers)
{
    bool result = false;

    Item<View*>* viewItem = m_viewList->LastItem();

    do
    {
        if( viewItem->Member()->PointInView(xPos, yPos) && 
            viewItem->Member()->Visible() &&
            viewItem->Member()->Enabled())
        {
            viewItem->Member()->LeftButtonDown(xPos, yPos, modifiers);

            m_captureView = viewItem->Member();

            SetCapture(m_hwnd);

            result = true;

            break;
        }

    }while(viewItem = m_viewList->PriorItem(viewItem) );

    return result;
}

bool
FreeAmpUI::
LeftButtonUp(   int32 xPos, 
                int32 yPos, 
                int32 modifiers)
{
    bool result = false;

    Item<View*>* viewItem = m_viewList->LastItem();

    if(m_captureView)
    {
        m_captureView->LeftButtonUp(xPos, yPos, modifiers);

        m_captureView = NULL;

        ReleaseCapture();

    }
    else
    {
        do
        {
            if( viewItem->Member()->PointInView(xPos, yPos) && 
                viewItem->Member()->Visible() &&
                viewItem->Member()->Enabled())
            {
                viewItem->Member()->LeftButtonUp(xPos, yPos, modifiers);

                result = true;

                break;
            }

        }while(viewItem = m_viewList->PriorItem(viewItem) );
    }


    return result;
}

bool
FreeAmpUI::
LeftButtonDoubleClick(  int32 xPos, 
                        int32 yPos, 
                        int32 modifiers)
{
    bool result = false;

    //OutputDebugString("LeftButtonDoubleClick\r\n");

    Item<View*>* viewItem = m_viewList->LastItem();
    
    do
    {
        if( viewItem->Member()->PointInView(xPos, yPos) && 
            viewItem->Member()->Visible() &&
            viewItem->Member()->Enabled())
        {
            viewItem->Member()->LeftButtonDoubleClick(xPos, yPos, modifiers);

            result = true;

            break;
        }

    }while(viewItem = m_viewList->PriorItem(viewItem) );

    return result;
}

bool
FreeAmpUI::
RightButtonUp(  int32 xPos, 
                int32 yPos, 
                int32 modifiers)
{
    bool result = false;

    if(PtInRegion(m_playerRegion, xPos, yPos))
    {
        HMENU menuHandle, popupHandle;
        HINSTANCE hinst = (HINSTANCE)GetWindowLong(m_hwnd, GWL_HINSTANCE);
        POINT pt;
        int32 command;

        // need coordinates relative to screen
        pt.x = xPos;
        pt.y = yPos;

        ClientToScreen(m_hwnd, &pt);

        // make sure we are in the foreground
        SetForegroundWindow(m_hwnd);

        // load the menu and retrieve its popup
        menuHandle = LoadMenu(hinst, MAKEINTRESOURCE(IDM_MAIN));
        popupHandle = GetSubMenu(menuHandle, 0);

        // display the popup
        command = TrackPopupMenu(   popupHandle,			
					                TPM_RETURNCMD | TPM_RIGHTBUTTON,
					                pt.x, 
                                    pt.y,       
					                0,  
					                m_hwnd,
					                NULL);

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

            case IDC_EXIT:
                SendMessage(m_hwnd, WM_COMMAND, kCloseControl, 0);
                break;
        }

    }

    return result;
}

bool
FreeAmpUI::
RightButtonDown(int32 xPos, 
                int32 yPos, 
                int32 modifiers)
{
    bool result = false;

    
    return result;
}

void 
FreeAmpUI::
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

int32 
FreeAmpUI::
Paint()
{
    int32 result = 0;
    PAINTSTRUCT ps;
    HDC hdc;

    hdc = BeginPaint( m_hwnd, &ps ); 

    Item<View*>* viewItem = m_viewList->FirstItem();

    do
    {
        if(viewItem->Member()->Visible())
        {
            viewItem->Member()->Draw(m_playerCanvas, &ps.rcPaint);
        }

    }while(viewItem = m_viewList->NextItem(viewItem) );

    /*int32 x = ps.rcPaint.left;
    int32 y = ps.rcPaint.top;
    int32 width = ps.rcPaint.right - ps.rcPaint.left;
    int32 height = ps.rcPaint.bottom - ps.rcPaint.top;*/

    int32 x = 0;
    int32 y = 0;
    int32 width = m_width;
    int32 height = m_height;

    if(m_palette)
    {
        SelectPalette(hdc, m_palette, FALSE);
        RealizePalette(hdc);
    }

	StretchDIBits(	hdc,
					x,y,width,height,
					x,y,width,height,
					m_playerCanvas->Bits(),
					m_playerCanvas->BitmapInfo(),
					DIB_RGB_COLORS,
					SRCCOPY);

	/*HBRUSH brush = CreateSolidBrush(RGB(0,0,0));
	FillRgn(hdc, m_windowRegion, brush);

	for(int32 i = 0; i < kNumControls; i++)
	{
		HBRUSH brush = CreateSolidBrush(g_controlColors[i].Pack());
		int32 ret = FillRgn(hdc, m_controlRegions[i], brush);
	}*/

    EndPaint( m_hwnd, &ps );    
    
    return result;
}

void 
FreeAmpUI::
CreateControls()
{
    m_viewList = new LinkedList<View*>();

    m_backgroundView = new BitmapView(  m_hwnd, 
                                        NULL, 
                                        m_windowRegion, 
                                        m_bodyBitmap);

    m_playlistBackView = new BitmapView(m_hwnd, 
                                        NULL, 
                                        m_playlistBodyRegion, 
                                        m_playlistBodyBitmap);

    m_playlistBackView->Hide();
/*
    m_resizeView = new ResizeView(  m_hwnd,
                                    m_backgroundView,
                                    m_controlRegions[kResizeControl],
                                    kPlayControl);
*/

    m_playView = new ButtonView(m_hwnd, 
                                m_backgroundView, 
                                m_controlRegions[kPlayControl], 
                                m_playBitmap,
                                kPlayControl);

    m_stopView = new ButtonView(m_hwnd, 
                                m_backgroundView, 
                                m_controlRegions[kPlayControl], 
                                m_stopBitmap,
                                kStopControl);
    m_stopView->Hide();

    m_pauseView = new ButtonView(   m_hwnd, 
                                    m_backgroundView, 
                                    m_controlRegions[kPauseControl], 
                                    m_pauseBitmap,
                                    kPauseControl,
                                    ButtonType_ToggleButton);

    m_nextView = new ButtonView(m_hwnd, 
                                m_backgroundView, 
                                m_controlRegions[kNextControl], 
                                m_nextBitmap,
                                kNextControl);

    m_lastView = new ButtonView(m_hwnd, 
                                m_backgroundView, 
                                m_controlRegions[kLastControl], 
                                m_lastBitmap,
                                kLastControl);

    m_modeView = new ButtonView(m_hwnd, 
                                m_backgroundView, 
                                m_controlRegions[kModeControl], 
                                m_modeBitmap,
                                kModeControl);

    m_minimizeView = new ButtonView(m_hwnd, 
                                    m_backgroundView, 
                                    m_controlRegions[kMinimizeControl], 
                                    m_minimizeBitmap,
                                    kMinimizeControl);

    m_closeView = new ButtonView(   m_hwnd, 
                                    m_backgroundView, 
                                    m_controlRegions[kCloseControl], 
                                    m_closeBitmap,
                                    kCloseControl);

    m_repeatView = new ButtonView(  m_hwnd, 
                                    m_backgroundView, 
                                    m_controlRegions[kRepeatControl], 
                                    m_repeatBitmap,
                                    kRepeatControl);

    m_shuffleView = new ButtonView( m_hwnd, 
                                    m_backgroundView, 
                                    m_controlRegions[kShuffleControl], 
                                    m_shuffleBitmap,
                                    kShuffleControl);

    m_openView = new ButtonView(m_hwnd, 
                                m_backgroundView, 
                                m_controlRegions[kOpenControl], 
                                m_openBitmap,
                                kOpenControl);

    m_volumeView = new DialView(m_hwnd, 
                                m_backgroundView, 
                                m_controlRegions[kVolumeControl], 
                                m_volumeDialBitmap,
                                kVolumeControl);

    m_seekView = new DialView(  m_hwnd, 
                                m_backgroundView, 
                                m_controlRegions[kSeekControl], 
                                m_seekDialBitmap,
                                kSeekControl);

    m_shuffleIconView = new StatusView( m_hwnd, 
                                        m_backgroundView, 
                                        m_controlRegions[kShuffleStatusControl], 
                                        m_shuffleIconBitmap);

    m_repeatIconView = new StatusView(  m_hwnd, 
                                        m_backgroundView, 
                                        m_controlRegions[kRepeatStatusControl], 
                                        m_repeatIconBitmap);

    m_repeatAllIconView = new StatusView(   m_hwnd, 
                                            m_backgroundView, 
                                            m_controlRegions[kRepeatAllStatusControl], 
                                            m_repeatAllIconBitmap);

    m_songTitleView = new TextView( m_hwnd, 
                                    m_backgroundView, 
                                    m_controlRegions[kSongTitleControl], 
                                    m_smallFontBitmap,
                                    10,
                                    smallFontWidth,
                                    kSongTitleControl,
                                    TextType_MouseWiggle);

	m_songTitleView->SetText("Welcome to FreeAmp");
    SetTrayTooltip("Welcome to FreeAmp");

    m_prevSongInfoText = new char [strlen(m_songTitleView->Text()) + 1];
    strcpy(m_prevSongInfoText, m_songTitleView->Text());

    m_timeView = new TimeView(  m_hwnd, 
                                m_backgroundView, 
                                m_controlRegions[kSongInfoControl], 
                                //m_timeBackgroundBitmap,
                                m_largeFontBitmap,
                                12,
                                largeFontWidth,
                                m_smallFontBitmap,
                                10,
                                smallFontWidth,
                                kSongInfoControl,
                                TimeType_TimeOnRight);

    m_timeView->SetLabel("current");

    m_volumeInfoView = new VolumeView(  m_hwnd, 
                                        m_backgroundView, 
                                        m_controlRegions[kSongInfoControl], 
                                        m_timeBackgroundBitmap,
                                        m_largeFontBitmap,
                                        12,
                                        largeFontWidth,
                                        m_smallFontBitmap,
                                        10,
                                        smallFontWidth,
                                        kSongInfoControl);

    m_volumeInfoView->Hide();

    m_panelBackingView = new BitmapView(m_hwnd, 
                                        NULL, 
                                        m_controlRegions[kPanelBackingControl], 
                                        m_panelBackingBitmap, 
                                        m_panelBackingMaskBitmap);

    m_panelBackingView->Hide();

    m_drawerView = new BitmapView(  m_hwnd, 
                                    NULL, 
                                    m_controlRegions[kPlaylistControl], 
                                    m_drawerBitmap, 
                                    m_drawerMaskBitmap, 
                                    kPlaylistControl);

    m_playlistView = new ListView(  m_hwnd,
                                    NULL,
                                    m_controlRegions[kPlaylistDisplayControl],
                                    MULTIPLE_SELECTION_LIST);
    
    m_playlistView->Hide();

    m_scrollbarView = new ScrollView(   m_hwnd,
                                        NULL,
                                        m_controlRegions[kScrollbarControl],
                                        m_scrollbarBitmap,
                                        ScrollType_Vertical);

    m_scrollbarView->Hide();

    m_scrollbarView->SetTarget(m_playlistView);

    m_addView = new ButtonView( m_hwnd, 
                                m_panelBackingView, 
                                m_controlRegions[kAddControl], 
                                m_addBitmap,
                                kAddControl);
    m_addView->Hide();

    m_deleteView = new ButtonView(  m_hwnd, 
                                    m_panelBackingView, 
                                    m_controlRegions[kDeleteControl], 
                                    m_deleteBitmap,
                                    kDeleteControl);
    m_deleteView->Hide();

    m_loadView = new ButtonView(m_hwnd, 
                                m_panelBackingView, 
                                m_controlRegions[kLoadControl], 
                                m_loadBitmap,
                                kLoadControl);
    m_loadView->Hide();

    m_saveView = new ButtonView(m_hwnd, 
                                m_panelBackingView, 
                                m_controlRegions[kSaveControl], 
                                m_saveBitmap,
                                kSaveControl);
    m_saveView->Hide();

                                    

    m_viewList->Append(m_backgroundView);
    m_viewList->Append(m_playlistBackView);
    //m_viewList->Append(m_resizeView);
    m_viewList->Append(m_playView);
    m_viewList->Append(m_stopView);
    m_viewList->Append(m_pauseView);
    m_viewList->Append(m_nextView);
    m_viewList->Append(m_lastView);
    m_viewList->Append(m_modeView);
    m_viewList->Append(m_minimizeView);
    m_viewList->Append(m_closeView);
    m_viewList->Append(m_repeatView);
    m_viewList->Append(m_shuffleView); 
    m_viewList->Append(m_openView);
    m_viewList->Append(m_volumeView);
    m_viewList->Append(m_seekView);
    m_viewList->Append(m_shuffleIconView);
    m_viewList->Append(m_repeatIconView);
    m_viewList->Append(m_repeatAllIconView);
    m_viewList->Append(m_songTitleView);
    m_viewList->Append(m_timeView);
    m_viewList->Append(m_volumeInfoView);
    m_viewList->Append(m_playlistView);
    m_viewList->Append(m_scrollbarView);
    m_viewList->Append(m_panelBackingView);
    m_viewList->Append(m_addView);
    m_viewList->Append(m_deleteView);
    m_viewList->Append(m_loadView);
    m_viewList->Append(m_saveView);
    m_viewList->Append(m_drawerView);
}

void 
FreeAmpUI::
LoadBitmaps()
{
    HDC hdc, memdc, bufferdc;
    int32 numBits;

    assert(m_hwnd);

    hdc = GetDC(m_hwnd);
    memdc = CreateCompatibleDC(hdc);
    bufferdc = CreateCompatibleDC(hdc);

    numBits = GetDeviceCaps(hdc, BITSPIXEL) * GetDeviceCaps(hdc, PLANES);
		
    if(numBits <= 8) 
    {
        m_playerCanvas = new DIB;
        m_playerCanvas->Create(m_width, m_height, 8);

        m_bodyBitmap = new DIB;
        m_bodyBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_BODY_256));

        m_playerCanvas->SetPalette( m_bodyBitmap->Palette(), 
                                    m_bodyBitmap->NumberOfPaletteEntries() );

        m_playlistBodyBitmap = new DIB;
        m_playlistBodyBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_PLAYLIST_BODY_256));

        m_playBitmap = new DIB;
        m_playBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_PLAY_256));

        m_stopBitmap = new DIB;
        m_stopBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_STOP_256));

        m_pauseBitmap = new DIB;
        m_pauseBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_PAUSE_256));

        m_nextBitmap = new DIB;
        m_nextBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_NEXT_256));

        m_lastBitmap = new DIB;
        m_lastBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_LAST_256));

        m_modeBitmap = new DIB;
        m_modeBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_MODE_256));

        m_minimizeBitmap = new DIB;
        m_minimizeBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_MINIMIZE_256));

        m_closeBitmap = new DIB;
        m_closeBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_CLOSE_256));

        assert(m_closeBitmap->NumberOfPaletteEntries() == m_bodyBitmap->NumberOfPaletteEntries());
        assert(!memcmp(m_closeBitmap->Palette(), m_bodyBitmap->Palette(), m_closeBitmap->NumberOfPaletteEntries()));

        m_repeatBitmap = new DIB;
        m_repeatBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_REPEAT_256));

        m_shuffleBitmap = new DIB;
        m_shuffleBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_SHUFFLE_256));

        m_openBitmap = new DIB;
        m_openBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_OPEN_256));

        m_volumeDialBitmap = new DIB;
        m_volumeDialBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_DIALS_256));

        m_seekDialBitmap = new DIB;
        m_seekDialBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_DIALS_256));

        m_shuffleIconBitmap = new DIB;
        m_shuffleIconBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_SHUFFLED_ICON_256));

        m_repeatIconBitmap = new DIB;
        m_repeatIconBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_REPEAT_ICON_256));

        m_repeatAllIconBitmap = new DIB;
        m_repeatAllIconBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_REPEAT_ALL_ICON_256));

        m_smallFontBitmap = new DIB;
        m_smallFontBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_SMALL_FONT_256));

        m_largeFontBitmap = new DIB;
        m_largeFontBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_BIG_FONT_256));

        m_timeBackgroundBitmap = new DIB;
        m_timeBackgroundBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_TIME_BACK_256)); 

        m_drawerBitmap = new DIB;
        m_drawerBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_DRAWER_256)); 

        m_panelBackingBitmap = new DIB;
        m_panelBackingBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_PANEL_BACKING_256));

        m_scrollbarBitmap = new DIB;
        m_scrollbarBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_SCROLLBAR_256));

        m_addBitmap = new DIB;
        m_addBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_ADD_256));

        m_deleteBitmap = new DIB;
        m_deleteBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_DELETE_256));

        m_loadBitmap = new DIB;
        m_loadBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_LOAD_256));

        m_saveBitmap = new DIB;
        m_saveBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_SAVE_256));

        CreatePalette();
	}
    else
    {
        m_playerCanvas = new DIB;
        m_playerCanvas->Create(m_width, m_height, 24);

        m_bodyBitmap = new DIB;
        m_bodyBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_BODY));

		m_playlistBodyBitmap = new DIB;
        m_playlistBodyBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_PLAYLIST_BODY));

        m_playBitmap = new DIB;
        m_playBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_PLAY));

        m_stopBitmap = new DIB;
        m_stopBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_STOP));

        m_pauseBitmap = new DIB;
        m_pauseBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_PAUSE));

        m_nextBitmap = new DIB;
        m_nextBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_NEXT));

        m_lastBitmap = new DIB;
        m_lastBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_LAST));

        m_modeBitmap = new DIB;
        m_modeBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_MODE));

        m_minimizeBitmap = new DIB;
        m_minimizeBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_MINIMIZE));

        m_closeBitmap = new DIB;
        m_closeBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_CLOSE));

        m_repeatBitmap = new DIB;
        m_repeatBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_REPEAT));

        m_shuffleBitmap = new DIB;
        m_shuffleBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_SHUFFLE));

        m_openBitmap = new DIB;
        m_openBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_OPEN));

        m_volumeDialBitmap = new DIB;
        m_volumeDialBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_DIALS));

        m_seekDialBitmap = new DIB;
        m_seekDialBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_DIALS));

        m_shuffleIconBitmap = new DIB;
        m_shuffleIconBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_SHUFFLED_ICON));

        m_repeatIconBitmap = new DIB;
        m_repeatIconBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_REPEAT_ICON));

        m_repeatAllIconBitmap = new DIB;
        m_repeatAllIconBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_REPEAT_ALL_ICON));

        m_smallFontBitmap = new DIB;
        m_smallFontBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_SMALL_FONT));

        m_largeFontBitmap = new DIB;
        m_largeFontBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_BIG_FONT));

        m_timeBackgroundBitmap = new DIB;
        m_timeBackgroundBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_TIME_BACK)); 

        m_drawerBitmap = new DIB;
        m_drawerBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_DRAWER)); 

        m_panelBackingBitmap = new DIB;
        m_panelBackingBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_PANEL_BACKING));

        m_scrollbarBitmap = new DIB;
        m_scrollbarBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_SCROLLBAR));

        m_addBitmap = new DIB;
        m_addBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_ADD));

        m_deleteBitmap = new DIB;
        m_deleteBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_DELETE));

        m_loadBitmap = new DIB;
        m_loadBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_LOAD));

        m_saveBitmap = new DIB;
        m_saveBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_SAVE));
    }

    m_drawerMaskBitmap = new DIB;
    m_drawerMaskBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_DRAWER_MASK));

    m_panelBackingMaskBitmap = new DIB;
    m_panelBackingMaskBitmap->Load(g_hinst, MAKEINTRESOURCE(IDB_PANEL_BACKING_MASK));

    /*DIB test;

    test.Load(g_hinst, MAKEINTRESOURCE(IDB_CONTROL_MASK_MID));

    Color color;

    test.Pixel(0,0,&color);
    test.Pixel(136,0,&color);
    test.Pixel(34,26,&color);
    test.Pixel(336,41,&color);*/


    DeleteDC(bufferdc);
    DeleteDC(memdc);

    ReleaseDC(m_hwnd, hdc);
}

void 
FreeAmpUI::
CreatePalette()
{
    LOGPALETTE* logicalPalette;
    RGBQUAD* rgbPalette;
    int32 numEntries = m_playerCanvas->NumberOfPaletteEntries();

    logicalPalette = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)*numEntries);

    logicalPalette->palVersion = 0x300;
    logicalPalette->palNumEntries = numEntries;

    rgbPalette = m_playerCanvas->Palette();

    for(int32 i = 0; i < numEntries; i++, rgbPalette++)
    {
        logicalPalette->palPalEntry[i].peRed = rgbPalette->rgbRed;
        logicalPalette->palPalEntry[i].peGreen = rgbPalette->rgbGreen;
        logicalPalette->palPalEntry[i].peBlue = rgbPalette->rgbBlue;
        logicalPalette->palPalEntry[i].peFlags = 0;
    }

    m_palette = ::CreatePalette(logicalPalette);

    free(logicalPalette);
}

void
FreeAmpUI::
CreateTooltips()
{
    // tooltip support
    HWND hwndTooltip = NULL;
    HINSTANCE hinst = (HINSTANCE)GetWindowLong( m_hwnd, 
										        GWL_HINSTANCE);

    hwndTooltip = CreateWindowEx(WS_EX_TOPMOST,
                                TOOLTIPS_CLASS, 
                                NULL, 
                                0, 
                                CW_USEDEFAULT, 
                                CW_USEDEFAULT, 
                                CW_USEDEFAULT, 
                                CW_USEDEFAULT, 
                                m_hwnd, 
                                (HMENU) NULL, 
                                hinst, 
                                NULL);

    RECT toolRect;
    TOOLINFO ti;

    for(int32 i = 0; i < kNumControls; i++)
    {
        GetRgnBox(m_controlRegions[i], &toolRect);

        ti.cbSize = sizeof(TOOLINFO); 
        ti.uFlags =  TTF_SUBCLASS; 
        ti.hwnd = m_hwnd; 
        ti.hinst = hinst; 
        ti.uId = (UINT) i; 
        ti.lpszText = (LPSTR) LPSTR_TEXTCALLBACK; 
        ti.rect.left = toolRect.left; 
        ti.rect.top = toolRect.top; 
        ti.rect.right = toolRect.right; 
        ti.rect.bottom = toolRect.bottom; 

        if(i != kStopControl)
        {
            SendMessage(hwndTooltip, 
                    TTM_ADDTOOL, 
                    0, 
                    (LPARAM) &ti);
        }
    }
}

void 
FreeAmpUI::
InitializeRegions()
{ 
    // first determine what the shape of the window is going to be
    DIB windowMask;
    windowMask.Load(g_hinst, MAKEINTRESOURCE(IDB_WINDOW_MASK_MID));
    Color color = {0,0,0,0};

    m_windowRegion = DetermineRegion(&windowMask, &color);

    // next determine what the shape and where the controls are
    DIB controlMask;
    
    controlMask.Load(g_hinst, MAKEINTRESOURCE(IDB_CONTROL_MASK_MID));

    DetermineControlRegions(&controlMask,
                            m_controlRegions,
			                g_controlColors,
                            kNumControls);

    controlMask.Load(g_hinst, MAKEINTRESOURCE(IDB_PLAYLIST_CONTROL_MASK));

    DetermineControlRegions(&controlMask,
                            &m_controlRegions[kPlaylistDisplayControl],
			                &g_controlColors[kPlaylistDisplayControl],
                            kNumControls - kPlaylistDisplayControl);

    // then determine what is the player body and what is a control
    m_playerRegion = CreateRectRgn(0,0,0,0);

    CombineRgn( m_playerRegion,
                m_playerRegion,
                m_windowRegion,
                RGN_OR);

    for(int32 i = 0; i < kNumControls; i++)
    {
        CombineRgn( m_playerRegion,
                    m_playerRegion,
                    m_controlRegions[i],
                    RGN_DIFF);
    }

    RECT rect;
    HRGN rectRgn;

    GetRgnBox(m_controlRegions[kPlaylistControl], &rect);

    m_playListRegion = CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);

    rect.left++;
    rect.right--;
    rect.bottom = rect.top;
    rect.top = rect.bottom - 15;

    rectRgn = CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);

    CombineRgn( m_playListRegion,
                m_playListRegion,
                rectRgn,
                RGN_OR);

    DeleteObject(rectRgn);

    rect.left++;
    rect.right--;
    rect.bottom = rect.top;
    rect.top = rect.bottom - 92;

    rectRgn = CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);

    CombineRgn( m_playListRegion,
                m_playListRegion,
                rectRgn,
                RGN_OR);

    OffsetRgn(rectRgn, 0, 133);

    m_playlistBodyRegion = rectRgn;
}

void 
FreeAmpUI::
DeleteBitmaps()
{
    if(m_playerCanvas)
    {
        delete m_playerCanvas;
        m_playerCanvas = NULL;
    }

    // shared bitmaps
    if(m_smallFontBitmap)
    {
        delete m_smallFontBitmap;
        m_smallFontBitmap = NULL;
    }

    if(m_largeFontBitmap)
    {
        delete m_largeFontBitmap;
        m_largeFontBitmap = NULL;
    }

    if(m_timeBackgroundBitmap)
    {
        delete m_timeBackgroundBitmap;
        m_timeBackgroundBitmap = NULL;
    }
}


void 
FreeAmpUI::
DeleteRegions()
{
    DeleteObject(m_playerRegion);
    DeleteObject(m_windowRegion);
    DeleteObject(m_playListRegion);
    DeleteObject(m_playlistBodyRegion);

    for(int32 i = 0; i < kNumControls; i++)
    {
        DeleteObject(m_controlRegions[i]);
    }
}

Error 
FreeAmpUI::
AcceptEvent(Event* event)
{
    Error result = kError_UnknownErr;

    if (event) 
    {
        result = kError_NoErr;

        switch (event->Type()) 
        {
		    case INFO_PlayListRepeat:
			{
                PlayListRepeatEvent* plre = (PlayListRepeatEvent*)event;

				switch(plre->GetRepeatMode()) 
                {
				    case REPEAT_CURRENT:
                        m_repeatIconView->On( true );
                        m_repeatAllIconView->On( false );
					    break;
				    case REPEAT_ALL:
					    m_repeatIconView->On( true );
                        m_repeatAllIconView->On( true );
					    break;
				    case REPEAT_NOT:
				    default:
					    m_repeatIconView->On( false );
                        m_repeatAllIconView->On( false ); 
					    break;
				}
				break;
			}

		    case INFO_PlayListShuffle:
			{
                PlayListShuffleEvent* plse = (PlayListShuffleEvent*)event;
                
				switch(plse->GetShuffleMode()) 
                {
				    case SHUFFLE_NOT_SHUFFLED:
                        m_shuffleIconView->On(false);
					    break;
				    case SHUFFLE_RANDOM:
					    m_shuffleIconView->On(true);
					    break;
				    default:
					    m_shuffleIconView->On(false);
                        break;
				}
				break;
			}

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


			case INFO_MPEGInfo: 
			{
				MpegInfoEvent *info = (MpegInfoEvent*)event;

				m_totalFrames = info->GetTotalFrames();

                m_secondsPerFrame = info->GetSecondsPerFrame();

				break;
			}

			case INFO_ID3TagInfo:
			{
                ID3TagEvent *info = (ID3TagEvent*)event;

				if(info->GetId3Tag().m_containsInfo) 
                {
					char foo[1024];
					strncpy(foo,info->GetId3Tag().m_artist,sizeof(foo)-1);

                    if(*info->GetId3Tag().m_album && *info->GetId3Tag().m_album != ' ')
                    {
					    strncat(foo," - ",sizeof(foo)-strlen(foo));

					    strncat(foo,info->GetId3Tag().m_album,sizeof(foo)-strlen(foo));
                    }

                    strncat(foo," - ",sizeof(foo)-strlen(foo));

					strncat(foo,info->GetId3Tag().m_songName,sizeof(foo)-strlen(foo));

                    m_songTitleView->SetText(foo);
                    SetTrayTooltip(foo);

                    if(m_prevSongInfoText)
                        delete m_prevSongInfoText;

                    m_prevSongInfoText = new char [strlen(m_songTitleView->Text()) + 1];
                    strcpy(m_prevSongInfoText, m_songTitleView->Text());

                    char title[256] = "FreeAmp - ";

                    strncat(title, m_songTitleView->Text(), sizeof(title) - strlen(title));

                    SetWindowText(m_hwnd, title);
				}

				break;
			}

            case INFO_MediaInfo: 
            {
				MediaInfoEvent *info = (MediaInfoEvent*)event;

				int32 seconds = (int32)ceil(info->m_totalSeconds);
				int32 hours = seconds / 3600;
				int32 minutes = (seconds - (hours * 3600)) / 60;

				seconds = seconds - (hours * 3600) - (minutes * 60);

				char *foo = strrchr(info->m_filename,'\\');

                char title[256] = "FreeAmp - ";

				if(foo) 
				{
					m_songTitleView->SetText(++foo);
				} 
				else 
				{
					m_songTitleView->SetText(info->m_filename);
				}

                SetTrayTooltip(m_songTitleView->Text());

                if(m_prevSongInfoText)
                        delete m_prevSongInfoText;

                m_prevSongInfoText = new char [strlen(m_songTitleView->Text()) + 1];
                strcpy(m_prevSongInfoText, m_songTitleView->Text());

                strncat(title, m_songTitleView->Text(), sizeof(title) - strlen(title));

                SetWindowText(m_hwnd, title);
                //OutputDebugString(m_songTitleView->Text());
                //OutputDebugString("\r\n");

                m_timeView->SetTotalTime(hours, minutes, seconds);
			    m_timeView->SetCurrentTime(0, 0, 0);

				//m_indexOfSong = info->m_indexOfSong;
				//m_totalSongs = info->m_totalSongs;

				break; 
            }

            case INFO_StreamInfo:
            { 
                StreamInfoEvent *info = (StreamInfoEvent*)event;

                char title[256] = "FreeAmp - ";

                m_songTitleView->SetText(info->m_streamTitle);

                SetTrayTooltip(m_songTitleView->Text());

                if(m_prevSongInfoText)
                        delete m_prevSongInfoText;

                m_prevSongInfoText = new char [strlen(m_songTitleView->Text()) + 1];
                strcpy(m_prevSongInfoText, m_songTitleView->Text());

                strncat(title, m_songTitleView->Text(), sizeof(title) - strlen(title));

                SetWindowText(m_hwnd, title);
                
                break;
            }


            case INFO_MediaTimeInfo: 
            {
                MediaTimeInfoEvent* info = (MediaTimeInfoEvent*)event;
 
				/*
                int32 seconds = (int32)ceil(info->m_totalSeconds);
				int32 hours = seconds / 3600;
				int32 minutes = (seconds - (hours * 3600)) / 60;
				seconds = seconds - (hours * 3600) - (minutes * 60);
                */

                int32 hours = info->m_hours;
				int32 minutes = info->m_minutes;
                int32 seconds = info->m_seconds;
		
                m_timeView->SetCurrentTime(hours, minutes, seconds);
				
				m_currentFrame = info->m_frame;

	            break; 
            }

            case INFO_DoneOutputting: 
            {
                m_timeView->SetCurrentTime(0, 0, 0);
	            break; 
            }

            case INFO_PlayListUpdated:
            {
                UpdatePlayList();
                break;
            }

            case INFO_PlayListItemUpdated:
            {
                PlayListItemUpdatedEvent* updateEvent = (PlayListItemUpdatedEvent*)event;

                PlayListItem* updateItem = updateEvent->UpdatedItem();

                if(updateItem)
                {
                    int32 i = 0;

                    StringItem* listItem;

                    while(listItem = (StringItem*)m_playlistView->ItemAt(i++))
                    {
                        if(updateItem == (PlayListItem*) listItem->UserValue())
                        {
                            listItem->SetText(updateItem->StringForPlayerToDisplay());
                            m_playlistView->Invalidate();

                            break;
                        }
                    }
                }
                break;
            }
            
            case INFO_PlayListDonePlay:
            {
                m_timeView->SetCurrentTime(0,0,0);
                m_currentFrame = 0;
                SetWindowText(m_hwnd, "FreeAmp");
                break;
            }

            case INFO_StatusMessage:
            {
                StatusMessageEvent* statusEvent = (StatusMessageEvent*)event;
                KillTimer(m_hwnd, TIMER_SHOW_STATUS_INFO);
                SetTimer(m_hwnd, TIMER_SHOW_STATUS_INFO, 5000, NULL);
                m_songTitleView->SetText(statusEvent->GetStatusMessage());
                break;
            }

            case INFO_VolumeInfo:
            {
                VolumeEvent* volumeEvent = (VolumeEvent*)event;
                m_volumeInfoView->SetPrefInt32(kVolumePref, volumeEvent->GetPrefInt32(kVolumePref, ));
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
FreeAmpUI::
SetArgs(int32 argc, char** argv)
{
    char *arg = NULL;
    bool shuffle = false;
    bool autoplay = false;
    int32 count = 0;

    for(int32 i = 1;i < argc; i++) 
    {
	    arg = argv[i];

	    if (arg[0] == '-') 
        {
	        switch (arg[1]) 
            {
		        case 's':
                {
                    shuffle = true;
		            break;
	            } 

                case 'p':
                {
                    autoplay = true;
		            break;
	            } 
            }
        }
        else 
        {
            HANDLE handle;
            WIN32_FIND_DATA data;

            handle = FindFirstFile( arg, &data);

            // find long filename for item
            if(handle != INVALID_HANDLE_VALUE)
            {
                char path[MAX_PATH];
                char* cp = NULL;

                strcpy(path, arg);
                cp = strrchr(path, '\\');

                if(cp)
                {
                    strcpy(cp + 1, data.cFileName);
                }

                m_plm->AddItem(path,0);

                FindClose(handle);
            }
            else
            {
                // probably a URL... should do more checking though
                m_plm->AddItem(arg,0);
            }
      

            count++;
	    }
    }

    m_plm->SetFirst();

    if(shuffle) 
        m_plm->SetShuffle(SHUFFLE_RANDOM);
    
    if(autoplay)
       m_target->AcceptEvent(new Event(CMD_Play));
}

void
FreeAmpUI::
SetPlayListManager(PlayListManager *plm)
{
	m_plm = plm;

    UpdatePlayList();
}

void
FreeAmpUI::
FilesReceived(char* array, int32 count)
{
    List<char*> fileList;
    int32 length = 0;

    for(int32 i = 0; i < count; i++)
    {
        char* foo = new char[MAX_PATH + 1];

        strcpy(foo, array);

        HANDLE handle;
        WIN32_FIND_DATA data;

        handle = FindFirstFile( foo, &data);

        if(handle != INVALID_HANDLE_VALUE)
        {
            char* cp = NULL;

            cp = strrchr(foo, '\\');

            if(cp)
            {
                strcpy(cp + 1, data.cFileName);
            }

            FindClose(handle);
        }

        fileList.AddItem(foo);

	    array += strlen(array) + 1;
    }
            
    if( m_state == UIState_Playing || m_state == UIState_Paused)
        m_target->AcceptEvent(new Event(CMD_Stop));

    m_plm->MakeEmpty();

    AddFileListToPlayList(&fileList);

    m_plm->SetFirst();

    m_target->AcceptEvent(new Event(CMD_Play));
}

void
FreeAmpUI::
AddFileListToPlayList(List<char*>* fileList)
{
    List<char*> m3uFileList;
    char* file = NULL;
    int32 i = 0;

	while(file = fileList->ItemAt(i++))
	{
        char* pExtension = NULL;

        pExtension = strrchr(file, '.');

        if(pExtension && strcasecmp(pExtension, ".m3u") == 0)
        { 
            Error error;

            error = m_plm->ExpandM3U(file, m3uFileList);

            if(IsError(error))
            {
                MessageBox(NULL, "Cannot open playlist file", file, MB_OK); 
            }
            else
            {
                char* item = NULL;
                int32 i = 0;

                while(item = m3uFileList.ItemAt(i++))
                {
                    m_plm->AddItem(item,0);
                    delete [] item;
                }
            }
        }
        else
        {
            m_plm->AddItem(file,0);
        }

        delete [] file;
	}
}

void
FreeAmpUI::
UpdatePlayList() 
{
    assert(m_plm);
    assert(m_playlistView);

    if(m_plm && m_playlistView)
    {
        int32 playlistCount = m_plm->CountItems();
        int32 listviewCount = m_playlistView->CountItems();

        /*char buffer[256];
        sprintf(buffer, "playlist count %d\r\nlistview count %d\r\n", playlistCount, listviewCount);
        OutputDebugString(buffer);*/

        if(playlistCount != listviewCount)      
        {
            int32 i = 0;
            PlayListItem* playlistItem;
            List<ListItem*> listitemList;

            m_playlistView->MakeEmpty();
            
            while(playlistItem = m_plm->ItemAt(i++))
            {
                MediaInfoEvent* info = playlistItem->GetMediaInfo();

                //char buffer[256];
                //sprintf(buffer, "This is StringItem #%d", i);

                StringItem* item = new StringItem(  playlistItem->StringForPlayerToDisplay(),
                                                    m_smallFontBitmap,
                                                    10,
                                                    smallFontWidth);
                item->SetUserValue(playlistItem);

                ListItem* i = item;

                listitemList.AddItem(i);

                /*OutputDebugString(playlistItem->StringForPlayerToDisplay());
                OutputDebugString("\r\n");*/
            }

            m_playlistView->AddList(&listitemList);
        }       
        else
        {
            bool different = false;
            int32 i = 0;

            PlayListItem* playlistItem;

            while(playlistItem = m_plm->ItemAt(i))
            {
                if( m_playlistView->ItemAt(i) &&
                    playlistItem != (PlayListItem*) m_playlistView->ItemAt(i++)->UserValue())
                {
                    different = true;
                    break;
                }
            }

            if(different)
            {
                PlayListItem* playlistItem;

                m_playlistView->MakeEmpty();
            
                i = 0;

                while(playlistItem = m_plm->ItemAt(i++))
                {
                    //MediaInfoEvent* info = playlistItem->GetMediaInfo();

                    //char buffer[256];
                    //sprintf(buffer, "This is StringItem #%d", i);

                    StringItem* item = new StringItem(  playlistItem->StringForPlayerToDisplay(),
                                                        m_smallFontBitmap,
                                                        10,
                                                        smallFontWidth);
                    item->SetUserValue(playlistItem);

                    m_playlistView->AddItem(item);
                    /*OutputDebugString(playlistItem->StringForPlayerToDisplay());
                    OutputDebugString("\r\n");*/
                }
            }

        }
    }
}

void 
FreeAmpUI::
ReadPreferences()
{
    m_prefs->GetPrefBoolean(kUseDebugLogPref, &m_log);

    if(m_log)
        m_prefs->GetPrefBoolean(kLogMainPref, &m_log);

    m_prefs->GetPrefBoolean(kStayOnTopPref, &m_onTop);
    m_prefs->GetPrefBoolean(kLiveInTrayPref, &m_liveInTray);

    SetWindowPos(   m_hwnd, 
                    (m_onTop ? HWND_TOPMOST: HWND_NOTOPMOST), 
                    0, 0, 0, 0, 
                    SWP_NOMOVE|SWP_NOSIZE);

    // what follows is a sad hack that actually makes windows 
    // refresh the toolbar correctly...
    ShowWindow(m_hwnd, FALSE);

    LONG styles, extendedStyles;

    styles = GetWindowLong(m_hwnd, GWL_STYLE);
    extendedStyles = GetWindowLong(m_hwnd, GWL_EXSTYLE);

    if(extendedStyles & WS_EX_TOOLWINDOW)
    {
        if(!m_liveInTray)       
        {
            extendedStyles ^= WS_EX_TOOLWINDOW;
            RemoveTrayIcon();
        }
    }
    else 
    {
        if(m_liveInTray)
        {
            extendedStyles |= WS_EX_TOOLWINDOW;
            AddTrayIcon();
        }

    }

    SetWindowLong(m_hwnd, GWL_STYLE, WS_VISIBLE);
    SetWindowLong(m_hwnd, GWL_STYLE, styles);
    SetWindowLong(m_hwnd, GWL_EXSTYLE, extendedStyles);

    ShowWindow(m_hwnd, TRUE);

}

void 
FreeAmpUI::
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
FreeAmpUI::
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
FreeAmpUI::
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
FreeAmpUI::
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
    wc.lpszClassName = "FreeAmp";

    if( RegisterClass( &wc ) )    
    {
        CreateWindow(   "FreeAmp", 
                        "FreeAmp",
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
FreeAmpUI::
ui_thread_function(void* arg)
{
    FreeAmpUI* ui = (FreeAmpUI*)arg;

    ui->UIThreadFunction();
}
