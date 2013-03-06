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
   
   $Id: eventbuffer.h,v 1.1 2004/10/13 16:16:07 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_EVENTBUFFER_H_
#define INCLUDED_EVENTBUFFER_H_

/* system headers */
#include <stdlib.h>

#include "pullbuffer.h"
#include "pmoevent.h"
#include "queue.h"

struct BufferEvent
{
    int    iIndex;
    Event *pEvent;
};

class EventBuffer : public PullBuffer
{
    public:

               EventBuffer(size_t iBufferSize, 
                           size_t iOverflowSize,
			                  FAContext *context);
      virtual ~EventBuffer(void);

      virtual  Error      AcceptEvent(Event *);
      virtual  Error      Clear(void);

      virtual  Event     *GetEvent(void);
      virtual  Event     *PeekEvent(void);
      virtual  Error      BeginRead(void *&pBuffer, size_t iNumBytes);

      virtual  uint32     GetBytesInUse(void);
      virtual  void       SetBytesInUse(uint32 iBytesInUse);
      virtual  int32      GetBufferPercentage(void);

    protected:
     
      Queue <BufferEvent *> *m_pQueue;
      uint32                 m_iBytesInUse;
};

#endif
