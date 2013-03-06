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

   $Id: PixTimeControl.cpp,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/ 

#include "stdio.h"
#include "PixTimeControl.h"
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

PixTimeControl::PixTimeControl(Window *pWindow, string &oName) : 
                Control(pWindow, oName, pTransitions)
{
    m_MinuteColon = "";
    m_SecondColon = "";
    m_Minus = "";
    m_HourHundred = "";
    m_HourTen = "";
    m_HourOne = "";
    m_MinuteTen = "";
    m_MinuteOne = "";
    m_SecondTen = "";
    m_SecondOne = "";
}

bool PixTimeControl::UseToDragWindow(void)
{
    return m_oName != string("Time"); 
}

PixTimeControl::~PixTimeControl(void)
{
}

void PixTimeControl::SetPartName(string part, string name)
{
    if (part == string("Minus"))
        m_Minus = name;
    else if (part == string("MinuteColon"))
        m_MinuteColon = name;
    else if (part == string("SecondColon"))
        m_SecondColon = name;
    else if (part == string("HourHundred"))
        m_HourHundred = name;
    else if (part == string("HourTen"))
        m_HourTen = name;
    else if (part == string("HourOne"))
        m_HourOne = name;
    else if (part == string("MinuteTen"))
        m_MinuteTen = name;
    else if (part == string("MinuteOne"))
        m_MinuteOne = name;
    else if (part == string("SecondTen"))
        m_SecondTen = name;
    else if (part == string("SecondOne"))
        m_SecondOne = name;
}

void PixTimeControl::Init(void)
{
    TextChanged();
}

void PixTimeControl::Transition(ControlTransitionEnum  eTrans,
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
        default:
            break;
    }
}

void PixTimeControl::TextChanged(void)
{
    Canvas *pCanvas;

    m_oMutex.Acquire();
    pCanvas = m_pParent->GetCanvas();
    m_oMutex.Release();

    int hours = 0, minutes = 0, seconds = 0;
    bool minus = false;
    bool show_hours = true;

    if (m_oValue.size() != 0) {
        char *timestr = new char[m_oValue.size() + 1];
        strcpy(timestr, m_oValue.c_str());

        char *p = timestr;        

        if (timestr[0] == '-') {
            minus = true;
            p++;
        }

        if (sscanf(p, "%d:%d:%d", &hours, &minutes, &seconds) != 3) {
            hours = 0;
            show_hours = false;
            sscanf(p, "%d:%d", &minutes, &seconds);
        }

        delete [] timestr;
    }

    if (m_HourHundred.size() == 0 && m_HourTen.size() == 0 &&
        m_HourOne.size() == 0) {
        minutes += hours * 60;
        show_hours = false;
    }

    minus = !minus;
    m_pParent->ControlEnable(m_Minus, true, minus);
  
    char digit[10];
    string tempstr;

    if (show_hours) {
        if (m_MinuteColon.size() > 0) {
            sprintf(digit, ":");
            tempstr = digit;
            m_pParent->ControlStringValue(m_MinuteColon, true, tempstr);
        }
        if (m_HourHundred.size() > 0) {
            sprintf(digit, "%d", hours / 100);
            tempstr = digit;
            m_pParent->ControlStringValue(m_HourHundred, true, tempstr);
        }
        hours %= 100;
        if (m_HourTen.size() > 0) {
            sprintf(digit, "%d", hours / 10);
            tempstr = digit;
            m_pParent->ControlStringValue(m_HourTen, true, tempstr);
        }
        hours %= 10;
        if (m_HourOne.size() > 0) {
            sprintf(digit, "%d", hours);
            tempstr = digit;
            m_pParent->ControlStringValue(m_HourOne, true, tempstr);
        }
    }

    if (m_SecondColon.size() > 0) {
        sprintf(digit, ":");
        tempstr = digit;
        m_pParent->ControlStringValue(m_SecondColon, true, tempstr);
    }

    if (m_MinuteTen.size() > 0) {
        sprintf(digit, "%d", minutes / 10);
        tempstr = digit;
        m_pParent->ControlStringValue(m_MinuteTen, true, tempstr);
    }
    minutes %= 10;
    if (m_MinuteOne.size() > 0) {
        sprintf(digit, "%d", minutes);
        tempstr = digit;
        m_pParent->ControlStringValue(m_MinuteOne, true, tempstr);
    }

    if (m_SecondTen.size() > 0) {
        sprintf(digit, "%d", seconds / 10);
        tempstr = digit;
        m_pParent->ControlStringValue(m_SecondTen, true, tempstr);
    }
    seconds %= 10;
    if (m_SecondOne.size() > 0) {
        sprintf(digit, "%d", seconds);
        tempstr = digit;
        m_pParent->ControlStringValue(m_SecondOne, true, tempstr);
    }
}
