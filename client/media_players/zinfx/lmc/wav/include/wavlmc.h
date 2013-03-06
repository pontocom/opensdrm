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
   
   $Id: wavlmc.h,v 1.1 2004/10/13 16:16:10 linha2 Exp $

____________________________________________________________________________*/

#ifndef INCLUDED_WAVLMC_H_
#define INCLUDED_WAVLMC_H_

/* system headers */
#include <stdlib.h>
#include <time.h>

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

#define FRAMES_FLAG     0x0001
#define BYTES_FLAG      0x0002
#define TOC_FLAG        0x0004
#define VBR_SCALE_FLAG  0x0008

#define FRAMES_AND_BYTES (FRAMES_FLAG | BYTES_FLAG)

#ifndef WIN32
#define WAVE_FORMAT_PCM 1
typedef unsigned int DWORD;
typedef unsigned char BYTE;
#define MAKEFOURCC(ch0,ch1,ch2,ch3) ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
struct WAVEFORMAT
{
   unsigned short     wFormatTag;
   unsigned short     nChannels;
   DWORD              nSamplesPerSec;
   DWORD              nAvgBytesPerSec;
   unsigned short     nBlockAlign;
};
#endif

enum
{
   lmcError_MinimumError = 1000,
   lmcError_DecodeFailed,
   lmcError_AudioDecodeInitFailed,
   lmcError_DecoderThreadFailed,
   lmcError_PMIError,
   lmcError_PMOError,
   lmcError_MaximumError
};

class     WavLMC:public LogicalMediaConverter
{

   public:
            WavLMC(FAContext *context);
   virtual ~WavLMC();

   virtual uint32 CalculateSongLength(const char *url);

   virtual Error ChangePosition(int32 position);

   virtual Error CanDecode();
   virtual void  Clear();
   virtual Error ExtractMediaInfo();

   virtual void  SetPMI(PhysicalMediaInput *pmi) { m_pPmi = pmi; };
   virtual void  SetPMO(PhysicalMediaOutput *pmo) { m_pPmo = pmo; };
   virtual Error Prepare(PullBuffer *pInputBuffer, PullBuffer *&pOutBuffer);
   virtual Error InitDecoder();

   virtual Error SetEQData(float *, float);
   virtual Error SetEQData(bool);

   virtual Error SetDecodeInfo(DecodeInfo &info);

   virtual vector<string> *GetExtensions(void);
   
 private:

   static void          DecodeWorkerThreadFunc(void *);
   void                 DecodeWork();
   Error                BeginRead(void *&pBuffer, unsigned int iBytesNeeded);
   Error                EndRead(size_t iBytesUsed);
   Error                GetHeadInfo();
   Error                GetStats(float &fTotalSeconds, int32 &iTotalFrames);

   PhysicalMediaInput  *m_pPmi;
   PhysicalMediaOutput *m_pPmo;

   int                  m_iMaxWriteSize;
   int32                m_frameBytes, m_iBufferUpInterval;
   size_t               m_lFileSize;
   int32                m_iTotalFrames;
   Thread              *m_decoderThread;

   int32                m_frameCounter;
   time_t               m_iBufferUpdate;
   char                *m_szUrl;
   const char          *m_szError;
   
   // These vars are used for a nasty hack.
   FILE                *m_fpFile;
   char                *m_pLocalReadBuffer;

   WAVEFORMAT          m_WaveFormat;
   unsigned long       m_ulWaveSize;
   unsigned long       m_ulWaveHeaderSize;
   int                 m_iFramesPerSecond;
};

#endif /* _WAVLMC_H */




