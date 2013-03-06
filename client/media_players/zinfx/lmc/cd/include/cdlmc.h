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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   
   $Id: cdlmc.h,v 1.1 2004/10/13 16:16:10 linha2 Exp $

____________________________________________________________________________*/

#ifndef INCLUDED_CDLMC_H_
#define INCLUDED_CDLMC_H_

/* project headers */
#include "config.h"

#include "pmi.h"
#include "pmo.h"
#include "lmc.h"

class  CDLMC:public LogicalMediaConverter
{

   public:
            CDLMC(FAContext *context);
   virtual ~CDLMC();

   virtual uint32 CalculateSongLength(const char *url);

   virtual Error ChangePosition(int32 position);

   virtual Error CanDecode();
   virtual void  Clear();
   virtual Error ExtractMediaInfo();

   virtual Error Prepare(PullBuffer *pInputBuffer, PullBuffer *&pOutBuffer);
   virtual Error InitDecoder();

   virtual Error SetEQData(float *, float);
   virtual Error SetEQData(bool);

   virtual Error SetDecodeInfo(DecodeInfo &info);

   virtual vector<string> *GetExtensions(void);
};

#endif /* _CDLMC_H */
