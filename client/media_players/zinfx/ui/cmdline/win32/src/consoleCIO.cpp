/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player

	Portions Copyright (C) 1998 EMusic.com

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
	
	$Id: consoleCIO.cpp,v 1.1 2004/10/13 16:16:13 linha2 Exp $
____________________________________________________________________________*/

#include <iostream>
#include <stdio.h>
#include <conio.h>

#include "ConsoleCIO.h"
#include "event.h"
#include "thread.h"

#define stdinfd 0

ConsoleCIO::ConsoleCIO() 
{
    keyboardListenThread = new Thread();
    keyboardListenThread->Create(ConsoleCIO::keyboardServiceFunction,this);
    
    cout << endl << "Command Line Interface" << endl << endl;
    cout << " * q    Quit" << endl;
    cout << " * +/=  Next Song" << endl;
    cout << " * -    Prev Song" << endl;
    cout << " * p    Pause / UnPause" << endl << endl;

}


ConsoleCIO::~ConsoleCIO() 
{
    if (keyboardListenThread) {
	keyboardListenThread->Destroy();
	delete keyboardListenThread;
	keyboardListenThread = NULL;
    }
}

void ConsoleCIO::keyboardServiceFunction(void *pclcio) {
    ConsoleCIO *pMe = (ConsoleCIO *)pclcio;
    char *pkey = new char[1];
    char chr;
    for (;;) 
	{
	chr = getch();

	switch (chr) {
	    case 'p':
	    case 'P': {
		Event *e = new Event(CMD_TogglePause);
		Player::getPlayer()->acceptEvent(*e);
		delete e;
		break;
	    }
	    case '-': {
		Event *e = new Event(CMD_PrevMediaPiece);
		Player::getPlayer()->acceptEvent(*e);
		e = new Event(CMD_Play);
		Player::getPlayer()->acceptEvent(*e);
		delete e;
		break;
	    }
	    case '=':
	    case '+':
	    case 'n':
	    case 'N': {
		Event *e = new Event(CMD_NextMediaPiece);
		Player::getPlayer()->acceptEvent(*e);
		e = new Event(CMD_Play);
		Player::getPlayer()->acceptEvent(*e);
		delete e;
		break; }
	    case 'q':
	    case 'Q': {
		Event *e = new Event(CMD_QuitPlayer);
		Player::getPlayer()->acceptEvent(*e);
		delete e;
		break; }
	    default:
		break;
	}
#if 0
	} else {
	    switch (errno) {
		case EIO: {
		    Event *e = new Event(CMD_QuitPlayer);
		    Player::getPlayer()->acceptEvent(*e);
		    delete e;
		    break; }
		default:
		    break;
	    }
	}
#endif
    }

	return 0;

}

int32 ConsoleCIO::acceptCIOEvent(Event *e) {
    if (e) {
	//cout << "ConsoleCIO: processing event " << e->getEvent() << endl;
	switch (e->getEvent()) {
	    case INFO_PlayListDonePlay: {
		Event *e = new Event(CMD_QuitPlayer);
		Player::getPlayer()->acceptEvent(*e);
		delete e;
		break; }
	    case CMD_Cleanup: {
		Event *e = new Event(INFO_ReadyToDieCIO);
		Player::getPlayer()->acceptEvent(*e);
		delete e;
		break; }
	    default:
		break;
	}
    }
    return 0;
}

void ConsoleCIO::setArgs(int argc, char **argv) {
    PlayList *pl = new PlayList();
    char *pc = NULL;
    for(int i=1;i<argc;i++) {
	//cout << "Adding arg " << i << ": " << argv[i] << endl;
	pc = argv[i];
	if (pc[0] == '-') {
	    processSwitch(&(pc[0]));
	} else {
	    pl->add(pc);
	}
    }
    pl->setFirst();
    Event *e = new Event(CMD_SetPlaylist,pl);
    Player::getPlayer()->acceptEvent(*e);
    delete e;
    e = new Event(CMD_Play);
    Player::getPlayer()->acceptEvent(*e);
    delete e;
}

void ConsoleCIO::processSwitch(char *pc) {
    return;
}






