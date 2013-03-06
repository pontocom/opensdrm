/*____________________________________________________________________________
   
   FreeAmp - The Free MP3 Player

   Portions Copyright (C) 1999-2000 EMusic.com

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, Write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   
   $Id: cdlmc.cpp,v 1.1 2004/10/13 16:16:10 linha2 Exp $
____________________________________________________________________________*/

#ifdef WIN32
#include <windows.h>
#endif

/* system headers */

#include "config.h"
#include "errors.h"
#include "cdlmc.h"
#include "facontext.h"
#include "debug.h"

#define DB Debug_v("%s:%d\n",  __FILE__, __LINE__);

extern    "C"
{
   LogicalMediaConverter *Initialize(FAContext *context)
   {
      return new CDLMC(context);
   }
}

CDLMC::CDLMC(FAContext *context) :
         LogicalMediaConverter(context)
{
   m_pContext = context;

   m_pmi = NULL;
   m_pmo = NULL;
}

CDLMC::~CDLMC()
{
}

Error CDLMC::Prepare(PullBuffer *pInputBuffer, PullBuffer *&pOutBuffer)
{
   return kError_NoErr;
}

void CDLMC::Clear()
{
}

vector<string> *CDLMC::GetExtensions(void)
{
   vector<string> *extList = new vector<string>;

   extList->push_back("CDA");

   return extList;
}

Error CDLMC::CanDecode()
{
   return kError_NoErr;
}

Error CDLMC::ExtractMediaInfo()
{
   return kError_NoErr;
}

uint32 CDLMC::CalculateSongLength(const char *szUrl)
{
    return 0;
}

Error CDLMC::InitDecoder()
{
   return kError_NoErr;
}

Error CDLMC::ChangePosition(int32 position)
{
   return kError_NoErr;
}

Error CDLMC::SetEQData(float *arrayEQ, float preamp) 
{
    return kError_NoErr;
}

Error CDLMC::SetEQData(bool enable)
{    
    return kError_NoErr;
}

Error CDLMC::SetDecodeInfo(DecodeInfo &info)
{
    m_decodeInfo = info;
    return kError_NoErr;
}

