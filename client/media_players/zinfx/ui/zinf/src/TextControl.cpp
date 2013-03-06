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

   $Id: TextControl.cpp,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/ 

#include "stdio.h"
#include "TextControl.h"
#include "Window.h"
#include "debug.h"
#include "sys/types.h"

#ifdef __QNX__
#include <strings.h>
#endif

#define DB Debug_v("%s:%d\n", __FILE__, __LINE__);

static TransitionInfo pTransitions[] =
{  
    { CS_Normal,    CT_SetValue,                 CS_Normal    },
    { CS_Normal,    CT_Show,                     CS_Normal    },
    { CS_Normal,    CT_Timer,                    CS_Normal    },
    { CS_Normal,    CT_MouseEnter,               CS_MouseOver },
    { CS_MouseOver, CT_SetValue,                 CS_MouseOver },
    { CS_MouseOver, CT_Timer,                    CS_MouseOver },
    { CS_MouseOver, CT_MouseLeave,               CS_Normal    },
    { CS_MouseOver, CT_MouseLButtonDown,         CS_Pressed   },
    { CS_Any,       CT_MouseLButtonDoubleClick,  CS_Same      },
    { CS_Pressed,   CT_MouseLButtonUp,           CS_MouseOver },
    { CS_Pressed,   CT_MouseLeave,               CS_Normal    },
    { CS_Normal,    CT_Hide,                     CS_Hidden    },
    { CS_Hidden,    CT_Show,                     CS_Normal    },
    { CS_LastState, CT_LastTransition,           CS_LastState } 
};

const int m_iMarqueeScrollIncrement = 5;

TextControl::TextControl(Window *pWindow, string &oName) : 
             Control(pWindow, oName, pTransitions)
{
    m_pFont = NULL;
    m_iMarqueePos = 0;
    m_bStyleSet = false;
}

void TextControl::SetStyle(Font *pFont, string &oAlign, 
                           Color &oColor, bool bBold,
                           bool bItalic, bool bUnderline)
{
    m_oMutex.Acquire();

    m_oColor = oColor;
    m_bBold = bBold;
    m_bItalic = bItalic;
    m_bUnderline = bUnderline;
    m_pFont = pFont;
       
    if (strcasecmp(oAlign.c_str(), "right") == 0)
       m_eAlign = eRight;
    else   
    if (strcasecmp(oAlign.c_str(), "center") == 0)
       m_eAlign = eCenter;
    else   
       m_eAlign = eLeft;

    m_bStyleSet = true;

    m_oMutex.Release();
}

bool TextControl::StyleHasBeenSet(void)
{
    return m_bStyleSet;
}      

bool TextControl::UseToDragWindow(void)
{
    return m_oName != string("Time") && 
           m_oName != string("TimeRemaining") &&
           m_oName != string("HeadlineInfo") &&
           m_oName != string("HeadlineStreamInfo");
}

TextControl::~TextControl(void)
{
#ifdef HAVE_GTK
    delete m_pFont;
#endif
}

void TextControl::Init(void)
{
    TextChanged();
}

void TextControl::Transition(ControlTransitionEnum  eTrans,
                             Pos                   *pMousePos)
{
    if (m_eCurrentState == CS_MouseOver && 
        eTrans == CT_MouseLButtonUp)
       m_pParent->SendControlMessage(this, CM_Pressed);

    switch(eTrans)
    {
        case CT_MouseEnter:
            m_pParent->SendControlMessage(this, CM_MouseEnter);
            break;
        case CT_MouseLeave:
            m_pParent->SendControlMessage(this, CM_MouseLeave);
            break;
        case CT_MouseLButtonDoubleClick:
            m_pParent->SendControlMessage(this, CM_MouseDoubleClick);
            break;
        case CT_SetValue:
            TextChanged();
            break;
        case CT_Timer:
            MarqueeText();
            break;
        case CT_Show:
            TextChanged();
            break;
        default:
            break;
    }
}

void TextControl::TextChanged(void)
{
    Canvas *pCanvas;
    int    iRet;

    if (m_pPanel->IsHidden())
        return;

    m_oMutex.Acquire();
    
    m_iMarqueePos = 0;
    pCanvas = m_pParent->GetCanvas();
    m_oMutex.Release();

    pCanvas->SetNoScreenUpdate(true);
    pCanvas->Erase(m_oRect);
    iRet = pCanvas->RenderText(m_oRect.Height(), m_oRect, 
                               m_oValue, m_eAlign, 
                               m_pFont, m_oColor, m_bBold, 
                               m_bItalic, m_bUnderline); 

    // If this is the time display, don't marquee it
    if (iRet > 0 && m_oName == string("Time") || 
                    m_oName == string("TimeRemaining"))
    {
       pCanvas->Erase(m_oRect);
       pCanvas->RenderText(m_oRect.Height(), m_oRect, 
                           m_oValue, eRight, 
                           m_pFont, m_oColor, m_bBold, 
                           m_bItalic, m_bUnderline); 
       iRet = 0;
    }
    pCanvas->SetNoScreenUpdate(false);
    pCanvas->Invalidate(m_oRect);
    pCanvas->Update();

    m_oMutex.Acquire();
    m_bWantsTimingMessages = (iRet > 0);
    m_oMutex.Release();
}

void TextControl::MarqueeText(void)
{
    Canvas *pCanvas;
    int    iRet;

    m_oMutex.Acquire();
	m_iMarqueePos += m_iMarqueeScrollIncrement;
    m_oMutex.Release();
    
    pCanvas = m_pParent->GetCanvas();
    pCanvas->Erase(m_oRect);
    iRet = pCanvas->RenderOffsetText(m_oRect.Height(), m_oRect, 
                                     m_oValue, m_iMarqueePos, 
                                     m_pFont, m_oColor, m_bBold, 
                                     m_bItalic, m_bUnderline); 
    if (iRet < 0)
    {
       m_oMutex.Acquire();
       m_iMarqueePos = -iRet;                           
       m_oMutex.Release();
       pCanvas->RenderOffsetText(m_oRect.Height(), m_oRect, 
                                 m_oValue, m_iMarqueePos, 
                                 m_pFont, m_oColor, m_bBold, 
                                 m_bItalic, m_bUnderline); 
    }                             
}
