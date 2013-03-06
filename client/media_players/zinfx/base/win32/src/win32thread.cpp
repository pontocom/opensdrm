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
	
	$Id: win32thread.cpp,v 1.1 2004/10/13 16:16:05 linha2 Exp $
____________________________________________________________________________*/

#include <process.h>

#include "win32thread.h"

int32 priority_values[] = {
    -15,
    -2,
    -1,
    0,
    1,
    2,
    15
};

const uint32 kNumPriorities = sizeof(priority_values)/sizeof(int32);

win32Thread::
win32Thread():
Thread()
{
	m_threadHandle	= NULL;	
	m_threadId		= 0;
    m_function      = NULL;
    m_arg           = NULL;
}

win32Thread::
~win32Thread()
{
    if(m_threadHandle)
    {
	    CloseHandle(m_threadHandle);
    }
}

unsigned long __stdcall 
win32Thread::
internalThreadFunction(void* arg)
{
    win32Thread* thread = (win32Thread*) arg;

    return thread->InternalThreadFunction();
}

uint32 
win32Thread::
InternalThreadFunction()
{
    m_function(m_arg);

    return 0;
}

bool 
win32Thread::
Create(thread_function function, void* arg, bool detach)
{
	bool result = false;

    m_function      = function;
    m_arg           = arg;

	m_threadHandle = ::CreateThread(NULL,
									0,
									internalThreadFunction,
									this,
									0,
									&m_threadId);
	//m_threadHandle = (HANDLE) _beginthreadex(
	//								NULL,
	//								0,
	//								(unsigned int (__stdcall *)(void *))internalThreadFunction,
	//								this,
	//								0,
	//								(unsigned int *)&m_threadId);
	if(m_threadHandle)
	{
		result = true;
	}
	
	return result;
}

void 
win32Thread::
Destroy()
{
	 TerminateThread(m_threadHandle, 0);
}

void 
win32Thread::
Suspend()
{
	 SuspendThread(m_threadHandle);
}

void 
win32Thread::
Resume()
{
	ResumeThread(m_threadHandle);
}

void
win32Thread::
Join()
{
	WaitForSingleObject(m_threadHandle, INFINITE);
}

uint32 
win32Thread::
GetPriority() const
{
    int32 priority = GetThreadPriority(m_threadHandle);

    for(int32 i=0; i < kNumPriorities; i++)
    {
        if(priority_values[i] == priority)
        {
            priority = i;
            break;
        }
    }

	return priority;
}

uint32 
win32Thread::
SetPriority(uint32 priority)
{
	int32 old = GetThreadPriority(m_threadHandle);

	SetThreadPriority(m_threadHandle, priority_values[priority]);

    for(int32 i=0; i < kNumPriorities; i++)
    {
        if(priority_values[i] == old)
        {
            old = i;
            break;
        }
    }

	return old;
}

