/*____________________________________________________________________________

   FreeAmp - The Free MP3 Player

   Portions Copyright (C) 1999 EMusic

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

   $Id: TextControl.h,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_TEXTCONTROL_H__
#define INCLUDED_TEXTCONTROL_H__

#include "Control.h"
#include "Canvas.h"
#include "Font.h"


class TextControl : public Control
{
    public:

               TextControl(Window *pWindow, string &oName); 
      virtual ~TextControl(void);

	  virtual void SetStyle(Font *pFont, string &oAlign, 
                            Color &oColor, bool bBold,
                            bool bItalic, bool bUnderline);
      virtual bool StyleHasBeenSet(void);                      

      void Transition(ControlTransitionEnum eTrans, Pos *pMousePos);
      virtual void Init(void);
      virtual bool UseToDragWindow(void);

    private:

      void       TextChanged(void);
      void       MarqueeText(void);
      
      AlignEnum	 m_eAlign;
      Color      m_oColor;
      string     m_oFont;
      bool       m_bBold, m_bItalic, m_bUnderline;
      Font      *m_pFont;
      int        m_iMarqueePos;
      bool       m_bStyleSet;
};

#endif
