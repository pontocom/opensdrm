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
	
	$Id: downloadformat.h,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_DOWNLOAD_FORMAT_H_
#define INCLUDED_DOWNLOAD_FORMAT_H_

#include <string>
#include <vector>

using namespace std;

#include <assert.h>

#include "config.h"
#include "errors.h"
#include "facontext.h"
#include "plmevent.h"

class DownloadFormatInfo;
class DownloadItem;

class DownloadFormat {

 public:

    DownloadFormat(FAContext *context) { m_context = context; }
    virtual ~DownloadFormat() {}

    virtual Error GetSupportedFormats(DownloadFormatInfo* info, uint32 index) = 0;
    virtual Error ReadDownloadFile(const char* url, vector<DownloadItem*>* items) = 0;

 protected:
    FAContext *m_context;   
};

typedef DownloadFormat* DownloadFormatRef;

class DownloadFormatInfo {

 public:

    DownloadFormatInfo() {}
    
    virtual ~DownloadFormatInfo() {}

    Error SetExtension(const char* extension)
    { m_extension = extension; return kError_NoErr; }
    const char* GetExtension() const { return m_extension.c_str(); }

    Error SetDescription(const char* description) 
    { m_description = description; return kError_NoErr; }
    const char* GetDescription() { return m_description.c_str(); }

    Error SetRef(DownloadFormatRef ref) { m_ref = ref; return kError_NoErr; }
    const DownloadFormatRef GetRef() const { return m_ref; }

 private:
  
    string m_extension;
    string m_description;
    DownloadFormatRef m_ref;
};



#endif // _DOWNLOAD_FORMAT_H_

