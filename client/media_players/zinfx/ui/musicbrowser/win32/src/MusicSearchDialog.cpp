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

        $Id: MusicSearchDialog.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
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
#include "utility.h"
#include "resource.h"
#include "Win32MusicBrowser.h"
#include "help.h"

BOOL CALLBACK MusicSearchDlgProc(HWND hwnd, 
                                 UINT msg, 
                                 WPARAM wParam, 
                                 LPARAM lParam )
{
    MusicBrowserUI* ui = (MusicBrowserUI*)GetWindowLong(hwnd, GWL_USERDATA);

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            ui = (MusicBrowserUI*)lParam;
            assert(ui != NULL);
            SetWindowLong(hwnd, GWL_USERDATA, (LONG)ui);
            break;
        }            
    }

    return ui->MusicSearchDlgProc(hwnd, msg, wParam, lParam);
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

const char* kAllDrives = "All Drives";
const char* kAllFolders = "All Folders";
const char* kOtherFolder = "Other...";
const char* kSelectOtherText = "Select a location...";

BOOL MusicBrowserUI::MusicSearchDlgProc(HWND hwnd, 
                                        UINT msg, 
                                        WPARAM wParam, 
                                        LPARAM lParam )
{
    BOOL result = FALSE;

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            HWND hwndDrives = GetDlgItem(hwnd, IDC_DRIVES);
            HWND hwndDirectory = GetDlgItem(hwnd, IDC_DIRECTORY);
            DWORD  dwDrives;
            char   szDrive[3] = "X:";
            int32  i, ret;

            i = ComboBox_AddString(hwndDrives, kAllDrives);
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
            ComboBox_AddString(hwndDrives, kOtherFolder);
            Edit_SetText(hwndDirectory, kAllFolders);

            break;
        }   
        
        case WM_HELP:
        {
            SendMessage(hwnd, WM_COMMAND, IDHELP, 0);
            result = TRUE;
            break;
        }

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDHELP:
                {
                    ShowHelp(m_context, Music_Browser_Drive_Selection);
                    break;
                }

                case IDCANCEL:
                    EndDialog(hwnd, FALSE);
                    break;

                case IDOK:
                {
                    HWND hwndDrives = GetDlgItem(hwnd, IDC_DRIVES);
                    HWND hwndDirectory = GetDlgItem(hwnd, IDC_DIRECTORY);
                    char temp[MAX_PATH];
                    BOOL endDialog = TRUE;

                    ComboBox_GetText(hwndDrives, 
                                     temp, 
                                     MAX_PATH);

                    BOOL allDrives = !strcmp(temp, kAllDrives);

                    if(allDrives)
                    {
                        DWORD  dwDrives;
                        char   szDrive[5];
                        int32  i, ret;

                        dwDrives = GetLogicalDrives();
                        for(i = 0; i < sizeof(DWORD) * 8; i++)
                        {
                           if (dwDrives & (1 << i))
                           {
                              szDrive[0] = 'A' + i;
                              szDrive[1] = ':';
                              szDrive[2] = '\\';
                              szDrive[3] = 0;
                              ret = GetDriveType(szDrive);
                              if (ret != DRIVE_CDROM && ret != DRIVE_REMOVABLE)
                              {
                                  m_searchPathList.push_back(szDrive);
                              }
                           }   
                        }
                    }
                    else
                    {
                        Edit_GetText(hwndDirectory, 
                                     temp,
                                     MAX_PATH);

                        if (strcmp(temp, kSelectOtherText)) 
                        {
                            m_searchPathList.push_back(temp);
                        } 
                        else 
                        {
                            MessageBox(m_hWnd, 
                                       "You must select a directory to search.",
                                       BRANDING, MB_OK);
                            endDialog = FALSE;
                        }
                    }

                    if (endDialog) 
                        EndDialog(hwnd, TRUE);

                    break;
                }

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
                        
                        BOOL enable = strcmp(temp, kAllDrives);

                        if(!enable)
                            strcpy(temp, kAllFolders);
                        else 
                        if (!strcmp(temp, kOtherFolder))
                            strcpy(temp, kSelectOtherText);
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
    }

    return result;


}
