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
        
   $Id: kjofol.cpp,v 1.1 2004/10/13 16:16:05 linha2 Exp $
____________________________________________________________________________*/

#include "config.h"

#include <string>
using namespace std;

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include "win32impl.h"
#endif

#ifdef __QNX__
#include <strings.h>
#endif

#ifdef WIN32
#include "Win32Bitmap.h"
#elif defined(HAVE_GTK)
#include "GTKBitmap.h"
#elif defined(__BEOS__)
#include "BeOSBitmap.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include "facontext.h"
#include "errors.h"
#include "utility.h"

#include "kjofol.h"

#include "Bitmap.h"

extern "C"
{
   ThemeFormat *Initialize(FAContext* context)
   {
      return new KJofol(context);
   }
}


static char deffontmap[] = 
"            <FontMap Map=\"ABCDEFGHIJKLMNOPQRSTUVWXYZ`@@@ \"/>\n"
"            <FontMap Map=\"0123456789 _:()-'!_+\\/[]^&%.=$#\"/>\n"
"            <FontMap Map=\"ежд?*                          \"/>";
static char timefontmap[] =
"            <FontMap Map=\"0123456789: \"/>";
static char volfontmap[] =
"            <FontMap Map=\"0123456789% \"/>";

KJofol::KJofol(FAContext *context):ThemeFormat(context)
{
}

KJofol::~KJofol()
{
}
 
bool KJofol::IsSupportedFormat(string &oDir)
{
    string testpath;

    testpath = FindRCFile(oDir);
    if (testpath == "")
        return false;
    return true;
}

Error KJofol::ConvertToNative(string &oDir)
{
    m_bmpvolume = false;
    m_understandvolume = false;
    haswinshade = false;
    hasdock = false;

    string outpath = oDir + string(DIR_MARKER_STR) + string("theme.xml");
    string inpath = FindRCFile(oDir);
 
    outfile = fopen(outpath.c_str(), "w");

    if (!outfile)
        return kError_FileNoAccess;

    BuildImageList(oDir);

    Write("\n");
    Write("<ThemeInfo Name=\"Converted KJofol Skin\" Author=\"Isaac Richards\"\n");
    Write("           EMail=\"info@freeamp.org\" WebPage=\"http://www.freeamp.org\"/>\n\n");

    ParseRCFile(inpath, "MainWindow");

    if (haswinshade) {
        inpath = oDir + string(DIR_MARKER_STR) + winshadercfilename;
        ParseRCFile(inpath, "ShadeMode");
    } 

    if (hasdock) {
        inpath = oDir + string(DIR_MARKER_STR) + dockrcfilename;
        ParseRCFile(inpath, "DockMode", true);
    }      

    fclose(outfile);
//cout << "done outputting " << outpath << endl;

    return kError_NoErr;
}

