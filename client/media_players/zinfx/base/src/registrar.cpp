/*____________________________________________________________________________
	
	FreeAmp - The Free MP3 Player
	Portions copyright (C) 1998-1999 EMusic.com

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
	
	$Id: registrar.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

/* System Includes */
#ifdef WIN32
#include <windows.h>
#else
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>
#include <set>
#endif // WIN32

#include <stdio.h>
#include <string.h>
#include <iostream>

/* Project Includes */
#include "config.h"
#include "registrar.h"
#include "errors.h"
#include "debug.h"

using namespace std;

Error 
Registrar::
InitializeRegistry(Registry* registry, Preferences* prefs)
{
    Error error = kError_NoErr;
    char dir[_MAX_PATH];

#ifndef WIN32
    set<string> *pHT = new set<string>;
#endif

    if(registry == NULL)
        error = kError_InvalidParam;

    if(m_search == NULL)
        error = kError_InvalidParam;

    if(m_subdir == NULL)
        error = kError_InvalidParam;

    uint32 len = sizeof(dir);
    uint32 totalFilesFound = 0;
    LibDirFindHandle *libDirHandle = 0;

    if(IsntError(error))
	libDirHandle = prefs->GetFirstLibDir(dir, &len);

    while(libDirHandle && (error != kError_NoMoreLibDirs))
    {
#ifndef WIN32
        if (dir[0] == '~') {
	    if (dir[1] != '/') {
		// a ~ then a users name
		char newdir[_MAX_PATH];
		char *tmp = strchr(dir,'/');
		char tmpChar = 0x00;
		if (tmp) { tmpChar = *tmp; *tmp = '\0'; }
		struct passwd *pwd = getpwnam(&(dir[1]));
		if (pwd) {
		    strcpy(newdir, pwd->pw_dir);
		    if (tmpChar) { *tmp = tmpChar; }
		    if (tmp) { strcat(newdir,tmp); }
		    memcpy(dir,newdir,_MAX_PATH);
		} else {
		    cerr << "Couldn't get user's identity: " << &(dir[1]) << endl;
		}
	    } else {
		// just a ~
		char newdir[_MAX_PATH];
		struct passwd *pwd = getpwuid(getuid());
		if (pwd) {
		    strcpy(newdir,pwd->pw_dir);
		    char *pSlash = strchr(dir,'/');
		    if (pSlash) {
			strcat(newdir,pSlash);
		    }
		    memcpy(dir,newdir,_MAX_PATH);
		} else {
		    cerr << "Couldn't get user's identity: " << getuid() << endl;
		}
	    }
	}
#endif
	len = sizeof(dir);
        WIN32_FIND_DATA find;
        HANDLE handle;
        char search[_MAX_PATH];

        if(*m_subdir != DIR_MARKER)
            strcat(dir, DIR_MARKER_STR);

        strcat(dir, m_subdir);

        strcpy(search, dir);

        if(*m_search != DIR_MARKER)
            strcat(search, DIR_MARKER_STR);

        strcat(search, m_search);

        handle = FindFirstFile(search, &find);

	//cerr << "Searching: " << search << endl;

        if(handle != INVALID_HANDLE_VALUE)
        {
            do
            {
#ifndef WIN32
		if (pHT) {
		    if (pHT->find(find.cFileName) != pHT->end()) {
			// only put things with same Name into registry once.
			continue;
		    }
		}
#endif
                char file[_MAX_PATH];

		//cerr << "Found file: " << find.cFileName << endl;

                sprintf(file, "%s%s%s", dir, DIR_MARKER_STR, find.cFileName);

		//cerr << "Trying to load: " << file << endl;

                RegistryItem* item = new RegistryItem;

                item->SetPath(file);
                item->SetName(find.cFileName);
                item->SetDescription(find.cFileName);

                HMODULE module = NULL;
                error = kError_LoadLibFailed;

                module = LoadLibrary(file);
                if(module)
                {
                    InitializeFunction init = NULL;
                    error = kError_FindFuncFailed;

                    item->SetModule(module);

                    init = (InitializeFunction)GetProcAddress(module, "Initialize");
                    if(init)
                    {
                        error = kError_NoErr;
                        item->SetInitFunction(init);

#ifdef WIN32
                        GetPluginDescription(file, item);
#endif
		        totalFilesFound++;
#ifndef WIN32
                        pHT->insert(find.cFileName);
#endif
                    }
                }
                
                if(IsntError(error))
                    registry->AddItem(item);
                else
                {
                    if(module)
                        FreeLibrary(module);
                }
                
            }while(FindNextFile(handle, &find));

            FindClose(handle);
        }
	error = prefs->GetNextLibDir(libDirHandle,dir,&len);
    }

    if (libDirHandle) 
        prefs->GetLibDirClose(libDirHandle);
#ifndef WIN32
    if (pHT) {
	delete pHT;
	pHT = NULL;
    }
#endif
    if (totalFilesFound == 0) 
	    error = kError_NoFiles;

    return error;
}

Error 
Registrar::
CleanupRegistry(Registry* registry)
{
    Error           error   = kError_NoErr;
    int count = registry->CountItems();
    for (int i = count - 1; i >= 0; i--) {
#ifndef WIN32
        FreeLibrary(registry->GetItem(i)->Module());
#endif
        delete (registry->GetItem(i));
    }
 
    return error;
}

#ifdef WIN32
void 
Registrar::
GetPluginDescription(char* filePath, RegistryItem* item)
{
    DWORD versionSize;
    DWORD dummyHandle;
    void* data;

    versionSize = GetFileVersionInfoSize(filePath, &dummyHandle);
    //RAK: Something is not kosher here!
    //     Boundschecker says that versionSize is 0
    //     I added the code to gracefully handle that case
    //     (fix the symptom, not the cause!)
    if (versionSize > 0)
       data = malloc(versionSize);
    else
       data = NULL;   

    // actually get the verionsinfo for the file
    if(data)
    {
        if(GetFileVersionInfo(filePath, dummyHandle, 
                               versionSize, data))
        {
            char* fileDescription;
            uint32 size;

            // I need to learn how to correctly grab the proper language
            // but for now we just hardcode English (US) Unicode
            if(VerQueryValue(data, "\\StringFileInfo\\040904B0\\ProductName", (void**)&fileDescription, &size))
            {     
                //cout << fileDescription << endl;
                item->SetDescription(fileDescription);
            }
        }

        free(data);
    }
}
#endif
