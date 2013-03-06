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
   
   $Id: pullbuffer.cpp,v 1.1 2004/10/13 16:16:08 linha2 Exp $
____________________________________________________________________________*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include "pullbuffer.h"
#include "log.h"
#include "facontext.h"

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#define DB printf("%s:%d\n",  __FILE__, __LINE__);

PullBuffer::PullBuffer(size_t iBufferSize,
                       size_t iOverflowSize,
		                 FAContext *context)
{
   m_context = context;

   m_bEOS = false;
   m_bExit = false;
   m_iBytesToRead = 0;
   m_iBytesToWrite = 0;
   m_pName = NULL;

   m_iReadIndex =  m_iWriteIndex = 0;
   m_iBytesInBuffer = 0;

   m_iOverflowSize = iOverflowSize;
   m_iBufferSize = iBufferSize;

   m_pPullBuffer = new unsigned char[m_iBufferSize + iOverflowSize];
   assert(m_pPullBuffer != NULL);

   m_pMutex = new Mutex();
}

PullBuffer::~PullBuffer(void)
{
   m_bExit = true;

   if (m_pPullBuffer)
      delete [] m_pPullBuffer;

   delete m_pMutex;
   if (m_pName)
       delete [] m_pName;
}

void PullBuffer::SetName(char *name)
{
   m_pName = new char[strlen(name) + 1];
   strcpy(m_pName, name);
}

Error PullBuffer::Clear(void)
{
   for(;;)
   {
       if (m_bExit)
          return kError_Interrupt;

       m_pMutex->Acquire();

       if (m_iBytesToRead || m_iBytesToWrite)
       {
           m_pMutex->Release();
           usleep(10000);
           continue;
       }
       break;
   }
 
   m_bEOS = false;
   m_iReadIndex =  m_iWriteIndex = 0;
   m_iBytesInBuffer = 0;

   m_pMutex->Release();

   return kError_NoErr;
}

int32 PullBuffer::GetReadIndex(void)
{
   int iRet;

   for(;;)
   {
       if (m_bExit)
          return kError_Interrupt;

       m_pMutex->Acquire();

       if (m_iBytesToRead)
       {
           m_pMutex->Release();
           usleep(10000);
           continue;
       }
       break;
   }

   iRet = m_iReadIndex;

   m_pMutex->Release();
 
   return iRet;
}

int32 PullBuffer::GetWriteIndex(void)
{
   int iRet;

   for(;;)
   {
       if (m_bExit)
          return kError_Interrupt;

       m_pMutex->Acquire();

       if (m_iBytesToWrite)
       {
           m_pMutex->Release();
           usleep(10000);
           continue;
       }
       break;
   }

   iRet = m_iWriteIndex;

   m_pMutex->Release();
 
   return iRet;
}

void PullBuffer::WrapPointer(void *&pBuffer)
{
   char *pPtr;

   pPtr = (char *)pBuffer;

   if (pPtr >= (char *)((unsigned int)m_pPullBuffer + m_iBufferSize))
	   pPtr -= m_iBufferSize;

   pBuffer = (void *)pPtr;
}


Error PullBuffer::Resize(size_t iNewSize, 
                         size_t iNewOverflowSize)
{
   unsigned char *pNew;

   if (iNewSize < m_iBytesInBuffer)
   {
       m_context->log->Error("Pullbuffer: Not resized. Too many bytes in buffer.\n");
       return kError_BufferTooSmall;
   }

   // Nothing should've been read from the buffer yet!
   if (m_iReadIndex != 0)
   {
       m_context->log->Error("Pullbuffer: Not resized. The buffer has been read.\n");
       return kError_InvalidError;
   }

   for(;;)
   {
       if (m_bExit)
          return kError_Interrupt;

       m_pMutex->Acquire();

       if (m_iBytesToRead || m_iBytesToWrite)
       {
           m_pMutex->Release();
		     usleep(10000);
           continue;
       }
       break;
   }

   pNew = new unsigned char[iNewSize + iNewOverflowSize];
   memcpy(pNew, m_pPullBuffer, m_iBytesInBuffer);

   delete [] m_pPullBuffer;
   m_pPullBuffer = pNew;
   m_iBufferSize = iNewSize;
   m_iOverflowSize = iNewOverflowSize;
   m_iWriteIndex = m_iBytesInBuffer;

   m_pMutex->Release();

   return kError_NoErr;
}

