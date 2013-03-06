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
    
    $Id: downloadui.cpp,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/

/* system headers */
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <math.h>

#include <string>
#include <iostream>
#include <sstream>

using namespace std;

/* project headers */
#include "config.h"
#include "thread.h"
#include "downloadui.h"
#include "event.h"
#include "eventdata.h"
#include "playlist.h"
#include "errors.h"
#include "help.h"
#include "resource.h"
#include "utility.h"
#include "debug.h"

static const int32 kProgressHeight = 8;
static const int32 kProgressWidth = 7;

static const int32 kPrePadding = 4;
static const int32 kElementPadding = 5;
static const int32 kPostPadding = 5;
static const int32 kMinProgressWidth = 3;
static const int32 kTotalPadding = kPrePadding + kElementPadding + kPostPadding;

static const char *szEMusicText = 
   "The Download Manager enables you to download music from the downloadable "
   "page at the EMusic site and other sites that support EMusic /"
   "package (EMP) downloads.";
static const char *szEMusicURLText = "Go to your EMusic Collection page";
static const char *szEMusicURL = "https://secure.emusic.com/perl/secure/downloadables.pl";

static const char *szFreeAmpText = 
   "The Download Manager enables you to download music from sites that "
   "support the EMusic Page (EMP) download format. To try it check "
   "out the free music at:";
   
static const char *szFreeAmpURLText = "http://www.emusic.com/music/free.html";
static const char *szFreeAmpURL = "http://www.emusic.com/music/free.html";

HINSTANCE g_hInstance = NULL;

BOOL CALLBACK MainProc( HWND hwnd, 
                        UINT msg, 
                        WPARAM wParam, 
                        LPARAM lParam ); 

LRESULT WINAPI 
ProgressWndProc(HWND hwnd, 
                UINT msg, 
                WPARAM wParam, 
                LPARAM lParam);

LRESULT WINAPI 
FreeTracksWndProc(HWND hwnd, 
                  UINT msg, 
                  WPARAM wParam, 
                  LPARAM lParam);
LRESULT WINAPI 
ListWndProc(HWND hwnd, 
            UINT msg, 
            WPARAM wParam, 
            LPARAM lParam);

extern "C" DownloadUI *Initialize(FAContext *context)
{
    return new DownloadUI(context);
}

