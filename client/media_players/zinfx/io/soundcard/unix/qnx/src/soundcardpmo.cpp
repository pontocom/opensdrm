/*____________________________________________________________________________
        
        FreeAmp - The Free MP3 Player
        Driver for Advanced Linux Sound Architecture 
              http://www.alsa-project.org
 
        Portions Copyright (C) 1998-1999 EMusic.com

        alsapmo.cpp by Fleischer Gabor <flocsy@usa.net>
        uses code by Anders Semb Hermansen <ahermans@vf.telia.no>
        and Jaroslav Kysela <perex@jcu.cz>

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
#include <stdlib.h>
#include <iostream.h>
#include <sys/asoundlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

#ifdef __QNX__
#include <strings.h>
#endif

/* project headers */
#include <config.h>
#include "soundcardpmo.h"
#include "facontext.h"
#include "log.h"

#define DB printf("%s:%d\n", __FILE__, __LINE__);

extern "C"
{
   PhysicalMediaOutput *Initialize(FAContext *context) {
      return new SoundCardPMO(context);
   }
}

static int parse_gid(const char *str, snd_mixer_gid_t *gid)
{
    unsigned int size;
    unsigned char *ptr;

    bzero(gid, sizeof(*gid));
    ptr = (unsigned char *)gid->name;
    size = 0;
    while (*str) {
        if (size < sizeof(gid->name)) {
            *ptr++ = *str;
            size++;
        }
        str++;
    }
    return 0;
}

SoundCardPMO::SoundCardPMO(FAContext *context) :
	    PhysicalMediaOutput(context)
{
	uint32 deviceNameSize = 128;
   char scard[128];
   snd_mixer_t *pMixer;
   char  mixer_id[25]="Master";

   m_properlyInitialized = false;
   myInfo = new OutputInfo();
   memset(myInfo, 0, sizeof(OutputInfo));

   m_pBufferThread = NULL;

   m_iOutputBufferSize = 0;
   m_iBytesPerSample = 0;
   m_iBaseTime = -1;
   m_iDataSize = 0;

   if (!m_pBufferThread)
   {
      m_pBufferThread = Thread::CreateThread();
      assert(m_pBufferThread);
      m_pBufferThread->Create(SoundCardPMO::StartWorkerThread, this);
   }

   m_handle = NULL;
   m_device = NULL;
   m_channels = -1;
   m_rate = -1;

	m_device = (char *) malloc(deviceNameSize);
	m_pContext->prefs->GetPrefString(kALSADevicePref, m_device,
					&deviceNameSize);

   if (m_device)
   {
        if (sscanf(m_device, "%[^:]: %d", scard, &m_iDevice) != 2)
        {
           ReportError("The ALSADevice statement in the preference file"
                       "is improperly formatted. Format: ALSADevice: "
                       "[card name/card number]:[device number]");
           return;
        }

        m_iCard = snd_card_name(scard);
        if (m_iCard < 0) 
        {
            ReportError("Invalid ALSA card name/number specified.");
            return;
        }
        if (m_iDevice < 0 || m_iDevice > 31) 
        {
            ReportError("Invalid ALSA device number specified.");
            return;
        }
   }
   else
       m_iCard = m_iDevice = 0;


   switch (m_iDevice)
   {
       case 0:
          strncpy(mixer_id,"PCM",sizeof(mixer_id));
          break;
       case 1:
          strncpy(mixer_id,"PCM1",sizeof(mixer_id));
          break;
   }

   parse_gid(mixer_id, &m_gid);

   snd_mixer_open(&pMixer, m_iCard, 0);

   bzero(&m_group, sizeof(m_group));
   m_group.gid = m_gid;

   if ((m_iChannel = snd_mixer_group_read(pMixer, &m_group )) < 0)
   {
       strncpy(mixer_id,"Master",sizeof(mixer_id));

       parse_gid(mixer_id, &m_gid);
       m_group.gid = m_gid;
       m_iChannel = snd_mixer_group_read(pMixer, &m_group );
   }
   snd_mixer_close(pMixer);

   delete m_device;
}

SoundCardPMO::~SoundCardPMO() 
{
    m_bExit = true;
    m_pSleepSem->Signal();
    m_pPauseSem->Signal();

    if (m_pBufferThread)
    {
       m_pBufferThread->Join();
       delete m_pBufferThread;
    }

    snd_pcm_close(m_handle);
    if (myInfo) {
        delete myInfo;
        myInfo = NULL;
    }
}


