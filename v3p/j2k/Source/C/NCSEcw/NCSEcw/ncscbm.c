/********************************************************** 
** Copyright 1998 Earth Resource Mapping Ltd.
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
** FILE:   	ncscbm.c
** CREATED:	1 March 1999
** AUTHOR: 	SNS
** PURPOSE:	NCS Client Block Manager functions
** EDITS:
** [01] sns	03-Mar-99 Created file
** [02] sns 07-Apr-99 updated callback logic
** [03] sns 09-May-99 removed mutexes from NCScbmReadViewBlock() code except when unpacking a block
** [04] sns 17-May-99 Added smarter handling of pending SetViews
** [05] sns 05-Jun-99 Added read mutex for debugging
** [06] sjc 09-Sep-99 Added proper error handling to SetFileView()
** [07] sjc 30-Dec-99 Added global mutex for stats seg
** [08]  ny 12-May-00 Added support to propagate lost of connection information
**					  higher up for proper handling.
** [09] sjc 15-May-00 QueueIDWT in this case, so callback called immediately after setview() 
**					  rather than waiting for network packets arriving
** [10] sjc 23-May-00 Only queue callback if some data is available
** [11] sjc 20-Jun-00 Out-of-Order block fix
** [12]  ny 20-Jun-00 Added support to propagate lost of connection information
**					  higher up for proper handling.
** [13] sjc 13-Jul-00 Fixed missing iDWT
** [14] ddi 14-Nov-00 Added NCScbmReadViewLineIEEE4() and NCScbmReadViewLineUINT16()
** [15]  ny 23-Nov-00 Mac port modification
** [16] rar 19-Jan-01 Mac port modification
** [17] sjc 25-Apr-01 Bug fixes for 9x with remote >2GB files and other read errors
** [18] rar 17-Aug-01 Changed return code in NCScbmReadViewLineBILEx from NCS_INVALID_PARAMETER to
**						NCSECW_READ_FAILED as NCS_INVALID_PARAMETER is not a valid return.
** [19] rar 13-Jun-02 Fixed potential memory leak
** [20] rar 17-Jun-02 Changed NCScbmReadFileBlockLocal() to check if a read failed due to network error
**						and if so set a flag (pNCSFile->bFileIOError).  This was needed so that if the
**						IWS and ecw_img knows the network file must be closed and reopened.
** [21] rar 19-Jun-02 Added mutex around NCScbmReadFileBlockLocal() and NCScbmGetFileBlockSizeLocal() to
**						prevent the IWS and ecw_img trying to read from the same file at the same time.
** [22] sjc 10-Dec-02 Fix for bug #1220, broken IO callbacks.
** [23] sjc 03-Nov-03 Check block size first, may be a corrupt ECW.
** [24] sjc 01-Sep-05 Change to caching logic (purge now occurs earlier)
** [25] tfl 15-Sep-05 Change to length calculation to align with word boundary (Bill Binko)
** [26] sjc 21-Sep-05 Use upstream NCSJP2 view tiling logic with >4kx4k views
 *******************************************************/

#include "NCSEcw.h"
#if !defined(_WIN32_WCE)
	#ifdef WIN32 // [16]
		#include "crtdbg.h"
	#endif
#endif
#include "NCSLog.h"
#include "NCSBuildNumber.h"

int NCScbmFileViewRequestBlocks(NCSFileView *pNCSFileView, QmfRegionStruct *pQmfRegion, NCSEcwBlockRequestMethod eRequest);
#ifdef MACINTOSH
Handle NCScbmReadFileBlockLocal(NCSFile *pNCSFile, NCSBlockId nBlock, UINT32 *pBlockLength );
#else
UINT8	*NCScbmReadFileBlockLocal(NCSFile *pNCSFile, NCSBlockId nBlock, UINT32 *pBlockLength );
#endif



#define DO_READ_MUTEX		// define to turn on READ MUTEX (should not be needed)
#undef USE_PARENT_CHECK	// do the parent block check

/*******************************************************
**	NCScbmSetFileView()	- set bands/extents/size for view area
**
**	Notes:
**	(1)	-	You can alter any or none of the parameters
**	(2)	-	You can't read from the view until at least one SetViewFile call is done
**	(3)	-	Only one thread fiddles with QMF Region at one time
**	(4) -	A refresh callback will be made immediately if all blocks are already available
**
********************************************************/

#ifdef NCSJPC_ECW_SUPPORT
NCSError	NCScbmSetFileViewEx_ECW(NCSFileView *pNCSFileView,
				UINT32 nBands,					// number of bands to read
				UINT32 *pBandList,				// index into actual band numbers from source file
			    UINT32 nTopX, UINT32 nLeftY,	// Top-Left in image coordinates
				UINT32 nBottomX, UINT32 nRightY,// Bottom-Right in image coordinates
				UINT32 nSizeX, UINT32 nSizeY,	// Output view size in window pixels
				IEEE8 fTopX, IEEE8 fLeftY,		// Top-Left in world coordinates
				IEEE8 fBottomX, IEEE8 fRightY)	// Bottom-Right in world coordinates
#else
NCSError	NCScbmSetFileViewEx(NCSFileView *pNCSFileView,
				UINT32 nBands,					// number of bands to read
				UINT32 *pBandList,				// index into actual band numbers from source file
			    UINT32 nTopX, UINT32 nLeftY,	// Top-Left in image coordinates
				UINT32 nBottomX, UINT32 nRightY,// Bottom-Right in image coordinates
				UINT32 nSizeX, UINT32 nSizeY,	// Output view size in window pixels
				IEEE8 fTopX, IEEE8 fLeftY,		// Top-Left in world coordinates
				IEEE8 fBottomX, IEEE8 fRightY)	// Bottom-Right in world coordinates
