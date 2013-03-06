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

        $Id: IntroductionWizard.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

// system includes
#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>

// project includes
#include "config.h"
#include "preferences.h"
#include "resource.h"
#include "Win32MusicBrowser.h"

static bool useRelatable = true;
static bool useMusicBrainz = true;

static BOOL CALLBACK IntroWizardHello(HWND hwnd, 
                                      UINT msg, 
                                      WPARAM wParam, 
                                      LPARAM lParam)
{
    BOOL result = FALSE;

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
            const char* kCaption1 = "Welcome!";
            const char* kCaption2 = "What is \'My Music\'?";
            const char* kMsg1 = "This wizard will help you begin organizing your "
                                "music collection and get you started playing "
                                "your music.";
            const char* kMsg2 = "My Music helps you organize the music you have "
                                "on your computer. The My Music window is divided "
                                "into two panes: "
                                "My Music Collection and Currently Listening To.";
                                //"\r\n"
            const char* kMsg3 = "My Music Collection provides a convenient, "
                                "organized view of the music you have on your "
                                "computer. It might help to think of this pane "
                                "as a CD rack for your computer.";
                                //"\r\n"
            const char* kMsg4 = "Currently Listening To displays a list of the "
                                "songs you have chosen to play. In order to listen "
                                "to music all you have to do is add items to the "
                                "list by dragging them from the My Music Collection "
                                "pane on the left to the Currently Listening To "
                                "pane on the right.";
                                //"\r\n"
            const char* kMsg5 = "For a more detailed explanation of how to use "
                                "the My Music window you should access the "
                                "application\'s help system through the Help menu "
                                "or by clicking the \'?\' in the main player window.";


            switch(ctrlId)
            {
                case IDC_CAPTION1:
                case IDC_CAPTION2:
                {
                    HFONT font, oldFont;

                    LOGFONT lf;

                    GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);

                    lf.lfWeight = FW_BOLD;

                    font = CreateFontIndirect(&lf);

                    oldFont = (HFONT)SelectObject(dis->hDC, font);

                    RECT clientRect;
                    GetClientRect(dis->hwndItem, &clientRect);

                    const char* caption;

                    if(ctrlId == IDC_CAPTION1)
                        caption = kCaption1;
                    else if(ctrlId == IDC_CAPTION2)
                        caption = kCaption2;

                    DrawText(dis->hDC, 
                             caption,
                             strlen(caption),
                             &clientRect,
                             DT_LEFT|DT_WORDBREAK);

                    SelectObject(dis->hDC, oldFont);

                    DeleteObject(font);

                    break;
                } 

                case IDC_TEXT1:
                {
                    HFONT font, oldFont;

                    font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

                    oldFont = (HFONT)SelectObject(dis->hDC, font);

                    RECT clientRect;
                    GetClientRect(dis->hwndItem, &clientRect);

                    DrawText(dis->hDC, 
                             kMsg1,
                             strlen(kMsg1),
                             &clientRect,
                             DT_LEFT|DT_WORDBREAK);

                    SelectObject(dis->hDC, oldFont);

                    DeleteObject(font);
                    break;
                }

                case IDC_TEXT2:
                {
                    HFONT font, oldFont;

                    font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

                    oldFont = (HFONT)SelectObject(dis->hDC, font);

                    RECT clientRect;
                    GetClientRect(dis->hwndItem, &clientRect);

                    RECT halfHeightRect = clientRect;
                    int halfHeight = DrawText(
                                         dis->hDC, 
                                         kMsg2,
                                         strlen(kMsg2),
                                         &halfHeightRect,
                                         DT_LEFT|DT_SINGLELINE|DT_CALCRECT)/2;
                    int height;

                    height = DrawText(
                             dis->hDC, 
                             kMsg2,
                             strlen(kMsg2),
                             &clientRect,
                             DT_LEFT|DT_WORDBREAK);

                    clientRect.top += height + halfHeight;

                    height = DrawText(
                             dis->hDC, 
                             kMsg3,
                             strlen(kMsg3),
                             &clientRect,
                             DT_LEFT|DT_WORDBREAK);

                    clientRect.top += height + halfHeight;

                    height = DrawText(
                             dis->hDC, 
                             kMsg4,
                             strlen(kMsg4),
                             &clientRect,
                             DT_LEFT|DT_WORDBREAK);

                    clientRect.top += height + halfHeight;

                    height = DrawText(
                             dis->hDC, 
                             kMsg5,
                             strlen(kMsg5),
                             &clientRect,
                             DT_LEFT|DT_WORDBREAK);

                    SelectObject(dis->hDC, oldFont);

                    DeleteObject(font);
                    break;
                }
            }

            break;
        }

        case WM_NOTIFY:
        {
            switch(((NMHDR*)lParam)->code)
            {
                case PSN_KILLACTIVE:
                {
                    SetWindowLong(hwnd,	DWL_MSGRESULT, FALSE);
                    result = TRUE;
                    break;
                }

                case PSN_RESET:
                {
                    SetWindowLong(hwnd,	DWL_MSGRESULT, FALSE);
                    break;
                }

                case PSN_SETACTIVE:
                {
                    PropSheet_SetWizButtons(GetParent(hwnd), PSWIZB_NEXT);
                    break;
                }

                case PSN_WIZNEXT:
                {
                    break;
                }

                case PSN_WIZBACK:
                {
                    break;
                }
            }

            break;
        }

    }

	return result;
}   

