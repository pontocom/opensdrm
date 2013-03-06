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

   $Id: Theme.cpp,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/ 

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#include "config.h"

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#ifndef _S_IFDIR
#define _S_IFDIR S_IFDIR
#endif
#define _stat stat
#endif
#include <map>
#include <algorithm>
#include <assert.h>
#include "Theme.h"
#include "ButtonControl.h"
#include "DialControl.h"
#include "SliderControl.h"
#include "VSliderControl.h"
#include "TextControl.h"
#include "MultiStateControl.h"
#include "PixFontControl.h"
#include "PixTimeControl.h"
#include "PixSliderControl.h"
#include "ThemeZip.h"
#include "MessageDialog.h"
#include "debug.h"
#include "ForeignTheme.h"
#include "utility.h"

#define DB Debug_v("%s:%d\n", __FILE__, __LINE__);

#ifdef WIN32
#include <direct.h>
//#define stat(a,b) _stat(a,b)
#define rmdir(a) _rmdir(a)
#define MKDIR(z) mkdir(z)
#else
#define MKDIR(z) mkdir(z, 0755)
#endif

#ifdef __QNX__
#include <strings.h>
#endif

#ifdef WIN32
#include "Win32Window.h"
#include "Win32Bitmap.h"
#include "Win32Font.h"
#elif defined(HAVE_GTK)
#include "GTKWindow.h"
#include "GTKBitmap.h"
#include "GTKFont.h"
#include "GTKUtility.h"
#elif defined(__BEOS__)
#include "BeOSWindow.h"
#include "BeOSBitmap.h"
#include "BeOSFont.h"
#endif

const int iThemeVersionMajor = 1;
const int iThemeVersionMinor = 0;
const char *szThemeNotFoundError = "Cannot open the theme specified in the preferences. Using default theme instead.";
const char *szThemeUnzipError = "Cannot unzip the specified theme. Using default theme instead.";
const char *szCannotCreateTempDirError = "Cannot create a temporary directory to unzip theme.";

Theme::Theme(FAContext *context)
{
    m_pContext = context;
    m_pCurrentWindow = NULL;
    m_pCurrentControl = NULL;
    m_pCurrentPanel = NULL;
    m_pParsedWindows = m_pWindows = NULL;
    m_pParsedBitmaps = m_pBitmaps = NULL;
    m_pParsedFonts = m_pFonts = NULL;
    m_pParsedHeadlines = m_pHeadlines = NULL;
    m_bReloadTheme = false;
    m_bReloadWindow = false;
    m_oReloadWindow = string("");
    m_eCurrentControl = eUndefinedControl;
    m_pThemeMan = new ThemeManager(m_pContext);
    m_pForeignThemes = new ForeignTheme(m_pContext);
    m_bThemeLoaded = false;
    m_bCreditsShown = false;
    m_pHeadlineGrabber = NULL;
    
    string funkyName = "Frunobulax"; 
#ifdef WIN32
    m_pWindow = new Win32Window(this, funkyName);
#elif defined(HAVE_GTK)
    InitializeGTK(context); 
    m_pWindow = new GTKWindow(this, funkyName);
#elif defined(__BEOS__)
    m_pWindow = new BeOSWindow(this, funkyName);
#endif
    m_pWindow->VulcanMindMeldHost(true);
}

Theme::~Theme(void)
{
    delete m_pCurrentWindow;
    delete m_pCurrentControl;
    delete m_pCurrentPanel;
    delete m_pThemeMan;
    delete m_pWindow;
    delete m_pForeignThemes;   
 
    ClearWindows();
    ClearBitmaps();
    ClearFonts();

    // We're not going to delete the headline grabber, since it might
    // be blocked in a read. The OS will clean up after us. Naughty...
    delete m_pHeadlines;

#ifdef HAVE_GTK
    ShutdownGTK();
#endif
}

void Theme::ClearWindows(void)
{
    if (m_pWindows)
    {
       while(m_pWindows->size() > 0)
       {
          delete (*m_pWindows)[0];
          m_pWindows->erase(m_pWindows->begin());
       }
       delete m_pWindows;
       m_pWindows = NULL;
    }   
}

void Theme::ClearBitmaps(void)
{  
    if (m_pBitmaps)
    {
       while(m_pBitmaps->size() > 0)
       {
          delete (*m_pBitmaps)[0];
          m_pBitmaps->erase(m_pBitmaps->begin());
       }
       delete m_pBitmaps;
       m_pBitmaps = NULL;
    }
}

void Theme::ClearFonts(void)
{  
    if (m_pFonts)
    {
       while(m_pFonts->size() > 0)
       {
          delete (*m_pFonts)[0];
          m_pFonts->erase(m_pFonts->begin());
       }
       delete m_pFonts;
       m_pFonts = NULL;
    }
}

void Theme::SetThemePath(string &oThemePath)
{
    m_oThemePath = oThemePath + string(DIR_MARKER_STR);
}