#endif
{
	QmfRegionStruct	*pOldQmfRegion;
	NCSCacheMethod	nOldCacheMethod, nNewCacheMethod;

	if (!pNCSEcwInfo) {
		NCSecwInitInternal();
	}

	NCSMutexBegin(&pNCSEcwInfo->mutex);

#define MAXRECONNECTCOUNT 4

	// If connection has been lost then try and reconnect
	if( !pNCSFileView->pNCSFile->bLocalFile && !pNCSFileView->pNCSFile->bIsConnected &&
				(pNCSFileView->pNCSFile->nReconnectCount < MAXRECONNECTCOUNT) && pNCSEcwInfo->bEcwpReConnect ) {
		NCSTimeStampMs tsNow = NCSGetTimeStampMs();
		if( tsNow > (pNCSFileView->pNCSFile->tsLastReconnectTry + 10*1000) ) {
			//Try and reconnect
			UINT8	*pFileHeaderMemImage=NULL;
			UINT32	nFileHeaderMemImageLen=0;
			NCSError eResult = NCS_SUCCESS;

			pNCSFileView->pNCSFile->tsLastReconnectTry = tsNow;
			pNCSFileView->pNCSFile->nReconnectCount++;

			//Close existing connection
			NCScnetDestroy(pNCSFileView->pNCSFile->pClientNetID);
			pNCSFileView->pNCSFile->pClientNetID = NULL;
			//Re-Connect
			eResult = NCScbmNetFileOpen(&pFileHeaderMemImage, &nFileHeaderMemImageLen, pNCSFileView->pNCSFile, pNCSFileView->pNCSFile->szUrlPath);
			if( eResult == NCS_SUCCESS) {
				//compare the headers to make sure they are the same
				if( pNCSFileView->pNCSFile->pTopQmf->pHeaderMemImage &&
					(nFileHeaderMemImageLen == pNCSFileView->pNCSFile->pTopQmf->nHeaderMemImageLen) &&
					(memcmp(pNCSFileView->pNCSFile->pTopQmf->pHeaderMemImage, pFileHeaderMemImage, nFileHeaderMemImageLen)==0) )
				{
					// the headers are the same
					pNCSFileView->pNCSFile->bIsConnected = TRUE;
					pNCSFileView->pNCSFile->nReconnectCount = 0;
				} else {
					pNCSFileView->pNCSFile->nReconnectCount = MAXRECONNECTCOUNT; // The headers were different so don't retry as the file must have changed
					NCScnetDestroy(pNCSFileView->pNCSFile->pClientNetID);
					pNCSFileView->pNCSFile->pClientNetID = NULL;
					pNCSFileView->pNCSFile->bIsConnected = FALSE;
				}
				NCS_SAFE_FREE( pFileHeaderMemImage );
			}
		}
	}


	if(pNCSFileView->pNCSFile->pTopQmf && pNCSFileView->pNCSFile->pTopQmf->p_file_qmf) {		/**[06]**/
		UINT32 x_size = pNCSFileView->pNCSFile->pTopQmf->p_file_qmf->x_size;
		UINT32 y_size = pNCSFileView->pNCSFile->pTopQmf->p_file_qmf->y_size;
		UINT32 nBand;

		// Range check region. Must be inside image, and number to extract can not be
		// larger than start/end region size. So if you want to read from [2,3] to [10,15],
		// then number_x must be <= (1 + 10 - 2) and number_y must be <= [1 + 15 - 3]
		if( nTopX > nBottomX || nLeftY > nRightY
			|| nBottomX >= x_size || nRightY >= y_size) {	/**[06]**/
			NCSMutexEnd(&pNCSEcwInfo->mutex);
			return(NCS_REGION_OUTSIDE_FILE);
		}
//		if(nSizeX > (1 + nBottomX - nTopX) || nSizeY > (1 + nRightY - nLeftY)) {/**[06]**/
//			NCSMutexEnd(&pNCSEcwInfo->mutex);
//			return(NCS_NO_SUPERSAMPLE);
//		}
		if(nSizeX == 0 || nSizeY == 0) {/**[06]**/
			NCSMutexEnd(&pNCSEcwInfo->mutex);
			return(NCS_ZERO_SIZE);
		}
		// Sanity check band list
		if( nBands > pNCSFileView->pNCSFile->pTopQmf->nr_bands ) {/**[06]**/
			NCSMutexEnd(&pNCSEcwInfo->mutex);
			return(NCS_TOO_MANY_BANDS);
		}
		for( nBand = 0; nBand < nBands; nBand++ ) {/**[06]**/
			if( pBandList[nBand] >= pNCSFileView->pNCSFile->pTopQmf->nr_bands ) {
				NCSMutexEnd(&pNCSEcwInfo->mutex);
				return(NCS_INVALID_BAND_NR);
			}
		}
	}

	if( pNCSFileView->pRefreshCallback && pNCSFileView->eCallbackState != NCSECW_VIEW_QUIET ) {
		// [04] if pending, and did complete a SetView recently,
		//		then flush the queue and start this Setview going
		//	Can only cancel the request here if not too far progressed
		if( pNCSFileView->eCallbackState == NCSECW_VIEW_SET || pNCSFileView->eCallbackState == NCSECW_VIEW_QUEUED ) {
			if( pNCSFileView->nPending >= NCSECW_HIGHMAX_SETVIEW_PENDING
			 ||	(pNCSFileView->nPending >= NCSECW_MAX_SETVIEW_PENDING
					&& pNCSFileView->nCancelled < NCSECW_MAX_SETVIEW_CANCELS) ) {
				// cancel this SetView and use the pending one instead.
				// If queued, we have to remove it from the queue
				if( pNCSFileView->eCallbackState == NCSECW_VIEW_QUEUED ) {
					int nQueue = 0;
					while( nQueue < pNCSEcwInfo->pIDWT->nQueueNumber ) {
						if( pNCSEcwInfo->pIDWT->ppNCSFileView[nQueue] == pNCSFileView ) {
							// We just set it to NULL - this is OK, and faster than copying
							// the array down. The IDWT thread can handle NULL SetViews
							pNCSEcwInfo->pIDWT->ppNCSFileView[nQueue] = NULL;
							break;
						}
						nQueue++;
					}
				}
				pNCSFileView->nCancelled += 1;
				pNCSFileView->eCallbackState = NCSECW_VIEW_QUIET;
			 }
		}

		// If still busy, then queue this request
		if( pNCSFileView->eCallbackState != NCSECW_VIEW_QUIET ) {
			/*
			**	Just make this request pending
			*/
			UINT32	nCopy = 0;
			pNCSFileView->nPending += 1;
			pNCSFileView->pending.nBands	= nBands;
			while( nCopy < nBands ) {
				pNCSFileView->pending.pBandList[nCopy] = pBandList[nCopy];
				nCopy++;
			}
			pNCSFileView->pending.nTopX	= nTopX;
			pNCSFileView->pending.nLeftY	= nLeftY;			// Top-Left in image coordinates
			pNCSFileView->pending.nBottomX	= nBottomX;
			pNCSFileView->pending.nRightY	= nRightY;			// Bottom-Left in image coordinates
			pNCSFileView->pending.nSizeX	= nSizeX;
			pNCSFileView->pending.nSizeY	= nSizeY;			// Size of window
			pNCSFileView->pending.fTopX		= fTopX;
			pNCSFileView->pending.fLeftY	= fLeftY;			// Top-Left in world coordinates
			pNCSFileView->pending.fBottomX	= fBottomX;
			pNCSFileView->pending.fRightY	= fRightY;			// Bottom-Right in world coordinates


			// [09] - added this NCScbmQueueIDWTCallback()
			if(pNCSFileView->pRefreshCallback && 
			   (pNCSFileView->eCallbackState == NCSECW_VIEW_SET) && 
			   (pNCSFileView->info.nBlocksAvailable != 0/**[10]**/)) {
				NCScbmQueueIDWTCallback(pNCSFileView, NCSECW_QUEUE_LIFO);
			}
			// [09] - end
			NCSMutexEnd(&pNCSEcwInfo->mutex);	

			if (!pNCSFileView->pNCSFile->bIsConnected) {	/**[12]**/
				if( (pNCSFileView->pNCSFile->nReconnectCount >= MAXRECONNECTCOUNT) || !pNCSEcwInfo->bEcwpReConnect ) {
					// The connection has been lost permantently
					return (NCS_CONNECTION_LOST);
				} else {
					// The connection has been lost but the client will try and reconnect
					return(NCS_SERVER_ERROR);
				}
			}
			else if(pNCSFileView->pNCSFile->bFileIOError) { // if read local block failed //[20]
				return(NCS_FILEIO_ERROR);
			}
			else										/**[12]**/
				return(NCS_SUCCESS);			/**[12]**/
		}
	}


	pOldQmfRegion = pNCSFileView->pQmfRegion;
	nOldCacheMethod = pNCSFileView->nCacheMethod;

	nNewCacheMethod = NCS_CACHE_VIEW;
#ifndef NCSJPC_ECW_SUPPORT /**[26]**/
	// decide on method used to cache image, based on current window size
	if( nSizeX > NCSECW_MAX_VIEW_SIZE_TO_CACHE || nSizeY > NCSECW_MAX_VIEW_SIZE_TO_CACHE ) {
		nNewCacheMethod = NCS_CACHE_DONT;
	}
#endif

	// If swapping from View to Non-view mode or vica-versa, must
	// flush the old blocks and close the old QMF structure first

	if( pOldQmfRegion && (nNewCacheMethod != nOldCacheMethod) ) {
		if( nOldCacheMethod == NCS_CACHE_VIEW )
			NCScbmFileViewRequestBlocks(pNCSFileView, pOldQmfRegion, NCSECW_BLOCK_CANCEL);
		erw_decompress_end_region(pOldQmfRegion);
		pOldQmfRegion = NULL;
	}

	// Request blocks for the new region

	pNCSFileView->info.nBands	= nBands;
	// [02] copy from client band list so that they can free it, and we don't have
	//		to track multiple band lists during multiple pending setviews
	{
		UINT32	nCopy = 0;
		while( nCopy < nBands ) {
			pNCSFileView->info.pBandList[nCopy] = pBandList[nCopy];
			nCopy++;
		}
	}
	pNCSFileView->info.nTopX		= nTopX;
	pNCSFileView->info.nLeftY	= nLeftY;			// Top-Left in image coordinates
	pNCSFileView->info.nBottomX	= nBottomX;
	pNCSFileView->info.nRightY	= nRightY;			// Bottom-Left in image coordinates
	pNCSFileView->info.nSizeX	= nSizeX;
	pNCSFileView->info.nSizeY	= nSizeY;			// Size of window
	pNCSFileView->info.fTopX	= fTopX;
	pNCSFileView->info.fLeftY	= fLeftY;			// Top-Left in world coordinates
	pNCSFileView->info.fBottomX	= fBottomX;
	pNCSFileView->info.fRightY	= fRightY;			// Bottom-Right in world coordinates
	pNCSFileView->info.nMissedBlocksDuringRead = 0;	// no blocks failed on read so far
	pNCSFileView->bIsRefreshView = FALSE;			// A new view
	pNCSFileView->bTriggerRefreshCallback = FALSE;
	pNCSFileView->eCallbackState = NCSECW_VIEW_SET;
	pNCSFileView->nPending = 0;						// nothing pending right now
	pNCSFileView->nCacheMethod = nNewCacheMethod;

	pNCSFileView->pQmfRegion = erw_decompress_start_region( 
				pNCSFileView->pNCSFile->pTopQmf,
				 nBands, pNCSFileView->info.pBandList,
				 nTopX, nLeftY, nBottomX, nRightY,
				 nSizeX, nSizeY);

	if( !pNCSFileView->pQmfRegion ) {
		if( pOldQmfRegion )
			erw_decompress_end_region(pOldQmfRegion);
		pNCSFileView->eCallbackState = NCSECW_VIEW_QUIET;	// error, so back to quiet state
		NCSMutexEnd(&pNCSEcwInfo->mutex);
		return(NCS_COULDNT_ALLOC_MEMORY);
	}

	pNCSFileView->pQmfRegion->pNCSFileView = pNCSFileView;

	// Now request blocks that cover the new view
	if( pNCSFileView->nCacheMethod == NCS_CACHE_VIEW )
		NCScbmFileViewRequestBlocks(pNCSFileView, pNCSFileView->pQmfRegion, NCSECW_BLOCK_REQUEST);

	// Now remove the old cache request count for the old view
	// Only VIEW types had block reads cached
	if( pOldQmfRegion && (nOldCacheMethod == NCS_CACHE_VIEW) )
		NCScbmFileViewRequestBlocks(pNCSFileView, pOldQmfRegion, NCSECW_BLOCK_CANCEL);

	// Now we can remove the old set view's QMF structures
	if( pOldQmfRegion )
		erw_decompress_end_region(pOldQmfRegion);

	_ASSERT( NCScbmSanityCheckFileView(pNCSFileView) );

	// Now, if there is a requirement to request or cancel blocks for the file, do this now
	// but only if there is not already a send in progress (which will automatically pick these up)
	if( !pNCSFileView->pNCSFile->bLocalFile /**[17]**/ && !pNCSFileView->pNCSFile->bSendInProgress
	 && (pNCSFileView->pNCSFile->nRequestsXmitPending || pNCSFileView->pNCSFile->nCancelsXmitPending) )
		NCScbmNetFileXmitRequests(NCS_SUCCESS, NULL, pNCSFileView->pNCSFile );

	_ASSERT(NCScbmSanityCheckFileView(pNCSFileView));

	// Last time a block was received for this file view. Initially set to time of File View
	// Also update last time a Set View was done, to assist cache purging logic
	pNCSFileView->tLastBlockTime = NCSGetTimeStampMs();

	NCScbmPurgeCache(pNCSFileView);	// see if we need to purge cache

	if( (pNCSFileView->nCacheMethod == NCS_CACHE_VIEW) && (pNCSFileView->pRefreshCallback) ) {
		if( (pNCSFileView->info.nBlocksAvailable == pNCSFileView->info.nBlocksInView)		// [02] all blocks are here
 		 || (pNCSFileView->info.nBlocksAvailable) ) {					// [02] view moved; some blocks are here
			NCScbmQueueIDWTCallback(pNCSFileView, NCSECW_QUEUE_LIFO);
		}
	}

	NCSMutexEnd(&pNCSEcwInfo->mutex);			// [02] moved mutex to prevent multiple callbacks
	if (pNCSFileView->pNCSFile->bLocalFile ) {
		if(pNCSFileView->pNCSFile->bFileIOError) { // if read local block failed //[20]
			return(NCS_FILEIO_ERROR);
		}
		else return(NCS_SUCCESS);
	}
	else if( !pNCSFileView->pNCSFile->bIsConnected ) {
		if( (pNCSFileView->pNCSFile->nReconnectCount >= MAXRECONNECTCOUNT) || !pNCSEcwInfo->bEcwpReConnect ) {
			// The connection has been lost permantently
			return (NCS_CONNECTION_LOST);
		} else {
			// The connection has been lost but the client will try and reconnect
			return(NCS_SERVER_ERROR);
		}
	}
	else										/**[08]**/
		return(NCS_SUCCESS);			/**[08]**/
}

