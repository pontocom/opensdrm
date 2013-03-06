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
	
	$Id: dsoundvolume.cpp,v 1.1 2004/10/13 16:16:06 linha2 Exp $
____________________________________________________________________________*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "dsoundvolume.h"

DSoundVolumeManager::
DSoundVolumeManager():
VolumeManager()
{

}

int32
DSoundVolumeManager::
GetPrefInt32(kVolumePref, void)
{
    

    return 0;
}

void
DSoundVolumeManager::
SetPrefInt32(kVolumePref, int32 v)
{
   
}