bool PullBuffer::IsEndOfStream(void)
{
   bool bRet;

   m_pMutex->Acquire();
   bRet = m_bEOS; 
   m_pMutex->Release();

   return bRet;
}

void PullBuffer::SetEndOfStream(bool bEOS)
{
   m_pMutex->Acquire();
   m_bEOS = bEOS;
   m_pMutex->Release();
}

// BeginWrite requests writing a certain number of bytes. If that number
// of bytes in not available, it returns buffertoosmall.
// Returns kError_Interrupt, kError_BufferTooSmall, kError_NoErr
Error PullBuffer::BeginWrite(void *&pBuffer, size_t iBytesNeeded)
{
   Error    eError = kError_NoErr;
   unsigned iAvail = 0;

   m_pMutex->Acquire();

   assert(m_pPullBuffer != NULL);
   assert(m_iReadIndex >= 0 && (uint32)m_iReadIndex < m_iBufferSize);
   assert(m_iWriteIndex >= 0 && (uint32)m_iWriteIndex < m_iBufferSize);
   assert(m_iBytesToWrite == 0);

   if (m_bExit)
   {
       m_pMutex->Release();
       return kError_Interrupt;
   }

   pBuffer = m_pPullBuffer + m_iWriteIndex;

   //printf("%08X: BeginWrite: ReadIndex: %d WriteIndex %d Size: %d Bytes: %d\n", 
   //    pthread_self(), m_iReadIndex, m_iWriteIndex, m_iBufferSize, m_iBytesInBuffer);
   if (m_iWriteIndex > m_iReadIndex)
   {
       iAvail = m_iBufferSize - m_iWriteIndex + 
                min((uint32)m_iReadIndex, m_iOverflowSize);
   }
   else
   {
      if (m_iWriteIndex == m_iReadIndex)
      {
          if (m_iBytesInBuffer == 0)
              iAvail = iBytesNeeded; 
          else
              iAvail = 0;
      }
      else
      {
          iAvail = m_iReadIndex - m_iWriteIndex;
      }
   }

   if (iBytesNeeded > iAvail)
	{
      m_pMutex->Release();
      return kError_BufferTooSmall;
	}

   assert(m_iWriteIndex + iBytesNeeded <= m_iBufferSize + m_iOverflowSize);
   assert(m_iBytesInBuffer + iBytesNeeded <= m_iBufferSize);

   m_iBytesToWrite = iBytesNeeded;
   m_pMutex->Release();
  
   return eError;
}

Error PullBuffer::EndWrite(size_t iBytesWritten)
{
   m_pMutex->Acquire();

   assert(m_pPullBuffer != NULL);
   assert(iBytesWritten <= m_iBytesToWrite);
   assert(m_iBytesInBuffer + iBytesWritten <= m_iBufferSize);

   m_iWriteIndex = m_iWriteIndex + iBytesWritten;
   if ((uint32)m_iWriteIndex > m_iBufferSize)
   {  
      assert(m_iOverflowSize > 0);
      assert(m_iOverflowSize >= m_iWriteIndex - m_iBufferSize);
      memmove(m_pPullBuffer, m_pPullBuffer + m_iBufferSize,
              m_iWriteIndex - m_iBufferSize);
   }

   m_iWriteIndex %= m_iBufferSize;
   m_iBytesInBuffer += iBytesWritten;

   //printf("%08X: bytesinbuffer: %d byteswritten %d buffsize: %d\n", 
   //   pthread_self(), m_iBytesInBuffer, iBytesWritten, m_iBufferSize);
   
   m_iBytesToWrite = 0;

   m_pMutex->Release();

   return kError_NoErr;
}

