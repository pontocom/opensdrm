/*____________________________________________________________________________
        
        FreeAmp - The Free MP3 Player

        Portions Copyright (C) 1998-1999 EMusic.com
        Portions Copyright (C) 1999 Jean Tourrilhes <jeant@rockfort.hpl.hp.com>

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
        
        $Id: obsinput.cpp,v 1.1 2004/10/13 16:16:07 linha2 Exp $
____________________________________________________________________________*/

/* system headers */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#ifdef WIN32
#include <winsock.h>
#include <time.h>
#else
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <fcntl.h>
#endif

#include <config.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#elif HAVE_IO_H
#include <io.h>
#else
#error Must have unistd.h or io.h!
#endif // HAVE_UNISTD_H

#include "obsinput.h"
#include "log.h"
#include "facontext.h"
#include "debug.h"

#ifdef WIN32
#define IN_MULTICAST(a)  ((((uint32) (a)) & 0xf0000000) == 0xe0000000)
#endif

const int iMaxHostNameLen = 64;
const int iGetHostNameBuffer = 1024;
const int iInitialBufferSize = 64;
const int iReadTimeout       = 10;       // in secs
const int iBufferSize = 8192;
const int iOverflowSize = 1536;
const char *szDefaultStreamTitle = "RTP Stream";

#define DB printf("%s:%d\n", __FILE__, __LINE__);

extern "C"
{
   PhysicalMediaInput *Initialize(FAContext *context)
   {
      return new ObsInput(context);
   }
}

ObsInput::ObsInput(FAContext *context): 
          PhysicalMediaInput(context)
{
    m_path = NULL;
    m_hHandle = -1;
    m_pBufferThread = NULL;
    m_bLoop = true;
    m_bDiscarded = false;
    m_pTitleStream = NULL;  
}


ObsInput::~ObsInput()
{
    m_bExit = true;
    m_pSleepSem->Signal();
    m_pPauseSem->Signal();

    if (m_pTitleStream)
           delete m_pTitleStream;  

    if (m_pBufferThread)
    {
       m_pBufferThread->Join();
       delete m_pBufferThread;
    }

    if (m_hHandle >= 0)
       close(m_hHandle);
}

bool ObsInput::CanHandle(const char *szUrl, char *szTitle)
{
   bool bRet;

   bRet = strncmp(szUrl, "rtp://", 6) == 0;
   if (szTitle && bRet)
      strcpy(szTitle, szDefaultStreamTitle);

   return bRet;
}

Error ObsInput::Prepare(PullBuffer *&pBuffer)
{
    int iBufferSize = iDefaultBufferSize;
    Error result;


    if (m_pOutputBuffer)
    {
       delete m_pOutputBuffer;
       m_pOutputBuffer = NULL;
    }

    if (!IsError(m_pContext->prefs->GetPrefInt32(kInputBufferSizePref, &iBufferSize)))
       iBufferSize *= 1024;

    m_pOutputBuffer = new PullBuffer(iBufferSize, iDefaultOverflowSize,
                                     m_pContext);
    assert(m_pOutputBuffer);

    pBuffer = m_pOutputBuffer;

    result = Run();
    if (IsError(result))
    {
        ReportError("Could not initialize rtp streaming plugin.");
        return result;
    }

    return kError_NoErr;
}  

Error ObsInput::Close(void)
{
   delete m_pOutputBuffer;
   m_pOutputBuffer = NULL;

   if (m_hHandle != 0)
   {
      close(m_hHandle);
      m_hHandle = -1;
   }

   return kError_NoErr;
}

Error ObsInput::Open(void)
{
    int    iRet, iPort;
    struct ip_mreq sMreq;
    int    iReuse=0;
    char   szAddr[100], szSourceAddr[100];
    bool   bUseTitleStreaming = false, bUseAltNIC = false;;

    ReportStatus("Setting up RTP stream...");

    iRet = sscanf(m_path, "rtp://%[^:]:%d", szAddr, &iPort);
    if (iRet < 2)
    {
        ReportError("Invalid URL. URL format: rtp://<multicast addr>[:port]");
        return (Error)obsError_BadUrl;
    }

    m_hHandle = socket( AF_INET, SOCK_DGRAM, 0 );
    if (m_hHandle < 0)
    {
       ReportError("Cannot create socket. Is TCP/IP networking properly installed?");
       return (Error)obsError_CannotCreateSocket;
    }

    m_pSin = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    assert(m_pSin);

    iReuse = 1;
    m_pSin->sin_family = AF_INET;
    m_pSin->sin_port = htons(iPort);

    m_pContext->prefs->GetPrefBoolean(kUseAlternateNICPref, &bUseAltNIC);
    if (bUseAltNIC)
    {
        uint32 len = 100;

        m_pContext->prefs->GetPrefString(kAlternateNICAddressPref, szSourceAddr, &len);
        if ( len == 0 )
            m_pContext->log->Error("UseAlternateNIC is true but AlternateNIC "
                                   "has no value ?!");

        m_pSin->sin_addr.s_addr = inet_addr(szSourceAddr);
    }  
    else
        m_pSin->sin_addr.s_addr = htonl(INADDR_ANY);

    sMreq.imr_multiaddr.s_addr = inet_addr(szAddr);
    sMreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (IN_MULTICAST(ntohl(sMreq.imr_multiaddr.s_addr)))
    {
        iRet = setsockopt(m_hHandle, SOL_SOCKET, SO_REUSEADDR, 
                          (const char *)&iReuse, sizeof(int));
        if (iRet < 0)
        {
           close(m_hHandle);
           m_hHandle= -1;
           ReportError("Cannot set socket options. Is TCP/IP networking properly installed?");
           return (Error)obsError_CannotSetSocketOpts;
        }
    }    

    iRet = bind(m_hHandle, (struct sockaddr *)m_pSin, 
                sizeof(struct sockaddr_in));
    if (iRet < 0)
    {
       close(m_hHandle);
       m_hHandle= -1;
       ReportError("Cannot bind the socket. Is TCP/IP networking properly installed?");
       return (Error)obsError_CannotBind;
    }

    if (IN_MULTICAST(ntohl(sMreq.imr_multiaddr.s_addr)))
    {
        iRet = setsockopt(m_hHandle, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                       (char *)&sMreq, sizeof(sMreq));
        if (iRet < 0)
        {
           close(m_hHandle);
           m_hHandle= -1;
           ReportError("Cannot set socket multicast options. Is TCP/IP networking properly installed?");
           return (Error)obsError_CannotSetSocketOpts;
        }
    }    

    m_pContext->prefs->GetPrefBoolean(kUseTitleStreamingPref, &bUseTitleStreaming);
    if (bUseTitleStreaming)
    {
        Error eRet;

        m_pTitleStream = new TitleStreamServer(m_pContext, m_pTarget);

        eRet = m_pTitleStream->MulticastInit(szAddr, iPort + 2);
        if (IsError(eRet))
        {
            delete m_pTitleStream;
            m_pTitleStream = NULL;
        }
        else
            m_pTitleStream->Run();
    } 

    return kError_NoErr;
}

