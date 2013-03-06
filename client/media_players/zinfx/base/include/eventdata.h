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
        
        $Id: eventdata.h,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_EVENTDATA_H_
#define INCLUDED_EVENTDATA_H_

#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
using namespace std;

#include "event.h"
#include "playlist.h"
#include "utility.h"

class     LogicalMediaConverter;
class     PhysicalMediaOutput;

class     UserMessageEvent:public Event
{
   private:
   char     *m_info;

   public:

   virtual ~ UserMessageEvent()
   {
      if (m_info)
      {
         delete [] m_info;
                   m_info = NULL;
      }
   }
   UserMessageEvent()
   {
      m_type = INFO_UserMessage;
      m_info = NULL;
   }
   UserMessageEvent(const char *info)
   {
      m_type = INFO_UserMessage;
      m_info = strdup_new(info);
   }
   const char *GetInfo()
   {
      return m_info;
   }
};

class StatusMessageEvent:public Event
{
   private:
   char     *m_info;

   public:
   virtual ~ StatusMessageEvent()
   {
      if (m_info)
      {
         delete [] m_info;
         m_info = NULL;
      }
   }
   StatusMessageEvent()
   {
      m_type = INFO_StatusMessage;
      m_info = NULL;
   }
   StatusMessageEvent(const char *info)
   {
      m_type = INFO_StatusMessage;
      m_info = strdup_new(info);
   }
   const char *GetStatusMessage()
   {
      return m_info;
   }
};

class ErrorMessageEvent:public Event
{
   private:
   char     *m_info;

   public:
   virtual ~ ErrorMessageEvent()
   {
      if (m_info)
      {
         delete [] m_info;
         m_info = NULL;
      }
   }
   ErrorMessageEvent()
   {
      m_type = INFO_ErrorMessage;
      m_info = NULL;
   }
   ErrorMessageEvent(const char *info)
   {
      m_type = INFO_ErrorMessage;
      m_info = strdup_new(info);
   }
   const char *GetErrorMessage()
   {
      return m_info;
   }
};


class     BrowserMessageEvent:public Event
{
   private:
   string m_info;

   public:
   virtual ~ BrowserMessageEvent() {}
   BrowserMessageEvent()
   {
      m_type = INFO_BrowserMessage;
      m_info = "";
   }
   BrowserMessageEvent(const string &info)
   {
      m_type = INFO_BrowserMessage;
      m_info = info;
   }
   const char *GetBrowserMessage()
   {
      return m_info.c_str();
   }
};

class     HeadlineMessageEvent:public Event
{
   private:
   char     *m_info, *m_url;

   public:
   virtual ~ HeadlineMessageEvent()
   {
      if (m_info)
      {
         delete [] m_info;
      }
      if (m_url)
      {
         delete [] m_url;
      }
   }
   HeadlineMessageEvent()
   {
      m_type = INFO_HeadlineText;
      m_info = NULL;
      m_url = NULL;
   }
   HeadlineMessageEvent(const char *info, const char *url)
   {
      m_type = INFO_HeadlineText;
      m_info = strdup_new(info);
      m_url = strdup_new(url);
   }
   const char *GetHeadlineMessage()
   {
      return m_info;
   }
   const char *GetHeadlineURL()
   {
      return m_url;
   }
};


class     MediaInfoEvent:public Event
{
   public:

   vector<Event*>*  m_childEvents;
   bool             m_filled;
   float            m_totalSeconds;
   int32            m_indexOfSong;
   int32            m_totalSongs;
   char             m_filename[512];
   uint32           m_plmID;
   bool             m_copy;

   virtual ~ MediaInfoEvent()
   {
      if (m_childEvents && (m_childEvents->size() != 0) && !m_copy)
      {
            vector<Event *>::iterator i = m_childEvents->begin();

            for (; i != m_childEvents->end(); i++) 
                delete *i;
            delete m_childEvents;
            m_childEvents = NULL;
      }
   }

   MediaInfoEvent()
   {
      m_type = INFO_MediaInfo;
      m_filled = false;
      m_copy = false;
      m_filename[0] = '\0';
      m_childEvents = new vector<Event *>;
   }

   MediaInfoEvent(MediaInfoEvent &other):Event(other)
   {
      m_copy = true;
      m_type = other.m_type;
      m_filled = other.m_filled;
      m_totalSeconds = other.m_totalSeconds;
      m_indexOfSong = other.m_indexOfSong;
      m_totalSongs = other.m_totalSongs;
      strcpy(m_filename, other.m_filename);

      m_childEvents = new vector <Event *>(other.m_childEvents->size());

      copy(other.m_childEvents->begin(), other.m_childEvents->end(),
           m_childEvents->begin());
   }

