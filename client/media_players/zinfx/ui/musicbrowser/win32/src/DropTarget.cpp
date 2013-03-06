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

        $Id: DropTarget.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786) 
#endif

// system header files
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>

// project header files
#include "DropTarget.h"
#include "DropObject.h"

DropTarget* g_me = NULL;

// IUnknown Methods

STDMETHODIMP DropTarget::QueryInterface(REFIID iid, void FAR* FAR* ppv) 
{
    if((iid == IID_IUnknown) ||(iid == IID_IDropTarget)) 
    {
        *ppv = this;
        ++m_refs;
        return NOERROR;
    }

    *ppv = NULL;

    return E_NOINTERFACE;
}


STDMETHODIMP_(ULONG) DropTarget::AddRef(void) 
{
    return ++m_refs;
}


STDMETHODIMP_(ULONG) DropTarget::Release(void) 
{
    if(--m_refs == 0) 
    {
        delete this;
        return 0;
    }
    return m_refs;
}  

// DropTarget Constructor
 
DropTarget::DropTarget(HWND	hwnd) 
{
    m_refs = 1;  
	m_hwnd = hwnd;
	m_acceptFormat = false;
    m_allowMove = false;
	m_enabled = true;
    m_targetIsSource = false;
    m_scrolling = true;

    g_me = this;

    short pattern[8];
    HBITMAP bmp;

	for (int i = 0; i < 8; i++)
		pattern[i] = (WORD)(0x5555 << (i & 1));

	bmp = CreateBitmap(8, 8, 1, 1, &pattern);

    m_insertBrush = CreatePatternBrush(bmp);
    DeleteObject(bmp);
}

DropTarget::~DropTarget() 
{
    DeleteObject(m_insertBrush);
}

//	Enable/disable dropping
void DropTarget::Enable(bool enable) 
{
    m_enabled = enable;  
}

void DropTarget::TargetIsSource(bool isSrc)
{
    m_targetIsSource = isSrc;
}

#define SCROLL_OFF 0
#define SCROLL_UP 1
#define SCROLL_DOWN -1
#define SCROLL_TIMER 92173

void DropTarget::CheckAutoScroll(POINT pt)
{
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE;

    BOOL hasScroll = GetScrollInfo(m_hwnd, SB_VERT, &si);

    if(hasScroll && si.nMin != si.nMax)
    {
        RECT rect;

        GetClientRect(m_hwnd, &rect);

        //char buf[256];
        //sprintf(buf, "%d, %d\r\n", pt.x, pt.y);

        //sprintf(buf, "%d, %d, %d, %d\r\n", 
        //    itemRect.top, itemRect.left, itemRect.bottom, itemRect.right);
        //OutputDebugString(buf);


        if(PtInRect(&rect, pt))
        {
            if(pt.y < rect.top + 2*GetSystemMetrics(SM_CYHSCROLL))
            {
                //OutputDebugString("scroll up\r\n");
                // scroll up
                AutoScroll(SCROLL_UP);
            }
            else if(pt.y > rect.bottom - GetSystemMetrics(SM_CYHSCROLL))
            {
                // scroll down
                //OutputDebugString("scroll down\r\n");
                AutoScroll(SCROLL_DOWN);
            }
            else
            {
                //OutputDebugString("scroll off\r\n");
                AutoScroll(SCROLL_OFF);
            }
        }
    }
}


void DropTarget::ScrollFunction()
{
    ImageList_DragShowNolock(FALSE);

    //OutputDebugString("scrolling\r\n");

    HDC hdc = GetDC(m_hwnd);

    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, m_insertBrush);

    //OutputDebugString("erase old - scrolling\r\n");

    // erase old
    PatBlt(hdc,
           m_scrollRect.left, 
           m_scrollRect.top, 
           m_scrollRect.right - m_scrollRect.left, 
           m_scrollRect.bottom - m_scrollRect.top, 
           PATINVERT);

    SendMessage(m_hwnd, WM_VSCROLL, MAKELONG(m_scrollCode, 0), NULL);

    // draw new
    PatBlt(hdc,
           m_scrollRect.left, 
           m_scrollRect.top, 
           m_scrollRect.right - m_scrollRect.left, 
           m_scrollRect.bottom - m_scrollRect.top, 
           PATINVERT);

    SelectObject(hdc, oldBrush);

    ReleaseDC(m_hwnd, hdc);

    ImageList_DragShowNolock(TRUE);
}


static void CALLBACK ScrollProc(HWND hwnd, UINT msg, UINT id, DWORD ticks)
{
    g_me->ScrollFunction();
}

