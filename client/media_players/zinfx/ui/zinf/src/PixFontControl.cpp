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

   $Id: PixFontControl.cpp,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#include "stdio.h"
#include "PixFontControl.h"
#include "Window.h"
#include "debug.h"

#define DB Debug_v("%s:%d\n", __FILE__, __LINE__);

static TransitionInfo pTransitions[] =
{  
    { CS_Normal,    CT_Show,             CS_Normal    }, 
    { CS_Normal,    CT_SetValue,         CS_Normal    },
    { CS_Normal,    CT_Timer,            CS_Normal    },
    { CS_Normal,    CT_MouseEnter,       CS_MouseOver },
    { CS_MouseOver, CT_SetValue,         CS_MouseOver },
    { CS_MouseOver, CT_Timer,            CS_MouseOver },
    { CS_MouseOver, CT_MouseLeave,       CS_Normal    },
    { CS_MouseOver, CT_MouseLButtonDown, CS_Pressed   },
    { CS_Pressed,   CT_MouseLButtonUp,   CS_MouseOver },
    { CS_Pressed,   CT_MouseLeave,       CS_Normal    },
    { CS_Normal,    CT_Hide,             CS_Hidden    },
    { CS_Hidden,    CT_Show,             CS_Normal    },
    { CS_LastState, CT_LastTransition,   CS_LastState } 
};

const int m_iMarqueeScrollIncrement = 5;

PixFontControl::PixFontControl(Window *pWindow, string &oName) : 
                Control(pWindow, oName, pTransitions)
{
    m_NumRows = 0;
    m_LetterWidth = 0;
    m_LetterHeight = 0;
    m_bNoCase = false;
    m_Default = ' ';
    m_iMarqueePos = 0;
    for (uint32 i = 0; i < 256; i++)
        m_lettermap[i].row = m_lettermap[i].col = 300;
}

bool PixFontControl::UseToDragWindow(void)
{
    return m_oName != string("Time") && 
           m_oName != string("TimeRemaining") &&
           m_oName != string("HeadlineInfo");
}

PixFontControl::~PixFontControl(void)
{
    vector<char *>::iterator i;
    for (i = m_Fontmap.begin(); i != m_Fontmap.end(); i++) 
        delete [] (*i);
}

void PixFontControl::AddMapRow(char *row)
{
    m_NumRows++;

    char *newrow = new char[strlen(row) + 1];
    strcpy(newrow, row);
    m_Fontmap.push_back(newrow);
}

void PixFontControl::Init(void)
{
    uint32 i, j, length;
    m_LetterHeight = (m_oBitmapRect.Height() + 1) / m_NumRows;
    for (i = 0; i < m_NumRows; i++) {
        length = strlen(m_Fontmap[i]);
        for (j = 0; j < length; j++) {
            int letter = m_Fontmap[i][j];

            if (m_lettermap[letter].row == 300 && (letter < 256)) {
                m_lettermap[letter].row = i;
                m_lettermap[letter].col = j;
                if (letter >= 'A' && letter <= 'Z' && m_bNoCase) {
                    letter = letter - 'A' + 'a';
                    m_lettermap[letter].row = i;
                    m_lettermap[letter].col = j;
                }
            }
        }
    }
    m_LetterWidth = (m_oBitmapRect.Width() + 1) / strlen(m_Fontmap[0]);

    TextChanged();
}

void PixFontControl::Transition(ControlTransitionEnum  eTrans,
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
        case CT_Show:
        case CT_SetValue:
            TextChanged();
            break;
        case CT_Timer:
            MarqueeText();
            break;

        default:
            break;
    }
}

void PixFontControl::BlitLetter(char letter, Rect oDestRect)
{
    if ((oDestRect.x2 + 1) < m_oRect.x1)
        return;
    if (oDestRect.x1 > m_oRect.x2)
        return;

    if (m_lettermap[letter].row == 300)
        letter = m_Default;

    Rect srcRect;

    srcRect.x1 = m_lettermap[letter].col * m_LetterWidth; 
    srcRect.y1 = m_lettermap[letter].row * m_LetterHeight;
    srcRect.x2 = srcRect.x1 + m_LetterWidth - 1;
    srcRect.y2 = srcRect.y1 + m_LetterHeight;

    m_pParent->GetCanvas()->MaskBlitRect(m_pBitmap, srcRect, oDestRect);
}

int PixFontControl::BlitString(string &oText, int iOffset)
{
    char *text = new char[strlen(oText.c_str()) + 2];
    strcpy(text, oText.c_str());
    strcat(text, " ");

    uint32 length = strlen(text);
    int width = (length - 1) * m_LetterWidth;
    uint32 i;

    Rect dest;
    int currentOffset = iOffset;

    if (iOffset != 0) {
        width += 20;
        if (iOffset > width) {
            delete [] text;
            return width - iOffset;
        }
    }

    for (i = 0; i < length; i++) {
        dest.y1 = m_oRect.y1;
        dest.y2 = m_oRect.y1 + m_LetterHeight;
        dest.x1 = m_oRect.x1 - currentOffset;
        dest.x2 = dest.x1 + m_LetterWidth;
      
        BlitLetter(text[i], dest);
        currentOffset -= m_LetterWidth;
    }

    if (iOffset != 0) {
        currentOffset = width - iOffset - m_oRect.Width() + 1;
        if (currentOffset < 0) {
            currentOffset = 0 - (width - iOffset);
            for (i = 0; i < length; i++) {
                dest.y1 = m_oRect.y1;
                dest.y2 = m_oRect.y1 + m_LetterHeight;
                dest.x1 = m_oRect.x1 - currentOffset;
                dest.x2 = dest.x1 + m_LetterWidth;

                BlitLetter(text[i], dest);
                currentOffset -= m_LetterWidth;
            }
        }
        currentOffset = width - iOffset - (m_oRect.Width() + 1);
    }
    else
        currentOffset = width - (m_oRect.Width() + 1);

    Canvas *pCanvas = m_pParent->GetCanvas();
    pCanvas->Invalidate(m_oRect);
    pCanvas->Update();

    delete [] text;

    if (currentOffset > 0)
        return currentOffset;
    return 0;    
}    

void PixFontControl::TextChanged(void)
{
    Canvas *pCanvas;
    int    iRet;

    m_oMutex.Acquire();
    
    m_iMarqueePos = 0;
    pCanvas = m_pParent->GetCanvas();
    m_oMutex.Release();

    pCanvas->Erase(m_oRect);

    iRet = BlitString(m_oValue, 0);

    m_oMutex.Acquire();
    m_bWantsTimingMessages = (iRet > 0);
    m_oMutex.Release();
}

void PixFontControl::MarqueeText(void)
{
    Canvas *pCanvas;
    int    iRet;

    m_oMutex.Acquire();
    m_iMarqueePos += m_iMarqueeScrollIncrement;
    m_oMutex.Release();
    
    pCanvas = m_pParent->GetCanvas();
    pCanvas->Erase(m_oRect);
    iRet = BlitString(m_oValue, m_iMarqueePos);

    if (iRet < 0)
    {
       m_oMutex.Acquire();
       m_iMarqueePos = -iRet;                           
       m_oMutex.Release();
       BlitString(m_oValue, m_iMarqueePos);
    }                             
}