INT WINAPI DllMain (HINSTANCE hInst,
                    ULONG ul_reason_being_called,
                    LPVOID lpReserved)
{
    switch (ul_reason_being_called)
    {
        case DLL_PROCESS_ATTACH:
            g_hInstance = hInst;
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


DownloadUI::DownloadUI(FAContext *context):UserInterface()
{
    m_context = context;
    m_prefs = context->prefs;
    m_plm = m_context->plm;
    m_target = m_context->target;
    m_propManager = m_context->props;
    m_dlm = m_context->downloadManager;
    m_overURL = false;
    m_duringUpdate = false;

    m_totalItems = 0;
    m_doneItems = 0;
    m_totalBytes = 0;
    m_doneBytes = 0;

    m_uiSemaphore = new Semaphore();

    m_uiThread = Thread::CreateThread();
    m_uiThread->Create(UIThreadFunc,this);

    m_progressBitmap = NULL;

    m_uiSemaphore->Wait();
}

DownloadUI::~DownloadUI()
{
    if(m_progressBitmap)
        DeleteObject(m_progressBitmap);

    if(m_handCursor)
        DestroyCursor(m_handCursor);

    delete m_uiSemaphore;
    delete m_uiThread;
}

Error DownloadUI::AcceptEvent(Event* event)
{
    Error result = kError_UnknownErr;

    if(event) 
    {
        switch(event->Type()) 
        {
            case CMD_Cleanup: 
            {
                LV_ITEM item;
                
                if (m_hwndList)
                {
                    uint32 itemCount = ListView_GetItemCount(m_hwndList);
                    for(uint32 i = 0; i < itemCount; i++)
                    {
                        item.mask = LVIF_PARAM;
                        item.iItem = i;
                        item.lParam = 0;

                        if(ListView_GetItem(m_hwndList, &item))
                        {
                            DownloadItem* dli = (DownloadItem*)item.lParam;

                            if(dli->GetState() == kDownloadItemState_Downloading)
                            {
                                m_dlm->CancelDownload(dli, true);  
                            }
                        }
                    }
                }

                PostMessage(m_hwnd, WM_QUIT, 0, 0);
                m_uiThread->Join();
                
                m_target->AcceptEvent(new Event(INFO_ReadyToDieUI));
                break; 
            }

            case CMD_ToggleDownloadUI:
            {
                BOOL visible = IsWindowVisible(m_hwnd);
                ShowWindow(m_hwnd, (visible ? SW_HIDE: SW_SHOW));

                if(!visible)
                    SetForegroundWindow(m_hwnd);
                
                break;
            }
            
            case INFO_DownloadItemAdded: 
            {
                DownloadItemAddedEvent* dliae = (DownloadItemAddedEvent*)event;
                
                LV_ITEM item;
               
                item.mask = LVIF_PARAM | LVIF_STATE;
                item.state = 0;
                item.stateMask = 0;
                item.iItem = ListView_GetItemCount(m_hwndList);
                item.iSubItem = 0;
                item.lParam = (LPARAM)dliae->Item();

                ListView_InsertItem(m_hwndList, &item);

                UpdateOverallProgress();

                // bring window into view
                ShowWindow(m_hwnd, SW_SHOW);
                SetForegroundWindow(m_hwnd);

                if (m_dlm->IsPaused())
                   m_dlm->ResumeDownloads();
               
                break; 
            }

            case INFO_DownloadItemRemoved: 
            {
                DownloadItemRemovedEvent* dlire = (DownloadItemRemovedEvent*)event;

                uint32 itemCount = ListView_GetItemCount(m_hwndList);

                if(itemCount)
                {
                    LV_ITEM item;

                    for(uint32 i = 0; i < itemCount; i++)
                    {
                        item.mask = LVIF_PARAM;
                        item.iItem = i;
                        item.lParam = 0;

                        if(ListView_GetItem(m_hwndList, &item))
                        {
                            if(dlire->Item() == (DownloadItem*)item.lParam)
                            {
                                ListView_RedrawItems(m_hwndList, i, i);
                                UpdateWindow(m_hwndList);
                                break;
                            }
                        }
                    }
                }

                UpdateOverallProgress();

                break; 
            }

            case INFO_DownloadItemNewState: 
            {
                DownloadItemNewStateEvent* dlinse = (DownloadItemNewStateEvent*)event;

                uint32 itemCount = ListView_GetItemCount(m_hwndList);

                if(itemCount)
                {
                    LV_ITEM item;

                    for(uint32 i = 0; i < itemCount; i++)
                    {
                        item.mask = LVIF_PARAM;
                        item.iItem = i;
                        item.lParam = 0;

                        if(ListView_GetItem(m_hwndList, &item))
                        {
                            if(dlinse->Item() == (DownloadItem*)item.lParam)
                            {
                                if (dlinse->Item()->GetState() == 
                                    kDownloadItemState_Downloading)
                                {    
                                    SetButtonStates(dlinse->Item());
                                    ListView_SetItemState(m_hwndList, i, 
                                        LVIS_SELECTED | LVIS_FOCUSED, 
                                        LVIS_SELECTED | LVIS_FOCUSED);
                                    ListView_EnsureVisible(m_hwndList, i, false);
                                }    
                            
                                ListView_RedrawItems(m_hwndList, i, i);
                                UpdateWindow(m_hwndList);
                                break;
                            }
                        }
                    }
                }

                UpdateOverallProgress();

                break; 
            }

            case INFO_DownloadItemProgress: 
            {
                DownloadItemProgressEvent* dlipe = (DownloadItemProgressEvent*)event;

                uint32 itemCount = ListView_GetItemCount(m_hwndList);

                if(itemCount)
                {
                    LV_ITEM item;

                    for(uint32 i = 0; i < itemCount; i++)
                    {
                        item.mask = LVIF_PARAM;
                        item.iItem = i;
                        item.lParam = 0;

                        if(ListView_GetItem(m_hwndList, &item))
                        {
                            if(dlipe->Item() == (DownloadItem*)item.lParam)
                            {
                                m_duringUpdate = true;
                                ListView_RedrawItems(m_hwndList, i, i);
                                UpdateWindow(m_hwndList);
                                m_duringUpdate = false;
                                break;
                            }
                        }
                    }
                }

                UpdateOverallProgress();

                break; 
            }
            
            default:
                break;
        }

        result = kError_NoErr;

    } 

    return result;
}

void DownloadUI::ParseArgs(int32 argc, char** argv)
{
    /*
    char *arg = NULL;
    int32 count = 0;

    for(int32 i = 1;i < argc; i++) 
    {
        arg = argv[i];

        if(arg[0] == '-') 
        {
            switch(arg[1]) 
            {
                
            }
        }
       
    }
    */
}

void DownloadUI::CreateUI()
{
    InitCommonControls();

    HWND hwnd;
    MSG msg;

    hwnd = CreateDialogParam( g_hInstance, 
                    MAKEINTRESOURCE(IDD_DIALOG),
                    NULL,
                    MainProc, 
                    (LPARAM)this);

    while(GetMessage(&msg,NULL,0,0))
    {
        if(hwnd)
        {
            IsDialogMessage(hwnd, &msg);
        }
    }
    ImageList_Destroy(m_noteImage);
    DestroyWindow(hwnd);
    m_hwnd = NULL;
    m_hwndList = NULL;

    m_target->AcceptEvent(new Event(CMD_QuitPlayer));
}

void DownloadUI::UIThreadFunc(void* arg)
{
    DownloadUI* ui = (DownloadUI*)arg;

    ui->CreateUI();
}

Error DownloadUI::Init(int32 startup_type) 
{ 
    ParseArgs(m_context->argc, m_context->argv);

    if(startup_type == PRIMARY_UI)
    {
        ShowWindow(m_hwnd, SW_SHOWNORMAL);
    }

    return kError_NoErr;
}


BOOL DownloadUI::InitDialog()
{
    char title[100];
    
    GetWindowText(m_hwnd, title, 100);
    strcat(title, BRANDING);
    SetWindowText(m_hwnd, title);
    
    // get hwnds for all my controls
    m_hwndList = GetDlgItem(m_hwnd, IDC_LIST);
    m_hwndInfo = GetDlgItem(m_hwnd, IDC_INFO);
    m_hwndPause = GetDlgItem(m_hwnd, IDC_PAUSE);
    m_hwndCancel = GetDlgItem(m_hwnd, IDC_CANCEL);
    m_hwndResume = GetDlgItem(m_hwnd, IDC_RESUME);
    m_hwndClose = GetDlgItem(m_hwnd, IDC_CLOSE);
    m_hwndProgress = GetDlgItem(m_hwnd, IDC_OVERALL_PROGRESS);
    m_hwndText = GetDlgItem(m_hwnd, IDC_FREETRACKS);
    m_hwndCheck = GetDlgItem(m_hwnd, IDC_CHECK1);
    
    // Set the proc address as a property 
    // of the window so it can get it
    SetProp(m_hwndText, 
            "oldproc",
            (HANDLE)GetWindowLong(m_hwndText, GWL_WNDPROC));

    SetProp(m_hwndText, 
            "ui",
            (HANDLE)this);

    // Subclass the window so we can draw it
    SetWindowLong(  m_hwndText, 
                    GWL_WNDPROC, 
                    (DWORD)::FreeTracksWndProc ); 


    // Set the proc address as a property 
    // of the window so it can get it
    SetProp(m_hwndList, 
            "oldproc",
            (HANDLE)GetWindowLong(m_hwndList, GWL_WNDPROC));

    SetProp(m_hwndList, 
            "ui",
            (HANDLE)this);

    // Subclass the window so we can draw it
    SetWindowLong(m_hwndList, 
                    GWL_WNDPROC, 
                  (DWORD)::ListWndProc ); 

    // Set the proc address as a property 
    // of the window so it can get it
    SetProp(m_hwndProgress, 
            "oldproc",
            (HANDLE)GetWindowLong(m_hwndProgress, GWL_WNDPROC));

    SetProp(m_hwndProgress, 
            "ui",
            (HANDLE)this);

    // Subclass the window so we can draw it
    SetWindowLong(m_hwndProgress, 
                    GWL_WNDPROC, 
                  (DWORD)::ProgressWndProc ); 

    HINSTANCE hinst = (HINSTANCE)GetWindowLong(m_hwnd, GWL_HINSTANCE);
    HICON appIcon = LoadIcon(hinst, MAKEINTRESOURCE(IDI_EXE_ICON));

    SetClassLong(m_hwnd, GCL_HICON, (LONG)appIcon);

    // initialize controls
    // first let's add our columns
    RECT rect;
    GetClientRect(m_hwndList, &rect);

    LV_COLUMN lvc;

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT; // left align column
    lvc.cx = (rect.right-rect.left)/2; // width of column in pixels
    lvc.pszText = "Song Title";
    lvc.iSubItem = 0;

    ListView_InsertColumn(m_hwndList, 0, &lvc);

    if((rect.right-rect.left)%2) // squeeze out every last pixel :)
        lvc.cx += 1;

    lvc.pszText = "Status";
    lvc.iSubItem = 1;

    ListView_InsertColumn(m_hwndList, 1, &lvc);

    lvc.pszText = "";
    lvc.cx = (rect.right-rect.left); // width of column in pixels
    lvc.iSubItem = 0;

    ListView_InsertColumn(m_hwndInfo, 0, &lvc);

    // init the images    
    m_noteImage = ImageList_Create(16, 16, ILC_MASK, 1, 0);
    HBITMAP bmp = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_NOTE));
    ImageList_AddMasked(m_noteImage, bmp, RGB(255,0,0));
    DeleteObject(bmp);

    m_progressBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_PROGRESS));

    // Set progress bitmap for overall progress view
    SetProp(m_hwndProgress, 
            "bitmap",
            (HANDLE)m_progressBitmap);

    ListView_SetImageList(m_hwndList, m_noteImage, LVSIL_SMALL);

    // Add items to info view
    LV_ITEM item;
    uint32 i;

    item.mask = LVIF_PARAM | LVIF_STATE;
    item.state = 0;
    item.stateMask = 0;
    item.iSubItem = 0;
    item.lParam = NULL;

    for(i = 0; i < 7; i++)
    {
        item.iItem = i;
        ListView_InsertItem(m_hwndInfo, &item);
    }

    // Add Items that are currently in the download manager
    /*DownloadItem* dli = NULL;
    i = 0;

    while(dli = m_dlm->ItemAt(i++))
    {
        item.mask = LVIF_PARAM | LVIF_STATE;
        item.state = 0;
        item.stateMask = 0;
        item.iItem = ListView_GetItemCount(m_hwndList);
        item.iSubItem = 0;
        item.lParam = (LPARAM)dli;

        ListView_InsertItem(m_hwndList, &item);

        if(dli->GetState() == kDownloadItemState_Queued ||
           dli->GetState() == kDownloadItemState_Downloading)
        {
            // bring window into view
            ShowWindow(m_hwnd, SW_SHOW);
            SetForegroundWindow(m_hwnd);
        }
    }

    UpdateOverallProgress();*/

	bool checkSet = false;
	m_prefs->GetPrefBoolean(kCloseDLMOnCompletePref, &checkSet);
	Button_SetCheck(m_hwndCheck, checkSet);
	
    m_handCursor = LoadCursor(g_hInstance, MAKEINTRESOURCE(IDC_HAND));

    if (strcasecmp(BRANDING_COMPANY, "EMusic") == 0)
    {
       SetWindowText(GetDlgItem(m_hwnd, IDC_DLMTEXT), szEMusicText);
       SetWindowText(GetDlgItem(m_hwnd, IDC_FREETRACKS), szEMusicURLText);
    }
    else
    {
       SetWindowText(GetDlgItem(m_hwnd, IDC_DLMTEXT), szFreeAmpText);
       SetWindowText(GetDlgItem(m_hwnd, IDC_FREETRACKS), szFreeAmpURLText);
    }

    m_uiSemaphore->Signal();
    return TRUE;
}

