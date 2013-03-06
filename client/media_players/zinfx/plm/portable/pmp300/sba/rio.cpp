///////////////////////////////////////////////////////////////////////////////
//
//	rio.cpp
//
//	v1.01 14/01/99	Initial launch.
//
//	v1.02 18/01/99	Additional debug added for ioperm() call.
//
//	v1.03 25/01/99	Added download support.
//					Added delete support.
//					Progress callback for upload/download file.
//					Boland Turbo C++ v1.01 supported.
//
//	v1.04 27/01/99	Win NT v4.0 supported, using RIOIO driver provided.
//
//	v1.05 29/01/99	Added version field in dir header to be compatible
//					with later Rio Manager v1.01 software.
//					Added CheckPresent() member function.
//					Support for Alpha platform.
//
//	v1.06 11/03/99	Added support for external flash ram.
//					Added initialization with bad block check.
//					Added file re-ordering support.
//
//	v1.07 10/06/99	Added support for FreeBSD.
//					Added support for BSDI.
//					Added support for OS/2.
//					Added support for Rio 64M SE (Special Edition).
//					Improved detection of device, using manufacturer ID.
//					Selectable IO delays.
//
///////////////////////////////////////////////////////////////////////////////
#include	<stdio.h>
#include	<string.h>
#include	<stdarg.h>
#include	<time.h>
#include	<errno.h>
#include	<sys/stat.h>

#include	"rio.h"
#include	"binary.h"

// platform dependencies
#if defined(_WINNT)
	// MS VC++ v5.0 for WinNT v4
	#include	<windows.h>
	#include	<winioctl.h>
	#include	"rioioctl.h"
	#define		OUTPORT( p, v )			WinNTOutPort( p, v )
	#define		INPORT( p )				WinNTInPort( p )
	#define		CLOCK_SECOND			CLOCKS_PER_SEC
	#define		DELETEARRAY				delete[]
	#define		ID_DRIVER_VERSION		101

#elif defined(_WIN32)
	// MS VC++ v5.0 for Win9x
	#include	<conio.h>
	#define		OUTPORT( p, v )			_outp( p, v )
	#define		INPORT( p )				_inp( p )
	#define		CLOCK_SECOND			CLOCKS_PER_SEC
	#define		DELETEARRAY				delete[]

#elif defined(__linux__)
	// linux g++
	#include	<unistd.h>
	#include	<sys/io.h>
	#include	<asm/io.h>
	#define		OUTPORT(p,v)			outb( v, p )
	#define		INPORT(p)				inb( p )
	#define		CLOCK_SECOND			CLOCKS_PER_SEC
	#define		DELETEARRAY				delete[]

#elif defined(__FreeBSD__)
	// FreeBSD g++
	#include	<fcntl.h>
	#include	<unistd.h>
	#include	<machine/cpufunc.h>
	#define		OUTPORT(p,v)			outb( p, v )
	#define		INPORT(p)				inb( p )
	#define		CLOCK_SECOND			CLOCKS_PER_SEC
	#define		DELETEARRAY				delete[]

#elif defined(__bsdi__)
	// BSD/OS g++
	#include	<unistd.h>
	#include	<machine/inline.h>
	#define		OUTPORT(p,v)			outb( p, v )
	#define		INPORT(p)				inb( p )
	#define		CLOCK_SECOND			CLOCKS_PER_SEC
	#define		DELETEARRAY				delete[]

#elif defined(__OS2__)
	// OS/2 VisualAge C++ v. 3
	#include	"iopl32.h"
	#define		OUTPORT(p,v)			WPORT( p, v )
	#define		INPORT(p)				((UCHAR)RPORT( p ))
	#define		CLOCK_SECOND			CLOCKS_PER_SEC
	#define		DELETEARRAY				delete[]

#elif defined(__TURBOC__)
	// turboc v1.01
	#include	<dos.h>
	#define		OUTPORT( p, v )			outp( p, v )
	#define		INPORT( p )				inp( p )
	#define		CLOCK_SECOND			((int)CLOCKS_PER_SEC)
	#define		DELETEARRAY				delete

#else
	// not supported
	#error ! ! compiler/platform not supported ! !
#endif

// port offset constants
#define		OFFSET_PORT_DATA		0
#define		OFFSET_PORT_STATUS		1
#define		OFFSET_PORT_CONTROL		2

// max tx/rx block retry
#define		MAX_RETRY				3

// flash ram bank pos
#define		POS_BANK_EXTERNALFLASH	4

// delay's
#define		IODELAY(c)				{ for( long _lA=0; _lA<c; ++_lA ) INPORT(m_iPortStatus); }
#define		DELAY(t)				{ long _lTime=clock()+t; while( _lTime > clock() ) ; }

// new, delete
#define		NEWBLOCK( p )			{ p = new UCHAR[ CRIO_SIZE_32KBLOCK ]; if ( !p ) { LogError( CRIO_ERROR_ALLOC, "new failed" ); return FALSE; } }
#define		ZERONEWBLOCK( p )		{ NEWBLOCK(p); memset(p, 0, CRIO_SIZE_32KBLOCK); }
#define		DELETEBLOCK( p )		{ if ( p ) { DELETEARRAY p; p = NULL; } }

// command out
#define		COMMANDOUT(v1, v2, v3)	{ OUTPORT(m_iPortData, v1); OUTPORT(m_iPortControl, v2); OUTPORT(m_iPortControl, v3); }
// wait for reply
#define		WAITNIBBLE( v1 )		{ if (!WaitInput(v1)) return FALSE; }
#define		WAITACK()				{ if (!WaitAck()) return FALSE; }

///////////////////////////////////////////////////////////////////////////////
// if WinNT
#if defined(_WINNT)

	// handle
	static HANDLE m_hDriver;

	// WinNT out port
	void WinNTOutPort( int iPort, int iValue )
	{
		DWORD dwSizeReturn;
		ULONG ulInput = ((ULONG)iPort-0x378) | ((ULONG)iValue << 16);
		DeviceIoControl( m_hDriver, RIOIO_IOCTL_WRITE, &ulInput,
			sizeof(long), NULL, 0, &dwSizeReturn, NULL );
	}

	// WinNT in port
	int WinNTInPort( int iPort )
	{
		DWORD dwSizeReturn;
		ULONG ulPort = iPort - 0x378;
		ULONG ulData = 0;
		DeviceIoControl( m_hDriver, RIOIO_IOCTL_READ, &ulPort, sizeof(ulPort),
			&ulData, sizeof(char), &dwSizeReturn, NULL );
		return (int)ulData;
	}

#endif

///////////////////////////////////////////////////////////////////////////////
// if DOS
#if defined(__TURBOC__)
	// get clock ticks
	long clock( void )
	{
		return (long) (*(int far*)MK_FP( 0x40, 0x6c ));
	}
