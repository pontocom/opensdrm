/*____________________________________________________________________________

  FreeAmp - The Free MP3 Player

  Portions Copyright (C) 1998-1999 EMusic.com
  Portions Copyright (C) 1998.Sylvain Rebaud (soothe@jps.net)

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

  $Id: dsoundcardpmo.cpp,v 1.1 2004/10/13 16:16:06 linha2 Exp $
____________________________________________________________________________*/

/* system headers */
#include <windows.h>
#include <dsound.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

/* project headers */
#include "config.h"
#include "dsoundcardpmo.h"
#include "eventdata.h"
#include "preferences.h"
#include "facontext.h"
#include "log.h"

#define DB Debug_v("%s:%d", __FILE__, __LINE__);

// XXX: This can and should be gotten rid of
static FAContext *g_Context = 0;

#define WRITESLEEPTIME        50      /*  50 msecs  */

extern "C"
{
  PhysicalMediaOutput* Initialize(FAContext *context)
  {
      g_Context = context;
      return new DSoundCardPMO(context);
  }
}

static
bool
CALLBACK
EnumThreadWndProc(  HWND hwnd,
                    LPARAM lParam )
{
  char  strName[128];
  int   nCount;

  DSoundCardPMO*  pDSoundCardPmo    = (DSoundCardPMO*)  lParam;
  pDSoundCardPmo->m_hMainWndHandle  = hwnd;

  nCount = GetWindowText(hwnd, strName, 128);
  // If we found our main window, stop the enumeration
  if (!_stricmp(strName, "FreeAmp"))
    return false;

  return true;
}

static
bool
CALLBACK
DSEnumCallback( LPGUID  lpGuid,
                LPSTR   strDescription,
                LPSTR   strModule,
                LPVOID  lpContext)
{
  DSDevice  *pDSDevice;
  HRESULT   hResult;
  size_t    size = 0;

  DSoundCardPMO*  pDSoundCardPmo = (DSoundCardPMO *)  lpContext;

  if (!pDSoundCardPmo)
    return false;

  if (pDSoundCardPmo->m_pDSDevices)
    size = _msize( pDSoundCardPmo->m_pDSDevices );

  pDSDevice = (DSDevice *)realloc(pDSoundCardPmo->m_pDSDevices, size + sizeof(DSDevice));

  if (pDSDevice == NULL)
  {
    g_Context->log->Log(LogOutput, "error reallocating DSDevices memory...\n");
    return false;
  }

  pDSoundCardPmo->m_pDSDevices = pDSDevice;
  pDSDevice = &pDSoundCardPmo->m_pDSDevices[pDSoundCardPmo->m_nNbDSDevices];

  if (lpGuid)
  {
    pDSDevice->pGuid = (GUID *)malloc(sizeof(GUID));
    memcpy(pDSDevice->pGuid, lpGuid, sizeof(GUID));
  }
  else
    pDSDevice->pGuid = NULL;

  pDSDevice->szName = strdup(strModule);
  pDSDevice->szDescription = strdup(strDescription);

  hResult = DirectSoundCreate(pDSDevice->pGuid, &pDSDevice->pDSObject, NULL);
  if (FAILED(hResult))
  {
    g_Context->log->Log(LogOutput, "error creating DirectSound Object...\n");
    if (lpGuid)
      free((void *) pDSDevice->pGuid);

    free((void *) pDSDevice->szName);
    free((void *) pDSDevice->szDescription);

    pDSDevice = (DSDevice *)realloc(pDSoundCardPmo->m_pDSDevices, size);

    if (pDSDevice != NULL)
      pDSoundCardPmo->m_pDSDevices = pDSDevice;

    return false;
  }

  pDSDevice->lCreated = 0;

  pDSoundCardPmo->m_nNbDSDevices++;

  return true;
}

