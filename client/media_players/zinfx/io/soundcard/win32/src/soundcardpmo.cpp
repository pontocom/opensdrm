/*____________________________________________________________________________
   
   FreeAmp - The Free MP3 Player

   Portions Copyright (C) 1998-1999 EMusic.com
   Portions Copyright (C) 1997 Jeff Tsay (ctsay@pasteur.eecs.berkeley.edu)

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
   
   $Id: soundcardpmo.cpp,v 1.1 2004/10/13 16:16:08 linha2 Exp $
____________________________________________________________________________*/

/* system headers */
#include <windows.h>
#include <winbase.h>
#include <stdlib.h>
#include <string.h>

/* project headers */
//#include "config.h"
#include "SoundCardPMO.h"
#include "eventdata.h"
#include "debug.h"

#define DB Debug_v("%s:%d", __FILE__, __LINE__);

#define MAXINT32 0x7FFFFFFF
const int iFramesPerHeader = 32;

Mutex *g_pHeaderMutex;

extern    "C"
{
   PhysicalMediaOutput *Initialize(FAContext *context)
   {
      return new SoundCardPMO(context);
   }
}

static void CALLBACK
MCICallBack(HWAVEOUT hwo, UINT msg, DWORD dwInstance,
            DWORD dwParam1, DWORD dwParam2)
{
   switch (msg)
   {
   case WOM_DONE:
      {
         LPWAVEHDR lpWaveHdr = (LPWAVEHDR) dwParam1;

         g_pHeaderMutex->Acquire();
         lpWaveHdr->dwUser = -((int)lpWaveHdr->dwUser);
         g_pHeaderMutex->Release();

         break;
      }
   }
}

SoundCardPMO::SoundCardPMO(FAContext *context) :
        PhysicalMediaOutput(context)
{
   MMRESULT mmresult = 0;
   Int32PropValue *pProp;
   m_hWnd =0;
   
   m_wfex = NULL;
   m_wavehdr_array = NULL;
   m_hwo = NULL;

   m_channels = 0;
   m_samples_per_second = 0;
   m_samples_per_frame = 0;
   m_hdr_size = 0;
   m_data_size = 0;
   m_initialized = false;
   m_pBufferThread = NULL;
   g_pHeaderMutex = new Mutex(); 
   m_iHead = 0;
   m_iTail = 0;
   m_iOffset = 0;
   m_iBaseTime = MAXINT32;
   m_iBytesPerSample = 0;
   m_num_headers = 0;
   m_iLastTime = 0;

   //   lillian
   m_volume = NULL;

pBase = 0;
   
   if (!m_pBufferThread)
   {
      m_pBufferThread = Thread::CreateThread();
      assert(m_pBufferThread);
      m_pBufferThread->Create(SoundCardPMO::StartWorkerThread, this);
   }

    
   if (IsError(m_pContext->props->GetProperty("MainWindow", 
              (PropValue **)&pProp)))
      return;        
   else
      m_hWnd = (HWND)pProp->GetInt32();
   
   m_volume = new Win32Volume(Win32Volume::eWaveOut, m_hWnd);
} 

SoundCardPMO::~SoundCardPMO()
{
   m_bExit = true;
   m_pSleepSem->Signal();
   m_pPauseSem->Signal();

   if (m_initialized)
   {
      waveOutReset(m_hwo);

      for(int iLoop = 0; iLoop < m_num_headers; iLoop++)
      {
          if ((int)m_wavehdr_array[iLoop].dwUser < 0)
          {
              waveOutUnprepareHeader(m_hwo, &m_wavehdr_array[iLoop], sizeof(WAVEHDR));
          }
      }

      while (waveOutClose(m_hwo) == WAVERR_STILLPLAYING)
      {
         usleep(100000);
      }

      delete m_wavehdr_array;
      delete m_wfex;
   }
   if (m_pBufferThread)
   {
      m_pBufferThread->Join();
      delete m_pBufferThread;
   }
   if (g_pHeaderMutex)
   {
      delete g_pHeaderMutex;
      g_pHeaderMutex = NULL;
   }

   if ( m_volume )
   {
        delete m_volume;
        m_volume = NULL;
   }
}

