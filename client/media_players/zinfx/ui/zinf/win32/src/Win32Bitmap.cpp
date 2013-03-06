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

   $Id: Win32Bitmap.cpp,v 1.1 2004/10/13 16:17:07 linha2 Exp $
____________________________________________________________________________*/ 

#include <assert.h>
#include "stdlib.h"
#include "string"
#include "Win32Bitmap.h"
#include "Median.h"
#include "debug.h"

#define DB Debug_v("%s:%d\n", __FILE__, __LINE__);

Win32Bitmap::Win32Bitmap(const string &oName)
            :Bitmap(oName)
{
   m_oBitmapName = oName;
   m_hBitmap = NULL;
   m_hMaskBitmap = NULL;
   m_pBitmapData = NULL;
   m_pMaskInfo = NULL;
}

Win32Bitmap::Win32Bitmap(int iWidth, int iHeight, const string &oName)
           :Bitmap(oName)
{
   HDC            hDc;
   BITMAPINFO     sBitmapInfo;
   COLORREF      *pColorRefPtr;

   m_iHeight = iHeight;
   m_iWidth = iWidth;

   hDc = GetDC(NULL);
   if (GetDeviceCaps(hDc, RASTERCAPS) & RC_PALETTE)
   {
       m_hBitmap = CreateBitmap(iWidth, iHeight, 1, 8, NULL);
       m_hMaskBitmap = CreateBitmap(iWidth, iHeight, 1, 1, NULL);
   }
   else
   {
       sBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
       sBitmapInfo.bmiHeader.biWidth = iWidth; 
       sBitmapInfo.bmiHeader.biHeight = iHeight; 
       sBitmapInfo.bmiHeader.biPlanes = 1; 
       sBitmapInfo.bmiHeader.biCompression = BI_RGB; 
       sBitmapInfo.bmiHeader.biSizeImage = 0; 
       sBitmapInfo.bmiHeader.biXPelsPerMeter = 0; 
       sBitmapInfo.bmiHeader.biYPelsPerMeter = 0; 
       sBitmapInfo.bmiHeader.biClrUsed = 0; 
       sBitmapInfo.bmiHeader.biClrImportant = 0; 
       sBitmapInfo.bmiHeader.biBitCount = 24;

       m_hBitmap = CreateDIBSection(hDc, &sBitmapInfo, DIB_RGB_COLORS, 
                                    (void **)&m_pBitmapData, NULL, NULL);

       m_pMaskInfo = (BITMAPINFO *)new char[sizeof(BITMAPINFOHEADER) + 
                                       64 * sizeof(RGBQUAD)];
       m_pMaskInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
       m_pMaskInfo->bmiHeader.biWidth = iWidth;
       m_pMaskInfo->bmiHeader.biHeight = iHeight;
       m_pMaskInfo->bmiHeader.biPlanes = 1;
       m_pMaskInfo->bmiHeader.biBitCount = 1;
       m_pMaskInfo->bmiHeader.biCompression = BI_RGB;
       m_pMaskInfo->bmiHeader.biSizeImage = 0;
       m_pMaskInfo->bmiHeader.biXPelsPerMeter = 0;
       m_pMaskInfo->bmiHeader.biYPelsPerMeter = 0;
       m_pMaskInfo->bmiHeader.biClrUsed = 2;
       m_pMaskInfo->bmiHeader.biClrImportant = 0;
       pColorRefPtr = (COLORREF *)(&m_pMaskInfo->bmiColors);
       pColorRefPtr[1] = RGB(255,255,255);
       pColorRefPtr[0] = RGB(0,0,0);
       m_hMaskBitmap = CreateDIBSection(hDc, m_pMaskInfo, DIB_RGB_COLORS, 
                                    (void **)&m_pMaskBitmapData, NULL, NULL);
   }       
   ReleaseDC(NULL, hDc);
}

Win32Bitmap::~Win32Bitmap(void)
{
   if (m_hBitmap)
   {
       DeleteObject(m_hBitmap);
       m_hBitmap = NULL;
   }
   if (m_hMaskBitmap)
   {
       DeleteObject(m_hMaskBitmap);
       m_hMaskBitmap = NULL;
   }    
   delete [] m_pMaskInfo;
}

