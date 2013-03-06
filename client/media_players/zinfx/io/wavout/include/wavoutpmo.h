/*____________________________________________________________________________

  FreeAmp - The Free MP3 Player

  Portions Copyright (C) 1998 EMusic.com
  Portions Copyright (C) 2000 Michael Rich

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

  $Id: wavoutpmo.h,v 1.1 2004/10/13 16:16:08 linha2 Exp $
____________________________________________________________________________*/


#ifndef INCLUDED_WAVOUTPMO_H_
#define INCLUDED_WAVOUTPMO_H_

/* project headers */
#ifdef WIN32
#include <windows.h>
#endif

#include "config.h"
#include "pmo.h"
#include "thread.h"
#include "mutex.h"
#include "properties.h"
#include "eventbuffer.h"
#include "semaphore.h"
#include "facontext.h"
#include "preferences.h"

#include "wav.h"

class WavOutPMO : public PhysicalMediaOutput
{

public:
  WavOutPMO(FAContext *context);
  virtual ~WavOutPMO();

  virtual Error   Init(OutputInfo* info);

  void            Pause(void);
  void            Resume(void);
  void            Quit(void);

  virtual void    GetVolume(int32 &, int32 &);
  virtual void    SetVolume(int32, int32);

  static  void    StartWorkerThread(void *);

 private:

  void            WorkerThread(void);
  virtual Error   Reset(bool user_stop);
  void            HandleTimeInfoEvent(PMOTimeInfoEvent *pEvent);
  Error           Write(void *pBuffer);

  void            Clear(void);
  WAVEFORMATEX*   m_wfex;

  uint32          m_samples_per_second, m_samples_per_frame;
  uint32          m_data_size;

  Thread*         m_pBufferThread;
  bool            m_initialized;
  int             m_iTotalBytesWritten, m_iBytesPerSample;
  int             m_iLastTime;

  WaveWriter     *m_Writer;
};

#endif /* INCLUDED_DISKOUTPMO_H_ */



