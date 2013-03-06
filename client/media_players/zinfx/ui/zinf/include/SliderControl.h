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

   $Id: SliderControl.h,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_SLIDERCONTROL_H__
#define INCLUDED_SLIDERCONTROL_H__

#include "Control.h"

class SliderControl : public Control
{
    public:

               SliderControl(Window *pWindow, string &oName, int iThumbs,
                             int iNumFrames,  int iNotchPercent = -1, 
                             int iNotchWidth = -1);
      virtual ~SliderControl(void);

              void Transition(ControlTransitionEnum eTrans, Pos *pMousePos);
      virtual void Init(void);
              void HandleDrag(ControlTransitionEnum  eTrans,
                              Pos                   *pPos);
              void HandleJump(ControlTransitionEnum  eTrans,
                              Pos                   *pPos);
              void SetTroughBitmap(Bitmap *pBitmap, Rect &oBitmapRect,
                                   int iFrames, bool bHoriz, int iDelta,
                                   bool bMiddle);

    private:

              void MoveThumb(int iNewPos);
              void BlitTrough(int iPos);
					  
      Pos     m_oOrigin;
      int     m_iRange, m_iNotchPercent, m_iNotchWidth;
      int     m_iCurrentPos;
      int     m_iThumbWidth;
      bool    m_bIsDrag, m_bInUpdate;
      int     m_iNumThumbStates, m_iNumFrames;
      bool    m_bHasTroughBitmap, m_bHorizontalTroughBitmap;
      bool    m_bTroughMiddle;
      int     m_iTroughFrames, m_iCurrentTroughFrame, m_iTroughDelta;
      Rect    m_oTroughBitmapRect;
      Bitmap *m_pTroughBitmap;
};

#endif
