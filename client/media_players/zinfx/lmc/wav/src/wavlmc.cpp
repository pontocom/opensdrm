/*____________________________________________________________________________
   
   FreeAmp - The Free MP3 Player

   Portions Copyright (C) 1998-1999 EMusic.com
   Portions Copyright (C) 2001 "John Cantrill" <thejohncantrill@hotmail.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, Write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   
   $Id: wavlmc.cpp,v 1.1 2004/10/13 16:16:10 linha2 Exp $
____________________________________________________________________________*/

#ifdef WIN32
#include <windows.h>
#endif

/* system headers */
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <string.h>

#include "config.h"
#include "errors.h"
#include "wavlmc.h"
#include "pmoevent.h"
#include "eventbuffer.h"
#include "event.h"
#include "eventdata.h"
#include "mutex.h"
#include "semaphore.h"
#include "preferences.h"
#include "lmc.h"
#include "facontext.h"
#include "log.h"
#include "debug.h"

#define DB Debug_v("%s:%d\n",  __FILE__, __LINE__);
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) (a >= b ? a : b)

const int iInitialOutputBufferSize = 64512;

extern    "C"
{
   LogicalMediaConverter *Initialize(FAContext *context)
   {
      return new WavLMC(context);
   }
}

const int iMaxDecodeRetries = 32;
const int iDefaultBufferUpInterval = 3;

const char *szFailRead = "Cannot read WAV data from input plugin.";
const char *szFailWrite = "Cannot write audio data to output buffer.";
const char *szCannotDecode = "Skipped corrupted file.";

WavLMC::WavLMC(FAContext *context) :
         LogicalMediaConverter(context)
{
   m_pContext = context;

   m_decoderThread = NULL;
   m_frameBytes = -1;
   m_szUrl = NULL;
   m_szError = NULL;
   m_iMaxWriteSize = 0;
   m_iBufferUpInterval = iDefaultBufferUpInterval;
   m_frameCounter = 0;
   m_pPmi = NULL;
   m_pPmo = NULL;
   m_fpFile = NULL;
   m_pLocalReadBuffer = NULL;
   m_iTotalFrames = -1;
   m_decodeInfo.downsample = 0;
   m_decodeInfo.mono = false;
   m_decodeInfo.eightbit = false;
   m_decodeInfo.sendInfo = true;

   memset(&m_WaveFormat, 0, sizeof(WAVEFORMAT));
   m_ulWaveSize = 0;
   m_ulWaveHeaderSize = 0;
   m_iFramesPerSecond = 100;
}

WavLMC::~WavLMC()
{
   if (m_decoderThread)
   {
      m_bExit = true;
      m_pPauseSem->Signal();
      m_pSleepSem->Signal();

      m_decoderThread->Join();
      m_pContext->log->Log(LogDecode, "LMC: Decoder thread exited.\n");

      delete m_decoderThread;
      m_decoderThread = NULL;
   }
}

Error WavLMC::Prepare(PullBuffer *pInputBuffer, PullBuffer *&pOutBuffer)
{
   m_pInputBuffer = pInputBuffer;


   m_pOutputBuffer = new EventBuffer(iInitialOutputBufferSize, 0, 
                                    m_pContext);

   if (!m_decoderThread)
   {
      m_decoderThread = Thread::CreateThread();
      if (!m_decoderThread)
      {
         return (Error) lmcError_DecoderThreadFailed;
      }
      m_decoderThread->Create(WavLMC::DecodeWorkerThreadFunc, this);
   }

   pOutBuffer =  m_pOutputBuffer;

   return kError_NoErr;
}

void WavLMC::Clear()
{
   if (m_pOutputBuffer)
      ((EventBuffer *)m_pOutputBuffer)->Clear();
}

