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
	
	$Id: downloadmanager.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786) 
#endif

#include "config.h"

#include <assert.h>

#include "downloadmanager.h"
#include "musiccatalog.h"

#if defined(unix) || defined(__BEOS__) || defined(_BSD_SOURCE)
#define SOCKET int
#endif

#if !defined(WIN32)
#include <sys/time.h>
#ifdef HAVE_SSTREAM
   #include <sstream>
#else
   #include <strstream>
   typedef ostrstream ostringstream;
#endif
#else
#include <sstream>
#endif

#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <algorithm>
#include <fstream>

#if defined (unix) || defined(_BSD_SOURCE)
#include <arpa/inet.h>
#define closesocket(x) close(x)
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif

using namespace std;

#include "facontext.h"

#include "errors.h"
#include "registrar.h"
#include "utility.h"
#include "eventdata.h"

#ifdef __QNX__
#include <strings.h>
#endif

DownloadManager::DownloadManager(FAContext* context)
{
    m_context = context;
    m_current = 0;
    m_downloadsPaused = true;
	m_downloadIndex = -1;
    m_exit = false;
 
    Registrar registrar;

    registrar.SetSubDir("plugins");
    registrar.SetSearchString("*.dlf");
    registrar.InitializeRegistry(&m_formatRegistry, context->prefs);

    const RegistryItem* module = NULL;
    DownloadFormat* dlf = NULL;
    int32 i = 0;

    while((module = m_formatRegistry.GetItem(i++)))
    {
        dlf = (DownloadFormat*) module->InitFunction()(m_context);

        if(dlf)
        {
            DownloadFormatInfo dlfi;

            uint32 index = 0;

            // error != kError_NoMoreFormats
            while(IsntError(dlf->GetSupportedFormats(&dlfi, index++)))
            {
                dlfi.SetRef(dlf);
                m_formats.push_back(new DownloadFormatInfo(dlfi));
            }
        }
    }

    LoadResumableDownloadItems();

    char path[MAX_PATH];
    uint32 length = sizeof(path);

    context->prefs->GetPrefString(kInstallDirPref, path, &length);
    strcat(path, "\\DownloadLog.txt");

    m_runDownloadThread = true;
    m_downloadThread = Thread::CreateThread();

    if(m_downloadThread)
    {
        m_downloadThread->Create(download_thread_function, this, true);
    }
}

DownloadManager::~DownloadManager()
{
    uint32 index = 0;
    uint32 size = 0;
    DownloadItem* item = NULL;

    m_exit = true;

    m_runDownloadThread = false;
    m_queueSemaphore.Signal();

    m_quitMutex.Acquire();

    SaveResumableDownloadItems();

    size = m_itemList.size();

    for(index = 0; index < size; index++)
    {
        item = m_itemList[index];

        if(item)
        {
            if(item->GetState() == kDownloadItemState_Cancelled)
            {
                CleanUpDownload(item);
            }
            delete item;
        }
    }

    size = m_formats.size();

    for(index = 0; index < size; index++)
    {
        delete m_formats[index]->GetRef();
        delete m_formats[index];
    }

    if(m_downloadThread)
    {
        delete m_downloadThread;
    }

    //delete m_debug;
}

// Functions for adding items to Download Manager
// Adding an item implicitly queues it for
// downloading.
Error DownloadManager::AddItem(const char* url, const char* filename)
{
    Error result = kError_InvalidParam;

    assert(url);

    if(url)
    {
        result = kError_OutOfMemory;

        if(!filename)
        {
            filename = strrchr(url, '/');

            if(filename)
                filename++;
            else
                filename = url;
        }

        DownloadItem* item = new DownloadItem(url, filename);

        if(item)
        {
            item->SetNormalDownload();
            result = AddItem(item);
        }
    }

    return result;
}

Error DownloadManager::AddItem(DownloadItem* item)
{
    Error result = kError_InvalidParam;
    vector<DownloadItem*>::iterator j;

    m_mutex.Acquire();

    assert(item);

    if(item)
    {
        for(j = m_itemList.begin(); j != m_itemList.end(); j++)
           if ((*j)->SourceURL() == item->SourceURL())
               break;

        if (j == m_itemList.end())
        {
            m_itemList.push_back(item);
            SendItemAddedMessage(item);
            QueueDownload(item);
        }
        result = kError_NoErr;
    }

    m_mutex.Release();
    return result;
}

Error DownloadManager::AddItems(vector<DownloadItem*>* list)
{
    Error result = kError_InvalidParam;
    vector<DownloadItem*>::iterator i, j;
    m_mutex.Acquire();

    assert(list);

    if(list)
    {
        for(i = list->begin(); i != list->end(); i++)
        {
            for(j = m_itemList.begin(); j != m_itemList.end(); j++)
                if ((*i)->SourceURL() == (*j)->SourceURL())
                    break;

            if (j == m_itemList.end())
            {
                m_itemList.push_back(*i);
                SendItemAddedMessage(*i);
                QueueDownload(*i);
            }
        }

        result = kError_NoErr;
    }

    m_mutex.Release();

    return result;
}

