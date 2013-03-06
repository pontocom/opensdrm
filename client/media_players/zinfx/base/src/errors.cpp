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
	
	$Id: errors.cpp,v 1.1 2004/10/13 16:16:04 linha2 Exp $
____________________________________________________________________________*/

#include "config.h"

const char* ErrorString[] = {

    "The operation was successful", //              kError_NoErr            = 0,
    "An unknown error occurred", //                 kError_UnknownErr       = 1,
    "An invalid parameter was used", //             kError_InvalidParam     = 2,
    "There was an error loading the library", //    kError_LoadLibFailed    = 3,
    "Could not find the function", //               kError_FindFuncFailed   = 4,
    "No files were found", //              = 5,
    "Unable to locate preferences", //              kError_NoPrefs = 6,
    "The buffer was too small to hold the data", // kError_BufferTooSmall = 7,
    "Memory allocation failed", //                  kError_OutOfMemory = 8,
    "Access to the file is denied", //              kError_FileNoAccess = 9,
    "The file already exists", //                   kError_FileExists = 10,
    "An invalid argument was used", //              kError_FileInvalidArg = 11,
    "The file was not found", //                    kError_FileNotFound = 12,
    "kError_FileNoHandles", //        = 13,
    "kError_NoMoreLibDirs", //        = 14,
    "A NULL value is invalid", //                   kError_NullValueInvalid = 15,
    "kError_InvalidError", //         = 16,
    "The plugin was not initialized", //            kError_PluginNotInitialized = 17,
    "kError_GotDefaultMethod", //     = 18,
    "Input was unsuccessful", //                    kError_InputUnsuccessful = 19,
    "Seeking is not supported", //                  kError_FileSeekNotSupported = 20,
    "Output was unsuccessful", //                   kError_OutputUnsuccessful = 21,
    "Initialization failed", //                     kError_InitFailed = 22,
    "Initialization failed safely", //              kError_InitFailedSafely = 23,
    "No data is available", //                      kError_NoDataAvail = 24,
    "The seek operation failed", //                 kError_SeekFailed = 25,
    "The thread was not created", //                kError_CreateThreadFailed = 26,
    "kError_BufferingUp", //          = 27,
    "kError_Interrupt", //            = 28,
    "kError_EventPending", //         = 29,
    "Hey, you screwed up", //                       kError_YouScrewedUp = 30,
    "No preference value was found", //             kError_NoPrefValue = 31,
    "There was an error in the syntax", //          kError_SyntaxError = 32,
    "Reached the end of the stream", //             kError_EndOfStream = 33,
    "Failed to connect to host", //                 kError_ConnectFailed = 34,
    "Failed to create socket", //                   kError_CantCreateSocket = 35,
    "Unable to set socket options", //              kError_CannotSetSocketOpts = 36,
    "Cannot bind socket", //                        kError_CannotBind = 37,
    "The database item was not stored", //          kError_DbaseItemNotStored = 38,
    "There was an error parsing the file", //       kError_ParseError = 39,
    "Error, not found", //                          kError_NotFound = 40,
    "There are no more formats left to iterate", // kError_NoMoreFormats = 41,
    "The requested format is not supported", //     kError_FormatNotSupported = 42,
    "There are no more devices left to iterate", // kError_NoMoreDevices = 43,
    "The requested device was not found", //        kError_DeviceNotFound = 44,
    "The requested feature is not supported", //    kError_FeatureNotSupported = 45,
    "The operation was cancelled by the user", //   kError_UserCancel = 46,
    "Unable to load bitmap image", //               kError_LoadBitmapFailed = 47,
    "The requested protocol is not supported", //   kError_ProtocolNotSupported = 48,
    "The URL is invalid", //                        kError_InvalidURL = 49,
    "Unable to locate host", //                     kError_CantFindHost = 50,
    "An I/O error occurred", //                     kError_IOError = 51,
    "There was an unknown server error", //         kError_UnknownServerError = 52,
    "A bad HTTP request was made", //               kError_BadHTTPRequest = 53,
    "Access is not authorized", //                  kError_AccessNotAuthorized = 54,
    "Access is forbidden", //                       kError_AccessForbidden = 55,  
    "The requested range is not acceptable", //     kError_RangeNotExceptable = 56,
    "Could not write to the file", //            = 57,
    "Could not read from the file", //             = 58,
    "Invalid version", //                           kError_InvalidVersion = 59,
    "An update is already being performed", //      kError_AlreadyUpdating = 60,
    "Failed to delete file", //                     kError_UnlinkFailed = 61,
    "Failed to copy file", //                       kError_CopyFailed = 62,
    "No items were updated", //                     kError_NoItemsUpdated = 63,
    "The database is not working", //               kError_DatabaseNotWorking = 64,
    "Duplicate database item", //                   kError_DuplicateItem = 65,
    "The requested item was not found", //          kError_ItemNotFound = 66,
    "The requested file was not found", //          kError_HTTPFileNotFound  = 67,
    "Failed to initialize your CD-ROM", //          kError_CDInitFailed = 68,
    "Cannot play CD track",             //          kError_InvalidTrack = 69,
    "Download denied", //                           kError_DownloadDenied = 70,
    "No CD is in the CD-ROM drive",     //          kError_NoDiscInDrive = 71,  

    "kError_LastError", //            = 9999
};