BOOL DownloadUI::DrawItem(int32 controlId, DRAWITEMSTRUCT* dis)
{
    BOOL result = TRUE;

    switch(controlId)
    {
        case IDC_FREETRACKS:
        {
            HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            HPEN pen = CreatePen(PS_SOLID, 1, RGB(0,0,238));
            HFONT oldFont;
            HPEN oldPen;
            char url[256];

            GetWindowText(dis->hwndItem, url, sizeof(url));

            oldFont = (HFONT)SelectObject(dis->hDC, font);
            oldPen = (HPEN)SelectObject(dis->hDC, pen);
               
            RECT clientRect;

            GetClientRect(dis->hwndItem, &clientRect);

            // Set the text background and foreground colors to the
            // standard window colors
            SetTextColor(dis->hDC, RGB(0,0,238));
            SetBkColor(dis->hDC, GetSysColor(COLOR_3DFACE));
             
            RECT rcClip = clientRect;
            int height = 0;

            height = DrawText(   
                        dis->hDC, 
                        url,
                        strlen(url),
                        &rcClip,
                        DT_LEFT|DT_WORDBREAK|DT_SINGLELINE|DT_CALCRECT);

            height = DrawText(   
                        dis->hDC, 
                        url,
                        strlen(url),
                        &rcClip,
                        DT_LEFT|DT_WORDBREAK|DT_SINGLELINE);

            rcClip.bottom = rcClip.top + height + 1;

            MoveToEx(dis->hDC, rcClip.left, rcClip.bottom, NULL);
            LineTo(dis->hDC, rcClip.right, rcClip.bottom);

            SelectObject(dis->hDC, oldFont);
            SelectObject(dis->hDC, oldPen);

            m_urlRect = rcClip;

            DeleteObject(font);
            DeleteObject(pen);
            break;
        }
        case IDC_OVERALL_PROGRESS:
        {
            HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            HFONT oldFont;
           
            if(m_progressBitmap)
            {
                string displayString;
                ostringstream ost;
                float total = m_totalBytes;
                float recvd = m_doneBytes;
                uint32 percent = 0;
                RECT clientRect;
                HDC hDc;
                HBITMAP hBitmap, hSavedBitmap;

                GetClientRect(m_hwndProgress, &clientRect);

                hDc = CreateCompatibleDC(dis->hDC);
                hBitmap = CreateCompatibleBitmap(dis->hDC, 
                           clientRect.right - clientRect.left,
                           clientRect.bottom - clientRect.top);
                hSavedBitmap = (HBITMAP)SelectObject(hDc, hBitmap);           
                oldFont = (HFONT)SelectObject(hDc, font);
            
                // Set the text background and foreground colors to the
                // standard window colors
                SetTextColor(hDc, GetSysColor(COLOR_WINDOWTEXT));
                SetBkColor(hDc, GetSysColor(COLOR_3DFACE));

                HBRUSH brush = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
                FillRect(hDc, &clientRect, brush);
                DeleteObject(brush);

                ost.precision(2);
                ost.flags(ios_base::fixed);

                if(total)
                    percent = (uint32)recvd/total*100;

                ost << percent << "%, " << m_doneItems << " of " << m_totalItems << " items (";

                if(total >= 1048576)
                {
                    total /= 1048576;
                    recvd /= 1048576;
                    
                    ost << recvd << " of " << total << " MB) ";
                }
                else if(total >= 1024)
                {
                    total /= 1024;
                    recvd /= 1024;
                    ost << recvd << " of " << total << " KB)";
                }
                else
                {
                    ost << m_doneBytes << " of " << m_totalBytes << " Bytes)";
                }

                displayString = string("Overall Progress:");

                SIZE stringSize;
                RECT rcClip = clientRect;

                GetTextExtentPoint32(hDc, displayString.c_str(), 
                                    displayString.size(), &stringSize);

                rcClip.left += 3;
                rcClip.top += ((rcClip.bottom - rcClip.top) - stringSize.cy)/2;
                rcClip.bottom = rcClip.top + stringSize.cy;

                ExtTextOut( hDc, 
                            rcClip.left, rcClip.top, 
                            ETO_CLIPPED | ETO_OPAQUE,
                            &rcClip, 
                            displayString.c_str(),
                            displayString.size(),
                            NULL);

                rcClip.left += stringSize.cx + kElementPadding;

                displayString = ost.str();

                GetTextExtentPoint32(hDc, displayString.c_str(), 
                                    displayString.size(), &stringSize);

                int32 progressWidth = 100;
                int32 bmpWidth = (float)(progressWidth - 3) * (float)percent/(float)100;
                int32 count = bmpWidth/(kProgressWidth);
                int32 remainder = bmpWidth%(kProgressWidth);

                HDC memDC = CreateCompatibleDC(hDc);              
                HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, m_progressBitmap);
                RECT progressRect = clientRect;

                progressRect.left = rcClip.left;
                progressRect.top += ((clientRect.bottom - clientRect.top) - kProgressHeight)/2 - 1;
                progressRect.bottom = progressRect.top + kProgressHeight + 2;
                progressRect.right = progressRect.left + progressWidth;

                DrawEdge(hDc, &progressRect, EDGE_SUNKEN, BF_RECT);

                uint32 i = 0;

                for(i = 0; i< count; i++)
                {
                    BitBlt(hDc, progressRect.left + 2 + i*kProgressWidth, progressRect.top + 2, kProgressWidth, kProgressHeight, 
                           memDC, 0, 0, SRCCOPY);

                }

                if(remainder)
                {
                    BitBlt(hDc, progressRect.left + 2 + i*kProgressWidth, progressRect.top + 2, remainder, kProgressHeight, 
                           memDC, 0, 0, SRCCOPY);
                }


                SelectObject(memDC, oldBitmap);
                DeleteDC(memDC);

                uint32 pad = 0;

                if(progressWidth)
                    pad = (progressWidth + kElementPadding);

                rcClip.left += pad;

                ExtTextOut( hDc, 
                            rcClip.left, rcClip.top, 
                            ETO_CLIPPED | ETO_OPAQUE,
                            &rcClip, 
                            displayString.c_str(),
                            displayString.size(),
                            NULL);

                BitBlt(dis->hDC, dis->rcItem.left, dis->rcItem.top,
                                 dis->rcItem.right - dis->rcItem.left, 
                                 dis->rcItem.bottom - dis->rcItem.top, 
                       hDc, 0, 0, SRCCOPY);          

                SelectObject(hDc, hSavedBitmap);
                DeleteObject(hBitmap);
                SelectObject(hDc, oldFont);
                DeleteDC(hDc);
            }

            DeleteObject(font);

            break;
        }

        case IDC_LIST:
        {
            uint32 uiFlags = ILD_TRANSPARENT;
            RECT rcClip;
            HIMAGELIST himl;
            int32 cxImage = 0, cyImage = 0;
            HDC hDc;
            HBITMAP hBitmap, hSavedBitmap;
            HFONT   hSavedFont;
            
            hDc = CreateCompatibleDC(dis->hDC);
            hBitmap = CreateCompatibleBitmap(dis->hDC, 
                       dis->rcItem.right - dis->rcItem.left,
                       dis->rcItem.bottom - dis->rcItem.top);
            hSavedBitmap = (HBITMAP)SelectObject(hDc, hBitmap);           
            hSavedFont = (HFONT)SelectObject(hDc, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
            
            rcClip.left = 0;
            rcClip.top = 0;
            rcClip.right = dis->rcItem.right - dis->rcItem.left;            
            rcClip.bottom = dis->rcItem.bottom - dis->rcItem.top;            

            // Get Image List
            himl = ListView_GetImageList(dis->hwndItem, LVSIL_SMALL);
            ImageList_GetIconSize(himl, &cxImage, &cyImage);

            // Check to see if this item is selected
            if(dis->itemState & ODS_SELECTED)
            {
                // Set the text background and foreground colors
                SetTextColor(hDc, GetSysColor(COLOR_HIGHLIGHTTEXT));
                SetBkColor(hDc, GetSysColor(COLOR_HIGHLIGHT));

                HBRUSH brush = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
                FillRect(hDc, &rcClip, brush);
                DeleteObject(brush);
            }
            else
            {
                // Set the text background and foreground colors to the
                // standard window colors
                SetTextColor(hDc, GetSysColor(COLOR_WINDOWTEXT));
                SetBkColor(hDc, GetSysColor(COLOR_WINDOW));
                
                HBRUSH brush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
                FillRect(hDc, &rcClip, brush);
                DeleteObject(brush);
            }

            //LV_ITEM* item = (LV_ITEM*)dis->itemData;
            DownloadItem* dli = (DownloadItem*)dis->itemData;

            string displayString;

            if(dli->GetMetaData().Title().size())
                displayString = dli->GetMetaData().Title();
            else
                displayString = dli->DestinationFile();

            // Check to see if the string fits in the clip rect.
            // If not, truncate the string and add "...".
            CalcStringEllipsis(hDc, 
                               displayString, 
                               ListView_GetColumnWidth(m_hwndList, 0) - (cxImage + 1));

            ExtTextOut( hDc, 
                        cxImage + 1, 1, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);

            // draw the icon
            if (himl)
            {
                ImageList_Draw( himl, 0, hDc, 0, 0, uiFlags);
            }

            // draw the progress column

            rcClip.left += ListView_GetColumnWidth(m_hwndList, 0);

            int32 progressWidth = 0;

            switch(dli->GetState())
            {
                case kDownloadItemState_Queued:
                {
                    if(!(dis->itemState & ODS_SELECTED))
                        SetTextColor(hDc, RGB(0, 127, 0));

                    ostringstream ost;
                    float total;
                    
                    ost.precision(2);
                    ost.flags(ios_base::fixed);

                    total = dli->GetTotalBytes();

                    if(total >= 1048576)
                    {
                        total /= 1048576;
                        ost << "Queued (" << total << " MB)";
                    }
                    else if(total >= 1024)
                    {
                        total /= 1024;
                        ost << "Queued (" << total << " KB)";
                    }
                    else
                    {
                        ost << "Queued (" << dli->GetTotalBytes() << " Bytes)";
                    }
                   
                    displayString = ost.str();

                    break;
                }

                case kDownloadItemState_Downloading:
                {
                    if(!(dis->itemState & ODS_SELECTED))
                        SetTextColor(hDc, RGB(0, 0, 192));

                    ostringstream ost;
                    float total;
                    float recvd;
                    uint32 percent;
                    
                    ost.precision(2);
                    ost.flags(ios_base::fixed);

                    total = dli->GetTotalBytes();
                    recvd = dli->GetBytesReceived();
                    percent= (uint32)recvd/total*100;

                    if(total >= 1048576)
                    {
                        total /= 1048576;
                        recvd /= 1048576;
                        ost  << percent << "% (" << recvd << " of "<< total << " MB) ";
                    }
                    else if(total >= 1024)
                    {
                        total /= 1024;
                        recvd /= 1024;
                        ost << percent << "% ("<< recvd << " of "<< total << " KB)";
                    }
                    else
                    {
                        ost << percent << "% (" << dli->GetBytesReceived() << " of " << 
                            dli->GetTotalBytes() << " Bytes)";
                    }
                   
                    displayString = ost.str();

                    SIZE stringSize;
                    
                    GetTextExtentPoint32(hDc, displayString.c_str(), 
                                            displayString.size(), &stringSize);
                    
                    int32 leftoverWidth = ListView_GetColumnWidth(m_hwndList, 1) - (stringSize.cx + kTotalPadding);

                    // do we have room to show a progress bar?
                    if(leftoverWidth - kMinProgressWidth > 0)
                    {
                        progressWidth = leftoverWidth; // padding on ends and between elements
                        int32 bmpWidth = (float)(progressWidth - 3) * (float)percent/(float)100;
                        int32 count = bmpWidth/(kProgressWidth);
                        int32 remainder = bmpWidth%(kProgressWidth);

                        //ostringstream debug;
                        //debug << "bmpWidth: " << bmpWidth << endl << "progressWidth: " << progressWidth << endl;
                        //OutputDebugString(debug.str().c_str());

                        HDC memDC = CreateCompatibleDC(hDc);
                        SelectObject(memDC, m_progressBitmap);

                        rcClip.left += kPrePadding;

                        RECT progressRect = rcClip;

                        progressRect.top += ((rcClip.bottom - rcClip.top) - kProgressHeight)/2 - 1;
                        progressRect.bottom = progressRect.top + kProgressHeight + 2;
                        progressRect.right = progressRect.left + progressWidth;

                        if(dis->itemState & ODS_SELECTED)
                        {
                            HBRUSH brush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));

                            FillRect(hDc, &progressRect, brush);
                            DeleteObject(brush);
                        }

                        DrawEdge(hDc, &progressRect, EDGE_SUNKEN, BF_RECT);

                        uint32 i = 0;

                        for(i = 0; i< count; i++)
                        {
                            BitBlt(hDc, progressRect.left + 2 + i*kProgressWidth, progressRect.top + 2, kProgressWidth, kProgressHeight, 
                                   memDC, 0, 0, SRCCOPY);

                        }

                        if(remainder)
                        {
                            BitBlt(hDc, progressRect.left + 2 + i*kProgressWidth, progressRect.top + 2, remainder, kProgressHeight, 
                                   memDC, 0, 0, SRCCOPY);
                        }

                        DeleteDC(memDC);
                    }

                    break;
                }

                case kDownloadItemState_Cancelled:
                {
                    if(!(dis->itemState & ODS_SELECTED))
                        SetTextColor(hDc, RGB(192, 0, 0));

                    ostringstream ost;

                    ost << "Cancelled";

                    displayString = ost.str();
                    break;
                }

                case kDownloadItemState_Paused:
                {
                    if(!(dis->itemState & ODS_SELECTED))
                        SetTextColor(hDc, RGB(0, 128, 128));

                    ostringstream ost;
                    float total;
                    float recvd;
                    uint32 percent;
                    
                    ost.precision(2);
                    ost.flags(ios_base::fixed);

                    total = dli->GetTotalBytes();
                    recvd = dli->GetBytesReceived();
                    percent= (uint32)recvd/total*100;

                    if(total >= 1048576)
                    {
                        total /= 1048576;
                        recvd /= 1048576;
                        ost << "Paused (" << recvd << " of "<< total << " MB - " << percent << "%)";
                    }
                    else if(total >= 1024)
                    {
                        total /= 1024;
                        recvd /= 1024;
                        ost << "Paused (" << recvd << " of "<< total << " KB - " << percent << "%)";
                    }
                    else
                    {
                        ost << "Paused (" << dli->GetBytesReceived() << " of " << 
                            dli->GetTotalBytes() << " Bytes - " << percent << "%)";
                    }
                   
                    displayString = ost.str();

                    break;
                }

                case kDownloadItemState_Error:
                {
                    if(!(dis->itemState & ODS_SELECTED))
                        SetTextColor(hDc, RGB(192, 0, 0));

                    ostringstream ost;
                    int32 index = (int32)dli->GetDownloadError();

                    ost << "Error: " <<  ErrorString[index];

                    displayString = ost.str();
                    break;
                }

                case kDownloadItemState_Done:
                    displayString = string("Download Complete");
                    break;

                default:
                    break;
            }
            uint32 pad = kPrePadding;

            if(progressWidth)
                pad = (progressWidth + kElementPadding);

            CalcStringEllipsis(hDc, 
                               displayString, 
                               ListView_GetColumnWidth(m_hwndList, 1) - pad);

            rcClip.left += pad;

            ExtTextOut( hDc, 
                        rcClip.left, rcClip.top + 1, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);

            // If we changed the colors for the selected item, undo it
            if(dis->itemState & ODS_SELECTED)
            {
                // Set the text background and foreground colors
                SetTextColor(hDc, GetSysColor(COLOR_WINDOWTEXT));
                SetBkColor(hDc, GetSysColor(COLOR_WINDOW));
            }

            // If the item is focused, now draw a focus rect around the entire row
            if(dis->itemState & ODS_FOCUS)
            {
                // Draw the focus rect
                DrawFocusRect(hDc, &dis->rcItem);
            }

            // paint the actual bitmap
            BitBlt(dis->hDC, dis->rcItem.left, dis->rcItem.top,
                             dis->rcItem.right - dis->rcItem.left, 
                             dis->rcItem.bottom - dis->rcItem.top, 
                   hDc, 0, 0, SRCCOPY);          

            SelectObject(hDc, hSavedBitmap);
            DeleteObject(hBitmap);
            SelectObject(hDc, hSavedFont);
            DeleteDC(hDc);

            break;
        }
 
        case IDC_INFO:
        {
            SetTextColor(dis->hDC, RGB(0, 0, 128));
            SetBkColor(dis->hDC, GetSysColor(COLOR_WINDOW));
            string displayString;

            string longestString = string("File Name :");

            SIZE stringSize;
                    
            GetTextExtentPoint32(dis->hDC, longestString.c_str(), 
                                    longestString.size(), &stringSize);

            uint32 dataOffset = stringSize.cx + 15;


            RECT rcClip = dis->rcItem;  

            rcClip.left += 2;

            uint32 itemCount =  ListView_GetItemCount(m_hwndList);
            DownloadItem* dli = NULL;

            //ostringstream debug;
            //debug << "itemCount: " << itemCount << endl;
            //OutputDebugString(debug.str().c_str());

            if(itemCount)
            {
                LV_ITEM item;

                for(uint32 i = 0; i < itemCount; i++)
                {
                    if(ListView_GetItemState(m_hwndList, i, LVIS_FOCUSED) & LVIS_FOCUSED)
                    {
                        item.mask = LVIF_PARAM;
                        item.iItem = i;
                        item.lParam = 0;

                        if(ListView_GetItem(m_hwndList, &item))
                        {
                            dli = (DownloadItem*)item.lParam;
                            break;
                        }
                    }
                }

                if(!dli)
                {
                    item.mask = LVIF_PARAM;
                    item.iItem = 0;
                    item.lParam = 0;

                    ListView_GetItem(m_hwndList, &item);

                    dli = (DownloadItem*)item.lParam;
                }
            }

            switch(dis->itemID)
            {
                case 0:
                {
                    displayString = string("Artist :");
                    ExtTextOut( dis->hDC, 
                        rcClip.left, rcClip.top + 1, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);

                    rcClip.left = dis->rcItem.left + dataOffset;

                    displayString = string("");

                    if(dli)
                        displayString = dli->GetMetaData().Artist();

                    SetTextColor(dis->hDC, GetSysColor(COLOR_WINDOWTEXT));

                    ExtTextOut( dis->hDC, 
                        rcClip.left, rcClip.top + 1, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);

                    break;
                }

                case 1:
                {
                    displayString = string("Album :");
                    ExtTextOut( dis->hDC, 
                        rcClip.left, rcClip.top + 1, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);

                    rcClip.left = dis->rcItem.left + dataOffset;

                    displayString = string("");

                    if(dli)
                        displayString = dli->GetMetaData().Album();

                    SetTextColor(dis->hDC, GetSysColor(COLOR_WINDOWTEXT));

                    ExtTextOut( dis->hDC, 
                        rcClip.left, rcClip.top + 1, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);
                    break;
                }

                case 2:
                {
                    displayString = string("Title :");
                    ExtTextOut( dis->hDC, 
                        rcClip.left, rcClip.top + 1, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);

                    rcClip.left = dis->rcItem.left + dataOffset;

                    displayString = string("");

                    if(dli)
                        displayString = dli->GetMetaData().Title();

                    SetTextColor(dis->hDC, GetSysColor(COLOR_WINDOWTEXT));

                    ExtTextOut( dis->hDC, 
                        rcClip.left, rcClip.top + 1, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);
                    break;
                }

                case 3:
                {
                    displayString = string("Genre :");
                    ExtTextOut( dis->hDC, 
                        rcClip.left, rcClip.top + 1, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);

                    rcClip.left = dis->rcItem.left + dataOffset;

                    displayString = string("");

                    if(dli)
                        displayString = dli->GetMetaData().Genre();


                    SetTextColor(dis->hDC, GetSysColor(COLOR_WINDOWTEXT));

                    ExtTextOut( dis->hDC, 
                        rcClip.left, rcClip.top + 1, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);
                    break;
                }

                case 4:
                {
                    displayString = string("Playlist :");
                    ExtTextOut( dis->hDC, 
                        rcClip.left, rcClip.top + 1, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);

                    rcClip.left = dis->rcItem.left + dataOffset;

                    displayString = string("");
                    if(dli)
                        displayString = dli->PlaylistName();

                    SetTextColor(dis->hDC, GetSysColor(COLOR_WINDOWTEXT));

                    ExtTextOut( dis->hDC, 
                        rcClip.left, rcClip.top + 1, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);
                    break;
                }

                case 5:
                {
                    displayString = string("File Name :");
                    ExtTextOut( dis->hDC, 
                        rcClip.left, rcClip.top + 1, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);

                    rcClip.left = dis->rcItem.left + dataOffset;

                    displayString = string("");
                    if(dli)
                        displayString = dli->DestinationFile();

                    SetTextColor(dis->hDC, GetSysColor(COLOR_WINDOWTEXT));

                    ExtTextOut( dis->hDC, 
                        rcClip.left, rcClip.top + 1, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);
                    break;
                }

                case 6:
                {
                    displayString = string("File Size :");
                    ExtTextOut( dis->hDC, 
                        rcClip.left, rcClip.top + 1, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);

                    rcClip.left = dis->rcItem.left + dataOffset;

                    ostringstream ost;
                    float total;
                    
                    ost.precision(2);
                    ost.flags(ios_base::fixed);

                    displayString = string("");
                    if(dli)
                    {
                        total = dli->GetTotalBytes();

                        if(total >= 1048576)
                        {
                            total /= 1048576;
                            ost <<  total << " MB";
                        }
                        else if(total >= 1024)
                        {
                            total /= 1024;
                            ost  << total << " KB";
                        }
                        else
                        {
                            ost <<  dli->GetTotalBytes() << " Bytes";
                        }
                   
                        displayString = ost.str();
                    }

                    SetTextColor(dis->hDC, GetSysColor(COLOR_WINDOWTEXT));

                    ExtTextOut( dis->hDC, 
                        rcClip.left, rcClip.top + 1, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);

                    break;
                }
            }

            SetTextColor(dis->hDC, GetSysColor(COLOR_WINDOWTEXT));
            SetBkColor(dis->hDC, GetSysColor(COLOR_WINDOW));

            break;
        }
    }

    return result;
}