// Changes item state to queued if it is cancelled or error.
// This will indicate to the download thread that it should
// attempt to retrieve this item. Has no effect if the item's
// state is Done, or Downloading.
Error DownloadManager::QueueDownload(DownloadItem* item,
                                     bool          bDownloadNow)
{
    Error result = kError_InvalidParam;
    unsigned int index;

    assert(item);

    if(item)
    {
        if(item->GetState() != kDownloadItemState_Downloading &&
           item->GetState() != kDownloadItemState_Done)
        {
            item->SetState(kDownloadItemState_Queued);
            SendStateChangedMessage(item);

            if (m_downloadsPaused && bDownloadNow)
               for(index = 0; index < m_itemList.size(); index++)
                   if (m_itemList[index] == item)
                   {
                      m_downloadIndex = index;
                      break;
                   }

            m_queueSemaphore.Signal();

            result = kError_NoErr;
        }
    }

    return result;
}

Error DownloadManager::QueueDownload(uint32 index)
{
    return QueueDownload(ItemAt(index));
}


// Changes item state to cancelled if it is queued or downloading.
// If allowResume is true then data is retained for later download.
// Has no effect if the item's state is Done, Cancelled, or Error.
Error DownloadManager::CancelDownload(DownloadItem* item, bool allowResume)
{
    Error result = kError_InvalidParam;

    assert(item);

    if(item)
    {
        if(item->GetState() == kDownloadItemState_Downloading ||
           item->GetState() == kDownloadItemState_Queued ||
           item->GetState() == kDownloadItemState_Paused)
        {
            if(!allowResume)
            {
                item->SetState(kDownloadItemState_Cancelled);
            }
            else
            {
                item->SetState(kDownloadItemState_Paused);
            }

            SendStateChangedMessage(item);

            result = kError_NoErr;
        }
    }

    return result;
}

Error DownloadManager::CancelDownload(uint32 index, bool allowResume)
{
    return CancelDownload(ItemAt(index), allowResume);
}


// File Format support
Error DownloadManager::GetSupportedDownloadFormats(DownloadFormatInfo* format,
                                                   uint32 index)
{
    Error result = kError_InvalidParam;

    assert(format);

    if(format)
    {
        result = kError_NoMoreFormats;

        uint32 numFormats = m_formats.size();

        if(index < numFormats)
        {
            result = kError_NoErr;

            *format = *m_formats[index];
        }
    }

    return result;
}

Error DownloadManager::ReadDownloadFile(char* url, 
                                        vector<DownloadItem*>* items)
{
    Error result = kError_InvalidParam;

    assert(url);

    if(url)
    {
        // find a suitable plugin
        result = kError_FormatNotSupported;
        char* extension = strrchr(url, '.');

        if(extension)
        {
            extension++;

            uint32 numFormats = m_formats.size();

            for(uint32 index = 0; index < numFormats; index++)
            {
                DownloadFormatInfo* format = m_formats[index];
                
                if(!strcmp(extension, format->GetExtension()))
                {
                    bool addToInternalList = false;

                    if(!items)
                    {
                        items = new vector<DownloadItem*>;
                        addToInternalList = true;
                    }

                    result = format->GetRef()->ReadDownloadFile(url, 
                                                                items);
                    if(addToInternalList)
                    {
                        AddItems(items);
                        delete items;
                    }

                    break;
                }
            }
        }
        
        //char   path[255];
        //uint32 length = sizeof(path);
        //URLToFilePath(url, path, &length);
        //unlink(path);
    }

    return result;
}

// Utility Functions
bool DownloadManager::IsEmpty()
{
    bool result;
    m_mutex.Acquire();

    result = m_itemList.empty();

    m_mutex.Release();
    return result;
}

uint32 DownloadManager::CountItems()
{
    uint32 result;
    m_mutex.Acquire();

    result = m_itemList.size();

    m_mutex.Release();
    return result;
}

DownloadItem* DownloadManager::ItemAt(uint32 index)
{
    DownloadItem* result = NULL;
    m_mutex.Acquire();
    
    index = CheckIndex(index);

    if(index != kInvalidIndex)
    {
        result = m_itemList[index];
    }
    
    m_mutex.Release();
    return result;
}

uint32 DownloadManager::IndexOf(DownloadItem* item)
{
    return InternalIndexOf(&m_itemList, item);
}

uint32 DownloadManager::InternalIndexOf(vector<DownloadItem*>* list,
                                        DownloadItem* item)
{
    uint32 result = kInvalidIndex;
    uint32 index = 0;
    uint32 size = 0;

    assert(list);
    assert(item);

    if(list && item)
    {
        size = list->size();

        for(index = 0; index < size; index++)
        {
            if(item == (*list)[index])
            {
                result = index;
                break;
            }
        }
    }
    
    return result;
}

bool DownloadManager::HasItem(DownloadItem* item)
{
    return (IndexOf(item) != kInvalidIndex);
}

// Internal functions

inline uint32 DownloadManager::CheckIndex(uint32 index)
{
	// If we're dealing with a bogus index then set it to -1.
	if(index >= CountItems())
    {
		index = kInvalidIndex;
    }

	return index;
}

DownloadItem* DownloadManager::GetNextQueuedItem()
{
    int           i, total = 0;

    if (m_downloadIndex < 0 || m_downloadIndex >= (int)m_itemList.size())
        m_downloadIndex = 0;

    for(i = m_downloadIndex, total = 0; i < (int)m_itemList.size(); 
        i = (i + 1) % m_itemList.size(), total++)
    {
        if (total >= (int)m_itemList.size())
            return NULL;
           
        if (m_itemList[i]->GetState() == kDownloadItemState_Queued)
        {
            m_downloadIndex++;
            return m_itemList[i];
        }
    }
    
    return NULL;
}

