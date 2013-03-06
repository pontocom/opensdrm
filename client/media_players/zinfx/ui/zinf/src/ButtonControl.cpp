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

   $Id: ButtonControl.cpp,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#include "stdio.h"
#include "ButtonControl.h"
#include "Window.h"
#include "debug.h"

// Bitmap info:
// Frame 0: Normal
// Frame 1: MouseOver
// Frame 2: Pressed
// Frame 3: Disabled


static TransitionInfo pTransitions[] =
{
    { CS_Normal,     CT_MouseEnter,       CS_MouseOver  },
    { CS_Normal,     CT_Disable,          CS_Disabled   },
    { CS_Normal,     CT_Hide,             CS_Hidden     },
    { CS_Normal,     CT_Show,             CS_Normal     },
    { CS_MouseOver,  CT_MouseLeave,       CS_Normal     },
    { CS_MouseOver,  CT_MouseLButtonDown, CS_Pressed    },
    { CS_MouseOver,  CT_Disable,          CS_Disabled   },
    { CS_MouseOver,  CT_Hide,             CS_Hidden     },
    { CS_MouseOver,  CT_Show,             CS_MouseOver  },
    { CS_Pressed,    CT_MouseLButtonUp,   CS_MouseOver  },
    { CS_Pressed,    CT_Disable,          CS_Disabled   },
    { CS_Pressed,    CT_MouseLeave,       CS_Normal     },
    { CS_Pressed,    CT_Show,             CS_Pressed    },
    { CS_Disabled,   CT_Enable,           CS_Normal     },
    { CS_Disabled ,  CT_MouseEnter,       CS_DisabledMO },
    { CS_DisabledMO, CT_MouseLeave,       CS_Disabled   },
    { CS_Hidden,     CT_Show,             CS_Normal     },
    { CS_LastState,  CT_LastTransition,   CS_LastState  }
};

ButtonControl::ButtonControl(Window *pWindow, string &oName) :
               Control(pWindow, oName, pTransitions)
{
   m_pPanelToToggle = NULL;
}

ButtonControl::ButtonControl(Window *pWindow, string &oName, string &oUrl) :
               Control(pWindow, oName, pTransitions)
{
   m_oValue = oUrl;
   m_pPanelToToggle = NULL;
}

ButtonControl::~ButtonControl(void)
{
}

void ButtonControl::Init(void)
{
    m_oMutex.Acquire();

    m_eCurrentState = CS_Normal;

    if (!m_bUsesStateBitmapRects) {
        for (int i = 0; i < 4; i++) {
           int iFrameWidth;
           Rect oFrameRect;
           
           if (m_bHorizontalBitmap) {
               iFrameWidth = (m_oBitmapRect.Width() + 1) / 4;
               oFrameRect = m_oBitmapRect;
               oFrameRect.x1 += i * iFrameWidth;
               oFrameRect.x2 = oFrameRect.x1 + iFrameWidth - 1;
           }
           else {
               iFrameWidth = (m_oBitmapRect.Height() + 1) / 4;
               oFrameRect = m_oBitmapRect;
               oFrameRect.y1 += i * iFrameWidth;
               oFrameRect.y2 = oFrameRect.y1 + iFrameWidth - 1;
           }

           switch (i) {
               case 0:
                   SetStateBitmap(m_pBitmap, oFrameRect, CS_Normal);
                    break;
               case 1:
                   SetStateBitmap(m_pBitmap, oFrameRect, CS_MouseOver);
                    break;
               case 2:
                   SetStateBitmap(m_pBitmap, oFrameRect, CS_Pressed);
                    break;
               case 3:
                   SetStateBitmap(m_pBitmap, oFrameRect, CS_Disabled);
                    break;
                default:
                   break;
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

    Transition(CT_None, NULL);
}

void ButtonControl::Transition(ControlTransitionEnum  eTrans,
                               Pos                   *pMousePos)
{
    Canvas *pCanvas;

    if (m_pPanel->IsHidden())
       return;

    switch(eTrans)
    {
       case CT_MouseEnter:
          m_pParent->SendControlMessage(this, CM_MouseEnter);
          break;
       case CT_MouseLeave:
          m_pParent->SendControlMessage(this, CM_MouseLeave);
          break;
       case CT_Hide:
       {
          Rect oRect = m_oRect;
          pCanvas = m_pParent->GetCanvas();
          pCanvas->Erase(oRect);
          pCanvas->Invalidate(oRect);
          break;
       }   

       default:
          break;
    }

    if (m_eCurrentState == CS_MouseOver && 
        eTrans == CT_MouseLButtonUp)
    {    
       if (m_oTargetWindow.length() > 0)
       {   
           m_oMutex.Acquire();
           m_oValue = m_oTargetWindow;
           m_oMutex.Release();
           
           m_pParent->SendControlMessage(this, CM_ChangeWindow);
           return;
       }
       if (m_pPanelToToggle)
       {
           m_pPanelToToggle->TogglePanelPos();
           m_pParent->SendControlMessage(this, CM_TogglePanel);
           m_eCurrentState = CS_MouseOver;
           BlitFrame(m_eCurrentState);
           return;
       }
       if (m_oName == string("ReloadTheme"))
       {
          m_pParent->SendControlMessage(this, CM_Pressed);
          return;
       }   
       m_pParent->SendControlMessage(this, CM_Pressed);
    }       

    pCanvas = m_pParent->GetCanvas();
    pCanvas->Erase(m_oRect);
    pCanvas->Invalidate(m_oRect);

    BlitFrame(m_eCurrentState);
}

bool ButtonControl::PosInControl(Pos &oPos)
{
    bool bRet;
    
    m_oMutex.Acquire();

    bRet = m_oRect.IsPosInRect(oPos);
    if (bRet && m_pBitmap)
    {
        Pos oLocalPos;
        
        oLocalPos.x = (oPos.x - m_oRect.x1) + 
                      m_oStateBitmapRect[0][m_eCurrentState].x1;
        oLocalPos.y = (oPos.y - m_oRect.y1) + 
                      m_oStateBitmapRect[0][m_eCurrentState].y1;
        m_oMutex.Release();
        
        return m_pBitmap->IsPosVisible(oLocalPos);
    }    
    
    m_oMutex.Release();
    
    return bRet;    
}        

void ButtonControl::SetTargetWindow(string &oWindow)
{
    m_oMutex.Acquire();
    m_oTargetWindow = oWindow;
    m_oMutex.Release();
}

void ButtonControl::SetPanelToggle(Panel *pPanel)
{
    m_oMutex.Acquire();
    m_pPanelToToggle = pPanel;
    m_oMutex.Release();
}

bool ButtonControl::UseToDragWindow(void)
{
    return m_oName == string("WindowStatus");
}

