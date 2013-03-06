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
	
	$Id: thread.h,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#ifndef THREAD_H
#define THREAD_H

#include "config.h"


typedef void (*thread_function)(void * arg);


class Thread {

public:
    virtual ~Thread(){}


    virtual bool Create(thread_function function, void* arg, 
                        bool detach = false) = 0;
    virtual void Destroy() = 0;
    virtual void Suspend() = 0;
    virtual void Resume() = 0;
    virtual void Join() = 0;

    virtual uint32 GetPriority() const = 0;
    virtual uint32 SetPriority(uint32 priority) = 0;

    static Thread* CreateThread();
};



#endif /* THREAD_H */
