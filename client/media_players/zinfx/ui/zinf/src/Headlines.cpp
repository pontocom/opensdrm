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
        
   $Id: Headlines.cpp,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/

#include <stdio.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>    

#ifdef WIN32
#include <io.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif

#if defined(unix) || defined(__BEOS__)
#define SOCKET int
#endif

#if defined(unix)
#include <arpa/inet.h>
#define closesocket(x) close(x)
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif  

#include "Headlines.h"
#include "Http.h"
#include "eventdata.h"
#include "event.h"
#include "debug.h"

#define DB printf("%s:%d\n", __FILE__, __LINE__);

HeadlineInfo::HeadlineInfo(const string &oUrl,
                           const string &oXMLPath, 
                           const string &oXMLURLPath, 
                           int iDownloadInterval,
                           int iHeadlineChangeInterval)
{
    m_oUrl = oUrl;
    m_oXMLPath = oXMLPath;
    m_oXMLURLPath = oXMLURLPath;
    m_iDownloadInterval = iDownloadInterval;
    m_iHeadlineChangeInterval = iHeadlineChangeInterval;
}

HeadlineInfo::~HeadlineInfo(void)
{

}

void HeadlineInfo::operator=(const HeadlineInfo &oOther)
{
    m_oUrl = oOther.m_oUrl;
    m_oXMLPath = oOther.m_oXMLPath;
    m_oXMLURLPath = oOther.m_oXMLURLPath;
    m_iDownloadInterval = oOther.m_iDownloadInterval;
    m_iHeadlineChangeInterval = oOther.m_iHeadlineChangeInterval;
}

Headlines::Headlines(FAContext * context)
{
    m_pContext = context;
    m_pWakeSem = new Semaphore();
    m_bPause = false;
    m_lLastDownload = 0;
    m_iIndex = 0;

    m_pThread = Thread::CreateThread();
    m_pThread->Create(worker_thread, this, true);
}

Headlines::~Headlines()
{

    delete m_pThread;
    delete m_pWakeSem;
}

void Headlines::SetInfo(HeadlineInfo &oInfo)
{
    m_oInfo = oInfo;
    m_lLastDownload = 0;
    m_iIndex = 0;
}

void Headlines::Pause(void)
{
    m_bPause = true;    
}

void Headlines::Resume(void)
{
    m_bPause = false;    
    m_pWakeSem->Signal();
}

void Headlines::worker_thread(void* arg)
{
    ((Headlines *)arg)->WorkerThread();
}

void Headlines::WorkerThread(void)
{
    time_t t;

    for(;;)
    {
        if (m_bPause)
           m_pWakeSem->Wait();

        time(&t);
        if (m_lLastDownload + m_oInfo.m_iDownloadInterval < t || m_oHeadlines.size() == 0)
        {
            Download();
            m_lLastDownload = t;
        }

        if (m_oHeadlines.size())
        {
            m_iIndex = m_iIndex % m_oHeadlines.size();
            m_pContext->target->AcceptEvent(new HeadlineMessageEvent(
                    m_oHeadlines[m_iIndex].c_str(),
                    (m_iIndex >= (int)m_oHeadlineURLs.size()) ? "" : 
                     m_oHeadlineURLs[m_iIndex].c_str()));
                                       
            m_iIndex++;
        }
        usleep(m_oInfo.m_iHeadlineChangeInterval * 1000000);
    }
}

Error Headlines::Download(void)
{
#ifndef __QNX__
    string oPage;
    Error  eRet;
    Http   oDownload(m_pContext);
    //vector<string>::iterator i;

    m_pContext->target->AcceptEvent(new StatusMessageEvent("Downloading headlines..."));
    eRet = oDownload.DownloadToString(m_oInfo.m_oUrl, oPage);
    if (eRet != kError_NoErr)
    {
       m_pContext->target->AcceptEvent(new StatusMessageEvent("Headline download failed."));
       return eRet;
    }   

    m_pContext->target->AcceptEvent(new StatusMessageEvent(""));
    m_oHeadlines.clear(); 

    eRet = ParseString(oPage);
    if (IsntError(eRet))
    {
        if (m_oHeadlines.size() != m_oHeadlineURLs.size())
            m_oHeadlineURLs.clear();
    }
    else
    {
        m_pContext->target->AcceptEvent(
                    new StatusMessageEvent (m_oLastError.c_str()));
    }
    
    return eRet;
#endif
    return kError_YouScrewedUp;
}

Error Headlines::BeginElement(string &oElement, AttrMap &oAttrMap)
{
    m_oPath += string("/") + oElement;


    return kError_NoErr;
}

Error Headlines::PCData(string &oData)
{
    if (m_oPath == m_oInfo.m_oXMLPath)
    {
        m_oHeadlines.push_back(oData);
        return kError_NoErr;
    }
    if (m_oPath == m_oInfo.m_oXMLURLPath)
    {
        m_oHeadlineURLs.push_back(oData);
        return kError_NoErr;
    }
    return kError_NoErr;
} 

Error Headlines::EndElement(string &oElement)
{
    char *pPtr;
    int   iOffset;

    pPtr = strrchr(m_oPath.c_str(), '/');
    if (pPtr == NULL)
       return kError_NoErr;

    iOffset = pPtr - m_oPath.c_str();
    m_oPath.erase(iOffset, m_oPath.length() - iOffset);

    return kError_NoErr;
}