DSoundCardPMO::
DSoundCardPMO(FAContext *context):
     PhysicalMediaOutput(context)
{
  HRESULT hResult;
  MMRESULT mmresult = 0;
  Int32PropValue *pProp;
  HWND hWnd;

  m_samples_per_second  = 0;
  m_data_size           = 0;
  m_wfex                = NULL;
  m_initialized         = false;
  m_pBufferThread       = NULL;
  m_iLastFrame          = -1;
  m_iTotalBytesWritten  = 0;

  m_nNbDSDevices        = 0;
  m_pDSDevices          = NULL;
  m_nCurrentDevice      = -1;
  m_bDSEnumFailed       = false;
  m_hMainWndHandle      = NULL;
  m_pDSWriteSem         = new Semaphore(1);
  m_pDSBufferSem        = new Semaphore(0);
  m_DSBufferManager.pDSSecondaryBuffer  = NULL;
  m_bIsBufferEmptyNow   = true;
  m_bLMCsaidToPlay      = false;

  // Get a list of windows handle
  BOOL breturn = EnumWindows((WNDENUMPROC) EnumThreadWndProc, (LPARAM) this);
  // get a list of devices
  hResult = DirectSoundEnumerate((LPDSENUMCALLBACK) DSEnumCallback, this);

  if (FAILED(hResult))
  {
    m_bDSEnumFailed = true;
  }

  if (!m_pBufferThread)
  {
    m_pBufferThread = Thread::CreateThread();
    assert(m_pBufferThread);
    m_pBufferThread->Create(DSoundCardPMO::StartWorkerThread, this);
  }
  
  if (IsError(m_pContext->props->GetProperty("MainWindow", 
             (PropValue **)&pProp)))
     return;        
  else
     hWnd = (HWND)pProp->GetInt32();
  
  mmresult = mixerOpen(&m_hmixer, 0, (DWORD)hWnd, NULL, 
                        MIXER_OBJECTF_MIXER | CALLBACK_WINDOW);
  if (mmresult != MMSYSERR_NOERROR)
  {
      m_hmixer = NULL;
      m_pContext->log->Error("Cannot open Mixer device.");
  }    
  SetupVolumeControl();
}

DSoundCardPMO::
~DSoundCardPMO()
{
  int i;

  m_bExit = true;
  m_pSleepSem->Signal();
  m_pPauseSem->Signal();
  m_pDSBufferSem->Signal();
  m_pDSWriteSem->Signal();

  mixerClose(m_hmixer);

  if (m_pBufferThread)
  {
    m_pBufferThread->Join();
    delete m_pBufferThread;
    m_pBufferThread = NULL;
  }

  delete m_pDSBufferSem;
  delete m_pDSWriteSem;

  if(m_initialized)
  {
    delete m_wfex;

    // delete the direct sound secondary buffer
    if (m_DSBufferManager.pDSSecondaryBuffer)
    {
      m_DSBufferManager.pDSSecondaryBuffer->Stop();
      m_DSBufferManager.pDSSecondaryBuffer->Release();
    }

    // Do I need to release primary as well ?
  }

  if (m_pDSDevices != NULL)
  {
    for (i=0;i<m_nNbDSDevices;i++)
    {
      if (&m_pDSDevices[i] != NULL)
      {
        // Should I destroy the DirectSound object as well ?

        if ((&m_pDSDevices[i])->pGuid)
        {
          free((void *) (&m_pDSDevices[i])->pGuid);
          (&m_pDSDevices[i])->pGuid = NULL;
        }

        free((void *) (&m_pDSDevices[i])->szName);
        (&m_pDSDevices[i])->szName = NULL;
        free((void *) (&m_pDSDevices[i])->szDescription);
        (&m_pDSDevices[i])->szDescription = NULL;
      }
    }
    if (m_pDSDevices)
    {
      free((void *) (m_pDSDevices));
      m_pDSDevices = NULL;
    }
  }
}

