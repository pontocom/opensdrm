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

   $Id: Control.h,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_CONTROL_H__
#define INCLUDED_CONTROL_H__

#include "config.h"

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <vector>
#include <map>

using namespace std;

#include "mutex.h"
#include "Types.h"
#include "Bitmap.h"
#include "Canvas.h"

enum ControlMessageEnum
{
    CM_Pressed,
    CM_MouseEnter,
    CM_MouseLeave,
    CM_MouseDoubleClick,
    CM_SliderUpdate,
    CM_ValueChanged,
    CM_ChangeWindow,
    CM_WindowEnter,
    CM_WindowLeave,
    CM_TogglePanel,

    CM_LastValue
};

enum ControlTransitionEnum
{
    CT_Show,
    CT_Hide,
    CT_Enable,
    CT_Disable,
    CT_MouseMove,
    CT_MouseEnter,
    CT_MouseLeave,
    CT_MouseLButtonUp,
    CT_MouseLButtonDown,
    CT_MouseLButtonDoubleClick,
    CT_SetValue,
    CT_Timer,

    CT_None,

    CT_LastTransition
};

enum ControlStateEnum
{
    CS_Normal,
    CS_MouseOver,
    CS_Pressed,
    CS_Disabled,
    CS_DisabledMO,
    CS_Dragging,
    CS_Hidden,
    CS_HiddenMO,

    CS_Any,			 // These states allow a transition message to be
    CS_Same,		 // sent to the control without actually changing state.

    CS_LastState
};

class TransitionInfo
{
    public:

       ControlStateEnum      eState;
       ControlTransitionEnum eAction;
       ControlStateEnum      eNextState;
};

// I HATE MICROSOFT   I HATE MICROSOFT   I HATE MICROSOFT   I HATE MICROSOFT
// These two functions are not necessary under gcc 2.95, but MSVC++ won't compile
// without them. BILL GATES MUST DIE!
bool operator<(const TransitionInfo &A, const TransitionInfo &b);
bool operator==(const TransitionInfo &A, const TransitionInfo &b);
// I HATE MICROSOFT   I HATE MICROSOFT   I HATE MICROSOFT   I HATE MICROSOFT

class Window;
class Control
{
    public:

               Control(Window *pParent, string &oName, 
                       TransitionInfo *pTransitions);
      virtual ~Control(void);
 
      virtual void  SetParent(Window *pParent);
      virtual void  SetPanel(Panel *pPanel) { m_pPanel = pPanel; };
      virtual Panel *GetPanel(void) { return m_pPanel; };
      virtual void  SetRect(Rect &oRect);
      virtual void  GetRect(Rect &oRect);
      virtual void  SetPos(Pos &oPos);
      virtual void  SetBitmap(Bitmap *pBitmap, Rect &oBitmapRect, 
                              bool bHoriz = true);
      virtual void  SetStateBitmap(Bitmap *pBitmap, Rect &oBitmapRect,
                                   ControlStateEnum eClickState,
                                   int iState = 0);
      virtual Bitmap *GetBitmap(void);
      virtual void  GetName(string &oName);
      virtual void  SetDesc(const string &oDesc);
      virtual void  SetTip(const string &oTip);
      virtual void  GetDesc(string &oDesc);
      virtual void  GetTip(string &oTip);
 
      virtual Error Show(bool bSet, bool &bShow);
      virtual Error Enable(bool bHide, bool &bEnable);
      virtual Error IntValue(bool bSet, int &iValue);
      virtual Error StringValue(bool bSet, string &oValue);

      virtual bool PosInControl(Pos &oPos);
      virtual bool WantsTimingMessages(void);
      virtual bool UseToDragWindow(void) { return false; };
      virtual void Init(void) = 0;
      virtual void Move(Pos &oPos);
      virtual void AcceptTransition(ControlTransitionEnum eTrans,
                                    Pos *pMousePos = NULL);

    protected:

      virtual void Transition(ControlTransitionEnum eTrans,
                              Pos *pMousePos = NULL) = 0;

      // Call this function to hide the control. It causes the
      // area occupied by the control to be painted with the
      // background bitmap
      virtual void Erase(void);

      // Call this function to blit the face of the control
      // Args: iFrame: Which frame to blit
      //       iNumFrames: how many frames in the x direction in the bitmap
      virtual void BlitFrame(ControlStateEnum eFrame, int iState = 0,
                             Rect *pRect = NULL, bool bUpdate = true);
      virtual void BlitFrame(int iFrame, int iNumFrames, Rect *pRect = NULL, 
                             bool bUpdate = true); 
      virtual void BlitFrameHoriz(int iFrame, int iNumFrames,Rect *pRect = NULL, 
                                  bool bUpdate = true);
      virtual void BlitFrameVert(int iFrame, int iNumFrames, Rect *pRect = NULL,
                                 bool bUpdate = true);

      string                 m_oName, m_oDesc, m_oToolTip;
      Rect                   m_oRect, m_oBitmapRect, m_oOrigRect;
      Bitmap                *m_pBitmap;
      ControlStateEnum       m_eCurrentState, m_eLastState;
      vector<TransitionInfo> m_oTransitions;
      Window                *m_pParent;
      Panel                 *m_pPanel;
      bool                   m_bShow, m_bEnable, m_bWantsTimingMessages;
      bool                   m_bHorizontalBitmap;
      int                    m_iValue;
      string                 m_oValue;
      Mutex                  m_oMutex;
      bool                   m_bUsesStateBitmapRects;

      // this is cute
      vector<map<ControlStateEnum, Rect> > m_oStateBitmapRect;
      vector<map<ControlStateEnum, Bitmap *> > m_oStateBitmaps;
};

#endif
