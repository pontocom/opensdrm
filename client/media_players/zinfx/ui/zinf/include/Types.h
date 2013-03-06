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

   $Id: Types.h,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_TYPES_H__
#define INCLUDED_TYPES_H__

#include <string.h>

struct Pos
{
    int x, y;

    Pos(void) { };
    Pos(const Pos &other) 
    {
        x = other.x;
        y = other.y;
    }
};

struct Rect
{
    int x1, y1;
    int x2, y2;

    Rect(void) { };
    Rect(const Rect &other)
    {
         x1 = other.x1;
         x2 = other.x2;
         y1 = other.y1;
         y2 = other.y2;
    };
    int Width(void)
    {
         return x2 - x1;
    };
    int Height(void)
    {
         return y2 - y1;
    }
    void Union(Rect &oOther)
    {
         x1 = (oOther.x1 < x1) ? oOther.x1 : x1;
         y1 = (oOther.y1 < y1) ? oOther.y1 : y1;
         x2 = (oOther.x2 > x2) ? oOther.x2 : x2;
         y2 = (oOther.y2 > y2) ? oOther.y2 : y2;
    }
    bool IsPosInRect(Pos &oPos)
    {
         if (oPos.x >= x1 && oPos.x <= x2 &&
             oPos.y >= y1 && oPos.y <= y2)
             return true;

         return false;
    }
};

struct Color
{
    Color(void)
    {
        red = blue = green = 0;
    };
    Color(unsigned char red, unsigned char green, unsigned char blue)
    { 
        this->red = red;
        this->green = green;
        this->blue = blue;
    };    
    bool IsEqual(Color &oOther)
    {
        return !memcmp(&red, &oOther.red, sizeof(unsigned char) * 3);
    }    
    unsigned char red, green, blue;
};


#endif