   MediaInfoEvent(const char *fn,
                  float ts)
   {
      m_copy = false;
      m_childEvents = new vector<Event *>;
      m_filled = true;
      m_type = INFO_MediaInfo;
      m_totalSeconds = ts;
      m_indexOfSong = 0;
      m_totalSongs = 0;

      if (fn)
      {
         strncpy(m_filename, fn, 511);
         m_filename[511] = '\0';
      }
      else
      {
         m_filename[0] = '\0';
      }
   }
   void      AddChildEvent(Event * pE)
   {
      if (pE)
      {
         m_childEvents->push_back(pE);
      }
   }
};

class     MediaTimeInfoEvent:public Event
{
   public:
   int32 m_hours, m_minutes, m_seconds, m_milliseconds, m_frame;
   float     m_totalSeconds;
             MediaTimeInfoEvent(int32 h, int32 m, int32 s, int32 ms, float ts, int32 f)
   {
      m_type = INFO_MediaTimeInfo;
      m_totalSeconds = ts;
      m_hours = h;
      m_minutes = m;
      m_seconds = s;
      m_milliseconds = ms;
      m_frame = f;
   }
   virtual ~ MediaTimeInfoEvent()
   {
   }
};

class     VolumeEvent:public Event
{
   private:
   int32 m_left, m_right;
   public:
   VolumeEvent(int32 t, int32 left = 0, int32 right = 0)
   {
      m_type = t;
      m_left = left;
      m_right = right;
   }
   int32     GetLeftVolume()
   {
      return m_left;
   }
   int32     GetRightVolume()
   {
      return m_right;
   }
   virtual ~ VolumeEvent()
   {
   }
};

class     ChangePositionEvent:public Event
{
   private:
   int32 m_frame;
   public:
   ChangePositionEvent(int32 p)
   {
      m_type = CMD_ChangePosition;
      m_frame = p;
   }
   int32     GetPosition()
   {
      return m_frame;
   }
   virtual ~ ChangePositionEvent()
   {
   }
};

class     MpegInfoEvent:public Event
{
   public:
   enum
   {
      STEREO = 1, JOINT_STEREO = 2, DUAL = 3, MONO = 4
   };
   enum
   {
      LAYER_1 = 1, LAYER_2, LAYER_3
   };
   enum
   {
      MPEG_1 = 1, MPEG_2 = 2, MPEG_25 = 3
   };
             private:
             int32 m_totalFrames;
   float     m_secondsPerFrame;
   int32     m_bytesPerFrame;
   int32     m_bitrate;
   int32     m_sampleRate;
   int32     m_layer;
   int32     m_mpeg;
   int32     m_channels;
   int32     m_original;
   int32     m_copyright;
   int32     m_emphasis;
   int32     m_stereo;
   int32     m_modeExt;
   int32     m_crc;
   int32     m_wav;
             public:
             MpegInfoEvent()
   {
      m_type = INFO_MPEGInfo;
   }
   MpegInfoEvent(int32 tf, float spf, int32 bpf, int32 br, int32 sr, int32 layer, int32 mpeg, int32 chans, int32 orig, int32 copy, int32 emph, int32 stereo, int32 mode_ext)
   {
      m_type = INFO_MPEGInfo;
      m_totalFrames = tf;
      m_secondsPerFrame = spf;
      m_bytesPerFrame = bpf;
      m_bitrate = br;
      m_sampleRate = sr;
      m_layer = layer;
      m_mpeg = mpeg;
      m_channels = chans;
      m_original = orig & 0x1;
      m_copyright = copy & 0x1;
      m_emphasis = emph;
      m_stereo = stereo;
      m_modeExt = mode_ext;
      m_wav = 0;
   }
   int32     GetTotalFrames()
   {
      return m_totalFrames;
   }
   float     GetSecondsPerFrame()
   {
      return m_secondsPerFrame;
   }
   int32     GetBytesPerFrame()
   {
      return m_bytesPerFrame;
   }
   int32     GetBitRate()
   {
      return m_bitrate;
   }
   int32     GetSampleRate()
   {
      return m_sampleRate;
   }
   int32     GetLayer()
   {
      return m_layer;
   }
   int32     GetMpegVersion()
   {
      return m_mpeg;
   }
   int32     GetChannels()
   {
      return m_channels;
   }
   int32     GetOriginal()
   {
      return m_original;
   }
   int32     GetCopyright()
   {
      return m_copyright;
   }
   int32     GetEmphasis()
   {
      return m_emphasis;
   }
   int32     GetStereo()
   {
      return m_stereo;
   }
   int32     GetModeExt()
   {
      return m_modeExt;
   }
   int32     GetCRC()
   {
      return m_crc;
   }
   int32     GetWAV()
   {
      return m_wav;
   }
   void      SetWAV(int wav)
   {
      m_wav = wav;
   }
   virtual ~MpegInfoEvent()
   {
   }

};