BOOL DownloadUI::Destroy()
{

    return TRUE;
}


BOOL DownloadUI::Command(int32 command, HWND src)
{
    switch(command)
    {
        case IDC_SHOWHELP:
        {
            ShowHelp();
            break;
        }

        case IDC_RESUME:
        case IDC_PAUSE:
        case IDC_CANCEL:
        {
            uint32 itemCount = ListView_GetItemCount(m_hwndList);

            if(itemCount)
            {
                uint32 selected = ListView_GetSelectedCount(m_hwndList);

                if(selected)
                {
                    DownloadItem* dli = NULL;
                    LV_ITEM item;

                    for(uint32 i = 0; i < itemCount; i++)
                    {
                        if(ListView_GetItemState(m_hwndList, i, LVIS_SELECTED) & LVIS_SELECTED)
                        {
                            item.mask = LVIF_PARAM;
                            item.iItem = i;
                            item.lParam = 0;

                            if(ListView_GetItem(m_hwndList, &item))
                            {
                                dli = (DownloadItem*)item.lParam;
                                break;
                            }
                        }
                    }

                    if(dli)
                    {
                       switch(command)
                       {
                            case IDC_PAUSE:
                            {
                                m_dlm->PauseDownloads();
                                m_dlm->CancelDownload(dli, true);
                                break;
                            }

                            case IDC_CANCEL:
                            {
                                m_dlm->CancelDownload(dli, false);  
                                break;
                            }

                            case IDC_RESUME:
                            {
                                char szText[100];
                                GetWindowText(m_hwndResume, szText, 100);

                                if (strcmp(szText, "Start") == 0)
                                    m_dlm->ResumeDownloads();
                                else
                                    m_dlm->QueueDownload(dli, true); 
                                    
                                break;
                            }
                        }
                        SetButtonStates(dli);
                    }
                }
            }

            break;
        }

        case IDC_ADDURL:
        {
            HWND hwndURL = GetDlgItem(m_hwnd, IDC_URL);

            uint32 length = GetWindowTextLength(hwndURL) + 1; // + 0x00

            if(length)
            {
                char* sp = NULL;
                char* url = new char[length + 7]; // http://
                
                GetWindowText(hwndURL, url + 7, length);

                if(sp = strstr(url + 7, "://"))
                {
                    if(strncasecmp(url + 7, "http://", 7))
                    {
                        MessageBox(m_hwnd, 
                                   "Only HTTP downloads are supported at this time.",
                                   "Invalid protocol",
                                   MB_OK|MB_ICONINFORMATION);
                        break;
                    }

                    sp = url + 7;
                }
                else
                {
                    memcpy(url, "http://", 7);
                    sp = url;
                }

                m_dlm->AddItem(sp);
            
                delete [] url;
                
                SetWindowText(hwndURL, "");
            }
            break;
        }

        case IDC_CLOSE:
        {
            if (GetAsyncKeyState(VK_SHIFT) < 0)
            {
                ShowWindow(GetDlgItem(m_hwnd, IDC_DLMTEXT), SW_HIDE);
                ShowWindow(GetDlgItem(m_hwnd, IDC_FREETRACKS), SW_HIDE);
                ShowWindow(GetDlgItem(m_hwnd, IDC_URL), SW_SHOW);
                ShowWindow(GetDlgItem(m_hwnd, IDC_ADDURL), SW_SHOW);
                break;
            }

            SendMessage(m_hwnd, WM_CLOSE, 0, 0);
            break;
        }
		case IDC_CHECK1:
		{
			bool checkSet = false;
			if (Button_GetCheck(m_hwndCheck) == BST_CHECKED)
				checkSet = true;

			m_prefs->SetPrefBoolean(kCloseDLMOnCompletePref, checkSet);
			break;
		}
    }

    return TRUE;        
}

