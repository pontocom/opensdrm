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

   $Id: Window.cpp,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/ 

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786) 
#endif

#include <stdio.h>
#include <algorithm>
#include "Window.h"
#include "Theme.h"
#include "debug.h"

#define DB Debug_v("%s:%d\n", __FILE__, __LINE__);

const int iDesktopSnapAmount = 10;

Window::Window(Theme *pTheme, string &oName)
{
    m_oName = oName;
    m_bExit = false;
    m_pTheme = pTheme;
    m_bWindowMove = false;
    m_bLButtonDown = false;
    m_bStayOnTop = false;
    m_bLiveInToolbar = false;
    m_bIsDockWindow = false;

    m_pCanvas = NULL;
    m_pMouseInControl = NULL;
    m_pCaptureControl = NULL;
    m_pMouseDownControl = NULL;
    m_bIsVulcanMindMeldHost = false;
    m_bMindMeldInProgress = false;    
    m_bNoOutsideMessages = false;

    m_pUsageMutex = new Mutex();
    m_pUsageSem = new Semaphore();
    m_iUsageCount = 0;
}

Window::~Window(void)
{
    vector<Panel *>::iterator i;

    if (!m_bIsVulcanMindMeldHost)
    {
       delete m_pCanvas;
       ClearControls();

       for(i = m_oPanels.begin(); i != m_oPanels.end(); i++)
	     	delete (Panel *)*i;
    }   

    delete m_pUsageMutex;
    delete m_pUsageSem;

}

void Window::IncUsageRef(void)
{
    for(;;)
    {
        m_pUsageMutex->Acquire();
    
        if (m_iUsageCount < 0)
        {
            m_pUsageMutex->Release();
            m_pUsageSem->Wait();
        }
        else
            break;
    }    
       
    m_iUsageCount++;
    
    m_pUsageMutex->Release();
}

void Window::DecUsageRef(void)
{
    m_pUsageMutex->Acquire();
    m_iUsageCount--;
    m_pUsageSem->Signal(); 
    m_pUsageMutex->Release();
}

void Window::LockUsageRef(void)
{
    for(;;)
    {
        m_pUsageMutex->Acquire();
    
        if (m_iUsageCount > 0)
        {
            m_pUsageMutex->Release();
            ProcessWaitingMessages();
            m_pUsageSem->Wait();
        }
        else
            break;
    }    
    m_iUsageCount = -1;
    m_pUsageMutex->Release();
}

void Window::UnlockUsageRef(void)
{
    m_pUsageMutex->Acquire();
    m_iUsageCount = 0;
    m_pUsageSem->Signal(); 
    m_pUsageMutex->Release();
}

void Window::VulcanMindMeldHost(bool bHost)
{
    m_bIsVulcanMindMeldHost = bHost;
    
    if (bHost)
    {
       delete m_pCanvas;
       m_pCanvas = NULL;
    }
}

Error Window::VulcanMindMeld(Window *pOther)
{
    vector<Panel *>::iterator   k;
    string                      oName;

    LockUsageRef();
    m_bNoOutsideMessages = true;

    m_oName = pOther->m_oName;
    m_pTheme = pOther->m_pTheme;

    m_bWindowMove = pOther->m_bWindowMove;
    m_bStayOnTop = pOther->m_bLiveInToolbar;
    m_bIsDockWindow = pOther->m_bIsDockWindow;
    m_oDockPos = pOther->m_oDockPos;

    m_pMouseInControl = NULL;
    m_pCaptureControl = NULL;
    m_pMouseDownControl = NULL;

    m_oPanels.clear();
    for(k = pOther->m_oPanels.begin(); k != pOther->m_oPanels.end(); k++)
    {
        (*k)->SetParentWindow(this);
        m_oPanels.push_back(*k);
    }    

    m_pCanvas = pOther->m_pCanvas;

    UnlockUsageRef();

    Init();   

    m_bNoOutsideMessages = false;

    return kError_NoErr;
}

void Window::Init(void)
{
    vector<Panel *>::iterator i;

    IncUsageRef();

    m_pCanvas->InitBackgrounds(&m_oPanels);
    m_pCanvas->Init();

    for(i = m_oPanels.begin(); i != m_oPanels.end(); i++)
        (*i)->Init();
        
    m_pTheme->InitControls();    
    
    DecUsageRef();
}

void Window::AddPanel(Panel *pPanel)
{
    string oName;

    IncUsageRef();
    m_oPanels.push_back(pPanel);
    DecUsageRef();
}

