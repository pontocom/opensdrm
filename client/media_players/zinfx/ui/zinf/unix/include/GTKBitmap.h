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

   $Id: GTKBitmap.h,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_GTKBITMAP_H__
#define INCLUDED_GTKBITMAP_H__

#include <string>

#include <gdk/gdk.h>
#include <stdio.h>

#include "Bitmap.h"
#include "Types.h"
#include "errors.h"

#define BI_RGB       0
#define BI_RLE8      1
#define BI_RLE4      2
#define BI_BITFIELDS 3

class GTKBitmap : public Bitmap
{
    public:

              GTKBitmap(string &oName);
	      GTKBitmap(int iWidth, int iHeight, const string &oName);
     virtual ~GTKBitmap(void);

     virtual Error LoadBitmapFromDisk(string &oFile);
     virtual bool  IsPosVisible(Pos &oPos);
     virtual Error BlitRect(Bitmap *pSrcBitmap, Rect &oSrcRect, 
                            Rect &oDestRect);
     virtual Error BlitRectMaskBitmap(Bitmap *pSrcBitmap, Rect &oSrcRect, 
                                      Rect &oDestRect);
     virtual Error MaskBlitRect(Bitmap *pSrcBitmap, Rect &oSrcRect,
		                Rect &oDestRect);
     virtual Bitmap *Clone(void);
     virtual Error MakeTransparent(Rect &oRect);

     virtual void GetColor(Pos oPos, Color &oColor);
     virtual void GetSize(Pos &oPos);

     GdkPixmap *GetBitmap() { return m_Bitmap; }
     GdkPixmap *GetMask() { return m_MaskBitmap; }

    protected:

     Error ReadleShort(FILE *file, gushort *ret);
     Error ReadleLong(FILE *file, gulong *ret);

     GdkPixmap *m_Bitmap;
     GdkPixmap *m_MaskBitmap;

     GdkGC *m_GC;
     bool shape_set;
     int  m_width, m_height;

     bool m_cache;
     GdkImage *m_image;
     GdkColormap *m_cmap;
     GdkVisual *m_v;
};

#endif
