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
	
	$Id: consoleCIO.h,v 1.1 2004/10/13 16:16:13 linha2 Exp $
____________________________________________________________________________*/

// CommandLineCIO.h


#ifndef INCLUDED_CONSOLE_CIO_H_
#define INCLUDED_CONSOLE_CIO_H_

#include "ui.h"
#include "event.h"
#include "thread.h"

class ConsoleCIO : public CIO {
 public:
    ConsoleCIO();
    virtual int32 acceptCIOEvent(Event *);
    virtual void setArgs(int argc, char **argv);
    ~ConsoleCIO();

	static void keyboardServiceFunction(void *);
 private:
    void processSwitch(char *);
    Thread *keyboardListenThread;

};


#endif // _CONSOLE_CIO_H_
