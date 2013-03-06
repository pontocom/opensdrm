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

   $Id: ttfont.h,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_TTFONT__H_
#define INCLUDED_TTFONT__H_

#include "config.h"
#include "mutex.h"

#include <gdk/gdk.h>
#ifdef HAVE_FREETYPE_FREETYPE_H
#include <freetype/freetype.h>
#else
#include <freetype.h>
#endif

typedef struct _efont
  {
     TT_Engine           engine;
     TT_Face             face;
     TT_Instance         instance;
     TT_Face_Properties  properties;
     int                 num_glyph;
     TT_Glyph           *glyphs;
     TT_Raster_Map     **glyphs_cached;
     int                 max_descent;
     int                 max_ascent;
     int                 ascent;
     int                 descent;
  }
Efont;

void EFont_draw_string(GdkPixmap *win, GdkGC *gc, int x, int y, char *text,
                       Efont *font);
void Efont_free(Efont * f);
Efont *Efont_load(char *file, int size);
void Efont_extents(Efont * f, char *text, int *font_ascent_return,
                   int *font_descent_return, int *width_return,
                   int *max_ascent_return, int *max_descent_return,
                   int *lbearing_return, int *rbearing_return);

#endif
