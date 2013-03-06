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

   $Id: Win32Canvas.cpp,v 1.1 2004/10/13 16:17:07 linha2 Exp $
____________________________________________________________________________*/ 

#include <windows.h>
#include "Win32Canvas.h"
#include "Win32Bitmap.h"
#include "Win32Font.h"
#include "debug.h"

#define DB Debug_v("%s:%d\n", __FILE__, __LINE__);

Win32Canvas::Win32Canvas(Win32Window *pParent)
{
   m_pParent = pParent;
   m_pBufferBitmap = NULL;
   m_hPal = NULL;
}

Win32Canvas::~Win32Canvas(void)
{
   delete m_pBufferBitmap;
}

void Win32Canvas::SetParent(Win32Window *pParent)
{
   m_pParent = pParent;
}   

void Win32Canvas::Init(void)
{
   Rect oDestRect;
   string oName("DoubleBuffer");
   
   assert(m_pBGBitmap);
   
   if (m_pBufferBitmap)
      delete m_pBufferBitmap;

   m_pBufferBitmap = new Win32Bitmap(m_oBGRect.Width(), 
                                     m_oBGRect.Height(),
                                     oName);
   
   oDestRect.x1 = oDestRect.y1 = 0;
   oDestRect.x2 = m_oBGRect.Width();
   oDestRect.y2 = m_oBGRect.Height();
   m_pBufferBitmap->BlitRect(m_pBGBitmap, m_oBGRect, oDestRect);
}

// This function returns the number of pixels of the text that
// were clipped if the text does not fit into the clipping rect.
// If the text all fit, it returns 0.
int Win32Canvas::RenderText(int iFontHeight, Rect &oClipRect, 
                            string &oText, AlignEnum eAlign,
                            Font *pFont, const Color &oColor,
                            bool bBold, bool bItalic, bool bUnderline)
{
   HDC        hRootDC, hMemDC;
   HFONT      hFont;
   RECT       sClip;
   SIZE       sSize;
   string     oFontFace;
   TEXTMETRIC sTm;

   ((Win32Font *)pFont)->GetFace(oFontFace);
   
   sClip.left = oClipRect.x1;
   sClip.right = oClipRect.x2;
   sClip.top = oClipRect.y1;
   sClip.bottom = oClipRect.y2;

   Erase(oClipRect);
   
   hRootDC = GetDC(NULL);
   hMemDC = CreateCompatibleDC(hRootDC);
   ReleaseDC(NULL, hRootDC);
   
   DeleteObject(SelectObject(hMemDC, m_pBufferBitmap->GetBitmapHandle()));
   
   hFont = CreateFont(-iFontHeight, 0, 0, 0, bBold ? FW_BOLD : FW_NORMAL, 
                      bItalic, bUnderline, 0, DEFAULT_CHARSET,
 					  OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                      DEFAULT_PITCH, oFontFace.c_str()); 
   
   // For debugging -- draw a red border around the text field
   //HPEN hPen = CreatePen(PS_SOLID, 1, RGB(254, 0, 0));
   //HPEN hOld = (HPEN)SelectObject(hMemDC, hPen);
   //Rectangle(hMemDC, sClip.left, sClip.top, sClip.right, sClip.bottom);
   //DeleteObject(SelectObject(hMemDC, hOld)); 

   // There is a bug in Windows 95/98 that when you DrawText into a bitmap
   // the anti aliasing does not work unless you select the font into the
   // main window and the select it into your bitmap DC. Whatever...
   HDC hExtra = GetDC(m_pParent->GetWindowHandle());
   SelectObject(hExtra, SelectObject(hExtra, hFont));
   SelectObject(hMemDC, hFont);
   ReleaseDC(m_pParent->GetWindowHandle(), hExtra);

   GetTextMetrics(hMemDC, &sTm);
   sClip.top -= sTm.tmInternalLeading;
   sClip.bottom -= sTm.tmExternalLeading - sTm.tmInternalLeading;

   SetBkMode(hMemDC, TRANSPARENT);
   SetTextColor(hMemDC, RGB(oColor.red, oColor.green, oColor.blue));
   DrawText(hMemDC, oText.c_str(), oText.length(), &sClip, DT_NOPREFIX |
            ((eAlign == eLeft) ? DT_LEFT :
		    (eAlign == eRight) ? DT_RIGHT : DT_CENTER));
            
   GetTextExtentPoint32(hMemDC, oText.c_str(), oText.length(), &sSize);
          
   DeleteDC(hMemDC);
   DeleteObject(hFont);

   Invalidate(oClipRect);
   Update();

   return max(0, sSize.cx - oClipRect.Width());
}


