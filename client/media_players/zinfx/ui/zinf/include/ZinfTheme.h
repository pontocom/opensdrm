
/*____________________________________________________________________________
        
   FreeAmp - The Free MP3 Player

   Copyright (C) 1999 EMusic.com

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
        
   $Id: ZinfTheme.h,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_FREEAMP_THEME_H
#define INCLUDED_FREEAMP_THEME_H

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "ui.h"
#include "thread.h"
#include "facontext.h"
#include "event.h"
#include "playlist.h"
#include "Theme.h"
#include "ThemeManager.h"
#include "Equalizer.h"

enum TimeDisplayState
{
    kNormal,
    kTimeRemaining
};

enum TitleDisplayState
{
    kName = 0,
    kNameArtist = 1,
    kNameArtistAlbum = 2
};

enum FreeAmpMenuCommand
{
   kMCMyMusic = 6900,
   kMCPlay    = 6901,
	kMCStop    = 6902,
	kMCPause   = 6903,
	kMCNext    = 6904,
	kMCPrev    = 6905,
	kMCExit    = 6906
};

enum SignatureDisplayState
{
    kIdle,
    kSigsPending,
    kGeneratingSigs
};

class FreeAmpTheme : public UserInterface, public Theme
{
    public:

        FreeAmpTheme(FAContext* context);
        virtual ~FreeAmpTheme();

        virtual Error AcceptEvent(Event* e);
        virtual Error Close(void);
        virtual Error Init(int32 startup_type);
        virtual void  LoadFreeAmpTheme(void);
        
        virtual void  HandleKeystroke(unsigned char cKey);
        virtual bool  HandleMenuCommand(uint32 uCommand);
        virtual void  HandleMouseWheelChange(int iSteps);
        virtual void  HandleMouseWheelClick(void);
        virtual void  VolumeChanged(void);
        virtual Error HandleControlMessage(string &oControlName, 
                                           ControlMessageEnum eMesg);
        virtual void  DropFiles(vector<string> *pFileList);
        virtual void  InitControls(void);
        virtual void  InitWindow(void);
        virtual void  PostWindowCreate(void);
                void  ShowOptions(uint32 defaultPage = 0);

                void  WorkerThread(void);

        static void update_thread(void*);
        static void options_thread(void*);

    protected:

        void             ShowHelp(void);
        void             ReloadTheme(void);
        void             SetVolume(int iVolume, int iBalance);
        void             UpdateTimeDisplay(int iCurrentTime);
        void             UpdateMetaData(const PlaylistItem *pItem);
        void             UpdateThread();
        void             OptionsThread(uint32 defaultPage);

        FAContext       *m_pContext;
        int              m_iCurrentSeconds, m_iTotalSeconds, m_iSeekSeconds;
        float            m_fSecondsPerFrame;
        int              m_iStartupType, m_iVolume, m_iBalance, m_iMuteVolume;
        int              m_iSeekPos, m_iFramesSinceSeek;
        Thread          *m_uiThread, *m_pUpdateThread;
        Thread          *m_pOptionsThread;
        Pos              m_oWindowPos;
        bool             m_bSeekInProgress, m_bVolumeChangeInProgress;
        bool             m_bPaused, m_bInOptions, m_bPlayShown;
        bool             m_bShowBuffers, m_bBufferingUp;
        string           m_oCurrentWindow, m_oTitle, m_oStreamInfo;
        string           m_oHeadlineUrl, m_oArtist, m_oAlbum, m_oTrackNo;
        string           m_oYear, m_oTrackName, m_oGenre, m_oComment;
        string           m_oFileName;
        TimeDisplayState      m_eTimeDisplayState;
        TitleDisplayState     m_eTitleDisplayState;
        string                m_themeCache;
        UpdateManager        *m_pUpdateMan;
        Equalizer            *m_eq;
};

#endif