Error WavLMC::GetHeadInfo()
{
   bool        bWaveFormat = false;
   if(m_ulWaveSize > 0)
      goto HeadInfoEnd;

   unsigned long ulRIFF;
   unsigned long ulLength;
   unsigned long ulWAVE;
   unsigned long ulType;
   unsigned long ulCount;
   unsigned long ulLimit;
   void        *pBuffer;
   Error       Err;

   // first three double words should be RIFF, length, WAVE
   Err = BeginRead(pBuffer, 12);
   if (IsError(Err))
   {
      if (Err != kError_EndOfStream && Err != kError_Interrupt)
      {
          if (m_decodeInfo.sendInfo) 
              ReportError(szFailRead);
          else
              ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(new PMOErrorEvent());
      }
      EndRead(0);
      return Err;
   }

   ulRIFF = (unsigned long)(((unsigned long *)pBuffer)[0]);
   ulLength = (unsigned long)(((unsigned long *)pBuffer)[1]);
   ulWAVE = (unsigned long)(((unsigned long *)pBuffer)[2]);
   EndRead(12);

   if(ulRIFF != MAKEFOURCC('R', 'I', 'F', 'F') || ulWAVE != MAKEFOURCC('W', 'A', 'V', 'E'))
      return kError_InputUnsuccessful;

   // Run through the bytes looking for the tags
   ulCount = 0;
   ulLimit = ulLength - 4;
   while (ulCount < ulLimit)
   {
      Err = BeginRead(pBuffer, 8);
      ulCount += 8;
      if (IsError(Err))
      {
         if (Err != kError_EndOfStream && Err != kError_Interrupt)
         {
             if (m_decodeInfo.sendInfo) 
                 ReportError(szFailRead);
             else
                 ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(new PMOErrorEvent());
         }
         EndRead(0);
         return Err;
      }
      ulType   = (unsigned long)(((unsigned long *)pBuffer)[0]);
      ulLength = (unsigned long)(((unsigned long *)pBuffer)[1]);
      EndRead(8);

      switch (ulType)
      {
         // format
         case MAKEFOURCC('f', 'm', 't', ' '):
            if (ulLength < sizeof(WAVEFORMAT))
               return kError_InputUnsuccessful;

            Err = BeginRead(pBuffer, ulLength);
            ulCount += ulLength;
            if (IsError(Err))
            {
               if (Err != kError_EndOfStream && Err != kError_Interrupt)
               {
                   if (m_decodeInfo.sendInfo) 
                      ReportError(szFailRead);
                   else
                      ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(new PMOErrorEvent());
               }
               EndRead(0);
               return Err;
            }
            memcpy(&m_WaveFormat, pBuffer, sizeof(WAVEFORMAT));
            EndRead(ulLength);
            bWaveFormat = true;
            break;

         // data
         case MAKEFOURCC('d', 'a', 't', 'a'):
            m_ulWaveSize = ulLength;
            if(bWaveFormat)
            {
               m_ulWaveHeaderSize = ulCount + 12;
               m_frameBytes = m_WaveFormat.nAvgBytesPerSec / m_iFramesPerSecond;
               goto HeadInfoEnd;
            }
            break;

         default:
            Err = BeginRead(pBuffer, ulLength);
            ulCount += ulLength;
            if (IsError(Err))
            {
               if (Err != kError_EndOfStream && Err != kError_Interrupt)
               {
                   if (m_decodeInfo.sendInfo) 
                      ReportError(szFailRead);
                   else
                      ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(new PMOErrorEvent());
               }
               EndRead(0);
               return Err;
            }
            EndRead(ulLength);
            break;

      }
   }

   return (Error)lmcError_DecodeFailed;

HeadInfoEnd:
   return (m_WaveFormat.wFormatTag == WAVE_FORMAT_PCM) ? kError_NoErr : kError_InputUnsuccessful;
}

vector<string> *WavLMC::GetExtensions(void)
{
   vector<string> *extList = new vector<string>;

   extList->push_back("WAV");

   return extList;
}

Error WavLMC::CanDecode()
{
   Error      Err;

   if (!m_pInputBuffer)
   {
      m_pContext->log->Error("CanDecode() called, with no PMI set.\n");
      return kError_PluginNotInitialized;
   }

   Err = GetHeadInfo();
   if (Err == kError_Interrupt)
      return Err;

   if (Err != kError_NoErr)
   {
       m_pContext->log->Log(LogDecode, "GetHeadInfo() in CanDecode() could not find the sync marker.\n");
       return Err;
   }

   if (IsError(m_pContext->prefs->
           GetPrefInt32(kStreamBufferIntervalPref, &m_iBufferUpInterval)))
      m_iBufferUpInterval = iDefaultBufferUpInterval;

   return kError_NoErr;
}

