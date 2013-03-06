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
    
    $Id: vorbis.cpp,v 1.1 2004/10/13 16:16:12 linha2 Exp $
____________________________________________________________________________*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

#include "vorbis.h"
#include "vorbis/vorbisfile.h"

#define CHUNK 4096

extern    "C"
{
   MetaDataFormat *Initialize(FAContext * context)
   {
      return new Vorbis(context);
   }
}

Vorbis::Vorbis(FAContext * context):MetaDataFormat(context)
{
}

Vorbis::~Vorbis()
{

}

bool Vorbis::WriteMetaData(const char *url, const MetaData & metadata)
{
   return false;
}

bool Vorbis::ReadMetaData(const char *url, MetaData * metadata)
{
    char            path[_MAX_PATH], *temp;
    uint32          len = _MAX_PATH;
    FILE           *fpFile;
    OggVorbis_File  vf;
    vorbis_comment *comment; 

    URLToFilePath(url, path, &len);
    if (strcasecmp(path + strlen(path) - 4, ".ogg"))
    {
       return false;
    }

    fpFile = fopen(path, "rb");
    if (fpFile == NULL)
       return false;

    memset(&vf, 0, sizeof(vf));
    if (ov_open(fpFile, &vf, NULL, 0) < 0)
    {
       fclose(fpFile);
       return false;
    }

    comment = ov_comment(&vf, -1); 
    if (comment)
    {
        temp = vorbis_comment_query(comment, "title", 0);
        if (temp)
        {
            string iso;
            iso = ConvertToISO(temp);
            metadata->SetTitle(iso);
        }
        temp = vorbis_comment_query(comment, "artist", 0);
        if (temp)
        {
            string iso;
            iso = ConvertToISO(temp);
            metadata->SetArtist(iso);
        }

        temp = vorbis_comment_query(comment, "album", 0);
        if (temp)
        {
            string iso;
            iso = ConvertToISO(temp);
            metadata->SetAlbum(iso);
        }

        temp = vorbis_comment_query(comment, "tracknumber", 0);
        if (temp)
            metadata->SetTrack(atoi(temp));
    }
    ov_clear(&vf);   

    return true;
}

const string Vorbis::ConvertToISO(const char *utf8)
{
   unsigned char *in, *buf;
   unsigned char *out, *end;
   string               ret;

   in = (unsigned char *)utf8;
   buf = out = new unsigned char[strlen(utf8) + 1];
   end = in + strlen(utf8);
   for(;*in != 0x00 && in <= end; in++, out++)
   {
       if (*in < 0x80)
       {  /* lower 7-bits unchanged */
          *out = *in;
       }
       else
       if (*in > 0xC3)
       { /* discard anything above 0xFF */
          *out = '?';
       }
       else
       if (*in & 0xC0)
       { /* parse upper 7-bits */
          if (in >= end)
            *out = 0;
          else
          {
            *out = (((*in) & 0x1F) << 6) | (0x3F & (*(++in)));
          }
       }  
       else
       {
          *out = '?';  /* this should never happen */
       }
   }
   *out = 0x00; /* append null */
   ret = string((char *)buf);
   delete buf;

   return ret;
}