void KJofol::ParseRCFile(string rcfile, string windowname, bool dock)
{
    char line[_MAX_PATH];
    char outline[_MAX_PATH];
    char left[_MAX_PATH], right[_MAX_PATH];
    int seekfields = 0;
    string seekinfo;

    infile = fopen(rcfile.c_str(), "r");
    if (!infile)
        return;

    sprintf(outline, "<Window Name=\"%s\">\n", windowname.c_str());
    Write(outline);
    if (dock) {
        sprintf(outline, "<DockPosition Pos=\"%d, %d\"/>\n", dockposxy.x, 
                dockposxy.y);
        Write(outline, 1);
    }

    while (IsntError(GetNextLine(line))) {
        MunchLine(line, left, right);
        if (!strcasecmp(left, "BackgroundImage")) {
            info["Back"] = right;
            Pos pos = bmp_sizes[right];
            sprintf(left, "0, 0, %d, %d", pos.x, pos.y);
            sprintf(outline, "<BackgroundBitmap Name=\"%s\" Rect=\"%s\"/>\n", right, left);
            Write(outline, 1);
            Write("<Controls>\n", 1);
        }
        else if (!strcasecmp(left, "BackgroundImageInactive")) {
            info["Inactive"] = right;
        }
        else if (!strcasecmp(left, "BackgroundImagePressed1")) {
            info["BMP1"] = right;
        }
        else if (!strcasecmp(left, "BackgroundImagePressed2")) {
            info["BMP2"] = right;
        }
        else if (!strcasecmp(left, "BackgroundImagePressed3")) {
            info["BMP3"] = right;
        }
        else if (!strcasecmp(left, "CloseButton")) {
            Button(right, "Quit", "Quit FreeAmp", "Quit");
        }
        else if (!strcasecmp(left, "MinimizeButton")) {
            Button(right, "Minimize", "Minimize FreeAmp", "Minimize");
        }
        else if (!strcasecmp(left, "AboutButton")) {
            Button(right, "Options", "Open the options window", "Options");
        }
        else if (!strcasecmp(left, "OpenFileButton")) {
            Button(right, "Files", "Open files to play", "Files");
        }
        else if (!strcasecmp(left, "StopButton")) {
            Button(right, "Stop", "Stop the current track", "Stop");
        }
        else if (!strcasecmp(left, "PlayButton")) {
            Button(right, "Play", "Play the current track", "Play");
        }
        else if (!strcasecmp(left, "PreviousSongButton")) {
            Button(right, "Prev", "Move to previous track", "Previous");
        }
        else if (!strcasecmp(left, "NextSongButton")) {
            Button(right, "Next", "Move to next track", "Next");
        }
        else if (!strcasecmp(left, "PauseButton")) {
            Button(right, "Pause", "Pause the current track", "Pause");
        }
        else if (!strcasecmp(left, "PreferencesButton")) {
            Button(right, "Options", "Open the optionswindow", "Options");
        }
        else if (!strcasecmp(left, "PlaylistButton")) {
            Button(right, "MyMusic", "Browse my music collection", "MyMusic");
        }
        else if (!strcasecmp(left, "DockModeButton")) {
            Button(right, "DockView", "Change to Dock view", "Dock view",
                   true, "<ChangeWindow Window=\"DockMode\"/>\n");
        }
        else if (!strcasecmp(left, "UnDockModeButton")) {
            Button(right, "UnDock", "Change to Normal view", "Normal view",
                   true, "<ChangeWindow Window=\"MainWindow\"/>\n");
        }
        //else if (!strcasecmp(left, "")) {
        //    Button(right, "MiniView", "Change to Shade view", "Shade view",
        //           true, "<ChangeWindow Window=\"ShadeMode\"/>\n");
        //}
        else if (!strcasecmp(left, "FontImage")) {
            KJofol_Font f = fonts["Default"];
            f.image = right;
            f.fontmap = deffontmap;
            fonts["Default"] = f;
        }
        else if (!strcasecmp(left, "FontSize")) {
            KJofol_Font f = fonts["Default"];
            ParsePosition(right, f.size); 
            f.width = f.size.x * 31 -1;
            f.height = f.size.y * 3 - 1;
            fonts["Default"] = f;
        }
        else if (!strcasecmp(left, "FontSpacing")) {
            KJofol_Font f = fonts["Default"];
            f.spacing = atoi(right);
            fonts["Default"] = f;
        }
        else if (!strcasecmp(left, "FontTransparent")) {
            KJofol_Font f = fonts["Default"];
            f.transparent = atoi(right);
            fonts["Default"] = f;
        }
       else if (!strcasecmp(left, "TimeFontImage")) {
            KJofol_Font f = fonts["Time"];
            f.image = right;
            f.fontmap = timefontmap;
            fonts["Time"] = f;
        }
        else if (!strcasecmp(left, "TimeFontSize")) {
            KJofol_Font f = fonts["Time"];
            ParsePosition(right, f.size);
            f.width = f.size.x * 12 - 1;
            f.height = f.size.y - 1;
            fonts["Time"] = f;
        }
        else if (!strcasecmp(left, "TimeFontSpacing")) {
            KJofol_Font f = fonts["Time"];
            f.spacing = atoi(right);
            fonts["Time"] = f;
        }
        else if (!strcasecmp(left, "TimeFontTransparent")) {
            KJofol_Font f = fonts["Time"];
            f.transparent = atoi(right);
            fonts["Time"] = f;
        }
       else if (!strcasecmp(left, "VolumeFontImage")) {
            KJofol_Font f = fonts["Volume"];
            f.image = right;
            f.fontmap = volfontmap;
            fonts["Volume"] = f;
        }
        else if (!strcasecmp(left, "VolumeFontSize")) {
            KJofol_Font f = fonts["Volume"];
            ParsePosition(right, f.size);
            f.width = f.size.x * 12 - 1;
            f.height = f.size.y - 1;
            fonts["Volume"] = f;
        }
        else if (!strcasecmp(left, "VolumeFontSpacing")) {
            KJofol_Font f = fonts["Volume"];
            f.spacing = atoi(right);
            fonts["Volume"] = f;
        }
        else if (!strcasecmp(left, "VolumeFontTransparent")) {
            KJofol_Font f = fonts["Volume"];
            f.transparent = atoi(right);
            fonts["Volume"] = f;
        }
        else if (!strcasecmp(left, "VolumeText")) {
            HandleTextWindow(right, "VolumeText", fonts["Volume"]);
        }
        else if (!strcasecmp(left, "FilenameWindow")) {
            HandleTextWindow(right, "Title", fonts["Default"]);
        }
        else if (!strcasecmp(left, "MP3KbpsWindow")) {
            HandleTextWindow(right, "BitRate", fonts["Default"]);
        }
        else if (!strcasecmp(left, "MP3KbpsString")) {
        }
        else if (!strcasecmp(left, "MP3KhzWindow")) {
            HandleTextWindow(right, "SampleRate", fonts["Default"]);
        }
        else if (!strcasecmp(left, "MP3KhzString")) {
        }
        else if (!strcasecmp(left, "MP3TimeWindow")) {
            HandleTimeWindow(right, "Time", fonts["Time"]);
        }
        else if (!strcasecmp(left, "SeekRegion")) {
            ParseRect(right, m_seekrect);
            seekfields++;
        }
        else if (!strcasecmp(left, "SeekImage")) {
            seekinfo = right;
            seekfields++;
        }
        else if (!strcasecmp(left, "VolumeControlType")) {
            if (!strncasecmp(right, "BMP", 3)) {
                m_bmpvolume = true; 
                m_understandvolume = true;
                m_bmpvolfields = 1;
            }
        }
        else if (!strcasecmp(left, "VolumeControlImage")) {
            info["VolumeBMPImage"] = right;
            m_bmpvolfields++;
        }
        else if (!strcasecmp(left, "VolumeControlImagePosition")) {
            info["VolumeBMPPos"] = right;
            m_bmpvolfields++;
        }
        else if (!strcasecmp(left, "VolumeControlImageXSize")) {
            m_volxsize = atoi(right);
            m_bmpvolfields++;
        }
        else if (!strcasecmp(left, "VolumeControlImageNb")) {
            m_volnum = atoi(right);
            m_bmpvolfields++;
        }
        else if (!strcasecmp(left, "VolumeControlButton")) {
            info["VolumeBMPControl"] = right;
            m_bmpvolfields++;
        }
        else if (!strcasecmp(left, "DockModePositionXY")) {
            ParsePosition(right, dockposxy);
        }
        else if (!strcasecmp(left, "DockModeRCFile")) {
            dockrcfilename = right;
            hasdock = true;
        }
        else if (!strcasecmp(left, "WinshadeModeRCFile")) {
            winshadercfilename = right;
            haswinshade = true;
        }
        else if (!strncasecmp(left, "Playlist", 8)) {
        }
        else if (!strcasecmp(left, "About")) {
        }
        else if (!strncasecmp(left, "Pitch", 5)) {
        }

        if (m_bmpvolume && m_bmpvolfields == 6) {
            VolumeBMP();
            m_bmpvolume = false;
        }
        if (seekfields == 2) {
            Seek((char *)seekinfo.c_str());
            seekfields = 0;
        }
    }

    Write("</Controls>\n", 1);
    Write("</Window>\n\n\n");

    fclose(infile);
}

