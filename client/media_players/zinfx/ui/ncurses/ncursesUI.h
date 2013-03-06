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

    ncurses user interface by stephan auerhahn (palpatine@midi.net)

    this is a hacked up version of cmdlineUI.h
	
	$Id: ncursesUI.h,v 1.1 2004/10/13 16:16:17 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_NCURSESUI_H_
#define INCLUDED_NCURSESUI_H_

#include "ui.h"
#include "event.h"
#include "thread.h"
#include "playlist.h"
#include "eventdata.h"

/* ncurses / curses include */
#include <curses.h>
#ifndef NCURSES_VERSION
#define color_set(a,b) ;
#endif

class FAContext;

class ncursesUI : public UserInterface {
 public:
    ncursesUI(FAContext *context);
    virtual Error AcceptEvent(Event *);
    virtual Error Init(int32);
    static void keyboardServiceFunction(void *);
    virtual ~ncursesUI();

 protected:
   FAContext *m_context;

 private:
   Properties *m_propManager;
   void ProcessArgs();
   void showInfo();
   void help();
   void playlist();
   int32 m_startupLevel;
   int32 m_argc;
   char **m_argv;
   EventQueue *m_playerEQ;
   void processSwitch(char *);
   Thread *keyboardListenThread;
   PlaylistManager *m_plm;
   int32 m_lastIndexPlayed;
   bool m_id3InfoPrinted;
   float totalTime;
   float lastSeconds;
   int titleStart;
   int titleDir;
   char *title;
   int counter;

   bool cursesStarted;

   WINDOW *helpwin;
   bool helpwin_open;
   WINDOW *playlistwin;
   bool playlistwin_open;
};


#endif // _NCURSESUI_H_



