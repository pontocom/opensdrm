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
        
        $Id: httpinput.cpp,v 1.1 2004/10/13 16:16:06 linha2 Exp $
____________________________________________________________________________*/

/* system headers */
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

#include "config.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#elif HAVE_IO_H
#include <io.h>
#else
#error Must have unistd.h or io.h!
#endif // HAVE_UNISTD_H

/* project headers */
#include "httpinput.h"
#include "facontext.h"
#include "log.h"
#include "tstream.h"
#include "debug.h"

const int iBufferSize = 8192;
const int iOverflowSize = 1536;
const int iTriggerSize = 1024;
const char *szDefaultStreamTitle = "SHOUTcast Stream";

#if !defined(WIN32) && !defined(__BEOS__)
#define closesocket(s) close(s)
#endif

#ifndef F_OK
#define F_OK 0
#endif

const int iHttpPort = 80;
const int iMaxHostNameLen = 64;
const int iGetHostNameBuffer = 1024;
const int iBufferUpInterval = 3;
const int iInitialBufferSize = 1024;
const int iHeaderSize = 1024;
const int iICY_OK = 200;
const int iICY_REDIRECT = 302;
const int iTransmitTimeout = 60;

#ifdef WIN32
const char cDirSepChar = '\\';
#else
const char cDirSepChar = '/';
#endif

#define DB Debug_v("%s:%d\n", __FILE__, __LINE__);

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

extern    "C"
{
   PhysicalMediaInput *Initialize(FAContext * context)
   {
      return new HttpInput(context);
   }
}

#ifdef WIN32
HINSTANCE g_hinst = NULL;
INT WINAPI
DllMain(HINSTANCE hInstance, ULONG ul_reason_being_called, LPVOID lpReserved)
{
   switch (ul_reason_being_called)
   {
   case DLL_PROCESS_ATTACH:
      g_hinst = hInstance;
      break;

   case DLL_THREAD_ATTACH:
      break;

   case DLL_THREAD_DETACH:
      break;

   case DLL_PROCESS_DETACH:
      break;
   }

   return 1;
}
#endif

HttpInput::HttpInput(FAContext * context):
PhysicalMediaInput(context)
{
   m_path = NULL;
   m_hHandle = -1;
   m_bLoop = false;
   m_bDiscarded = false;
   m_bIsStreaming = true;
   m_pBufferThread = NULL;
   m_fpSave = NULL;
   m_szError = new char[iMaxErrorLen];

   m_pTitleStream = NULL;
   m_bUseBufferReduction = true;
   m_iMetaDataInterval = 0;
   m_uBytesReceived = 0;
   m_bExit = false;
#ifdef WIN32
   m_hWnd = NULL;
#endif

   m_pContext->prefs->GetPrefBoolean(kUseProxyPref, &m_bUseProxy);
   if (m_bUseProxy)
   {
      uint32    len = iMaxUrlLen;

      m_pContext->prefs->GetPrefString(kProxyHostPref, m_szProxyHost, &len);
      if (len == 0)
         m_pContext->log->Error("useProxy is true but ProxyHost "
                                "has no value ?!");
   }
}

HttpInput::~HttpInput()
{
#ifdef WIN32
   if (m_hWnd)
      PostMessage(m_hWnd, WM_QUIT, 0, 0);
#endif

   m_bExit = true;
   m_pSleepSem->Signal();
   m_pPauseSem->Signal();

   if (m_pTitleStream)
      delete    m_pTitleStream;

   if (m_pBufferThread)
   {
      m_pBufferThread->Join();
      delete    m_pBufferThread;
   }

   if (m_hHandle >= 0)
   {
      shutdown(m_hHandle, 2);
      closesocket(m_hHandle);
   }

   if (m_fpSave)
      fclose(m_fpSave);

   delete [] m_szError;
}

bool
HttpInput::CanHandle(const char *szUrl, char *szTitle)
{
   bool      bRet;

   bRet = strncmp(szUrl, "http://", 7) == 0;

   if (szTitle && bRet)
      strcpy(szTitle, szDefaultStreamTitle);

   return bRet;
}

