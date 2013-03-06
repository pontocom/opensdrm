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
	
   $Id: Win32PreferenceWindow.cpp,v 1.1 2004/10/13 16:17:07 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

/* system headers */
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <commctrl.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <sstream>
#include <set>

using namespace std;

#include "eventdata.h"
#include "thread.h"
#include "Win32PreferenceWindow.h"
#include "Win32Window.h"
#include "help.h"
#include "Debug.h"
#include "player.h"

#define DB Debug_v("%s:%d\n", __FILE__, __LINE__);

static Win32PreferenceWindow *g_pCurrentPrefWindow = NULL;
const char* kThemeFileFilter =
            BRANDING" Themes (.fat)\0"
            "*.fat\0"
            "All Files (*.*)\0"
            "*.*\0";

uint32 CalcStringEllipsis(HDC hdc, string& displayString, int32 columnWidth);

#define UWM_HELP WM_USER + 1

static BOOL CALLBACK 
MainCallback(HWND hwnd, 
             UINT msg, 
             WPARAM wParam, 
             LPARAM lParam)
{
	return g_pCurrentPrefWindow->MainProc(hwnd, msg, wParam, lParam);
}

static BOOL CALLBACK 
PrefGeneralCallback(HWND hwnd, 
                    UINT msg, 
                    WPARAM wParam, 
                    LPARAM lParam)
{
	return g_pCurrentPrefWindow->PrefGeneralProc(hwnd, msg, wParam, lParam);
}

static BOOL CALLBACK 
PrefThemeCallback(HWND hwnd, 
                  UINT msg, 
                  WPARAM wParam, 
                  LPARAM lParam)
{
	return g_pCurrentPrefWindow->PrefThemeProc(hwnd, msg, wParam, lParam);
}

static BOOL CALLBACK 
PrefStreamingCallback(HWND hwnd, 
                      UINT msg, 
                      WPARAM wParam, 
                      LPARAM lParam)
{
	return g_pCurrentPrefWindow->PrefStreamingProc(hwnd, msg, wParam, lParam);
}

static BOOL CALLBACK 
PrefPluginsCallback(HWND hwnd, 
                    UINT msg, 
                    WPARAM wParam, 
                    LPARAM lParam)
{
	return g_pCurrentPrefWindow->PrefPluginsProc(hwnd, msg, wParam, lParam);
}

static BOOL CALLBACK 
PrefCDAudioCallback(HWND hwnd, 
                    UINT msg, 
                    WPARAM wParam, 
                    LPARAM lParam)
{
	return g_pCurrentPrefWindow->PrefCDAudioProc(hwnd, msg, wParam, lParam);
}

static BOOL CALLBACK 
PrefAdvancedCallback(HWND hwnd, 
                     UINT msg, 
                     WPARAM wParam, 
                     LPARAM lParam)
{
	return g_pCurrentPrefWindow->PrefAdvancedProc(hwnd, msg, wParam, lParam);
}

static BOOL CALLBACK 
PrefAboutCallback(HWND hwnd, 
                  UINT msg, 
                  WPARAM wParam, 
                  LPARAM lParam)
{
	return g_pCurrentPrefWindow->PrefAboutProc(hwnd, msg, wParam, lParam);
}

static BOOL CALLBACK 
PrefDirectoryCallback(HWND hwnd, 
                  UINT msg, 
                  WPARAM wParam, 
                  LPARAM lParam)
{
	return g_pCurrentPrefWindow->PrefDirectoryProc(hwnd, msg, wParam, lParam);
}   

static BOOL CALLBACK 
PrefDebugCallback(HWND hwnd, 
                  UINT msg, 
                  WPARAM wParam, 
                  LPARAM lParam)
{
	return g_pCurrentPrefWindow->PrefDebugProc(hwnd, msg, wParam, lParam);
}            

static BOOL CALLBACK 
PrefBrowserCallback(HWND hwnd, 
                    UINT msg, 
                    WPARAM wParam, 
                    LPARAM lParam)
{
      return g_pCurrentPrefWindow->PrefBrowserProc(hwnd, msg, wParam, lParam);
}

Win32PreferenceWindow::Win32PreferenceWindow(FAContext *context,
                                             ThemeManager *pThemeMan,
                                             uint32 defaultPage) :
     PreferenceWindow(context, pThemeMan)
{     
	g_pCurrentPrefWindow = this;
    m_defaultPage = defaultPage;
    m_prefs = context->prefs;
    m_hwndPref = NULL;
    m_startPage = defaultPage;
    m_currentPage = NULL;
}

Win32PreferenceWindow::~Win32PreferenceWindow(void)
{
	g_pCurrentPrefWindow = NULL;
} 

bool Win32PreferenceWindow::Show(Window *pWindow)
{
	HWND hWnd;
    
    hWnd = ((Win32Window *)pWindow)->GetWindowHandle(); 
    return DisplayPreferences(hWnd);
}

bool Win32PreferenceWindow::DisplayPreferences(HWND hwndParent)
{
    bool result = false;
    HINSTANCE hinst = (HINSTANCE)GetWindowLong(hwndParent, GWL_HINSTANCE);

    PrefPage page;

    page.hInstance = hinst;
    page.pszTemplate = MAKEINTRESOURCE(IDD_PREF_GENERAL);
    page.pfnDlgProc = PrefGeneralCallback;
    page.bChild = false;
    page.lParam = 0;
    page.hwnd = NULL;
    
    m_pages.push_back(page);

    page.pszTemplate = MAKEINTRESOURCE(IDD_PREF_THEME);
    page.pfnDlgProc = PrefThemeCallback;
    m_pages.push_back(page);

    page.pszTemplate = MAKEINTRESOURCE(IDD_PREF_DIRECTORY);
    page.pfnDlgProc = PrefDirectoryCallback;
    m_pages.push_back(page);

    page.pszTemplate = MAKEINTRESOURCE(IDD_PREF_STREAMING);
    page.pfnDlgProc = PrefStreamingCallback;
    m_pages.push_back(page);

    page.pszTemplate = MAKEINTRESOURCE(IDD_PREF_PLUGINS);
    page.pfnDlgProc = PrefPluginsCallback;
    m_pages.push_back(page);

    page.pszTemplate = MAKEINTRESOURCE(IDD_PREF_CDAUDIO);
    page.pfnDlgProc = PrefCDAudioCallback;
    m_pages.push_back(page);

    page.pszTemplate = MAKEINTRESOURCE(IDD_PREF_ADVANCED);
    page.pfnDlgProc = PrefAdvancedCallback;
    m_pages.push_back(page);

    page.pszTemplate = MAKEINTRESOURCE(IDD_PREF_BROWSER);
    page.pfnDlgProc = PrefBrowserCallback;
    m_pages.push_back(page);

    page.pszTemplate = MAKEINTRESOURCE(IDD_PREF_ABOUT);
    page.pfnDlgProc = PrefAboutCallback;
    m_pages.push_back(page);
        
    GetPrefsValues(&m_originalValues);
    GetPrefsValues(&m_currentValues);
    GetPrefsValues(&m_proposedValues);

    result = (DialogBox(hinst, 
                        MAKEINTRESOURCE(IDD_MAIN_PREF_DIALOG),
                        hwndParent,
                        MainCallback) > 0);

    return result;
}

void Win32PreferenceWindow::GetPrefsValues(PrefsStruct* values)
{
    uint32 bufferSize = 1;
    uint32 size;

    // get the string prefs
    char* buffer = (char*)malloc(bufferSize);

    size = bufferSize;

    if(kError_BufferTooSmall == m_prefs->GetPrefString(kPMOPref, buffer, &size))
    {
        bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        m_prefs->GetPrefString(kPMOPref, buffer, &size);
    }

    values->defaultPMO = buffer;
    size = bufferSize;

    if(kError_BufferTooSmall == m_prefs->GetPrefString(kUIPref, buffer, &size))
    {
        bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        m_prefs->GetPrefString(kUIPref, buffer, &size);
    }
    
    values->defaultUI = buffer;
    size = bufferSize;

    if(kError_BufferTooSmall == m_prefs->GetPrefString(kProxyHostPref, buffer, &size))
    {
        bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        m_prefs->GetPrefString(kProxyHostPref, buffer, &size);
    }

    values->proxyServer = buffer;
    size = bufferSize;

    if(kError_BufferTooSmall == m_prefs->GetPrefString(kSaveStreamsDirPref, buffer, &size))
    {
        bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        m_prefs->GetPrefString(kSaveStreamsDirPref, buffer, &size);
    }

    values->saveStreamsDirectory = buffer;
    size = bufferSize;

    if(kError_BufferTooSmall == m_prefs->GetPrefString(kAlternateNICAddressPref, buffer, &size))
    {
        bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        m_prefs->GetPrefString(kAlternateNICAddressPref, buffer, &size);
    }

    values->alternateIP = buffer;
    size = bufferSize;

    if(kError_BufferTooSmall == m_prefs->GetPrefString(kThemeDefaultFontPref, buffer, &size))
    {
        bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        m_prefs->GetPrefString(kThemeDefaultFontPref, buffer, &size);
    }

    values->defaultFont = buffer;
    size = bufferSize;

    if(kError_BufferTooSmall == m_prefs->GetPrefString(kPlaylistHeaderColumnsPref, buffer, &size))
    {
        bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        m_prefs->GetPrefString(kPlaylistHeaderColumnsPref, buffer, &size);
    }

    values->playlistHeaderColumns = buffer;
    size = bufferSize;

    if(kError_BufferTooSmall == m_prefs->GetPrefString(kSaveMusicDirPref, buffer, &size))
    {
        bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        m_prefs->GetPrefString(kSaveMusicDirPref, buffer, &size);
    }

    values->saveMusicDirectory = buffer;
    size = bufferSize;

    if(kError_BufferTooSmall == m_prefs->GetPrefString(kMBServerPref, buffer, &size))
    {
        bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        m_prefs->GetPrefString(kMBServerPref, buffer, &size);
    }

    values->MBServerURL = buffer;
    size = bufferSize;

    if(kError_BufferTooSmall == m_prefs->GetPrefString(kWatchThisDirectoryPref, buffer, &size))
    {
        bufferSize = size;
        buffer = (char*)realloc(buffer, bufferSize);
        m_prefs->GetPrefString(kWatchThisDirectoryPref, buffer, &size);
    }

    char *cp = buffer;
    char *name = cp;

    while(cp = strchr(cp, ';'))
    {
        *cp = 0x00;
        values->watchDirectories.insert(string(name));

        cp++;
        name = cp;
    }

    if(*name)
    {
        values->watchDirectories.insert(string(name));
    }

    int32 timeOut;

    m_prefs->GetPrefInt32(kWatchThisDirTimeoutPref, &timeOut);
    values->watchForNewMusic = (timeOut != 0);


    size = bufferSize;
    m_pThemeMan->GetCurrentTheme(values->currentTheme);

    // get the other m_prefs
    m_prefs->GetPrefInt32(kDecoderThreadPriorityPref, &values->decoderThreadPriority);
    m_prefs->GetPrefInt32(kInputBufferSizePref, &values->inputBufferSize);
    m_prefs->GetPrefInt32(kOutputBufferSizePref, &values->outputBufferSize);
    m_prefs->GetPrefInt32(kPreBufferPref, &values->preBufferLength);
    m_prefs->GetPrefBoolean(kStayOnTopPref, &values->stayOnTop);
    m_prefs->GetPrefBoolean(kLiveInTrayPref, &values->liveInTray);
    m_prefs->GetPrefBoolean(kSaveStreamsPref, &values->saveStreams);
    m_prefs->GetPrefBoolean(kUseProxyPref, &values->useProxyServer);
    m_prefs->GetPrefBoolean(kUseAlternateNICPref, &values->useAlternateIP);
    m_prefs->GetPrefBoolean(kUseDebugLogPref, &values->enableLogging);
    m_prefs->GetPrefBoolean(kLogMainPref, &values->logMain);
    m_prefs->GetPrefBoolean(kLogDecodePref, &values->logDecoder);
    m_prefs->GetPrefBoolean(kLogInputPref, &values->logInput);
    m_prefs->GetPrefBoolean(kLogOutputPref, &values->logOutput);
    m_prefs->GetPrefBoolean(kLogPerformancePref, &values->logPerformance);
    m_prefs->GetPrefBoolean(kAskToReclaimFiletypesPref, &values->askReclaimFiletypes);
    m_prefs->GetPrefBoolean(kReclaimFiletypesPref, &values->reclaimFiletypes);
    m_prefs->GetPrefBoolean(kShowToolbarTextLabelsPref, &values->useTextLabels);
    m_prefs->GetPrefBoolean(kShowToolbarImagesPref, &values->useImages);
    m_prefs->GetPrefBoolean(kSaveCurrentPlaylistOnExitPref, &values->savePlaylistOnExit);
    m_prefs->GetPrefBoolean(kPlayImmediatelyPref, &values->playImmediately);
    m_prefs->GetPrefBoolean(kConvertUnderscoresToSpacesPref, &values->convertUnderscores);
    m_prefs->GetPrefBoolean(kCheckCDAutomaticallyPref, &values->updateCDAutomatically);

    free(buffer);
}

