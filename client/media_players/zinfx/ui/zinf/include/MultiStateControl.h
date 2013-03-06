/*____________________________________________________________________________

   FreeAmp - The Free MP3 Player

   Portions Copyright (C) 1999-2000 EMusic

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

   $Id: MultiStateControl.h,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_MULTISTATECONTROL_H__
#define INCLUDED_MULTISTATECONTROL_H__

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <string>

using namespace std;

#include "Control.h"

class MultiStateControl : public Control
{
    public:

               MultiStateControl(Window *pParent, string &oName,
                                 int iNumStates);
      virtual ~MultiStateControl(void);

      void Transition(ControlTransitionEnum eTrans, Pos *pMousePos);
      virtual void Init(void);
      virtual bool PosInControl(Pos &oPos);
      virtual void GetDesc(string &oDesc);
      virtual void GetTip(string &oTip);

    private:

      int            m_iState, m_iNumStates;
      vector<string> m_oTips, m_oDescs;   
};

#endif