Error Theme::LoadTheme(string &oFile, string &oWindowName)
{
    char     *pTemp;
    ThemeZip *pZip = NULL;
    string    oCompleteFile, oTempPath, oDefaultPath;
    Error     eRet;
    struct    _stat buf;

    if (m_pWindow)
       m_pWindow->EnableTimer(false);

    if (_stat(oFile.c_str(), &buf) == 0 && (buf.st_mode & _S_IFDIR))
    {
        SetThemePath(oFile);
        ConvertForeignFormat(oFile);
        oCompleteFile = oFile + string(DIR_MARKER_STR) + string("theme.xml");
        eRet = Parse::ParseFile(oCompleteFile);
    }
    else
    {
        pTemp = tempnam(NULL, "fat");
        oTempPath = pTemp;
        free(pTemp);
        if (MKDIR(oTempPath.c_str()))
        {
            MessageDialog oBox(m_pContext);
            string        oErr, oMessage(szCannotCreateTempDirError);

            oBox.Show(oMessage.c_str(), string(BRANDING), kMessageOk);
            return kError_InvalidParam;
        }    
        SetThemePath(oTempPath);

        pZip = new ThemeZip();
        eRet = pZip->DecompressTheme(oFile, oTempPath);
        if (eRet == kError_FileNotFound)
        {
            string oDefaultPath;
        
            if (oFile.length() > 0)
            {
                MessageDialog oBox(m_pContext);
       
                string        oErr, oMessage(szThemeNotFoundError);

                oBox.Show(oMessage.c_str(), string(BRANDING), kMessageOk);
            }
       
            m_pThemeMan->GetDefaultTheme(oFile);
            eRet = pZip->DecompressTheme(oFile, oTempPath);
            if (IsError(eRet))
            {
                m_oLastError = "Cannot find default theme";
                rmdir(oTempPath.c_str());
                
                delete pZip;
                return kError_InvalidParam;
            }
            m_pThemeMan->UseTheme(oFile);
        }

        if (IsError(eRet))
        {
            MessageDialog oBox(m_pContext);
            string        oErr, oMessage(szThemeUnzipError);

            oBox.Show(oMessage.c_str(), string(BRANDING), kMessageOk);
            
            m_pThemeMan->GetDefaultTheme(oFile);
            eRet = pZip->DecompressTheme(oFile, oTempPath);
            if (IsError(eRet))
            {
                m_oLastError = "Cannot find default theme";
                rmdir(oTempPath.c_str());
                delete pZip;
                return kError_InvalidParam;
            }    
        }    

        ConvertForeignFormat(oTempPath);

        oCompleteFile = oTempPath + string(DIR_MARKER_STR) 
                    + string("theme.xml");
        eRet = Parse::ParseFile(oCompleteFile);
        pZip->CleanupThemeZip(oTempPath);
        rmdir(oTempPath.c_str());
        delete pZip;
    }    

	if (!IsError(eRet))
    {
       string                      oTemp;
       vector<Window *>::iterator  i;
       Window                     *pMainWindow, *pNewWindow = NULL; 
  
      
       // Is this a reload, as opposed to a new load?
       if (m_pWindows)
       {
          vector<Window *> *oWindows = NULL;
          vector<Bitmap *> *oBitmaps = NULL;
          vector<Font *>   *oFonts = NULL;
          
          oWindows = m_pWindows;

          // Adopt the new window vector
          m_pWindows = m_pParsedWindows;

          // Select the new window, based on the passed in name
          for(i = m_pWindows->begin(); i != m_pWindows->end(); i++)
          {
              (*i)->GetName(oTemp);
              if (oTemp == string("MainWindow"))
                  pMainWindow = *i;
              if (oTemp == oWindowName)
              {
                  pNewWindow = *i;
                  break;
              }    
          }

          // If we can't find the proper mode, switch to mainwindow
          if (!pNewWindow) 
             pNewWindow = pMainWindow;

          // Save the old bitmaps and fonts
          oFonts = m_pFonts;
          oBitmaps = m_pBitmaps;

          // Accept the new bitmaps and font.
          m_pBitmaps = m_pParsedBitmaps;
          m_pFonts = m_pParsedFonts;
          m_pHeadlines = m_pParsedHeadlines;
          m_pParsedWindows = NULL;
          m_pParsedBitmaps = NULL;
          m_pParsedFonts = NULL;
          m_pParsedHeadlines = NULL;
         
#ifdef WIN32         
          ((Win32Window *)m_pWindow)->ConvertTo256Color(m_pBitmaps);
#endif          
          
          // Now the window lists have been properly adjusted, so
          // adopt all the info from the new window into the existing
          // window via the VulcanMindLink 
          m_pWindow->VulcanMindMeld(pNewWindow);
          // And if god doesn't stike me down right now,
          // everything *should* be fine.

          // Now delete the old crap
          while(oFonts && oFonts->size() > 0)
          {
             delete (*oFonts)[0];
             oFonts->erase(oFonts->begin());
          }
          delete oFonts;
          while(oBitmaps->size() > 0)
          {
             delete (*oBitmaps)[0];
             oBitmaps->erase(oBitmaps->begin());
          }
          delete oBitmaps;
          while(oWindows->size() > 0)
          {
             delete (*oWindows)[0];
             oWindows->erase(oWindows->begin());
          }
          delete oWindows;
       }
       else
       {
          // Its a new load -- just take things verbatim and run
          m_pWindows = m_pParsedWindows;
          m_pBitmaps = m_pParsedBitmaps;
          m_pFonts = m_pParsedFonts;
          m_pHeadlines = m_pParsedHeadlines;
          m_pParsedWindows = NULL;
          m_pParsedBitmaps = NULL;
          m_pParsedFonts = NULL;
          m_pParsedHeadlines = NULL;

          for(i = m_pWindows->begin(); i != m_pWindows->end(); i++)
          {
              (*i)->GetName(oTemp);
              if (oTemp == oWindowName)
              {
                  pNewWindow = *i;
                  break;
              }    
              if (oTemp == string("MainWindow"))
                  pMainWindow = *i;
          }
          if (!pNewWindow)
             pNewWindow = pMainWindow;
             
#ifdef WIN32         
          ((Win32Window *)m_pWindow)->ConvertTo256Color(m_pBitmaps);
#endif       
  
          m_pWindow->VulcanMindMeld(pNewWindow);
       }   
       m_bThemeLoaded = true;
    }   

    if (m_pWindow)
       m_pWindow->EnableTimer(true);
       
    return eRet;
}

Error Theme::SwitchWindow(const string &oWindowName)
{
    string                      oTemp;
    vector<Window *>::iterator  i;
    Window                     *pMainWindow, *pNewWindow = NULL; 
    
    for(i = m_pWindows->begin(); i != m_pWindows->end(); i++)
    {
        (*i)->GetName(oTemp);
        if (oTemp == oWindowName)
        {
            pNewWindow = *i;
            break;
        }    
        if (oTemp == string("MainWindow"))
              pMainWindow = *i;
    }
    if (!pNewWindow)
       pNewWindow = pMainWindow;

    return m_pWindow->VulcanMindMeld(pNewWindow);
}

Error Theme::Run(Pos &oWindowPos)
{
    if (!m_bThemeLoaded)
       return kError_YouScrewedUp;
       
    InitWindow();
    return m_pWindow->Run(oWindowPos);
}

Error Theme::Close(void)
{
    if (m_pWindow)
       return m_pWindow->Close();
    else
       return kError_NoErr;
}

Error Theme::BeginElement(string &oElement, AttrMap &oAttrMap)
{
    Error  eRet;
    string oCompleteFile;

    if (oElement == string("Version"))
    {
       int iRet, iVersionMajor, iVersionMinor;

       iRet = sscanf(oAttrMap["Version"].c_str(), " %d.%d", &iVersionMajor,
                                                    &iVersionMinor);
       if (iRet != 2)
       {
           m_oLastError = string("Improperly formatted version number "
                                 "in the <Version> tag");
           return kError_ParseError;
       }

       if (iVersionMajor != iThemeVersionMajor ||
           iVersionMinor != iThemeVersionMinor)
       {
           char szError[512];

           
           sprintf(szError, "This ThemeManager cannot use version %d.%d "
                            "themes", iVersionMajor, iVersionMinor);
           m_oLastError = string(szError);

           return kError_ParseError;
       }

       return kError_NoErr;
    }

    if (oElement == string("Bitmap"))
    {
       Bitmap *pBitmap;
       Color   oColor;

#ifdef WIN32
       pBitmap = new Win32Bitmap(oAttrMap["Name"]);
#elif defined(HAVE_GTK)
       pBitmap = new GTKBitmap(oAttrMap["Name"]);
#elif defined(__BEOS__)
       pBitmap = new BeOSBitmap(oAttrMap["Name"]);
#endif

       if (oAttrMap.find("TransColor") != oAttrMap.end())
       {
           eRet = ParseColor(oAttrMap["TransColor"], oColor);
           if (eRet == kError_NoErr)
               pBitmap->SetTransColor(oColor);
       }

           
       if (oAttrMap.find("File") == oAttrMap.end())
       {
           m_oLastError = string("the <Bitmap> tag needs a File attribute");
           return kError_ParseError;
       }        
 
       if (IsRelative(oAttrMap["File"].c_str()))
           oCompleteFile = m_oThemePath + oAttrMap["File"];
       else
           oCompleteFile = oAttrMap["File"];

       eRet = pBitmap->LoadBitmapFromDisk(oCompleteFile);
       if (eRet != kError_NoErr)
       {
           string oBitmapErr;

           pBitmap->GetErrorString(oBitmapErr);
           m_oLastError = string("Cannot load bitmap ") +
                          oAttrMap["File"] + string(": ") + oBitmapErr;
           return eRet;
       }

       if (!m_pParsedBitmaps)
           m_pParsedBitmaps = new vector<Bitmap *>;
           
       m_pParsedBitmaps->push_back(pBitmap);

       return kError_NoErr;
    }
    if (oElement == string("Font"))
    {
       Font *pFont;

       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <Font> tag needs a Name attribute");
           return kError_ParseError;
       }        
       if (oAttrMap.find("Face") == oAttrMap.end() &&
           oAttrMap.find("File") == oAttrMap.end())
       {
           m_oLastError = string("the <Font> tag needs a Face or File attribute");
           return kError_ParseError;
       }        

       if (oAttrMap.find("File") != oAttrMap.end())
           oCompleteFile = m_oThemePath + oAttrMap["File"];
       else
           oCompleteFile = "";    

#ifdef WIN32
       pFont = new Win32Font(oAttrMap["Name"], oAttrMap["Face"], 
                             oCompleteFile, m_oDefaultFont);
#elif defined (HAVE_GTK)
       pFont = new GTKFont(m_pContext, oAttrMap["Name"], oAttrMap["Face"], 
                           oCompleteFile, m_oDefaultFont);
#elif defined (__BEOS__)
       pFont = new BeOSFont(oAttrMap["Name"], oAttrMap["Face"], 
                           oCompleteFile, m_oDefaultFont);
#endif
       if (!m_pParsedFonts)
           m_pParsedFonts = new vector<Font *>;
           
       m_pParsedFonts->push_back(pFont);

       return kError_NoErr;
    }
    if (oElement == string("Window"))
    {
       if (m_pCurrentWindow)
       {
           m_oLastError = string("<Window> tags cannot be nested");
           return kError_InvalidParam;
       }
       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <Window> tag needs a Name attribute");
           return kError_ParseError;
       }        

