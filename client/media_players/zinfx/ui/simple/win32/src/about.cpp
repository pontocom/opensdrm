/*____________________________________________________________________________
	
	FreeAMP - The Free MP3 Player
	Portions copyright (C) 1998 EMusic.com

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
	
	$Id: about.cpp,v 1.1 2004/10/13 16:16:17 linha2 Exp $
____________________________________________________________________________*/

/* System Includes */
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>

/* Project Includes */
#include <config.h>

#include "resource.h"

#define ADDHEIGHT	0
#define XINCREMENT	6
#define YINCREMENT	10
#define EDGEWIDTH	6

#define VERSIONX	32
#define VERSIONY	26

static BOOL		showCredits;
static int32	numBits;
static int32	xSize;
static int32 	ySize;
static int32	xPos;
static int32	yPos;
static int32	aboutIndex;
static int32	creditsIndex;
static char		weblink[256];
static HBITMAP	aboutBitmap;
static HBITMAP	creditsBitmap;
static COLORREF	crVersionTextColor;



BOOL CALLBACK AboutProc(HWND hwnd, 
						UINT msg, 
						WPARAM wParam, 
						LPARAM lParam )
{
	BOOL result = FALSE;
	
	switch(msg)
	{
		case WM_INITDIALOG:
		{
			HINSTANCE hinst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
			HWND hwndOK = GetDlgItem(hwnd, IDOK);
			HWND hwndWeb = GetDlgItem(hwnd, IDC_WEBLINK);

			showCredits = FALSE;
			numBits = 0;
			
			LoadString(	hinst,
						IDS_WEBLINK,
						weblink,
						sizeof(weblink));

			HDC hdc = GetDC(hwnd);

			if(hdc)
			{
				numBits = GetDeviceCaps(hdc, BITSPIXEL) * 
					GetDeviceCaps(hdc, PLANES);
					
				ReleaseDC(hwnd, hdc);
			}

			aboutIndex = IDB_ABOUT8;
			//creditsIndex = IDB_CREDITS8;
			crVersionTextColor = RGB (0,0,0);
			
			aboutBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(aboutIndex));
			//creditsBitmap = LoadBitmap(hinst, MAKEINTRESOURCE(creditsIndex));

			BITMAP bmp;
	
			GetObject(aboutBitmap, sizeof(BITMAP), (LPSTR)&bmp);
			
			xSize = bmp.bmWidth;
			ySize = bmp.bmHeight;
			
			xPos = (GetSystemMetrics (SM_CXFULLSCREEN) - xSize - EDGEWIDTH) / 2;
			yPos = (GetSystemMetrics (SM_CYFULLSCREEN) - ySize) / 2;
				
			SetWindowPos (	hwnd,
							HWND_TOP, 
							xPos, 
							yPos, 
							xSize + 6, 
							ySize + ADDHEIGHT, 
							0);

			RECT rect;

			GetClientRect(hwndOK, &rect);
	
			int x,y;

			x = xSize - XINCREMENT - rect.right;
			y = ySize + ADDHEIGHT - YINCREMENT - rect.bottom;
			
			SetWindowPos(	hwndOK,
							0, 
							x, 
							y, 
							0, 
							0, 
							SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
			
			
			
			GetClientRect(hwndWeb, &rect);
						
			x = x - XINCREMENT - rect.right;
			y = ySize + ADDHEIGHT - YINCREMENT - rect.bottom;
			
			SetWindowPos(	hwndWeb,
							NULL, 
							x, 
							y, 
							0, 
							0, 
							SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);


			result = TRUE;

			break;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint (hwnd, &ps);
			HDC memdc = CreateCompatibleDC (hdc);
			HBITMAP oldBitmap;

			if (showCredits) 
			{
				//oldBitmap = SelectObject(memdc, creditsBitmap);
			}
			else 
			{
				oldBitmap = (HBITMAP)SelectObject(memdc, aboutBitmap);
			}

			BitBlt (hdc, 0, 0, xSize, ySize, memdc, 0, 0, SRCCOPY);
			SelectObject (memdc, oldBitmap);
			DeleteDC (memdc);
			EndPaint (hwnd, &ps);

			result = FALSE;
			break;
		}

		case WM_COMMAND:
		{
			switch(wParam)
			{
				case IDC_WEBLINK:
				{
					ShellExecute(	NULL, 
									"open", 
									weblink, 
									NULL, 
									"C:\\", 
									SW_SHOWNORMAL);
				}
				case IDOK:
				case IDCANCEL:
				{
					DeleteObject(aboutBitmap);
					DeleteObject(creditsBitmap);
					EndDialog(hwnd, TRUE);
					break;
				}

			}
		}
	}

	return result;
}
