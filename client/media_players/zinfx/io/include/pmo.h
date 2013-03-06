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
	
	$Id: pmo.h,v 1.1 2004/10/13 16:16:07 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PMO_H_
#define INCLUDED_PMO_H_

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
#include "pipeline.h"
#include "mutex.h"
#include "eventdata.h"
#include "errors.h"
#include "properties.h"
#include "facontext.h"
#include "lmc.h"
#include "pmi.h"

#define MAXCHANNELS		2


typedef struct OutputInfo
{
    uint32 bits_per_sample;
    uint32 number_of_channels;
    uint32 samples_per_second;
    uint32 max_buffer_size;
    uint32 samples_per_frame;
}OutputInfo;

class EventBuffer;
class PullBuffer;

class PhysicalMediaOutput : public PipelineUnit
{
public:
            PhysicalMediaOutput(FAContext *context);
    virtual ~PhysicalMediaOutput();

    virtual Error Init(OutputInfo* /*info*/) = 0;
    virtual void  GetVolume(int32 &left, int32 &right) = 0;
    virtual void  SetVolume(int32 left, int32 right) = 0;

    virtual Error Reset(bool bUserReset) = 0;
    virtual void  Pause(void);
    virtual void  Resume(void);
    virtual Error ChangePosition(int32);

    virtual void  SetLMC(LogicalMediaConverter *pLMC);
    virtual void  SetPMI(PhysicalMediaInput *pPMI);
    virtual Error SetTo(const char *url);  

    virtual const char *GetErrorString(int32) { return NULL; };

protected:

    virtual bool  WasteTime();
    virtual void  PreBuffer();
    virtual void  UpdateBufferStatus(void);
    virtual void  CheckForBufferUp(bool bForceBufferUp = false);

    PhysicalMediaInput    *m_pPmi;
    LogicalMediaConverter *m_pLmc;
    int32                  m_iPreBuffer;
    int                    m_iBufferUpdate;
    PullBuffer            *m_pPmiBuffer;
};

#endif /* _PMO_H_ */
