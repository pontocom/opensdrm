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

        $Id: EditTrackInfoDialog.h,v 1.1 2004/10/13 16:16:15 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_EDITTRACKINFO_H_
#define INCLUDED_EDITTRACKINFO_H_

#include <windows.h>
#include <commctrl.h>

#include <vector>
#include <string>
#include <set>

using namespace std;

#include "config.h"
#include "musiccatalog.h"
#include "DataIndex.h"

class FAContext;

class EditTrackInfoDialog
{
 public:

    EditTrackInfoDialog(FAContext* context,
                        HINSTANCE hinst,
                        HWND hwnd, 
                        const vector<ArtistList*>* artistList, 
                        MetaData* editMetaData,
                        const char* location);

    virtual ~EditTrackInfoDialog();

    bool Show();

    BOOL DialogProc(HWND hwnd, 
                    UINT msg, 
                    WPARAM wParam, 
                    LPARAM lParam); 
 protected:
    void CreateEditInfoLists(set<string>& artists,
                             set<string>& albums,
                             set<string>& genres);

 private:
    FAContext* m_context;
    HINSTANCE m_hinst;
    HWND m_hwnd;
    const vector<ArtistList*>* m_artistList; 
    MetaData* m_editMetaData;
    const char* m_location;
};

#endif

