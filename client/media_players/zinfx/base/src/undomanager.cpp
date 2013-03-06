/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player

	Portions Copyright (C) 1999 EMusic.com

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
	
	$Id: undomanager.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <assert.h>
#include <list>
#include <functional>

using namespace std;

#include "config.h"
#include "facontext.h"
#include "errors.h"
#include "mutex.h"
#include "thread.h"
#include "undomanager.h"


UndoManager::UndoManager()
{
    m_current = kRootIndex;
    m_doing = false;
}

UndoManager::~UndoManager()
{
    vector<UndoItem*>::iterator i = m_items.begin();

    for(;i != m_items.end(); i++)
        delete (*i);
}
 
void UndoManager::Undo()
{
    m_mutex.Acquire();

    if(CanUndo())
    {
        m_doing = true;

        m_items[m_current--]->Undo();

        m_doing = false;
    }

    m_mutex.Release();
}

void UndoManager::Redo()
{
    m_mutex.Acquire();

    if(CanRedo())
    {
        m_doing = true;

        m_items[++m_current]->Redo();

        m_doing = false;
    }

    m_mutex.Release();
}

bool UndoManager::CanUndo()
{
    m_mutex.Acquire();
    
    bool result = false;
    uint32 size = m_items.size();

    if(size && m_current != kRootIndex)
        result = true;

    m_mutex.Release();

    return result;
}

bool UndoManager::CanRedo()
{
    m_mutex.Acquire();
    
    bool result = false;
    uint32 size = m_items.size();

    if(size && m_current != (size - 1))
        result = true;

    m_mutex.Release();

    return result;
}

void UndoManager::AddItem(UndoItem* item)
{
    m_mutex.Acquire();

    if(!m_doing)
    {
        uint32 size = m_items.size();

        if(size && m_current != (size - 1))
        {
            vector<UndoItem*>::iterator i = m_items.begin() + (m_current + 1);

            for(;i != m_items.end(); i++)
                delete (*i);

            m_items.erase(m_items.begin() + (m_current + 1), m_items.end());  
        }

        m_items.push_back(item);
        m_current = m_items.size() - 1;
    }

    m_mutex.Release();
}
 
void UndoManager::Clear()
{
    m_mutex.Acquire();

    m_items.clear();

    m_current = kRootIndex;

    m_mutex.Release();
}


/* MultiItem allows you to clump complex actions into one Undo */

UndoMultiItem::UndoMultiItem():UndoItem()
{
    
}

UndoMultiItem::~UndoMultiItem()
{
    vector<UndoItem*>::iterator i = m_items.begin();

    for(;i != m_items.end(); i++)
        delete (*i);
}

void UndoMultiItem::Undo()
{
    vector<UndoItem*>::reverse_iterator i = m_items.rbegin();

    for(;i != m_items.rend(); i++)
        (*i)->Undo();
}

void UndoMultiItem::Redo()
{
    vector<UndoItem*>::iterator i = m_items.begin();

    for(;i != m_items.end(); i++)
        (*i)->Redo();
}

void UndoMultiItem::AddItem(UndoItem* item)
{
    m_items.push_back(item);
}
