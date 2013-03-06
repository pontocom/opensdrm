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
	
	$Id: win32thread.h,v 1.1 2004/10/13 16:16:05 linha2 Exp $
____________________________________________________________________________*/


#ifndef INCLUDED_WIN32_THREAD_H
#define INCLUDED_WIN32_THREAD_H

#include <windows.h>
#include "config.h"
#include "thread.h"


class win32Thread : public Thread {

public:
	win32Thread();
	~win32Thread();


	virtual bool Create(thread_function function, void* arg,
                            bool detach = false);
	virtual void Destroy();
	virtual void Suspend();
	virtual void Resume();
	virtual void Join();

	virtual uint32 GetPriority() const;
	virtual uint32 SetPriority(uint32 priority);

    static unsigned long __stdcall internalThreadFunction(void* arg);

    uint32 InternalThreadFunction(); 

private:
	HANDLE		    m_threadHandle;	
	unsigned long	m_threadId;
    thread_function m_function;
    void*           m_arg;


};

#endif /* _WIN32_THREAD_H */
