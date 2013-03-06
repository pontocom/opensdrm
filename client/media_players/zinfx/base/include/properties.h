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
	
	$Id: properties.h,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PROPERTIES_H_
#define INCLUDED_PROPERTIES_H_

#include <string.h>
#include "config.h"
#include "errors.h"

class PropValue {
 public:
    virtual ~PropValue() {}
};

class StringPropValue : public PropValue {
 public:
    StringPropValue(const char *pChars) {
	m_pChars = NULL;
	m_length = 0;
	if (pChars) {
	    m_length = strlen(pChars);
	    m_pChars = new char[m_length+1];
	    memcpy(m_pChars,pChars,m_length);
	    m_pChars[m_length] = '\0';
	}
    }
    virtual ~StringPropValue() {
	if (m_pChars) {
	    delete m_pChars;
	    m_pChars = 0;
	    m_length = 0;
	}
    }
    const char *GetString() { return m_pChars; }
 private:
    char *m_pChars;
    int32 m_length;
};

class Int32PropValue : public PropValue {
 public:
    Int32PropValue(int32 i) {
	m_int = i;
    }
    virtual ~Int32PropValue() {}
    int32 GetInt32() { return m_int; }
 private:
    int32 m_int;
};

class PropertyWatcher {
 public:
    virtual Error PropertyChange(const char *, PropValue *) = 0;
};

class Properties {
 public:
    virtual Error GetProperty(const char *, PropValue **) = 0;
    virtual Error SetProperty(const char *, PropValue *) = 0;
    virtual Error RegisterPropertyWatcher(const char *, PropertyWatcher *) = 0;
    virtual Error RemovePropertyWatcher(const char *, PropertyWatcher *) = 0;
};


#endif // _PROPERTIES_H_

