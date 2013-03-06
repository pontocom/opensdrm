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
	
	$Id: pmp300.cpp,v 1.1 2004/10/13 16:16:12 linha2 Exp $
____________________________________________________________________________*/

#include <assert.h>
#include <iostream>
#include <algorithm>
#ifdef unix
#include <strstream>
typedef ostrstream ostringstream;
#else
#include <sstream>
#endif
#include <string>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

using namespace std;

#include "config.h"
#include "errors.h"
#include "errno.h"

#include "pmp300.h"
#include "rio.h"
#include "utility.h"

typedef struct DeviceInfoStruct {
    char* manufacturer;
    char* device;
    ConnectionType type;
} DeviceInfoStruct; 

DeviceInfoStruct devices[] = {
    {"Diamond", "Rio PMP-300", kConnection_Parallel}
};

const uint32 ports[] = { 0x378, 0x278, 0x03BC };

#define kNumDevices (sizeof(devices)/sizeof(DeviceInfoStruct))

Error privateReadPlaylist(CRio& rio, 
                           bool useExternal,
                           uint32* totalMem,
                           uint32* usedMem,
                           vector<PlaylistItem*>* list,
                           PLMCallBackFunction function,
                           void* cookie);

bool FindRio(CRio& rio, 
             DeviceInfo* device,
             PLMCallBackFunction function,
             void* cookie);

extern "C"
{
   PortableDevice *Initialize(FAContext* context)
   {
      return new PMP300(context);
   }
}

#ifdef WIN32
bool LoadDriver(const char* driverName, const char* driverPath);
bool UnloadDriver(const char* driverName);
#endif


class PlaylistItemCompare {

 public:
    PlaylistItemCompare(PlaylistItem* item):m_item(item){}

    // Function object used for comparing PlaylistItems
    bool operator() (PlaylistItem* item) const
    {
        bool result = false;

        assert(item);

        if(item)
        {
            result = item->URL() == m_item->URL();
        }

        return result;
    }

 private:
    PlaylistItem* m_item;

};

PMP300::PMP300(FAContext* context):PortableDevice(context)
{
#ifdef WIN32
    OSVERSIONINFO osid;

	osid.dwOSVersionInfoSize = sizeof (osid);
	GetVersionEx (&osid);

	switch (osid.dwPlatformId) 
    {
        // Window 3.x
	    case VER_PLATFORM_WIN32s:
		    break;

	    // Windows 95
	    case VER_PLATFORM_WIN32_WINDOWS:
 		    break;

	    // Windows NT
	    case VER_PLATFORM_WIN32_NT:
        {
#if !defined( _M_ALPHA )
		    if(osid.dwMajorVersion >= 4)
            {
                // this will allow us port access thru
                // inp and outp so the rio stuff will
                // work.

                const char* driver = "portio";
                char path[MAX_PATH];
                uint32 size = sizeof(path);

                context->prefs->GetPrefString(kInstallDirPref, path, &size);

                strcat(path, "\\");
                strcat(path, driver);
                strcat(path, ".sys");

                LoadDriver(driver, path);
            }
#endif // !defined( _M_ALPHA )
            break;
        }
    }
#endif  //WIN32
}

PMP300::~PMP300()
{
#ifdef WIN32
    // platform specific uninitialization
    OSVERSIONINFO osid;

	osid.dwOSVersionInfoSize = sizeof (osid);
	GetVersionEx (&osid);

	switch (osid.dwPlatformId) 
    {
        // Window 3.x
	    case VER_PLATFORM_WIN32s:
		    break;

	    // Windows 95
	    case VER_PLATFORM_WIN32_WINDOWS:
 		    break;

	    // Windows NT
	    case VER_PLATFORM_WIN32_NT:
        {
#if !defined( _M_ALPHA )
		    if(osid.dwMajorVersion >= 4)
            {
                UnloadDriver("portio");
            }
#endif // !defined( _M_ALPHA )
            break;
        }
    }
#endif // WIN32
}

Error PMP300::GetSupportedDevices(DeviceInfo* device, uint32 index)
{
    Error result = kError_InvalidParam;

    assert(device);

    if(device)
    {
        result = kError_NoMoreDevices;

        if(index < kNumDevices)
        {
            device->SetManufacturer(devices[index].manufacturer);
            device->SetDevice(devices[index].device);
            device->SetConnectionType(devices[index].type);

            result = kError_NoErr;
        }
    }

    return result;
}

