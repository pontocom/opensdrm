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

        $Id: MissingFileDialog.h,v 1.1 2004/10/13 16:16:15 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_MISSINGFILEDLG_H_
#define INCLUDED_MISSINGFILEDLG_H_

#include <windows.h>
#include <commctrl.h>

#include <vector>
#include <string>
#include <set>

using namespace std;

#include "config.h"

class FAContext;
class PlaylistItem;

class MissingFileDialog
{
 public:

    MissingFileDialog(FAContext* context, HWND hwnd, PlaylistItem* item);

    virtual ~MissingFileDialog();

    bool Show();

    BOOL DialogProc(HWND hwnd, 
                    UINT msg, 
                    WPARAM wParam, 
                    LPARAM lParam); 
 protected:

 private:
    FAContext* m_context;
    HWND m_hwnd;
    PlaylistItem* m_item;
    char m_path[MAX_PATH];
    
};

#endif // INCLUDED_MISSINGFILEDLG_H_

