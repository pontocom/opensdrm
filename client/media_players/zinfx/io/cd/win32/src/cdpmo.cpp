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

#include "cdaudio.h"

#include "cdpmo.h"
#include "lmc.h"
#include "eventdata.h"
#include "facontext.h"
#include "log.h"

#include <windows.h>
#include <time.h>

#define DB printf("%s:%d\n", __FILE__, __LINE__);

static Mutex m_locker;

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
   m_cdDesc = "";
   cddbid = 0;
   sentData = false;
   trackDone = false;
   Int32PropValue *pProp;
   m_volume = NULL;

   m_hWnd = 0;

   if (IsError(m_pContext->props->GetProperty("MainWindow", 
             (PropValue **)&pProp)))
       return;        
   else
       m_hWnd = (HWND)pProp->GetInt32();

   m_volume = new Win32Volume( Win32Volume::eCDOut, m_hWnd );
}

CDPMO::~CDPMO()
{
   m_bExit = true;

   if (m_pBufferThread)
   {
      m_pBufferThread->Join();
	  cd_finish(m_thcdDesc);
      delete m_pBufferThread;
   }

   if (m_properlyInitialized)
       Reset(true);

   cd_finish(m_cdDesc);

   delete m_volume;
}

void CDPMO::SetVolume(int32 left, int32 right)
{
    if (m_cdDesc != "")
    {
        m_volume->SetVolume( left, right );
    }
}

void CDPMO::GetVolume(int &left, int &right)
{
    if (m_cdDesc != "")
    {
        m_volume->GetVolume( left, right );
    }
} 

Error CDPMO::SetTo(const char *url)
{
   OutputInfo blah;

   if (IsError(Init(&blah))) {
	   ReportError("Initialization of the CD-ROM failed.");
       return kError_CDInitFailed;
   }

   char *tracknumber = strrchr(url, '\\');

   if (!tracknumber)
       tracknumber = (char *)url;
   
   while (!isdigit(tracknumber[0])) 
       tracknumber++;

   m_track = atoi(tracknumber);

   if (m_track < 1 || m_track > MAX_TRACKS) {
	   char errormsg[256];
	   sprintf(errormsg, "Attempting to play an invalid track %d", m_track);
	   ReportError(errormsg);

       return kError_InvalidTrack;
   }

   if (!m_pBufferThread)
   {
      m_pBufferThread = Thread::CreateThread();
      assert(m_pBufferThread);
      m_pBufferThread->Create(CDPMO::StartWorkerThread, this);
   }
 
   m_locker.Acquire();
   cd_play_track(m_cdDesc, m_track, m_track);
   m_locker.Release();

   m_properlyInitialized = true;
   return kError_NoErr;
}

