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

   $Id: MultiStateControl.cpp,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#include "stdio.h"
#include "MultiStateControl.h"
#include "Window.h"
#include "debug.h"

// Bitmap info:
// For each 'state'
// Frame 0: Normal
// Frame 1: MouseOver
// Frame 2: Pressed
// Frame 3: Disabled
// Frames are horizontal, states are vertical

static TransitionInfo pTransitions[] =
{
    { CS_Normal,     CT_Show,             CS_Normal     }, 
    { CS_Normal,     CT_MouseEnter,       CS_MouseOver  },
    { CS_Normal,     CT_Disable,          CS_Disabled   },
    { CS_Normal,     CT_Hide,             CS_Hidden     },
    { CS_Normal,     CT_Show,             CS_Normal     },
    { CS_MouseOver,  CT_MouseLeave,       CS_Normal     },
    { CS_MouseOver,  CT_MouseLButtonDown, CS_Pressed    },
    { CS_MouseOver,  CT_Disable,          CS_Disabled   },
    { CS_MouseOver,  CT_Hide,             CS_Hidden     },
    { CS_Pressed,    CT_MouseLButtonUp,   CS_MouseOver  },
    { CS_Pressed,    CT_Disable,          CS_Disabled   },
    { CS_Pressed,    CT_MouseLeave,       CS_Normal     },
    { CS_Disabled,   CT_Enable,           CS_Normal     },
    { CS_Disabled ,  CT_MouseEnter,       CS_DisabledMO },
    { CS_DisabledMO, CT_MouseLeave,       CS_Disabled   },
    { CS_Hidden,     CT_Show,             CS_Normal     },
    { CS_Any,        CT_SetValue,         CS_Same       },
    { CS_LastState,  CT_LastTransition,   CS_LastState  }
};

MultiStateControl::MultiStateControl(Window *pWindow, 
                                     string &oName, 
                                     int iNumStates) :
               Control(pWindow, oName, pTransitions)
{
    m_iState = 0;
    m_iNumStates = iNumStates;
}

MultiStateControl::~MultiStateControl(void)
{

}

void MultiStateControl::GetDesc(string &oDesc)
{
    uint32 i = m_iState;

    m_oMutex.Acquire();

    if (m_oDescs.size() == 0)
    {
       oDesc = "";
       m_oMutex.Release();
       return;
    }
    
    if (i >= m_oDescs.size())
      i = 0;
       
    oDesc = m_oDescs[i];
    
    m_oMutex.Release();
}    

void MultiStateControl::GetTip(string &oTip)
{
    uint32 i = m_iState;
    
    m_oMutex.Acquire();
    
    if (m_oTips.size() == 0)
    {
       oTip = "";
       m_oMutex.Release();
       return;
    }
    
    if (i >= m_oTips.size())
      i = 0;
       
    oTip = m_oTips[i];
    
    m_oMutex.Release();
}    

void MultiStateControl::Init(void)
{
    uint32 iOffset;
    int    iRet;
    char  *szDup;

    m_oMutex.Acquire();

    szDup = strdup(m_oDesc.c_str());
    iOffset = 0;
    for(;;)
    {
        iRet = sscanf((char *)m_oDesc.c_str() + iOffset, "%[^|]", szDup);
        if (iRet < 1) 
            break;
            
        m_oDescs.push_back(string(szDup));
            
        iOffset += strlen(szDup) + 2;
        if (iOffset > m_oDesc.length())
            break;
    }
    free(szDup);

    szDup = strdup(m_oToolTip.c_str());
    iOffset = 0;
    for(;;)
    {
        iRet = sscanf((char *)m_oToolTip.c_str() + iOffset, "%[^|]", szDup);
        if (iRet < 1) 
            break;
            
        m_oTips.push_back(string(szDup));
            
        iOffset += strlen(szDup) + 2;
        if (iOffset > m_oToolTip.length())
            break;
    }
    free(szDup);

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
        //m_oOrigRect = m_oRect;
    }    
    m_oMutex.Release();
    
    BlitFrame(CS_Normal, m_iState);
}

void MultiStateControl::Transition(ControlTransitionEnum  eTrans,
                                   Pos                   *pMousePos)
{
    Canvas *pCanvas;
    
    switch(eTrans)
    {
       case CT_MouseEnter:
          m_pParent->SendControlMessage(this, CM_MouseEnter);
          break;
       case CT_MouseLeave:
          m_pParent->SendControlMessage(this, CM_MouseLeave);
          break;
       case CT_SetValue:
       	  m_iState = min(max(m_iValue, 0), m_iNumStates - 1);
          break;
       case CT_Hide:
       {
       	  Rect oRect = m_oRect;

           if (m_pPanel->IsHidden())
               return;

           pCanvas = m_pParent->GetCanvas();
           pCanvas->Erase(oRect);
           pCanvas->Invalidate(oRect);
       	  break;
       }   

       default:
          break;
    }

    if (m_pPanel->IsHidden())
        return;

    if (m_eCurrentState == CS_MouseOver && 
        eTrans == CT_MouseLButtonUp)
        m_pParent->SendControlMessage(this, CM_Pressed);

    BlitFrame(m_eCurrentState, m_iState);
}

bool MultiStateControl::PosInControl(Pos &oPos)
{
    bool bRet;
    
    m_oMutex.Acquire();
    
    bRet = m_oRect.IsPosInRect(oPos);
    if (bRet && m_pBitmap)
    {
    	Pos oLocalPos;
        
        oLocalPos.x = (oPos.x - m_oRect.x1) + 
                      m_oStateBitmapRect[m_iState][m_eCurrentState].x1;
        oLocalPos.y = (oPos.y - m_oRect.y1) + 
                      m_oStateBitmapRect[m_iState][m_eCurrentState].y1;
        
        m_oMutex.Release();
        return m_pBitmap->IsPosVisible(oLocalPos);
    }    
    
    m_oMutex.Release();
    
    return bRet;    
}        

