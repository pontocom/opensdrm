/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player

	Portions Copyright (C) 1998-1999 EMusic.com

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
	
	$Id: lcdui.cpp,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/

#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include <string.h>

#include <sys/time.h>
#include <termios.h>
#include <signal.h>


#include "config.h"
#include "lcdui.h"

#include "event.h"
#include "playlist.h"
#include "thread.h"
#include "eventdata.h"

#define LCDPROC_4 1

#if LCDPROC_4
extern "C" {
#include "sockets.h"
	   }
#else
extern "C" {
#include "MtxOrb.h"
#include "lcd.h"
#include "drv_base.h"
	   }
#endif
#define stdinfd 0

extern "C" {

UserInterface *Initialize(FAContext *context) {
    return new LcdUI(context);
}

	   }

//static struct termios normalTTY;
//static struct termios rawTTY;
//int getKey() {
//    fd_set rdfs;
//
//    FD_ZERO(&rdfs);
//    FD_SET(stdinfd, &rdfs);
//    if (select (stdinfd+1,&rdfs,NULL,NULL,NULL) == 1) {
//	if (FD_ISSET(stdinfd, &rdfs)) return 1;
//    }
//    return 0;
//}


LcdUI::LcdUI(FAContext *context) {
    m_context = context;
    m_timeType = TIME_CURR;
    m_plm = NULL;
    m_playerEQ = NULL;
    m_lcdLock = new Mutex();

    keyboardListenThread = NULL;

    m_currHours = m_currMinutes = m_currSeconds = m_totalHours = m_totalMinutes = m_totalSeconds = 0;

//    cout << endl << "Command Line Interface" << endl << endl;
//    cout << " * q    Quit" << endl;
//    cout << " * +/=  Next Song" << endl;
//    cout << " * -    Prev Song" << endl;
//    cout << " * p    Pause / UnPause" << endl;
//    cout << " * s    Shuffle" << endl << endl;
}


LcdUI::~LcdUI() {
//    if (m_startupType == PRIMARY_UI) {
//	tcsetattr(stdinfd, TCSANOW, &normalTTY);
//    }
    //cout << "LcdUI: begin deleted..." << endl;
    
    m_lcdLock->Acquire();
#if LCDPROC_4
    sock_close(m_sock);
#else
    lcd.clear();
    lcd.string(1,2,"  Thanks for using  ");
    lcd.string(1,3,"       "BRANDING"      ");
    lcd.flush();
#endif

    Quit = true;
    if (keyboardListenThread) {
	keyboardListenThread->Join();
	delete keyboardListenThread;
	keyboardListenThread = NULL;
    }
    if (m_lcdLock) {
	delete m_lcdLock;
	m_lcdLock = NULL;
    }
}

#define STOCK_CONTRAST 140

Error LcdUI::Init(int32 startupType) {
#if !LCDPROC_4
    lcd_init("/dev/lcd","MtxOrb");
    lcd.contrast(STOCK_CONTRAST);
#endif
    cout << "startup..." << endl;
    m_lcdLock->Acquire();
#if LCDPROC_4
    m_sock = sock_connect("localhost",LCDPORT);
    cout << "connecting" << endl;
    if (m_sock <= 0) {
	//screwwwwed
	return kError_InitFailed;
    }
    cout << "done connecting" << endl;
    sock_send_string(m_sock, "hello\n");
    sock_send_string(m_sock, "client_set -name freeamp\n");
    sock_send_string(m_sock, "screen_add FA\n");
    sock_send_string(m_sock, "screen_set FA -name {"BRANDING"} -heartbeat off -priority 32\n");
    sock_send_string(m_sock, "widget_add FA songname scroller\n");
    sock_send_string(m_sock, "widget_add FA album scroller\n");
    sock_send_string(m_sock, "widget_add FA timeline string\n");
    sock_send_string(m_sock, "widget_add FA artist scroller\n");
    sock_send_string(m_sock, "widget_set FA songname 1 1 20 1 h 2 {Welcome To "the_BRANDING"!}\n");
    sock_send_string(m_sock, "widget_set FA timeline 1 4 {total       00:00:00}\n");
#else 
    lcd.string(1,1," Welcome To "the_BRANDING"!");
    lcd.flush();
#endif
    m_lcdLock->Release();

    m_plm = m_context->plm;
    m_playerEQ = m_context->target;
    m_propManager = m_context->props;
 
    m_argc = m_context->argc;
    m_argv = m_context->argv;

    if ((m_startupType = startupType) == PRIMARY_UI) {
	ProcessArgs();
    }
    Quit = false;
    keyboardListenThread = Thread::CreateThread();
    keyboardListenThread->Create(LcdUI::keyboardServiceFunction,this);
    
    return kError_NoErr;
}

