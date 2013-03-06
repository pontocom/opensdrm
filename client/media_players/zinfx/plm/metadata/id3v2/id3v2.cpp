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
	
	$Id: id3v2.cpp,v 1.1 2004/10/13 16:16:11 linha2 Exp $
____________________________________________________________________________*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#ifdef WIN32
#include <winsock.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#endif

#ifdef __QNX__
#include <strings.h>
#endif

#include "id3config.h"
#include "errors.h"
#include "utility.h"
#include "debug.h"

// Define the HAVE_ID3V2 since we have the support for that built right now
#define HAVE_ID3V2
#include "id3.h"
#include "id3v2.h"

#define DB printf("%s:%d\n", __FILE__, __LINE__);

const int iDataFieldLen = 1024;
const int genreOther = 12;
const int genreMax = 126; 

const char *genreList[] = 
{
     "Blues",
     "Classic Rock",
     "Country",
     "Dance",
     "Disco",
     "Funk",
     "Grunge",
     "Hip-Hop",
     "Jazz",
     "Metal",
     "New Age",
     "Oldies",
     "Other",
     "Pop",
     "R&B",
     "Rap",
     "Reggae",
     "Rock",
     "Techno",
     "Industrial",
     "Alternative",
     "Ska",
     "Death Metal",
     "Pranks",
     "Soundtrack",
     "Euro-Techno",
     "Ambient",
     "Trip-Hop",
     "Vocal",
     "Jazz+Funk",
     "Fusion",
     "Trance",
     "Classical",
     "Instrumental",
     "Acid",
     "House",
     "Game",
     "Sound Clip",
     "Gospel",
     "Noise",
     "AlternRock",
     "Bass",
     "Soul",
     "Punk",
     "Space",
     "Meditative",
     "Instrumental Pop",
     "Instrumental Rock",
     "Ethnic",
     "Gothic",
     "Darkwave",
     "Techno-Industrial",
     "Electronic",
     "Pop-Folk",
     "Eurodance",
     "Dream",
     "Southern Rock",
     "Comedy",
     "Cult",
     "Gangsta",
     "Top 40",
     "Christian Rap",
     "Pop/Funk",
     "Jungle",
     "Native American",
     "Cabaret",
     "New Wave",
     "Psychadelic",
     "Rave",
     "Showtunes",
     "Trailer",
     "Lo-Fi",
     "Tribal",
     "Acid Punk",
     "Acid Jazz",
     "Polka",
     "Retro",
     "Musical",
     "Rock & Roll",
     "Hard Rock",
     "Folk",
     "Folk-Rock",
     "National Folk",
     "Swing",
     "Fast Fusion",
     "Bebob",
     "Latin",
     "Revival",
     "Celtic",
     "Bluegrass",
     "Avantgarde",
     "Gothic Rock",
     "Progressive Rock",
     "Psychedelic Rock",
     "Symphonic Rock",
     "Slow Rock",
     "Big Band",
     "Chorus",
     "Easy Listening",
     "Acoustic",
     "Humour",
     "Speech",
     "Chanson",
     "Opera",
     "Chamber Music",
     "Sonata",
     "Symphony",
     "Booty Bass",
     "Primus",
     "Porn Groove",
     "Satire",
     "Slow Jam",
     "Club",
     "Tango",
     "Samba",
     "Folklore",
     "Ballad",
     "Power Ballad",
     "Rhythmic Soul",
     "Freestyle",
     "Duet",
     "Punk Rock",
     "Drum Solo",
     "Acapella",
     "Euro-House",
     "Dance Hall",
     "\0"
};

#ifndef COMPILING_SIGAPP
extern "C"
{
   MetaDataFormat *Initialize(FAContext* context)
   {
      return new ID3v2(context);
   }
}
#endif

ID3v2::ID3v2(FAContext* context):MetaDataFormat(context)
{
}

ID3v2::~ID3v2()
{

}