static int32 GetContentLengthFromHeader(const char* buffer)
{
    int32 result = -1;

    char* cp = strstr(buffer, "Content-Length:");

    if(cp)
    {
        cp += strlen("Content-Length:") + 1;

        result = atoi(cp);
    }

    return result;
}

static void GetContentTimeFromHeader(const char* buffer, string &mTime)
{
    char* cp = strstr(buffer, "Last-Modified:");
    if(cp)
    {
        string::size_type pos;
        
        cp += strlen("Last-Modified:") + 1;
        mTime = string(cp);

        pos = mTime.find(string("\r"), 0);
        if (pos != string::npos)
           mTime.erase(pos, mTime.length() - 2);

        pos = mTime.find(string("\n"), 0);
        if (pos != string::npos)
           mTime.erase(pos, mTime.length() - 2);
    }
    else
        mTime = string("");
}


const uint8 kHttpPort = 80;
const uint32 kMaxHostNameLen = 64;
const uint32 kMaxURLLen = 1024;

static bool IsHTTPHeaderComplete(char* buffer, uint32 length)
{
    bool result = false;

    //if(length >= 4)
    //{
        //if( (buffer[0] == 'H' && buffer[1] == 'T' 
        //       && buffer[2] == 'T' && buffer[3] == 'P'))
        //{
            //cout << "buffer is HTTP" << endl;

            for(char* cp = buffer; cp < buffer + length; cp++)
            {
                if(!strncmp(cp, "\n\n", 2) || !strncmp(cp, "\r\n\r\n", 4))
                {
                    result = true;
                    break;
                }
            }
        //}
    //}

    return result;
}

#ifdef WIN32
#define WM_WINDOWS_IS_DONE_PICKING_ITS_BUTT (WM_USER + 1)

static LRESULT WINAPI WndProc(HWND hwnd, UINT msg,
                              WPARAM wParam, LPARAM lParam);

Error
DownloadManager::Win32GetHostByName(char *szHostName, struct hostent *pHostInfo)
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
   wc.hInstance = m_context->hInstance;
   wc.hCursor = NULL;
   wc.hIcon = NULL;
   wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
   wc.lpszClassName = "WindowsSucks";

   result = RegisterClass(&wc);

   hWnd = CreateWindow(wc.lpszClassName, "Fuss",
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                       CW_USEDEFAULT, NULL, NULL, m_context->hInstance, NULL);
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

   if (m_exit)
   {
      WSACancelAsyncRequest(hHandle);
   }

   m_hWnd = NULL;
   DestroyWindow(hWnd);

   if (m_exit)
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