Error
HttpInput::Prepare(PullBuffer * &pBuffer)
{
   int32     iBufferSize = iDefaultBufferSize;
   Error     result;

   if (m_pOutputBuffer)
   {
      delete    m_pOutputBuffer;

      m_pOutputBuffer = NULL;
   }

   if (!IsError
       (m_pContext->prefs->GetPrefInt32(kInputBufferSizePref, &iBufferSize)))
      iBufferSize *= 1024;

   m_pOutputBuffer = new PullBuffer(iBufferSize, iDefaultOverflowSize,
                                    m_pContext);
   assert(m_pOutputBuffer);

   pBuffer = m_pOutputBuffer;

   result = Run();
   if (IsError(result))
   {
      ReportError("Could not initialize http streaming plugin.");
      return result;
   }

   return kError_NoErr;
}

Error
HttpInput::Close(void)
{
#ifdef WIN32
   if (m_hWnd)
      PostMessage(m_hWnd, WM_QUIT, 0, 0);
#endif

   delete    m_pOutputBuffer;

   m_pOutputBuffer = NULL;

   if (m_hHandle >= 0)
   {
      shutdown(m_hHandle, 2);
      closesocket(m_hHandle);
   }

   if (m_fpSave)
      fclose(m_fpSave);

   return kError_NoErr;
}

Error
HttpInput::Run(void)
{
   if (!m_pBufferThread)
   {
      m_pBufferThread = Thread::CreateThread();
      if (!m_pBufferThread)
      {
         return (Error) kError_CreateThreadFailed;
      }
      m_pBufferThread->Create(HttpInput::StartWorkerThread, this);
   }

   return kError_NoErr;
}

bool
HttpInput::PauseLoop(bool bLoop)
{
   bool      bRet;

   m_bLoop = bLoop;
   bRet = m_bDiscarded;
   m_bDiscarded = false;

   return bRet;
}

#ifdef WIN32
#define WM_WINDOWS_IS_DONE_PICKING_ITS_BUTT (WM_USER + 1)

static LRESULT WINAPI WndProc(HWND hwnd, UINT msg,

                              WPARAM wParam, LPARAM lParam);

Error
HttpInput::Win32GetHostByName(char *szHostName, struct hostent *pHostInfo)
{
   WNDCLASS  wc;
   MSG       msg;
   HWND      hWnd;
   HANDLE    hHandle;
   char      szBuffer[MAXGETHOSTSTRUCT];
   int       result = -1;

   memset(&wc, 0x00, sizeof(WNDCLASS));

   wc.style = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc = WndProc;
   wc.hInstance = g_hinst;
   wc.hCursor = NULL;
   wc.hIcon = NULL;
   wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
   wc.lpszClassName = "WindowsSucks";

   result = RegisterClass(&wc);

   hWnd = CreateWindow(wc.lpszClassName, "Fuss",
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                       CW_USEDEFAULT, NULL, NULL, g_hinst, NULL);
   if (hWnd == NULL)
      return kError_NoDataAvail;

   m_hWnd = hWnd;
   hHandle =
      WSAAsyncGetHostByName(hWnd, WM_WINDOWS_IS_DONE_PICKING_ITS_BUTT,
                            szHostName, szBuffer, MAXGETHOSTSTRUCT);
   if (hHandle == NULL)
   {
      DestroyWindow(hWnd);
      return kError_NoDataAvail;
   }

   while (GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);

      if (msg.message == WM_WINDOWS_IS_DONE_PICKING_ITS_BUTT)
         result = WSAGETASYNCERROR(msg.lParam);

      DispatchMessage(&msg);
   }

   if (m_bExit)
   {
      WSACancelAsyncRequest(hHandle);
   }

   m_hWnd = NULL;
   DestroyWindow(hWnd);

   if (m_bExit)
   {
      return kError_Interrupt;
   }

   if (result == 0)
   {
      memcpy(pHostInfo, szBuffer, sizeof(struct hostent));

      return kError_NoErr;
   }
   else
   {
      static unsigned long IP_Adr;
      static char *AdrPtrs[2] = { (char *) &IP_Adr, NULL };

      // That didn't work.  On some stacks a numeric IP address
      // will not parse with gethostbyname.  Try to convert it as a
      // numeric address before giving up.
      if ((IP_Adr = inet_addr(szHostName)) == INADDR_NONE)
         return kError_NoDataAvail;

      pHostInfo->h_length = sizeof(uint32);
      pHostInfo->h_addrtype = AF_INET;
      pHostInfo->h_addr_list = (char **) &AdrPtrs;
      return kError_NoErr;
   }

   return kError_NoDataAvail;
}

