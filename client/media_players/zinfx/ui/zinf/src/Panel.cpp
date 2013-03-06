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

   $Id: Panel.cpp,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786) 
#endif

#include <stdio.h>
#include <algorithm>
#include "Panel.h"
#include "Theme.h"
#include "debug.h"

#define DB Debug_v("%s:%d\n", __FILE__, __LINE__);

Panel::Panel(const string &oName)
{
    m_oName = oName;
    m_pWindow = NULL;
    m_pPanelBitmap = NULL;
    m_iZOrder = -1;
    m_oPos.x = m_oPos.y = -1;
    m_oTogglePos.x = m_oTogglePos.y = -1;
    m_pOnOpenHide = m_pOnOpenShow = m_pOnCloseHide = m_pOnCloseShow = NULL;
    m_bIsOpen = false;
    m_bIsHidden = false;
}

Panel::~Panel(void)
{
}

void Panel::Init(void)
{
    vector<Control *>::iterator i;

    for(i = m_oControls.begin(); i != m_oControls.end(); i++)
    {
        (*i)->SetPanel(this);
        (*i)->SetParent(m_pWindow);
        (*i)->Init();
        if (!m_bIsHidden)
            (*i)->AcceptTransition(CT_Show);
    }    
}

void Panel::AddControl(Control *pControl)
{
    string oName;

    pControl->GetName(oName);
    m_oControlMap.insert(ControlMap::value_type(oName, pControl));
    m_oControls.push_back(pControl);
}

void Panel::ClearControls(void)
{
    while(m_oControls.size() > 0)
    {
        delete m_oControls[0];
        m_oControls.erase(m_oControls.begin());
    }
    m_oControlMap.clear();
}

void Panel::GetControlToolTips(vector<pair<Rect, string> > &oList)
{
   vector<Control *>::iterator i;
    Rect                        oRect;
    string                      oTip;
    pair<Rect, string>          oTemp;

    if (m_bIsHidden)
        return;

    for(i = m_oControls.begin(); i != m_oControls.end(); i++)
    {
        (*i)->GetTip(oTip);
        (*i)->GetRect(oRect);
        oTemp.first = oRect;
        oTemp.second = oTip;
        oList.push_back(oTemp);
    }
}

void Panel::SetParentWindow(Window *pWin)
{
    vector<Control *>::iterator i;

    m_pWindow = pWin;    
    for(i = m_oControls.begin(); i != m_oControls.end(); i++)
    {
        (*i)->SetParent(pWin);
    }    
}

void Panel::GetName(string &oName)
{
    oName = m_oName;
}

void Panel::SetOnOpenHide(Panel *pPanel) 
{ 
    m_pOnOpenHide = pPanel; 
}

void Panel::SetOnOpenShow(Panel *pPanel) 
{ 
    m_pOnOpenShow = pPanel; 
}

void Panel::SetOnCloseHide(Panel *pPanel)
{ 
    m_pOnCloseHide = pPanel; 
    m_pOnCloseHide->Hide(true);
}

void Panel::SetOnCloseShow(Panel *pPanel) 
{ 
    m_pOnCloseShow = pPanel; 
    m_pOnCloseShow->Hide(false);
} 

Error Panel::ControlEnable(const string &oTarget, bool bSet, bool &bEnable)
{
    ControlMapIterator i;

    pair<ControlMapIterator, ControlMapIterator> keyRange
        = m_oControlMap.equal_range(oTarget);

    for (i = keyRange.first; i != keyRange.second; i++)
    {
        (*i).second->Enable(bSet, bEnable);
    }

    return kError_NoErr;
}

Error Panel::ControlShow(const string &oTarget, bool bSet, bool &bShow)
{
    Pos                 oPos;
    Rect                oRect;
    Error               eRet;
    Control            *pControl;
    ControlMapIterator  i;

    pair<ControlMapIterator, ControlMapIterator> keyRange
        = m_oControlMap.equal_range(oTarget);

    for (i = keyRange.first; i != keyRange.second; i++)
    {
        pControl = (*i).second;
        
        eRet = pControl->Show(bSet, bShow);

        m_pWindow->GetMousePos(oPos);
        m_pWindow->GetWindowPosition(oRect);
        oPos.x -= oRect.x1;
        oPos.y -= oRect.y1;
        if (bSet && bShow && pControl->PosInControl(oPos))
            pControl->AcceptTransition(CT_MouseEnter);
    }        

    return kError_NoErr;
}

