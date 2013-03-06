/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player

        Portions Copyright (C) 1999 EMusic.com
	Copyright (C) 1999 Mark H. Weaver <mhw@netris.org>

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
	
	$Id: facontext.h,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_FACONTEXT_H_
#define INCLUDED_FACONTEXT_H_

#include "config.h"
#include "preferences.h"
#include "log.h"
#include "mutex.h"

static const int32 c_majorVersion = 1;
static const int32 c_minorVersion = 0;

// argFlags bits
#define FAC_ARGFLAGS_SAVE_STREAMS 1

class PlaylistManager;
class Properties;
class EventQueue;
class MusicCatalog;
class DownloadManager;
class UpdateManager;
class Player;
class TimerManager;
class APSInterface;

class FAContext
{
 public:
    FAContext()
	: majorVersion(c_majorVersion),
	  minorVersion(c_minorVersion),
	  prefs(0),
	  log(0),
          props(0),
          plm(0),
          target(0),
	  catalog(0),
          downloadManager(0),
          aps(0),
          //updateManager(0),
          player(0),
#ifdef HAVE_GTK
	  gtkInitialized(false),
          gtkRunning(false),
#endif	  
#ifdef WIN32
      hInstance(0),
#endif      
	  argc(0),
          argv(0),	  
	  argFlags(0) { }
    
    ~FAContext()
    {
	    if (log)
	        delete log;
	    if (prefs)
	        delete prefs;
    }

    bool CompatibleVersion() { return majorVersion == c_majorVersion; }

    int32 majorVersion, minorVersion;
    Preferences *prefs;
    LogFile *log;
    Properties *props;
    PlaylistManager *plm;
    EventQueue *target;
    MusicCatalog *catalog;
    DownloadManager *downloadManager;
    APSInterface *aps;
    //UpdateManager *updateManager;
    TimerManager *timerManager;
    Player *player;
#ifdef HAVE_GTK
    Mutex gtkLock;
    bool gtkInitialized;
    bool gtkRunning;
#endif    
#ifdef WIN32
    HINSTANCE hInstance;
#endif
    int32 argc;
    char** argv;
    uint32 argFlags;
};

#endif // _FACONTEXT_H_