void Win32PreferenceWindow::SavePrefsValues(PrefsStruct* values)
{
    m_prefs->SetPrefString(kPMOPref, values->defaultPMO.c_str());
    m_prefs->SetPrefString(kUIPref, values->defaultUI.c_str());
    m_prefs->SetPrefInt32(kDecoderThreadPriorityPref, values->decoderThreadPriority);
    m_prefs->SetPrefInt32(kInputBufferSizePref, values->inputBufferSize);
    m_prefs->SetPrefInt32(kOutputBufferSizePref, values->outputBufferSize);
    m_prefs->SetPrefInt32(kPreBufferPref, values->preBufferLength);
    m_prefs->SetPrefBoolean(kStayOnTopPref, values->stayOnTop);
    m_prefs->SetPrefBoolean(kLiveInTrayPref, values->liveInTray);

    m_prefs->SetPrefBoolean(kSaveStreamsPref, values->saveStreams);
    m_prefs->SetPrefString(kSaveStreamsDirPref, values->saveStreamsDirectory.c_str());
    m_prefs->SetPrefString(kProxyHostPref, values->proxyServer.c_str());
    m_prefs->SetPrefBoolean(kUseProxyPref, values->useProxyServer);
    m_prefs->SetPrefString(kAlternateNICAddressPref, values->alternateIP.c_str());
    m_prefs->SetPrefBoolean(kUseAlternateNICPref, values->useAlternateIP);

    m_prefs->SetPrefBoolean(kUseDebugLogPref, values->enableLogging);
    m_prefs->SetPrefBoolean(kLogMainPref, values->logMain);
    m_prefs->SetPrefBoolean(kLogDecodePref, values->logDecoder);
    m_prefs->SetPrefBoolean(kLogInputPref, values->logInput);
    m_prefs->SetPrefBoolean(kLogOutputPref, values->logOutput);
    m_prefs->SetPrefBoolean(kLogPerformancePref, values->logPerformance);

    m_prefs->SetPrefString(kThemeDefaultFontPref, values->defaultFont.c_str());
    if (m_oThemeList[values->currentTheme].length() > 0)
       m_pThemeMan->UseTheme(m_oThemeList[values->currentTheme]);

    m_prefs->SetPrefString(kSaveMusicDirPref, values->saveMusicDirectory.c_str());
    m_prefs->SetPrefString(kPlaylistHeaderColumnsPref, values->playlistHeaderColumns.c_str());
    m_prefs->SetPrefBoolean(kAskToReclaimFiletypesPref, values->askReclaimFiletypes);
    m_prefs->SetPrefBoolean(kReclaimFiletypesPref, values->reclaimFiletypes);

    set<string>::const_iterator i;

    m_prefs->SetPrefBoolean(kShowToolbarTextLabelsPref, values->useTextLabels);
    m_prefs->SetPrefBoolean(kShowToolbarImagesPref, values->useImages);

    m_prefs->SetPrefBoolean(kSaveCurrentPlaylistOnExitPref, values->savePlaylistOnExit);
    m_prefs->SetPrefBoolean(kPlayImmediatelyPref, values->playImmediately);
    m_prefs->SetPrefBoolean(kConvertUnderscoresToSpacesPref, values->convertUnderscores);

    string dirList;

    for(i = values->watchDirectories.begin(); 
        i != values->watchDirectories.end(); 
        i++)
    {
        dirList += *i;
        dirList += ";";
    }

    m_prefs->SetPrefString(kWatchThisDirectoryPref, dirList.c_str());
    m_prefs->SetPrefInt32(kWatchThisDirTimeoutPref, (values->watchForNewMusic ? kDefaultWatchThisDirTimeout : 0));
    m_prefs->SetPrefBoolean(kCheckCDAutomaticallyPref, values->updateCDAutomatically);
    m_prefs->SetPrefString(kMBServerPref, values->MBServerURL.c_str());

    m_pContext->target->AcceptEvent(new Event(INFO_PrefsChanged));
    m_currentValues = m_proposedValues = *values;
}

bool Win32PreferenceWindow::MainProc(HWND hwnd, 
						             UINT msg, 
						             WPARAM wParam, 
						             LPARAM lParam)
{
	bool result = false;

	switch(msg)
	{
		case WM_INITDIALOG:
		{
            m_hwndPref = hwnd;

            SetWindowText(hwnd, BRANDING" Preferences");

            m_caption = "Testing...1...2...3...";

            InitializePrefDialog();

            HWND hwndList = GetDlgItem(hwnd, IDC_LIST);

            HTREEITEM item = TreeView_GetRoot(hwndList);

            for(uint32 i = 0; i < m_startPage; i++)
            {
                HTREEITEM child = TreeView_GetChild(hwndList, item);

                if(child)
                    item = child;
                else
                {
                    HTREEITEM sibling = TreeView_GetNextSibling(hwndList, item);

                    if(sibling)
                        item = sibling;
                    else
                    {
                        item = TreeView_GetNextSibling(hwndList, 
                                                       TreeView_GetParent(hwndList, 
                                                                          item));
                    }
                }
            }

            TreeView_Select(hwndList, item, TVGN_CARET);

            HWND hwndApply = GetDlgItem(hwnd, IDC_APPLY);

            EnableWindow(hwndApply, FALSE);

			//result = true;

			break;
		}

        case WM_PAINT:
        {
            RECT fadeRect;
            HWND hwndFade = GetDlgItem(hwnd, IDC_FADE);
            GetClientRect(hwndFade, &fadeRect);

            MapWindowPoints(hwndFade, hwnd, (LPPOINT)&fadeRect, 2);

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            COLORREF c1, c2, textColor;

            c1 = GetSysColor(COLOR_ACTIVECAPTION);
            c2 = GetSysColor(COLOR_3DFACE);
            textColor = GetSysColor(COLOR_CAPTIONTEXT);

            float r1 = GetRValue(c1);
            float g1 = GetGValue(c1);
            float b1 = GetBValue(c1);

            float r2 = GetRValue(c2);
            float g2 = GetGValue(c2);
            float b2 = GetBValue(c2);

            float r3, g3, b3;

            int dx = fadeRect.right - fadeRect.left;
            float dt = 1/(float)dx;
            float t = 1;

            for(int x = 0; x < dx; x++)
            {
                r3 = r1*t + r2*(1-t);
                g3 = g1*t + g2*(1-t);
                b3 = b1*t + b2*(1-t);

                RECT rect = fadeRect;

                rect.left += x;
                rect.right = rect.left + 1;

                HBRUSH brush = CreateSolidBrush(RGB(r3, g3, b3));
                FillRect(hdc, &rect, brush);
                DeleteObject(brush);

                t -= dt;
            }

            RECT textRect = fadeRect;

            textRect.left += 5;

            SetTextColor(hdc, textColor);
            SetBkMode(hdc, TRANSPARENT);

            DrawText(hdc, m_caption.c_str(), m_caption.size(), &textRect, 
                        DT_VCENTER|DT_SINGLELINE|DT_LEFT);
       
            EndPaint(hwnd, &ps);
            break;
        }

        case WM_CLOSE:
		{
            //PostQuitMessage(0);
            EndDialog(hwnd, FALSE);
			result = TRUE;
			break;
		}

        case WM_NOTIFY:
        {
            LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) lParam;

            if(pnmtv->hdr.idFrom == IDC_LIST)
            {
                if(pnmtv->hdr.code == TVN_SELCHANGED)
                {
                    ShowPrefPage((PrefPage*)pnmtv->itemOld.lParam, false);

                    ShowPrefPage((PrefPage*)pnmtv->itemNew.lParam, true);
                }
            }

            break;
        }

        case PSM_UNCHANGED:
        {
            HWND hwndApply = GetDlgItem(hwnd, IDC_APPLY);

            EnableWindow(hwndApply, FALSE);
            break;
        }
        case PSM_CHANGED:
        {
            HWND hwndApply = GetDlgItem(hwnd, IDC_APPLY);

            EnableWindow(hwndApply, TRUE);
            break;
        }
       
        case WM_HELP:
        {
            if(m_currentPage)
                SendMessage(m_currentPage->hwnd, UWM_HELP, 0, 0);
            break;
        }

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDCANCEL:
                    //PostQuitMessage(0);

					// 
					//  No changed message needs to be broadcast, really.
                    //
					//SavePrefsValues(&m_originalValues);
                    EndDialog(hwnd, FALSE);
                    break;

                case IDOK:
                    //PostQuitMessage(0);
                    SavePrefsValues(&m_proposedValues);
                    EndDialog(hwnd, TRUE);
                    break;

                case IDC_HELPME:
                    if(m_currentPage)
                        SendMessage(m_currentPage->hwnd, UWM_HELP, 0, 0);
                    break;

                case IDC_APPLY:
                {
                    SavePrefsValues(&m_proposedValues);
                    HWND hwndApply = GetDlgItem(hwnd, IDC_APPLY);

                    EnableWindow(hwndApply, FALSE);
                    break;
                }
            }

            break;
        }
	
	}

	return result;
}

