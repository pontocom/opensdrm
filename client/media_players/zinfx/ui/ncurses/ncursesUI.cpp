/*____________________________________________________________________________
        
        FreeAmp - The Free MP3 Player

        Copyright (C) 1999 Stephan Auerhahn

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
        
----------------------------------------------------------------------------
ncurses user interface by stephan auerhahn (palpatine@midi.net)

this is a hacked up version of cmdlineUI.cpp
____________________________________________________________________________*/

#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include <sys/time.h>
#include <termios.h>
#include <signal.h>

#include "config.h"
#include "ncursesUI.h"
#include "event.h"
#include "playlist.h"
#include "thread.h"
#include "eventdata.h"

#define stdinfd 0

extern "C" {

UserInterface *Initialize(FAContext *context) {
    return new ncursesUI(context);
}

           }
	   
static struct termios normalTTY;
static struct termios rawTTY;

int getKey() {
    fd_set rdfs;

    FD_ZERO(&rdfs);
    FD_SET(stdinfd, &rdfs);
    if (select (stdinfd+1,&rdfs,NULL,NULL,NULL) == 1) {
        if (FD_ISSET(stdinfd, &rdfs)) return 1;
    }
    return 0;
}


ncursesUI::ncursesUI(FAContext *context) {
    m_context = context;
    m_plm = NULL;
    m_playerEQ = NULL;
    m_lastIndexPlayed = -1;
    m_id3InfoPrinted = false;
    helpwin = NULL;
    helpwin_open = false;
    playlistwin = NULL;
    playlistwin_open = false;
/* This stuff seems to mess up some events - palp
    m_mediaInfo_set = false;
    m_mediaInfo = NULL;
    m_mpegInfo_set = false;
    totalFrames = 0; */
    totalTime = 0;
    keyboardListenThread = NULL;
}

Error ncursesUI::Init(int32 startup_level) {
    cursesStarted = false;
    if ((m_startupLevel = startup_level) == PRIMARY_UI) {
        tcgetattr(stdinfd, &::normalTTY);
        ::rawTTY = ::normalTTY;
        ::rawTTY.c_lflag &= ~ICANON;
        ::rawTTY.c_lflag &= ~ECHO;
        tcsetattr(stdinfd, TCSANOW, &rawTTY);

        keyboardListenThread = Thread::CreateThread();
        keyboardListenThread->Create(ncursesUI::keyboardServiceFunction,this);

        /* curses initialization */
        initscr();
        cbreak();
        noecho();
        nonl();
        intrflush( stdscr, false );
        keypad( stdscr, true );
        start_color();
        cursesStarted = true;
        init_pair(1, COLOR_BLUE, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, COLOR_CYAN, COLOR_BLACK);
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(6, COLOR_YELLOW, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_BLACK);
        init_pair(8, COLOR_BLACK, COLOR_BLACK);
        curs_set(0);

        showInfo();

        m_plm = m_context->plm;
        m_propManager = m_context->props;
        m_playerEQ = m_context->target;

        m_argc = m_context->argc;
        m_argv = m_context->argv;

        ProcessArgs();
    }
    return kError_NoErr;
}

ncursesUI::~ncursesUI() {
    if (cursesStarted) {
        curs_set(1);
        move(0,0);
        clear();
        refresh();
        endwin();
    }
    if (m_startupLevel == PRIMARY_UI) {
        tcsetattr(stdinfd, TCSANOW, &normalTTY);
    }
    //cout << "ncursesUI: begin deleted..." << endl;
    if (keyboardListenThread) {
        keyboardListenThread->Destroy();
        delete keyboardListenThread;
        keyboardListenThread = NULL;
    }
//    delete m_mediaInfo;
}

