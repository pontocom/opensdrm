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
	
	$Id: win32updatemanager.cpp,v 1.1 2004/10/13 16:16:05 linha2 Exp $
____________________________________________________________________________*/

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#ifdef WIN32
#pragma warning(disable:4786)
#define STRICT
#endif


#include <assert.h>
#include <windows.h>
#include <io.h>
#include <direct.h>
#include <time.h>
#include <errno.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace std;

#include "config.h"
#include "facontext.h"

#include "errors.h"
#include "win32updatemanager.h"
#include "utility.h"
#include "event.h"
#include "eventdata.h"
#include "zlib.h"


Win32UpdateManager::Win32UpdateManager(FAContext* context):
UpdateManager(context)
{
    m_context = context;
}

Win32UpdateManager::~Win32UpdateManager()
{
  
}

Error Win32UpdateManager::DetermineLocalVersions()
{
    Error result = kError_AlreadyUpdating;

    if(m_mutex.Acquire(0))
    {
        char appPath[MAX_PATH];
        uint32 length = sizeof(appPath);
        m_context->prefs->GetPrefString(kInstallDirPref, appPath, &length);

        // paths i need to ignore
        length = sizeof(m_musicPath);
        m_context->prefs->GetPrefString(kSaveMusicDirPref, m_musicPath, &length);
   
        strcpy(m_updatePath, appPath);
        strcat(m_updatePath, "\\update");

        // analyze all the files in our install dir
        result = GetFileVersions(appPath);

        // what about the system files this version depends on?
        if(IsntError(result))
            result = GetSystemFileVersions();
        
        m_mutex.Release();
    }

    return result;
}

Error Win32UpdateManager::UpdateComponents(UMCallBackFunction function,
                                           void* cookie)
{
    Error result;
    
    result = UpdateManager::UpdateComponents(function, cookie);

    if(IsntError(result))
    {
        int32 response;

        response = MessageBox(NULL, 
                              The_BRANDING" needs to close down and restart in order to replace components\r\n"
                              "which are being used. If you do not wish to quit the application you\r\n"
                              "can choose \"Cancel\" and update again at a later time.",
                              "Restart "the_BRANDING"?", 
                              MB_OKCANCEL|MB_ICONQUESTION|MB_SETFOREGROUND);

        if(response == IDOK)
        {
            char appPath[MAX_PATH];
            char updatePath[MAX_PATH];
            uint32 length = sizeof(appPath);
            m_context->prefs->GetPrefString(kInstallDirPref, appPath, &length);
            
            strcpy(updatePath, appPath);

            strcat(appPath, "\\update.exe");
            strcat(updatePath, "\\update\\update.exe");

            // if the update program has been updated we need to 
            // move it before execing it...
            HANDLE findFileHandle = NULL;
            WIN32_FIND_DATA findData;

            findFileHandle = FindFirstFile(updatePath, &findData);

            if(findFileHandle != INVALID_HANDLE_VALUE)
            {
                int32 failureCount;
                BOOL success;

                failureCount = 0;
                success = TRUE;

                do
                {
                    // remove old file
                    success = DeleteFile(appPath);

                    if(!success)
                        Sleep(1000);

                }while(!success && failureCount++ < 3);

                
                if(success)
                {
                    failureCount = 0;

                    do
                    {
                       // actually move the file
                        success = MoveFile(updatePath, appPath);

                        if(!success)
                            Sleep(1000);

                    }while(!success && failureCount++ < 3);
                }
                
                if(!success)
                {
                    LPVOID lpMessageBuffer;

		            FormatMessage(
		              FORMAT_MESSAGE_ALLOCATE_BUFFER |
		              FORMAT_MESSAGE_FROM_SYSTEM,
		              NULL,
		              GetLastError(),
		              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		              (LPTSTR) &lpMessageBuffer,
		              0,
		              NULL );

		            // now display this string
 		            MessageBox(NULL, (char*)lpMessageBuffer, appPath, MB_OK);

		            // Free the buffer allocated by the system
		            LocalFree( lpMessageBuffer );
                }

                FindClose(findFileHandle);
            }

            m_context->target->AcceptEvent(new Event(CMD_QuitPlayer));

            WinExec(appPath, SW_NORMAL);
        }
    }
    
    return result;
}

