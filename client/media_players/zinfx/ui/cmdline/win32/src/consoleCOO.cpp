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
	
	$Id: consoleCOO.cpp,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/

#include <iostream>
#include <stdio.h>

#include "consoleCOO.h"
#include "event.h"
#include "player.h"

#include "eventdata.h"

ConsoleCOO::ConsoleCOO() {}

ConsoleCOO::~ConsoleCOO() {
    //cout << "ConsoleCOO: begin deleted..." << endl;
}

void ConsoleCOO::setArgs(int a, char **b) {}


int32 ConsoleCOO::acceptCOOEvent(Event *e) {
    if (e) {
	//cout << "ConsoleCOO: processing event " << e->getEvent() << endl;
	switch (e->getEvent()) {
	    case INFO_MediaVitalStats: {
		MediaVitalInfo *pmvi = (MediaVitalInfo *)e->getArgument();
		if (pmvi) {
		}
		break; }
	    case CMD_Cleanup: {
		Event *e = new Event(INFO_ReadyToDieCOO);
		Player::getPlayer()->acceptEvent(*e);
		delete e;
		break; }
	    default:
		break;
	}
    }
    //cout << "Done cmdlinecoo: accpet event" << endl;
    return 0;
}






