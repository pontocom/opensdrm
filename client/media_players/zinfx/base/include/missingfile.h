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

        $Id: missingfile.h,v 1.1 2004/10/13 16:16:04 linha2 Exp $
 ____________________________________________________________________________*/

#ifndef INCLUDED_MISSINGFILE_H_
#define INCLUDED_MISSINGFILE_H_

#include <string>
using namespace std;

#include "eventdata.h"

class FAContext;

class MissingFile 
{
 public:
    MissingFile(FAContext *context);
    virtual ~MissingFile();

    Error FindMissingFile(PlaylistItem *item, const string &searchPath, 
                          string &newUrl);
    Error AcceptLocation(PlaylistItem *item,
                         const string &newUrl);

 protected:

    Error RecurseFindMissingFile(const string &file, const string &searchPath, 
                                 string &newUrl);

    FAContext *m_context;
 private:
};
#endif