bool Win32PreferenceWindow::PrefGeneralProc(HWND hwnd, 
                                            UINT msg, 
                                            WPARAM wParam, 
                                            LPARAM lParam)      
{
    bool result = false;    

    switch(msg)
    {
        case WM_INITDIALOG:
        {
            // get the handles to all our controls
            HWND hwndStayOnTop = GetDlgItem(hwnd, IDC_STAYONTOP);
            HWND hwndLiveInTray = GetDlgItem(hwnd, IDC_TRAY);
            HWND hwndAskReclaimFiletypes = GetDlgItem(hwnd, IDC_ASKRECLAIM);
            HWND hwndReclaimFiletypes = GetDlgItem(hwnd, IDC_RECLAIMFILETYPES);
            HWND hwndTextOnly = GetDlgItem(hwnd, IDC_TEXTONLY);
            HWND hwndImagesOnly = GetDlgItem(hwnd, IDC_IMAGESONLY);
            HWND hwndTextAndImages = GetDlgItem(hwnd, IDC_TEXTANDIMAGES);
            HWND hwndSavePlaylistOnExit = GetDlgItem(hwnd, IDC_SAVECURRENTLIST);
            HWND hwndDefaultAction = GetDlgItem(hwnd, IDC_DEFAULTACTION);
            HWND hwndConvertUnderscores = GetDlgItem(hwnd, IDC_UNDERSCORES);


            Button_SetCheck(hwndStayOnTop, m_originalValues.stayOnTop);
            Button_SetCheck(hwndLiveInTray, m_originalValues.liveInTray);

            Button_SetCheck(hwndReclaimFiletypes, m_originalValues.reclaimFiletypes);
            Button_SetCheck(hwndAskReclaimFiletypes, m_originalValues.askReclaimFiletypes);

            Button_SetCheck(hwndSavePlaylistOnExit, m_originalValues.savePlaylistOnExit);

            Button_SetCheck(hwndDefaultAction, !m_originalValues.playImmediately);

            Button_SetCheck(hwndConvertUnderscores, m_originalValues.convertUnderscores);

            Button_SetCheck(hwndTextOnly, 
                m_originalValues.useTextLabels && !m_originalValues.useImages);
            Button_SetCheck(hwndImagesOnly,
                !m_originalValues.useTextLabels && m_originalValues.useImages);
            Button_SetCheck(hwndTextAndImages, 
                m_originalValues.useTextLabels && m_originalValues.useImages);
            
            break;
        }

        case UWM_HELP:
        case WM_HELP:
        {
            ShowHelp(m_pContext, Preferences_General);
            break;
        }

        case WM_COMMAND:
        {
            HWND hwndCtrl = (HWND) lParam;

            switch(LOWORD(wParam))
            {
                case IDC_TEXTONLY:
                {
                    if(Button_GetCheck(hwndCtrl) == BST_CHECKED)
                    {
                        m_proposedValues.useTextLabels = true;
                        m_proposedValues.useImages = false;
                    }

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_IMAGESONLY:
                {
                    if(Button_GetCheck(hwndCtrl) == BST_CHECKED)
                    {
                        m_proposedValues.useTextLabels = false;
                        m_proposedValues.useImages = true;
                    }

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_TEXTANDIMAGES:
                {
                    if(Button_GetCheck(hwndCtrl) == BST_CHECKED)
                    {
                        m_proposedValues.useTextLabels = true;
                        m_proposedValues.useImages = true;
                    }

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_UNDERSCORES:
                {
                    if(Button_GetCheck(hwndCtrl) == BST_CHECKED)
                    {
                        m_proposedValues.convertUnderscores = true;
                    }
                    else
                    {
                        m_proposedValues.convertUnderscores = false;
                    }

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_DEFAULTACTION:
                {
                    if(Button_GetCheck(hwndCtrl) == BST_CHECKED)
                    {
                        m_proposedValues.playImmediately = false;
                    }
                    else
                    {
                        m_proposedValues.playImmediately = true;
                    }

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_SAVECURRENTLIST:
                {
                    if(Button_GetCheck(hwndCtrl) == BST_CHECKED)
                    {
                        m_proposedValues.savePlaylistOnExit = true;
                    }
                    else
                    {
                        m_proposedValues.savePlaylistOnExit = false;
                    }

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_STAYONTOP:
                {
                    if(Button_GetCheck(hwndCtrl) == BST_CHECKED)
                    {
                        m_proposedValues.stayOnTop = true;
                    }
                    else
                    {
                        m_proposedValues.stayOnTop = false;
                    }

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_TRAY:
                {
                    if(Button_GetCheck(hwndCtrl) == BST_CHECKED)
                    {
                        m_proposedValues.liveInTray = true;
                    }
                    else
                    {
                        m_proposedValues.liveInTray = false;
                    }

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_RECLAIMFILETYPES:
                {
                    if(Button_GetCheck(hwndCtrl) == BST_CHECKED)
                    {
                        m_proposedValues.reclaimFiletypes = true;
                    }
                    else
                    {
                        m_proposedValues.reclaimFiletypes = false;
                    }

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_ASKRECLAIM:
                {
                    if(Button_GetCheck(hwndCtrl) == BST_CHECKED)
                    {
                        m_proposedValues.askReclaimFiletypes = true;
                    }
                    else
                    {
                        m_proposedValues.askReclaimFiletypes = false;
                    }

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }
                
            }

            break;
        }

        case WM_NOTIFY:
        {
            NMHDR* notify = (NMHDR*)lParam;

            switch(notify->code)
            {
                case PSN_HELP:
                {
                    ShowHelp(m_pContext, Preferences_General);
                    break;
                }
            
                case PSN_SETACTIVE:
                {
                    break;
                }

                case PSN_APPLY:
                {
                    SavePrefsValues(&m_proposedValues);
                    break;
                }

                case PSN_KILLACTIVE:
                {
                    break;
                }

                case PSN_RESET:
                {
                    SavePrefsValues(&m_originalValues);
                    break;
                }
            }

            break;
        }
    }

    return result;
}

bool Win32PreferenceWindow::PrefStreamingProc(HWND hwnd, 
                                              UINT msg, 
                                              WPARAM wParam, 
                                              LPARAM lParam)      
{
    bool result = false;
    static HWND hwndSaveStreams = NULL;
    static HWND hwndSaveStreamsDirectory = NULL;
    static HWND hwndBrowse = NULL;
    static HWND hwndSaveLocationText = NULL;
    static HWND hwndUseProxyServer = NULL;
    static HWND hwndProxyServerAddress = NULL;
    static HWND hwndProxyServerPort = NULL;
    static HWND hwndProxyServerAddressText = NULL;
    static HWND hwndProxyServerPortText = NULL;
    static HWND hwndColon = NULL;

    static HWND hwndUseAlternateIP = NULL;
    static HWND hwndUseAlternateIPText = NULL;
    static HWND hwndAlternateIPAddress1 = NULL;
    static HWND hwndAlternateIPAddress2 = NULL;
    static HWND hwndAlternateIPAddress3 = NULL;
    static HWND hwndAlternateIPAddress4 = NULL;
    static HWND hwndPeriod1 = NULL;
    static HWND hwndPeriod2 = NULL;
    static HWND hwndPeriod3 = NULL;
   
    switch(msg)
    {
        case WM_INITDIALOG:
        {
            // get the handles to all our controls

            hwndSaveStreams = GetDlgItem(hwnd, IDC_SAVESTREAMS);
            hwndSaveStreamsDirectory = GetDlgItem(hwnd, IDC_STREAMSAVEDIR);
            hwndBrowse = GetDlgItem(hwnd, IDC_BROWSE);
            hwndSaveLocationText = GetDlgItem(hwnd, IDC_SAVELOCATION_TEXT);

            hwndUseProxyServer = GetDlgItem(hwnd, IDC_USEPROXY);
            hwndProxyServerAddress = GetDlgItem(hwnd, IDC_PROXYADDRESS);
            hwndProxyServerAddressText = GetDlgItem(hwnd, IDC_PROXYADDRESS_TEXT);
            hwndProxyServerPort = GetDlgItem(hwnd, IDC_PORT);
            hwndProxyServerPortText = GetDlgItem(hwnd, IDC_PORT_TEXT);
            hwndColon = GetDlgItem(hwnd, IDC_COLON_TEXT);

            hwndUseAlternateIP = GetDlgItem(hwnd, IDC_USETHISIP);
            hwndUseAlternateIPText = GetDlgItem(hwnd, IDC_IPADDRESS_TEXT);
            hwndAlternateIPAddress1 = GetDlgItem(hwnd, IDC_IPADDRESS1);
            hwndAlternateIPAddress2 = GetDlgItem(hwnd, IDC_IPADDRESS2);
            hwndAlternateIPAddress3 = GetDlgItem(hwnd, IDC_IPADDRESS3);
            hwndAlternateIPAddress4 = GetDlgItem(hwnd, IDC_IPADDRESS4);
            hwndPeriod1 = GetDlgItem(hwnd, IDC_PERIOD1);
            hwndPeriod2 = GetDlgItem(hwnd, IDC_PERIOD2);
            hwndPeriod3 = GetDlgItem(hwnd, IDC_PERIOD3);
   

            // initialize our controls
            char temp[256];
            
            Button_SetCheck(hwndSaveStreams, m_originalValues.saveStreams);


            Edit_SetText(   hwndSaveStreamsDirectory, 
                            m_originalValues.saveStreamsDirectory.c_str());

            Button_Enable(  hwndSaveStreamsDirectory, 
                            m_originalValues.saveStreams);

            Button_Enable(  hwndBrowse, 
                            m_originalValues.saveStreams);

            Button_Enable(  hwndSaveLocationText,
                            m_originalValues.saveStreams);

            char* port = NULL;

            strcpy(temp, m_originalValues.proxyServer.c_str());
            port = strrchr(temp, ':');

            if(port)
            {
                *port = 0x00;
                port++;

                Edit_SetText(hwndProxyServerPort, 
                             port);

            }

            Edit_SetText(   hwndProxyServerAddress, 
                            temp);

            Edit_LimitText(hwndProxyServerPort, 5);


            Button_SetCheck(hwndUseProxyServer, m_originalValues.useProxyServer);


            Button_Enable(  hwndProxyServerAddress, 
                            m_originalValues.useProxyServer);

            Button_Enable(  hwndProxyServerAddressText, 
                            m_originalValues.useProxyServer);

            Button_Enable(  hwndProxyServerPort,
                            m_originalValues.useProxyServer);

            Button_Enable(  hwndProxyServerPortText, 
                            m_originalValues.useProxyServer);

            Button_Enable(  hwndColon, 
                            m_originalValues.useProxyServer);

            char ip[4][10];
			sscanf(m_originalValues.alternateIP.c_str(), "%[^.].%[^.].%[^.].%[^.]",
				   ip[0], ip[1], ip[2], ip[3]);
            
            Edit_SetText(hwndAlternateIPAddress1, ip[0]);
            Edit_SetText(hwndAlternateIPAddress2, ip[1]);
            Edit_SetText(hwndAlternateIPAddress3, ip[2]);
            Edit_SetText(hwndAlternateIPAddress4, ip[3]);

            Edit_LimitText(hwndAlternateIPAddress1, 3);
            Edit_LimitText(hwndAlternateIPAddress2, 3);
            Edit_LimitText(hwndAlternateIPAddress3, 3);
            Edit_LimitText(hwndAlternateIPAddress4, 3);

            Button_SetCheck(hwndUseAlternateIP, m_originalValues.useAlternateIP);


            Button_Enable(  hwndUseAlternateIPText, 
                            m_originalValues.useAlternateIP);

            Button_Enable(  hwndAlternateIPAddress1,
                            m_originalValues.useAlternateIP);

            Button_Enable(  hwndPeriod1, 
                            m_originalValues.useAlternateIP);

            Button_Enable(  hwndAlternateIPAddress2, 
                            m_originalValues.useAlternateIP);

            Button_Enable(  hwndPeriod2, 
                            m_originalValues.useAlternateIP);

            Button_Enable(  hwndAlternateIPAddress3, 
                            m_originalValues.useAlternateIP);

            Button_Enable(  hwndPeriod3, 
                            m_originalValues.useAlternateIP);

            Button_Enable(  hwndAlternateIPAddress4, 
                            m_originalValues.useAlternateIP);

            
            break;
        }

        case UWM_HELP:
        case WM_HELP:
        {
            ShowHelp(m_pContext, Preferences_Streaming);
            break;
        }

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDC_SAVESTREAMS:
                {
                    BOOL enabled;

                    if(Button_GetCheck(hwndSaveStreams) == BST_CHECKED)
                    {
                        m_proposedValues.saveStreams = true;
                    }
                    else
                    {
                        m_proposedValues.saveStreams = false;
                    }

                    enabled = (m_proposedValues.saveStreams ? TRUE : FALSE);

                    Button_Enable(hwndSaveStreamsDirectory, enabled); 
                    Button_Enable(hwndBrowse, enabled);
                    Button_Enable(hwndSaveLocationText,enabled);

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_STREAMSAVEDIR:
                {
                    if(HIWORD(wParam) == EN_CHANGE)
                    {
                        char temp[MAX_PATH];

                        Edit_GetText(   hwndSaveStreamsDirectory, 
                                        temp,
                                        MAX_PATH);

                        m_proposedValues.saveStreamsDirectory = temp;

                        if(m_proposedValues != m_currentValues)
                        {
                            PropSheet_Changed(GetParent(hwnd), hwnd);
                        }
                        else
                        {
                            PropSheet_UnChanged(GetParent(hwnd), hwnd);
                        }
                    }

                    break;
                }

                case IDC_BROWSE:
                {
                    LPMALLOC pMalloc;

                    if(SUCCEEDED(SHGetMalloc(&pMalloc)))
                    {
                        BROWSEINFO bi; 
                        LPITEMIDLIST browseId;
                        char displayName[MAX_PATH + 1];

                        bi.hwndOwner = hwnd;
                        bi.pidlRoot = NULL;
                        bi.pszDisplayName = displayName;
                        bi.lpszTitle = "Please select the folder to which you want to save streams.";
                        bi.ulFlags = BIF_RETURNONLYFSDIRS;
                        bi.lpfn = NULL;

                        browseId = SHBrowseForFolder(&bi);
            
                        if(browseId)
                        {
                            char temp[MAX_PATH];

                            SHGetPathFromIDList(browseId, temp);
                            
                            m_proposedValues.saveStreamsDirectory = temp;

                            Edit_SetText(hwndSaveStreamsDirectory, temp);

                            pMalloc->Free(browseId);
                        }
                    }
                
                    break;
                } 

                case IDC_USEPROXY:
                {
                    BOOL enabled;

                    if(Button_GetCheck(hwndUseProxyServer) == BST_CHECKED)
                    {
                        m_proposedValues.useProxyServer = true;
                    }
                    else
                    {
                        m_proposedValues.useProxyServer = false;
                    }

                    enabled = (m_proposedValues.useProxyServer ? TRUE : FALSE);

                    Button_Enable(hwndProxyServerAddress, enabled);

                    Button_Enable(hwndProxyServerAddressText, enabled);

                    Button_Enable(hwndProxyServerPort, enabled);

                    Button_Enable(hwndProxyServerPortText, enabled);

                    Button_Enable(hwndColon, enabled);


                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_PROXYADDRESS:
                case IDC_PORT:
                {
                    if(HIWORD(wParam) == EN_CHANGE)
                    {
                        char temp[MAX_PATH];
                        char port[6];

                        Edit_GetText( hwndProxyServerAddress, 
                                      temp,
                                      MAX_PATH);

                        m_proposedValues.proxyServer = temp;


                        Edit_GetText( hwndProxyServerPort, 
                                      port,
                                      sizeof(port));

                        if(*port)
                        {
                            m_proposedValues.proxyServer += ":";
                            m_proposedValues.proxyServer += port;
                        }

                        if(m_proposedValues != m_currentValues)
                        {
                            PropSheet_Changed(GetParent(hwnd), hwnd);
                        }
                        else
                        {
                            PropSheet_UnChanged(GetParent(hwnd), hwnd);
                        }
                    }

                    break;
                }

                case IDC_USETHISIP:
                {
                    BOOL enabled;

                    if(Button_GetCheck(hwndUseAlternateIP) == BST_CHECKED)
                    {
                        m_proposedValues.useAlternateIP = true;
                    }
                    else
                    {
                        m_proposedValues.useAlternateIP = false;
                    }

                    enabled = (m_proposedValues.useAlternateIP ? TRUE : FALSE);

                    Button_Enable(hwndUseAlternateIPText, enabled);
                    Button_Enable(hwndAlternateIPAddress1, enabled);
                    Button_Enable(hwndPeriod1, enabled);
                    Button_Enable(hwndAlternateIPAddress2, enabled);
                    Button_Enable(hwndPeriod2, enabled);
                    Button_Enable(hwndAlternateIPAddress3, enabled);
                    Button_Enable(hwndPeriod3, enabled);
                    Button_Enable(hwndAlternateIPAddress4, enabled);
                    


                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_IPADDRESS1:
                case IDC_IPADDRESS2:
                case IDC_IPADDRESS3:
                case IDC_IPADDRESS4:
                {
                    if(HIWORD(wParam) == EN_CHANGE)
                    {
                        char ip[4];

                        Edit_GetText(hwndAlternateIPAddress1, ip, 4);

                        if(*ip)
                        {
                            m_proposedValues.alternateIP = ip;
                        }
                        else
                        {
                            m_proposedValues.alternateIP = "0";
                        }

                        m_proposedValues.alternateIP += ".";

                        Edit_GetText(hwndAlternateIPAddress2, ip, 4);

                        if(*ip)
                        {
                            m_proposedValues.alternateIP += ip;
                        }
                        else
                        {
                            m_proposedValues.alternateIP += "0";
                        }

                        m_proposedValues.alternateIP += ".";

                        Edit_GetText(hwndAlternateIPAddress3, ip, 4);

                        if(*ip)
                        {
                            m_proposedValues.alternateIP += ip;
                        }
                        else
                        {
                            m_proposedValues.alternateIP += "0";
                        }

                        m_proposedValues.alternateIP += ".";

                        Edit_GetText(hwndAlternateIPAddress4, ip, 4);

                        if(*ip)
                        {
                            m_proposedValues.alternateIP += ip;
                        }
                        else
                        {
                           m_proposedValues.alternateIP += "0";
                        }

                        if(m_proposedValues != m_currentValues)
                        {
                            PropSheet_Changed(GetParent(hwnd), hwnd);
                        }
                        else
                        {
                            PropSheet_UnChanged(GetParent(hwnd), hwnd);
                        }
                    }

                    break;
                }

            }

            break;
        }


        case WM_NOTIFY:
        {
            NMHDR* notify = (NMHDR*)lParam;

            switch(notify->code)
            {
                case PSN_HELP:
                {
                    ShowHelp(m_pContext, Preferences_Streaming);
                    break;
                }
                case PSN_SETACTIVE:
                {
                    
                    break;
                }

                case PSN_APPLY:
                {
                    SavePrefsValues(&m_proposedValues);
                    break;
                }

                case PSN_KILLACTIVE:
                {
                    
                    break;
                }

                case PSN_RESET:
                {
                    SavePrefsValues(&m_originalValues);
                    break;
                }
            }

            break;
        }
    }

    return result;
}

bool Win32PreferenceWindow::PrefDebugProc(HWND hwnd, 
                                          UINT msg, 
                                          WPARAM wParam, 
                                          LPARAM lParam)      
{
    bool result = false;
    static HWND hwndLog = NULL;
    static HWND hwndLogDecoder = NULL;
    static HWND hwndLogInput = NULL;
    static HWND hwndLogOutput = NULL;
    static HWND hwndLogMain = NULL;
    static HWND hwndLogPerformance = NULL;
    
    switch(msg)
    {
        case WM_INITDIALOG:
        {
            // get the handles to all our controls
            hwndLog = GetDlgItem(hwnd, IDC_LOG);
            hwndLogDecoder = GetDlgItem(hwnd, IDC_LOGDECODER);
            hwndLogInput = GetDlgItem(hwnd, IDC_LOGINPUT);
            hwndLogOutput = GetDlgItem(hwnd, IDC_LOGOUTPUT);
            hwndLogMain = GetDlgItem(hwnd, IDC_LOGMAIN);
            hwndLogPerformance = GetDlgItem(hwnd, IDC_LOGPERFORMANCE);

            // initialize our controls
            bool value;

            value = m_originalValues.enableLogging;

            Button_SetCheck(hwndLog, value); 
            Button_Enable(hwndLogDecoder, value); 
            Button_Enable(hwndLogInput, value);
            Button_Enable(hwndLogOutput, value);
            Button_Enable(hwndLogMain, value);
            Button_Enable(hwndLogPerformance, value);

            Button_SetCheck(hwndLogMain, m_originalValues.logMain); 
            Button_SetCheck(hwndLogDecoder, m_originalValues.logDecoder); 
            Button_SetCheck(hwndLogInput, m_originalValues.logInput); 
            Button_SetCheck(hwndLogOutput, m_originalValues.logOutput); 
            Button_SetCheck(hwndLogPerformance, m_originalValues.logPerformance); 
                     
            break;
        }

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDC_LOG:
                {
                    BOOL enabled;

                    if(Button_GetCheck(hwndLog) == BST_CHECKED)
                    {
                        m_proposedValues.enableLogging = true;
                       
                    }
                    else
                    {
                        m_proposedValues.enableLogging = false;
                    }

                    enabled = (m_proposedValues.enableLogging ? TRUE : FALSE);

                    Button_Enable(hwndLogDecoder, enabled); 
                    Button_Enable(hwndLogInput, enabled);
                    Button_Enable(hwndLogOutput, enabled);
                    Button_Enable(hwndLogMain, enabled);
                    Button_Enable(hwndLogPerformance, enabled);

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_LOGDECODER:
                {
                    if(Button_GetCheck(hwndLogDecoder) == BST_CHECKED)
                    {
                        m_proposedValues.logDecoder = true;
                    }
                    else
                    {
                        m_proposedValues.logDecoder = false;
                    }

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_LOGINPUT:
                {
                    if(Button_GetCheck(hwndLogInput) == BST_CHECKED)
                    {
                        m_proposedValues.logInput = true;
                    }
                    else
                    {
                        m_proposedValues.logInput = false;
                    }

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_LOGOUTPUT:
                {
                    if(Button_GetCheck(hwndLogOutput) == BST_CHECKED)
                    {
                        m_proposedValues.logOutput = true;
                    }
                    else
                    {
                        m_proposedValues.logOutput = false;
                    }

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_LOGPERFORMANCE:
                {
                    if(Button_GetCheck(hwndLogPerformance) == BST_CHECKED)
                    {
                        m_proposedValues.logPerformance = true;
                    }
                    else
                    {
                        m_proposedValues.logPerformance = false;
                    }

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_LOGMAIN:
                {
                    if(Button_GetCheck(hwndLogMain) == BST_CHECKED)
                    {
                        m_proposedValues.logMain = true;
                    }
                    else
                    {
                        m_proposedValues.logMain = false;
                    }

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }
            }

            break;
        }

        case WM_NOTIFY:
        {
            NMHDR* notify = (NMHDR*)lParam;

            switch(notify->code)
            {
                case PSN_SETACTIVE:
                {
                    
                    break;
                }

                case PSN_APPLY:
                {
                    SavePrefsValues(&m_proposedValues);
                    break;
                }

                case PSN_KILLACTIVE:
                {
                    
                    break;
                }

                case PSN_RESET:
                {
                    SavePrefsValues(&m_originalValues);
                    break;
                }
            }

            break;
        }
    }

    return result;
}

bool Win32PreferenceWindow::PrefAboutProc(HWND hwnd, 
                                          UINT msg, 
                                          WPARAM wParam, 
                                          LPARAM lParam)      
{
    bool result = false;
    
    switch(msg)
    {
        case WM_INITDIALOG:
        {
            break;
        }

        case WM_DRAWITEM:
        {
            DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
            UINT ctrlId = wParam;
            HFONT font, oldFont;

            font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

            oldFont = (HFONT)SelectObject(dis->hDC, font);

            RECT clientRect;
            GetClientRect(dis->hwndItem, &clientRect);

            switch(ctrlId)
            {
                case IDC_APP:
                {
                    DrawText(dis->hDC, 
                             BRANDING,
                             strlen(BRANDING),
                             &clientRect,
                             DT_CENTER|DT_SINGLELINE);
                    break;
                }

                case IDC_VERSION:
                {
                    char version[32];

                    sprintf(version, "version %s", BRANDING_VERSION);

                    DrawText(dis->hDC, 
                             version,
                             strlen(version),
                             &clientRect,
                             DT_CENTER|DT_SINGLELINE);
                    break;
                }

                case IDC_BASED_ON_FREEAMP:
                {
                    if(strcmp(BRANDING, "FreeAmp"))
                    {
                        const char* text = "(based on FreeA*p)";

                        DrawText(dis->hDC, 
                             text,
                             strlen(text),
                             &clientRect,
                             DT_CENTER|DT_SINGLELINE);
                    }

                    break;
                }

                case IDC_CREDIT:
                {
                    const char* credit1 =
                        "Zinf is an Open Source effort to build the most useful "
                        "digital audio player around. In the interest of "
                        "supporting the free software community, while at "
                        "the same time fostering the growth of the online "
                        "delivery of music, EMusic.com has funded both the "
                        "FreeA*p.org domain and the efforts of the FreeA*p "
                        "team. The FreeA*p team consisted of: Mark B. Elrod, "
                        "Robert Kaye, Isaac Richards, Brett Thomas, and "
                        "Jason Woodward.";
                    const char* credit2 =
                        "Other people have also contributed to FreeA*p:";
                    /*const char* credit3 =
                        "Brian Almeida, Stephan Auerhahn, William Bull, Jimen Ching, "
                        "Alan Cutter, Gabor Fleischer, Skip Hansen, Reece Hart, "
                        "Jean-Michel HERVE, Henrik Johnson, Hiromasa Kato, Harald Klein, "
                        "Anton Kruger, Chad Loder, Michael Bruun "
                        "Petersen, Daniel Pustka, Sylvain Rebaud, The Snowblind Alliance, "
                        "Tom Spindler, Chen Su, Jean Tourrilhes, Valters Vingolds, "
                        "and Mark Weaver.";*/
                    const char* credit3 =
                        "William Bull, Alan Cutter, Gabor Fleischer, "
                        "Jean-Michel HERVE, Hiromasa Kato, Michael Bruun "
                        "Petersen, Sylvain Rebaud, The Snowblind Alliance, "
                        "Tom Spindler,Valters Vingolds, and Mark Weaver.";
                    const char* credit4 =
                        "Zinf is being released under the terms of the "
                        "GPL. Zinf is looking for some talented programmers "
						"who are interested in continuing to work on Zinf. "
						"Please check the zinf.org website for more details.";

                    RECT halfHeightRect = clientRect;
                    int halfHeight = DrawText(
                                         dis->hDC, 
                                         credit1,
                                         strlen(credit1),
                                         &halfHeightRect,
                                         DT_LEFT|DT_SINGLELINE|DT_CALCRECT)/3;
                    int height;

                    height = DrawText(
                             dis->hDC, 
                             credit1,
                             strlen(credit1),
                             &clientRect,
                             DT_LEFT|DT_WORDBREAK);

                    clientRect.top += height + halfHeight;

                    height = DrawText(
                             dis->hDC, 
                             credit2,
                             strlen(credit2),
                             &clientRect,
                             DT_LEFT|DT_WORDBREAK);

                    clientRect.top += height + halfHeight;

                    height = DrawText(
                             dis->hDC, 
                             credit3,
                             strlen(credit3),
                             &clientRect,
                             DT_LEFT|DT_WORDBREAK);

                    clientRect.top += height + halfHeight;

                    height = DrawText(
                             dis->hDC, 
                             credit4,
                             strlen(credit4),
                             &clientRect,
                             DT_LEFT|DT_WORDBREAK);

                    break;
                }
            }

            SelectObject(dis->hDC, oldFont);

            DeleteObject(font);
        }

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDC_GOTOFREEAMP:
                {
                    ShellExecute(   hwnd, 
                                    "open", 
                                    "http://www.freeamp.org/", 
                                    NULL, 
                                    NULL, 
                                    SW_SHOWNORMAL);
                    break;
                }

                case IDC_GOTOEMUSIC:
                {
                    ShellExecute(   hwnd, 
                                    "open", 
                                    "http://www.emusic.com/", 
                                    NULL, 
                                    NULL, 
                                    SW_SHOWNORMAL);
                    break;
                }
				case IDC_GOTORELATABLE:
				{
					ShellExecute(	hwnd,
									"open",
									"http://www.relatable.com/",
									NULL,
									NULL,
									SW_SHOWNORMAL);
					break;
				}
            }

            break;
        }

        case UWM_HELP:
        case WM_HELP:
        {
            ShowHelp(m_pContext, Preferences_About);
            break;
        }

        case WM_NOTIFY:
        {
            NMHDR* notify = (NMHDR*)lParam;

            switch(notify->code)
            {
                case PSN_HELP:
                {
                    ShowHelp(m_pContext, Preferences_About);
                    break;
                }
                case PSN_SETACTIVE:
                {
                    
                    break;
                }

                case PSN_APPLY:
                {
                    SavePrefsValues(&m_proposedValues);
                    break;
                }

                case PSN_KILLACTIVE:
                {
                    
                    break;
                }

                case PSN_RESET:
                {
                    SavePrefsValues(&m_originalValues);
                    break;
                }
            }

            break;
        }
    }

    return result;

}

bool Win32PreferenceWindow::PrefDirectoryProc(HWND hwnd, 
                                              UINT msg, 
                                              WPARAM wParam, 
                                              LPARAM lParam)      
{
    bool result = false;
    static HWND hwndSaveMusicDir = NULL;
    static HWND hwndWatchDirs = NULL;
    static HWND hwndList = NULL;
    static HWND hwndAddDir = NULL;
    static HWND hwndRemoveDir = NULL;
    
    switch(msg)
    {
        case WM_INITDIALOG:
        {
            hwndSaveMusicDir = GetDlgItem(hwnd, IDC_SAVEMUSICDIR);
            hwndWatchDirs = GetDlgItem(hwnd, IDC_WATCHDIRS);
            hwndList = GetDlgItem(hwnd, IDC_DIRLIST);
            hwndAddDir = GetDlgItem(hwnd, IDC_ADDDIR);
            hwndRemoveDir = GetDlgItem(hwnd, IDC_REMOVEDIR);

            Edit_SetText(hwndSaveMusicDir, 
                         m_originalValues.saveMusicDirectory.c_str());

            HINSTANCE hinst = (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE);

            // get our folder image
            HIMAGELIST imageList = ImageList_Create(16, 13, ILC_COLOR24|ILC_MASK, 2, 0);

            HBITMAP bmp;
            bmp = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_FOLDER));
            ImageList_AddMasked(imageList, bmp, RGB(255,0,0));
            DeleteObject(bmp);

            ListView_SetImageList(hwndList, imageList, LVSIL_SMALL);             

            // add our columns
            RECT rect;
            GetClientRect(hwndList, &rect);

            LV_COLUMN lvc;

            lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM;
            lvc.fmt = LVCFMT_LEFT; // left align column
            lvc.cx = (rect.right-rect.left); // width of column in pixels
            lvc.iSubItem = 0;

            ListView_InsertColumn(hwndList, 0, &lvc);

            // add items to dir list
            LV_ITEM lv_item;
            HDC hdc = GetDC(hwndList);
            
            lv_item.mask = LVIF_PARAM | LVIF_STATE | LVIF_TEXT;
            lv_item.state = 0;
            lv_item.iSubItem = 0;

            set<string>::const_iterator i;

            for(i = m_originalValues.watchDirectories.begin(); 
                i != m_originalValues.watchDirectories.end(); 
                i++)
            {
                SIZE sizeString;

                GetTextExtentPoint32(hdc, (*i).c_str(), (*i).size(), &sizeString);

                if(ListView_GetColumnWidth(hwndList, 0) < sizeString.cx)
                    ListView_SetColumnWidth(hwndList, 0, sizeString.cx);

                lv_item.pszText = (char*)(*i).c_str();
                lv_item.cchTextMax = strlen(lv_item.pszText);
                lv_item.iItem = ListView_GetItemCount(hwndList);
                lv_item.lParam = (LPARAM) new string(*i);
                lv_item.iSubItem = 0;

                ListView_InsertItem(hwndList, &lv_item);
            }

            ReleaseDC(hwndList, hdc);

            Button_SetCheck(hwndWatchDirs, m_originalValues.watchForNewMusic);
            Button_Enable(hwndList, m_originalValues.watchForNewMusic);
            Button_Enable(hwndAddDir, m_originalValues.watchForNewMusic);
            Button_Enable(hwndRemoveDir, m_originalValues.watchForNewMusic);

            break;
        }

        case WM_DESTROY:
        {
            uint32 count = ListView_GetItemCount(hwndList);
            
            for(uint32 i = 0; i < count; i++)
            {
                LV_ITEM item;

                item.mask = LVIF_PARAM;
                item.iItem = i;
                item.lParam = 0;

                if(ListView_GetItem(hwndList, &item))
                {
                    if(item.lParam)
                    {
                        delete (string*)item.lParam;
                        break;
                    }
                }
            }

            break;
        }

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDC_SAVEMUSICDIR:
                {
                    if(HIWORD(wParam) == EN_CHANGE)
                    {
                        char temp[MAX_PATH];

                        Edit_GetText(   hwndSaveMusicDir, 
                                        temp,
                                        MAX_PATH);

                        m_proposedValues.saveMusicDirectory = temp;

                        if(m_proposedValues != m_currentValues)
                        {
                            PropSheet_Changed(GetParent(hwnd), hwnd);
                        }
                        else
                        {
                            PropSheet_UnChanged(GetParent(hwnd), hwnd);
                        }
                    }

                    break;
                }

                case IDC_BROWSE:
                {
                    LPMALLOC pMalloc;

                    if(SUCCEEDED(SHGetMalloc(&pMalloc)))
                    {
                        BROWSEINFO bi; 
                        LPITEMIDLIST browseId;
                        char displayName[MAX_PATH + 1];

                        bi.hwndOwner = hwnd;
                        bi.pidlRoot = NULL;
                        bi.pszDisplayName = displayName;
                        bi.lpszTitle = "Please select the folder to which you want to save music.";
                        bi.ulFlags = BIF_RETURNONLYFSDIRS;
                        bi.lpfn = NULL;

                        browseId = SHBrowseForFolder(&bi);
            
                        if(browseId)
                        {
                            char temp[MAX_PATH];

                            SHGetPathFromIDList(browseId, temp);
                            
                            m_proposedValues.saveMusicDirectory = temp;

                            Edit_SetText(hwndSaveMusicDir, temp);

                            pMalloc->Free(browseId);
                        }
                    }
                
                    break;
                }
                
                case IDC_WATCHDIRS:
                {
                    BOOL enabled;

                    if(Button_GetCheck(hwndWatchDirs) == BST_CHECKED)
                    {
                        m_proposedValues.watchForNewMusic = true;
                    }
                    else
                    {
                        m_proposedValues.watchForNewMusic = false;
                    }

                    enabled = (m_proposedValues.watchForNewMusic ? TRUE : FALSE);

                    Button_Enable(hwndList, enabled); 
                    Button_Enable(hwndAddDir, enabled);
                    Button_Enable(hwndRemoveDir,enabled);

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_REMOVEDIR:
                {
                    uint32 index = 0;
                    uint32 count = ListView_GetItemCount(hwndList);

                    for(index = 0; index < count; index++)
                    {
                        LV_ITEM item;

                        item.mask = LVIF_PARAM|LVIF_STATE;
                        item.stateMask = LVIS_SELECTED;
                        item.iItem = index;
                        item.iSubItem = 0;
                        item.lParam = 0;
                        item.state = 0;

                        if(ListView_GetItem(hwndList, &item))
                        {
                            if(item.state & LVIS_SELECTED && item.lParam)
                            {
                                string* s = (string*)item.lParam;
                                set<string>::iterator i;

                                i = m_proposedValues.watchDirectories.find(*s);

                                if(i != m_proposedValues.watchDirectories.end())
                                {
                                    m_proposedValues.watchDirectories.erase(i);
                                }

                                delete s;

                                ListView_DeleteItem(hwndList, index);
                                //break;
                            }
                        }  
                    }

                    if(m_proposedValues != m_currentValues)
                    {
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    }
                    else
                    {
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                    }

                    break;
                }

                case IDC_ADDDIR:
                {
                    LPMALLOC pMalloc;

                    if(SUCCEEDED(SHGetMalloc(&pMalloc)))
                    {
                        BROWSEINFO bi; 
                        LPITEMIDLIST browseId;
                        char displayName[MAX_PATH + 1];

                        bi.hwndOwner = hwnd;
                        bi.pidlRoot = NULL;
                        bi.pszDisplayName = displayName;
                        bi.lpszTitle = "Please select a folder you wish to watch for music.";
                        bi.ulFlags = BIF_RETURNONLYFSDIRS;
                        bi.lpfn = NULL;

                        browseId = SHBrowseForFolder(&bi);
            
                        if(browseId)
                        {
                            char temp[MAX_PATH];

                            SHGetPathFromIDList(browseId, temp);
                            
                            m_proposedValues.watchDirectories.insert(string(temp));

                            // add items to dir list
                            LV_ITEM lv_item;
                            HDC hdc = GetDC(hwndList);
            
                            lv_item.mask = LVIF_PARAM | LVIF_STATE | LVIF_TEXT;
                            lv_item.state = 0;
                            lv_item.iSubItem = 0;
                            lv_item.lParam = (LPARAM) new string(temp);
                   
                            SIZE sizeString;

                            GetTextExtentPoint32(hdc, temp, strlen(temp), &sizeString);

                            if(ListView_GetColumnWidth(hwndList, 0) < sizeString.cx)
                                ListView_SetColumnWidth(hwndList, 0, sizeString.cx);

                            lv_item.pszText = temp;
                            lv_item.cchTextMax = strlen(lv_item.pszText);
                            lv_item.iItem = ListView_GetItemCount(hwndList);

                            ListView_InsertItem(hwndList, &lv_item);

                            ReleaseDC(hwndList, hdc);

                            pMalloc->Free(browseId);
                        }

                        if(m_proposedValues != m_currentValues)
                        {
                            PropSheet_Changed(GetParent(hwnd), hwnd);
                        }
                        else
                        {
                            PropSheet_UnChanged(GetParent(hwnd), hwnd);
                        }
                    }
                
                    break;
                }
            }

            break;
        }

        case WM_HELP:
        case UWM_HELP:
        {
            ShowHelp(m_pContext, Preferences_Directory);
            break;
        }

        case WM_NOTIFY:
        {
            NMHDR* notify = (NMHDR*)lParam;

            switch(notify->code)
            {
                case PSN_HELP:
                {
                    ShowHelp(m_pContext, Preferences_Directory);
                    break;
                }
                case PSN_SETACTIVE:
                {
                    
                    break;
                }

                case PSN_APPLY:
                {
                    SavePrefsValues(&m_proposedValues);
                    break;
                }

                case PSN_KILLACTIVE:
                {
                    
                    break;
                }

                case PSN_RESET:
                {
                    SavePrefsValues(&m_originalValues);
                    break;
                }
            }

            break;
        }
    }

    return result;
}

void Win32PreferenceWindow::LoadThemeListBox(HWND hwnd)
{
    map<string, string>::iterator i;

	//m_pThemeMan->GetCurrentTheme(m_proposedValues.currentTheme);

    m_oThemeList.clear();    

    SendDlgItemMessage(hwnd, IDC_THEMELISTBOX, LB_RESETCONTENT, 0, 0);

    m_pThemeMan->GetThemeList(m_oThemeList);

    for(i = m_oThemeList.begin(); i != m_oThemeList.end(); i++)
    {
        int index;

    	index = SendDlgItemMessage(hwnd, IDC_THEMELISTBOX, LB_ADDSTRING,
                           0, (LPARAM)(*i).first.c_str());

        if ((*i).first == m_proposedValues.currentTheme) 
            SendDlgItemMessage(hwnd, IDC_THEMELISTBOX, LB_SETCURSEL, index, 0); 
		else {
			char *temps = new char[(*i).second.size() + 1];
			strcpy(temps, (*i).second.c_str());
			char *name = strrchr(temps, '.');
			if (name)  
				*name = '\0';
			name = strrchr(temps, '\\'); 
			if (name) {
				name++;
				if (name && *name) {
					if (!strcmp(name, m_proposedValues.currentTheme.c_str())) 
						SendDlgItemMessage(hwnd, IDC_THEMELISTBOX, LB_SETCURSEL, index, 0);
				}
			}

			delete [] temps;
		}
    }                      
     
    EnableWindow(GetDlgItem(hwnd, IDC_DELETETHEME), 0);
}

bool Win32PreferenceWindow::PrefThemeProc(HWND hwnd, 
                                          UINT msg, 
                                          WPARAM wParam, 
                                          LPARAM lParam)      
{
    bool result = false;
    
    switch(msg)
    {
        case WM_INITDIALOG:
        {
            LoadThemeListBox(hwnd);

            //SetFocus(GetDlgItem(hwnd, IDC_THEMELISTBOX));
            result = false;
            break;
        }

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
				case IDC_THEMELISTBOX:
                {
                    switch (HIWORD(wParam)) 
                    { 
                        case LBN_SELCHANGE: 
                        {
                	        int    iIndex;
                            char   szCurSel[256];
                    
                            iIndex = SendDlgItemMessage(hwnd, IDC_THEMELISTBOX, 
                                                            LB_GETCURSEL, 0, 0);

                            if(iIndex >= 0)
                            {
                                SendDlgItemMessage(hwnd, IDC_THEMELISTBOX, 
                                                   LB_GETTEXT, iIndex, 
                                                   (LPARAM)szCurSel);
                                if (strcasecmp(szCurSel, "freeamp"))                   
                                   EnableWindow(GetDlgItem(hwnd, IDC_DELETETHEME), 1);
                                else   
                                   EnableWindow(GetDlgItem(hwnd, IDC_DELETETHEME), 0);

                                m_proposedValues.currentTheme = string(szCurSel);

                                if(m_proposedValues != m_currentValues)
                                {
                                    PropSheet_Changed(GetParent(hwnd), hwnd);
                                }
                                else
                                {
                                    PropSheet_UnChanged(GetParent(hwnd), hwnd);
                                }
                            }  
                            
                            break;
                        }
                    }
                
                	break;
                }
                case IDC_ADDTHEME:
                {
                    if(GetKeyState(VK_CONTROL) < 0)
                    {
                        LPMALLOC pMalloc;

                        if(SUCCEEDED(SHGetMalloc(&pMalloc)))
                        {
                            BROWSEINFO bi; 
                            LPITEMIDLIST browseId;
                            char displayName[MAX_PATH + 1];

                            bi.hwndOwner = hwnd;
                            bi.pidlRoot = NULL;
                            bi.pszDisplayName = displayName;
                            bi.lpszTitle = "Please select the folder which contains your theme.";
                            bi.ulFlags = BIF_RETURNONLYFSDIRS;
                            bi.lpfn = NULL;

                            browseId = SHBrowseForFolder(&bi);
            
                            if(browseId)
                            {
                                char temp[MAX_PATH];

                                SHGetPathFromIDList(browseId, temp);

                                string oThemeName(temp);
                                Error  eRet;
                        
                                eRet = m_pThemeMan->AddTheme(oThemeName);
                                if(IsError(eRet))
                                    MessageBox(hwnd, "Could not add theme.", "Add Theme", MB_OK);
                                else    
                                    LoadThemeListBox(hwnd);
                            
                                pMalloc->Free(browseId);
                            }
                        }
                    }
                    else
                    {
                	    OPENFILENAME sOpen;
                        char szFile[256];
                    
                        szFile[0] = 0;
                        sOpen.lStructSize = sizeof(OPENFILENAME);
                        sOpen.hwndOwner = hwnd;
                        sOpen.hInstance = NULL;
                        sOpen.lpstrFilter = kThemeFileFilter;
                        sOpen.lpstrCustomFilter = NULL;
                        sOpen.nMaxCustFilter = 0;
                        sOpen.nFilterIndex = 1;
                        sOpen.lpstrFile = szFile;
                        sOpen.nMaxFile = 256;
                        sOpen.lpstrFileTitle = NULL;
                        sOpen.lpstrInitialDir = NULL;
                        sOpen.lpstrTitle = "Open Theme";
                        sOpen.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                        sOpen.lpstrDefExt = "fat";
                      
                	    if (GetOpenFileName(&sOpen))
                        {
                            string       oThemeName(sOpen.lpstrFile);
                            Error        eRet;
                        
                            eRet = m_pThemeMan->AddTheme(oThemeName);
                            if (IsError(eRet))
                                MessageBox(hwnd, "Could not add theme.", "Add Theme", MB_OK);
                            else    
                                LoadThemeListBox(hwnd);
                        }
                    }
                    
                    break;
                }

                case IDC_DELETETHEME:
                {
                    int          iIndex;
                    string       oMesg, oThemeName;
                    Error        eRet;
                    char         szCurSel[256];
                    
                    
                    iIndex = SendDlgItemMessage(hwnd, IDC_THEMELISTBOX, LB_GETCURSEL, 0, 0);
                    SendDlgItemMessage(hwnd, IDC_THEMELISTBOX, 
                                       LB_GETTEXT, iIndex, 
                                       (LPARAM)szCurSel);
                    
                    oThemeName = szCurSel;
                    oMesg = string("Are you sure you want to delete theme ") +
                            oThemeName + string("?");
                    if(MessageBox(hwnd, oMesg.c_str(), "Confirm Delete", MB_YESNO) == IDYES)        
                    {
                        eRet = m_pThemeMan->DeleteTheme(m_oThemeList[oThemeName]);
                        if(IsError(eRet))
                            MessageBox(hwnd, "Could not delete theme.", "Delete Theme", MB_OK);
                        LoadThemeListBox(hwnd);
                    }    
                    
                    break;
                }

                case IDC_CHOOSEFONT:
                {
                	CHOOSEFONT sFont;
                    LOGFONT    sLogFont;
                    
                    memset(&sLogFont, 0, sizeof(sLogFont));
                    
                    strcpy(sLogFont.lfFaceName, m_proposedValues.defaultFont.c_str());
                    sLogFont.lfWeight = FW_NORMAL;
                    sLogFont.lfHeight = -24;
                    
                    sFont.lStructSize = sizeof(sFont);
                    sFont.hwndOwner = hwnd;
                    sFont.hDC = NULL;
                    sFont.lpLogFont = &sLogFont;
                    sFont.iPointSize = 18;
                    sFont.Flags = CF_TTONLY | CF_FORCEFONTEXIST |
                                  CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT |
                                  CF_LIMITSIZE;
                    sFont.nSizeMin = 18;              
                    sFont.nSizeMax = 18;              
                    
                    if(ChooseFont(&sFont))
                    {
                    	m_proposedValues.defaultFont= sLogFont.lfFaceName;

                        if(m_proposedValues != m_currentValues)
                        {
                            PropSheet_Changed(GetParent(hwnd), hwnd);
                        }
                        else
                        {
                            PropSheet_UnChanged(GetParent(hwnd), hwnd);
                        }
                    }    
                    
                    break;
                }
            }

            break;
        }

        case WM_HELP:
        case UWM_HELP:
        {
            ShowHelp(m_pContext, Preferences_Themes);
            break;
        }

        case WM_NOTIFY:
        {
            NMHDR* notify = (NMHDR*)lParam;

            switch(notify->code)
            {
                case PSN_HELP:
                {
                    ShowHelp(m_pContext, Preferences_Themes);
                    break;
                }
                case PSN_SETACTIVE:
                {
                    
                    break;
                }

                case PSN_APPLY:
                {
                    SavePrefsValues(&m_proposedValues);
                    LoadThemeListBox(hwnd);
                    break;
                }

                case PSN_KILLACTIVE:
                {
                    
                    break;
                }

                case PSN_RESET:
                {
                    SavePrefsValues(&m_originalValues);                         
                    break;
                }
            }

            break;
        }
    }

    return result;
}

uint32 CalcStringEllipsis(HDC hdc, string& displayString, int32 columnWidth)
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

bool Win32PreferenceWindow::PrefBrowserProc(HWND hwnd, 
                                           UINT msg, 
                                           WPARAM wParam, 
                                           LPARAM lParam)      
{
    bool result = false;
    switch(msg)
    {
        case WM_INITDIALOG:
        {
            //
            //  Add all the columns that are currently in use to the
            // rightmost listbox.
            //
            string used;
			char *columns = new char[m_originalValues.playlistHeaderColumns.size() + 2];
			strcpy(columns, m_originalValues.playlistHeaderColumns.c_str());
            char *token = strtok( columns, "|" );
            while( token != NULL )
            {
                SendDlgItemMessage(hwnd, IDC_CURRENT_COLUMN_LIST, LB_ADDSTRING,
                                   0, (LPARAM)token);
                      
                used += token;
                token = strtok( NULL, "|" );

            }

            //
            //  Now add all those columns that aren't included to the 
            // "Available" listbox.
            //
            char *available [] = {  "Artist",
                                    "Album",
                                    "Comment",
                                    "Genre",
                                    "Location",
                                    "Time",
                                    "Title",
                                    "Track",
                                    "Year"
                                  };

            int nCols = sizeof( available ) / sizeof( available[ 0 ] );
            for( int i = 0; i < nCols; i++ )
            {
                if ( !strstr( used.c_str(), available[ i ] ) )
                {
                    SendDlgItemMessage(hwnd, IDC_AVAILABLE_COLUMN_LIST, LB_ADDSTRING,
                                       0, (LPARAM)available[ i ] );
                }
            }

			delete [] columns;
            break;
        }

        case WM_COMMAND:
        {
            HWND hwndCtrl = (HWND) lParam;

            switch(LOWORD(wParam))
            {
                case IDC_REMOVE_COLUMN:
                {
                    int selIndex = SendDlgItemMessage( hwnd, IDC_CURRENT_COLUMN_LIST, LB_GETCURSEL, 0, 0 );

                    if ( selIndex == LB_ERR )
                    {
                        MessageBox( hwnd, "No item selected for removing.", "Error", MB_OK );
                        break;
                    }
                                      
                    char selectedText[ 100];

                    // Get the text of the item to remove.
                    SendDlgItemMessage( hwnd, IDC_CURRENT_COLUMN_LIST, LB_GETTEXT, selIndex, (LPARAM)selectedText );
                    // Add to end of the available list
                    SendDlgItemMessage(hwnd, IDC_AVAILABLE_COLUMN_LIST, LB_ADDSTRING,
                                       0, (LPARAM)selectedText);
                    // Remove the item.
                    SendDlgItemMessage( hwnd, IDC_CURRENT_COLUMN_LIST, LB_DELETESTRING, selIndex, 0);

                    int nCols      = SendDlgItemMessage( hwnd, IDC_CURRENT_COLUMN_LIST, LB_GETCOUNT, 0, 0 );
                    string columns = "";
                    for( int i = 0; i < nCols; i++ )
                    {
                         SendDlgItemMessage( hwnd, IDC_CURRENT_COLUMN_LIST, LB_GETTEXT, i, (LPARAM)selectedText );
                         columns += selectedText;
                         if ( i != (nCols -1 ))
                         columns += "|";
                    }
                    m_proposedValues.playlistHeaderColumns = columns;
                    break;
                }
                case IDC_ADD_COLUMN:
                {
                    int selIndex = SendDlgItemMessage( hwnd, IDC_AVAILABLE_COLUMN_LIST, LB_GETCURSEL, 0, 0 );
                    if ( selIndex == LB_ERR )
                    {
                        MessageBox( hwnd, "No item selected for adding.", "Error", MB_OK );
                        break;
                    }
                                      
                    char selectedText[ 100];

                    // Get the text of the item to remove.
                    SendDlgItemMessage( hwnd, IDC_AVAILABLE_COLUMN_LIST, LB_GETTEXT, selIndex, (LPARAM)selectedText );
                    // Add to end of the available list
                    SendDlgItemMessage(hwnd, IDC_CURRENT_COLUMN_LIST, LB_ADDSTRING,
                                       0, (LPARAM)selectedText);
                    // Remove the item.
                    SendDlgItemMessage( hwnd, IDC_AVAILABLE_COLUMN_LIST, LB_DELETESTRING, selIndex, 0);

                    int nCols      = SendDlgItemMessage( hwnd, IDC_CURRENT_COLUMN_LIST, LB_GETCOUNT, 0, 0 );
                    string columns = "";
                    for( int i = 0; i < nCols; i++ )
                    {
                        SendDlgItemMessage( hwnd, IDC_CURRENT_COLUMN_LIST, LB_GETTEXT, i, (LPARAM)selectedText );
                        columns += selectedText;
                        if ( i != (nCols -1 ))
                            columns += "|";
                    }
                    m_proposedValues.playlistHeaderColumns = columns;
                    break;
                }
            }    
            break;   
        }

        case UWM_HELP:
        case WM_HELP:
        {
            ShowHelp(m_pContext, Preferences_MusicBrowser);
            break;
        }

        case WM_NOTIFY:
        {
            NMHDR* notify = (NMHDR*)lParam;

            switch(notify->code)
            {
                case PSN_HELP:
                {
                    ShowHelp(m_pContext, Preferences_MusicBrowser);
                    break;
                }
                case PSN_SETACTIVE:
                {
                    
                    break;
                }

                case PSN_APPLY:
                {
                    SavePrefsValues(&m_proposedValues);
                    break;
                }

                case PSN_KILLACTIVE:
                {
                    
                    break;
                }

                case PSN_RESET:
                {
                    SavePrefsValues(&m_originalValues);
                    break;
                }
            }

            break;
        }
    }
    return( result );
}

bool Win32PreferenceWindow::PrefAdvancedProc(HWND hwnd, 
                                             UINT msg, 
                                             WPARAM wParam, 
                                             LPARAM lParam)      
{
    bool result = false;
    static HWND hwndPMO = NULL;
    static HWND hwndPriority = NULL;
    static HWND hwndInput = NULL;
    static HWND hwndOutput = NULL;
    static HWND hwndPrebuffer = NULL;
    static HWND hwndStayOnTop = NULL;
    static HWND hwndLiveInTray = NULL;
    

    switch(msg)
    {
        case WM_INITDIALOG:
        {
            // get the handles to all our controls
            hwndPMO = GetDlgItem(hwnd, IDC_PMO);
            hwndPriority = GetDlgItem(hwnd, IDC_PRIORITY);
            hwndInput = GetDlgItem(hwnd, IDC_INPUT);
            hwndOutput = GetDlgItem(hwnd, IDC_OUTPUT);
            hwndPrebuffer = GetDlgItem(hwnd, IDC_PREBUFFER);

            // get registries
            Registrar registrar;
            Registry pmo;
            Registry ui;

            registrar.SetSubDir("plugins");
            registrar.SetSearchString("*.pmo");
            registrar.InitializeRegistry(&pmo, m_prefs);
            registrar.SetSearchString("*.ui");
            registrar.InitializeRegistry(&ui, m_prefs);

            // initialize our controls

            int32 i = 0;
            int32 pos = 0;
            RegistryItem *item;

            while(item = pmo.GetItem(i++))
            {
                if(*item->Description())
                    pos = ComboBox_AddString(hwndPMO, item->Description());
                else
                    pos = ComboBox_AddString(hwndPMO, item->Name());

                string* name = new string(item->Name());

                ComboBox_SetItemData(hwndPMO, pos, (DWORD)name);

                if(m_originalValues.defaultPMO == item->Name())
                {
                    ComboBox_SetCurSel(hwndPMO, pos);
                }
            }

            SendMessage(hwndPriority, 
                        TBM_SETRANGE, 
                        (WPARAM) TRUE,                   
                        (LPARAM) MAKELONG(0, 6));

            SendMessage(hwndPriority, 
                        TBM_SETPOS, 
                        (WPARAM) TRUE,                   
                        (LPARAM) m_originalValues.decoderThreadPriority);

            int32 value;
            char temp[256];

            value = m_originalValues.inputBufferSize;
            sprintf(temp, "%d", value);
            Edit_LimitText(hwndInput, 4);
            Edit_SetText(hwndInput, temp);

            value = m_originalValues.outputBufferSize;
            sprintf(temp, "%d", value);
            Edit_LimitText(hwndOutput, 4);
            Edit_SetText(hwndOutput, temp);

            value = m_originalValues.preBufferLength;
            sprintf(temp, "%d", value);
            Edit_LimitText(hwndPrebuffer, 2);
            Edit_SetText(hwndPrebuffer, temp);
            
            break;
        }

        case WM_DESTROY:
        {
            int32 count = ComboBox_GetCount(hwndPMO);

            if(count != CB_ERR)
            {
                for(int32 i = 0; i < count;i++)
                {
                    string* s;
                    s = (string*)ComboBox_GetItemData(hwndPMO, i);

                    if(s)
                        delete s;
                }
            }
            break;
        }

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDC_PMO:
                {
                    if(HIWORD(wParam) == CBN_CLOSEUP)
                    {

                        int32 sel = ComboBox_GetCurSel(hwndPMO);

                        if(sel != CB_ERR)
                        {
                            string* s;
                            s = (string*)ComboBox_GetItemData(hwndPMO, sel);
                            
                            m_proposedValues.defaultPMO = *s;
                        }

                        /*ComboBox_GetText( hwndPMO, 
                                            m_proposedValues.defaultPMO, 
                                            256);*/

                        if(m_proposedValues != m_currentValues)
                        {
                            PropSheet_Changed(GetParent(hwnd), hwnd);
                        }
                        else
                        {
                            PropSheet_UnChanged(GetParent(hwnd), hwnd);
                        }
                    }

                    break;
                }                

                case IDC_INPUT:
                {
                    if(HIWORD(wParam) == EN_CHANGE)
                    {
                        char text[128];

                        Edit_GetText(hwndInput, text, sizeof(text));

                        m_proposedValues.inputBufferSize = atoi(text);

                        if(m_proposedValues != m_currentValues)
                        {
                            PropSheet_Changed(GetParent(hwnd), hwnd);
                        }
                        else
                        {
                            PropSheet_UnChanged(GetParent(hwnd), hwnd);
                        }
                    }

                    break;
                }

                case IDC_OUTPUT:
                {
                    if(HIWORD(wParam) == EN_CHANGE)
                    {
                        char text[128];

                        Edit_GetText(hwndOutput, text, sizeof(text));

                        m_proposedValues.outputBufferSize = atoi(text);

                        if(m_proposedValues != m_currentValues)
                        {
                            PropSheet_Changed(GetParent(hwnd), hwnd);
                        }
                        else
                        {
                            PropSheet_UnChanged(GetParent(hwnd), hwnd);
                        }
                    }

                    break;
                }
                
                case IDC_USEDEFAULTS:
                {
                    // We should pull these values from the 
                    // preferences rather than just hard coding them
                    SendMessage(hwndPriority, 
                                TBM_SETRANGE, 
                                (WPARAM) TRUE,                   
                                (LPARAM) MAKELONG(0, 6));
                    
                    SendMessage(hwndPriority, 
                                TBM_SETPOS, 
                                (WPARAM) TRUE,                   
                                (LPARAM) 5);
                    m_proposedValues.decoderThreadPriority = 5;
                    
                    Edit_LimitText(hwndInput, 4);
                    Edit_SetText(hwndInput, "64");
                    m_proposedValues.inputBufferSize = 64;
                    
                    Edit_LimitText(hwndOutput, 4);
                    Edit_SetText(hwndOutput, "512");
                    m_proposedValues.outputBufferSize = 512;
                    
                    Edit_LimitText(hwndPrebuffer, 2);
                    Edit_SetText(hwndPrebuffer, "0");
                    m_proposedValues.preBufferLength = 0;
                    
                    if(m_proposedValues != m_currentValues)
                        PropSheet_Changed(GetParent(hwnd), hwnd);
                    else
                        PropSheet_UnChanged(GetParent(hwnd), hwnd);
                
                    break;
                }

                case IDC_PREBUFFER:
                {
                    if(HIWORD(wParam) == EN_CHANGE)
                    {
                        char text[128];

                        Edit_GetText(hwndPrebuffer, text, sizeof(text));

                        m_proposedValues.preBufferLength = atoi(text);

                        if(m_proposedValues != m_currentValues)
                        {
                            PropSheet_Changed(GetParent(hwnd), hwnd);
                        }
                        else
                        {
                            PropSheet_UnChanged(GetParent(hwnd), hwnd);
                        }
                    }

                    break;
                }
            }

            break;
        }

        case WM_HSCROLL:
		{
			LRESULT position;
            HWND hwndScroll = (HWND) lParam;

			switch (LOWORD(wParam)) 
			{
				case TB_LINEUP:
				case TB_LINEDOWN:
				case TB_PAGEDOWN:
				case TB_PAGEUP:
				case TB_THUMBPOSITION:
                case TB_THUMBTRACK:
				{
                    if(hwndScroll == hwndPriority)
                    {
		  			    position = SendMessage(	hwndScroll, 
											    TBM_GETPOS, 
											    0, 
											    0);

                        m_proposedValues.decoderThreadPriority = position;

                        if(m_proposedValues != m_currentValues)
                        {
                            PropSheet_Changed(GetParent(hwnd), hwnd);
                        }
                        else
                        {
                            PropSheet_UnChanged(GetParent(hwnd), hwnd);
                        }
                    }
                }
			}

			break;
        }

        case UWM_HELP:
        case WM_HELP:
        {
            ShowHelp(m_pContext, Preferences_Advanced);
            break;
        }

        case WM_NOTIFY:
        {
            NMHDR* notify = (NMHDR*)lParam;

            switch(notify->code)
            {
                case PSN_HELP:
                {
                    ShowHelp(m_pContext, Preferences_Advanced);
                    break;
                }
                case PSN_SETACTIVE:
                {
                    
                    break;
                }

                case PSN_APPLY:
                {
                    SavePrefsValues(&m_proposedValues);
                    break;
                }

                case PSN_KILLACTIVE:
                {
                    
                    break;
                }

                case PSN_RESET:
                {
                    SavePrefsValues(&m_originalValues);
                    break;
                }
            }

            break;
        }
    }

    return result;
}

LRESULT WINAPI 
ListViewWndProc(HWND hwnd, 
                UINT msg, 
                WPARAM wParam, 
                LPARAM lParam);

bool Win32PreferenceWindow::PrefPluginsProc(HWND hwnd, 
                                            UINT msg, 
                                            WPARAM wParam, 
                                            LPARAM lParam)      
{
    bool result = false;
    static HWND hwndList = NULL;
    static HWND hwndPMO = NULL;
    static set<string> originalSet;
    static set<string> currentSet;

    
    switch(msg)
    {
        case WM_INITDIALOG:
        {
            // get the handles to all our controls
            hwndPMO = GetDlgItem(hwnd, IDC_PMO);

            // Init our controls

            // get registries
            Registrar registrar;
            Registry pmo;
            Registry ppp;

            registrar.SetSubDir("plugins");
            registrar.SetSearchString("*.pmo");
            registrar.InitializeRegistry(&pmo, m_prefs);

            registrar.SetSearchString("*.ppp");
            registrar.InitializeRegistry(&ppp, m_prefs);

            uint32 i = 0;
            int32 pos = 0;
            RegistryItem *item;

            while(item = pmo.GetItem(i++))
            {
                if(!strncasecmp("cd.pmo", item->Name(), 7))
                    continue;

                if(*item->Description())
                    pos = ComboBox_AddString(hwndPMO, item->Description());
                else
                    pos = ComboBox_AddString(hwndPMO, item->Name());

                string* name = new string(item->Name());

                ComboBox_SetItemData(hwndPMO, pos, (DWORD)name);

                if(m_originalValues.defaultPMO == item->Name())
                {
                    ComboBox_SetCurSel(hwndPMO, pos);
                }
            }
            
            HINSTANCE hinst = (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE);

            // get our checkbox images
            HIMAGELIST imageList = ImageList_Create(16, 16, ILC_COLOR24|ILC_MASK, 2, 0);

            HBITMAP bmp;
            bmp = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_UNCHECKED));
            ImageList_AddMasked(imageList, bmp, RGB(255,0,0));
            DeleteObject(bmp);
            bmp = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_CHECKED));
            ImageList_AddMasked(imageList, bmp, RGB(255,0,0));
            DeleteObject(bmp);

            ListView_SetImageList(hwndList, imageList, LVSIL_SMALL); 

            break;
        }

        case WM_DESTROY:
        {
            int32 count = ComboBox_GetCount(hwndPMO);

            if(count != CB_ERR)
            {
                for(int32 i = 0; i < count;i++)
                {
                    string* s;
                    s = (string*)ComboBox_GetItemData(hwndPMO, i);

                    if(s)
                        delete s;
                }
            }


            break;
        }

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDC_PMO:
                {
                    if(HIWORD(wParam) == CBN_CLOSEUP)
                    {

                        int32 sel = ComboBox_GetCurSel(hwndPMO);

                        if(sel != CB_ERR)
                        {
                            string* s;
                            s = (string*)ComboBox_GetItemData(hwndPMO, sel);
                            
                            m_proposedValues.defaultPMO = *s;
                        }

                        /*ComboBox_GetText( hwndPMO, 
                                            m_proposedValues.defaultPMO, 
                                            256);*/

                        if(m_proposedValues != m_currentValues)
                        {
                            PropSheet_Changed(GetParent(hwnd), hwnd);
                        }
                        else
                        {
                            PropSheet_UnChanged(GetParent(hwnd), hwnd);
                        }
                    }

                    break;
                }
            }

            break;
        }

        case WM_MEASUREITEM:
        {
            MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*) lParam;
            TEXTMETRIC tm;
            HDC hdc;
	        HFONT hFont, oldFont;
	        HWND hwndLV;


            // Make sure the control is the listview control
            if (mis->CtlType != ODT_LISTVIEW)
                return FALSE;

	        // Get the handle of the ListView control we're using
	        hwndLV = GetDlgItem(hwnd, mis->CtlID);

	        // Get the font the control is currently using
	        hFont = (HFONT)(DWORD) SendMessage(hwndLV, WM_GETFONT, 0, 0L);

	        // Set the font of the DC to the same font the control is using
            hdc = GetDC(hwndLV);
	        oldFont = (HFONT)SelectObject(hdc, hFont);

            // Get the height of the font used by the control
            if (!GetTextMetrics(hdc, &tm))
                return FALSE;

            // Add a little extra space between items
            mis->itemHeight = tm.tmHeight + 1;

            // Make sure there is enough room for the images which are CY_SMICON high
            if (mis->itemHeight < 17)
    	        mis->itemHeight = 17;

            // width ?
            RECT rect;
            GetClientRect(hwndLV, &rect);

            mis->itemWidth = (rect.right-rect.left); // width of column in pixels

	        // Clean up
            SelectObject(hdc, oldFont);
	        ReleaseDC(hwndLV, hdc);

            return TRUE;

            break;
        }
