///////////////////////////////////////////////////////////////////////////////
//
//	app.cpp
//
//	see RIO.CPP for version history
//
///////////////////////////////////////////////////////////////////////////////
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	<time.h>
#include	<sys/stat.h>

#include	"std.h"
#include	"rio.h"

// output
#define		INFOSTR						printf
#define		ERRORSTR					INFOSTR

// platform dependencies
#if defined(_WINNT)
	// MS VC++ v5.0 for WinNT v4
	#include	<stdlib.h>
	#define		SIZE_MAXPATH			_MAX_PATH
	#define		DELETEARRAY				delete[]

#elif defined(_WIN32)
	// MS VC++ v5.0 for Win9x
	#include	<stdlib.h>
	#define		SIZE_MAXPATH			_MAX_PATH
	#define		DELETEARRAY				delete[]

#elif defined(__linux__)
	// linux g++
	#include	<unistd.h>
	#include	<values.h>
	#include	<linux/limits.h>
	#if defined(PATH_MAX)
		#define		SIZE_MAXPATH		PATH_MAX
	#else
		#define		SIZE_MAXPATH		256
	#endif
	#define		DELETEARRAY				delete[]

#elif defined(__FreeBSD__)
	// FreeBSD g++
	#include	<unistd.h>
	#include	<sys/syslimits.h>
	#define		SIZE_MAXPATH			PATH_MAX
	#define		DELETEARRAY				delete[]

#elif defined(__bsdi__)
	// BSD/OS g++
	#include	<unistd.h>
	#include	<limits.h>
	#define		SIZE_MAXPATH			PATH_MAX
	#define		DELETEARRAY				delete[]

#elif defined(__OS2__)
	// OS/2 VisualAge C++ v. 3
	#include	<stdlib.h>

#elif defined(__TURBOC__)
	// turboc v1.01
	#include	<dir.h>
	#define		SIZE_MAXPATH			MAXPATH
	#define		DELETEARRAY				delete

#else
	// not supported
	#error ! ! compiler/platform not supported ! !
#endif

// default port base
#if defined(__alpha)
	#define		PORT_BASE_DEFAULT		0x3bc
#else
	#define		PORT_BASE_DEFAULT		0x378
#endif

// return code
#define		CLEANUP_RETURN( ret )		\
{ 										\
	if ( pRio ) 						\
	{ 									\
		delete pRio; 					\
		pRio = NULL; 					\
	} 									\
	if ( pszFilePlaylistTemp ) 			\
	{ 									\
		unlink( pszFilePlaylistTemp );	\
		pszFilePlaylistTemp = NULL; 	\
	} 									\
	return ret; 						\
}

///////////////////////////////////////////////////////////////////////////////
// return pointer to static string containing datetime
static char* TimeStr( long lValue )
{
	static char szBuf[ 64 ];
	struct tm* psDateTime;
	psDateTime = localtime( &lValue );

	if ( !psDateTime )
		strcpy( szBuf, "INVALID DATE/TIME" );
	else
	{
		sprintf(
			szBuf,
			"%02u/%02u/%02u %02u:%02u:%02u",
			(UINT)psDateTime->tm_mday % 100,
			(UINT)psDateTime->tm_mon+1 % 100,
			(UINT)psDateTime->tm_year % 100,
			(UINT)psDateTime->tm_hour % 100,
			(UINT)psDateTime->tm_min % 100,
			(UINT)psDateTime->tm_sec % 100
		);
	}

	return szBuf;
}

///////////////////////////////////////////////////////////////////////////////
// return maxpath
static int GetMaxPathSize( void )
{
	#if defined(__OS2__)
		ULONG ulSize;
        DosQuerySysInfo( QSV_MAX_PATH_LENGTH, QSV_MAX_PATH_LENGTH, &ulSize, sizeof(ulSize) );
        return ulSize;
	#endif

	return SIZE_MAXPATH;
}