#ifdef NCSJPC_ECW_SUPPORT
NCSError	NCScbmSetFileView_ECW(NCSFileView *pNCSFileView,
				UINT32 nBands,					// number of bands to read
				UINT32 *pBandList,				// index into actual band numbers from source file
				UINT32 nTopX, UINT32 nLeftY,	// Top-Left in image coordinates
				UINT32 nBottomX, UINT32 nRightY,// Bottom-Left in image coordinates
				UINT32 nSizeX, UINT32 nSizeY)	// Output view size in window pixels
#else
NCSError	NCScbmSetFileView(NCSFileView *pNCSFileView,
				UINT32 nBands,					// number of bands to read
				UINT32 *pBandList,				// index into actual band numbers from source file
				UINT32 nTopX, UINT32 nLeftY,	// Top-Left in image coordinates
				UINT32 nBottomX, UINT32 nRightY,// Bottom-Left in image coordinates
				UINT32 nSizeX, UINT32 nSizeY)	// Output view size in window pixels
#endif
{
	return(NCScbmSetFileViewEx(pNCSFileView,
							   nBands,
							   pBandList,
							   nTopX, nLeftY,
							   nBottomX, nRightY,
							   nSizeX, nSizeY,
							   nTopX, nLeftY,
							   nBottomX, nRightY));
}

/*******************************************************
**	NCScbmSanityCheckFileView() - debug Sanity check on block usage
**
**	returns FALSE if error, TRUE if all OK
**
********************************************************/

#ifdef _DEBUG
int	NCScbmSanityCheckFileView(NCSFileView *pNCSFileView)
{
	if(pNCSFileView) {
		NCSFileCachedBlock	*pBlock;
		NCSFile				*pNCSFile;
		UINT32				nCancelCount = 0, nRequestCount = 0;

		pNCSFile = pNCSFileView->pNCSFile;

		pBlock = pNCSFile->pFirstCachedBlock;
		while( pBlock ) {
			if(pBlock->bRequested && !pBlock->nUsageCount )
				nCancelCount += 1;
			if(!pBlock->bRequested && !pBlock->pPackedECWBlock && pBlock->nUsageCount )
				nRequestCount += 1;

			pBlock = pBlock->pNextCachedBlock;
		}
		_ASSERT( pNCSFile->nCancelsXmitPending == nCancelCount );
		_ASSERT( pNCSFile->nRequestsXmitPending == nRequestCount );
		if((pNCSFile->nCancelsXmitPending != nCancelCount) ||
			(pNCSFile->nRequestsXmitPending != nRequestCount)) {
			return(FALSE);
		}
	}
	return( TRUE );
}

#endif

