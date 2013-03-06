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
	
	$Id: tstream.h,v 1.1 2004/10/13 16:16:07 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_TSTREAM_H_
#define INCLUDED_TSTREAM_H_

/* system headers */
#include <stdlib.h>
#include <assert.h>


#if HAVE_UNISTD_H
#include <unistd.h>
#elif HAVE_IO_H
#include <io.h>
#else 
#error Must have unistd.h or io.h!
#endif // HAVE_UNISTD_H


/* project headers */
#include "config.h"
#include "errors.h"
#include "facontext.h"
#include "thread.h"
#include "event.h"

class TitleStreamServer
{
public:
            TitleStreamServer(FAContext *, EventQueue *);
    virtual ~TitleStreamServer();

    Error    Run(in_addr &, int iRemotePort);
    Error    Init(int &iLocalPort);

    Error    Run(void);
    Error    MulticastInit(char *szAddr, int iPort); 

protected:

    static void StartWorkerThread(void *pVoidBuffer); 
    void        WorkerThread(void);

    EventQueue            *m_pTarget;
    bool                   m_bExit, m_bUseMulticast;
    FAContext             *m_pContext;
    int                    m_hHandle;
    Thread                *m_pBufferThread; 
    struct sockaddr_in    *m_pSin;
};

#endif
