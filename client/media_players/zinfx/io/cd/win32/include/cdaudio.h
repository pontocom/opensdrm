/*____________________________________________________________________________
 
        FreeAmp - The Free MP3 Player
 
        Portions Copyright (C) 1998-1999 Tony Arcieri
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
 
        $Id: cdaudio.h,v 1.1 2004/10/13 16:16:06 linha2 Exp $
 ____________________________________________________________________________*/


#ifndef _CDAUDIO_H
#define _CDAUDIO_H

#include <string>
using namespace std;

#define LIBCDAUDIO_VERSION_MAJOR 0
#define LIBCDAUDIO_VERSION_MINOR 99
#define LIBCDAUDIO_VERSION_MICRO 4

#define LIBCDAUDIO_VERSION \
        ((LIBCDAUDIO_VERSION_MAJOR<<16)| \
         (LIBCDAUDIO_VERSION_MINOR<< 8)| \
         (LIBCDAUDIO_VERSION_MICRO))

/* Used with disc_info */
#define CDAUDIO_PLAYING				0
#define CDAUDIO_PAUSED				1
#define CDAUDIO_COMPLETED			2
#define CDAUDIO_NOSTATUS			3
#define CDAUDIO_INVALID				4
#define CDAUDIO_ERROR				5

#define CDAUDIO_TRACK_AUDIO 			0
#define CDAUDIO_TRACK_DATA 			1

#define MAX_TRACKS				100
#define MAX_SLOTS				100 /* For CD changers */

#define PLAY_START_TRACK                        0
#define PLAY_END_TRACK				1
#define PLAY_START_POSITION			2
#define PLAY_END_POSITION			4

extern long cdaudio_getversion(void);

/* Used for keeping track of times */
struct disc_timeval {
   int minutes;
   int seconds;
   int frames;
};

/* Brief disc information */
struct disc_status {
   int status_present;				/* Is disc present? */
   int status_mode;				/* Disc mode */
   struct disc_timeval status_disc_time;	/* Current disc time */
   struct disc_timeval status_track_time; 	/* Current track time */
   int status_current_track;			/* Current track */
};

/* Track specific information */
struct track_info {
   struct disc_timeval track_length;		/* Length of track */
   struct disc_timeval track_pos;		/* Position of track */
   int track_lba;				/* Logical Block Address */
   int track_type;				/* CDAUDIO_TRACK_AUDIO or CDAUDIO_TRACK_DATA */
};

/* Disc information such as current track, amount played, etc */
struct disc_info {
   int disc_present;				/* Is disc present? */
   int disc_mode;				/* Current disc mode */
   struct disc_timeval disc_track_time;		/* Current track time */
   struct disc_timeval disc_time;		/* Current disc time */
   struct disc_timeval disc_length;		/* Total disc length */
   int disc_current_track;			/* Current track */
   int disc_first_track;			/* First track on the disc */
   int disc_total_tracks;			/* Number of tracks on disc */
   struct track_info disc_track[MAX_TRACKS];	/* Track specific information */
};

/* Invisible volume structure */
struct __volume { 
   int left;
   int right;
};

/* Volume structure */
struct disc_volume {
   struct __volume vol_front;			/* Normal volume settings */
   struct __volume vol_back;			/* Surround sound volume settings */
};

/* Function prototopes */
int cd_frames_to_lba(int frames);
int cd_lba_to_frames(int lba);
int cd_msf_to_frames(struct disc_timeval time);
int cd_msf_to_lba(struct disc_timeval time);
void cd_frames_to_msf(struct disc_timeval *time, int frames);
void cd_lba_to_msf(struct disc_timeval *time, int frames);
int cd_init_device(string device_name);
int cd_finish(string cd_desc);
int cd_stat(string cd_desc, struct disc_info *disc, bool read_toc = true);
int cd_poll(string cd_desc, struct disc_status *status);
int cd_update(struct disc_info *disc, struct disc_status status);
int cd_play_track_pos(string cd_desc, int starttrack, int endtrack, int startpos);
int cd_play_track(string cd_desc, int starttrack, int endtrack);
int cd_stop(string cd_desc);
int cd_pause(string cd_desc);
int cd_resume(string cd_desc);
int cd_set_volume(string cd_desc, struct disc_volume vol);
int cd_get_volume(string cd_desc, struct disc_volume *vol);
long unsigned cddb_direct_discid(struct disc_info *disc);
long cddb_discid(string cd_desc);
int cdindex_direct_discid(struct disc_info *disc, char *discid, int len);
int cdindex_discid(string cd_desc, char *discid, int len);
#endif
