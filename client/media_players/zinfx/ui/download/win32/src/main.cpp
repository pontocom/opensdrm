/*____________________________________________________________________________
	
	FreeAMP - The Free MP3 Player
	Portions copyright (C) 1999 EMusic.com

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
	
	$Id: main.cpp,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/

/* System Includes */
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

/* Project Includes */
#include "event.h"
#include "about.h"
#include "resource.h"


BOOL CALLBACK MainProc(	HWND hwnd, 
						UINT msg, 
						WPARAM wParam, 
						LPARAM lParam ); 


BOOL CALLBACK MainProc(	HWND hwnd, 
						UINT msg, 
						WPARAM wParam, 
						LPARAM lParam )
{
	BOOL result = FALSE;
	//static Player* player = NULL;

	switch(msg)
	{
		case WM_INITDIALOG:
		{
			HWND hwndStatus = NULL;
			HWND hwndPlay	= GetDlgItem(hwnd, IDC_PLAY);
			HWND hwndStop	= GetDlgItem(hwnd, IDC_STOP);
			HWND hwndPause	= GetDlgItem(hwnd, IDC_PAUSE);
			HWND hwndNext	= GetDlgItem(hwnd, IDC_NEXTSONG);
			HWND hwndLast	= GetDlgItem(hwnd, IDC_LASTSONG);
			HWND hwndSlider	= GetDlgItem(hwnd, IDC_SLIDER);
			char* cmdline	= (char*) lParam;

			EnableWindow(hwndPlay, FALSE);
			EnableWindow(hwndNext, FALSE);
			EnableWindow(hwndLast, FALSE);

			EnableWindow(hwndStop, FALSE);
			EnableWindow(hwndPause, FALSE);
			EnableWindow(hwndSlider, FALSE);

			hwndStatus= CreateStatusWindow(	WS_CHILD | WS_VISIBLE, 
											"", 
											hwnd, 
											IDC_STATUS);
			const int32 kNumPanes = 3;
			int32 panes[kNumPanes]= {60, 125,-1};

			SendMessage(hwndStatus, SB_SETPARTS, kNumPanes, (LPARAM) panes);

			//player = new Player();

			//SetProp( hwnd, "player",(HANDLE)player); 
			
			//player->SetCallback(CallbackFunction, hwnd);

			/* turn it on - heh heh*/
			//player->Power();

			if(*cmdline)
			{
				//PlayList* playlist = new PlayList;

				/* these are magical variables provided for us
				   in MS's STDLIB.H file. Much easier than 
				   writing our own parsing code */
				for (int i = 1; i < __argc; i++)
				{
					//playlist->Add(__argv[i]);
				}

				//player->Load(playlist);
			}
			/*else
			{
				PlayList* playlist = new PlayList;

				playlist->Add(kSong1);
				playlist->Add(kSong2);

				player->Load(playlist);
			}*/

			result = TRUE;

			break;
		}

		case WM_COMMAND:
		{
			switch(wParam)
			{
				case IDC_PLAY:
				{
					break;
				}

				case IDC_PAUSE:
				{
					break;
				}

				case IDC_STOP:
				{
					break;
				}

				case IDC_NEXTSONG:
				{
					break;
				}

				case IDC_LASTSONG:
				{
					break;
				}

				case IDC_OPEN:
				{
					OPENFILENAME ofn;
					char szTitle[] = "Open Audio File";
					char szFilter[] =
					"MPEG Audio Streams (.mp1, .mp2, .mp3, .mpp)\0"
					"*.mp1;*.mp2;*.mp3;*.mpp\0"
					//"Playlists (.txt, .lst, .m3u)\0"
					//"*.lst;*.m3u;*.txt\0"
					"All Files (*.*)\0"
					"*.*\0";
					const int32 kBufferSize = MAX_PATH * 128;
					char* filelist = new char[kBufferSize];
					
					*filelist = 0x00;

					// Setup open file dialog box structure
					ofn.lStructSize       = sizeof(OPENFILENAME);
					ofn.hwndOwner         = hwnd;
					ofn.hInstance         = (HINSTANCE)GetWindowLong(hwnd, 
															GWL_HINSTANCE);
					ofn.lpstrFilter       = szFilter;
					ofn.lpstrCustomFilter = NULL;
					ofn.nMaxCustFilter    = 0;
					ofn.nFilterIndex      = 1;
					ofn.lpstrFile         = filelist;
					ofn.nMaxFile          = kBufferSize;
					ofn.lpstrFileTitle    = NULL;
					ofn.nMaxFileTitle     = 0;
					ofn.lpstrInitialDir   = "";
					ofn.lpstrTitle        = szTitle;
					ofn.Flags             = OFN_FILEMUSTEXIST | 
											OFN_PATHMUSTEXIST |
  	     				   					OFN_HIDEREADONLY | 
											OFN_ALLOWMULTISELECT |
											OFN_EXPLORER;
					ofn.nFileOffset       = 0;
					ofn.nFileExtension    = 0;
					ofn.lpstrDefExt       = "MP*";
					ofn.lCustData         = 0;
					ofn.lpfnHook          = NULL;
					ofn.lpTemplateName    = NULL;

					if(GetOpenFileName(&ofn))
					{
						char file[MAX_PATH + 1];
						char* cp = NULL;
						//PlayList* templist;

						//templist = new PlayList;

						strcpy(file, filelist);
						strcat(file, "\\");

						cp = filelist + ofn.nFileOffset;

						while(*cp)
						{
							strcpy(file + ofn.nFileOffset, cp);

							//templist->Add(file);

							cp += strlen(cp) + 1;
						}

						/*PlayList* playlist = player->Load(templist);

						if(playlist)
						{
							delete playlist;
						}*/
					}

					delete [] filelist;

					break;
				}

				case IDC_ABOUT:
				{
					HINSTANCE hinst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
					
					DialogBox(	hinst, 
								MAKEINTRESOURCE(IDD_ABOUT),
								NULL,
								AboutProc);
					break;
				}

				case IDC_EDIT_CURRENTPLAYLIST:
				{
					HINSTANCE hinst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
					
					/*PlayList* playlist = player->CurrentPlayList();

					DialogBoxParam(	hinst, 
									MAKEINTRESOURCE(IDD_PLAYLISTEDITOR),
									NULL,
									PlayListProc,
									(LPARAM)playlist);*/
					break;
				}

				case IDC_EDIT_PLAYLISTFILE:
				{
					HINSTANCE hinst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
					
					/*PlayList* playlist = player->CurrentPlayList();

					DialogBoxParam(	hinst, 
									MAKEINTRESOURCE(IDD_PLAYLISTEDITOR),
									NULL,
									PlayListProc,
									(LPARAM)playlist);*/
					break;
				}

				case IDC_EXIT:
				{
					PostQuitMessage(0);
					break;
				}
			}

			result = TRUE;
			break;
		}

		case WM_DROPFILES:
		{
			HDROP hDrop = (HDROP) wParam;
			int32 count;
			char szFile[MAX_PATH + 1];
			//PlayList* templist;

			count = DragQueryFile(	hDrop,
									-1L,
									szFile,
									sizeof(szFile));

			//templist = new PlayList;

			for(int32 i = 0; i < count; i++)
			{
				DragQueryFile(	hDrop,
								i,
								szFile,
								sizeof(szFile));

				//templist->Add(szFile);
			}

			/*PlayList* playlist = player->Load(templist);

			if(playlist)
			{
				delete playlist;
			}*/

			break;
		}

		case WM_HSCROLL:
		{
			HWND hwndSlider = GetDlgItem(hwnd, IDC_SLIDER);
			LRESULT position;

			switch (LOWORD(wParam)) 
			{
				case TB_LINEUP:
				case TB_LINEDOWN:
				case TB_PAGEDOWN:
				case TB_PAGEUP:
				case TB_THUMBPOSITION:
				{
		  			position = SendMessage(	hwndSlider, 
											TBM_GETPOS, 
											0, 
											0);

					//player->SeekToPosition(position);
		  	
					g_scrolling = false;
					break;
				}

				case TB_THUMBTRACK:
				{
					g_scrolling = true;
					break;
				}
			}

			break;
        }

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			result = TRUE;
			break;
		}
	
	}

	return result;
}
