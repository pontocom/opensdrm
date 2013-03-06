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
	
	$Id: rmp.h,v 1.1 2004/10/13 16:16:05 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_RMP_H
#define INCLUDED_RMP_H

#include <string>

using namespace std;

#include <assert.h>

#include "config.h"
#include "errors.h"
#include "facontext.h"
#include "Parse.h"
#include "metadata.h"

#include "downloadformat.h"
#include "downloadmanager.h"

class RMP : public DownloadFormat, public Parse {
 public:
    RMP(FAContext *context);
    virtual ~RMP();

    virtual Error GetSupportedFormats(DownloadFormatInfo* info, uint32 index);
    virtual Error ReadDownloadFile(const char* url, vector<DownloadItem*>* items);

 protected:
 
            Error BeginElement(string &oElement, AttrMap &oAttrMap);
            Error EndElement(string &oElement);
            Error PCData(string &oData);
            void  MangleLocation(string &oLocation);

 private:
 
     vector<DownloadItem*> *m_pList;
     string                 m_oPath, m_oFileName, m_oLocation;
	 string                 m_oPackageId, m_oListId, m_oTrackId;  
     string                 m_oPlaylist, m_oServer, m_oTrackDir;
	 string                 m_oCookie, m_oCookieName, m_oCookieValue;
     MetaData              *m_pMetaData;
};



#endif // INCLUDED_RMP_H

