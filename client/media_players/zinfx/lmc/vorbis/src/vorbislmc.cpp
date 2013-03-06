/*____________________________________________________________________________
   
   FreeAmp - The Free MP3 Player

   Portions Copyright (C) 2000 Monty

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
   
   $Id: vorbislmc.cpp,v 1.1 2004/10/13 16:16:10 linha2 Exp $
____________________________________________________________________________*/

/* system headers */
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "errors.h"
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

#include "vorbislmc.h" 

#define DB printf("%s:%d\n",  __FILE__, __LINE__);

extern    "C"
{
   LogicalMediaConverter *Initialize(FAContext *context)
   {
      return new VorbisLMC(context);
   }
}

const int iDecodeBlockSize = 8192;
const int iFramesPerSecond = 10;
const int iBitrateLoopsPerUpdate = iFramesPerSecond * 3;
const int iInitialOutputBufferSize = 65536; 
const char *szFailRead = "Cannot read vorbis data from input plugin.";
const char *szFailWrite = "Cannot write audio data to output buffer.";
const char *szCannotDecode = "Skipped corrupted file.";

VorbisLMC::VorbisLMC(FAContext *context) :
         LogicalMediaConverter(context)
{
   m_pContext = context;

   m_decoderThread = NULL;
   m_szUrl = NULL;
   m_szError = NULL;
   m_bInit = false;
   m_newPos = -1;
   m_decodeInfo.sendInfo = true;
}

VorbisLMC::~VorbisLMC()
{
   if (m_decoderThread)
   {
      m_bExit = true;
      m_pPauseSem->Signal();
      m_pSleepSem->Signal();

      m_decoderThread->Join();

      delete m_decoderThread;
      m_decoderThread = NULL;
   }
}

Error VorbisLMC::Prepare(PullBuffer *pInputBuffer, PullBuffer *&pOutBuffer)
{
   m_pInputBuffer = pInputBuffer;


   m_pOutputBuffer = new EventBuffer(iInitialOutputBufferSize, 0, 
                                    m_pContext);
   if (!m_decoderThread)
   {
      m_decoderThread = Thread::CreateThread();
      if (!m_decoderThread)
      {
         return kError_CreateThreadFailed;
      }
      m_decoderThread->Create(VorbisLMC::DecodeWorkerThreadFunc, this);
   }

   pOutBuffer =  m_pOutputBuffer;

   m_pInputBuffer->SetName("Input");
   m_pOutputBuffer->SetName("Output");

   return kError_NoErr;
}

void VorbisLMC::Clear()
{
   if (m_pOutputBuffer)
      ((EventBuffer *)m_pOutputBuffer)->Clear();
}

vector<string> *VorbisLMC::GetExtensions(void)
{
   vector<string> *extList = new vector<string>;

   extList->push_back("OGG");

   return extList;
}

Error VorbisLMC::CanDecode()
{
   Error err;

   if (!m_bInit)
   {
       err = InitDecoder();
       if (err != kError_NoErr)
           return err;
   }

   return kError_NoErr;
}

Error VorbisLMC::InitDecoder()
{
   ov_callbacks     callbacks;
   vorbis_info     *vi;
   Error            result;
   int              iNewSize;

   if (!m_pTarget || !m_pPmi || !m_pPmo || !m_pInputBuffer || !m_pOutputBuffer)
   {
      return kError_PluginNotInitialized;
   }

   memset(&m_vf, 0, sizeof(m_vf));
   callbacks.read_func = VorbisLMC::ReadWrapper;
   callbacks.seek_func = VorbisLMC::SeekWrapper;
   callbacks.close_func = VorbisLMC::CloseWrapper;
   callbacks.tell_func = VorbisLMC::TellWrapper;    

   if (ov_open_callbacks(this, &m_vf, NULL, 0, callbacks) < 0)
       return kError_UnknownErr;

   vi = ov_info(&m_vf, -1);
   m_channels = vi->channels;
   m_rate = vi->rate;
   m_section = -1;

   m_pContext->prefs->GetPrefInt32(kOutputBufferSizePref, &iNewSize);
   iNewSize = max(iNewSize, iMinimumOutputBufferSize);
   iNewSize *= 1024;

   result = m_pOutputBuffer->Resize(iNewSize, iNewSize / 6);
   if (IsError(result))
   {
       ReportError("Internal buffer sizing error occurred.");
       m_pContext->log->Error("Resize output buffer failed.");
       return result;
   }   
   m_bInit = true;

   return kError_NoErr;
}
 
