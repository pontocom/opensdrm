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

   $Id: PreferenceWindow.h,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_PREFERENCEWINDOW_H__
#define INCLUDED_PREFERENCEWINDOW_H__

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "facontext.h"
#include "Window.h"
#include "ThemeManager.h"
#include "debug.h"

class PreferenceWindow
{
    public:

               PreferenceWindow(FAContext *context, 
                                ThemeManager *pThemeMan)
                 { 
                     m_pContext = context; 
                     m_pThemeMan = pThemeMan;
                 };
      virtual ~PreferenceWindow(void) 
                 { ; };
      
      virtual  bool Show(Window *pParent) = 0;

	protected:
    
      FAContext    *m_pContext;
      ThemeManager *m_pThemeMan;
};

#endif
