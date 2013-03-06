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

        $Id: PerformDBCheckDialog.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
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
#include "resource.h"

BOOL CALLBACK PerformDBCheckDlgProc(HWND hwnd, 
                                    UINT msg, 
                                    WPARAM wParam, 
                                    LPARAM lParam )
{
    BOOL result = FALSE;
    static bool* doCheck = NULL;

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            doCheck = (bool*)lParam;
            break;
        }      

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDCANCEL:
                {
                    HWND hwndCheck = GetDlgItem(hwnd, IDC_CHECK);
                    
                    *doCheck = !(BST_CHECKED == Button_GetCheck(hwndCheck));

                    EndDialog(hwnd, FALSE);
                    break;
                }

                case IDOK:
                {
                    HWND hwndCheck = GetDlgItem(hwnd, IDC_CHECK);
                    
                    *doCheck = !(BST_CHECKED == Button_GetCheck(hwndCheck));

                    EndDialog(hwnd, TRUE);
                    break;
                }
            }
  
            break;
        }
    }

    return result;
}