/*____________________________________________________________________________
        
        FreeAmp - The Free MP3 Player

        Portions Copyright (C) 1999-2000 EMusic.com

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
        
        $Id: cdpmo.cpp,v 1.1 2004/10/13 16:16:06 linha2 Exp $
____________________________________________________________________________*/

/* system headers */
#include <assert.h>

/* project headers */
#include <config.h>

#ifdef WIN32
#else
#include "cdaudio.h"
#endif

#include "cdpmo.h"
#include "lmc.h"
#include "eventdata.h"
#include "facontext.h"
#include "log.h"

#define DB printf("%s:%d\n", __FILE__, __LINE__);

extern    "C"
{
   PhysicalMediaOutput *Initialize(FAContext *context)
   {
      return new CDPMO(context);
   }
}

CDPMO::CDPMO(FAContext *context) :
       PhysicalMediaOutput(context)
{
   m_properlyInitialized = false;

   m_pBufferThread = NULL;
   m_track = -1;
   m_cdDesc = -1;
   sentData = false;
   trackDone = false;
}

CDPMO::~CDPMO()
{
   m_bExit = true;

   if (m_pBufferThread)
   {
      m_pBufferThread->Join();
      delete m_pBufferThread;
   }

   if (m_properlyInitialized)
       Reset(true);

   cd_finish(m_cdDesc);
}

void CDPMO::SetVolume( int32 left, int32 right)
{
    if (m_cdDesc != -1)
    {
        left = (int32)(((double)left / (double)100) * (double)255);
        right = (int32)(((double)right / (double)100) * (double)255);
        struct disc_volume vol;

        vol.vol_front.left = left;
        vol.vol_back.right = right;

        cd_set_volume(m_cdDesc, vol);
    }
}

void CDPMO::GetVolume(int32 &left, int32 &right)
{
    left = right = -1;
    if (m_cdDesc != -1)
    {
        struct disc_volume vol;
      
        cd_get_volume(m_cdDesc, &vol);
        left = (int32)(((double)vol.vol_front.left / (double)255) * 100);
        right = (int32)(((double)vol.vol_front.right / (double)255) * 100);
    }
} 

Error CDPMO::SetTo(const char *url)
{
   OutputInfo blah;

   if (IsError(Init(&blah))) 
       return kError_CDInitFailed;

   char *tracknumber = strrchr(url, '/');

   if (!tracknumber)
       tracknumber = (char *)url;
   else
       tracknumber++;

   m_track = atoi(tracknumber);

   if (m_track < 1 || m_track > MAX_TRACKS)
       return kError_InvalidTrack;

   if (!m_pBufferThread)
   {
      m_pBufferThread = Thread::CreateThread();
      assert(m_pBufferThread);
      m_pBufferThread->Create(CDPMO::StartWorkerThread, this);
   }

   cd_play_track(m_cdDesc, m_track, m_track);

   m_properlyInitialized = true;
   return kError_NoErr;
}

Error CDPMO::Init(OutputInfo *info)
{
   char device[256];
   uint32 len = 256;

   if (IsError(m_pContext->prefs->GetPrefString(kCDDevicePathPref, device, &len)))
           return (Error)pmoError_DeviceOpenFailed;

   m_cdDesc = cd_init_device(device);

   if (m_cdDesc < 0) {
       if (info)
           ReportError("Cannot open the CD-ROM device.  Please make sure that "
                       "the CD Device preference is set properly.");
       return (Error)pmoError_DeviceOpenFailed;
   }

   cddbid = 0;
   cdindexid = "";

   if (cd_stat(m_cdDesc, &dinfo, false) < 0) {
       if (info)
           ReportError("There is no disc in the CD-ROM drive.");

       return kError_NoDiscInDrive;
   }
   
   if (!dinfo.disc_present) {
       if (info)
           ReportError("There is no disc in the CD-ROM drive.");
       return kError_NoDiscInDrive;
   }

   cd_stat(m_cdDesc, &dinfo);

   uint32 tracks = dinfo.disc_total_tracks;
   cddbid = cddb_direct_discid(dinfo);
   char *cdindex = new char[256];
   cdindex_direct_discid(dinfo, cdindex, 256);

   cdindexid = cdindex;

   CDInfoEvent *cie = new CDInfoEvent(tracks, cddbid, cdindex);

   m_pTarget->AcceptEvent(cie);

   delete [] cdindex;

   return kError_NoErr;
}

