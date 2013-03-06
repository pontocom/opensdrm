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
	
	$Id: lcdui.h,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_LcdUI_H_
#define INCLUDED_LcdUI_H_

#include "ui.h"
#include "event.h"
#include "thread.h"
#include "playlist.h"

enum {
    TIME_TOTAL = 0,
    TIME_CURR = 1,
    TIME_REMAIN = 2,
    TIME_MAX = 3
};

class LcdUI : public UserInterface {
 public:
    LcdUI(FAContext *context);
    virtual Error AcceptEvent(Event *);
    virtual Error Init(int32);
    static void keyboardServiceFunction(void *);
    virtual ~LcdUI();

 protected:
    FAContext *m_context;

 private:
    Properties *m_propManager;

    int32 m_sock;

    int32 m_startupType;
    int32 m_argc;
    char **m_argv;
    void ProcessArgs();
    void BlitTimeLine();
    int32 m_timeType;
    int32 m_currHours, m_currMinutes, m_currSeconds;
    int32 m_totalHours, m_totalMinutes, m_totalSeconds;
    EventQueue *m_playerEQ;
    void processSwitch(char *);
    Thread *keyboardListenThread;
    PlaylistManager *m_plm;
    Mutex *m_lcdLock;
    bool Quit;
};


#endif // _LcdUI_H_