// This function returns the number of pixels of the text that
// were clipped if the text does not fit into the clipping rect.
// If the text all fit, it returns 0.
int Win32Canvas::RenderOffsetText(int iFontHeight, Rect &oClipRect, 
                                  string &oText, int iOffset,
                                  Font *pFont, const Color &oColor,
                                  bool bBold, bool bItalic, bool bUnderline)
{
   HDC    hRootDC, hMemDC;
   HFONT  hFont;
   RECT   sClip;
   SIZE   sSize;
   int    iRet;
   string oFontFace;
   TEXTMETRIC sTm;

   ((Win32Font *)pFont)->GetFace(oFontFace);
   
   sClip.left = oClipRect.x1;
   sClip.right = oClipRect.x2;
   sClip.top = oClipRect.y1;
   sClip.bottom = oClipRect.y2;

   Erase(oClipRect);
   
   hRootDC = GetDC(NULL);
   hMemDC = CreateCompatibleDC(hRootDC);
   ReleaseDC(NULL, hRootDC);
   
   DeleteObject(SelectObject(hMemDC, m_pBufferBitmap->GetBitmapHandle()));
   
   hFont = CreateFont(-iFontHeight, 0, 0, 0, bBold ? FW_BOLD : FW_NORMAL, 
                      bItalic, bUnderline, 0, DEFAULT_CHARSET,
 					  OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                      DEFAULT_PITCH, oFontFace.c_str()); 

   // For debugging -- draw a red border around the text field
   //HPEN hPen = CreatePen(PS_SOLID, 1, RGB(254, 0, 0));
   //HPEN hOld = (HPEN)SelectObject(hMemDC, hPen);
   //Rectangle(hMemDC, sClip.left, sClip.top, sClip.right, sClip.bottom);
   //DeleteObject(SelectObject(hMemDC, hOld)); 

   // There is a bug in Windows 95/98 that when you DrawText into a bitmap
   // the anti aliasing does not work unless you select the font into the
   // main window and the select it into your bitmap DC. Whatever...
   HDC hExtra = GetDC(m_pParent->GetWindowHandle());
   SelectObject(hExtra, SelectObject(hExtra, hFont));
   SelectObject(hMemDC, hFont);
   ReleaseDC(m_pParent->GetWindowHandle(), hExtra);

   GetTextExtentPoint32(hMemDC, oText.c_str(), oText.length(), &sSize);

   GetTextMetrics(hMemDC, &sTm);
   sClip.top -= sTm.tmInternalLeading;
   sClip.bottom -= sTm.tmExternalLeading - sTm.tmInternalLeading;

   // The size of this text is artificially inflated to insert some
   // spaces between occurances of this string. Otherwise, the end of
   // the first string would run right into the beginning of the next.
   sSize.cx += iMarqueeSpacer;
   if (iOffset > sSize.cx)
   {
      DeleteDC(hMemDC);
      DeleteObject(hFont);
      return sSize.cx - iOffset;
   }   

   SetBkMode(hMemDC, TRANSPARENT);
   SetTextColor(hMemDC, RGB(oColor.red, oColor.green, oColor.blue));
   ExtTextOut(hMemDC, oClipRect.x1 - iOffset, sClip.top, ETO_CLIPPED,
              &sClip, oText.c_str(), oText.length(), NULL);

   // If some space was left in the clipping rectangle, textout
   // the same string right at the end of the previous string
   // The above sSize.cx += iMarqueeSpacer ensures that there is
   // a space between the first and second string.
   iRet = sSize.cx - iOffset - oClipRect.Width();
   if (iRet < 0)
       ExtTextOut(hMemDC, oClipRect.x1 + (sSize.cx - iOffset), 
                  sClip.top, ETO_CLIPPED,
                  &sClip, oText.c_str(), oText.length(), NULL);
          
   DeleteDC(hMemDC);
   DeleteObject(hFont);

   Invalidate(oClipRect);
   Update();
   
   return max(iRet, 0);
}

Error Win32Canvas::Invalidate(Rect &oRect)
{
   RECT sRect;
   HWND hWnd;
   
   hWnd = m_pParent->GetWindowHandle();
   if (hWnd == NULL)
      return kError_YouScrewedUp;
   
   sRect.left = oRect.x1;
   sRect.right = oRect.x2;
   sRect.top = oRect.y1;
   sRect.bottom = oRect.y2;
   InvalidateRect(hWnd, &sRect, FALSE);

   return kError_NoErr;
}

Error Win32Canvas::Update(void)
{
   HWND hWnd;

   if (m_bNoScreenUpdate)
       return kError_NoErr;
   
   hWnd = m_pParent->GetWindowHandle();
   if (hWnd == NULL)
      return kError_YouScrewedUp;
   
   UpdateWindow(hWnd);

   return kError_NoErr;
}