void DropTarget::AutoScroll(int scrollCode)
{
    m_scrollCode = (scrollCode == SCROLL_UP ? SB_LINEUP : SB_LINEDOWN);

    if(scrollCode == SCROLL_OFF)
    {
        //OutputDebugString("KillTimer\r\n");
        KillTimer(GetParent(m_hwnd), SCROLL_TIMER);
        m_scrolling = false;
    }
    else if(!m_scrolling)
    {
        m_scrolling = true;
		m_scrollRect = m_insertRect;

        m_timer = SetTimer(GetParent(m_hwnd), SCROLL_TIMER, 250, ScrollProc);
    }
}

//	________________________________________
//
//	IDropTarget Methods

STDMETHODIMP DropTarget::DragEnter(LPDATAOBJECT pDataObj, 
                                   DWORD dwKeyState, 
                                   POINTL pt, 
                                   LPDWORD pdwEffect) 
{  
    FORMATETC fmtetc;

    // default to not allowing drop
    m_acceptFormat = false;
    m_allowMove = false;
    *pdwEffect = DROPEFFECT_NONE;

    if(!m_enabled) 
    {
        return NOERROR;
    }

    /*IEnumFORMATETC* ief;

    if(S_OK == pDataObj->EnumFormatEtc(DATADIR_GET, &ief))
    {
        FORMATETC e;
        ULONG i;

        while (S_OK == ief->Next(1, &e, &i))
        {

            ief->Skip(1);
        }



    }*/

    // Does the drag source provide our private format?     
    fmtetc.cfFormat = RegisterClipboardFormat(CFSTR_FREEAMP_CATALOGITEM);
    fmtetc.ptd      = NULL;
    fmtetc.dwAspect = DVASPECT_CONTENT;  
    fmtetc.lindex   = -1;
    fmtetc.tymed    = TYMED_HGLOBAL; 

    if(pDataObj->QueryGetData(&fmtetc) == NOERROR) 
    {
        m_acceptFormat = true;
        *pdwEffect = DROPEFFECT_COPY;
    }

    fmtetc.cfFormat = RegisterClipboardFormat(CFSTR_FREEAMP_PLAYLISTITEM);
    fmtetc.ptd      = NULL;
    fmtetc.dwAspect = DVASPECT_CONTENT;  
    fmtetc.lindex   = -1;
    fmtetc.tymed    = TYMED_HGLOBAL; 

    if(pDataObj->QueryGetData(&fmtetc) == NOERROR) 
    {
        m_acceptFormat = true;
        m_allowMove = true;
        if(dwKeyState & MK_CONTROL)
            *pdwEffect = DROPEFFECT_COPY;
        else
            *pdwEffect = DROPEFFECT_COPY;
    }

    // Does the drag source provide CF_HDROP?     
    fmtetc.cfFormat = CF_HDROP;
    fmtetc.ptd      = NULL;
    fmtetc.dwAspect = DVASPECT_CONTENT;  
    fmtetc.lindex   = -1;
    fmtetc.tymed    = TYMED_HGLOBAL; 

    if(pDataObj->QueryGetData(&fmtetc) == NOERROR) 
    {
        m_acceptFormat = true;
        *pdwEffect = DROPEFFECT_COPY;
    }

    if(m_acceptFormat)
    {
        // draw a line to indicate insertion point in list
        LV_HITTESTINFO hti;
        RECT itemRect;

        hti.pt.x = pt.x;
        hti.pt.y = pt.y;

        MapWindowPoints(NULL, m_hwnd, (LPPOINT)&hti.pt, 1);

        m_oldItem = ListView_HitTest(m_hwnd, &hti);

        if(m_oldItem < 0)
        {
            itemRect.top = 0;
            itemRect.bottom = 0;
            itemRect.left = 0;
            itemRect.right = 0;
        }
        else
        {   
            int middle;
 
            ListView_GetItemRect(m_hwnd, hti.iItem, &itemRect, LVIR_BOUNDS);

            middle = itemRect.top + (itemRect.bottom - itemRect.top)/2;

            if(pt.y < middle)
            {
                //itemRect.top -= 2;
                itemRect.bottom = itemRect.top;
				itemRect.top -= 2;
            }
            else
            {
                //itemRect.bottom += 2;
                itemRect.top = itemRect.bottom - 2;
            }
        }

        HDC hdc = GetDC(m_hwnd);

        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, m_insertBrush);

        // draw new
        PatBlt(hdc,
               itemRect.left, 
               itemRect.top, 
               itemRect.right - itemRect.left, 
               itemRect.bottom - itemRect.top, 
               PATINVERT);

        SelectObject(hdc, oldBrush);

        ReleaseDC(m_hwnd, hdc);

        m_insertRect = itemRect;
    }
   
	return NOERROR;
}

