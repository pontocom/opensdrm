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
        
        $Id: win32volume.cpp,v 1.1 2004/10/13 16:16:08 linha2 Exp $
____________________________________________________________________________*/

#include "win32volume.h"

Win32Volume::Win32Volume( eDeviceType eType, HWND hWnd ) : 
    m_hMixer( NULL )
{
	InitVolumeControl( eType, hWnd );
}

Win32Volume::~Win32Volume(void)
{

}

void Win32Volume::GetVolume(int32 &left, int32 &right) 
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
    ret = mixerGetControlDetails((HMIXEROBJ)m_hMixer,
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

void Win32Volume::SetVolume(int32 left, int32 right) 
{
    DWORD dwValLeft, dwValRight;
    
    dwValLeft = (left * (m_dwMaximum - m_dwMinimum) / 100);
    dwValRight = (right * (m_dwMaximum - m_dwMinimum) / 100);

    MIXERCONTROLDETAILS_UNSIGNED mxcdVolume[2];
    MIXERCONTROLDETAILS mxcd;
    
    memcpy(&mxcdVolume[0], &dwValLeft, sizeof(MIXERCONTROLDETAILS_UNSIGNED));
    memcpy(&mxcdVolume[1], &dwValRight, sizeof(MIXERCONTROLDETAILS_UNSIGNED));
    
    mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
    mxcd.dwControlID = m_dwVolumeControlID;
    mxcd.cChannels = 2;
    mxcd.cMultipleItems = 0;
    mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    mxcd.paDetails = &mxcdVolume;
    
    mixerSetControlDetails((HMIXEROBJ)m_hMixer, &mxcd, 
                              MIXER_OBJECTF_HMIXER | 
                              MIXER_SETCONTROLDETAILSF_VALUE);
}

bool Win32Volume::InitVolumeControl( eDeviceType eType, HWND hWnd )
{
    MMRESULT mmresult = 0;
    //HWND            hWnd = 0;
    mmresult = mixerOpen(&m_hMixer, 0, (DWORD)hWnd, NULL, 
                        MIXER_OBJECTF_MIXER | CALLBACK_WINDOW);
    //if (mmresult != MMSYSERR_NOERROR)
    //{
    //    m_hMixer = NULL;
        //m_pContext->log->Error("Cannot open Mixer device.");
    //}    

    switch ( mmresult ) {

        case MMSYSERR_NOERROR :
            //  All Is Well...
            break;
    
        case MMSYSERR_ALLOCATED :
            m_hMixer = NULL;
            break;

        case MMSYSERR_BADDEVICEID :
            m_hMixer = NULL;
            break;

        case MMSYSERR_INVALFLAG :
            m_hMixer = NULL;
            break;

        case MMSYSERR_INVALHANDLE :
            m_hMixer = NULL;
            break;

        case MMSYSERR_INVALPARAM :
            m_hMixer = NULL;
            break;

        case MMSYSERR_NODRIVER :
            m_hMixer = NULL;
            break;

        case MMSYSERR_NOMEM :
            m_hMixer = NULL;
            break;

        default:
            m_hMixer = NULL;
            break;

    }

    m_oDstLineName = "";
    m_oVolumeControlName = "";

    if (m_hMixer == NULL)
        return FALSE;

    mxl.cbStruct = sizeof(MIXERLINE);
    
    switch ( eType ) {
        
        case eWaveOut :
            mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
            break;

        case eCDOut :
            mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;
            break;

        default:
            mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
            break;

    }
    
    if (mixerGetLineInfo((HMIXEROBJ)m_hMixer,
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

    if (mixerGetLineControls((HMIXEROBJ)m_hMixer,
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