#if 0
        case WM_DRAWITEM:
        {
            DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*) lParam;
            int32 controlId = (UINT) wParam;
            HIMAGELIST himl;
            int32 cxImage = 0, cyImage = 0;
            PortablePlayer* item = (PortablePlayer*)dis->itemData;

            // Get Image List
            himl = ListView_GetImageList(dis->hwndItem, LVSIL_SMALL);
            ImageList_GetIconSize(himl, &cxImage, &cyImage);

            uint32 uiFlags = ILD_TRANSPARENT;
            RECT rcClip;

            // Check to see if this item is selected
            /*if(dis->itemState & ODS_SELECTED)
            {
                // Set the text background and foreground colors
                SetTextColor(dis->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
                SetBkColor(dis->hDC, GetSysColor(COLOR_HIGHLIGHT));
            }
            else*/
            {
                // Set the text background and foreground colors 
                // to the standard window colors
                SetTextColor(dis->hDC, GetSysColor(COLOR_WINDOWTEXT));
                SetBkColor(dis->hDC, GetSysColor(COLOR_WINDOW));
            }

            rcClip = dis->rcItem;            

            // Check to see if the string fits in the clip rect.  If not, truncate
            // the string and add "...".
            string displayString = item->name;

            /*CalcStringEllipsis(dis->hDC, 
                               displayString, 
                               ListView_GetColumnWidth(hwndList, 0));*/

            // Get the height of the font used by the control
            TEXTMETRIC tm;
            GetTextMetrics(dis->hDC, &tm);
            
            uint32 top = rcClip.top + ((rcClip.bottom - rcClip.top) - tm.tmHeight)/2;

            ExtTextOut( dis->hDC, 
                        rcClip.left + cxImage + 3, top, 
                        ETO_CLIPPED | ETO_OPAQUE,
                        &rcClip, 
                        displayString.c_str(),
                        displayString.size(),
                        NULL);

            // Draw the icon. Drawing it after the first item allows us
            // to let ExtTextOut draw the correct background 
            if(himl)
            {
                top = rcClip.top + ((rcClip.bottom - rcClip.top) - cyImage)/2;
                int32 index = item->checked ? 1 : 0;
                ImageList_Draw( himl, index, dis->hDC, 
                                rcClip.left, top,
                                uiFlags);
            }

            // If we changed the colors for the selected item, undo it
            if(dis->itemState & ODS_SELECTED)
            {
                // Set the text background and foreground colors
                SetTextColor(dis->hDC, GetSysColor(COLOR_WINDOWTEXT));
                SetBkColor(dis->hDC, GetSysColor(COLOR_WINDOW));
            }

            // If the item is focused, now draw a focus rect around the entire row
            /*if(dis->itemState & ODS_FOCUS)
            {
                // Draw the focus rect
                DrawFocusRect(dis->hDC, &dis->rcItem);
            }*/

            

            result = true;
            break;
        }