bool PMP300::IsDeviceAvailable(DeviceInfo* device)
{
    bool result = false;

    assert(device);

    if(device)
    {
        if(!strcasecmp(device->GetDevice(), devices[0].device))
        {
            CRio rio;

            result = FindRio(rio, device, NULL, NULL);
        }
    }

    return result;
}

Error PMP300::GetDeviceInfo(DeviceInfo* device)
{
    Error result = kError_InvalidParam;

    assert(device);

    if(device)
    {
        result = kError_DeviceNotFound;

        if(!strcasecmp(device->GetDevice(), devices[0].device))
        {
            CRio rio;
            bool rioPresent = false;

            rioPresent = FindRio(rio, device, NULL, NULL);

            if(rioPresent)
            {
                if( rio.RxDirectory() )
	            {
		            CDirBlock& cDirBlock = rio.GetDirectoryBlock();
                    CDirHeader& cDirHeader = cDirBlock.m_cDirHeader;

                    uint32 numEntries, totalMem, usedMem; 

                    numEntries = cDirHeader.m_usCountEntry;
                    totalMem = ((long)cDirHeader.m_usCount32KBlockAvailable * CRIO_SIZE_32KBLOCK);
                    usedMem = ((long)cDirHeader.m_usCount32KBlockUsed * CRIO_SIZE_32KBLOCK);

                    rio.UseExternalFlash( true );

                    if( rio.RxDirectory() )
	                {
                        CDirBlock& cDirBlock = rio.GetDirectoryBlock();
                        CDirHeader& cDirHeader = cDirBlock.m_cDirHeader;

                        numEntries += cDirHeader.m_usCountEntry;
                        totalMem += ((long)cDirHeader.m_usCount32KBlockAvailable * CRIO_SIZE_32KBLOCK);
                        usedMem += ((long)cDirHeader.m_usCount32KBlockUsed * CRIO_SIZE_32KBLOCK);
                    }

                    device->SetNumEntries(numEntries);
                    device->SetCapacity(totalMem, usedMem);
	            }

                result = kError_NoErr;
            }
        }
    }

    return result;
}

typedef struct RioProgressStruct {
    PLMCallBackFunction function;
    void* cookie;
    PlaylistItem* item;
} RioProgressStruct;

static BOOL rioProgress(int pos, int total, void* cookie)
{
    BOOL result = false;

    if(cookie)
    {
        RioProgressStruct* ps = (RioProgressStruct*)cookie;

        if(ps->function)
        {
            PLMEvent event;
            
            event.type = kPLMEvent_Progress;
            event.data.progressData.position = pos;
            event.data.progressData.total = total;
            event.data.progressData.item = ps->item;

            result = ps->function(&event, ps->cookie);
        }
    }

    return result;
}

Error PMP300::InitializeDevice(DeviceInfo* device, 
                               PLMCallBackFunction function,
                               void* cookie)
{
    Error result = kError_InvalidParam;

    assert(device);

    if(device)
    {
        result = kError_DeviceNotFound;

        if(!strcasecmp(device->GetDevice(), devices[0].device))
        {
            CRio rio;
            bool rioPresent = false;

            rioPresent = FindRio(rio, device, function, cookie);

            if(rioPresent)
            {
                if(function)
                {
                    PLMEvent event;
    
                    event.type = kPLMEvent_Status;
                    event.eventString += "A ";
                    event.eventString = device->GetDevice();
                    event.eventString += " has been found. Initializing internal memory...";

                    function(&event, cookie);
                }

                RioProgressStruct ps;

                memset(&ps, 0x00, sizeof(ps));

                ps.function = function;
                ps.cookie = cookie;
                ps.item = NULL;

                result = kError_NoErr;

                if(!rio.Initialize(true, rioProgress, &ps))
                {
                    if(function)
                    {
                        PLMEvent event;
            
                        event.type = kPLMEvent_Error;
                        event.data.errorData.errorCode = rio.GetErrorID();
                        event.eventString = "Initialize failed, ";
                        event.eventString += rio.GetErrorStr();

                        function(&event, cookie);
                    }

                    result = kError_UnknownErr;
                }

                if(function)
                {
                    PLMEvent event;

                    event.type = kPLMEvent_Done;

                    function(&event, cookie);
                }
            }
        }
    }

    return result;

}