#ifdef HAVE_ID3V2
bool ID3v2::ReadMetaData(const char* url, MetaData* metadata)
{
    ID3Tag   *pTag;
    ID3Frame *pFrame;
    char     *pData;
    char     *ptr;
    ID3Field *pField;

    assert(url);
    assert(metadata);

    ptr = strrchr(url, '.');
    if (ptr == NULL)
        return false;

    if (strcasecmp(ptr, ".mp3")) 
        return false;

    char path[_MAX_PATH];
    uint32 length = sizeof(path);

    URLToFilePath(url, path, &length);

    pTag = ID3Tag_New();
#ifdef WIN32
    int ret = ID3Tag_Link(pTag, path);
	if (ret <= 0)
	{
		ID3Tag_Delete(pTag);
		return false;
	}
#else
    ID3Tag_Link(pTag, path);
#endif

    pData = new char[iDataFieldLen + 1];

    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_TITLE);
    if (pFrame)
    {
        pData[0] = 0;
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_GetASCII(pField, pData, iDataFieldLen); 
        if (strlen(pData) > 0)
           metadata->SetTitle(pData);
    }
    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_ALBUM);
    if (pFrame)
    {
        pData[0] = 0;
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_GetASCII(pField, pData, iDataFieldLen); 
        if (strlen(pData) > 0)
           metadata->SetAlbum(pData);
    }

    // Pull artist out of LEADARTIST if it exists
    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_LEADARTIST);
    if (pFrame)
    {
        pData[0] = 0;
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_GetASCII(pField, pData, iDataFieldLen); 
        if (strlen(pData) > 0)
           metadata->SetArtist(pData);
    }
    else // No?, Ok, try to pull artist out of BAND if it exists
    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_BAND);
    if (pFrame)
    {
        pData[0] = 0;
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_GetASCII(pField, pData, iDataFieldLen); 
        if (strlen(pData) > 0)
           metadata->SetArtist(pData);
    }
    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_COMMENT);
    if (pFrame)
    {
        pData[0] = 0;
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_GetASCII(pField, pData, iDataFieldLen); 
        if (strlen(pData) > 0)
           metadata->SetComment(pData);
    }
    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_SONGLEN);
    if (pFrame)
    {
        pData[0] = 0;
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_GetASCII(pField, pData, iDataFieldLen); 
        if (strlen(pData) > 0) 
           metadata->SetTime(atoi(pData) / 1000);
    }
    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_YEAR);
    if (pFrame)
    {
        pData[0] = 0;
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_GetASCII(pField, pData, iDataFieldLen); 
        if (strlen(pData) > 0)
           metadata->SetYear(atoi(pData));
    }
    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_SIZE);
    if (pFrame)
    {
        pData[0] = 0;
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_GetASCII(pField, pData, iDataFieldLen); 
        if (strlen(pData) > 0)
           metadata->SetSize(atoi(pData));
    }
    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_TRACKNUM);
    if (pFrame)
    {
        pData[0] = 0;
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_GetASCII(pField, pData, iDataFieldLen); 
        if (strlen(pData) > 0)
           metadata->SetTrack(atoi(pData));
    }
    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_TRACKNUM);
    if (pFrame)
    {
        pData[0] = 0;
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_GetASCII(pField, pData, iDataFieldLen); 
        if (strlen(pData) > 0)
           metadata->SetTrack(atoi(pData));
    }
    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_CONTENTTYPE);
    if (pFrame)
    {
        char genre[255];
        int  genreId, ret;
 
        pData[0] = 0;
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_GetASCII(pField, pData, iDataFieldLen); 
        if (strlen(pData) > 0)
        {
           genre[0] = 0;
           ret = sscanf(pData, "(%d)%[^\n]", &genreId, genre);
           if (ret > 0)
           {
               if (ret == 2)
                   metadata->SetGenre(genre);
               else
               if (genreId != genreOther)
               {
                   LookupGenre(genreId, genre);
                   metadata->SetGenre(genre);
               }
           }
           else
           {
               metadata->SetGenre(pData);
           }
        }
    }

    delete pData;
    ID3Tag_Delete(pTag);

    return true;
}

