/*____________________________________________________________________________
    
    FreeAmp - The Free MP3 Player

    Portions Copyright (C) 2000 EMusic.com

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
    
    $Id: mbcd.cpp,v 1.1 2004/10/13 16:16:12 linha2 Exp $
____________________________________________________________________________*/

#include <assert.h>
#include <string>
#include <errno.h>

using namespace std;

#include "config.h"
#include "errors.h"
#include "utility.h"

#include "mbcd.h"
#include "player.h"

const int iDataLen = 256;

#define DB Debug_v("%d", __LINE__);

extern "C"
{
   MetaDataFormat *Initialize(FAContext* context)
   {
      return new MusicBrainzCD(context);
   }
}

MusicBrainzCD::MusicBrainzCD(FAContext* context)
              :MetaDataFormat(context)
{
    o = NULL;
    m_nextTrack = -1;
	m_notFoundDiskId[0] = 0;
}

MusicBrainzCD::~MusicBrainzCD()
{
}

bool MusicBrainzCD::ReadMetaData(const char* url, MetaData* metadata)
{
    int  track, numTracks;
    char data[iDataLen], *ptr;

    assert(url);
    assert(metadata);

    if(!url || !metadata)
       return false;

    char *ext = strrchr(url, '.');
    if (!ext)
       return false;

    if (strncasecmp(".CDA", ext, 4))
       return false;

    ptr = strrchr(url, '/');
    if (ptr == NULL)
       return false;

    m_mutex.Acquire();

    track = atoi(ptr + 1);
    if (track != m_nextTrack)
    {
        if (o)
          mb_Delete(o);

        o = mb_New();
        mb_UseUTF8(o, 0);
        if (!LookupCD())
        {
           mb_Delete(o);
           o = NULL; 
           m_nextTrack = 2;
           m_mutex.Release();
           return false;
        }
        m_nextTrack = 1;
    }
    if (o == NULL)
    {
       m_mutex.Release();
       return false;
    }

    mb_Select1(o, MBS_SelectAlbum, 1);  

    numTracks = mb_GetResultInt(o, MBE_AlbumGetNumTracks);
    mb_GetResultData1(o, MBE_AlbumGetAlbumName, data, iDataLen, track);
    metadata->SetAlbum(data);
    mb_GetResultData1(o, MBE_AlbumGetArtistName, data, 256, track);
    metadata->SetArtist(data);
    mb_GetResultData1(o, MBE_AlbumGetTrackName, data, 256, track);
    metadata->SetTitle(string(data));
    metadata->SetTrack(track + 1);
    metadata->SetTime(m_trackLens[track - 1]);

    if (track == numTracks)
    {
       mb_Delete(o);
       o = NULL;
       m_nextTrack = -1;
    }
    m_nextTrack++;

    m_mutex.Release();

    return true;
}

