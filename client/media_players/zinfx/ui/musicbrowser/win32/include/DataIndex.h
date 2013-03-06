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

        $Id: DataIndex.h,v 1.1 2004/10/13 16:16:15 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_DATAINDEX_H_
#define INCLUDED_DATAINDEX_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "playlist.h"
#include "musiccatalog.h"

class TreeData
{
 public:

    TreeData();
    TreeData(const TreeData &oOther);

    virtual ~TreeData();

    bool  IsPlaylist();
    bool  IsArtist();
    bool  IsAlbum();
    bool  IsTrack();
    bool  IsLeaf();
    bool  IsUncatagorized();
    bool  IsPortable();
    bool  IsStream();
    int32 GetLevel();

    int32         m_iLevel;
    ArtistList   *m_pArtist;
    AlbumList    *m_pAlbum;
    PlaylistItem *m_pTrack;
    DeviceInfo   *m_pPortable;
    PlaylistItem *m_pStream;
    string        m_oPlaylistName, m_oPlaylistPath;
};

bool operator<(const TreeData &A, const TreeData &b);
bool operator==(const TreeData &A, const TreeData &b);

#endif