void SoundCardPMO::GetVolume(int32 &left, int32 &right) 
{
    m_volume->GetVolume(left, right);
}

void SoundCardPMO::SetVolume(int32 left, int32 right) 
{
    m_volume->SetVolume(left, right);
}

Error SoundCardPMO::Init(OutputInfo * info)
{
   Error           result = kError_UnknownErr;
   MMRESULT        mmresult = 0;
   Int32PropValue *pProp = NULL;


   m_channels = info->number_of_channels;
   m_samples_per_second = info->samples_per_second;
   m_samples_per_frame = info->samples_per_frame;
   m_data_size = info->max_buffer_size;

   m_iBytesPerSample = info->number_of_channels * (info->bits_per_sample / 8);

   m_num_headers = (m_pInputBuffer->GetBufferSize() / m_data_size) - 1;
   
   m_hdr_size = sizeof(WAVEHDR);

   m_wfex = new WAVEFORMATEX;

   m_wfex->wBitsPerSample = info->bits_per_sample;
   m_wfex->wFormatTag = WAVE_FORMAT_PCM;
   m_wfex->nChannels = (WORD) m_channels;
   m_wfex->nSamplesPerSec = info->samples_per_second;
   m_wfex->nBlockAlign = (info->bits_per_sample / 8) * m_channels;
   m_wfex->nAvgBytesPerSec = info->samples_per_second * m_wfex->nBlockAlign;
   m_wfex->cbSize = 0;

   mmresult = waveOutOpen(&m_hwo,
                          WAVE_MAPPER,
                          m_wfex,
                          (DWORD) MCICallBack,
                          NULL,
                          WAVE_ALLOWSYNC | CALLBACK_FUNCTION);

   if (mmresult == MMSYSERR_NOERROR)
   {
      result = kError_NoErr;
   }

   uint32    i;

   m_wavehdr_array = new WAVEHDR[m_num_headers];
   for (i = 0; i < m_num_headers; i++)
   {
      m_wavehdr_array[i].dwBufferLength = m_data_size;
      m_wavehdr_array[i].dwBytesRecorded = 0;
      m_wavehdr_array[i].dwUser = 0;   
      m_wavehdr_array[i].dwLoops = 0;
      m_wavehdr_array[i].dwFlags = NULL;
   }
   m_iBytesPerSample = info->number_of_channels * (info->bits_per_sample / 8);

   m_initialized = true;


   return result;
}

void SoundCardPMO::HandleTimeInfoEvent(PMOTimeInfoEvent *pEvent)
{
   MediaTimeInfoEvent *pmtpi;
   int32               hours, minutes, seconds;
   int                 iTotalTime = 0;
   MMTIME              sTime;

   if (m_iBaseTime == MAXINT32)
   {
       if (!pEvent)
          return;
          
       m_iBaseTime = (pEvent->GetFrameNumber() * m_samples_per_frame) / 
                      m_samples_per_second;

       sTime.wType = TIME_BYTES;
       if (waveOutGetPosition(m_hwo, &sTime, sizeof(sTime)) != MMSYSERR_NOERROR)
           return;
   
       m_iBaseTime -= (sTime.u.cb / (m_samples_per_second * m_iBytesPerSample));
	   m_iLastTime = 0;
   }

   if (m_samples_per_second <= 0)
      return;

   sTime.wType = TIME_BYTES;
   if (waveOutGetPosition(m_hwo, &sTime, sizeof(sTime)) != MMSYSERR_NOERROR)
       return;
   
   iTotalTime = (sTime.u.cb / (m_samples_per_second * m_iBytesPerSample)) +
                m_iBaseTime;

   // Time needs to be greater or equal to the last time for each pass,
   // otherwise we have a break in playback, in which case we need to
   // reset the base time. After a break in playback the waveOutGetPosition()
   // function will reset back to 0. 
   // Alternatively, if m_iBaseTime gets messed up (i.e. its more than 24 hours!)
   // force a reset as well.
   if (iTotalTime < m_iLastTime || (m_iBaseTime > 86400 && m_iBaseTime < MAXINT32))
   {
	   Debug_v("Reset: %d %d %d", iTotalTime, m_iLastTime, m_iBaseTime);
	   m_iBaseTime = MAXINT32;
	   return;
   }
      
   hours = iTotalTime / 3600;
   minutes = (iTotalTime - 
                (hours * 3600)) / 60;
   seconds = iTotalTime - 
                (hours * 3600) - 
                (minutes * 60);

   // If something became wonky, reset.
   if (minutes < 0 || minutes > 59 || 
	   seconds < 0 || seconds > 59 || 
	   hours < 0 || hours > 24)
   {
	  m_iBaseTime = MAXINT32;
      return;
   }

   pmtpi = new MediaTimeInfoEvent(hours, minutes, seconds, 0,
                                  (float)iTotalTime, 0);
   m_pTarget->AcceptEvent(pmtpi);
   m_iLastTime = iTotalTime;
}