STDMETHODIMP DropTarget::DragOver(DWORD dwKeyState, 
                                  POINTL pt, 
                                  LPDWORD pdwEffect) 
{
    //OutputDebugString("Drag Over\r\n");

    if(m_acceptFormat && m_enabled) 
    {
        *pdwEffect = DROPEFFECT_COPY; 

        if(m_allowMove && !m_targetIsSource)
        {
            if(dwKeyState & MK_CONTROL)
                *pdwEffect = DROPEFFECT_COPY; 
            else
                *pdwEffect = DROPEFFECT_MOVE; 
        }
    }
	else 
        *pdwEffect = DROPEFFECT_NONE; 

    
    if(m_acceptFormat && m_enabled)
    {
        // draw a line to indicate insertion point in list
        LV_HITTESTINFO hti;
        RECT itemRect;

        hti.pt.x = pt.x;
        hti.pt.y = pt.y;

        MapWindowPoints(NULL, m_hwnd, (LPPOINT)&hti.pt, 1);

        m_oldItem = ListView_HitTest(m_hwnd, &hti);

		bool checkScroll = true;
        if (m_oldItem == 0)
			checkScroll = false;

        if(m_oldItem < 0)
        {
            itemRect = m_insertRect;
        }
        else
        {   
            int middle;

            ListView_GetItemRect(m_hwnd, hti.iItem, &itemRect, LVIR_BOUNDS);

            middle = itemRect.top + (itemRect.bottom - itemRect.top)/2;

            if(hti.pt.y < middle)
            {
                if(m_oldItem == 0)
                {
                    itemRect.bottom = itemRect.top + 2;
					checkScroll = true;
                }
                else
                {
                    itemRect.bottom = itemRect.top;
                    itemRect.top -= 2;
                }
                //OutputDebugString("top\r\n");
            }
            else
            {
                itemRect.top = itemRect.bottom - 2;
                //itemRect.bottom;
                //OutputDebugString("bottom\r\n");
            }
        }

        //char buf[256];
        //sprintf(buf, "%d, %d\r\n", pt.x, pt.y);

        //sprintf(buf, "%d, %d, %d, %d\r\n", 
        //    itemRect.top, itemRect.left, itemRect.bottom, itemRect.right);
        //OutputDebugString(buf);

		bool scrolling = m_scrolling;

        if(!m_scrolling)
        {
            ImageList_DragShowNolock(FALSE);
       
            HDC hdc = GetDC(m_hwnd);

            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, m_insertBrush);

            //OutputDebugString("erase old - drag over\r\n");
            // erase old
            PatBlt(hdc,
                   m_insertRect.left, 
                   m_insertRect.top, 
                   m_insertRect.right - m_insertRect.left, 
                   m_insertRect.bottom - m_insertRect.top, 
                   PATINVERT);

            //OutputDebugString("draw new - drag over\r\n");

            // draw new
            PatBlt(hdc,
                   itemRect.left, 
                   itemRect.top, 
                   itemRect.right - itemRect.left, 
                   itemRect.bottom - itemRect.top, 
                   PATINVERT);

            SelectObject(hdc, oldBrush);

            ReleaseDC(m_hwnd, hdc);

            m_insertRect = itemRect;

            ImageList_DragShowNolock(TRUE);
        }

		if (checkScroll)
            CheckAutoScroll(hti.pt); 
    }

	return NOERROR;
}


STDMETHODIMP DropTarget::DragLeave() 
{   
    if(m_acceptFormat)
    {
        AutoScroll(SCROLL_OFF);

        ImageList_DragShowNolock(FALSE);

        HDC hdc = GetDC(m_hwnd);

        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, m_insertBrush);

        // erase old
        PatBlt(hdc,
               m_insertRect.left, 
               m_insertRect.top, 
               m_insertRect.right - m_insertRect.left, 
               m_insertRect.bottom - m_insertRect.top, 
               PATINVERT);

        // erase old
        SelectObject(hdc, oldBrush);

        ReleaseDC(m_hwnd, hdc);

        ImageList_DragShowNolock(TRUE);
    }

    m_acceptFormat = false;
    m_allowMove = false;

    return NOERROR;
}

