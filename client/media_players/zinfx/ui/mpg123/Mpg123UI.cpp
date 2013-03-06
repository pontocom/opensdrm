/*____________________________________________________________________________
    
    FreeAmp - The Free MP3 Player

    Portions Copyright (C) 1998-2000 EMusic.com

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
    
    $Id: Mpg123UI.cpp,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <signal.h>

#include "Mpg123UI.h"
#include "event.h"
#include "eventdata.h"
#include "playlist.h"

void      mysigterm(int);
void      mysigint(int);

extern    "C"
{
   UserInterface *Initialize(FAContext * context)
   {
      return new Mpg123UI(context);
   }
}

EventQueue *Mpg123UI::m_playerEQ = NULL;

Mpg123UI::Mpg123UI(FAContext * context)
{
   m_context = context;
   m_mediaInfo_set = false;
   m_mediaInfo = NULL;
   m_mpegInfo_set = false;

   verboseMode = false;
   totalFrames = 0;
   lastSeconds = -1;
   fileName[0] = '\0';

   m_plm = NULL;

   m_displaystuff = true;
}

Error     Mpg123UI::
Init(int32 startup_level)
{
   if ((m_startupType = startup_level) == PRIMARY_UI)
   {

      m_plm = m_context->plm;
      m_playerEQ = m_context->target;
      m_propManager = m_context->props;

      m_argc = m_context->argc;
      m_argv = m_context->argv;

      ProcessArgs();
      // cout << endl << "mpg123 0.59k command line compatability mode" << endl 
      // << endl;
      cerr << "High Performance MPEG 1.0/2.0/2.5 Audio Player for Layer 1, 2 and 3" << endl;
      cerr << "Version 2.1.0 (2000/Feb/04).  " << endl;
      cerr << "Copyrights EMusic, XingTech. See 'README' for more!" << endl;
      cerr << "This software is distributed under the GNU GPL." << endl;
      signal(SIGTERM, mysigterm);
      signal(SIGINT, mysigint);

   }
   return kError_NoErr;
}

void 
mysigterm(int f)
{
   Event    *e = new Event(CMD_Stop);

 Mpg123UI::m_playerEQ->AcceptEvent(e);
   e = new Event(CMD_QuitPlayer);
 Mpg123UI::m_playerEQ->AcceptEvent(e);
}

void 
mysigint(int f)
{
   mysigterm(f);
}
Mpg123UI::~Mpg123UI()
{
   // cout << "deleting Mpg123UI" << endl;
   // NOTE: we don't call cleanup on this EventQueueRef because it is pointing
   // to the player here...
   if (m_playerEQ)
   {
      delete    m_playerEQ;

      m_playerEQ = NULL;
      delete    m_mediaInfo;
   }
}

Error     Mpg123UI::
AcceptEvent(Event * e)
{
   if (e)
   {
      //cerr << "Mpg123COO: processing event " << e->Type() << endl;
      switch (e->Type())
      {
      case INFO_ErrorMessage:
         {
           ErrorMessageEvent *eme = (ErrorMessageEvent *)e;
           string ErrMessage(eme->GetErrorMessage());
 
           cout << ErrMessage << endl;
           break;
         }
      case INFO_PlaylistDonePlay:
         {
            Event    *ev = new Event(CMD_QuitPlayer);

            m_playerEQ->AcceptEvent(ev);
            break;
         }
      case INFO_MPEGInfo:
         {
            MpegInfoEvent *mie = (MpegInfoEvent *) e;

            if (mie)
            {
               m_mpegInfo = *mie;
               m_mpegInfo_set = true;
               DisplayMPEGStuff();
            }
            totalFrames = m_mpegInfo.GetTotalFrames();
            break;
         }
      case INFO_VorbisInfo:
         {
            VorbisInfoEvent *info = (VorbisInfoEvent *) e;
            DisplayVorbisStuff(info);
            break;
         }  
      case INFO_MediaInfo:
         {
            MediaInfoEvent *pmvi = (MediaInfoEvent *) e;

            if (pmvi)
            {
               m_mediaInfo = new MediaInfoEvent(*pmvi);
               m_mediaInfo_set = true;
               DisplayMPEGStuff();
            }
            totalTime = m_mediaInfo->m_totalSeconds;
            break;
         }
      case INFO_MediaTimeInfo:
         {
            MediaTimeInfoEvent *pmtp = (MediaTimeInfoEvent *) e;

            if (verboseMode == true)
            {
               fprintf(stderr, "\rFrame# %5d [%5d], ", pmtp->m_frame, totalFrames - pmtp->m_frame);
               fprintf(stderr, "Time: %3.2f [%3.2f], ", pmtp->m_totalSeconds, totalTime - pmtp->m_totalSeconds);
            }
            lastSeconds = pmtp->m_totalSeconds;
            break;
         }
      case INFO_Stopped:
         {
            if (lastSeconds <= 0 || fileName[0] == '\0')
               break;

            int       m = (int) lastSeconds / 60;
            int       s = (int) lastSeconds % 60;

            m_displaystuff = true;
            fprintf(stderr, "\n[%d:%02d] Decoding of %s finished.\n", m, s, fileName);
            break;
         }
      case CMD_Cleanup:
         {
            Event    *e = new Event(INFO_ReadyToDieUI);

            // cout << "I'm gonna ACTUALLY kill myself" << endl;
            m_playerEQ->AcceptEvent(e);
            break;
         }
      default:
         break;
      }
   }
//cerr << "Done cmdlinecoo: accpet event" << endl;
   return kError_NoErr;
}

void      Mpg123UI::
DisplayMPEGStuff()
{
   if (!m_mediaInfo_set)
      return;

   if (!m_mpegInfo_set)
      return;

   if (!m_displaystuff)
       return;

   m_displaystuff = false;

   char     *path = m_mediaInfo->m_filename;
   char     *pLastSlash = strrchr(path, '/');
   char     *dir = NULL;
   char     *fname = NULL;

   m_mpegInfo_set = false;
   if (pLastSlash)
   {
      *pLastSlash = '\0';
      fname = pLastSlash + 1;
      dir = path;
   }
   else
   {
      fname = path;
   }

   strncpy(fileName, fname, 511);
   fileName[511] = '\0';
   
/*    if (m_id3Tag.m_containsInfo) {
   fprintf(stderr,"Title  : %30s  Artist: %s\n",m_id3Tag.m_songName,m_id3Tag.m_artist);
   fprintf(stderr,"Album  : %30s  Year: %4s, Genre: %d\n",m_id3Tag.m_album,m_id3Tag.m_year,(int)m_id3Tag.m_genre);
   fprintf(stderr,"Comment: %30s \n",m_id3Tag.m_comment);
   } */
   cerr << endl;

   static const char *mpeg_string[4] =
   {"1.0", "1.0", "2.0", "2.5"};
   static const char *mpeg_layer[4] =
   {"Unknown", "I", "II", "III"};
   static const char *mpeg_stereo[4] =
   {"Stereo", "Joint-Stereo", "Dual-Channel", "Single-Channel"};
   static const char *mpeg_short_stereo[4] =
   {"stereo", "joint-stereo", "dual-channel", "mono"};

   if (verboseMode == false)
   {
      // NORMAL MODE
      if (dir)
      {
         cerr << "Directory: " << dir << "/" << endl;
      }
      cerr << "Playing MPEG stream from " << fileName << " ..." << endl;
      cerr << "MPEG " << mpeg_string[m_mpegInfo.GetMpegVersion()] << " layer " << mpeg_layer[m_mpegInfo.GetLayer()] << ", " << m_mpegInfo.GetBitRate() / 1000 << " KBit/s" << ", " << m_mpegInfo.GetSampleRate() << " Hz" << " " << mpeg_short_stereo[m_mpegInfo.GetStereo()] << endl;
   }
   else
   {
      // VERBOSE MODE
      if (dir)
      {
         cerr << "Directory: " << dir << "/" << endl;
      }
      cerr << "Playing MPEG stream from " << fileName << " ..." << endl;
      cerr << "MPEG " << mpeg_string[m_mpegInfo.GetMpegVersion()];
      cerr << ", Layer: " << mpeg_layer[m_mpegInfo.GetLayer()];
      cerr << ", Freq: " << m_mpegInfo.GetSampleRate();
      cerr << ", mode: " << mpeg_stereo[m_mpegInfo.GetStereo()] << ", modext: " << m_mpegInfo.GetModeExt() << ", BPF : " << m_mpegInfo.GetBytesPerFrame() << endl;
      cerr << "Channels: " << m_mpegInfo.GetChannels() << ", copyright: " << (m_mpegInfo.GetCopyright()? "Yes" : "No") << ", original: ";
      cerr << (m_mpegInfo.GetOriginal()? "Yes" : "No") << ", CRC: " << (m_mpegInfo.GetCRC()? "Yes" : "No") << ", emphasis: " << m_mpegInfo.GetEmphasis() << "." << endl;
      cerr << "Bitrate: " << m_mpegInfo.GetBitRate() / 1000 << " KBits/s, Extension value: 0" << endl;
      cerr << "Audio: 1:1 conversion, rate: " << m_mpegInfo.GetSampleRate() << ", encoding: signed 16 bit, channels: " << m_mpegInfo.GetChannels() << endl;
   }
}