void Window::PanelStateChanged(void)
{
    vector<Panel *>::iterator i;
    Rect oRect, oWindowRect;

    IncUsageRef();
    m_pCanvas->InitBackgrounds(&m_oPanels);
    m_pCanvas->Init();
    for(i = m_oPanels.begin(); i != m_oPanels.end(); i++)
        (*i)->ShowAllControls();

    GetWindowPosition(oWindowRect);
    m_pCanvas->GetBackgroundRect(oRect);
    oWindowRect.x2 = oWindowRect.x1 + oRect.Width();
    oWindowRect.y2 = oWindowRect.y1 + oRect.Height();
    SetWindowPosition(oWindowRect);

    m_pCanvas->Invalidate(oRect);

    DecUsageRef();
}

void Window::ClearControls(void)
{
    vector<Panel *>::iterator i;

    IncUsageRef();
       
    for(i = m_oPanels.begin(); i != m_oPanels.end(); i++)
        (*i)->ClearControls();

    DecUsageRef();
}

Canvas *Window::GetCanvas(void)
{
    return m_pCanvas;
}

void Window::GetName(string &oName)
{
    oName = m_oName;
}

void Window::SetDockPosition(Pos &oPos)
{
    m_oDockPos = oPos;
    m_bIsDockWindow = true;
}

Error Window::ControlEnable(const string &oTarget, bool bSet, bool &bEnable)
{
    vector<Panel *>::iterator i;

    IncUsageRef();
       
    for(i = m_oPanels.begin(); i != m_oPanels.end(); i++)
        (*i)->ControlEnable(oTarget, bSet, bEnable);

    DecUsageRef();

    return kError_NoErr;
}

Error Window::ControlShow(const string &oTarget, bool bSet, bool &bShow)
{
    vector<Panel *>::iterator i;

    IncUsageRef();

    for(i = m_oPanels.begin(); i != m_oPanels.end(); i++)
        (*i)->ControlShow(oTarget, bSet, bShow);

    DecUsageRef();

    return kError_NoErr;
}

Error Window::ControlIntValue(const string &oTarget, bool bSet, int &iValue)
{
    vector<Panel *>::iterator i;

    IncUsageRef();

    for(i = m_oPanels.begin(); i != m_oPanels.end(); i++)
        (*i)->ControlIntValue(oTarget, bSet, iValue);
       
    DecUsageRef();

    return kError_NoErr;
}

Error Window::ControlStringValue(const string &oTarget, bool bSet, string &oValue)
{
    vector<Panel *>::iterator i;

    IncUsageRef();
       
    for(i = m_oPanels.begin(); i != m_oPanels.end(); i++)
        (*i)->ControlStringValue(oTarget, bSet, oValue);

    DecUsageRef();

    return kError_NoErr;
}

Error Window::ControlGetDesc(const string &oTarget, string &oDesc)
{
    vector<Panel *>::iterator i;

    IncUsageRef();
       
    for(i = m_oPanels.begin(); i != m_oPanels.end(); i++)
        (*i)->ControlGetDesc(oTarget, oDesc);

    DecUsageRef();

    return kError_InvalidParam;
}

Error Window::ControlGetTip(const string &oTarget, string &oTip)
{
    vector<Panel *>::iterator i;

    IncUsageRef();
       
    for(i = m_oPanels.begin(); i != m_oPanels.end(); i++)
        (*i)->ControlGetTip(oTarget, oTip);

    DecUsageRef();

    return kError_InvalidParam;
}

Error Window::SendControlMessage(Control *pControl, 
                                 ControlMessageEnum eMesg)
{
    string oControlName;
    Error  eRet;

    IncUsageRef();

    pControl->GetName(oControlName);

    eRet = m_pTheme->HandleControlMessage(oControlName, eMesg);
    DecUsageRef();
    
    return eRet;
}

bool Window::DoesControlExist(const string &oName)
{
    bool bRet = false;
    vector<Panel *>::iterator i;

    IncUsageRef();
    for(i = m_oPanels.begin(); i != m_oPanels.end() && !bRet; i++)
    {
        bRet = (*i)->DoesControlExist(oName);
    }
    DecUsageRef();
    
    return bRet;
}

Control *Window::ControlFromPos(Pos &oPos)
{
    vector<Panel *>::iterator   i;
    Control                    *pControl = NULL;

    IncUsageRef();
    
    for(i = m_oPanels.begin(); i != m_oPanels.end() && !pControl; i++)
    {
        pControl = (*i)->ControlFromPos(oPos);
    }
    DecUsageRef();

    return pControl;
}