Error ObsInput::Run(void)
{
    if (!m_pBufferThread)
    {
       m_pBufferThread = Thread::CreateThread();
       if (!m_pBufferThread)
       {
           return (Error)kError_CreateThreadFailed;
       }
       m_pBufferThread->Create(ObsInput::StartWorkerThread, this);
    }

    return kError_NoErr;
}

bool ObsInput::PauseLoop(bool bLoop)
{
   bool bRet;

   m_bLoop = bLoop;
   bRet = m_bDiscarded;
   m_bDiscarded = false;

   return bRet;
} 

void ObsInput::StartWorkerThread(void *pVoidBuffer)
{
   ((ObsInput*)pVoidBuffer)->WorkerThread();
}

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

void ObsInput::WorkerThread(void)
{
   int             iRead, iPacketNum = -1, iCurrNum, iRet, iHeaderSize;
   fa_socklen_t    iStructSize;
   RTPHeader      *pHeader;
   void           *pBuffer;
   unsigned        char *pTemp;
   Error           eError;
   fd_set          sSet;
   struct timeval  sTv;

   eError = Open();
   if (IsError(eError) || m_bExit)
   {
      return;
   }   

   m_pSleepSem->Wait(); 

   ReportStatus("Listening for RTP stream...");

   pTemp = new unsigned char[iMAX_PACKET_SIZE];
   pHeader = (RTPHeader *)pTemp;
   for(; !m_bExit;)
   {
      if (m_pOutputBuffer->IsEndOfStream())
      {
          m_pSleepSem->Wait();
          continue;
      }
      if (m_bPause)
      {
          m_pPauseSem->Wait();
          continue;
      }

      sTv.tv_sec = 0;
      sTv.tv_usec = 100000;  // .1 second
      FD_ZERO(&sSet);
      FD_SET(m_hHandle, &sSet);
      iRet = select(m_hHandle + 1, &sSet, NULL, NULL, &sTv);  
      if (!iRet)
      {
         continue;
      }

      iStructSize = sizeof(struct sockaddr_in);
      iRead = recvfrom(m_hHandle, (char *)pTemp, iMAX_PACKET_SIZE, 0,
                       (struct sockaddr *)m_pSin, &iStructSize);
      if (iRead <= 0)
      {
         m_pOutputBuffer->SetEndOfStream(true);
         break;
      }

      for(;;)
      {
          eError = m_pOutputBuffer->BeginWrite(pBuffer, iRead);
          if (eError == kError_BufferTooSmall)
          {
              if (m_bLoop)
              {
                 m_pOutputBuffer->DiscardBytes();
                 m_bDiscarded = true;
              }
              else
                 m_pSleepSem->Wait();

              continue;
          }
          break;
      }
      if (eError != kError_NoErr)
         break; 

      pHeader->iFlags = ntohl(pHeader->iFlags);
      iCurrNum = pHeader->iFlags & 0xFFFF;
      if (iPacketNum != -1 && iPacketNum != iCurrNum - 1)
      {
          time_t t;

          time(&t);
          m_pContext->log->Log(LogPerf, "Lost packet (%d, %d): %s", 
             iPacketNum, iCurrNum, ctime(&t)); 
      }
      iPacketNum = iCurrNum;

      iHeaderSize = sizeof(RTPHeader) + sizeof(int32);
      iHeaderSize += sizeof(int32) * ((pHeader->iFlags >> 24) & 0xF);

      iRead -= iHeaderSize;
      memcpy(pBuffer, pTemp + iHeaderSize, iRead);
      eError = m_pOutputBuffer->EndWrite(iRead);
      if (IsError(eError))
      {
         m_pContext->log->Error("Obs: EndWrite returned: %d\n", eError);
      }
   }

   delete pTemp;
   if (m_hHandle >= 0)
      close(m_hHandle);
      
   m_hHandle = -1;
   m_pContext->log->Log(LogInput, "Worker thread done");
}

vector<string> *ObsInput::GetProtocols(void)
{
   vector<string> *protoList = new vector<string>;

   protoList->push_back("rtp");

   return protoList;
}

