/*____________________________________________________________________________
        
   FreeAmp - The Free MP3 Player

   Copyright (C) 1999 EMusic.com

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
        
   $Id: Equalizer.h,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_EQUALIZER_H
#define INCLUDED_EQUALIZER_H

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786 4800)
#endif

#include "facontext.h"
#include "Window.h"

class Equalizer
{
    public:

        Equalizer(FAContext* context);
        virtual ~Equalizer();

        void Enable(bool enable);
        void LoadSettings(void);
        void SaveSettings(void);
        void InitControls(Window *pWindow);
        void ChangeValue(int sliderNum, int value);
        void ChangePreamp(int value);
        bool IsEnabled(void);

    protected:

        FAContext       *m_context;
        bool             m_settingsChanged, m_enabled;
        float            m_equalizer[32], m_preamp;
        int              m_sliders[10];
};

#endif