void      Mpg123UI::
DisplayVorbisStuff(VorbisInfoEvent *info)
{
   if (!m_mediaInfo_set)
      return;

   if (!m_displaystuff)
       return;

   m_displaystuff = false;

   char     *path = m_mediaInfo->m_filename;
   char     *pLastSlash = strrchr(path, '/');
   char     *dir = NULL;
   char     *fname = NULL;

   m_mpegInfo_set = false;
   if (pLastSlash)
   {
      *pLastSlash = '\0';
      fname = pLastSlash + 1;
      dir = path;
   }
   else
   {
      fname = path;
   }

   strncpy(fileName, fname, 511);
   fileName[511] = '\0';
   
   if (verboseMode == false)
   {
      cerr << "\n";
      // NORMAL MODE
      if (dir)
      {
         cerr << "Directory: " << dir << "/" << endl;
      }
      cerr << "Playing Vorbis stream from " << fileName << " ..." << endl;
      fprintf(stderr, "%ldkbps %ldkHz %s Vorbis\n\n",
               (long int)(info->GetBitRate() / 1000),
               (long int)(info->GetSampleRate() / 1000),
               info->GetChannels() ? "Stereo" : "Mono");  
   }
}

void      Mpg123UI::
ProcessArgs()
{
   char     *pc = NULL;
   int       addedStuff = 0;

   for (int i = 1; i < m_argc; i++)
   {
      pc = m_argv[i];
      if (pc[0] == '-')
      {
         switch (pc[1])
         {
         case 't':
            cout << "test mode" << endl;
            break;
         case 'v':
            verboseMode = true;
            // cout << "verbose mode" << endl;
            break;
         case 'k':
            i++;
            pc = m_argv[i];
            skipFirst = atoi(pc);
            // cout << "skiping first " << skipFirst << " frames" << endl;
            break;
         case 'c':
            cout << "check range violations" << endl;
            break;
         case 'b':
            i++;
            pc = m_argv[i];
            cout << "output buffer: " << pc << "Kbytes" << endl;
            break;
         case 'r':
            i++;
            pc = m_argv[i];
            cout << "override samplerate: " << pc << endl;
            break;
         case 'o':
            {
               switch (pc[2])
               {
               case 's':
                  cout << "output to builtin speaker" << endl;
                  break;
               case 'l':
                  cout << "output to line connector" << endl;
                  break;
               case 'h':
                  cout << "output to headphones" << endl;
                  break;
               }
            }
         case '2':
            cout << "downsample 1:2 (22 kHz)" << endl;
            break;
         case 'd':
            i++;
            pc = m_argv[i];
            cout << "play only every nth frame" << endl;
            break;
         case '0':
            cout << "decode channel 0 (left) only" << endl;
            break;
         case 'm':
            cout << "mix both channels (mono)" << endl;
            break;
         case '@':
            i++;
            pc = m_argv[i];
            cout << "read filenames/URLs from " << pc << endl;
            break;
         case 'q':
            cout << "quiet" << endl;
            break;
         case 's':
            cout << "write to standard out" << endl;
            break;
         case 'n':
            i++;
            pc = m_argv[i];
            cout << "decode only " << pc << " frames" << endl;
            break;
         case 'y':
            cout << "disable resync on errors" << endl;
            break;
         case 'f':
            i++;
            pc = m_argv[i];
            cout << "change scalefactor to " << pc << endl;
            break;
         case 'g':
            i++;
            pc = m_argv[i];
            cout << "set audio output hardware gain to " << pc << endl;
            break;
         case 'a':
            i++;
            pc = m_argv[i];
            cout << "set audio device " << pc << endl;
            break;
         case '4':
            cout << "downsample 1:4 (11 kHz)" << endl;
            break;
         case 'h':
            i++;
            pc = m_argv[i];
            cout << " play every frame " << pc << " times" << endl;
            break;
         case '1':
            cout << "decode channel 1 only (right) " << endl;
            break;
         case 'p':
            i++;
            pc = m_argv[i];
            cout << "use HTTP proxy " << pc << endl;
            break;
         case 'z':
            m_plm->SetShuffleMode(true);
            // cout << "shuffle play (with wildcards) " << endl;
            break;
         case 'Z':
            // random play has been temporarily disabled.
            // m_plm->SetShuffle(PlaylistManager::ORDER_RANDOM);
            // cout << "Random Play" << endl;
            break;
         case 'u':
         case 'U':
            i++;
            // the -ui to select the UI
            break;
         default:
            // cout << "Unknown option:  " << pc << endl;
            break;
         }
      }
      else
      {
         // ut << "Adding: " <<m_argv[i] << endl;
         //m_plm->AddItem(m_argv[i], 0);
         addedStuff++;
      }
   }

   if (addedStuff)
   {

      m_plm->SetCurrentIndex(0);
      //RAK: This should not be necessary as the player auto plays
      //Event    *e = new Event(CMD_Play);
      //m_playerEQ->AcceptEvent(e);
   }
   else
   {
      // cout << "Gonna try and quit..." << endl;
      m_playerEQ->AcceptEvent(new Event(CMD_QuitPlayer));
   }
}
