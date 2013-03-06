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
	
	$Id: Icecast.h,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_ICECAST_H
#define INCLUDED_ICECAST_H

#include <string>
#include <vector>

using namespace std;

#include <assert.h>

#include "config.h"
#include "errors.h"
#include "Parse.h"

struct IcecastStreamInfo
{
    string m_streamUrl;
    string m_name;
    string m_webUrl;
    string m_eMail;
    string m_desc;
    int    m_bitrate;
};

/*

The data will look like this:
<directory>
    <resource href="http://icecast.server.com:8000/stream">
       <bitrate>64000</bitrate>
       <name>Some stream</name>
       <url>http://www.icecast.org</url>
       <email>jack@icecast.org</email>
       <description>This is some description</description>
    </resource>
</directory>

And it should be available at:

http://yp.icecast.org/yplist_long.xml 

*/

class IcecastStreams : public Parse 
{
 public:
             IcecastStreams(void);
    virtual ~IcecastStreams(void);

       Error ParseStreamXML(const string &xml, 
                            vector<IcecastStreamInfo> &list);

 protected:
 
       Error BeginElement(string &oElement, AttrMap &oAttrMap);
       Error EndElement(string &oElement);
       Error PCData(string &oData);

 private:
 
     string                     m_path;
     IcecastStreamInfo         *m_info;
     vector<IcecastStreamInfo> *m_list;
};

#endif // INCLUDED_RMP_H