#endif
        case WM_HELP:
        case UWM_HELP:
        {
            ShowHelp(m_pContext, Preferences_Plugins);
            break;
        }

        case WM_NOTIFY:
        {
            NMHDR* notify = (NMHDR*)lParam;

            switch(notify->code)
            {
                case PSN_HELP:
                {
                    ShowHelp(m_pContext, Preferences_Plugins);
                    break;
                }
                case PSN_SETACTIVE:
                {
                    break;
                }

                case PSN_APPLY:
                {
                    SavePrefsValues(&m_proposedValues);
                    break;
                }

                case PSN_KILLACTIVE:
                {
                    break;
                }

                case PSN_RESET:
                {
                    SavePrefsValues(&m_originalValues);
                    break;
                }
            }

            break;
        }
    }

    return result;
}

// MS does not define the Extended version of the 
// dialog template structure so we do so here. This is 
// only a partial definition that will allow us to get 
// to the caption.
#include <pshpack2.h>
typedef struct {  
    WORD   dlgVer; // 0x001
    WORD   signature; // 0xFFFF
    DWORD  helpID; 
    DWORD  exStyle; 
    DWORD  style; 
    WORD   cDlgItems; 
    short  x; 
    short  y; 
    short  cx; 
    short  cy; 
} DLGTEMPLATEEX;
#include <poppack.h> /* Resume normal packing */

