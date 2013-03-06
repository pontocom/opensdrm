/*____________________________________________________________________________

   FreeAmp - The Free MP3 Player

   Copyright (C) 1999 EMusic

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

   $Id: Canvas.cpp,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#define STRICT
#endif


#include <algorithm>
#include "Canvas.h"
#include "Window.h"

#ifdef WIN32
#include "Win32Bitmap.h"
#elif defined(HAVE_GTK)
#include "GTKBitmap.h"
#elif defined(__BEOS__)
#include "BeOSBitmap.h"
#endif  

#include "debug.h"

#define DB Debug_v("%s:%d\n", __FILE__, __LINE__);

Canvas::Canvas(void)
{
    m_pBGBitmap = NULL;
    m_pMaskBitmap = NULL;
    m_bDeleteBitmap = false;
    m_bNoScreenUpdate = false;
}

Canvas::~Canvas(void)
{
	// The bitmap that was loaded by the theme will be destroyed by the theme
	// However, if we created any clones, we need to kill those...
	if (m_bDeleteBitmap)
		delete m_pBGBitmap;
}

void Canvas::SetBackgroundRect(Rect &oRect)
{
    m_oBGRect = oRect;
}

void Canvas::SetBackgroundBitmap(Bitmap *pBitmap)
{
    delete m_pBGBitmap;
    m_pBGBitmap = pBitmap;
}

void Canvas::SetMaskBitmap(Bitmap *pBitmap)
{
    delete m_pMaskBitmap;
    m_pMaskBitmap = pBitmap;
}

void Canvas::GetBackgroundRect(Rect &oRect)
{
    oRect = m_oBGRect;
}

Bitmap *Canvas::GetBackgroundBitmap(void)
{
    return m_pBGBitmap;
}

class PanelCompare
{
    public:

      bool operator()(Panel *a, Panel *b)
      {
          if (a->GetZOrder() > b->GetZOrder())
             return true;
          return false;
      }
}; 

void Canvas::InitBackgrounds(vector<Panel *> *pPanels)
{
    vector<Panel *>::iterator i;
    Rect                      oDestRect, oSrcRect, oUnion, oTemp;
    Pos                       oOffset, oBitmapPos, oTempPos;
    int                       iLoop;
    Color                     oColor;

    // If the bitmap we have came from the theme parser and we're
    // not supposed to delete it, we've got a normal, non-panel theme
    // and we should not execute this function
    if (m_bDeleteBitmap == false && m_pBGBitmap)
       return;

	 if (m_pBGBitmap)
	   delete m_pBGBitmap;

    // Calculate the bounding rectangle of all the panels
    for(i = pPanels->begin(), iLoop = 0; i != pPanels->end(); i++, iLoop++)
    {
        if ((*i)->IsHidden())
        {
            iLoop--;
            continue;
        }

        string j;
        (*i)->GetName(j);

        (*i)->GetRect(oTemp);
        (*i)->GetPos(oOffset);
        oTemp.x2 = oOffset.x + oTemp.Width();
        oTemp.y2 = oOffset.y + oTemp.Height();
        oTemp.x1 = oOffset.x;
        oTemp.y1 = oOffset.y;

        if (iLoop == 0)
           oUnion = oTemp;
        else
           oUnion.Union(oTemp);

    }
     
    oOffset.x = oUnion.x1;
    oOffset.y = oUnion.y1;

    // If the bounding rectangle does not start at the origin, move 
    // all controls relative to the origin
    if (oOffset.x != 0 && oOffset.y != 0)
    {
        oUnion.x1 -= oOffset.x;
        oUnion.y1 -= oOffset.y;
        oUnion.x2 -= oOffset.x;
        oUnion.y2 -= oOffset.y;
    }
    m_bDeleteBitmap = true;

#ifdef WIN32
    m_pBGBitmap = new Win32Bitmap(oUnion.Width(), oUnion.Height(), 
                              string("Background"));
#elif defined(HAVE_GTK)
    m_pBGBitmap = new GTKBitmap(oUnion.Width(), oUnion.Height(), 
                            string("Background"));
#elif defined(__BEOS__)
    m_pBGBitmap = new BeOSBitmap(oUnion.Width(), oUnion.Height(), 
                             string("Background"));
#endif 
    (*(pPanels->begin()))->GetPanelBitmap()->GetTransColor(oColor);
    m_pBGBitmap->SetTransColor(oColor);
    m_pBGBitmap->MakeTransparent(oUnion);
    SetBackgroundRect(oUnion);

    // Sort the panels in ascending Zorder
    sort(pPanels->begin(), pPanels->end(), PanelCompare());

    // And now blit the panel bitmaps to the background bitmap
    for(i = pPanels->end() - 1; i >= pPanels->begin(); i--)
    {
        if ((*i)->IsHidden())
        {
            continue;
        }

        (*i)->GetRect(oSrcRect);
        (*i)->GetPos(oBitmapPos);

        oTempPos.x = oOffset.x + oBitmapPos.x;
        oTempPos.y = oOffset.y + oBitmapPos.y;
        (*i)->MoveControls(oTempPos);

        oDestRect.x1 = oBitmapPos.x - oOffset.x; 
        oDestRect.y1 = oBitmapPos.y - oOffset.y; 
        oDestRect.x2 = oDestRect.x1 + oSrcRect.Width();
        oDestRect.y2 = oDestRect.y1 + oSrcRect.Height();
        m_pBGBitmap->BlitRect((*i)->GetPanelBitmap(), oSrcRect, oDestRect);
        m_pBGBitmap->BlitRectMaskBitmap((*i)->GetPanelBitmap(), 
                                        oSrcRect, oDestRect);
    }
}
