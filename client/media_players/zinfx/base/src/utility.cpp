/*____________________________________________________________________________
    
    FreeAmp - The Free MP3 Player

    Portions Copyright (C) 1999 EMusic.com

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
    
    $Id: utility.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786) 
#endif

#include <assert.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>

#include <string>
#include <vector>

using namespace std;

#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#include <windows.h>
#include <direct.h>
#include <shlobj.h>
#define MKDIR(z) mkdir(z)
#else
#include "win32impl.h"
#include "browser.h"
#include <unistd.h>
#define MKDIR(z) mkdir(z, 0755)
#define _stat stat
#ifndef _S_IFDIR
#define _S_IFDIR S_IFDIR 
#endif
#endif

#ifdef __QNX__
#include <strings.h>
#endif

#include "config.h"
#include "facontext.h"
#include "utility.h"
#include "errors.h"
#include "properties.h"

void CreateDirectoryPath(const char* path)
{
    char* temp = new char[strlen(path) + 1];

    // make a copy we can destroy
    strcpy(temp, path);

    char* cp = temp;
    char* end;

    do
    {
        end = strchr(cp, DIR_MARKER);

        if(end)
            *end = 0x00;

        MKDIR(temp);

        if(end)
        {
            *end = DIR_MARKER;
            cp = end + 1;
        }

    }while(end);

    delete [] temp;
}

bool IsRelative(const char* path)
{
    bool result = false;

    assert(path);

    if(path)
    {
        result = (  path[0] != DIR_MARKER );

#ifdef WIN32
       if(result && strlen(path) > 2)
       {
            result = !(path[1] == ':' && path[2] == '\\');
       }
#endif
    }

    return result;
}

void ResolvePath(char** path)
{
    assert(path);
    assert(*path);

    if(IsRelative(*path))
    {
        // need to get cwd and resolve this path in
        // relation to it.
        char* cwd = new char[_MAX_PATH];

        getcwd(cwd, _MAX_PATH);

        char* fullPath = new char[strlen(cwd) + strlen(*path) + 2];
        
        strcpy(fullPath, cwd);
        strcat(fullPath, DIR_MARKER_STR);
        strcat(fullPath, *path);

        delete [] *path;

        *path = fullPath;

        delete [] cwd;
    }

    char* cp = *path;

    // remove multiple separators between elements
    // and make sure end does not contain separator
    // unless it is the root directory and remove
    // relative indicators in the *path

    
    bool separator = false;

    char dotslash[3] = ".\\";
    char dotdotslash[4] = "..\\";

    dotslash[1] = DIR_MARKER;
    dotdotslash[2] = DIR_MARKER;

#ifdef WIN32

    // network path, skip initial double slash
    if(strlen(cp) > 1 && cp[0] == '\\' && cp[1] == '\\')
    {
        cp += 2;
    }

#endif

    while(*cp)
    {
        if(cp[0] == DIR_MARKER)
        {
            if(separator)
            {
                //memcpy(cp - 1, cp, strlen(cp) + 1);
                strcpy(cp - 1, cp);
                continue;
            }

            separator = true;
        }
        else
        {
            separator = false;

            if(cp[0] == '.')
            {
               
                if(!strncmp(cp, dotslash, 2))
                {
                    strcpy(cp, cp + 2);
                    continue;
                }
                else if(!strncmp(cp, dotdotslash, 3) && cp != *path)
                {
                    char* lastDir;
                    bool found = false;

                    lastDir = cp - 2;

                    while(lastDir > *path)
                    {
                        if(*lastDir == DIR_MARKER)
                        {
                            lastDir++;
                            found = true;
                            break;
                        }

                        lastDir--;
                    }

                    if(found || lastDir == *path)
                    {
                        strcpy(lastDir, cp + 3);

                        cp = lastDir;
                    }

                    continue;
                }
            }
        }

        cp++;
    }

    // check the end for stragglers
    if(*(cp - 1) == '.')
    {

        if( cp - 1 != *path && 
            *(cp - 2) == '.' && 
            cp - 2 != *path) 
        {
            *(cp - 2) = 0x00;
            cp = cp - 2;
        }
        else if(cp - 1 != *path && 
                *(cp - 2) != '.') 
        {
            *(cp - 1) = 0x00;
            cp = cp - 1;
        }
    }

#ifdef WIN32
    if(*(cp - 1) == DIR_MARKER && 
        cp - 1 != *path &&
        *(cp - 2) != ':') 
    {
        *(cp - 1) = 0x00;

    }

#else
    if(*(cp - 1) == DIR_MARKER && 
        cp - 1 != *path) 
    {
        *(cp - 1) = 0x00;
    }
#endif
    
}

void RFC822GMTTimeString(struct tm* time, char buf[32])
{
    const char* day[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char* month[] = {"Jan", "Feb",  "Mar", "Apr", "May", "Jun",  
                           "Jul", "Aug", "Sep", "Oct",  "Nov", "Dec"};

    // Example: Sun, 06 Nov 1994 08:49:37 GMT
    const char* k822Format = "%s, %02d %s %4d %02d:%02d:%02d GMT";

    sprintf(buf, k822Format, day[time->tm_wday],
                             time->tm_mday,
                             month[time->tm_mon],
                             time->tm_year + 1900,
                             time->tm_hour,
                             time->tm_min,
                             time->tm_sec);
}

char *strdup_new(const char *str)
{
    char *n;

    n = new char[strlen(str) + 1];
    strcpy(n, str);
 
    return n;
}

#ifdef WIN32

char *FreeampDir(Preferences *pref)
{
    char path[_MAX_PATH] = {0x00};
    uint32 len = sizeof(path);
    char *s;

    pref->GetPrefString(kInstallDirPref, path, &len);

    s = new char[strlen(path) + 1];
    strcpy(s, path);

    return s;
}

#elif defined(__BEOS__)

#include <be/storage/FindDirectory.h>
#include <be/storage/Path.h>

char *FreeampDir(Preferences *pref)
{
    BPath prefPath;
    assert(find_directory(B_USER_SETTINGS_DIRECTORY, &prefPath) == B_NO_ERROR);
    prefPath.Append( "freeamp" );

    const char *path = prefPath.Path();
    char *s = new char[strlen(path) + 1];
    strcpy(s, path);

    return s;
}

#else

#include <stdlib.h>

char *FreeampDir(Preferences *prefs)
{
    char *homeDir = getenv("HOME");
    const char *fadir = "/."BRANDING_APP_NAME;
    char *s;

    if (!homeDir) {
        s = new char[2];
        strcpy(s, "/");
        return s;
    }

    s = new char[strlen(homeDir) + strlen(fadir) + 1];
    strcpy(s, homeDir);
    strcat(s, fadir);

    return s;
}
#endif

const char* protocol = "file://";

Error FilePathToURL(const char* path, char* url, uint32* length)
{
    Error result = kError_InvalidParam;

    assert(path);
    assert(url);
    assert(length);

    if(path && url && length)
    {
        result = kError_BufferTooSmall;
        int32 extra = 0;

#ifdef WIN32
         // is this relative or network path
         if(path[0] == '\\' && path[1] != '\\') 
         {
             extra = 2;
         }
#endif

        if(*length >= strlen(path) + strlen(protocol) + 1 + extra)
        {

            strcpy(url, protocol);

#ifdef WIN32
            if(extra)
            {
                int32 drive = _getdrive();
           
                url[7] = drive + 'A' - 1;
                url[8] = '|';
                url[9] = 0x00;
            }
#endif

            strcat(url, path);

#ifdef WIN32
            if(strlen(path) > 1 && path[1] == ':')
            {
                url[8] = '|';
            }

            for(int32 index = strlen(url) - 1; index >=0; index--)
            {
                if(url[index] == '\\')
                    url[index] = '/';
            }
#endif  
            result = kError_NoErr;
        }

        *length = strlen(path) + strlen(protocol) + 1 + extra;
    }

    return result;
}

Error URLToFilePath(const char* url, char* path, uint32* length)
{
    Error result = kError_InvalidParam;

    assert(path);
    assert(url);
    assert(length);

    if(path && url && length && !strncasecmp(url, protocol, strlen(protocol)))
    {
        result = kError_BufferTooSmall;

        if(*length >= strlen(url) - strlen(protocol) + 1)
        {
            strcpy(path, url + strlen(protocol));
#ifdef WIN32
            if(strlen(path) > 1 && path[1] == '|')
            {
                path[1] = ':';
            }

            for(int32 index = strlen(path) - 1; index >=0; index--)
            {
                if(path[index] == '/')
                    path[index] = '\\';
            }
#endif 
            result = kError_NoErr; 
        }

        *length = strlen(url) - strlen(protocol) + 1;
    }

    return result;
}

void ToUpper(char *s)
{
    char *p;
    
    for(p = s; *p != '\0'; p++)
       *p = toupper(*p);
}       

void ToLower(char *s)
{
    char *p;
    
    for(p = s; *p != '\0'; p++)
       *p = tolower(*p);
}      

void ReplaceSpaces(string &in, string &encoded)
{
    encoded = "";
    
    for (unsigned int i = 0; i < in.size(); i++)
    {
        if (in[i] == ' ')
            encoded += "%20";
        else if ((unsigned char)in[i] > 127)
        {
            char enc[10];
            sprintf(enc, "%%%02X", in[i] & 0xFF);
            enc[3] = 0;
            encoded += enc;
        }
        else
            encoded += in[i];
    }
}

#ifdef WIN32
#elif __BEOS__

#include <be/app/Roster.h>
#include <be/be_apps/NetPositive/NetPositive.h>

void LaunchBrowser(const char* url)
{
    status_t err;

    BMessenger netpositive(B_NETPOSITIVE_APP_SIGNATURE, -1, &err);
    if (err == B_OK)
    {
        BMessage msg(B_NETPOSITIVE_OPEN_URL);
        msg.AddString("be:url", url);
        err = netpositive.SendMessage(&msg);
        if (err < B_OK)
        {
            printf("error sending msg to netpositive: %s\n", strerror(err));
        }
    }
    else
    {
        const char *browser = "NetPositive";
        char *command = new char[strlen(browser) + strlen(url) + 10];
        sprintf(command, "%s \"%s\" &", browser, url);
        system(command);
        delete[] command;
    }
}

#else
void LaunchBrowser(const char* url)
{
    if (fork() > 0) 
    {
       return;
    }
    launch_browser(url, eBrowserNetscape);
}
#endif


void FindMusicFiles(const char* rootPath, 
                    vector<string>& urls, 
                    vector<string>& queries)
{
    HANDLE findFileHandle = NULL;
    WIN32_FIND_DATA findData;
    string findPath;
    string::size_type pos = string::npos;

    vector<string>::iterator query = queries.begin();

    // first run each query on this directory
    for(;query != queries.end(); query++)
    {
        findPath = rootPath;
        findPath += DIR_MARKER_STR;
        pos = findPath.size();
        findPath += *query;

        findFileHandle = FindFirstFile((char *)findPath.c_str(), &findData);

        if(findFileHandle != INVALID_HANDLE_VALUE)
        {
            do
            {
                findPath.replace(pos, 
                                 findPath.size() - pos, 
                                 findData.cFileName);
                
                char url[MAX_PATH + 8];
                uint32 length = sizeof(url);

                FilePathToURL(findPath.c_str(), url, &length);

                urls.push_back(url);

            }while(FindNextFile(findFileHandle, &findData));

            FindClose(findFileHandle);
        }
    }

    // next find all the directories in this directory and
    // and run the queries on them
#ifdef WIN32
    findPath.replace(pos, 
                     findPath.size() - pos, 
                     "*.*");
#else
    findPath.replace(pos,
                     findPath.size() - pos,
                     "*");
#endif

    findFileHandle = FindFirstFile((char *)findPath.c_str(), &findData);

    if(findFileHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(strcmp(findData.cFileName, ".") && 
               strcmp(findData.cFileName, ".."))
            {
                findPath.replace(pos, 
                                 findPath.size() - pos, 
                                 findData.cFileName);
                struct stat st;

                stat(findPath.c_str(), &st);

                if(st.st_mode & _S_IFDIR)
                {
                    FindMusicFiles(findPath.c_str(),
                                   urls,
                                   queries);
                }
            }

        }while(FindNextFile(findFileHandle, &findData));

        FindClose(findFileHandle);
    }
}

#ifdef WIN32
bool ResolveLink(string& path)
{
    bool result = false;
    HRESULT hres = NULL;

    hres = CoInitialize(NULL);

    if(SUCCEEDED(hres))
    {
        IShellLink* psl = NULL;

        // Get a pointer to the IShellLink interface
        hres = CoCreateInstance(CLSID_ShellLink, 
                                NULL, 
                                CLSCTX_INPROC_SERVER, 
                                IID_IShellLink, 
                                (void**)&psl);

        if(SUCCEEDED(hres))
        {
            IPersistFile* ppf;

            // Get a pointer to the IPersistFile interface
            hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);

            if(SUCCEEDED(hres))
            {
                WORD wsz[MAX_PATH];

                // Ensure string is UNICODE
                MultiByteToWideChar(CP_ACP, 
                                    0, 
                                    path.c_str(), 
                                    -1, 
                                    wsz, 
                                    MAX_PATH);

                // Load Shortcut
                hres = ppf->Load(wsz, STGM_READ);

                if(SUCCEEDED(hres))
                {
                    // Resolve the link
                    hres = psl->Resolve(NULL, SLR_ANY_MATCH|SLR_NO_UI);
            
                    if(SUCCEEDED(hres))
                    {
                        WIN32_FIND_DATA wfd;
                        char buf[MAX_PATH];

                        // Resolve the link
                        hres = psl->GetPath(buf,sizeof(buf),&wfd, 0);

                        if(SUCCEEDED(hres))
                        {
                            path = buf;
                            result = true;
                        }
                    }
                }

                // Release the IPersist Interface
                ppf->Release();
            }
    
            // Release the IShellLink Interface
            psl->Release();
        }

        CoUninitialize(); 
    }

    return result;
}

#endif

#ifndef WIN32

const unsigned iCopyBufferSize = 8192;

bool CopyFile(const char *pExistingFileName, 
              const char *pNewFileName,      // name of new file
              bool bFailIfExists)      // operation if file exists
{
    FILE  *fpDest, *fpSrc;
    char   szBuffer[iCopyBufferSize];
    unsigned iRet;

    if (bFailIfExists && !access(pNewFileName, 0))
        return false;

    fpDest = fopen(pNewFileName, "wb");
    if (fpDest == NULL)
        return false;

    fpSrc = fopen(pExistingFileName, "rb");
    if (fpSrc == NULL)
        return false;

    for(;;)
    {
        iRet = fread(szBuffer, 1, iCopyBufferSize, fpSrc);
        if ((int)iRet < 0)
        {
            fclose(fpDest);
            fclose(fpSrc);
            return false;
        }

        if (fwrite(szBuffer, 1, iRet, fpDest) != iRet)
        {
            fclose(fpDest);
            fclose(fpSrc);
            return false;
        }
        if (iRet != iCopyBufferSize)
           break; 
    }

    fclose(fpDest);
    fclose(fpSrc);

    return true;
}

#endif

string FindFile(string oPath)
{
    char *findpath, *path, *filename, *slash;
    string retvalue = oPath;

    path = new char[_MAX_PATH];
    strcpy(path, oPath.c_str());

    slash = strrchr(path, DIR_MARKER);

    if (!slash)
        return retvalue;

    slash++;
    filename = new char[strlen(slash) + 1];
    strcpy(filename, slash);
    *slash = '\0';

    findpath = new char[strlen(path) + 1];
    strcpy(findpath, path);

    strcat(findpath, "*");
#ifdef WIN32
    strcat(findpath, ".*");
#endif

    WIN32_FIND_DATA find;
    HANDLE handle;

    handle = FindFirstFile(findpath, &find);
    if (handle != INVALID_HANDLE_VALUE) {
        do {
            if (!strcasecmp(find.cFileName, filename)) {
                retvalue = string(path) + string(find.cFileName);
                break;
            }
        }
        while (FindNextFile(handle, &find));
        FindClose(handle);
    }

    delete [] findpath;
    delete [] path;
    delete [] filename;

    return retvalue;
}

bool ShowHelp(FAContext *m_context, const char *helpurl)
{
    string  oHelpFile;
    char   *dir;
    uint32  len = _MAX_PATH;

    dir = new char[_MAX_PATH];

    m_context->prefs->GetPrefString(kInstallDirPref, dir, &len);
    oHelpFile = string(dir);

    oHelpFile += string(DIR_MARKER_STR);
#ifdef WIN32
    oHelpFile += string("help");
    oHelpFile += string(DIR_MARKER_STR);
#endif
#ifdef unix
    oHelpFile += string("../share/"BRANDING_APP_NAME"/help/");
#endif
    oHelpFile += string(helpurl);

    struct _stat   st;
    if (_stat(oHelpFile.c_str(), &st) != 0 || st.st_mode & S_IFREG == 0)
    {
         delete [] dir;
         return false;
    }

    len = _MAX_PATH;
    FilePathToURL(oHelpFile.c_str(), dir, &len);

#ifdef HAVE_GTK 
    LaunchBrowser(dir);
#endif
#ifdef WIN32

    Int32PropValue *pProp;
    HWND            hWnd;
    if (IsError(m_context->props->GetProperty("MainWindow", 
                (PropValue **)&pProp)))
       hWnd = NULL;
    else
       hWnd = (HWND)pProp->GetInt32();

    ShellExecute(hWnd, "open", dir, NULL, NULL, SW_SHOWNORMAL);
#endif
    delete [] dir;

    return true;
} 

bool GetProxySettings(FAContext *context, string &server, unsigned short &port)
{
    bool   useProxy;
    int32  numFields;
    uint32 length;
    char   proxyname[256], hostname[256];

    context->prefs->GetPrefBoolean(kUseProxyPref, &useProxy);

    length = sizeof(proxyname);
    context->prefs->GetPrefString(kProxyHostPref, proxyname, &length);

    if(useProxy)
    {
        numFields = sscanf(proxyname,
                           "http://%[^:/]:%hu", hostname, &port);
        if (numFields > 0)
        {
            server = string(hostname);
            if (numFields == 1)
               port = 80;

            return true;
        }
    }

    return false;
}
