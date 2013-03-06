/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player

	Portions Copyright (C) 1999 EMusic.com

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
	
	$Id: downloadui.h,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_DOWNLOAD_UI_H_
#define INCLUDED_DOWNLOAD_UI_H_

/* system headers */
#include <stdlib.h>

/* project headers */
#include "config.h"
#include "ui.h"
#include "semaphore.h"
#include "event.h"
#include "thread.h"
#include "mutex.h"
#include "queue.h"
#include "facontext.h"
#include "preferences.h"
#include "downloadmanager.h"


class DownloadUI : public UserInterface {
 public:
    DownloadUI(FAContext *context);
    ~DownloadUI();

    virtual Error Init(int32 startup_type);
    virtual Error AcceptEvent(Event*);
    void CreateUI();

    static BOOL CALLBACK MainProc(	HWND hwnd, 
						            UINT msg, 
						            WPARAM wParam, 
						            LPARAM lParam );

    void SetWindowHandle(HWND hwnd){m_hwnd = hwnd;}

    BOOL InitDialog();
    BOOL Command(int32 command, HWND src);
    BOOL Notify(int32 controlId, NMHDR* nmh);
    BOOL DrawItem(int32 controlId, DRAWITEMSTRUCT* dis);
    BOOL MeasureItem(HWND hwnd, MEASUREITEMSTRUCT* mis);
    BOOL Destroy();
    void ShowHelp(void);

    LRESULT FreeTracksWndProc(HWND hwnd, 
                              UINT msg, 
                              WPARAM wParam, 
                              LPARAM lParam);
    LRESULT ListWndProc(HWND hwnd, 
                        UINT msg, 
                        WPARAM wParam, 
                        LPARAM lParam);
   
 protected:

    void ParseArgs(int32 argc, char** argv);

    static void UIThreadFunc(void *);

    uint32 CalcStringEllipsis(HDC hdc, string& displayString, int32 columnWidth);
    void UpdateOverallProgress();
    void SetButtonStates(DownloadItem *dli);

 private:
    FAContext*		    m_context;
    Semaphore*          m_uiSemaphore;
    PlaylistManager*    m_plm;
    Preferences*        m_prefs;
    Properties*         m_propManager;
    Thread*             m_uiThread;
    EventQueue*         m_target;
    DownloadManager*    m_dlm;

    HWND    m_hwnd;
    HWND    m_hwndList;
    HWND    m_hwndInfo;
    HWND    m_hwndPause;
    HWND    m_hwndCancel;
    HWND    m_hwndResume;
    HWND    m_hwndClose;
    HWND    m_hwndProgress;
    HWND    m_hwndText;
	HWND    m_hwndCheck;

    HIMAGELIST  m_noteImage;
    HBITMAP  m_progressBitmap;

    uint32 m_totalItems;
    uint32 m_doneItems;
    uint32 m_totalBytes;
    uint32 m_doneBytes;

    RECT m_urlRect;
    HCURSOR m_handCursor;
    bool m_overURL, m_duringUpdate;
};


#endif // INCLUDED_DOWNLOAD_UI_H_
