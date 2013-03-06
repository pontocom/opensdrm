/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player

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
	
	$Id: misc.cpp,v 1.1 2004/10/13 16:16:12 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#ifdef __QNX__
#include <strings.h>
#endif

#include "config.h"
#include "errors.h"
#include "utility.h"

#include "misc.h"
#include "lmc.h"
#include "pmi.h"


#ifndef COMPILING_SIGAPP
extern "C"
{
   MetaDataFormat *Initialize(FAContext* context)
   {
      return new Misc(context);
   }
}
#endif

Misc::Misc(FAContext* context):MetaDataFormat(context)
{
    Registrar registrar;

    registrar.SetSubDir("plugins");
    registrar.SetSearchString("*.lmc");
    registrar.InitializeRegistry(&m_lmcReg, m_context->prefs);

    registrar.SetSearchString("*.pmi");
    registrar.InitializeRegistry(&m_pmiReg, m_context->prefs);

    for (uint32 i = 0; i < (uint32)m_lmcReg.CountItems(); i++)
    {
        LogicalMediaConverter* lmc;
        RegistryItem* temp = m_lmcReg.GetItem(i);

        lmc = (LogicalMediaConverter *)temp->InitFunction()(m_context);
        vector<string>* extList = lmc->GetExtensions();

        for (uint32 j = 0; j < extList->size(); j++)
        {
            string ext = (*extList)[j];
            m_extensions[ext] = temp;
        }

        delete extList;
        delete lmc;
    }
}

Misc::~Misc()
{

}

bool Misc::ReadMetaData(const char* url, MetaData* metadata)
{
    // bool result = false;

    assert(url);
    assert(metadata);

    // do we need to calculate the song length?
    if(!strncasecmp(url, "file://", 7) && !metadata->Time())
    {
        RegistryItem *lmcItem = NULL;
        char* cp;

        cp = strrchr(url, '.');
        if(cp)
        {
            cp++;
            
            char temp[256];

            strcpy(temp, cp);

            cp = temp;

            while(*cp)
            {
                *cp = toupper(*cp);
                cp++;
            }

            string ext = temp;

            ExtensionMap::const_iterator iter = m_extensions.find(ext);
            if(iter != m_extensions.end())
            {
                LogicalMediaConverter *lmc;
                uint32                 length = 0;

                lmcItem = (*iter).second; 

                lmc = (LogicalMediaConverter*)lmcItem->InitFunction()(m_context);
                length = lmc->CalculateSongLength(url);
                metadata->SetTime(length);

                delete lmc;
            }
        }
    }

    // do we need to come up with a name?
    if(!strncasecmp(url, "file://", 7) && !metadata->Title().size())
    {
        char* temp = new char[strlen(url) + 1];

        strcpy(temp, url);

        char* ext = strrchr(temp, '.');
        char* file = strrchr(temp, '/'); // these are all URLs so we don't need DIR_MARKER
        
        if(ext) {
            *ext = 0x00;
            ext++;
        }
        
        if(!file)
            file = temp;
        else
            file++;
    
        if (ext && *ext && !strncasecmp("CDA", ext, 3) && atoi(file) != 0) {
            char *cdTrack = new char[20];
            sprintf(cdTrack, "CD Track %d", atoi(file));
            metadata->SetTitle(cdTrack);
            delete [] cdTrack;
        }
        else
            metadata->SetTitle(file);

        delete [] temp;
    }
    else if(!strncasecmp(url, "http://", 7) && !metadata->Title().size())
    {
        metadata->SetTitle("HTTP Stream");
    }
    else if(!strncasecmp(url, "rtp://", 6) && !metadata->Title().size())
    {
        metadata->SetTitle("RTP Stream");
    }

    // do we need to come up with a track number?
    if (!strncasecmp(url, "file://", 7) && !metadata->Track())
    {
        char *temp = new char[strlen(url) + 1];
        strcpy(temp, url);

        char *ext = strrchr(temp, '.');
        char *file = strrchr(temp, '/');

        if (ext) {
            *ext = '\0';
            ext++;
        }
        if (!file)
            file = temp;
        else
            file++;
        
        while (file && !isdigit(*file))
            file++;

        if (strlen(file) > 4)
            metadata->SetTrack(atoi(file));

        delete [] temp;
    }

    // do we need to convert underscores?
    bool convertUnderscores = false;

    m_context->prefs->GetPrefBoolean(kConvertUnderscoresToSpacesPref, &convertUnderscores);

    if(convertUnderscores)
    {
        string::size_type pos = 0;
        string temp;

        temp = metadata->Title();
        
        while(pos != string::npos)
        {
            pos = temp.find('_', pos);

            if(pos != string::npos)
            {
                temp[pos++] = ' ';
            }
        }

        metadata->SetTitle(temp);

        pos = 0;
        temp = metadata->Artist();
        
        while(pos != string::npos)
        {
            pos = temp.find('_', pos);

            if(pos != string::npos)
            {
                temp[pos++] = ' ';
            }
        }

        metadata->SetArtist(temp.c_str());

        pos = 0;
        temp = metadata->Album();
        
        while(pos != string::npos)
        {
            pos = temp.find('_', pos);

            if(pos != string::npos)
            {
                temp[pos++] = ' ';
            }
        }

        metadata->SetAlbum(temp.c_str());
    }

    return true;
}

bool Misc::WriteMetaData(const char* url, const MetaData& metadata)
{
    bool result = false;
    return result;
}

