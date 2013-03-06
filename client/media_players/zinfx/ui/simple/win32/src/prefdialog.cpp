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
	
	$Id: prefdialog.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
____________________________________________________________________________*/

/* system headers */
#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <commctrl.h>
#include <stdlib.h>
#include <assert.h>

/* project headers */
#include "config.h"
#include "prefdialog.h"
#include "preferences.h"
#include "log.h"
#include "registrar.h"
#include "resource.h"

typedef struct PrefsStruct {
    Preferences* prefs;

    // page 1
    char defaultUI[256];
    char defaultPMO[256];
    int32 inputBufferSize;
    int32 outputBufferSize;
    int32 preBufferLength;
    int32 decoderThreadPriority;
    bool stayOnTop;
    bool liveInTray;

    // page 2
    int32 streamInterval;
    bool saveStreams;
    char saveStreamsDirectory[MAX_PATH + 1];
    bool useProxyServer;
    char proxyServer[256]; // is there a domain name length limit???
    bool useAlternateIP;
    char alternateIP[16];
    
    // page 3
    bool enableLogging;
    bool logMain;
    bool logInput;
    bool logOutput;
    bool logDecoder;
    bool logPerformance;

    PrefsStruct()
    {
        memset(defaultUI, 0x00, sizeof(defaultUI));
        memset(defaultPMO, 0x00, sizeof(defaultPMO));
        memset(saveStreamsDirectory, 0x00, sizeof(saveStreamsDirectory));
        memset(proxyServer, 0x00, sizeof(proxyServer));
        memset(alternateIP, 0x00, sizeof(alternateIP));
    }

}PrefsStruct;

static PrefsStruct originalValues;
static PrefsStruct currentValues;


static 
void 
GetPrefsValues(Preferences* prefs, 
               PrefsStruct* values)
{
    uint32 size = 256;

    prefs->GetPrefString(kPMOPref, values->defaultPMO, &size);
    size = 256;
    prefs->GetPrefString(kUIPref, values->defaultUI, &size);
    prefs->GetPrefInt32(kDecoderThreadPriorityPref, &values->decoderThreadPriority);
    prefs->GetPrefInt32(kInputBufferSizePref, &values->inputBufferSize);
    prefs->GetPrefInt32(kOutputBufferSizePref, &values->outputBufferSize);
    prefs->GetPrefInt32(kPreBufferPref, &values->preBufferLength);
    prefs->GetPrefBoolean(kStayOnTopPref, &values->stayOnTop);
    prefs->GetPrefBoolean(kLiveInTrayPref, &values->liveInTray);

    prefs->GetPrefInt32(kStreamBufferIntervalPref, &values->streamInterval);
    prefs->GetPrefBoolean(kSaveStreamsPref, &values->saveStreams);
    size = 256;
    prefs->GetPrefString(kProxyHostPref, values->proxyServer, &size);
    prefs->GetPrefBoolean(kUseProxyPref, &values->useProxyServer);
    size = MAX_PATH;
    prefs->GetPrefString(kSaveStreamsDirPref, values->saveStreamsDirectory, &size);
    size = 16;
    prefs->GetPrefString(kAlternateNICAddressPref, values->alternateIP, &size);
    prefs->GetPrefBoolean(kUseAlternateNICPref, &values->useAlternateIP);

    prefs->GetPrefBoolean(kUseDebugLogPref, &values->enableLogging);
    prefs->GetPrefBoolean(kLogMainPref, &values->logMain);
    prefs->GetPrefBoolean(kLogDecodePref, &values->logDecoder);
    prefs->GetPrefBoolean(kLogInputPref, &values->logInput);
    prefs->GetPrefBoolean(kLogOutputPref, &values->logOutput);
    prefs->GetPrefBoolean(kLogPerformancePref, &values->logPerformance);
}