BOOL DownloadUI::Notify(int32 controlId, NMHDR* nmh)
{
    BOOL result = TRUE;

    switch(controlId)
    {
        case IDC_LIST:
        {
            NM_LISTVIEW* nmlv = (NM_LISTVIEW*)nmh;

            if(nmh->code == LVN_ITEMCHANGED)
            {
                ListView_RedrawItems(m_hwndInfo, 0, ListView_GetItemCount(m_hwndInfo) - 1);
                SetButtonStates((DownloadItem*)nmlv->lParam);
            }

            break;
        }

    }

    return result;
}


void DownloadUI::SetButtonStates(DownloadItem *dli)
{
   switch(dli->GetState())
   {
       case kDownloadItemState_Queued:
           EnableWindow(m_hwndPause, FALSE);
           EnableWindow(m_hwndCancel, TRUE);
           EnableWindow(m_hwndResume, m_dlm->IsPaused());
           SetWindowText(m_hwndResume, "Start");
           break;
           
       case kDownloadItemState_Downloading:
           EnableWindow(m_hwndPause, TRUE);
           EnableWindow(m_hwndCancel, TRUE);
           EnableWindow(m_hwndResume, FALSE);
           SetWindowText(m_hwndResume, "Resume");
           break;
       
       case kDownloadItemState_Cancelled:
       case kDownloadItemState_Error:
           EnableWindow(m_hwndPause, FALSE);
           EnableWindow(m_hwndCancel, FALSE);
           EnableWindow(m_hwndResume, TRUE);
           SetWindowText(m_hwndResume, "Resume");
           break;

       case kDownloadItemState_Paused:
           EnableWindow(m_hwndPause, FALSE);
           EnableWindow(m_hwndCancel, TRUE);
           EnableWindow(m_hwndResume, TRUE);
           SetWindowText(m_hwndResume, "Resume");
           break;
       
       case kDownloadItemState_Done:
           EnableWindow(m_hwndPause, FALSE);
           EnableWindow(m_hwndCancel, FALSE);
           EnableWindow(m_hwndResume, FALSE);
           SetWindowText(m_hwndResume, "Resume");
           break;

       default:
           break;
   }
}

