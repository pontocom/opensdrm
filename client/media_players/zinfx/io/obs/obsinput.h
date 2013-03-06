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
        
        $Id: obsinput.h,v 1.1 2004/10/13 16:16:07 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_OBSFILEINPUT_H_
#define INCLUDED_OBSFILEINPUT_H_

/* system headers */
#include <stdlib.h>

/* project headers */
#include "config.h"
#include "pmi.h"
#include "tstream.h"

const int iMaxUrlLen = 1024;
const int iMaxErrorLen = 1024;
const int iMAX_PACKET_SIZE = 8192;
const int iSTREAM_EOS = 0x02;
const int iMAX_RETRYS = 10;

struct ID3Tag
{
   char szTag[3];
   char szTitle[30];
   char szArtist[30];
   char szAlbum[30];
   char szYear[4];
   char szComment[30];
   char cGenre;
};

struct RTPHeader
{
    int32 iFlags;
    int32 iTimestamp;
    int32 iSyncSourceId;
};

enum
{
   obsError_MinimumError = 3000,
   obsError_BadUrl,
   obsError_CannotSetSocketOpts,
   obsError_CannotCreateSocket,
   obsError_CannotBind,
   obsError_SocketRead,
   obsError_MaximumError
};

class FAContext;

class ObsInput :public PhysicalMediaInput
{
 public:

   ObsInput(FAContext *context);
   ObsInput(char *path);
   virtual ~ObsInput(void);

   virtual Error Prepare(PullBuffer *&pBuffer);
   virtual Error Run(void);

   virtual bool  CanHandle(const char *szUrl, char *szTitle);
   virtual bool  IsStreaming(void)
                 { return true; };
   virtual bool  PauseLoop(bool bLoop);

   virtual Error Close(void);
   virtual const char *Url(void) const
   {
      return m_path;
   }

   void           WorkerThread(void);

   virtual vector<string> *GetProtocols(void);

 protected:

   virtual Error       Open(void);
   static   void       StartWorkerThread(void *);

 private:
   int                 m_hHandle;
   Thread             *m_pBufferThread;
   bool                m_bLoop, m_bDiscarded;
   struct sockaddr_in *m_pSin;
   TitleStreamServer *m_pTitleStream; 
};

#endif /* _OBSFILEINPUT_H_ */
