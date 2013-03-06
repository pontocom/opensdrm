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

   $Id: PixSliderControl.cpp,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/ 

#include "stdio.h"
#include "PixSliderControl.h"
#include "Window.h"
#include "debug.h"

static TransitionInfo pTransitions[] =
{
    { CS_Normal,     CT_MouseEnter,       CS_MouseOver  },
    { CS_Normal,     CT_Disable,          CS_Disabled   },
    { CS_Normal,     CT_Hide,             CS_Hidden     },
    { CS_Normal,     CT_Show,             CS_Normal     },
    { CS_MouseOver,  CT_MouseLeave,       CS_Normal     },
    { CS_MouseOver,  CT_MouseLButtonDown, CS_Pressed    },
    { CS_MouseOver,  CT_Disable,          CS_Disabled   },
    { CS_Pressed,    CT_Disable,          CS_Disabled   },
    { CS_Pressed,    CT_MouseMove,        CS_Pressed    }, 
    { CS_Pressed,    CT_MouseLButtonUp,   CS_MouseOver  },
    { CS_Pressed,    CT_MouseLeave,       CS_Normal     },
    { CS_Disabled,   CT_Show,             CS_Disabled   },
    { CS_Disabled,   CT_Enable,           CS_Normal     },
    { CS_Disabled ,  CT_MouseEnter,       CS_DisabledMO },
    { CS_DisabledMO, CT_MouseLeave,       CS_Disabled   },
    { CS_Hidden,     CT_Show,             CS_Normal     },
    { CS_Any,        CT_SetValue,         CS_Same       },
    { CS_LastState,  CT_LastTransition,   CS_LastState  }
};

PixSliderControl::PixSliderControl(Window *pWindow, 
                                   string &oName, 
                                   int iNumStates,
                                   bool bActivationReveal) :
                  Control(pWindow, oName, pTransitions)
{
    m_iState = 0;
    m_iNumStates = iNumStates;
    m_bHasActivationBitmap = false;
    m_bActivationReveal = bActivationReveal;
}

PixSliderControl::~PixSliderControl(void)
{
}

void PixSliderControl::SetActivationBitmap(Bitmap *pBitmap, Rect oRect,
                                           bool bSmooth)
{
    m_bSmooth = bSmooth;
    m_pActivationBitmap = pBitmap;
    m_bHasActivationBitmap = true;
    m_oActivationRect = oRect;
}

