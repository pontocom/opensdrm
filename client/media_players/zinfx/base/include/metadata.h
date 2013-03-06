/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player

	Portions Copyright (C) 1999 EMusic.com

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
	
	$Id: metadata.h,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_METADATA_H_
#define INCLUDED_METADATA_H_

#include <string>

using namespace std;

#include <assert.h>

#include "config.h"
#include "errors.h"
#include "facontext.h"


class MetaData {

 public:

    MetaData()
    {
        m_year = 0;
        m_track = 0;
        m_time = 0;
        m_size = 0;
        m_playcount = 0;
    }

    virtual ~MetaData(){}

    Error SetArtist(const string &artist){ m_artist = artist; return kError_NoErr;}
    Error GetArtist(char* buf, uint32* len) { return SetBuffer(buf, m_artist.c_str(), len); }
    const string& Artist() const { return m_artist; }

    Error SetAlbum(const string &album) { m_album = album; return kError_NoErr; }
    Error GetAlbum(char* buf, uint32* len) { return SetBuffer(buf, m_album.c_str(), len); }
    const string& Album() const { return m_album; }

    Error SetTitle(const string &title){ m_title = title; return kError_NoErr; }
    Error GetTitle(char* buf, uint32* len) { return SetBuffer(buf, m_title.c_str(), len); }
    const string& Title() const { return m_title; }

    Error SetComment(const string &comment){ m_comment = comment; return kError_NoErr; }
    Error GetComment(char* buf, uint32* len) { return SetBuffer(buf, m_comment.c_str(), len); }
    const string& Comment() const { return m_comment; }

    Error SetGenre(const string &genre) { m_genre = genre; return kError_NoErr; }
    Error GetGenre(char* buf, uint32* len) { return SetBuffer(buf, m_genre.c_str(), len); }
    const string& Genre() const { return m_genre; }

	Error SetFormatExtension(const string &extension){ m_extension = extension; return kError_NoErr; }
    Error GetFormatExtension(char* buf, uint32* len) { return SetBuffer(buf, m_extension.c_str(), len); }
    const string& FormatExtension() const { return m_extension; }

    Error SetYear(uint32 year) { m_year = year; return kError_NoErr;}
    uint32 Year() const { return m_year; }

    Error SetTrack(uint32 track){ m_track = track; return kError_NoErr;}
    uint32 Track() const { return m_track; }

    Error SetTime(uint32 time){ m_time = time; return kError_NoErr;}
    uint32 Time() const { return m_time; }

    Error SetSize(uint32 bytes){ m_size = bytes; return kError_NoErr;}
    uint32 Size() const { return m_size; }

    Error SetGUID(const string &guid) { m_guid = guid; return kError_NoErr; }
    Error GetGUID(char *buf, uint32 *len) { return SetBuffer(buf, m_guid.c_str(), len); }
    const string& GUID() const { return m_guid; }

    Error SetPlayCount(uint32 playcount) { m_playcount = playcount; return kError_NoErr; }
    uint32 PlayCount() const { return m_playcount; }
    void AddPlayCount() { m_playcount++; }

    bool operator==(const MetaData& data) const
    {
        bool result = false;

        result = ( m_artist == data.m_artist &&
                   m_album == data.m_album &&
                   m_title == data.m_title &&
                   m_genre == data.m_genre &&
                   m_comment == data.m_comment &&
                   m_year == data.m_year &&
                   m_track == data.m_track &&
                   m_time == data.m_time &&
                   m_size == data.m_size &&
		   m_extension == data.m_extension);

        return result;
    }

    bool operator!=(const MetaData& data) const
    {
	    return !(*this == data);
    }
 
 protected:
    Error SetBuffer(char* dest, const char* src, uint32* len)
    {
        Error result = kError_InvalidParam;

        assert(dest);
        assert(src);
        assert(len);

        if(dest && src)
        {
            uint32 srclen = strlen(src) + 1;

            if(*len >= srclen)
            {
                strcpy(dest, src);
                result = kError_NoErr;
            }
            else
            {
                result = kError_BufferTooSmall;
            }

            *len = srclen;
        }

        return result;
    }

 private:
  
    string m_artist;
    string m_album;
    string m_title;
    string m_genre;
    string m_comment;
    string m_extension;
    string m_guid;
    uint32 m_year;
    uint32 m_track;
    uint32 m_time;
    uint32 m_size;
    uint32 m_playcount;
};

typedef enum MetaDataOrder {
    MetaDataOrder_First  =    0,  // for some reason this plugin should be called before others
    MetaDataOrder_Low    =    25, // not very reliable, perhaps parsing the filename
    MetaDataOrder_Medium =    50, // not too reliable, perhaps limited in the amount of data allowed
    MetaDataOrder_High   =    75, // reliable, no restrictions on length of data
    MetaDataOrder_Last   =    100 // for some reason this plugin should be called after others
} MetaDataOrder;

class MetaDataFormat {
 public:
    MetaDataFormat(FAContext *context) { m_context = context; }
    virtual ~MetaDataFormat() {}

    virtual bool ReadMetaData(const char* url, MetaData* metadata) = 0;
    virtual bool WriteMetaData(const char* url, const MetaData& metadata) = 0;
    virtual MetaDataOrder Order() = 0;

 protected:
    FAContext *m_context;
};



#endif // _METADATA_H_