Error VorbisLMC::ExtractMediaInfo()
{
   Error           err;
   float           totalSeconds;
   MediaInfoEvent *pMIE;
   vorbis_info    *vi;

   if (!m_bInit)
   {
       err = InitDecoder();
       if (err != kError_NoErr)
           return err;
   }

   totalSeconds = ov_time_total(&m_vf, 0);

   vi = ov_info(&m_vf, -1);
   pMIE = new MediaInfoEvent(m_pPmi->Url(), totalSeconds);
   if (!pMIE)
      return kError_OutOfMemory;

   VorbisInfoEvent *mie = new VorbisInfoEvent(ov_bitrate(&m_vf, 0),
                                              vi->channels, 
                                              vi->rate, 
                                              1. / (float)iFramesPerSecond);
   if (mie)
   {
       pMIE->AddChildEvent((Event *) mie);
       mie = NULL;
   }
   else
   {
       return kError_OutOfMemory;
   }

   if (m_pTarget)
      m_pTarget->AcceptEvent(pMIE);

   return kError_NoErr;
}

uint32 VorbisLMC::CalculateSongLength(const char *url)
{
    char            path[_MAX_PATH];
    uint32          len = _MAX_PATH;
    FILE           *fpFile;
    OggVorbis_File  vf;
    double          dur;

    URLToFilePath(url, path, &len);
    fpFile = fopen(path, "rb");
    if (fpFile == NULL)
       return 0;  

    memset(&vf, 0, sizeof(vf));
    if (ov_open(fpFile, &vf, NULL, 0) < 0)
    {
       fclose(fpFile);
       return 0; 
    }

    dur = ov_time_total(&vf, 0);

    ov_clear(&vf);

    return (int)dur;
}

void VorbisLMC::DecodeWorkerThreadFunc(void *pxlmc)
{
   if (pxlmc)
   {
      VorbisLMC  *xlmc = (VorbisLMC *) pxlmc;

      xlmc->DecodeWork();
   }
}

void VorbisLMC::DecodeWork()
{
   void          *pOutBuffer;
   Error          Err;
   int32          iValue;
   int32          section, ret;
   OutputInfo    *info;
   vorbis_info   *vi;
   uint32         bytesCopied, bytesPerFrame;
   int            bitrateLoops = 0;

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

   if (!m_bInit)
   {
       Err = InitDecoder();
       if (Err == kError_Interrupt)
          return;

       if (IsError(Err))
       {
           m_pContext->log->Error("Initializing the decoder failed: %d\n", Err);
           ReportError("Initializing the decoder failed.");
    
           return;
       }
   }

   m_pContext->prefs->GetPrefInt32(kDecoderThreadPriorityPref, &iValue);
   m_decoderThread->SetPriority(iValue);

   bytesCopied = 0;
   bytesPerFrame = 1;
   for (m_frameCounter = 0; !m_bExit;)
   {
      if (m_bPause)
      {
          m_pPauseSem->Wait();
          if (m_bExit)
              break;
      }

      if (m_newPos >= 0)
      {   
          ov_time_seek(&m_vf, (double)(m_newPos / iFramesPerSecond));
          m_frameCounter = m_newPos - 1;
          m_newPos = -1;
          bytesCopied = bytesPerFrame;
      }

      if (bytesCopied >= bytesPerFrame)
      {
          m_frameCounter += bytesCopied / bytesPerFrame;
          bytesCopied %= bytesPerFrame;
          ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(
             new PMOTimeInfoEvent(m_frameCounter));

          bitrateLoops++;
          if (bitrateLoops == iBitrateLoopsPerUpdate && m_decodeInfo.sendInfo)
          {
             int b;

             b = ov_bitrate_instant(&m_vf),
             vi = ov_info(&m_vf, -1);
             VorbisInfoEvent *mie = new VorbisInfoEvent(b,
                                           vi->channels, 
                                           vi->rate, 
                                           1. / (float)iFramesPerSecond);
             m_pTarget->AcceptEvent(mie);

             bitrateLoops = 0;
          }
      }

      Err = m_pOutputBuffer->BeginWrite(pOutBuffer, iDecodeBlockSize);
      if (Err == kError_Interrupt)
      {
          break;
      }
      if (Err == kError_BufferTooSmall)
      {
          if (Sleep())
             break;

          continue;
      }
      if (Err != kError_NoErr)
      {
          ReportError(szFailWrite);
          m_pContext->log->Error("LMC: Cannot write to eventbuffer: %s (%d)\n",
                                  m_szError, Err);
          break;
      } 

      section = -1;
      ret = ov_read(&m_vf, (char *)pOutBuffer, iDecodeBlockSize, 
                    0, 2, 1, &section);
      if (ret == 0)
      {
         m_pOutputBuffer->EndWrite(0);
         break;
      }

      if (section != m_section)
      { 
          vi = ov_info(&m_vf, -1);

          info = new OutputInfo;
          info->bits_per_sample = 16;
          info->number_of_channels = m_channels = vi->channels;
          info->samples_per_second = m_rate = vi->rate;
          info->samples_per_frame = vi->rate / iFramesPerSecond;
          info->max_buffer_size = 16384;
          m_frameCounter = 0;
          bytesCopied = 0;
          bytesPerFrame = (vi->rate / iFramesPerSecond) * 
                          sizeof(ogg_int16_t) * vi->channels;
          m_section = section;
       
          m_pOutputBuffer->EndWrite(0);
          ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(new PMOInitEvent(info));
          ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(
             new PMOTimeInfoEvent(m_frameCounter));

          Err = m_pOutputBuffer->BeginWrite(pOutBuffer, iDecodeBlockSize);
          if (Err != kError_NoErr)
          {
             assert(0);
          }

          vorbis_comment *comment;

          comment = ov_comment(&m_vf, -1);
          if (comment)
          {
              PlaylistItem *plItem = m_pContext->plm->GetCurrentItem();
              if (plItem)
              {
                  MetaData mdata = plItem->GetMetaData();
                  string iso;

                  char *temp;
                  temp = vorbis_comment_query(comment, "title", 0);
                  if (temp)
                  {
                      iso = ConvertToISO(temp);
                      mdata.SetTitle(iso);
                  }
 
                  temp = vorbis_comment_query(comment, "artist", 0);
                  if (temp)
                  {
                      iso = ConvertToISO(temp);
                      mdata.SetArtist(iso);
                  }

                  temp = vorbis_comment_query(comment, "album", 0);
                  if (temp)
                  {
                      iso = ConvertToISO(temp);
                      mdata.SetAlbum(iso);
                  }

                  temp = vorbis_comment_query(comment, "tracknumber", 0);
                  if (temp)
                      mdata.SetTrack(atoi(temp));

                  plItem->SetMetaData(&mdata);
                  m_pContext->target->AcceptEvent(
                             new PlaylistCurrentItemInfoEvent(plItem, 
                                                              m_pContext->plm));

              }
          }
      }
      if(ret <0) 
         ret=0; // hole/error in data - we can safely ignore this
      m_pOutputBuffer->EndWrite(ret);

      bytesCopied += ret;
   }
   ((EventBuffer *)m_pOutputBuffer)->AcceptEvent(new PMOQuitEvent());
   ov_clear(&m_vf);

   return;
}

