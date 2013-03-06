/*____________________________________________________________________________
        
   FreeAmp - The Free MP3 Player

   Copyright (C) 2000 EMusic.com

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
        
   $Id: Headlines.h,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_SLASHDOT_H
#define INCLUDED_SLASHDOT_H

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <vector>
#include <string>
#include <time.h>
#include "thread.h" 
#include "semaphore.h" 
#include "Parse.h" 
#include "facontext.h"

class HeadlineInfo
{
    public:

        HeadlineInfo(void) { ; };
        HeadlineInfo(const string &oUrl,
                     const string &oXMLPath, 
                     const string &oXMLURLPath, 
                     int iDownloadInterval,
                     int iHeadlineChangeInterval);
        virtual ~HeadlineInfo();

        void   operator=(const HeadlineInfo &oOther);

        string m_oUrl, m_oXMLPath, m_oXMLURLPath;
        int    m_iDownloadInterval, m_iHeadlineChangeInterval;
};

class Headlines : public Parse
{
    public:

        Headlines(FAContext * context);
        virtual ~Headlines();

        void        SetInfo(HeadlineInfo &oInfo);
        void        Pause(void);
        void        Resume(void);
        static void worker_thread(void*);

    protected:

        virtual Error BeginElement(string &oElement, AttrMap &oAttrMap);
        virtual Error PCData(string &oData);
        virtual Error EndElement(string &oElement);

        void   WorkerThread(void);
        Error  Download(void);
        Error  DownloadHeadlines(string &oUrl, string &oPage);

        FAContext       *m_pContext;
        Thread          *m_pThread;
        string           m_oPath;
        vector<string>   m_oHeadlines, m_oHeadlineURLs;
        bool             m_bPause;
        Semaphore       *m_pWakeSem;
		HeadlineInfo     m_oInfo;
		time_t           m_lLastDownload;
		int              m_iIndex;
};

#endif