void Win32PreferenceWindow::InitializePrefDialog()
{
    HGLOBAL         hDlgResMem;
    HRSRC           hDlgRes;
    DLGTEMPLATE*    lpDlgRes;
    DLGTEMPLATEEX*  lpDlgExRes;
    long            cx = 0, cy = 0;
    WORD*           caption;

    HWND hwndList = GetDlgItem(m_hwndPref, IDC_LIST);
    HTREEITEM parentItem = NULL;

    vector<PrefPage>::iterator i = m_pages.begin();

    for(;i != m_pages.end(); i++)
    {
        // We have not loaded this yet so make sure it is NULL
        (*i).hwnd = NULL;

        // Load the resource
        hDlgRes = FindResource( (*i).hInstance,
                                (*i).pszTemplate,
                                RT_DIALOG);

        hDlgResMem = LoadResource((*i).hInstance, hDlgRes);
        lpDlgRes = (DLGTEMPLATE*) LockResource(hDlgResMem);
        lpDlgExRes = (DLGTEMPLATEEX*)lpDlgRes;

        if(lpDlgExRes->dlgVer == 1 && lpDlgExRes->signature == 0xFFFF)
        {
            // How big is this dialog?
            if(lpDlgExRes->cx > cx)
                cx = lpDlgExRes->cx;

            if(lpDlgExRes->cy > cy)
                cy = lpDlgExRes->cy;

            // What is the caption for this pref page?
            caption = (WORD*)((char*)lpDlgExRes + sizeof(DLGTEMPLATEEX));
        }
        else
        {
            // How big is this dialog?
            if(lpDlgRes->cx > cx)
                cx = lpDlgRes->cx;

            if(lpDlgRes->cy > cy)
                cy = lpDlgRes->cy;

            // What is the caption for this pref page?
            caption = (WORD*)((char*)lpDlgRes + sizeof(DLGTEMPLATE));
        }

        // Skip menu if it is there
        if (0xffff == *caption)
            caption += 2;     // Menu by ordinal.  Skip 0xffff and the ordinal.
        else
            caption += (wcslen(caption) + 1);      // Menu by name, or no menu at all.
        
        // Pass the class name
        caption += wcslen(caption); ++caption; 

        // Here we are...
        size_t captionLength = wcslen(caption) + 1;

		char* buf = (char*)malloc(captionLength);

		WideCharToMultiByte(CP_ACP, 
							0,
							caption,
							captionLength,
							buf,
							captionLength,
							NULL, NULL);					



        // Add the item to the list
        TVINSERTSTRUCT tv_insert;
        
        tv_insert.hParent = ((*i).bChild ? parentItem : NULL);
        tv_insert.hInsertAfter = TVI_LAST ;
        tv_insert.item.mask = TVIF_PARAM | TVIF_TEXT;
        tv_insert.item.pszText = buf;
        tv_insert.item.cchTextMax = strlen(tv_insert.item.pszText);
        tv_insert.item.lParam = (LPARAM)&(*i);

        TreeView_InsertItem(hwndList, &tv_insert);

        free(buf);

        // Free the resource
        UnlockResource(hDlgResMem);
        FreeResource(hDlgResMem);
    }

    RECT pageRect;
    HWND hwndPage = GetDlgItem(m_hwndPref, IDC_PAGE);

    GetClientRect(hwndPage, &pageRect);

    MapWindowPoints(hwndPage, m_hwndPref, (LPPOINT)&pageRect, 2);

    RECT mapRect;

    mapRect.left = 0;
    mapRect.top = 0;
    mapRect.right = cx;
    mapRect.bottom = cy;

    MapDialogRect(m_hwndPref, &mapRect);

    cx = mapRect.right;
    cy = mapRect.bottom;

    if(cx > pageRect.right - pageRect.left)
        cx -= pageRect.right - pageRect.left;
    else
        cx = 0;

    if(cy > pageRect.bottom - pageRect.top)
        cy -= pageRect.bottom - pageRect.top;
    else
        cy = 0;  

    // Move windows into position
    HWND hwndFade = GetDlgItem(m_hwndPref, IDC_FADE);
    HWND hwndOK = GetDlgItem(m_hwndPref, IDOK);
    HWND hwndCancel = GetDlgItem(m_hwndPref, IDCANCEL);
    HWND hwndApply = GetDlgItem(m_hwndPref, IDC_APPLY);
    HWND hwndHelp = GetDlgItem(m_hwndPref, IDC_HELPME);
    
    HWND hwndEtch = FindWindowEx(m_hwndPref, NULL, "STATIC", NULL);

    RECT rect;

    GetWindowRect(m_hwndPref, &rect);

    SetWindowPos(m_hwndPref, NULL, 0, 0, 
                 rect.right - rect.left + cx, 
                 rect.bottom - rect.top + cy, 
                 SWP_NOMOVE|SWP_NOZORDER);

    GetWindowRect(hwndFade, &rect);
    MapWindowPoints(NULL, m_hwndPref, (LPPOINT)&rect, 2);
    
    SetWindowPos(hwndFade, NULL, 0, 0, 
                 rect.right - rect.left + cx, 
                 rect.bottom - rect.top, 
                 SWP_NOMOVE|SWP_NOZORDER);

    GetWindowRect(hwndList, &rect);
    MapWindowPoints(NULL, m_hwndPref, (LPPOINT)&rect, 2);
    
    SetWindowPos(hwndList, NULL, 0, 0, 
                 rect.right - rect.left, 
                 rect.bottom - rect.top + cy, 
                 SWP_NOMOVE|SWP_NOZORDER);

    GetWindowRect(hwndOK, &rect);
    MapWindowPoints(NULL, m_hwndPref, (LPPOINT)&rect, 2);

    SetWindowPos(hwndOK, NULL, rect.left + cx, rect.top + cy, 
                    0, 0, SWP_NOSIZE|SWP_NOZORDER);

    GetWindowRect(hwndCancel, &rect);
    MapWindowPoints(NULL, m_hwndPref, (LPPOINT)&rect, 2);

    SetWindowPos(hwndCancel, NULL, rect.left + cx, rect.top + cy, 
                    0, 0, SWP_NOSIZE|SWP_NOZORDER);

    GetWindowRect(hwndApply, &rect);
    MapWindowPoints(NULL, m_hwndPref, (LPPOINT)&rect, 2);

    SetWindowPos(hwndApply, NULL, rect.left + cx, rect.top + cy, 
                    0, 0, SWP_NOSIZE|SWP_NOZORDER);

    GetWindowRect(hwndHelp, &rect);
    MapWindowPoints(NULL, m_hwndPref, (LPPOINT)&rect, 2);

    SetWindowPos(hwndHelp, NULL, rect.left + cx, rect.top + cy, 
                    0, 0, SWP_NOSIZE|SWP_NOZORDER);

    GetWindowRect(hwndEtch, &rect);
    MapWindowPoints(NULL, m_hwndPref, (LPPOINT)&rect, 2);

    SetWindowPos(hwndEtch, NULL, rect.left, rect.top + cy, 
                 rect.right - rect.left + cx, 
                 rect.bottom - rect.top, 
                 SWP_NOZORDER);
}