uint32 CDPMO::GetCDDBDiscID(void)
{
   return cddbid;
}

char *CDPMO::GetcdindexDiscID(void)
{
    return (char *)cdindexid.c_str();
}

struct disc_info *CDPMO::GetDiscInfo(void)
{
    return &dinfo;
}

void CDPMO::Eject(void)
{
    if (m_cdDesc < 0)
        return;

    cd_eject(m_cdDesc);
}

Error CDPMO::Reset(bool user_stop)
{
   if (m_cdDesc < 0)
      return kError_NoErr;

   if (user_stop)
   {
       cd_stop(m_cdDesc);
   }
   else
   {
       cd_stop(m_cdDesc);
   }
   return kError_NoErr;
}

void CDPMO::Pause(void)
{
    if (m_cdDesc < 0)
        return;

    cd_pause(m_cdDesc);
}

void CDPMO::Resume(void)
{
    if (m_cdDesc < 0)
        return;

    cd_resume(m_cdDesc);
}

Error CDPMO::ChangePosition(int32 newposition)
{
    if (m_cdDesc < 0 || m_track < 0)
        return kError_NoErr;

    cd_play_track_pos(m_cdDesc, m_track, m_track, newposition);

    return kError_NoErr;
}

void CDPMO::HandleTimeInfoEvent(PMOTimeInfoEvent *pEvent)
{
   if (m_cdDesc < 0 || m_track < 0)
      return;

   struct disc_info disc;
   struct disc_timeval val;

   if (cd_stat(m_cdDesc, &disc, false) < 0) {
       trackDone = true;
       return;
   }
 
   if (!disc.disc_present) {
       trackDone = true;
       return;
   }

   cd_stat(m_cdDesc, &disc);

   if (!sentData) {
       val = disc.disc_track[m_track - 1].track_length;
       uint32 length = val.minutes * 60 + val.seconds;

       char *url = new char[30];
       sprintf(url, "CD Track %d", m_track);

       MediaInfoEvent *mie = new MediaInfoEvent(url, length);

       MpegInfoEvent *mpie = new MpegInfoEvent(length, 1, 0, 1411200, 44100, 0,
                                               0, 1, 0, 0, 0, 1, 0);
       mie->AddChildEvent((Event *)mpie);
       m_pTarget->AcceptEvent(mie);
       sentData = true;
   }

   if (m_track < disc.disc_current_track || (disc.disc_mode != CDAUDIO_PLAYING
       && disc.disc_mode != CDAUDIO_PAUSED))
   {
       trackDone = true;
   }
    
   int iTotalTime = disc.disc_track_time.minutes * 60 + 
                    disc.disc_track_time.seconds; 

   int hours = iTotalTime / 3600;
   int minutes = (iTotalTime / 60) % 60;
   int seconds = iTotalTime % 60;

   if (hours < 0 ||
       minutes < 0 || minutes > 59 || 
       seconds < 0 || seconds > 59)
      return;

   MediaTimeInfoEvent *pmtpi;

   pmtpi = new MediaTimeInfoEvent(hours, minutes, seconds, 0, 
                                  iTotalTime, 0);
   m_pTarget->AcceptEvent(pmtpi);
}

void CDPMO::StartWorkerThread(void *pVoidBuffer)
{
   ((CDPMO*)pVoidBuffer)->WorkerThread();
}

void CDPMO::WorkerThread(void)
{
#ifdef __linux__
   struct sched_param sParam;

   sParam.sched_priority = sched_get_priority_max(SCHED_OTHER);
   pthread_setschedparam(pthread_self(), SCHED_OTHER, &sParam);
#endif

   for(; !m_bExit;)
   {
      // Loop until we get an Init event from the LMC
      if (!m_properlyInitialized)
      {
          WasteTime();
      }
      else {
          HandleTimeInfoEvent(NULL);
   
          if (trackDone)
          {
              m_pTarget->AcceptEvent(new Event(INFO_DoneOutputting));

              return;
          }
          WasteTime();
      }
   }
}
