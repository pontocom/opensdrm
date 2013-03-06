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
	
	$Id: Icecast.cpp,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <iostream>
#include <string>

using namespace std;

#include "config.h"
#include "errors.h"
#include "Icecast.h"

IcecastStreams::IcecastStreams(void)
{
    m_list = NULL;
    m_info = NULL;
}

IcecastStreams::~IcecastStreams(void)
{
    delete m_list;
    delete m_info;
}

Error IcecastStreams::ParseStreamXML(const string &xml, 
                                     vector<IcecastStreamInfo> &list)
{
    Error result = kError_InvalidParam;

    m_list = &list;
    result = ParseString(xml);
    m_list = NULL;

    return result;
}

Error IcecastStreams::BeginElement(string &oElement, AttrMap &oAttrMap)
{
    m_path += string("/") + oElement;

    //printf("Path: '%s'\n", m_path.c_str());

    if (m_path == string("/directory/resource"))
    {
        string href("href");

        delete m_info;
        m_info = new IcecastStreamInfo();
        if (oAttrMap.find(href) != oAttrMap.end())
            m_info->m_streamUrl = oAttrMap[href];
    }

    return kError_NoErr;
}

Error IcecastStreams::PCData(string &data)
{
    if (m_path == string("/directory/resource/name"))
    {
        m_info->m_name = data;
        return kError_NoErr;
    }
    if (m_path == string("/directory/resource/url"))
    {
        m_info->m_webUrl = data;
        return kError_NoErr;
    }
    if (m_path == string("/directory/resource/email"))
    {
        m_info->m_eMail = data;
        return kError_NoErr;
    }
    if (m_path == string("/directory/resource/description"))
    {
        m_info->m_desc = data;
        return kError_NoErr;
    }
    if (m_path == string("/directory/resource/bitrate"))
    {
        m_info->m_bitrate = atoi(data.c_str());
        return kError_NoErr;
    }
        
    return kError_NoErr;
}


Error IcecastStreams::EndElement(string &oElement)
{
    char *pPtr;
    int   iOffset;

    if (m_path == string("/directory/resource"))
    {
        m_list->push_back(*m_info);
        delete m_info;
        m_info = NULL;
    }
       
  
    pPtr = strrchr(m_path.c_str(), '/');
    if (pPtr == NULL)
       return kError_NoErr;
       
    iOffset = pPtr - m_path.c_str();
    m_path.erase(iOffset, m_path.length() - iOffset);   

    return kError_NoErr;
}

#if 0

const char *test = "<directory>"
                   "  <resource href=\"http://icecast.server.com:8000/stream\">"
                   "    <bitrate>64000</bitrate>"
                   "    <name>Some stream</name>"
                   "    <url>http://www.icecast.org</url>"
                   "    <email>jack@icecast.org</email>"
                   "    <description>This is some description</description>"
                   "  </resource>"
                   "</directory>\n";

void main(void)
{
    vector<IcecastStreamInfo>           list;
    vector<IcecastStreamInfo>::iterator i;
    IcecastStreams                      o;

    o.ParseStreamXML(string(test), list);
    if (list.size() == 0)
        printf("0 items in vector\n");
    else
    {
        printf("%d items in vector:\n\n", list.size());

        for(i = list.begin(); i != list.end(); i++)
        {
            printf("streamurl: '%s'\n", i->m_streamUrl.c_str());
            printf("     name: '%s'\n", i->m_name.c_str());
            printf("   weburl: '%s'\n", i->m_webUrl.c_str());
            printf("    email: '%s'\n", i->m_eMail.c_str());
            printf("     desc: '%s'\n", i->m_desc.c_str());
            printf("  bitrate:  %d\n\n", i->m_bitrate);
        }
    }
}
#endif