Error Win32UpdateManager::GetFileVersions(const char* path)
{
    Error result = kError_NoErr;
    
    HANDLE findFileHandle = NULL;
    WIN32_FIND_DATA findData;  
    char filePath[MAX_PATH];
    char* fp;

    strcpy(filePath, path);
    strcat(filePath, "\\*.*");
    fp = strrchr(filePath, '\\') + 1;
    
    findFileHandle = FindFirstFile(filePath, &findData);

    //cout << "searching in "<< path << ":" << endl;

    if(findFileHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            strcpy(fp, findData.cFileName);

            // skip these two special entries && music dir
            if( strcmp(findData.cFileName, ".") && 
                strcmp(findData.cFileName, "..") &&
                strcmp(filePath, m_updatePath) &&
                strcmp(filePath, m_musicPath))
            {
                if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
                {
                    // call ourselves on that directory
                    GetFileVersions(filePath);      
                }
                else 
                {
                    DWORD versionSize;
                    DWORD dummyHandle;
                    void* data;

                    versionSize = GetFileVersionInfoSize(filePath, &dummyHandle);
                    
                    if (versionSize > 0)
                       data = malloc(versionSize);
                    else
                       data = NULL;   

                    // actually get the versioninfo for the file
                    if(data)
                    {
                        if(GetFileVersionInfo(filePath, dummyHandle, 
                                               versionSize, data))
                        {
                            VS_FIXEDFILEINFO* fileInfo;
                            char* fileDescription;
                            uint32 size;
                            UpdateItem* item = new UpdateItem;

                            item->SetLocalFileName(string(findData.cFileName));
                            item->SetLocalFilePath(string(filePath));

                            if(VerQueryValue(data, "\\", (void**)&fileInfo, &size))
                            {        
                                ostringstream ost;

                                //cout << filePath << ":" << endl;
                                //cout << "dwFileVersionLS: " << fileInfo->dwFileVersionLS << endl;
                                //cout << "dwFileVersionMS: " << fileInfo->dwFileVersionMS << endl;
                                //cout << "hi: " << HIWORD(fileInfo->dwFileVersionLS) << " lo: "<< LOWORD(fileInfo->dwFileVersionLS) << endl;
                                //cout << "hi: " << HIWORD(fileInfo->dwFileVersionMS) << " lo: "<< LOWORD(fileInfo->dwFileVersionMS) << endl << endl;

                                uint32 major = HIWORD(fileInfo->dwFileVersionMS);
                                uint32 minor = LOWORD(fileInfo->dwFileVersionMS);
                                uint32 rev = HIWORD(fileInfo->dwFileVersionLS);
                                uint32 file = LOWORD(fileInfo->dwFileVersionLS);

                                ost << major << "." << minor << "." << rev << "." << file;
                                
                                item->SetLocalFileVersion(ost.str());

                            }

                            // I need to learn how to correctly grab the proper language
                            // but for now we just hardcode English (US) Unicode
                            if(VerQueryValue(data, "\\StringFileInfo\\040904B0\\FileDescription", (void**)&fileDescription, &size))
                            {     
                                //cout << fileDescription << endl;
                                item->SetFileDescription(string(fileDescription));
                            }

                            //cout << endl;

                            AddItem(item);
                        }

                        free(data);
                    }
                    else // no version info, is it one of our special files?
                    {
                        const char* kHelpDescription = The_BRANDING" help file provides detailed instructions for all the "
                                                        "features in the player.";
                        const char* kThemeDescription = "This is the default theme that determines how "the_BRANDING" looks. "
                                                        "Updating the default theme will not change your theme preference if "
                                                        "you have chosen to use another theme.";
                        const char* kThemeHowToDescription = "The Theme HowTo describes how to create new themes which can be used to change "
                                                             "the default look of "the_BRANDING".";
                        const char* kChangesDescription = "The Changes file describes what has been changed between each version "
                                                          "of "the_BRANDING".";


                        if(!strcasecmp(findData.cFileName, BRANDING_HELP_FILE) ||
                           !strcasecmp(findData.cFileName, BRANDING_DEFAULT_THEME) ||
                           !strcasecmp(findData.cFileName, "ThemeHowTo.txt") ||
                           !strcasecmp(findData.cFileName, "Changes.txt"))
                        {
                            UpdateItem* item = new UpdateItem;

                            item->SetLocalFileName(string(findData.cFileName));
                            item->SetLocalFilePath(string(filePath));

                            SYSTEMTIME sysTime;

                            FileTimeToSystemTime(&findData.ftLastWriteTime, 
                                                 &sysTime);

                            ostringstream ost;

                            ost << sysTime.wYear << "-" <<
                                setw(2) << setfill('0') << sysTime.wMonth << "-" <<
                                setw(2) << setfill('0') << sysTime.wDay;

                            item->SetLocalFileTime(ost.str());

                            if(!strcasecmp(findData.cFileName, BRANDING_HELP_FILE))
                            {
                                item->SetFileDescription(string(kHelpDescription));
                            }
                            else if(!strcasecmp(findData.cFileName, BRANDING_DEFAULT_THEME))
                            {
                                item->SetFileDescription(string(kThemeDescription));
                            }
                            else if(!strcasecmp(findData.cFileName, "ThemeHowTo.txt"))
                            {
                                item->SetFileDescription(string(kThemeHowToDescription));
                            }
                            else if(!strcasecmp(findData.cFileName, "Changes.txt"))
                            {
                                item->SetFileDescription(string(kChangesDescription));
                            }

                            AddItem(item);
                        }
                    }
                }
            }

        }while(FindNextFile(findFileHandle, &findData));

        FindClose(findFileHandle);
    }

    return result;
}

