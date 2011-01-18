/********************************************************** 
** Copyright 1999 Earth Resource Mapping Ltd.
** This document contains proprietary source code of
** Earth Resource Mapping Ltd, and can only be used under
** one of the three licenses as described in the 
** license.txt file supplied with this distribution. 
** See separate license.txt file for license details 
** and conditions.
**
** This software is covered by US patent #6,442,298,
** #6,102,897 and #6,633,688.  Rights to use these patents 
** is included in the license agreements.
** 
** FILE:   	ncsecw.c
** CREATED:	1 March 1999
** AUTHOR: 	SNS
** PURPOSE:	Glue functions to access ECW files from the NCS system
** EDITS:
** [01] sns	01Mar99 Created file
** [02] sns 02Apr99 Not doing Mutex locks during process shutdown
** [03] sns 09May99 Using new logic for callback interface
** [04] sjc 30Dec99 Changed Sanity check to global func from static
** [05] sjc 30Mar00 Modified to use Shared Memory for stats instead of
**					  Shared dataseg in dll.  Handles backwards compatibility.
**					  Means NCSEcw.dll can be anywhere in system and still
**					  use global caching stats
** [06] sjc 30Apr00	Merged Mac SDK port
** [07] ny  12May00	Added bIsConnected field to the NCSFileStruct structure
** [08] sjc 20Jun00 Out-of-Order block fix
** [09] rdh 04Oct00 Added NCSEcwReadLineType enum type
** [10] ddi 14Nov00	Moved NCSECWReadLineType to ecw.h
** [11]  ny 23Nov00	Added #include "NCSUtil.h" so that MACINTOSH is defined
** [12]  ny 06Dec00	Change #include "NCSUtil.h" to #include "NCSTypes.h"
** [13] jmp 23Jul01 Fix where IDWT thread can become suspended after a SetView.
** [14] mjs 26Nov01 Fixed old ermapper include warnings.
** [15] sjc 04Feb02 Added MAXOPEN
** [16] sjc 19Feb02 Exposed cache purge parameters
** [17] rar 17Jun02 Changed NCScbmReadFileBlockLocal() to check if a read failed due to network error
**						and if so set a flag (pNCSFile->bFileIOError).  This was needed so that if the
**						IWS and ecw_img knows the network file must be closed and reopened.
** [18] rar 19Jun02 Added mutex around NCScbmReadFileBlockLocal() and NCScbmGetFileBlockSizeLocal() to
**						prevent the IWS and ecw_img trying to read from the same file at the same time.
** [19] rar 02Sep03 Added option to try ecwp re-connection if connection lost
** [21] tfl 20Jan06 Added option to control maximum size of progressive views in NCSEcwInfo
** 
** NOTES
**
********************************************************/

#ifndef NCSECW_H
#define NCSECW_H

#include "NCSDefs.h" /**[12]**/
#if !defined(_WIN32_WCE)
	#include <time.h>
#endif
#ifdef MACINTOSH
#include <MacTypes.h>
#include <stat.h>
#include <stdio.h>
#else	/* MACINTOSH */
#if !defined(_WIN32_WCE)
	#include <sys/types.h>
	#include <sys/stat.h>
#endif
#endif	/* MACINTOSH */
#include <stdio.h>
#include "NCSMemPool.h"				// need pool management
#include "NCSThread.h"				// we need locks and threads
#include "NCSMutex.h"
#include "NCSPackets.h"				// Server Packets
#include "NCScnet.h"				// client network layer
#include "ECW.h"
#include "NCSECWClient.h"			// [03] to get public structure information

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************
**	NCSECW Cache management values
**********************************************************/

// If a File View currently has a view size (window size) that
// is smaller than this number, blocks will be cached,
// otherwise the blocks will be freed as soon as they are used

#define NCSECW_MAX_VIEW_SIZE_TO_CACHE	4000