Error WavLMC::ExtractMediaInfo()
{
   Error           eRet;
   float           totalSeconds = -1;
   MediaInfoEvent *pMIE;

   eRet = GetStats(totalSeconds, m_iTotalFrames);
   if (IsError(eRet))
      return eRet; 

   pMIE = new MediaInfoEvent(m_pPmi->Url(), totalSeconds);
   if (!pMIE)
      return kError_OutOfMemory;

   MpegInfoEvent *mie = new MpegInfoEvent(m_iTotalFrames,
       /*(float)m_WaveFormat.nBlockAlign */ (float)1.0 / m_iFramesPerSecond,
       m_frameBytes,
       m_WaveFormat.nAvgBytesPerSec * 8, m_WaveFormat.nSamplesPerSec, 0,
      0, m_WaveFormat.nChannels, 0, 0, 0, 1, 0);
   mie->SetWAV(1);
   if (mie)
   {
       pMIE->AddChildEvent((Event *) mie);
       mie = NULL;
   } else
       return kError_OutOfMemory;

   if (m_pTarget && m_decodeInfo.sendInfo)
      m_pTarget->AcceptEvent(pMIE);
   else 
      delete pMIE;

   return kError_NoErr;
}

Error WavLMC::GetStats(float &fTotalSeconds, int32 &iTotalFrames)
{
   Error        Err;

   fTotalSeconds = -1.0;
   iTotalFrames = 0;

   if (!m_pPmi && !m_fpFile)
      return kError_NullValueInvalid;
 
   if (m_frameBytes < 0)
   {
       Err = GetHeadInfo();
       if (Err != kError_NoErr)
            return Err;
   }

   iTotalFrames = (m_ulWaveSize + m_frameBytes - 1) / m_frameBytes;
   fTotalSeconds = m_ulWaveSize / m_WaveFormat.nAvgBytesPerSec;

   return kError_NoErr;
}

uint32 WavLMC::CalculateSongLength(const char *szUrl)
{
    char   path[_MAX_PATH];
    uint32 len = _MAX_PATH;
    float  fTotalSeconds;
    int32    iTotalFrames;
    Error  eRet;

    URLToFilePath(szUrl, path, &len);
    m_fpFile = fopen(path, "rb");
    if (m_fpFile == NULL)
       return 0;

    eRet = GetStats(fTotalSeconds, iTotalFrames );
    fclose(m_fpFile);
    m_fpFile = NULL;
                              
    if (!IsError(eRet))   
    {
        if (fTotalSeconds < 0)
           return 0;

        return (int)fTotalSeconds;
    }
    
    return 0;
}

Error WavLMC::InitDecoder()
{
   Error          Err;

   m_pContext->log->Log(LogDecode, "InitDecoder\n");

   if (!m_pTarget || !m_pPmi || !m_pPmo || !m_pInputBuffer || !m_pOutputBuffer)
   {
      return kError_NullValueInvalid;
   }

   if (m_frameBytes < 0)
   {
       Err = GetHeadInfo();
         if (Err != kError_NoErr)
            return Err;
   }

   Error         result;
   int32         iNewSize;
   OutputInfo *info = new OutputInfo;

   info->bits_per_sample = (m_WaveFormat.nBlockAlign * 8) / m_WaveFormat.nChannels;
   info->number_of_channels = m_WaveFormat.nChannels;
   info->samples_per_second = m_WaveFormat.nSamplesPerSec;

   m_iMaxWriteSize = m_frameBytes;
   info->samples_per_frame = m_frameBytes;
   info->max_buffer_size = m_iMaxWriteSize;

   m_pContext->prefs->GetPrefInt32(kOutputBufferSizePref, &iNewSize);
   iNewSize = max(iNewSize, iMinimumOutputBufferSize);
   iNewSize *= 1024;
   
   iNewSize -= iNewSize % m_iMaxWriteSize;
   result = m_pOutputBuffer->Resize(iNewSize, 0);

   if (IsError(result))
   {
      ReportError("Internal buffer sizing error occurred.");
      m_pContext->log->Error("Resize output buffer failed.");
      return result;
   } 

   ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(new PMOInitEvent(info));

   return (Error) kError_NoErr;
}