Error DownloadManager::Download(DownloadItem* item)
{
    Error result = kError_InvalidParam;

    assert(item);
    if(item)
    {
        char hostname[kMaxHostNameLen + 1] = {0x00};
        char proxyname[kMaxURLLen + 1] = {0x00};
        unsigned short port;
        struct sockaddr_in  addr;
        struct hostent      host;
        SOCKET s = -1;
        char* file = NULL;
        char* destPath = NULL;
        bool useProxy;
        struct stat st;

        destPath = new char[_MAX_PATH];
        uint32 length = _MAX_PATH;

        m_context->prefs->GetPrefString(kSaveMusicDirPref, destPath, &length);

        strcat(destPath, DIR_MARKER_STR);
        strcat(destPath, item->DestinationFile().c_str());
        
        if(-1 != stat(destPath, &st))
        {
            if ((unsigned int)st.st_size >= item->GetTotalBytes())
            {
               if(destPath)
                   delete [] destPath;
               return kError_NoErr;    
            }
        }

        result = kError_ProtocolNotSupported;

        // where should we connect to?
        if(!strncasecmp(item->SourceURL().c_str(), "http://", 7))
        {
            int32 numFields;
            uint32 length;

            result = kError_NoErr;  

            m_context->prefs->GetPrefBoolean(kUseProxyPref, &useProxy);

            if(useProxy)
            {
                char proxy[kMaxURLLen + 1] = {0x00};

                length = sizeof(proxy);
                m_context->prefs->GetPrefString(kProxyHostPref, proxy, &length);

                numFields = sscanf(proxy, 
                                   "http://%[^:/]:%hu", proxyname, &port);

                sscanf(item->SourceURL().c_str(), 
                               "http://%[^:/]", hostname);

                //strncpy(proxyname, item->SourceURL().c_str(), kMaxURLLen);
                //proxyname[kMaxURLLen - 1] = 0;
                file = (char*)item->SourceURL().c_str();
            }
            else
            {
                numFields = sscanf(item->SourceURL().c_str(), 
                               "http://%[^:/]:%hu", hostname, &port);

                file = strchr(item->SourceURL().c_str() + 7, '/');
            }

            if(numFields < 1)
            {
                result = kError_InvalidURL;     
            }

            if(numFields < 2)
            {
                port = kHttpPort;
            }            
        }

        if(item->GetState() != kDownloadItemState_Downloading) 
            result = kError_UserCancel;

        // get hostname
        if(IsntError(result))
        {
            char* server = (*proxyname ? proxyname : hostname);
            struct hostent* hostByName;
            struct hostent  hostByIP;

            //*m_debug << "gethostbyname: " << hostname << endl;
            hostByName = gethostbyname(server);
#ifdef WIN32
            Error     eRet;
            struct hostent TempHostent;
         
            eRet = Win32GetHostByName(server, &TempHostent);
            if (eRet == kError_Interrupt)
               return eRet;
         
            if (IsError(eRet))
               hostByName = NULL;
            else
               hostByName = &TempHostent;
#else
            hostByName = gethostbyname(server);
#endif

            // On some stacks a numeric IP address
            // will not parse with gethostbyname.  
            // If that didn't work try to convert it as a
            // numeric address before giving up.
            if(!hostByName)
            {
                static unsigned long ip;
                static char *addr_ptr[2] = {(char*)&ip, NULL};

                if((int)(ip = inet_addr(server)) < 0) 
                    result =  kError_CantFindHost;
                else
                {
                    hostByIP.h_length = sizeof(uint32);
                    hostByIP.h_addrtype = AF_INET;
                    hostByIP.h_addr_list = (char**)&addr_ptr;
                    hostByName = &hostByIP;
                }
            }

            if(IsntError(result))
            {
                memcpy(&host, hostByName, sizeof(struct hostent));
            }

            if(item->GetState() != kDownloadItemState_Downloading) 
                result = kError_UserCancel;
        }

        // open socket
        if(IsntError(result))
        {
            memset(&addr, 0x00, sizeof(struct sockaddr_in));
            memcpy(&addr.sin_addr, host.h_addr, host.h_length);
            addr.sin_family= host.h_addrtype;
            addr.sin_port= htons(port); 

            //*m_debug << "socket" << endl;

            s = socket(host.h_addrtype, SOCK_STREAM, 0);

            if(s < 0)
                result = kError_CantCreateSocket;

            if(item->GetState() != kDownloadItemState_Downloading)
                result = kError_UserCancel;
        }

        // connect and send request
        if(IsntError(result))
        {
            Error err;
            int   ret;
            
            err = Connect(s, (const struct sockaddr*)&addr, ret, item);
            if (IsError(err))
                result = kError_UserCancel;
                
            if (ret < 0)    
                result = kError_ConnectFailed;

            if(item->GetState() != kDownloadItemState_Downloading)
                result = kError_UserCancel;

            if(IsntError(result))
            {
                const char* kHTTPQuery = "GET %s HTTP/1.1\r\n"
                                         "Host: %s\r\n"
                                         "Accept: */*\r\n" 
                                         "User-Agent: FreeAmp/%s\r\n";

                const char* kRange = "Range: bytes=%lu-\r\n";
                const char* kIfRange = "If-Range: %s\r\n";

                const char* kCookie = "Cookie: %s\r\n";

                // the magic 256 is enough for a time field that
                // we got from the server
                char* query = new char[ strlen(kHTTPQuery) + 
                                        strlen(file) +
                                        strlen(hostname) +
                                        strlen(ZINF_VERSION)+
                                        (item->GetBytesReceived() ? (strlen(kRange) + 256): 0 ) +
                                        (item->SourceCookie().size() ? (item->SourceCookie().size() + strlen(kCookie)): 0) +
                                        2];
            
                sprintf(query, kHTTPQuery, file, hostname, ZINF_VERSION);

                // do we need to request a range?
                if(item->GetBytesReceived())
                { 
                    if(-1 != stat(destPath, &st))
                    {
                        char* range = new char[strlen(kRange) + 256 + 1];

                        sprintf(range, kRange, item->GetBytesReceived());
                        strcat(query, range);

                        if (item->MTime().length() > 0)
                        {
                            sprintf(range, kIfRange, item->MTime().c_str());
                            strcat(query, range);
                        }
                        delete [] range;
                    }
                    else
                    {
                        item->SetBytesReceived(0);
                        item->SetMTime("");
                    }
                }

                if(item->SourceCookie().size())
                {
                    char* cookie = new char[strlen(kCookie) + item->SourceCookie().size() + 1];

                    sprintf(cookie, kCookie, item->SourceCookie().c_str());

                    strcat(query, cookie);

                    delete [] cookie;
                }
            
                strcat(query, "\n");
                
                int count;

                err = Send(s, query, strlen(query), 0, count, item);
                if (IsError(err))
                    result = kError_UserCancel; 
                if(count != (int)strlen(query))
                {
                    result = kError_IOError;
                }

                if(item->GetState() != kDownloadItemState_Downloading)
                    result = kError_UserCancel;

                delete [] query;
            }
        }

        // receive response
        if(IsntError(result))
        {
            uint32 bufferSize = 2048;
            char* buffer = NULL;
            int count;
            uint32 total = 0;

            buffer = (char*)malloc(bufferSize);

            result = kError_OutOfMemory;

            if(buffer)
            {
                Error err;
                result = kError_NoErr;

                do
                {
                    if(total >= bufferSize - 1)
                    {
                        bufferSize *= 2;

                        buffer = (char*) realloc(buffer, bufferSize);

                        if(!buffer)
                        {
                            result = kError_OutOfMemory;
                            break;
                        }
                    }

                    err = Recv(s, buffer + total, bufferSize - total - 1, 0, count, item);
                    if (IsError(err))
                        result = kError_UserCancel;

                    if(count > 0)
                        total += count;
                    else
                    {
                        result = kError_IOError;
                    }

                    if(item->GetState() != kDownloadItemState_Downloading)
                        result = kError_UserCancel;


                }while(IsntError(result) && !IsHTTPHeaderComplete(buffer, total));
            }

            // parse header
            if(IsntError(result))
            {
                uint32 returnCode = atoi(buffer+9);
                buffer[total] = 0x00;
                //cout << buffer << endl;

                //cout << returnCode << endl;

                switch(buffer[9])
                {
                    // 1xx: Informational - Request received, continuing process
                    case '1':
                    {
                        // not sure what to do here... continue receiving???
                    }    

                    // 2xx: Success - The action was successfully received,
                    // understood, and accepted
                    case '2':
                    {
                        result = kError_UnknownErr;

                        int32 fileSize = GetContentLengthFromHeader(buffer);
                
                        //cout << destPath << endl;

                        int openFlags = O_BINARY|O_CREAT|O_RDWR|O_APPEND;

                        if(returnCode != 206) // server didn't ok partial
                        {
                            string mTime;

                            item->SetBytesReceived(0);
                            openFlags |= O_TRUNC;
                            
                            GetContentTimeFromHeader(buffer, mTime);
                            item->SetMTime(mTime.c_str());
                            if(fileSize > 0)
                               item->SetTotalBytes(fileSize);
                        }

                        int fd = open(destPath, openFlags, S_IREAD | S_IWRITE);

                        if(fd >= 0)
                        {
                            Error err;

                            lseek(fd, 0, SEEK_END);

                            result = kError_NoErr;
                            int wcount = 0;

                            char* cp = strstr(buffer, "\n\n");

                            if(cp)
                                cp += 2;
                            else
                            {
                                cp = strstr(buffer, "\r\n\r\n");

                                if(cp)
                                    cp += 4;
                            }

                            if(cp)
                            {
                                if(cp - buffer < (int)total)
                                {
                                    wcount = write(fd, cp, total - (cp - buffer));
                                    item->SetBytesReceived(total - (cp - buffer) + item->GetBytesReceived());
                                    SendProgressMessage(item);
                                }
                            }

                            do
                            {
                            
                                err = Recv(s, buffer, bufferSize, 0, count, item);
                                if (IsError(err))
                                    result = kError_UserCancel;
                                else 
                                   if(count > 0)
                                   {
                                       wcount = write(fd, buffer, count);
                                       item->SetBytesReceived(count + item->GetBytesReceived());
                                       SendProgressMessage(item);
                                   }

                                if(count < 0) 
                                    result = kError_IOError;
                                
                                if(wcount < 0)
                                    result = kError_WriteFile;

                                if(item->GetState() != kDownloadItemState_Downloading)
                                    result = kError_UserCancel;

                            }while(count > 0 && IsntError(result) && 
                                   m_runDownloadThread && wcount >= 0 && 
                                   (item->GetTotalBytes() > item->GetBytesReceived()));
                            close(fd);                           
                        }
                        else
                        {
                            //*m_debug << "error opening  file: " <<  endl;

                            switch(errno)
                            {
                                case EEXIST:
                                    result = kError_FileExists;
                                    break;

                                case EACCES:
                                    result = kError_FileNoAccess;
                                    break;

                                case ENOENT:
                                    result = kError_FileNotFound;
                                    break;

                                case EMFILE:
                                    result = kError_FileNoHandles;
                                    break;

                                case EINVAL:
                                    result = kError_FileInvalidArg;
                                    break;
                
                            }
                        }
                        
                        break;
                    }

                    // 3xx: Redirection - Further action must be taken in order to
                    // complete the request
                    case '3':
                    {
                        char* cp = strstr(buffer, "Location:");
                        //int32 length;

                        if(cp)
                        {
                            cp += 9;

                            if(*cp == 0x20)
                                cp++;

                            char *end;
                            for(end = cp; end < buffer + total; end++)
                                if(*end=='\r' || *end == '\n') break;

                            *end = 0x00;

                            if (305 == returnCode) // proxy
                            {
                                char* proxy = new char[strlen(cp) + 
                                                       strlen(item->SourceURL().c_str()) + 1];

                                sprintf(proxy, "%s%s", cp, item->SourceURL().c_str());

                                item->SetSourceURL(proxy);

                                delete [] proxy;
                            }
                            else // redirect of some type
                            {
                                item->SetSourceURL(cp);
                            }

                            result = Download(item);
                        }
                        
                        break;
                    }

                    // 4xx: Client Error - The request contains bad syntax or cannot
                    // be fulfilled
                    case '4':
                    {
                        //*m_debug << "HTTP Error: " << returnCode << endl;

                        switch(returnCode)
                        {
                            case 400:
                                result = kError_BadHTTPRequest;
                                break;

                            case 401:
                                result = kError_AccessNotAuthorized;
                                break;                           

                            case 403:
                                result = kError_DownloadDenied;
                                break;

                            case 404:
                                result = kError_HTTPFileNotFound;
                                break;

                            case 416:
                                // try to grab the whole thing...
                                item->SetBytesReceived(0);
                                result = Download(item);
                                break;

                            default:
                                result = kError_UnknownErr;
                                break;
                        }

                        break;
                    }

                    // 5xx: Server Error - The server failed to fulfill an apparently
                    // valid request
                    case '5':
                    {
                        result = kError_UnknownServerError;
                        break;
                    }
                }

            }

            // cleanup
            if(buffer)
                free(buffer);            
        }

        // cleanup
        if(s > 0)
            closesocket(s);

        if(destPath)
            delete [] destPath;
    }

    return result;
}

