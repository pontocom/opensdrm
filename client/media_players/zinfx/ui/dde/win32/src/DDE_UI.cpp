/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player

	Portions Copyright (C) 1998 EMusic.com

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
	
	$Id: DDE_UI.cpp,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/

#include <windows.h>
#include <dde.h>
#include <stdio.h>
#include <math.h>

#include "DDE_UI.h"
#include "Playlist.h"
#include "event.h"
#include "eventdata.h"
#include "thread.h"

#define DDE_TIMEOUT	3000
#define STEP_SIZE	10
#define kAdviseFileName 	1
#define kAdviseSamplingFreq	2

HINSTANCE g_hInstance = NULL;

enum {
	kFastForward = 0,
	kFastReverse,
	kFileName,
	kNextTrack,
	kPause,
	kPlay,
	kPreviousTrack,
	kSamplingFreq,
	kShuffle,
	kStop
};

static const char *zCommands[11] = { "fast-forward", "fast-reverse", "fileName", "next", "pause",
	"play", "previous", "samplingFreq", "shuffle", "stop", NULL };

static long typeFromIndex[11] = {
	0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0
};

extern "C" DDE_UI *Initialize(FAContext *context)
{
    return new DDE_UI(context);
}


INT WINAPI DllMain(HINSTANCE hInst, ULONG ul_reason_being_called, LPVOID lpReserved)

{
	switch (ul_reason_being_called) {
		case DLL_PROCESS_ATTACH:
			g_hInstance = hInst;
	    	break;
		case DLL_THREAD_ATTACH:
		    break;
		case DLL_THREAD_DETACH:
		    break;
		case DLL_PROCESS_DETACH:
		    break;
	}
    return 1;                 
}


void DDE_UI::ParseArgs(int32 argc, char** argv)

{
/*    PlaylistManager	*playlist = plm;
    char         	*arg = NULL;
    bool         	shuffle = false;
    bool        	autoplay = false;
    int32         	count = 0;

    for(int32 i = 1;i < argc; i++)  {
	    arg = argv[i];

	    if (arg[0] == '-') {
	        switch (arg[1]) {
		        case 's':
                    shuffle = true;
		            break;
                case 'p':
                    autoplay = true;
		            break;
            }
        } else {
            playlist->AddItem(arg,0);
            count++;
	    }
    }

    playlist->SetCurrentIndex(0);

    if(shuffle) 
        playlist->SetShuffleMode(true);
*/
}


Error DDE_UI::Init(int32 startup_type) 

{ 
    ParseArgs(context->argc, context->argv);
    return kError_NoErr;
}


DDE_UI::DDE_UI(FAContext *theContext) 

{
	adviseList = NULL;
    context = theContext;
    plm = context->plm;
    target = context->target;
	winHwnd = NULL;
    uiThread = Thread::CreateThread();
    uiThread->Create(DDE_UI::UIThreadFunc,this);
}


DDE_UI::~DDE_UI() 

{
    if (uiThread) {
		uiThread->Destroy();
		delete uiThread;
		uiThread = NULL;
    }
}


void DDE_UI::CreateUI()