void WavLMC::DecodeWorkerThreadFunc(void *pxlmc)
{
   if (pxlmc)
   {
      WavLMC  *wlmc = (WavLMC *) pxlmc;

      wlmc->DecodeWork();
   }
}

void WavLMC::DecodeWork()
{
   m_pContext->log->Log(LogDecode, "DecodeWork\n");

   void          *pBuffer, *pOutBuffer;
   Error          Err;
   int32          iLoop = 0, iValue, iReadSize = 0;
   bool           bRestart = false;

   assert(m_pPmi);
   assert(m_pPmo);

   m_pSleepSem->Wait();

   m_pPmi->Wake();

   Err = CanDecode();
   if (Err == kError_Interrupt)
      return;
   if (Err != kError_NoErr)
   {
       m_pContext->log->Error("CanDecode returned false.\n");
       if (m_decodeInfo.sendInfo)
       {
           ReportStatus(szCannotDecode);
           m_pTarget->AcceptEvent(new Event(INFO_DoneOutputtingDueToError));
       }
       else
           ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(new PMOErrorEvent());
       return;
   }

   Err = ExtractMediaInfo();
   if (Err == kError_Interrupt)
      return;

   if (IsError(Err))
   {
       m_pContext->log->Error("ExtractMediaInfo failed: %d\n", Err);
       if (m_decodeInfo.sendInfo)
       {
           ReportStatus(szCannotDecode);
           m_pTarget->AcceptEvent(new Event(INFO_DoneOutputtingDueToError));
       }
       else
           ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(new PMOErrorEvent());
       return;
   }

   Err = InitDecoder();
   if (Err == kError_Interrupt)
      return;

   if (IsError(Err))
   {
       m_pContext->log->Error("Initializing the decoder failed: %d\n", Err);
       if (m_decodeInfo.sendInfo)
           ReportError("Initializing the decoder failed.");
       else
           ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(new PMOErrorEvent());
       return;
   }

   m_pContext->prefs->GetPrefInt32(kDecoderThreadPriorityPref, &iValue);
   m_decoderThread->SetPriority(iValue);

   for (m_frameCounter = 0; !m_bExit;)
   {
      bRestart = false;
      ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(
             new PMOTimeInfoEvent(m_frameCounter));

      for(; !m_bExit; )
      {
          if (m_bPause)
          {
              m_pPauseSem->Wait();
              if (m_bExit)
                  break;
          }

          if (m_frameCounter > m_iTotalFrames)
          {
              ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(new PMOQuitEvent());

              // Hang around until we're told to leave in case
              // the user seeks and wants to restart the decoder.
              while(!m_bExit)
              {
                  Sleep();
              }   
              if (!m_pInputBuffer->IsEndOfStream())
              {
                 bRestart = true;
                 break;
              }   
             
              return;
          }

          if (m_frameBytes > (int)m_pInputBuffer->GetNumBytesInBuffer() &&
              m_pInputBuffer->GetNumBytesInBuffer() > 0 &&
              m_pInputBuffer->IsEndOfStream())
          {
              iReadSize = m_pInputBuffer->GetNumBytesInBuffer();
          }
          else
          {
              iReadSize = m_frameBytes;    
          }

          Err = BeginRead(pBuffer, iReadSize);
          if (Err == kError_Interrupt)
          {
              break;
          }

          if (Err == kError_EndOfStream)
          {
              ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(new PMOQuitEvent());

              // Hang around until we're told to leave in case
              // the user seeks and wants to restart the decoder.
              while(!m_bExit && m_pInputBuffer->IsEndOfStream())
              {
                  Sleep();
              }   
              if (!m_pInputBuffer->IsEndOfStream())
              {
                 bRestart = true;
                 break;
              }   
              
              return;
          }
          if (Err == kError_NoDataAvail)
          {
              if (Sleep())
                  break;

              continue;
          }

          if (Err != kError_NoErr)
          {
              ReportError(szFailRead);
              m_pContext->log->Error("LMC: Cannot read from pullbuffer: %s\n", m_szError); 
              return;
          }

          Err = m_pOutputBuffer->BeginWrite(pOutBuffer, iReadSize);
          if (Err == kError_Interrupt)
          {
              m_pInputBuffer->EndRead(0);
              break;
          }
          if (Err == kError_BufferTooSmall)
          {
              m_pInputBuffer->EndRead(0);
              if (Sleep())
                 break;

              continue;
          }
          if (Err != kError_NoErr)
          {
              m_pInputBuffer->EndRead(0);
              if (m_decodeInfo.sendInfo)
                  ReportError(szFailWrite);
              else
                  ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(new PMOErrorEvent());
              m_pContext->log->Error("LMC: Cannot write to eventbuffer: %s (%d)\n", 
                        m_szError, Err); 
              return;
          }
          memcpy(pOutBuffer, pBuffer, iReadSize);
          break;
      }
      if (bRestart)
         continue;
         
      if (m_bExit || Err == kError_Interrupt || Err == kError_EndOfStream)
      {
          return;
      }

      EndRead(iReadSize);
      m_pPmi->Wake();

      if (m_pOutputBuffer)
      {
         Err = m_pOutputBuffer->EndWrite(iReadSize);
         if (Err == kError_Interrupt)
            break;

         if (IsError(Err))
         {
             m_pContext->log->Error("lmc: EndWrite returned %d\n", Err);
             if (m_decodeInfo.sendInfo)
                 ReportError(szFailWrite);
             else
                 ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(new PMOErrorEvent());
             return;
         }
      }

      if (iLoop == iMaxDecodeRetries)
      {
         m_pContext->log->Error("LMC: Maximum number of retries reached"
                      " while trying to decode.\n");
         if (m_decodeInfo.sendInfo) 
             ReportStatus(szCannotDecode);

         ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(new PMOErrorEvent());
         return;
      }

      m_frameCounter++;
   }

   return;
}