void Window::GetControlToolTips(vector<pair<Rect, string> > &oList)
{
    vector<Panel *>::iterator   i;

    IncUsageRef();
	oList.clear();
    for(i = m_oPanels.begin(); i != m_oPanels.end(); i++)
    {
        (*i)->GetControlToolTips(oList);
    }
    DecUsageRef();
}	 

Error Window::StartMouseCapture(Control *pControl)
{
    IncUsageRef();
    m_pCaptureControl = pControl;
    DecUsageRef();
    
    return CaptureMouse(true);
}

Error Window::EndMouseCapture(void)
{
    IncUsageRef();
    m_pCaptureControl = NULL;
    DecUsageRef();
    
    return CaptureMouse(false);
}

void Window::HandleMouseWheelChange(int iSteps)
{
    m_pTheme->HandleMouseWheelChange(iSteps);
}

void Window::HandleMouseMove(Pos &oScreenPos)
{
    Control *pControl;
    Pos      oPos;
    Rect     oRect;

    IncUsageRef();

    if (m_bWindowMove)
    {
       Rect oActualPos;

       m_oMoveStart.x1 += (oScreenPos.x - m_oMovePos.x);
       m_oMoveStart.x2 += (oScreenPos.x - m_oMovePos.x);
       m_oMoveStart.y1 += (oScreenPos.y - m_oMovePos.y);
       m_oMoveStart.y2 += (oScreenPos.y - m_oMovePos.y);

       oActualPos = m_oMoveStart;

       if (m_iDesktopWidth > 0 && m_iDesktopHeight > 0)
       {
           if ((oActualPos.x1 >= 0 && oActualPos.x1 < iDesktopSnapAmount) || 
               (oActualPos.x1 < 0 && oActualPos.x1 > -iDesktopSnapAmount)) 
           {
               oActualPos.x2 -= oActualPos.x1;
               oActualPos.x1 = 0;           
           }
           if ((oActualPos.y1 >= 0 && oActualPos.y1 < iDesktopSnapAmount) ||
               (oActualPos.y1 < 0 && oActualPos.y1 > -iDesktopSnapAmount)) 
           {
               oActualPos.y2 -= oActualPos.y1;
               oActualPos.y1 = 0;           
           }
           if ((oActualPos.x2 < m_iDesktopWidth && 
                oActualPos.x2 >= m_iDesktopWidth - iDesktopSnapAmount) || 
               (oActualPos.x2 > m_iDesktopWidth && 
                oActualPos.x2 <= m_iDesktopWidth + iDesktopSnapAmount)) 
           {
               oActualPos.x1 += m_iDesktopWidth - oActualPos.x2;
               oActualPos.x2 = m_iDesktopWidth;           
           }
           if ((oActualPos.y2 < m_iDesktopHeight && 
                oActualPos.y2 >= m_iDesktopHeight - iDesktopSnapAmount) || 
               (oActualPos.y2 > m_iDesktopHeight && 
                oActualPos.y2 <= m_iDesktopHeight + iDesktopSnapAmount)) 
           {
               oActualPos.y1 += m_iDesktopHeight - oActualPos.y2;
               oActualPos.y2 = m_iDesktopHeight;           
           }
       }       

       m_oMovePos = oScreenPos;
       SetWindowPosition(oActualPos);
       DecUsageRef();
       
       return; 
    }

    if (m_bLButtonDown && !LButtonDown())
    {
       m_bLButtonDown = false;
       m_pMouseDownControl = NULL;
    }

    GetWindowPosition(oRect);
    oPos.x = oScreenPos.x - oRect.x1;
    oPos.y = oScreenPos.y - oRect.y1;
   
    if (m_pCaptureControl)
    {
       m_pCaptureControl->AcceptTransition(CT_MouseMove, &oPos);
       DecUsageRef();
       return;
    }

    pControl = ControlFromPos(oPos);
    if (pControl)
    {
       if (m_pMouseDownControl && m_pMouseInControl &&
           m_pMouseDownControl != pControl)
       {
           m_pMouseInControl->AcceptTransition(CT_MouseLeave);
           m_pMouseInControl = NULL;
       }    
       else
       if (m_pMouseDownControl && m_pMouseInControl == NULL &&
           m_pMouseDownControl == pControl)
       {
           m_pMouseInControl = m_pMouseDownControl;
           m_pMouseInControl->AcceptTransition(CT_MouseEnter);
           m_pMouseInControl->AcceptTransition(CT_MouseLButtonDown);
       }    
       else
       if (!m_bLButtonDown)
       {
           if (m_pMouseInControl && m_pMouseInControl != pControl)
           {
               m_pMouseInControl->AcceptTransition(CT_MouseLeave);
               m_pMouseInControl = pControl;
               m_pMouseInControl->AcceptTransition(CT_MouseEnter);
               if (m_bLButtonDown)
               {
                  m_pMouseInControl->AcceptTransition(CT_MouseLButtonDown);
               }   
           }
           else
           if (m_pMouseInControl == NULL)
           {
               m_pMouseInControl = pControl;
               m_pMouseInControl->AcceptTransition(CT_MouseEnter);
               if (m_bLButtonDown)
               {
                  m_pMouseInControl->AcceptTransition(CT_MouseLButtonDown);
               }
           }
       }

       pControl->AcceptTransition(CT_MouseMove, &oPos);
       DecUsageRef();
       return;
    }
    else
    {
       if (m_pMouseInControl)
       {
           m_pMouseInControl->AcceptTransition(CT_MouseLeave);
           m_pMouseInControl = NULL;
       }
    }
    DecUsageRef();

    return;
}

