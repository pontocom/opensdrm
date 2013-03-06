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
        
        $Id: pmi.cpp,v 1.1 2004/10/13 16:16:08 linha2 Exp $
____________________________________________________________________________*/

/* system headers */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <errno.h>
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
#include "pmi.h"
#include "pullbuffer.h"
#include "facontext.h"
#include "log.h"
#include "debug.h"

#define DB printf("%s:%d\n", __FILE__, __LINE__);

PhysicalMediaInput::PhysicalMediaInput(FAContext *context):
                PipelineUnit(context)
{
    m_path = NULL;
}

PhysicalMediaInput::~PhysicalMediaInput()
{

    m_bExit = true; 
    m_bPause = false;
    m_pSleepSem->Signal();
    m_pPauseSem->Signal();

    if (m_path)
    {
       delete [] m_path;
       m_path = NULL;
    }
}

Error PhysicalMediaInput::SetTo(const char *url)
{
    if (m_path != NULL) 
	{
		delete [] m_path;
		m_path = NULL;
	}

    m_path = new char[strlen(url) + 1];
    strcpy(m_path, url);

    return kError_NoErr;
}

Error PhysicalMediaInput::Close(void)
{
    if (m_pOutputBuffer)
       m_pOutputBuffer->Clear();

    return kError_NoErr;
}

