
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
	
	$Id: semaphore.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/


#include "config.h"

#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <iostream.h>
#include <errno.h>

#include "semaphore.h"


Semaphore::Semaphore(int cnt) {
    count = cnt;
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);
}

Semaphore::~Semaphore() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

bool Semaphore::Wait(int iMilliSecs) 
{
    if (iMilliSecs > 0)
        return TimedWait(iMilliSecs);
    
    pthread_mutex_lock(&mutex);
    count--;
    while (count <=0) 
    {
        pthread_cond_wait(&cond,&mutex);
    }
    pthread_mutex_unlock(&mutex);
    return true;
}

bool Semaphore::TimedWait(int iMilliSecs)
{
    struct timespec due;
    struct timeval  tv;
    bool            ret = true;

    pthread_mutex_lock(&mutex);

    gettimeofday(&tv, NULL);
    due.tv_nsec = (tv.tv_usec * 1000) + (iMilliSecs * 1000000);
    due.tv_sec = tv.tv_sec + (due.tv_nsec / 1000000000);
    due.tv_nsec %= 1000000000;

    count--;
    while (count <=0) 
    {
        if (pthread_cond_timedwait(&cond,&mutex, &due) == ETIMEDOUT)
        {
            count++;
            ret = false;
            break;
        }
    }

    pthread_mutex_unlock(&mutex);

    return ret;
}

void Semaphore::Signal() {
    // increment the semaphore
    pthread_mutex_lock(&mutex);
    count++;
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);
}