#endif

///////////////////////////////////////////////////////////////////////////////
// return file only
static const char* GetFile( const char* pszPathFile )
{
	int iLength = strlen( pszPathFile );
	if ( !iLength )
		return pszPathFile;

	const char* pc = pszPathFile + iLength - 1;
	while( *pc != '\\' && *pc != '/' && *pc != ':' )
	{
		if ( pc == pszPathFile )
			return pc;
		--pc;
	}
	++pc;

	return pc;
}

///////////////////////////////////////////////////////////////////////////////
// set/unset, constructors and destructors
void CRio::Unset( void )
{
	// if WinNT
	#if defined(_WINNT)
		// close device file
		if ( m_hDriver )
		{
			CloseHandle( m_hDriver );
			m_hDriver = NULL;
		}
	#endif
}

BOOL CRio::Set( int iPortBase )
{
	// cleanup any previous Set()
	Unset();

	// determine ports
	m_iPortBase = iPortBase;
	m_iPortData = m_iPortBase + OFFSET_PORT_DATA;
	m_iPortStatus = m_iPortBase + OFFSET_PORT_STATUS;
	m_iPortControl = m_iPortBase + OFFSET_PORT_CONTROL;

	// if linux
	#if defined(__linux__)
		// request access to required ports
		if ( ioperm(m_iPortBase, 3, 1) )
		{
			LogError( CRIO_ERROR_IOPRERM, "ioperm() failed, reason '%s'", SZERROR );
			return FALSE;
		}
	#endif

	// if FreeBSD
	#if defined(__FreeBSD__)
		// request direct access to memory addresses
		if ( open("/dev/io", O_RDONLY) == -1 )
		{
			LogError( CRIO_ERROR_IOPRERM, "open() failed, reason '%s'\n", SZERROR );
			return FALSE;
		}
	#endif

	// if WinNT
	#if defined(_WINNT)
		// open generic IO device
		m_hDriver = CreateFile( "\\\\.\\RioDev", GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
		if ( m_hDriver == INVALID_HANDLE_VALUE )
		{
			LogError( CRIO_ERROR_CREATEFILE, "CreateFile() failed, reason %ld\n", GetLastError() );
			return FALSE;
		}
		// check version
		DWORD dwSizeReturn;
		ULONG ulVersion;
		DeviceIoControl( m_hDriver, RIOIO_IOCTL_GETVERSION, NULL,
			0, &ulVersion, sizeof(ulVersion), &dwSizeReturn, NULL );
		if ( ulVersion != ID_DRIVER_VERSION )
		{
			LogError( CRIO_ERROR_DRIVERVERSION, "incorrect RioIO driver version, v%d.%d loaded, v%d.%d expected\n",
				 ulVersion/100, ulVersion%100, ID_DRIVER_VERSION/100, ID_DRIVER_VERSION%100 );
			return FALSE;
		}

	#endif

	return TRUE;
}

CRio::CRio()
{
	// if WinNT
	#if defined(_WINNT)
		m_hDriver = NULL;
	#endif

	// default to use internal flash
	m_bUseExternalFlash = FALSE;

	// default to not using special edition
	m_bSpecialEdition = FALSE;

	// default io delay's
	m_lTimeIODelayInit = CRIO_TIME_IODELAY_INIT;
	m_lTimeIODelayTx = CRIO_TIME_IODELAY_TX;
	m_lTimeIODelayRx = CRIO_TIME_IODELAY_RX;
}

CRio::~CRio()
{
	Unset();
}

///////////////////////////////////////////////////////////////////////////////
// log error
void CRio::LogError( int iIDError, const char* pszFormat, ... )
{
	va_list ArgPtr;
	va_start( ArgPtr, pszFormat );
	vsprintf( m_szError, pszFormat, ArgPtr );
	va_end( ArgPtr );

	m_iIDError = iIDError;
}

// find first free 32K block
UINT CRio::FindFirstFree32KBlock( void )
{
	UINT uiA;
	UCHAR* puc = m_cDirBlock.m_auc32KBlockUsed;
	for( uiA=0; uiA<CRIO_MAX_32KBLOCK; ++uiA, ++puc )
	{
		if ( *puc == CRIO_ID_32KBLOCK_FREE )
			return uiA;
	}
	return 0xffff;
}

// calculate checksum1 (checksum of directory header)
UINT CRio::CalculateChecksum1( void )
{
	USHORT usChecksum = m_cDirBlock.m_cDirHeader.m_usChecksum1;
	USHORT* paus = (USHORT*)&m_cDirBlock.m_cDirHeader;
	for( unsigned int iA=0; iA<(sizeof(CDirHeader)/sizeof(short)); ++iA )
		usChecksum -= *paus++;
	return usChecksum;
}

// calculate checksum2 (checksum of directory entries, used flags and FAT)
UINT CRio::CalculateChecksum2( void )
{
	USHORT usChecksum = 0;
	USHORT* paus = (USHORT*)m_cDirBlock.m_acDirEntry;
	int iSize = ( sizeof(CDirBlock) - sizeof(CDirHeader) ) / sizeof(short);
	for( int iA=0; iA<iSize; ++iA )
		usChecksum -= *paus++;
	return usChecksum;
}

// wait for	requested input from status port
BOOL CRio::WaitInput( int iValue )
{
	long lTime = clock() + CLOCK_SECOND;
	UCHAR ucRx;
	while( lTime > clock() )
	{
		ucRx = INPORT( m_iPortStatus ) & 0xf8;

		if ( ucRx == (int)iValue )
			return TRUE;
	}

	return FALSE;
}

// wait for	ack
BOOL CRio::WaitAck( void )
{
	long lTime = clock() + CLOCK_SECOND;
	while( lTime > clock() )
	{
		UCHAR ucRx = INPORT( m_iPortStatus );
		if ( ucRx & 0x08 )
			return TRUE;
	}

	return FALSE;
}

// get byte from status port
UINT CRio::GetDataByte( void )
{
	// get hi nibble
	OUTPORT( m_iPortControl, B_00000000 );
	IODELAY( m_lTimeIODelayRx );
	UCHAR ucRx = INPORT( m_iPortStatus );
	UCHAR ucIn = ((ucRx & 0xf0) ^ 0x80) >> 4;

	// get lo nibble and combine with previous nibble to make byte
	OUTPORT( m_iPortControl, B_00000100 );
	IODELAY( m_lTimeIODelayRx );
	ucRx = INPORT( m_iPortStatus );
	ucIn |= (ucRx & 0xf0) ^ 0x80;

	// reverse bits in byte
	UCHAR ucReversed = 0;
	for( int iC=0; iC<8; ++iC )
	{
		ucReversed <<= 1;
		ucReversed |= (ucIn & 1);
		ucIn >>= 1;
	}

	return ucReversed;
}

// io intro
BOOL CRio::IOIntro( void )
{
	OUTPORT( m_iPortControl, B_00000100 );

	COMMANDOUT( B_10101000, B_00001100, B_00000100 );

	OUTPORT( m_iPortControl, B_00000000 );
	IODELAY( m_lTimeIODelayInit );

	OUTPORT( m_iPortControl, B_00000100 );
	IODELAY( m_lTimeIODelayInit );

	COMMANDOUT( B_10101101, B_00001100, B_00000100 );
	COMMANDOUT( B_01010101, B_00000000, B_00000100 );
	COMMANDOUT( B_10101110, B_00001100, B_00000100 );
	COMMANDOUT( B_10101010, B_00000000, B_00000100 );
	COMMANDOUT( B_10101000, B_00001100, B_00000100 );

	OUTPORT( m_iPortControl, B_00000000 );
	IODELAY( m_lTimeIODelayInit );

	OUTPORT( m_iPortControl, B_00000100 );
	IODELAY( m_lTimeIODelayInit );

	return TRUE;
}

// io outro
BOOL CRio::IOOutro( void )
{
	COMMANDOUT( B_10101101, B_00001100, B_00000100 );
	COMMANDOUT( B_11111111, B_00000000, B_00000100 );
	OUTPORT( m_iPortData, B_00000000 );
	return TRUE;
}

// tx 32K block retry
BOOL CRio::Tx32KBlockRetry( void* pv, UINT uiPos32KBlock, UINT uiPos32KBlockPrev,
	UINT uiPos32KBlockNext )
{
	ULONG ulPos512ByteBlock;
	ULONG ulPosHi;
	ULONG ulPosMid;
	ULONG ulPosLo;
	unsigned int iA, iB;

	// io intro
	if ( !IOIntro() )
		return FALSE;

	// if internal, blocksize = 8K else if external blocksize = 16K
	int iCount8K = m_bUseExternalFlash ? 2 : 1;

	// prepare all pages in block
	for( iA=0; iA<4; iA+=iCount8K )
	{
		ulPos512ByteBlock = ((ULONG)uiPos32KBlock * 32768 + (ULONG)iA * 8192) / 512;
		ulPosLo = ulPos512ByteBlock & 0xff;

		if ( m_bUseExternalFlash )
		{
			ulPosHi = POS_BANK_EXTERNALFLASH;
			ulPosMid = (ulPos512ByteBlock & 0xff00) >> 8;
		}
		else if ( m_bSpecialEdition )
		{
			ulPosHi	 = ulPos512ByteBlock / 32768;
			ulPosMid = (ulPos512ByteBlock & 0x7f00) >> 8;
		}
		else
		{
			ulPosHi = ulPos512ByteBlock / 16384;
			ulPosMid = (ulPos512ByteBlock & 0x3f00) >> 8;
		}

		COMMANDOUT( B_10101011, B_00001100, B_00000100 );
		COMMANDOUT( ulPosHi, B_00000000, B_00000100 );
		COMMANDOUT( B_10100001, B_00001100, B_00000100 );
		COMMANDOUT( B_01100000, B_00000000, B_00000100 );
		COMMANDOUT( B_10100010, B_00001100, B_00000100 );
		COMMANDOUT( ulPosLo, B_00000000, B_00000100 );
		COMMANDOUT( ulPosMid, B_00000000, B_00000100 );
		COMMANDOUT( B_10100001, B_00001100, B_00000100 );
		COMMANDOUT( B_11010000, B_00000000, B_00000100 );

		WAITACK();

		COMMANDOUT( B_10100001, B_00001100, B_00000100 );
		COMMANDOUT( B_01110000, B_00000000, B_00000100 );
		COMMANDOUT( B_10100000, B_00001100, B_00000100 );

		OUTPORT( m_iPortControl, B_00000000 );
		IODELAY( m_lTimeIODelayTx );

		OUTPORT( m_iPortControl, B_00000100 );
		IODELAY( m_lTimeIODelayTx );
	}

	// send 32K in 512 byte chunks
	UCHAR* pauc = (UCHAR*)pv;
	for( iA=0; iA<(32768/512); ++iA, pauc+=512 )
	{
		ulPos512ByteBlock = ((ULONG)uiPos32KBlock * 32768 + (ULONG)iA * 512) / 512;
		ulPosLo = ulPos512ByteBlock & 0xff;

		if ( m_bUseExternalFlash )
		{
			ulPosHi = POS_BANK_EXTERNALFLASH;
			ulPosMid = (ulPos512ByteBlock & 0xff00) >> 8;
		}
		else if ( m_bSpecialEdition )
		{
			ulPosHi	 = ulPos512ByteBlock / 32768;
			ulPosMid = (ulPos512ByteBlock & 0x7f00) >> 8;
		}
		else
		{
			ulPosHi = ulPos512ByteBlock / 16384;
			ulPosMid = (ulPos512ByteBlock & 0x3f00) >> 8;
		}

		// issue upload 512 byte block command
		COMMANDOUT( B_10101011, B_00001100, B_00000100 );
		COMMANDOUT( ulPosHi, B_00000000, B_00000100 );
		COMMANDOUT( B_10100001, B_00001100, B_00000100 );
		COMMANDOUT( B_10000000, B_00000000, B_00000100 );
		COMMANDOUT( B_10100010, B_00001100, B_00000100 );
		COMMANDOUT( B_00000000, B_00000000, B_00000100 );
		COMMANDOUT( ulPosLo, B_00000000, B_00000100 );
		COMMANDOUT( ulPosMid, B_00000000, B_00000100 );

		COMMANDOUT( B_10100011, B_00001100, B_00000100 );

		// create checksum of 512 byte block
		USHORT usChecksum = 0;
		USHORT* paus = (USHORT*)pauc;
		for( iB=0; iB<(512/sizeof(short)); ++iB, ++paus )
			usChecksum -= *paus;

		// clock out data
		#if defined(_WINNT)
		{
			DWORD dwSizeReturn;
			DeviceIoControl( m_hDriver, RIOIO_IOCTL_WRITEBLOCK, pauc, 512,
				NULL, 0, &dwSizeReturn, NULL );
		}
		#else
			UCHAR* pauc2 = pauc;
			for( iB=0; iB<512; ++iB, ++pauc2 )
			{
				OUTPORT( m_iPortData, (*pauc2) );

				if ( !(iB & 1) )
					OUTPORT( m_iPortControl, B_00000000 );
				else
					OUTPORT( m_iPortControl, B_00000100 );

				IODELAY( 1 );
			}
		#endif

		// prepare end of block
		CEnd512ByteBlock cEnd512ByteBlock;
		memset( &cEnd512ByteBlock, 0, sizeof(CEnd512ByteBlock) );
		cEnd512ByteBlock.m_usChecksum = usChecksum;
		if ( uiPos32KBlockNext == 0xffff )
		{
			cEnd512ByteBlock.m_ulPos512ByteBlockNextMult256 = 0xffffffff;
			cEnd512ByteBlock.m_ucPos8192KBlockNext1 = 0xff;
			cEnd512ByteBlock.m_ucPos8192KBlockNext2 = 0xff;
		}
		else
		{
			cEnd512ByteBlock.m_ulPos512ByteBlockNextMult256 = ((ULONG)uiPos32KBlockNext * 64) * 256;

			if ( !m_bSpecialEdition )
			{
				cEnd512ByteBlock.m_ucPos8192KBlockNext1 = uiPos32KBlockNext / 256;
				cEnd512ByteBlock.m_ucPos8192KBlockNext2 = uiPos32KBlockNext / 256;
			}
			else
			{
				cEnd512ByteBlock.m_ucPos8192KBlockNext1 = uiPos32KBlockNext / 512;
				cEnd512ByteBlock.m_ucPos8192KBlockNext2 = uiPos32KBlockNext / 512;
			}

			if ( m_bUseExternalFlash )
			{
				cEnd512ByteBlock.m_ulPos512ByteBlockNextMult256	+= 0x01000000;
				cEnd512ByteBlock.m_ucPos8192KBlockNext1 = 0;
				cEnd512ByteBlock.m_ucPos8192KBlockNext2 = 0;
			}
		}

		if ( uiPos32KBlockPrev == 0xffff )
		{
			cEnd512ByteBlock.m_ulPos512ByteBlockPrevMult256 = 0xffffffff;
			cEnd512ByteBlock.m_ucPos8192KBlockPrev1 = 0xff;
			cEnd512ByteBlock.m_ucPos8192KBlockPrev2 = 0xff;
			cEnd512ByteBlock.m_usPos32KBlockPrevMult256 = 0xffff;
		}
		else
		{
			cEnd512ByteBlock.m_ulPos512ByteBlockPrevMult256 = ((ULONG)uiPos32KBlockPrev * 64) * 256;

			if ( !m_bSpecialEdition )
			{
				cEnd512ByteBlock.m_ucPos8192KBlockPrev1 = uiPos32KBlockPrev / 256;
				cEnd512ByteBlock.m_ucPos8192KBlockPrev2 = uiPos32KBlockPrev / 256;
			}
			else
			{
				cEnd512ByteBlock.m_ucPos8192KBlockPrev1 = uiPos32KBlockPrev / 512;
				cEnd512ByteBlock.m_ucPos8192KBlockPrev2 = uiPos32KBlockPrev / 512;
			}

			if ( m_bUseExternalFlash )
			{
				cEnd512ByteBlock.m_ulPos512ByteBlockPrevMult256	+= 0x01000000;
				cEnd512ByteBlock.m_ucPos8192KBlockPrev1 = 0;
				cEnd512ByteBlock.m_ucPos8192KBlockPrev2 = 0;
			}

			cEnd512ByteBlock.m_usPos32KBlockPrevMult256 = uiPos32KBlockPrev * 256;
		}

		// output end of block
		#if defined(_WINNT)
		{
			DWORD dwSizeReturn;
			DeviceIoControl( m_hDriver, RIOIO_IOCTL_WRITEBLOCK, &cEnd512ByteBlock,
				sizeof(CEnd512ByteBlock), NULL, 0, &dwSizeReturn, NULL );
		}
		#else
			pauc2 = (UCHAR*)&cEnd512ByteBlock;
			for( iB=0; iB<sizeof(CEnd512ByteBlock); ++iB, ++pauc2 )
			{
				OUTPORT( m_iPortData, (*pauc2) );

				if ( !(iB & 1) )
					OUTPORT( m_iPortControl, B_00000000 );
				else
					OUTPORT( m_iPortControl, B_00000100 );

				IODELAY( 1 );
			}
		#endif

		// end of tx
		COMMANDOUT( B_10100001, B_00001100, B_00000100 );
		COMMANDOUT( B_00010000, B_00000000, B_00000100 );

		WAITACK();

		COMMANDOUT( B_10100001, B_00001100, B_00000100 );
		COMMANDOUT( B_01110000, B_00000000, B_00000100 );
		COMMANDOUT( B_10100000, B_00001100, B_00000100 );

		OUTPORT( m_iPortControl, B_00000000 );
		IODELAY( m_lTimeIODelayTx );

		OUTPORT( m_iPortControl, B_00000100 );
		IODELAY( m_lTimeIODelayTx );
	}

	return TRUE;
}

// tx 32K block
BOOL CRio::Tx32KBlock( void* pv, UINT uiPos32KBlock, UINT uiPos32KBlockPrev,
	UINT uiPos32KBlockNext )
{
	int iRetry = 0;
	while( iRetry < MAX_RETRY )
	{
		if ( Tx32KBlockRetry(pv, uiPos32KBlock, uiPos32KBlockPrev, uiPos32KBlockNext) )
			return TRUE;

		DELAY( CLOCK_SECOND );
		++iRetry;
	}

	LogError( CRIO_ERROR_TXBLOCKRETRY, "too many retries for tx block" );

	return FALSE;
}

// rx 32K block retry
BOOL CRio::Rx32KBlockRetry( void* pv, UINT uiPos32KBlock )
{
	ULONG ulPos512ByteBlock;
	ULONG ulPosHi;
	ULONG ulPosMid;
	ULONG ulPosLo;
	int iA, iB;

	// io intro
	if ( !IOIntro() )
		return FALSE;

	// get 32K in 512 byte chunks
	UCHAR* pauc = (UCHAR*)pv;
	for( iA=0; iA<(32768/512); ++iA, pauc+=512 )
	{
		ulPos512ByteBlock = ((ULONG)uiPos32KBlock * 32768 + (ULONG)iA * 512) / 512;
		ulPosLo = ulPos512ByteBlock & 0xff;

		if ( m_bUseExternalFlash )
		{
			ulPosHi = POS_BANK_EXTERNALFLASH;
			ulPosMid = (ulPos512ByteBlock & 0xff00) >> 8;
		}
		else if ( m_bSpecialEdition )
		{
			ulPosHi	 = ulPos512ByteBlock / 32768;
			ulPosMid = (ulPos512ByteBlock & 0x7f00) >> 8;
		}
		else
		{
			ulPosHi = ulPos512ByteBlock / 16384;
			ulPosMid = (ulPos512ByteBlock & 0x3f00) >> 8;
		}

		// issue download 512 byte block command
		COMMANDOUT( B_10101011, B_00001100, B_00000100 );
		COMMANDOUT( ulPosHi, B_00000000, B_00000100 );
		COMMANDOUT( B_10100001, B_00001100, B_00000100 );
		COMMANDOUT( B_00000000, B_00000000, B_00000100 );
		COMMANDOUT( B_10100010, B_00001100, B_00000100 );
		COMMANDOUT( B_00000000, B_00000000, B_00000100 );
		COMMANDOUT( ulPosLo, B_00000000, B_00000100 );
		COMMANDOUT( ulPosMid, B_00000000, B_00000100 );

		WAITACK();

		COMMANDOUT( B_10100000, B_00001100, B_00000100 );

		// clock in data
		#if defined(_WINNT)
		{
			DWORD dwSizeReturn;
			DeviceIoControl( m_hDriver, RIOIO_IOCTL_READBLOCK, pauc, 512,
				pauc, 512, &dwSizeReturn, NULL );
		}
		#else
			for( iB=0; iB<512; ++iB )
				*(pauc+iB) = GetDataByte();
		#endif

		// clock in 16 bytes which are ignored
		for( iB=0; iB<16; ++iB )
			GetDataByte();

		// delay
		WAITACK();
	}

	return TRUE;
}

// rx 32K block
BOOL CRio::Rx32KBlock( void* pv, UINT uiPos32KBlock )
{
	int iRetry = 0;
	while( iRetry < MAX_RETRY )
	{
		if ( Rx32KBlockRetry(pv, uiPos32KBlock) )
			return TRUE;

		DELAY( CLOCK_SECOND );
		++iRetry;
	}

	LogError( CRIO_ERROR_RXBLOCKRETRY, "too many retries for rx block" );

	return FALSE;
}

// mark bad blocks
BOOL CRio::MarkBadBlocks( BOOL (*pfProgress)(int iPos, int iCount, void* cookie), void* cookie)
{
	// create temp block
	UCHAR* paucBlock;
	NEWBLOCK( paucBlock );

	// block count
	USHORT usPos32KBlockEnd;
	if ( m_bUseExternalFlash )
		usPos32KBlockEnd = m_uiCount32KBlockAvailableExternal;
	else
		usPos32KBlockEnd = m_bSpecialEdition ? CRIO_COUNT_32KBLOCKIN64M : CRIO_COUNT_32KBLOCKIN32M;

	// assume directory block is ok
	m_cDirBlock.m_auc32KBlockUsed[ 0 ] = CRIO_ID_32KBLOCK_FREE;

	// process all blocks (except directory block)
	int iCount32KBlockBad = 0;
	USHORT usPos32KBlock;
	for( usPos32KBlock=1; usPos32KBlock<usPos32KBlockEnd; ++usPos32KBlock )
	{
		UINT uiB;

		// progress callback
		if ( pfProgress )
		{
			if ( !pfProgress(usPos32KBlock, usPos32KBlockEnd, cookie) )
			{
				LogError( CRIO_ERROR_INTERRUPTED, "operation interrupted" );
				break;
			}
		}

		// default to bad block
		UCHAR ucState = CRIO_ID_32KBLOCK_BAD;

		// create and send test block 1
		memset( paucBlock, B_10101010, CRIO_SIZE_32KBLOCK );
		if ( !Tx32KBlock(paucBlock, usPos32KBlock, 0, 0) )
			break;
		// get test block 1
		if ( !Rx32KBlock(paucBlock, usPos32KBlock) )
			break;
		// check test block 1
		for( uiB=0; uiB<CRIO_SIZE_32KBLOCK; ++uiB )
		{
			if ( *(paucBlock+uiB) != B_10101010 )
				break;
		}
		// if ok
		if ( uiB == CRIO_SIZE_32KBLOCK )
		{
			// create and send test block 2
			memset( paucBlock, B_01010101, CRIO_SIZE_32KBLOCK);
			if ( !Tx32KBlock(paucBlock, usPos32KBlock, 0, 0) )
				break;
			// get test block 2
			if ( !Rx32KBlock(paucBlock, usPos32KBlock) )
				break;
			// check test block 2
			for( uiB=0; uiB<CRIO_SIZE_32KBLOCK; ++uiB )
			{
				if ( *(paucBlock+uiB) != B_01010101 )
					break;
			}
			// if ok, block ok
			if ( uiB == CRIO_SIZE_32KBLOCK )
				ucState = CRIO_ID_32KBLOCK_FREE;
		}

		// store block state
		m_cDirBlock.m_auc32KBlockUsed[ usPos32KBlock ] = ucState;

		// adjust bad block count
		if ( ucState == CRIO_ID_32KBLOCK_BAD )
			++iCount32KBlockBad;
	}
	// if tx,rx block error or interrupted
	if ( usPos32KBlock < usPos32KBlockEnd )
		return FALSE;

	// store blocks used and bad block count
	m_cDirBlock.m_cDirHeader.m_usCount32KBlockUsed = iCount32KBlockBad;
	m_cDirBlock.m_cDirHeader.m_usCount32KBlockBad = iCount32KBlockBad;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// operations
void CRio::UseExternalFlash( BOOL bUseExternalFlash )
{
	m_bUseExternalFlash = bUseExternalFlash;
}

BOOL CRio::CheckPresent( void )
{
	// io intro
	if ( !IOIntro() )
		return FALSE;

	// bank to check (bank 0-3 are internal, bank 4 external)
	int iPos = m_bUseExternalFlash ? POS_BANK_EXTERNALFLASH : 0;

	// issue read id command
	COMMANDOUT( 0xab, B_00001100, B_00000100 );
	COMMANDOUT( iPos, B_00000000, B_00000100 );
	COMMANDOUT( 0xa1, B_00001100, B_00000100 );
	COMMANDOUT( 0x90, B_00000000, B_00000100 );
	COMMANDOUT( 0xa2, B_00001100, B_00000100 );
	COMMANDOUT( 0x00, B_00000000, B_00000100 );
	COMMANDOUT( 0xa0, B_00001100, B_00000100 );
	UCHAR ucManufacturer = GetDataByte();
	UCHAR ucDevice = GetDataByte();

	// check manufacturer code
	switch( ucManufacturer )
	{
		case 0xa0:
		case 0xaa:
		case 0xff:
		case 0xdd:
		case 0x00:
			LogError( CRIO_ERROR_DEVICENOTFOUND, "device not found" );
			return FALSE;
	}

	// determine bank size in 512 byte blocks
	ULONG ulCount512ByteBlock;
	switch( ucDevice )
	{
		case 0xea:
		case 0x64:	ulCount512ByteBlock = 4096;		break;
		case 0xe3:
		case 0xe5:	ulCount512ByteBlock = 8192;		break;
		case 0xe6:	ulCount512ByteBlock = 16384;	break;
		case 0x73:	ulCount512ByteBlock = 32768;	break;
		case 0x75:	ulCount512ByteBlock = 65536;	break;
		default:
			LogError( CRIO_ERROR_DEVICENOTFOUND, "device not found" );
			return FALSE;
	};

	// if internal
	if ( !m_bUseExternalFlash )
	{
		// if special edition found (each bank of 4 is 512 * 32768)
		if ( ulCount512ByteBlock == 32768 )
			m_bSpecialEdition = TRUE;
	}
	// else external
	else
	{
		// convert page count to 32K block count
		m_uiCount32KBlockAvailableExternal = (ulCount512ByteBlock * 512) / CRIO_SIZE_32KBLOCK;
	}

	return TRUE;
}

CDirEntry* CRio::FindFile( const char* pszFile, CFindFile* pFindFile)
{
	// search directory entries for matching filename

    int iCountEntry = m_cDirBlock.m_cDirHeader.m_usCountEntry;
    CDirEntry* pDirEntry = m_cDirBlock.m_acDirEntry;
    int iA = 0;

    if(pFindFile)
    {
        if(pFindFile->m_iCount < iCountEntry)
        {
            iA = ++pFindFile->m_iCount;
            pDirEntry = ++pFindFile->m_iEntry;
        }
    }
	
	for(; iA<iCountEntry; ++iA, ++pDirEntry )
	{
		if ( !strcmp(pszFile, pDirEntry->m_szName) )
        {
            if(pFindFile)
            {
                pFindFile->m_iCount = iA;
                pFindFile->m_iEntry = pDirEntry;
            }

			return pDirEntry;
        }
	}

	return NULL;
}

BOOL CRio::Initialize( BOOL bMarkBadBlock, BOOL (*pfProgress)(int iPos, int iCount, void* cookie), void* cookie)
{
	CDirHeader& cDirHeader = m_cDirBlock.m_cDirHeader;

	// init directory header
	memset( &m_cDirBlock.m_cDirHeader, 0, sizeof(m_cDirBlock.m_cDirHeader) );

	//	set version (so compatible with Rio Manager v1.01)
	cDirHeader.m_usVersion = 0x0100;
	// init directory entries
	memset( &m_cDirBlock.m_acDirEntry, 0, sizeof(m_cDirBlock.m_acDirEntry) );
	// init block used flags
	memset( &m_cDirBlock.m_auc32KBlockUsed, CRIO_ID_32KBLOCK_FREE, sizeof(m_cDirBlock.m_auc32KBlockUsed) );
	// init FAT
	memset( &m_cDirBlock.m_ausFAT, 0, sizeof(m_cDirBlock.m_ausFAT) );

	// if mark bad block request
	if ( bMarkBadBlock )
	{
		if ( !MarkBadBlocks(pfProgress, cookie) )
			return FALSE;
	}

	// available blocks
	if ( m_bUseExternalFlash )
		cDirHeader.m_usCount32KBlockAvailable = m_uiCount32KBlockAvailableExternal;
	else
		cDirHeader.m_usCount32KBlockAvailable = m_bSpecialEdition ? CRIO_COUNT_32KBLOCKIN64M : CRIO_COUNT_32KBLOCKIN32M;

	// blocks remaining (taking into account bad blocks)
	cDirHeader.m_usCount32KBlockRemaining =
		cDirHeader.m_usCount32KBlockAvailable -
		cDirHeader.m_usCount32KBlockBad;

	return TRUE;
}

BOOL CRio::RemoveFile( const char* pszFile )
{
	// get directory entry for file
	CDirEntry* pDirEntry = FindFile( pszFile );
	if ( !pDirEntry )
	{
		LogError( CRIO_ERROR_FILENOTFOUND, "file '%s' not present on device", pszFile );
		return FALSE;
	}

	// free FAT and blocks used
	USHORT usPos32KBlock = pDirEntry->m_usPos32KBlock;
	while( usPos32KBlock )
	{
		m_cDirBlock.m_auc32KBlockUsed[ usPos32KBlock ] = CRIO_ID_32KBLOCK_FREE;
		USHORT usTemp = m_cDirBlock.m_ausFAT[ usPos32KBlock ];
		m_cDirBlock.m_ausFAT[ usPos32KBlock ] = 0;
		usPos32KBlock = usTemp;
	}

	// adjust directory header
	CDirHeader& cDirHeader = m_cDirBlock.m_cDirHeader;
	--cDirHeader.m_usCountEntry;
	cDirHeader.m_usCount32KBlockUsed -= pDirEntry->m_usCount32KBlock;
	cDirHeader.m_usCount32KBlockRemaining += pDirEntry->m_usCount32KBlock;

	// clear directory entry
	memset( pDirEntry, 0, sizeof(CDirEntry) );

	// shuffle directory entries
	int iPosEntry = pDirEntry - m_cDirBlock.m_acDirEntry;
	int iCount = (int)cDirHeader.m_usCountEntry - iPosEntry;
	for( int iA=0; iA<iCount; ++iA )
	{
		memcpy(
			&m_cDirBlock.m_acDirEntry[ iPosEntry+iA ],
			&m_cDirBlock.m_acDirEntry[ iPosEntry+iA+1 ],
			sizeof(CDirEntry)
		);
	}

	return TRUE;
}

BOOL CRio::RemoveAllFiles( void )
{
	CDirHeader& cDirHeader = m_cDirBlock.m_cDirHeader;

	// init entries
	cDirHeader.m_usCountEntry = 0;

	// init block used flags
	UCHAR* pauc = m_cDirBlock.m_auc32KBlockUsed;
	for( int iA=0; iA<CRIO_MAX_32KBLOCK; ++iA, ++pauc )
	{
		if ( *pauc != CRIO_ID_32KBLOCK_BAD )
			*pauc = CRIO_ID_32KBLOCK_FREE;
	}

	// init FAT
	memset( &m_cDirBlock.m_ausFAT, 0, sizeof(m_cDirBlock.m_ausFAT) );

	// available blocks
	if ( m_bUseExternalFlash )
		cDirHeader.m_usCount32KBlockAvailable = m_uiCount32KBlockAvailableExternal;
	else
		cDirHeader.m_usCount32KBlockAvailable = m_bSpecialEdition ? CRIO_COUNT_32KBLOCKIN64M : CRIO_COUNT_32KBLOCKIN32M;

	// used
	cDirHeader.m_usCount32KBlockUsed = cDirHeader.m_usCount32KBlockBad;

	// blocks remaining
	cDirHeader.m_usCount32KBlockRemaining =
		cDirHeader.m_usCount32KBlockAvailable -
		cDirHeader.m_usCount32KBlockUsed;

	return TRUE;
}

BOOL CRio::SetFileOrder( UINT* pauiPosOrder, UINT uiCount )
{
	// create copy of directory entries
	CDirEntry* paDirEntry = m_cDirBlock.m_acDirEntry;
	CDirEntry* paDirEntryNew = new CDirEntry[ CRIO_MAX_DIRENTRY ];
	if ( !paDirEntryNew )
	{
		LogError( CRIO_ERROR_ALLOC, "new failed" );
		return FALSE;
	}
	memcpy( paDirEntryNew, paDirEntry, CRIO_MAX_DIRENTRY * sizeof(CDirEntry) );

	// current entry count
	UINT uiCountEntry = m_cDirBlock.m_cDirHeader.m_usCountEntry;

	// move entries
	for( UINT uiPosNew=0; uiPosNew<uiCount; ++uiPosNew )
	{
		UINT uiPosOld = pauiPosOrder[ uiPosNew ];

		if ( uiPosOld >= uiCountEntry || uiPosNew >= uiCountEntry )
		{
			LogError( CRIO_ERROR_INVALIDFILEPOSITION, "invalid file position" );
			DELETEARRAY paDirEntryNew;
			return FALSE;
		}

		memcpy( &paDirEntryNew[uiPosNew], &paDirEntry[uiPosOld], sizeof(CDirEntry) );
	}

	// update current directory entries
	memcpy( paDirEntry, paDirEntryNew, CRIO_MAX_DIRENTRY * sizeof(CDirEntry) );

	DELETEARRAY paDirEntryNew;

	return TRUE;
}

BOOL CRio::TxDirectory( void )
{
	// create zero initialized temp block
	UCHAR* paucBlock;
	ZERONEWBLOCK( paucBlock );

	// directory header
	CDirHeader& cDirHeader = m_cDirBlock.m_cDirHeader;
	// update directory header time
	cDirHeader.m_lTimeLastUpdate = time( NULL );
	// create checksums (note: second checksum needs to be calculated first as
	//	this makes up part of the first checksum)
	cDirHeader.m_usChecksum2 = CalculateChecksum2();
	cDirHeader.m_usChecksum1 = CalculateChecksum1();

	// copy directory block to temp block
	memcpy( paucBlock, &m_cDirBlock, sizeof(m_cDirBlock) );

	// send block
	BOOL bResult = Tx32KBlock( paucBlock, 0, 0, 0 );
	if ( bResult )
	{
		// IO outro
		IOOutro();
	}

	// release temp block
	DELETEBLOCK( paucBlock );

	return bResult;
}

BOOL CRio::RxDirectory( void )
{
	// create temp block
	UCHAR* paucBlock;
	NEWBLOCK( paucBlock );

	// get block
	BOOL bResult = Rx32KBlock( paucBlock, 0 );
	if ( bResult )
	{
		// io outro
		IOOutro();

		// store directory
		memcpy( &m_cDirBlock, paucBlock, sizeof(m_cDirBlock) );

		// validate checksums
		USHORT usChecksum1 = (USHORT)CalculateChecksum1();
		USHORT usChecksum2 = (USHORT)CalculateChecksum2();
		if (
			usChecksum1 != m_cDirBlock.m_cDirHeader.m_usChecksum1 ||
			usChecksum2 != m_cDirBlock.m_cDirHeader.m_usChecksum2
		)
		{
			LogError( CRIO_ERROR_CORRUPT, "invalid directory checksum, initialization recommended" );
			bResult = FALSE;
		}
	}

	// release temp block
	DELETEBLOCK( paucBlock );

	return bResult;
}

BOOL CRio::TxFile( const char* pszPathFile, BOOL (*pfProgress)(int iPos, int iCount, void* cookie), void* cookie)
{
	// directory header
	CDirHeader& cDirHeader = m_cDirBlock.m_cDirHeader;

	// if no enough room in directory for entry
	if ( cDirHeader.m_usCountEntry >= CRIO_MAX_DIRENTRY )
	{
		LogError( CRIO_ERROR_MAXDIRENTRY, "max number of directory entries (%d) already in use", CRIO_MAX_DIRENTRY );
		return FALSE;
	}

	// open file for read
	FILE* fpFile = fopen( pszPathFile, "rb" );
	if ( !fpFile )
	{
		LogError( CRIO_ERROR_OPEN, "unable to open '%s' for read", pszPathFile );
		return FALSE;
	}

	// get fileinfo
	struct stat sStat;
	if ( stat(pszPathFile, &sStat) )
	{
		LogError( CRIO_ERROR_STAT, "stat() failed for '%s'", pszPathFile );
		fclose( fpFile );
		return FALSE;
	}

	// determine if enough room to store file
	USHORT usCount32KBlock = (sStat.st_size/CRIO_SIZE_32KBLOCK) +
		(sStat.st_size % CRIO_SIZE_32KBLOCK ? 1 : 0);
	if ( usCount32KBlock > cDirHeader.m_usCount32KBlockRemaining )
	{
		LogError( CRIO_ERROR_MEMORY, "not enough memory on device to upload '%s'", pszPathFile );
		fclose( fpFile );
		return FALSE;
	}

	// get first four bytes of file which determine mp3 properties
	UCHAR aucProperty[ 4 ];
	if ( fread(aucProperty, sizeof(aucProperty), 1, fpFile) < 1 )
	{
		LogError( CRIO_ERROR_READ, "error reading from file '%s'", pszPathFile );
		fclose( fpFile );
		return FALSE;
	}
	rewind( fpFile );

	// point to directory entry that will be used
	CDirEntry& cDirEntry = m_cDirBlock.m_acDirEntry[ cDirHeader.m_usCountEntry ];

	// update directory header
	if ( !cDirHeader.m_usCountEntry )
	{
		// adjust for dir block
		++cDirHeader.m_usCount32KBlockUsed;
		--cDirHeader.m_usCount32KBlockRemaining;

		// mark first block as used by directory
		m_cDirBlock.m_auc32KBlockUsed[ 0 ] = CRIO_ID_32KBLOCK_USED;

		// first entry in FAT used by directory
		m_cDirBlock.m_ausFAT[ 0 ] = 0;
	}
	++cDirHeader.m_usCountEntry;
	cDirHeader.m_usCount32KBlockUsed += usCount32KBlock;
	cDirHeader.m_usCount32KBlockRemaining -= usCount32KBlock;

	// find first free 32K block
	USHORT usPos32KBlockFree = FindFirstFree32KBlock();
	if ( usPos32KBlockFree == 0xffff )
	{
		LogError( CRIO_ERROR_CORRUPT, "no free 32K blocks, initialization recommended" );
		fclose( fpFile );
		return FALSE;
	}

	// update directory entry
	memset( &cDirEntry, 0, sizeof(CDirEntry) );
	cDirEntry.m_usPos32KBlock = usPos32KBlockFree;
	cDirEntry.m_usCount32KBlock = usCount32KBlock;
	cDirEntry.m_usSize32KMod = sStat.st_size % CRIO_SIZE_32KBLOCK;
	cDirEntry.m_lSize = sStat.st_size;
	cDirEntry.m_lTimeUpload = time( NULL );
	cDirEntry.m_aucProperty[ 0 ] = aucProperty[ 3 ];
	cDirEntry.m_aucProperty[ 1 ] = aucProperty[ 2 ];
	cDirEntry.m_aucProperty[ 2 ] = aucProperty[ 1 ];
	cDirEntry.m_aucProperty[ 3 ] = aucProperty[ 0 ];
	strncpy( cDirEntry.m_szName, GetFile(pszPathFile), sizeof(cDirEntry.m_szName) );

	// create zero initialized temp block
	UCHAR* paucBlock;
	ZERONEWBLOCK( paucBlock );

	// default return
	BOOL bResult = FALSE;

	// process all 32K blocks of file
	USHORT usPos32KBlockCurrent = cDirEntry.m_usPos32KBlock;
	USHORT usPos32KBlockPrev = 0xffff;
	USHORT usPos32KBlockEnd = cDirEntry.m_usCount32KBlock;
	USHORT usPos32KBlockNext;
	USHORT usPos32KBlock;
	for( usPos32KBlock=0; usPos32KBlock<usPos32KBlockEnd; ++usPos32KBlock )
	{
		// progress callback
		if ( pfProgress )
		{
			if ( !pfProgress(usPos32KBlock, usPos32KBlockEnd, cookie) )
			{
				LogError( CRIO_ERROR_INTERRUPTED, "operation interrupted" );
				break;
			}
		}

		// mark as block used
		m_cDirBlock.m_auc32KBlockUsed[ usPos32KBlockCurrent ] = CRIO_ID_32KBLOCK_USED;

		// get next block and update FAT
		if ( usPos32KBlock == (usPos32KBlockEnd-1) )
		{
			usPos32KBlockNext = 0xffff;
			m_cDirBlock.m_ausFAT[ usPos32KBlockCurrent ] = 0;
		}
		else
		{
			usPos32KBlockNext = FindFirstFree32KBlock();
			if ( usPos32KBlockNext == 0xffff )
			{
				LogError( CRIO_ERROR_CORRUPT, "no free 32K blocks, initialization recommended" );
				break;
			}
			m_cDirBlock.m_ausFAT[ usPos32KBlockCurrent ] = usPos32KBlockNext;
		}

		// determine read size
		UINT uiSize;
		if ( usPos32KBlock == (usPos32KBlockEnd-1) && cDirEntry.m_usSize32KMod )
		{
			uiSize = cDirEntry.m_usSize32KMod;
			memset( paucBlock+uiSize, 0, CRIO_SIZE_32KBLOCK-uiSize );
		}
		else
			uiSize = CRIO_SIZE_32KBLOCK;

		// read block
		if ( fread(paucBlock, uiSize, 1, fpFile) < 1 )
		{
			LogError( CRIO_ERROR_READ, "error reading from file '%s'", pszPathFile );
			break;
		}

		// tx block
		if ( !Tx32KBlock(paucBlock, usPos32KBlockCurrent, usPos32KBlockPrev,
			usPos32KBlockNext) )
			break;

		// update current and prev block markers
		usPos32KBlockPrev = usPos32KBlockCurrent;
		usPos32KBlockCurrent = usPos32KBlockNext;
	}

	// if transfer ok
	if ( usPos32KBlock == usPos32KBlockEnd )
	{
		// flag as ok
		bResult = TRUE;
		// IO outro
		IOOutro();
	}

	// release temp block
	DELETEBLOCK( paucBlock );

	// close file
	fclose( fpFile );

	return bResult;
}

BOOL CRio::RxFile( const char* pszPathFile, BOOL (*pfProgress)(int iPos, int iCount, void* cookie), void* cookie)
{
	// get directory entry for file
	const char* pszFile = GetFile( pszPathFile );
	CDirEntry* pDirEntry = FindFile( pszFile );
	if ( !pDirEntry )
	{
		LogError( CRIO_ERROR_FILENOTFOUND, "file '%s' not present on device", pszFile );
		return FALSE;
	}

	// open file for write
	FILE* fpFile = fopen( pszPathFile, "wb" );
	if ( !fpFile )
	{
		LogError( CRIO_ERROR_OPEN, "unable to open '%s' for write", pszPathFile );
		return FALSE;
	}

	// create temp block
	UCHAR* paucBlock;
	NEWBLOCK( paucBlock );

	// default return
	BOOL bResult = FALSE;

	// get all blocks
	USHORT usPos32KBlockCurrent = pDirEntry->m_usPos32KBlock;
	USHORT usPos32KBlockEnd = pDirEntry->m_usCount32KBlock;
	USHORT usPos32KBlock;
	for( usPos32KBlock=0; usPos32KBlock<usPos32KBlockEnd; ++usPos32KBlock )
	{
		// progress callback
		if ( pfProgress )
		{
			if ( !pfProgress(usPos32KBlock, usPos32KBlockEnd, cookie) )
			{
				LogError( CRIO_ERROR_INTERRUPTED, "operation interrupted" );
				break;
			}
		}

		// rx block
		if ( !Rx32KBlock(paucBlock, usPos32KBlockCurrent) )
			break;

		// determine write size
		int iSizeWrite;
		if ( usPos32KBlock == (usPos32KBlockEnd-1) && pDirEntry->m_usSize32KMod )
			iSizeWrite = pDirEntry->m_usSize32KMod;
		else
			iSizeWrite = CRIO_SIZE_32KBLOCK;

		// save block
		if ( fwrite(paucBlock, iSizeWrite, 1, fpFile) < 1 )
		{
			LogError( CRIO_ERROR_WRITE, "error writing to file '%s'", pszPathFile );
			break;
		}

		// next block
		usPos32KBlockCurrent = m_cDirBlock.m_ausFAT[ usPos32KBlockCurrent ];
	}

	// if transfer ok
	if ( usPos32KBlock == usPos32KBlockEnd )
	{
		// flag as ok
		bResult = TRUE;
		// IO outro
		IOOutro();
	}

	// release temp block
	DELETEBLOCK( paucBlock );

	// close file
	fclose( fpFile );

	return bResult;
}