#define NCSECW_MAX_UNUSED_CACHED_FILES	10000	// Maximum number of files that can be open and unused
#define NCSECW_CACHED_BLOCK_POOL_SIZE	1000	// initial size of cached block pointers pool
#define NCSECW_MAX_SEND_PACKET_SIZE		(8*1024)	// maximum size of a packet to send
#define NCSECW_IDWT_QUEUE_GRANULARITY	32		// realloc the setview queue every this often

// Time related
#define NCSECW_PURGE_DELAY_MS			1000	// only purge cache after at least this time has passed since last purge
// These are stored in the global shared application structure, and can be changed by users
#define NCSECW_BLOCKING_TIME_MS			10000	// wait up to 10 seconds before giving up, for blocking clients
#define NCSECW_QUIET_WAIT_TIME_MS		10000	// wait up to 10 seconds before giving up, for blocking clients
#define	NCSECW_REFRESH_TIME_MS			500		// allow 0.5 seconds to pass before issuing a new refresh if new blocks have arrived
#define NCSECW_FILE_PURGE_TIME_MS		(30*60*1000)	// Ideally keep files for up to 30 minutes when idle
#define NCSECW_FILE_MIN_PURGE_TIME_MS	(30*1000)		// never purge files in less than 30 seconds

#define NCSECW_MAX_SETVIEW_PENDING	1	// maximum number of SetViews that can be pending
#define NCSECW_MAX_SETVIEW_CANCELS	3	// maximum number of SetViews that will be cancelled,
										// before forcing completion regardless of number pending
#define NCSECW_HIGHMAX_SETVIEW_PENDING 10	// unless this many setviews are pending, in which case
											// we flush pending's regardless (if we can)
#define NCSECW_MAX_OFFSET_CACHE		1024	// Offset cache size for ECW files with RAW block tables

// The type of caching used for a particular file view
typedef enum {
	NCS_CACHE_INVALID = 0,	// invalid caching method
	NCS_CACHE_DONT	  = 1,	// Don't cache for this file view (typically because a big file view)
	NCS_CACHE_VIEW	  = 2	// A "normal" file view, so cache blocks where possible
} NCSCacheMethod;

// The type of request for a block in a file cache list
typedef enum {
	NCSECW_BLOCK_INVALID	= 0,	// invalid block request
	NCSECW_BLOCK_REQUEST	= 1,	// Post a Request for the block
	NCSECW_BLOCK_CANCEL		= 2,	// Post a Cancel for the block
	NCSECW_BLOCK_RETURN		= 3		// Return the block in the cache list
} NCSEcwBlockRequestMethod;

// [03] current thread state
typedef enum {
	NCSECW_THREAD_INVALID	= 0,	// invalid state
	NCSECW_THREAD_DEAD		= 1,	// Thread is not running
	NCSECW_THREAD_ALIVE		= 2,	// Thread alive and running
	NCSECW_THREAD_SUSPENDED	= 3		// Thread alive and suspended
} NCSEcwThreadState;

// [03] current view callback state. ONLY used for callback style views
typedef enum {
	NCSECW_VIEW_INVALID		= 0,	// invalid state
	NCSECW_VIEW_QUIET		= 1,	// open, but no active setview, and not processing the IDWT
	NCSECW_VIEW_SET			= 2,	// Setview done, but not yet reading data
	NCSECW_VIEW_QUEUED		= 3,	// queued to process a iDWT for the view (in Thread queue)
	NCSECW_VIEW_IDWT		= 4		// processing a IDWT for the view
} NCSEcwViewCallbackState;

// [03] insertion order into a queue. Generally, LIFO is faster than FIFO if you don't care
typedef enum {
	NCSECW_QUEUE_INVALID	= 0,	// invalid state
	NCSECW_QUEUE_LIFO		= 1,	// Insert into queue in LIFO order
	NCSECW_QUEUE_FIFO		= 2		// Insert into queue in FIFO order
} NCSEcwQueueInsertOrder;

/*********************************************************
**	NCSECW Structure definitions
**********************************************************/