void PixSliderControl::Init(void)
{
    m_oMutex.Acquire();

    if (!m_bUsesStateBitmapRects) {
        for (int row = 0; row < m_iNumStates; row++) { 
            for (int i = 0; i < 4; i++) {
                int iFrameWidth, iFrameHeight;
                Rect oFrameRect;
    
                iFrameWidth = (m_oBitmapRect.Width() + 1) / 4;
                iFrameHeight = (m_oBitmapRect.Height() + 1) / m_iNumStates;
     
                oFrameRect = m_oBitmapRect;
                oFrameRect.x1 += i * iFrameWidth;
                oFrameRect.x2 = oFrameRect.x1 + iFrameWidth - 1;
                oFrameRect.y1 += row * iFrameHeight;
                oFrameRect.y2 = oFrameRect.y1 + iFrameHeight - 1;

                switch (i) {
                    case 0:
                        SetStateBitmap(m_pBitmap, oFrameRect, CS_Normal, row);
                        break;
                    case 1:
                        SetStateBitmap(m_pBitmap, oFrameRect, CS_MouseOver, row);
                        break;
                    case 2:
                        SetStateBitmap(m_pBitmap, oFrameRect, CS_Pressed, row);
                        break;
                    case 3:
                        SetStateBitmap(m_pBitmap, oFrameRect, CS_Disabled, row);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    if (m_oRect.x2 == -1 && m_oRect.y2 == -1)
    {
        m_oRect.x2 = m_oRect.x1 + m_oStateBitmapRect[0][CS_Normal].x2 -
                     m_oStateBitmapRect[0][CS_Normal].x1; 
        m_oRect.y2 = m_oRect.y1 + m_oStateBitmapRect[0][CS_Normal].y2 -
                     m_oStateBitmapRect[0][CS_Normal].y1; 
    }    
    m_oMutex.Release();
    
    if (!m_bActivationReveal) 
        BlitFrame(CS_Normal, m_iState);
}

void PixSliderControl::Transition(ControlTransitionEnum  eTrans,
                                  Pos                   *pMousePos)
{
    Canvas *pCanvas;
    
    if (m_eCurrentState == CS_Pressed && eTrans == CT_SetValue)
        return;

    switch(eTrans)
    {
       case CT_MouseEnter:
          m_pParent->SendControlMessage(this, CM_MouseEnter);
          return;
       case CT_MouseLeave:
          m_pParent->SendControlMessage(this, CM_MouseLeave);
          return;
       case CT_SetValue: {
          if (m_iValue < 0 || m_iValue > 100)
              return;
         
          int temp = m_iValue * m_iNumStates / 100; 
          m_iState = min(temp, m_iNumStates - 1);

         if (!m_bActivationReveal) 
             BlitFrame(CS_Normal, m_iState);
         else
             DrawReveal((float)m_iValue * 2.55);

         return; }
       case CT_Hide:
       {
       	  Rect oRect = m_oRect;

           if (m_pPanel->IsHidden())
               return;

           pCanvas = m_pParent->GetCanvas();
           pCanvas->Erase(oRect);
           pCanvas->Invalidate(oRect);
       	   return;
       }   

       default:
          break;
    }

    if (m_pPanel->IsHidden())
        return;
    
    if (m_bHasActivationBitmap && pMousePos)
        m_iState = GetStateNum(*pMousePos);

    if (m_eCurrentState == CS_MouseOver && 
        eTrans == CT_MouseLButtonUp)
        m_pParent->SendControlMessage(this, CM_Pressed);

    if (m_eCurrentState == CS_MouseOver &&
        eTrans == CT_MouseLButtonUp) {
        if (!m_bActivationReveal)
            m_iValue = (int)((float)(m_iState * 100) / m_iNumStates);
        else
            m_iValue = (int)((float)(m_iState * 100) / 255); 
        m_pParent->SendControlMessage(this, CM_ValueChanged);
    }

    if (!m_bActivationReveal)
        BlitFrame(CS_Normal, m_iState);
    else if (pMousePos)
        DrawReveal(*pMousePos);
    else
        DrawReveal((float)m_iValue * 2.55);
}

bool PixSliderControl::PosInControl(Pos &oPos)
{
    bool bRet;
    
    m_oMutex.Acquire();
    bRet = m_oRect.IsPosInRect(oPos);
    m_oMutex.Release();

    if (bRet && m_bHasActivationBitmap)
        return m_pActivationBitmap->IsPosVisible(oPos);
    
    return bRet;    
}

int PixSliderControl::GetStateNum(Pos &oPos)
{
    Color trans, color;
    int statenum;

    m_pActivationBitmap->GetTransColor(trans);
    m_pActivationBitmap->GetColor(oPos, color);

    if (color.IsEqual(trans))
        statenum = -1;
    else
        statenum = ((color.red + color.green + color.blue) / 3) & 0xFF;

    if (!m_bActivationReveal) {
        int temp = statenum * m_iNumStates / 255;
        statenum = min(m_iNumStates - 1, temp);
    }

    return statenum;
}

void PixSliderControl::DrawReveal(Pos &oPos)
{
   int reveal;
   Color trans, seek;

   m_pActivationBitmap->GetTransColor(trans);
   m_pActivationBitmap->GetColor(oPos, seek);

   if (seek.IsEqual(trans))
       return;

   reveal = ((seek.red + seek.green + seek.blue) / 3) & 0xFF;

   DrawReveal(reveal);
}

void PixSliderControl::DrawReveal(int reveal)
{
   int x, y, testreveal;
   Canvas *pCanvas;
   Color trans, color;
   Pos point;
   Rect blit;
   Bitmap *copy;
   int type = 0;

   if (reveal == m_iOldRevealPos)
       return;

   if (reveal == 0) {
       m_iOldRevealPos = 0;
       return;
   }

   m_pActivationBitmap->GetTransColor(trans);
   pCanvas = m_pParent->GetCanvas();

   for (y = m_oRect.y1; y < m_oRect.y2; y++) 
   {
       point.y = y;
       blit.y1 = blit.y2 = y;
       blit.y2++;
       blit.x1 = blit.x2 = m_oRect.x1;
       for (x = m_oRect.x1; x < m_oRect.x2; x++) 
       {
           point.x = x; 
           
           m_pActivationBitmap->GetColor(point, color);
           if (!color.IsEqual(trans)) {
               testreveal = (color.red + color.green + color.blue) / 3;
               if (testreveal <= reveal) {
                   if (type != 1) {
                       if (type == 2) {
                           copy = m_oStateBitmaps[0][CS_Normal];
                           pCanvas->MaskBlitRect(copy, blit, blit);
                       }
                       blit.x1 = blit.x2 = x;
                       blit.y1 = blit.y2 = y;
                       blit.y2++;
                       type = 1;
                   }
                   else 
                       blit.x2++;
               }
               else {
                   if (type != 2) {
                       if (type == 1) {
                           copy = m_oStateBitmaps[0][CS_Pressed];
                           pCanvas->MaskBlitRect(copy, blit, blit);
                       }
                       blit.x1 = blit.x2 = x;
                       blit.y1 = blit.y2 = y;
                       blit.y2++;
                       type = 2;
                   }
                   else 
                       blit.x2++;
               }
           }
           else {
               if (type != 0) {
                   if (type == 2)
                       copy = m_oStateBitmaps[0][CS_Normal]; 
                   else
                       copy = m_oStateBitmaps[0][CS_Pressed];
                   pCanvas->MaskBlitRect(copy, blit, blit);
                   blit.x1 = blit.x2 = x;
                   blit.y1 = blit.y2 = y;
                   blit.y2++;
                   type = 0;
               } 
           }   
       }
       if (type != 0) {
           if (type == 2)
               copy = m_oStateBitmaps[0][CS_Normal];
           else
               copy = m_oStateBitmaps[0][CS_Pressed];
           pCanvas->MaskBlitRect(copy, blit, blit);
           type = 0;
       } 
    }

    m_iOldRevealPos = reveal;

    pCanvas->Invalidate(m_oRect);
    pCanvas->Update();    
}
 