Error PMP300::ReadPlaylist(DeviceInfo* device, 
                           vector<PlaylistItem*>* list,
                           PLMCallBackFunction function,
                           void* cookie)
{
    Error result = kError_InvalidParam;

    assert(device);
    assert(list);

    if(device && list)
    {
        result = kError_DeviceNotFound;

        if(!strcasecmp(device->GetDevice(), devices[0].device))
        {
            CRio rio;
            bool rioPresent = false;

            rioPresent = FindRio(rio, device, function, cookie);

            if(rioPresent)
            {
                uint32 totalMem, usedMem; 

                if(function)
                {
                    PLMEvent event;
    
                    event.type = kPLMEvent_Status;
                    event.eventString += "A ";
                    event.eventString = device->GetDevice();
                    event.eventString += " has been found. Scanning internal memory...";

                    function(&event, cookie);
                }

                result = privateReadPlaylist(rio, 
                                             false,  
                                             &totalMem, 
                                             &usedMem, 
                                             list, 
                                             function, 
                                             cookie);

                if(IsntError(result))
	            {
                    uint32 externalTotal, externalUsed;

                    if(function)
                    {
                        PLMEvent event;
    
                        event.type = kPLMEvent_Status;
                        event.eventString = " Scanning external memory...";

                        function(&event, cookie);
                    }

                    rio.UseExternalFlash(true);

                    privateReadPlaylist(rio, 
                                        true,  
                                        &externalTotal,
                                        &externalUsed,
                                        list, 
                                        function, 
                                        cookie);    
                    
                    totalMem += externalTotal;
                    usedMem += externalUsed;

                    rio.UseExternalFlash(false);
                }

                device->SetNumEntries(list->size());
                device->SetCapacity(totalMem, usedMem);

                if(function)
                {
                    PLMEvent event;

                    event.type = kPLMEvent_Done;

                    function(&event, cookie);
                }
            }
        }
    }

    return result;
}