string KJofol::FindRCFile(string &oDir)
{
    string rcfile = "";

    string searchpath = oDir + string(DIR_MARKER_STR) + string("*.rc");

    WIN32_FIND_DATA find;
    HANDLE          handle; 
    
    handle = FindFirstFile((char *)searchpath.c_str(), &find);
    if (handle != INVALID_HANDLE_VALUE) {
        rcfile = oDir + string(DIR_MARKER_STR) + string(find.cFileName);
        FindClose(handle);
    }
    
    return rcfile;
}

Error KJofol::GetNextLine(char *line)
{
    if (!fgets(line, _MAX_PATH, infile))
        return kError_UnknownErr;
    while (!TestForComment(line)) 
        if (!fgets(line, _MAX_PATH, infile))
            return kError_UnknownErr;
    return kError_NoErr;
}

bool KJofol::TestForComment(char *line)
{
    if ((line[0] == '#') || (line[0] == '\n') || (line[0] == '\0') ||
        (line[0] == '\r'))
        return false;
    return true;
}

void KJofol::BuildImageList(string &oDir)
{
    string searchpath = oDir + string(DIR_MARKER_STR) + string("*.*");

    WIN32_FIND_DATA find;
    HANDLE          handle;

    handle = FindFirstFile((char *)searchpath.c_str(), &find);
    if (handle != INVALID_HANDLE_VALUE) {
        do {
            char *ext = strrchr(find.cFileName, '.');
            if (ext) {
                ext++;
                if (strcasecmp("rc", ext) && strcasecmp("xml", ext) &&
                    strcasecmp("zip", ext) && strcasecmp("txt", ext) &&
                    strcasecmp("dck", ext) && strcasecmp("wsh", ext) &&
                    strcasecmp("ttf", ext) && strcasecmp("pls", ext) &&
                    strcasecmp("pl", ext) && find.cFileName[0] != '.') 
                    HandleBitmap(oDir, find.cFileName);
            }
        }
        while (FindNextFile(handle, &find));
        FindClose(handle);
    }
}