typedef struct NCSFileCachedBlockStruct {
	NCSBlockId	nBlockNumber;	// block number being cached
	struct NCSFileCachedBlockStruct	*pNextCachedBlock;	// next block in the cache list for the file
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
	Handle	pPackedECWBlock;
	Handle  pUnpackedECWBlock;
#else
	UINT8	*pPackedECWBlock;	// the packed ECW block that is currently cached (or NULL)
	UINT8	*pUnpackedECWBlock;	// the packed ECW block that is currently cached (or NULL)
#endif
	UINT32	nPackedECWBlockLength;	// Length of the packed block
	UINT32	nUnpackedECWBlockLength;// Length of the unpacked block
	UINT32	nUsageCount;		// number of times in use by FileViews
	UINT32	nHitCount;			// number of times block has been read during inverse DWT operations
	BOOLEAN	bRequested;			// only TRUE if block request HAS BEEN SENT to the server
								// (so FALSE in every other situation, even if block not loaded yet)
	UINT64	nDecodeMissID;		/**[08]**/
} NCSFileCachedBlock;

// This is used during cache purging. There is one entry per level in the file
typedef struct NCSFileCachePurgeStruct {
	NCSFileCachedBlock	*pPreviousBlock;	// the block BEFORE the first block at this level
	NCSFileCachedBlock	*pLevelBlocks;		// the first block at this level
} NCSFileCachePurge;

typedef struct {
	NCSBlockId	nID;
	UINT32		nLength;
	UINT64		nOffset;
	NCSTimeStampMs tsLastUsed;
} NCSFileBlockOffsetEntry;

struct NCSFileStruct {
	QmfLevelStruct	*pTopQmf;				// Pointer to the top level of the ECW QMF structure
	struct	NCSFileStruct		*pNextNCSFile, *pPrevNCSFile;	// NCSECW linked list of NCS files cached
	UINT32	nUsageCount;			// number of times this file is currently open
	UINT32	SemiUniqueId;			// Somewhat unique ID, based on File Name
	BOOLEAN bReadOffsets;			// TRUE if the block offsets table has been read and is valid for the QMF
	BOOLEAN bReadMemImage;			// TRUE if the NCSFile has a memory image of the header present
	BOOLEAN	bLocalFile;				// TRUE if this is a local file
	char	*szUrlPath;				// URL (filename) for this file
	BOOLEAN	bValid;					// File is currently valid (it has not changed on disk since open).
	// Client side information (not valid when file opened at the server end)
	NCSTimeStampMs	tLastSetViewTime;// Last time a SetView was done, used to decide when to purge files from cache
	NCSPool			*pBlockCachePool;		// Pointer to pool of cached blocks
	NCSFileCachedBlock	*pFirstCachedBlock;	// Pointer to first block in the cached block list
	NCSFileCachedBlock	*pWorkingCachedBlock;	// Pointer to block last accessed (reduces search times)
	NCSFileCachedBlock	*pLastReceivedCachedBlock;	// Pointer to last block received to speed list access
	NCSPacket			*pSendPacket;		// Pointer to Send Packet - only one packet allocated & kept for life of File open
	NCSClientUID	nClientUID;		// Unique UID for this client file opened
	NCSSequenceNr	nServerSequence; // current maximum sequence number read from back from the server
	NCSSequenceNr	nClientSequence; // current sequence number client has issued
	UINT8	*pLevel0ZeroBlock;		// a level 0 zero block containing all zeros for all bands and sidebands
	UINT8	*pLevelnZeroBlock;		// a > level 0 zero block (has one less sideband than level 0) 
	struct	NCSFileViewStruct	*pNCSFileViewList;			// list of OPEN file views for this file
	struct	NCSFileViewStruct	*pNCSCachedFileViewList;	// CLOSED but Cached File Views for this file
	// Client network related information (not valid when file opened at the server end)
	pNCSnetClient	pClientNetID;		// client network layer file channel NetID
	BOOLEAN	bSendInProgress;			// if TRUE, a send pack (request and/or cancel) has been made, and callback will be made once complete
	UINT32	nRequestsXmitPending;		// if non-zero, number of block read requests waiting to send
										// (NOT requests already sent that responses are waiting for)
	UINT32	nCancelsXmitPending;		// if non-zero, number of block cancel requests waiting to sent
	UINT32	nUnpackedBlockBandLength;	// length of one band in an unpacked block (always unpacked out to max X&Y block size)
	NCSFileCachePurge	*pNCSCachePurge;// an array, one entry per QMF level
	UINT16	nCachePurgeLevelCount;		// maximum levels CURRENTLY in the NCSCachePurge array, might be less than max_levels+1
	ECWFileInfoEx *pFileInfo;				// Handy collection of information about the file.
	BOOLEAN bIsConnected;				// Are we still connected to the server serving this file /**[07]**/
	BOOLEAN	bIsCorrupt;						// File is corrupt - displayed message for user