// returns: kError_EndOfStream, kError_NoDataAvail, kError_NoErr
Error PullBuffer::BeginRead(void *&pBuffer, size_t iBytesNeeded)
{
   assert(m_pPullBuffer != NULL);
   assert(m_iBytesToRead == 0);
   
   Error  eError = kError_UnknownErr;
   int    iOverflow;
   size_t iAvail = 0;

   if (m_bExit)
   {
       return kError_Interrupt;
   }

   m_pMutex->Acquire();
   if (m_bEOS && iBytesNeeded > m_iBytesInBuffer)
   {
       m_pMutex->Release();
       return kError_EndOfStream;
   }

   if (iBytesNeeded > m_iBytesInBuffer)
   {
       m_pMutex->Release();

       return kError_NoDataAvail;
   }

   if (m_iReadIndex > m_iWriteIndex)
   {
	   if (m_iOverflowSize == 0)
           iAvail = (m_iBufferSize - m_iReadIndex) + m_iWriteIndex;
       else
           iAvail = m_iBufferSize - m_iReadIndex + 
                min((uint32)m_iWriteIndex, m_iOverflowSize);

   }
   else
   {
      if (m_iReadIndex == m_iWriteIndex)
      {
          if (m_iBytesInBuffer == 0)
              iAvail = 0; 
          else
              iAvail = iBytesNeeded;
      }
      else
      {
          iAvail = m_iWriteIndex - m_iReadIndex;
      }
   }

   if (iBytesNeeded > iAvail)
	{
      m_pMutex->Release();
      return kError_NoDataAvail;
	}

   // We need to copy the beginning part of the buffer into the
   // overflow.
   iOverflow = (m_iReadIndex + iBytesNeeded) - m_iBufferSize; 
   if (m_iOverflowSize != 0 && iOverflow > (int)m_iOverflowSize)
   {
       m_pMutex->Release();
       m_context->log->Error("Overflow buffer is too small!! \n"
                    "Needed %d but had only %d for overflow.\n",
                    iOverflow, m_iOverflowSize);

       return kError_InvalidParam;
   } 
   if (m_iOverflowSize != 0 && iOverflow > 0)
   {
       assert(m_iOverflowSize >= (size_t)iOverflow);
       memcpy(m_pPullBuffer + m_iBufferSize, m_pPullBuffer, iOverflow);
   }

   m_iBytesToRead = iBytesNeeded;
   m_pMutex->Release();

   pBuffer = m_pPullBuffer + m_iReadIndex;
 
   return eError = kError_NoErr;
}

Error PullBuffer::EndRead(size_t iBytesUsed)
{
   assert(m_pPullBuffer != NULL);
   assert(iBytesUsed <= m_iBytesToRead);
   assert(iBytesUsed <= m_iBytesInBuffer);

   m_pMutex->Acquire();

   m_iReadIndex = (m_iReadIndex + iBytesUsed) % m_iBufferSize;

   m_iBytesInBuffer -= iBytesUsed;
   assert(m_iBytesInBuffer <= m_iBufferSize);

   m_iBytesToRead = 0;

   m_pMutex->Release();

   return kError_NoErr;
}

Error PullBuffer::DiscardBytes()
{
   int iBytesToDiscard;

   for(;;)
   {
        if (m_bExit)
           return kError_Interrupt;

        m_pMutex->Acquire();
        if (m_iBytesToRead || m_iBytesToWrite)
        {
            m_pMutex->Release();
            continue;
        }
        break;
   }

   iBytesToDiscard = m_iBytesInBuffer - (m_iBufferSize / 2);
   if (iBytesToDiscard > 0)
   {
      m_iReadIndex = (m_iReadIndex + iBytesToDiscard) % m_iBufferSize;
    	m_iBytesInBuffer -= iBytesToDiscard;
      assert(m_iBytesInBuffer <= m_iBufferSize);
      m_context->log->Log(LogInput, "Discarding %d bytes.\n", iBytesToDiscard);
   }

   m_pMutex->Release();

   return kError_NoErr;
}