static 
void 
SavePrefsValues(Preferences* prefs, 
                PrefsStruct* values)
{
    prefs->SetPrefString(kPMOPref, values->defaultPMO);
    prefs->SetPrefString(kUIPref, values->defaultUI);
    prefs->SetPrefInt32(kDecoderThreadPriorityPref, values->decoderThreadPriority);
    prefs->SetPrefInt32(kInputBufferSizePref, values->inputBufferSize);
    prefs->SetPrefInt32(kOutputBufferSizePref, values->outputBufferSize);
    prefs->SetPrefInt32(kPreBufferPref, values->preBufferLength);
    prefs->SetPrefBoolean(kStayOnTopPref, values->stayOnTop);
    prefs->SetPrefBoolean(kLiveInTrayPref, values->liveInTray);

    prefs->SetPrefInt32(kStreamBufferIntervalPref, values->streamInterval);
    prefs->SetPrefBoolean(kSaveStreamsPref, values->saveStreams);
    prefs->SetPrefString(kSaveStreamsDirPref, values->saveStreamsDirectory);
    prefs->SetPrefString(kProxyHostPref, values->proxyServer);
    prefs->SetPrefBoolean(kUseProxyPref, values->useProxyServer);
    prefs->SetPrefString(kAlternateNICAddressPref, values->alternateIP);
    prefs->SetPrefBoolean(kUseAlternateNICPref, values->useAlternateIP);

    prefs->SetPrefBoolean(kUseDebugLogPref, values->enableLogging);
    prefs->SetPrefBoolean(kLogMainPref, values->logMain);
    prefs->SetPrefBoolean(kLogDecodePref, values->logDecoder);
    prefs->SetPrefBoolean(kLogInputPref, values->logInput);
    prefs->SetPrefBoolean(kLogOutputPref, values->logOutput);
    prefs->SetPrefBoolean(kLogPerformancePref, values->logPerformance);
}


