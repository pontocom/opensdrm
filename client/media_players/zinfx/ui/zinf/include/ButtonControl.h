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

   $Id: ButtonControl.h,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_BUTTONCONTROL_H__
#define INCLUDED_BUTTONCONTROL_H__

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <string>

using namespace std;

#include "Control.h"

class Panel;
class ButtonControl : public Control
{
    public:

               ButtonControl(Window *pParent, string &oName);
               ButtonControl(Window *pParent, string &oName, string &oUrl);
      virtual ~ButtonControl(void);

      virtual void SetTargetWindow(string &oWindow);
      virtual void SetPanelToggle(Panel *pPanel);

      void Transition(ControlTransitionEnum eTrans, Pos *pMousePos);
      virtual void Init(void);
      virtual bool PosInControl(Pos &oPos);
      virtual bool UseToDragWindow(void);
      
    private:

	  string  m_oTargetWindow;
     Panel  *m_pPanelToToggle;
     Pos     m_oPos;
};

#endif
