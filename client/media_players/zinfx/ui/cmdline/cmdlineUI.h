/*____________________________________________________________________________
        
        FreeAmp - The Free MP3 Player

        Portions Copyright (C) 1998-1999 EMusic.com
        Portions Copyright (C) 1999      Henrik Johnson 

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
        
        $Id: cmdlineUI.h,v 1.1 2004/10/13 16:16:13 linha2 Exp $
____________________________________________________________________________*/
// CommandLineCIO.h

#ifndef INCLUDED_COMMANDLINECIO_H_
#define INCLUDED_COMMANDLINECIO_H_

#include "ui.h"
#include "event.h"
#include "thread.h"
#include "playlist.h"

class     FAContext;

class     cmdlineUI:public UserInterface
{
   public:
   cmdlineUI(FAContext * context);
   virtual Error AcceptEvent(Event *);
   virtual Error Init(int32);
   static void keyboardServiceFunction(void *);
             virtual ~ cmdlineUI();

             protected:
             FAContext * m_context;

             private:
             Properties * m_propManager;
   void      ProcessArgs();
   int32     m_startupLevel;
   int32     m_argc;
   char    **m_argv;
   EventQueue *m_playerEQ;
   void      processSwitch(char *);
   Thread   *keyboardListenThread;
   PlaylistManager *m_plm;
   int32     m_lastIndexPlayed;
   bool      m_id3InfoPrinted;
   float     m_secondsPerFrame;
   int32     m_currSeconds;
   int32     m_totalFrames;
   int32     m_totalSeconds;
};

#endif // _COMMANDLINECIO_H_