static int CALLBACK BrowseCallbackProc(HWND hwnd, 
                                       UINT uMsg, 
                                       LPARAM lParam,
                                       LPARAM lpData)
{
        // Called just after the dialog is initialized
        // Select the dir passed in BROWSEINFO.lParam
        // TAKE CARE THAT IF THE DIR STRING ENDS WITH \ IT WILL NOT WORK ???
        if (uMsg == BFFM_INITIALIZED)
                ::SendMessage(hwnd, BFFM_SETSELECTION, FALSE, lpData);

        return 0;
}

//const char* kAllDrives = "All Drives";
//const char* kAllFolders = "All Folders";


static BOOL CALLBACK IntroWizardSearch( HWND hwnd,
                                        UINT msg, 
                                        WPARAM wParam, 
                                        LPARAM lParam)
{
	BOOL result = FALSE;
    static vector<string>* searchPaths;

    switch(msg)
    {
        case WM_INITDIALOG:
        {
            HWND hwndDrives = GetDlgItem(hwnd, IDC_DRIVES);
            HWND hwndDirectory = GetDlgItem(hwnd, IDC_DIRECTORY);
            DWORD  dwDrives;
            //char   szDrive[3] = "X:";
            char *szDrive = new char[4];
            memset(szDrive, 0, 4);
            szDrive[1] = ':';
            int32  i, ret;

            i = ComboBox_AddString(hwndDrives, "All Drives");
            ComboBox_SetCurSel(hwndDrives, i);

            dwDrives = GetLogicalDrives();
            for(i = 0; i < sizeof(DWORD) * 8; i++)
            {
               if (dwDrives & (1 << i))
               {
                  szDrive[0] = 'A' + i;
                  ret = GetDriveType(szDrive);
                  if (ret != DRIVE_CDROM && ret != DRIVE_REMOVABLE)
                  {
                      ComboBox_AddString(hwndDrives, szDrive);
                  }
               }   
            }

            ComboBox_AddString(hwndDrives, "Other...");
            Edit_SetText(hwndDirectory, "All Folders");

            PROPSHEETPAGE* psp = (PROPSHEETPAGE*)lParam;
            searchPaths = (vector<string>*)psp->lParam;

            delete [] szDrive;
            break;
        }

        case WM_DRAWITEM:
        {
            DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
            UINT ctrlId = wParam;
            const char* kCaption1 = "Search Computer For Music";
            const char* kCaption2 = "Where Would You Like to Look for Music?";
            const char* kMsg1 = "In order to populate the My Music Collection pane "
                                the_BRANDING" will search your computer for supported music "
                                "files. The files will not be moved or modified during "
                                "this process. Their location will be remembered in "
                                "order to provide you with an organized view of your "
                                "music collection.";
                               
            const char* kMsg2 = "By default "the_BRANDING" will search all the disk drives on "
                                "your computer for music. If you would like to limit the "
                                "scope of the search you may do so by selecting a "
                                "disk drive for us to search. If you wish, you may "
                                "also select a specific folder on that drive.";
                                
            switch(ctrlId)
            {
                case IDC_CAPTION1:
                case IDC_CAPTION2:
                {
                    HFONT font, oldFont;

                    LOGFONT lf;

                    GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);

                    lf.lfWeight = FW_BOLD;

                    font = CreateFontIndirect(&lf);

                    oldFont = (HFONT)SelectObject(dis->hDC, font);

                    RECT clientRect;
                    GetClientRect(dis->hwndItem, &clientRect);

                    const char* caption;

                    if(ctrlId == IDC_CAPTION1)
                        caption = kCaption1;
                    else if(ctrlId == IDC_CAPTION2)
                        caption = kCaption2;

                    DrawText(dis->hDC, 
                             caption,
                             strlen(caption),
                             &clientRect,
                             DT_LEFT|DT_WORDBREAK);

                    SelectObject(dis->hDC, oldFont);

                    DeleteObject(font);

                    break;
                } 

                case IDC_TEXT1:
                case IDC_TEXT2:
                {
                    HFONT font, oldFont;

                    font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

                    oldFont = (HFONT)SelectObject(dis->hDC, font);

                    RECT clientRect;
                    GetClientRect(dis->hwndItem, &clientRect);

                    const char* text;

                    if(ctrlId == IDC_TEXT1)
                        text = kMsg1;
                    else if(ctrlId == IDC_TEXT2)
                        text = kMsg2;

                    DrawText(dis->hDC, 
                             text,
                             strlen(text),
                             &clientRect,
                             DT_LEFT|DT_WORDBREAK);

                    SelectObject(dis->hDC, oldFont);

                    DeleteObject(font);
                    break;
                }
            }

            break;
        }

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDC_DRIVES:
                {
                    if(HIWORD(wParam) == CBN_CLOSEUP)
                    {
                        HWND hwndCombo = (HWND) lParam;
                        char temp[MAX_PATH];
                        int32 sel = ComboBox_GetCurSel(hwndCombo);

                        ComboBox_GetText( hwndCombo, 
                                          temp, 
                                          MAX_PATH);

                        HWND hwndDirectory = GetDlgItem(hwnd, IDC_DIRECTORY);
                        HWND hwndBrowse = GetDlgItem(hwnd, IDC_BROWSE);
                        
                        BOOL enable = strcmp(temp, "All Drives");

                        if(!enable)
                            strcpy(temp, "All Folders");
                        else 
                        if (!strcmp(temp, "Other..."))
                            strcpy(temp, "Select a location...");
                        else
                            sprintf(temp, "%s\\", temp);

                            
                        Edit_SetText(hwndDirectory, temp);

                        //EnableWindow(hwndText, enable);
                        //EnableWindow(hwndDirectory, enable);
                        EnableWindow(hwndBrowse, enable);
                    }

                    break;
                }          

                case IDC_DIRECTORY:
                {
                    /*if(HIWORD(wParam) == EN_CHANGE)
                    {
                        char temp[MAX_PATH];
                        HWND hwndEdit = (HWND) lParam;

                        Edit_GetText( hwndEdit, 
                                      temp,
                                      MAX_PATH);
                    }*/

                    break;
                }



                case IDC_BROWSE:
                {
                    LPMALLOC pMalloc;

                    if(SUCCEEDED(SHGetMalloc(&pMalloc)))
                    {
                        HWND hwndDrives = GetDlgItem(hwnd, IDC_DRIVES);
                        HWND hwndDirectory = GetDlgItem(hwnd, IDC_DIRECTORY);
                        char temp[MAX_PATH];

                        LPITEMIDLIST pidlDrive;

                        // get the PIDL for this dir and set the root
                        LPSHELLFOLDER desktop;

                        if(SUCCEEDED(SHGetDesktopFolder(&desktop)))
                        {
                            OLECHAR drive[MAX_PATH];
                            OLECHAR path[MAX_PATH];
                            ULONG eaten;
                            LPITEMIDLIST pidlPath;

                            ComboBox_GetText(hwndDrives, 
                                             temp, 
                                             MAX_PATH);

                            strcat(temp, "\\");

                            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, temp,
                                                -1, drive, sizeof(drive));

                            desktop->ParseDisplayName(hwnd, NULL, drive, &eaten, &pidlDrive, NULL);                            

                            Edit_GetText(hwndDirectory, 
                                         temp,
                                         MAX_PATH);

                            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, temp, 
                                                -1, path, sizeof(path));

                            desktop->ParseDisplayName(hwnd, NULL, path, &eaten, &pidlPath, NULL);

                            BROWSEINFO bi; 
                            LPITEMIDLIST browseId;
                            char displayName[MAX_PATH + 1];

                            bi.hwndOwner = hwnd;
                            bi.pidlRoot = pidlDrive;
                            bi.pszDisplayName = displayName;
                            bi.lpszTitle = "Please select the folder in which you want to search.";
                            bi.ulFlags = BIF_RETURNONLYFSDIRS;
                            bi.lpfn = BrowseCallbackProc;
                            bi.lParam = (LPARAM)pidlPath;

                            browseId = SHBrowseForFolder(&bi);
        
                            if(browseId)
                            {
                                SHGetPathFromIDList(browseId, temp);
                        
                                Edit_SetText(hwndDirectory, temp);

                                pMalloc->Free(browseId);
                            }

                            // clean up
                            if(pidlPath)
                                pMalloc->Free(pidlPath);

                            if(pidlDrive)
                                pMalloc->Free(pidlDrive);

                            desktop->Release();
                        }                        
                    }

                    break;
                }
            }

            break;
        }
                 
        case WM_NOTIFY:
        {
            switch(((NMHDR*)lParam)->code)
            {
                case PSN_KILLACTIVE:
                {
                    SetWindowLong(hwnd,	DWL_MSGRESULT, FALSE);
                    result = TRUE;
                    break;
                }

                case PSN_RESET:
                {
                    SetWindowLong(hwnd,	DWL_MSGRESULT, FALSE);
                    break;
                }

                case PSN_SETACTIVE:
                {
                    
                    PropSheet_SetWizButtons(GetParent(hwnd), PSWIZB_BACK | PSWIZB_FINISH);
                    //HWND hwndFinish = GetDlgItem(GetParent(hwnd), 3025);
                    HWND hwndFinish = FindWindowEx(GetParent(hwnd), NULL, NULL, "Finish");

                    SetWindowText(hwndFinish, "Search");
                    break;
                }

                case PSN_WIZFINISH:
                {
                    HWND hwndDrives = GetDlgItem(hwnd, IDC_DRIVES);
                    HWND hwndDirectory = GetDlgItem(hwnd, IDC_DIRECTORY);
                    char temp[MAX_PATH];

                    ComboBox_GetText(hwndDrives, 
                                     temp, 
                                     MAX_PATH);

                    BOOL allDrives = !strcmp(temp, "All Drives");

                    if(allDrives)
                    {
                        DWORD  dwDrives;
                        char   *szDrive = new char[5];
						memset(szDrive, 0x00, sizeof(char)*5);
                        szDrive[1] = ':';
						int32  i, ret;

                        dwDrives = GetLogicalDrives();
                        for(i = 0; i < sizeof(DWORD) * 8; i++)
                        {
                           if (dwDrives & (1 << i))
                           {
                              szDrive[0] = 'A' + i;
                              ret = GetDriveType(szDrive);
                              if (ret != DRIVE_CDROM && ret != DRIVE_REMOVABLE)
                              {
                                  searchPaths->push_back(szDrive);
                              }
                           }   
                        }
						delete [] szDrive;
                    }
                    else
                    {
                        Edit_GetText(hwndDirectory, 
                                     temp,
                                     MAX_PATH);

                        if (strcmp(temp, "Select a location...")) 
                        {
                            searchPaths->push_back(temp);
                        } 
                        else 
                        {
                            MessageBox(hwnd, "You must select a directory to search.",
                                       BRANDING, MB_OK);
                            SetWindowLong(hwnd, DWL_MSGRESULT, IDD_INTROWIZARD_SEARCH);
                            result = TRUE;
                        }
                    }
                    break;
                }

                case PSN_WIZBACK:
                {
                    if (!useRelatable)
                    {
                        SetWindowLong(hwnd, DWL_MSGRESULT, IDD_INTROWIZARD_RELATABLE);
                        result = TRUE;
                    }
					else
					if (!useMusicBrainz)
					{
	                    SetWindowLong(hwnd, DWL_MSGRESULT, IDD_INTROWIZARD_MUSICBRAINZ);
		                result = TRUE;
					}
                    break;
                }
            }

            break;
        }

    }

	return result;
}   