void SoundCardPMO::SetVolume(int32 left, int32 right)
{
   int   err;
   snd_mixer_t *pMixer;

   err = snd_mixer_open(&pMixer, m_iCard, 0);
   if (err < 0)
      return;

   if (m_iChannel >= 0)
   {
      err = snd_mixer_group_read(pMixer, &m_group);
      if (err < 0)
         return;

      left = (int)((double)(m_group.max - m_group.min) * 
               (double)left * 0.01) + m_group.min;
      right = (int)((double)(m_group.max - m_group.min) * 
               (double)right * 0.01) + m_group.min;
      for (int chn = 0; chn <= SND_MIXER_CHN_LAST; chn++) {
          if (!(m_group.channels & (1<<chn)))
              continue;

          if (chn == 0)
              m_group.volume.values[chn] = left;
          else
          if (chn == 1)
              m_group.volume.values[chn] = right;
          else
              m_group.volume.values[chn] = (left + right) / 2;
      }
      snd_mixer_group_write(pMixer, &m_group);
   }
   snd_mixer_close(pMixer);
} 

void SoundCardPMO::GetVolume(int32 &left, int32 &right)
{
   int   err;
   snd_mixer_t *pMixer = NULL;

   err = snd_mixer_open(&pMixer, m_iCard, 0);
   if (err != 0)
   {
      return;
   }

   if (m_iChannel >= 0)
   {
      err = snd_mixer_group_read(pMixer, &m_group);
      if (err < 0)
         return;
   }
   else
      return;

   snd_mixer_close(pMixer);
   left = (int)(((float)((m_group.volume.values[0] - m_group.min) * 100) /
                        (float)(m_group.max - m_group.min)) + 0.5);
   right = (int)(((float)((m_group.volume.values[1] - m_group.min) * 100) /
                        (float)(m_group.max - m_group.min)) + 0.5);
} 

Error SoundCardPMO::Init(OutputInfo* info) 
{
    int                      err;
    snd_pcm_channel_params_t params;

    m_properlyInitialized = false;
    if (!info) 
    {
        info = myInfo;
    } 
    else 
    {
        // got info, so this is the beginning...
        m_iDataSize = info->max_buffer_size;

        err=snd_pcm_open(&m_handle, m_iCard, m_iDevice, 
                         SND_PCM_OPEN_PLAYBACK);
        if (err < 0)
        {
            ReportError("Audio device is busy. Please make sure that "
                        "another program is not using the device.");
            return (Error)pmoError_DeviceOpenFailed;
        }

        snd_pcm_nonblock_mode(m_handle, 1);
    }

    // configure the device:
    m_channels=info->number_of_channels;
    m_rate=info->samples_per_second;

    memset(&params, 0, sizeof(params)); 
    params.format.format = SND_PCM_SFMT_S16_LE;
    params.format.interleave = 1;
    //printf("Chan: %d rate: %d\n", m_channels, info->samples_per_second);
    params.format.voices = m_channels;
    params.format.rate = info->samples_per_second;
    params.channel = SND_PCM_CHANNEL_PLAYBACK;
    params.mode = SND_PCM_MODE_BLOCK;
    params.start_mode = SND_PCM_START_DATA;
    params.stop_mode = SND_PCM_STOP_STOP;
    params.buf.block.frag_size = m_iDataSize;
    params.buf.block.frags_max = 32;
    params.buf.block.frags_min = 1;

    err = snd_pcm_channel_params(m_handle, &params);
    if (err < 0)
    {
        ReportError("Cannot initialized audio device.");
        return (Error)pmoError_DeviceOpenFailed;
    }
    err = snd_pcm_channel_prepare(m_handle, SND_PCM_CHANNEL_PLAYBACK);
    if (err < 0)
    {
        ReportError("Cannot initialized audio device.");
        return (Error)pmoError_DeviceOpenFailed;
    }

    memcpy(myInfo, info, sizeof(OutputInfo));

    snd_pcm_channel_setup_t aInfo;
    aInfo.channel = SND_PCM_CHANNEL_PLAYBACK;
    err = snd_pcm_channel_setup(m_handle,&aInfo);
    if (err < 0)
    {
        ReportError("Cannot initialized audio device.");
        return (Error)pmoError_DeviceOpenFailed;
    }

    m_iOutputBufferSize = aInfo.buf.block.frag_size * aInfo.buf.block.frags;
    m_iBytesPerSample = info->number_of_channels * (info->bits_per_sample / 8);



    m_properlyInitialized = true;
    return kError_NoErr;
}

Error SoundCardPMO::Reset(bool user_stop) {

    if (user_stop) 
        snd_pcm_playback_drain(m_handle);
    else
        snd_pcm_playback_flush(m_handle);

    snd_pcm_playback_prepare(m_handle);

    return kError_NoErr;
}

void SoundCardPMO::Pause(void)
{
    m_iBaseTime = -1;

    PhysicalMediaOutput::Pause();
}

bool SoundCardPMO::WaitForDrain(void)
{
   snd_pcm_channel_status_t ainfo;

   for(; !m_bExit && !m_bPause; )
   {
       ainfo.channel = SND_PCM_CHANNEL_PLAYBACK;
       snd_pcm_channel_status(m_handle,&ainfo);

       if (ainfo.underrun || ainfo.status == SND_PCM_STATUS_UNDERRUN)
       {
           return true;
       }
       WasteTime();
   }

   return false;
} 