#ifdef WIN32
       m_pCurrentWindow = new Win32Window(this, oAttrMap["Name"]);
#elif defined(HAVE_GTK)
       m_pCurrentWindow = new GTKWindow(this, oAttrMap["Name"]);
#elif defined(__BEOS__)
       m_pCurrentWindow = new BeOSWindow(this, oAttrMap["Name"]);
#endif

       return kError_NoErr;
    }
    if (oElement == string("BackgroundBitmap"))
    {
       Canvas *pCanvas;
       Bitmap *pBitmap;
       Rect    oRect;

       if (m_pCurrentWindow == NULL)
       {
           m_oLastError = string("<BackgroundBitmap> must occur inside "
                                 "of a <Window> tag");
           return kError_InvalidParam;
       }

       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <BackgroundBitmap> tag needs a Name attribute");
           return kError_ParseError;
       }        

       pBitmap = FindBitmap(oAttrMap["Name"]);
       if (pBitmap == NULL)
       {
           m_oLastError = string("Undefined bitmap ") + 
                          oAttrMap["Name"] + 
                          string("in tag <BackgroundBitmap>");
           return kError_InvalidParam;
       }

       if (oAttrMap.find("Rect") == oAttrMap.end())
       {
           m_oLastError = string("the <BackgroundBitmap> tag needs a Rect attribute");
           return kError_ParseError;
       }        
       
       eRet = ParseRect(oAttrMap["Rect"], oRect);
       if (eRet != kError_NoErr)
       {
           m_oLastError = string("Improperly formatted rect coordinates: ") +
                          oAttrMap["Rect"];
           return kError_InvalidParam;
       }
       pCanvas = m_pCurrentWindow->GetCanvas();
       pCanvas->SetBackgroundBitmap(pBitmap);
       pCanvas->SetBackgroundRect(oRect);

       return kError_NoErr;
    }
    
    if (oElement == string("Panel"))
    {
       Rect    oRect;
       Pos     oPos, oTogglePos;
       Bitmap *pBitmap;
       Canvas *pCanvas;

       oTogglePos.x = -1;
       oTogglePos.y = -1;


       pCanvas = m_pCurrentWindow->GetCanvas();
       if (pCanvas->GetBackgroundBitmap())
       {
           m_oLastError = string("Do not use a <BackgroundBitmap> tag in "
                                 "conjunction with a <Panel>."); 
           return kError_InvalidParam;
       }

       if (m_pCurrentWindow == NULL)
       {
           m_oLastError = string("<Panel> must occur inside "
                                 "of a <Window> tag");
           return kError_InvalidParam;
       }

       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <Panel> tag needs a Name attribute");
           return kError_ParseError;
       }        
       if (oAttrMap.find("ZOrder") == oAttrMap.end())
       {
           m_oLastError = string("the <Panel> tag needs a ZOrder attribute");
           return kError_ParseError;
       }        
       if (oAttrMap.find("Rect") == oAttrMap.end())
       {
           m_oLastError = string("the <Panel> tag needs a Rect attribute");
           return kError_ParseError;
       }        
       if (oAttrMap.find("Pos") == oAttrMap.end())
       {
           m_oLastError = string("the <Panel> tag needs a Pos attribute");
           return kError_ParseError;
       }        
       if (oAttrMap.find("Bitmap") == oAttrMap.end())
       {
           m_oLastError = string("the <Panel> tag needs a Name attribute");
           return kError_ParseError;
       }        
       pBitmap = FindBitmap(oAttrMap["Bitmap"]);
       if (pBitmap == NULL)
       {
           m_oLastError = string("Undefined bitmap ") +
                          oAttrMap["Name"] +
                          string(" in tag <Panel>");
           return kError_InvalidParam;
       }

       eRet = ParseRect(oAttrMap["Rect"], oRect);
       if (eRet != kError_NoErr)
       {
           m_oLastError = string("Improperly formatted Rect coordinates: ") +
                          oAttrMap["Rect"];
           return kError_InvalidParam;
       }
       eRet = ParsePos(oAttrMap["Pos"], oPos);
       if (eRet != kError_NoErr)
       {
           m_oLastError = string("Improperly formatted Pos coordinates: ") +
                          oAttrMap["Pos"];
           return kError_InvalidParam;
       }

       m_pCurrentPanel = FindPanel(oAttrMap["Name"]);
       m_pCurrentPanel->SetPanelBitmap(pBitmap); 
       m_pCurrentPanel->SetZOrder(atoi(oAttrMap["ZOrder"].c_str())); 
       m_pCurrentPanel->SetRect(oRect);
       m_pCurrentPanel->SetPos(oPos);
       m_pCurrentPanel->SetParentWindow(m_pCurrentWindow);

       if (oAttrMap.find("TogglePos") != oAttrMap.end())
       {
           eRet = ParsePos(oAttrMap["TogglePos"], oTogglePos);
           if (eRet != kError_NoErr)
           {
               m_oLastError = string("Improperly formatted Pos coordinates: ") +
                              oAttrMap["Pos"];
               return kError_InvalidParam;
           }
           m_pCurrentPanel->SetTogglePos(oTogglePos);
       }

       if (oAttrMap.find("OnCloseHide") != oAttrMap.end())
           m_pCurrentPanel->SetOnCloseHide(FindPanel(oAttrMap["OnCloseHide"]));
       if (oAttrMap.find("OnCloseShow") != oAttrMap.end())
           m_pCurrentPanel->SetOnCloseShow(FindPanel(oAttrMap["OnCloseShow"]));
       if (oAttrMap.find("OnOpenHide") != oAttrMap.end())
           m_pCurrentPanel->SetOnOpenHide(FindPanel(oAttrMap["OnOpenHide"]));
       if (oAttrMap.find("OnOpenShow") != oAttrMap.end())
           m_pCurrentPanel->SetOnOpenShow(FindPanel(oAttrMap["OnOpenShow"]));

       return kError_NoErr;
    }
    if (oElement == string("Controls"))
    {
       if (m_pCurrentWindow == NULL)
       {
           m_oLastError = string("<Controls> must occur inside of a "
                          "<Window> tag");
           return kError_InvalidParam;
       }

       if (m_pCurrentPanel == NULL)
       {
           m_pCurrentPanel = new Panel("DummyPanel");
           m_pCurrentPanel->SetParentWindow(m_pCurrentWindow);
       }

       return kError_NoErr;
    }

    if (oElement == string("ButtonControl"))
    {
       m_bPosDefined = m_bBitmapDefined = m_bInfoDefined = false;
       
       if (m_pCurrentControl)
       {
           m_oLastError = string("Controls cannot be nested");
           return kError_InvalidParam;
       }

       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <ButtonControl> tag needs a Name attribute");
           return kError_ParseError;
       }        
       if (oAttrMap.find("URL") != oAttrMap.end() &&
           oAttrMap["Name"] != string("Logo"))
       {
           m_oLastError = string("only the Logo button can have a URL attribute");
           return kError_ParseError;
       }        

       m_eCurrentControl = eButtonControl;
       if (oAttrMap.find("URL") != oAttrMap.end())
           m_pCurrentControl = new ButtonControl(m_pCurrentWindow, 
                                                 oAttrMap["Name"],
                                                 oAttrMap["URL"]);
       else                                          
           m_pCurrentControl = new ButtonControl(m_pCurrentWindow, 
                                                 oAttrMap["Name"]);

       return kError_NoErr;
    }

    if (oElement == string("DialControl"))
    {
       m_bPosDefined = m_bBitmapDefined = m_bInfoDefined = false;
       
       if (m_pCurrentControl)
       {
           m_oLastError = string("Controls cannot be nested");
           return kError_InvalidParam;
       }

       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <DialControl> tag needs a Name attribute");
           return kError_ParseError;
       }        

       if (oAttrMap.find("NumFrames") == oAttrMap.end())
       {
           m_oLastError = string("the <DialControl> tag needs a NumFrames attribute");
           return kError_ParseError;
       }        

       if (oAttrMap.find("PixelsPerFrame") == oAttrMap.end())
       {
           m_oLastError = string("the <DialControl> tag needs a PixelsPerFrame attribute");
           return kError_ParseError;
       }        

       m_eCurrentControl = eDialControl;
       m_pCurrentControl = new DialControl(m_pCurrentWindow, 
                                           oAttrMap["Name"],
                                           atoi(oAttrMap["NumFrames"].c_str()),
                                           atoi(oAttrMap["PixelsPerFrame"].c_str()));
       return kError_NoErr;
    }

    if (oElement == string("MultiStateControl"))
    {
       m_bPosDefined = m_bBitmapDefined = m_bInfoDefined = false;
       
       if (m_pCurrentControl)
       {
           m_oLastError = string("Controls cannot be nested");
           return kError_InvalidParam;
       }

       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <MultiStateControl> tag needs a Name attribute");
           return kError_ParseError;
       }        
       if (oAttrMap.find("NumStates") == oAttrMap.end())
       {
           m_oLastError = string("the <MultiStateControl> tag needs a NumStates attribute");
           return kError_ParseError;
       }        

       m_eCurrentControl = eMultiStateControl;
       m_pCurrentControl = new MultiStateControl(m_pCurrentWindow, 
                                                 oAttrMap["Name"],
                                                 atoi(oAttrMap["NumStates"].
                                                 c_str()));
       return kError_NoErr;
    }

    if (oElement == string("PixSliderControl"))
    {
       bool reveal = true;
       m_bPosDefined = m_bBitmapDefined = m_bInfoDefined = false;

       if (m_pCurrentControl)
       {
           m_oLastError = string("Controls cannot be nested");
           return kError_InvalidParam;
       }

       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <PixSliderControl> tag needs a Name attribute");
           return kError_ParseError;
       }
       if (oAttrMap.find("NumStates") == oAttrMap.end())
       {
           m_oLastError = string("the <PixSliderControl> tag needs a NumStates attribute");
           return kError_ParseError;
       }
       if (oAttrMap.find("Reveal") != oAttrMap.end())
       {
           reveal = strcasecmp(oAttrMap["Reveal"].c_str(), "yes") == 0;
       }

       m_eCurrentControl = ePixSliderControl;
       m_pCurrentControl = new PixSliderControl(m_pCurrentWindow,
                                                oAttrMap["Name"],
                                                atoi(oAttrMap["NumStates"].
                                                c_str()), reveal);
       return kError_NoErr;
    }

    if (oElement == string("SliderControl"))
    {
       m_bPosDefined = m_bBitmapDefined = m_bInfoDefined = false;
       int iThumbStates = 1, iNumFrames = 3;    
       int iNotch = -1, iNotchWidth = -1;

       if (m_pCurrentControl)
       {
           m_oLastError = string("Controls cannot be nested");
           return kError_InvalidParam;
       }
       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <SliderControl> tag needs a Name attribute");
           return kError_ParseError;
       }        

       if (oAttrMap.find("ThumbStates") != oAttrMap.end())
       {
           iThumbStates = atoi(oAttrMap["ThumbStates"].c_str());
       }
 
       if (oAttrMap.find("Frames") != oAttrMap.end())
       {
           iNumFrames = atoi(oAttrMap["Frames"].c_str());
       }

       if (oAttrMap.find("Notch") != oAttrMap.end())
       {
           iNotch = atoi(oAttrMap["Notch"].c_str());
       }

       if (oAttrMap.find("NotchWidth") != oAttrMap.end())
       {
           iNotchWidth = atoi(oAttrMap["NotchWidth"].c_str());
       }

       m_eCurrentControl = eSliderControl;
       m_pCurrentControl = new SliderControl(m_pCurrentWindow,
                                             oAttrMap["Name"],
                                             iThumbStates, iNumFrames,
                                             iNotch, iNotchWidth);
       return kError_NoErr;
    }

    if (oElement == string("VSliderControl"))
    {
       m_bPosDefined = m_bBitmapDefined = m_bInfoDefined = false;
       int iThumbStates = 1, iNumFrames = 3;    
       int iNotch = -1, iNotchWidth = -1;
   
       if (m_pCurrentControl)
       {
           m_oLastError = string("Controls cannot be nested");
           return kError_InvalidParam;
       }
       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <VSliderControl> tag needs a Name attribute");
           return kError_ParseError;
       }        

       if (oAttrMap.find("ThumbStates") != oAttrMap.end())
       {
           iThumbStates = atoi(oAttrMap["ThumbStates"].c_str());
       }

       if (oAttrMap.find("Frames") != oAttrMap.end())
       {
           iNumFrames = atoi(oAttrMap["Frames"].c_str());
       }

       if (oAttrMap.find("Notch") != oAttrMap.end())
       {
           iNotch = atoi(oAttrMap["Notch"].c_str());
       }

       if (oAttrMap.find("NotchWidth") != oAttrMap.end())
       {
           iNotchWidth = atoi(oAttrMap["NotchWidth"].c_str());
       }
       m_eCurrentControl = eVSliderControl;
       m_pCurrentControl = new VSliderControl(m_pCurrentWindow,
                                              oAttrMap["Name"],
                                              iThumbStates, iNumFrames,
                                              iNotch, iNotchWidth);
       return kError_NoErr;
    }

    if (oElement == string("TextControl"))
    {
       m_bPosDefined = m_bBitmapDefined = m_bInfoDefined = false;
       
       if (m_pCurrentControl)
       {
           m_oLastError = string("Controls cannot be nested");
           return kError_InvalidParam;
       }
       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <TextControl> tag needs a Name attribute");
           return kError_ParseError;
       }        

       m_eCurrentControl = eTextControl;
       m_pCurrentControl = new TextControl(m_pCurrentWindow,
                                           oAttrMap["Name"]);
       return kError_NoErr;
    }

    if (oElement == string("PixmapFontControl"))
    {
       m_bPosDefined = m_bBitmapDefined = m_bInfoDefined = false;
       
       if (m_pCurrentControl)
       {
           m_oLastError = string("Controls cannot be nested");
           return kError_InvalidParam;
       }

       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <PixmapFontControl> tag needs a Name attribute");
           return kError_ParseError;
       }

       m_eCurrentControl = ePixFontControl;
       m_pCurrentControl = new PixFontControl(m_pCurrentWindow, 
                                              oAttrMap["Name"]);

       bool bCase = false;
       if (oAttrMap.find("IgnoreCase") != oAttrMap.end())
          bCase = strcasecmp(oAttrMap["IgnoreCase"].c_str(), "yes") == 0;

       if (bCase) 
           ((PixFontControl *)m_pCurrentControl)->SetIgnoreCase(true);

       return kError_NoErr;
    }

    if (oElement == string("FontMap"))
    {
       char *row;

       if (m_pCurrentControl == NULL || m_eCurrentControl != ePixFontControl)
       {
           m_oLastError = string("The <FontMap> tag must be inside of a "
                                 "<PixFontControl> tag");
           return kError_InvalidParam;
       }

       if (oAttrMap.find("Map") == oAttrMap.end())
       {
           m_oLastError = string("the <FontMap> tag needs a Map attribute");
           return kError_ParseError;
       }
 
       row = (char *)oAttrMap["Map"].c_str();
       ((PixFontControl *)m_pCurrentControl)->AddMapRow(row);
    
       return kError_NoErr;
    }

    if (oElement == string("PixmapTimeControl"))
    {
       m_bPosDefined = m_bBitmapDefined = m_bInfoDefined = false;

       if (m_pCurrentControl)
       {
           m_oLastError = string("Controls cannot be nested");
           return kError_InvalidParam;
       }

       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <PixmapTimeControl> tag needs a Name attribute");
           return kError_ParseError;
       }

       m_eCurrentControl = ePixTimeControl;
       m_pCurrentControl = new PixTimeControl(m_pCurrentWindow,
                                              oAttrMap["Name"]);

       return kError_NoErr;
    }

    if (oElement == string("PartName"))
    {
       if (m_pCurrentControl == NULL || m_eCurrentControl != ePixTimeControl)
       {
           m_oLastError = string("The <PartName> tag must be inside of a "
                                 "<PixmapTimeControl> tag");
           return kError_InvalidParam;
       }

       if (oAttrMap.find("Part") == oAttrMap.end())
       {
           m_oLastError = string("the <PartName> tag needs a Part attribute");
           return kError_ParseError;
       }

       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <PartName> tag needs a Name attribute");
           return kError_ParseError;
       }

       ((PixTimeControl *)m_pCurrentControl)->SetPartName(oAttrMap["Part"],
                                                          oAttrMap["Name"]);

       return kError_NoErr;
    }
    
    if (oElement == string("Style"))
    {
       Color  oColor(0, 0, 0);
       Font  *pFont;
       bool   bBold = false, bItalic = false, bUnderline = false;
       string oAlign("");
       
       if (m_pCurrentControl == NULL || m_eCurrentControl != eTextControl)
       {
           m_oLastError = string("The <Style> tag must be inside of a "
                                 "<TextControl> tag");
           return kError_InvalidParam;
       }

       if (oAttrMap.find("Font") == oAttrMap.end())
       {
           m_oLastError = string("the <Style> tag needs a Font attribute");
           return kError_ParseError;
       }        

       pFont = FindFont(oAttrMap["Font"]);
       if (pFont == NULL)
       {
           m_oLastError = string("Undefined font ") + 
                          oAttrMap["Name"] + 
                          string("in tag <Style>");
           return kError_InvalidParam;
       }

#ifdef HAVE_GTK
       pFont = new GTKFont(*(GTKFont *)pFont);
#endif
       if (oAttrMap.find("Color") != oAttrMap.end() && 
           oAttrMap["Color"] != string(""))
       {
           eRet = ParseColor(oAttrMap["Color"], oColor);
           if (eRet != kError_NoErr)
           {
               m_oLastError = string("Improperly formatted color info: ") +
                              oAttrMap["Color"];
               return kError_InvalidParam;
           }
       }

       if (oAttrMap.find("Bold") != oAttrMap.end())
          bBold = strcasecmp(oAttrMap["Bold"].c_str(), "yes") == 0;
       if (oAttrMap.find("Italic") != oAttrMap.end())
          bItalic = strcasecmp(oAttrMap["Italic"].c_str(), "yes") == 0;
       if (oAttrMap.find("Underline") != oAttrMap.end())
          bUnderline = strcasecmp(oAttrMap["Underline"].c_str(), "yes") == 0;

       if (oAttrMap.find("Align") != oAttrMap.end()) 
          oAlign = oAttrMap["Align"]; 

       ((TextControl *)m_pCurrentControl)->SetStyle(pFont, oAlign,
               oColor, bBold, bItalic, bUnderline);
       
       return kError_NoErr;
    }

    if (oElement == string("ChangeWindow"))
    {
       if (m_pCurrentControl == NULL || m_eCurrentControl != eButtonControl)
       {
           m_oLastError = string("The <ChangeWindow> tag must be inside of a "
                                 "<ButtonControl> tag");
           return kError_InvalidParam;
       }

       if (oAttrMap.find("Window") == oAttrMap.end())
       {
           m_oLastError = string("the <ChangeWindow> tag needs a Window attribute");
           return kError_ParseError;
       }        

       ((ButtonControl *)m_pCurrentControl)->SetTargetWindow(oAttrMap["Window"]);
       
       return kError_NoErr;
    }
    if (oElement == string("TogglePanel"))
    {
       Panel *pPanel;

       if (m_pCurrentControl == NULL || m_eCurrentControl != eButtonControl)
       {
           m_oLastError = string("The <TogglePanel> tag must be inside of a "
                                 "<ButtonControl> tag");
           return kError_InvalidParam;
       }
       if (oAttrMap.find("Panel") == oAttrMap.end())
       {
           m_oLastError = string("the <TogglePanel> tag needs a Panel attribute");
           return kError_ParseError;
       }        

       pPanel = FindPanel(oAttrMap["Panel"]);
       if (pPanel == NULL)
       {
           m_oLastError = string("Undefined Panel ") +
                          oAttrMap["Panel"] +
                          string(" in tag <TogglePanel>");
           return kError_InvalidParam;
       }

       ((ButtonControl *)m_pCurrentControl)->SetPanelToggle(pPanel);
       
       return kError_NoErr;
    }


    if (oElement == string("DockPosition"))
    {
       Pos oPos;

       if (m_pCurrentControl)
       {
          m_oLastError = string("The <DockPosition> tag cannot be inside of a "
                                "<XXXXControl> tag");
          return kError_InvalidParam;
       }

       if (m_pCurrentWindow == NULL)
       {
          m_oLastError = string("The <DockPosition> tag must be inside of a "
                                "<Window> tag");
          return kError_InvalidParam;
       }

       if (oAttrMap.find("Pos") == oAttrMap.end())
       {
          m_oLastError = string("the <DockPosition> tag needs a Pos attribute");
          return kError_ParseError;
       }

       eRet = ParsePos(oAttrMap["Pos"], oPos);
       if (eRet != kError_NoErr)
       {
           m_oLastError = string("Improperly formatted Pos coordinates: ") +
                          oAttrMap["Pos"];
           return kError_InvalidParam;
       }
       m_pCurrentWindow->SetDockPosition(oPos);

       return kError_NoErr;
    }

    if (oElement == string("Position"))
    {
       Rect oRect;
       Pos  oPos;

       if (m_pCurrentControl == NULL)
       {
          m_oLastError = string("The <Position> tag must be inside of a "
                                "<XXXXControl> tag");
          return kError_InvalidParam;
       }

       if (oAttrMap.find("Rect") == oAttrMap.end() &&
           oAttrMap.find("Pos") == oAttrMap.end())
       {
           m_oLastError = string("the <Position> tag needs a Rect or a Pos attribute");
           return kError_ParseError;
       }        

       if (oAttrMap.find("Pos") != oAttrMap.end() &&
           m_eCurrentControl != eButtonControl &&
           m_eCurrentControl != eMultiStateControl &&
           m_eCurrentControl != ePixSliderControl)
       {
           m_oLastError = string("the Pos attribute in the <Position> tag can only be used "
                                 "for a MultiStateControl or a ButtonControl");
           return kError_ParseError;
       }        
       
       if (oAttrMap.find("Rect") != oAttrMap.end())
       {
           eRet = ParseRect(oAttrMap["Rect"], oRect);
           if (eRet != kError_NoErr)
           {
               m_oLastError = string("Improperly formatted Rect coordinates: ") +
                              oAttrMap["Rect"];
               return kError_InvalidParam;
           }
           m_pCurrentControl->SetRect(oRect);
       }    
       if (oAttrMap.find("Pos") != oAttrMap.end())
       {
           eRet = ParsePos(oAttrMap["Pos"], oPos);
           if (eRet != kError_NoErr)
           {
               m_oLastError = string("Improperly formatted Pos coordinates: ") +
                              oAttrMap["Pos"];
               return kError_InvalidParam;
           }
           m_pCurrentControl->SetPos(oPos);
       }    

       m_bPosDefined = true;

       return kError_NoErr;
    }

    if (oElement == string("ControlBitmap"))
    {
       Bitmap *pBitmap = NULL;
       Rect    oRect;
       bool    bHoriz = true;

       if (m_pCurrentControl == NULL)
       {
          m_oLastError = string("The <ControlBitmap> tag must be inside of a "
                                "<XXXXControl> tag");
          return kError_InvalidParam;
       }

       if (oAttrMap.find("Rect") == oAttrMap.end())
       {
           m_oLastError = string("the <ControlBitmap> tag needs a Rect attribute");
           return kError_ParseError;
       }        
       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <ControlBitmap> tag needs a Name attribute");
           return kError_ParseError;
       }        
       
       if (oAttrMap.find("Style") != oAttrMap.end())
       {
           if (oAttrMap["Style"] == string("Vert"))
               bHoriz = false;
       }        

       eRet = ParseRect(oAttrMap["Rect"], oRect);
       if (eRet != kError_NoErr)
       {
           m_oLastError = string("Improperly formatted Rect coordinates: ") +
                                 oAttrMap["Rect"];
           return kError_InvalidParam;
       }

       pBitmap = FindBitmap(oAttrMap["Name"]);
       if (pBitmap == NULL)
       {
           m_oLastError = string("Undefined bitmap ") +
                          oAttrMap["Name"] +
                          string(" in tag <ControlBitmap>");
           return kError_InvalidParam;
       }

       m_pCurrentControl->SetBitmap(pBitmap, oRect, bHoriz);
       
       m_bBitmapDefined = true;
       
       return kError_NoErr;
    }

    if (oElement == string("SliderTroughBitmap"))
    {
       Bitmap *pBitmap = NULL;
       Rect    oRect;
       bool    bHoriz = true;
       int     iDelta = 0;

       if (m_pCurrentControl == NULL)
       {
          m_oLastError = string("The <SliderTroughBitmap> tag must be inside of a <XXXXControl> tag");
          return kError_InvalidParam;
       }

       if (m_eCurrentControl != eSliderControl &&
           m_eCurrentControl != eVSliderControl)
       {
           m_oLastError = string("the <SliderTroughBitmap> tag can only be used "
                                 "for a SliderControl or a VSliderControl");
           return kError_ParseError;
       }

       if (oAttrMap.find("Rect") == oAttrMap.end())
       {
           m_oLastError = string("the <SliderTroughBitmap> tag needs a Rect attribute");
           return kError_ParseError;
       }
       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <SliderTroughBitmap> tag needs a Name attribute");
           return kError_ParseError;
       }
       if (oAttrMap.find("NumFrames") == oAttrMap.end())
       {
           m_oLastError = string("the <SliderTroughBitmap> tag needs a NumFrames attribute");
           return kError_ParseError;
       }

       if (oAttrMap.find("Style") != oAttrMap.end())
       {
           if (oAttrMap["Style"] == string("Vert"))
               bHoriz = false;
       }

       eRet = ParseRect(oAttrMap["Rect"], oRect);
       if (eRet != kError_NoErr)
       {
           m_oLastError = string("Improperly formatted Rect coordinates: ") +
                                 oAttrMap["Rect"];
           return kError_InvalidParam;
       }

       pBitmap = FindBitmap(oAttrMap["Name"]);
       if (pBitmap == NULL)
       {
           m_oLastError = string("Undefined bitmap ") +
                          oAttrMap["Name"] +
                          string(" in tag <ControlBitmap>");
           return kError_InvalidParam;
       }

       int iFrames = atoi(oAttrMap["NumFrames"].c_str());
 
       if (oAttrMap.find("DeltaBetweenFrames") != oAttrMap.end()) 
       {
           iDelta = atoi(oAttrMap["DeltaBetweenFrames"].c_str());
       }

       bool bMiddle = false;

       if (oAttrMap.find("MaxInCenter") != oAttrMap.end())
       {
           if (oAttrMap["MaxInCenter"] == string("true"))
               bMiddle = true;
       } 

       if (m_eCurrentControl == eSliderControl)
           ((SliderControl *)m_pCurrentControl)->SetTroughBitmap(pBitmap, oRect, 
                                                                 iFrames, 
                                                                 bHoriz, 
                                                                 iDelta,
                                                                 bMiddle);
       else
           ((VSliderControl *)m_pCurrentControl)->SetTroughBitmap(pBitmap, oRect,
                                                                 iFrames,
                                                                 bHoriz,
                                                                 iDelta,
                                                                 bMiddle);

       return kError_NoErr;
    }

    if (oElement == string("ActivationBitmap"))
    {
       Bitmap *pBitmap = NULL;
       Rect    oRect;

       if (m_pCurrentControl == NULL)
       {
          m_oLastError = string("The <ActivationBitmap> tag must be inside of a <XXXXControl> tag");
          return kError_InvalidParam;
       }

       if (m_eCurrentControl != ePixSliderControl) 
       {
           m_oLastError = string("the <ActivationBitmap> tag can only be used "
                                 "for a PixSliderControl");
           return kError_ParseError;
       }

       if (oAttrMap.find("Rect") == oAttrMap.end())
       {
           m_oLastError = string("the <ActivationBitmap> tag needs a Rect attribute");
           return kError_ParseError;
       }
       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <ActivationBitmap> tag needs a Name attribute");
           return kError_ParseError;
       }

       eRet = ParseRect(oAttrMap["Rect"], oRect);
       if (eRet != kError_NoErr)
       {
           m_oLastError = string("Improperly formatted Rect coordinates: ") +
                                 oAttrMap["Rect"];
           return kError_InvalidParam;
       }
       pBitmap = FindBitmap(oAttrMap["Name"]);
       if (pBitmap == NULL)
       {
           m_oLastError = string("Undefined bitmap ") +
                          oAttrMap["Name"] +
                          string(" in tag <ActivationBitmap>");
           return kError_InvalidParam;
       }

       ((PixSliderControl *)m_pCurrentControl)->SetActivationBitmap(pBitmap, 
                                                                    oRect, 
                                                                    true);
       return kError_NoErr;
    }


    if (oElement == string("ControlStateBitmap"))
    {
       Bitmap *pBitmap = NULL;
       Rect    oRect;
       ControlStateEnum eState;
       int     iFrameSet = 0;

       if (m_pCurrentControl == NULL)
       {
          m_oLastError = string("The <ControlStateBitmap> tag must be inside of a <XXXXControl> tag");
          return kError_InvalidParam;
       }

       if (oAttrMap.find("Rect") == oAttrMap.end())
       {
           m_oLastError = string("the <ControlStateBitmap> tag needs a Rect attribute");
           return kError_ParseError;
       }
       if (oAttrMap.find("Name") == oAttrMap.end())
       {
           m_oLastError = string("the <ControlStateBitmap> tag needs a Name attribute");
           return kError_ParseError;
       }

       if (oAttrMap.find("State") == oAttrMap.end())
       {
           m_oLastError = string("the <ControlStateBitmap> tag needs a State attribute");
           return kError_ParseError;
       }

       if (oAttrMap.find("StateNum") != oAttrMap.end())
       {
           iFrameSet = atoi(oAttrMap["StateNum"].c_str());
       }

       eRet = ParseRect(oAttrMap["Rect"], oRect);
       if (eRet != kError_NoErr)
       {
           m_oLastError = string("Improperly formatted Rect coordinates: ") +
                                 oAttrMap["Rect"];
           return kError_InvalidParam;
       }

       eRet = ParseState(oAttrMap["State"], eState);
       if (eRet != kError_NoErr)
       {
           m_oLastError = string("Unknown State: ") + oAttrMap["State"];
           return kError_InvalidParam;
       }

       pBitmap = FindBitmap(oAttrMap["Name"]);
       if (pBitmap == NULL)
       {
           m_oLastError = string("Undefined bitmap ") +
                          oAttrMap["Name"] +
                          string(" in tag <ControlStateBitmap>");
           return kError_InvalidParam;
       }

       m_pCurrentControl->SetStateBitmap(pBitmap, oRect, eState, iFrameSet);

       m_bBitmapDefined = true;

       return kError_NoErr;
    }

    if (oElement == string("Info"))
    {
       if (m_pCurrentControl == NULL)
       {
          m_oLastError = string("The <Info> tag must be inside of a "
                                "<XXXXControl> tag");
          return kError_InvalidParam;
       }

       if (oAttrMap.find("Desc") != oAttrMap.end())
           m_pCurrentControl->SetDesc(oAttrMap["Desc"]);
       if (oAttrMap.find("Tip") != oAttrMap.end())
           m_pCurrentControl->SetTip(oAttrMap["Tip"]);
       
       m_bInfoDefined = true;
       
       return kError_NoErr;
    }

    if (oElement == string("ThemeInfo"))
    {
       if (m_pCurrentControl != NULL)
       {
          m_oLastError = string("The <ThemeInfo> tag must be top level tag");
          return kError_InvalidParam;
       }

       if (oAttrMap.find("Name") != oAttrMap.end())
           m_oThemeName = oAttrMap["Name"];
       if (oAttrMap.find("Author") != oAttrMap.end())
           m_oThemeAuthor = oAttrMap["Author"];
       if (oAttrMap.find("EMail") != oAttrMap.end())
           m_oAuthorEMail = oAttrMap["EMail"];
       if (oAttrMap.find("WebPage") != oAttrMap.end())
           m_oAuthorWebPage = oAttrMap["WebPage"];
       if (oAttrMap.find("Misc") != oAttrMap.end())
           m_oMiscInfo = oAttrMap["Misc"];
       
       return kError_NoErr;
    }

    if (oElement == string("Headlines"))
    {
       string oURLPath;
       
       if (m_pCurrentWindow)
       {
           m_oLastError = string("A <Headlines> tag must be outside any <Window> tags.");
           return kError_InvalidParam;
       }

       if (m_pParsedHeadlines)
       {
           m_oLastError = string("Each theme file can only have one <Headlines> tag.");
           return kError_InvalidParam;
       }

       if (oAttrMap.find("URL") == oAttrMap.end())
       {
           m_oLastError = string("the <Headlines> tag needs a URL attribute");
           return kError_ParseError;
       }        

       if (oAttrMap.find("XMLPath") == oAttrMap.end())
       {
           m_oLastError = string("the <Headlines> tag needs a XMLPath attribute");
           return kError_ParseError;
       }        
       if (oAttrMap.find("XMLURLPath") != oAttrMap.end())
           oURLPath = oAttrMap["XMLURLPath"];

       if (oAttrMap.find("DownloadInterval") == oAttrMap.end())
       {
           m_oLastError = string("the <Headlines> tag needs a DownloadInterval attribute");
           return kError_ParseError;
       }        

       if (oAttrMap.find("ChangeInterval") == oAttrMap.end())
       {
           m_oLastError = string("the <Headlines> tag needs a ChangeInterval attribute");
           return kError_ParseError;
       }        

       m_pParsedHeadlines = new HeadlineInfo(oAttrMap["URL"],
                                             oAttrMap["XMLPath"],
                                             oURLPath,
                                             atoi(oAttrMap["DownloadInterval"].c_str()),
                                             atoi(oAttrMap["ChangeInterval"].c_str()));
       return kError_NoErr;
    }

    return kError_NoErr;
}

