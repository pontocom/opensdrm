/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player
	Portions copyright (C) 1999 EMusic.com

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
	
	$Id: main.cpp,v 1.1 2004/10/13 16:17:07 linha2 Exp $
____________________________________________________________________________*/

/* System Includes */
#include <windows.h>
#include <winsock.h>
#include <commctrl.h>
#include <setupapi.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <sys/stat.h>

/* Project Includes */
#include "win32prefs.h"

const char* kMessage = "An instance of "the_BRANDING" is currently running. "
                       "Please close the application before continuing the "
                       "update.";

const char* kReboot = The_BRANDING" needs to restart your computer in order "
                      "to complete the update process.\r\n"
                      "If you do not wish to restart your computer now you "
                      "may do so later.\r\n\r\n"
                      "Do you wish to restart your computer?";

void MoveFiles(const char* src, const char* dest);
bool MoveSystemFiles(const char* src);
void MoveFileOnReboot(const char* src, const char* dest);

int APIENTRY WinMain(	HINSTANCE hInstance, 
						HINSTANCE hPrevInstance,
		 				LPSTR lpszCmdLine, 
						int cmdShow)
{
    HANDLE runOnceMutex;

    runOnceMutex = CreateMutex(	NULL,
							    TRUE,
							    The_BRANDING" Should Only Run One Time!");

    // make sure FreeAmp is not running while this happens...
    while(WAIT_TIMEOUT == WaitForSingleObject(runOnceMutex, 5000))
    {
        int32 result;

        result = MessageBox(NULL, kMessage, "Updating "the_BRANDING, 
                            MB_RETRYCANCEL);

        if(result == IDCANCEL)
        {
            CloseHandle(runOnceMutex);
            return 0;
        }
    }
    
    // get our handy dandy directories
    Win32Prefs prefs;
    char appPath[MAX_PATH];
    char updatePath[MAX_PATH];
    uint32 length = sizeof(appPath);

    prefs.GetPrefString(kInstallDirPref, appPath, &length);

    strcpy(updatePath, appPath);
    strcat(updatePath, "\\update");

    // Move all those new files
    MoveFiles(updatePath, appPath);

    strcat(updatePath, "\\_system_");

    bool needToReboot;

    needToReboot = MoveSystemFiles(updatePath);

    CloseHandle(runOnceMutex);

    if(needToReboot)
    {
        // let ourselves know we need to reboot with a lock file
        strcat(appPath, "\\NeedToRebootForUpdate");
        creat(appPath, _S_IREAD | _S_IWRITE);

        // but make sure it gets axed when the reboot occurs
        MoveFileOnReboot(appPath, NULL);

        int result;

        result = MessageBox(NULL, kReboot, "Restart Your Computer?", 
                            MB_ICONQUESTION|MB_YESNO|MB_SETFOREGROUND);

        if(result == IDYES)
        {
            OSVERSIONINFO osid;

	        osid.dwOSVersionInfoSize = sizeof(osid);
	        GetVersionEx(&osid);

            if(osid.dwPlatformId == VER_PLATFORM_WIN32_NT)
            {
                HANDLE token;

                if(OpenProcessToken(GetCurrentProcess(),
                                    TOKEN_ADJUST_PRIVILEGES,
                                    &token))
                {
                    TOKEN_PRIVILEGES tp;
                    LUID luid;

                    LookupPrivilegeValue(NULL,
                                         SE_SHUTDOWN_NAME,
                                         &luid);

                    tp.PrivilegeCount = 1;
                    tp.Privileges[0].Luid = luid;
                    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

                    AdjustTokenPrivileges(token, 
                                          FALSE, 
                                          &tp, 
                                          sizeof(TOKEN_PRIVILEGES), 
                                          NULL, 
                                          NULL); 
 
                    CloseHandle(token);
                }
            }

            ExitWindowsEx(EWX_REBOOT, 0);
        }
    }
    else
    {
        strcat(appPath, "\\freeamp.exe");
        WinExec(appPath, SW_NORMAL);
    }

	return 0;
}