STDMETHODIMP DropTarget::Drop(LPDATAOBJECT pDataObj,
                              DWORD dwKeyState, 
                              POINTL pt, 
                              LPDWORD pdwEffect) 
{   
    FORMATETC fmtetc;   
    STGMEDIUM medium;   
    HGLOBAL hGlobal;
    HRESULT hr;
 
    *pdwEffect = DROPEFFECT_NONE;
    hr = NOERROR;

    AutoScroll(SCROLL_OFF);

    if(m_acceptFormat && m_enabled) 
    {   
        if(m_oldItem > 0)
            m_oldItem--;

        //ListView_RedrawItems(m_hwnd, m_oldItem, m_oldItem + 1);
        ImageList_DragShowNolock(FALSE);

        HDC hdc = GetDC(m_hwnd);

        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, m_insertBrush);

        // erase old
        PatBlt(hdc,
               m_insertRect.left, 
               m_insertRect.top, 
               m_insertRect.right - m_insertRect.left, 
               m_insertRect.bottom - m_insertRect.top, 
               PATINVERT);

        // erase old
        SelectObject(hdc, oldBrush);

        ReleaseDC(m_hwnd, hdc);

        AutoScroll(SCROLL_OFF);

        ImageList_DragShowNolock(TRUE);

        // User has dropped on us. First, try getting data in the 
        // private FreeAmp format

        fmtetc.cfFormat = RegisterClipboardFormat(CFSTR_FREEAMP_CATALOGITEM);
        fmtetc.ptd = NULL;
        fmtetc.dwAspect = DVASPECT_CONTENT;  
        fmtetc.lindex = -1;
        fmtetc.tymed = TYMED_HGLOBAL;       
        
        hr = pDataObj->GetData(&fmtetc, &medium);
        if(!FAILED(hr)) 
        {
            // Import the data and release it.
            hGlobal = medium.hGlobal;

            // need to update the point so we know 
            // where it was droppped in the window
            DROPFILES* df = (DROPFILES*)GlobalLock(hGlobal);
            df->pt.x = pt.x;
            df->pt.y = pt.y;

            MapWindowPoints(NULL, m_hwnd, (LPPOINT)&df->pt, 1);

            GlobalUnlock(hGlobal);

            // our format is the same as the DROPFILES format
            // except we pass URLs not paths... 
            SendMessage(m_hwnd, UWM_DROPURLS, (WPARAM)hGlobal, 0);
           
            ReleaseStgMedium(&medium);
            *pdwEffect = DROPEFFECT_COPY;

            return NOERROR;
        }

        fmtetc.cfFormat = RegisterClipboardFormat(CFSTR_FREEAMP_PLAYLISTITEM);
        fmtetc.ptd = NULL;
        fmtetc.dwAspect = DVASPECT_CONTENT;  
        fmtetc.lindex = -1;
        fmtetc.tymed = TYMED_HGLOBAL;       
        
        hr = pDataObj->GetData(&fmtetc, &medium);
        if(!FAILED(hr)) 
        {
            // if target is source we simply do an internal move
            if(m_targetIsSource)
            {
                *pdwEffect = DROPEFFECT_NONE;

                MapWindowPoints(NULL, m_hwnd, (LPPOINT)&pt, 1);
                
                SendMessage(m_hwnd, UWM_MOVEITEMS, 0, (LPARAM)&pt);
            }
            else // we export the items to the target
            {
                // Import the data and release it.
                hGlobal = medium.hGlobal;

                // need to update the point so we know 
                // where it was droppped in the window
                DROPFILES* df = (DROPFILES*)GlobalLock(hGlobal);
                df->pt.x = pt.x;
                df->pt.y = pt.y;

                MapWindowPoints(NULL, m_hwnd, (LPPOINT)&df->pt, 1);

                GlobalUnlock(hGlobal);

                // our format is the same as the DROPFILES format
                // except we pass URLs not paths... 
                SendMessage(m_hwnd, UWM_DROPURLS, (WPARAM)hGlobal, 0);

                if(dwKeyState & MK_CONTROL)
                    *pdwEffect = DROPEFFECT_COPY;
                else
                    *pdwEffect = DROPEFFECT_MOVE;
            }

            ReleaseStgMedium(&medium);

            return NOERROR;
        }
        
        // if not our format, try getting CF_HDROP data from drag source
        fmtetc.cfFormat = CF_HDROP;
        fmtetc.ptd = NULL;
        fmtetc.dwAspect = DVASPECT_CONTENT;  
        fmtetc.lindex = -1;
        fmtetc.tymed = TYMED_HGLOBAL;       
    
        hr = pDataObj->GetData(&fmtetc, &medium);

        if(!FAILED(hr)) 
        {
            // Import the data and release it.
            hGlobal = medium.hGlobal;

            // need to update the point so we know 
            // where it was droppped in the window
            DROPFILES* df = (DROPFILES*)GlobalLock(hGlobal);
            df->pt.x = pt.x;
            df->pt.y = pt.y;

            MapWindowPoints(NULL, m_hwnd, (LPPOINT)&df->pt, 1);

            GlobalUnlock(hGlobal);

            SendMessage(m_hwnd, WM_DROPFILES, (WPARAM)hGlobal, 0);

            ReleaseStgMedium(&medium);
            *pdwEffect = DROPEFFECT_COPY;

            return NOERROR;
        }
    }

	return hr;      
}