bool SoundCardPMO::WaitForDrain(void)
{
   unsigned iLoop, iNumHeadersPending = 0;

   for(; !m_bExit && !m_bPause; )
   {   
       g_pHeaderMutex->Acquire();

       for(iLoop = 0, iNumHeadersPending = 0; iLoop < m_num_headers; iLoop++)
       {
           if ((int)m_wavehdr_array[iLoop].dwUser > 0)
               iNumHeadersPending++;
       }
       g_pHeaderMutex->Release();
   
       if (iNumHeadersPending == 0)
       {
          return true;
       }
       WasteTime();
       HandleTimeInfoEvent(NULL);
   }

   return false;
}

void SoundCardPMO::Pause(void)
{
    m_iBaseTime = MAXINT32;

    PhysicalMediaOutput::Pause();
}

void SoundCardPMO::Resume(void)
{
    if (m_initialized)
       waveOutRestart(m_hwo);
    
    PhysicalMediaOutput::Resume();
}

void SoundCardPMO::Clear(void)
{
    if (m_initialized)
    {
       int i;
       
       waveOutReset(m_hwo);
       g_pHeaderMutex->Acquire();
       for (i = 0; i < m_num_headers; i++)
       {
          m_wavehdr_array[i].dwBufferLength = m_data_size;
          m_wavehdr_array[i].dwBytesRecorded = 0;
          m_wavehdr_array[i].dwUser = 0;   
          m_wavehdr_array[i].dwLoops = 0;
          m_wavehdr_array[i].dwFlags = NULL;
       }
       m_iOffset = 0;   
       m_iHead = m_iTail = 0;
       g_pHeaderMutex->Release();
    }   
    
    PhysicalMediaOutput::Clear();
}

Error SoundCardPMO::Reset(bool user_stop)
{
   waveOutPause(m_hwo);

   return kError_NoErr;
}


Error SoundCardPMO::Write(void *pBuffer, uint32 uSize)
{
   Error    result = kError_NoErr;
   WAVEHDR *wavhdr = NULL;

   wavhdr = NextHeader();
   if (!wavhdr)
   {
      return kError_Interrupt;
   }

   if (pBase == NULL)
      pBase = (char *)pBuffer;
      
   if ((((char *)pBuffer) + uSize)- pBase > m_pInputBuffer->GetBufferSize())
   {
      //Debug_v("Diff: %d Size: %u", 
      //    ((((char *)pBuffer) + uSize) - pBase), m_pInputBuffer->GetBufferSize());
      assert(0);
   }   
         
   wavhdr->dwBufferLength = uSize;
   wavhdr->lpData = (char *)pBuffer;

   // Prepare & write newest header
   waveOutPrepareHeader(m_hwo, wavhdr, m_hdr_size);
   waveOutWrite(m_hwo, wavhdr, m_hdr_size);

   return result;
}

