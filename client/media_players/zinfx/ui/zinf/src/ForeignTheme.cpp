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
        
   $Id: ForeignTheme.cpp,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/

#include "config.h"
#include "ForeignTheme.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include "facontext.h"
#include "registrar.h"

ForeignTheme::ForeignTheme(FAContext *context)
{
    m_context = context;

    Registrar registrar;

    registrar.SetSubDir("plugins");
    registrar.SetSearchString("*.ftf");
    registrar.InitializeRegistry(&m_formatRegistry, context->prefs);

    RegistryItem *plug = NULL;
    ThemeFormat *ftf = NULL;
    int32 i = 0;

    while ((plug = m_formatRegistry.GetItem(i++))) {
        ftf = (ThemeFormat *) plug->InitFunction()(m_context);
        if (ftf) 
            m_formats.push_back(ftf);
    }
}

ForeignTheme::~ForeignTheme()
{
    uint32 index = 0;
    uint32 size = 0;

    size = m_formats.size();
    for (index = 0; index < size; index++) 
        delete m_formats[index];
}

void ForeignTheme::TryToConvert(string &oDir)
{
    ThemeFormat *ftf = NULL;
    uint32 numFormats = m_formats.size();

    string testfile = oDir + string(DIR_MARKER_STR) + string("theme.xml");
    struct stat st;
 
    if (stat(testfile.c_str(), &st) != -1)
        return;

    for (uint32 i = 0; i < numFormats; i++) {
        ftf = m_formats[i];

        if (ftf) {
            if (ftf->IsSupportedFormat(oDir)) {
                ftf->ConvertToNative(oDir);
                break;
            }
        }
    }
}