	NCSFileBlockOffsetEntry *pOffsetCache;
	UINT32					nOffsetCache;
	BOOLEAN		bFileIOError; //[17]
	NCSMutex	mFileAccess; //[18]
	
	NCSTimeStampMs tsLastReconnectTry;  //[19] The time of the last attempt to reconnect to the IWS
	UINT32	nReconnectCount;
};

struct NCSFileViewStruct {
	NCSFile			*pNCSFile;
	QmfRegionStruct *pQmfRegion;	// pointer to current QMF region
	NCSCacheMethod	nCacheMethod;	// caching method used by this file view
	struct NCSFileViewStruct	*pNextNCSFileView, *pPrevNCSFileView;	// list of views for this file
	NCSTimeStampMs	tLastBlockTime;	// Time that the last block was received.
									// Currently only valid for served (not local) files
	NCSEcwReadStatus (*pRefreshCallback)(struct NCSFileViewStruct *pNCSFileView);	// Refresh callback function
	BOOLEAN		bTriggerRefreshCallback;	// TRUE if we need to trigger a refresh callback.
									// Deferred so can start in other threads and outside mutex's
	NCSEcwViewCallbackState	eCallbackState;
	BOOLEAN	bGoToQuietState;		// TRUE if the view is being forced to quiet (cancelling iDWTs etc)
	NCSFileViewSetInfo	info;		// public information, and current view values
	NCSFileViewSetInfo	pending;	// pending SetView, which is waiting for current view to finish processing
	UINT16		nPending;			// Number of pending SetView's outstanding
	UINT16		nCancelled;			// Number of SetViews cancelled since a successful one
	BOOLEAN		bIsRefreshView;		// TRUE if the current view is just updating the previous view.
									// If true, the current view will be cancelled when a SetView is done
	UINT64		nNextDecodeMissID;	/**[08]**/
};

typedef struct NCSecwStatisticsStruct {
	// NOTE:  DO NOT CHANGE THIS STRUCTURE IN ANY WAY.

	UINT32	nApplicationsOpen;		// number of applications currently open

	// Time wait for network server to respond with blocks
	UINT32	nBlockingTime;			// Time in ms to block clients that don't support callbacks
	UINT32	nRefreshTime;			// Time in ms to wait between refresh callbacks to smart clients

	// Statistics

	UINT32	nFilesOpen;				// number of files currently open
	UINT32	nFilesCached;			// number of files currently cached but not open
	UINT32	nFilesCacheHits;		// number of times an open found an existing cached file
	UINT32	nFilesCacheMisses;		// number of times an open could not find the file in cache
	UINT32	nFilesModified;			// number of times files were invalidated because they changed on disk while open

	UINT32	nFileViewsOpen;			// number of file views currently open
	UINT32	nFileViewsCached;		// number of file views cached but not open

	UINT32	nSetViewBlocksCacheHits;		// number of times a SetView hit a cached block structure
	UINT32	nSetViewBlocksCacheMisses;		// number of times a SetView missed a cached block

	UINT32	nReadBlocksCacheHits;			// number of times a read hit a cached block
	UINT32	nReadUnpackedBlocksCacheHits;	// number of times a read hit a unpacked cached block
	UINT32	nReadBlocksCacheMisses;			// number of times a read missed a cached block
	UINT32	nReadBlocksCacheBypass;			// number of times a read bypassed cache (for large view IO)

	UINT32	nRequestsSent;					// number of block read requests sent to the server
	UINT32	nCancelsSent;					// number of block read request cancels sent to the server
	UINT32	nBlocksReceived;				// number of blocks received
	UINT32  nCancelledBlocksReceived;		// number of blocks cancelled that were still received

	UINT32	nRequestsXmitPending;			// number of block read requests waiting to be sent to server
	UINT32	nCancelsXmitPending;			// number of block cancel requests waiting to be sent to server

	UINT32	nPackedBlocksCacheSize;			// Size in bytes of packed blocks in cache
	UINT32	nUnpackedBlocksCacheSize;		// Size in bytes of unpacked blocks in cache
	UINT32	nMaximumCacheSize;				// Maximum allowed size of cache
} NCSecwStatisticsV1;

