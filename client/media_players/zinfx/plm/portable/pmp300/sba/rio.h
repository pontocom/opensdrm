///////////////////////////////////////////////////////////////////////////////
//
//	rio.h
//
///////////////////////////////////////////////////////////////////////////////

#ifndef		_RIO_
#define		_RIO_

// dependencies
#include	"std.h"

// version
#define		CRIO_ID_VERSION				107

// blocks available on 32M and SE 64M unit
#define		CRIO_COUNT_32KBLOCKIN32M	1024
#define		CRIO_COUNT_32KBLOCKIN64M	2048

// max available 32k blocks
#define		CRIO_MAX_32KBLOCK			8192

// size of 32k block
#define		CRIO_SIZE_32KBLOCK			32768

// max directory entries
#define		CRIO_MAX_DIRENTRY			60

// 32Kblock type
#define		CRIO_ID_32KBLOCK_USED		0x00
#define		CRIO_ID_32KBLOCK_BAD		0x0f
#define		CRIO_ID_32KBLOCK_FREE		0xff

// default io delay
#if defined(_WINNT)
	#define		CRIO_TIME_IODELAY_INIT	2000
	#define		CRIO_TIME_IODELAY_RX	2
	#define		CRIO_TIME_IODELAY_TX	10
#elif defined(__OS2__)
	#define		CRIO_TIME_IODELAY_INIT	3000
	#define		CRIO_TIME_IODELAY_RX	0
	#define		CRIO_TIME_IODELAY_TX	15
#else
	#define		CRIO_TIME_IODELAY_INIT	20000
	#define		CRIO_TIME_IODELAY_RX	2
	#define		CRIO_TIME_IODELAY_TX	100
#endif

// 32 bit word
#if defined(__alpha)
	typedef int SIGNED32;
	typedef unsigned int UNSIGNED32;
#else
	typedef long SIGNED32;
	typedef unsigned long UNSIGNED32;
#endif

// error id's
enum
{
	CRIO_ERROR_ALLOC = 0,
	CRIO_ERROR_IOPRERM,
	CRIO_ERROR_CREATEFILE,
	CRIO_ERROR_DEVICENOTFOUND,
	CRIO_ERROR_DRIVERVERSION,
	CRIO_ERROR_TXBLOCKRETRY,
	CRIO_ERROR_RXBLOCKRETRY,
	CRIO_ERROR_CORRUPT,
	CRIO_ERROR_FILENOTFOUND,
	CRIO_ERROR_INVALIDFILEPOSITION,
	CRIO_ERROR_MAXDIRENTRY,
	CRIO_ERROR_MEMORY,
	CRIO_ERROR_OPEN,
	CRIO_ERROR_STAT,
	CRIO_ERROR_READ,
	CRIO_ERROR_WRITE,
	CRIO_ERROR_INTERRUPTED,
};

///////////////////////////////////////////////////////////////////////////////
// enable byte align structs
#pragma pack( 1 )

// directory header
struct CDirHeader
{
	USHORT m_usCountEntry;
	USHORT m_usCount32KBlockAvailable;
	USHORT m_usCount32KBlockUsed;
	USHORT m_usCount32KBlockRemaining;
	USHORT m_usCount32KBlockBad;
	SIGNED32 m_lTimeLastUpdate;
	USHORT m_usChecksum1;
	USHORT m_usChecksum2;
	char m_acNotUsed2[ 2 ];
	USHORT m_usVersion;
	char m_acNotUsed3[ 512 - 22 ];
};

// directory entry
struct CDirEntry
{
	USHORT m_usPos32KBlock;
	USHORT m_usCount32KBlock;
	USHORT m_usSize32KMod;
	SIGNED32 m_lSize;
	char m_acNotUsed[ 5 ];
	SIGNED32 m_lTimeUpload;
	UCHAR m_aucProperty[ 4 ];
	char m_acNotUsed3[ 5 ];
	char m_szName[ 128 - 28 ];
};

// directory block
struct CDirBlock
{
	CDirHeader m_cDirHeader;
	CDirEntry m_acDirEntry[ CRIO_MAX_DIRENTRY ];
	UCHAR m_auc32KBlockUsed[ CRIO_MAX_32KBLOCK ];
	USHORT m_ausFAT[ CRIO_MAX_32KBLOCK ];
};

