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

   $Id: Bitmap.h,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_BITMAP_H__
#define INCLUDED_BITMAP_H__

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <string>

using namespace std;

#include "Types.h"
#include "errors.h"

class Bitmap
{
    public:

              Bitmap(const string &oName);
              Bitmap(const string &oName, int i);
     virtual ~Bitmap(void);

     virtual void    GetName(string &oName);
     virtual void    GetErrorString(string &oName);
     virtual void    SetTransColor(Color &oColor);
     virtual bool    GetTransColor(Color &oColor);
     virtual bool    IsPosVisible(Pos &oPos) = 0;
     virtual Error   LoadBitmapFromDisk(string &oFile) = 0;
     virtual Error   BlitRect(Bitmap *pSrcBitmap, Rect &oSrcRect, 
                              Rect &oDestRect) = 0;
     virtual Error   MaskBlitRect(Bitmap *pSrcBitmap, Rect &oSrcRect, 
                                  Rect &oDestRect) = 0;
     virtual Error   BlitRectMaskBitmap(Bitmap *pSrcBitmap, Rect &oSrcRect, 
                                        Rect &oDestRect) = 0;
     virtual Bitmap *Clone(void) = 0;
     virtual Error   MakeTransparent(Rect &oRect) = 0; 

     virtual void    GetColor(Pos oPos, Color &oColor) = 0;
     virtual void    GetSize(Pos &oPos) = 0;
    protected:

     string  m_oBitmapName, m_oLastError;
     Color   m_oTransColor;
     bool    m_bHasTransColor;
};

#endif