Error Theme::EndElement(string &oElement)
{
    if (oElement == string("Bitmap") ||
        oElement == string("BackgroundBitmap") ||
        oElement == string("TogglePanel") ||
        oElement == string("Font") ||
        oElement == string("ChangeWindow") ||
        oElement == string("ThemeInfo") ||
        oElement == string("Headlines") ||
        oElement == string("MaskBitmap"))
       return kError_NoErr;

    if (oElement == string("Controls"))
    {
       if (m_pCurrentWindow == NULL)
       {
           m_oLastError = string("The <Controls> tag must be inside a "
                                 "<Window> tag");
           return kError_InvalidParam;
       }
       return kError_NoErr;
    }

    if (oElement == string("Action") ||
        oElement == string("Position") ||
        oElement == string("Style") ||
        oElement == string("Info") ||
        oElement == string("ControlBitmap") ||
        oElement == string("ControlStateBitmap") ||
        oElement == string("SliderTroughBitmap") ||
        oElement == string("ActivationBitmap") ||
        oElement == string("FontMap") ||
        oElement == string("PartName"))
    {
       if (m_pCurrentControl == NULL)
       {
           m_oLastError = string("The <") + 
                          oElement.c_str()+ 
                          string("> tag must be inside a <XXXXControl>");
           return kError_InvalidParam;
       }
       return kError_NoErr;
    }

    if (oElement == string("ButtonControl") ||
        oElement == string("DialControl") ||
        oElement == string("SliderControl") ||
        oElement == string("VSliderControl") ||
        oElement == string("MultiStateControl") ||
        oElement == string("TextControl") ||
        oElement == string("PixmapFontControl") ||
        oElement == string("PixSliderControl")) 
    {
       if (!m_bPosDefined)
       {
           m_oLastError = string("The Control is missing the <Position> tag");
           return kError_InvalidParam;
       }
       if (!m_bBitmapDefined && m_eCurrentControl != eTextControl)
       {
           m_oLastError = string("The Control is missing the <ControlBitmap> tag");
           return kError_InvalidParam;
       }
       if (!m_bInfoDefined && (m_eCurrentControl != eTextControl && 
                               m_eCurrentControl != ePixFontControl))
       {
           m_oLastError = string("The Control is missing the <Info> tag");
           return kError_InvalidParam;
       }
       if (m_eCurrentControl == eTextControl)
       {
           if (!((TextControl *)m_pCurrentControl)->StyleHasBeenSet())
           {
               m_oLastError = "A <TextControl> needs to define a <Style> tag";
               return kError_InvalidParam;
           }    
       }

       // fix lameass broken CocaCola themes... someone didn't read the theme
       // HOWTO
       if (m_pCurrentPanel == NULL) 
       {
           m_pCurrentPanel = new Panel("DummyPanel");
           m_pCurrentPanel->SetParentWindow(m_pCurrentWindow);
       }

       m_pCurrentPanel->AddControl(m_pCurrentControl);
       m_pCurrentControl = NULL;
       m_eCurrentControl = eUndefinedControl;

       return kError_NoErr;
    }

    if (oElement == string("Window"))
    {
       vector<Panel *>::iterator i;

       if (!m_pParsedWindows)
           m_pParsedWindows = new vector<Window *>;

       if (m_pCurrentPanel)
       {
          m_oPanels.push_back(m_pCurrentPanel);
          m_pCurrentWindow->AddPanel(m_pCurrentPanel);
          m_pCurrentPanel = NULL;
       }

       m_pParsedWindows->push_back(m_pCurrentWindow);
       m_oPanels.clear();
           
       m_pCurrentWindow = NULL;

       for(i = m_oPanels.begin(); i != m_oPanels.end(); i++)
       {
           if ((*i)->GetParentWindow() == NULL)
           {
               string oName;
               (*i)->GetName(oName);
               m_oLastError = string("Panel ") + oName + 
                              string(" was referenced but never defined");
               return kError_InvalidParam;
           }
       }
       return kError_NoErr;
    }

    if (oElement == string("Panel"))
    {
       m_oPanels.push_back(m_pCurrentPanel);
       m_pCurrentWindow->AddPanel(m_pCurrentPanel);
       m_pCurrentPanel = NULL;

       return kError_NoErr;
    }

    if (oElement == string("PixmapTimeControl"))
    {
       m_pCurrentPanel->AddControl(m_pCurrentControl);
       m_pCurrentControl = NULL;
       m_eCurrentControl = eUndefinedControl;

       return kError_NoErr;
    }

    return kError_NoErr;
}    

