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
        
   $Id: Http.cpp,v 1.1 2004/10/13 16:16:08 linha2 Exp $
____________________________________________________________________________*/

#include "config.h"

#include <stdio.h> 
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <sys/time.h>
#endif
#include <fcntl.h>    
#include <errno.h>    
#include <ctype.h>    

#ifdef WIN32
#include <io.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif

#if defined(unix) || defined(__BEOS__) || defined(_BSD_SOURCE)
#define SOCKET int
#endif

#if defined(unix) || defined(_BSD_SOURCE)
#include <arpa/inet.h>
#define closesocket(x) close(x)
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif  

#ifdef __QNX__
#include <strings.h>
#endif

#include "Http.h"

#ifndef min
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#endif

const unsigned short kHttpPort = 80;
const unsigned int kMaxHostNameLen = 64;
const unsigned int kMaxURLLen = 1024;
const unsigned int kBufferSize=8192;

#define DB printf("%s:%d\n", __FILE__, __LINE__);

Http::Http(FAContext *context)
{
    m_exit = false;
    m_context = context;
    m_file = NULL;
    m_buffer = NULL;
    m_bytesInBuffer = 0;
    m_bufferSize = 0;
}

Http::~Http(void)
{
    if (m_buffer)
       delete m_buffer;

    if (m_file)
        fclose(m_file);
}

Error Http::DownloadToString(const string &url, string &page)
{
    Error eRet;

    eRet = Download(url, false);
    if (IsntError(eRet))
        page = string((char *)m_buffer, m_bytesInBuffer);

    if (m_buffer)
    {
       delete m_buffer;
       m_buffer = NULL;
    }

    return eRet;
}

Error Http::DownloadToFile(const string &url, const string &destPath)
{
    Error eRet;

    m_destPath = destPath;
    eRet = Download(url, true);
    if (m_file)
    {
        fclose(m_file);
        m_file = NULL;
    }

    return eRet;
}

int Http::WriteToBuffer(unsigned char *buffer, unsigned int size)
{
    if (m_buffer == NULL)
    {
        m_bufferSize = kBufferSize;
        m_buffer = new unsigned char[m_bufferSize];
    }
    if (m_bytesInBuffer + size > m_bufferSize)
    {
        unsigned char *pTemp;

        m_bufferSize += (kBufferSize < size) ? kBufferSize : kBufferSize + size;
        pTemp = new unsigned char[m_bufferSize];

        memcpy(pTemp, m_buffer, m_bytesInBuffer);
        delete m_buffer;
        m_buffer = pTemp;
    }

    memcpy(m_buffer + m_bytesInBuffer, buffer, size);
    m_bytesInBuffer += size;

    return size;
}

int Http::WriteToFile(unsigned char *buffer, unsigned int size)
{

    if (m_file == NULL)
    {
        m_file = fopen(m_destPath.c_str(), "wb");
        if (m_file == NULL)
            return -1;
    }

    return fwrite(buffer, sizeof(unsigned char), size, m_file);
}