class VorbisInfoEvent:public Event
{
   public:
   VorbisInfoEvent(int32 bitrate, int32 channels, int32 rate, float spf)
   {
      m_type = INFO_VorbisInfo;
      m_bitrate = bitrate;
      m_channels = channels;
      m_rate = rate; 
      m_secondsPerFrame = spf;
   }
   int32     GetBitRate()
   {
      return m_bitrate;
   };
   int32     GetSampleRate()
   {
      return m_rate;
   };
   int32     GetChannels()
   {
      return m_channels;
   };
   float     GetSecondsPerFrame()
   {
      return m_secondsPerFrame;
   }
   private:
   int32     m_bitrate;
   int32     m_rate;
   int32     m_channels;
   float     m_secondsPerFrame;
};

class     SetEqualizerDataEvent:public Event
{
   private:
      float    *m_eq;
      bool      m_enable;
      bool      m_IsEQData;
      float     m_preamp;

   public:

   SetEqualizerDataEvent(bool enable)
   {
      m_type = CMD_SetEQData;
      m_enable = enable;
      m_IsEQData = false;
   }
   SetEqualizerDataEvent(float *eq, float preamp)
   {
      m_type = CMD_SetEQData;
      m_eq = eq;
      m_IsEQData = true;
      m_preamp = preamp;
   }
   float    *GetEQData()
   {
      return m_eq;
   }
   bool      IsEQData()
   {
      return m_IsEQData;
   }
   float     GetPreamp()
   {
      return m_preamp;
   }
   bool      GetEnableState()
   {
      return m_enable;
   }
   virtual ~ SetEqualizerDataEvent()
   {
   }
};

#define _VISUAL_ENABLE_
#ifdef  _VISUAL_ENABLE_
class     SendVisBufEvent:public Event
{
   private:
   int32 m_length;
   unsigned char *m_buf;
             public:
             SendVisBufEvent(int32 & wrote, void *pBuffer, int32 length)
   {
      m_type = CMD_SendVisBuf;
      m_buf = new unsigned char[length];
                m_length = length;
                memcpy(m_buf, pBuffer, length);
                wrote = length;
   }
   unsigned char *GetBuf()
   {
      return m_buf;
   }
   int32     GetLength()
   {
      return m_length;
   }
   virtual ~ SendVisBufEvent()
   {
      delete    m_buf;
   }
};
#endif // _VISUAL_ENABLE_
#undef  _VISUAL_ENABLE_

class     StreamBufferEvent:public Event
{
   private:
   bool      m_bBufferingUp;
   int32     m_iInputPercent, m_iOutputPercent;
   public:

   StreamBufferEvent(bool bBufferingUp, int32 iInputPercent, int iOutputPercent)
   {
      m_type = INFO_BufferStatus;
      m_bBufferingUp = bBufferingUp;
      m_iInputPercent = iInputPercent;
      m_iOutputPercent = iOutputPercent;
   };

   virtual ~ StreamBufferEvent()
   {
   };

   bool      IsBufferingUp()
   {
      return m_bBufferingUp;
   };
   int32     GetInputPercent()
   {
      return m_iInputPercent;
   };
   int32     GetOutputPercent()
   {
      return m_iOutputPercent;
   };
};

class     StreamInfoEvent:public Event
{
   

public:

   char     *m_streamURL;
   char     *m_streamTitle;

             StreamInfoEvent(char *szTitle, char *szURL)
   {
      m_type = INFO_StreamInfo;
      if (szURL)
          m_streamURL = strdup_new(szURL);
      else
          m_streamURL = strdup_new("");
      if (szTitle)
          m_streamTitle = strdup_new(szTitle);
      else
          m_streamTitle = strdup_new("");
   };

   virtual ~ StreamInfoEvent()
   {
      delete [] m_streamURL;
      delete [] m_streamTitle;
   };

