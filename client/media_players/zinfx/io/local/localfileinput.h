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
        
        $Id: localfileinput.h,v 1.1 2004/10/13 16:16:07 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_LOCALFILEINPUT_H_
#define INCLUDED_LOCALFILEINPUT_H_

/* system headers */
#include <stdlib.h>
#include <string>

/* project headers */
#include "thread.h"
#include "config.h"
#include "pmi.h"

const int32 iMaxFileNameLen = 255;
class FAContext;

class LocalFileInput:public PhysicalMediaInput
{
   public:

   LocalFileInput(FAContext *context);
   LocalFileInput(char *path);
   virtual ~ LocalFileInput(void);

	virtual bool  CanHandle(const char *szUrl, char *szTitle);
	virtual bool  IsStreaming(void)
	              { return false; };
   virtual Error Seek(int32 & rtn, int32 offset, int32 origin);
   virtual Error Tell(int32 & rtn);
   virtual Error GetLength(size_t &iSize);

   virtual Error Prepare(PullBuffer *&pBuffer);
   virtual Error SetTo(const char *url);
   virtual Error Close(void);
   virtual void  Clear(void);
   virtual const char *Url(void) const
   {
      return m_path;
   }
   virtual Error Run(void);
   
   void          WorkerThread(void);

   virtual vector<string> *GetProtocols(void);

 private:

   virtual Error Open(void);
   static  void  StartWorkerThread(void *pVoidBuffer);
           void  SkipID3v2Tag(void);

   FILE           *m_fpFile;
   bool            m_bLoop;
   size_t          m_iFileSize;
   Thread        *m_pBufferThread; 
};

#endif /* _LOCALFILEINPUT_H_ */
