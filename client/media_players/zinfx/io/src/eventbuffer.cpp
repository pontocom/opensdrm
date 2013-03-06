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

   $Id: eventbuffer.cpp,v 1.1 2004/10/13 16:16:08 linha2 Exp $
____________________________________________________________________________*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>

#include "eventbuffer.h"

#define DB Debug_v("%s:%d\n", __FILE__, __LINE__);

EventBuffer::EventBuffer(size_t iBufferSize, size_t iOverFlowSize, 
                         FAContext *context) : 
				 PullBuffer(iBufferSize, iOverFlowSize, context)
{
    m_pQueue = new Queue < BufferEvent * >();
    m_iBytesInUse = 0;
}

EventBuffer::~EventBuffer(void)
{
    delete m_pQueue;
}

Error EventBuffer::BeginRead(void *&pBuffer, size_t iBytesWanted)
{
   BufferEvent   *pEvent;
   int   iReadIndex;
   size_t iMaxBytes;

   pEvent = m_pQueue->Peek();
   iReadIndex = GetReadIndex();

   if (pEvent && pEvent->iIndex == iReadIndex)
   {
      return kError_EventPending;
   }

   if (!pEvent)
   {
       return PullBuffer::BeginRead(pBuffer, iBytesWanted);
   }

   if (pEvent->iIndex > iReadIndex)
   {
      iMaxBytes = pEvent->iIndex - iReadIndex;
   }
   else
   {
      iMaxBytes = (GetBufferSize() - iReadIndex) + pEvent->iIndex;
   }

   if (iBytesWanted >= iMaxBytes)
      return kError_EventPending;
   else
      return PullBuffer::BeginRead(pBuffer, iBytesWanted);

}

Error EventBuffer::AcceptEvent(Event *pPMOEvent)
{
   BufferEvent *pEvent;

   assert(this);
   assert(m_pQueue);
   assert(pPMOEvent);

   pEvent = new BufferEvent;
   pEvent->iIndex = GetWriteIndex(); 
   pEvent->pEvent = pPMOEvent;

   m_pQueue->Write(pEvent);

   return kError_NoErr;
}

Event *EventBuffer::GetEvent()
{
   BufferEvent *pEvent;
   Event       *pPMOEvent;

   assert(this);
   assert(m_pQueue);

   pEvent = m_pQueue->Read();
   if (pEvent == NULL)
	  return NULL;

   pPMOEvent = pEvent->pEvent;
   delete pEvent;

   return pPMOEvent;
}

Event *EventBuffer::PeekEvent()
{
   BufferEvent *pEvent;

   pEvent = m_pQueue->Peek();
   return pEvent->pEvent;
}

Error EventBuffer::Clear()
{
   Event *e; 
   while ((e = GetEvent()) != NULL) {
       delete e;
   }
   m_pQueue->Clear();

   return PullBuffer::Clear();
}

void EventBuffer::SetBytesInUse(uint32 iBytesInUse)
{
   m_iBytesInUse = iBytesInUse;
}

uint32 EventBuffer::GetBytesInUse(void)
{
   return m_iBytesInUse;
}

int32 EventBuffer::GetBufferPercentage(void)
{
   return (100 * m_iBytesInBuffer) / m_iBufferSize;
}