static LRESULT WINAPI
WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   LRESULT   result = 0;

   switch (msg)
   {
   case WM_WINDOWS_IS_DONE_PICKING_ITS_BUTT:
      {
         PostMessage(hwnd, WM_QUIT, 0, 0);
      }
   default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
   }
   return result;
}

#endif

Error
HttpInput::GetHostByName(char *szHostName, struct hostent * pResult)
{
   struct hostent *pTemp;
   struct hostent TempHostent;
   static unsigned long IP_Adr;
   static char *AdrPtrs[2] = { (char *) &IP_Adr, NULL };

#ifdef WIN32
   Error     eRet;

   eRet = Win32GetHostByName(szHostName, &TempHostent);
   if (eRet == kError_Interrupt)
      return eRet;

   if (IsError(eRet))
      pTemp = NULL;
   else
      pTemp = &TempHostent;

#else
   pTemp = gethostbyname(szHostName);
#endif

   if (pTemp == NULL)
   {
      // That didn't work.  On some stacks a numeric IP address
      // will not parse with gethostbyname.  Try to convert it as a
      // numeric address before giving up.
      if ((int) (IP_Adr = inet_addr(szHostName)) == INADDR_ANY)
         return kError_NoDataAvail;

      TempHostent.h_length = sizeof(uint32);
      TempHostent.h_addrtype = AF_INET;
      TempHostent.h_addr_list = (char **) &AdrPtrs;
      pTemp = &TempHostent;
   }

   memcpy(pResult, pTemp, sizeof(struct hostent));

   return kError_NoErr;
}

static void
EncodeURI(string & URI)
{
   string::size_type convert = 0;
   const char *legalCharacters =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/?&=-.";

   if (strncmp(URI.c_str(), "http://", 7) == 0)
       convert = URI.find(string("/"), 7);

   while ((convert = URI.find_first_not_of(legalCharacters, convert)) !=
          string::npos)
   {
      string    hex = "%";
      char      num[8];

      // Do not replace %## sequences -- they are already encoded and
      // ready to roll
      if (URI[convert] == '%' && URI.length() - convert > 2 &&
          isdigit(URI[convert + 1]) && 
          isdigit(URI[convert + 2])) 
      {
          convert++;
          continue;
      }

      sprintf(num, "%02x", URI[convert] & 0xFF);
      hex += num;

      URI.replace(convert, 1, hex);

      convert += hex.length();
   }
}