// UP this version if you add more fields to structure.
#define ECW_STATS_STRUCT_VERSION_2 2
#define ECW_STATS_STRUCT_VERSION 3

#define ECW_STATS_LOCK_NAME "NCSCBMStatsLock"
#ifdef _DEBUG
#define NCS_STATUS_STRUCT_NAME_VERSION_2 "NCSEcwSharedStatisticsSHM DEBUG"
#define NCS_STATUS_STRUCT_NAME_VERSION "NCSEcwSharedStatisticsSHM3 DEBUG"
#else
#define NCS_STATUS_STRUCT_NAME_VERSION_2 "NCSEcwSharedStatisticsSHM"
#define NCS_STATUS_STRUCT_NAME_VERSION "NCSEcwSharedStatisticsSHM3"
#endif

#ifdef WIN32
typedef LONG NCSEcwStatsType;
#else
typedef INT32 NCSEcwStatsType;
#endif

typedef struct tagNCSEcwStatistics {
	// NOTE:  DO NOT CHANGE ANY OF THESE FIELDS.  ADD NEW FIELDS AT END OF STRUCTURE.
	// FIELDS DOWN TO nMaximumCacheSize MUST BE IDENTICAL TO NCSecwStatisticsV1

	NCSEcwStatsType	nApplicationsOpen;		// number of applications currently open

	// Time wait for network server to respond with blocks
	NCSEcwStatsType	nBlockingTime;			// Time in ms to block clients that don't support callbacks
	NCSEcwStatsType	nRefreshTime;			// Time in ms to wait between refresh callbacks to smart clients

	// Statistics

	NCSEcwStatsType	nFilesOpen;				// number of files currently open
	NCSEcwStatsType	nFilesCached;			// number of files currently cached but not open
	NCSEcwStatsType	nFilesCacheHits;		// number of times an open found an existing cached file
	NCSEcwStatsType	nFilesCacheMisses;		// number of times an open could not find the file in cache
	NCSEcwStatsType	nFilesModified;			// number of times files were invalidated because they changed on disk while open

	NCSEcwStatsType	nFileViewsOpen;			// number of file views currently open
	NCSEcwStatsType	nFileViewsCached;		// number of file views cached but not open

	NCSEcwStatsType	nSetViewBlocksCacheHits;		// number of times a SetView hit a cached block structure
	NCSEcwStatsType	nSetViewBlocksCacheMisses;		// number of times a SetView missed a cached block

	NCSEcwStatsType	nReadBlocksCacheHits;			// number of times a read hit a cached block
	NCSEcwStatsType	nReadUnpackedBlocksCacheHits;	// number of times a read hit a unpacked cached block
	NCSEcwStatsType	nReadBlocksCacheMisses;			// number of times a read missed a cached block
	NCSEcwStatsType	nReadBlocksCacheBypass;			// number of times a read bypassed cache (for large view IO)

	NCSEcwStatsType	nRequestsSent;					// number of block read requests sent to the server
	NCSEcwStatsType	nCancelsSent;					// number of block read request cancels sent to the server
	NCSEcwStatsType	nBlocksReceived;				// number of blocks received
	NCSEcwStatsType  nCancelledBlocksReceived;		// number of blocks cancelled that were still received

	NCSEcwStatsType	nRequestsXmitPending;			// number of block read requests waiting to be sent to server
	NCSEcwStatsType	nCancelsXmitPending;			// number of block cancel requests waiting to be sent to server

	NCSEcwStatsType	nPackedBlocksCacheSize;			// Size in bytes of packed blocks in cache
	NCSEcwStatsType	nUnpackedBlocksCacheSize;		// Size in bytes of unpacked blocks in cache
	NCSEcwStatsType	nMaximumCacheSize;				// Maximum allowed size of cache

	// Add NEW things below ALWAYS AT END!
	//
	// NOTE: If you add extra stats here, increment ECW_STATS_STRUCT_VERSION and always check the nStatsStructVersion global
	// to ensure the new version of the structure is available.  If NCSecw.dll exists in the C:\winnt\system32 (NT/2000) or 
	// C:\Windows\system (Win9x) directories, that Dll will be loaded and the shared data segment in it used instead of a
	// shared memory segment.  This means you may be using an older version of the stats structure than you compiled in, hence
	// the need to check the version (and hence size).  This is all done for backwards compatibility due to the move from a
	// shared Dll in the system directory to a local dll but the need to shared global stats across multiple applications.
} NCSecwStatistics;