Error Theme::PCData(string &oData)
{
    m_oLastError = string("Invalid character data: ") + oData;
    
    return kError_InvalidParam;
}

Bitmap *Theme::FindBitmap(string &oName)
{
    vector<Bitmap *>::iterator i;
    string                     oTemp;

    if (!m_pParsedBitmaps)
        return NULL;
        
    for(i = m_pParsedBitmaps->begin(); i != m_pParsedBitmaps->end(); i++)
    {
       (*i)->GetName(oTemp);
       if (oTemp == oName)
          return *i;
    }

    return NULL;
}

Panel *Theme::FindPanel(string &oName)
{
    vector<Panel *>::iterator i;
    string                     oTemp;
    Panel                     *pTemp;

    if (m_pCurrentPanel)
    {
        m_pCurrentPanel->GetName(oTemp);
        if (oTemp == oName)
           return m_pCurrentPanel;
    }

    for(i = m_oPanels.begin(); i != m_oPanels.end(); i++)
    {
       (*i)->GetName(oTemp);
       if (oTemp == oName)
          return *i;
    }

    // No panels found. Lets assume that this is a forward reference.
    pTemp = new Panel(oName);
    m_oPanels.push_back(pTemp);

    return pTemp;
}

Font *Theme::FindFont(string &oName)
{
    vector<Font *>::iterator i;
    string                   oTemp;

    if (!m_pParsedFonts)
        return NULL;
        
    for(i = m_pParsedFonts->begin(); i != m_pParsedFonts->end(); i++)
    {
       (*i)->GetName(oTemp);
       if (oTemp == oName)
          return *i;
    }

    return NULL;
}

