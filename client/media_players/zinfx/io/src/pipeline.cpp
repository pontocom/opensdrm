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
        
        $Id: pipeline.cpp,v 1.1 2004/10/13 16:16:08 linha2 Exp $
____________________________________________________________________________*/

/* system headers */
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>

/* project headers */
#include <config.h>
#include "pipeline.h"
#include "pullbuffer.h"
#include "eventbuffer.h"
#include "facontext.h"
#include "log.h"
#include "debug.h"

#define DB printf("%s:%d\n", __FILE__, __LINE__); 

PipelineUnit::PipelineUnit(FAContext *context)
{
    m_pContext = context;
    m_pTarget = context->target;
    m_bPause = false;
    m_pPropManager = NULL;
    m_bExit = false;
    m_bSleeping = true;
    m_pOutputBuffer = NULL;
    m_pInputBuffer = NULL;

    m_pPauseSem = new Semaphore();
    assert(m_pPauseSem);

    m_pSleepSem = new Semaphore();
    assert(m_pSleepSem);

    m_pMutex = new Mutex();
    assert(m_pMutex);
}

PipelineUnit::~PipelineUnit()
{
    m_pMutex->Acquire();

    m_bExit = true;
    m_bPause = false;

    m_pPauseSem->Signal();
    m_pSleepSem->Signal();

    m_pMutex->Release();

    if (m_pOutputBuffer)
        delete m_pOutputBuffer;

    delete m_pMutex;
    delete m_pPauseSem;
    delete m_pSleepSem;
}

void PipelineUnit::SetInputBuffer(PullBuffer *pBuffer)
{
    m_pMutex->Acquire();

    m_pInputBuffer = pBuffer;

    m_pMutex->Release();
}   

void PipelineUnit::SetPropManager(Properties * p)
{
    m_pMutex->Acquire();

    m_pPropManager = p;
    m_pMutex->Release();
}

void PipelineUnit::ReportError(const char * format, ...)
{
    assert(m_pTarget);

    char szBuffer[4096];
    va_list argptr;

    va_start(argptr, format);
    vsprintf(szBuffer, format, argptr);
    va_end(argptr);

    m_pTarget->AcceptEvent(new ErrorMessageEvent(szBuffer));
    m_pTarget->AcceptEvent(new Event(INFO_DoneOutputtingDueToError));
}   

void PipelineUnit::ReportStatus(const char * format, ...)
{
    assert(m_pTarget);

    char szBuffer[4096];
    va_list argptr;

    va_start(argptr, format);
    vsprintf(szBuffer, format, argptr);
    va_end(argptr);

    m_pTarget->AcceptEvent(new StatusMessageEvent(szBuffer));
}   

void PipelineUnit::Pause(void)
{
    m_pMutex->Acquire();

    m_bPause = true;
    Wake();

    m_pMutex->Release();
}

void PipelineUnit::Resume(void)
{
    m_pMutex->Acquire();

    m_bPause = false;
    m_pPauseSem->Signal();

    m_pMutex->Release();
}

void PipelineUnit::Clear(void)
{
    m_pMutex->Acquire();

    if (m_pOutputBuffer)
        m_pOutputBuffer->Clear();
    
    m_pMutex->Release();
}

void PipelineUnit::Wake(void)
{
    m_pMutex->Acquire();

    m_pSleepSem->Signal();
    
    m_pMutex->Release();
}

bool PipelineUnit::Sleep(void)
{
    m_pSleepSem->Wait();

    return m_bExit;
}


void PipelineUnit::DebugPrint(void)
{
    printf("[%c %c %ld]", 
        m_bSleeping ? 'S' : 'W',
        m_bPause ? 'P' : 'R',
        (long int)m_pOutputBuffer->GetBufferPercentage());
}
