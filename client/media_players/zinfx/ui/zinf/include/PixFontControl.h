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

   $Id: PixFontControl.h,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_PIXFONTCONTROL_H__
#define INCLUDED_PIXFONTCONTROL_H__

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <vector>
using namespace std;

#include "Control.h"
#include "Canvas.h"
#include "errors.h"

typedef struct {
   int row;
   int col;
} PixFontLocation;

class PixFontControl : public Control
{
    public:

               PixFontControl(Window *pWindow, string &oName); 
      virtual ~PixFontControl(void);
      
      void Transition(ControlTransitionEnum eTrans, Pos *pMousePos);
      virtual void Init(void);
      virtual bool UseToDragWindow(void);

      void AddMapRow(char *row);
      void SetDefaultLetter(char new_default) { m_Default = new_default; }
      void SetIgnoreCase(bool setting) { m_bNoCase = setting; }

    private:

      void BlitLetter(char letter, Rect oDestRect);
      int  BlitString(string &oText, int iOffset);
      void TextChanged(void);
      void MarqueeText(void);

      vector<char *>  m_Fontmap;
      uint32          m_NumRows;
      int             m_LetterWidth, m_LetterHeight;
      bool            m_bNoCase;
      char            m_Default;
      PixFontLocation m_lettermap[256];
      int             m_iMarqueePos;
};

#endif