Error PMP300::WritePlaylist(DeviceInfo* device, 
                            vector<PlaylistItem*>* list,
                            PLMCallBackFunction function,
                            void* cookie)
{
    Error result = kError_InvalidParam;

    assert(device);
    assert(list);

    if(device && list)
    {
        result = kError_DeviceNotFound;

        if(!strcasecmp(device->GetDevice(), devices[0].device))
        {
            CRio rioInternal, rioExternal;
            bool rioPresent = false;

            rioPresent = FindRio(rioInternal, device, function, cookie);

            if(rioPresent)
            {
                FindRio(rioExternal, device, function, cookie);

                rioExternal.UseExternalFlash(true);

                if(function)
                {
                    PLMEvent event;
    
                    event.type = kPLMEvent_Status;
                    event.eventString += "A ";
                    event.eventString = device->GetDevice();
                    event.eventString += " has been found. Scanning internal memory...";

                    function(&event, cookie);
                }     

                vector<PlaylistItem*> origInternal;
                vector<PlaylistItem*> origExternal;
                vector<PlaylistItem*> newInternal;
                vector<PlaylistItem*> newExternal;
                uint32 externalTotal = 0, internalTotal = 0, usedMem; 
                uint32 count, index;
                char* path = new char[_MAX_PATH];
                char* tempPath = new char[_MAX_PATH];
                uint32 length = _MAX_PATH;

                // get a temp path
                m_context->prefs->GetPrefString(kInstallDirPref, tempPath, &length);
                strcat(tempPath, tmpnam(NULL));

#ifdef WIN32
                mkdir(tempPath);
#else
                mkdir(tempPath, S_IRWXU);
#endif

                // first get current state of device
                // we break our lists into internal and 
                // external lists for ease of organizing
                result = privateReadPlaylist(rioInternal, 
                                             false,  
                                             &internalTotal,
                                             &usedMem,
                                             &origInternal, 
                                             function, 
                                             cookie); 
                
                if(IsntError(result))
                {
                    if(function)
                    {
                        PLMEvent event;

                        event.type = kPLMEvent_Status;
                        event.eventString = "Scanning external memory...";

                        function(&event, cookie);
                    }

                    privateReadPlaylist(rioExternal, 
                                        true,  
                                        &externalTotal,
                                        &usedMem,
                                        &origExternal, 
                                        function, 
                                        cookie);    
                }

                count = list->size();
                bool useExternal = false;

                for(index = 0; index < count; index++)
                {
                    PlaylistItem* item = (*list)[index];
                    
                    if(item)
                    {
                        MetaData metadata = item->GetMetaData();

                        int32 size = metadata.Size();

                        if(!size)
                        {
                            struct stat st;

                            length = _MAX_PATH;

                            URLToFilePath(item->URL().c_str(), path, &length);

                            if(!stat(path, &st))
                                size = st.st_size;
                            else
                            {
                                result = kError_FileNoAccess;
                                break;
                            }
                        }
                        
                        // figure out where to put it...
                        uint32* memorySize;
                        vector<PlaylistItem*>* addList;

                        if(!useExternal)
                        {
                            memorySize = &internalTotal;
                            addList = &newInternal;

                            if(*memorySize < (uint32)size)
                                useExternal = true;
                            else
                                *memorySize -= size;
                        }
                        
                        if(useExternal)
                        {
                            memorySize = &externalTotal;
                            addList = &newExternal;

                            if(*memorySize < (uint32)size)
                                break;
                            else
                                *memorySize -= size;
                        }
                        
                        addList->push_back(item);
                    }
                }

                // if all is well we delete old files
                // and temporarily download files
                // that are being moved from internal
                // to external and vice versa...
                if(IsntError(result))
                {
                    if(function)
                    {
                        PLMEvent event;

                        event.type = kPLMEvent_Status;
                        event.eventString = "Deleting files...";

                        function(&event, cookie);
                    }

                    count = origInternal.size();

                    for(index = 0; index < count; index++)
                    {
                        PlaylistItem* item = origInternal[index];

                        if(find_if(newInternal.begin(), 
                                   newInternal.end(), 
                                   PlaylistItemCompare(item)) == newInternal.end())
                        {
                            // need to delete it
                            vector<PlaylistItem*>::iterator position;

                            if((position = find_if(newExternal.begin(), 
                                                   newExternal.end(), 
                                                   PlaylistItemCompare(item))) != newExternal.end())
                            {
                                // need to download it to temp file first
                                // and then upload it to other card
                                // in the next stage...

                                string itemPath = item->URL();
                                string downloadPath = tempPath;
                                downloadPath += DIR_MARKER_STR;

                                downloadPath.insert(downloadPath.size(), 
                                                    itemPath, 
                                                    itemPath.rfind('/') + 1, 
                                                    itemPath.size());

                                RioProgressStruct ps;

                                memset(&ps, 0x00, sizeof(ps));

                                ps.function = function;
                                ps.cookie = cookie;
                                ps.item = item;

                                if(!rioInternal.RxFile(downloadPath.c_str(), rioProgress, &ps))
                                {
                                    if(function)
                                    {
                                        PLMEvent event;
    
                                        event.type = kPLMEvent_Error;
                                        event.data.errorData.errorCode = rioInternal.GetErrorID();
                                        event.eventString = "Download failed, ";
                                        event.eventString += rioInternal.GetErrorStr();

                                        function(&event, cookie);
                                    }

                                    if(rioInternal.GetErrorID() == CRIO_ERROR_INTERRUPTED)
                                        result = kError_UserCancel;
                                    else
                                        result = kError_UnknownErr;
                                    break;
                                }

                                length = _MAX_PATH;
                                FilePathToURL(downloadPath.c_str(), path, &length);

                                (*position)->SetURL(path);
                            }

                            string::size_type pos = item->URL().rfind("/") + 1;
                            const char* cp = item->URL().c_str();

                            if(!rioInternal.RemoveFile(cp + pos))
                            {
                                if(function)
                                {
                                    PLMEvent event;
            
                                    event.type = kPLMEvent_Error;
                                    event.data.errorData.errorCode = rioInternal.GetErrorID();
                                    event.eventString = "Delete failed, ";
                                    event.eventString += rioInternal.GetErrorStr();

                                    function(&event, cookie);
                                }

                                result = kError_UnknownErr;
                                break;
                            }

                            delete item;
                            origInternal[index] = NULL;
                        }
                    }

                    if(IsntError(result))
                    {
                        count = origExternal.size();

                        for(index = 0; index < count; index++)
                        {
                            PlaylistItem* item = origExternal[index];

                            if(find_if(newExternal.begin(), 
                                       newExternal.end(), 
                                       PlaylistItemCompare(item)) == newExternal.end())
                            {
                                // need to delete it

                                vector<PlaylistItem*>::iterator position;

                                if((position = find_if(newInternal.begin(), 
                                                    newInternal.end(), 
                                                    PlaylistItemCompare(item))) != newInternal.end())
                                {
                                    // need to download it to temp file first
                                    // and then upload it to other card
                                    // in the next stage...
                                    
                                    string itemPath = item->URL();
                                    string downloadPath = tempPath;
                                    downloadPath += DIR_MARKER_STR;

                                    downloadPath.insert(downloadPath.size(), 
                                                        itemPath, 
                                                        itemPath.rfind('/') + 1, 
                                                        itemPath.size());

                                    RioProgressStruct ps;

                                    memset(&ps, 0x00, sizeof(ps));

                                    ps.function = function;
                                    ps.cookie = cookie;
                                    ps.item = item;

                                    if(!rioExternal.RxFile(downloadPath.c_str(), rioProgress, &ps))
                                    {
                                        if(function)
                                        {
                                            PLMEvent event;
        
                                            event.type = kPLMEvent_Error;
                                            event.data.errorData.errorCode = rioExternal.GetErrorID();
                                            event.eventString = "Download failed, ";
                                            event.eventString += rioExternal.GetErrorStr();

                                            function(&event, cookie);
                                        }

                                        if(rioInternal.GetErrorID() == CRIO_ERROR_INTERRUPTED)
                                            result = kError_UserCancel;
                                        else
                                            result = kError_UnknownErr;
                                        break;
                                    }

                                    length = _MAX_PATH;
                                    FilePathToURL(downloadPath.c_str(), path, &length);

                                    (*position)->SetURL(path);
                                }

                                string::size_type pos = item->URL().rfind("/") + 1;
                                const char* cp = item->URL().c_str();

                                if(!rioExternal.RemoveFile(cp + pos))
                                {
                                    if(function)
                                    {
                                        PLMEvent event;
        
                                        event.type = kPLMEvent_Error;
                                        event.data.errorData.errorCode = rioExternal.GetErrorID();
                                        event.eventString = "Delete failed, ";
                                        event.eventString += rioExternal.GetErrorStr();

                                        function(&event, cookie);
                                    }

                                    result = kError_UnknownErr;
                                    break;
                                }

                                delete item;
                                origExternal[index] = NULL;
                            }
                        }
                    }
                }

                // if all is well we add new files
                // to each card
                if(IsntError(result))
                {                 
                    // remove NULLs caused by deletes
                    origInternal.erase(
                        remove(origInternal.begin(), 
                               origInternal.end(), 
                               (PlaylistItem*)NULL), 
                        origInternal.end());

                    origExternal.erase(
                        remove(origExternal.begin(), 
                               origExternal.end(), 
                               (PlaylistItem*)NULL), 
                        origExternal.end());

                    // sync deletes back to the cards
                    rioInternal.TxDirectory();
                    rioExternal.TxDirectory();

                    if(function)
                    {
                        PLMEvent event;

                        event.type = kPLMEvent_Status;
                        event.eventString = "Uploading new files...";

                        function(&event, cookie);
                    }

                    count = newInternal.size();

                    for(index = 0; index < count; index++)
                    {
                        PlaylistItem* item = newInternal[index];

                        if(item->URL().find("file://") != string::npos)
                        {
                            length = _MAX_PATH;

                            URLToFilePath(item->URL().c_str(), path, &length);

                            RioProgressStruct ps;

                            memset(&ps, 0x00, sizeof(ps));

                            ps.function = function;
                            ps.cookie = cookie;
                            ps.item = item;
                            
                            if(!rioInternal.TxFile(path, rioProgress, &ps))
                            {
                                if(function)
                                {
                                    PLMEvent event;
        
                                    event.type = kPLMEvent_Error;
                                    event.data.errorData.errorCode = rioInternal.GetErrorID();
                                    event.eventString = "Upload failed, ";
                                    event.eventString += rioInternal.GetErrorStr();

                                    function(&event, cookie);
                                }

                                if(rioInternal.GetErrorID() == CRIO_ERROR_INTERRUPTED)
                                    result = kError_UserCancel;
                                else
                                    result = kError_UnknownErr;
                                break;
                            }

                            origInternal.push_back(new PlaylistItem(*item));
                        }
                    }

                    if(IsntError(result))
                    {
                        count = newExternal.size();

                        for(index = 0; index < count; index++)
                        {
                            PlaylistItem* item = newExternal[index];

                            if(item->URL().find("file://") != string::npos)
                            {
                                length = _MAX_PATH;

                                URLToFilePath(item->URL().c_str(), path, &length);

                                RioProgressStruct ps;

                                memset(&ps, 0x00, sizeof(ps));

                                ps.function = function;
                                ps.cookie = cookie;
                                ps.item = item;
                            
                                if(!rioExternal.TxFile(path, rioProgress, &ps))
                                {
                                    if(function)
                                    {
                                        PLMEvent event;
        
                                        event.type = kPLMEvent_Error;
                                        event.data.errorData.errorCode = rioExternal.GetErrorID();
                                        event.eventString = "Upload failed, ";
                                        event.eventString += rioExternal.GetErrorStr();

                                        function(&event, cookie);
                                    }

                                    if(rioExternal.GetErrorID() == CRIO_ERROR_INTERRUPTED)
                                        result = kError_UserCancel;
                                    else
                                        result = kError_UnknownErr;
                                    break;
                                }

                                origExternal.push_back(new PlaylistItem(*item));
                            }
                        }
                    }
                }

                // finally put it all in the correct order
                // and we should be done!
                if(IsntError(result))
                {
                    uint32 entryOrder[ CRIO_MAX_DIRENTRY ];

	                count = newInternal.size();

                    if(count)
                    {

                        for(index = 0; index < count; index++)
                        {
                            vector<PlaylistItem*>::iterator position;
                            PlaylistItem* item = newInternal[index];

                            if((position = find_if(origInternal.begin(), 
                                                  origInternal.end(), 
                                                  PlaylistItemCompare(item))) != origInternal.end())
                            {
//#ifdef WIN32
//								distance(origInternal.begin(), position /*, entryOrder[index] */ );
//#else
								entryOrder[index] = distance(origInternal.begin(), position /*, entryOrder[index] */ );
//#endif
                            }

                        }

                        rioInternal.SetFileOrder(entryOrder, count);
                    }

                    count = newExternal.size();

                    if(count)
                    {
                        for(index = 0; index < count; index++)
                        {
                            vector<PlaylistItem*>::iterator position;
                            PlaylistItem* item = newExternal[index];

                            if((position = find_if(origExternal.begin(), 
                                                  origExternal.end(), 
                                                  PlaylistItemCompare(item))) != origExternal.end())
                            {
//#ifdef WIN32
//                                distance(origExternal.begin(), position /*, entryOrder[index] */ );
//#else	                            
								entryOrder[index] = distance(origExternal.begin(), position /*, entryOrder[index] */ );
//#endif

                            }
                        }

                        rioExternal.SetFileOrder(entryOrder, count);
                    }

                    // sync uploads back to the cards
                    rioInternal.TxDirectory();
                    rioExternal.TxDirectory();
                }

                if(function)
                {
                    PLMEvent event;

                    event.type = kPLMEvent_Done;

                    function(&event, cookie);
                }

                // clean up
                length = _MAX_PATH;
                FilePathToURL(tempPath, path, &length);
                strcpy(tempPath, path);

                count = origInternal.size();

                for(index = 0; index < count; index++)
                {
                    PlaylistItem* item = origInternal[index];

                    if(item->URL().find(tempPath) != string::npos)
                    {
                        length = _MAX_PATH;

                        URLToFilePath(item->URL().c_str(), path, &length);

                        remove(path);
                    }

                    delete item;
                }

                count = origExternal.size();

                for(index = 0; index < count; index++)
                {
                    PlaylistItem* item = origExternal[index];

                    if(item->URL().find(tempPath) != string::npos)
                    {
                        length = _MAX_PATH;

                        URLToFilePath(item->URL().c_str(), path, &length);

                        remove(path);
                    }

                    delete item;
                }
 
                
                URLToFilePath(tempPath, path, &length);
                rmdir(path);

                delete [] tempPath;
                delete [] path;
            }
        }       
    }

    return result;
}

