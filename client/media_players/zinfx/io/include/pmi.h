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
	
	$Id: pmi.h,v 1.1 2004/10/13 16:16:07 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PMI_H_
#define INCLUDED_PMI_H_

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
#include "pipeline.h"
#include "thread.h"
#include "errors.h"
#include "eventdata.h"

#define SEEK_FROM_START		SEEK_SET
#define SEEK_FROM_CURRENT	SEEK_CUR
#define SEEK_FROM_END		SEEK_END

const int32 iDefaultBufferSize = 65536;
const int32 iDefaultOverflowSize = 16384; 

class PhysicalMediaInput : public PipelineUnit
{
public:
            PhysicalMediaInput(FAContext *context);
    virtual ~PhysicalMediaInput();

    virtual Error SetTo(const char *url);
    virtual Error Close(void);
    virtual Error Open(void) = 0;
    virtual const char* Url(void) const = 0;
    virtual Error Prepare(PullBuffer *&pBuffer) = 0;

    virtual Error Seek(int32 & rtn, int32 offset, int32 origin)
                  { return kError_FileSeekNotSupported; };
    virtual Error Tell(int32 & rtn)
                  { return kError_FileSeekNotSupported; };
    virtual bool  CanHandle(const char *szUrl, char *szTitle)
	               {return false;}
    virtual Error GetLength(size_t &iSize)
                  { return kError_FileSeekNotSupported; };
    virtual bool  IsStreaming(void)
	               {return false;}
    virtual bool  UseBufferReduction(void)
	               {return false;}
    virtual bool  PauseLoop(bool bLoop) { return false; };

     virtual vector<string> *GetProtocols(void) = 0;

protected:

    char          *m_path;
};

#endif /* _PMI_H_ */
