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
	
	$Id: id3v1.h,v 1.1 2004/10/13 16:16:11 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_ID3V1_H
#define INCLUDED_ID3V1_H

#include <string>

using namespace std;

#include <assert.h>

#include "config.h"
#include "errors.h"
#include "facontext.h"

#include "metadata.h"

class ID3v1 : public MetaDataFormat {
 public:
    ID3v1(FAContext *context);
    virtual ~ID3v1();

    virtual bool ReadMetaData(const char* url, MetaData* metadata);
    virtual bool WriteMetaData(const char* url, const MetaData& metadata);
    virtual MetaDataOrder Order() { return MetaDataOrder_Medium; }

 private:
     void KillTrailingSpaces(char* string);
};



#endif // INCLUDED_ID3V1_H