static
BOOL 
CALLBACK 
PrefPage1Proc(  HWND hwnd, 
                UINT msg, 
                WPARAM wParam, 
                LPARAM lParam)
{
    UINT result = 0;
    static PROPSHEETPAGE* psp = NULL;
    static Preferences* prefs = NULL;
    static HWND hwndUI = NULL;
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
            // remember these for later...
            psp = (PROPSHEETPAGE*)lParam;
            prefs = (Preferences*)psp->lParam;

            // get the handles to all our controls
            hwndUI = GetDlgItem(hwnd, IDC_UI);
            hwndPMO = GetDlgItem(hwnd, IDC_PMO);
            hwndPriority = GetDlgItem(hwnd, IDC_PRIORITY);
            hwndInput = GetDlgItem(hwnd, IDC_INPUT);
            hwndOutput = GetDlgItem(hwnd, IDC_OUTPUT);
            hwndPrebuffer = GetDlgItem(hwnd, IDC_PREBUFFER);
            hwndStayOnTop = GetDlgItem(hwnd, IDC_STAYONTOP);
            hwndLiveInTray = GetDlgItem(hwnd, IDC_TRAY);

            // get registries
            Registrar registrar;
            Registry pmo;
            Registry ui;

            registrar.SetSubDir("plugins");
            registrar.SetSearchString("*.pmo");
            registrar.InitializeRegistry(&pmo, prefs);
            registrar.SetSearchString("*.ui");
            registrar.InitializeRegistry(&ui, prefs);

            // initialize our controls

            int32 i = 0;
            RegistryItem *item;

            while(item = pmo.GetItem(i++))
            {
                ComboBox_AddString(hwndPMO, item->Name());

                if(!strcmp(originalValues.defaultPMO, item->Name()))
                {
                    ComboBox_SetCurSel(hwndPMO, i-1);
                }
            }
            
            i = 0;

            while(item = ui.GetItem(i++))
            {
                ComboBox_AddString(hwndUI, item->Name());

                if(!strcmp(originalValues.defaultUI, item->Name()))
                {
                    ComboBox_SetCurSel(hwndUI, i-1);
                }
            }

            SendMessage(hwndPriority, 
                        TBM_SETRANGE, 
                        (WPARAM) TRUE,                   
                        (LPARAM) MAKELONG(0, 6));

            SendMessage(hwndPriority, 
                        TBM_SETPOS, 
                        (WPARAM) TRUE,                   
                        (LPARAM) originalValues.decoderThreadPriority);

            int32 value;
            char temp[256];

            value = originalValues.inputBufferSize;
            sprintf(temp, "%d", value);
            Edit_LimitText(hwndInput, 4);
            Edit_SetText(hwndInput, temp);

            value = originalValues.outputBufferSize;
            sprintf(temp, "%d", value);
            Edit_LimitText(hwndOutput, 4);
            Edit_SetText(hwndOutput, temp);

            value = originalValues.preBufferLength;
            sprintf(temp, "%d", value);
            Edit_LimitText(hwndPrebuffer, 2);
            Edit_SetText(hwndPrebuffer, temp);

            Button_SetCheck(hwndStayOnTop, originalValues.stayOnTop);

            Button_SetCheck(hwndLiveInTray, originalValues.liveInTray);
            
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
                        memset(currentValues.defaultPMO, 0x00, 256);
                        ComboBox_GetText(   hwndPMO, 
                                            currentValues.defaultPMO, 
                                            256);

                        if(memcmp(  &originalValues, 
                                    &currentValues, 
                                    sizeof(PrefsStruct)))
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

                case IDC_UI:
                {
                    if(HIWORD(wParam) == CBN_CLOSEUP)
                    {
                        memset(currentValues.defaultUI, 0x00, 256);
                        ComboBox_GetText(   hwndUI, 
                                            currentValues.defaultUI, 
                                            256);

                        if(memcmp(  &originalValues, 
                                    &currentValues, 
                                    sizeof(PrefsStruct)))
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

                        currentValues.inputBufferSize = atoi(text);

                        if(memcmp(  &originalValues, 
                                    &currentValues, 
                                    sizeof(PrefsStruct)))
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

                        currentValues.outputBufferSize = atoi(text);

                        if(memcmp(  &originalValues, 
                                    &currentValues, 
                                    sizeof(PrefsStruct)))
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

                case IDC_PREBUFFER:
                {
                    if(HIWORD(wParam) == EN_CHANGE)
                    {
                        char text[128];

                        Edit_GetText(hwndPrebuffer, text, sizeof(text));

                        currentValues.preBufferLength = atoi(text);

                        if(memcmp(  &originalValues, 
                                    &currentValues, 
                                    sizeof(PrefsStruct)))
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

                case IDC_STAYONTOP:
                {
                    if(Button_GetCheck(hwndStayOnTop) == BST_CHECKED)
                    {
                        currentValues.stayOnTop = true;
                    }
                    else
                    {
                        currentValues.stayOnTop = false;
                    }

                    if(memcmp(  &originalValues, 
                                &currentValues, 
                                sizeof(PrefsStruct)))
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
                    if(Button_GetCheck(hwndLiveInTray) == BST_CHECKED)
                    {
                        currentValues.liveInTray = true;
                    }
                    else
                    {
                        currentValues.liveInTray = false;
                    }

                    if(memcmp(  &originalValues, 
                                &currentValues, 
                                sizeof(PrefsStruct)))
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

                        currentValues.decoderThreadPriority = position;

                        if(memcmp(  &originalValues, 
                                    &currentValues, 
                                    sizeof(PrefsStruct)))
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
                    SavePrefsValues(prefs, &currentValues);
                    break;
                }

                case PSN_KILLACTIVE:
                {
                    
                    break;
                }

                case PSN_RESET:
                {
                    SavePrefsValues(prefs, &originalValues);
                    break;
                }
            }

            break;
        }
    }

    return result;
}

static
BOOL 
CALLBACK 
PrefPage2Proc(  HWND hwnd, 
                UINT msg, 
                WPARAM wParam, 
                LPARAM lParam)
{
    UINT result = 0;
    static PROPSHEETPAGE* psp = NULL;
    static Preferences* prefs = NULL;
    static HWND hwndStreamInterval = NULL;
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
            // remember these for later...
            psp = (PROPSHEETPAGE*)lParam;
            prefs = (Preferences*)psp->lParam;

            // get the handles to all our controls
            hwndStreamInterval = GetDlgItem(hwnd, IDC_STREAM_INTERVAL);

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
            int32 value;
            char temp[256];

            value = originalValues.streamInterval;
            sprintf(temp, "%d", value);
            Edit_LimitText(hwndStreamInterval, 2);
            Edit_SetText(hwndStreamInterval, temp);
            
            Button_SetCheck(hwndSaveStreams, originalValues.saveStreams);


            Edit_SetText(   hwndSaveStreamsDirectory, 
                            originalValues.saveStreamsDirectory);

            Button_Enable(  hwndSaveStreamsDirectory, 
                            originalValues.saveStreams);

            Button_Enable(  hwndBrowse, 
                            originalValues.saveStreams);

            Button_Enable(  hwndSaveLocationText,
                            originalValues.saveStreams);

            char* port = NULL;

            strcpy(temp, originalValues.proxyServer);
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


            Button_SetCheck(hwndUseProxyServer, originalValues.useProxyServer);


            Button_Enable(  hwndProxyServerAddress, 
                            originalValues.useProxyServer);

            Button_Enable(  hwndProxyServerAddressText, 
                            originalValues.useProxyServer);

            Button_Enable(  hwndProxyServerPort,
                            originalValues.useProxyServer);

            Button_Enable(  hwndProxyServerPortText, 
                            originalValues.useProxyServer);

            Button_Enable(  hwndColon, 
                            originalValues.useProxyServer);

            char* dot = NULL;
            char* ip[4];
            int32 i = 1;

            strcpy(temp, originalValues.alternateIP);
            ip[0] = temp;
            dot = temp;

            while(dot = strchr(dot, '.'))
            {
                *dot = 0x00;
                ip[i++] = ++dot;
            }

            Edit_SetText(hwndAlternateIPAddress1, ip[0]);
            Edit_SetText(hwndAlternateIPAddress2, ip[1]);
            Edit_SetText(hwndAlternateIPAddress3, ip[2]);
            Edit_SetText(hwndAlternateIPAddress4, ip[3]);

            Edit_LimitText(hwndAlternateIPAddress1, 3);
            Edit_LimitText(hwndAlternateIPAddress2, 3);
            Edit_LimitText(hwndAlternateIPAddress3, 3);
            Edit_LimitText(hwndAlternateIPAddress4, 3);

            Button_SetCheck(hwndUseAlternateIP, originalValues.useAlternateIP);


            Button_Enable(  hwndUseAlternateIPText, 
                            originalValues.useAlternateIP);

            Button_Enable(  hwndAlternateIPAddress1,
                            originalValues.useAlternateIP);

            Button_Enable(  hwndPeriod1, 
                            originalValues.useAlternateIP);

            Button_Enable(  hwndAlternateIPAddress2, 
                            originalValues.useAlternateIP);

            Button_Enable(  hwndPeriod2, 
                            originalValues.useAlternateIP);

            Button_Enable(  hwndAlternateIPAddress3, 
                            originalValues.useAlternateIP);

            Button_Enable(  hwndPeriod3, 
                            originalValues.useAlternateIP);

            Button_Enable(  hwndAlternateIPAddress4, 
                            originalValues.useAlternateIP);

            
            break;
        }

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDC_STREAM_INTERVAL:
                {
                    if(HIWORD(wParam) == EN_CHANGE)
                    {
                        char text[128];

                        Edit_GetText(hwndStreamInterval, text, sizeof(text));

                        currentValues.streamInterval = atoi(text);

                        if(memcmp(  &originalValues, 
                                    &currentValues, 
                                    sizeof(PrefsStruct)))
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

                case IDC_SAVESTREAMS:
                {
                    BOOL enabled;

                    if(Button_GetCheck(hwndSaveStreams) == BST_CHECKED)
                    {
                        currentValues.saveStreams = true;
                    }
                    else
                    {
                        currentValues.saveStreams = false;
                    }

                    enabled = (currentValues.saveStreams ? TRUE : FALSE);

                    Button_Enable(hwndSaveStreamsDirectory, enabled); 
                    Button_Enable(hwndBrowse, enabled);
                    Button_Enable(  hwndSaveLocationText,enabled);


                    if(memcmp(  &originalValues, 
                                &currentValues, 
                                sizeof(PrefsStruct)))
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
                        memset(currentValues.saveStreamsDirectory, 0x00, MAX_PATH);
                        Edit_GetText(   hwndSaveStreamsDirectory, 
                                        currentValues.saveStreamsDirectory,
                                        MAX_PATH);

                        if(memcmp(  &originalValues, 
                                    &currentValues, 
                                    sizeof(PrefsStruct)))
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
                            memset( currentValues.saveStreamsDirectory, 
                                    0x00, 
                                    MAX_PATH);

                            SHGetPathFromIDList(browseId,
                                                currentValues.saveStreamsDirectory);
                            
                            Edit_SetText(   hwndSaveStreamsDirectory, 
                                            currentValues.saveStreamsDirectory);

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
                        currentValues.useProxyServer = true;
                    }
                    else
                    {
                        currentValues.useProxyServer = false;
                    }

                    enabled = (currentValues.useProxyServer ? TRUE : FALSE);

                    Button_Enable(hwndProxyServerAddress, enabled);

                    Button_Enable(hwndProxyServerAddressText, enabled);

                    Button_Enable(hwndProxyServerPort, enabled);

                    Button_Enable(hwndProxyServerPortText, enabled);

                    Button_Enable(hwndColon, enabled);


                    if(memcmp(  &originalValues, 
                                &currentValues, 
                                sizeof(PrefsStruct)))
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
                        char port[6];

                        memset(currentValues.proxyServer, 0x00, MAX_PATH);
                        Edit_GetText(   hwndProxyServerAddress, 
                                        currentValues.proxyServer,
                                        MAX_PATH);

                        Edit_GetText(   hwndProxyServerPort, 
                                        port,
                                        sizeof(port));

                        if(*port)
                        {
                            strcat(currentValues.proxyServer, ":");
                            strcat(currentValues.proxyServer, port);
                        }

                        if(memcmp(  &originalValues, 
                                    &currentValues, 
                                    sizeof(PrefsStruct)))
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
                        currentValues.useAlternateIP = true;
                    }
                    else
                    {
                        currentValues.useAlternateIP = false;
                    }

                    enabled = (currentValues.useAlternateIP ? TRUE : FALSE);

                    Button_Enable(hwndUseAlternateIPText, enabled);
                    Button_Enable(hwndAlternateIPAddress1, enabled);
                    Button_Enable(hwndPeriod1, enabled);
                    Button_Enable(hwndAlternateIPAddress2, enabled);
                    Button_Enable(hwndPeriod2, enabled);
                    Button_Enable(hwndAlternateIPAddress3, enabled);
                    Button_Enable(hwndPeriod3, enabled);
                    Button_Enable(hwndAlternateIPAddress4, enabled);
                    


                    if(memcmp(  &originalValues, 
                                &currentValues, 
                                sizeof(PrefsStruct)))
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

                        memset(currentValues.alternateIP, 0x00, 16);
                        Edit_GetText(hwndAlternateIPAddress1, ip, 4);

                        if(*ip)
                        {
                            strcat(currentValues.alternateIP, ip);
                        }
                        else
                        {
                            strcat(currentValues.alternateIP, "0");
                        }

                        strcat(currentValues.alternateIP, ".");


                        Edit_GetText(hwndAlternateIPAddress2, ip, 4);

                        if(*ip)
                        {
                            strcat(currentValues.alternateIP, ip);
                        }
                        else
                        {
                            strcat(currentValues.alternateIP, "0");
                        }

                        strcat(currentValues.alternateIP, ".");

                        Edit_GetText(hwndAlternateIPAddress3, ip, 4);

                        if(*ip)
                        {
                            strcat(currentValues.alternateIP, ip);
                        }
                        else
                        {
                            strcat(currentValues.alternateIP, "0");
                        }

                        strcat(currentValues.alternateIP, ".");

                        Edit_GetText(hwndAlternateIPAddress4, ip, 4);

                        if(*ip)
                        {
                            strcat(currentValues.alternateIP, ip);
                        }
                        else
                        {
                            strcat(currentValues.alternateIP, "0");
                        }

                        if(memcmp(  &originalValues, 
                                    &currentValues, 
                                    sizeof(PrefsStruct)))
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
                case PSN_SETACTIVE:
                {
                    
                    break;
                }

                case PSN_APPLY:
                {
                    SavePrefsValues(prefs, &currentValues);
                    break;
                }

                case PSN_KILLACTIVE:
                {
                    
                    break;
                }

                case PSN_RESET:
                {
                    SavePrefsValues(prefs, &originalValues);
                    break;
                }
            }

            break;
        }
    }

    return result;
}

