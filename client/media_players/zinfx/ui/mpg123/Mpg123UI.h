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
	
	$Id: Mpg123UI.h,v 1.1 2004/10/13 16:16:14 linha2 Exp $
____________________________________________________________________________*/


#ifndef INCLUDED_Mpg123UI_H_
#define INCLUDED_Mpg123UI_H_

#include "ui.h"
#include "event.h"
#include "eventdata.h"
#include "playlist.h"

class FAContext;

class Mpg123UI : public UserInterface {
 public:
    Mpg123UI(FAContext *context);

    virtual Error AcceptEvent(Event *);
    virtual Error Init(int32);
    
    ~Mpg123UI();

    static EventQueue *m_playerEQ;

 protected:
    FAContext *m_context;

 private:
    Properties *m_propManager;
    int32 m_argc;
    char **m_argv;
    int32 m_startupType;
    void ProcessArgs();

    PlaylistManager *m_plm;
    void DisplayMPEGStuff();
    void DisplayVorbisStuff(VorbisInfoEvent *);
    MediaInfoEvent *m_mediaInfo;
    bool m_mediaInfo_set;
    MpegInfoEvent m_mpegInfo;
    bool m_mpegInfo_set;
    bool verboseMode;
    int32 totalFrames;
    float totalTime;
    int32 skipFirst;
    char fileName[512];
    float lastSeconds;

    bool m_displaystuff;
};


#endif // _Mpg123UI_H_