void Window::HandleMouseLButtonDown(Pos &oScreenPos)
{
    Control *pControl;
    Rect     oRect;
    Pos      oPos;

    IncUsageRef();

    GetWindowPosition(oRect);
    oPos.x = oScreenPos.x - oRect.x1;
    oPos.y = oScreenPos.y - oRect.y1;

    if (m_pCaptureControl)
    {
       m_pCaptureControl->AcceptTransition(CT_MouseLButtonDown, &oPos);
       DecUsageRef();
       return;
    }

    pControl = ControlFromPos(oPos);
    if (pControl && pControl->UseToDragWindow())
       pControl = NULL;

    if (pControl)
    {
       m_bLButtonDown = true;
       if (m_pMouseInControl != pControl) {
           if (m_pMouseInControl)
               m_pMouseInControl->AcceptTransition(CT_MouseLeave);
           m_pMouseInControl = pControl;
           m_pMouseInControl->AcceptTransition(CT_MouseEnter);
       }
       m_pMouseDownControl = pControl;
       pControl->AcceptTransition(CT_MouseLButtonDown, &oPos);
       DecUsageRef();
       return;
    }

#ifdef __QNX__
    DecUsageRef();
    return;
#endif

    m_bWindowMove = true;
#ifndef HAVE_GTK
    CaptureMouse(true);
#endif
       
    GetWindowPosition(m_oMoveStart);
    m_pCanvas->GetBackgroundRect(oRect);
    m_oMoveStart.x2 = m_oMoveStart.x1 + oRect.Width();
    m_oMoveStart.y2 = m_oMoveStart.y1 + oRect.Height();
    m_oMovePos = oScreenPos;

    if (IsError(GetDesktopSize(m_iDesktopWidth, m_iDesktopHeight)))
       m_iDesktopWidth = m_iDesktopHeight = 0;

    DecUsageRef();

    return;
}

void Window::HandleMouseLButtonDoubleClick(Pos &oScreenPos)
{
    Control *pControl;
    Rect     oRect;
    Pos      oPos;

    IncUsageRef();

    GetWindowPosition(oRect);
    oPos.x = oScreenPos.x - oRect.x1;
    oPos.y = oScreenPos.y - oRect.y1;

    pControl = ControlFromPos(oPos);
    if (pControl)
        m_pMouseInControl->AcceptTransition(CT_MouseLButtonDoubleClick);

    DecUsageRef();

    return;
}

void Window::HandleMouseLButtonUp(Pos &oScreenPos)
{
    Control *pControl;
    Pos      oPos;
    Rect     oRect;

    IncUsageRef();

    GetWindowPosition(oRect);
    oPos.x = oScreenPos.x - oRect.x1;
    oPos.y = oScreenPos.y - oRect.y1;

    if (m_bWindowMove)
    {
       m_bWindowMove = false;
#ifndef HAVE_GTK
       CaptureMouse(false);
#endif        
       DecUsageRef();
  
       return; 
    }

    m_bLButtonDown = false;
    m_pMouseDownControl = NULL;
    if (m_pCaptureControl)
    {
       m_pCaptureControl->AcceptTransition(CT_MouseLButtonUp, &oPos);
       DecUsageRef();
       return;
    }

    pControl = ControlFromPos(oPos);
    if (pControl)
    {
       pControl->AcceptTransition(CT_MouseLButtonUp, &oPos);
       DecUsageRef();
       return;
    }

    DecUsageRef();
    
    return;
}

