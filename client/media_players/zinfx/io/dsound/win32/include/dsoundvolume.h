/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player

	Portions Copyright (C) 1998-1999 EMusic.com

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
	
	$Id: dsoundvolume.h,v 1.1 2004/10/13 16:16:06 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_DSOUNDVOLUME_H_
#define INCLUDED_DSOUNDVOLUME_H_

#include "config.h"
#include "volume.h"

class DSoundVolumeManager : public VolumeManager
{
 public:

    DSoundVolumeManager();

    virtual void SetVolume(int32);
    virtual int32 GetVolume(void);

 private:

};

#endif // _DSOUNDVOLUME_H_
