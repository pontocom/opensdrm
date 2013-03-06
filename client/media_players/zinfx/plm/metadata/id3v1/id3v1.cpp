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
	
	$Id: id3v1.cpp,v 1.1 2004/10/13 16:16:11 linha2 Exp $
____________________________________________________________________________*/

#include <assert.h>
#include <string>
#include <errno.h>

#ifdef __QNX__
#include <strings.h>
#endif

using namespace std;

#include "config.h"
#include "errors.h"
#include "utility.h"

#include "id3v1.h"

typedef struct id3v1_0 {
    char id[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[30];
    char genre;

} id3v1_0;

typedef struct id3v1_1 {
    char id[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[28];
    char zero;
    char track;
    char genre;

} id3v1_1;

typedef struct id3v1 {
    union {
        struct id3v1_0 v1_0;
        struct id3v1_1 v1_1;
    } id3;
} id3v1;

#define v1_0 id3.v1_0
#define v1_1 id3.v1_1

const char* genre_strings[] = {
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
    "Alternative Rock",
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
    "Goa", 
    "Drum & Bass", 
    "Club-House", 
    "Hardcore", 
    "Terror", 
    "Indie", 
    "BritPop", 
    "Negerpunk", 
    "Polsk Punk", 
    "Beat", 
    "Christian Gangsta Rap", 
    "Heavy Metal", 
    "Black Metal", 
    "Crossover", 
    "Contemporary Christian", 
    "Christian Rock",
    "Merengue", 
    "Salsa", 
    "Thrash Metal", 
    "Anime", 
    "Jpop", 
    "Synthpop" 
};

#define kNumGenres sizeof(genre_strings)/sizeof(char*)

#ifndef COMPILING_SIGAPP
extern "C"
{
   MetaDataFormat *Initialize(FAContext* context)
   {
      return new ID3v1(context);
   }
}
#endif

ID3v1::ID3v1(FAContext* context):MetaDataFormat(context)
{
}

ID3v1::~ID3v1()
{

}

bool ID3v1::ReadMetaData(const char* url, MetaData* metadata)
{
    bool result = false;
    Error error;
    id3v1 id3;
    char *ptr;

    assert(url);
    assert(metadata);

    ptr = strrchr(url, '.');
    if (ptr == NULL)
        return false;

    if (strcasecmp(ptr, ".mp3"))
        return false;  

    if(url && metadata)
    {
        FILE* fp;
        char path[_MAX_PATH];
        uint32 length = sizeof(path);

        error = URLToFilePath(url, path, &length);

        if(IsntError(error))
        {

            fp = fopen(path, "rb");

            if(fp)
            {
                if(!fseek(fp, -128, SEEK_END))
                {
                    if(fread(&id3, 128, 1, fp))
                    {
                        if(!strncmp(id3.v1_0.id, "TAG", 3))
                        {
                            char buffer[31];
         
                            strncpy(buffer, id3.v1_0.artist, 30);
                            buffer[30] = 0;
                            KillTrailingSpaces(buffer);
                            // simple test to see if we have "more" data
                            // would be nice if we can test for "better" data
                            if(strlen(buffer) > metadata->Artist().size())
                                metadata->SetArtist(buffer);

                            strncpy(buffer, id3.v1_0.album, 30);
                            buffer[30] = 0;
                            KillTrailingSpaces(buffer);
                            // simple test to see if we have "more" data
                            // would be nice if we can test for "better" data
                            if(strlen(buffer) > metadata->Album().size())
                                metadata->SetAlbum(buffer);

                            strncpy(buffer, id3.v1_0.title, 30);
                            buffer[30] = 0;
                            KillTrailingSpaces(buffer);
                            // simple test to see if we have "more" data
                            // would be nice if we can test for "better" data
                            if(strlen(buffer) > metadata->Title().size())
                                metadata->SetTitle(buffer);

                            strncpy(buffer, id3.v1_0.year,4);
                            buffer[4] = 0;
                            KillTrailingSpaces(buffer);
                            metadata->SetYear(atoi(buffer));

                            strncpy(buffer, id3.v1_0.comment, 
                                (id3.v1_1.zero ? 30 : 28));
                            buffer[(id3.v1_1.zero ? 30 : 28)] = 0;
                            KillTrailingSpaces(buffer);
                            // simple test to see if we have "more" data
                            // would be nice if we can test for "better" data
                            if(strlen(buffer) > metadata->Comment().size())
                                metadata->SetComment(buffer);

                            if( id3.v1_1.zero == 0x00 &&
                                id3.v1_1.track != 0x00)
                            {
                                uint32 track = id3.v1_1.track;

                                metadata->SetTrack(track);
                            }

                            if(id3.v1_0.genre >= 0 && (uint32)id3.v1_0.genre < kNumGenres)
                            {
                                // simple test to see if we have "more" data
                                // would be nice if we can test for "better" data
                                if(strlen(buffer) > metadata->Genre().size())
                                    metadata->SetGenre(genre_strings[id3.v1_0.genre]);
                            }
                            else
                            {
                                // simple test to see if we have "more" data
                                // would be nice if we can test for "better" data
                                if(!metadata->Genre().size())
                                    metadata->SetGenre("{unknown}");

                            }

                            result = true;
                        }
                    }
                }

                fclose(fp);
            }
        }
    }

    return result;
}

bool ID3v1::WriteMetaData(const char* url, const MetaData& metadata)
{
    bool result = true;

    // The id3v2 libary also handles id3v1 tags and you should therefore 
	// use the id3v2 object to write metadata.

    return result;
}

void ID3v1::KillTrailingSpaces(char* string)
{
	char* cp = &(string[strlen(string)]);

	do 
    {
	    *cp = '\0';
	    cp--;
	}while ((*cp == ' ') && (cp != string));
}
