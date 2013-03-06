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
	
	$Id: propimpl.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#include "propimpl.h"

PropertiesImpl::PropertiesImpl() 
{
}

PropertiesImpl::~PropertiesImpl() 
{
}

Error PropertiesImpl::GetProperty(const char *pProp, PropValue **ppVal) 
{
    Error rtn = kError_UnknownErr;
    m_lock.Acquire();
    if (ppVal) 
    {
	   *ppVal = NULL;
       if (pProp) 
       {
           map<string, PropElem *>::iterator i;
		   i = m_props.find(pProp);
           if (i != m_props.end()) 
           {
		       *ppVal = (*i).second->m_val;
		       rtn = kError_NoErr;
           }
       }
    }
    m_lock.Release();
    return rtn;
}

Error PropertiesImpl::SetProperty(const char *pProp, PropValue *pVal) 
{
    Error rtn = kError_UnknownErr;
    m_lock.Acquire();

	if (pProp) 
    {
       map<string, PropElem *>::iterator  i;
       PropElem                                *ppe;
       i = m_props.find(pProp);
       
       bool needToAddItem = false;
       if (i == m_props.end()) 
       {
	       ppe = new PropElem();
           needToAddItem = true;
       }
       else
           ppe = (*i).second;
           
	   if (ppe->m_val) 
       {
	       delete ppe->m_val;
	   }
       ppe->m_val = pVal;
       if (needToAddItem) 
       {
	      m_props[pProp] = ppe;
       }   
          
	   PropertyWatcher *pw = NULL;
       
	   for(uint32 j = 0; j < ppe->m_propWatchers.size(); j++) 
       {
	       pw = ppe->m_propWatchers[j];
	       if (pw) 
           {
	   	       pw->PropertyChange(pProp, pVal);
	       }
	   }
	   rtn = kError_NoErr;
    }
    
    m_lock.Release();
    return rtn;
}

Error PropertiesImpl::RegisterPropertyWatcher(const char *pProp, PropertyWatcher *pw) 
{
    Error rtn = kError_UnknownErr;
    m_lock.Acquire();

	if (pProp) 
    {
        map<string, PropElem *>::iterator i;
        i = m_props.find(pProp);
        
	    if (i != m_props.end()) 
        {
            PropElem *ppe = (*i).second;
            
	        ppe->m_propWatchers.push_back(pw);
	        rtn = kError_NoErr;
	    }
	}

    m_lock.Release();
    return rtn;
}

Error PropertiesImpl::RemovePropertyWatcher(const char *pProp, PropertyWatcher *pw) 
{
    Error rtn = kError_UnknownErr;
    m_lock.Acquire();

	if (pProp) 
    {
        map<string, PropElem *>::iterator i;
        vector<PropertyWatcher *>::iterator j;
        i = m_props.find(pProp);
        
	    if (i != m_props.end()) 
        {
           PropElem *ppe = (*i).second;
           
           for (j = ppe->m_propWatchers. end(); 
                j >= ppe->m_propWatchers.begin(); j--)
           {
	          if (pw == *j)
	              ppe->m_propWatchers.erase(j);
	       }
	       rtn = kError_NoErr;
	    }
	}

    m_lock.Release();
    return rtn;

}



