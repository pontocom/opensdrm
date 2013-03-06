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
	
	$Id: registry.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#ifdef WIN32
#include <windows.h>
#else
#include "win32impl.h"
#endif

#include <iostream>
#include "registry.h"

using namespace std;

Registry::Registry()
{
    
}

Registry::~Registry()
{
#if 0
    uint32 count = m_elements.size();

    for(uint32 i = 0; i < count; i++) {
        FreeLibrary(m_elements[i]->Module());
        delete m_elements[i];
    }
#endif
}

void Registry::AddItem(RegistryItem* item)
{
    m_elements.push_back(item);
}

RegistryItem* Registry::GetItem(int32 index)
{
    RegistryItem* result = NULL;

    if(index < (int32) m_elements.size())
    {
        result = m_elements[index];
    }

    return result;
}

int32 Registry::CountItems()
{
    return m_elements.size();
}

RegistryItem::RegistryItem()
{
    m_init = NULL;
    m_module = NULL;
}

RegistryItem::~RegistryItem()
{
#ifdef WIN32
    if(m_module)
        FreeLibrary(m_module);

    m_module = NULL;
#endif

//    cout << "Delete Registry Item" << endl;

}

void RegistryItem::SetPath(char* path)
{
    if(path)
    {
       m_path = path;
    }
}

void RegistryItem::SetName(char* name)
{
    if(name)
    {
        m_name = name;
    }
}
 

void RegistryItem::SetDescription(char* description)
{
    if(description)
    {
        m_description = description;
    }
}

void RegistryItem::SetInitFunction(InitializeFunction function)
{
    if(function)
        m_init = function;
}

void RegistryItem::SetModule(ModuleRef module)
{
    if(module)
        m_module = module;
}
