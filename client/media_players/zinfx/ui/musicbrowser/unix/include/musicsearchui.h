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

        $Id: musicsearchui.h,v 1.1 2004/10/13 16:16:15 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_MUSICSEARCHUI_H_
#define INCLUDED_MUSICSEARCHUI_H_

#include "config.h"

#include <gtk/gtk.h>

#include "facontext.h"

class MusicBrowserUI;

class musicsearchUI {
 public:
    musicsearchUI(FAContext *, MusicBrowserUI *parent);
   ~musicsearchUI();
  
    void Show(bool runMain = true);

    Error AcceptEvent(Event *);
 
    void SetSearchPath(string &newpath) { searchPath = newpath; }
    void SetSearchPath(const char *newpath) { searchPath = newpath; }

    void UpdateEntry(void);

    void StartSearch();
    void EndSearch();
    void DeleteEvent();
    void Close();

    bool custom;
    bool searchInProgress;
    bool searchDone;
    bool done;
	FAContext *GetContext(void) { return m_context; }
    GtkWidget *m_window;
 protected:
    FAContext *m_context;

 private:
    GtkWidget *m_searchButton;
    GtkWidget *textEntry;
    GtkWidget *browseButton;
    GtkWidget *buttonLabel;

    string searchPath;

    bool m_main;
    MusicBrowserUI *m_parent;
};
   

#endif
