/*____________________________________________________________________________

   FreeAmp - The Free MP3 Player

   Copyright (C) 1999-2000 EMusic

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

   $Id: GTKCanvas.cpp,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/ 

#include "GTKCanvas.h"
#include "GTKFont.h"

GTKCanvas::GTKCanvas(GTKWindow *pParent)
{
    m_pParent = pParent;
    m_pBufferBitmap = NULL;
}

GTKCanvas::~GTKCanvas(void)
{
    delete m_pBufferBitmap;
}

void GTKCanvas::Init(void)
{
    Rect oDestRect;
    string oName("DoubleBuffer");

    assert(m_pBGBitmap);

    if (m_pBufferBitmap)
        delete m_pBufferBitmap;

    m_pBufferBitmap = new GTKBitmap(m_oBGRect.Width(), 
                                    m_oBGRect.Height(),
                                    oName);

    oDestRect.x1 = oDestRect.y1 = 0;
    oDestRect.x2 = m_oBGRect.Width();
    oDestRect.y2 = m_oBGRect.Height();
    m_pBufferBitmap->MaskBlitRect(m_pBGBitmap, m_oBGRect, oDestRect);
}

int GTKCanvas::RenderText(int iFontHeight, Rect &oClipRect,
                          string &oText, AlignEnum eAlign, 
                          Font *pFont, const Color &oColor,
                          bool bBold, bool bItalic, bool bUnderline)
{
    GTKFont *ourFont = (GTKFont *)pFont;
    Error err = ourFont->Load(iFontHeight, bBold, bItalic);
    if (IsError(err)) 
        return 0;

    int width = ourFont->GetLength(oText);
    int offset = 0;

    if (eAlign == eLeft)
        offset = 0;
    else if (eAlign == eRight)
        offset = oClipRect.Width() - width;
    else  
        offset = (oClipRect.Width() - width) / 2;
   
    ourFont->Render(oClipRect, oText, offset, oColor, m_pBufferBitmap,
                    bUnderline);

    Update();
    return MAX(0, width - oClipRect.Width());
}

int GTKCanvas::RenderOffsetText(int iFontHeight, Rect &oClipRect,
                                string &oText, int iOffset,
                                Font *pFont, const Color &oColor,
                                bool bBold, bool bItalic, bool bUnderline)
{
    GTKFont *ourFont = (GTKFont *)pFont;
    Error err = ourFont->Load(iFontHeight, bBold, bItalic);
    if (IsError(err))
        return 0;

    int width = ourFont->GetLength(oText);
   
    width += iMarqueeSpacer;
    if (iOffset > width)
        return width - iOffset;
 
    ourFont->Render(oClipRect, oText, 0 - iOffset, oColor, m_pBufferBitmap,
                    bUnderline);

    int iRet = width - iOffset - oClipRect.Width();
    if (iRet < 0)
        ourFont->Render(oClipRect, oText, width - iOffset, oColor, 
                        m_pBufferBitmap, bUnderline);

    Update();
    return MAX(iRet, 0);
}

Error GTKCanvas::Invalidate(Rect &oRect)
{
   return kError_NoErr;
}

Error GTKCanvas::Update(void)
{
   if (m_bNoScreenUpdate)
       return kError_NoErr;
   
   Rect foo;  
   Paint(foo);

   return kError_NoErr;
}

Error GTKCanvas::BlitRect(Bitmap *pSrcBitmap, Rect &oSrcRect, Rect &oDestRec)
{
   if (!m_pBufferBitmap)
       return kError_NoErr;

   return m_pBufferBitmap->BlitRect(pSrcBitmap, oSrcRect, oDestRec);
}

Error GTKCanvas::MaskBlitRect(Bitmap *pSrcBitmap, Rect &oSrcRect, Rect &oDestRec)
{
   if (!m_pBufferBitmap)
       return kError_NoErr;

   return m_pBufferBitmap->MaskBlitRect(pSrcBitmap, oSrcRect, oDestRec);
}

void GTKCanvas::Paint(Rect &oRect)
{
    assert(m_pParent);
    GtkWidget *w = m_pParent->GetWindow();
    if (!w->window || !m_pBufferBitmap)
        return;
    gdk_threads_enter();
    gdk_window_set_back_pixmap(w->window, m_pBufferBitmap->GetBitmap(), 0);
    gdk_window_clear(w->window);
    gdk_flush();
    gdk_threads_leave();
}

void GTKCanvas::Erase(Rect &oRect)
{
    Rect newRect = oRect;
    newRect.y2++;
    newRect.x2++;
    if (m_pBufferBitmap)
        m_pBufferBitmap->MaskBlitRect(m_pBGBitmap, newRect, newRect);
}

void GTKCanvas::InitBackgrounds(vector<Panel *> *pPanels)
{
   Canvas::InitBackgrounds(pPanels);

   //if (m_pBufferBitmap)
   //   m_pBufferBitmap->MaskBlitRect(m_pBGBitmap, m_oBGRect, m_oBGRect);
}  
