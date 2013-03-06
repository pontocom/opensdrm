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

   $Id: ThemeManager.cpp,v 1.1 2004/10/13 16:17:06 linha2 Exp $
____________________________________________________________________________*/ 

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>
#include "ThemeManager.h"
#include "ThemeZip.h"
#include "utility.h"
#include "win32impl.h"

#ifdef __QNX__
#include <strings.h>
#endif

#define THEME_IN_DEVEL "<theme in development>"

ThemeManager::ThemeManager(FAContext *pContext)
{
    char   szThemePath[_MAX_PATH];
    uint32 len = _MAX_PATH;
    Error eRet;

    m_pContext = pContext;
    m_oCurrentTheme = "";
    m_bDevelTheme = false;

    szThemePath[0] = 0;
    eRet = pContext->prefs->GetPrefString(kThemePathPref, szThemePath, &len);
    if (IsError(eRet) || strlen(szThemePath) == 0) {
        m_oCurrentTheme = BRANDING_DEFAULT_THEME;
    }
    else {
        struct stat buf;

        m_oCurrentTheme = szThemePath;
        if (stat(szThemePath, &buf) == 0 && S_ISDIR(buf.st_mode)) {
            m_bDevelTheme = true; 
            m_oDevelTheme = m_oCurrentTheme;
            m_oCurrentTheme = THEME_IN_DEVEL;
        }
    }
}

ThemeManager::~ThemeManager(void)
{
}

Error ThemeManager::GetDefaultTheme(string &oThemePath)
{
    map<string, string> oThemeList;

    GetThemeList(oThemeList);

    char *themeName = new char[strlen(BRANDING_DEFAULT_THEME) + 1];
    strcpy(themeName, BRANDING_DEFAULT_THEME);
    char *dot;

    if ((dot = strchr(themeName, '.')))
        *dot = '\0';

    string strName = themeName;
    oThemePath = oThemeList[strName];

    delete [] themeName;

    return kError_NoErr;
}

Error ThemeManager::GetThemeList(map<string, string> &oThemeFileMap)
{
    WIN32_FIND_DATA find;
    HANDLE          handle;
    char            dir[MAX_PATH], *ptr;
    uint32          len = sizeof(dir);
    string          oThemePath, oThemeBasePath, oThemeFile;

    if (m_bDevelTheme)
        oThemeFileMap[THEME_IN_DEVEL] = m_oDevelTheme;

    m_pContext->prefs->GetPrefString(kInstallDirPref, dir, &len);
    oThemeBasePath = string(dir) + "/" + BRANDING_SHARE_PATH + "/themes";
    oThemePath = oThemeBasePath + string("/*.*");    

    handle = FindFirstFile((char *)oThemePath.c_str(), &find);
    if(handle != INVALID_HANDLE_VALUE) {
        do {
   	    oThemeFile = oThemeBasePath + string("/") + string(find.cFileName);
    	    ptr = strrchr(find.cFileName, '.');
            if (ptr) {
               *ptr = 0;
               ptr++;
            }
            if (ptr && *ptr) {
                if (!strcasecmp("fat", ptr) || !strcasecmp("zip", ptr) ||
                    !strcasecmp("wsz", ptr))
                    oThemeFileMap[find.cFileName] = oThemeFile;
            }
        }
        while(FindNextFile(handle, &find));
        FindClose(handle);
    }

    char *fadir = FreeampDir(NULL);
    oThemeBasePath = string(fadir) + string("/themes");
    delete [] fadir;

    struct stat st;
    if (-1 == stat(oThemeBasePath.c_str(), &st))
        mkdir(oThemeBasePath.c_str(), 0755);
    oThemePath = oThemeBasePath + string("/*.*");
    handle = FindFirstFile((char *)oThemePath.c_str(), &find);
    if (handle != INVALID_HANDLE_VALUE) {
        do {
            oThemeFile = oThemeBasePath + string("/") + string(find.cFileName);
            ptr = strrchr(find.cFileName, '.');
            if (ptr) {
               *ptr = 0;
               ptr++;
            }
            if (ptr && *ptr) {
                if (!strcasecmp("fat", ptr) || !strcasecmp("zip", ptr) ||
                    !strcasecmp("wsz", ptr))
                    oThemeFileMap[find.cFileName] = oThemeFile;
            }
        }
        while(FindNextFile(handle, &find));
        FindClose(handle);
    }

    oThemeBasePath = "./themes";
    oThemePath = oThemeBasePath + string("/*.*");
    handle = FindFirstFile((char *)oThemePath.c_str(), &find);
    if (handle != INVALID_HANDLE_VALUE) {
        do {
            oThemeFile = oThemeBasePath + string("/") + string(find.cFileName);
            ptr = strrchr(find.cFileName, '.');
            if (ptr) {
               *ptr = 0;
               ptr++;
            }
            if (ptr && *ptr) {
                if (!strcasecmp("fat", ptr) || !strcasecmp("zip", ptr) ||
                    !strcasecmp("wsz", ptr))
                    oThemeFileMap[find.cFileName] = oThemeFile;
            }
        }
        while(FindNextFile(handle, &find));
        FindClose(handle);
    }

    return kError_NoErr;
}