void ncursesUI::keyboardServiceFunction(void *pclcio) {
    ncursesUI *pMe = (ncursesUI *)pclcio;
    //char *pkey = new char[1];
    char chr;
    //size_t rtn;
    //int fn = STDIN_FILENO;
    for (;;) {
        ::getKey();
        read(stdinfd,&chr,1);
        switch (chr) {
            case 'p':
            case 'P': {
                Event *e = new Event(CMD_TogglePause);
                pMe->m_playerEQ->AcceptEvent(e);
                break;
            }
            case '-': {
                Event *e = new Event(CMD_PrevMediaPiece);
                pMe->m_playerEQ->AcceptEvent(e);
                break;
            }
            case '=':
            case '+':
            case 'n':
            case 'N': {
                Event *e = new Event(CMD_NextMediaPiece);
                pMe->m_playerEQ->AcceptEvent(e);
                break; }
            case 'q':
            case 'Q': {
                Event *e = new Event(CMD_QuitPlayer);
                pMe->m_playerEQ->AcceptEvent(e);
                clear();
                move(0,0);
                endwin();
                break; }
            case 's':
            case 'S': {
                if (pMe->m_plm) {
                    pMe->m_plm->SetShuffleMode(true);
                    pMe->m_plm->SetCurrentIndex(0);
                }
                Event *e = new Event(CMD_Stop);
                pMe->m_playerEQ->AcceptEvent(e);
                e = new Event(CMD_Play);
                pMe->m_playerEQ->AcceptEvent(e);
                break;}
            case 'h':
            case 'H': {
                pMe->help();
                break;}
//            case '/': {
//                pMe->playlist();
//                break;}
//          case 'f':{
//              Event *e = new Event(CMD_ChangePosition,(void *)200);
//              pMe->m_playerEQ->AcceptEvent(pMe->m_playerEQ,e);
//          }
            default:
                break;
        }
    }

}

Error ncursesUI::AcceptEvent(Event *e) {
    if (e) {
        switch (e->Type()) {
            case INFO_PlaylistDonePlay: {
                Event *e = new Event(CMD_QuitPlayer);
                m_playerEQ->AcceptEvent(e);
                break; }
            case CMD_Cleanup: {
                if (m_startupLevel == PRIMARY_UI) 
                   tcsetattr(stdinfd, TCSANOW, &normalTTY);
                Event *e = new Event(INFO_ReadyToDieUI);
                m_playerEQ->AcceptEvent(e);
                break; }
            case INFO_MPEGInfo: {
                MpegInfoEvent *mie = (MpegInfoEvent *)e;
                if (mie) {
                    char buf[1024];
                    sprintf(buf, "%d kbps", mie->GetBitRate() / 1000);
                    move(LINES - 6, COLS - strlen(buf));
                    addstr(buf);
                    sprintf(buf, "%d kHz %s", mie->GetSampleRate() / 1000,
                                              mie->GetStereo() < 4 ? "stereo" : "mono");
                    move(LINES - 5, COLS - strlen(buf));
                    addstr(buf);
                    sprintf(buf, "MPEG-%s layer %d",
                                 mie->GetMpegVersion() == 1 ? "1" :
                                 mie->GetMpegVersion() == 2 ? "2" : "2.5",
                                 mie->GetLayer());
                    move(LINES - 4, COLS - strlen(buf));
                    addstr(buf);
                    refresh();
                }
                break;
            }
            case INFO_MediaInfo: {
                MediaInfoEvent *pmvi = (MediaInfoEvent *)e;
                if (pmvi)
                {
//                    m_mediaInfo = new MediaInfoEvent(*pmvi);
                    totalTime = pmvi->m_totalSeconds;
//                    m_mediaInfo_set = true;
                }
                if (pmvi && m_lastIndexPlayed != pmvi->m_indexOfSong) {
                    const PlaylistItem *pItem;
                    MetaData md;
                    char buf[1024];                    
                    m_lastIndexPlayed = pmvi->m_indexOfSong;
//                    m_id3InfoPrinted = false;
                    pItem = m_plm->GetCurrentItem();
                    if (!pItem)
                       break;
                    md = pItem->GetMetaData();

                    clear();
                    move(0,0);
                    showInfo();
                    move(2, 0);
                    addstr("Title  : ");
                    if (md.Title().c_str()[0] != '\0')
		        addstr((char *)md.Title().c_str());
		    else
                        addstr(pmvi->m_filename);
                    addstr("\nArtist : ");
                    addstr((char *)md.Artist().c_str());
                    addstr("\nAlbum  : ");
                    addstr((char *)md.Album().c_str());
                    addstr("\nYear   : ");
                    if (md.Year() != 0)
		    {
			sprintf(buf, "%d", md.Year());
		    	addstr(buf);
                    }
                    addstr("\nGenre  : ");
                    addstr((char *)md.Genre().c_str());
                    addstr("\nTrack  : ");
                    if (md.Track() != 0)
                    {
                        sprintf(buf, "%d", md.Track());
                        addstr(buf);
                    }
                    addstr("\nComment: ");
                    addstr((char *)md.Comment().c_str());
                    addstr("\n");
                    refresh();

                    counter = 0;
                    title = (char *)malloc((2*sizeof(char)) * (12 + strlen(pmvi->m_filename)));
                    sprintf( title, BRANDING " " ZINF_VERSION " - [%s]", pmvi->m_filename);
                    if ( strlen(title) > (unsigned)COLS - 13 )
                    {
                        titleStart = 0;
                        titleDir = -1;
                        move( 0, 6 );
                        //color_set(7, NULL);
                        attron(A_REVERSE);
                        for (int i = 6; i < COLS - 7; i++)
                            addch(title[i-6]);
                        attroff(A_REVERSE);
                    }
                    else
                    {
                        move(0, (COLS / 2) - ( strlen(title) ) / 2 );
                        //color_set(7, NULL);
                        attron(A_REVERSE);
                        addstr(title);
                        attroff(A_REVERSE);
                    }
                }
                refresh();
                break; }
            case INFO_StreamInfo:
            {
                char szTitle[100];
                int  i;
            
                StreamInfoEvent *pInfo = (StreamInfoEvent *)e;
           
                pInfo->GetTitle(szTitle, 100);
                move(2, 0);
                addstr("Title  : ");
                addstr(szTitle);
                for(i = 9 + strlen(szTitle); i < COLS; i++)
                    addch(' ');
                      
                break;
            } 
            case INFO_MediaTimeInfo: {
                MediaTimeInfoEvent *pmtp = ( MediaTimeInfoEvent *)e;
                float percentAmount;
                int i=0, hours, minutes, seconds;
                char buf[1024];

                if (title && ( strlen(title) > (unsigned)COLS - 13 ) )
                   counter++;
                
                if ( counter > 50
                   || (counter > 4 && (titleDir == 0 || titleDir == 1)))
                {
                    int i;
                    counter = 0;
                    if (titleDir == -1)
                       titleDir = 0;
                    else if (titleDir == 2)
                       titleDir = 1;
                    if (titleDir == 0)
                        titleStart++;
                    else
                        titleStart--;
                    move( 0, 6 );
                    //color_set(7, NULL);
                    attron(A_REVERSE);
                    for (i = 6; i < COLS - 7; i++)
                        addch(title[i-6+titleStart]);
                    attroff(A_REVERSE);
                    if ( titleStart <= 0 )
                       titleDir = -1;
                    else if ( i - 6 + (unsigned)titleStart >= strlen(title))
                       titleDir = 2;
                }

                
                percentAmount = ( totalTime / ( COLS + 1));
                move(LINES-2, 0);
                sprintf(buf, "%02d:%02d:%02d", pmtp->m_hours, pmtp->m_minutes,
                                               pmtp->m_seconds);
                addstr(buf);
                move(LINES-2, COLS-8);
                hours = (int)totalTime / 60 / 60;
                minutes = (int)totalTime / 60;
                seconds = (int)totalTime;
                seconds -= minutes * 60;
                minutes -= hours * 60;                

                sprintf(buf, "%02d:%02d:%02d", hours, minutes,
                                               seconds);
                addstr(buf);                
                move(LINES-1, 0);
                for (i=0; i < pmtp->m_totalSeconds / percentAmount; i++)
                    addch(ACS_BLOCK);
                for (; i < COLS; i++)
                    addch(ACS_BOARD);
                refresh();
                lastSeconds = pmtp->m_totalSeconds;
                break;}
              default:
                break;
        }
    }
    return kError_NoErr;
}