Error PMP300::DownloadSong(DeviceInfo* device, 
                           PlaylistItem* item,
                           const char* url,
                           PLMCallBackFunction function,
                           void* cookie)

{
    Error result = kError_InvalidParam;

    assert(device);
    assert(url);
    assert(item);

    if(device && url && item)
    {
        result = kError_DeviceNotFound;

        if(!strcasecmp(device->GetDevice(), devices[0].device))
        {
            CRio rio;
            bool rioPresent = false;

            rioPresent = FindRio(rio, device, function, cookie);

            if(rioPresent)
            {
                char* path = new char[_MAX_PATH];
                uint32 length = _MAX_PATH;

                URLToFilePath(url, path, &length);

                string itemPath = item->URL();
                string downloadPath = path;

                downloadPath.replace(downloadPath.rfind(DIR_MARKER) + 1, 
                                     downloadPath.size(), 
                                     itemPath, 
                                     itemPath.rfind('/') + 1, 
                                     itemPath.size());
                
                RioProgressStruct ps;

                memset(&ps, 0x00, sizeof(ps));

                ps.function = function;
                ps.cookie = cookie;
                ps.item = item;

                result = kError_NoErr;

                rio.RxDirectory();

                if(!rio.RxFile(downloadPath.c_str(), rioProgress, &ps))
                {
                    result = kError_UnknownErr;

                    if(rio.GetErrorID() == CRIO_ERROR_FILENOTFOUND)
                    {
                        rio.UseExternalFlash(true);
                        rio.RxDirectory();

                        if(rio.RxFile(downloadPath.c_str(), rioProgress, &ps))
                        {
                            result = kError_NoErr;
                        }
                    }                   
                }

                if(IsntError(result))
                {
                    if(downloadPath != path)
                    {
                        if(rename(downloadPath.c_str(), path))
                        {
                            result = kError_FileNoAccess;
                        }
                    }
                }
                else
                {
                    if(function)
                    {
                        PLMEvent event;

                        event.type = kPLMEvent_Error;
                        event.data.errorData.errorCode = rio.GetErrorID();
                        event.eventString = "Download failed, ";
                        event.eventString += rio.GetErrorStr();

                        function(&event, cookie);
                    }

                    if(rio.GetErrorID() == CRIO_ERROR_INTERRUPTED)
                        result = kError_UserCancel;
                }
                
                if(function)
                {
                    PLMEvent event;

                    event.type = kPLMEvent_Done;

                    function(&event, cookie);
                }

                delete [] path;
            }
        }   
    }

    return result;
}