/*******************************************************
**	NCScbmFileViewRequestBlocks() - Request blocks for the area
**	covered by the QMF region WHICH MAY BE DIFFERENT
**	from the QMF region pointed to by the File View.
**	If the eRequest == NCSECW_BLOCK_REQUEST, then REQUEST the blocks.
**	If the eRequest == NCSECW_BLOCK_CANCEL, then CANCEL the blocks.
**
**	If the eRequest == NCSECW_BLOCK_REQUEST, then updates Blocks in View and other values
********************************************************/
int NCScbmFileViewRequestBlocks(NCSFileView *pNCSFileView, QmfRegionStruct *pQmfRegion, NCSEcwBlockRequestMethod eRequest)
{
	UINT16					nLevel = 0;
	UINT32					nBlocks, nBlocksAvailable, nLevelBlocks;

	nBlocks = nBlocksAvailable = 0;
#ifdef LOG_REQUESTS
	FILE *pFile = fopen("C:\\Temp\\NCSClient.log", "a+");

	fprintf(pFile, "RequestBlocks(%s)\r\n", (eRequest == NCSECW_BLOCK_REQUEST) ? "Request" : ((eRequest == NCSECW_BLOCK_CANCEL) ? "Cancel" : "Unknown");
#endif
	// Traverse from smallest to largest level, working out blocks we need
	while( nLevel <= pQmfRegion->p_largest_qmf->level ) {
		QmfRegionLevelStruct	*pLevel = &(pQmfRegion->p_levels[nLevel]);
		QmfLevelStruct			*pQmf = pLevel->p_qmf;

		UINT32					nStartXBlock, nEndXBlock, nStartYBlock, nEndYBlock;
		UINT32					nRowsOfBlocks;

		nStartXBlock	= pLevel->start_x_block;
		nEndXBlock		= (nStartXBlock + pLevel->x_block_count) - 1;
		nStartYBlock	= pLevel->level_start_y / pQmf->y_block_size;
		nEndYBlock		= pLevel->level_end_y   / pQmf->y_block_size;
		nLevelBlocks = ((nEndXBlock - nStartXBlock)+1) * ((nEndYBlock - nStartYBlock)+1);

		// We now have the rectangle of blocks required for this level.
		// Loop through the rows of blocks, and request the blocks from the actual file
		nRowsOfBlocks = 1 + (nEndYBlock - nStartYBlock);
		while(nRowsOfBlocks--) {
			UINT32					nFileBlock;
			UINT32					nFileBlockCount;
			// Now compute block number in the file for this level's block

			nFileBlock = pQmf->nFirstBlockNumber +
							(pQmf->nr_x_blocks * nStartYBlock) + nStartXBlock;
			nFileBlockCount = 1 + (nEndXBlock - nStartXBlock);
			nStartYBlock += 1;		// move on to the next row

			while(nFileBlockCount--) {
				NCSFileCachedBlock	*pBlock;
				pBlock = NCScbmGetCacheBlock(pNCSFileView->pNCSFile, pNCSFileView->pNCSFile->pWorkingCachedBlock,
											nFileBlock, eRequest);
				pNCSFileView->pNCSFile->pWorkingCachedBlock = pBlock;
				_ASSERT( pBlock );
				if( !pBlock )
					return(1);		// Internal logic error
#ifdef LOG_REQUESTS
				fprintf(pFile, "Block %ld, Usage %ld, Requested %s\r\n", nFileBlock, pBlock->nUsageCount, pBlock->bRequested ? "True" : "False");
#endif
				if( eRequest == NCSECW_BLOCK_REQUEST && pBlock->pPackedECWBlock)
					nBlocksAvailable += 1;
				nFileBlock += 1;
			}
		}

		nBlocks += nLevelBlocks;
		nLevel++;
	}
#ifdef LOG_REQUESTS
	fclose(pFile);
#endif
	if( eRequest == NCSECW_BLOCK_REQUEST ) {
		pNCSFileView->info.nBlocksInView = nBlocks;
		pNCSFileView->info.nBlocksAvailableAtSetView = pNCSFileView->info.nBlocksAvailable = nBlocksAvailable;
	}
	return(0);
}

/*******************************************************
**	NCScbmGetCacheBlock() - add/remove/find block in cache list
**	NOTE:
**	(1)	This uses the pWorkingCachedBlock pointer in the NCSFile,
**		which assumes that calls into this routine happen in an ascending order
**		most of the time, and that blocks can never be freed while they are needed
**		in a current SetView (so the pWorkingCachedBlock is always valid)
**	(2)	Assumes that the caller has mutexed the list in the case of
**		NCSECW_BLOCK_CANCEL/NCSECW_BLOCK_REQUEST.
**	(3) In the case of NCSECW_BLOCK_RETURN, the list is NOT mutexed, as we
**		want high performance on reads. This works because items are inserted
**		into the list / deleted from the list in a way that preserves
**		the link structure at all times for reads, even outside a mutex.
**		FINAL NOTE: NOW HAVING TO MUTEX FOR NCSECW_BLOCK_RETURN AS WELL
**	(4) The workingpoint pointers are really hints only. They
**		are at the File not FileView level, so multiple file views, running
**		at the same time, introduce inefficiencies. This should not be too
**		significant because (a) at worst it is no worse than starting from the start
**		and (b) most of the accesses (except block reads) are mutexed and (c)
**		the request blocks from server logic have their own working pointer.
**  (5)	IMPORTANT: The FREE BLOCK logic has to note if the working pointer is pointing
**		to a block being deleted, and if so, it must move the working pointers to new,
**		valid blocks. This works OK because the FREE BLOCK from cache only ever removed
**		blocks from cache if they are not currently in use by a file view.
********************************************************/

NCSFileCachedBlock *NCScbmGetCacheBlock(NCSFile *pNCSFile, NCSFileCachedBlock *pWorkingCachedBlock,
											   NCSBlockId nBlock, NCSEcwBlockRequestMethod eRequest)
{
	NCSFileCachedBlock	*pBlock, *pPreviousBlock;

	// Find the block in the list, if there. Start looking from the pWorkingCached list for speed
	pBlock = pWorkingCachedBlock;

	if( pBlock ) {
		if( pBlock->nBlockNumber > nBlock )
			pBlock = pNCSFile->pFirstCachedBlock;	// gone too far in the list, so start again
	}
	else
		pBlock = pNCSFile->pFirstCachedBlock;

	// now scan the list looking for this block. We finish when:
	//(1)		The list is currently empty	(pBlock will be set to NULL)
	//(2)	or	The block has been found	(bBlock will point to the block, 
	//										 pPreviousBlock will point to previous block or NULL if start of the list)
	//(3)	or	The block is not there		(pBlock will point to the block before this one,
	//										 unless at start of list, in which case will be NULL)
	pPreviousBlock = NULL;

	while( pBlock ) {
		if(pBlock->nBlockNumber == nBlock )
			break;				// found the block
		if(pBlock->nBlockNumber > nBlock ) {
			if( pPreviousBlock ) {
				pBlock = pPreviousBlock;				// go to the previous block
				break;
			}
			else {
				pBlock = pNCSFile->pFirstCachedBlock;	// started too far down the list, so have to
														// wrap back to the start
				if( pBlock->nBlockNumber > nBlock ) {
					pBlock = NULL;						// if before start of list, return ptr as NULL
					break;
				}
				pPreviousBlock = NULL;					// else start beginning of list again
				continue;
			}
		}
		pPreviousBlock = pBlock;
		pBlock = pBlock->pNextCachedBlock;
		if( pBlock && pPreviousBlock->nBlockNumber >= pBlock->nBlockNumber )
			return(NULL);			// SERIOUS ERROR - corrupted memory structure

		if( !pBlock ) {
			pBlock = pPreviousBlock;
			break;					// got to the end, and nBlock is > than largest block in the list
		}
	}

	// now process the type of request (return pointer, add block to cache, cancel block from cache)
	switch( eRequest ) {
	case NCSECW_BLOCK_RETURN :
			if( pBlock ) {
				if( pBlock->nBlockNumber == nBlock ) {
					pBlock->nHitCount += 1;
					return(pBlock);
				}
				else
					return( NULL );					// ERROR - we could not find the requested block
			}
			return(NULL);
		break;
	case NCSECW_BLOCK_CANCEL :
		if( !pBlock )
			return( NULL );							// ERROR - should never happen
		if( pBlock->nBlockNumber != nBlock )
			return( NULL );							// ERROR - should never happen

		_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

		// Decrement usage count
		pBlock->nUsageCount -= 1;
		if( pBlock->nUsageCount == 0) {
			// indicate we have a block request to cancel. We leave the bRequested
			// set (the CANCEL packet will unset it) to flag this block as one that
			// was requested, that we now want to cancel the request for
			if( pBlock->bRequested ) {
				NCSEcwStatsLock();
				
				NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nCancelsXmitPending, 1);
				pNCSFile->nCancelsXmitPending += 1;
				
				NCSEcwStatsUnLock();
			}
			else if( !pBlock->pPackedECWBlock ) {
				NCSEcwStatsLock();

				pNCSFile->nRequestsXmitPending -= 1;			// No longer need the packet
				NCSEcwStatsDecrement(&pNCSEcwInfo->pStatistics->nRequestsXmitPending, 1);

				_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

				NCSEcwStatsUnLock();
			}
		}
		_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));
		return(pBlock);
		break;
	case NCSECW_BLOCK_REQUEST : {
			NCSFileCachedBlock *pNewBlock;

			_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

			if( pBlock ) {
				if( pBlock->nBlockNumber == nBlock ) {
					// block is already in the list. Just increment usage count
					NCSEcwStatsLock();

					NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nSetViewBlocksCacheHits, 1);
					
					pBlock->nUsageCount += 1;
					// If usage gone to 1, and no block in memory, and request not
					// already in progress, request it by incrementing Xmit pending requests by 1
					// There also exists the situation where (1) a request went out (2) the
					// request was flagged as to be canceled (3) the block is re-requested before
					// the cancel could be xmitted. In this case, we have to decrement the cancel
					// request count.
					if( (pBlock->nUsageCount == 1) && !pBlock->pPackedECWBlock ) {
						if( pBlock->bRequested ) {
							pNCSFile->nCancelsXmitPending -= 1;		// cancel the cancel operation
							NCSEcwStatsDecrement(&pNCSEcwInfo->pStatistics->nCancelsXmitPending, 1);
						}
						else {
							pNCSFile->nRequestsXmitPending += 1;	// request the packet
							NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nRequestsXmitPending, 1);
						}
					}
					NCSEcwStatsUnLock();
					_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

					return(pBlock);
				}
			}
			_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

			// else allocate a new block
			pNewBlock = NCSPoolAlloc(pNCSFile->pBlockCachePool, TRUE);
			if( !pNewBlock )
				return( NULL );						// ERROR - out of memory
			pNewBlock->nBlockNumber = nBlock;
			pNewBlock->nUsageCount = 1;
			pNewBlock->bRequested = FALSE;
			// make sure we add the item carefully, so READS won't break without Mutex's
			if( pBlock ) {
				pNewBlock->pNextCachedBlock = pBlock->pNextCachedBlock;
				pBlock->pNextCachedBlock = pNewBlock;
			}
			else {
					// insert at start of list. If list is NULL, sets pNextCachedBlock to NULL
					pNewBlock->pNextCachedBlock = pNCSFile->pFirstCachedBlock;
					pNCSFile->pFirstCachedBlock = pNewBlock;
			}
			if( pNCSFile->bLocalFile ) {
				UINT8 *pPackedECWBlock;
				UINT32 nPackedECWBlockLength;

				pPackedECWBlock = NCScbmReadFileBlockLocal(pNCSFile, nBlock, &nPackedECWBlockLength );
				if( pPackedECWBlock && nPackedECWBlockLength != 0 && align_ecw_block(pNCSFile, nBlock, 
																				   &pNewBlock->pPackedECWBlock, 
																				   &pNewBlock->nPackedECWBlockLength, 
																				   pPackedECWBlock, 
																				   nPackedECWBlockLength) == 0) {
					NCSEcwStatsLock();
					//if( pNCSEcwInfo->pStatistics )
					NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nPackedBlocksCacheSize, pNewBlock->nPackedECWBlockLength);
					NCSEcwStatsUnLock();
				}
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
				DisposeHandle( pPackedECWBlock);
#else
				NCSFree(pPackedECWBlock);//[19]
#endif

				if(nPackedECWBlockLength != 0) {
					pNewBlock->pUnpackedECWBlock = NULL;
					pNewBlock->nUnpackedECWBlockLength = 0;
				} else {
					NCSPoolFree(pNCSFile->pBlockCachePool, pNewBlock);
					pNewBlock = NULL;
				}
			}
			else {
				NCSEcwStatsLock();

				pNewBlock->pPackedECWBlock = NULL;
				pNewBlock->pUnpackedECWBlock = NULL;
				pNewBlock->nPackedECWBlockLength = 0;
				pNewBlock->nUnpackedECWBlockLength = 0;
				// request the block at some stage in the future by incrementing Requests Pending
				// count
				pNCSFile->nRequestsXmitPending += 1;
				NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nRequestsXmitPending, 1);

				_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

				NCSEcwStatsUnLock();
			}
			NCSEcwStatsLock();
			NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nSetViewBlocksCacheMisses, 1);
			NCSEcwStatsUnLock();
			_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

			return(pNewBlock);
		}
		break;
	}

	return( NULL);
}

