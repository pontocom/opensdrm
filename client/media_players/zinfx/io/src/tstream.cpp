/*____________________________________________________________________________
   
   FreeAmp - The Free MP3 Player

   Copyright (C) 1998-1999 EMusic.com
   Copyright (C) 1999 Alexander Haväng <eel@musiknet.se>

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
   
   $Id: tstream.cpp,v 1.1 2004/10/13 16:16:08 linha2 Exp $
____________________________________________________________________________*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <assert.h>
#ifdef WIN32
#include <winsock.h>
#include <time.h>
#else
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifndef __BEOS__
#include <arpa/inet.h>
#endif
#include <netdb.h>
#include <fcntl.h>
#endif

/* project headers */
#include "config.h"
#include "tstream.h"
#include "facontext.h"
#include "log.h"
#include "eventdata.h"

#if !defined(WIN32) && !defined(__BEOS__)
#define closesocket(s) close(s)
#endif

#define DB printf("%s:%d\n", __FILE__, __LINE__);

static char *splitc(char *first, char *rest, const char divider);

TitleStreamServer::TitleStreamServer(FAContext * context,
                  EventQueue * target)
{
   m_pTarget = target;
   m_pContext = context;

   m_bExit = false;
   m_bUseMulticast = false;
   m_pBufferThread = NULL;
   m_pSin = NULL;
}

TitleStreamServer::~TitleStreamServer()
{
   m_bExit = true;

   if (m_pBufferThread)
   {
      m_pBufferThread->Join();
      delete    m_pBufferThread;
   }

   if (m_pSin)
      free(m_pSin);
}

Error     TitleStreamServer::
Init(int &iPort)
{
   struct sockaddr_in sin;
   uint32    sinlen = sizeof(struct sockaddr_in);
   int       port, startport = 10000;
   char      szSourceAddr[100];
   bool      bUseAltNIC;

   if ((m_hHandle = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
   {
      return kError_UnknownErr;
   }
   else
   {
      memset(&sin, 0, sinlen);
      sin.sin_family = AF_INET;

      m_pContext->prefs->GetPrefBoolean(kUseAlternateNICPref, &bUseAltNIC);
      if (bUseAltNIC)
      {
         uint32    len = 100;

         m_pContext->prefs->GetPrefString(kAlternateNICAddressPref, szSourceAddr, &len);
         if (len == 0)
            m_pContext->log->Error("UseAlternateNIC is true but AlternateNIC "
                                   "has no value ?!");

         sin.sin_addr.s_addr = inet_addr(szSourceAddr);
      }
      else
         sin.sin_addr.s_addr = htonl(INADDR_ANY);

      for (port = startport; port < 32767; port++)
      {
         sin.sin_port = htons(port);

         if (bind(m_hHandle, (struct sockaddr *) &sin, sinlen) < 0)
            continue;
         else
         {
            iPort = port;
            return kError_NoErr;
         }
      }

      close(m_hHandle);
      return kError_UnknownErr;
   }
}

Error     TitleStreamServer::
MulticastInit(char *szAddr, int iPort)
{
#ifdef __BEOS__
   return kError_CantCreateSocket;
#else
   int       iRet;
   struct ip_mreq sMreq;
   int       iReuse = 0;
   char      szSourceAddr[100];
   bool      bUseAltNIC = false;

   m_bUseMulticast = true;

   m_hHandle = socket(AF_INET, SOCK_DGRAM, 0);
   if (m_hHandle < 0)
   {
      return kError_CantCreateSocket;
   }

   m_pSin = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));

   assert(m_pSin);

   iReuse = 1;
   m_pSin->sin_family = AF_INET;
   m_pSin->sin_port = htons(iPort);

   m_pContext->prefs->GetPrefBoolean(kUseAlternateNICPref, &bUseAltNIC);
   if (bUseAltNIC)
   {
      uint32    len = 100;

      m_pContext->prefs->GetPrefString(kAlternateNICAddressPref, szSourceAddr, &len);
      if (len == 0)
         m_pContext->log->Error("UseAlternateNIC is true but AlternateNIC "
                                "has no value ?!");

      m_pSin->sin_addr.s_addr = inet_addr(szSourceAddr);
   }
   else
      m_pSin->sin_addr.s_addr = htonl(INADDR_ANY);

   iRet = setsockopt(m_hHandle, SOL_SOCKET, SO_REUSEADDR,
                     (const char *) &iReuse, sizeof(int));

   if (iRet < 0)
   {
      close(m_hHandle);
      m_hHandle = -1;
      return kError_CannotSetSocketOpts;
   }

   iRet = bind(m_hHandle, (struct sockaddr *) m_pSin,
               sizeof(struct sockaddr_in));

   if (iRet < 0)
   {
      close(m_hHandle);
      m_hHandle = -1;
      return kError_CannotBind;
   }

   sMreq.imr_multiaddr.s_addr = inet_addr(szAddr);
   sMreq.imr_interface.s_addr = htonl(INADDR_ANY);
   iRet = setsockopt(m_hHandle, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                     (char *) &sMreq, sizeof(sMreq));
   if (iRet < 0)
   {
      close(m_hHandle);
      m_hHandle = -1;
      return kError_CannotSetSocketOpts;
   }

   return kError_NoErr;
#endif
}