void LcdUI::keyboardServiceFunction(void *pclcio) {
    LcdUI *pMe = (LcdUI *)pclcio;
    int len;
    char buf[8192];
    char *argv[256];
    int newtoken;
    int argc;
    int i;
    while(!pMe->Quit) {
	// Check for server input...
	len = sock_recv(pMe->m_sock, buf, 8000);
	
	// Handle server input...
	while(len > 0) {
	    // Now split the string into tokens...
	    argc = 0; newtoken = 1;
	    for(i=0; i<len; i++) {
		if(buf[i]) {    // For regular letters, keep tokenizing...
		    switch(buf[i])  {
			case ' ':
			    newtoken = 1;
			    buf[i] = 0;
			    break;
			case '\n':
			    buf[i] = 0;
			default:
			    if(newtoken) {
				argv[argc] = buf+i;
				argc++;
			    }
			    newtoken = 0;
			    break;
		    }
		} else {  // If we've got a zero, it's the end of a string...
		    if(argc > 0) {
			if(0 == strcmp(argv[0], "listen")) {
			    //cout << "lcd: Listen " << argv[1] << endl;
			} else if(0 == strcmp(argv[0], "ignore")) {
			    //cout << "lcd: Ignore " << argv[1] << endl;
			} else if(0 == strcmp(argv[0], "key")) {
			    cout << "lcd: Key " << argv[1] << endl;
                            switch (argv[1][0])
                            {
                                case 'E': {
                                    cout << "Toggle Pause" << endl;
                                    Event    *e = new Event(CMD_TogglePause);
                                    pMe->m_playerEQ->AcceptEvent(e);
                                    break; 
                                }
                                case 'F': {
                                    cout << "Next Piece" << endl;
                                    Event   *e = new Event(CMD_NextMediaPiece);
                                    pMe->m_playerEQ->AcceptEvent(e);
                                    break;
                                }
                                case 'G': { 
                                    cout << "Prev Piece" << endl;
                                    Event   *e = new Event(CMD_PrevMediaPiece);
                                    pMe->m_playerEQ->AcceptEvent(e);
                                    break;
                                }
                                case 'H': {
                                    cout << "Shuffle" << endl;
                                    if (pMe->m_plm) {
                                        pMe->m_plm->SetShuffleMode(true);
                                        pMe->m_plm->SetCurrentIndex(0);
                                    }
                                    Event   *e = new Event(CMD_Stop);
                                    pMe->m_playerEQ->AcceptEvent(e);
                                    e = new Event(CMD_Play);
                                    pMe->m_playerEQ->AcceptEvent(e);
                                    break;
                                }
                                default:
                                    break;
                            }
			} else if(0 == strcmp(argv[0], "menu")) {
			} else if(0 == strcmp(argv[0], "connect")) {
			} else {
			    cout << "lcd: ";
			    for(int j=0;j<argc;j++) cout << argv[j] << " ";
			    cout << endl;
			}

		    }
		    argc = 0;  newtoken = 1;
		}
	    }
	    len = sock_recv(pMe->m_sock, buf, 8000);
	    //debug("\n");
	}
#define TIME_UNIT 125000
	usleep(TIME_UNIT);
    }
}

