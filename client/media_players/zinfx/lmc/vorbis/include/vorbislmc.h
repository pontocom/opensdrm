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
   
   $Id: vorbislmc.h,v 1.1 2004/10/13 16:16:10 linha2 Exp $

____________________________________________________________________________*/

#ifndef INCLUDED_VORBISLMC_H_
#define INCLUDED_VORBISLMC_H_

/* system headers */
#include <stdlib.h>
#include <time.h>
#include <vorbis/vorbisfile.h>

/* project headers */
#include "config.h"

#include "pmi.h"
#include "pmo.h"
#include "mutex.h"
#include "event.h"
#include "lmc.h"
#include "thread.h"
#include "mutex.h"
#include "queue.h"
#include "semaphore.h"

class VorbisLMC : public LogicalMediaConverter
{

   public:
            VorbisLMC(FAContext *context);
   virtual ~VorbisLMC();

   virtual uint32 CalculateSongLength(const char *url);

   virtual Error ChangePosition(int32 position);

   virtual Error CanDecode();
   virtual void  Clear();
   virtual Error ExtractMediaInfo();

   virtual void  SetPMI(PhysicalMediaInput *pmi) { m_pPmi = pmi; };
   virtual void  SetPMO(PhysicalMediaOutput *pmo) { m_pPmo = pmo; };
   virtual Error Prepare(PullBuffer *pInputBuffer, PullBuffer *&pOutBuffer);
   virtual Error InitDecoder();

   virtual vector<string> *GetExtensions(void);

   virtual Error SetEQData(float *f, float) { return kError_YouScrewedUp; };
   virtual Error SetEQData(bool b) { return kError_YouScrewedUp; };
  
   virtual Error SetDecodeInfo(DecodeInfo &info);
 
 private:

   static int    SeekWrapper(void *stream, ogg_int64_t offset, int whence);
   static long   TellWrapper(void *stream);
   static int CloseWrapper(void *stream);
   static size_t ReadWrapper(void *buf, size_t size, size_t num, void *stream);
   int    Seek(long offset, int whence);
   long   Tell(void);
   size_t Read(void *buf, size_t size, size_t num);

   const string ConvertToISO(const char *utf8);

   static void          DecodeWorkerThreadFunc(void *);
   void                 DecodeWork();

   PhysicalMediaInput  *m_pPmi;
   PhysicalMediaOutput *m_pPmo;

   Thread              *m_decoderThread;

   char                *m_szUrl;
   const char          *m_szError;
   bool                 m_bInit;
   int                  m_channels, m_section, m_rate;
   long                 m_frameCounter, m_newPos;

   OggVorbis_File       m_vf;
};

#endif