///////////////////////////////////////////////////////////////////////////////
// return file size
static long GetFileSize( char* pszPathFile )
{
	long lReturn = 0;

	FILE* fpFile = fopen( pszPathFile, "rb" );
	if ( fpFile )
	{
		struct stat sStat;
		if ( !stat(pszPathFile, &sStat) )
			lReturn = sStat.st_size;

		fclose( fpFile );
	}

	return lReturn;
}

///////////////////////////////////////////////////////////////////////////////
// progress callback
static BOOL ProgressCallback( int iPos, int iCount )
{
	INFOSTR( "blocks %-5hd\r", iCount-iPos );
	fflush( stdout );
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// compute sample frequency
static UINT GetSampleFreq( UCHAR* paucProp )
{
	int iVersion, iFreq;
	UINT auiFreq[3][4] =
	{
		{ 44100, 48000, 32000, 0 },
		{ 22050, 24000, 16000, 0 },
		{ 11025, 8000, 8000, 0 }
	};

	switch( (paucProp[2] >> 3 & 0x3) )
	{
		case 3:		iVersion = 0;	break;
		case 2:		iVersion = 1;	break;
		case 0:		iVersion = 2;	break;
		default:	return 0;
	}
	iFreq = (paucProp[1] >> 2) & 0x03;
	return auiFreq[ iVersion ][ iFreq ];
}

///////////////////////////////////////////////////////////////////////////////
// compute bit rate
static int GetBitRate( UCHAR* paucProp )
{
	int iVersion, iLay, iPosBitrate;
	int aBitrate[3][3][15] =
	{
		{
			{ 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448 },
			{ 0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384 },
			{ 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 }
		},
		{
			{ 0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256 },
			{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160 },
			{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160 }
		},
		{
			{ 0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256 },
			{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160 },
			{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160 }
		}
	};

    iLay = 4 - ((paucProp[2] >> 1) & 0x03);
    iPosBitrate = (paucProp[1] >> 4) & 0x0f;

    switch( (paucProp[2]>>3 & 0x03) )
	{
		case 3:	iVersion = 0;	break;
		case 2:	iVersion = 1;	break;
		case 0:	iVersion = 2;	break;
		default: return 0;
	}

    return aBitrate[ iVersion][iLay - 1 ][ iPosBitrate ];
}

///////////////////////////////////////////////////////////////////////////////
// display directory
static void DisplayDirectory( CRio& cRio, BOOL bVerbose )
{
	CDirBlock& cDirBlock = cRio.GetDirectoryBlock();

	CDirHeader& cDirHeader = cDirBlock.m_cDirHeader;
	INFOSTR( "\n" );
	INFOSTR( "   entry count: %hu\n", cDirHeader.m_usCountEntry );
	INFOSTR( "  total memory: %ld KB\n", ((long)cDirHeader.m_usCount32KBlockAvailable * CRIO_SIZE_32KBLOCK) / 1024 );
	INFOSTR( "   used memory: %ld KB\n", ((long)cDirHeader.m_usCount32KBlockUsed * CRIO_SIZE_32KBLOCK) / 1024 );
	INFOSTR( " unused memory: %ld KB\n", ((long)cDirHeader.m_usCount32KBlockRemaining * CRIO_SIZE_32KBLOCK) / 1024 );
	INFOSTR( "flash ram type: %s\n", cRio.GetUseExternalFlashStatus() ? "external" : "internal" );
	if ( bVerbose )
	{
		INFOSTR( "bad 32K blocks: %hu\n", cDirHeader.m_usCount32KBlockBad );
		INFOSTR( "   last update: %s\n", TimeStr(cDirHeader.m_lTimeLastUpdate) );
		INFOSTR( "     checksum1: 0x%04hx\n", cDirHeader.m_usChecksum1 );
		INFOSTR( "     checksum2: 0x%04hx\n", cDirHeader.m_usChecksum2 );
		INFOSTR( "  host version: 0x%04hx\n", cDirHeader.m_usVersion );
	}

	UINT uiCountEntry = cDirHeader.m_usCountEntry;
	if ( uiCountEntry )
	{
		CDirEntry* pDirEntry = cDirBlock.m_acDirEntry;
		if ( uiCountEntry > CRIO_MAX_DIRENTRY )
			uiCountEntry = CRIO_MAX_DIRENTRY;

		// extended output
		if ( bVerbose )
		{
			INFOSTR( "\n" );
			INFOSTR( "No 32KPos 32KCount Mod32K Size     Upload Date/Time  Title\n" );
			INFOSTR( "-----------------------------------------------------------------------------\n" );

			for( UINT uiA=0; uiA<uiCountEntry; ++uiA, ++pDirEntry )
			{
				INFOSTR( "%02u 0x%04hx %-4hu     0x%04hx %-8ld %s %-24.24s\n",
					uiA+1,
					pDirEntry->m_usPos32KBlock,
					pDirEntry->m_usCount32KBlock,
					pDirEntry->m_usSize32KMod,
					pDirEntry->m_lSize,
					TimeStr(pDirEntry->m_lTimeUpload),
					pDirEntry->m_szName
				);
			}
		}
		// normal output
		else
		{
			INFOSTR( "\n" );
			INFOSTR( "No Size     K/s SFreq Upload Date/Time  Title\n" );
			INFOSTR( "----------------------------------------------------------------------------\n" );

			for( UINT uiA=0; uiA<uiCountEntry; ++uiA, ++pDirEntry )
			{
				INFOSTR( "%02u %-8ld %-3d %-5u %s %-36.36s\n",
					uiA+1,
					pDirEntry->m_lSize,
					GetBitRate(pDirEntry->m_aucProperty),
					GetSampleFreq(pDirEntry->m_aucProperty),
					TimeStr(pDirEntry->m_lTimeUpload),
					pDirEntry->m_szName
				);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// hex dump directory block
static void DumpDirectory( CRio& cRio, int iPos, int iSize )
{
	int iA, iB;

	CDirBlock& cDirBlock = cRio.GetDirectoryBlock();
	UCHAR* pucS = ((UCHAR*)&cDirBlock) + iPos;

	if ( iSize % 16 )
		iSize = ((iSize / 16)+1) * 16;

	for( iA=0; iA<(iSize/16); ++iA, pucS+=16 )
	{
		INFOSTR( "%08lx  ", iA*16 );

		for( iB=0; iB<16; ++iB )
		{
			int iC = *(pucS+iB);
			INFOSTR( "%02x ", iC );
			if ( (iB%8) == 7 )
				INFOSTR( " " );
		}

		for( iB=0; iB<16; ++iB )
		{
			int iC = *(pucS+iB);
			INFOSTR( "%c", iC > 32 ? iC : '.' );
			if ( (iB%8) == 7 )
				INFOSTR( " " );
		}

		INFOSTR( "\n" );

		if ( (iA%8) == 7 )
			INFOSTR( "\n" );
	}
}

///////////////////////////////////////////////////////////////////////////////
// process playlist file
static BOOL ProcessPlaylist( CRio& cRio, char* pszFile, BOOL bVerbose )
{
	// open playlist for read
	FILE* fpFile = fopen( pszFile, "r" );
	if ( !fpFile )
	{
		ERRORSTR( "unable to open '%s' for read\n", pszFile );
		return FALSE;
	}

	// create buffer
	int iSizeMaxPath = GetMaxPathSize();
	char* pszBuf = new char[ iSizeMaxPath ];

	// check if device can accommodate all files in playlist
	CDirBlock& cDirBlock = cRio.GetDirectoryBlock();
	CDirHeader& cDirHeader = cDirBlock.m_cDirHeader;
	long lSizeAvailable = (long)cDirHeader.m_usCount32KBlockAvailable * CRIO_SIZE_32KBLOCK;
	long lSizeCurrent = (long)cDirHeader.m_usCount32KBlockUsed * CRIO_SIZE_32KBLOCK;
	int iCountEntryCurrent = cDirHeader.m_usCountEntry;
	while( fgets(pszBuf, iSizeMaxPath, fpFile) )
	{
		// strip 'new line' char
		int iLength = strlen( pszBuf );
		pszBuf[ --iLength ] = 0;

		// if blank line or comment
		if ( !iLength || pszBuf[0] == '#' || pszBuf[0] == ';' )
			continue;

		// get file size
		long lSize = GetFileSize( pszBuf );
		if ( !lSize )
		{
			ERRORSTR( "unable to open '%s' for read\n", pszBuf );
			fclose( fpFile );
			DELETEARRAY pszBuf;
			return FALSE;
		}

		// check space
		lSizeCurrent += lSize;
		if ( lSizeCurrent > lSizeAvailable )
		{
			ERRORSTR( "entry '%s' exceed's memory capacity of device\n", pszBuf );
			fclose( fpFile );
			DELETEARRAY pszBuf;
			return FALSE;
		}

		// check enough entries
		++iCountEntryCurrent;
		if ( iCountEntryCurrent > CRIO_MAX_DIRENTRY )
		{
			ERRORSTR( "entry '%s' exceed's maximum directory entry count for device\n", pszBuf );
			fclose( fpFile );
			DELETEARRAY pszBuf;
			return FALSE;
		}
	}

	// rewind playlist
	rewind( fpFile );

	// process entries
	while( fgets(pszBuf, iSizeMaxPath, fpFile) )
	{
		// strip 'new line' char
		int iLength = strlen( pszBuf );
		pszBuf[ --iLength ] = 0;

		// if blank line or comment
		if ( !iLength || pszBuf[0] == '#' || pszBuf[0] == ';' )
			continue;

		// upload
		if ( bVerbose )
			INFOSTR( "uploading %s\n", pszBuf );
		if ( !cRio.TxFile(pszBuf, bVerbose ? ProgressCallback : NULL) )
		{
			ERRORSTR( "tx file failed, %s\n", cRio.GetErrorStr() );
			fclose( fpFile );
			DELETEARRAY pszBuf;
			return FALSE;
		}
	}

	// cleanup
	fclose( fpFile );
	DELETEARRAY pszBuf;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// change playlist order
static BOOL ChangePlaylistOrder( CRio& cRio, char* pszPlaylistOrder )
{
	UINT auiPosNew[ CRIO_MAX_DIRENTRY ];

	UINT uiPosNew = 0;
	char* pszEntry;
	do
	{
		pszEntry = strtok( uiPosNew ? NULL : pszPlaylistOrder, " " );
		if ( pszEntry )
			auiPosNew[ uiPosNew++ ] = atoi( pszEntry ) - 1;
	} while( pszEntry && (uiPosNew < CRIO_MAX_DIRENTRY) );

	if ( !cRio.SetFileOrder(auiPosNew, uiPosNew) )
	{
		ERRORSTR( "change in playlist order failed, %s\n", cRio.GetErrorStr() );
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// display help
static void Help( void )
{
	INFOSTR( "\nRio utility v%d.%02d - The Snowblind Alliance (c) 1999\n", CRIO_ID_VERSION/100, CRIO_ID_VERSION%100 );
	INFOSTR( "----------------------------------------------------\n" );
	INFOSTR( "command line switches available :-\n" );
	INFOSTR( "	-d  display directory\n" );
	INFOSTR( "	-iy initialize with check for bad blocks\n" );
	INFOSTR( "	-in initialize without check for bad blocks\n" );
	INFOSTR( "	-x  perform operations on external flash ram\n" );
	INFOSTR( "	-u  specify file(s) to upload\n" );
	INFOSTR( "	-g  specify file to download\n" );
	INFOSTR( "	-f  specify text based playlist file which contains files to be upload\n" );
	INFOSTR( "	-z  specify file to delete\n" );
	INFOSTR( "	-za delete all files\n" );
	INFOSTR( "	-o  specify new playlist order in quotes\n" );
	INFOSTR( "	-p  specify parallel port base IO address, default=0x%x\n", PORT_BASE_DEFAULT );
	INFOSTR( "	-v  enable verbose mode\n" );
	INFOSTR( "	-di specify initialization delay (default is %ld)\n", CRIO_TIME_IODELAY_INIT );
	INFOSTR( "	-dt specify tx delay (default is %ld)\n", CRIO_TIME_IODELAY_TX );
	INFOSTR( "	-dr specify rx delay (default is %ld)\n", CRIO_TIME_IODELAY_RX );
	INFOSTR( "\nexamples...\n" );
	INFOSTR( "	; display directory using parallel port at 0x278\n" );
	INFOSTR( "		rio -p 0x278 -d\n" );
	INFOSTR( "	; initialize (with bad block check) and upload files\n" );
	INFOSTR( "		rio -iy -u *.mp3\n" );
	INFOSTR( "	; delete existing files and upload playlist onto external flash ram\n" );
	INFOSTR( "		rio -za -f playlist.txt -x\n" );
	INFOSTR( "	; initialize, upload files in playlist and then display directory\n" );
	INFOSTR( "		rio -d -in -f playlist.txt\n" );
	INFOSTR( "	; download file then delete it and finally display directory\n" );
	INFOSTR( "		rio -d -g mp3Files/song.mp3 -z song.mp3\n" );
	INFOSTR( "	; reverse playlist order and display directory, also change rx iodelay\n" );
	INFOSTR( "		rio -o \"5 4 3 2 1\" -d -dr %ld\n", CRIO_TIME_IODELAY_RX*2 );
}

///////////////////////////////////////////////////////////////////////////////
int main( int iCountArg, char* paszArg[] )
{
	// default settings
	BOOL bDisplayDir = FALSE;
	BOOL bInit = FALSE;
	BOOL bInitMarkBadBlock = FALSE;
	BOOL bUseExternalFlash = FALSE;
	BOOL bDeleteAll = FALSE;
	BOOL bVerbose = FALSE;
	CRio* pRio = NULL;
	char* pszFileDelete = NULL;
	char* pszFileDownload = NULL;
	char* pszFilePlaylist = NULL;
	char* pszFilePlaylistTemp = NULL;
	char* pszPlaylistOrder = NULL;
	long lTimeIODelayInit = -1;
	long lTimeIODelayTx = -1;
	long lTimeIODelayRx = -1;
	int iPortBase = PORT_BASE_DEFAULT;
	int iPosDumpDirectory = 0;
	int iSizeDumpDirectory = 0;

	// process command line args
	if ( iCountArg < 2 )
	{
		Help();
		CLEANUP_RETURN( FALSE );
	}
	for( int iA=1; iA<iCountArg; ++iA )
	{
		// check for display directory request
		if ( !strcmp(paszArg[iA], "-d") )
			bDisplayDir = TRUE;
		// check for init with mark bad block request
		else if ( !strcmp(paszArg[iA], "-iy") )
		{
			bInit = TRUE;
			bInitMarkBadBlock = TRUE;
		}
		// check for init without mark bad block request
		else if ( !strcmp(paszArg[iA], "-in") )
			bInit = TRUE;
		// check for delete all file request
		else if ( !strcmp(paszArg[iA], "-za") )
			bDeleteAll = TRUE;
		// check for request to use external flash memory
		else if ( !strcmp(paszArg[iA], "-x") )
			bUseExternalFlash = TRUE;
		// check for verbose mode request
		else if ( !strcmp(paszArg[iA], "-v") )
			bVerbose = TRUE;
		// check for dump directory request
		else if ( !strcmp(paszArg[iA], "-h") )
		{
			if ( (iA+2) < iCountArg )
			{
				++iA;
				iPosDumpDirectory = atoi( paszArg[iA] );
				++iA;
				iSizeDumpDirectory = atoi( paszArg[iA] );
			}
		}
		// check for delete file request
		else if ( !strcmp(paszArg[iA], "-z") )
		{
			if ( (iA+1) < iCountArg )
			{
				++iA;
				pszFileDelete = paszArg[iA];
			}
		}
		// check for upload file request
		else if ( !strcmp(paszArg[iA], "-u") )
		{
			if ( (iA+1) < iCountArg )
			{
				++iA;

				// append all command line upload requests to temp playlist
				if ( !pszFilePlaylistTemp )
					pszFilePlaylistTemp = tmpnam( NULL );

				if ( pszFilePlaylistTemp )
				{
					FILE* fpFile = fopen( pszFilePlaylistTemp, "a" );
					if ( !fpFile )
					{
						ERRORSTR( "unable to open '%s' for amend\n", pszFilePlaylistTemp );
						CLEANUP_RETURN( FALSE );
					}
					while( iA < iCountArg )
					{
						fprintf( fpFile, "%s\n", paszArg[iA] );
						if ( (iA+1) < iCountArg )
						{
							if ( paszArg[iA+1][0] == '-' )
								break;
						}
						++iA;
					}
					fclose( fpFile );
				}
			}
		}
		// check for download file request
		else if ( !strcmp(paszArg[iA], "-g") )
		{
			if ( (iA+1) < iCountArg )
			{
				++iA;
				pszFileDownload = paszArg[iA];
			}
		}
		// check for playlist file request
		else if ( !strcmp(paszArg[iA], "-f") )
		{
			if ( (iA+1) < iCountArg )
			{
				++iA;
				pszFilePlaylist = paszArg[iA];
			}
		}
		// check for playlist order request
		else if ( !strcmp(paszArg[iA], "-o") )
		{
			if ( (iA+1) < iCountArg )
			{
				++iA;
				pszPlaylistOrder = paszArg[iA];
			}
		}
		// check for port base request
		else if ( !strcmp(paszArg[iA], "-p") )
		{
			if ( (iA+1) < iCountArg )
			{
				++iA;
				sscanf( paszArg[iA], "%x", &iPortBase );
			}
		}
		// check for io delay's
		else if ( !strcmp(paszArg[iA], "-di") )
		{
			if ( (iA+1) < iCountArg )
			{
				++iA;
				lTimeIODelayInit = atol( paszArg[iA] );
			}
		}
		else if ( !strcmp(paszArg[iA], "-dt") )
		{
			if ( (iA+1) < iCountArg )
			{
				++iA;
				lTimeIODelayTx = atol( paszArg[iA] );
			}
		}
		else if ( !strcmp(paszArg[iA], "-dr") )
		{
			if ( (iA+1) < iCountArg )
			{
				++iA;
				lTimeIODelayRx = atol( paszArg[iA] );
			}
		}
		// else help
		else
		{
			Help();
			CLEANUP_RETURN( FALSE );
		}
	}

	// setup
	pRio = new CRio;
	if ( !pRio )
	{
		ERRORSTR( "not enough memory\n" );
		CLEANUP_RETURN( FALSE );
	}
	CRio& cRio = *pRio;
	if ( !cRio.Set(iPortBase) )
	{
		ERRORSTR( "%s\n", cRio.GetErrorStr() );
		CLEANUP_RETURN( FALSE );
	}

	// if io delay's specified
	if ( lTimeIODelayInit >= 0 )
		cRio.SetIODelayInit( lTimeIODelayInit );
	if ( lTimeIODelayTx >= 0 )
		cRio.SetIODelayTx( lTimeIODelayTx );
	if ( lTimeIODelayRx >= 0 )
		cRio.SetIODelayRx( lTimeIODelayRx );

	// select internal or external flash memory
	if ( bVerbose )
		INFOSTR( "performing operations on %s flash ram\n", bUseExternalFlash ? "external" : "internal" );
	cRio.UseExternalFlash( bUseExternalFlash );

	// check device present
	if ( bVerbose )
		INFOSTR( "check device present\n" );
	if ( !cRio.CheckPresent() )
	{
		ERRORSTR( "%s\n", cRio.GetErrorStr() );
		CLEANUP_RETURN( FALSE );
	}

	// update directory flag
	BOOL bUpdateDirectory = FALSE;

	// get current directory
	if ( bVerbose )
		INFOSTR( "downloading directory\n" );
	if ( !cRio.RxDirectory() )
	{
		if ( cRio.GetErrorID() == CRIO_ERROR_CORRUPT )
			ERRORSTR( "%s\n", cRio.GetErrorStr() );
		else
		{
			ERRORSTR( "rx directory failed, %s\n", cRio.GetErrorStr() );
			CLEANUP_RETURN( FALSE );
		}
	}

	// if dump directory request
	if ( iSizeDumpDirectory )
		DumpDirectory( cRio, iPosDumpDirectory, iSizeDumpDirectory );

	// if init request
	if ( bInit )
	{
		if ( bVerbose )
			INFOSTR( "initializing with bad block check %s\n", bInitMarkBadBlock ? "enabled" : "disabled" );

		if ( !cRio.Initialize(bInitMarkBadBlock, bVerbose ? ProgressCallback : NULL) )
		{
			ERRORSTR( "Initialize failed, %s\n", cRio.GetErrorStr() );
			CLEANUP_RETURN( FALSE );
		}

		bUpdateDirectory = TRUE;
	}

	// if delete request
	if ( pszFileDelete )
	{
		if ( bVerbose )
			INFOSTR( "deleting %s\n", pszFileDelete );

		if ( !cRio.RemoveFile(pszFileDelete) )
		{
			ERRORSTR( "delete file failed, %s\n", cRio.GetErrorStr() );
			CLEANUP_RETURN( FALSE );
		}

		bUpdateDirectory = TRUE;
	}

	// if delete all request
	if ( bDeleteAll )
	{
		if ( bVerbose )
			INFOSTR( "deleting all files\n" );

		if ( !cRio.RemoveAllFiles() )
		{
			ERRORSTR( "delete all files failed, %s\n", cRio.GetErrorStr() );
			CLEANUP_RETURN( FALSE );
		}

		bUpdateDirectory = TRUE;
	}

	// if playlist request
	if ( pszFilePlaylist )
	{
		if ( !ProcessPlaylist(cRio, pszFilePlaylist, bVerbose) )
			CLEANUP_RETURN( FALSE );

		bUpdateDirectory = TRUE;
	}

	// if temp playlist request
	if ( pszFilePlaylistTemp )
	{
		if ( !ProcessPlaylist(cRio, pszFilePlaylistTemp, bVerbose) )
			CLEANUP_RETURN( FALSE );

		bUpdateDirectory = TRUE;
	}

	// if download request
	if ( pszFileDownload )
	{
		if ( bVerbose )
			INFOSTR( "downloading %s\n", pszFileDownload );

		if ( !cRio.RxFile(pszFileDownload, bVerbose ? ProgressCallback : NULL) )
		{
			ERRORSTR( "rx file failed, %s\n", cRio.GetErrorStr() );
			CLEANUP_RETURN( FALSE );
		}
	}

	// if playlist order request
	if ( pszPlaylistOrder )
	{
		if ( bVerbose )
			INFOSTR( "changing playlist order\n" );

		if ( !ChangePlaylistOrder(cRio, pszPlaylistOrder) )
			CLEANUP_RETURN( FALSE );

		bUpdateDirectory = TRUE;
	}

	// if directory update required
	if ( bUpdateDirectory )
	{
		if ( bVerbose )
			INFOSTR( "updating directory\n" );

		if ( !cRio.TxDirectory() )
		{
			ERRORSTR( "tx directory failed, %s\n", cRio.GetErrorStr() );
			CLEANUP_RETURN( FALSE );
		}
	}

	// if display directory request
	if ( bDisplayDir )
		DisplayDirectory( cRio, bVerbose );

	// restore
	CLEANUP_RETURN( TRUE );
}

