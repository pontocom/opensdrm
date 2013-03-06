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
	
	$Id: irmanui.h,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_IRManUI_H_
#define INCLUDED_IRManUI_H_

#include "ui.h"
#include "event.h"
#include "thread.h"
#include "playlist.h"

#include <map>
using namespace std;

#define DEFAULT_DEVICE "/dev/irman"

class FAContext;

class IRManUI : public UserInterface {
 public:
    IRManUI(FAContext *context);
    virtual Error AcceptEvent(Event *);
    virtual Error Init(int32);
    static void irServiceFunction(void *);
    virtual ~IRManUI();

 protected:
    FAContext *m_context;

 private:
    map<string, int32>  m_commands;
    bool m_quitIRListen;
    Properties *m_propManager;
    int32 m_startupType;
    int32 m_argc;
    char **m_argv;
    void ProcessArgs();
    EventQueue *m_playerEQ;
    void processSwitch(char *);
    Thread *irListenThread;
    PlaylistManager *m_plm;
};


#endif // _IRManUI_H_


