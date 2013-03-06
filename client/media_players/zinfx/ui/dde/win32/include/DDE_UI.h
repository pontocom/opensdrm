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
	
	$Id: DDE_UI.h,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/

// DDE_CIO.h


#ifndef INCLUDED_DDE_CIO_H_
#define INCLUDED_DDE_CIO_H_

#include "ui.h"
#include "event.h"
#include "thread.h"

#define	UIState_Stopped 0
#define	UIState_Playing 1
#define	UIState_Paused  2

class FreeAmpAdvise {
	protected:
		HWND         	hwndClient;
		char        	fDeferUpd;
		char         	fAckReq;
		long         	userCount;
		short       	type;  /* By now only 1 type is supported that returns the playing file name */
		char        	*command;
		FreeAmpAdvise	*next;
	public:
		             	FreeAmpAdvise(HWND client, short type, char deferUpd, char ackReq);
		void         	Add(FreeAmpAdvise *theAdvise);
		void        	UpdateAdvises(HWND hwnd, short updateType, char *dataStr);
		short          	CheckTypeAndClient(HWND theClient, long index);
		FreeAmpAdvise	*GetNext(void);
		void        	SetNext(FreeAmpAdvise *newNext);
		HWND         	GetClient(BOOL *defer, BOOL *ack);
		short       	GetType(void);
		             	~FreeAmpAdvise(void);
};

class DDE_UI : public UserInterface {
	private:
		FreeAmpAdvise	*adviseList;
		PlaylistManager	*plm;
		int32		    totalSeconds;
		int32        	currSeconds;
		int32       	totalFrames;
		float		    secondsPerFrame;
		EventQueue     	*target;

	private:
		Thread         	*uiThread;
		HWND          	winHwnd;
	    FAContext   	*context;
		int32		    state;

		int         	CommandSearch(const char *list[], int nCommands, char *name, int *key);
		void        	InterpretCommand(char *command);
		long        	CollectToSeparator(char **scanner, char separator, char *str);
		BOOL        	UpdateAdvises(HWND hwnd, short updateType, char *dataStr);
		void        	RemoveAdvise(FreeAmpAdvise *theAdvise);
		FreeAmpAdvise	*FindAdvise(HWND theClient, long index);
		BOOL         	HandleAdvise(HWND hwnd, WORD wParam, DWORD lParam);
		BOOL        	HandleUnadvise(HWND hwnd, WORD wParam, DWORD lParam);
		static BOOL   	PostDataMessage(HWND hwndServer, HWND hwndClient, char *question, char *answer, BOOL fDeferUpd, BOOL fAckReq, BOOL fResponse);

	public:
		            	DDE_UI(FAContext *context);

		void          	ParseArgs(int32 argc, char** argv);
	    virtual Error	Init(int32 startup_type);
	    virtual Error	AcceptEvent(Event*);
		void          	CreateUI();
		
		static long CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		            	~DDE_UI();

		static void  	UIThreadFunc(void *);
};


#endif // INCLUDED_DDE_CIO_H_