void ncursesUI::ProcessArgs() {
    //char *pc = NULL;
    //for(int i=1;i<m_argc;i++) {
        //cout << "Adding arg " << i << ": " << argv[i] << endl;
        //pc = m_argv[i];
        //if (pc[0] == '-') 
        //    processSwitch(pc);
    //}
    m_plm->SetCurrentIndex(0);
}

void ncursesUI::processSwitch(char *pc) {

    cursesStarted = false;
    curs_set(1);
    move(0,0);
    clear();
    refresh();
    endwin();
    if (pc[1] == 'h' || pc[1] == 'H')
    {
        cout << BRANDING << " ncurses user interface" << endl;
        cout << "Syntax: freeamp [-h] <filenames>" << endl;
        cout << "-h shows this help" << endl;
        cout << "If you are running freeamp with the -ui argument to select" << endl;
        cout << "the ncurses ui, you can edit ~/.freeamp_prefs and change the" << endl;
        cout << "value of TextUI." << endl << endl;
        cout << "ncurses user interface Copyright (C) 1999 Stephan Auerhahn" << endl;
    }
    else
    {
        cout << "Invalid command line argument, try -h for help." << endl;
    }
    Event *e = new Event(CMD_QuitPlayer);
    m_playerEQ->AcceptEvent(e);
    return;
}

