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
        
        $Id: soundcardpmo.cpp,v 1.1 2004/10/13 16:16:08 linha2 Exp $
____________________________________________________________________________*/

/* system headers */
#include <stdlib.h>
#include <iostream.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/audioio.h>
#include <stropts.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <assert.h>

/* project headers */
#include <config.h>
#include "soundcardpmo.h"
#include "lmc.h"
#include "eventdata.h"
#include "facontext.h"
#include "log.h"

#define DB printf("%s:%d\n", __FILE__, __LINE__);

extern    "C"
{
   PhysicalMediaOutput *Initialize(FAContext *context)
   {
      return new SoundCardPMO(context);
   }
}

SoundCardPMO::SoundCardPMO(FAContext *context) :
       PhysicalMediaOutput(context)
{
   m_properlyInitialized = false;

   myInfo = new OutputInfo();
   memset(myInfo, 0, sizeof(OutputInfo));

   m_pBufferThread = NULL;

   m_iOutputBufferSize = 0;
   m_iTotalBytesWritten = 0;
   m_iBytesPerSample = 0;
   m_iLastFrame = -1;
   m_iDataSize = 0;
   audio_fd = -1;

   if (!m_pBufferThread)
   {
      m_pBufferThread = Thread::CreateThread();
      assert(m_pBufferThread);
      m_pBufferThread->Create(SoundCardPMO::StartWorkerThread, this);
   }
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
   Reset(true);
   close(audio_fd);

   if (myInfo)
   {
      delete    myInfo;

      myInfo = NULL;
   }
}

void SoundCardPMO::SetVolume(int32 left, int32 right)
{
  struct audio_info ainfo;
  int v = (left + right) / 2;
  int mixFd = open("/dev/audioctl",O_RDWR | O_NONBLOCK);
  if (mixFd != -1) {
    ioctl(mixFd, AUDIO_GETINFO, &ainfo);
    ainfo.play.gain = (v*255)/100;
    ioctl(mixFd, AUDIO_SETINFO, &ainfo);
    close(mixFd);
  }
}

void SoundCardPMO::GetVolume(int32 &left, int32 &right)
{
  struct audio_info ainfo;
  int mixFd = open("/dev/audioctl",O_RDWR);
  int volume = 0;
  if (mixFd != -1) {
    ioctl(mixFd, AUDIO_GETINFO, &ainfo);
    volume = (ainfo.play.gain*100)/255;
    volume &= 0xFF;
    close(mixFd);
  }
  left = volume;
  right = volume;
}

int SoundCardPMO::audio_fd = -1;

//PORTING: This function contains a ton of OS specific stuff. Hack and
//         slash at will.
Error SoundCardPMO::Init(OutputInfo * info)
{
   m_properlyInitialized = false;

   if (!info)
   {
      info = myInfo;
   }
   else
   {
      // got info, so this is the beginning...
      if ((audio_fd = open("/dev/audio", O_WRONLY, 0)) < 0)
      {
         if (errno == EBUSY)
         {
            ReportError("Audio device is busy. Please make sure that "
                        "another program is not using the device.");
            return (Error) pmoError_DeviceOpenFailed;
         }
         else
         {
            ReportError("Cannot open audio device. Please make sure that "
                        "the audio device is properly configured.");
            return (Error) pmoError_DeviceOpenFailed;
         }
      }

      m_iDataSize = info->max_buffer_size;
   }

   int       fd = audio_fd;
   struct audio_info ainfo;

   if (ioctl(audio_fd, AUDIO_GETINFO, &ainfo) < 0)
   {
      ReportError("Cannot get the flags on the audio device.");
      return (Error) pmoError_IOCTL_F_GETFL;
   }

   audio_fd = fd;

   channels = info->number_of_channels;

   for (unsigned int i = 0; i < info->number_of_channels; ++i)
      bufferp[i] = buffer + i;

   // configure the device:
   int       play_precision = 16;
//   int       play_stereo = channels - 1;
   int       play_sample_rate = info->samples_per_second;

   if (ioctl(audio_fd, I_FLUSH, FLUSHRW) == -1)
   {
      ReportError("Cannot reset the soundcard.");
      return (Error) pmoError_IOCTL_SNDCTL_DSP_RESET;
   }
   
   AUDIO_INITINFO(&ainfo);
   ainfo.play.precision = play_precision;
   ainfo.play.channels = channels;
   ainfo.play.sample_rate = play_sample_rate;
   ainfo.play.encoding = AUDIO_ENCODING_LINEAR;

   if (ioctl(audio_fd, AUDIO_SETINFO, &ainfo) == -1)
   {
      ReportError("Cannot set the soundcard's sampling speed.");
      return (Error) pmoError_IOCTL_SNDCTL_DSP_SPEED;
   }
   myInfo->bits_per_sample = info->bits_per_sample;
   myInfo->number_of_channels = info->number_of_channels;
   myInfo->samples_per_second = info->samples_per_second;
   myInfo->max_buffer_size = info->max_buffer_size;
   m_properlyInitialized = true;

   // PORTING: The GETOSPACE ioctl determines how much space the kernel's
   // output buffer has. Your OS may not have this.

   m_iTotalFragments = 2048; /* An arbitrary value of 2048. */
   m_iOutputBufferSize = play_precision * m_iTotalFragments;
   m_iBytesPerSample = info->number_of_channels * (info->bits_per_sample / 8);

   return kError_NoErr;
}

