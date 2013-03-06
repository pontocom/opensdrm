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
        
        $Id: timer.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "config.h"
#include "timer.h"

#if defined(__linux__) || defined(solaris) || defined(__FreeBSD__) || \
    defined(__QNX__) || defined(irix)
#include <unistd.h>
#define GoToSleep(x) usleep(x*1000)
#elif defined(WIN32)
#include <windows.h>
#define GoToSleep(x) Sleep(x)
#elif defined(HAVE_BEOS)
#include <be/kernel/OS.h>
#define GoToSleep(x) snooze(x*1000)
#else
    #error GoToSleep needs to be defined for this platform
#endif


TimerManager::TimerManager(): m_alive(true)
{
        m_thread = Thread::CreateThread();
        m_thread->Create(TimerManager::thread_function, this);
}

TimerManager::~TimerManager()
{
    m_alive = false;

    if(m_thread) {
        m_semaphore.Wait();
        m_thread->Join();
        delete m_thread;
    }

    vector<TimerRef>::iterator i = m_list.begin();

    for(; i != m_list.end(); i++)
    {
        delete (*i);
    }
}

void TimerManager::StartTimer(TimerRef* timerRef,
                              TimerFunction function,
                              uint32 seconds, 
                              void* userValue)
{
    if(function)
    {
        Timer* t = new Timer;

        t->ticks = 0;
        t->duration = seconds;
        t->function = function;
        t->userValue = userValue;

        *timerRef = t;
        
        m_list.push_back(t);
    }
}

void TimerManager::StopTimer(TimerRef timer)
{
    if (timer == NULL)
        return;

    timer->ticks = 0;
    timer->duration = 0;

    vector<TimerRef>::iterator i = m_list.begin();

    for(; i != m_list.end(); i++)
    {
        if(*i == timer)
        {
            m_mutex.Acquire();
            m_list.erase(i);
            delete timer;
            m_mutex.Release();
            break;
        }
    }
}

void TimerManager::SetTimer(TimerRef timer, uint32 seconds)
{
    if (timer == NULL)
        return;

    vector<TimerRef>::iterator i = m_list.begin();

    for(; i != m_list.end(); i++)
    {
        if(*i == timer)
        {
            timer->duration = seconds;
            break;
        }
    }
    
}

void TimerManager::ThreadFunction()
{
    do
    {
        GoToSleep(1000);

        m_mutex.Acquire();

        vector<TimerRef>::iterator i = m_list.begin();

        for(; i != m_list.end(); i++)
        {
                if (!(*i) || !(*i)->function)
                    continue;

                (*i)->ticks++;

                if((*i)->duration && (*i)->ticks >= (*i)->duration)
                {
                    Timer* t = new Timer(*(*i));
                    t->thread = Thread::CreateThread();
                    t->thread->Create(TimerManager::timer_function, t, true);
                    (*i)->ticks = 0;
                }
        }

        m_mutex.Release();

    } while(m_alive);  

    m_semaphore.Signal();
}

void TimerManager::thread_function(void* arg)
{
    TimerManager* _this = (TimerManager*)arg;

    _this->ThreadFunction();
}

void TimerManager::timer_function(void* arg)
{
    Timer* t = (Timer*)arg;

    t->function(t->userValue);

    delete t->thread;
    delete t;
}