static
BOOL 
CALLBACK 
PrefPage3Proc(  HWND hwnd, 
                UINT msg, 
                WPARAM wParam, 
                LPARAM lParam)
{
    UINT result = 0;
    static PROPSHEETPAGE* psp = NULL;
    static Preferences* prefs = NULL;
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
            // remember these for later...
            psp = (PROPSHEETPAGE*)lParam;
            prefs = (Preferences*)psp->lParam;

            // get the handles to all our controls
            hwndLog = GetDlgItem(hwnd, IDC_LOG);
            hwndLogDecoder = GetDlgItem(hwnd, IDC_LOGDECODER);
            hwndLogInput = GetDlgItem(hwnd, IDC_LOGINPUT);
            hwndLogOutput = GetDlgItem(hwnd, IDC_LOGOUTPUT);
            hwndLogMain = GetDlgItem(hwnd, IDC_LOGMAIN);
            hwndLogPerformance = GetDlgItem(hwnd, IDC_LOGPERFORMANCE);

            // initialize our controls
            bool value;

            value = originalValues.enableLogging;

            Button_SetCheck(hwndLog, value); 
            Button_Enable(hwndLogDecoder, value); 
            Button_Enable(hwndLogInput, value);
            Button_Enable(hwndLogOutput, value);
            Button_Enable(hwndLogMain, value);
            Button_Enable(hwndLogPerformance, value);

            Button_SetCheck(hwndLogMain, originalValues.logMain); 
            Button_SetCheck(hwndLogDecoder, originalValues.logDecoder); 
            Button_SetCheck(hwndLogInput, originalValues.logInput); 
            Button_SetCheck(hwndLogOutput, originalValues.logOutput); 
            Button_SetCheck(hwndLogPerformance, originalValues.logPerformance); 
                     
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
                        currentValues.enableLogging = true;
                       
                    }
                    else
                    {
                        currentValues.enableLogging = false;
                    }

                    enabled = (currentValues.enableLogging ? TRUE : FALSE);

                    Button_Enable(hwndLogDecoder, enabled); 
                    Button_Enable(hwndLogInput, enabled);
                    Button_Enable(hwndLogOutput, enabled);
                    Button_Enable(hwndLogMain, enabled);
                    Button_Enable(hwndLogPerformance, enabled);

                    if(memcmp(  &originalValues, 
                                &currentValues, 
                                sizeof(PrefsStruct)))
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
                        currentValues.logDecoder = true;
                    }
                    else
                    {
                        currentValues.logDecoder = false;
                    }

                    if(memcmp(  &originalValues, 
                                &currentValues, 
                                sizeof(PrefsStruct)))
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
                        currentValues.logInput = true;
                    }
                    else
                    {
                        currentValues.logInput = false;
                    }

                    if(memcmp(  &originalValues, 
                                &currentValues, 
                                sizeof(PrefsStruct)))
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
                        currentValues.logOutput = true;
                    }
                    else
                    {
                        currentValues.logOutput = false;
                    }

                    if(memcmp(  &originalValues, 
                                &currentValues, 
                                sizeof(PrefsStruct)))
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
                        currentValues.logPerformance = true;
                    }
                    else
                    {
                        currentValues.logPerformance = false;
                    }

                    if(memcmp(  &originalValues, 
                                &currentValues, 
                                sizeof(PrefsStruct)))
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
                        currentValues.logMain = true;
                    }
                    else
                    {
                        currentValues.logMain = false;
                    }

                    if(memcmp(  &originalValues, 
                                &currentValues, 
                                sizeof(PrefsStruct)))
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
                    SavePrefsValues(prefs, &currentValues);
                    break;
                }

                case PSN_KILLACTIVE:
                {
                    
                    break;
                }

                case PSN_RESET:
                {
                    SavePrefsValues(prefs, &originalValues);
                    break;
                }
            }

            break;
        }
    }

    return result;
}