Error Win32UpdateManager::GetSystemFileVersions()
{
    Error result = kError_NoErr;
    
    char filePath[MAX_PATH];
    char* fp;
    char* systemFiles[] = { "msvcrt.dll",
                            "msvcp60.dll",
                            NULL};
    uint32 index = 0;

    GetSystemDirectory(filePath, sizeof(filePath));

    strcat(filePath, "\\");
    fp = strrchr(filePath, '\\') + 1;

    while(systemFiles[index])
    {
        strcpy(fp, systemFiles[index]);

        DWORD versionSize;
        DWORD dummyHandle;
        void* data;

        versionSize = GetFileVersionInfoSize(filePath, &dummyHandle);
    
        if (versionSize > 0)
           data = malloc(versionSize);
        else
           data = NULL;   

        // actually get the versioninfo for the file
        if(data)
        {
            if(GetFileVersionInfo(filePath, dummyHandle, versionSize, data))
            {
                VS_FIXEDFILEINFO* fileInfo;
                char* fileDescription;
                uint32 size;
                UpdateItem* item = new UpdateItem;

                item->SetLocalFileName(string(systemFiles[index]));
                item->SetLocalFilePath(string(filePath));

                if(VerQueryValue(data, "\\", (void**)&fileInfo, &size))
                {        
                    ostringstream ost;

                    uint32 major = HIWORD(fileInfo->dwFileVersionMS);
                    uint32 minor = LOWORD(fileInfo->dwFileVersionMS);
                    uint32 rev = HIWORD(fileInfo->dwFileVersionLS);
                    uint32 file = LOWORD(fileInfo->dwFileVersionLS);

                    ost << major << "." << minor << "." << rev << "." << file;
                
                    item->SetLocalFileVersion(ost.str());

                }

                // I need to learn how to correctly grab the proper language
                // but for now we just hardcode English (US) Unicode
                if(VerQueryValue(data, "\\StringFileInfo\\040904B0\\FileDescription", (void**)&fileDescription, &size))
                {     
                    item->SetFileDescription(string(fileDescription));
                }

                // for the system files we set the currentFileLocation to _system_
                // to keep it from being listed in the UI... kinda kludgy...

                item->SetCurrentFileLocation(string("_system_"));

                AddItem(item);
            }

            free(data);
        }

        index++;
    }

    return result;
}


BOOL CALLBACK 
UpdateAvailableDlgProc(HWND hwnd, 
                       UINT msg, 
                       WPARAM wParam, 
                       LPARAM lParam)
{
    BOOL result = FALSE;

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            
            break;
        }      

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDCANCEL:
                    EndDialog(hwnd, FALSE);
                    break;

                case IDOK:
                {
                    EndDialog(hwnd, TRUE);
                    break;
                }
            }
  
            break;
        }
    }

    return result;
}