Error Theme::ParseState(string &oStatestring, ControlStateEnum &eState)
{
    Error ret = kError_NoErr;;

    if (!strcmp("Normal", oStatestring.c_str())) 
        eState = CS_Normal;
    else if (!strcmp("MouseOver", oStatestring.c_str())) 
        eState = CS_MouseOver;
    else if (!strcmp("Pressed", oStatestring.c_str()))
        eState = CS_Pressed;
    else if (!strcmp("DisabledMouseOver", oStatestring.c_str()))
        eState = CS_DisabledMO;
    else if (!strcmp("Disabled", oStatestring.c_str()))
        eState = CS_Disabled;
    else if (!strcmp("Dragging", oStatestring.c_str()))
        eState = CS_Dragging;
    else if (!strcmp("HiddenMouseOver", oStatestring.c_str()))
        eState = CS_HiddenMO;
    else if (!strcmp("Hidden", oStatestring.c_str()))
        eState = CS_Hidden;
    else
        ret = kError_InvalidParam;

    return ret;
}

Error Theme::ParseRect(string &oRectstring, Rect &oRect)
{
    int iRet;

    iRet = sscanf(oRectstring.c_str(), " %d , %d , %d , %d",
             &oRect.x1, &oRect.y1, &oRect.x2, &oRect.y2);

    return (iRet == 4) ? kError_NoErr : kError_InvalidParam;
}

