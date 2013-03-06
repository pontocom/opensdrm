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
	
	$Id: misc.h,v 1.1 2004/10/13 16:16:12 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_MISC_H
#define INCLUDED_MISC_H

#include <assert.h>

#include <string>
#include <vector>
#include <map>

using namespace std;

#include "config.h"
#include "errors.h"
#include "facontext.h"
#include "registrar.h"
#include "registry.h"

#include "metadata.h"

typedef map<string, RegistryItem*> ExtensionMap;

class Misc : public MetaDataFormat 
{
 public:
    Misc(FAContext *context);
    virtual ~Misc();

    virtual bool ReadMetaData(const char* url, MetaData* metadata);
    virtual bool WriteMetaData(const char* url, const MetaData& metadata);
    virtual MetaDataOrder Order() { return MetaDataOrder_Last; }

 private:

    Registry  m_lmcReg;
    Registry  m_pmiReg;
    ExtensionMap m_extensions;
};


#endif // INCLUDED_MISC_H

