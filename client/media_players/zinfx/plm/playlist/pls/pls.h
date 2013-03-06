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
	
	$Id: pls.h,v 1.1 2004/10/13 16:16:12 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PLS_H
#define INCLUDED_PLS_H

#include <string>

using namespace std;

#include <assert.h>

#include "config.h"
#include "errors.h"
#include "facontext.h"

#include "playlistformat.h"
#include "playlist.h"

class PLS : public PlaylistFormat {
 public:
    PLS(FAContext *context);
    virtual ~PLS();

    virtual Error GetSupportedFormats(PlaylistFormatInfo* info, uint32 index);
    virtual Error ReadPlaylist( const char* url, vector<PlaylistItem*>* items,
                                PLMCallBackFunction function = NULL,
                                void* cookie = NULL);
    virtual Error WritePlaylist(const char* url, PlaylistFormatInfo* format, 
                                vector<PlaylistItem*>* items,
                                PLMCallBackFunction function = NULL,
                                void* cookie = NULL); 

 private:

     void AddItem(vector<PlaylistItem*>* list, char *entry, 
	              char *title, int32 len, char *root);
};



#endif // INCLUDED_PLS_H