void KJofol::Write(char *line, int indent)
{
    char space[] = "    ";
    for (int i = 0; i < indent; i++)
        fwrite(space, strlen(space), sizeof(char), outfile);
    fwrite(line, strlen(line), sizeof(char), outfile);
}

void KJofol::MunchLine(char *orig, char *left, char *right)
{
    if (strlen(left) > 0)
        memset(left, 0, strlen(left));
    if (strlen(right) > 0)
        memset(right, 0, strlen(right));
    sscanf(orig, "%s %[^\r\n]", left, right);
}

void KJofol::ParseButton(char *orig, Rect &oRect, string &name)
{
    char temp1[256];
    char temp2[256];

    //int n = 
    sscanf(orig, "%i %i %i %i %s %s", &oRect.x1, &oRect.y1, &oRect.x2, 
                   &oRect.y2, temp1, temp2);
    //if (n < 6)
    //    cout << "error parsing " << orig << " for button information..\n";
    name = temp2;
}

void KJofol::ParsePosition(char *orig, Pos &oPos)
{
    //int n = 
    sscanf(orig, "%i %i", &oPos.x, &oPos.y);
    //if (n < 2)
    //    cout << "error parsing " << orig << " for position information..\n";
}

void KJofol::ParseRect(char *orig, Rect &oRect)
{
    //int n = 
    sscanf(orig, "%i %i %i %i", &oRect.x1, &oRect.y1, &oRect.x2, &oRect.y2);
    //if (n < 4)
    //    cout << "error parsing " << orig << " for rectangle coords..\n";

    // hack around a broken skin used for testing..
    if (oRect.y2 < oRect.y1) 
        oRect.y2 += 10;
}

void KJofol::HandleBitmap(string &oDir, char *name)
{
    Bitmap *pBitmap;

    string sname = name;
#ifdef WIN32
    pBitmap = new Win32Bitmap(sname);
#elif defined(HAVE_GTK)
    pBitmap = new GTKBitmap(sname);
#elif defined(__BEOS__)
    pBitmap = new BeOSBitmap(sname);
#endif

    string path = oDir + string(DIR_MARKER_STR) + string(name);
    if (IsError(pBitmap->LoadBitmapFromDisk(path))) {
        delete pBitmap;
        return;
    }

    Color color;
    Pos pos;

    pos.x = 0;
    pos.y = 0;
    pBitmap->GetSize(pos);

    bmp_sizes[name] = pos;

    pos.x -= 2;
    pos.y -= 2;
    pBitmap->GetColor(pos, color);

    char trans[32];
    char outline[_MAX_PATH];
    sprintf(trans, "#%02X%02X%02X", color.red, color.green, color.blue);
    sprintf(outline, "<Bitmap Name=\"%s\" File=\"%s\" TransColor=\"%s\"/>\n", 
                     name, name, trans);
    Write(outline);

    delete pBitmap;
}