bool ID3v2::WriteMetaData(const char* url, const MetaData& metadata)
{
    ID3Tag   *pTag;
    ID3Frame *pFrame;
    ID3Field *pField;
    ID3_Err   err;
    char      dummy[20];
    bool      bWriteID3v1, bWriteID3v2;
    luint     whichTags;
    char     *ptr;

    ptr = strrchr(url, '.');
    if (ptr == NULL)
        return false;

    if (strcasecmp(ptr, ".mp3"))
        return false;  

    m_context->prefs->GetPrefBoolean(kWriteID3v1Pref, &bWriteID3v1);
    m_context->prefs->GetPrefBoolean(kWriteID3v2Pref, &bWriteID3v2);

    whichTags = (bWriteID3v1) ? ID3TT_ID3V1 : 0;
    whichTags |= (bWriteID3v2) ? ID3TT_ID3V2 : 0;

    if (!whichTags)
        return true;

    assert(url);

    char path[_MAX_PATH];
    uint32 length = sizeof(path);

    URLToFilePath(url, path, &length);

    pTag = ID3Tag_New();
    ID3Tag_Link(pTag, path);

    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_TITLE);
    if (!pFrame)
    {
        pFrame = ID3Frame_NewID(ID3FID_TITLE);
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, (char *)metadata.Title().c_str());
        ID3Tag_AttachFrame(pTag, pFrame);
    }
    else
    {
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, (char *)metadata.Title().c_str());
    }
   
    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_ALBUM);
    if (!pFrame)
    {
        pFrame = ID3Frame_NewID(ID3FID_ALBUM);
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, (char *)metadata.Album().c_str());
        ID3Tag_AttachFrame(pTag, pFrame);
    }
    else
    {
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, (char *)metadata.Album().c_str());
    }

    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_LEADARTIST);
    if (!pFrame)
    {
        pFrame = ID3Frame_NewID(ID3FID_LEADARTIST);
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, (char *)metadata.Artist().c_str());
        ID3Tag_AttachFrame(pTag, pFrame);
    }
    else
    {
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, (char *)metadata.Artist().c_str());
    }

    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_COMMENT);
    if (!pFrame)
    {
        pFrame = ID3Frame_NewID(ID3FID_COMMENT);
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, (char *)metadata.Comment().c_str());
        ID3Tag_AttachFrame(pTag, pFrame);
    }
    else
    {
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, (char *)metadata.Comment().c_str());
    }

    sprintf(dummy, "%d", metadata.Time() * 1000);
    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_SONGLEN);
    if (!pFrame)
    {
        pFrame = ID3Frame_NewID(ID3FID_SONGLEN);
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, dummy);
        ID3Tag_AttachFrame(pTag, pFrame);
    }
    else
    {
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, dummy);
    }

    sprintf(dummy, "%d", metadata.Year());
    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_YEAR);
    if (!pFrame)
    {
        pFrame = ID3Frame_NewID(ID3FID_YEAR);
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, dummy);
        ID3Tag_AttachFrame(pTag, pFrame);
    }
    else
    {
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, dummy);
    }

    sprintf(dummy, "%d", metadata.Size());
    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_SIZE);
    if (!pFrame)
    {
        pFrame = ID3Frame_NewID(ID3FID_SIZE);
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, dummy);
        ID3Tag_AttachFrame(pTag, pFrame);
    }
    else
    {
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, dummy);
    }

    sprintf(dummy, "%d", metadata.Track());
    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_TRACKNUM);
    if (!pFrame)
    {
        pFrame = ID3Frame_NewID(ID3FID_TRACKNUM);
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, dummy);
        ID3Tag_AttachFrame(pTag, pFrame);
    }
    else
    {
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, dummy);
    }

    sprintf(dummy, "(12)%s", metadata.Genre().c_str());
    pFrame = ID3Tag_FindFrameWithID(pTag, ID3FID_CONTENTTYPE);
    if (!pFrame)
    {
        pFrame = ID3Frame_NewID(ID3FID_CONTENTTYPE);
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, dummy);
        ID3Tag_AttachFrame(pTag, pFrame);
    }
    else
    {
        pField = ID3Frame_GetField(pFrame, ID3FN_TEXT);
        ID3Field_SetASCII(pField, dummy);
    }

    err = ID3Tag_UpdateByTagType(pTag, whichTags);

    ID3Tag_Delete(pTag);

    return err == ID3E_NoError;
}

#else

const int supportedVersion = 3;

struct ID3Header
{
   char          tag[3];
   unsigned char versionMajor;
   unsigned char versionRevision;
   unsigned char flags;
   unsigned char size[4];
};
struct FrameHeader
{
   char           tag[4];
   unsigned int   size;
   unsigned short flags;
};
const unsigned frameHeaderSize = 10;