Error Win32Canvas::BlitRect(Bitmap *pSrcBitmap, Rect &oSrcRect, Rect &oDestRect)
{
   if (!m_pBufferBitmap)
      return kError_NoErr;
      
   return m_pBufferBitmap->BlitRect(pSrcBitmap, oSrcRect, oDestRect);
}

Error Win32Canvas::MaskBlitRect(Bitmap *pSrcBitmap, Rect &oSrcRect, Rect &oDestRect)
{
   if (!m_pBufferBitmap)
      return kError_NoErr;
      
   return m_pBufferBitmap->MaskBlitRect(pSrcBitmap, oSrcRect, oDestRect);
}

void Win32Canvas::Paint(HDC hDC, Rect &oRect)
{
   HDC   hMemDC;

   hMemDC = CreateCompatibleDC(hDC);
   
   DeleteObject(SelectObject(hMemDC, m_pBufferBitmap->GetBitmapHandle()));
   
   if (GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE)
   {
      SelectPalette(hDC, m_hPal, false);
      RealizePalette(hDC);
   }

   BitBlt(hDC, oRect.x1, oRect.y1, oRect.Width(), oRect.Height(),
                 hMemDC, oRect.x1, oRect.y1, SRCCOPY);

   DeleteDC(hMemDC);  
}

void Win32Canvas::Erase(Rect &oRect)
{
   Rect oMine = oRect;
   
   if (m_pBufferBitmap)
       m_pBufferBitmap->BlitRect(m_pBGBitmap, oRect, oRect);
}

HRGN Win32Canvas::GetMaskRgn(void)
{
   HDC         hRootDC, hMemDC;
   int         iScanLine, iRet, iLine, iStart;
   char       *pData;
   BITMAPINFO *pInfo;
   BITMAP      sInfo;
   HRGN        hMask, hTemp;
   HBITMAP     hMaskBitmap;

   hMask = CreateRectRgn(0,0,0,0);
   
   hRootDC = GetDC(NULL);
   hMemDC = CreateCompatibleDC(hRootDC);
   ReleaseDC(NULL, hRootDC);

   hMaskBitmap = ((Win32Bitmap *)m_pBGBitmap)->GetMaskBitmapHandle();

   GetObject(hMaskBitmap, sizeof(BITMAP), (LPSTR)&sInfo);

   pInfo = (BITMAPINFO *)new char[sizeof(BITMAPINFOHEADER) + 2 * 
                                  sizeof(RGBQUAD)];
   pInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   pInfo->bmiHeader.biWidth = sInfo.bmWidth;
   pInfo->bmiHeader.biHeight = sInfo.bmHeight;
   pInfo->bmiHeader.biPlanes = 1;
   pInfo->bmiHeader.biBitCount = 1;
   pInfo->bmiHeader.biCompression = BI_RGB;
   
   pData = new char[sInfo.bmWidth * 4];
   for(iScanLine = 0; iScanLine < sInfo.bmHeight; iScanLine++)
   {
       iRet = GetDIBits(hMemDC, hMaskBitmap, 
                        (sInfo.bmHeight - 1)- iScanLine, 
                        1, pData, pInfo, DIB_PAL_COLORS);
       if (iRet == 0)
       {
           return NULL;
       }    
           
       for(iLine = 0, iStart = -1; iLine < sInfo.bmWidth; iLine++)
       {
       	  if ((pData[iLine / 8] & (0x80 >> (iLine % 8))) == 0)
          {
             if (iStart == -1)
                iStart = iLine;
             continue;
          }
          else
          {
             if (iStart >= 0)
             {
                 hTemp = CreateRectRgn(iStart, iScanLine, iLine, iScanLine + 1);
                 CombineRgn(hMask, hTemp, hMask, RGN_OR);
                 DeleteObject(hTemp);
                 iStart = -1;
             }
          }
       }    
       if (iStart >= 0)
       {
           hTemp = CreateRectRgn(iStart, iScanLine, iLine, iScanLine + 1);
           CombineRgn(hMask, hTemp, hMask, RGN_OR);
           DeleteObject(hTemp);
           iStart = -1;
       }
   }
   
   delete [] pData;
   delete [] pInfo;

   DeleteDC(hMemDC);

   return hMask;
}

void Win32Canvas::SetPalette(HPALETTE hPal)
{
   if (m_hPal)
       DeleteObject(m_hPal);
       
   m_hPal = hPal;
}

void Win32Canvas::InitBackgrounds(vector<Panel *> *pPanels)
{
	Canvas::InitBackgrounds(pPanels);

	if (m_pBufferBitmap)
 	   m_pBufferBitmap->BlitRect(m_pBGBitmap, m_oBGRect, m_oBGRect);
}
