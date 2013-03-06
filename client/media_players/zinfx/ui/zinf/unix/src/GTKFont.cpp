/*____________________________________________________________________________

   FreeAmp - The Free MP3 Player

   Copyright (C) 1999-2000 EMusic

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

   $Id: GTKFont.cpp,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/ 

#include <sys/stat.h>
#include "GTKFont.h"
#include "GTKUtility.h"
#include "win32impl.h"
#include "utility.h"
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#ifdef __QNX__
#include <strings.h>
#endif

GTKFont::GTKFont(FAContext *context, string &oName, string &oFace, 
                 string &oFile, string &oDefault) :
                Font(oName, oFace, oFile, oDefault)
{
    m_context = context;

    type = kFontTypeUnknown;
 
#ifdef HAVE_FREETYPE
    if (oFile.length() > 0) {
        AddFont(oFile);
        type = kFontTypeTTF;
        gfont = NULL;
        ttfont = NULL;
        bold = false;
        italic = false;
        underline = false;
        first = true;
        return;
    }
#endif

    string finalName;    
    vector<string> Names;
    vector<string>::iterator i;

    char *dup = strdup(m_oFace.c_str());
    char *token = strtok(dup, ",");
    while (token) {
         Names.push_back(string(token));
         token = strtok(NULL, ",");
    }

    bool gdkfound = false;
    bool ttffound = false;

#ifdef HAVE_FREETYPE
    char dir[_MAX_PATH];
    uint32 len = sizeof(dir);
    string ttfbase, ttfpath;
    WIN32_FIND_DATA find;
    HANDLE handle;
 
    m_context->prefs->GetPrefString(kInstallDirPref, dir, &len);
    ttfbase = string(dir) + "/" + BRANDING_SHARE_PATH + "/fonts";
    ttfpath = ttfbase + "/*.ttf";
 
    struct stat st;
    if (-1 == stat(ttfbase.c_str(), &st))
        mkdir(ttfbase.c_str(), 0755);

    handle = FindFirstFile((char *)ttfpath.c_str(), &find);
    if (handle != INVALID_HANDLE_VALUE) {
        do { 
            i = Names.begin(); 
            for (; i != Names.end(); i++) {
                if (!strncasecmp(find.cFileName, (*i).c_str(), (*i).length())) {
                    finalName = ttfbase + string("/") + string(find.cFileName);
                    ttffound = true;
                    break;
                }
            }
            if (ttffound == true)
                break;
        } while (FindNextFile(handle, &find));
        FindClose(handle);
    }

    char *fadir = FreeampDir(NULL);
    ttfbase = string(fadir) + string("/fonts");
    delete [] fadir;

    if (-1 == stat(ttfbase.c_str(), &st))
        mkdir(ttfbase.c_str(), 0755);

    ttfpath = ttfbase + string("/*.ttf");
    handle = FindFirstFile((char *)ttfpath.c_str(), &find);
    if (handle != INVALID_HANDLE_VALUE) {
        do {
            i = Names.begin();
            for (; i != Names.end(); i++) {
                if (!strncasecmp(find.cFileName, (*i).c_str(), (*i).length())) {
                    finalName = ttfbase + string("/") + string(find.cFileName);
                    ttffound = true;
                    break;
                }
            }
            if (ttffound == true)
                break;
        } while (FindNextFile(handle, &find));
        FindClose(handle);
    }

    ttfbase = "./fonts";
    ttfpath = ttfbase + string("/*.ttf");
    handle = FindFirstFile((char *)ttfpath.c_str(), &find);
    if (handle != INVALID_HANDLE_VALUE) {
        do {
            i = Names.begin();
            for (; i != Names.end(); i++) {
                if (!strncasecmp(find.cFileName, (*i).c_str(), (*i).length())) {
                    finalName = ttfbase + string("/") + string(find.cFileName);
                    ttffound = true;
                    break;
                }
            }
            if (ttffound == true)
                break;
        } while (FindNextFile(handle, &find));
        FindClose(handle);
    }
#endif

    if (!ttffound) {
        gdk_threads_enter();
        i = Names.begin();
        for (; i != Names.end(); i++) {
            if (gdkfound = ListFonts((char *)(*i).c_str())) {
                finalName = *i;
                break;
            }
        }
        gdk_threads_leave();
    }

    if (gdkfound) {
        m_oFace = finalName; 
        type = kFontTypeGdk;
    }
#ifdef HAVE_FREETYPE
    else if (ttffound) { 
        m_oFace = finalName;
        type = kFontTypeTTF;
    }
#endif
    else {
        m_oFace = m_oDefault;
        type = kFontTypeGdk;
    }
    free(dup);

    gfont = NULL;
#ifdef HAVE_FREETYPE
    ttfont = NULL;
#endif
    bold = false;
    italic = false;
    underline = false;
    first = true;
}

Error GTKFont::AddFont(string &oFontFile)
{
    string oFontDest;
    char fcopy[_MAX_PATH], *filename, *ext;
    struct stat st;
    string tempfile;

    char *fadir = FreeampDir(NULL);
    oFontDest = string(fadir) + string ("/fonts");
    delete [] fadir;

    if (-1 == stat(oFontDest.c_str(), &st))
        mkdir(oFontDest.c_str(), 0755);

    strcpy(fcopy, oFontFile.c_str());
    filename = strrchr(fcopy, '/');
    if (filename) 
        filename = filename + 1;
    ext = strrchr(filename, '.');
    if (ext) {
        *ext = '\0';
        ext++;
    }

    tempfile = oFontFile;
    if (-1 == stat(tempfile.c_str(), &st)) 
        tempfile = FindFile(tempfile);
         
    oFontDest += string("/") + string(filename);
    if (ext)
        oFontDest += string(".") + string(ext);

    CopyFile(tempfile.c_str(), oFontDest.c_str(), true);

    m_oFace = oFontDest;

    return kError_NoErr;
}
    
Error GTKFont::Load(int iFontHeight, bool bBold, bool bItalic)
{
    gdk_threads_enter();
    if (type == kFontTypeGdk) {
        if (bold != bBold || italic != bItalic || size != iFontHeight || 
            !gfont) {
            bold = bBold;
            italic = bItalic;
            size = iFontHeight;
            if (gfont)
                gdk_font_unref(gfont);
            string fontname = BuildFontString(bold, italic, size);
            gfont = gdk_font_load(fontname.c_str());
            if (!gfont) {
                fontname = BuildFontString(bold, italic, size - 1);
                gfont = gdk_font_load(fontname.c_str());
                if (!gfont) {
                    gfont = gdk_font_load("variable");  
                    if (!gfont) {
                        gfont = gdk_font_load("fixed");
                        if (!gfont) {
                            gdk_threads_leave();
                            //cout << "oops, couldn't load _any_ fonts...\n";
                            return kError_YouScrewedUp;
                        }
                    }
                }
            } 
        }
    }
#ifdef HAVE_FREETYPE
    else {
        if (!ttfont || iFontHeight != size) {
            size = iFontHeight;
            if (ttfont)
                Efont_free(ttfont);
            ttfont = Efont_load((char *)m_oFace.c_str(), iFontHeight - 3);
            if (!ttfont) {
                gdk_threads_leave();
                //cout << "ERROR loading ttf " << m_oFace << "\n";
                return kError_YouScrewedUp;
            }
        }
    }
#endif
    gdk_threads_leave();
    return kError_NoErr;
}

int GTKFont::GetLength(string &oText)
{
    int retvalue = 0;

    gdk_threads_enter();
    if (type == kFontTypeGdk) 
        retvalue = gdk_string_measure(gfont, oText.c_str());
#ifdef HAVE_FREETYPE
    else if (type == kFontTypeTTF) {
        Efont_extents(ttfont, (char *)oText.c_str(), NULL, NULL, &retvalue, 
                      NULL, NULL, NULL, NULL);
    }
#endif
    gdk_threads_leave();
    return retvalue;
}

int GTKFont::GetHeight(string &oText)
{
    int retvalue = 0;

    gdk_threads_enter();
    if (type == kFontTypeGdk)
       retvalue = gdk_string_height(gfont, oText.c_str());
#ifdef HAVE_FREETYPE
    else if (type == kFontTypeTTF) {
       int ascent = 0, descent = 0;
       Efont_extents(ttfont, (char *)oText.c_str(), &ascent, &descent, NULL, 
                     NULL, NULL, NULL, NULL);
       retvalue = ascent + descent;
    }
#endif
    gdk_threads_leave();
    return retvalue;
}

void GTKFont::Render(Rect &oClipRect, string &oText, int iOffset, 
                     const Color &oColor, GTKBitmap *bitmap, bool bUnderline)
{
    if (!bitmap->GetBitmap())
        return;

    gdk_threads_enter();
    GdkGC *gc = gdk_gc_new((GdkWindow *)bitmap->GetBitmap());
    gdk_rgb_gc_set_foreground(gc, (oColor.red << 16) | (oColor.green << 8) |
                              oColor.blue);

    GdkRectangle clipRect;
    clipRect.x = oClipRect.x1;
    clipRect.y = oClipRect.y1;
    clipRect.width = oClipRect.Width();
    clipRect.height = oClipRect.Height() + 1;

    gdk_gc_set_clip_rectangle(gc, &clipRect);

    if (type == kFontTypeGdk) {
       gdk_draw_string(bitmap->GetBitmap(), gfont, gc, oClipRect.x1 + iOffset, 
                       oClipRect.y1 + oClipRect.Height() - 3, oText.c_str());

    // TODO: Underline...
    }
#ifdef HAVE_FREETYPE
    else if (type == kFontTypeTTF) {
       int ycoord;
       ycoord = oClipRect.y1 + oClipRect.Height();
       if (ttfont->ascent < oClipRect.Height()) 
           ycoord -= ttfont->descent;
       else 
           ycoord -= ttfont->descent / 2;
       ycoord++;

       EFont_draw_string(bitmap->GetBitmap(), gc, oClipRect.x1 + iOffset, 
                         ycoord, (char *)oText.c_str(), ttfont);
    }
#endif
    gdk_flush();
    gdk_gc_unref(gc);
    gdk_threads_leave();

    return;
}

string GTKFont::BuildFontString(bool bBold, bool bItalic, int iFontHeight)
{
   // This isn't the greatest method, as it will probably return stuff we
   // don't have....
   string retstring = "-*-" + m_oFace;
   if (bBold)
       retstring += string("-bold-");
   else
       retstring += string("-medium-");
   if (bItalic)
       retstring += string("i-normal-*-");
   else 
       retstring += string("r-normal-*-");
   char tempstr[16];
   sprintf(tempstr, "%d", iFontHeight);
   retstring += string(tempstr) + string("-*");
   return retstring;
}

GTKFont::~GTKFont(void)
{
    gdk_threads_enter();
    if (type == kFontTypeGdk) {
        if (gfont)
            gdk_font_unref(gfont);
    }
#ifdef HAVE_FREETYPE
    else {
        if (ttfont) {
            Efont_free(ttfont);
         }
    }
#endif
    gdk_threads_leave();
}
