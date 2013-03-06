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
	
	$Id: propimpl.h,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PROPIMPL_H_
#define INCLUDED_PROPIMPL_H_

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <stdlib.h>

#include <vector>
#include <map>
#include <string>

using namespace std;

#include "config.h"
#include "mutex.h"
#include "properties.h"
#include "debug.h"

class PropElem 
{
   public:
       PropElem() 
         { m_val = NULL; }
      ~PropElem() 
         {  
            if (m_val) 
            {
               Debug_v("Propelem: delete %p", m_val);
               delete m_val; 
            }   
         }
         
      vector<PropertyWatcher *> m_propWatchers;
      PropValue *m_val;
};

class PropertiesImpl : public Properties {
 private:
    map<string, PropElem *> m_props;
    Mutex m_lock;
 public:
    PropertiesImpl();
    virtual ~PropertiesImpl();
    virtual Error GetProperty(const char *, PropValue **);
    virtual Error SetProperty(const char *, PropValue *);
    virtual Error RegisterPropertyWatcher(const char *, PropertyWatcher *);
    virtual Error RemovePropertyWatcher(const char *, PropertyWatcher *);
};

#endif // _PROPIMPL_H_


