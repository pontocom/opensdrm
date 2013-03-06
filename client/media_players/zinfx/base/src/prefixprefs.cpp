/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player

	Portions Copyright (C) 1998-1999 EMusic.com
	Portions Copyright (C) 1999 Mark H. Weaver <mhw@netris.org>

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
	
	$Id: prefixprefs.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#include <stdlib.h>
#include <string.h>
#include "prefixprefs.h"

static char *AppendStringsWithDot(const char *a, const char *b)
{
    int aLen = strlen(a);
    int bLen = strlen(b);
    char *str = new char[aLen + bLen + 2];

    memcpy(str, a, aLen);
    str[aLen] = '.';
    memcpy(str + aLen + 1, b, bLen);
    str[aLen + bLen + 1] = '\0';
    return str;
}

PrefixPrefs::
PrefixPrefs(Preferences *parent, const char *prefix)
     : m_parent(parent), m_prefix(strdup(prefix))
{
}

PrefixPrefs::
~PrefixPrefs()
{
    free(m_prefix);
}

Error
PrefixPrefs::
SetDefaults()
{
    // Nothing to do
    return kError_NoErr;
}

Error
PrefixPrefs::
Save()
{
    return m_parent->Save();
}

Preferences *
PrefixPrefs::
ComponentPrefs(const char *componentName)
{
    const char *newPrefix = AppendStringsWithDot(m_prefix, componentName);
    Preferences *result = new PrefixPrefs(m_parent, newPrefix);
    delete[] newPrefix;

    return result;
}

Error
PrefixPrefs::
GetPrefString(const char* pref, char* buf, uint32* len)
{
    char *newPref = AppendStringsWithDot(m_prefix, pref);
    Error result = m_parent->GetPrefString(newPref, buf, len);
    delete[] newPref;

    return result;
}

Error
PrefixPrefs::
SetPrefString(const char* pref, const char* buf)
{
    char *newPref = AppendStringsWithDot(m_prefix, pref);
    Error result = m_parent->SetPrefString(newPref, buf);
    delete[] newPref;

    return result;
}

Error
PrefixPrefs::
GetPrefBoolean(const char* pref, bool* value)
{
    char *newPref = AppendStringsWithDot(m_prefix, pref);
    Error result = m_parent->GetPrefBoolean(newPref, value);
    delete[] newPref;

    return result;
}

Error
PrefixPrefs::
SetPrefBoolean(const char* pref, bool value)
{
    char *newPref = AppendStringsWithDot(m_prefix, pref);
    Error result = m_parent->SetPrefBoolean(newPref, value);
    delete[] newPref;

    return result;
}

Error
PrefixPrefs::
GetPrefInt32(const char* pref, int32* value)
{
    char *newPref = AppendStringsWithDot(m_prefix, pref);
    Error result = m_parent->GetPrefInt32(newPref, value);
    delete[] newPref;

    return result;
}

Error
PrefixPrefs::
SetPrefInt32(const char* pref, int32 value)
{
    char *newPref = AppendStringsWithDot(m_prefix, pref);
    Error result = m_parent->SetPrefInt32(newPref, value);
    delete[] newPref;

    return result;
}

const char *
PrefixPrefs::
GetLibDirs()
{
    return m_parent->GetLibDirs();
}

LibDirFindHandle *
PrefixPrefs::
GetFirstLibDir(char *path, uint32 *len)
{
    return m_parent->GetFirstLibDir(path, len);
}

Error
PrefixPrefs::
GetNextLibDir(LibDirFindHandle *hLibDirFind, char *path, uint32 *len)
{
    return m_parent->GetNextLibDir(hLibDirFind, path, len);
}

Error
PrefixPrefs::
GetLibDirClose(LibDirFindHandle *hLibDirFind)
{
    return m_parent->GetLibDirClose(hLibDirFind);
}

