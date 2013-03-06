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

   $Id: Canvas.h,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_CANVAS_H__
#define INCLUDED_CANVAS_H__

#include "Types.h"
#include "Bitmap.h"
#include "Font.h"

const int iMarqueeSpacer = 20;	 // Number of pixels of space to insert
                                 // between wrappings of the same text.
const int iMarqueeDelay = 250;   // Number of milliseconds between text
                                 // marquee frames.
enum AlignEnum
{
    eLeft,
    eCenter,
    eRight
};

// This class must keep an internal bitmap of the current display image
// and then paint from that image in response to paint events.
class Panel;
class Canvas
{
    public:

              Canvas(void);
     virtual ~Canvas(void);

     void SetBackgroundRect(Rect &oRect);
     void GetBackgroundRect(Rect &oRect);
     void SetBackgroundBitmap(Bitmap *pBitmap);
     void SetMaskBitmap(Bitmap *pBitmap);
     Bitmap *GetBackgroundBitmap(void);
     virtual void InitBackgrounds(vector<Panel *> *pPanels);
     void SetNoScreenUpdate(bool bUpdate) { m_bNoScreenUpdate = bUpdate; };
     bool GetNoScreenUpdate(void) { return m_bNoScreenUpdate; };

     virtual void  Init(void) = 0;
     virtual void  Erase(Rect &oPaintRect) = 0;
     virtual int   RenderText(int iFontHeight, Rect &oClipRect,
                              string &oText, AlignEnum eAlign,
                              Font *pFont, const Color &oColor,
                              bool bBold, bool bItalic, bool bUnderline) = 0;
     virtual int   RenderOffsetText(int iFontHeight, Rect &oClipRect, 
                                    string &oText, int iOffset,
                                    Font *pFont, const Color &oColor,
                                    bool bBold, bool bItalic, bool bUnderline) = 0;
     virtual Error Invalidate(Rect &oRect) = 0;
     virtual Error Update(void) = 0;
     virtual Error BlitRect(Bitmap *pSrcBitmap, Rect &oSrcRect, Rect &oDestRec) = 0;
     virtual Error MaskBlitRect(Bitmap *pSrcBitmap, Rect &oSrcRect, Rect &oDestRec) = 0;

    protected:

     Bitmap *m_pBGBitmap, *m_pMaskBitmap;
     Rect    m_oBGRect;
     string  m_oDefaultFont;
     bool    m_bDeleteBitmap, m_bNoScreenUpdate;
};

#endif