{
	WNDCLASSEX wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = g_hInstance;
	wndclass.hIcon = NULL;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "FreeAmpDDE";
	wndclass.hIconSm = NULL;

	RegisterClassEx(&wndclass);

	winHwnd = CreateWindow("FreeAmpDDE", "FreeAmpDDE", WS_OVERLAPPEDWINDOW,
		                   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
                           CW_USEDEFAULT, NULL, NULL, g_hInstance, NULL);

    if(winHwnd)
    {
        MSG msg;

        while(GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}


void DDE_UI::UIThreadFunc(void *arg)

{
    DDE_UI	*ui = (DDE_UI*)arg;

    ui->CreateUI();
}


int DDE_UI::CommandSearch(const char *list[], int nCommands, char *name, int *key)

{
	int	i, j, k, cmp;

	i = 0;
	j = nCommands - 1;
	do {
		k = (i + j) >> 1;
		cmp = strcasecmp(list[k], name);
		if (cmp <= 0)
			i = k + 1;
		if (cmp >= 0)
			j = k - 1;
	} while (i <= j);
	if (strcasecmp(list[k], name) == 0) {
		*key = k;
		return(TRUE);
	} else {
		*key = -1;
		return(FALSE);
	}
}


long DDE_UI::CollectToSeparator(char **scanner, char separator, char *str)

{
        char    *lastChar;
        char    c;
        long    len = 0;

        lastChar = str;
        if (separator == ' ') {
                while ((**scanner == ' ') && (**scanner != '\n') && (**scanner != '\0'))
                        (*scanner)++;
        } else {
                while ((**scanner == ' ') && (**scanner != separator) &&
                         (**scanner != '\n') && (**scanner != '\0'))
                        (*scanner)++;
        }
        while ((**scanner != separator) && (**scanner != '\n') && (**scanner != '\0')) {
                c = *str++ = *(*scanner)++;
                if (c != ' ')
                        lastChar = str;
                len++;
        }
        *lastChar = '\0';
        if (**scanner == separator)
                (*scanner)++;
        return(len);
}


void DDE_UI::InterpretCommand(char *command)

{
	Event	*e;
	char	theResult[512];
	char	s[256];
	char 	*cmd;
	int32 	index;
	int32	cf;

	cmd = command;
	theResult[0] = '\0';
	CollectToSeparator(&cmd, ' ', s);
	if (CommandSearch(zCommands, 6, s, &index)) {
		switch (index) {
			case kFastForward:
				cf = (int32)((currSeconds + STEP_SIZE) / secondsPerFrame);
				if (cf > totalFrames)
					cf = totalFrames;
				currSeconds += STEP_SIZE;
				e = new ChangePositionEvent(cf);
				target->AcceptEvent(e);
				break;
			case kFastReverse:
				break;
			case kNextTrack:
				e = new Event(CMD_NextMediaPiece);
				target->AcceptEvent(e);
				break;
			case kPause:
				state = UIState_Paused;
				e = new Event(CMD_TogglePause);
				target->AcceptEvent(e);
				break;
			case kPlay:
				if (state == UIState_Paused) {
					e = new Event(CMD_TogglePause);
					target->AcceptEvent(e);
				} else {
					e = new Event(CMD_Play);
					target->AcceptEvent(e);
				}
				break;
			case kPreviousTrack:
				e = new Event(CMD_PrevMediaPiece);
				target->AcceptEvent(e);
				break;
			case kShuffle:
				if (plm) {
					plm->SetShuffleMode(true);
					plm->SetCurrentIndex(0);
				}
				e = new Event(CMD_Stop);
				target->AcceptEvent(e);
				e = new Event(CMD_Play);
				target->AcceptEvent(e);
				break;
			case kStop:
				e = new Event(CMD_Stop);
				target->AcceptEvent(e);
			default:
				break;
		}
		command[0] = '\0';
		if (theResult[0] != '\0') {
			strcpy(command, theResult);
			strcat(command, "\n");
		}
		strcat(command, "Ok.");	
	} else
		sprintf(command, "ERR> Command '%s' not recognized.", s);
}


FreeAmpAdvise *DDE_UI::FindAdvise(HWND theClient, long index)

{
	FreeAmpAdvise	*p;
	FreeAmpAdvise	*result = NULL;

	if (adviseList) {
		p = adviseList;
		while (p) {
			if (p->CheckTypeAndClient(theClient, index)) {
				result = p;
				break;
			}
		}
	}
	return(result);
}


void DDE_UI::RemoveAdvise(FreeAmpAdvise *theAdvise)

{
	FreeAmpAdvise	*p, *q;

	p = adviseList;
	q = NULL;
	while ((p) && (p != theAdvise))
		p = p->GetNext();
	if (p) {
		if (q)
			q->SetNext(theAdvise->GetNext());
		else
			adviseList = theAdvise->GetNext();
		delete theAdvise;
	}
}

BOOL DDE_UI::UpdateAdvises(HWND hwnd, short updateType, char *dataStr)

{
	FreeAmpAdvise	*p;
	HWND        	theClient;
	char         	*command;
	BOOL        	fDeferUpd;
	BOOL         	fAckReq;
	BOOL        	result = TRUE;

	switch (updateType) {
		case kAdviseFileName:
			command = "fileName";
			break;
		case kAdviseSamplingFreq:
			command = "sampleFreq";
			break;
		default:
			command = NULL;
			break;
	}
	p = adviseList;
	while ((p) && (result))  {
		if (p->GetType() == updateType) {
			theClient = p->GetClient(&fDeferUpd, &fAckReq);
			result = PostDataMessage(hwnd, theClient, command, dataStr, fDeferUpd, fAckReq, FALSE);
		}
		p = p->GetNext();
	}
	return(result);
}

BOOL DDE_UI::HandleAdvise(HWND hwnd, WORD wParam, DWORD lParam)

{
	HWND         	hwndClient;
	ATOM         	aItem;
	DDEACK      	ddeAck;
	GLOBALHANDLE	hDDEAdvise;
	DDEADVISE FAR	*lpDDEAdvise;
	FreeAmpAdvise  	*advise;
	short         	wStatus;
	short       	deferUpd, ackReq;
	short       	successful = FALSE;
	int         	index;
	char         	*cmd;
	char        	s[256];
	char        	command[256];
	unsigned int	uiLow, uiHi;

	UnpackDDElParam(WM_DDE_ADVISE, lParam, &uiLow, &uiHi);
	FreeDDElParam(WM_DDE_ADVISE, lParam);
	hDDEAdvise = (GLOBALHANDLE)uiLow;
	aItem = (ATOM)uiHi;
	lpDDEAdvise = (DDEADVISE *)GlobalLock(hDDEAdvise);
	hwndClient = (HWND)wParam;

	if ((lpDDEAdvise) && (lpDDEAdvise->cfFormat == CF_TEXT)) {
		GlobalGetAtomName(aItem, command, sizeof(command));
		cmd = command;
		CollectToSeparator(&cmd, ' ', s);
		if ((CommandSearch(zCommands, 11, s, &index)) && (index == kFileName)) {

			if (!FindAdvise(hwndClient, typeFromIndex[index])) {
				deferUpd = lpDDEAdvise->fDeferUpd;
				ackReq = lpDDEAdvise->fAckReq;
				advise = new FreeAmpAdvise((HWND)hwndClient, index, (char)deferUpd, (char)ackReq);
				GlobalUnlock(hDDEAdvise);
				GlobalFree(hDDEAdvise);
				ddeAck.bAppReturnCode = 0;
				ddeAck.reserved = 0;
				ddeAck.fBusy = FALSE;
				if (advise) {
					if (adviseList)
						adviseList->Add(advise);
					else
						adviseList = advise;
					ddeAck.fAck = TRUE;
				} else
					ddeAck.fAck = FALSE;
				wStatus = *(WORD *)&ddeAck;
				if (!PostMessage(hwndClient, WM_DDE_ACK, (WPARAM)hwnd, PackDDElParam(WM_DDE_ACK, wStatus, aItem))) {
					GlobalFree(hDDEAdvise);
					GlobalDeleteAtom(aItem);
				}
				successful = TRUE;
			}
		}
	}
	if (!successful) {
		ddeAck.bAppReturnCode = 0;
		ddeAck.reserved = 0;
		ddeAck.fBusy = FALSE;
		ddeAck.fAck = FALSE;
		wStatus = *(WORD *)&ddeAck;
		GlobalUnlock(hDDEAdvise);
		if (!PostMessage(hwndClient, WM_DDE_ACK, (WPARAM)hwnd, MAKELONG(wStatus, aItem))) {
			GlobalFree(hDDEAdvise);
			GlobalDeleteAtom(aItem);
		}
	}
	return 0;
}


BOOL DDE_UI::HandleUnadvise(HWND hwnd, WORD wParam, DWORD lParam)

{
	HWND         	hwndClient;
	ATOM         	aItem;
	DDEACK      	ddeAck;
	FreeAmpAdvise  	*p, *q, *adv;
	short       	cfFormat;
	short         	wStatus;
	int          	index;
	char        	command[256];

	hwndClient = (HWND)wParam;
	cfFormat = (short)LOWORD(lParam);
	aItem = HIWORD(lParam);

	ddeAck.bAppReturnCode = 0;
	ddeAck.reserved = 0;
	ddeAck.fBusy = FALSE;
	ddeAck.fAck = TRUE;

	if ((cfFormat == CF_TEXT) || (cfFormat == 0)) {
		if (aItem == NULL) {
			p = adviseList;   // Remove all advises
			while (p) {
				q = p;
				p = p->GetNext();
				delete q;
			}
			adviseList = NULL;
		} else {
			GlobalGetAtomName(aItem, command, sizeof(command));
			if ((CommandSearch(zCommands, 11, command, &index)) && (index == kFileName)) {
				if (NULL != (adv = FindAdvise(hwndClient, typeFromIndex[index]))) {
					RemoveAdvise(adv);
					delete adv;
				} else
					ddeAck.fAck = FALSE;
			}
		}
	} else
		ddeAck.fAck = FALSE;
	wStatus = *(WORD *)&ddeAck;
	if (!PostMessage(hwndClient, WM_DDE_ACK, (WPARAM)hwnd, PackDDElParam(WM_DDE_ACK, wStatus, aItem))) {
		if (aItem != (ATOM)NULL)
			GlobalDeleteAtom(aItem);
	}
	return 0;
}


BOOL DDE_UI::PostDataMessage(HWND hwndServer, HWND hwndClient, char *question, char *answer,
	BOOL fDeferUpd, BOOL fAckReq, BOOL fResponse)

{
	ATOM        	aItem;
	DDEACK      	ddeAck;
	DDEDATA FAR 	*lpDDEData;
	GLOBALHANDLE	hDDEData;
	DWORD       	dwTime;
	MSG           	msg;
	WORD        	wStatus;

	aItem = GlobalAddAtom(question);
	if (fDeferUpd)
		hDDEData = NULL;
	else {
		hDDEData = GlobalAlloc(GHND | GMEM_DDESHARE, sizeof(DDEDATA) + strlen(answer));
		if (hDDEData) {
			lpDDEData = (DDEDATA FAR *)GlobalLock(hDDEData);
			lpDDEData->fResponse = fResponse;
			lpDDEData->fRelease = TRUE;
			lpDDEData->fAckReq = fAckReq;
			lpDDEData->cfFormat = CF_TEXT;
			lstrcpy((LPSTR)lpDDEData->Value, answer);
			GlobalUnlock(hDDEData);
		}
	}
	if (!PostMessage(hwndClient, WM_DDE_DATA, (WPARAM)hwndServer, PackDDElParam(WM_DDE_DATA, (UINT)hDDEData, aItem))) {
		if (hDDEData) {
			GlobalFree(hDDEData);
			hDDEData = NULL;
		}
		GlobalDeleteAtom(aItem);
		return FALSE;
	}
	if (fAckReq) {
		ddeAck.fAck = FALSE;
		dwTime = GetCurrentTime();
		while (GetCurrentTime() - dwTime < DDE_TIMEOUT) {
			if (PeekMessage(&msg, hwndServer, WM_DDE_ACK, WM_DDE_ACK, PM_REMOVE)) {
				wStatus = LOWORD(msg.lParam);
				ddeAck = *(DDEACK *)&wStatus;
				aItem = HIWORD(msg.lParam);
				GlobalDeleteAtom(aItem);
				break;
			}
		}
		if (ddeAck.fAck == FALSE) {
			if (hDDEData)
				GlobalFree(hDDEData);
			return FALSE;
		}
	}
	return TRUE;
}


long CALLBACK DDE_UI::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)

{
	DDE_UI      	*ui = NULL;
	HWND        	hwndClient;
	ATOM         	aApp, aTop, aItem;
	DDEACK      	ddeAck;
	GLOBALHANDLE	hDDEPoke, hCommands;
	short       	cfFormat, wStatus;
	char        	question[256];
	char        	command[256];
	static char		appName[] = "freedde";
	static char  	topic[] = "dde_ui";
	long         	result = FALSE;

	switch (msg) {
		case WM_CREATE:
			break;
		case WM_DDE_INITIATE:
			aApp = GlobalAddAtom(appName);
			aTop = GlobalAddAtom(topic);
			if (((LOWORD(lParam) == NULL) || (LOWORD(lParam) == aApp)) &&
				((HIWORD(lParam) == NULL) || (HIWORD(lParam) == aTop))) {
				if (hwnd) {
					SendMessage((HWND)wParam, WM_DDE_ACK, (WPARAM)hwnd, MAKELONG(aApp, aTop));
					return(FALSE);
				}
			}
			GlobalDeleteAtom(aApp);
			GlobalDeleteAtom(aTop);
			return(TRUE);
		case WM_DDE_REQUEST:
			hwndClient = (HWND)wParam;
			cfFormat = LOWORD(lParam);
			aItem = HIWORD(lParam);
			if (cfFormat == CF_TEXT) {
				 GlobalGetAtomName(aItem, command, sizeof(command));
				 strcpy(question, command);
				 ui = (DDE_UI *)lParam;
				 ui->InterpretCommand(command);
				 if (strlen(command) <= 0)
					strcpy(command, "ok");
				 GlobalDeleteAtom(aItem);
				 PostDataMessage(hwnd, hwndClient, question, command, FALSE, FALSE, TRUE);
			} else {
				ddeAck.bAppReturnCode = 0;
				ddeAck.reserved = 0;
				ddeAck.fBusy = FALSE;
				ddeAck.fAck = FALSE;
				wStatus = *(short *)&ddeAck;
				if (!PostMessage(hwndClient, WM_DDE_ACK, (WPARAM)hwnd, MAKELONG(wStatus, aItem)))
					GlobalDeleteAtom(aItem);
			}
			return(FALSE);
		case WM_DDE_ADVISE:
			ui = (DDE_UI *)lParam;
			ui->HandleAdvise(hwnd, wParam, lParam);
			result = TRUE;
			break;
		case WM_DDE_UNADVISE:
			ui = (DDE_UI *)lParam;
			ui->HandleUnadvise(hwnd, wParam, lParam);
			result = TRUE;
			break;
		case WM_DDE_EXECUTE:
			hwndClient = (HWND)wParam;
			hCommands = (GLOBALHANDLE)HIWORD(lParam);
			ddeAck.bAppReturnCode = 0;
			ddeAck.reserved = 0;
			ddeAck.fBusy = FALSE;
			ddeAck.fAck = FALSE;
			wStatus = *(WORD *)&ddeAck;
			if (!PostMessage(hwndClient, WM_DDE_ACK, (WPARAM)hwnd, MAKELONG(wStatus, hCommands)))
				GlobalFree(hCommands);
			return(FALSE);
		case WM_DDE_POKE:
			hwndClient = (HWND)wParam;
			hDDEPoke = (GLOBALHANDLE)LOWORD(lParam);
			aItem = HIWORD(lParam);
			ddeAck.bAppReturnCode = 0;
			ddeAck.reserved = 0;
			ddeAck.fBusy = FALSE;
			ddeAck.fAck = FALSE;
			wStatus = *(WORD *)&ddeAck;
			if (!PostMessage(hwndClient, WM_DDE_ACK, (WPARAM)hwnd, MAKELONG(wStatus, aItem))) {
				GlobalFree(hDDEPoke);
				GlobalDeleteAtom(aItem);
			}
			return(FALSE);
		case WM_DDE_TERMINATE:
			if (hwnd)
				PostMessage((HWND)wParam, WM_DDE_TERMINATE, (WPARAM)hwnd, 0L);
			return(TRUE);
		case WM_DESTROY :
			PostQuitMessage(0) ;
			return 0 ;
        default:
            result = DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return (result);
}


Error DDE_UI::AcceptEvent(Event* event)

{
    Error result = kError_UnknownErr;

    if (event) {
        switch (event->Type()) {
            case INFO_Playing:  
                state = UIState_Playing;
	            break; 
            case INFO_Paused: 
                state = UIState_Paused;
	            break; 
            case INFO_Stopped: 
                state = UIState_Stopped;
	            break; 
			case INFO_MPEGInfo: 
            {
				MpegInfoEvent *info = (MpegInfoEvent *)event;
				char szTemp[256];
				secondsPerFrame = info->GetSecondsPerFrame();
                sprintf(szTemp, "%d kbps",  info->GetBitRate()/1000);
                // SendMessage(m_hwndStatus, SB_SETTEXT, 0, (LPARAM)szTemp);

                sprintf(szTemp, "%.1f kHz",  ((float)info->GetSampleRate())/1000);
			    // SendMessage(m_hwndStatus, SB_SETTEXT, 1, (LPARAM) szTemp);				
				break;
            }

            case INFO_MediaInfo: 
            {
                MediaInfoEvent *info = (MediaInfoEvent*)event; 

				UpdateAdvises(winHwnd, kAdviseFileName, info->m_filename);
	            break; 
            }

            case INFO_MediaTimeInfo: 
            {
                MediaTimeInfoEvent	*info = (MediaTimeInfoEvent*)event;
                char timeString[256] = "00:00:00";
                static int32 lastSeconds = 0, lastMinutes = 0, lastHours = 0;
	            
                if(lastSeconds != info->m_seconds || lastMinutes != info->m_minutes || lastHours != info->m_hours) {
                    lastSeconds = info->m_seconds;
                    lastMinutes = info->m_minutes;
                    lastHours = info->m_hours;

                    sprintf(timeString,"%02d:%02d:%02d",info->m_hours, info->m_minutes, info->m_seconds);

			        // SetWindowText(m_hwndCurrent, timeString);
                }
	            break; 
            }

            case INFO_PlaylistDonePlay:
                break;

	        case CMD_Cleanup: 
	            target->AcceptEvent(new Event(INFO_ReadyToDieUI));
	            break;
	        default:
	            break;
	    }
	    result = kError_NoErr;
    } 
    return result;
}

/**********************************************************************************/

FreeAmpAdvise::FreeAmpAdvise(HWND client, short type, char deferUpd, char ackReq)

{
	next = NULL;
	type = 0;
	hwndClient = client;
	fDeferUpd = deferUpd;
	fAckReq = ackReq;
}



void FreeAmpAdvise::Add(FreeAmpAdvise *theAdvise)

{
	if (theAdvise) {
		theAdvise->SetNext(next);
		next = theAdvise;
	}
}



short FreeAmpAdvise::GetType(void)

{
	return(type);
}


FreeAmpAdvise *FreeAmpAdvise::GetNext(void)

{
    return(next);
}


void FreeAmpAdvise::SetNext(FreeAmpAdvise *newNext)

{
    next = newNext;

}

HWND FreeAmpAdvise::GetClient(BOOL *defer, BOOL *ack)

{
	if (defer)
		*defer = fDeferUpd;
	if (ack)
		*ack = fAckReq;
	return(hwndClient);
}



short FreeAmpAdvise::CheckTypeAndClient(HWND theClient, long theType)

{
	return((theClient == hwndClient) && (theType == type));
}


FreeAmpAdvise::~FreeAmpAdvise(void)

{

}