void Win32PreferenceWindow::ShowPrefPage(PrefPage* page, bool show)
{
    if(!page)
        return;

    if(!show)
    {
        if(page->hwnd)
        {
            ShowWindow(page->hwnd, FALSE);
            m_currentPage = NULL;
        }
    }
    else
    {
        if(page->hwnd)
        {
            ShowWindow(page->hwnd, TRUE);
        }
        else
        {

            HGLOBAL         hDlgResMem;
            HRSRC           hDlgRes;
            DLGTEMPLATE*    lpDlgRes;
            DLGTEMPLATEEX*  lpDlgExRes;
        
            // Find the dialog resource
            hDlgRes = FindResource( page->hInstance,
                                    page->pszTemplate,
                                    RT_DIALOG);

            hDlgResMem = LoadResource(page->hInstance, hDlgRes);
            lpDlgRes = (DLGTEMPLATE*) LockResource(hDlgResMem);
            lpDlgExRes = (DLGTEMPLATEEX*)lpDlgRes;

            // The positioning of the controls will be aligned 
            // with the invisible page control
            RECT pageRect;

            HWND hwndPage = GetDlgItem(m_hwndPref, IDC_PAGE);

            GetClientRect(hwndPage, &pageRect);

            MapWindowPoints(hwndPage, m_hwndPref, (LPPOINT)&pageRect, 2);

            // Create the Pref page
            page->hwnd = CreateDialogIndirect(
                                 page->hInstance, 
                                 lpDlgRes, 
                                 m_hwndPref, 
                                 page->pfnDlgProc);

            // Move it into position
            SetWindowPos(page->hwnd, NULL, pageRect.left, pageRect.top, 
                            0, 0, SWP_NOSIZE|SWP_NOZORDER);

            
            // Free the resource
            UnlockResource(hDlgResMem);
            FreeResource(hDlgResMem);
        }

        m_currentPage = page;

        // change the caption for this page
        uint32 length = GetWindowTextLength(page->hwnd) + 1;
        char* foo = new char[length];

        GetWindowText(page->hwnd, foo, length);

        m_caption = foo;

        delete [] foo;

        RECT fadeRect;
        HWND hwndFade = GetDlgItem(m_hwndPref, IDC_FADE);
        GetClientRect(hwndFade, &fadeRect);

        MapWindowPoints(hwndFade, m_hwndPref, (LPPOINT)&fadeRect, 2);

        InvalidateRect(m_hwndPref, &fadeRect, FALSE);
    }
}

