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

        $Id: DataIndex.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
____________________________________________________________________________*/

#include <windows.h>
#include <windowsx.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>

#include "config.h"
#include "utility.h"
#include "resource.h"
#include "Win32MusicBrowser.h"
#include "debug.h"


TreeData::TreeData()
{
    m_iLevel = -1;
    m_pArtist = NULL;
    m_pAlbum = NULL;
    m_pTrack = NULL;
    m_pPortable = NULL;
    m_pStream = NULL;
}

TreeData::TreeData(const TreeData &oOther)
{
    m_iLevel = oOther.m_iLevel; 
    m_pArtist= oOther.m_pArtist;
    m_pAlbum = oOther.m_pAlbum; 
    m_pTrack = oOther.m_pTrack; 
    m_oPlaylistName = oOther.m_oPlaylistName;
    m_oPlaylistPath = oOther.m_oPlaylistPath;
    m_pStream = oOther.m_pStream;

    if(oOther.m_pPortable)
        m_pPortable = new DeviceInfo(*oOther.m_pPortable);
    else
        m_pPortable = NULL;
};

TreeData::~TreeData()
{
    if(m_pPortable) 
        delete m_pPortable; 
        
    m_pPortable = NULL;
}


bool TreeData::IsPlaylist()
{    
    return m_oPlaylistName.length() > 0;
}

bool TreeData::IsArtist()
{
    bool result = false;

    result = m_pArtist != NULL &&
             m_pAlbum == NULL &&
             m_pTrack == NULL;
   
    return result;
}

bool TreeData::IsAlbum()
{
    bool result = false;

    result = m_pAlbum != NULL &&
             m_pArtist != NULL &&
             m_pTrack == NULL;

    return result;
}

bool TreeData::IsTrack()
{
   bool result = false;

    result = m_pTrack != NULL;
    
    return result;
}

bool TreeData::IsUncatagorized()
{
   bool result = false;

    result = m_pAlbum == NULL &&
             m_pArtist == NULL &&
             m_pTrack != NULL;

    return result;
}

bool TreeData::IsPortable()
{
   bool result = false;

    result = m_pPortable != NULL;

    return result;
}

bool TreeData::IsStream()
{
   bool result = false;

    result = m_pStream != NULL;
    
    return result;
}

bool TreeData::IsLeaf()
{
    return ( IsTrack() || IsPlaylist() || IsUncatagorized());
}

int32 TreeData::GetLevel()
{
    return m_iLevel;
}