// end 512 byte block
struct CEnd512ByteBlock
{
	UNSIGNED32 m_ulPos512ByteBlockNextMult256;
	UCHAR m_ucPos8192KBlockNext1;
	UCHAR m_ucPos8192KBlockNext2;
	UNSIGNED32 m_ulPos512ByteBlockPrevMult256;
	UCHAR m_ucPos8192KBlockPrev1;
	UCHAR m_ucPos8192KBlockPrev2;
	USHORT m_usChecksum;
	USHORT m_usPos32KBlockPrevMult256;
};

// find file structure
struct CFindFile {
    int m_iCount;
    CDirEntry* m_iEntry;

 private:
     CFindFile():m_iCount(-1),m_iEntry(0){}

};

// disable byte align
#pragma pack()

///////////////////////////////////////////////////////////////////////////////
// rio class
class CRio
{
protected:
	// directory block
	CDirBlock m_cDirBlock;

	// error str
	char m_szError[ 128 ];

	// external flash 32K block count
	UINT m_uiCount32KBlockAvailableExternal;

	// io port delay's
	long m_lTimeIODelayInit;
	long m_lTimeIODelayTx;
	long m_lTimeIODelayRx;

	// port constants
	int m_iPortBase;
	int m_iPortData;
	int m_iPortStatus;
	int m_iPortControl;

	// error
	int m_iIDError;

	// internal/external flash ram flag
	BOOL m_bUseExternalFlash;

	// special edition flag
	BOOL m_bSpecialEdition;

	// operations
	void LogError( int iType, const char* pszFormat, ... );
	UINT FindFirstFree32KBlock( void );
	UINT CalculateChecksum1( void );
	UINT CalculateChecksum2( void );
	BOOL WaitInput( int iValue );
	BOOL WaitAck( void );
	UINT GetDataByte( void );
	BOOL IOIntro( void );
	BOOL IOOutro( void );

	BOOL Tx32KBlockRetry( void* pv, UINT uiPos32KBlock, UINT uiPos32KBlockPrev,
		UINT uiPos32KBlockNext );
	BOOL Tx32KBlock( void* pv, UINT uiPos32KBlock, UINT uiPos32KBlockPrev,
		UINT uiPos32KBlockNext );
	BOOL Rx32KBlockRetry( void* pv, UINT uiPos32KBlock );
	BOOL Rx32KBlock( void* pv, UINT uiPos32KBlock );
	BOOL MarkBadBlocks( BOOL (*pfProgress)(int iPos, int iCount, void* cookie), void* cookie );

public:

	// set/unset
	void Unset( void );
	BOOL Set( int iPortBase );

	// constructors/destructors
	CRio();
	~CRio();

	// retrieval
	long GetIODelayInit( void ) { return m_lTimeIODelayInit; }
	long GetIODelayTx( void ) { return m_lTimeIODelayTx; }
	long GetIODelayRx( void ) { return m_lTimeIODelayRx; }
	CDirBlock& GetDirectoryBlock( void ) { return m_cDirBlock; }
	int GetErrorID( void ) { return m_iIDError; }
	char* GetErrorStr( void ) { return m_szError; }
	BOOL GetUseExternalFlashStatus( void ) { return m_bUseExternalFlash; }
	BOOL GetSpecialEditionStatus( void ) { return m_bSpecialEdition; }

	// operations
	void SetIODelayInit( long lTime ) { m_lTimeIODelayInit = lTime; }
	void SetIODelayTx( long lTime ) { m_lTimeIODelayTx = lTime; }
	void SetIODelayRx( long lTime ) { m_lTimeIODelayRx = lTime; }
	void UseExternalFlash( BOOL bUseExternalFlash );
	BOOL CheckPresent( void );
	CDirEntry* FindFile( const char* pszFile,CFindFile* pFindFile = NULL );
	BOOL Initialize( BOOL bMarkBadBlock, BOOL (*pfProgress)(int iPos, int iCount, void* cookie), void* cookie);
	BOOL RemoveFile( const char* pszFile );
	BOOL RemoveAllFiles( void );
	BOOL SetFileOrder( UINT* pauiPosOrder, UINT uiCount );
	BOOL TxDirectory( void );
	BOOL RxDirectory( void );
	BOOL TxFile( const char* pszPathFile, BOOL (*pfProgress)(int iPos, int iCount, void* cookie), void* cookie);
	BOOL RxFile( const char* pszPathFile, BOOL (*pfProgress)(int iPos, int iCount, void* cookie), void* cookie);
};

#endif		// _RIO_

