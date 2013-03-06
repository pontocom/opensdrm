/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player

	Portions Copyright (C) 1998-2000 EMusic.com

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
	
	$Id: pthreadthread.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/


#include "config.h"

#include <pthread.h>
#include <signal.h>
#include <iostream.h>

#include "pthreadthread.h"
#include "mutex.h"

void pthreadThread::Join() {
    pthread_join(m_threadHandle,NULL);
}

pthreadThread::
pthreadThread():
Thread()
{
    m_threadHandle	= (pthread_t) NULL;
    m_threadId		= 0;
    m_suspended         = false;
    m_suspendMutex      = new Mutex();
}

pthreadThread::
~pthreadThread()
{
    if (m_suspendMutex) {
	delete m_suspendMutex;
	m_suspendMutex = NULL;
    }
}

void *
pthreadThread::
internalThreadFunction(void* arg)
{
    pthreadThread* thread = (pthreadThread*) arg;
    thread->InternalThreadFunction();
    return NULL;
}

void *
pthreadThread::
InternalThreadFunction()
{
    m_semaphore.Wait();

    if (m_function) 
        m_function(m_arg);

    return NULL;
}

bool 
pthreadThread::
Create(thread_function function, void* arg, bool detach)
{
    bool result = true;
    m_function = function;
    m_arg = arg;
    if (pthread_create(&m_threadHandle, NULL,
        pthreadThread::internalThreadFunction, this))
    {
	result = false;
    }
    if (detach && result) {
        pthread_detach(m_threadHandle);
    }

    m_semaphore.Signal();

    return result;
}

void 
pthreadThread::
Destroy()
{
    pthread_cancel(m_threadHandle);
}

void 
pthreadThread::
Suspend()
{
    m_suspendMutex->Acquire();
    if (!m_suspended) {
	pthread_kill(m_threadHandle, SIGSTOP);
	m_suspended = true;
    }
    m_suspendMutex->Release();
}

void 
pthreadThread::
Resume()
{
    m_suspendMutex->Acquire();
    if (m_suspended) {
	pthread_kill(m_threadHandle, SIGCONT);
	m_suspended = false;
    }
    m_suspendMutex->Release();
}

uint32 
pthreadThread::
GetPriority() const
{
    return( 0);
}

uint32 
pthreadThread::
SetPriority(uint32 priority)
{
    return( 0);
}