Error VorbisLMC::ChangePosition(int32 position)
{
   m_newPos = position;
   return kError_NoErr;
}

int VorbisLMC::SeekWrapper(void *stream, ogg_int64_t offset, int whence)
{
   return ((VorbisLMC *)stream)->Seek((int32)offset, whence);
}

int VorbisLMC::Seek(long offset, int whence)
{
   int32 ret = -1;

   if (m_pPmi->IsStreaming())
      return -1;

   m_pPmi->Seek(ret, offset, whence);   
   return ret;
}

long VorbisLMC::TellWrapper(void *stream)
{
   return ((VorbisLMC *)stream)->Tell();
}

long VorbisLMC::Tell(void)
{
   int32 ret = -1;

   if (m_pPmi->IsStreaming())
      return -1;

   m_pPmi->Tell(ret);   
   return ret;
}

int VorbisLMC::CloseWrapper(void *stream)
{
   return 0;
}

size_t VorbisLMC::ReadWrapper(void *buf, size_t size, size_t num, void *stream)
{
   return ((VorbisLMC *)stream)->Read(buf, size, num);
}

size_t VorbisLMC::Read(void *buf, size_t size, size_t num)
{
   size_t bytes = size * num;
   void   *ptr;
   Error   Err;

   for(;;)
   {
       Err = m_pInputBuffer->BeginRead(ptr, bytes);
       if (Err == kError_NoDataAvail)
       {
           m_pPmi->Wake();
           if (Sleep())
              return kError_Interrupt;
           continue;
       }
       if (Err == kError_EndOfStream)
       {
           bytes = m_pInputBuffer->GetNumBytesInBuffer();
           if (bytes == 0)
              return (size_t)0;
           continue;
       }
       break;
   }

   if (Err != kError_NoErr)
   {
       return (size_t)-1;
   }

   memcpy(buf, ptr, bytes);
   m_pInputBuffer->EndRead(bytes);

   return bytes / size;   
}

Error VorbisLMC::SetDecodeInfo(DecodeInfo &info)
{
    m_decodeInfo = info;
    return kError_NoErr;
}

const string VorbisLMC::ConvertToISO(const char *utf8)
{
   unsigned char *in, *buf;
   unsigned char *out, *end;
   string               ret;

   in = (unsigned char *)utf8;
   buf = out = new unsigned char[strlen(utf8) + 1];
   end = in + strlen(utf8);
   for(;*in != 0x00 && in <= end; in++, out++)
   {
       if (*in < 0x80)
       {  /* lower 7-bits unchanged */
          *out = *in;
       }
       else
       if (*in > 0xC3)
       { /* discard anything above 0xFF */
          *out = '?';
       }
       else
       if (*in & 0xC0)
       { /* parse upper 7-bits */
          if (in >= end)
            *out = 0;
          else
          {
            *out = (((*in) & 0x1F) << 6) | (0x3F & (*(++in)));
          }
       }
       else
       {
          *out = '?';  /* this should never happen */
       }
   }
   *out = 0x00; /* append null */
   ret = string((char *)buf);
   delete buf;

   return ret;
}

