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
	
	$Id: registry.h,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_REGISTRY_H_
#define INCLUDED_REGISTRY_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#if _MSC_VER <= 1100
#define STRICT
#endif
#include <windows.h>
#endif

#include <vector>
#include <string>

using namespace std;

#include "config.h"

class FAContext;

#ifdef WIN32
typedef HMODULE ModuleRef;
#else
typedef void* ModuleRef;
#endif

typedef void *(*InitializeFunction)(FAContext *);

class RegistryItem {
 public:
    RegistryItem();
    virtual ~RegistryItem();

    void SetPath(char* path);
    const char* Path() {return m_path.c_str();}

    void SetName(char* name);
    const char* Name() {return m_name.c_str();}

    void SetDescription(char* description);
    const char* Description() { return m_description.c_str();}

    void SetInitFunction(InitializeFunction function);
    const InitializeFunction InitFunction() const { return m_init;}

    void SetModule(ModuleRef module);
    const ModuleRef Module() const { return m_module;}

    bool operator < (const RegistryItem& x) const
    {
        return (x.m_path < m_path);
    }

    bool operator == (const RegistryItem& x) const
    {
        return (x.m_path == m_path);
    }

 private:
    string              m_path;
    string              m_name;
    string              m_description;
    InitializeFunction  m_init;
    ModuleRef           m_module;

};

class Registry {
 public:
    Registry();
    virtual ~Registry();

    void AddItem(RegistryItem* item);
    RegistryItem* GetItem(int32 index);
	int32 CountItems();

 private:
    vector<RegistryItem*> m_elements;
};


#endif // _REGISTRY_H_
