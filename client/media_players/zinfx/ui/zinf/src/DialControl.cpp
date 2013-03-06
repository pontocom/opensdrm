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

   $Id: DialControl.cpp,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#include "stdio.h"
#include "DialControl.h"
#include "Window.h"
#include "debug.h"

static TransitionInfo pTransitions[] =
{  
    { CS_Normal,     CT_Show,             CS_Normal     }, 
    { CS_Normal,     CT_MouseEnter,       CS_MouseOver  }, 
    { CS_Normal,     CT_Disable,          CS_Disabled   }, 
    { CS_Normal,     CT_Hide,             CS_Hidden     },
    { CS_MouseOver,  CT_MouseLeave,       CS_Normal     }, 
    { CS_MouseOver,  CT_MouseLButtonDown, CS_Dragging   }, 
    { CS_MouseOver,  CT_Disable,          CS_Disabled   }, 
    { CS_Dragging,   CT_MouseMove,        CS_Dragging   }, 
    { CS_Dragging,   CT_MouseLButtonUp,   CS_MouseOver  }, 
    { CS_Disabled ,  CT_Enable,           CS_Normal     },
    { CS_Disabled ,  CT_MouseEnter,       CS_DisabledMO },
    { CS_DisabledMO, CT_MouseLeave,       CS_Disabled   },
    { CS_Hidden,     CT_Show,             CS_Normal     },
    { CS_Any,        CT_SetValue,         CS_Same       },
    { CS_LastState,  CT_LastTransition,   CS_LastState  }
};

DialControl::DialControl(Window *pWindow, string &oName,
                         int iNumFrames, int iNumPixelsPerFrame) :
             Control(pWindow, oName, pTransitions)
{
    m_iCurrentFrame = 0;
    m_iNumFrames = iNumFrames;
    m_iNumPixelsPerFrame = iNumPixelsPerFrame;
}

DialControl::~DialControl(void)
{

}

void DialControl::Init(void)
{
    BlitFrame(m_iCurrentFrame, m_iNumFrames);
}

void DialControl::Transition(ControlTransitionEnum  eTrans,
                             Pos                   *pMousePos)

{
    int iFrames, iValue;

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

       case CT_MouseLButtonDown:
           m_oMutex.Acquire();
           m_iLastPos = pMousePos->y;
           m_oMutex.Release();
           break;

       case CT_Show:
           Init();
           break;

       default:
          break;
    }  

    if (m_eCurrentState == CS_Dragging && 
        m_eLastState != CS_Dragging)
    {    
       Pos oTemp;
       
       m_pParent->StartMouseCapture(this);
       m_pParent->GetMousePos(oTemp);
       
       m_oMutex.Acquire();
       m_oOrigin = oTemp;
       m_oMutex.Release();
       
       m_pParent->HideMouse(true);
       return;
    }

    if (m_eCurrentState != CS_Dragging && 
        m_eLastState == CS_Dragging)
    {    
       m_pParent->EndMouseCapture();
       m_pParent->SetMousePos(m_oOrigin);
       m_pParent->HideMouse(false);
       m_pParent->SendControlMessage(this, CM_ValueChanged);
       return;
    }   

    if (eTrans != CT_MouseMove || m_eCurrentState != CS_Dragging)
       return;

    iFrames = (pMousePos->y - m_iLastPos) / m_iNumPixelsPerFrame;
    if (iFrames == 0)
       return;
       
    m_oMutex.Acquire();
    m_iCurrentFrame = ((m_iCurrentFrame + m_iNumFrames) - iFrames) % m_iNumFrames;
    m_oMutex.Release();
    
    BlitFrame(m_iCurrentFrame, m_iNumFrames);

    m_oMutex.Acquire();
    iValue = m_iValue - iFrames;
    if (iValue != m_iValue && iValue <= 100 && iValue >= 0)
    {
        m_iValue = iValue;

        m_oMutex.Release();
        m_pParent->SendControlMessage(this, CM_SliderUpdate);
        m_oMutex.Acquire();
    }    

    m_iLastPos = pMousePos->y;
    
    m_oMutex.Release();
}