void MoveFiles(const char* src, const char* dest)
{
    // iterate over the files in the update directory
    HANDLE findFileHandle = NULL;
    WIN32_FIND_DATA findData;
    char srcPath[MAX_PATH];
    char destPath[MAX_PATH];
    char* dp;
    char* sp;
    
    strcpy(srcPath, src);
    strcat(srcPath, "\\*.*");
    sp = strrchr(srcPath, '\\') + 1;

    strcpy(destPath, dest);
    strcat(destPath, "\\");
    dp = strrchr(destPath, '\\') + 1;

    findFileHandle = FindFirstFile(srcPath, &findData);

    if(findFileHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            // skip these two special entries & the _system_ directory
            if( strcmp(findData.cFileName, ".") && 
                strcmp(findData.cFileName, "..") &&
                strcmp(findData.cFileName, "_system_"))
            {
                strcpy(sp, findData.cFileName);
                strcpy(dp, findData.cFileName);

                if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
                {
                    // call ourselves on that directory
                    MoveFiles(srcPath, destPath);  
                    RemoveDirectory(srcPath);
                }
                else 
                {
                    int32 failureCount;
                    BOOL success;

                    failureCount = 0;
                    success = TRUE;

                    do
                    {
                        // remove old file
                        success = DeleteFile(destPath);

                        if(!success)
                            Sleep(1000);

                    }while(!success && failureCount++ < 3);
                    
                    if(success)
                    {
                        failureCount = 0;

                        do
                        {
                           // actually move the file
                            success = MoveFile(srcPath, destPath);

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
 		                MessageBox(NULL, (char*)lpMessageBuffer, destPath, 
                                   MB_OK);

		                // Free the buffer allocated by the system
		                LocalFree( lpMessageBuffer );
                    }
                }
            }

        }while(FindNextFile(findFileHandle, &findData));

        FindClose(findFileHandle);
    }
}

bool MoveSystemFiles(const char* src)
{
    HANDLE findFileHandle = NULL;
    WIN32_FIND_DATA findData;
    char srcPath[MAX_PATH];
    char destPath[MAX_PATH];
    char* dp;
    char* sp;
    bool needToReboot = false;

    strcpy(srcPath, src);
    strcat(srcPath, "\\*.*");
    sp = strrchr(srcPath, '\\') + 1;

    GetSystemDirectory(destPath, sizeof(destPath));
    strcat(destPath, "\\");
    dp = strrchr(destPath, '\\') + 1;

    // is there a _system_ directory for us to look at?
    findFileHandle = FindFirstFile(srcPath, &findData);

    if(findFileHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            // skip these two special entries
            if( strcmp(findData.cFileName, ".") && 
                strcmp(findData.cFileName, ".."))
            {
                strcpy(sp, findData.cFileName);
                strcpy(dp, findData.cFileName);

                BOOL success;

                success = TRUE;

                success = DeleteFile(destPath);

                if(success)
                {
                    // actually move the file
                    success = MoveFile(srcPath, destPath);
                }  
        
                if(!success)
                {
                    needToReboot = true;

                    // First tell it to delete the old file
                    MoveFileOnReboot(destPath, NULL);

                    // Now replace it with the new one
                    MoveFileOnReboot(srcPath, destPath);                
                }
            }

        }while(FindNextFile(findFileHandle, &findData));

        FindClose(findFileHandle);
    }

    return needToReboot;
}

void MoveFileOnReboot(const char* src, const char* dest)
{
    OSVERSIONINFO osid;

	osid.dwOSVersionInfoSize = sizeof(osid);
	GetVersionEx(&osid);

	switch (osid.dwPlatformId)
    {
	    // Windows 95, 98
	    case VER_PLATFORM_WIN32_WINDOWS:
        {
            const uint32 MAX_INI_SECTION_SIZE = 32767;
            char* section;
            uint32 sectionSize = 0;
            char iniPath[MAX_PATH];
            const char* kNULL = "NUL";

            if(!dest)
                dest = kNULL;
    
            GetWindowsDirectory(iniPath, sizeof(iniPath));
            strcat(iniPath, "\\WININIT.INI");

            section = new char[MAX_INI_SECTION_SIZE];

            *section = 0x00;

            sectionSize = GetPrivateProfileSection("rename", section, 
                                                   MAX_INI_SECTION_SIZE, 
                                                   iniPath);
            
            char srcShortFileName[MAX_PATH];
            char destShortFileName[MAX_PATH];

            GetShortPathName(src, srcShortFileName, sizeof(srcShortFileName));
            GetShortPathName(dest, destShortFileName, sizeof(srcShortFileName));

            strcpy(section + sectionSize, destShortFileName);
            sectionSize += strlen(destShortFileName);

            strcpy(section + sectionSize, "=");
            sectionSize++;

            strcpy(section + sectionSize, srcShortFileName);
            sectionSize += strlen(srcShortFileName) + 1;

            strcpy(section + sectionSize, "\0");

            WritePrivateProfileSection("rename", section, iniPath);

            delete [] section;
 		    break;
        }

	    // Windows NT
	    case VER_PLATFORM_WIN32_NT:
        {
            MoveFileEx(src, dest, MOVEFILE_DELAY_UNTIL_REBOOT);
		    break;    
        }
    } 
}