// These two functions are here just to support the CalculcateSongLength()
// function which opens a local file and then reads from it, rather than
// reading from the pipeline.
Error WavLMC::BeginRead(void *&pBuffer, unsigned int iBytesNeeded)
{
   Error eRet;

   if (m_fpFile)
   {
       int iRead;
       
       pBuffer = new char[iBytesNeeded];
       m_pLocalReadBuffer = (char*)pBuffer;
       
       iRead = fread(pBuffer, sizeof(char), iBytesNeeded, m_fpFile);
       if (iRead != (int)iBytesNeeded)
          return kError_EndOfStream;
          
       return kError_NoErr;
   }

   for(; !m_bExit;)
   {
       eRet = m_pInputBuffer->BeginRead(pBuffer, iBytesNeeded);
       if (eRet == kError_NoDataAvail)
       {
           m_pPmi->Wake();
           if (Sleep()) 
              return kError_Interrupt; 
           continue;
       }
       break;
   }
   if (m_bExit)
      return kError_Interrupt;

   return eRet;
}

Error WavLMC::EndRead(size_t iBytesUsed)
{
   if (m_fpFile)
   {
      delete [] m_pLocalReadBuffer;
      m_pLocalReadBuffer = NULL;
      return kError_NoErr;
   }
       
   return m_pInputBuffer->EndRead(iBytesUsed);
}

Error WavLMC::SetEQData(float *arrayEQ, float preamp) 
{
    return kError_NoErr;
}

Error WavLMC::SetEQData(bool enable)
{
    return kError_NoErr;
}

Error WavLMC::SetDecodeInfo(DecodeInfo &info) 
{
    m_decodeInfo = info;
    return kError_NoErr;
}

Error WavLMC::ChangePosition(int32 position)
{
   int32   dummy;
   uint32  lSeekTo;

   assert(position >= 0 && position < m_iTotalFrames);

   m_frameCounter = position;

   lSeekTo = m_ulWaveHeaderSize + (position * m_frameBytes);
      
   m_pPmi->Seek(dummy, lSeekTo, SEEK_FROM_START);

   return kError_NoErr;
}