static void
EncodeURI(string & URI)
{
   string::size_type convert = 0;
   const char *legalCharacters =

      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/?.";

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

Error Http::Download(const string &url, bool fileDownload)
{
    Error          result = kError_InvalidParam;
    char           hostname[kMaxHostNameLen + 1];
    char           localname[kMaxHostNameLen + 1];
    char           proxyname[kMaxURLLen + 1];
    unsigned short port;
    struct         sockaddr_in  addr;
    struct         hostent      host;
    SOCKET         s = -1;
    string         file;
    bool           useProxy;
    unsigned int   bytesReceived = 0;

    result = kError_ProtocolNotSupported;

    // where should we connect to?
    if(!strncasecmp(url.c_str(), "http://", 7))
    {
        int32 numFields;
        uint32 length;

        result = kError_NoErr;  

        m_context->prefs->GetPrefBoolean(kUseProxyPref, &useProxy);

        length = sizeof(proxyname);
        m_context->prefs->GetPrefString(kProxyHostPref, proxyname, &length);

        if(useProxy)
        {
            numFields = sscanf(proxyname, 
                               "http://%[^:/]:%hu", hostname, &port);

            strcpy(proxyname, url.c_str());
            file = string(proxyname);
        }
        else
        {
            const char *ptr;
            numFields = sscanf(url.c_str(), 
                           "http://%[^:/]:%hu", hostname, &port);

            ptr = strchr(url.c_str() + 7, '/');
            file = string(ptr ? ptr : "");
        }
        EncodeURI(file);

        if(numFields < 1)
        {
            result = kError_InvalidURL;     
        }

        if(numFields < 2)
        {
            port = kHttpPort;
        }            
    }

    // get hostname
    if(IsntError(result))
    {
        struct hostent* hostByName;
        struct hostent  hostByIP;

        //*m_debug << "gethostbyname: " << hostname << endl;
        hostByName = gethostbyname(hostname);

        // On some stacks a numeric IP address
        // will not parse with gethostbyname.  
        // If that didn't work try to convert it as a
        // numeric address before giving up.
        if(!hostByName)
        {
            static unsigned long ip;
            static char *addr_ptr[2] = {(char*)&ip, NULL};

            if((ip = inet_addr(hostname)) == INADDR_NONE) 
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
    }

    // connect and send request
    if(IsntError(result))
    {
        Error err;
        int   ret;
       
        err = Connect(s, (struct sockaddr*)&addr, ret);
        if (IsError(err))
            result = kError_UserCancel;
            
        if (ret < 0)    
            result = kError_ConnectFailed;

        if(IsntError(result))
        {
            gethostname(localname, kMaxHostNameLen);    

            const char* kHTTPQuery = "GET %s HTTP/1.0\r\n"
                                     "Host: %s:80\r\n"
                                     "User-Agent: FreeAmp/%s\r\n"
                                     "Accept: */*\r\n";

            // the magic 256 is enough for a time field that
            // we got from the server
            char* query = new char[ strlen(kHTTPQuery) + 
                                    file.length() +
                                    strlen(hostname) +
                                    strlen(ZINF_VERSION)+
                                    2];
        
            sprintf(query, kHTTPQuery, file.c_str(), hostname, ZINF_VERSION);
            strcat(query, "\r\n");

            int count;

            err = Send(s, query, strlen(query), 0, count);
            if (IsError(err))
                result = kError_UserCancel; 

            if(count != (int)strlen(query))
            {
                result = kError_IOError;
            }

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

                err = Recv(s, buffer + total, bufferSize - total - 1, 0, count);
                if (IsError(err))
                    result = kError_UserCancel;

                if(count > 0)
                    total += count;
                else
                {
                    result = kError_IOError;
                }

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
                    Error err;
                    result = kError_UnknownErr;

                    int32 fileSize = GetContentLengthFromHeader(buffer);


                    result = kError_NoErr;
                    int wcount = 0;

                    char* cp;

                    cp = strstr(buffer, "\r\n\r\n");
                    if(cp)
                        cp += 4;
                    if(cp)
                    {
                        if(cp - buffer < (int)total)
                        {
                            if (fileDownload)
                               wcount = WriteToFile((unsigned char *)cp,
                                                    total - (cp - buffer));
                            else
                               wcount = WriteToBuffer((unsigned char *)cp,
                                                      total - (cp-buffer));
                            bytesReceived = total - (cp - buffer);
                        }
                    }

                    do
                    {
                        if (min(bufferSize, fileSize - bytesReceived) != 0)
						{
							err = Recv(s, buffer,
									min(bufferSize, fileSize - bytesReceived),
									0, count);
						}
						else
							count = 0;
                        if (IsError(err))
                            result = kError_UserCancel;
                        else
                           if(count > 0)
                           {
                               if (fileDownload)
                                  wcount = WriteToFile((unsigned char *)buffer,
                                                       (unsigned int)count);
                               else
                                  wcount = WriteToBuffer((unsigned char *)buffer
,
                                                       (unsigned int)count);
                               bytesReceived += count;
                               Progress(bytesReceived, fileSize);
                           }

                        if(count < 0)
                            result = kError_IOError;

                        if(wcount < 0)
                            result = kError_WriteFile;

                    }while(count > 0 && IsntError(result) &&
                           !m_exit && wcount >= 0 &&
                           (int)bytesReceived != fileSize);

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
                                                   strlen(url.c_str()) + 1];

                            sprintf(proxy, "%s%s", cp, url.c_str());

                            result = Download(string(proxy), fileDownload);
                            delete [] proxy;
                        }
                        else // redirect of some type
                        {
                            result = Download(string(cp), fileDownload);
                        }
                    }
                    
                    break;
                }

                // 4xx: Client Error - The request contains bad syntax or cannot
                // be fulfilled
                case '4':
                {
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

    return result;
}

Error Http::Connect(int hHandle, const sockaddr *pAddr, int &iRet)
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
       return kError_NoErr;
#endif  
    for(; iRet && !m_exit;)
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
    
    if (m_exit)
       return kError_Interrupt;
       
    return kError_NoErr;
}

Error Http::Recv(int hHandle, char *pBuffer, int iSize, 
                 int iFlags, int &iRead)
{
    fd_set              sSet; 
    struct timeval      sTv;
    int                 iRet;

    iRead = 0;
    for(; !m_exit;)
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

    if (m_exit)
       return kError_Interrupt;
       
    return kError_NoErr;
}                            

Error Http::Send(int hHandle, char *pBuffer, int iSize, 
                 int iFlags, int &iRead)
{
    fd_set              sSet; 
    struct timeval      sTv;
    int                 iRet;

    iRead = 0;
    for(; !m_exit;)
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
           return kError_NoErr;
        }
        break;
    }

    if (m_exit)
       return kError_Interrupt;
       
    return kError_NoErr;
}                            

void Http::Progress(unsigned int bytesReceived, unsigned int maxBytes)
{

}

bool Http::IsHTTPHeaderComplete(char* buffer, uint32 length)
{
    bool result = false;

    for(char* cp = buffer; cp < buffer + length; cp++)
    {
        if(!strncmp(cp, "\n\n", 2) || !strncmp(cp, "\r\n\r\n", 4))
        {
            result = true;
            break;
        }
    }

    return result;
}

int32 Http::GetContentLengthFromHeader(const char* buffer)
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