/*******************************************************
**	NCScbmReadViewBlock()	- Read a block for the view
**
**	Notes:
**	-	If block is not available, and view cachemode is
**		NCS_CACHE_VIEW, a fake "zero" block is returned,
**		otherwise reading will wait (blocking IO) until
**		the block becomes available.
**	-	If error, a NULL pointer is returned
**	-	You MUST call NCScbmFreeViewBlock() to finish with
**		using the block - don't try freeing it yourself!
********************************************************/
UINT8	*NCScbmReadViewBlock(QmfRegionLevelStruct	*pQmfRegionLevel,
					  UINT32 nBlockX, UINT32 nBlockY)
{
	UINT32	nBlock;
	UINT8	*pECWBlock = NULL;

	QmfRegionStruct *pRegion = pQmfRegionLevel->p_region;
	QmfLevelStruct	*pQmfLevel = pQmfRegionLevel->p_qmf;
	NCSFileView		*pNCSFileView = pRegion->pNCSFileView;
	NCSFile			*pNCSFile = pNCSFileView->pNCSFile;
	NCSFileCachedBlock	*pNCSBlock;

	nBlock = pQmfLevel->nFirstBlockNumber + (pQmfLevel->nr_x_blocks * nBlockY) + nBlockX;

	// Reading from cache.
	// WARNING!! There could be a problem with this code given it allocates
	// a new ECW block, if multiple threads are running through the file at the
	// same time.  This code most likely has to be mutexed. The WorkingCachedBlock
	// is OK - it is intended as a hint pointer, and multiple threads will make
	// it not as efficient anyway. The main problem is the allocation of the packed block.
	// For this reason, despite MUTEX's being slow, we have to mutex the block read.
	// Sigh.  A possible improvement might be to only MUTEX if there is more than file view
	// currently open on the file.
	// [03] Latest version: now only mutex's if adding an unpacked block

	if( pNCSFileView->nCacheMethod == NCS_CACHE_VIEW ) {
		UINT32 nReadUnpackedBlocksCacheHits = 0;
		UINT32 nUnpackedBlocksCacheSize = 0;
	//	QmfLevelStruct	*pParentQmfLevel = pQmfLevel->p_smaller_qmf;
	//	INT32 nParentBlockX = nBlockX / 2;
	//	INT32 nParentBlockY = nBlockY / 2;

#ifdef DO_READ_MUTEX
		NCSMutexBegin(&pNCSEcwInfo->mutex);		// have to MUTEX because of the possible unpacked memory allocation
#endif
		pNCSBlock = pNCSFile->pWorkingCachedBlock = 
			NCScbmGetCacheBlock(pNCSFileView->pNCSFile,
								pNCSFile->pWorkingCachedBlock, nBlock, NCSECW_BLOCK_RETURN);
#ifdef NOTDEF
//_DEBUG
		if(pNCSBlock && (pNCSBlock->pPackedECWBlock || pNCSBlock->pUnpackedECWBlock)) {
			while(pParentQmfLevel) {	/**[11]*/
				/*
				** Check to make sure all "parent" blocks to this one exist,
				** before returning this block.  This is because in some 
				** instances it's possible for blocks to be returned from the
				** server (ECWP:) out-of-order, so a "parent" block may be missing.
				** If included, this can cause an "edge" effect (high-pass filter).
				*/
				NCSFileCachedBlock	*pParentNCSBlock;
				UINT32	nParentBlock = pParentQmfLevel->nFirstBlockNumber + (pParentQmfLevel->nr_x_blocks * nParentBlockY) + nParentBlockX;
					
				pParentNCSBlock = NCScbmGetCacheBlock(pNCSFileView->pNCSFile, pNCSFile->pWorkingCachedBlock, nParentBlock, NCSECW_BLOCK_RETURN);

				if(pParentNCSBlock && 
				   (pParentNCSBlock->pPackedECWBlock || pParentNCSBlock->pUnpackedECWBlock) && 
				   (pParentNCSBlock->nDecodeMissID != pNCSFileView->nNextDecodeMissID)) {
				
					nParentBlockX = nParentBlockX / 2;
					nParentBlockY = nParentBlockY / 2;
					pParentQmfLevel = pParentQmfLevel->p_smaller_qmf;
				} else {
					if(pParentNCSBlock && pParentNCSBlock->nUsageCount) {
						if(pParentNCSBlock->pPackedECWBlock || pParentNCSBlock->pUnpackedECWBlock) {
							if(!pParentNCSBlock->bRequested) {
								MessageBox(NULL, "NOT REQUESTED BUT HAVE IT", "DEBUG", MB_OK);
							} else {
								MessageBox(NULL, "REQUESTED BUT HAVE IT", "DEBUG", MB_OK);
							}
						} else {
							if(!pParentNCSBlock->bRequested) {
								MessageBox(NULL, "NOT REQUESTED AND DON'T HAVE IT", "DEBUG", MB_OK);
							} else {
								MessageBox(NULL, "REQUESTED AND DON'T HAVE IT", "DEBUG", MB_OK);
							}						
						}
						if(pNCSFileView->pNCSFile->nRequestsXmitPending) {
							char buf[1024];
							sprintf(buf, "%ld PENDING", pNCSFileView->pNCSFile->nRequestsXmitPending);
							MessageBox(NULL, buf, "DEBUG", MB_OK);
						}
						if(!NCScbmSanityCheckFileView(pNCSFileView->pNCSFile->pNCSFileViewList)) {
							MessageBox(NULL, "FileView is INSANE!", "DEBUG", MB_OK);
						}
					}
					break;
				}
			}
		}
#endif
#ifdef USE_PARENT_CHECK
		while(pParentQmfLevel) {	/**[11]*/
			/*
			** Check to make sure all "parent" blocks to this one exist,
			** before returning this block.  This is because in some 
			** instances it's possible for blocks to be returned from the
			** server (ECWP:) out-of-order, so a "parent" block may be missing.
			** If included, this can cause an "edge" effect (high-pass filter).
			*/
			NCSFileCachedBlock	*pParentNCSBlock;
			UINT32	nParentBlock = pParentQmfLevel->nFirstBlockNumber + (pParentQmfLevel->nr_x_blocks * nParentBlockY) + nParentBlockX;
				
			pParentNCSBlock = NCScbmGetCacheBlock(pNCSFileView->pNCSFile, pNCSFile->pWorkingCachedBlock, nParentBlock, NCSECW_BLOCK_RETURN);

			if(pParentNCSBlock && 
			   (pParentNCSBlock->pPackedECWBlock || pParentNCSBlock->pUnpackedECWBlock) && 
			   (pParentNCSBlock->nDecodeMissID != pNCSFileView->nNextDecodeMissID)) {
			
				nParentBlockX = nParentBlockX / 2;
				nParentBlockY = nParentBlockY / 2;
				pParentQmfLevel = pParentQmfLevel->p_smaller_qmf;
			} else {
				if(pParentNCSBlock) {
					if((pParentNCSBlock->pPackedECWBlock || pParentNCSBlock->pUnpackedECWBlock) &&	/**[13]**/
						(pParentNCSBlock->nDecodeMissID == pNCSFileView->nNextDecodeMissID)) {		/**[13]**/
						NCScbmQueueIDWTCallback(pNCSFileView, NCSECW_QUEUE_LIFO);					/**[13]**/
					}																				/**[13]**/
					pParentNCSBlock->nDecodeMissID = pNCSFileView->nNextDecodeMissID;
				}
#ifdef DO_READ_MUTEX
				NCSMutexEnd(&pNCSEcwInfo->mutex);
#endif
				if( pQmfLevel->level )
					return( pNCSFileView->pNCSFile->pLevelnZeroBlock );
				else
					return( pNCSFileView->pNCSFile->pLevel0ZeroBlock );
			}
		}
#endif	/* USE_PARENT_CHECK */

		if( !pNCSBlock ) {
			// ERROR - internal logic has broken, as this should never happen
			// So just try and return a zero-block instead
			NCSEcwStatsLock();
			NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nReadBlocksCacheMisses, 1);
			NCSEcwStatsUnLock();

			pNCSFileView->info.nMissedBlocksDuringRead += 1;		// [02]
#ifdef DO_READ_MUTEX
			NCSMutexEnd(&pNCSEcwInfo->mutex);
#endif
			_ASSERT( pNCSBlock );
			if( pQmfLevel->level )
				return( pNCSFileView->pNCSFile->pLevelnZeroBlock );
			else
				return( pNCSFileView->pNCSFile->pLevel0ZeroBlock );
//			return( NULL );			// ERROR - internal logic has broken, as this should never happen
		}

		// If there is no block available, and no callback has been set, we
		// wait until the block comes in. We have to go in and out of the MUTEX while this
		// is happening, to ensure that the rest of the world gets a look in
		if( !pNCSFileView->pRefreshCallback && !pNCSBlock->pPackedECWBlock ) {
#ifdef DO_READ_MUTEX
			NCSMutexEnd(&pNCSEcwInfo->mutex);		// Out of MUTEX while waiting for the block
#endif
			while( !pNCSBlock->pPackedECWBlock ) {
				NCSTimeStampMs	tNow  = NCSGetTimeStampMs();

				// If the view is being shut down, just return zero blocks.
				// The higher level will return the view state to QUIET once the line has been
				// processed
				if( pNCSFileView->bGoToQuietState ) {
					if( pQmfLevel->level )
						return( pNCSFileView->pNCSFile->pLevelnZeroBlock );
					else
						return( pNCSFileView->pNCSFile->pLevel0ZeroBlock );
				}

				// Note: nBlockingTime == -1 is INDEFINITE delay.
				if( (tNow < pNCSFileView->tLastBlockTime)	// ms timer wrapped
					|| ((pNCSEcwInfo->pStatistics->nBlockingTime != 0xffffffff) && (tNow > (pNCSFileView->tLastBlockTime + pNCSEcwInfo->pStatistics->nBlockingTime)))) {
					break;			// Too much time has passed, so quit waiting
				}
				NCSSleep(500);			// Wait for a while to see if this (and possibly other) blocks turn up
			}
#ifdef DO_READ_MUTEX
			NCSMutexBegin(&pNCSEcwInfo->mutex);		// Back in to MUTEX
#endif
		}

#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
		pECWBlock = pNCSBlock->pUnpackedECWBlock ? *pNCSBlock->pUnpackedECWBlock : NULL;
#else
		pECWBlock = pNCSBlock->pUnpackedECWBlock;
#endif
		if( pECWBlock ) {
				// unpacked block cache hit
			nReadUnpackedBlocksCacheHits += 1;
		}
		else {
			// Unpack the block into cache. The unpack might fail, in which case we still
			// use the packed block. First, we make sure our cache size has not grown too large -
			// if so, we use the packed block size and don't attempt an unpack

#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
			pECWBlock = pNCSBlock->pPackedECWBlock ? *pNCSBlock->pPackedECWBlock : NULL;		 // [02] try the unpack if the block is there
			// Force a garbage collection?
			NCSPhysicalMemorySize();
#else
			pECWBlock = pNCSBlock->pPackedECWBlock;		 // [02] try the unpack if the block is there
#endif
			/** [24] Change to caching logic, see inequality below */
			if( pECWBlock && (pNCSEcwInfo->pStatistics->nUnpackedBlocksCacheSize + pNCSEcwInfo->pStatistics->nPackedBlocksCacheSize)
				   < pNCSEcwInfo->pStatistics->nMaximumCacheSize /** [24] ((pNCSEcwInfo->pStatistics->nMaximumCacheSize / 3)*2)*/) {
				// we have a packed block, and enough RAM to be OK to unpack it
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
				Handle pUnpackedECWBlock = NULL;
#else
				UINT8	*pUnpackedECWBlock = NULL;
#endif
				UINT32	nUnpackedLength = 0;

				// [03] now ONLY mutex when adding unpacked blocks. We now mutex here, and
				// [03] then if the unpacked block is still not there, we can add it in,
				// [03] otherwise just grab the unpacked block that popped up since we did the
				// [03] unpacked block test a few lines ago

				NCSMutexBegin(&pNCSEcwInfo->mutex);				// [03] mutex for the unpack
				if( pNCSBlock->pUnpackedECWBlock ) {			// [03]
					// [03] the unpacked block came into being after our out-of-mutex test, so use it
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
					pECWBlock = pNCSBlock->pUnpackedECWBlock ? *pNCSBlock->pUnpackedECWBlock : NULL;
#else
					pECWBlock = pNCSBlock->pUnpackedECWBlock;	// [03]
#endif
					nReadUnpackedBlocksCacheHits += 1;
				}												// [03]
				else {
#ifdef WIN32		// [16]
					static char msg[16384] = { '\0' };

					__try {
#endif //WIN32					
						if( unpack_ecw_block(pQmfLevel, nBlockX, nBlockY, &pUnpackedECWBlock, &nUnpackedLength,pECWBlock) == 0 ) {
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
							pECWBlock = pUnpackedECWBlock ? *pUnpackedECWBlock : NULL;
#else
							pECWBlock = pUnpackedECWBlock;
#endif
							pNCSBlock->pUnpackedECWBlock = pUnpackedECWBlock;
							pNCSBlock->nUnpackedECWBlockLength = nUnpackedLength;
							nUnpackedBlocksCacheSize += nUnpackedLength;
						}
#ifdef WIN32		// [16]
					} __except (pNCSFile->bIsCorrupt ? EXCEPTION_EXECUTE_HANDLER : NCSDbgGetExceptionInfoMsg(_exception_info(), msg)) {
						pECWBlock = NULL;
						pNCSBlock->pUnpackedECWBlock = NULL;
						pNCSBlock->nUnpackedECWBlockLength = 0;

						if(!pNCSFile->bIsCorrupt) {
#if !defined(_WIN32_WCE)
							char extended_msg[16384] = { '\0' };
							sprintf(extended_msg,
								"(ncscbm) Version : %s\n%s",
								NCS_VERSION_STRING,msg);
							NCSLog(LOG_LOW, extended_msg);//, NCS_FILE_INVALID, (char*)NCSGetErrorText(NCS_FILE_INVALID));
#else
							char *pExtMsg = (char *) NCSMalloc(strlen(msg) + strlen(NCS_VERSION_STRING) + 50, FALSE);
							if (pExtMsg)
							{
								sprintf(pExtMsg,
									"(ncscbm) Version : %s\n%s",
									NCS_VERSION_STRING,msg);
								NCSLog(LOG_LOW, pExtMsg);//, NCS_FILE_INVALID, (char*)NCSGetErrorText(NCS_FILE_INVALID));
								NCSFree(pExtMsg);
							}
#endif
							pNCSFile->bIsCorrupt = TRUE;
						}
					}
#endif //WIN32					
			}
				NCSMutexEnd(&pNCSEcwInfo->mutex);		// [03] end unpack mutex
			}
		}

		// now safe to quit the MUTEX
#ifdef DO_READ_MUTEX
		NCSMutexEnd(&pNCSEcwInfo->mutex);
#endif
		
		if( pECWBlock ) {
			NCSEcwStatsLock();
			// there was a block already loaded
			NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nReadBlocksCacheHits, 1);
			NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nReadUnpackedBlocksCacheHits, nReadUnpackedBlocksCacheHits);
			NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nUnpackedBlocksCacheSize, nUnpackedBlocksCacheSize);

			NCSEcwStatsUnLock();
		}
		else {
			NCSEcwStatsLock();
			// If no ECW block loaded yet, return a fake ZERO block
			NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nReadBlocksCacheMisses, 1);
			NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nReadUnpackedBlocksCacheHits, nReadUnpackedBlocksCacheHits);
			NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nUnpackedBlocksCacheSize, nUnpackedBlocksCacheSize);
			NCSEcwStatsUnLock();

			pNCSFileView->info.nMissedBlocksDuringRead += 1;			// [02]
			if( pQmfLevel->level )
				return( pNCSFileView->pNCSFile->pLevelnZeroBlock );
			else
				return( pNCSFileView->pNCSFile->pLevel0ZeroBlock );
		}
		return(pECWBlock);
	}

	// Direct reading, in non-VIEW mode (e.g. non-cached)
	// FIXME!! Currently only VIEW (cached) mode supported for non-local files
	if( !pNCSFile->bLocalFile )
		return( NULL );			// FIXME!! Not implemented yet for served files

	NCSEcwStatsLock();
	// Read block directly from local file, not cache
	NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nReadBlocksCacheBypass, 1);
	NCSEcwStatsUnLock();

	{
		UINT32	nBlockLength = 0;
		UINT8 *pPackedBlock = NCScbmReadFileBlockLocal(pNCSFileView->pNCSFile, nBlock, &nBlockLength );
		UINT8 *pAlignedBlock = (UINT8*)NULL;
		UINT32 nAlignedLength = 0;

		if(pPackedBlock && nBlockLength/**[23]**/) {
			align_ecw_block(pNCSFile, nBlock, &pAlignedBlock, &nAlignedLength, pPackedBlock, nBlockLength);
		
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
			DisposeHandle(pPackedBlock);
#else
			NCSFree(pPackedBlock);
#endif
		}
		return(pAlignedBlock);
//		return( NCScbmReadFileBlockLocal(pNCSFileView->pNCSFile, nBlock, &nBlockLength ));
	}
}

