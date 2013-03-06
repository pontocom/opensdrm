/*____________________________________________________________________________
	
	FreeAMP - The Free MP3 Player
	Portions copyright (C) 1998-2000 EMusic.com

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
	
	$Id: utility.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

/* System Includes */
#include <stdio.h>
#include <string.h>
#include <iostream.h>
#include <unistd.h>

/* Project Includes */
#include "utility.h"
#include "errors.h"

#define MAIN_KEY    HKEY_CURRENT_USER
#define INSTALL     "InstallDirectory"
#define UI          "UI"
#define DEFAULT_UI  "freeamp"

Error GetInstallDirectory(char* path, int32 len)
{
    getcwd(path,len);
    return kError_NoErr;
}