bool DSoundCardPMO::SetupVolumeControl(void)
{
    MIXERLINE mxl;
    MIXERCONTROL mxc;
    MIXERLINECONTROLS mxlc;
    
    m_oDstLineName = "";
    m_oVolumeControlName = "";

    if (m_hmixer == NULL)
        return FALSE;

    mxl.cbStruct = sizeof(MIXERLINE);
    mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
    if (mixerGetLineInfo((HMIXEROBJ)m_hmixer,
                         &mxl,
                         MIXER_OBJECTF_HMIXER |
                         MIXER_GETLINEINFOF_COMPONENTTYPE)
        != MMSYSERR_NOERROR)
        return false;

    mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
    mxlc.dwLineID = mxl.dwLineID;
    mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
    mxlc.cControls = 1;
    mxlc.cbmxctrl = sizeof(MIXERCONTROL);
    mxlc.pamxctrl = &mxc;

    if (mixerGetLineControls((HMIXEROBJ)m_hmixer,
                             &mxlc,
                             MIXER_OBJECTF_HMIXER |
                             MIXER_GETLINECONTROLSF_ONEBYTYPE)
        != MMSYSERR_NOERROR)
        return false;

    // record dwControlID
    m_oDstLineName = mxl.szName;
    m_oVolumeControlName = mxc.szName;
    m_dwMinimum = mxc.Bounds.dwMinimum;
    m_dwMaximum = mxc.Bounds.dwMaximum;
    m_dwVolumeControlID = mxc.dwControlID;

    return TRUE;
}


void
DSoundCardPMO::
GetVolume(int32 &left, int32 &right)
{
    MIXERCONTROLDETAILS_UNSIGNED mxcdVolume[2];
    MIXERCONTROLDETAILS mxcd;
    int                 ret;
    
    mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
    mxcd.dwControlID = m_dwVolumeControlID;
    mxcd.cChannels = 2;
    mxcd.cMultipleItems = 0;
    mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    mxcd.paDetails = &mxcdVolume;
    ret = mixerGetControlDetails((HMIXEROBJ)m_hmixer,
                                 &mxcd,
                                 MIXER_OBJECTF_HMIXER |
                                 MIXER_GETCONTROLDETAILSF_VALUE);
    if (ret != MMSYSERR_NOERROR)
        return;

    left = (int)(((float)((mxcdVolume[0].dwValue - m_dwMinimum) * 100) /  
                  (float)(m_dwMaximum - m_dwMinimum)) + 0.5); 
    right = (int)(((float)((mxcdVolume[1].dwValue - m_dwMinimum) * 100) /  
                   (float)(m_dwMaximum - m_dwMinimum)) + 0.5); 
}

void
DSoundCardPMO::
SetVolume(int32 left, int32 right)
{
    DWORD dwLeft, dwRight;
    
    dwLeft = (left * (m_dwMaximum - m_dwMinimum) / 100);
    dwRight = (right * (m_dwMaximum - m_dwMinimum) / 100);

    MIXERCONTROLDETAILS_UNSIGNED mxcdVolume[2];
    MIXERCONTROLDETAILS mxcd;
    
    memcpy(&mxcdVolume[0], &dwLeft, sizeof(MIXERCONTROLDETAILS_UNSIGNED));
    memcpy(&mxcdVolume[1], &dwRight, sizeof(MIXERCONTROLDETAILS_UNSIGNED));
    
    mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
    mxcd.dwControlID = m_dwVolumeControlID;
    mxcd.cChannels = 2;
    mxcd.cMultipleItems = 0;
    mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    mxcd.paDetails = &mxcdVolume;
    
    mixerSetControlDetails((HMIXEROBJ)m_hmixer, &mxcd, 
                           MIXER_OBJECTF_HMIXER | 
                           MIXER_SETCONTROLDETAILSF_VALUE);
}

