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

   $Id: MessageDialog.h,v 1.1 2004/10/13 16:16:18 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_MESSAGEDIALOG_H__
#define INCLUDED_MESSAGEDIALOG_H__

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <string>

using namespace std;

#include "facontext.h"

enum MessageDialogEnum
{
    kMessageOk,
    kMessageYesNo,
    kMessageOkCancel,
    kMessageMonicaSucks
};

enum MessageDialogReturnEnum
{
    kMessageReturnOk,
    kMessageReturnCancel,
    kMessageReturnYes,
    kMessageReturnNo,
    kMessageReturnMonicaDoesSuck,
    kMessageReturnUnknown
};

class MessageDialog
{
  public:
    
                MessageDialog(FAContext *context);
       virtual ~MessageDialog(void);                  

       MessageDialogReturnEnum Show(const char *szMessage, 
                                    const char *szTitle, 
                                    MessageDialogEnum eType,
                                    bool InEventLoop = false);
       MessageDialogReturnEnum Show(const string &oMessage, 
                                    const string &oTitle, 
                                    MessageDialogEnum eType,
                                    bool InEventLoop = false);

  private:
  
       FAContext *m_context;
};

#endif