Error SoundCardPMO::AllocHeader(void *&pBuffer, uint32 &uSize)
{
    Error    eRet;
    unsigned iRead, uMaxBytes;
    int      iNumBlocks;

    uMaxBytes = m_pInputBuffer->GetBufferSize() -
                (m_pInputBuffer->GetReadIndex() + m_iOffset);

    iNumBlocks = iFramesPerHeader;
    if (iNumBlocks * m_data_size >= uMaxBytes && uMaxBytes != 0)
    {
       iNumBlocks = uMaxBytes / m_data_size;
       assert(uMaxBytes % m_data_size == 0);
    }   

    while(iNumBlocks > 0)
    {
       uSize = m_data_size * iNumBlocks;
       iRead = m_iOffset + uSize;
       eRet = ((EventBuffer *)m_pInputBuffer)->BeginRead(pBuffer, iRead);
       if (eRet == kError_NoDataAvail)
       {
          iNumBlocks >>= 1;
          continue;
       }   
       if (eRet != kError_NoErr)
          return eRet;
       else
          break;    
    }       

    if (iNumBlocks == 0)
       return kError_NoDataAvail;

    eRet = ((EventBuffer *)m_pInputBuffer)->EndRead(0);
    if (eRet != kError_NoErr)
        return eRet;

    pBuffer = (char *)pBuffer + m_iOffset;
    ((EventBuffer *)m_pInputBuffer)->WrapPointer(pBuffer);

    m_iOffset += uSize;
    ((EventBuffer *)m_pInputBuffer)->SetBytesInUse(m_iOffset);

    return kError_NoErr;
}

Error SoundCardPMO::FreeHeader(uint32 uSize)
{
    Error     eRet;
    void     *pBuffer;
    unsigned  iRead;

    iRead = uSize;
    eRet = ((EventBuffer *)m_pInputBuffer)->BeginRead(pBuffer, iRead);
    if (eRet != kError_NoErr)
       return eRet;

    eRet = ((EventBuffer *)m_pInputBuffer)->EndRead(iRead);
    if (eRet != kError_NoErr)
       return eRet;

    m_iOffset -= uSize;
    ((EventBuffer *)m_pInputBuffer)->SetBytesInUse(m_iOffset);

    return kError_NoErr;
}

WAVEHDR *SoundCardPMO::NextHeader(bool bFreeHeadersOnly)
{
   WAVEHDR  *result = NULL;
   unsigned  iLoop;
   Error     eRet;

   assert(m_initialized);

   for(; !m_bExit;)
   {
       g_pHeaderMutex->Acquire();
       for(iLoop = 0; iLoop < m_num_headers; iLoop++)
       {
           if ((int)m_wavehdr_array[iLoop].dwUser < 0 &&
              (-(int)m_wavehdr_array[iLoop].dwUser) == m_iTail + 1)
          {
              waveOutUnprepareHeader(m_hwo, &m_wavehdr_array[iLoop], sizeof(WAVEHDR));

              eRet = FreeHeader(m_wavehdr_array[iLoop].dwBufferLength);
              if (IsError(eRet))
              {
                 g_pHeaderMutex->Release();
                 return NULL;
              }

              m_wavehdr_array[iLoop].dwUser = 0;
              m_iTail++;
          }
          if (!bFreeHeadersOnly && !m_wavehdr_array[iLoop].dwUser)
          {
             result = &m_wavehdr_array[iLoop];
             result->dwUser = ++m_iHead;

             g_pHeaderMutex->Release();

             return result;
          }
       }
       g_pHeaderMutex->Release();

       if (bFreeHeadersOnly)
           return NULL;

	   // Toss out frames that have been consumed
       NextHeader(true);
	   if (m_iHead == m_iTail)
	      m_iBaseTime = MAXINT32;

       CheckForBufferUp();
       usleep(10000);
   }

   return NULL;
}

