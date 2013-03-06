/*____________________________________________________________________________
        
        FreeAmp - The Free MP3 Player

        Portions Copyright (C) 1998-1999 EMusic.com
        Portions Copyright (C) 1999 Mark H. Weaver <mhw@netris.org>

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
        
        $Id: unixprefs.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#include "config.h"

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "utility.h"
#include "unixprefs.h"
#include "prefixprefs.h"

// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
// THis module uses the strdup function  to pass strings to the lists class 
// (among others) which will in turn use delete to reclaim the memory.
// This is NOT VALID! A strdup()ed string must be free()ed, not deleted!
// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING

// default values
const char*  kDefaultLibraryPath = ".:~/."BRANDING_APP_NAME":"UNIX_LIBDIR"/"BRANDING_APP_NAME;
const char*  kDefaultUI = "zinf.ui";
const char*  kDefaultTextUI = "freeampcmd.ui";
const char*  kDefaultPMO = "soundcard.pmo";
const char*  kDefaultALSADevice = "0:0";
const char*  kDefaultESOUNDHost = "localhost";


class LibDirFindHandle {
 public:
    vector <char *> *m_pLibDirs;
    int32 m_current;
};


static
Error
ScanQuotableString(const char *in, const char **endPtr,
                   char *out, int32 *lengthPtr, const char *delim)
{
    bool quoted = false;
    int32 length = 0;

    // In case of error, clear return fields now
    if (endPtr)
        *endPtr = 0;
    if (lengthPtr)
        *lengthPtr = 0;

    if (*in == '"')
    {
        in++;
        quoted = true;
    }
    while (*in)
    {
        if (*in == '"' && quoted)
        {
            in++;
            break;
        }
        else if ((isspace(*in) || strchr(delim, *in)) && !quoted)
            break;
        else if (*in == '\\' && quoted)
        {
            char ch;

            in++;
            switch (*in)
            {
                case '\\':  ch = '\\'; in++; break;
                case  '"':  ch =  '"'; in++; break;
                case  'n':  ch = '\n'; in++; break;
                case  't':  ch = '\t'; in++; break;

                default:
                    if (in[0] < '0' || in[0] > '3' ||
                        in[1] < '0' || in[1] > '7' ||
                        in[2] < '0' || in[2] > '7')
                    {
                        *endPtr = in;
                        return kError_SyntaxError;
                    }

                    ch = (char)(((in[0] - '0') << 6) |
                                ((in[1] - '0') << 3) |
                                (in[2] - '0'));
                    in += 3;
                    break;
            }
            if (out)
                *out++ = ch;
            length++;
        }
        else if (*in == '\n')
        {
            if (quoted)
            {
                *endPtr = in;
                return kError_SyntaxError;
            }
            break;
        }
        else
        {
            if (out)
                *out++ = *in;
            length++;
            in++;
        }
    }

    if (out)
        *out++ = '\0';
    if (lengthPtr)
        *lengthPtr = length;
    if (endPtr)
        *endPtr = in;
    return kError_NoErr;
}

static
char *
ReadQuotableString(const char *in, const char **endPtr, const char *delim)
{
    Error error;
    int32 length;

    error = ScanQuotableString(in, endPtr, 0, &length, delim);
    if (IsError(error))
        return 0;

    char *out = new char[length + 1];

    ScanQuotableString(in, 0, out, 0, delim);
    return out;
}

static
void
PrepareQuotableString(const char *str, bool *needQuotes,
                      char *out, int32 *lengthPtr, const char *delim)
{
    int32 unquotedLength = 0;
    int32 quotedLength = 2;
    bool quoted = *needQuotes;

    if (out && quoted)
        *out++ = '"';

    if (*str == '"')
        *needQuotes = true;

    for (; *str; str++)
    {
        if (strchr(delim, *str) || *str == ' ')
        {
            *needQuotes = true;
            quotedLength++;
            if (out)
                *out++ = *str;
        }
        else if (*str == '\\' || *str == '"')
        {
            quotedLength += 2;
            unquotedLength++;
            if (out)
            {
                if (quoted)
                    *out++ = '\\';
                *out++ = *str;
            }
        }
        else if (*str == '\n' || *str == '\t')
        {
            *needQuotes = true;
            quotedLength += 2;
            if (out)
            {
                *out++ = '\\';
                *out++ = (*str == '\n') ? 'n' : 't';
            }
        }
        else if (isspace(*str) || !isgraph(*str))
        {
            *needQuotes = true;
            quotedLength += 4;
            if (out)
            {
                unsigned char ch = *str;

                *out++ = '\\';
                *out++ = '0' + (ch >> 6);
                *out++ = '0' + ((ch >> 3) & 7);
                *out++ = '0' + (ch & 7);
            }
        }
        else
        {
            unquotedLength++;
            quotedLength++;
            if (out)
                *out++ = *str;
        }
    }

    if (out)
    {
        if (quoted)
            *out++ = '"';
        *out++ = '\0';
    }
    if (lengthPtr)
        *lengthPtr = *needQuotes ? quotedLength : unquotedLength;
}

static
char *
WriteQuotableString(const char *str, const char *delim)
{
    int32 length;
    bool needQuotes = false;

    PrepareQuotableString(str, &needQuotes, 0, &length, delim);

    char *out = new char[length + 1];

    PrepareQuotableString(str, &needQuotes, out, 0, delim);
    return out;
}

static
void
AppendToString(char **destPtr, const char *src, int32 length)
{
    char *oldStr = *destPtr;
    int32 oldLen = oldStr ? strlen(oldStr) : 0;
    int32 newLen = oldLen + length;
    char *newStr = new char[newLen + 1];

    if (oldStr)
    {
        strncpy(newStr, oldStr, oldLen);
        delete[] oldStr;
        *destPtr = NULL;
    }
    strncpy(newStr + oldLen, src, length);
    newStr[newLen] = '\0';
    *destPtr = newStr;
}

UnixPrefEntry::
UnixPrefEntry()
{
    prefix = NULL;
    key = NULL;
    separator = NULL;
    value = NULL;
    suffix = NULL;
}

UnixPrefEntry::
~UnixPrefEntry()
{
    if (prefix)    delete [] prefix;
    if (key)       delete [] key;
    if (separator) delete [] separator;
    if (value)     delete [] value;
    if (suffix)    delete [] suffix;
}

static bool file_exists(char *s)
{
    struct stat st;
    if ((!s) || (!*s))
        return false;
    if (stat(s, &st) < 0)
        return false;
    return true;
}

UnixPrefs::
UnixPrefs()
     : m_prefsFilePath(0), m_errorLineNumber(0),
       m_saveEnable(true), m_changed(false)
{
    const char *old_suffix = "/.freeamp_prefs";
    char *old_prefsFilePath;
    const char *fadir = "/."BRANDING_APP_NAME;
    const char *suffix = "/preferences";
    char *homeDir = getenv("HOME"); 

    if (!homeDir)
    {
        m_saveEnable = false;
        return;
    }

    // Compute pathname of preferences file
    old_prefsFilePath = new char[strlen(homeDir) + strlen(old_suffix) + 1];
    strcpy(old_prefsFilePath, homeDir);
    strcat(old_prefsFilePath, old_suffix);

    m_prefsFilePath = new char[strlen(homeDir) + strlen(fadir) + strlen(suffix) 
                               + 1];
    strcpy(m_prefsFilePath, homeDir);
    strcat(m_prefsFilePath, fadir);
    if (!file_exists(m_prefsFilePath))
        mkdir(m_prefsFilePath, S_IRWXU);
    strcat(m_prefsFilePath, suffix);

    if (file_exists(old_prefsFilePath))
        rename(old_prefsFilePath, m_prefsFilePath);

    delete [] old_prefsFilePath;

    FILE *prefsFile = fopen(m_prefsFilePath, "r");
    if (!prefsFile && errno != ENOENT)
    {
        m_saveEnable = false;
        fprintf(stderr, "Error opening %s: %s\n",
                m_prefsFilePath, strerror(errno));
    }

    if (prefsFile)
    {
        UnixPrefEntry *entry = new UnixPrefEntry;
        char buffer[1024];
        char *p;
        int lineNumber = 0;

        while (fgets(buffer, sizeof(buffer) - 1, prefsFile))
        {
            lineNumber++;

            p = buffer;
            while (*p && (*p == ' ' && *p == '\t'))
                p++;

            if (*p == '#')
            {
                // No data on this line, skip to the end of the comment
                    while (*p)
                        p++;
            }

            if (p > buffer)
                AppendToString(&entry->prefix, buffer, p - buffer);

            if (*p)
            {
                char *end;
                // char *out;
                int32 length;
                
                entry->key = ReadQuotableString(p, (const char **)&end, ":#");
                if (entry->key && entry->key[0] == '/')
                    continue;
                else if (entry->key && (m_ht.find(entry->key) == m_ht.end()))
                    m_ht[entry->key] = entry;
                else if (!m_errorLineNumber)
                    m_errorLineNumber = lineNumber;
                p = end;
                
                while (*p && (*p == ' ' || *p == '\t'))
                    p++;
                if (*p == ':')
                    p++;
                else if (!m_errorLineNumber)
                    m_errorLineNumber = lineNumber;
                while (*p && (*p == ' ' || *p == '\t'))
                    p++;
                
                AppendToString(&entry->separator, end, p - end);
                
                entry->value = ReadQuotableString(p, (const char **)&end, "#");
                if (!entry->value && !m_errorLineNumber)
                    m_errorLineNumber = lineNumber;
                p = end;
                length = strlen(p);
                if (p[length - 1] != '\n')
                {
                    p[length++] = '\n';
                    p[length] = '\0';
                }
                AppendToString(&entry->suffix, p, length);
                
                m_entries.push_back(entry);
                entry = new UnixPrefEntry;
            }
        }

        if (entry->prefix)
            m_entries.push_back(entry);
        else
            delete entry;
        
        if (m_errorLineNumber)
            m_saveEnable = false;
        
        fclose(prefsFile);
    }

    SetDefaults();
    Save();

    // Test(this);
}

UnixPrefs::
~UnixPrefs()
{
    Save();

    while (m_entries.size() > 0) {
        delete m_entries[0];
        m_entries.erase(m_entries.begin());
    }
    if (m_libDirs)
        delete [] m_libDirs;
    delete[] m_prefsFilePath;
}

Error
UnixPrefs::
SetDefaults()
{
    char buf[1024];
    uint32 size;

    // set install directory value
    size = sizeof(buf);
    if (GetPrefString(kInstallDirPref, buf, &size) == kError_NoPrefValue)
        SetPrefString(kInstallDirPref, UNIX_LIBDIR);

    // set default freeamp library path value
    size = sizeof(buf);
    if (GetPrefString(kLibraryPathPref, buf, &size) == kError_NoPrefValue)
        SetPrefString(kLibraryPathPref, kDefaultLibraryPath);
    
    // set default ui value
    size = sizeof(buf);
    if (GetPrefString(kUIPref, buf, &size) == kError_NoPrefValue)
        SetPrefString(kUIPref, kDefaultUI);
    
    // set default text ui value
    size = sizeof(buf);
    if (GetPrefString(kTextUIPref, buf, &size) == kError_NoPrefValue)
        SetPrefString(kTextUIPref, kDefaultTextUI);
    
    // set default pmo value
    size = sizeof(buf);
    if (GetPrefString(kPMOPref, buf, &size) == kError_NoPrefValue)
        SetPrefString(kPMOPref, kDefaultPMO);

    // set default ALSA device
    size = sizeof(buf);
    if (GetPrefString(kALSADevicePref, buf, &size) == kError_NoPrefValue)
        SetPrefString(kALSADevicePref, kDefaultALSADevice);
   
    // set default ESD host
    size = sizeof(buf); 
    if (GetPrefString(kESOUNDHostPref, buf, &size) == kError_NoPrefValue)
        SetPrefString(kESOUNDHostPref, kDefaultESOUNDHost);

    size = sizeof(buf);
    if (GetPrefString(kDatabaseDirPref, buf, &size) == kError_NoPrefValue) {
        char *fadir = FreeampDir(NULL);
        string tempdir = fadir;
        tempdir += "/db/";
        SetPrefString(kDatabaseDirPref, tempdir.c_str());
        delete [] fadir;
    }

    size = sizeof(buf);
    if (GetPrefString(kSaveMusicDirPref, buf, &size) == kError_NoPrefValue) {
        string tempdir = string(getenv("HOME"));
        tempdir += "/MyMusic";
        SetPrefString(kSaveMusicDirPref, tempdir.c_str());
    }

    size = sizeof(buf);
    if (GetPrefString(kWatchThisDirectoryPref, buf, &size) == kError_NoPrefValue) 
    {
        string tempdir = string(getenv("HOME"));
        tempdir += "/MyMusic";
        SetPrefString(kWatchThisDirectoryPref, tempdir.c_str());
    }

    Preferences::SetDefaults();

    return kError_NoErr;
}

Error
UnixPrefs::
Save()
{
    if (!m_saveEnable || !m_changed)
        return kError_NoErr;

    // XXX: We should check the modification date of the file,
    //      and refuse to save if it's been modified since we
    //      read it.

    m_changed = false;

    const char *tmpSuffix = ".tmp";
    char *tmpFilePath = new char[strlen(m_prefsFilePath) +
                                 strlen(tmpSuffix) + 1];
    strcpy(tmpFilePath, m_prefsFilePath);
    strcat(tmpFilePath, tmpSuffix);

    const char *bakSuffix = ".bak";
    char *bakFilePath = new char[strlen(m_prefsFilePath) +
                                 strlen(bakSuffix) + 1];
    strcpy(bakFilePath, m_prefsFilePath);
    strcat(bakFilePath, bakSuffix);

    {
        FILE *prefsFile = fopen(tmpFilePath, "w");
        if (!prefsFile)
        {
            delete[] tmpFilePath;
            delete[] bakFilePath;
            return kError_UnknownErr; // XXX: Be more informative
        }

        m_mutex.Acquire();
                
        int32 numEntries = m_entries.size();
        int32 i;
        UnixPrefEntry *entry;
        
        for (i = 0; i < numEntries; i++)
        {
            entry = m_entries[i];
            if (entry->prefix) 
                fputs(entry->prefix, prefsFile);
            if (entry->key && entry->separator && entry->value)
            {
                char *outStr;

                outStr = WriteQuotableString(entry->key, ":#");
                fputs(outStr, prefsFile);
                delete[] outStr;

                fputs(entry->separator, prefsFile);

                outStr = WriteQuotableString(entry->value, "#");
                fputs(outStr, prefsFile);
                delete[] outStr;
            }
            if (entry->suffix)
                fputs(entry->suffix, prefsFile);
        }
        
        m_mutex.Release();

        if (ferror(prefsFile))
        {
            fclose(prefsFile);
            delete[] tmpFilePath;
            delete[] bakFilePath;
            return kError_UnknownErr; // XXX: Be more informative
        }
        
        fclose(prefsFile);
    }

    if (rename(m_prefsFilePath, bakFilePath) && errno != ENOENT)
    {
        delete[] tmpFilePath;
        delete[] bakFilePath;
        return kError_UnknownErr; // XXX: Be more informative
    }

    if (rename(tmpFilePath, m_prefsFilePath))
        rename(bakFilePath, m_prefsFilePath);

    delete[] tmpFilePath;
    delete[] bakFilePath;
    return kError_NoErr;
}

Preferences *
UnixPrefs::
ComponentPrefs(const char *componentName)
{
    return new PrefixPrefs(this, componentName);
}

Error
UnixPrefs::
GetPrefString(const char* pref, char* buf, uint32* len)
{
    UnixPrefEntry *entry;
    // int32 index;

    m_mutex.Acquire();

    buf[0] = '\0';
    if (m_ht.find(pref) == m_ht.end())
    {
        *len = 0;
        m_mutex.Release();
        return kError_NoPrefValue;
    }

    entry = m_ht[pref];
    if (!entry || !entry->value)
    {
        *len = 0;
        m_mutex.Release();
        return kError_NoPrefValue;
    }

    char *value = entry->value;
    uint32 value_len = strlen(value) + 1;

    if (value_len > *len)
    {
        *len = value_len;
        m_mutex.Release();
        return kError_BufferTooSmall;
    }

    strncpy(buf, value, value_len);
    *len = value_len;
    m_mutex.Release();
    return kError_NoErr;
}

Error
UnixPrefs::
SetPrefString(const char* pref, const char* buf)
{
    UnixPrefEntry *entry;

    m_mutex.Acquire();

    if (m_ht.find(pref) == m_ht.end())
    {
        entry = new UnixPrefEntry;
        AppendToString(&entry->key, pref, strlen(pref));
        AppendToString(&entry->separator, ": ", 2);
        AppendToString(&entry->suffix, "\n", 1);
        m_entries.push_back(entry);
        m_ht[pref] = entry;
    }
    else {
        entry = m_ht[pref];
        if (entry) 
        {
            delete [] entry->value;
            entry->value = NULL;
        }
        else 
        {
            entry = new UnixPrefEntry;
            AppendToString(&entry->key, pref, strlen(pref));
            AppendToString(&entry->separator, ": ", 2);
            AppendToString(&entry->suffix, "\n", 1);
            m_entries.push_back(entry);
            m_ht[pref] = entry;
        }
    }

    AppendToString(&entry->value, buf, strlen(buf));

    m_changed = true;

    m_mutex.Release();
    return kError_NoErr;
}

char *UnixPrefs::m_libDirs = NULL;

const char *
UnixPrefs::
GetLibDirs()
{
    if (!m_libDirs) {
        uint32 size = 1024;
        m_libDirs = new char[size];

        strcpy(m_libDirs, kDefaultLibraryPath);
        //GetPrefString(kLibraryPathPref, m_libDirs, &size);
    }
    return m_libDirs;
}

LibDirFindHandle *
UnixPrefs::
GetFirstLibDir(char *path, uint32 *len)
{
    // if no ZINF_PATH, use kLibraryPathPref
    // if ZINF_PATH, then its ZINF_PATH
    char *pEnv = getenv(ZINF_PATH_ENV);
    char *pPath = NULL;
    if (pEnv) {
//      cout << "Using env: " << pEnv << endl;
        pPath = strdup_new(pEnv);
    } else {
        pPath = strdup_new((char *)GetLibDirs());
    }
    pEnv = pPath;
    LibDirFindHandle *hLibDirFind = new LibDirFindHandle();
    hLibDirFind->m_pLibDirs = new vector<char *>();
    hLibDirFind->m_current = 0;

    char *pCol = (char *)1;
    char *pPart = pPath;
    while (pCol) {
        pCol = strchr(pPart,':');
        if (pCol) *pCol = '\0';
        char *pFoo = strdup_new(pPart);
        hLibDirFind->m_pLibDirs->push_back(pFoo);
        pPart = pCol + sizeof(char);
    }

    if (hLibDirFind->m_pLibDirs->size() > 0) {
        pPath = (*hLibDirFind->m_pLibDirs)[0];
        strncpy(path,pPath,*len);
        *len = strlen(pPath);
    } else {
        *path = '\0';
        *len = 0;
        delete hLibDirFind->m_pLibDirs;
        delete hLibDirFind;
        hLibDirFind = 0;
    }

    if (pEnv) delete pEnv;
    //cout << "returning " << path << endl;
    return hLibDirFind;
}

Error
UnixPrefs::
GetNextLibDir(LibDirFindHandle *hLibDirFind, char *path, uint32 *len)
{
    if (hLibDirFind) {
        if (++hLibDirFind->m_current < (int32)hLibDirFind->m_pLibDirs->size()) {
            char *pPath = (*hLibDirFind->m_pLibDirs)[hLibDirFind->m_current];
            strncpy(path,pPath,*len);
            *len = strlen(pPath);
//          cout << "returning next: " << path << endl;
            return kError_NoErr;
        }
        *path = '\0';
        *len = 0;
//      cout << "returning no next " << path << endl;
    }
    return kError_NoMoreLibDirs;
}

Error
UnixPrefs::
GetLibDirClose(LibDirFindHandle *hLibDirFind)
{
    if (hLibDirFind) {
        while (hLibDirFind->m_pLibDirs->size() > 0)
        {
            delete (*(hLibDirFind->m_pLibDirs))[0];
            hLibDirFind->m_pLibDirs->erase(hLibDirFind->m_pLibDirs->begin());
        }
        delete hLibDirFind->m_pLibDirs;
        delete hLibDirFind;
    }
    return kError_NoErr;
}