Error
HttpInput::Open(void)
{
   char      szHostName[iMaxHostNameLen + 1], *szQuery;
   char     *pEnd;
   char     *pInitialBuffer, szSourceAddr[100];
   char     *szStreamName, *szStreamUrl;
   unsigned  iPort;
   int       iRet, iRead = 0, iConnect;
   struct sockaddr_in sAddr, sSourceAddr;
   struct hostent sHost;
   Error     eRet;
   char     *pHeaderData = NULL, *pPtr;
   fd_set    sSet;
   struct timeval sTv;
   bool      bUseTitleStreaming = true, bUseAltNIC = false;
   int       iHeaderBytes = 0, iCurHeaderSize = 1024;
   string    file;

   szStreamName = NULL;
   szStreamUrl = NULL;
   if (!m_bUseProxy)
   {
      const char* ptr;

      iRet = sscanf(m_path, " http://%[^:/]:%d", szHostName, &iPort);
      if (iRet < 1)
      {
         ReportError("Bad URL format. URL format: http://<host name>"
                     ":[port][/path]. Please check the URL and try again.");
         return (Error) httpError_BadUrl;
      }
      ptr = strchr(m_path + 7, '/');
      file = (ptr ? ptr : "");
   }
   else
   {
      iRet = sscanf(m_szProxyHost, " http://%[^:/]:%d", szHostName, &iPort);
      if (iRet < 1)
      {
         ReportError("Bad Proxy URL format. URL format: http:"
                     "//<host name>:[port]. Please check your proxy settings "
                     "in the Options.");

         m_pContext->log->Error("Debug: m_szProxyHost: '%s'\n", m_szProxyHost);
         return (Error) httpError_BadUrl;
      }
      file = string(m_path);
   }
   EncodeURI(file);

   if (iRet < 2)
      iPort = iHttpPort;

   memset(&sAddr, 0, sizeof(struct sockaddr_in));

   ReportStatus("Looking up host %s...", szHostName);

   eRet = GetHostByName(szHostName, &sHost);
   if (eRet != kError_NoErr)
   {
      sprintf(m_szError, "Cannot find host %s\n", szHostName);
      ReportError(m_szError);
      return (Error) httpError_CustomError;
   }

   memcpy((char *) &sAddr.sin_addr, sHost.h_addr, sHost.h_length);
   sAddr.sin_family = sHost.h_addrtype;
   sAddr.sin_port = htons((unsigned short) iPort);

   ReportStatus("Contacting host %s...", szHostName);
   m_hHandle = socket(sHost.h_addrtype, SOCK_STREAM, 0);
   if (m_hHandle < 0)
   {
      ReportError
         ("Cannot create socket. Is TCP/IP networking properly installed?");
      return (Error) httpError_CannotOpenSocket;
   }

   m_pContext->prefs->GetPrefBoolean(kUseAlternateNICPref, &bUseAltNIC);
   if (bUseAltNIC)
   {
      uint32    len = 100;

      m_pContext->prefs->GetPrefString(kAlternateNICAddressPref, szSourceAddr,
                                       &len);
      if (len == 0)
         m_pContext->log->Error("UseAlternateNIC is true but AlternateNIC "
                                "has no value ?!");

      sSourceAddr.sin_family = AF_INET;
      sSourceAddr.sin_port = 0;
      sSourceAddr.sin_addr.s_addr = inet_addr(szSourceAddr);
      iRet = bind(m_hHandle, (struct sockaddr *) &sSourceAddr,

                  sizeof(struct sockaddr_in));
      if (iRet < 0)
      {
         close(m_hHandle);
         m_hHandle = -1;
         ReportError
            ("Cannot bind the socket. Please make sure that your TCP/IP networking is correctly configured.");
         return kError_CannotBind;
      }
   }

#if defined(WIN32)
   unsigned long lMicrosoftSucksBalls = 1;

   ioctlsocket(m_hHandle, FIONBIO, &lMicrosoftSucksBalls);
#elif defined(__BEOS__)
//  int on = 1;
//  setsockopt( m_hHandle, SOL_SOCKET, SO_NONBLOCK, &on, sizeof( on ) );
#else
   fcntl(m_hHandle, F_SETFL, fcntl(m_hHandle, F_GETFL) | O_NONBLOCK);
#endif

   iConnect = connect(m_hHandle, (const sockaddr *) &sAddr, sizeof(sAddr));

   if (iConnect == -1)
   {

#ifndef WIN32
      int       error = errno;

      if (error != EINPROGRESS)
#else
      int       error = WSAGetLastError();

      if (error != WSAEINPROGRESS && error != WSAEWOULDBLOCK)
#endif
      {
         ReportError("Cannot connect to host: %s", szHostName);
         closesocket(m_hHandle);
         return (Error) httpError_CannotConnect;
      }

      int       conattempt = 0;

      for (; iConnect && !m_bExit;)
      {
         if (conattempt > 50)
         {
            cout << "timed out\n";
            ReportError("Host not answering: %s", szHostName);
            closesocket(m_hHandle);
            return (Error) httpError_CannotConnect;
         }

         conattempt++;
         sTv.tv_sec = 0;
         sTv.tv_usec = 0;
         FD_ZERO(&sSet);
         FD_SET(m_hHandle, &sSet);
         iRet = select(m_hHandle + 1, NULL, &sSet, NULL, &sTv);
         if (!iRet)
         {
            usleep(100000);
            continue;
         }

         if (iRet < 0)
         {
            ReportError("Cannot connect to host: %s", szHostName);
            closesocket(m_hHandle);
            return (Error) httpError_CannotConnect;
         }
         break;
      }
   }
   if (m_bExit)
      return (Error) kError_Interrupt;

   szQuery = new char[iMaxUrlLen];

   if (file.length() > 0)
   {
      sprintf(szQuery, "GET %s HTTP/1.0\r\n"
              "Host: %s\r\n"
              "Accept: */*\r\n"
              "Icy-MetaData:1\r\n"
              "User-Agent: Zinf/%s\r\n",
              file.c_str(), szHostName, ZINF_VERSION);
   }
   else
   {
      sprintf(szQuery, "GET / HTTP/1.0\r\n"
              "Host: %s\r\n"
              "Accept: */*\r\n"
              "Icy-MetaData:1\r\n"
              "User-Agent: Zinf/%s\r\n", szHostName, ZINF_VERSION);
   }

   m_pContext->prefs->GetPrefBoolean(kUseTitleStreamingPref,
                                     &bUseTitleStreaming);
   if (bUseTitleStreaming)
   {
      int       iPort;
      Error     eRet;

      m_pTitleStream = new TitleStreamServer(m_pContext, m_pTarget);

      eRet = m_pTitleStream->Init(iPort);
      if (IsntError(eRet))
      {
         sprintf(szQuery + strlen(szQuery), "x-audiocast-udpport: %d\r\n",
                 iPort);
      }
      else
      {
         delete    m_pTitleStream;

         m_pTitleStream = NULL;
      }
   }

   strcat(szQuery, "\r\n");

   ReportStatus("Requesting stream...");

   iRet = send(m_hHandle, szQuery, strlen(szQuery), 0);
   if (iRet != (int) strlen(szQuery))
   {
      delete [] szQuery;

      ReportError("Cannot send data to host: %s", szHostName);
      closesocket(m_hHandle);
      return (Error) httpError_SocketWrite;
   }
   delete [] szQuery;

   pInitialBuffer = new char[iInitialBufferSize + 1];

   for (; !m_bExit;)
   {
      sTv.tv_sec = 0;
      sTv.tv_usec = 0;
      FD_ZERO(&sSet);
      FD_SET(m_hHandle, &sSet);
      iRet = select(m_hHandle + 1, &sSet, NULL, NULL, &sTv);
      if (!iRet)
      {
         usleep(10000);
         continue;
      }
      iRead = recv(m_hHandle, pInitialBuffer, iInitialBufferSize, 0);
      if (iRead < 0)
      {
         ReportError("Cannot receive data from host: %s", szHostName);
         closesocket(m_hHandle);
         return (Error) httpError_SocketRead;
      }
      break;
   }
   if (m_bExit)
      return (Error) kError_Interrupt;

   if (sscanf(pInitialBuffer, " %*s %d %255[^\n\r]", &iRet, m_szError))
   {
      void     *pData;

      if (iRet == iICY_REDIRECT)
      {
         char *redir;

         redir = strstr(pInitialBuffer, "Location: ");
         if (redir)
         {
             char url[MAX_PATH];

             int ret = sscanf(redir, "Location: %255[^\r\n]", url);
             if (ret == 1)
             {
                 delete [] m_path;
                 m_path = new char[strlen(url) + 1];
                 strcpy(m_path, url);

                 sprintf(url, "Redirected to: %s", m_path);
                 ReportStatus(url);

                 delete [] pInitialBuffer;
                 closesocket(m_hHandle);
                 return Open(); 
             }
         } 
      }
      if (iRet != iICY_OK)
      {
         ReportStatus("");
         ReportError("This stream is not available: %s\n", m_szError);

         delete [] pInitialBuffer;

         closesocket(m_hHandle);
         return (Error) httpError_CustomError;
      }

      // for(int i = 0; i < 25; i++)
      // Debug_v("%c - %d", pInitialBuffer[i], pInitialBuffer[i]);

      pHeaderData = new char[iHeaderSize];

      for (;;)
      {
         if (iHeaderBytes + iRead > iCurHeaderSize)
         {
            char     *pNew;

            iCurHeaderSize += iHeaderSize;
            pNew = new char[iCurHeaderSize + 1];

            memset(pNew, 0, iCurHeaderSize + 1);
            memcpy(pNew, pHeaderData, iHeaderBytes);
            delete []  pHeaderData;

            pHeaderData = pNew;
         }

         memcpy(pHeaderData + iHeaderBytes, pInitialBuffer, iRead);
         iHeaderBytes += iRead;

         pEnd = strstr(pHeaderData, "\r\n\r\n");
         if (pEnd)
         {
            *(pEnd + 3) = 0;
            break;
         }
         pEnd = strstr(pHeaderData, "\n\n");
         if (pEnd)
         {
            *(pEnd + 1) = 0;
            break;
         }

         for (; !m_bExit;)
         {
            sTv.tv_sec = 0;
            sTv.tv_usec = 0;
            FD_ZERO(&sSet);
            FD_SET(m_hHandle, &sSet);
            iRet = select(m_hHandle + 1, &sSet, NULL, NULL, &sTv);
            if (!iRet)
            {
               usleep(10000);
               continue;
            }
            iRead = recv(m_hHandle, pInitialBuffer, iInitialBufferSize, 0);
            if (iRead < 0)
            {
               ReportError("Cannot receive data from host: %s", szHostName);
               closesocket(m_hHandle);
               return (Error) httpError_SocketRead;
            }
            break;
         }
         if (m_bExit)
            return (Error) kError_Interrupt;
      }

      pPtr = strstr(pHeaderData, "icy-name");
      if (pPtr)
      {
         pPtr += strlen("icy-name:");
         szStreamName = new char[strlen(pPtr) + 1];

         sscanf(pPtr, " %[^\r\n]", szStreamName);
      }

      pPtr = strstr(pHeaderData, "icy-url");
      if (pPtr)
      {
         pPtr += strlen("icy-url:");
         szStreamUrl = new char[strlen(pPtr) + 1];

         sscanf(pPtr, " %[^\r\n]", szStreamUrl);
      }

      pPtr = strstr(pHeaderData, "icy-metaint");
      if (pPtr)
      {
         pPtr += strlen("icy-metaint:");
         m_iMetaDataInterval = atoi(pPtr);
      }

      // If this is a stream from a web server and not a shout/ice
      // server we don't want to use buffer reduction when the
      // input buffers fill up
      if (strstr(pHeaderData, "Server:") && strstr(pHeaderData, "Date:"))
         m_bUseBufferReduction = false;

      if (szStreamName && strlen(szStreamName))
      {
         PlaylistItemsUpdatedEvent *e;
         PlaylistItem *pItem;

         pItem = m_pContext->plm->GetCurrentItem();
         if (pItem && szStreamName)
         {
            MetaData  oData;

            oData = pItem->GetMetaData();
            oData.SetTitle(szStreamName);
            pItem->SetMetaData(&oData);

            vector < PlaylistItem * >pl_items;
            pl_items.push_back(pItem);

            e = new PlaylistItemsUpdatedEvent(&pl_items, m_pContext->plm);
            m_pTarget->AcceptEvent(e);
         }
         delete [] szStreamUrl;
      }

      pPtr = strstr(pHeaderData, "x-audiocast-udpport:");
      if (pPtr)
      {
         // Debug_v("x-audiocast-udpport: %s", atoi(pPtr));
         if (m_pTitleStream)
            m_pTitleStream->Run(sAddr.sin_addr, atoi(pPtr + 20));
      }

      // Let's save the bytes we've read into the pullbuffer.
      iRead = iHeaderBytes - strlen(pHeaderData) - 1;
      if (iRead > 0)
      {
         m_pOutputBuffer->BeginWrite(pData, iRead);
         memcpy(pData, (char *) pHeaderData + strlen(pHeaderData) + 1, iRead);
         m_pOutputBuffer->EndWrite(iRead);
         m_uBytesReceived = iRead;
      }
   }

   delete [] pInitialBuffer;

   bool      bSave;
   uint32    size = 255;

   m_pContext->prefs->GetPrefBoolean(kSaveStreamsPref, &bSave);
   if (bSave || (m_pContext->argFlags & FAC_ARGFLAGS_SAVE_STREAMS))
   {
      char      szPath[255], szFile[255];
      unsigned  i;

      if (szStreamName == NULL)
      {
         szStreamName = new char[255];

         sprintf(szStreamName, "%s:%d", szHostName, iPort);
      }

      for (i = 0; i < strlen(szStreamName); i++)
         if (strchr("\\/?*{}[]()*|:<>\"'", szStreamName[i]))
            szStreamName[i] = '-';

      if (m_pContext->prefs->
          GetPrefString(kSaveStreamsDirPref, szPath,
                        &size) == kError_NoPrefValue)
         strcpy(szPath, ".");
      if (szPath[strlen(szPath) - 1] == cDirSepChar)
         szPath[strlen(szPath) - 1] = 0;

      for (i = 0;; i++)
      {
         if (!i)
            sprintf(szFile, "%s%c%s.mp3", szPath, cDirSepChar, szStreamName);
         else
            sprintf(szFile, "%s%c%s-%d.mp3", szPath, cDirSepChar, szStreamName,
                    i);

         if (access(szFile, F_OK))
            break;
      }

      m_fpSave = fopen(szFile, "wb");
      if (m_fpSave == NULL)
         ReportError("Cannot open file to save HTTP stream. Check "
                     "the save streams locally setting in the "
                     "options dialog.");

      if (pHeaderData && m_fpSave && iRead > 0)
      {
         iRet = fwrite((char *) pHeaderData + strlen(pHeaderData) + 1,
                       sizeof(char), iRead, m_fpSave);

         if (iRet != iRead)
         {
            delete [] pHeaderData;

            ReportError("Cannot save http stream to disk. Disk full? (1)");
            return kError_WriteFile;
         }
      }
   }
   delete [] pHeaderData;
   delete [] szStreamName;

   return kError_NoErr;
}