Error
DSoundCardPMO::
Init(OutputInfo* info)
{
  HRESULT       hResult;
  DSBUFFERDESC  DSBufferInfo;
  Error         result  = kError_UnknownErr;
  int32         iNewSize = iDefaultBufferSize;
  PropValue     *pProp;

  if ((m_bDSEnumFailed) || (m_nNbDSDevices == 0) || (m_pDSDevices == NULL))
    return result;

  m_samples_per_second  = info->samples_per_second;
  m_data_size           = info->max_buffer_size;

  m_pPropManager->GetProperty("MainHwnd", &pProp);
  if (pProp)
  {
    m_hMainWndHandle = (HWND) atoi(((StringPropValue *)pProp)->GetString());
  }

  m_iBytesPerSample = info->number_of_channels * (info->bits_per_sample >> 3);

  m_DSBufferManager.state             = UNDERFLOW;
  // Use Primary Sound Driver
  m_nCurrentDevice                    = 0;
  m_DSBufferManager.pDSDevice         = &m_pDSDevices[m_nCurrentDevice];
  m_DSBufferManager.pDSSecondaryBuffer= NULL;

  // What if the Callback hasn't finished yet here ?
  // Do it only if it didn't happen already
  if (InterlockedExchange(&m_DSBufferManager.pDSDevice->lCreated,1) == 0)
  {
    // the device was not initialized
    // create primary buffer

    // Should set cooperative level here to set primary buffer format
    // but we need a handle to a window ... need to talk to Mark about that
    hResult = m_DSBufferManager.pDSDevice->pDSObject->SetCooperativeLevel( m_hMainWndHandle,
                                                                                                                                                        DSSCL_PRIORITY);
    if (FAILED(hResult))
    {
      m_pContext->log->Log(LogOutput, "error cannot set DirectSound DSSCL_PRIORITY Cooperative Level...\n");
      return result;
    }

    memset(&DSBufferInfo, 0, sizeof(DSBUFFERDESC));
    DSBufferInfo.dwSize   = sizeof(DSBUFFERDESC);
    DSBufferInfo.dwFlags  = DSBCAPS_PRIMARYBUFFER;

    hResult = m_DSBufferManager.pDSDevice->pDSObject->CreateSoundBuffer(
                                              &DSBufferInfo,
                                              &m_DSBufferManager.pDSPrimaryBuffer,
                                              NULL);
    if (FAILED(hResult))
    {
      m_pContext->log->Log(LogOutput, "error Creating DirectSound Primary Buffer...\n");
      return result;
    }

    // try to set the primary buffer to 44.1 stereo 16 bits
    if (SUCCEEDED(hResult))
    {
      WAVEFORMATEX format;

      format.wBitsPerSample   = 16;
      format.wFormatTag       = WAVE_FORMAT_PCM;
      format.nChannels        = 2;
      format.nSamplesPerSec   = 44100;
      format.nAvgBytesPerSec  = 2*2*44100;
      format.nBlockAlign      = 2*2;
      format.cbSize           = 0;

      hResult = m_DSBufferManager.pDSPrimaryBuffer->SetFormat(&format);
      if (FAILED(hResult))
      {
        m_pContext->log->Log(LogOutput, "Cannot Set DirectSound Primary Buffer Format...\n");
        return result;
      }
    }
  }

  memset(&DSBufferInfo, 0, sizeof(DSBUFFERDESC));
  m_wfex              = new WAVEFORMATEX;

  // compute the levels for monitoring
  m_DSBufferManager.dwBufferSize  = 32*m_data_size;
  m_DSBufferManager.dwOverflow    = m_DSBufferManager.dwBufferSize - m_data_size;
  m_DSBufferManager.dwUnderflow   = m_data_size;
  m_DSBufferManager.dwTrigger     = 3*m_data_size;
  m_DSBufferManager.dwZerofill    = 4*m_data_size;

  m_wfex->wBitsPerSample          = info->bits_per_sample;
  m_wfex->wFormatTag              = WAVE_FORMAT_PCM;
  m_wfex->nChannels               = (WORD) info->number_of_channels;
  m_wfex->nSamplesPerSec          = info->samples_per_second;
  m_wfex->nAvgBytesPerSec         = info->number_of_channels * info->samples_per_second *
                                      (info->bits_per_sample >> 3);
  m_wfex->nBlockAlign             = (WORD) (info->number_of_channels * (info->bits_per_sample >> 3));
  m_wfex->cbSize                  = 0;

  DSBufferInfo.dwSize             = sizeof(DSBUFFERDESC);
  DSBufferInfo.dwFlags            =   DSBCAPS_CTRLFREQUENCY       |
                                      DSBCAPS_CTRLPAN             |
                                      DSBCAPS_CTRLVOLUME          |
                                      DSBCAPS_GETCURRENTPOSITION2 |
                                      DSBCAPS_GLOBALFOCUS;
  DSBufferInfo.dwBufferBytes      = m_DSBufferManager.dwBufferSize;
  DSBufferInfo.lpwfxFormat        = m_wfex;

  // Create Secondary Buffer
  hResult = m_DSBufferManager.pDSDevice->pDSObject->CreateSoundBuffer(
                                            &DSBufferInfo,
                                            &m_DSBufferManager.pDSSecondaryBuffer,
                                            NULL);
  if (FAILED(hResult))
  {
    m_pContext->log->Log(LogOutput, "error Creating DirectSound Secondary Buffer...\n");
    m_DSBufferManager.pDSSecondaryBuffer = NULL;
    return result;
  }

  m_initialized = true;

  // Clear the new buffer
  DSClear();

  // Release Semaphore so that the DSMonitorBufferThread can start polling buffer state
  m_pDSBufferSem->Signal();

  return kError_NoErr;
}

