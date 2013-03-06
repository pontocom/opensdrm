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

   $Id: Panel.h,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_PANEL__H_
#define INCLUDED_PANEL__H_

#include "config.h"

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <string> 
#include <vector>
#include <map>
#include <utility>

using namespace std;

#include "Control.h"
#include "Canvas.h"
#include "thread.h"
#include "semaphore.h"
#include "mutex.h"

typedef multimap<string, Control *> ControlMap;
typedef multimap<string, Control *>::iterator ControlMapIterator;

class Theme;
class Panel
{
    public:
    
              Panel(const string &oName);
     virtual ~Panel();

      bool    IsOpen(void) { return m_bIsOpen; };
      void    OpenPanel(bool bOpen) { m_bIsOpen = bOpen; };
      bool    IsHidden(void) { return m_bIsHidden; };
      void    Hide(bool bHidden) { m_bIsHidden = bHidden; };
      void    AddControl(Control *pControl);
      void    ClearControls(void);
      void    Init(void);
      void    ShowAllControls(void);
      void    GetRect(Rect &oRect) { oRect = m_oRect; };
      void    SetRect(Rect &oRect) { m_oRect = oRect; };
      void    GetName(string &oName);
      void    GetPos(Pos &oPos) { oPos = m_oPos; };
      void    SetPos(Pos &oPos) { m_oPos = oPos; };
      void    GetTogglePos(Pos &oPos) { oPos = m_oTogglePos; };
      void    SetTogglePos(Pos &oPos) { m_oTogglePos = oPos; };
      int     GetZOrder(void) { return m_iZOrder; };
      void    SetZOrder(int i) { m_iZOrder = i; };
      void    SetPanelBitmap(Bitmap *pBitmap) { m_pPanelBitmap = pBitmap; };
      Bitmap *GetPanelBitmap(void) { return m_pPanelBitmap; };
      void    TogglePanelPos(void);
      void    SetParentWindow(Window *pWin);
      Window *GetParentWindow(void) { return m_pWindow; };

      void    SetOnOpenHide(Panel *pPanel);
      void    SetOnOpenShow(Panel *pPanel);
      void    SetOnCloseHide(Panel *pPanel);
      void    SetOnCloseShow(Panel *pPanel);

      Control *ControlFromPos(Pos &oPos);
      void    GetControlToolTips(vector<pair<Rect, string> > &oList);
 
      Error   ControlEnable(const string &oControlName, bool bSet, bool &bEnable);
      Error   ControlShow(const string &oControlName, bool bSet, bool &bShow);
      Error   ControlIntValue(const string &oControlName, bool bSet, 
                             int &iValue);
      Error   ControlStringValue(const string &oControlName, bool bSet,
                                 string &oValue);
      Error   ControlGetTip(const string &oName, string &oTip);
      Error   ControlGetDesc(const string &oName, string &oDesc);
      bool    DoesControlExist(const string &oName);

      void    TimerEvent(void);
      void    MoveControls(Pos &oPos);
      bool    operator<(const Panel *&a); 

    protected:

      string              m_oName;
      Pos                 m_oPos, m_oTogglePos;
      Rect                m_oRect;
      Bitmap             *m_pPanelBitmap;
      bool                m_bIsOpen, m_bIsHidden;
      vector<Control *>   m_oControls;
      ControlMap          m_oControlMap;
      Window             *m_pWindow;
      int                 m_iZOrder;

      Panel *m_pOnOpenHide, *m_pOnOpenShow, *m_pOnCloseHide, *m_pOnCloseShow;
};

#endif