BOOL CALLBACK DownloadUI::MainProc( HWND hwnd, 
                                    UINT msg, 
                                    WPARAM wParam, 
                                    LPARAM lParam )
{
    BOOL result = FALSE;
    static DownloadUI* m_ui = NULL;

    switch(msg)
    {
        case WM_INITDIALOG:
        {
            m_ui = (DownloadUI*)lParam;

            m_ui->SetWindowHandle(hwnd);

            result = m_ui->InitDialog();

            result = FALSE;
            break;
        }     

        case WM_INITMENUPOPUP:
        {
            HMENU menuPopup = (HMENU)wParam;
            BOOL fSystemMenu = (BOOL) HIWORD(lParam);

            if(fSystemMenu)
            {
                int32 count = GetMenuItemCount(menuPopup);

                for(int32 i = 0; i < count; i++)
                {
                    int32 id = GetMenuItemID(menuPopup, i);

                    if(61488 == id || id == 61440) // MAXIMIZE & SIZE COMMAND
                    {
                        EnableMenuItem(menuPopup, id, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
                    }
                }
            }

            result = TRUE;
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
            if (mis->CtlType != ODT_LISTVIEW || mis->CtlID == IDC_INFO)
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

            result =  TRUE;
            break;
        }

        case WM_DRAWITEM:
        {
            result = m_ui->DrawItem(wParam, (DRAWITEMSTRUCT*) lParam);
            break;
        }

        case WM_COMMAND:
        {
            result = m_ui->Command(wParam, (HWND)lParam);
            break;
        }     
        
        case WM_HELP:
        {
            m_ui->ShowHelp();
            return 1;
        }
            
        case WM_NOTIFY:
        {
            result = m_ui->Notify(wParam, (NMHDR*)lParam);
            break;
        }  

        case WM_CLOSE:
        {
            ShowWindow(hwnd, SW_HIDE);
            result = TRUE;
            break;
        }

        case WM_DESTROY:
        {
            result = m_ui->Destroy();
            break;
        }
    }

    return result;
}

uint32 DownloadUI::CalcStringEllipsis(HDC hdc, string& displayString, int32 columnWidth)
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
    if(sizeString.cx >= columnWidth)
    {
        GetTextExtentPoint32(hdc, szEllipsis, strlen(szEllipsis), &sizeEllipsis);
       
        while(temp.size() > 1)
        {
            temp.erase(temp.size() - 1, 1);

            GetTextExtentPoint32(hdc, temp.c_str(), temp.size(), &sizeString);
            
            if((sizeString.cx + sizeEllipsis.cx) < columnWidth)
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

void DownloadUI::UpdateOverallProgress()
{
    uint32 itemCount =  ListView_GetItemCount(m_hwndList);
    uint32 totalBytes = 0, doneBytes = 0;
    uint32 totalItems = 0, doneItems = 0;
    DownloadItem* dli = NULL;

    if(itemCount)
    {
        LV_ITEM item;

        for(uint32 i = 0; i < itemCount; i++)
        {
            item.mask = LVIF_PARAM;
            item.iItem = i;
            item.lParam = 0;

            if(ListView_GetItem(m_hwndList, &item))
            {
                dli = (DownloadItem*)item.lParam;

                DownloadItemState state = dli->GetState();

                if(state == kDownloadItemState_Queued ||
                    state == kDownloadItemState_Downloading)
                {
                    totalItems++;
                    totalBytes += dli->GetTotalBytes();
                    doneBytes += dli->GetBytesReceived();
                }
                else if(state == kDownloadItemState_Done)
                {
                    doneItems++;
                    totalItems++;
                    totalBytes += dli->GetTotalBytes();
                    doneBytes += dli->GetTotalBytes();
                }
            }
        }
    }

    m_totalBytes = totalBytes;
    m_doneBytes = doneBytes;
    m_totalItems = totalItems;
    m_doneItems = doneItems;
    InvalidateRect(m_hwndProgress, NULL, FALSE);

	if (m_doneItems == m_totalItems && m_totalItems != 0) {
		bool checkSet = false;
		if (Button_GetCheck(m_hwndCheck) == BST_CHECKED)
			checkSet = true;

		if (checkSet) {
			SendMessage(m_hwnd, WM_CLOSE, 0, 0);
		}
	}
}

LRESULT WINAPI 
ListWndProc(HWND hwnd, 
            UINT msg, 
            WPARAM wParam, 
            LPARAM lParam)
{
    DownloadUI* ui = (DownloadUI*)GetProp( hwnd, "ui" );

    //  Pass all non-custom messages to old window proc
    return ui->ListWndProc(hwnd, msg, wParam, lParam);
}

LRESULT DownloadUI::ListWndProc(HWND hwnd, 
                                UINT msg, 
                                WPARAM wParam, 
                                LPARAM lParam)
{
    WNDPROC lpOldProc = (WNDPROC)GetProp( hwnd, "oldproc" );

    switch(msg)
    {
        case WM_ERASEBKGND:
        {
            if (m_duringUpdate)
               return 1;
            break;
        }
    }

    return CallWindowProc(lpOldProc, hwnd, msg, wParam, lParam);
}


LRESULT WINAPI ProgressWndProc(HWND hwnd, 
                               UINT msg, 
                               WPARAM wParam, 
                               LPARAM lParam)
{
    WNDPROC lpOldProc = (WNDPROC)GetProp( hwnd, "oldproc" );

    switch(msg)
    {
        case WM_ERASEBKGND:
        {
            return 1;
        }
    }

    return CallWindowProc(lpOldProc, hwnd, msg, wParam, lParam);
}

LRESULT WINAPI 
FreeTracksWndProc(HWND hwnd, 
                  UINT msg, 
                  WPARAM wParam, 
                  LPARAM lParam)
{
    DownloadUI* ui = (DownloadUI*)GetProp( hwnd, "ui" );

    

    //  Pass all non-custom messages to old window proc
    return ui->FreeTracksWndProc(hwnd, msg, wParam, lParam);
}

LRESULT DownloadUI::FreeTracksWndProc(HWND hwnd, 
                                      UINT msg, 
                                      WPARAM wParam, 
                                      LPARAM lParam)
{
    WNDPROC lpOldProc = (WNDPROC)GetProp( hwnd, "oldproc" );

    switch(msg)
    {
        case WM_DESTROY:   
        {
            //  Put back old window proc and
            SetWindowLong( hwnd, GWL_WNDPROC, (DWORD)lpOldProc );

            // remove window property
            RemoveProp( hwnd, "oldproc" );
            RemoveProp( hwnd, "ui" ); 
            break;
        }

        case WM_SETCURSOR:   
        {
            if(m_overURL)
            {
                SetCursor(m_handCursor);
                return TRUE;
            }
            break;
        }

        case WM_MOUSEMOVE:
        {
            POINT pt;

            pt.x = LOWORD(lParam);  // horizontal position of cursor 
            pt.y = HIWORD(lParam);  // vertical position of cursor 

            if(PtInRect(&m_urlRect, pt))
                m_overURL = true;
            else
                m_overURL = false;
            break;
        }

        case WM_LBUTTONDOWN:
        {
            POINT pt;

            pt.x = LOWORD(lParam);  // horizontal position of cursor 
            pt.y = HIWORD(lParam);  // vertical position of cursor 

            if(PtInRect(&m_urlRect, pt))
            {
                if (strcasecmp(BRANDING_COMPANY, "EMusic") == 0)
                    ShellExecute(hwnd, "open", szEMusicURL, NULL, 
                                 NULL, SW_SHOWNORMAL);
                else
                    ShellExecute(hwnd, "open", szFreeAmpURL, NULL, 
                                 NULL, SW_SHOWNORMAL);
            }
                
            break;
        }
    }

    return CallWindowProc(lpOldProc, hwnd, msg, wParam, lParam);
}

void DownloadUI::ShowHelp(void)
{
    ::ShowHelp(m_context, Download_Manager);
}