void DownloadManager::CleanUpDownload(DownloadItem* item)
{
    //cout << "Cleaning item: " << item->SourceURL() << endl;

    char path[_MAX_PATH];
    uint32 length = sizeof(path);

    m_context->prefs->GetPrefString(kSaveMusicDirPref, path, &length);

    strcat(path, DIR_MARKER_STR);
    strcat(path, item->DestinationFile().c_str());

    remove(path);

    item->SetBytesReceived(0);
}

Error DownloadManager::SubmitToDatabase(DownloadItem* item)
{
    Error result = kError_InvalidParam;

    assert(item);

    if(item)
    {
        //cout << "Submitting item: " << item->SourceURL() << endl;

        char path[_MAX_PATH];
        uint32 length = sizeof(path);

        m_context->prefs->GetPrefString(kSaveMusicDirPref, path, &length);

        strcat(path, DIR_MARKER_STR);
        strcat(path, item->DestinationFile().c_str());

        uint32 urlLength = strlen(path) + 10;
        char *url = new char[urlLength];
        
        if (IsntError(FilePathToURL(path, url, &urlLength)))
        {
            PlaylistItem *newtrack;
            
            if (item->IsNormalDownload())
               newtrack = new PlaylistItem(url);
            else   
               newtrack = new PlaylistItem(url, &(item->GetMetaData()));
               
            m_context->plm->RetrieveMetaDataNow(newtrack);

            MetaData tempdata = (MetaData)(newtrack->GetMetaData());
            m_context->catalog->WriteMetaDataToDatabase(url, tempdata);
            m_context->catalog->AddSong(url);

            delete newtrack;
        }

        delete [] url;
    }

    return result;
}

