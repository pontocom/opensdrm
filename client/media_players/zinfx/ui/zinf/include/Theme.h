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

   $Id: Theme.h,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_THEME_H__
#define INCLUDED_THEME_H__

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <string>
#include <vector>
#include <map>

using namespace std;

#include "facontext.h"
#include "Parse.h"
#include "Window.h"
#include "Bitmap.h"
#include "Font.h"
#include "ThemeManager.h"
#include "Headlines.h"
#include "ForeignTheme.h"

typedef map<string, string, less<string> > AttrMap;

enum ControlTypeEnum
{
    eButtonControl,
    eTextControl,
    eSliderControl,
    eVSliderControl,
    eDialControl,
    eMultiStateControl,
    ePixFontControl,
    ePixTimeControl,
    ePixSliderControl,
    
    eUndefinedControl
};

class Theme : public Parse
{
    public:

               Theme(FAContext *context);
      virtual ~Theme(void);

      virtual Error SwitchWindow(const string &oWindowName); 
      virtual Error Run(Pos &oWindowPos); 
      virtual Error Close(void);
      virtual Error LoadTheme(string &oFile, string &oWindowName);
      virtual Error HandleControlMessage(string &oControlName, 
                                         ControlMessageEnum eMesg) = 0;
      virtual void  HandleKeystroke(unsigned char cKey) = 0;
      virtual bool  HandleMenuCommand(uint32 uCommand) = 0;
      virtual void  HandleMouseWheelChange(int iSteps) = 0;
      virtual void  HandleMouseWheelClick(void) = 0;
      virtual void  VolumeChanged(void) = 0;
      virtual void  InitControls(void) = 0;
      virtual void  InitWindow(void) = 0;
      virtual void  PostWindowCreate(void);
      virtual void  DropFiles(vector<string> *pFileList) = 0;
      virtual void  SetDefaultFont(const string &oFont);

    protected:
     
      virtual void  SetThemePath(string &oPath);
      virtual Error BeginElement(string &oElement, AttrMap &oAttrMap);  
      virtual Error PCData(string &oData);
      virtual Error EndElement(string &oElement);
      void    ShowThemeCredits(void);
	  
      ThemeManager *m_pThemeMan;
      string        m_oThemePath;
      Window       *m_pWindow;
      FAContext    *m_pContext;
      Headlines    *m_pHeadlineGrabber;
      HeadlineInfo *m_pHeadlines, *m_pParsedHeadlines;

    private:

      Bitmap *FindBitmap(string &oName);
      Font   *FindFont(string &oName);
      Panel  *FindPanel(string &oName);
      Error   ParseState(string &oStateString, ControlStateEnum &eState);
      Error   ParseRect(string &oRectString, Rect &oRect);
      Error   ParseColor(string &oColorString, Color &oColor);
      Error   ParsePos(string &oPosString, Pos &oPos);
      void    ClearWindows(void);
      void    ClearBitmaps(void);
      void    ClearFonts(void);

      void    ConvertForeignFormat(string &oDir);
      ForeignTheme    *m_pForeignThemes;

      Window          *m_pCurrentWindow;
      Control         *m_pCurrentControl;
      Panel           *m_pCurrentPanel;

      vector<Window *> *m_pWindows, *m_pParsedWindows;
      vector<Bitmap *> *m_pBitmaps, *m_pParsedBitmaps;
      vector<Font *>   *m_pFonts, *m_pParsedFonts;
      vector<Panel *>   m_oPanels;
      bool              m_bReloadTheme, m_bReloadWindow, m_bThemeLoaded;
      bool              m_bPosDefined, m_bBitmapDefined;
      bool              m_bInfoDefined;
      string            m_oReloadWindow, m_oReloadFile;
      ControlTypeEnum   m_eCurrentControl;
      string            m_oDefaultFont;
      string            m_oThemeName, m_oThemeAuthor;
      string            m_oAuthorEMail, m_oAuthorWebPage;
      string            m_oSavedText, m_oMiscInfo;
      bool              m_bCreditsShown;
};

#endif