void ncursesUI::showInfo() {
    move(0,0);
    //color_set(7, NULL);
    attron(A_REVERSE);
    for (int i=0; i < COLS; i++)
        addstr(" ");
    move(0, (COLS/2) - 4);
    addstr(BRANDING " " ZINF_VERSION);
    attroff(A_REVERSE);
//    refresh();
    //color_set(7, NULL);
    move(LINES - 4 ,0);
    addstr(" press h for keystroke help ");
/*
    addstr("       Keystroke Help     \n");
    addstr("  ( q )    Quit           \n");
    addstr("  ( + )    Next Song      \n");
    addstr("  ( - )    Prev Song      \n");
    addstr("  ( p )    Pause / UnPause\n");
    addstr("  ( s )    Shuffle        \n");
*/
    move(LINES - 2, 0);
    addstr("00:00:00");
    move(LINES - 2, COLS - 8);
    addstr("00:00:00");
    move(LINES - 1, 0);
    for (int i = 0; i < COLS; i++)
    {
         addch(ACS_BOARD);
    }
    
//    refresh();
    return;
}

void ncursesUI::help() {
     if (helpwin_open)
     {
        delwin(helpwin);
        helpwin_open = false;
        touchwin(stdscr);
        refresh();
        if (playlistwin_open)
        {
            touchwin(playlistwin);
            wrefresh(playlistwin);
        }
     }
     else
     {
        helpwin = newwin( 10, 30, (LINES / 2) - 5, (COLS / 2) - 15 );
        wmove(helpwin, 0, 0);
        waddch(helpwin, ACS_ULCORNER);
        for (int i = 0; i < 2; i++)
            waddch(helpwin, ACS_HLINE);
        waddch(helpwin, ACS_RTEE);
        waddstr(helpwin, "Help( h to close )");
        waddch(helpwin, ACS_LTEE);
        for (int i = 0; i < 2; i++)
            waddch(helpwin, ACS_HLINE);
        waddch(helpwin, ACS_URCORNER);
        waddch(helpwin, 10);
        waddch(helpwin, ACS_VLINE);
        waddstr(helpwin, " ( q )    Quit          ");
        waddch(helpwin, ACS_VLINE);
        waddch(helpwin, 10);
        waddch(helpwin, ACS_VLINE);
        waddstr(helpwin, " ( + )    Next Song     ");
        waddch(helpwin, ACS_VLINE);
        waddch(helpwin, 10);
        waddch(helpwin, ACS_VLINE);
        waddstr(helpwin, " ( - )    Prev Song     ");
        waddch(helpwin, ACS_VLINE);
        waddch(helpwin, 10);
        waddch(helpwin, ACS_VLINE);
        waddstr(helpwin, " ( p )    Pause/UnPause ");
        waddch(helpwin, ACS_VLINE);
        waddch(helpwin, 10);
        waddch(helpwin, ACS_VLINE);
        waddstr(helpwin, " ( s )    Shuffle       ");
        waddch(helpwin, ACS_VLINE);
        waddch(helpwin, 10);
        waddch(helpwin, ACS_LLCORNER);
        for (int i = 0; i < 24; i++)
            waddch(helpwin, ACS_HLINE);
        waddch(helpwin, ACS_LRCORNER);
        wrefresh(helpwin);
        helpwin_open = true;
     }
}

void ncursesUI::playlist() {
/*
     if (playlistwin_open)
     {
        delwin(playlistwin);
        playlistwin_open = false;
        touchwin(stdscr);
        refresh();
        if (helpwin_open)
        {
            touchwin(helpwin);
            wrefresh(helpwin);
        }
     }
     else
     {
        playlistwin = newwin( LINES - 10, 70, 4, (COLS / 2) - 35);
        wmove(playlistwin, 0, 0);
        waddch(playlistwin, ACS_ULCORNER);
        for (int i = 0; i < 68; i++)
            waddch(playlistwin, ACS_HLINE);
        waddch(playlistwin, ACS_URCORNER);
        for (int i = 1; i < LINES - 10; i++)
        {
            mvwaddch(playlistwin, i, 0, ACS_VLINE);
            mvwaddch(playlistwin, i, 69, ACS_VLINE);
        }
        wmove(playlistwin, LINES - 11, 0);
        waddch(playlistwin, ACS_LLCORNER);
        for (int i = 0; i < 68; i++)
            waddch(playlistwin, ACS_HLINE);
        waddch(playlistwin, ACS_LRCORNER);
        for (int i = 0; i < m_plm->CountItems() && i < LINES - 12; i++)
        {
             wmove(playlistwin, i + 1, 3);
             waddstr(playlistwin, m_plm->ItemAt(i)->StringForPlayerToDisplay());
        }
        wrefresh(playlistwin);
        playlistwin_open = true;
     }
*/
}


