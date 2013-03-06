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

    $Id: MissingFileDialog.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
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
#include <assert.h>

// project includes
#include "config.h"
#include "FAContext.h"
#include "playlist.h"
#include "utility.h"
#include "resource.h"
#include "MissingFileDialog.h"
#include "missingfile.h"
#include "help.h"


MissingFileDialog::MissingFileDialog(FAContext* context, 
                                     HWND hwnd, 
                                     PlaylistItem* item):
    m_context(context), m_hwnd(hwnd), m_item(item)
{
    *m_path = 0x00;
}

MissingFileDialog::~MissingFileDialog()
{

}

static
BOOL CALLBACK MissingFileDlgProc(HWND hwnd,
                                 UINT msg,
                                 WPARAM wParam,
                                 LPARAM lParam)
{
    MissingFileDialog* _this = 
        (MissingFileDialog*)GetWindowLong(hwnd, GWL_USERDATA);

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            _this = (MissingFileDialog*)lParam;
            assert(_this != NULL);
            SetWindowLong(hwnd, GWL_USERDATA, (LONG)_this);
            break;
        }            
    }

    return _this->DialogProc(hwnd, msg, wParam, lParam);
}    

bool MissingFileDialog::Show()   
{
    bool result = false;
    HINSTANCE hinst = (HINSTANCE)GetWindowLong(m_hwnd, GWL_HINSTANCE);

    result = (0 < DialogBoxParam(
                          hinst, 
                          MAKEINTRESOURCE(IDD_MISSINGTRACK),
                          m_hwnd, 
                          ::MissingFileDlgProc, 
                          (LPARAM )this));
    return result;
}

BOOL MissingFileDialog::DialogProc(HWND hwnd, 
                                   UINT msg, 
                                   WPARAM wParam, 
                                   LPARAM lParam )
{
    BOOL result = FALSE;

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            HWND hwndRemove = GetDlgItem(hwnd, IDC_REMOVE);
            HWND hwndPath = GetDlgItem(hwnd, IDC_PATH);
            HWND hwndBrowse = GetDlgItem(hwnd, IDC_BROWSE);
            HWND hwndMissing = GetDlgItem(hwnd, IDC_MISSINGTRACK);

            Button_SetCheck(hwndRemove, TRUE);
            SetFocus(hwndRemove);

            EnableWindow(hwndPath, FALSE);
            EnableWindow(hwndBrowse, FALSE);

            char path[MAX_PATH];
            uint32 size = sizeof(path);

            URLToFilePath(m_item->URL().c_str(), path, &size);

            SetWindowText(hwndMissing, path);

            result = TRUE;
            break;
        }      

        case WM_HELP:
        {
            ShowHelp(m_context, Edit_Info);
            result = TRUE;
            break;
        }

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDHELP:
                {
                    ShowHelp(m_context, Edit_Info);
                    break;
                }

                case IDC_REMOVE:
                case IDC_LOCATE:
                {
                    HWND hwndPath = GetDlgItem(hwnd, IDC_PATH);
                    HWND hwndBrowse = GetDlgItem(hwnd, IDC_BROWSE);

                    BOOL enable = (LOWORD(wParam) == IDC_REMOVE ? FALSE : TRUE);
                    EnableWindow(hwndPath, enable);
                    EnableWindow(hwndBrowse, enable);
                    break;
                }

                case IDC_BROWSE:
                {
                    OPENFILENAME ofn;
                    char initialDir[MAX_PATH] = {0x00};
                    uint32 size = sizeof(initialDir);
                    char file[MAX_PATH] = {0x00};

                    strcpy(file, m_path);

                    char filter[] = "MPEG Audio Streams (.mp1;.mp2;.mp3;.mpp)\0"
                                    "*.mp1;*.mp2;*.mp3;*.mpp\0"
                                    "Ogg/Vorbis Files (*.ogg)\0"
                                    "*.ogg\0"
                                    "All Files (*.*)\0"
                                    "*.*\0"
                                    "\0";

                    m_context->prefs->GetPrefString(kSaveMusicDirPref, initialDir, &size);

                    // Setup open file dialog box structure
                    ofn.lStructSize       = sizeof(OPENFILENAME);
                    ofn.hwndOwner         = hwnd;
                    ofn.hInstance         = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
                    ofn.lpstrFilter       = filter;
                    ofn.lpstrCustomFilter = NULL;
                    ofn.nMaxCustFilter    = 0;
                    ofn.nFilterIndex      = 1;
                    ofn.lpstrFile         = file;
                    ofn.nMaxFile          = sizeof(file);
                    ofn.lpstrFileTitle    = NULL;
                    ofn.nMaxFileTitle     = 0;
                    ofn.lpstrInitialDir   = initialDir;
                    ofn.lpstrTitle        = "Locate Missing Track";
                    ofn.Flags             = OFN_FILEMUSTEXIST | 
					                        OFN_PATHMUSTEXIST |
                                            OFN_HIDEREADONLY  | 
					                        OFN_EXPLORER;
                    ofn.nFileOffset       = 0;
                    ofn.nFileExtension    = 0;
                    ofn.lpstrDefExt       = "MP3";

                    if(GetOpenFileName(&ofn))
                    {
                        HWND hwndPath = GetDlgItem(hwnd, IDC_PATH);

                        SetWindowText(hwndPath, file);
                        strcpy(m_path, file);
                    }

                    break;
                }

                case IDCANCEL:
                {
		    m_context->plm->RemoveItem(m_item);
                    m_context->target->AcceptEvent(new Event(CMD_Stop));
                    EndDialog(hwnd, FALSE);
                    break;
                }

                case IDOK:
                {
                    HWND hwndRemove = GetDlgItem(hwnd, IDC_REMOVE);

                    BOOL remove = Button_GetCheck(hwndRemove);

                    if(remove)
                    {
                        m_context->plm->RemoveItem(m_item);
                    }
                    else
                    {
                        MissingFile mf(m_context);

                        char url[MAX_PATH + 7];
                        uint32 size = sizeof(url);

                        FilePathToURL(m_path, url, &size);
                        mf.AcceptLocation(m_item, url);
                    }
                                        
                    EndDialog(hwnd, TRUE);
                    break;
                }                  
            }
  
            break;
        }
    }

    return result;
}

