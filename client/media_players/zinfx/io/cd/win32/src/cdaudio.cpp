/*____________________________________________________________________________
 
        FreeAmp - The Free MP3 Player
 
        Portions Copyright (C) 1999-2000 EMusic.com
        Portions Copyright (C) 1998-1999 Tony Arcieri
        Portions Copyright (C) 1999 Quinton Dolan
  
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
 
        $Id: cdaudio.cpp,v 1.1 2004/10/13 16:16:06 linha2 Exp $
____________________________________________________________________________*/


/* hacked from libcdaudio */

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include "mmsystem.h"

 #include "cdaudio.h"

/* Return the version of libcdaudio */
long cdaudio_getversion(void)
{
   return LIBCDAUDIO_VERSION;
}

/* Initialize the CD-ROM for playing audio CDs */
int
cd_init_device(string device_name)
{
   char mciCommand[128];
   char mciReturn[128];

   sprintf(mciCommand, "sysinfo cdaudio quantity wait");

   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);
	
   if (atoi(mciReturn) <= 0)
	   return -1;

   sprintf(mciCommand, "open cdaudio shareable alias %s wait", device_name.c_str());
   int ret = mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);

   if (ret != 0) 
   {
//	   mciGetErrorString(ret, mciCommand, 128);
//	   sprintf(mciReturn, "error opening %s", device_name.c_str());
//	   MessageBox(NULL, mciReturn, mciCommand, MB_OK|MB_SETFOREGROUND);
	   return -1;
   }

   return 0;
}

/* Close a device handle and free its resources. */
int
cd_finish(string cd_desc)
{
   char mciCommand[128];
   char mciReturn[128];

   sprintf(mciCommand, "close %s wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);

   return 0;
}

/* Update a CD status structure... because operating system interfaces vary
   so does this function. */
int
cd_stat(string cd_desc, struct disc_info *disc, bool read_toc)
{
   /* Since every platform does this a little bit differently this gets pretty
      complicated... */

   struct disc_status status;
   int readtracks, pos;
   
   if(cd_poll(cd_desc, &status) < 0)
     return -1;
 
   if(!status.status_present) {
      disc->disc_present = 0;
      return 0;
   }
   
   if(!read_toc) {
      cd_update(disc, status);
      return 0;
   }

   char mciCommand[128];
   char mciReturn[128];

   sprintf(mciCommand, "status %s number of tracks wait", cd_desc.c_str());
   int ret = mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);

   char buf[256];
   mciGetErrorString(ret, buf, sizeof(buf));

   int numTracks = atoi(mciReturn);

   disc->disc_first_track = 1;
   disc->disc_total_tracks = numTracks;

   int lastpos = 0;

   for(readtracks = 0; readtracks < disc->disc_total_tracks; readtracks++) {
      sprintf(mciCommand, "set %s time format msf wait", cd_desc.c_str());
      mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);
	  sprintf(mciCommand, "status %s position track %d wait", cd_desc.c_str(), readtracks + 1);
	  mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);

      disc->disc_track[readtracks].track_pos.minutes = atoi(mciReturn);
      disc->disc_track[readtracks].track_pos.seconds = atoi(mciReturn + 3);
      disc->disc_track[readtracks].track_pos.frames = atoi(mciReturn + 6);

	  pos = cd_msf_to_frames(disc->disc_track[readtracks].track_pos);
	  if (pos < lastpos)
		  MessageBox(NULL, mciReturn, "Track pos < lastpos", MB_OK);
	  lastpos = pos;

	  sprintf(mciCommand, "status %s cdaudio type track %d wait", cd_desc.c_str(), readtracks + 1);
	  mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);
      disc->disc_track[readtracks].track_type = (!strcmp("audio", mciReturn)) ? CDAUDIO_TRACK_AUDIO : CDAUDIO_TRACK_DATA;
      disc->disc_track[readtracks].track_lba = cd_msf_to_lba(disc->disc_track[readtracks].track_pos);

      sprintf(mciCommand, "set %s time format msf wait", cd_desc.c_str());
      mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);
      sprintf(mciCommand, "status %s length track %d wait", cd_desc.c_str(), readtracks + 1);
      mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);

      disc->disc_track[readtracks].track_length.minutes = atoi(mciReturn);
      disc->disc_track[readtracks].track_length.seconds = atoi(mciReturn + 3);
      disc->disc_track[readtracks].track_length.frames = atoi(mciReturn + 6);
   }

   pos = cd_msf_to_frames(disc->disc_track[disc->disc_total_tracks-1].track_pos) +
         cd_msf_to_frames(disc->disc_track[disc->disc_total_tracks-1].track_length);
   cd_frames_to_msf(&disc->disc_track[disc->disc_total_tracks].track_pos, pos);
   
   disc->disc_length.minutes = disc->disc_track[disc->disc_total_tracks].track_pos.minutes;
   disc->disc_length.seconds = disc->disc_track[disc->disc_total_tracks].track_pos.seconds;
   disc->disc_length.frames = disc->disc_track[disc->disc_total_tracks].track_pos.frames;
   
   cd_update(disc, status);
 
   return 0;
}