void
HttpInput::StartWorkerThread(void *pVoidBuffer)
{
   ((HttpInput *) pVoidBuffer)->WorkerThread();
}

void
HttpInput::WorkerThread(void)
{
   int       iRead, iRet, iReadSize = 1024, iMaxReadBytes;
   void     *pBuffer;
   Error     eError;
   fd_set    sSet;
   struct timeval sTv;
   char      cNumBlocks;
   static int iSize = 0;

   eError = Open();
   if (IsError(eError) || m_bExit)
   {
      return;
   }

   m_pSleepSem->Wait();

   for (; !m_bExit;)
   {
      if (m_pOutputBuffer->IsEndOfStream())
      {
         m_pSleepSem->Wait();
         continue;
      }

      sTv.tv_sec = 0;
      sTv.tv_usec = 0;
      FD_ZERO(&sSet);
      FD_SET(m_hHandle, &sSet);
      iRet = select(m_hHandle + 1, &sSet, NULL, NULL, &sTv);
      if (!iRet)
      {
         usleep(10000);
         continue;
      }

      eError = m_pOutputBuffer->BeginWrite(pBuffer, iReadSize);
      if (eError == kError_NoErr)
      {
         if (m_iMetaDataInterval > 0)
         {
            iMaxReadBytes =
               min(iReadSize, m_iMetaDataInterval - m_uBytesReceived);
            if (iMaxReadBytes == 0)
            {
               iRead = recv(m_hHandle, &cNumBlocks, 1, 0);
               if (cNumBlocks > 0)
               {
                  char     *pMeta, *pPtr, *pNull;

                  pMeta = new char[cNumBlocks * 16 + 1];

                  recv(m_hHandle, pMeta, cNumBlocks * 16, 0);
                  pMeta[cNumBlocks * 16] = 0;

                  pPtr = strstr(pMeta, "StreamTitle=");
                  if (pPtr)
                  {
                     pPtr += strlen("StreamTitle='");
                     pNull = strchr(pPtr, '\'');
                     if (pNull)
                     {
                        *pNull = 0;
                        m_pTarget->AcceptEvent(new StreamInfoEvent(pPtr, NULL));
                     }

                  }
                  delete [] pMeta;
               }

               iMaxReadBytes = iReadSize;
               m_uBytesReceived = 0;
            }
         }
         else
            iMaxReadBytes = iReadSize;

         iRead = recv(m_hHandle, (char *) pBuffer, iMaxReadBytes, 0);
         if (iRead == 0)
         {
            m_pOutputBuffer->SetEndOfStream(true);
            m_pOutputBuffer->EndWrite(0);
         }
         if (iRead < 0)
         {
#ifdef WIN32
            if (WSAGetLastError() == WSAEWOULDBLOCK)
#else
            if (errno == EAGAIN)
#endif
            {
               iRead = 0;
            }
         }
         if (iRead < 0)
         {
            m_pOutputBuffer->SetEndOfStream(true);
            m_pOutputBuffer->EndWrite(0);
            break;
         }
         iSize += iRead;
         m_uBytesReceived += iRead;

         if (m_fpSave)
         {
            iRet = fwrite(pBuffer, sizeof(char), iRead, m_fpSave);

            if (iRet != iRead)
            {
               ReportError("Cannot save http stream to disk. Disk full? (2)");
               break;
            }
         }

         eError = m_pOutputBuffer->EndWrite(iRead);
         if (IsError(eError))
         {
            m_pContext->log->Error("http: EndWrite returned: %d\n", eError);
            break;
         }
      }
      if (eError == kError_BufferTooSmall)
      {
         if (m_bLoop)
         {
            m_pOutputBuffer->DiscardBytes();
            m_bDiscarded = true;
         }
         else
         {
            iSize = 0;
            m_pSleepSem->Wait();
         }
         continue;
      }
   }
   shutdown(m_hHandle, 2);
   closesocket(m_hHandle);
   m_hHandle = -1;
}

vector < string > *HttpInput::GetProtocols(void)
{
   vector < string > *protoList = new vector < string >;

   protoList->push_back("http");

   return protoList;
}
