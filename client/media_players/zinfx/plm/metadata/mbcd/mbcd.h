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
	
	$Id: mbcd.h,v 1.1 2004/10/13 16:16:12 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_MBCD_H
#define INCLUDED_MBCD_H

#include <string>
#include <vector>

using namespace std;

#include <assert.h>
#include <time.h>

#if defined(solaris)
#include <stdarg.h>
#endif

#include "config.h"
#include "errors.h"
#include "metadata.h"
#include "mutex.h"
#include "database.h"
#include "musicbrainz/mb_c.h"

class MusicBrainzCD : public MetaDataFormat {
 public:
    MusicBrainzCD(FAContext *context);
    virtual ~MusicBrainzCD();

    virtual bool ReadMetaData(const char* url, MetaData* metadata);
    virtual bool WriteMetaData(const char* url, const MetaData& metadata);
    virtual MetaDataOrder Order() { return MetaDataOrder_Medium; }

 private:

    bool    LookupCD(void);
    void    WriteToCache(Database *db, char *diskId, 
                         vector<int> &trackLens, char *rdf);
    bool    ReadFromCache(Database *db, char *diskId, 
                         vector<int> &trackLens, string &rdf);

    musicbrainz_t o;
    vector<int> m_trackLens;
    int         m_nextTrack;
	Mutex       m_mutex;
	char        m_notFoundDiskId[48];
	time_t      m_notFoundDiskIdExpire;
};

#endif