Error Theme::ParseColor(string &oColorstring, Color &oColor)
{
    int iRet;
    int iRed, iGreen, iBlue;

    iRet = sscanf(oColorstring.c_str(), "#%02X%02X%02X",
                  &iRed, &iGreen, &iBlue);

    if (iRet == 3)
    {
        oColor.red = iRed;
        oColor.green = iGreen;
        oColor.blue = iBlue;
        return kError_NoErr;
    }     

    return kError_InvalidParam;
}

Error Theme::ParsePos(string &oPosstring, Pos &oPos)
{
    int iRet;

    iRet = sscanf(oPosstring.c_str(), " %d , %d", &oPos.x, &oPos.y);
    return (iRet == 2) ? kError_NoErr : kError_InvalidParam;
}

void Theme::SetDefaultFont(const string &oFont)
{ 
    m_oDefaultFont = oFont;
}

void Theme::PostWindowCreate(void)
{
}

void Theme::ShowThemeCredits(void)
{
    string oText;

    if (m_bCreditsShown)
    {
       m_bCreditsShown = false;
       m_pWindow->ControlStringValue("Title", true, m_oSavedText);
       return;
    }
    
    m_pWindow->ControlStringValue("Title", false, m_oSavedText);
    if (m_oThemeName.size() > 0)
    {
        oText = m_oThemeName;
       
        if (m_oThemeAuthor.size() > 0)
           oText += string(" written by ") + m_oThemeAuthor;     

        if (m_oAuthorEMail.size() > 0)
           oText += string(", ") + m_oAuthorEMail;     

        if (m_oAuthorWebPage.size() > 0)
           oText += string(" (") + m_oAuthorWebPage + string(")");     
           
        if (m_oAuthorWebPage.size() > 0)
           oText += string(" ") + m_oMiscInfo + string(" ");     
    }   
    else
       oText = "<No theme credit info available>";
       
    m_pWindow->ControlStringValue("Title", true, oText);
    m_bCreditsShown = true;
}

void Theme::ConvertForeignFormat(string &oDir)
{
    m_pForeignThemes->TryToConvert(oDir);
}
