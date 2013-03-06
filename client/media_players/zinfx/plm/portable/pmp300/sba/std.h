//////////////////////////////////////////////////////////////////////////////
//
//	std.h
//
//////////////////////////////////////////////////////////////////////////////

#if defined(__OS2__)
	#define		INCL_DOSMISC
	#define		INCL_BASE
	#include	<os2.h>
	#define		_STD_
#endif

#ifndef		_STD_
#define		_STD_

#ifndef		TRUE
#define		TRUE		(1==1)
#endif

#ifndef		FALSE
#define		FALSE		(1==0)
#endif

#ifndef		BOOL
typedef int BOOL;
#endif

#ifndef		CBOOL
typedef char CBOOL;
#endif

#ifndef		SZERROR
#define		SZERROR						sys_errlist[errno]
#endif

typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned int UINT;
typedef unsigned long ULONG;

#endif		// _STD_