   void      GetURL(char *szUrl, int iSize)
   {
      strncpy(szUrl, m_streamURL, iSize - 1);
      szUrl[iSize - 1] = 0;
   }
   void      GetTitle(char *szTitle, int iSize)
   {
      strncpy(szTitle, m_streamTitle, iSize - 1);
      szTitle[iSize - 1] = 0;
   }
};

class CDInfoEvent : public Event {
private:
    uint32 m_totalTracks;
    uint32 m_cddb;
    string m_cdindex;
public:
    CDInfoEvent(const uint32 numtracks, const uint32 cddb, const char *cdindex)
    { m_type = INFO_CDDiscStatus; m_totalTracks = numtracks; 
      m_cddb = cddb; m_cdindex = cdindex; }
    virtual ~CDInfoEvent() {}

    const uint32 GetNumTracks() const { return m_totalTracks; }
    const uint32 GetCDDB() const { return m_cddb; }
    const string GetCdindex() const { return m_cdindex; }
};

class MusicCatalogStreamAddedEvent : public Event {
private:
    const PlaylistItem *m_item;
public:
    MusicCatalogStreamAddedEvent(const PlaylistItem *item)
    { m_type = INFO_MusicCatalogStreamAdded; m_item = item; }
    virtual ~MusicCatalogStreamAddedEvent() {}

    const PlaylistItem *Item() const { return m_item; }
};

class MusicCatalogStreamRemovedEvent : public Event {
private:
    const PlaylistItem* m_item;
public:
    MusicCatalogStreamRemovedEvent(const PlaylistItem *item)
    { m_type = INFO_MusicCatalogStreamRemoved; m_item = item; }
    virtual ~MusicCatalogStreamRemovedEvent() {}

    const PlaylistItem *Item() const { return m_item; }
};

class ArtistList;
class AlbumList;

class MusicCatalogTrackAddedEvent : public Event {
private:
    const PlaylistItem *m_item;
    const ArtistList *m_artist;
    const AlbumList *m_album;
public:
    MusicCatalogTrackAddedEvent(const PlaylistItem *item, 
                                const ArtistList *artist,
                                const AlbumList *album)
    { m_type = INFO_MusicCatalogTrackAdded; m_item = item; 
      m_artist = artist; m_album = album; }
    virtual ~MusicCatalogTrackAddedEvent() {}

    const PlaylistItem *Item() const { return m_item; }
    const ArtistList *Artist() const { return m_artist; }
    const AlbumList *Album() const { return m_album; }
};

class MusicCatalogTrackRemovedEvent : public Event {
private:
    const PlaylistItem* m_item;
    const ArtistList *m_artist;
    const AlbumList *m_album;
public:
    MusicCatalogTrackRemovedEvent(const PlaylistItem *item,
                                  const ArtistList *artist,
                                  const AlbumList *album)
    { m_type = INFO_MusicCatalogTrackRemoved; m_item = item;
      m_artist = artist; m_album = album; }
    virtual ~MusicCatalogTrackRemovedEvent() {}

    const PlaylistItem *Item() const { return m_item; }
    const ArtistList *Artist() const { return m_artist; }
    const AlbumList *Album() const { return m_album; }
};

class MusicCatalogTrackChangedEvent : public Event {
private:
    const PlaylistItem *m_newItem;
    const PlaylistItem *m_oldItem; 
    const ArtistList *m_newArtist;
    const ArtistList *m_oldArtist;
    const AlbumList *m_newAlbum;
    const AlbumList *m_oldAlbum;
public:
    MusicCatalogTrackChangedEvent(const PlaylistItem *oldItem,
                                  const PlaylistItem *newItem,
                                  const ArtistList *oldArtist,
                                  const ArtistList *newArtist,
                                  const AlbumList *oldAlbum,
                                  const AlbumList *newAlbum)
    { m_type = INFO_MusicCatalogTrackChanged; m_oldItem = oldItem; 
      m_newItem = newItem; m_oldArtist = oldArtist; m_newArtist = newArtist; 
      m_oldAlbum = oldAlbum; m_newAlbum = newAlbum; }
    virtual ~MusicCatalogTrackChangedEvent() {}

    const PlaylistItem *NewItem() const { return m_newItem; }
    const PlaylistItem *OldItem() const { return m_oldItem; }
    const ArtistList *NewArtist() const { return m_newArtist; }
    const ArtistList *OldArtist() const { return m_oldArtist; }
    const AlbumList *NewAlbum() const { return m_newAlbum; }
    const AlbumList *OldAlbum() const { return m_oldAlbum; }
};