void 
DSoundCardPMO::
Pause()
{
  DSReset();
  m_bLMCsaidToPlay = false;
  m_bIsBufferEmptyNow = false;

  PhysicalMediaOutput::Pause();
}

void
DSoundCardPMO::
Resume()
{
  m_bLMCsaidToPlay = true;
  // If the LMC is pausing then resume manually
  // otherwise, it's a seek or a stop (DSClear has been called)
  // and we let the Monitor decide to start playing when there are data
  if ((m_DSBufferManager.pDSSecondaryBuffer) && (m_bIsBufferEmptyNow == false))
    m_DSBufferManager.pDSSecondaryBuffer->Play( 0, 0, DSBPLAY_LOOPING);

  PhysicalMediaOutput::Resume();
}

Error
DSoundCardPMO::
Reset(bool user_stop)
{
  if(user_stop)
  {
    DSReset();
    DSClear();
  }

  return kError_NoErr;
}

void 
DSoundCardPMO::
Clear()
{
    DSClear();
    PhysicalMediaOutput::Clear();
}

void
DSoundCardPMO::
HandleTimeInfoEvent(PMOTimeInfoEvent *pEvent)
{
  MediaTimeInfoEvent* pmtpi;
  int32               hours, minutes, seconds;
  int                 iTotalTime = 0;

  if (m_samples_per_second <= 0)
    return;

  // Since the Monitor modifies the m_iTotalBytesWritten, we meed exclusive access
  m_pDSBufferSem->Wait();

  if (pEvent->GetFrameNumber() != m_iLastFrame + 1)
  {
    m_iTotalBytesWritten = m_samples_per_frame * 
                           pEvent->GetFrameNumber() *  
                           m_iBytesPerSample;
  }
  m_iLastFrame = pEvent->GetFrameNumber();

  iTotalTime  = (m_iTotalBytesWritten) / (m_iBytesPerSample * m_samples_per_second);

  m_pDSBufferSem->Signal();

  hours       = iTotalTime / 3600;
  minutes     = (iTotalTime - hours) / 60;
  seconds     = iTotalTime - hours * 3600 - minutes * 60;

  if (minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59)
    return;

  pmtpi = new MediaTimeInfoEvent(hours, minutes, seconds, 0,
                                (float)iTotalTime, 0);
  m_pTarget->AcceptEvent(pmtpi);
}

Error
DSoundCardPMO::
GetData(void *&pBuffer)
{
  Error     eRet;
  unsigned  iRead;

  iRead = m_data_size;
  eRet = ((EventBuffer *)m_pInputBuffer)->BeginRead(pBuffer, iRead);
  if (eRet != kError_NoErr)
    return eRet;

  eRet = ((EventBuffer *)m_pInputBuffer)->EndRead(0);
  if (eRet != kError_NoErr)
    return eRet;

  return kError_NoErr;
}

