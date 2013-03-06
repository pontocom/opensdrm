/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player

	Portions Copyright (C) 1998-1999 EMusic.com

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
	
	$Id: simpleui.h,v 1.1 2004/10/13 16:16:17 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_SIMPLE_UI_H_
#define INCLUDED_SIMPLE_UI_H_

/* system headers */
#include <stdlib.h>

/* project headers */
#include "config.h"
#include "ui.h"
#include "semaphore.h"
#include "event.h"
#include "thread.h"
#include "mutex.h"
#include "queue.h"
#include "facontext.h"
#include "preferences.h"

#define	UIState_Stopped 0
#define	UIState_Playing 1
#define	UIState_Paused  2


class SimpleUI : public UserInterface {
 public:
    SimpleUI(FAContext *context);
    ~SimpleUI();

    virtual Error Init(int32 startup_type);
    virtual Error AcceptEvent(Event*);
    void CreateUI();

    static BOOL CALLBACK MainProc(	HWND hwnd, 
						            UINT msg, 
						            WPARAM wParam, 
						            LPARAM lParam );
    
    void SetHwnd(HWND hwnd);
    void ReadPreferences();
    void AddTrayIcon();
    void RemoveTrayIcon();
    void SetTrayTooltip(char *str);

    Semaphore*      m_uiSemaphore;

    bool            m_scrolling;
    HICON           m_trayIcon;
    Preferences*    m_prefs;
    int32		    m_state;

 protected:
    void ParseArgs(int32 argc, char** argv);

    static void UIThreadFunc(void *);

    FAContext*		m_context;

 private:
    PlaylistManager*    m_plm;
    Properties*         m_propManager;
    int32			    m_totalSeconds;
    float			    m_secondsPerFrame;
    Thread*             m_uiThread;
    EventQueue*         m_target;

    bool                m_onTop;
    bool                m_liveInTray;
    char                m_trayTooltip[64];

    HWND            m_hwnd;
    HWND            m_hwndPlay;
	HWND            m_hwndStop;
	HWND            m_hwndPause;
	HWND            m_hwndNext;
	HWND            m_hwndLast;
	HWND            m_hwndSlider;
    HWND            m_hwndCurrent;
    HWND            m_hwndTotal;
	HWND            m_hwndStatus;

    
};


#endif // _SIMPLE_UI_H_
