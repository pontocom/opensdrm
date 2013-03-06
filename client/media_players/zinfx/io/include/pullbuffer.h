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
   
   $Id: pullbuffer.h,v 1.1 2004/10/13 16:16:07 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PULLBUFFER_H_
#define INCLUDED_PULLBUFFER_H_

/* system headers */
#include <stdlib.h>

#include "errors.h"
#include "mutex.h"
#include "semaphore.h"

class FAContext;

class PullBuffer
{
    public:

               PullBuffer(size_t iBufferSize, 
                          size_t iOverflowSize,
			                 FAContext *context);
      virtual ~PullBuffer(void);

      virtual  Error    BeginWrite (void *&pBuffer, size_t iBytesToWrite);
      virtual  Error    EndWrite   (size_t iNumBytesWritten);

      virtual  Error    BeginRead  (void *&pBuffer, size_t iBytesNeeded);
      virtual  Error    EndRead    (size_t iBytesUsed);

      virtual  Error    DiscardBytes();

      Error    Clear        (void);
      Error    Resize       (size_t iNewSize,
                             size_t iNewOverflowSize);

      bool     IsEndOfStream(void);
      void     SetEndOfStream(bool bEOS);
      size_t   GetNumBytesInBuffer(void)
               {
                   return m_iBytesInBuffer;
               };
      size_t   GetBufferSize(void)
               {
                   return m_iBufferSize;
               };
      virtual int32 GetBufferPercentage(void)
		       {
			       return (100 * m_iBytesInBuffer) / m_iBufferSize;
			   };
      void     SetName(char *name);

      // These function really should not be public, but
	  // the stupid windows PMO needs them. Grrr.
      void       WrapPointer(void *&pBuffer);
      int32      GetWriteIndex();
      int32      GetReadIndex();

    protected:

      FAContext *m_context;
      Mutex     *m_pMutex;
      bool       m_bExit;
      size_t     m_iBytesInBuffer, m_iOverflowSize, m_iBufferSize;

    private:

      void Check(void);

      int32          m_iReadIndex, m_iWriteIndex;
      bool           m_bEOS;
      size_t         m_iBytesToWrite, m_iBytesToRead;
      unsigned char *m_pPullBuffer;
      char          *m_pName;
};

#endif