void KJofol::HandleButton(string name, string inf, string tip,
                          Rect oRect, string bmpname, bool hasextra,
                          string extrainfo)
{
    char outline[_MAX_PATH];
    sprintf(outline, "<ButtonControl Name=\"%s\">\n", name.c_str());
    Write(outline, 2);
    sprintf(outline, "<Info Desc=\"%s\" Tip=\"%s\"/>\n", inf.c_str(), 
            tip.c_str());
    Write(outline, 4);
    sprintf(outline, "<Position Pos=\"%d, %d\"/>\n", oRect.x1, oRect.y1);
    Write(outline, 3);
    sprintf(outline, "<ControlStateBitmap Rect=\"%d, %d, %d, %d\" State=\"Normal\" Name=\"%s\"/>\n", oRect.x1, oRect.y1, oRect.x2, oRect.y2, info["Back"].c_str());
    Write(outline, 3);
    sprintf(outline, "<ControlStateBitmap Rect=\"%d, %d, %d, %d\" State=\"Pressed\" Name=\"%s\"/>\n", oRect.x1, oRect.y1, oRect.x2, oRect.y2, info[bmpname].c_str());
    Write(outline, 3);
    sprintf(outline, "<ControlStateBitmap Rect=\"%d, %d, %d, %d\" State=\"Disabled\" Name=\"%s\"/>\n", oRect.x1, oRect.y1, oRect.x2, oRect.y2, info["Inactive"].c_str());
    Write(outline, 3);
    if (hasextra)
        Write((char *)extrainfo.c_str(), 3);
    sprintf(outline, "</ButtonControl>\n");
    Write(outline, 2);
}

void KJofol::Button(char *desc, string name, string inf, string tip,
                    bool hasextra, string extrainfo)
{
   Rect oRect;
   string bmpname;

   ParseButton(desc, oRect, bmpname);
   HandleButton(name, inf, tip, oRect, bmpname, hasextra, extrainfo);
}

void KJofol::HandleFont(KJofol_Font font)
{
    char outline[_MAX_PATH];
    sprintf(outline, "<ControlBitmap Rect=\"0, 0, %d, %d\" Name=\"%s\"/>\n",
            font.width, font.height, font.image.c_str());
    Write(outline, 3);
    sprintf(outline, "%s\n", font.fontmap);
    Write(outline);
}

void KJofol::HandleTextWindow(char *desc, string name, KJofol_Font font)
{
    char outline[_MAX_PATH];
    sprintf(outline, "<PixmapFontControl Name=\"%s\" IgnoreCase=\"yes\">\n",
            name.c_str());
    Write(outline, 2);

    Rect coord;
    ParseRect(desc, coord);

    sprintf(outline, "<Position Rect=\"%d, %d, %d, %d\"/>\n", coord.x1, 
            coord.y1, coord.x2, coord.y2);
    Write(outline, 3);

    HandleFont(font);

    sprintf(outline, "</PixmapFontControl>\n");
    Write(outline, 2);
}

void KJofol::HandleTimeWindow(char *desc, string name, KJofol_Font font)
{
    char outline[_MAX_PATH];
    Rect coord;

    ParseRect(desc, coord);

    sprintf(outline, "<PixmapTimeControl Name=\"%s\">\n", name.c_str());
    Write(outline, 2);
    sprintf(outline, "<Position Rect=\"%d, %d, %d, %d\"/>\n",
            coord.x1, coord.y1, coord.x2, coord.y2);
    Write(outline, 3);
    sprintf(outline, "<PartName Part=\"MinuteTen\" Name=\"MinuteTens\"/>\n");
    Write(outline, 3);
    sprintf(outline, "<PartName Part=\"MinuteOne\" Name=\"MinuteOnes\"/>\n");
    Write(outline, 3);
    sprintf(outline, "<PartName Part=\"SecondTen\" Name=\"SecondTens\"/>\n");
    Write(outline, 3);
    sprintf(outline, "<PartName Part=\"SecondOne\" Name=\"SecondOnes\"/>\n");
    Write(outline, 3);
    sprintf(outline, "<PartName Part=\"SecondColon\" Name=\"Colons\"/>\n");
    Write(outline, 3);
    sprintf(outline, "</PixmapTimeControl>\n"); 
    Write(outline, 2);

    uint32 offset = font.size.x + font.spacing;
    char position[256];    

    sprintf(position, "%d %d %d %d", coord.x1, coord.y1, coord.x1 + offset,
            coord.y2);
    HandleTextWindow(position, "MinuteTens", font);

    sprintf(position, "%d %d %d %d", coord.x1 + offset, coord.y1, 
            coord.x1 + offset * 2, coord.y2);
    HandleTextWindow(position, "MinuteOnes", font);

    sprintf(position, "%d %d %d %d", coord.x1 + offset * 2, coord.y1, 
            coord.x1 + offset * 3, coord.y2);
    HandleTextWindow(position, "Colons", font);

    sprintf(position, "%d %d %d %d", coord.x1 + offset * 3, coord.y1, 
            coord.x1 + offset * 4, coord.y2);
    HandleTextWindow(position, "SecondTens", font);

    sprintf(position, "%d %d %d %d", coord.x1 + offset * 4, coord.y1, 
            coord.x1 + offset * 5, coord.y2);
    HandleTextWindow(position, "SecondOnes", font);
}