bool MusicBrowserUI::IntroductionWizard(vector<string>* searchPaths,
                                        APSInterface *pInterface)
{
    PROPSHEETPAGE psp[2];
    PROPSHEETHEADER psh;

    HINSTANCE hinst = (HINSTANCE)GetWindowLong(m_hWnd, GWL_HINSTANCE);

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = 0;
    psp[0].hInstance = hinst;
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_INTROWIZARD_HELLO);
    psp[0].pszIcon = NULL;
    psp[0].pfnDlgProc = IntroWizardHello;
    psp[0].pszTitle = NULL;
    psp[0].lParam = (LPARAM)0;   

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = 0;
    psp[1].hInstance = hinst;
    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_INTROWIZARD_SEARCH);
    psp[1].pszIcon = NULL;
    psp[1].pfnDlgProc = IntroWizardSearch;
    psp[1].pszTitle = NULL;
    psp[1].lParam = (LPARAM)searchPaths;

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_WIZARD | PSH_NOAPPLYNOW;
    psh.hwndParent = m_hWnd;
    psh.hInstance = hinst;
    psh.pszIcon = NULL;
    psh.pszCaption = "Welcome to "the_BRANDING;
    psh.nPages = sizeof(psp)/sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = psp;
    psh.pfnCallback = NULL;

    return (PropertySheet(&psh) > 0);
}