void Window::HandleMouseMButtonDown(Pos &oScreenPos)
{
    return;
}

void Window::HandleMouseMButtonUp(Pos &oScreenPos)
{
    m_pTheme->HandleMouseWheelClick();
    return;
}

void Window::MouseHasEnteredWindow(void)
{
    IncUsageRef();
    string StatusControlName = string("WindowStatus");
    m_pTheme->HandleControlMessage(StatusControlName, CM_WindowEnter);
    DecUsageRef();
}

// Do you have any idea how much I wanted to call 
// this ElvisHasLeftTheBuilding()?
void Window::MouseHasLeftWindow(void)
{
    IncUsageRef();

    string StatusControlName = string("WindowStatus");
    m_pTheme->HandleControlMessage(StatusControlName, CM_WindowLeave);

    if (m_pMouseInControl)
    {
       m_pMouseInControl->AcceptTransition(CT_MouseLeave);
       m_pMouseInControl = NULL;
    }
    else if (!m_pMouseInControl)
    { 
       Pos oPos;

       GetMousePos(oPos);
       m_pMouseInControl = ControlFromPos(oPos);
       if (m_pMouseInControl) {
          m_pMouseInControl->AcceptTransition(CT_MouseEnter);
          m_pTheme->HandleControlMessage(StatusControlName, CM_WindowEnter);
       }
    }      
    DecUsageRef();
}

void Window::TimerEvent(void)
{
    vector<Panel *>::iterator i;

    if (!m_bTimerEnabled)
       return;

    IncUsageRef();

    for(i = m_oPanels.begin(); i != m_oPanels.end(); i++)
        (*i)->TimerEvent();
    
    DecUsageRef();
}

void Window::EnableTimer(bool bEnable)
{
    m_bTimerEnabled = bEnable;
}  

void Window::SetStayOnTop(bool bStay)
{
    m_bStayOnTop = bStay;
}

void Window::SetLiveInToolbar(bool bLive)
{
    m_bLiveInToolbar = bLive;
}

void Window::Keystroke(unsigned char cKey)
{
    m_pTheme->HandleKeystroke(cKey);
}

bool Window::MenuCommand(uint32 uCommand)
{
    bool bRet;

    IncUsageRef();
    bRet = m_pTheme->HandleMenuCommand(uCommand);
    DecUsageRef();
    
    return bRet;
}

void Window::VolumeChanged(void)
{
    IncUsageRef();
    m_pTheme->VolumeChanged();
    DecUsageRef();
}

void Window::GetReloadWindowPos(Rect &oOldRect, int iNewWidth, int iNewHeight, 
                                Rect &oNewRect)
{
    int32 iSizeX, iSizeY;
 
    oNewRect = oOldRect;
    oNewRect.x2 = oNewRect.x1 + iNewWidth;
    oNewRect.y2 = oNewRect.y1 + iNewHeight;

    GetDesktopSize(iSizeX, iSizeY);
    if (oNewRect.x2 > iSizeX)
    {
       oNewRect.x1 -= oNewRect.x2 - iSizeX;
       oNewRect.x2 -= oNewRect.x2 - iSizeX;
    }   
    if (oNewRect.y2 > iSizeY)
    {
       oNewRect.y1 -= oNewRect.y2 - iSizeY;
       oNewRect.y2 -= oNewRect.y2 - iSizeY;
    }
    if (oNewRect.x1 < 0)
    {
       oNewRect.x2 -= oNewRect.x1;
       oNewRect.x1 = 0;
    }   
    if (oNewRect.y1 < 0)
    {
       oNewRect.y2 -= oNewRect.y1;
       oNewRect.y1 = 0;
    }   
   
    if (oOldRect.x2 == iSizeX)
    {
       oNewRect.x2 = iSizeX;
       oNewRect.x1 = iSizeX - iNewWidth;
    }
    if (oOldRect.y2 == iSizeY)
    {
       oNewRect.y2 = iSizeY;
       oNewRect.y1 = iSizeY - iNewHeight;
    }
    if (oOldRect.x1 == 0)
    {
       oNewRect.x1 = 0;
       oNewRect.x2 = iNewWidth;
    }
    if (oOldRect.y2 == 0)
    {
       oNewRect.y1 = 0;
       oNewRect.y2 = iNewHeight;
    }
}