bool MusicBrainzCD::LookupCD(void)
{
    char          error[iDataLen], *result;
    char          diskId[64];
    int           ret;
    unsigned short  proxyPort;
    char         *tempDir = new char[_MAX_PATH];
    uint32        length = _MAX_PATH;
    Database     *db;
    string        rdf, proxyServer;
    char          url[MAX_PATH], hostname[MAX_PATH];
    int           i, port, numSectors, numTracks;
    uint32        len = MAX_PATH;
    string        message;

    error[0] = 0;
    m_context->prefs->GetPrefString(kDatabaseDirPref, tempDir, &length);

    string database = string(tempDir) + string(DIR_MARKER_STR) +
                      string("mbcd");

    m_context->prefs->GetPrefString(kCDDevicePathPref, tempDir, &length);
    mb_SetDevice(o, tempDir);

    // Parse the musicbrainz server from the preference
    m_context->prefs->GetPrefString(kMBServerPref, url, &len);
    i = sscanf(url, " http://%[^:/]:%d", hostname, &port);
    if (i == 0)
        i = sscanf(url, " %[^:/]:%d", hostname, &port);
    if (i < 2)
        port = MUSICBRAINZ_PORT;
    if (i < 1)
        strcpy(hostname, MUSICBRAINZ_SERVER);

    mb_SetServer(o, hostname, port);
    if (GetProxySettings(m_context, proxyServer, proxyPort))
       mb_SetProxy(o, (char *)proxyServer.c_str(), proxyPort);

    ret = mb_Query(o, MBQ_GetCDTOC);
    if (!ret)
    {
       mb_GetQueryError(o, error, iDataLen);
       message = error;
       m_context->target->AcceptEvent(new BrowserMessageEvent(message));
       return false;
    }

    m_trackLens.clear();
    numTracks = mb_GetResultInt(o, MBE_TOCGetLastTrack);
    for(i = 2; i <= numTracks + 1; i++)
    {
       numSectors = mb_GetResultInt1(o, MBE_TOCGetTrackNumSectors, i) / 75;
       m_trackLens.push_back(numSectors);
    }

    mb_GetResultData(o, MBE_TOCGetCDIndexId, diskId, 64);

 	 if (strcmp(diskId, m_notFoundDiskId) == 0)
    {
	   time_t t;

	   time(&t);
	   if (t < m_notFoundDiskIdExpire)
          return false;

	   m_notFoundDiskId[0] = 0;
    }

    db = new Database(database.c_str());
    delete [] tempDir;

    if (ReadFromCache(db, diskId, m_trackLens, rdf) &&
        mb_SetResultRDF(o, (char *)rdf.c_str()))
    { 
        message = "Retrieved CD from local cache.";
        m_context->target->AcceptEvent(new  BrowserMessageEvent(message));
    }
    else
    {
        message = "Sending CD lookup query to MusicBrainz...";
        m_context->target->AcceptEvent(new BrowserMessageEvent(message));
        ret = mb_Query(o, MBQ_GetCDInfo);
        if (!ret)
        {
           mb_GetQueryError(o, error, iDataLen);
           message = error;
           m_context->target->AcceptEvent(new BrowserMessageEvent(message));
           delete db;
           return false;
        }
    
        if (mb_GetResultInt(o, MBE_GetNumAlbums) == 0)
        {
           char url[MAX_PATH];

           // Due to a shortcoming in the design of the metadata stuff, the
           // player will continue to ask for more tracks even after a CD has
           // not been found. In order to avoid hitting the server for each track
           // we'll keep the diskid around and if subsequently we're asked for a
           // query on the same CD, we will just skip the query. A timeout value
           // is associated with the not found cd id, so that if the cd info 
           // is entered into MB, and the user updates the cd we force the query
           // to the server after the timeout (suggested: 60 seconds)
           strcpy(m_notFoundDiskId, diskId);
           time(&m_notFoundDiskIdExpire);
           m_notFoundDiskIdExpire += 60;

           mb_GetWebSubmitURL(o, url, MAX_PATH);
           message = "CD not found.";
           m_context->target->AcceptEvent(new BrowserMessageEvent(message));
           m_context->target->AcceptEvent(new CDNotFoundEvent(url));
           delete db;
           return false;
        }  
    
        ret = mb_GetResultRDFLen(o) + 1;
        result = new char[ret];
        if (mb_GetResultRDF(o, result, ret))
        {
            message = "Found CD.";
            m_context->target->AcceptEvent(new BrowserMessageEvent(message));
            WriteToCache(db, diskId, m_trackLens, result); 
        }
        else
        {
            message = "Query failed.";
            m_context->target->AcceptEvent(new BrowserMessageEvent(message));
        }
        delete result;
    }
    delete db;

    return true;
}


bool MusicBrainzCD::WriteMetaData(const char* url, const MetaData& metadata)
{
    bool result = false;
    return result;
}

void MusicBrainzCD::WriteToCache(Database *db, char *diskId, 
                                 vector<int> &trackLens, char *rdf)
{
    char                  *data;
    vector<int>::iterator  i;

    if (db->Exists(diskId))
    {
       db->Remove(diskId);
    }

    data = new char[strlen(diskId) + strlen(rdf) + 1024];
    data[0] = 0;
    for(i = trackLens.begin(); i != trackLens.end(); i++)
       sprintf(data + strlen(data), "%d ", *i); 

    sprintf(data + strlen(data) - 1, "%s", rdf); 
    db->Insert(diskId, data);
    delete data;
}

bool MusicBrainzCD::ReadFromCache(Database *db, char *diskId, 
                                 vector<int> &trackLens, string &rdf)
{
    char *data, temp[1024], *ptr;
    bool  ret = false;

    if (db->Exists(diskId))
    {
        data = db->Value(diskId);    

        if (sscanf(data, "%[^<]", temp) > 0)
        {
           trackLens.clear();
           for(ptr = strtok(temp, " "); ptr; ptr = strtok(NULL, " "))
              trackLens.push_back(atoi(ptr));

           ptr = strchr(data, '<');
           if (ptr)
           {
               rdf = string(ptr);
               ret = true;
           }
        }

        delete data;
    }

    return ret;
}