class MusicCatalogPlaylistAddedEvent : public Event {
private:
    string m_item;
public:
    MusicCatalogPlaylistAddedEvent(string &item)
    { m_type = INFO_MusicCatalogPlaylistAdded; m_item = item; }
    virtual ~MusicCatalogPlaylistAddedEvent() {}

    const string Item() const { return m_item; }
};

class MusicCatalogPlaylistRemovedEvent : public Event {
private:
    string m_item;
public:
    MusicCatalogPlaylistRemovedEvent(string & item)
    { m_type = INFO_MusicCatalogPlaylistRemoved; m_item = item; }
    virtual ~MusicCatalogPlaylistRemovedEvent() {}

    const string Item() const { return m_item; }
};

class PlaylistItemAddedEvent : public Event {
  private:
    const PlaylistItem* m_item;
    const PlaylistManager* m_plm;
public:
	PlaylistItemAddedEvent(const PlaylistItem* item, const PlaylistManager* plm) 
    { m_type = INFO_PlaylistItemAdded; m_item = item; m_plm = plm;}
	virtual ~PlaylistItemAddedEvent() {}

	const PlaylistItem* Item() const { return m_item; }
    const PlaylistManager* Manager() const { return m_plm; }
};

class PlaylistItemsAddedEvent : public Event {
  private:
    vector<PlaylistItem*> m_items;
    const PlaylistManager* m_plm;
public:
	PlaylistItemsAddedEvent(vector<PlaylistItem*>* items, const PlaylistManager* plm) 
    { m_type = INFO_PlaylistItemsAdded; m_items = *items; m_plm = plm;}
	virtual ~PlaylistItemsAddedEvent() {}

	const vector<PlaylistItem*>* Items() const { return &m_items; }
    const PlaylistManager* Manager() const { return m_plm; }
};

class PlaylistItemMovedEvent : public Event {
private:
	const PlaylistItem* m_item;
    const PlaylistManager* m_plm;
    uint32 m_oldIndex, m_newIndex;
public:
	PlaylistItemMovedEvent(uint32 oldIndex, 
                           uint32 newIndex, 
                           const PlaylistItem* item,
                           const PlaylistManager* plm) 
    { m_type = INFO_PlaylistItemMoved; m_item = item; m_plm = plm;
      m_oldIndex = oldIndex; m_newIndex = newIndex;}
	virtual ~PlaylistItemMovedEvent() {}

	const PlaylistItem* Item() const { return m_item; }
    const PlaylistManager* Manager() const { return m_plm; }
    uint32 OldIndex() const { return m_oldIndex; }
    uint32 NewIndex() const { return m_newIndex; }
};

class PlaylistItemRemovedEvent : public Event {
private:
	vector<PlaylistItem*> m_items;
    const PlaylistManager* m_plm;
    vector<uint32> m_indices;
public:
	PlaylistItemRemovedEvent(vector<PlaylistItem*>* items, 
                             vector<uint32>* indices, 
                             const PlaylistManager* plm) 
    { m_type = INFO_PlaylistItemRemoved; m_items = *items; m_plm = plm; m_indices = *indices; }
	virtual ~PlaylistItemRemovedEvent() {}

	const vector<PlaylistItem*>* Items() const { return &m_items; }
    const vector<uint32>* Indices() const { return &m_indices; }
    const PlaylistManager* Manager() const { return m_plm; }
};

class PlaylistItemsUpdatedEvent : public Event {
  private:
    vector<PlaylistItem*> m_items;
    const PlaylistManager* m_plm;
public:
	PlaylistItemsUpdatedEvent(vector<PlaylistItem*>* items, const PlaylistManager* plm) 
    { m_type = INFO_PlaylistItemsUpdated; m_items = *items; m_plm = plm;}
	virtual ~PlaylistItemsUpdatedEvent() {}

	const vector<PlaylistItem*>* Items() const { return &m_items; }
    const PlaylistManager* Manager() const { return m_plm; }
};

class PlaylistCurrentItemInfoEvent : public Event {
private:
	const PlaylistItem* m_item;
    const PlaylistManager* m_plm;
public:
	PlaylistCurrentItemInfoEvent(const PlaylistItem* item, const PlaylistManager* plm) 
    { m_type = INFO_PlaylistCurrentItemInfo; m_item = item; m_plm = plm;}
	virtual ~PlaylistCurrentItemInfoEvent() {}

	const PlaylistItem* Item() const { return m_item; }
    const PlaylistManager* Manager() const { return m_plm; }
};

