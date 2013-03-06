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
        
        $Id: localfileinput.cpp,v 1.1 2004/10/13 16:16:07 linha2 Exp $
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
#else
#include <netinet/in.h>
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
#include "localfileinput.h"
#include "pullbuffer.h"
#include "facontext.h"
#include "log.h"
#include "utility.h"
#include "debug.h"

const uint32 iReadBlock = 8192;

const int supportedVersion = 3;

struct ID3Header
{
   char          tag[3];
   unsigned char versionMajor;
   unsigned char versionRevision;
   unsigned char flags;
   unsigned char size[4];
};

#define DB Debug_v("%s:%d\n", __FILE__, __LINE__);

extern    "C"
{
   PhysicalMediaInput *Initialize(FAContext *context)
   {
      return new LocalFileInput(context);
   }
}

LocalFileInput::LocalFileInput(FAContext *context):
                PhysicalMediaInput(context)
{
    m_fpFile = NULL;
    m_bLoop = false;
    m_pBufferThread = NULL;
}

LocalFileInput::~LocalFileInput()
{
    m_bExit = true;
    m_bPause = false;
    m_pSleepSem->Signal();
    m_pPauseSem->Signal();

    if (m_pBufferThread)
    {
       m_pBufferThread->Join();
       delete m_pBufferThread;
    }

    if (m_fpFile)
    {
       fclose(m_fpFile); 
       m_fpFile = NULL;
    }
}

Error LocalFileInput::Prepare(PullBuffer *&pBuffer)
{
    int32 iBufferSize = iDefaultBufferSize;
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

    result = Open();
    if (!IsError(result))
    {
        result = Run();
        if (IsError(result))
        {
            ReportError("Could not run the input plugin.");
            return result;
        }
    }
    else
    {
       if (result != kError_FileNotFound)
           ReportError("Cannot open file %s.", m_path);
       return result;
    }

    pBuffer = m_pOutputBuffer;

    return kError_NoErr;
}


bool LocalFileInput::CanHandle(const char *szUrl, char *szTitle)
{
    bool bRet;
 
    bRet = strncmp(szUrl, "file://", 7) == 0;
    if (szTitle && bRet)
       *szTitle = 0;
 
    return bRet;
}

Error LocalFileInput::SetTo(const char *url)
{
    Error  result = kError_NoErr;
    uint32  len = strlen(url) + 1;

    if (m_path)
    {
       delete [] m_path;
       m_path = NULL;
    }
 
    m_path = new char[len];
    if (m_path)
    {
        if (strncmp(url, "file://", 7) == 0)
        {
            URLToFilePath(url, m_path, &len);
        }
        else
           memcpy(m_path, url, len);
    }
    else
    {
        result = kError_OutOfMemory;
    }

   return result;
}

Error LocalFileInput::Close(void)
{
    if (m_fpFile)
    {
       fclose(m_fpFile);
       m_fpFile = NULL;
    }

    PipelineUnit::Clear();

    return kError_NoErr;
}

Error LocalFileInput::GetLength(size_t &iSize)
{
    iSize = m_iFileSize;
    return kError_NoErr;
}; 

Error LocalFileInput::Tell(int32 &iRet) 
{
    iRet = ftell(m_fpFile);
    if (iRet < 0)
       return kError_SeekFailed;

    iRet -= m_pOutputBuffer->GetNumBytesInBuffer();
       
    return kError_NoErr;
}

Error LocalFileInput::Seek(int32 &iRet, int32 iPos, int32 iFrom) 
{ 
    Error eRet = kError_NoErr;

    if (iPos == 0 && iFrom == SEEK_CUR)
       return Tell(iRet);

    iRet = fseek(m_fpFile, iPos, iFrom);
    if (iRet < 0)
    {
       eRet = kError_SeekFailed;
    }
    else
    {
       eRet = Tell(iRet);
    }

    m_pOutputBuffer->Clear();

    return eRet;
}

void LocalFileInput::Clear(void) 
{ 
    if (m_fpFile)
       fseek(m_fpFile, SEEK_SET, 0); 
    m_pOutputBuffer->Clear();
    PipelineUnit::Clear();
}

#define iID3TagSize 128