void DownloadManager::PauseDownloads(void)
{ 
    m_downloadsPaused = true;
}

void DownloadManager::ResumeDownloads(void)
{ 
    m_downloadsPaused = false;
    m_queueSemaphore.Signal();
}

bool DownloadManager::IsPaused(void)
{
    return m_downloadsPaused;
}

void DownloadManager::DownloadThreadFunction()
{
    DownloadItem* item;
    Error         result;
    
    m_quitMutex.Acquire();

    while(m_runDownloadThread)
    {
        if (m_downloadsPaused)
        {
            m_queueSemaphore.Wait();
            continue;
        }
    
        item = GetNextQueuedItem();
        if(item)
        {
            item->SetState(kDownloadItemState_Downloading);
            SendStateChangedMessage(item);

            result = Download(item);

            if(IsntError(result))
            {
                item->SetState(kDownloadItemState_Done);

                result = SubmitToDatabase(item);
            }
            else if(result == kError_UserCancel)
            {
                if(item->GetState() == kDownloadItemState_Cancelled)
                {
                    CleanUpDownload(item);
                }
            }
            else
            {
                item->SetState(kDownloadItemState_Error);
                if (result != kError_IOError)
                    CleanUpDownload(item);
            }

            item->SetDownloadError(result);

            SendStateChangedMessage(item);
        }
        else
        {
            m_queueSemaphore.Wait();
        }    
    }

    m_quitMutex.Release();
}

void DownloadManager::download_thread_function(void* arg)
{
    DownloadManager* dlm = (DownloadManager*)arg;

    dlm->DownloadThreadFunction();
}

// Messaging functions

void DownloadManager::SendItemAddedMessage(DownloadItem* item)
{    
    m_context->target->AcceptEvent(new DownloadItemAddedEvent(item));
}

void DownloadManager::SendItemRemovedMessage(DownloadItem* item)
{    
    m_context->target->AcceptEvent(new DownloadItemRemovedEvent(item));
}

void DownloadManager::SendStateChangedMessage(DownloadItem* item)
{    
    m_context->target->AcceptEvent(new DownloadItemNewStateEvent(item));
}

void DownloadManager::SendProgressMessage(DownloadItem* item)
{    
    m_context->target->AcceptEvent(new DownloadItemProgressEvent(item));
}