static
BOOL 
CALLBACK 
PrefPage4Proc(  HWND hwnd, 
                UINT msg, 
                WPARAM wParam, 
                LPARAM lParam)
{
    UINT result = 0;
    static PROPSHEETPAGE* psp = NULL;
    static Preferences* prefs = NULL;
    
    switch(msg)
    {
        case WM_INITDIALOG:
        {
            // remember these for later...
            psp = (PROPSHEETPAGE*)lParam;
            prefs = (Preferences*)psp->lParam;
            
            break;
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
                    SavePrefsValues(prefs, &currentValues);
                    break;
                }

                case PSN_KILLACTIVE:
                {
                    
                    break;
                }

                case PSN_RESET:
                {
                    SavePrefsValues(prefs, &originalValues);
                    break;
                }
            }

            break;
        }
    }

    return result;
}

bool DisplayPreferences(HWND hwndParent, Preferences* prefs)
{
    bool result = false;
    PROPSHEETPAGE psp[4];
    PROPSHEETHEADER psh;
    HINSTANCE hinst = (HINSTANCE)GetWindowLong(hwndParent, GWL_HINSTANCE);

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = 0;
    psp[0].hInstance = hinst;
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_PREF1);
    psp[0].pszIcon = NULL;
    psp[0].pfnDlgProc = PrefPage1Proc;
    psp[0].pszTitle = NULL;
    psp[0].lParam = (LPARAM)prefs;

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = 0;
    psp[1].hInstance = hinst;
    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_PREF2);
    psp[1].pszIcon = NULL;
    psp[1].pfnDlgProc = PrefPage2Proc;
    psp[1].pszTitle = NULL;
    psp[1].lParam = (LPARAM)prefs;

    psp[2].dwSize = sizeof(PROPSHEETPAGE);
    psp[2].dwFlags = 0;
    psp[2].hInstance = hinst;
    psp[2].pszTemplate = MAKEINTRESOURCE(IDD_PREF3);
    psp[2].pszIcon = NULL;
    psp[2].pfnDlgProc = PrefPage3Proc;
    psp[2].pszTitle = NULL;
    psp[2].lParam = (LPARAM)prefs;

    psp[3].dwSize = sizeof(PROPSHEETPAGE);
    psp[3].dwFlags = 0;
    psp[3].hInstance = hinst;
    psp[3].pszTemplate = MAKEINTRESOURCE(IDD_PREF4);
    psp[3].pszIcon = NULL;
    psp[3].pfnDlgProc = PrefPage4Proc;
    psp[3].pszTitle = NULL;
    psp[3].lParam = (LPARAM)prefs;

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE;
    psh.hwndParent = hwndParent;
    psh.hInstance = hinst;
    psh.pszIcon = NULL;
    psh.pszCaption = "FreeAmp Preferences";
    psh.nPages = sizeof(psp)/sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = psp;
    psh.pfnCallback = NULL;

    GetPrefsValues(prefs, &originalValues);

    currentValues = originalValues;

    result = (PropertySheet(&psh) > 0);

    return result;
}