/*******************************************************
**	NCScbmFreeViewBlock() - Request freeing a block.
**	This routine will ONLY free the block if the current
**	NCSFileView is DONT cache, in which case the block
**	is immediatelly freed. Otherwise, it is not freed -
**	the SetView() logic will later free the block at the
**	termination of the SetView() if the overall block
**	usage has gone to zero.
**
**	IMPORTANT!! This is the ECWBlock being freed here,
**	not the pNCSBlock, which can potentially point to
**	a packed *and* an unpacked ECW block. This all works
**	because this free routine is really only here for
**	freeing non-cached blocks - cached blocks never get
**	free for a view until that view shuts down.
********************************************************/

void NCScbmFreeViewBlock(QmfRegionLevelStruct	*pQmfRegionLevel, UINT8 *pECWBlock)
{

	QmfRegionStruct *pRegion = pQmfRegionLevel->p_region;
	NCSFileView		*pNCSFileView = pRegion->pNCSFileView;

	if( pECWBlock ) {
		if( !pNCSFileView )
			NCSFree(pECWBlock);		// should never happen, but just in case someone uses the old ECW calls
		else if( pNCSFileView->nCacheMethod == NCS_CACHE_DONT )
			NCSFree(pECWBlock);		// the ECW block bypassed the cache system, so free it
	}
}