bool FindRio ( CRio& rio, 
               DeviceInfo* device,
               PLMCallBackFunction function,
               void* cookie)
{
    bool rioPresent = false;

    if(function)
    {
        PLMEvent event;

        event.type = kPLMEvent_Status;
        event.eventString = "Searching for portable device...";

        function(&event, cookie);
    }

    if(device->GetPortAddress() && 
       rio.Set(device->GetPortAddress()) && 
       rio.CheckPresent())
    {
        rioPresent = true;
    }
    else // brute force it...
    {
        for(uint32 count = 0; count < sizeof(ports); count++)
        {
            if(rio.Set(ports[count]) && rio.CheckPresent())
            {
                device->SetPortAddress(ports[count]);
                rioPresent = true;
                break;
            }
        }
    }

    return rioPresent;
}

Error privateReadPlaylist(CRio& rio, 
                           bool useExternal,
                           uint32* totalMem,
                           uint32* usedMem,
                           vector<PlaylistItem*>* list,
                           PLMCallBackFunction function,
                           void* cookie)
{
    Error result = kError_UnknownErr;

    if(rio.RxDirectory())
	{
        result = kError_NoErr;

        CDirBlock& cDirBlock = rio.GetDirectoryBlock();
        CDirHeader& cDirHeader = cDirBlock.m_cDirHeader;

        *totalMem = ((long)cDirHeader.m_usCount32KBlockAvailable * CRIO_SIZE_32KBLOCK);
        *usedMem = ((long)cDirHeader.m_usCount32KBlockUsed * CRIO_SIZE_32KBLOCK);

        uint32 count = cDirHeader.m_usCountEntry;

	    if(count)
	    {
		    CDirEntry* pDirEntry = cDirBlock.m_acDirEntry;

		    if(count > CRIO_MAX_DIRENTRY)
			    count = CRIO_MAX_DIRENTRY;


            for(uint32 index = 0; index < count; ++index, ++pDirEntry)
            {
                string url;
                ostringstream ost;
                MetaData metadata;

                ost << "portable://rio_pmp300/" << 
                    (useExternal ?  "external/" : "internal/") << index <<
                    "/" << pDirEntry->m_szName;
                url = ost.str();

                metadata.SetSize(pDirEntry->m_lSize);
                metadata.SetTitle(pDirEntry->m_szName);

                PlaylistItem* item = new PlaylistItem(url.c_str(), &metadata);

                if(!item)
                {
                    result = kError_OutOfMemory;  
                    break;
                }

                list->push_back(item);

                if(function)
                {
                    PLMEvent event;

                    event.type = kPLMEvent_Progress;
                    event.data.progressData.position = index + 1;
                    event.data.progressData.total = count;
                    event.data.progressData.item = item;

                    function(&event, cookie);
                }
            }
	    }       
    }

    return result;
}