Error SoundCardPMO::Reset(bool user_stop)
{
   int a;

   if (audio_fd <= 0)
      return kError_NoErr;

   if (user_stop)
   {
      //PORTING: DSP_RESET stops playback immediately and returns
      if (ioctl(audio_fd, I_FLUSH, FLUSHRW) == -1)
      {
         ReportError("Cannot reset the soundcard.");
         return (Error)pmoError_IOCTL_SNDCTL_DSP_RESET;
      }
      Init(NULL);
   }
   else
   {
      //PORTING: DSP_SYNC blocks until the soundcard is done playing and
      //         then returns
      if (ioctl(audio_fd, AUDIO_DRAIN, &a) == -1)
      {
         ReportError("Cannot reset the soundcard.");
         return (Error)pmoError_IOCTL_SNDCTL_DSP_RESET;
      }
   }
   return kError_NoErr;
}

// PORTING: This function returns when the sound card is done playing, or
// when exit or pause is signaled. Returns true if the card naturally ran
// out of things to play. False if m_bExit or m_bPause became true
bool SoundCardPMO::WaitForDrain(void)
{
   struct audio_info info;

   for(; !m_bExit && !m_bPause; )
   {
       ioctl(audio_fd, AUDIO_GETINFO, &info);
       if (info.play.error)
       {
           return true;
       }
       WasteTime();
   }
   return false;
}

// PORTING: This is the bitchy function. In essence, this function is
// trying to figure out what samples are currently playing. Under linux
// you don't always know how many bytes have been played so far, so
// this function makes a best guess by keeping track of the
// total number of bytes fed to the card (m_iTotalBytesWritten).
// It uses that number minus the amount of data in the buffer to
// guess at which sample is currently playing. That info is converted
// into time and then set to the UI. Have fun!
void SoundCardPMO::HandleTimeInfoEvent(PMOTimeInfoEvent *pEvent)
{
   MediaTimeInfoEvent *pmtpi;
   int32               hours, minutes, seconds;
   int                 x, iTotalTime = 0;
   audio_info          info;

   if (pEvent->GetFrameNumber() != m_iLastFrame + 1)
   {
       m_iTotalBytesWritten = 1152 * pEvent->GetFrameNumber() * 
                              m_iBytesPerSample; 
       AUDIO_INITINFO(&info);
       info.play.samples = m_iTotalBytesWritten / 4;
       x = ioctl(audio_fd, AUDIO_SETINFO, &info);
       //       printf("x == %d (%s)\n", x, strerror(errno));
   }
   m_iLastFrame = pEvent->GetFrameNumber();

   if (myInfo->samples_per_second <= 0 || pEvent->GetFrameNumber() < 2)
      return;

   ioctl(audio_fd, AUDIO_GETINFO, &info);

   // why 4? 2 bytes x 2 channels
   iTotalTime = (info.play.samples * 4) / 
                (m_iBytesPerSample * myInfo->samples_per_second);

   hours = iTotalTime / 3600;
   minutes = (iTotalTime / 60) % 60;
   seconds = iTotalTime % 60;

   if (hours < 0 || hours > 23 ||
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
   void       *pBuffer;
   Error       eErr;
   size_t      iRet;
   Event      *pEvent;
   //   audio_info info;
   bool        bPerfWarn = false;

   // Don't do anything until resume is called.
   m_pPauseSem->Wait();

   // Wait a specified prebuffer time... 
   PreBuffer();

   // The following should be abstracted out into the general thread
   // classes:
#ifdef __linux__
   struct sched_param sParam;

   sParam.sched_priority = sched_get_priority_max(SCHED_OTHER);
   pthread_setschedparam(pthread_self(), SCHED_OTHER, &sParam);
#endif

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

              if (!bPerfWarn)
              {
                  time_t t;
    
                  time(&t);
                  m_pContext->log->Log(LogPerf, "Output buffer underflow: %s", 
                             ctime(&t));
                  bPerfWarn = true;
              }
    
              WasteTime();
              continue;
          }

          // Is there an event pending that we need to take care of
          // before we play this block of samples?
          if (eErr == kError_EventPending)
          {
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
                  if (WaitForDrain())
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
          bPerfWarn = false;
          break;
      }

      // Now write the block to the audio device. If the block doesn't
      // all fit, pause and loop until the entire block has been played.
      // This loop could be written using non-blocking io...
      for(;;)
      {
          if (m_bExit || m_bPause)
              break;

	  //          ioctl(audio_fd, AUDIO_GETINFO, &info);
          if (0) //((unsigned)(info.fragments * info.fragsize) < m_iDataSize)
          {
              WasteTime();
              continue;
          }
          break;
      }        
      if (m_bExit || m_bPause)
      {
          m_pInputBuffer->EndRead(0);
          continue;
      }

      iRet = write(audio_fd, pBuffer, m_iDataSize);
      // write(audio_fd, pBuffer, 0); // for WaitForDrain(), maybe?
      if (iRet < 0)
      {
         m_pInputBuffer->EndRead(0);
         ReportError("Could not write sound data to the soundcard.");
         m_pContext->log->Error("Failed to write to the soundcard: %s\n", 
                               strerror(errno));
         break;
      }

      m_iTotalBytesWritten += iRet;
      m_pInputBuffer->EndRead(iRet);
      m_pLmc->Wake();
      UpdateBufferStatus();
   }
}

