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
	
	$Id: pipeline.h,v 1.1 2004/10/13 16:16:07 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PIPELINE_H_
#define INCLUDED_PIPELINE_H_

/* system headers */
#include <stdlib.h>
#include <assert.h>

#include "config.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#elif HAVE_IO_H
#include <io.h>
#else 
#error Must have unistd.h or io.h!
#endif // HAVE_UNISTD_H


/* project headers */
#include "semaphore.h"
#include "mutex.h"
#include "errors.h"
#include "facontext.h"
#include "event.h"
#include "properties.h"
#include "pullbuffer.h"

class PipelineUnit
{
public:
            PipelineUnit(FAContext *);
    virtual ~PipelineUnit();

    virtual void  Pause(void);
    virtual void  Resume(void);
    virtual void  Wake(void);
    virtual void  Clear(void);

    virtual void  SetPropManager(Properties *);
    virtual void  SetInputBuffer(PullBuffer *pBuffer);

    virtual void  ReportError(const char * format, ...);
    virtual void  ReportStatus(const char * format, ...);
    virtual void  DebugPrint(void);

protected:

    virtual bool  Sleep(void);

    Semaphore             *m_pSleepSem;
    Semaphore             *m_pPauseSem;
    Mutex                 *m_pMutex;

    EventQueue            *m_pTarget;
    PullBuffer            *m_pOutputBuffer; 
    PullBuffer            *m_pInputBuffer;
    Properties            *m_pPropManager;
    bool                   m_bPause, m_bExit;
    FAContext             *m_pContext;

private:
 
    bool                   m_bSleeping;
};

#endif /* _PMO_H_ */
