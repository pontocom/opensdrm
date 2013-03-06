/*____________________________________________________________________________
        
        FreeAmp - The Free MP3 Player
        Portions copyright (C) 1998-1999 EMusic.com

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
        
        $Id: artspmo.h,v 1.1 2004/10/13 16:16:05 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_ARTSDPMO_H_
#define INCLUDED_ARTSDPMO_H_

/* system headers */
#include <stdlib.h>

/* project headers */
#include <config.h>
#include "thread.h"
#include "mutex.h"
#include "pmo.h"
#include "pmoevent.h"
#include "eventbuffer.h"

enum
{
   pmoError_MinimumError = 4000,
   pmoError_DeviceOpenFailed,
   pmoError_MaximumError
};

class FAContext;

class artsPMO:public PhysicalMediaOutput
{
 public:

              artsPMO(FAContext *context);
     virtual ~artsPMO();

     virtual Error Init(OutputInfo * info);

     static void   StartWorkerThread(void *);
     void          GetVolume(int32 &left, int32 &right);
     void          SetVolume(int32 left, int32 right);
     
   private:
     void          WorkerThread(void);
     virtual Error Reset(bool user_stop);
     void          HandleTimeInfoEvent(PMOTimeInfoEvent *pEvent);

     bool         m_properlyInitialized;
		 arts_stream_t	audio_stream;
     int          mixer_fd, stream_id;
     char         stream_name[64];
     char        *m_espeaker;
     OutputInfo  *myInfo;
     Thread      *m_pBufferThread;
     int          m_iBytesPerSample;
     long long    m_iTotalBytesWritten;
     int          m_iLastFrame;
     unsigned int m_iDataSize;
     int          m_iVolume;
};

#endif /* _ARTSPMO_H_ */ 