bool Win32PreferenceWindow::PrefCDAudioProc(HWND hwnd, 
                                           UINT msg, 
                                           WPARAM wParam, 
                                           LPARAM lParam)      
{
    bool result = false;
    
    switch(msg)
    {
        case WM_INITDIALOG:
        {
            SetWindowText(GetDlgItem(hwnd, IDC_MB_URL), m_originalValues.MBServerURL.c_str());
            Button_SetCheck(GetDlgItem(hwnd, IDC_UPDATE_AUDIO), m_originalValues.updateCDAutomatically);
            break;
        }

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDC_MB_URL:
                {
					char temp[MAX_PATH];

                    GetWindowText(GetDlgItem(hwnd, IDC_MB_URL), temp, MAX_PATH);
					m_proposedValues.MBServerURL = string(temp);
                    break;
                }
                case IDC_UPDATE_AUDIO:
                {
					m_proposedValues.updateCDAutomatically = 
                       Button_GetCheck(GetDlgItem(hwnd, IDC_UPDATE_AUDIO)) == BST_CHECKED;
                    break;
                }
            }
            if(m_proposedValues != m_currentValues)
            {
                PropSheet_Changed(GetParent(hwnd), hwnd);
            }
            else
            {
                PropSheet_UnChanged(GetParent(hwnd), hwnd);
            }

            break;
        }

        case UWM_HELP:
        case WM_HELP:
        {
            ShowHelp(m_pContext, Preferences_About);
            break;
        }

        case WM_NOTIFY:
        {
            NMHDR* notify = (NMHDR*)lParam;

            switch(notify->code)
            {
                case PSN_HELP:
                {
                    ShowHelp(m_pContext, Preferences_About);
                    break;
                }
                case PSN_SETACTIVE:
                {
                    
                    break;
                }

                case PSN_APPLY:
                {
                    SavePrefsValues(&m_proposedValues);
                    break;
                }

                case PSN_KILLACTIVE:
                {
                    
                    break;
                }

                case PSN_RESET:
                {
                    SavePrefsValues(&m_originalValues);
                    break;
                }
            }

            break;
        }
    }

    return result;
}

