///////////////////////////////////////////////////////////////////////////////
//
//	rioioctl.h
//
///////////////////////////////////////////////////////////////////////////////

// Device type
#define		RIOIO_ID_DEVICE			40000

#define		RIOIO_IOCTL_GETVERSION	CTL_CODE( RIOIO_ID_DEVICE, 0x900, METHOD_BUFFERED, FILE_READ_ACCESS )
#define		RIOIO_IOCTL_READ		CTL_CODE( RIOIO_ID_DEVICE, 0x901, METHOD_BUFFERED, FILE_READ_ACCESS )
#define		RIOIO_IOCTL_WRITE		CTL_CODE( RIOIO_ID_DEVICE, 0x902, METHOD_BUFFERED, FILE_WRITE_ACCESS )
#define		RIOIO_IOCTL_READBLOCK	CTL_CODE( RIOIO_ID_DEVICE, 0x903, METHOD_BUFFERED, FILE_WRITE_ACCESS )
#define		RIOIO_IOCTL_WRITEBLOCK	CTL_CODE( RIOIO_ID_DEVICE, 0x904, METHOD_BUFFERED, FILE_WRITE_ACCESS )