Error
DSoundCardPMO::
Write(void *pBuffer)
{
  int   nState;
  int32 wrote;

  for(; !m_bExit;)
  {
    // wait until we are in a good state to write
    nState = DSMonitorBufferState();
    if (nState == -1 || nState == OVERFLOW || nState == STOPPING)
    {
      ::Sleep(WRITESLEEPTIME);
    }
    else
      break;
  }

  if (m_bExit)
    return kError_Interrupt;

  return DSWriteToSecBuffer(wrote, pBuffer, m_data_size);
}

Error
DSoundCardPMO::
DSWriteToSecBuffer(int32& wrote, void *pBuffer, int32 length)
{
  HRESULT   hResult;
  LPVOID    ptr1;
  DWORD     dwBytes1;
  LPVOID    ptr2;
  DWORD     dwBytes2;
  Error     result  = kError_UnknownErr;
  unsigned  iRead;

  iRead = m_data_size;
  result = ((EventBuffer *)m_pInputBuffer)->BeginRead(pBuffer, iRead);
  if (result != kError_NoErr)
    return result;

  // grab the write semaphore
  m_pDSWriteSem->Wait();

  // get a lock to a memory region to write to
  hResult = m_DSBufferManager.pDSSecondaryBuffer->Lock( m_DSBufferManager.dwWritePtr,
                                                        length,
                                                        &ptr1,
                                                        &dwBytes1,
                                                        &ptr2,                                                                                                          &dwBytes2,
                                                        0);
  if (hResult == DSERR_BUFFERLOST)
  {
    hResult = m_DSBufferManager.pDSSecondaryBuffer->Restore();
    if (FAILED(hResult))
    {
      wrote = 0;
      m_pDSWriteSem->Signal();
      m_pContext->log->Log(LogOutput, "Exiting DSWriteToSecBuffer with DSound error\n");
      return result;
    }
    hResult = m_DSBufferManager.pDSSecondaryBuffer->Lock(   m_DSBufferManager.dwWritePtr,
                                                            length,
                                                            &ptr1,
                                                            &dwBytes1,
                                                            &ptr2,
                                                            &dwBytes2,
                                                            0);
  }

  if (FAILED(hResult))
  {
    wrote = 0;
    m_pDSWriteSem->Signal();
    m_pContext->log->Log(LogOutput, "Exiting DSWriteToSecBuffer with DSound error\n");
    return result;
  }

  // copy the samples
  if (pBuffer)
    memcpy(ptr1, pBuffer, dwBytes1);
  else
    memset(ptr1, 0, dwBytes1);

  m_DSBufferManager.dwWritePtr += dwBytes1;
  if (m_DSBufferManager.dwWritePtr >= m_DSBufferManager.dwBufferSize)
  {
    m_DSBufferManager.dwWritePtr = 0;
  }
  if (ptr2)
  {
    if (pBuffer)
      memcpy(ptr2, (unsigned char *)pBuffer + dwBytes1, dwBytes2);
    else
      memset(ptr2, 0, dwBytes2);
    m_DSBufferManager.dwWritePtr = dwBytes2;
  }

  // unlock the memory region
  m_DSBufferManager.pDSSecondaryBuffer->Unlock( ptr1, dwBytes1, ptr2, dwBytes2);
  if (FAILED(hResult))
  {
    wrote = 0;
    m_pDSWriteSem->Signal();
    m_pContext->log->Log(LogOutput, "Exiting DSWriteToSecBuffer with DSound error\n");
    return result;
  }

  wrote = length;
  // release the write semaphore
  m_pDSWriteSem->Signal();

  result = ((EventBuffer *)m_pInputBuffer)->EndRead(iRead);
  if (result != kError_NoErr)
    return result;

  return kError_NoErr;
}