Error     TitleStreamServer::
Run(in_addr & sAddr, int iPort)
{
   struct sockaddr_in sin;

   memset(&sin, 0, sizeof(struct sockaddr_in));

   sin.sin_family = AF_INET;
   sin.sin_addr = sAddr;
   sin.sin_port = htons(iPort);

   if (connect(m_hHandle, (struct sockaddr *) &sin, sizeof(sin)) < 0)
   {
      close(m_hHandle);
      return kError_ConnectFailed;
   }

   return Run();
}

Error     TitleStreamServer::
Run(void)
{
   if (!m_pBufferThread)
   {
    m_pBufferThread = Thread::CreateThread();
      if (!m_pBufferThread)
      {
         return (Error) kError_CreateThreadFailed;
      }
    m_pBufferThread->Create(TitleStreamServer::StartWorkerThread, this);
   }

   return kError_NoErr;
}

void      TitleStreamServer::
StartWorkerThread(void *pVoidBuffer)
{
   ((TitleStreamServer *) pVoidBuffer)->WorkerThread();
}

void      TitleStreamServer::
WorkerThread(void)
{
   char      buf[1024], line[1024];
   char     *szTitle = NULL, *szURL = NULL, *lasturl = NULL, *valptr = NULL;
   fd_set    sSet;
   struct timeval sTv;
   int       iRet, go_on = 1;
   fa_socklen_t iStructSize;

   for (; !m_bExit;)
   {
      sTv.tv_sec = 0;
      sTv.tv_usec = 0;
      FD_ZERO(&sSet);
      FD_SET(m_hHandle, &sSet);

      iRet = select(m_hHandle + 1, &sSet, NULL, NULL, &sTv);

      if (!iRet)
      {
         usleep(10000);         /* Hope you know usleep is not threadsafe... */
         continue;
      }

      if (m_bUseMulticast)
      {
         iStructSize = sizeof(struct sockaddr_in);

         iRet = recvfrom(m_hHandle, buf, 1024, 0, (struct sockaddr *) m_pSin,
                         &iStructSize);
      }
      else
         iRet = recv(m_hHandle, buf, 1024, 0);

      if (iRet > 0)
      {
         buf[iRet] = 0;
         go_on = 1;
         /* Data received, line by line parsing */
         while (go_on)
         {
            if (splitc(line, buf, '\n') == NULL)
            {
               go_on = 0;
               strcpy(line, buf);
            }

            valptr = strchr(line, ':');

            if (!valptr)
               continue;
            else
               valptr++;

            while (valptr && valptr[0] && valptr[0] == ' ')
               valptr++;

            if (valptr[strlen(valptr) - 1] == '\r')
               valptr[strlen(valptr) - 1] = '\0';

            if (strstr(line, "x-audiocast-streamtitle") != NULL)
            {
               if (valptr)
                  szTitle = strdup(valptr);
            }

            else if (strstr(line, "x-audiocast-streammsg") != NULL)
            {
               if (valptr)
                  szTitle = strdup(valptr);

            }

            else if (strstr(line, "x-audiocast-streamurl") != NULL)
            {
               if (lasturl && strcmp(valptr, lasturl))
               {
                  if (lasturl)
                     free(lasturl);
                  lasturl = strdup(valptr);
                  szURL = strdup(valptr);
               }
               else
               {
                  lasturl = strdup(valptr);
                  szURL = strdup(valptr);
               }
            }

            else if (strstr(line, "x-audiocast-udpseqnr:") != NULL)
            {
               char      obuf[1024];

#ifdef WIN32
               _snprintf(obuf, 1024, "x-audiocast-ack: %ld\r\n", atol(valptr));
#else
               snprintf(obuf, 1024, "x-audiocast-ack: %ld\r\n", atol(valptr));
#endif
               if (send(m_hHandle, obuf, strlen(obuf), 0) == -1)
                  fprintf(stderr, "Could not send ack to server\n");
            }
         }

         if (szTitle)
         {
            if (szURL)
                m_pTarget->AcceptEvent(new StreamInfoEvent(szTitle, szURL));
            else
                m_pTarget->AcceptEvent(new StreamInfoEvent(szTitle, ""));
            free(szTitle);
            free(szURL);
            szTitle = NULL;
            szURL = NULL;
         }
      }
   }

   free(szTitle);
   free(szURL);

   shutdown(m_hHandle, 2);
   closesocket(m_hHandle);
   m_hHandle = -1;
}

static char *
splitc(char *first, char *rest, const char divider)
{
   char     *p;

   p = strchr(rest, divider);
   if (p == NULL)
   {
      if ((first != rest) && (first != NULL))
         first[0] = 0;

      return NULL;
   }

   *p = 0;
   if (first != NULL)
      strcpy(first, rest);
   if (first != rest)
      strcpy(rest, p + 1);

   return rest;
}