// [03] One of these per iDWT thread in the iDWT pool of threads (currently only 1 thread)
// These threads are ONLY used to process iDWT's for callback based SetViews.
typedef struct {
	NCSThread	tIDWT;						// Inverse Descrete Wavelet Transform thread ID
	NCSEcwThreadState	eIDWTState;			// current state of the iDWT thread
	NCSFileView	**ppNCSFileView;			// array of pointers to FileViews to process callbacks for
	INT32		nQueueAllocLength;			// total queue length currently allocated; it might grow
	INT32		nQueueNumber;				// number of active items in the queue, could be zero
} NCSidwt;

typedef struct {
	NCSFile	*pNCSFileList;					// List of NCS files currently open
	NCSSHMInfo	*pStatisticsSHM;
	UINT8		nStatsStructVersion;
	NCSecwStatistics	*pStatistics;
#ifdef WIN32
	NCSThreadLSKey		StatsLockKey;
#endif
	NCSMutex	mutex;
	NCSidwt		*pIDWT;						// iDWT thread. Currently only one thread ever started
	BOOLEAN	bShutdown;						// [02] true if in process shutdown mode
	BOOLEAN	nForceFileReopen;				// normally FALSE - only set to TRUE for server load testing
	BOOLEAN bNoTextureDither;				// normally FALSE - TRUE for compression, so input compressed files have no texture dither added
	BOOLEAN	bForceLowMemCompress;			// Force a low memory compressioin
	NCSTimeStampMs	tLastCachePurge;		// How long ago the last Cache Purge was carried out
	UINT32	nAggressivePurge;				// Higher number means previous purge did not reduce down to
											// desired purge budget, so be more aggressive on next purge
	UINT32	nMaximumOpen;					// [15] Max # files open in cache
#ifdef WIN32
	HANDLE m_hSuspendEvent;					// /**[13]**/ used to fix bug where IDWT thread can be left suspended after a SetView
#endif // WIN32
	NCSTimeStampMs	nPurgeDelay;			// [16] Time delay after last purge before new purge allowed
	NCSTimeStampMs  nFilePurgeDelay;		// [16] Time delay between last view closing and file being purged from cache
	NCSTimeStampMs  nMinFilePurgeDelay;		// [16] Min Time delay between last view closing and file being purged from cache

	UINT32			nMaxOffsetCache;		// [16] Max size of offset cache for ECW files with RAW block tables.

	BOOLEAN bEcwpReConnect;					// [19] normally FALSE - if TRUE the ecw library will try and reconnect if connection has been lost to IWS
	BOOLEAN bJP2ICCManage;					// [20] normally TRUE - if FALSE the ecw library does not do ICC management on READ
	UINT32	nMaxJP2FileIOCache;				// [20] JP2 file IO cache size
	UINT32	nMaxProgressiveViewSize;		// [21] Maximum height and width of progressive mode views
} NCSEcwInfo;



