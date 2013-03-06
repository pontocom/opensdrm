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

   $Id: Parse.h,v 1.1 2004/10/13 16:16:09 linha2 Exp $
____________________________________________________________________________*/ 

#ifndef INCLUDED_PARSE_H__
#define INCLUDED_PARSE_H__

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <string>
#include <map>

using namespace std;

#include "errors.h"

typedef map<string, string > AttrMap;

class Parse
{
    public:

               Parse(void);
      virtual ~Parse(void);

      Error    ParseFile(const string &oFile);
      Error    ParseString(const string &oXML);
      void     GetErrorString(string &oError);
      int      GetErrorLine(void);

    protected:
     
              Error DoParse(void);
              int   Scanf(const char *szFormat, char *szData);
              bool  Eof(void);

      virtual Error BeginElement(string &oElement, AttrMap &oAttrMap) = 0; 
      virtual Error EndElement(string &oElement) = 0;
      virtual Error PCData(string &oData) = 0;
              int   CountNewlines(char *szElement);
              void  UnXMLize(string &oData);

      string        m_oLastError;
      int           m_iErrorLine; 
      FILE         *m_fpFile;
      string        m_oXML;
      uint32        m_uScanOffset;
};

#endif