#ifdef WIN32

#include <windows.h>

static 
bool
InstallDriver(  SC_HANDLE  scManager,
                const char* driverName,
                const char* driverPath)

{
    bool result = false;
    SC_HANDLE  service;

    service = CreateService(scManager,              // SCManager database
                            driverName,             // name of service
                            driverName,             // name to display
                            SERVICE_ALL_ACCESS,     // desired access
                            SERVICE_KERNEL_DRIVER,  // service type
                            SERVICE_DEMAND_START,   // start type
                            SERVICE_ERROR_NORMAL,   // error control type
                            driverPath,             // service's binary
                            NULL,                   // no load ordering group
                            NULL,                   // no tag identifier
                            NULL,                   // no dependencies
                            NULL,                   // LocalSystem account
                            NULL);                  // no password


                                
    if(service || ERROR_SERVICE_EXISTS == GetLastError())
    {
        result = true;
    }
    /*else
    {
        LPVOID lpMessageBuffer;
        int32 error = GetLastError();

		FormatMessage(
		  FORMAT_MESSAGE_ALLOCATE_BUFFER |
		  FORMAT_MESSAGE_FROM_SYSTEM,
		  NULL,
		  error,
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lpMessageBuffer,
		  0,
		  NULL );

		// now display this string
 		MessageBox(NULL, (char*)lpMessageBuffer, 0, MB_OK);

		// Free the buffer allocated by the system
		LocalFree( lpMessageBuffer );

    }*/
    
    CloseServiceHandle(service);

    return result;
}