void SoundCardPMO::HandleTimeInfoEvent(PMOTimeInfoEvent *pEvent)
{
   MediaTimeInfoEvent *pmtpi;
   int32               hours, minutes, seconds;
   int                 iTotalTime = 0;
   snd_pcm_channel_status_t ainfo;

   if (m_iBaseTime < 0)
   {
       m_iBaseTime = (pEvent->GetFrameNumber() * 
                      myInfo->samples_per_frame) / 
                      myInfo->samples_per_second;
   }

   ainfo.channel = SND_PCM_CHANNEL_PLAYBACK;
   snd_pcm_channel_status(m_handle,&ainfo);

   iTotalTime = ainfo.scount / (m_iBytesPerSample * myInfo->samples_per_second);
   //printf("total: %d base: %d\n", iTotalTime, m_iBaseTime);
   iTotalTime = (iTotalTime + m_iBaseTime) % 86400;

   hours = iTotalTime / 3600;
   minutes = (iTotalTime / 60) % 60;
   seconds = iTotalTime % 60;

   if (hours < 0  ||
       minutes < 0 || minutes > 59 || 
       seconds < 0 || seconds > 59)
      return;

   pmtpi = new MediaTimeInfoEvent(hours, minutes, seconds, 0, 
                                  iTotalTime, 0);
   m_pTarget->AcceptEvent(pmtpi);
}

void SoundCardPMO::StartWorkerThread(void *pVoidBuffer)
{
   ((SoundCardPMO*)pVoidBuffer)->WorkerThread();
}

void SoundCardPMO::WorkerThread(void)
{
   void                      *pBuffer;
   Error                      eErr;
   int                        iRet = -1;
   Event                     *pEvent;
   snd_pcm_channel_status_t  ainfo;

   // Don't do anything until resume is called.
   m_pPauseSem->Wait();

   // Sleep for a pre buffer period
   PreBuffer();

   // The following should be abstracted out into the general thread
   // classes:
#ifdef __linux__
   struct sched_param sParam;

   sParam.sched_priority = sched_get_priority_max(SCHED_OTHER);
   pthread_setschedparam(pthread_self(), SCHED_OTHER, &sParam);
#endif

   ainfo.channel = SND_PCM_CHANNEL_PLAYBACK;
   for(; !m_bExit;)
   {
      if (m_bPause)
      {
          m_pPauseSem->Wait();
          continue;
      }

      // Loop until we get an Init event from the LMC
      if (!m_properlyInitialized)
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
          eErr = ((EventBuffer *)m_pInputBuffer)->BeginRead(pBuffer, 
                                                             m_iDataSize);
          if (eErr == kError_EndOfStream || eErr == kError_Interrupt)
             break;

          if (eErr == kError_NoDataAvail)
          {
              m_pLmc->Wake();
              CheckForBufferUp();

              WasteTime();
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
                  ((EventBuffer *)m_pInputBuffer)->GetNumBytesInBuffer() > 0) 
              {
                  if (WaitForDrain())
				  {
                     Reset(true);
                     m_pTarget->AcceptEvent(new Event(INFO_DoneOutputting));
                     return;
				  }
                  continue;
              }
              
              pEvent = ((EventBuffer *)m_pInputBuffer)->GetEvent();

              if (pEvent->Type() == PMO_Init)
                  Init(((PMOInitEvent *)pEvent)->GetInfo());
    
              if (pEvent->Type() == PMO_Reset)
                  Reset(false);
    
              if (pEvent->Type() == PMO_Info) 
                  HandleTimeInfoEvent((PMOTimeInfoEvent *)pEvent);
    
              if (pEvent->Type() == PMO_Quit) 
              {
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
              break;
          }
          break;
      }

      // Now write the block to the audio device. If the block doesn't
      // all fit, pause and loop until the entire block has been played.
      // This loop could be written using non-blocking io...
      for(;!m_bExit && !m_bPause;)
      {
          iRet = snd_pcm_write(m_handle,pBuffer,m_iDataSize);
          if (iRet == -EAGAIN)
          {
               WasteTime();
               continue;
          }
          break;
      }
      if (m_bExit)
          return;

      if (m_bPause)
      {
         if (iRet == -EAGAIN)
             m_pInputBuffer->EndRead(0);
         else
         {
             m_pInputBuffer->EndRead(iRet);
             UpdateBufferStatus();
         }
         continue;   
      }

      if (iRet < 0)
      {
         //printf("ALSA: %s (%d==%d)\n", snd_strerror(iRet), iRet, EAGAIN);
         m_pInputBuffer->EndRead(0);
         ReportError("Could not write sound data to the soundcard.");
         m_pContext->log->Error("Failed to write to the soundcard: %s\n", 
                               strerror(errno));
         break;
      }

      m_pInputBuffer->EndRead(iRet);
      m_pLmc->Wake();
      UpdateBufferStatus();
   }
}