int
cd_poll(string cd_desc, struct disc_status *status)
{
   
   char mciCommand[128];
   char mciReturn[128];

   sprintf(mciCommand, "status %s media present wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);

   if (!strcmp("true", mciReturn) && !strcmp("TRUE", mciReturn)) {
	   status->status_present = 0;
	   return 0;
   }

   status->status_present = 1;

   sprintf(mciCommand, "set %s time format msf wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);
   sprintf(mciCommand, "status %s position wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);

   status->status_disc_time.minutes = atoi(mciReturn);
   status->status_disc_time.seconds = atoi(mciReturn + 3);
   status->status_disc_time.frames = atoi(mciReturn + 6);

   sprintf(mciCommand, "set %s time format tmsf wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);
   sprintf(mciCommand, "status %s position wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);

   char *colon = strchr(mciReturn, ':');

   if (colon && colon++) {
      status->status_track_time.minutes = atoi(colon);
      status->status_track_time.seconds = atoi(colon + 3);
      status->status_track_time.frames = atoi(colon + 6);
      status->status_current_track = atoi(mciReturn);
   }
   else {
      status->status_track_time.minutes = 0;
	  status->status_track_time.seconds = 0;
	  status->status_track_time.frames = 0;
	  status->status_current_track = 0;
   }

   
   sprintf(mciCommand, "set %s time format msf wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);
   sprintf(mciCommand, "status %s mode wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);

   if (!strcmp(mciReturn, "playing"))
      status->status_mode = CDAUDIO_PLAYING;
   else if (!strcmp(mciReturn, "paused"))
	  status->status_mode = CDAUDIO_PAUSED;
   else if (!strcmp(mciReturn, "stopped"))
      status->status_mode = CDAUDIO_COMPLETED;
   else
      status->status_mode = CDAUDIO_NOSTATUS;

   return 0;
}

/* Play starttrack at position pos to endtrack */
int
cd_play_track_pos(string cd_desc, int starttrack, int endtrack, int startpos)
{
   struct disc_timeval time;
  
   time.minutes = startpos / 60;
   time.seconds = startpos % 60;
   time.frames = 0;
  
   char mciCommand[128], mciPlay[128];
   char mciReturn[128];

   sprintf(mciCommand, "status %s number of tracks wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);

   int numtracks = atoi(mciReturn);

   if (endtrack < numtracks)
       sprintf(mciPlay, "play %s from %d:%d:%d to %d", cd_desc.c_str(), starttrack, time.minutes, 
	           time.seconds, endtrack + 1);
   else 
	   sprintf(mciPlay, "play %s from %d:%d:%d", cd_desc.c_str(), starttrack, time.minutes, 
	           time.seconds);
   sprintf(mciCommand, "set %s time format tmsf wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);
   mciSendString(mciPlay, mciReturn, sizeof(mciReturn), NULL);
   sprintf(mciCommand, "set %s time format msf wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);

   return 0;
}

/* Play starttrack to endtrack */
int
cd_play_track(string cd_desc, int starttrack, int endtrack)
{
   char mciCommand[128], mciPlay[128];
   char mciReturn[128];

   sprintf(mciCommand, "status %s number of tracks wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);

   int numtracks = atoi(mciReturn);

   if (endtrack < numtracks)
       sprintf(mciPlay, "play %s from %d:0 to %d:0", cd_desc.c_str(), starttrack, endtrack + 1);
   else
	   sprintf(mciPlay, "play %s from %d", cd_desc.c_str(), starttrack);
   sprintf(mciCommand, "set %s time format tmsf wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);
   mciSendString(mciPlay, mciReturn, sizeof(mciReturn), NULL);
   sprintf(mciCommand, "set %s time format msf wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);

   return 0;
}

/* Stop the CD, if it is playing */
int
cd_stop(string cd_desc)
{
   char mciCommand[128];
   char mciReturn[128];

   sprintf(mciCommand, "stop %s wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);
   
   return 0;
}

/* Pause the CD */
int
cd_pause(string cd_desc)
{
   char mciCommand[128];
   char mciReturn[128];

   sprintf(mciCommand, "pause %s wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);
   
   return 0;
}

/* Resume playing */
int
cd_resume(string cd_desc)
{ 
   char mciCommand[128];
   char mciReturn[128];
   
   sprintf(mciCommand, "resume %s wait", cd_desc.c_str());
   mciSendString(mciCommand, mciReturn, sizeof(mciReturn), NULL);

   return 0;
}

/* Return the current volume setting */
int
cd_get_volume(string cd_desc, struct disc_volume *vol)
{
   return 0;
}

/* Set the volume */
int
cd_set_volume(string cd_desc, struct disc_volume vol)
{
   return 0;
}

/*
Because all these functions are solely mathematical and/or only make callbacks
to previously existing functions they can be used for any platform.
 */

/* Convert frames to a logical block address */
int
cd_frames_to_lba(int frames)
{
   if(frames >= 150)
     return frames - 150;
   
   return 0;
}

/* Convert a logical block address to frames */
int
cd_lba_to_frames(int lba)
{
   return lba + 150;
}

/* Convert disc_timeval to frames */
int
cd_msf_to_frames(struct disc_timeval time)
{
   return time.minutes * 4500 + time.seconds * 75 + time.frames;
}

/* Convert disc_timeval to a logical block address */
int
cd_msf_to_lba(struct disc_timeval time)
{
   if(cd_msf_to_frames(time) > 150)
     return cd_msf_to_frames(time) - 150;
   
   return 0;
}

/* Convert frames to disc_timeval */
void
cd_frames_to_msf(struct disc_timeval *time, int frames)
{
   time->minutes = frames / 4500;
   time->seconds = (frames % 4500) / 75;
   time->frames = frames % 75;
}

/* Convert a logical block address to disc_timeval */
void
cd_lba_to_msf(struct disc_timeval *time, int lba)
{
   cd_frames_to_msf(time, lba + 150);
}

/* Update information in a disc_info structure using a disc_status structure */
int
cd_update(struct disc_info *disc, struct disc_status status)
{
   if(!(disc->disc_present = status.status_present))
     return -1;
   
   disc->disc_mode = status.status_mode;
   memcpy(&disc->disc_time, &status.status_disc_time, sizeof(struct disc_timeval));
   memcpy(&disc->disc_track_time, &status.status_track_time, sizeof(struct disc_timeval));

   if (status.status_current_track != 0)
	   disc->disc_current_track = status.status_current_track;
   else {
       disc->disc_current_track = 0;
       while(disc->disc_current_track < disc->disc_total_tracks && 
             cd_msf_to_frames(disc->disc_time) >= 
			     cd_msf_to_frames(disc->disc_track[disc->disc_current_track].track_pos) )
           disc->disc_current_track++;
   }

   return 0;
}