Error LcdUI::AcceptEvent(Event *e) {
    if (e) {
	//cout << "LcdUI: processing event " << e->Type() << endl;
	switch (e->Type()) {
	    case INFO_PlaylistDonePlay: {
		if (m_startupType == PRIMARY_UI) {
		    Event *e = new Event(CMD_QuitPlayer);
		    m_playerEQ->AcceptEvent(e);
		}
		break; }
	    case CMD_Cleanup: {
		Quit = true;
		Event *e = new Event(INFO_ReadyToDieUI);
		m_playerEQ->AcceptEvent(e);
		break; }
	    case INFO_Stopped: {
		m_currHours = m_currMinutes = m_currSeconds = 0;
		m_lcdLock->Acquire();
		BlitTimeLine();
#if !LCDPROC_4
		lcd.flush();
#endif
		m_lcdLock->Release();
		break;
	    }
	    case INFO_UserMessage: {
		if (!strcmp("time_curr_mode",((UserMessageEvent *)e)->GetInfo())) {
		    m_timeType = TIME_CURR;
		    m_lcdLock->Acquire();
		    BlitTimeLine();
#if !LCDPROC_4
		    lcd.flush();
#endif
		    m_lcdLock->Release();
		} else if (!strcmp("time_remain_mode",((UserMessageEvent *)e)->GetInfo())) {
		    m_timeType = TIME_REMAIN;
		    m_lcdLock->Acquire();
		    BlitTimeLine();
#if !LCDPROC_4
		    lcd.flush();
#endif
		    m_lcdLock->Release();

		}else if (!strcmp("time_total_mode",((UserMessageEvent *)e)->GetInfo())) {
		    m_timeType = TIME_TOTAL;
		    m_lcdLock->Acquire();
		    BlitTimeLine();
#if !LCDPROC_4
		    lcd.flush();
#endif
		    m_lcdLock->Release();

		}
		break;
	    }
	    case INFO_MediaInfo: {
		MediaInfoEvent *pmvi = (MediaInfoEvent *)e;
		if (pmvi) {


		MetaData md;
		    char bar[128];
                    const PlaylistItem *pItem;
                    pItem = m_plm->GetCurrentItem();

                    md = pItem->GetMetaData();


		   //cout << "artist" << (char *)md.Artist().c_str() << endl;


		    //cout << "writing lcd" << endl;
		    //cout << "total seconds" << pmvi->m_totalSeconds << endl;
		    m_totalHours = (int32)pmvi->m_totalSeconds / 3600;
		    m_totalMinutes = ((int32)pmvi->m_totalSeconds - m_totalHours*3600) / 60;
		    m_totalSeconds = ((int32)pmvi->m_totalSeconds - m_totalHours*3600 - m_totalMinutes*60);
		    //cout << "total hours " << m_totalHours << " " << m_totalMinutes << " " << m_totalSeconds << endl;
		    m_lcdLock->Acquire();
#if !LCDPROC_4
		    lcd.clear();
#endif
		    char *foo = strrchr(pmvi->m_filename,'/');
		    if (foo) foo++; else foo = pmvi->m_filename;
#if LCDPROC_4

                    if (md.Title().c_str()[0] == '\0')        // check id3 title, if not exists print filename, else title
		    {
		    	sprintf(bar,"widget_set FA songname 1 1 20 3 h 2 {%s}\n",foo);
		    	sock_send_string(m_sock,bar);
		    } else
		    {
		    	sprintf(bar,"widget_set FA songname 1 1 20 3 h 2 {%s}\n",(char *)md.Title().c_str());
		    	sock_send_string(m_sock,bar);
		    }

                    if (md.Album().c_str()[0] == '\0')        // check id3 album, if not exists clear line, else print album
                    {
			sprintf(bar,"widget_set FA album 1 2 20 1 h 2 { }\n");
                        sock_send_string(m_sock,bar);
		    } else
		    {
		    	sprintf(bar,"widget_set FA album 1 2 20 1 h 2 {%s}\n",(char *)md.Album().c_str());
		    	sock_send_string(m_sock,bar);
		    }

                    if (md.Album().c_str()[0] == '\0')        
		    {
                        sprintf(bar,"widget_set FA artist 1 3 20 1 h 3 { }\n");
                        sock_send_string(m_sock,bar);
		    } else
		    {
		    	sprintf(bar,"widget_set FA artist 1 3 20 1 h 3 {%s}\n",(char *)md.Artist().c_str());
		    	sock_send_string(m_sock,bar);
		    }

//	id3 end



#else
		    lcd.string(1,1,foo);
#endif
		    BlitTimeLine();
#if !LCDPROC_4		    
		    lcd.flush();
#endif
		    m_lcdLock->Release();
		    //cout << "wrote lcd" << endl;
		    //cout << "Playing: " << pmvi->m_filename << endl;
		}
		break; }
	    case INFO_MediaTimeInfo: {
		MediaTimeInfoEvent *info = (MediaTimeInfoEvent *)e;
		if ( (m_currHours != info->m_hours) || (m_currMinutes != info->m_minutes) || (m_currSeconds != info->m_seconds)) {
		    m_currHours = info->m_hours;
		    m_currMinutes = info->m_minutes;
		    m_currSeconds = info->m_seconds;
		    m_lcdLock->Acquire();
		    BlitTimeLine();
#if !LCDPROC_4
		    lcd.flush();
#endif
		    m_lcdLock->Release();
		}
		break;
	    }
/*	    case INFO_ID3TagInfo: {
		ID3TagEvent *ite = (ID3TagEvent *)e;
		if (ite) {
		    Id3TagInfo ti = ite->GetId3Tag();
		    if (ti.m_containsInfo) {
			m_lcdLock->Acquire();
#if !LCDPROC_4
			lcd.clear();
#endif
#if LCDPROC_4
		    char bar[64];
		    sprintf(bar,"widget_set FA artist 1 1 20 1 h 2 {%s}\n",ti.m_artist);
		    sock_send_string(m_sock,bar);
		    sprintf(bar,"widget_set FA songname 1 2 20 1 h 2 {%s}\n",ti.m_songName);
		    sock_send_string(m_sock,bar);
#else
			lcd.string(1,1,ti.m_artist);
			lcd.string(1,2,ti.m_songName);
#endif
			BlitTimeLine();
#if !LCDPROC_4
			lcd.flush();
#endif
			m_lcdLock->Release();
			//cout << "Title  : " << ti.m_songName << endl;
			//cout << "Artist : " << ti.m_artist << endl;
			//cout << "Album  : " << ti.m_album << endl;
			//cout << "Year   : " << ti.m_year << endl;
			//cout << "Comment: " << ti.m_comment << endl;
		    }
		}
		break;
	    }
*/	    default:
		break;
	}
    }
    return kError_NoErr;
}

