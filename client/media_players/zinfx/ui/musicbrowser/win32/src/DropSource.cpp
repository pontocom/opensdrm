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

        $Id: DropSource.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
____________________________________________________________________________*/

// system header files
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

// project header files
#include "DropSource.h"
#include "resource.h"

// IUnknown Methods

STDMETHODIMP DropSource::QueryInterface(REFIID iid, void FAR* FAR* ppv) 
{
    if((iid == IID_IUnknown) || (iid == IID_IDropSource)) 
    {
        *ppv = this;
        ++m_refs;
        return NOERROR;
    }

    *ppv = NULL;

    return E_NOINTERFACE;
}


STDMETHODIMP_(ULONG) DropSource::AddRef(void) 
{
    return ++m_refs;
}


STDMETHODIMP_(ULONG) DropSource::Release(void) 
{
    if(--m_refs == 0) 
    {
        delete this;
        return 0;
    }
    return m_refs;
}  
 
// DropSource Constructor
 
DropSource::DropSource(HWND hwndSrc, HIMAGELIST himl, 
                       POINT hotspot, POINT dragPt)
{
    m_hwnd = hwndSrc;
    m_refs = 1; 
  
    // Start the drag operation. 
    ImageList_BeginDrag(himl, 
                        0, 
                        hotspot.x,
                        hotspot.y);

    // ImageList_BeginDrag makes its own copy...
    ImageList_Destroy(himl);
 
    POINT pt = dragPt;
    
    ClientToScreen(hwndSrc, &pt);

    ImageList_DragEnter(NULL, pt.x, pt.y);
}

DropSource::~DropSource() 
{
    ImageList_DragShowNolock(FALSE);
    ImageList_DragLeave(NULL);
    ImageList_EndDrag();
}


//	________________________________________
//
//	IDropSource Methods

STDMETHODIMP DropSource::QueryContinueDrag(BOOL fEscapePressed, 
                                           DWORD grfKeyState) 
{  
    POINT	pt;

	GetCursorPos(&pt);

    ImageList_DragMove(pt.x, pt.y);

    if(fEscapePressed)
        return DRAGDROP_S_CANCEL;
    else if(!(grfKeyState & MK_LBUTTON) && !(grfKeyState & MK_RBUTTON))
        return DRAGDROP_S_DROP;
    else
        return NOERROR;
}

STDMETHODIMP DropSource::GiveFeedback(DWORD dwEffect) 
{

	return DRAGDROP_S_USEDEFAULTCURSORS;
}