void ID3v2::HandleFrame(char *tag, char *frameData, MetaData *metadata)
{
    char tagName[5];

    strncpy(tagName, tag, 4);
    tagName[4] = 0;

    if (strcmp(tagName, "TIT2") == 0)
        metadata->SetTitle(frameData);

    if (strcmp(tagName, "TALB") == 0)
        metadata->SetAlbum(frameData);

    if (strcmp(tagName, "TPE1") == 0)
        metadata->SetArtist(frameData);

    if (strcmp(tagName, "TCOM") == 0)
        metadata->SetComment(frameData);

    if (strcmp(tagName, "TLEN") == 0)
        metadata->SetTime(atoi(frameData) / 1000);

    if (strcmp(tagName, "TYER") == 0)
        metadata->SetYear(atoi(frameData));

    if (strcmp(tagName, "TRCK") == 0)
        metadata->SetTrack(atoi(frameData));

    if (strcmp(tagName, "TSIZ") == 0)
        metadata->SetSize(atoi(frameData));

    if (strcmp(tagName, "TCON") == 0)
    {
        char genre[255];
        int  genreId, ret;

        genre[0] = 0;
        ret = sscanf(frameData, "(%d)%[^\n]", &genreId, genre);
        if (ret > 0)
        {
            if (ret == 2)
                metadata->SetGenre(genre);
            else
            if (genreId != genreOther)
            {
                LookupGenre(genreId, genre);
                metadata->SetGenre(genre);
            }
        }
        else
        {
            metadata->SetGenre(frameData);
        }
    }
}

bool ID3v2::ReadMetaData(const char* url, MetaData* metadata)
{
    bool        result = false;
    FILE       *inFile;
    char        buffer[1024], *frameData;
    ID3Header   sHead;
    FrameHeader sFrame;
    int         ret;
    int         size;
    Error       error;
    char       *ptr;

    assert(url);
    assert(metadata);

    ptr = strrchr(url, '.');
    if (ptr == NULL)
        return false;

    if (strcasecmp(ptr, ".mp3"))
        return false;  

    char path[_MAX_PATH];
    uint32 length = sizeof(path);

    error = URLToFilePath(url, path, &length);

    if(IsntError(error))
    {
        inFile = fopen(path, "rb");
        if (inFile == NULL)
            return result;

        ret = fread(&sHead, 1, sizeof(ID3Header), inFile);
        if (ret != sizeof(ID3Header))
        {
            fclose(inFile);
            return result;
        }

        if (strncmp(sHead.tag, "ID3", 3))
        {
            fclose(inFile);
            return result;
        }

        if (sHead.versionMajor != supportedVersion)
        {
            fclose(inFile);
            return result;
        }
        size = ( sHead.size[3] & 0x7F       ) |
               ((sHead.size[2] & 0x7F) << 7 ) |
               ((sHead.size[1] & 0x7F) << 14) |
               ((sHead.size[0] & 0x7F) << 21);
        if (sHead.flags & (1 << 6))
        {
            unsigned extHeaderSize;

            if (fread(&extHeaderSize, 1, sizeof(int), inFile) != sizeof(int))
            {
                fclose(inFile);
                return result;
            }
            if (fread(buffer, 1, extHeaderSize, inFile) != extHeaderSize)
            {
                fclose(inFile);
                return result;
            }
        }
        for(; size > 0;)
        {
            if (fread(&sFrame, 1, frameHeaderSize, inFile) != frameHeaderSize)
            {
                fclose(inFile);
                return result;
            }
            sFrame.size = ntohl(sFrame.size);
            frameData = new char[sFrame.size + 1];
            if (fread(frameData, 1, sFrame.size, inFile) != sFrame.size)
            {
                delete frameData;
                fclose(inFile);
                return result;
            }
            frameData[sFrame.size] = 0;
            HandleFrame(sFrame.tag, &frameData[1], metadata);

            delete frameData;
            size -= sizeof(FrameHeader) + sFrame.size;
        }

        fclose(inFile);
    }
    return true;
}

bool ID3v2::WriteMetaData(const char* url, const MetaData& metadata)
{
    bool result = false;
    return result;
}

#endif

void ID3v2::LookupGenre(int genreId, char* genre)
{
    if (genreId < 0 || genreId > genreMax)
    {
        genre[0] = 0;
        return;
    }

    strcpy(genre, genreList[genreId]);
}