Error CDPMO::Init(OutputInfo *info)
{
   Error retvalue = (Error)pmoError_DeviceOpenFailed;
   char device[256];
   uint32 len = 256;
   int initreturn;
   uint32 tracks = 0;

   m_locker.Acquire();

   memset(&dinfo, 0, sizeof(struct disc_info));

   cddbid = 0;
   cdindexid = "";

   if (IsError(m_pContext->prefs->GetPrefString(kCDDevicePathPref, device, &len))) {
       CDInfoEvent *cie = new CDInfoEvent(tracks, cddbid, (char *)cdindexid.c_str());
       m_pTarget->AcceptEvent(cie);
	   m_locker.Release();
       return retvalue;
   }

   m_cdDesc = device;
   if (m_cdDesc == "") 
	   m_cdDesc = "cdaudio";
   
   sprintf(device, "%d_%d", GetCurrentThreadId(), time(NULL));
   m_cdDesc += device;

   if ((initreturn = cd_init_device(m_cdDesc)) != 0) {
       if (info) {
		   char errormsg[256];
           mciGetErrorString(initreturn, errormsg, 256);
           ReportError(errormsg);
	   }
       CDInfoEvent *cie = new CDInfoEvent(tracks, cddbid, (char *)cdindexid.c_str());
       m_pTarget->AcceptEvent(cie);
	   m_locker.Release();
       return retvalue;
   }

   retvalue = kError_NoDiscInDrive;

   if (cd_stat(m_cdDesc, &dinfo, false) < 0) {
       if (info)
           ReportError("There is no disc in the CD-ROM drive.");
       CDInfoEvent *cie = new CDInfoEvent(tracks, cddbid, (char *)cdindexid.c_str());
       m_pTarget->AcceptEvent(cie);
	   m_locker.Release();
       return retvalue;
   }
   
   if (!dinfo.disc_present) {
       if (info)
           ReportError("There is no disc in the CD-ROM drive.");
       CDInfoEvent *cie = new CDInfoEvent(tracks, cddbid, (char *)cdindexid.c_str());
       m_pTarget->AcceptEvent(cie);
	   m_locker.Release();
       return retvalue;
   }

   retvalue = kError_NoErr;

   memset(&dinfo, 0, sizeof(struct disc_info));

   cd_stat(m_cdDesc, &dinfo);
   m_locker.Release();

   tracks = dinfo.disc_total_tracks;
   cddbid = cddb_direct_discid(&dinfo);

   char *cdindex = new char[256];
   cdindex_direct_discid(&dinfo, cdindex, 256);
   cdindexid = cdindex;
   delete [] cdindex;

   CDInfoEvent *cie = new CDInfoEvent(tracks, cddbid, (char *)cdindexid.c_str());
   m_pTarget->AcceptEvent(cie);

   return retvalue;
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

Error CDPMO::Reset(bool user_stop)
{
   if (m_cdDesc == "")
      return kError_NoErr;

   m_locker.Acquire();
   cd_stop(m_cdDesc);
   m_locker.Release();

   return kError_NoErr;
}

void CDPMO::Pause(void)
{
    if (m_cdDesc == "")
        return;

	m_locker.Acquire();
    cd_pause(m_cdDesc);
	m_locker.Release();
}

void CDPMO::Resume(void)
{
    if (m_cdDesc == "")
        return;

	m_locker.Acquire();
    cd_resume(m_cdDesc);
	m_locker.Release();
}

Error CDPMO::ChangePosition(int32 newposition)
{
    if (m_cdDesc == "" || m_track < 0)
        return kError_NoErr;

	m_locker.Acquire();
    cd_play_track_pos(m_cdDesc, m_track, m_track, newposition);
    m_locker.Release();

    return kError_NoErr;
}

void CDPMO::HandleTimeInfoEvent(PMOTimeInfoEvent *pEvent)
{
   if (m_thcdDesc == "" || m_track < 0)
      return;

   struct disc_info disc;
   struct disc_timeval val;

   m_locker.Acquire();
   if (cd_stat(m_thcdDesc, &disc, false) < 0) {
       trackDone = true;
	   m_locker.Release();
       return;
   }
 
   if (!disc.disc_present) {
       trackDone = true;
	   m_locker.Release();
       return;
   }

   cd_stat(m_thcdDesc, &disc);
   m_locker.Release();

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

   if (m_track != disc.disc_current_track) {
	   cd_stop(m_thcdDesc);
       trackDone = true;
	   return;
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
   char device[256];
   uint32 len = 256;

   m_locker.Acquire();

   if (IsError(m_pContext->prefs->GetPrefString(kCDDevicePathPref, device, &len))) {
	   m_locker.Release();
       return;
   }

   m_thcdDesc = device;
   if (m_thcdDesc == "") 
	   m_thcdDesc = "cdaudio";
   
   sprintf(device, "%d_%d", GetCurrentThreadId(), time(NULL));
   m_thcdDesc += device; 

   if (cd_init_device(m_thcdDesc) != 0) {
 	   m_locker.Release();
       return;
   }

   m_locker.Release();
   for(; !m_bExit;)
   {
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
          usleep(10000);
      }
   }

   m_locker.Acquire();
   cd_finish(m_thcdDesc);
   m_locker.Release();
}