int32
DSoundCardPMO::
DSMonitorBufferState()
{
  DWORD     dwBuffered;
  DSState   new_state;
  int32     wrote;
  DWORD     dwReadPos;
  DWORD     dwWritePos;

  if (m_DSBufferManager.pDSSecondaryBuffer == NULL)
    return -1;

  m_pDSBufferSem->Wait();

  new_state = m_DSBufferManager.state;

  m_DSBufferManager.pDSSecondaryBuffer->GetCurrentPosition( &dwReadPos,
                                                            &dwWritePos);
  if (m_DSBufferManager.dwWritePtr >= dwReadPos)
  {
    dwBuffered = m_DSBufferManager.dwWritePtr - dwReadPos;
  }
  else
  {
    dwBuffered = (m_DSBufferManager.dwBufferSize - dwReadPos) +
                  m_DSBufferManager.dwWritePtr;
  }
  if (dwReadPos >= m_DSBufferManager.dwOldReadPos)
  {
    m_iTotalBytesWritten += dwReadPos - m_DSBufferManager.dwOldReadPos;
  }
  else
  {
    m_iTotalBytesWritten +=  (m_DSBufferManager.dwBufferSize - m_DSBufferManager.dwOldReadPos) +
                              dwReadPos;
  }
  m_DSBufferManager.dwOldReadPos  = dwReadPos;

  switch(m_DSBufferManager.state)
  {
    case UNDERFLOW:
//      m_pContext->log->Log(LogOutput, "UNDERFLOW    : %d\n", dwBuffered*100/m_DSBufferManager.dwBufferSize);
      if (dwBuffered > m_DSBufferManager.dwTrigger)
      {
        if (dwBuffered >= m_DSBufferManager.dwBufferSize)
        {
          new_state = OVERFLOW;
        }
        else
        {
          new_state = NORMAL;
        }

        // restart the buffer
        if (m_bLMCsaidToPlay && m_bIsBufferEmptyNow)
        m_DSBufferManager.pDSSecondaryBuffer->Play( 0, 0, DSBPLAY_LOOPING);

        m_bIsBufferEmptyNow = false;
      }
      break;

    case NORMAL:
//      m_pContext->log->Log(LogOutput, "NORMAL       : %d\n", dwBuffered*100/m_DSBufferManager.dwBufferSize);
      if (dwBuffered < m_DSBufferManager.dwUnderflow)
      {
        new_state = STOPPING;

        DSWriteToSecBuffer(wrote, NULL, m_DSBufferManager.dwZerofill);

      }
      else if (dwBuffered >= m_DSBufferManager.dwOverflow)
      {
        new_state = OVERFLOW;
      }
      else
      {
        new_state = NORMAL;
      }
    break;

    case OVERFLOW:
//      m_pContext->log->Log(LogOutput, "OVERFLOW     : %d\n", dwBuffered*100/m_DSBufferManager.dwBufferSize);
      if (dwBuffered < m_DSBufferManager.dwUnderflow)
      {
        new_state = STOPPING;
        DSWriteToSecBuffer( wrote, NULL, m_DSBufferManager.dwZerofill);
      }
      else if (dwBuffered < m_DSBufferManager.dwOverflow)
      {
        new_state = NORMAL;
      }
      break;

    case STOPPING:
//      m_pContext->log->Log(LogOutput, "STOPPING     : %d\n", dwBuffered*100/m_DSBufferManager.dwBufferSize);
      if (dwBuffered < m_DSBufferManager.dwZerofill)
      {
        Reset(true);
        new_state = UNDERFLOW;
      }
      break;
  }

  m_DSBufferManager.state = new_state;

  m_pDSBufferSem->Signal();

  return new_state;
}

void
DSoundCardPMO::
DSReset()
{
  if (m_DSBufferManager.pDSSecondaryBuffer)
  {
    m_DSBufferManager.pDSSecondaryBuffer->Stop();
  }
}

