/*____________________________________________________________________________

   FreeAmp - The Free MP3 Player

   Copyright (C) 1999 EMusic

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

   $Id: Bitmap.cpp,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#include "string"
#include "Bitmap.h"
#include "debug.h"

Bitmap::Bitmap(const string &oName)
{
   m_oBitmapName = oName;
   m_bHasTransColor = false;
}

Bitmap::~Bitmap(void)
{
}

void Bitmap::GetName(string &oName)
{
    oName = m_oBitmapName;
}

void Bitmap::GetErrorString(string &oError)
{
    oError = m_oLastError;
}

void Bitmap::SetTransColor(Color &oColor)
{
    m_bHasTransColor = true;
    m_oTransColor = oColor;
}

bool Bitmap::GetTransColor(Color &oColor)
{
    oColor = m_oTransColor;
    return m_bHasTransColor;
}