void DownloadManager::SaveResumableDownloadItems()
{
    char path[_MAX_PATH];
    uint32 length = sizeof(path);

    m_context->prefs->GetPrefString(kDatabaseDirPref, path, &length);

    if(DoesDBDirExist(path))
    {
        strcat(path, DIR_MARKER_STR);
        strcat(path, "ResumeDownloadDB");

        Database database(path);  

        if(database.Working())
        {
            uint32 size = 0;
            DownloadItem* item = NULL;

            size = m_itemList.size();

            for(uint32 index = 0; index < size; index++)
            {
                item = m_itemList[index];

                if(item && 
                  (item->GetState() == kDownloadItemState_Paused ||
                   item->GetState() == kDownloadItemState_Queued ||
                   item->GetState() == kDownloadItemState_Error))
                {
                    ostringstream ost;
                    char num[256];
                    const char* kDatabaseDelimiter = " ";
                    MetaData metadata = item->GetMetaData();

                    if (item->GetState() == kDownloadItemState_Error)
                       item->SetState(kDownloadItemState_Queued);

                    // write out the number of elements we have
                    ost << 16 << kDatabaseDelimiter;
                    // next record the length of each element
                    ost << item->SourceURL().size() << kDatabaseDelimiter;
                    ost << item->SourceCookie().size() << kDatabaseDelimiter;
                    ost << item->DestinationFile().size() << kDatabaseDelimiter;
                    ost << item->PlaylistName().size() << kDatabaseDelimiter;

                    sprintf(num, "%ld", (long int)item->GetTotalBytes());
                    ost << strlen(num) << kDatabaseDelimiter;

                    sprintf(num, "%ld", (long int)item->GetBytesReceived());
                    if (item->MTime().length())
                    {
                        int iTimeSize = strlen(num) + 1 + item->MTime().length();
                        ost << iTimeSize << kDatabaseDelimiter;
                    }
                    else
                        ost << strlen(num) << kDatabaseDelimiter;

                    // metadata lengths
                    ost << metadata.Artist().size() << kDatabaseDelimiter;
                    ost << metadata.Album().size() << kDatabaseDelimiter;
                    ost << metadata.Title().size() << kDatabaseDelimiter;
                    ost << metadata.Comment().size() << kDatabaseDelimiter;
                    ost << metadata.Genre().size() << kDatabaseDelimiter;

                    sprintf(num, "%ld", (long int)metadata.Year());
                    ost << strlen(num) << kDatabaseDelimiter;
                    sprintf(num, "%ld", (long int)metadata.Track());
                    ost << strlen(num) << kDatabaseDelimiter;
                    sprintf(num, "%ld", (long int)metadata.Time());
                    ost << strlen(num) << kDatabaseDelimiter;
                    sprintf(num, "%ld", (long int)metadata.Size());
                    ost << strlen(num) << kDatabaseDelimiter;
                    sprintf(num, "%ld", (long int)item->GetState());
                    ost << strlen(num) << kDatabaseDelimiter;

                    // now stuff all the data in there
                    ost << item->SourceURL();
                    ost << item->SourceCookie();
                    ost << item->DestinationFile();
                    ost << item->PlaylistName();
                    ost << item->GetTotalBytes();
                    ost << item->GetBytesReceived();

                    if (item->MTime().length())
                        ost << " " << item->MTime();
                        
                    ost << metadata.Artist();
                    ost << metadata.Album();
                    ost << metadata.Title();
                    ost << metadata.Comment();
                    ost << metadata.Genre();
                    ost << metadata.Year();
                    ost << metadata.Track();
                    ost << metadata.Time();
                    ost << metadata.Size();
                    
                    sprintf(num, "%ld", (long int)item->GetState());
                    ost << num;
                    ost << '\0';     
                    
                    sprintf(num, "%ld", (long int)index);
#if defined(WIN32) || defined(HAVE_SSTREAM) 
                    database.Insert(num, (char*)ost.str().c_str());  
#else
                    database.Insert(num, (char*)ost.str());
#endif
                }
            }
        }
    }
}

void DownloadManager::LoadResumableDownloadItems()
{
    char   path[_MAX_PATH];
    uint32 length = sizeof(path);

    m_context->prefs->GetPrefString(kDatabaseDirPref, path, &length);

    if(DoesDBDirExist(path))
    {
        strcat(path, DIR_MARKER_STR);
        strcat(path, "ResumeDownloadDB");

        Database database(path);  

        if(database.Working())
        {
            char *key = NULL;
            
            while ((key = database.NextKey(key)))
            {
                char* value = database.Value(key);

                if (!value)
                   continue;

                uint32 numFields = 0;
                int offset = 0;
 
                sscanf(value,"%lu%n", (long unsigned int *)&numFields, &offset);
                uint32* fieldLength =  new uint32[numFields];
 
                for(uint32 i = 0; i < numFields; i++)
                {
                   int temp;
 
                   sscanf(value + offset, " %lu %n", 
		          (long unsigned int *)&fieldLength[i], &temp);
                   if (i == numFields - 1) {
                       char intholder[10];
                       sprintf(intholder, "%lu", 
		               (long unsigned int)fieldLength[i]);
                       offset += strlen(intholder) + 1;
                   }
                   else
                       offset += temp;
                }

                string data = string(value);
                data.erase(0, offset);
                delete [] value;
                value = NULL;

                DownloadItem* item = new DownloadItem();
                MetaData metadata;
                uint32 count = 0;

                for(uint32 j = 0; j < numFields; j++)
                {
                    switch(j)
                    {
                        case 0:
                            item->SetSourceURL(data.substr(count, fieldLength[j]));
                            break;
                        case 1:
                            item->SetSourceCookie(data.substr(count, fieldLength[j]));
                            break;
                        case 2:
                            item->SetDestinationFile(data.substr(count, fieldLength[j]));
                            break;
                        case 3:
                            item->SetPlaylistName(data.substr(count, fieldLength[j]));
                            break;
                        case 4:
                            item->SetTotalBytes(atoi(data.substr(count, fieldLength[j]).c_str()));
                            break;
                        case 5:
                        {
                            string            val;
                            string::size_type pos;
                            
                            val = data.substr(count, fieldLength[j]).c_str();
                            item->SetBytesReceived(atoi(val.c_str()));
                            
                            pos = val.find(string(" "), 0);
                            if (pos != string::npos)
                            {
                                val.erase(0, pos + 1);
                                item->SetMTime(val.c_str());
                            }
                            break;
                        }    
                        case 6:
                            metadata.SetArtist(data.substr(count, fieldLength[j]));
                            break;
                        case 7:
                            metadata.SetAlbum(data.substr(count, fieldLength[j]));
                            break;
                        case 8:
                            metadata.SetTitle(data.substr(count, fieldLength[j]));
                            break;
                        case 9:
                            metadata.SetComment(data.substr(count, fieldLength[j]));
                            break;
                        case 10:
                            metadata.SetGenre(data.substr(count, fieldLength[j]));
                            break;
                        case 11:
                            metadata.SetYear(atoi(data.substr(count, fieldLength[j]).c_str()));
                            break;
                        case 12:
                            metadata.SetTrack(atoi(data.substr(count, fieldLength[j]).c_str()));
                            break;
                        case 13:
                            metadata.SetTime(atoi(data.substr(count, fieldLength[j]).c_str()));
                            break;
                        case 14:
                            metadata.SetSize(atoi(data.substr(count, fieldLength[j]).c_str()));
                            break;
                        case 15:
                            item->SetState((DownloadItemState)atoi(data.substr(count, fieldLength[j]).c_str()));
                            break;
                        default:
                            break;

                    }

                    count += fieldLength[j];
                }

                delete [] fieldLength;

                fieldLength = NULL;

                item->SetMetaData(&metadata);
                m_itemList.push_back(item);
                SendItemAddedMessage(item);
            }
            
            while ((key = database.NextKey(NULL)))
            {
                database.Remove(key);
                delete [] key;
            }
        }
    }
}

