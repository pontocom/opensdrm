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

   $Id: GTKCanvas.h,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_GTKCANVAS_H__
#define INCLUDED_GTKCANVAS_H__

#include "Types.h"
#include "Bitmap.h"
#include "Canvas.h"
#include "GTKWindow.h"
#include "GTKBitmap.h"

class GTKCanvas : public Canvas
{
    public:

              GTKCanvas(GTKWindow *pParent);
     virtual ~GTKCanvas(void);

     virtual void  Init(void);
     virtual int   RenderText(int iFontHeight, Rect &oClipRect,
                              string &oText, AlignEnum eAlign,
			      Font *pFont, const Color &oColor,
			      bool bBold, bool bItalic, bool bUnderline);
     virtual int   RenderOffsetText(int iFontHeight, Rect &oClipRect,
		              string &oText, int iOffset,
			      Font *pFont, const Color &oColor,
			      bool bBold, bool bItalic, bool bUnderline);
     virtual Error Invalidate(Rect &oRect);
     virtual Error Update(void);
     virtual Error BlitRect(Bitmap *pSrcBitmap, Rect &oSrcRect, Rect &oDestRec);
     virtual Error MaskBlitRect(Bitmap *pSrcBitmap, Rect &oSrcRect, Rect &oDestRec);
     virtual void  Paint(Rect &oRect);
     virtual void  Erase(Rect &oRect);

     GdkPixmap *GetMask() { return ((GTKBitmap *)m_pBGBitmap)->GetMask(); }
     void SetParent(GTKWindow *newParent) { m_pParent = newParent; }
     void InitBackgrounds(vector<Panel *> *pPanels);

    protected:
     GTKBitmap *m_pBufferBitmap;
     GTKWindow *m_pParent;
};

#endif
