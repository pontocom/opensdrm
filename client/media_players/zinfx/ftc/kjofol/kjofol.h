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
        
   $Id: kjofol.h,v 1.1 2004/10/13 16:16:05 linha2 Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_KJOFOL_H
#define INCLUDED_KJOFOL_H

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <map>
using namespace std;

#include "Types.h"

#include "config.h"
#include "facontext.h"
#include "errors.h"
#include "themeformat.h"

class KJofol_Font {
  public:
    KJofol_Font() { image = ""; size.x = 0; size.y = 0; spacing = 0;
                    transparent = 0; 
                  }
    string image;
    Pos    size;
    uint32 width;
    uint32 height;
    uint32 spacing;
    uint32 transparent;
    char  *fontmap;
};

class KJofol : public ThemeFormat 
{
    public:

        KJofol(FAContext *context);
        virtual ~KJofol(); 

        virtual bool IsSupportedFormat(string &oDir);
        virtual Error ConvertToNative(string &oDir);

    private:
        string FindRCFile(string &oDir);

        void  ParseRCFile(string rcfile, string windowname, bool dock = false);

        Error GetNextLine(char *line);
        bool  TestForComment(char *line);
        void  BuildImageList(string &oDir);
        void  Write(char *line, int indent = 0);

        void  MunchLine(char *orig, char *left, char *right);
        void  ParseButton(char *orig, Rect &oRect, string &name);
        void  ParsePosition(char *orig, Pos &oPos);
        void  ParseRect(char *orig, Rect &oRect);

        void HandleBitmap(string &oDir, char *name);
        void HandleButton(string name, string inf, string tip, Rect oRect, 
                          string bmpname, bool hasextra, string extrainfo);
        void HandleFont(KJofol_Font font);
        void HandleTextWindow(char *desc, string name, KJofol_Font font);
        void HandleTimeWindow(char *desc, string name, KJofol_Font font);

        void Seek(char *desc);
        void Button(char *desc, string name, string inf, string tip, 
                    bool hasextra = false, string extrainfo = "");
        void VolumeBMP(void);

        FILE *infile, *outfile;

        map<string, Pos> bmp_sizes;
        map<string, string> info;
        map<string, KJofol_Font> fonts;

        Rect m_seekrect;

        bool m_bmpvolume;
        int m_bmpvolfields;
        bool m_understandvolume;
        int m_volxsize;
        int m_volnum;

        string dockrcfilename;
        bool   hasdock;
        Pos    dockposxy;

        string winshadercfilename;
        bool   haswinshade;
};

#endif
