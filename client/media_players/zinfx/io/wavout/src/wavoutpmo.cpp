/*____________________________________________________________________________

  FreeAmp - The Free MP3 Player

  Portions Copyright (C) 1998-1999 EMusic.com
  Portions Copyright (C) 2000 Michael Rich

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

  $Id: wavoutpmo.cpp,v 1.1 2004/10/13 16:16:08 linha2 Exp $
____________________________________________________________________________*/

/* system headers */
#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

/* project headers */
#include "config.h"
#include "wavoutpmo.h"
#include "eventdata.h"
#include "preferences.h"
#include "facontext.h"
#include "log.h"
#include "debug.h"
#include "wav.h"

#define DB Debug_v("%s:%d", __FILE__, __LINE__);

extern "C"
{
  PhysicalMediaOutput* Initialize(FAContext *context)
  {
      return new WavOutPMO(context);
  }
}

WavOutPMO::
WavOutPMO(FAContext *context):
     PhysicalMediaOutput(context)
{
    m_pBufferThread       = NULL;
    m_iTotalBytesWritten  = 0;
    m_iLastTime           = -1000;

    if (!m_pBufferThread)
    {
       m_pBufferThread = Thread::CreateThread();
       assert(m_pBufferThread);
       m_pBufferThread->Create(WavOutPMO::StartWorkerThread, this);
    }

    m_initialized = false;
    m_Writer = new WaveWriter;
}

WavOutPMO::
~WavOutPMO()
{
  m_bExit = true;
  m_pSleepSem->Signal();
  m_pPauseSem->Signal();

  if (m_pBufferThread)
  {
     m_pBufferThread->Join();
     delete m_pBufferThread;
     m_pBufferThread = NULL;
  }
}

void
WavOutPMO::
GetVolume(int32 &left, int32 &right)
{
    left = right = -1;
}

void
WavOutPMO::
SetVolume(int32 left, int32 right)
{
    ;
}

Error
WavOutPMO::
Init(OutputInfo* info)
{
   uint32 len;
    
   m_samples_per_second  = info->samples_per_second;
   m_data_size           = info->max_buffer_size;
   m_iBytesPerSample = info->number_of_channels * (info->bits_per_sample >> 3);

   WAVEFORMATEX format;

   format.wBitsPerSample   = 16;
   format.wFormatTag       = WAVE_FORMAT_PCM;
   format.nChannels        = info->number_of_channels;
   format.nSamplesPerSec   = m_samples_per_second;
   format.nAvgBytesPerSec  = info->number_of_channels *
                              info->samples_per_second * 2;
   format.nBlockAlign      = 4;
   format.cbSize           = 0;

   char path[MAX_PATH];
   char base[MAX_PATH];
   char url[MAX_PATH];
   char *pPtr;

   // using the current file, split it apart, and rebuilt
   // it, appending an ! to the filename, and changing the extention
   // to wav
   strcpy(url, m_pPmi->Url());
   pPtr = strrchr(url, DIR_MARKER);
   if (pPtr)
   {
      strcpy(path, pPtr+1);
      pPtr = strrchr(path, '.');
      if (pPtr)
         *pPtr = 0;
      strcat(path, ".wav"); 
   }
   else
      strcpy(path, "unknown.wav");

   len = _MAX_PATH;
   m_pContext->prefs->GetPrefString(kSaveMusicDirPref, base, &len);
   strcat(base, DIR_MARKER_STR);
   strcat(base, path);

   m_Writer->Create(base, &format);

   m_initialized = true;

   return kError_NoErr;
}

void 
WavOutPMO::
Pause()
{
   PhysicalMediaOutput::Pause();
}

void
WavOutPMO::
Resume()
{
   PhysicalMediaOutput::Resume();
}

Error
WavOutPMO::
Reset(bool user_stop)
{
   m_Writer->Close();
   return kError_NoErr;
}

void 
WavOutPMO::
Clear()
{
   m_Writer->Close();
   PhysicalMediaOutput::Clear();
}

void
WavOutPMO::
Quit()
{
   m_Writer->Close();
}

void
WavOutPMO::
HandleTimeInfoEvent(PMOTimeInfoEvent *pEvent)
{
  MediaTimeInfoEvent* pmtpi;
  int32               hours, minutes, seconds;
  int                 iTotalTime = 0;

  if (m_samples_per_second <= 0)
     return;

  iTotalTime  = (m_iTotalBytesWritten) / 
                (m_iBytesPerSample * m_samples_per_second);

  hours       = iTotalTime / 3600;
  minutes     = (iTotalTime - hours) / 60;
  seconds     = iTotalTime - hours * 3600 - minutes * 60;

  if (minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59)
     return;

  if (iTotalTime < m_iLastTime + 10)
     return;

  pmtpi = new MediaTimeInfoEvent(hours, minutes, seconds, 0,
                                (float)iTotalTime, 0);
  m_pTarget->AcceptEvent(pmtpi);
  m_iLastTime = iTotalTime;
}

void
WavOutPMO::
StartWorkerThread(void *pVoidBuffer)
{
  ((WavOutPMO*)pVoidBuffer)->WorkerThread();
}

void
WavOutPMO::
WorkerThread(void)
{
   void*   pBuffer;
   Error   eErr;
   Event*  pEvent;

   // Don't do anything until resume is called.
   m_pPauseSem->Wait();

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
                  DB
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
        
          eErr = ((EventBuffer *)m_pInputBuffer)->BeginRead(pBuffer, m_data_size);
          if (eErr == kError_NoDataAvail)
          {
              m_pLmc->Wake();
    
              WasteTime();
              continue;
          }

          // Is there an event pending that we need to take care of
          // before we play this block of samples?
          if (eErr == kError_EventPending)
          {
              pEvent = ((EventBuffer *)m_pInputBuffer)->GetEvent();
           if (pEvent == NULL)
              continue;

              if (pEvent->Type() == PMO_Init)
                  Init(((PMOInitEvent *)pEvent)->GetInfo());
    
              if (pEvent->Type() == PMO_Reset)
                  Reset(true);
    
              if (pEvent->Type() == PMO_Info) 
                  HandleTimeInfoEvent((PMOTimeInfoEvent *)pEvent);
    
              if (pEvent->Type() == PMO_Quit) 
              {
              Quit();
                  delete pEvent;
                  m_pTarget->AcceptEvent(new Event(INFO_DoneOutputting));
              
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

      if (m_Writer->Write((char *)pBuffer, m_data_size) == 0)
      {
         ReportError("Write failed. Disk full?");
         return;
      }
      
      m_iTotalBytesWritten += m_data_size;
      m_pInputBuffer->EndRead(m_data_size);
      
     m_pLmc->Wake();
      UpdateBufferStatus();
  }
}

