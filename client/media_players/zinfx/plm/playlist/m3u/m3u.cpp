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
	
	$Id: m3u.cpp,v 1.1 2004/10/13 16:16:12 linha2 Exp $
____________________________________________________________________________*/

#include <assert.h>
#include <iostream>
#include <string>
#include <stdlib.h>

#ifdef __QNX__
#include <strings.h>
#endif

using namespace std;

#include "config.h"
#include "errors.h"
#include "errno.h"
#include "utility.h"

#include "m3u.h"

typedef struct FormatInfoStruct {
    const char* extension;
    const char* description;

} FormatInfoStruct; 

static FormatInfoStruct formats[] = {
    {"m3u", "M3U Playlist Format"},
    {"gqmpeg", "M3U Playlist Format"}
};

#define kNumFormats (sizeof(formats)/sizeof(FormatInfoStruct))

extern "C"
{
   PlaylistFormat *Initialize(FAContext* context)
   {
      return new M3U(context);
   }
}

M3U::M3U(FAContext* context):PlaylistFormat(context)
{
}

M3U::~M3U()
{

}

Error M3U::GetSupportedFormats(PlaylistFormatInfo* info, uint32 index)
{
    Error result = kError_InvalidParam;

    assert(info);

    if(info)
    {
        result = kError_NoMoreFormats;

        if(index < kNumFormats)
        {
            info->SetExtension(formats[index].extension);
            info->SetDescription(formats[index].description);
            result = kError_NoErr;
        }
    }

    return result;
}

Error M3U::ReadPlaylist(const char* url, 
                        vector<PlaylistItem*>* list,
                        PLMCallBackFunction function,
                        void* cookie)
{
    Error result = kError_InvalidParam;

    assert(url);
    assert(list);

    if(url && list)
    {
        FILE* fp = NULL;
        char root[_MAX_PATH];
        char entry[_MAX_PATH];
        char path[_MAX_PATH];
        PlaylistItem* item;
        char* cp = NULL;
        uint32 length = sizeof(path);

        result = URLToFilePath(url, path, &length);

        if(IsntError(result))
        {

            strcpy(root, path);

            cp = strrchr(root, DIR_MARKER);

            if(cp)
                *(cp + 1) = 0x00;
	        
            result = kError_FileNotFound;

            fp = fopen(path, "rb");

            if(fp)
            {
                while(fgets(entry, _MAX_PATH, fp))
                {
                    int32 index;

                    // is this a comment line?
                    if(entry[0] == '#')
                        continue;

                    // if this is not a URL then let's
                    // enable people with different platforms 
                    // to swap files by changing the path 
                    // separator as necessary
                    if( strncmp(entry, "http://", 7) &&
                        strncmp(entry, "rtp://", 6) &&
                        strncmp(entry, "file://", 7))
                    {
                        for (index = strlen(entry) - 1; index >=0; index--)
                        {
                            if(entry[index] == '\\' && DIR_MARKER == '/')
                                entry[index] = DIR_MARKER;
                            else if(entry[index] == '/' && DIR_MARKER == '\\')
                                entry[index] = DIR_MARKER;
                        }
                    }

                    // get rid of nasty trailing whitespace
                    for (index = strlen(entry) - 1; index >=0; index--)
                    {
                        if(isspace(entry[index]))
                        {
                            entry[index] = 0x00;
                        }
                        else
                            break;
                    }

                    // is there anything left?
                    if(strlen(entry))
                    {
                        // is it a url already?
                        if (!strncmp(entry, "http://", 7) ||
                            !strncmp(entry, "rtp://", 6) ||
                            !strncmp(entry, "file://", 7))
                        {
                            strcpy(path, entry);
                        }
                        else 
                        {
                            // is the path relative?
                            if( !strncmp(entry, "..", 2) ||
                                (strncmp(entry + 1, ":\\", 2) &&
                                 strncmp(entry, DIR_MARKER_STR, 1)))
                            {
                                strcpy(path, root);
                                strcat(path, entry);
                            }
                            else
                            {
                                strcpy(path, entry);
                            }
                        
                            // make it a url so we can add it to the playlist
                            length = strlen(path) + 15;
                            char *itemurl = new char[length];

                            if (IsntError(FilePathToURL(path, itemurl, &length)))
                                strcpy(path, itemurl);
 
                            delete [] itemurl;
                        }

                        item = new PlaylistItem(path);

                        list->push_back(item);
                    }
                }

                fclose(fp);

                result = kError_NoErr;
            }
        }
    }

    return result;
}

Error M3U::WritePlaylist(const char* url, PlaylistFormatInfo* format, 
                         vector<PlaylistItem*>* list,
                         PLMCallBackFunction function,
                         void* cookie)
{
    Error result = kError_InvalidParam;

    assert(url);
    assert(format);
    assert(list);

    if(url && format && list)
    {
        result = kError_FormatNotSupported;

        if(!strcasecmp("m3u", format->GetExtension()))
        {
            FILE* fp = NULL;
            char path[_MAX_PATH];
            uint32 length = sizeof(path);

            URLToFilePath(url, path, &length);

            result = kError_FileNoAccess;

            fp = fopen(path, "wb");

            if(fp)
            {
                uint32 index;
                uint32 count;

                count = list->size();

                for(index = 0; index < count; index++)
                {
                    PlaylistItem* item = (*list)[index];

                    length = sizeof(path);

                    if(IsError(URLToFilePath(item->URL().c_str(), path, &length)))
                        fprintf(fp, "%s%s", item->URL().c_str(), LINE_END_MARKER_STR);
                    else
                        fprintf(fp, "%s%s", path, LINE_END_MARKER_STR);
                }

                fclose(fp);

                result = kError_NoErr;
            }
            else
            {
                int err = errno;

                switch(err)
                {
                    case EACCES:
                        result = kError_FileNoAccess;
                        break;

                    case EEXIST:
                        result = kError_FileNoAccess;
                        break;

                    case EINVAL:
                        result = kError_FileNoAccess;
                        break;

                    case ENOENT:
                        result = kError_FileNoAccess;
                        break;
                }
            }
        }
    }

    return result;
}
