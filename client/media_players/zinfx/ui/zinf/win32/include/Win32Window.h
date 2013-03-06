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

   $Id: Win32Window.h,v 1.1 2004/10/13 16:17:07 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_WIN32WINDOW__H_
#define INCLUDED_WIN32WINDOW__H_

#include <string>
#include <vector>
#include <map>
#include <deque>
#include <windows.h>
#include <winuser.h>
#include <shellapi.h>
#include <commctrl.h>
#include <mmsystem.h>

#include "Window.h"

using namespace std;

class Bitmap;
class Win32Window : public Window
{
    public:

              Win32Window(Theme *pTheme, string &oName);
     virtual ~Win32Window(void);

     virtual void  Init(void);
     virtual Error VulcanMindMeld(Window *pOther);
     virtual Error Run(Pos &oWindowPos);
     virtual Error Close(void);
     virtual Error Show(void);
     virtual Error Hide(void);
     virtual Error Enable(void);
     virtual Error Disable(void);
     virtual Error SetTitle(string &oTitle);
     virtual Error CaptureMouse(bool bCapture);
     virtual Error HideMouse(bool bHide);
     virtual Error SetMousePos(Pos &oPos);
     virtual Error GetMousePos(Pos &oPos);
     virtual Error SetWindowPosition(Rect &oWindowRect);
     virtual Error GetWindowPosition(Rect &oWindowRect);
     virtual Error Minimize(void);
     virtual Error Restore(void);
     virtual bool  LButtonDown(void);
     virtual Error GetDesktopSize(int32 &iX, int32 &iY);
     virtual void  BringWindowToFront(void);
     virtual void  PanelStateChanged(void);

	 virtual HWND  GetWindowHandle(void);
     LRESULT       WindowProc(HWND hwnd, UINT msg, 
                               WPARAM wParam, LPARAM lParam);

             void  ConvertTo256Color(vector<Bitmap *> *pList);
             void  Create256ColorPalette(BYTE pColorMap[236][3], 
                                         RGBQUAD *pWinColorMap);

    protected:
     
     void  SaveWindowPos(Pos &oPos);
     void  DropFiles(HDROP dropHandle);
     void  Notify(int32 command, LPNMHDR notifyMsgHdr);
     void  CreateTooltips(void);
     void  Paint(void);
     void  TimerEvent(void);
     void  SetStayOnTop(bool bStay);
     void  SetLiveInToolbar(bool bLive);
	 void  MouseLeaveCheck(void);
     void  AddToSystemMenu(HWND hWnd);
     void  ProcessWaitingMessages(void);
    
     HWND     m_hWnd;
	 HPALETTE m_hPal;
     Pos      m_oWindowPos;
     Mutex   *m_pMindMeldMutex;
	 bool     m_bMouseInWindow;
     bool     m_bMindMeldInProgress;
};

#endif
