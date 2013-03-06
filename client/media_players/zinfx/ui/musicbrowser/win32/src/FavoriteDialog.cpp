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

    $Id: FavoriteDialog.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
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
#include <assert.h>

// project includes
#include "config.h"
#include "FAContext.h"
#include "playlist.h"
#include "musiccatalog.h"
#include "utility.h"
#include "resource.h"
#include "FavoriteDialog.h"
#include "help.h"


FavoriteDialog::FavoriteDialog(FAContext* context, 
                               HWND hwnd, 
                               PlaylistItem* item):
    m_context(context), m_hwnd(hwnd), m_item(item)
{

}

FavoriteDialog::~FavoriteDialog()
{

}

static
BOOL CALLBACK FavoriteDlgProc(HWND hwnd,
                              UINT msg,
                              WPARAM wParam,
                              LPARAM lParam)
{
    FavoriteDialog* _this = 
        (FavoriteDialog*)GetWindowLong(hwnd, GWL_USERDATA);

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            _this = (FavoriteDialog*)lParam;
            assert(_this != NULL);
            SetWindowLong(hwnd, GWL_USERDATA, (LONG)_this);
            break;
        }            
    }

    return _this->DialogProc(hwnd, msg, wParam, lParam);
}    

bool FavoriteDialog::Show()   
{
    bool result = false;
    HINSTANCE hinst = (HINSTANCE)GetWindowLong(m_hwnd, GWL_HINSTANCE);

    result = (0 < DialogBoxParam(
                          hinst, 
                          MAKEINTRESOURCE(IDD_FAVORITE),
                          m_hwnd, 
                          ::FavoriteDlgProc, 
                          (LPARAM )this));
    return result;
}

BOOL FavoriteDialog::DialogProc(HWND hwnd, 
                                UINT msg, 
                                WPARAM wParam, 
                                LPARAM lParam )
{
    BOOL result = FALSE;

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            HWND hwndName = GetDlgItem(hwnd, IDC_NAME);
            HWND hwndURL = GetDlgItem(hwnd, IDC_URL);
            HWND hwndDescription = GetDlgItem(hwnd, IDC_DESCRIPTION);

            if(m_item)
            {
                SetWindowText(hwndName, m_item->GetMetaData().Title().c_str());
                SetWindowText(hwndURL, m_item->URL().c_str());
                SetWindowText(hwndDescription, m_item->GetMetaData().Comment().c_str());
            }
            
            SetFocus(hwndName);

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

                case IDCANCEL:
                {
                    EndDialog(hwnd, FALSE);
                    break;
                }

                case IDOK:
                {
                    HWND hwndName = GetDlgItem(hwnd, IDC_NAME);
                    HWND hwndURL = GetDlgItem(hwnd, IDC_URL);
                    HWND hwndDescription = GetDlgItem(hwnd, IDC_DESCRIPTION);
                    bool newItem = (m_item == NULL);

                    if(newItem)
                    {
                        m_item = new PlaylistItem;
                    }
                    else
                    {
                        m_context->catalog->RemoveStream(m_item->URL().c_str()); 
                    }
                    
                    MetaData metadata;
                    char* buf;
                    uint32 size;

                    size = GetWindowTextLength(hwndName) + 1;
                    buf = new char[size];
                    GetWindowText(hwndName, buf, size);
                    metadata.SetTitle(buf);
                    delete [] buf;

                    size = GetWindowTextLength(hwndDescription) + 1;
                    buf = new char[size];
                    GetWindowText(hwndDescription, buf, size);
                    metadata.SetComment(buf);
                    delete [] buf;

                    size = GetWindowTextLength(hwndURL) + 1;
                    buf = new char[size];
                    GetWindowText(hwndURL, buf, size);
                    m_item->SetURL(buf);
                    delete [] buf;

                    m_context->catalog->WriteMetaDataToDatabase(m_item->URL().c_str(),
                                                                metadata,
                                                                kTypeStream);

                    m_context->catalog->AddStream(m_item->URL().c_str());

                    if(newItem)
                    {
                        delete m_item;
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

