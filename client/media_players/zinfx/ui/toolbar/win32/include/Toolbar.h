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
	
	$Id: Toolbar.h,v 1.1 2004/10/13 16:16:17 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_TOOLBAR_UI_H_
#define INCLUDED_TOOLBAR_UI_H_

/* system headers */
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <stdlib.h>

using namespace std;

/* project headers */   
#include "config.h"
#include "ui.h"
#include "event.h"
#include "errors.h"
#include "properties.h"
#include "preferences.h"
#include "facontext.h"

class FAContext;

class ToolbarUI : public UserInterface 
{
 public:
    ToolbarUI(FAContext *context);
    ~ToolbarUI();

    virtual Error Init(int32 startup_type) { return kError_NoErr;}
    virtual Error AcceptEvent(Event*);

    void InitMenuPopup(HMENU menuHandle, uint32 position, bool systemMenu);
    void TrayNotify(int32 notifyMessage);

 protected:

    void AddTrayIcon();
    void RemoveTrayIcon();
    void SetTrayTooltip(char *str);

 private:

    HICON               m_trayIcon;
    char                m_trayTooltip[64];
    FAContext*          m_context;
    HWND                m_hwnd;
};


#endif /* _TOOLBAR_UI_H_ */