void
DSoundCardPMO::
DSClear()
{
  HRESULT hResult;
  LPVOID  ptr;
  DWORD dwBytes;

  // grab the write semaphore
  m_pDSWriteSem->Wait();

  // init the fields
  m_DSBufferManager.dwWritePtr    = 0;
  m_DSBufferManager.dwOldReadPos  = 0;
  m_DSBufferManager.state         = UNDERFLOW;

  // reset the play cursor
  m_DSBufferManager.pDSSecondaryBuffer->SetCurrentPosition(0);

  // write zeros into secondary buffer
  hResult = m_DSBufferManager.pDSSecondaryBuffer->Lock( 0,
                                                        m_DSBufferManager.dwBufferSize,
                                                        &ptr,
                                                        &dwBytes,
                                                        NULL,
                                                        NULL,
                                                        0);
  if (SUCCEEDED(hResult))
  {
    memset(ptr, 0, m_DSBufferManager.dwBufferSize);
    m_DSBufferManager.pDSSecondaryBuffer->Unlock(   ptr,
                                                    dwBytes,
                                                    NULL,
                                                    0);
  }
  m_bIsBufferEmptyNow = true;

  // grab the write semaphore
  m_pDSWriteSem->Signal();
}

void
DSoundCardPMO::
StartWorkerThread(void *pVoidBuffer)
{
  ((DSoundCardPMO*)pVoidBuffer)->WorkerThread();
}

void
DSoundCardPMO::
WorkerThread(void)
{
   void*   pBuffer;
   Error   eErr;
   Event*  pEvent;
   int32 iValue;

   // Don't do anything until resume is called.
   m_pPauseSem->Wait();

   // Wait for prebuffer period
   PreBuffer();

   m_pContext->prefs->GetPrefInt32(kDecoderThreadPriorityPref, &iValue);
   m_pBufferThread->SetPriority(iValue);

   for(; !m_bExit;)
   {
      if (m_bPause)
      {
          m_pPauseSem->Wait();
          continue;
      }

      // Loop until we get an Init event from the LMC
      if (!m_initialized)
      {
          pEvent = ((EventBuffer *)m_pInputBuffer)->GetEvent();

          if (pEvent == NULL)
          {
              m_pLmc->Wake();
              WasteTime();

              continue;
          }

          if (pEvent->Type() == PMO_Init)
          {
              if (IsError(Init(((PMOInitEvent *)pEvent)->GetInfo())))
              {
                  delete pEvent;
                  break;
              }
          }
          delete pEvent;

          continue;
      }

      // Set up reading a block from the buffer. If not enough bytes are
      // available, sleep for a little while and try again.
      for(;;)
      {
          if (m_bPause || m_bExit)
              break;
          
          eErr = GetData(pBuffer);
          if (eErr == kError_EndOfStream || eErr == kError_Interrupt)
             break;


          if (eErr == kError_NoDataAvail)
          {
              m_pLmc->Wake();
              CheckForBufferUp();
    
              WasteTime();
              continue;
          }

          // Is there an event pending that we need to take care of
          // before we play this block of samples?
          if (eErr == kError_EventPending)
          {
              pEvent = ((EventBuffer *)m_pInputBuffer)->GetEvent();
              if (pEvent == NULL)
                  continue;

              if (pEvent->Type() == PMO_Init)
                  Init(((PMOInitEvent *)pEvent)->GetInfo());
    
              if (pEvent->Type() == PMO_Reset)
                  Reset(true);
    
              if (pEvent->Type() == PMO_Info) 
                  HandleTimeInfoEvent((PMOTimeInfoEvent *)pEvent);
    
              if (pEvent->Type() == PMO_Quit) 
              {
                  delete pEvent;
                  m_pTarget->AcceptEvent(new Event(INFO_DoneOutputting));
                  
                  return;
              }
 
              delete pEvent;
    
              continue;
          }
          
          if (IsError(eErr))
          {
              ReportError("Internal error occured.");
              m_pContext->log->Error("Cannot read from buffer in PMO "
                                    "worker tread: %d\n", eErr);
              return;
          }
          break;
      }
      if (m_bPause || m_bExit)
         continue;

      Write(pBuffer);
      m_pLmc->Wake();
      UpdateBufferStatus();
  }

  m_pContext->log->Log(LogOutput, "PMO: Soundcard thread exiting\n");
}