Error LocalFileInput::Open(void)
{
    char pBuffer[iID3TagSize];

    if (strcmp(m_path, "-") == 0)
    {
       m_fpFile = stdin;
    }
    else
       m_fpFile = fopen(m_path, "rb");

    if (m_fpFile == NULL)
    {
        switch (errno)
        {
            case EACCES:
               m_pContext->log->Error("Access to the file was denied.\n");
               return kError_FileNoAccess;
               break;

            case EINVAL:
               m_pContext->log->Error("Internal error: The file could not be opened.\n");
               return kError_FileInvalidArg;
               break;

            case EMFILE:
               m_pContext->log->Error("Internal error: The file could not be opened.\n");
               return kError_FileNoHandles;
               break;

            case ENOENT:
               m_pContext->log->Error("File not found.\n");
               return kError_FileNotFound;
               break;

            default:
               return kError_UnknownErr;
               break;
        }
    }

    fseek(m_fpFile, 0, SEEK_END);
    m_iFileSize = ftell(m_fpFile);

    fseek(m_fpFile, -iID3TagSize, SEEK_CUR);

    int iRet = fread(pBuffer, sizeof(char), iID3TagSize, m_fpFile);

    if (iRet == iID3TagSize)
    {
        if (!strncmp(pBuffer, "TAG", 3))
        {
            m_iFileSize -= iID3TagSize;
        }
    }

    fseek(m_fpFile, 0, SEEK_SET);

    SkipID3v2Tag();

    return kError_NoErr;
}

void LocalFileInput::SkipID3v2Tag(void)
{
    ID3Header   sHead;
    int         ret, padding = 0;
    int         size;

    ret = fread(&sHead, 1, sizeof(ID3Header), m_fpFile);
    if (ret != sizeof(ID3Header))
        return;

    if (strncmp(sHead.tag, "ID3", 3))
    {
        fseek(m_fpFile, 0, SEEK_SET);
        return;
    }    

    if (sHead.versionMajor != supportedVersion)
        return;

    size = ( sHead.size[3] & 0x7F       ) |
           ((sHead.size[2] & 0x7F) << 7 ) |
           ((sHead.size[1] & 0x7F) << 14) |
           ((sHead.size[0] & 0x7F) << 21);
    if (sHead.flags & (1 << 6))
    {
        unsigned extHeaderSize;
        long lNet;

        if (fread(&extHeaderSize, 1, sizeof(int), m_fpFile) != sizeof(int))
            return;
       
        fseek(m_fpFile, sizeof(int32) + sizeof(int16), SEEK_CUR); 
        if (fread(&lNet, sizeof(int32), 1, m_fpFile) != 1)
            return;

        padding = ntohl(lNet);
    }
    fseek(m_fpFile, padding + size, SEEK_CUR); 
} 

Error LocalFileInput::Run(void)
{
    if (!m_pBufferThread)
    {
       m_pBufferThread = Thread::CreateThread();
       if (!m_pBufferThread)
       {
           m_pContext->log->Error("Could not create filebuffer thread.");
           return (Error)kError_CreateThreadFailed;
       }
       m_pBufferThread->Create(LocalFileInput::StartWorkerThread, this);
    }

    return kError_NoErr;
}

void LocalFileInput::StartWorkerThread(void *pVoidBuffer)
{
   ((LocalFileInput*)pVoidBuffer)->WorkerThread();
}  

void LocalFileInput::WorkerThread(void)
{
   size_t  iRead; 
   void   *pBuffer;
   Error   eError;

   m_pSleepSem->Wait();

   for(; !m_bExit;)
   {
      if (m_bPause)
      {
          m_pPauseSem->Wait();
          continue;
      }
      if (m_pOutputBuffer->IsEndOfStream())
      {
          m_pSleepSem->Wait();
          continue;
      }
          
      eError = m_pOutputBuffer->BeginWrite(pBuffer, iReadBlock);
      if (eError == kError_Interrupt)
         break;

      if (eError == kError_NoErr)
      {
          iRead = fread((unsigned char *)pBuffer, 1, iReadBlock, m_fpFile);
          eError = m_pOutputBuffer->EndWrite(iRead);

          if (IsError(eError))
          {
              m_pContext->log->Error("local: EndWrite returned: %d\n", eError);
              break;
          }

          if (iRead < iReadBlock)
          {
             m_pOutputBuffer->SetEndOfStream(true);
          }   
      }

      if (eError == kError_BufferTooSmall)
      {
          m_pSleepSem->Wait();
      }
   }
   m_pContext->log->Log(LogInput, "PMI: filebuffer thread exit\n");
}

vector<string> * LocalFileInput::GetProtocols(void)
{
   vector<string> *protoList = new vector<string>;

   protoList->push_back("file");

   return protoList;
}