class PlaylistSortedEvent : public Event {
private:
	PlaylistSortKey m_key;
    const PlaylistManager* m_plm;
public:
	PlaylistSortedEvent(PlaylistSortKey key, const PlaylistManager* plm) 
    { m_type = INFO_PlaylistSorted; m_key = key; m_plm = plm;}
	virtual ~PlaylistSortedEvent() {}

	PlaylistSortKey GetSortKey() const { return m_key; }
    const PlaylistManager* Manager() const { return m_plm; }
};

class PlaylistRepeatEvent : public Event {
private:
	RepeatMode m_rm;
    const PlaylistManager* m_plm;
public:
	PlaylistRepeatEvent(RepeatMode rm, const PlaylistManager* plm) 
    { m_type = INFO_PlaylistRepeat; m_rm = rm; m_plm = plm;}
	virtual ~PlaylistRepeatEvent() {}

	RepeatMode GetRepeatMode() const { return m_rm; }
    const PlaylistManager* Manager() const { return m_plm; }
};

class PlaylistShuffleEvent : public Event {
private:
	bool m_sm;
    const PlaylistManager* m_plm;
public:
	PlaylistShuffleEvent(bool sm, const PlaylistManager* plm) 
    { m_type = INFO_PlaylistShuffle; m_sm = sm; m_plm = plm;}
	virtual ~PlaylistShuffleEvent() {}

	bool GetShuffleMode() const { return m_sm; }
    const PlaylistManager* Manager() const { return m_plm; }
};

class DownloadItem;

class DownloadItemAddedEvent : public Event {
public:
    DownloadItemAddedEvent(DownloadItem* item) 
    { m_type = INFO_DownloadItemAdded; m_item = item; }
	virtual ~DownloadItemAddedEvent() {}

	DownloadItem* Item() { return m_item; }
private:
    DownloadItem* m_item;
};

class DownloadItemRemovedEvent : public Event {
public:
    DownloadItemRemovedEvent(DownloadItem* item) 
    { m_type = INFO_DownloadItemRemoved; m_item = item; }
	virtual ~DownloadItemRemovedEvent() {}

	DownloadItem* Item() { return m_item; }
private:
    DownloadItem* m_item;
};

class DownloadItemNewStateEvent : public Event {
public:
    DownloadItemNewStateEvent(DownloadItem* item) 
    { m_type = INFO_DownloadItemNewState; m_item = item; }
	virtual ~DownloadItemNewStateEvent() {}

	DownloadItem* Item() { return m_item; }
private:
    DownloadItem* m_item;
};

class DownloadItemProgressEvent : public Event {
public:
    DownloadItemProgressEvent(DownloadItem* item) 
    { m_type = INFO_DownloadItemProgress; m_item = item; }
	virtual ~DownloadItemProgressEvent() {}

	DownloadItem* Item() { return m_item; }
private:
    DownloadItem* m_item;
};

class LoadThemeEvent : public Event {
public:
    LoadThemeEvent(const char *url, const char *saved) 
         { m_type = CMD_LoadTheme; 
           m_url = strdup_new(url); 
           m_saved = strdup_new(saved);
         }
	virtual ~LoadThemeEvent() 
        { 
            if (m_url) delete [] m_url; 
            if (m_saved) delete [] m_saved;
        }

   const char *URL(void) { return m_url; };
   const char *SavedTheme(void) { return m_saved; };

private:

   char *m_url;
   char *m_saved;
};

class ShowPreferencesEvent:public Event
{
 private:
   int32 m_page;
 public:
   ShowPreferencesEvent(int32 page = 0)
   { m_type = CMD_ShowPreferences; m_page = page; }
   int32 GetDefaultPage() const{ return m_page; }
   virtual ~ ShowPreferencesEvent(){ }
};

class MissingFileEvent:public Event
{
 private:
   PlaylistItem *m_item;
 public:
   MissingFileEvent(PlaylistItem *item)
   { m_type = INFO_FileNotFound; m_item = item; }
   virtual ~ MissingFileEvent(){ }
   PlaylistItem *Item() const{ return m_item; }
};

class CDNotFoundEvent:public Event
{
 private:
   string m_submitUrl;
 public:
   CDNotFoundEvent(const string &url)
   { m_type = INFO_CDNotFound; m_submitUrl = url; }
   virtual ~ CDNotFoundEvent(){ }
   const string &URL() const{ return m_submitUrl; }
};

#endif /* _EVENTDATA_H_ */
