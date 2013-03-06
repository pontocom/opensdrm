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
	
	$Id: win32prefs.h,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_WIN32PREFS_H
#define INCLUDED_WIN32PREFS_H

#include <windows.h>
#include "preferences.h"


class Win32Prefs : public Preferences {
 public:
    Win32Prefs();
    Win32Prefs(const char* componentName);
    ~Win32Prefs();

    virtual Error SetDefaults();
    virtual Error Save();

    virtual Preferences* ComponentPrefs(const char* componentName);

    // foundation preference functions
    virtual Error GetPrefString(const char* pref, char* buf, uint32* len);
    virtual Error SetPrefString(const char* pref, const char* buf);

    virtual Error GetPrefBoolean(const char* pref, bool* value);
    virtual Error SetPrefBoolean(const char* pref, bool value);

    virtual Error GetPrefInt32(const char* pref, int32* value);
    virtual Error SetPrefInt32(const char* pref, int32 value);
    virtual const char* GetLibDirs() { return NULL; }

 private:
    Error Initialize();

    HKEY   m_prefsKey;
};

#endif /* _WIN32PREFS_H */