Error Win32Bitmap::LoadBitmapFromDisk(string &oFile)
{
   DIBSECTION sSection;
   BITMAPINFO sBitmapInfo;
   
   m_hBitmap = (HBITMAP)LoadImage(NULL, oFile.c_str(), IMAGE_BITMAP,
                                  0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
   if (m_hBitmap == NULL)
           return kError_LoadBitmapFailed;

   GetObject(m_hBitmap, sizeof(DIBSECTION), (LPSTR)&sSection);
   memcpy(&sBitmapInfo, &sSection.dsBmih, sizeof(BITMAPINFOHEADER));
   
   m_iHeight = sBitmapInfo.bmiHeader.biHeight;
   m_iWidth = sBitmapInfo.bmiHeader.biWidth;
   m_pBitmapData = sSection.dsBm.bmBits;

   if (m_bHasTransColor)
      CreateMaskBitmap();

   return kError_NoErr;
}

HBITMAP Win32Bitmap::GetBitmapHandle(void)
{
   return m_hBitmap;
}

HBITMAP Win32Bitmap::GetMaskBitmapHandle(void)
{
   return m_hMaskBitmap;
}

void Win32Bitmap::CreateMaskBitmap(void)
{
   BITMAP      sInfo;
   BITMAPINFO *pInfo, *pMaskInfo;
   HDC         hRootDC, hMemDC;
   int         iLine, iCol;
   char       *pData, *pMaskData;
   Color       sTrans, *pColorPtr;
   COLORREF   *pColorRefPtr;

   hRootDC = GetDC(NULL);
   hMemDC = CreateCompatibleDC(hRootDC);
   ReleaseDC(NULL, hRootDC);

   if (m_hMaskBitmap)
      DeleteObject(m_hMaskBitmap);

   GetObject(m_hBitmap, sizeof(BITMAP), (LPSTR)&sInfo);
   m_hMaskBitmap = CreateBitmap(sInfo.bmWidth, sInfo.bmHeight, 1, 1, NULL);
   
   pInfo = (BITMAPINFO *)new char[sizeof(BITMAPINFOHEADER)];
   pInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   pInfo->bmiHeader.biWidth = sInfo.bmWidth;
   pInfo->bmiHeader.biHeight = sInfo.bmHeight;
   pInfo->bmiHeader.biPlanes = 1;
   pInfo->bmiHeader.biBitCount = 24;
   pInfo->bmiHeader.biCompression = BI_RGB;
   pInfo->bmiHeader.biSizeImage = 0;
   pInfo->bmiHeader.biXPelsPerMeter = 0;
   pInfo->bmiHeader.biYPelsPerMeter = 0;
   pInfo->bmiHeader.biClrUsed = 0;
   pInfo->bmiHeader.biClrImportant = 0;

   pMaskInfo = (BITMAPINFO *)new char[sizeof(BITMAPINFOHEADER) + 
                                      64 * sizeof(RGBQUAD)];
   pMaskInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   pMaskInfo->bmiHeader.biWidth = sInfo.bmWidth;
   pMaskInfo->bmiHeader.biHeight = sInfo.bmHeight;
   pMaskInfo->bmiHeader.biPlanes = 1;
   pMaskInfo->bmiHeader.biBitCount = 1;
   pMaskInfo->bmiHeader.biCompression = BI_RGB;
   pMaskInfo->bmiHeader.biSizeImage = 0;
   pMaskInfo->bmiHeader.biXPelsPerMeter = 0;
   pMaskInfo->bmiHeader.biYPelsPerMeter = 0;
   pMaskInfo->bmiHeader.biClrUsed = 2;
   pMaskInfo->bmiHeader.biClrImportant = 0;
   pColorRefPtr = (COLORREF *)(&pMaskInfo->bmiColors);
   pColorRefPtr[0] = RGB(255,255,255);
   pColorRefPtr[1] = RGB(0,0,0);

   int imageSpan  = ((((sInfo.bmWidth * 24) + 31) & ~31) >> 3);
   int maskSpan   = ((((sInfo.bmWidth * 1 ) + 31) & ~31) >> 3);
   int nImageSize = imageSpan  * sInfo.bmHeight;
   int nMaskSize  = maskSpan   * sInfo.bmHeight;
   
   pData     = new char[nImageSize];
   pMaskData = new char[nMaskSize]; 
   memset (pMaskData, 0, nMaskSize);
 
   if (GetDIBits(hMemDC, m_hBitmap, 0, sInfo.bmHeight, pData, pInfo, DIB_RGB_COLORS))
   {
       char* pImagePtr = pData;     // points to current line of image
       char* pMaskPtr  = pMaskData; // points to current line of mask
       char* pMaskCol;              // points to current column of mask
 
       for (iLine = 0; iLine<sInfo.bmHeight; iLine++) 
       {
           pColorPtr = (Color*)pImagePtr; // reset image column pointer
           pMaskCol  = pMaskPtr;    // reset mask column to beginning of line
 
           for (iCol = 0; iCol<sInfo.bmWidth; iCol++)
           {
             if (iCol && (iCol%8==0)) // increment mask column every 8 bits
             {
                 pMaskCol++;
             }
             if (pColorPtr->red != m_oTransColor.blue ||
                 pColorPtr->green != m_oTransColor.green ||
                 pColorPtr->blue != m_oTransColor.red)
             {
                 *pMaskCol |= (0x80 >> (iCol % 8)); // set the correct mask bit
             }
             pColorPtr++;             // increment image column pointer
           }
           pImagePtr += imageSpan;    // add span to image line pointer
           pMaskPtr  += maskSpan;     // add span to mask line pointer
       }
       SetDIBits(hMemDC, m_hMaskBitmap, 0, sInfo.bmHeight, pMaskData, pMaskInfo,
                 DIB_RGB_COLORS);
   }

   delete [] pData;
   delete [] pMaskData;
   delete [] pInfo;
   delete [] pMaskInfo;
   DeleteDC(hMemDC);
}

bool Win32Bitmap::IsPosVisible(Pos &oPos)
{
   HDC      hRootDC, hMemDC;
   COLORREF sColor;
   HBITMAP  hSaved;

   if (!m_hMaskBitmap)
      return true;

   hRootDC = GetDC(NULL);
   hMemDC = CreateCompatibleDC(hRootDC);
   ReleaseDC(NULL, hRootDC);

   hSaved = (HBITMAP)SelectObject(hMemDC, m_hMaskBitmap);
   sColor = GetPixel(hMemDC, oPos.x, oPos.y);
   SelectObject(hMemDC, hSaved);
   DeleteDC(hMemDC);
   
   return sColor == 0;
}

Error Win32Bitmap::BlitRect(Bitmap *pOther, Rect &oSrcRect, 
                            Rect &oDestRect)
{
   HDC hRootDC, hSrcDC, hDestDC;
   Win32Bitmap *pSrcBitmap = (Win32Bitmap *)pOther;
 
   if (pSrcBitmap->m_hBitmap == NULL)
      return kError_UnknownErr;

   hRootDC = GetDC(NULL);
   hSrcDC = CreateCompatibleDC(hRootDC);
   hDestDC = CreateCompatibleDC(hRootDC);
   ReleaseDC(NULL, hRootDC);
   
   DeleteObject(SelectObject(hSrcDC, pSrcBitmap->m_hBitmap));
   DeleteObject(SelectObject(hDestDC, m_hBitmap));

   BitBlt(hDestDC, oDestRect.x1, oDestRect.y1, 
                 oDestRect.Width(), oDestRect.Height(),
                 hSrcDC, oSrcRect.x1, oSrcRect.y1, SRCCOPY);
   
   DeleteDC(hSrcDC);
   DeleteDC(hDestDC);

   return kError_NoErr;
}

Error Win32Bitmap::MaskBlitRect(Bitmap *pOther, Rect &oSrcRect, 
                                Rect &oDestRect)
{
   HDC hRootDC, hSrcDC, hDestDC, hMaskDC;
   Win32Bitmap *pSrcBitmap = (Win32Bitmap *)pOther;

   if (!pSrcBitmap->m_hMaskBitmap)
      return BlitRect(pOther, oSrcRect, oDestRect);
   
   if (pSrcBitmap->m_hBitmap == NULL)
      return kError_UnknownErr;
   
   hRootDC = GetDC(NULL);
   hSrcDC = CreateCompatibleDC(hRootDC);
   hDestDC = CreateCompatibleDC(hRootDC);
   hMaskDC = CreateCompatibleDC(hRootDC);
   ReleaseDC(NULL, hRootDC);
   
   SelectObject(hSrcDC, pSrcBitmap->m_hBitmap);
   SelectObject(hMaskDC, pSrcBitmap->m_hMaskBitmap);
   SelectObject(hDestDC, m_hBitmap);

   SetBkColor(hSrcDC, RGB(m_oTransColor.red,m_oTransColor.green,m_oTransColor.blue));
   BitBlt(hDestDC, oDestRect.x1, oDestRect.y1, 
                   oDestRect.Width(), oDestRect.Height(),
          hSrcDC, oSrcRect.x1, oSrcRect.y1, SRCINVERT);
   BitBlt(hDestDC, oDestRect.x1, oDestRect.y1, 
                   oDestRect.Width(), oDestRect.Height(),
          hMaskDC, oSrcRect.x1, oSrcRect.y1, SRCAND);
   BitBlt(hDestDC, oDestRect.x1, oDestRect.y1, 
                   oDestRect.Width(), oDestRect.Height(),
          hSrcDC, oSrcRect.x1, oSrcRect.y1, SRCINVERT);

   DeleteDC(hSrcDC);
   DeleteDC(hDestDC);
   DeleteDC(hMaskDC);

   return kError_NoErr;
}

Error Win32Bitmap::BlitRectMaskBitmap(Bitmap *pOther, Rect &oSrcRect, 
                                      Rect &oDestRect)
{
   HDC hRootDC, hSrcDC, hDestDC;
   Win32Bitmap *pSrcBitmap = (Win32Bitmap *)pOther;
 
   if (pSrcBitmap->m_hMaskBitmap == NULL)
      return kError_UnknownErr;

   hRootDC = GetDC(NULL);
   hSrcDC = CreateCompatibleDC(hRootDC);
   hDestDC = CreateCompatibleDC(hRootDC);
   ReleaseDC(NULL, hRootDC);
   
   DeleteObject(SelectObject(hSrcDC, pSrcBitmap->m_hMaskBitmap));
   DeleteObject(SelectObject(hDestDC, m_hMaskBitmap));

   BitBlt(hDestDC, oDestRect.x1, oDestRect.y1, 
                 oDestRect.Width(), oDestRect.Height(),
                 hSrcDC, oSrcRect.x1, oSrcRect.y1, SRCCOPY);
   
   DeleteDC(hSrcDC);
   DeleteDC(hDestDC);

   return kError_NoErr;
}
void Win32Bitmap::UpdateHistogram(WORD *pHist)
{
   BITMAP      sInfo;
   BITMAPINFO *pInfo;
   HDC         hRootDC, hMemDC;
   int         iRet, iLine, iCol;
   char       *pData;
   Color       sTrans, *pColorPtr;

   hRootDC = GetDC(NULL);
   hMemDC = CreateCompatibleDC(hRootDC);
   ReleaseDC(NULL, hRootDC);

   GetObject(m_hBitmap, sizeof(BITMAP), (LPSTR)&sInfo);
   
   pInfo = (BITMAPINFO *)new char[sizeof(BITMAPINFOHEADER)];
   pInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   pInfo->bmiHeader.biWidth = sInfo.bmWidth;
   pInfo->bmiHeader.biHeight = sInfo.bmHeight;
   pInfo->bmiHeader.biPlanes = 1;
   pInfo->bmiHeader.biBitCount = 24;
   pInfo->bmiHeader.biCompression = BI_RGB;
   pInfo->bmiHeader.biSizeImage = 0;
   pInfo->bmiHeader.biXPelsPerMeter = 0;
   pInfo->bmiHeader.biYPelsPerMeter = 0;
   pInfo->bmiHeader.biClrUsed = 0;
   pInfo->bmiHeader.biClrImportant = 0;

   pData = new char[sInfo.bmWidth * 4];
   for(iLine = 0; iLine < sInfo.bmHeight; iLine++)
   {
       iRet = GetDIBits(hMemDC, m_hBitmap, 
                        (sInfo.bmHeight - 1) - iLine, 1, 
                        pData, pInfo, DIB_RGB_COLORS);
       if (!iRet)
       {
          break;
       }   

       for(iCol = 0, pColorPtr = (Color *)pData; 
           iCol < sInfo.bmWidth; iCol++, pColorPtr++)
       {    
          if (pColorPtr->red != m_oTransColor.blue ||
              pColorPtr->green != m_oTransColor.green ||
              pColorPtr->blue != m_oTransColor.red)
               pHist[mcRGB(pColorPtr->blue, pColorPtr->green, pColorPtr->red)]++;
              
       }      
   }

   delete [] pData;
   delete [] pInfo;
   DeleteDC(hMemDC);
}

void Win32Bitmap::ConvertTo256Color(WORD            *pHist,
                                    RGBQUAD         *pColorTable)
{
   BITMAP         sInfo;
   BITMAPINFO    *pInfo;
   HDC            hRootDC, hMemDC;
   int            iRet, iLine, iCol, iPack;
   unsigned char *pData, *p8BitData, *pDest;
   Color          sTrans, *pColorPtr;
   HBITMAP        h8BitBitmap;
               
   hRootDC = GetDC(NULL);
   hMemDC = CreateCompatibleDC(hRootDC);
   ReleaseDC(NULL, hRootDC);

   GetObject(m_hBitmap, sizeof(BITMAP), (LPSTR)&sInfo);
   
   pInfo = (BITMAPINFO *)new char[sizeof(BITMAPINFOHEADER)];
   pInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   pInfo->bmiHeader.biWidth = sInfo.bmWidth;
   pInfo->bmiHeader.biHeight = sInfo.bmHeight;
   pInfo->bmiHeader.biPlanes = 1;
   pInfo->bmiHeader.biBitCount = 24;
   pInfo->bmiHeader.biCompression = BI_RGB;
   pInfo->bmiHeader.biSizeImage = 0;
   pInfo->bmiHeader.biXPelsPerMeter = 0;
   pInfo->bmiHeader.biYPelsPerMeter = 0;
   pInfo->bmiHeader.biClrUsed = 0;
   pInfo->bmiHeader.biClrImportant = 0;

   iPack = sInfo.bmWidth % 2; 
   
   pData = new unsigned char[sInfo.bmWidth * 4];
   p8BitData = new unsigned char[(sInfo.bmWidth + iPack) * sInfo.bmHeight];
   for(iLine = 0, pDest = p8BitData; iLine < sInfo.bmHeight; iLine++)
   {
       iRet = GetDIBits(hMemDC, m_hBitmap, 
                        (sInfo.bmHeight - 1) - iLine, 1, 
                        pData, pInfo, DIB_RGB_COLORS);
       if (!iRet)
       {
          break;
       }   

       for(iCol = 0, pColorPtr = (Color *)pData; 
           iCol < sInfo.bmWidth; iCol++, pColorPtr++, pDest++)
       {
          *pDest = pHist[mcRGB(pColorPtr->blue, pColorPtr->green, pColorPtr->red)] + 10;
       }
       pDest += iPack;
   }

   h8BitBitmap = CreateBitmap(sInfo.bmWidth, sInfo.bmHeight, 1, 8, p8BitData);
   
   delete [] pData;
   delete [] p8BitData;
   delete [] pInfo;
   DeleteDC(hMemDC);

   if (m_hBitmap)
   {
      DeleteObject(m_hBitmap);
      m_pBitmapData = NULL;
   }
      
   m_hBitmap = h8BitBitmap;
   m_pBitmapData = NULL;
}

void Win32Bitmap::SetPalette(HPALETTE hPal)
{
   if (m_hPal)
       DeleteObject(m_hPal);
       
   m_hPal = hPal;
}

Bitmap *Win32Bitmap::Clone(void)
{
   Win32Bitmap *pTemp;
   Rect         oRect;

   pTemp = new Win32Bitmap(m_iWidth, m_iHeight, m_oBitmapName);
   pTemp->m_oTransColor = m_oTransColor;
   pTemp->m_bHasTransColor = m_bHasTransColor;
   oRect.x1 = oRect.y1 = 0;
   oRect.x2 = m_iWidth;
   oRect.y2 = m_iHeight;
   pTemp->BlitRect(this, oRect, oRect);
   pTemp->BlitRectMaskBitmap(this, oRect, oRect);

   return pTemp;
}

Error Win32Bitmap::MakeTransparent(Rect &oRect)
{
   HDC    hRootDC, hDC;
   RECT   sRect;
 
   if (m_hMaskBitmap == NULL)
      return kError_UnknownErr;

   hRootDC = GetDC(NULL);
   hDC = CreateCompatibleDC(hRootDC);
   ReleaseDC(NULL, hRootDC);

   sRect.left = oRect.x1;
   sRect.right = oRect.x2;
   sRect.top = oRect.y1;
   sRect.bottom = oRect.y2;
   
   SelectObject(hDC, m_hMaskBitmap);
   FillRect(hDC, &sRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
   DeleteDC(hDC);

   return kError_NoErr;
}

void Win32Bitmap::BlitIt(int x, int y)
{
   HDC hSrcDC, hDestDC;

   if (m_hBitmap == NULL)
   {
      return;
   }

   hDestDC = GetDC(NULL);
   hSrcDC = CreateCompatibleDC(hDestDC);
   
   SelectObject(hSrcDC, m_hBitmap);

   BitBlt(hDestDC, x, y, 
          m_iWidth, m_iHeight,
          hSrcDC, 0, 0,
		  SRCCOPY);

   ReleaseDC(NULL, hDestDC);
   DeleteDC(hSrcDC);
}

void Win32Bitmap::GetColor(Pos oPos, Color &oColor)
{
}

void Win32Bitmap::GetSize(Pos &oPos)
{
    oPos.x = m_iWidth;
    oPos.y = m_iHeight;
}