/**********************************************************
**	NCS to ECW Glue and wrapper functions
**********************************************************/


extern NCSError NCSecwOpenFile(
					NCSFile **ppNCSFile,
					char *szInputFilename,		// input file name or network path
					BOOLEAN bReadOffsets,		// TRUE if the client wants the block Offset Table
					BOOLEAN bReadMemImage);		// TRUE if the client wants a Memory Image of the Header

extern int	NCSecwCloseFile( NCSFile *pNCSFile);

extern int	NCSecwReadLocalBlock( NCSFile *pNCSFile, UINT64 nBlockNumber,
					UINT8 **ppBlock, UINT32 *pBlockLength);
extern BOOLEAN NCScbmGetFileBlockSizeLocal(NCSFile *pNCSFile, NCSBlockId nBlock, UINT32 *pBlockLength, UINT64 *pBlockOffset );

extern NCSecwStatistics *NCSecwGetStatistics( void );

// Internal functions to the library - not visible to the DLL
int	 NCSecwCloseFileCompletely( NCSFile *pNCSFile);
void NCSEcwStatsLock(void);
void NCSEcwStatsUnLock(void);
void NCSEcwStatsIncrement(NCSEcwStatsType *pVal, INT32 n);
void NCSEcwStatsDecrement(NCSEcwStatsType *pVal, INT32 n);

/**********************************************************
**	Routines internal to the library - not visible to the DLL
**********************************************************/

int	NCScbmCloseFileViewCompletely(NCSFileView **ppNCSFileViewList, NCSFileView *pNCSFileView);
UINT8 *NCSecwConstructZeroBlock(QmfLevelStruct *p_qmf, QmfRegionStruct *p_region,
						UINT32 x_block, UINT32 y_block);
UINT8	*NCScbmReadViewBlock(QmfRegionLevelStruct	*pQmfRegionLevel,
					  UINT32 nBlockX, UINT32 nBlockY);
void NCScbmFreeViewBlock(QmfRegionLevelStruct	*pQmfRegionLevel, UINT8 *pECWBlock);
BOOLEAN	NCScbmNetFileBlockRequest(NCSFile *pNCSFile, NCSBlockId nBlock );
NCSError NCScbmNetFileOpenInternal(UINT8 **ppHeaderMemImage, UINT32 *pnHeaderMemImageLen, pNCSnetClient *pClientID, NCSClientUID *pClientUID, NCSnetPacketSentCB *pSentCB, NCSnetPacketRecvCB *pRecvCB, void *pCBData, char *szUrlPath);
NCSError NCScbmNetFileOpen(UINT8 **ppHeaderMemImage, UINT32 *pnHeaderMemImageLen, NCSFile *pNCSFile, char *szUrlPath); //[19]
void	NCScbmNetFileXmitRequests(NCSError nError, UINT8 *pLastPacketSent, NCSFile *pNCSFile );
NCSFileCachedBlock *NCScbmGetCacheBlock(NCSFile *pNCSFile, NCSFileCachedBlock *pWorkingCachedBlock,
											   NCSBlockId nBlock, NCSEcwBlockRequestMethod eRequest);



void NCScbmInitThreadIDWT(NCSidwt *pIDWT);
void NCScbmFinishThreadIDWT(NCSidwt *pIDWT);
void NCScbmQueueIDWTCallback(NCSFileView *pNCSFileView,			// queue a callback for this file view
							 NCSEcwQueueInsertOrder eOrder);	// insert LIFO or FIFO
#ifdef _DEBUG
int	NCScbmSanityCheckFileView(NCSFileView *pNCSFileView);	/**[04]**/
#endif
/**********************************************************
**	GLOBAL NCSECW variables
**********************************************************/

#ifdef NCSECWGLOBAL
NCSEcwInfo *pNCSEcwInfo = NULL;
#else
extern NCSEcwInfo *pNCSEcwInfo;
#endif

#ifdef __cplusplus
}
#endif

#endif	// NCSECW_H
