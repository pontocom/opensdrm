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
	
	$Id: vorbis.h,v 1.1 2004/10/13 16:16:12 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_VORBIS_H
#define INCLUDED_VORBIS_H

#include <string>

using namespace std;

#include <assert.h>

#include "config.h"
#include "errors.h"
#include "facontext.h"

#include "metadata.h"

class Vorbis : public MetaDataFormat {
 public:
            Vorbis(FAContext *context);
    virtual ~Vorbis();

    virtual bool ReadMetaData(const char* url, MetaData* metadata);
    virtual bool WriteMetaData(const char* url, const MetaData& metadata);
    virtual MetaDataOrder Order() { return MetaDataOrder_Medium; }

 private:

    const string ConvertToISO(const char *utf8);
};

#endif // INCLUDED_VORBIS_H

