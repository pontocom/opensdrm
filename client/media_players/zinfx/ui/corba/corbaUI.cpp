/*____________________________________________________________________________
        
        FreeAmp - The Free MP3 Player

        Portions Copyright (C) 1998-1999 EMusic.com
        Portions Copyright (C) 1999      Henrik Johnson 
        Portions Copyright (C) 2000      Martin Enlund

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
        
        $Id: corbaUI.cpp,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/

#include <iostream.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/time.h>
#include <termios.h>
#include <signal.h>

#include "config.h"
#include "corbaUI.h"
#include "event.h"
#include "playlist.h"
#include "thread.h"
#include "eventdata.h"

#include "corba.h"

#define stdinfd 0
// #define CORBADEBUG 
extern "C"
{

	UserInterface *Initialize(FAContext * context)
	{
		return new corbaUI(context);
	}

	int freeamp_toggle_pause(void *pclcio) {
#ifdef CORBADEBUG
		printf("TogglePause i C++\n");
#endif
		Event *e = new Event(CMD_TogglePause);
		corbaUI *pMe = (corbaUI *) pclcio;
		pMe->m_playerEQ->AcceptEvent(e);
		//  		delete e;
#ifdef CORBADEBUG
		printf("TogglePause i C++ avklarat\n");
#endif
		return 1;
	}
	
	int freeamp_next(void *pclcio) {
#ifdef CORBADEBUG
		printf("-> freeamp_next\n");
#endif
		Event *e = new Event(CMD_NextMediaPiece);
		corbaUI *pMe = (corbaUI *) pclcio;
		pMe->m_playerEQ->AcceptEvent(e);
		//  		delete e;
#ifdef CORBADEBUG
		printf("<- freeamp_next\n");
#endif
		return 1;
	}

	int freeamp_previous(void *pclcio) {
#ifdef CORBADEBUG
		printf("-> freeamp_previous\n");
#endif
		Event *e = new Event(CMD_PrevMediaPiece);
		corbaUI *pMe = (corbaUI *) pclcio;
		pMe->m_playerEQ->AcceptEvent(e);
		//  		delete e;
#ifdef CORBADEBUG
		printf("<- freeamp_previous\n");
#endif
		return 1;
	}

	int freeamp_quit(void *pclcio) {
#ifdef CORBADEBUG
		printf("-> freeamp_quit\n");
#endif
		Event *e = new Event(CMD_QuitPlayer);
		corbaUI *pMe = (corbaUI *) pclcio;
		pMe->m_playerEQ->AcceptEvent(e);
		//  		delete e;
#ifdef CORBADEBUG
		printf("<- freeamp_quit\n");
#endif
		return 1;
	}

	int freeamp_play(void *pclcio) {
#ifdef CORBADEBUG
		printf("-> freeamp_play\n");
#endif
		corbaUI *pMe = (corbaUI *) pclcio;
		pMe->m_playerEQ->AcceptEvent( new Event(CMD_Play) );
		//  		delete e;
#ifdef CORBADEBUG
		printf("<- freeamp_play\n");
#endif
		return 1;
	}

	int freeamp_stop(void *pclcio) {
#ifdef CORBADEBUG
		printf("-> freeamp_stop\n");
#endif
		corbaUI *pMe = (corbaUI *) pclcio;
		pMe->m_playerEQ->AcceptEvent( new Event(CMD_Stop) );
		//  		delete e;
#ifdef CORBADEBUG
		printf("<- freeamp_stop\n");
#endif
		return 1;
	}

	int freeamp_addsong(void *pclcio, char *url) {
#ifdef CORBADEBUG
		printf("-> freeamp_addsong\n");
#endif
		//  		Event *e = new Event(CMD_QuitPlayer);
		corbaUI *pMe = (corbaUI *) pclcio;
		pMe->m_plm->AddItem(url);
#ifdef CORBADEBUG
		printf("<- freeamp_addsong\n");
#endif
		return 1;
	}

	int freeamp_getplaylist(void *pclcio, char ***lst) {
#ifdef CORBADEBUG
		printf("-> freeamp_getplaylist\n");
#endif
		int numEntries = 0;
		PlaylistItem *entry;
		char **resultlist;
		
		corbaUI *pMe = (corbaUI *) pclcio;
		numEntries = pMe->m_plm->CountItems();
		resultlist = new char*[numEntries];
#ifdef CORBADEBUG
		printf("numEntries: %d\n", numEntries);
#endif

		for(int i=0; i < numEntries; i++) {
			entry = pMe->m_plm->ItemAt(i);
			resultlist[i]=strdup( entry->URL().c_str() );
#ifdef CORBADEBUG
			printf("%i -> %s\n", numEntries, resultlist[i]);
#endif
		}
		//  		delete e;
#ifdef CORBADEBUG
		printf("<- freeamp_getplaylist\n");
#endif
		*lst = resultlist;
		return numEntries;
	}

	int freeamp_playnumber(void *pclcio, char *number) {
#ifdef CORBADEBUG
		printf("-> freeamp_playnumber\n");
#endif
		corbaUI *pMe = (corbaUI *) pclcio;
		int numEntries = pMe->m_plm->CountItems();
		int index = atol(number);
		
		if (pMe->m_plm) {
			//  				pMe->m_plm->SetShuffleMode(true);
			
			if (index < numEntries && index >= 0)
				pMe->m_plm->SetCurrentIndex(index);
#ifdef CORBADEBUG
			else
				printf("playnumber out of bounds %d (max %d)\n", index, numEntries);
#endif
		}
		
		pMe->m_playerEQ->AcceptEvent( new Event(CMD_Stop)  );
		pMe->m_playerEQ->AcceptEvent( new Event(CMD_Play) );
#ifdef CORBADEBUG
		printf("<- freeamp_playnumber\n");
#endif
		return 1;
	}

	unsigned long freeamp_getnumber(void *pclcio) {
#ifdef CORBADEBUG
		printf("-> freeamp_getnumber\n");
#endif
		corbaUI *pMe = (corbaUI *) pclcio;
		unsigned long index = pMe->m_plm->GetCurrentIndex();
#ifdef CORBADEBUG
		printf("<- freeamp_getnumber\n");
#endif
		return index;
	}

	
	int freeamp_deletenumber(void *pclcio, unsigned long number) {
#ifdef CORBADEBUG
		printf("-> freeamp_deletenumber\n");
#endif
		corbaUI *pMe = (corbaUI *) pclcio;
		pMe->m_plm->RemoveItem(number);
#ifdef CORBADEBUG
		printf("<- freeamp_deletenumber\n");
#endif
		return 1;
	}

}


corbaUI::corbaUI(FAContext * context)
{
	m_context = context;

	 
	m_plm = NULL;
	m_playerEQ = NULL;
	m_lastIndexPlayed = -1;
	m_id3InfoPrinted = false;
	m_currSeconds = 0;
	m_totalFrames = 1;
	m_secondsPerFrame = 1;

	keyboardListenThread = NULL;
}


Error     corbaUI::
Init(int32 startup_level)
{
	if ((m_startupLevel = startup_level) == PRIMARY_UI)
		{

      m_propManager = m_context->props;
      m_plm = m_context->plm;
      m_playerEQ = m_context->target;

			keyboardListenThread = Thread::CreateThread();
			keyboardListenThread->Create(corba_init, this);

			m_argc = m_context->argc;
			m_argv = m_context->argv;
			ProcessArgs();
		}
	return kError_NoErr;
}


corbaUI::~corbaUI()
{
	if (m_startupLevel == PRIMARY_UI)
		{
		}
	cout << "corbaUI: begin deleted..." << endl;
	if (keyboardListenThread)
		{
			keyboardListenThread->Destroy();
			delete    keyboardListenThread;
			 
			keyboardListenThread = NULL;
		}
}



#define STEP_SIZE 10


void corbaUI::keyboardServiceFunction(void *pclcio)
{
	corbaUI *pMe = (corbaUI *) pclcio;
	
	for (;;) {
		sleep(1);
	}
	
	char      chr;
	// size_t rtn;
	// int fn = STDIN_FILENO;
	for (;;)
		{
      switch (chr)
				{
				case 'p':
				case 'P':
					{
            Event    *e = new Event(CMD_TogglePause);

            pMe->m_playerEQ->AcceptEvent(e);
            break;
					}
				case '-':
					{
            Event    *e = new Event(CMD_PrevMediaPiece);

            pMe->m_playerEQ->AcceptEvent(e);
            break;
					}
				case 'f':
					{
            int32     cf = int32((pMe->m_currSeconds + STEP_SIZE) / pMe->m_secondsPerFrame);

            if (cf > pMe->m_totalFrames)
							cf = pMe->m_totalFrames;
            Event    *e = new ChangePositionEvent(cf);

            pMe->m_currSeconds += STEP_SIZE;
            pMe->m_playerEQ->AcceptEvent(e);
            break;
					}
				case 'b':
					{
            int32     nsec = pMe->m_currSeconds - STEP_SIZE;

            if (nsec < 0)
							nsec = 0;
            if (pMe->m_currSeconds > 0)
							{
								Event    *e = new ChangePositionEvent(nsec / pMe->m_secondsPerFrame);

								pMe->m_playerEQ->AcceptEvent(e);
							}
            break;
					}
				case '=':
				case '+':
				case 'n':
				case 'N':
					{
            Event    *e = new Event(CMD_NextMediaPiece);

            pMe->m_playerEQ->AcceptEvent(e);
            break;
					}
				case 'q':
				case 'Q':
					{
            Event    *e = new Event(CMD_QuitPlayer);

            pMe->m_playerEQ->AcceptEvent(e);
            break;
					}
				case 's':
				case 'S':
					{
            if (pMe->m_plm)
							{
								pMe->m_plm->SetShuffleMode(true);
								pMe->m_plm->SetCurrentIndex(0);
							}
            Event    *e = new Event(CMD_Stop);

            pMe->m_playerEQ->AcceptEvent(e);
            e = new Event(CMD_Play);
            pMe->m_playerEQ->AcceptEvent(e);
            break;
					}
					//          case 'f':{
					// Event *e = new Event(CMD_ChangePosition,(void *)200);
					// pMe->m_playerEQ->AcceptEvent(pMe->m_playerEQ,e);
					// }
				default:
					break;
				}
		}
	//	return kError_NoErr;
}

Error     corbaUI::
AcceptEvent(Event * e)
{
	if (e)
		{
      // cout << "corbaUI: processing event " << e->getEvent() << endl;
      switch (e->Type())
				{
				case INFO_ErrorMessage:
					{
						ErrorMessageEvent *eme = (ErrorMessageEvent *)e;
						string ErrMessage(eme->GetErrorMessage());

						cout << ErrMessage << endl;
						break;
					}
				case INFO_PlaylistDonePlay:
					{
						//              Event    *e = new Event(CMD_QuitPlayer);

						//              m_playerEQ->AcceptEvent(e);
						//              break;
					}
				case INFO_Stopped:
					if (m_currSeconds == 0)
            break;
				case CMD_Cleanup:
					{
            Event    *e = new Event(INFO_ReadyToDieUI);

            m_playerEQ->AcceptEvent(e);
            break;
					}
				case INFO_MediaInfo:
					{
            MediaInfoEvent *pmvi = (MediaInfoEvent *) e;

            if (pmvi && m_lastIndexPlayed != pmvi->m_indexOfSong)
							{
								m_lastIndexPlayed = pmvi->m_indexOfSong;
								m_id3InfoPrinted = false;
								cout << "Playing: " << pmvi->m_filename << endl;
							}
            break;
					}
				case INFO_MediaTimeInfo:
					{
            MediaTimeInfoEvent *info = (MediaTimeInfoEvent *) e;

            m_currSeconds = (info->m_hours * 3600) + (info->m_minutes * 60) + info->m_seconds;
            break;
					}
				case INFO_MPEGInfo:
					{
            MpegInfoEvent *info = (MpegInfoEvent *) e;

            m_secondsPerFrame = info->GetSecondsPerFrame();
            m_totalFrames = info->GetTotalFrames();
            break;
					}
					/*          case INFO_ID3TagInfo: {
											ID3TagEvent *ite = (ID3TagEvent *)e;
											if (ite) {
											Id3TagInfo ti = ite->GetId3Tag();
											if (ti.m_containsInfo && !m_id3InfoPrinted) {
											m_id3InfoPrinted = true;
											cout << "Title  : " << ti.m_songName << endl;
											cout << "Artist : " << ti.m_artist << endl;
											cout << "Album  : " << ti.m_album << endl;
											cout << "Year   : " << ti.m_year << endl;
											cout << "Comment: " << ti.m_comment << endl;
											}
											}
											break;
											}
					*/ default:
						break;
				}
		}
	return kError_NoErr;
}

void      corbaUI::
ProcessArgs()
{
	char     *pc = NULL;

	for (int i = 1; i < m_argc; i++)
		{
      // cout << "Adding arg " << i << ": " << argv[i] << endl;
      pc = m_argv[i];
      if (pc[0] == '-')
				processSwitch(&(pc[0]));
		}
	m_plm->SetCurrentIndex(0);
	Event    *e = new Event(CMD_Play);

	m_playerEQ->AcceptEvent(e);
}


void corbaUI::
processSwitch(char *pc)
{
	return;
}

