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
	
	$Id: portio.c,v 1.1 2004/10/13 16:16:09 linha2 Exp $
____________________________________________________________________________*/
/* system headers */
#include <ntddk.h>
#include <stdlib.h>

typedef unsigned char BYTE;

// Normally, under Windows NT, access is not permitted to the I/O
// port range. Using an array we will allow access by setting a 0
// bit to the corresponding port. Any bits which are set to 1 
// indicate that access should be denied.

// The I/O Permission Map (IOPM) is 64K bits under the Intel x86 
// architecture. We will probably have to do something different 
// here in order to get this working under Alpha NT.
// here is our bit array definition...
typedef BYTE IOPM[0x2000];

// and here is a pointer that we will use to hold the array.
IOPM *ioMap = NULL;

WCHAR devicePath[] = L"\\Device\\portio";
WCHAR dosDevicePath[] = L"\\DosDevices\\portio";

// Special thanks to Dale Roberts and Dr. Dobb's Journal <http://www.ddj.com>
// for telling us about these undocumented functions. According to Dale:
//
// Ke386IoSetAccessMap() copies the passed map to the TSS.
//
// Ke386IoSetAccessProcess() adjusts the IOPM offset pointer so that
// the newly copied map is actually used.  Otherwise, the IOPM offset
// points beyond the end of the TSS segment limit, causing any I/O
// access by the user mode process to generate an exception.

void Ke386SetIoAccessMap(int, IOPM*);
void Ke386QueryIoAccessMap(int, IOPM*);
void Ke386IoSetAccessProcess(PEPROCESS, int);

// Be good computer citizens and clean up after ourselves...
void DriverUnload(IN PDRIVER_OBJECT driverObject)
{
	UNICODE_STRING dosName;

    // free the memory we used to store our IOPM
	if(ioMap)
    {
		MmFreeNonCachedMemory(ioMap, sizeof(IOPM));
    }

    RtlInitUnicodeString(&dosName, dosDevicePath);

	IoDeleteSymbolicLink (&dosName);
	IoDeleteDevice(driverObject->DeviceObject);
}

// This is where the magic happens... when the driver is loaded we
// call the magic undocumented kernel functions and write over the 
// I/O Permission Map (IOPM).
NTSTATUS DriverCreateDispatch(
    IN  PDEVICE_OBJECT  deviceObject,
    IN  PIRP            irp)
{
    NTSTATUS result = STATUS_SUCCESS;

	Ke386IoSetAccessProcess(PsGetCurrentProcess(), 1);
	Ke386SetIoAccessMap(1, ioMap);

    irp->IoStatus.Information = 0;
    irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return result;
}

// Standard entry point for the device driver. Initialize ourselves
// and create the proper links to ourselves...
NTSTATUS DriverEntry(
    IN PDRIVER_OBJECT driverObject,
    IN PUNICODE_STRING registryPath)
{
    NTSTATUS result = STATUS_INSUFFICIENT_RESOURCES;
	PDEVICE_OBJECT deviceObject;
	UNICODE_STRING deviceName, dosName;

	ioMap = MmAllocateNonCachedMemory(sizeof(IOPM));

	if(ioMap)
    {
        // this will allow access to all ports. if we want to 
        // allow limited access we could set only ports we want
        // to access to 0.
	    RtlZeroMemory(ioMap, sizeof(IOPM));

	    // create unicode string for device name;
        //wcscpy(foo, L"\\Device\\portio");
	    RtlInitUnicodeString(&deviceName, devicePath);

        // create the actual device
        result = IoCreateDevice(driverObject, 
                                0,
					            &deviceName,
					            FILE_DEVICE_UNKNOWN,
					            0, 
                                FALSE, 
                                &deviceObject);

        if(NT_SUCCESS(result))
        {
            // create symbolic link to driver
	        RtlInitUnicodeString(&dosName, dosDevicePath);

	        result = IoCreateSymbolicLink (&dosName, &deviceName);

	        if(NT_SUCCESS(result))
            {
                // finally register the entry points for the driver functions
                driverObject->MajorFunction[IRP_MJ_CREATE] = DriverCreateDispatch;
	            driverObject->DriverUnload = DriverUnload;

                result = STATUS_SUCCESS;
            }
        }
    }
    
    return result;
}