void LcdUI::BlitTimeLine() {
    char foo[32];
    switch (m_timeType) {
	case TIME_CURR:
	    sprintf(foo,"current     %.02d:%.02d:%.02d",m_currHours, m_currMinutes, m_currSeconds);
	    break;
	case TIME_TOTAL:
	    sprintf(foo,"total       %.02d:%.02d:%.02d",m_totalHours,m_totalMinutes,m_totalSeconds);
	    break;
	case TIME_REMAIN: {
	    int32 s,m,h;
	    s = ((m_totalHours * 3600) + (m_totalMinutes * 60) + m_totalSeconds) - ((m_currHours * 3600) + (m_currMinutes * 60) + m_currSeconds);
	    h = s / 3600;
	    m = (s % 3600) / 60;
	    s = (s % 3600) % 60;
	    sprintf(foo,"remain      %.02d:%.02d:%.02d",h,m,s);
	    break; }
	default:
	    sprintf(foo,"Screwed up!!");
    }
#if LCDPROC_4
    char bar[64];
    sprintf(bar,"widget_set FA timeline 1 4 {%s}\n",foo);
    sock_send_string(m_sock,bar);
#else 
    lcd.string(1,4,foo);
#endif
}

void LcdUI::ProcessArgs() {
    char *pc = NULL;
    for(int i=1;i<m_argc;i++) {
	//cout << "Adding arg " << i << ": " << argv[i] << endl;
	pc = m_argv[i];
	if (pc[0] == '-') 
	    processSwitch(&(pc[0]));
    }
    m_plm->SetCurrentIndex(0);
    Event *e = new Event(CMD_Play);
    m_playerEQ->AcceptEvent(e);
}

void LcdUI::processSwitch(char *pc) {
    return;
}





