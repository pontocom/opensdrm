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
        
   $Id: Http.h,v 1.1 2004/10/13 16:16:08 linha2 Exp $
____________________________________________________________________________*/
#ifndef __HTTP_H__
#define __HTTP_H__

#include "errors.h"
#include "facontext.h"
#include <string>

using namespace std;

class Http
{

    public:
     
               Http(FAContext *context);
      virtual ~Http(void);

      Error        DownloadToFile(const string &url, const string &destPath);
      Error        DownloadToString(const string &url, string &page);
      virtual void Progress(unsigned int bytesReceived, unsigned int maxBytes);

    private:

      Error    Download(const string &url, bool fileDownload);
      int      WriteToFile(unsigned char *buffer, unsigned int size);
      int      WriteToBuffer(unsigned char *buffer, unsigned int size);

      int32    GetContentLengthFromHeader(const char* buffer);
      bool     IsHTTPHeaderComplete(char* buffer, uint32 length);
      Error    Connect(int hHandle, const sockaddr *pAddr, int &iRet);
      Error    Recv(int hHandle, char *pBuffer, int iSize, 
                    int iFlags, int &iRead);
      Error    Send(int hHandle, char *pBuffer, int iSize, 
                    int iFlags, int &iSend);

      bool           m_exit;
      FAContext     *m_context;
      unsigned char *m_buffer;
      uint32         m_bufferSize, m_bytesInBuffer;
      FILE          *m_file;
      string         m_destPath;
};

#endif
