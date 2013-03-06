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

        $Id: SavePlaylistDialog.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
____________________________________________________________________________*/

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

static
LRESULT WINAPI 
EditWndProc( HWND hwnd, 
             UINT msg, 
             WPARAM wParam, 
             LPARAM lParam)
{
    WNDPROC lpOldProc = (WNDPROC)GetProp(hwnd, "oldproc" );

    switch(msg)
	{
		case WM_DESTROY:   
		{
			//  Put back old window proc and
			SetWindowLong( hwnd, GWL_WNDPROC, (DWORD)lpOldProc );

			// remove window property
			RemoveProp( hwnd, "oldproc" ); 

			break;
        }

        /*case WM_GETDLGCODE:
        {
            return DLGC_WANTALLKEYS;
        }*/

        case WM_CHAR:
        {
            if( wParam == '\\'  ||
                wParam == '/'   ||
                wParam == ':'   ||
                wParam == '*'   ||
                wParam == '?'   ||
                wParam == '\"'  ||
                wParam == '<'   ||
                wParam == '>'   ||
                wParam == '|' )
            {
                return 1;
            }
            

            break;
        }


    }

    return CallWindowProc(lpOldProc, hwnd, msg, wParam, lParam );
}



BOOL CALLBACK SavePlaylistDlgProc(HWND hwnd, 
                                  UINT msg, 
                                  WPARAM wParam, 
                                  LPARAM lParam )
{
    BOOL result = FALSE;
    static char* szName = NULL;

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            HWND hwndName = GetDlgItem(hwnd, IDC_NAME);

            szName = (char*)lParam;
            
            SetFocus(hwndName);
            Edit_SetText(hwndName, szName);
            Edit_SetSel(hwndName, 0, -1);

            SetProp(hwndName, 
                    "oldproc",
                    (HANDLE)GetWindowLong(hwndName, GWL_WNDPROC));

	        // Subclass the window so we can handle bad characters
	        SetWindowLong(hwndName, 
			              GWL_WNDPROC, 
                          (DWORD)::EditWndProc);  
            
            return FALSE;
            break;
        }      

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDCANCEL:
                    EndDialog(hwnd, FALSE);
                    break;

                case IDOK:
                {
                    HWND hwndName = GetDlgItem(hwnd, IDC_NAME);
                    
                    Edit_GetText(hwndName, 
                                 szName,
                                 MAX_PATH);

                    EndDialog(hwnd, TRUE);
                    break;
                }
            }
  
            break;
        }
    }

    return result;
}