/*******************************************************
**	NCScbmReadFileBlockLocal()	- Read a physical block for the file
**
**	Notes:
**	-	Always reads the requested block from the local file
********************************************************/
#ifdef NCSJPC_ECW_SUPPORT
#ifdef MACINTOSH
Handle NCScbmReadFileBlockLocal_ECW(NCSFile *pNCSFile, NCSBlockId nBlock, UINT32 *pBlockLength )
#else
UINT8	*NCScbmReadFileBlockLocal_ECW(NCSFile *pNCSFile, NCSBlockId nBlock, UINT32 *pBlockLength )
#endif
#else
#ifdef MACINTOSH
Handle NCScbmReadFileBlockLocal(NCSFile *pNCSFile, NCSBlockId nBlock, UINT32 *pBlockLength )
#else
UINT8	*NCScbmReadFileBlockLocal(NCSFile *pNCSFile, NCSBlockId nBlock, UINT32 *pBlockLength )
#endif
#endif
{
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
	Handle	pECWBlock = NULL;
	OSErr   osErr;
#else
	UINT8	*pECWBlock = NULL;
#endif
	UINT64	offset = 0;
	UINT32	length = 0;

	NCSMutexBegin(&pNCSFile->mFileAccess); //[21]

	if(NCScbmGetFileBlockSizeLocal(pNCSFile, nBlock, &length, &offset)) {
		QmfLevelStruct	*pTopQmf = pNCSFile->pTopQmf;		// we always go relative to the top level
		UINT32  nPaddedLength = 1;
#ifdef POSIX
		/**[25]**/
		// align nPaddedLength on a word boundary
		// change submitted by Bill Binko 
		if (length % 4 == 0)
		{
			nPaddedLength = length;
		}
		else
		{
			nPaddedLength = (((int)(length/4)) + 1)*4;
		}
		*pBlockLength = length;
#else		
		while(nPaddedLength < length) {
			nPaddedLength *= 2;
		}
		*pBlockLength = length;
#endif

#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
		pECWBlock = TempNewHandle( nPaddedLength, &osErr );
#else
		pECWBlock = NCSMalloc((UINT32)nPaddedLength /*length*/, FALSE);
#endif

		if( !pECWBlock ) {
			NCSMutexEnd(&pNCSFile->mFileAccess); //[21]
			return( NULL ); 
		}

		if( EcwFileSetPos(pTopQmf->hEcwFile, offset) ) {
			//[20] start...
#ifdef WIN32
			DWORD dError = GetLastError();
			*pBlockLength = dError;
			if( dError == ERROR_ADAP_HDW_ERR || dError == ERROR_NETNAME_DELETED || dError == ERROR_UNEXP_NET_ERR ) {
				pNCSFile->bFileIOError = TRUE;
				NCSLog( LOG_MED, "NCScbmReadFileBlockLocal() EcwFileSetPos, File IO Error %d.", dError );
			}
#else
			pNCSFile->bFileIOError = TRUE;
#endif //WIN32
			//[20] ...end
			if( pECWBlock )
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
				DisposeHandle( pECWBlock );
#else
				NCSFree(pECWBlock);//[19]
#endif
			NCSMutexEnd(&pNCSFile->mFileAccess); //[21]
			return( NULL );
		}

#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
		if( EcwFileRead(pTopQmf->hEcwFile, *pECWBlock, length) ) {
#else
		if( EcwFileRead(pTopQmf->hEcwFile, pECWBlock, length) ) {
#endif
			//[20] start...
#ifdef WIN32
			DWORD dError = GetLastError();
			*pBlockLength = dError;
			if( dError == ERROR_ADAP_HDW_ERR || dError == ERROR_NETNAME_DELETED || dError == ERROR_UNEXP_NET_ERR ) {
				pNCSFile->bFileIOError = TRUE;
				NCSLog( LOG_MED, "NCScbmReadFileBlockLocal() EcwFileRead, File IO Error %d.", dError );
			}
#else
			pNCSFile->bFileIOError = TRUE;
#endif //WIN32
			//[20] ...end
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
				DisposeHandle( pECWBlock );
#else
				NCSFree(pECWBlock);//[19]
#endif
	//		ERS_setup_error(ERS_RASTER_ERROR,"\nNCScbmReadViewBlock: error reading QMF block");
			NCSMutexEnd(&pNCSFile->mFileAccess); //[21]
			return(NULL);
		}
	} else {
		//[20] start...
#ifdef WIN32
		DWORD dError = GetLastError();
		*pBlockLength = dError;
		NCSLog( LOG_LOW, "NCScbmReadFileBlockLocal() File IO Error %d.", dError );
#endif //WIN32
		//[20] ...end
	}

	NCSMutexEnd(&pNCSFile->mFileAccess); //[21]
	return(pECWBlock);
}

#ifdef NCSJPC_ECW_SUPPORT
BOOLEAN NCScbmGetFileBlockSizeLocal_ECW(NCSFile *pNCSFile, NCSBlockId nBlock, UINT32 *pBlockLength, UINT64 *pBlockOffset )
#else
BOOLEAN NCScbmGetFileBlockSizeLocal(NCSFile *pNCSFile, NCSBlockId nBlock, UINT32 *pBlockLength, UINT64 *pBlockOffset )
#endif
{
	UINT64	offset = 0;
	UINT32	length = 0;

	QmfLevelStruct	*pTopQmf = pNCSFile->pTopQmf;		// we always go relative to the top level

	NCSMutexBegin(&pNCSFile->mFileAccess); //[21]

	// Read block directly from local file, not cache
	if(pTopQmf->p_block_offsets) {
		UINT64	*p_block_offset = pTopQmf->p_block_offsets + nBlock;	// will go up to higher levels if need be
		offset = *p_block_offset++;							// get offset to block
		length = (UINT32) (*p_block_offset - offset);		// get length
		offset += pTopQmf->file_offset;// add offset to first block in file
	} else if(pTopQmf->bRawBlockTable) {
		INT32 i;
		
		for(i = 0; i < (INT32)pNCSFile->nOffsetCache; i++) {
			if(pNCSFile->pOffsetCache[i].nID == nBlock) {
				pNCSFile->pOffsetCache[i].tsLastUsed = NCSGetTimeStampMs();
				length = pNCSFile->pOffsetCache[i].nLength;
				offset = pNCSFile->pOffsetCache[i].nOffset;
				break;
			}
		}
		if(length == 0 || offset == 0) {
			UINT64 offset2;
			EcwFileSetPos(pTopQmf->hEcwFile, pTopQmf->nHeaderMemImageLen + sizeof(UINT32) + sizeof(UINT8) + sizeof(UINT64) * nBlock);
		//[22]	NCSFileReadUINT64_LSB(pTopQmf->hEcwFile.hFile, &offset);
		//[22]	NCSFileReadUINT64_LSB(pTopQmf->hEcwFile.hFile, &offset2);
			EcwFileRead(pTopQmf->hEcwFile, &offset, sizeof(offset));		//[22]
			EcwFileRead(pTopQmf->hEcwFile, &offset2, sizeof(offset2));		//[22]
#ifdef NCSBO_MSBFIRST														//[22]
			offset = NCSByteSwap64(offset);									//[22]
			offset2 = NCSByteSwap64(offset2);								//[22]
#endif																		//[22]
			length = (UINT32)(offset2 - offset);
			offset += pTopQmf->file_offset;

			if(pNCSFile->nOffsetCache < pNCSEcwInfo->nMaxOffsetCache) {
				NCSFileBlockOffsetEntry Entry;
				Entry.nID = nBlock;
				Entry.nLength = length;
				Entry.nOffset = offset;
				Entry.tsLastUsed = NCSGetTimeStampMs();
				NCSArrayAppendElement(pNCSFile->pOffsetCache, pNCSFile->nOffsetCache, &Entry);
			} else {
				INT32 nLFU = 0;
				for(i = 0; i < (INT32)pNCSFile->nOffsetCache; i++) {
					if((pNCSFile->pOffsetCache[i].tsLastUsed < pNCSFile->pOffsetCache[nLFU].tsLastUsed) ||
					   ((pNCSFile->pOffsetCache[i].tsLastUsed == pNCSFile->pOffsetCache[nLFU].tsLastUsed) && 
						(pNCSFile->pOffsetCache[i].nID > pNCSFile->pOffsetCache[nLFU].nID))) {
						nLFU = i;
					}
				}
				pNCSFile->pOffsetCache[nLFU].nID = nBlock;
				pNCSFile->pOffsetCache[nLFU].nLength = length;
				pNCSFile->pOffsetCache[nLFU].nOffset = offset;
				pNCSFile->pOffsetCache[nLFU].tsLastUsed = NCSGetTimeStampMs();
			}
		}
	}
	if(pBlockLength) {
		*pBlockLength = length;
	}
	if(pBlockOffset) {
		*pBlockOffset = offset;
	}

	NCSMutexEnd(&pNCSFile->mFileAccess); //[21]

	return(TRUE);
}


/*******************************************************
**	NCScbmReadViewLineBIL() - read a block from an ECW file in BIL format (UINT8)
** Returns NCSECW_READ_OK if no error on the read.
********************************************************/
#ifdef NCSJPC_ECW_SUPPORT
NCSEcwReadStatus NCScbmReadViewLineBIL_ECW( NCSFileView *pNCSFileView, UINT8 **p_p_output_line)
#else
NCSEcwReadStatus NCScbmReadViewLineBIL( NCSFileView *pNCSFileView, UINT8 **p_p_output_line)
#endif
{
	if( pNCSFileView->bGoToQuietState ) {
		pNCSFileView->eCallbackState = NCSECW_VIEW_QUIET;
		return(NCSECW_READ_CANCELLED);
	}
	if( erw_decompress_read_region_line_bil(pNCSFileView->pQmfRegion, p_p_output_line, NCSECW_READLINE_UINT8) )	// [14]
		return(NCSECW_READ_FAILED);
	else
		return(NCSECW_READ_OK);
}

/*******************************************************
**	NCScbmReadViewLineBIL() - read a block from an ECW file in BIL format (UINT8)
** Returns NCSECW_READ_OK if no error on the read.
********************************************************/
#ifdef NCSJPC_ECW_SUPPORT
NCSEcwReadStatus NCScbmReadViewLineBILEx_ECW( NCSFileView *pNCSFileView, NCSEcwCellType eType, void **p_p_output_line)
#else
NCSEcwReadStatus NCScbmReadViewLineBILEx( NCSFileView *pNCSFileView, NCSEcwCellType eType, void **p_p_output_line)
#endif
{
	if( pNCSFileView->bGoToQuietState ) {
		pNCSFileView->eCallbackState = NCSECW_VIEW_QUIET;
		return(NCSECW_READ_CANCELLED);
	}
	switch (eType) {
		case NCSCT_UINT16 :
				if( erw_decompress_read_region_line_bil(pNCSFileView->pQmfRegion,
														(UINT8 **)p_p_output_line,
														NCSECW_READLINE_UINT16)) {
					return(NCSECW_READ_FAILED);
				} else {
					return(NCSECW_READ_OK);
				}
				break;
		case NCSCT_IEEE4 :
				if( erw_decompress_read_region_line_bil(pNCSFileView->pQmfRegion,
														(UINT8 **)p_p_output_line,
														NCSECW_READLINE_IEEE4)) {
					return(NCSECW_READ_FAILED);
				} else {
					return(NCSECW_READ_OK);
				}
				break;
		case NCSCT_UINT8 :
				if( erw_decompress_read_region_line_bil(pNCSFileView->pQmfRegion,
														(UINT8 **)p_p_output_line,
														NCSECW_READLINE_UINT8)) {
					return(NCSECW_READ_FAILED);
				} else {
					return(NCSECW_READ_OK);
				}
				break;
		case NCSCT_UINT32 :
		case NCSCT_UINT64 :
		case NCSCT_INT8	:
		case NCSCT_INT16 :
		case NCSCT_INT32 :
		case NCSCT_INT64 :
		case NCSCT_IEEE8 :
			return (NCSECW_READ_FAILED);//NCS_INVALID_PARAMETER); [18]
		default :
			return (NCSECW_READ_FAILED);//NCS_INVALID_PARAMETER); [18]
	}
//unreachable	return(NCSECW_READ_FAILED);
}

/******************************************************
**	NCScbmReadViewLineRGB() - read a block from an ECW file in RGB format
** Returns NCSECW_READ_OK if no error on the read.
********************************************************/
#ifdef NCSJPC_ECW_SUPPORT
NCSEcwReadStatus NCScbmReadViewLineRGB_ECW( NCSFileView *pNCSFileView, UINT8 *pRGBTriplets)
#else
NCSEcwReadStatus NCScbmReadViewLineRGB( NCSFileView *pNCSFileView, UINT8 *pRGBTriplets)
#endif
{
	if( pNCSFileView->bGoToQuietState ) {
		pNCSFileView->eCallbackState = NCSECW_VIEW_QUIET;
		return(NCSECW_READ_CANCELLED);
	}
	if( erw_decompress_read_region_line_rgb(pNCSFileView->pQmfRegion,pRGBTriplets) )
		return(NCSECW_READ_FAILED);
	else
		return(NCSECW_READ_OK);
}

/******************************************************
**	NCScbmReadViewLineRGB() - read a block from an ECW file in RGB format
** Returns NCSECW_READ_OK if no error on the read.
********************************************************/
#ifdef NCSJPC_ECW_SUPPORT
NCSEcwReadStatus NCScbmReadViewLineRGBA_ECW( NCSFileView *pNCSFileView, UINT32 *pRGBTriplets)
#else
NCSEcwReadStatus NCScbmReadViewLineRGBA( NCSFileView *pNCSFileView, UINT32 *pRGBTriplets)
#endif
{
	if( pNCSFileView->bGoToQuietState ) {
		pNCSFileView->eCallbackState = NCSECW_VIEW_QUIET;
		return(NCSECW_READ_CANCELLED);
	}
	if( erw_decompress_read_region_line_rgba(pNCSFileView->pQmfRegion, pRGBTriplets) )
		return(NCSECW_READ_FAILED);
	else
		return(NCSECW_READ_OK);
}

/******************************************************
**	NCScbmReadViewLineRGB() - read a block from an ECW file in RGB format
** Returns NCSECW_READ_OK if no error on the read.
********************************************************/
#ifdef NCSJPC_ECW_SUPPORT
NCSEcwReadStatus NCScbmReadViewLineBGRA_ECW( NCSFileView *pNCSFileView, UINT32 *pRGBTriplets)
#else
NCSEcwReadStatus NCScbmReadViewLineBGRA( NCSFileView *pNCSFileView, UINT32 *pRGBTriplets)
#endif
{
	if( pNCSFileView->bGoToQuietState ) {
		pNCSFileView->eCallbackState = NCSECW_VIEW_QUIET;
		return(NCSECW_READ_CANCELLED);
	}
	if( erw_decompress_read_region_line_bgra(pNCSFileView->pQmfRegion, pRGBTriplets) )
		return(NCSECW_READ_FAILED);
	else
		return(NCSECW_READ_OK);
}

/******************************************************
**	NCScbmReadViewLineRGB() - read a block from an ECW file in RGB format
** Returns NCSECW_READ_OK if no error on the read.
********************************************************/
#ifdef NCSJPC_ECW_SUPPORT
NCSEcwReadStatus NCScbmReadViewLineBGR_ECW( NCSFileView *pNCSFileView, UINT8 *pRGBTriplets)
#else
NCSEcwReadStatus NCScbmReadViewLineBGR( NCSFileView *pNCSFileView, UINT8 *pRGBTriplets)
#endif
{
	if( pNCSFileView->bGoToQuietState ) {
		pNCSFileView->eCallbackState = NCSECW_VIEW_QUIET;
		return(NCSECW_READ_CANCELLED);
	}
	if( erw_decompress_read_region_line_bgr(pNCSFileView->pQmfRegion,pRGBTriplets) )
		return(NCSECW_READ_FAILED);
	else
		return(NCSECW_READ_OK);
}

/*******************************************************
**	NCScbmReadViewFake() - Do a fake read on the View
** ONLY used for testing server performance. Used to
**	ensure cache is updated, to simulate a normal
**	client block request cache hit.
**	FIXME!! For now, does a top-down traverse to get
**	the block hits, which is different to the normal
**	inverse DWT which does a recursive by line approach,
**	and frees on a recursive line basis also.
**	Should not make much difference, but it is not EXACTLY
**	simulating reality...
********************************************************/
#ifdef NCSJPC_ECW_SUPPORT
NCSEcwReadStatus NCScbmReadViewFake_ECW( NCSFileView *pNCSFileView)
#else
NCSEcwReadStatus NCScbmReadViewFake( NCSFileView *pNCSFileView)
#endif
{
	UINT8 *pECWBlock;
	QmfRegionStruct *pQmfRegion;
	UINT16					nLevel = 0;

	if( pNCSFileView->bGoToQuietState ) {
		pNCSFileView->eCallbackState = NCSECW_VIEW_QUIET;
		return(NCSECW_READ_CANCELLED);
	}

	NCSMutexBegin(&pNCSEcwInfo->mutex);

	pQmfRegion = pNCSFileView->pQmfRegion;
	// Traverse from smallest to largest level, working out blocks we need
	while( nLevel <= pQmfRegion->p_largest_qmf->level ) {
		QmfRegionLevelStruct	*pLevel = &(pQmfRegion->p_levels[nLevel]);
		QmfLevelStruct			*pQmf = pLevel->p_qmf;

		UINT32					nStartXBlock, nEndXBlock, nStartYBlock, nEndYBlock;
		UINT32					nCurrentXBlock, nCurrentYBlock;

		nStartXBlock	= pLevel->start_x_block;
		nEndXBlock		= (nStartXBlock + pLevel->x_block_count) - 1;
		nStartYBlock	= pLevel->level_start_y / pQmf->y_block_size;
		nEndYBlock		= pLevel->level_end_y   / pQmf->y_block_size;
		// We now have the rectangle of blocks required for this level.
		// Loop through the rows of blocks, and request the blocks from the actual file
		for(nCurrentYBlock = nStartYBlock; nCurrentYBlock <= nEndYBlock; nCurrentYBlock++ )
			for(nCurrentXBlock = nStartXBlock; nCurrentXBlock <= nEndXBlock; nCurrentXBlock++ ) {
				// read the block
				pECWBlock = NCScbmReadViewBlock(pLevel, nCurrentXBlock, nCurrentYBlock);
				if( !pECWBlock ) {
					NCSMutexEnd(&pNCSEcwInfo->mutex);
					return( NCSECW_READ_FAILED );		// Error - internal logic failure
				}
				// and free it again
				NCScbmFreeViewBlock(pLevel, pECWBlock);
			}
		nLevel++;
	}
	
	NCSMutexEnd(&pNCSEcwInfo->mutex);
	return(NCSECW_READ_OK);
}