Error ThemeManager::UseTheme(string &oThemeFile)
{
    char   dir[_MAX_PATH];
    string oThemePath;

    char *temp_dir;
    temp_dir = strrchr(oThemeFile.c_str(), '/');
    if (temp_dir) {
        temp_dir = temp_dir + 1;
        strcpy(dir, temp_dir);
        if (oThemeFile == m_oCurrentTheme)
        {
            return kError_NoErr;
        }
    }

    m_pContext->prefs->SetPrefString(kThemePathPref, (char *)oThemeFile.c_str());
    m_oCurrentTheme = string(dir);
    
    return kError_NoErr;
}

Error ThemeManager::AddTheme(string &oThemeFile, bool bRename)
{
    string oThemeDest;
    bool   bRenameFailed = false;
    char fcopy[_MAX_PATH], *filename = NULL, *ext = NULL;

    char *fadir = FreeampDir(NULL);
    oThemeDest = string(fadir) + string("/themes");
    delete [] fadir;

    if (bRename)
    {
        ThemeZip oZip;
        string   oName;

        if (IsntError(oZip.GetDescriptiveName(oThemeFile, oName)) &&
            oName.length() > 0)
        {
            int i;

            for(i = oName.length() - 1; i >= 0; i--)
                if (!isalnum(oName[i]) && oName[i] != ' ')
                    oName.erase(i, 1);

            oThemeDest += string("/") + oName + string(".fat");
        }
        else
            bRenameFailed = true;
    }

    if (!bRename || bRenameFailed)
    {
        strcpy(fcopy, oThemeFile.c_str());
        filename = strrchr(fcopy, '/');
        if (filename)
            filename = filename + 1;
        else
            filename = fcopy;
        ext = strrchr(filename, '.');
        if (ext) {
            *ext = '\0';
            ext++;
        }

        oThemeDest += string("/");
        if (!filename)
            oThemeDest += string("unknown");
        else
            oThemeDest += string(filename);
        if (ext)
            oThemeDest += string(".") + string(ext);   
    }

    if (!CopyFile(oThemeFile.c_str(), oThemeDest.c_str(), false))
        return kError_FileNotFound;

    // So the caller knows where the theme ended up
    oThemeFile = oThemeDest;
    
    return kError_NoErr;
}

Error ThemeManager::DeleteTheme(string &oThemeFile)
{
    return unlink(oThemeFile.c_str()) == 0 ? kError_NoErr : kError_UnlinkFailed;
}

Error ThemeManager::GetCurrentTheme(string &oTheme)
{
    oTheme = m_oCurrentTheme;
    return kError_NoErr;
}