Error Panel::ControlIntValue(const string &oTarget, bool bSet, int &iValue)
{
    ControlMapIterator  i;

    pair<ControlMapIterator, ControlMapIterator> keyRange
        = m_oControlMap.equal_range(oTarget);

    for (i = keyRange.first; i != keyRange.second; i++)
    {
         (*i).second->IntValue(bSet, iValue);
    }        

    return kError_NoErr;
}

Error Panel::ControlStringValue(const string &oTarget, bool bSet, string &oValue)
{
    ControlMapIterator  i;

    pair<ControlMapIterator, ControlMapIterator> keyRange
        = m_oControlMap.equal_range(oTarget);

    for (i = keyRange.first; i != keyRange.second; i++)
    {
        (*i).second->StringValue(bSet, oValue);
    }        

    return kError_NoErr;
}

Error Panel::ControlGetDesc(const string &oTarget, string &oDesc)
{
    ControlMapIterator  i;

    pair<ControlMapIterator, ControlMapIterator> keyRange
        = m_oControlMap.equal_range(oTarget);

    for (i = keyRange.first; i != keyRange.second; i++)
    {
         (*i).second->GetDesc(oDesc);
         return kError_NoErr;
    }        

    return kError_InvalidParam;
}

Error Panel::ControlGetTip(const string &oTarget, string &oTip)
{
    ControlMapIterator  i;

    pair<ControlMapIterator, ControlMapIterator> keyRange
        = m_oControlMap.equal_range(oTarget);

    for (i = keyRange.first; i != keyRange.second; i++)
    {
         (*i).second->GetTip(oTip);
         return kError_NoErr;
    }        

    return kError_InvalidParam;
}

bool Panel::DoesControlExist(const string &oName)
{
    return m_oControlMap.find(oName) != m_oControlMap.end();
}

Control *Panel::ControlFromPos(Pos &oPos)
{
    vector<Control *>::iterator i;
    bool                        bShown;
    Control                    *pControl;

    if (m_bIsHidden)
       return NULL;

    for(i = m_oControls.begin(); i != m_oControls.end(); i++)
    {
        (*i)->Show(false, bShown);
        if ((*i)->PosInControl(oPos) && bShown) 
        {
            pControl = (*i);
            return pControl;
        }    
    }        

    return NULL;
}

void Panel::TimerEvent(void)
{
    vector<Control *>::iterator i;

    for(i = m_oControls.begin(); i != m_oControls.end(); i++)
    {
        if ((*i)->WantsTimingMessages())
            (*i)->AcceptTransition(CT_Timer);
    }
} 

void Panel::MoveControls(Pos &oPos)
{
    vector<Control *>::iterator i;

    for(i = m_oControls.begin(); i != m_oControls.end(); i++)
    {
        (*i)->Move(oPos);
    }
}

void Panel::TogglePanelPos(void)
{
    Pos oTemp;

    if (m_bIsOpen)
    {
        if (m_pOnCloseShow)
            m_pOnCloseShow->Hide(false);
        if (m_pOnCloseHide)
            m_pOnCloseHide->Hide(true);
    } 
    else
    {
        if (m_pOnOpenShow)
            m_pOnOpenShow->Hide(false);
        if (m_pOnOpenHide)
            m_pOnOpenHide->Hide(true);
    } 

    m_bIsOpen = !m_bIsOpen;
    oTemp = m_oPos;
    m_oPos = m_oTogglePos;
    m_oTogglePos = oTemp;
}

void Panel::ShowAllControls(void)
{
    ControlMapIterator i;

    for (i = m_oControlMap.begin(); i != m_oControlMap.end(); i++)
    {
         (*i).second->Init();
         (*i).second->AcceptTransition(m_bIsHidden ? CT_Hide : CT_Show);
    }        
}