static
bool
StartDriver(SC_HANDLE scManager,
            const char* driverName)
{
    bool result = false;
    SC_HANDLE  service;

    service = OpenService(  scManager,
                            driverName,
                            SERVICE_ALL_ACCESS);

    if(service)
    {
        BOOL error;

        error = StartService(service, // service identifier
                             0,       // number of arguments
                             NULL);   // pointer to arguments

        result = (error != 0);

        if(!result)
        {
            if(GetLastError() == ERROR_SERVICE_ALREADY_RUNNING)
                result = true;
        }

        CloseServiceHandle(service);    
    }
    
    return result;
}

static
bool
OpenDevice(const char* driverName)
{
    bool    result = false;
    char    path[MAX_PATH];
    HANDLE  handle;

    sprintf(path, "\\\\.\\%s", driverName);

    handle = CreateFile(path,
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);

    if(handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(handle);
        result = true;
    }
   
    return result;
}


bool 
LoadDriver(const char* driverName, 
           const char* driverPath)
{

    bool result = false;
    SC_HANDLE scManager;

    scManager = OpenSCManager(NULL,
                              NULL,
                              SC_MANAGER_ALL_ACCESS);

    if(scManager)
    {
        result = InstallDriver( scManager,
                                driverName,
                                driverPath);
        if(result)
        {
            result = StartDriver(scManager,
                                 driverName);

            if(result)
            {
                result = OpenDevice(driverName);
            }
        }

        CloseServiceHandle(scManager);
    }

    return result;
}

static
bool
StopDriver( SC_HANDLE scManager,
            const char* driverName)
{
    bool result = false;
    SC_HANDLE service;
    SERVICE_STATUS  serviceStatus;

    service = OpenService(  scManager,
                            driverName,
                            SERVICE_ALL_ACCESS);

    if(service)
    {
        BOOL error;

        error = ControlService( service,
                                SERVICE_CONTROL_STOP,
                                &serviceStatus);

        result = (error > 0);

        CloseServiceHandle(service);
    }

    return result;
}

static
bool
RemoveDriver(   SC_HANDLE scManager,
                const char* driverName)
{
    bool result = false;
    SC_HANDLE  service;

    service = OpenService(  scManager,
                            driverName,
                            SERVICE_ALL_ACCESS);
    if(service)
    {
        BOOL error;

        error = DeleteService(service);

        result = (error > 0);

        CloseServiceHandle(service);
    }

    return result;
}

bool 
UnloadDriver(const char* driverName)
{
    bool result = false;
    SC_HANDLE scManager;

    scManager = OpenSCManager(NULL,
                              NULL,
                              SC_MANAGER_ALL_ACCESS);

    if(scManager)
    {
        result = StopDriver(scManager,
                            driverName);

        if(result)
        {
            result = RemoveDriver(  scManager, 
                                    driverName);
        }

        CloseServiceHandle(scManager);
    }

    return result;
}

#endif