void KJofol::Seek(char *desc)
{
    char outline[_MAX_PATH];
    char rect[256];

    sprintf(rect, "%d, %d, %d, %d", m_seekrect.x1, m_seekrect.y1, m_seekrect.x2,
            m_seekrect.y2);
    
    sprintf(outline, "<PixSliderControl Name=\"Seek\" NumStates=\"1\" Reveal=\"yes\">\n");
    Write(outline, 2);
    sprintf(outline, "<Info Desc=\"Seek in the current track\" Tip=\"Seek\"/>\n");
    Write(outline, 4);
    sprintf(outline, "<Position Rect=\"%s\"/>\n", rect);
    Write(outline, 3);
    sprintf(outline, "<ControlStateBitmap Rect=\"%s\" State=\"Normal\" Name=\"%s\"/>\n", rect, info["Back"].c_str());
    Write(outline, 3);
    sprintf(outline, "<ControlStateBitmap Rect=\"%s\" State=\"Pressed\" Name=\"%s\"/>\n", rect, info["BMP1"].c_str());
    Write(outline, 3);

    Pos pos = bmp_sizes[desc];
    sprintf(rect, "0, 0, %d, %d", pos.x, pos.y);

    sprintf(outline, "<ActivationBitmap Rect=\"%s\" Name=\"%s\"/>\n", rect, desc);
    Write(outline, 3);

    sprintf(outline, "</PixSliderControl>\n");
    Write(outline, 2);
}

void KJofol::VolumeBMP(void)
{
    char outline[_MAX_PATH];
    char rect[256];
    Rect volrect;
    string pixname = info["VolumeBMPImage"];

    sscanf(info["VolumeBMPControl"].c_str(), 
           "%i %i %i %i %s", &volrect.x1, &volrect.y1, &volrect.x2,
           &volrect.y2, rect);
    
    sprintf(rect, "%d, %d, %d, %d", volrect.x1, volrect.y1, volrect.x2,
            volrect.y2);

    sprintf(outline, "<PixSliderControl Name=\"Volume\" NumStates=\"%d\" Reveal=\"no\">\n", m_volnum);
    Write(outline, 2);
    sprintf(outline, "<Info Desc=\"Change Volume\" Tip=\"Volume\"/>\n");
    Write(outline, 4);
    sprintf(outline, "<Position Pos=\"%d, %d\"/>\n", volrect.x1, volrect.y1);
    Write(outline, 3);

    volrect.y1 = 0;
    volrect.y2 = bmp_sizes[pixname].y;
    for (int i = 0; i < m_volnum; i++) {
        volrect.x1 = i * m_volxsize;
        volrect.x2 = (i + 1) * m_volxsize - 1;
        sprintf(rect, "%d, %d, %d, %d", volrect.x1, volrect.y1, volrect.x2, 
                volrect.y2);
        sprintf(outline, "<ControlStateBitmap Rect=\"%s\" State=\"Normal\" StateNum=\"%d\" Name=\"%s\"/>\n", rect, i, pixname.c_str());
        Write(outline, 3);
    }

    Pos pos = bmp_sizes[info["VolumeBMPPos"]];
    sprintf(rect, "0, 0, %d, %d", pos.x, pos.y);

    sprintf(outline, "<ActivationBitmap Rect=\"%s\" Name=\"%s\"/>\n", rect, info["VolumeBMPPos"].c_str());
    Write(outline, 3);

    sprintf(outline, "</PixSliderControl>\n");
    Write(outline, 2);
}