void SoundCardPMO::StartWorkerThread(void *pVoidBuffer)
{
   ((SoundCardPMO*)pVoidBuffer)->WorkerThread();
}
 
void SoundCardPMO::WorkerThread(void)
{
   void       *pBuffer;
   Error       eErr;
   Event      *pEvent;
   int         iValue;
   uint32      uSize;

   // Don't do anything until resume is called.
   m_pPauseSem->Wait();

   // Wait for prebuffer period
   PreBuffer();

   m_pContext->prefs->GetPrefInt32(kDecoderThreadPriorityPref, &iValue);
   m_pBufferThread->SetPriority(iValue);

   for(; !m_bExit;)
   {
      if (m_bPause)
      {
          m_pPauseSem->Wait();
          continue;
      }

      // Loop until we get an Init event from the LMC
      if (!m_initialized)
      {
          pEvent = ((EventBuffer *)m_pInputBuffer)->GetEvent();

          if (pEvent == NULL)
          {
              m_pLmc->Wake();
              WasteTime();

              continue;
          }

          if (pEvent->Type() == PMO_Init)
          {
              if (IsError(Init(((PMOInitEvent *)pEvent)->GetInfo())))
              {
                  delete pEvent;
                  break;
              }
          }
          delete pEvent;

          continue;
      }

      // Set up reading a block from the buffer. If not enough bytes are
      // available, sleep for a little while and try again.
      for(;;)
      {
          if (m_bPause || m_bExit)
              break;
          
          eErr = AllocHeader(pBuffer, uSize);
          if (eErr == kError_EndOfStream || eErr == kError_Interrupt)
             break;

          if (eErr == kError_NoDataAvail)
          {
              m_pLmc->Wake();

              // Calling NextHeader with a true arguments just  
              // cleans up the pending headers so the bytes in use
              // value is correct.
              NextHeader(true);
              HandleTimeInfoEvent(NULL);
			  if (m_iHead == m_iTail)
				  m_iBaseTime = MAXINT32;
			  WasteTime();

              CheckForBufferUp();

              continue;
          }

          // Is there an event pending that we need to take care of
          // before we play this block of samples?
          if (eErr == kError_EventPending)
          {
              pEvent = ((EventBuffer *)m_pInputBuffer)->PeekEvent();
              if (pEvent == NULL)
                  continue;
                  
              if (pEvent->Type() == PMO_Quit && 
                  ((EventBuffer *)m_pInputBuffer)->GetBytesInUse() > 0) 
              {
                  if (WaitForDrain())
                  {
                     m_pTarget->AcceptEvent(new Event(INFO_DoneOutputting));
                     return;
                  }
                  continue;
              }

              pEvent = ((EventBuffer *)m_pInputBuffer)->GetEvent();
              if (pEvent == NULL)
                  continue;

              if (pEvent->Type() == PMO_Init)
			  {
                  Init(((PMOInitEvent *)pEvent)->GetInfo());
			  }
    
              if (pEvent->Type() == PMO_Reset)
                  Reset(true);
    
              if (pEvent->Type() == PMO_Info) 
                  HandleTimeInfoEvent((PMOTimeInfoEvent *)pEvent);

              if (pEvent->Type() == PMO_Quit)
              {
                 m_pTarget->AcceptEvent(new Event(INFO_DoneOutputting));
                 delete pEvent;
                 return;
              }
    
              delete pEvent;
    
              continue;
          }
          
          if (IsError(eErr))
          {
              ReportError("Internal error occured.");
              m_pContext->log->Error("Cannot read from buffer in PMO "
                                    "worker tread: %d\n", eErr);
              return;
          }
          break;
      }
      if (m_bPause || m_bExit)
         continue;

      Write(pBuffer, uSize);
      m_pLmc->Wake();

      UpdateBufferStatus();
   }
}    