bool DownloadManager::DoesDBDirExist(char* path)
{
    bool result = false;
    struct stat st;

    if(-1 != stat(path, &st))
    {
        if(st.st_mode & S_IFDIR)
            result = true;
    }

    return result;
}

Error DownloadManager::Connect(int hHandle, const sockaddr *pAddr, 
                               int &iRet, DownloadItem *item)
{
    fd_set              sSet; 
    struct timeval      sTv;

#if defined(WIN32)
	unsigned long lMicrosoftSucksBalls = 1;
	ioctlsocket(hHandle, FIONBIO, &lMicrosoftSucksBalls);
#elif defined(__BEOS__)
//	int on = 1;
//	setsockopt( hHandle, SOL_SOCKET, SO_NONBLOCK, &on, sizeof( on ) );
#else
    fcntl(hHandle, F_SETFL, fcntl(hHandle, F_GETFL) | O_NONBLOCK);
#endif

    iRet = connect(hHandle, (const sockaddr *)pAddr,sizeof(*pAddr));
#ifndef WIN32
    if (iRet == -1 && errno != EINPROGRESS)
    {
       perror("connect");
       return kError_NoErr;
    }
#endif

    for(; iRet && !m_exit && 
        item->GetState() == kDownloadItemState_Downloading;)
    {
        sTv.tv_sec = 0; sTv.tv_usec = 0;
        FD_ZERO(&sSet); FD_SET(hHandle, &sSet);
        iRet = select(hHandle + 1, NULL, &sSet, NULL, &sTv);
        if (!iRet)
        {
           usleep(100000);
           continue;
        }

        if (iRet < 0)
           return kError_NoErr;
           
        break;
    }
    
    if (m_exit || item->GetState() != kDownloadItemState_Downloading)
       return kError_Interrupt;
      
    return kError_NoErr;
}

Error DownloadManager::Recv(int hHandle, char *pBuffer, int iSize, 
                            int iFlags, int &iRead, DownloadItem *item)
{
    fd_set              sSet; 
    struct timeval      sTv;
    int                 iRet;

    iRead = 0;
    for(; !m_exit && item->GetState() == kDownloadItemState_Downloading;)
    {
        sTv.tv_sec = 0; sTv.tv_usec = 0;
        FD_ZERO(&sSet); FD_SET(hHandle, &sSet);
        iRet = select(hHandle + 1, &sSet, NULL, NULL, &sTv);
        if (!iRet)
        {
		   usleep(10000);
           continue;
        }
        iRead = recv(hHandle, pBuffer, iSize, iFlags);
        if (iRead < 0)
        {
           return kError_NoErr;
        }
        break;
    }

    if (m_exit || item->GetState() != kDownloadItemState_Downloading)
       return kError_Interrupt;
       
    return kError_NoErr;
}                            

Error DownloadManager::Send(int hHandle, char *pBuffer, int iSize, 
                            int iFlags, int &iRead, DownloadItem *item)
{
    fd_set              sSet; 
    struct timeval      sTv;
    int                 iRet;

    iRead = 0;
    for(; !m_exit && item->GetState() == kDownloadItemState_Downloading;)
    {
        sTv.tv_sec = 0; sTv.tv_usec = 0;
        FD_ZERO(&sSet); FD_SET(hHandle, &sSet);
        iRet = select(hHandle + 1, NULL, &sSet, NULL, &sTv);
        if (!iRet)
        {
		   usleep(10000);
           continue;
        }
        iRead = send(hHandle, pBuffer, iSize, iFlags);
        if (iRead < 0)
        {
           perror("Send failed:");
           return kError_NoErr;
        }
        break;
    }

    if (m_exit || item->GetState() != kDownloadItemState_Downloading)
       return kError_Interrupt;
       
    return kError_NoErr;
}                            
