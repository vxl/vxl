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
** FILE:   	ncscbmopen.c
** CREATED:	8 May 1999
** AUTHOR: 	SNS
** PURPOSE:	NCS CBM View Open/Close/GetInfo functions (split out from ncscbm.c)
** EDITS:
** [01] sns	03-Mar-99 Created file
** [02] sns 07-Apr-99 updated callback logic
** [03] sns 09-May-99 Moved to separate file
** [04] sjc 08-Jul-99 Send Cancel packets on view close
** [05] dmm 27-Jul-99 Propagate NCSError return codes
** [06] sjc 24-Aug-99 Moved bandlist free outside QMF check - was leaking if no setview()
** [07] sjc 30-Sep-99 Added NCScbmCloseFileViewEx() to be able to force file closure
** [08] sjc 30-Dec-99 Added global lock on stats seg
** [09] sjc 20-Jun-00 Out-of-Order block fix
** [10] sjc 11-Aug-00 Unlock mutex in loop to allow clean shutdown of iDWT thread (can block and ends up being terminated)
** [11]  ny 23-Nov-00 Mac port changes
** [12] sjc 25-Apr-01 Bug fixes for 9x with remote >2GB files and other read errors
** [13] sjc 25-Jan-02 Fix for 16bit EncodeFormat
** [14] GAN 15-Apr-04 Fix to check if files are already closed
** [15] tfl 16-Jan-05 Changed error returns on NCScbmGetViewFileInfo fns to NCS_INVALID_PARAMETER
 *******************************************************/

#include "NCSEcw.h"

static void NCScbmFileViewGoToQuietState(NCSFileView *pNCSFileView);
int NCScbmFileViewRequestBlocks(NCSFileView *pNCSFileView, QmfRegionStruct *pQmfRegion, NCSEcwBlockRequestMethod eRequest);
UINT8 *NCScbmConstructZeroBlock(QmfLevelStruct *p_qmf, UINT32 *pLength);

/*******************************************************
**	NCScbmOpenFileView() -	Opens a file view
**
**	Notes:
**	-	File could be local or remote
**	-	Blocks until file can be opened
**	-	The RefreshCallback can be NULL or a client refresh
**		callback function to call when blocks have arrived
**	Special notes for the RefreshCallback:
**	(1)	You must set this for each FileView that you want to have refresh
**		calls received for.  If you don't set it, the FileView
**		will instead revert to the blocking timer when blocks are
**		requested.
**
**	Returns:	NCSError (cast to int).	FIXME: change return type to NCSError, along 
**				with return types for all SDK public funcs, and ship NCSError.h in SDK
**
********************************************************/

#ifdef NCSJPC_ECW_SUPPORT
NCSError NCScbmOpenFileView_ECW(char *szUrlPath, NCSFileView **ppNCSFileView, /**[11]**/
					   NCSEcwReadStatus (*pRefreshCallback)(NCSFileView *pNCSFileView))
#else
NCSError NCScbmOpenFileView(char *szUrlPath, NCSFileView **ppNCSFileView, /**[11]**/
					   NCSEcwReadStatus (*pRefreshCallback)(NCSFileView *pNCSFileView))
#endif
{
	NCSFile	*pNCSFile;
	NCSFileView *pNCSFileView;
	NCSError nError;

	if (!pNCSEcwInfo) {
		NCSecwInitInternal();
	}

#ifdef MACINTOSH_NJ
    OSErr myErr;
    long myAttr;
	long	cLang = -1;
 
    myErr = Gestalt(gestaltKeyboardType, &myAttr);
 	cLang = GetScriptVariable(smSystemScript,smScriptLang);
       
	if ( /*myAttr == 198  || TEST ONLY*/ 
		 /*cLang == langEnglish ||TEST ONLY*/ 
	 	    myAttr == gestaltJapanAdjustADBKbd 
		 || myAttr == gestaltPwrBkEKJISKbd 
		 || myAttr == gestaltUSBCosmoJISKbd
		 || cLang == langJapanese)
	{
		return NCS_UNSUPPORTEDLANGUAGE;
	}
	
#endif

	// Lock to add File View list
	NCSMutexBegin(&pNCSEcwInfo->mutex);
	*ppNCSFileView = NULL;			// [02] make sure is NULL in case of error

	nError = NCSecwOpenFile(&pNCSFile, szUrlPath, TRUE, TRUE);

	if( nError == NCS_SUCCESS ) {

		pNCSFileView = (NCSFileView *) NCSMalloc( sizeof(NCSFileView), FALSE);
		if( pNCSFileView ) {
			// If no client block caching memory pool allocated yet, allocate it, and the zero blocks

			if( !pNCSFile->pBlockCachePool ) {
				// create the memory block pool
				pNCSFile->pBlockCachePool = NCSPoolCreate(sizeof(NCSFileCachedBlock), NCSECW_CACHED_BLOCK_POOL_SIZE);
			} 
			if(!pNCSFile->pLevel0ZeroBlock) {
				// create the fake 0zero blocks
				pNCSFile->pLevel0ZeroBlock = NCScbmConstructZeroBlock(pNCSFile->pTopQmf, NULL);
			}
			if(!pNCSFile->pLevelnZeroBlock) {
				// create the fake nzero blocks
				pNCSFile->pLevelnZeroBlock = NCScbmConstructZeroBlock(pNCSFile->pTopQmf->p_larger_qmf, NULL);
			}
			
			if( pNCSFile->pBlockCachePool && pNCSFile->pLevel0ZeroBlock && pNCSFile->pLevelnZeroBlock) {

				NCSEcwStatsLock();
				NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nFileViewsOpen, 1);
				NCSEcwStatsUnLock();

				// caching methodology not determined until the first setview
				pNCSFileView->eCallbackState = NCSECW_VIEW_QUIET;
				pNCSFileView->bGoToQuietState = FALSE;
				pNCSFileView->nCacheMethod = NCS_CACHE_INVALID;
				pNCSFileView->bIsRefreshView = FALSE;

				pNCSFileView->pPrevNCSFileView = NULL;
				pNCSFileView->pQmfRegion = NULL;
				pNCSFileView->pNCSFile = pNCSFile;

				pNCSFileView->info.nBlocksInView = 0;				// total number of blocks that cover the view area
				pNCSFileView->info.nBlocksAvailable = 0;				// current number of blocks available at this instant
				pNCSFileView->info.nBlocksAvailableAtSetView = 0;	// Blocks that were available at the time of the SetView
				pNCSFileView->info.nMissedBlocksDuringRead = 0;

				// [02] make sure callback values are clean
				pNCSFileView->bTriggerRefreshCallback = FALSE;

				pNCSFileView->info.pClientData = NULL;
				pNCSFileView->info.nTopX	= pNCSFileView->info.nLeftY = 0;
				pNCSFileView->info.nBottomX = pNCSFileView->info.nRightY = 0;
				pNCSFileView->info.nSizeX	= pNCSFileView->info.nSizeY = 0;
				pNCSFileView->info.fTopX	= pNCSFileView->info.fLeftY = 0.0;
				pNCSFileView->info.fBottomX	= pNCSFileView->info.fRightY = 0.0;
				pNCSFileView->info.nBands	= 0;					// [02]
				// [02] we keep our own master BandList array, and copy into that list from,
				//		the SetView calls. This way we don't have to track the client bandlist
				pNCSFileView->info.pBandList = NCSMalloc( sizeof(UINT32) * pNCSFile->pTopQmf->p_file_qmf->nr_bands, FALSE);
				if(pNCSFileView->info.pBandList) {

					pNCSFileView->pending.pBandList = NCSMalloc( sizeof(UINT32) * pNCSFile->pTopQmf->p_file_qmf->nr_bands, FALSE);
					if(pNCSFileView->pending.pBandList) {

						pNCSFileView->nPending = 0;
						pNCSFileView->nCancelled = 0;
						pNCSFileView->pRefreshCallback = pRefreshCallback;

						pNCSFileView->nNextDecodeMissID = 0;	/**[09]**/

						pNCSFileView->pNextNCSFileView = pNCSFile->pNCSFileViewList;
						if( pNCSFile->pNCSFileViewList )
							pNCSFile->pNCSFileViewList->pPrevNCSFileView = pNCSFileView;
						pNCSFile->pNCSFileViewList = pNCSFileView;

						*ppNCSFileView = pNCSFileView;

						NCSMutexEnd(&pNCSEcwInfo->mutex);
						return NCS_SUCCESS;		
					} else {
						nError = NCS_COULDNT_ALLOC_MEMORY;
					}
					NCSFree(pNCSFileView->info.pBandList);
				} else {
					nError = NCS_COULDNT_ALLOC_MEMORY;
				}
			} else {
				nError = NCS_FILE_NO_MEMORY;
			}
			NCSFree(pNCSFileView);
		} else {
			nError = NCS_FILE_NO_MEMORY;
		}
		NCSecwCloseFile(pNCSFile);
	} 
	
	NCSMutexEnd(&pNCSEcwInfo->mutex);
	return nError;
}


/*******************************************************
**	NCSecwConstructZeroBlock()	- Allocates and returns a zero-block
**
**	Notes:
********************************************************/
 
UINT8 *NCScbmConstructZeroBlock(QmfLevelStruct *p_qmf,UINT32 *pLength)
{
	UINT32	nSidebands;
	UINT8	*pZeroBlock, *pZeroBlockSideband;
	UINT8	*pZeroBlock32;
	UINT32 nLength = 0;

	if( p_qmf->level )
		nSidebands = p_qmf->nr_sidebands - 1;
	else
		nSidebands = p_qmf->nr_sidebands;
	nSidebands = nSidebands * p_qmf->nr_bands;
	// we need room for N-1 UINT32's of sidebands, and N bytes of compression (zero block flags)
	nLength = (sizeof(UINT32) * (nSidebands-1)) + nSidebands  * sizeof(EncodeFormat);
	pZeroBlock = NCSMalloc(nLength , FALSE);/**[13]**/
	if(pLength) {
		*pLength = nLength;
	}
	if( !pZeroBlock )
		return( NULL );
	pZeroBlock32 = pZeroBlock;
	pZeroBlockSideband = pZeroBlock + (sizeof(UINT32) * (nSidebands - 1));
	*((EncodeFormat*)pZeroBlockSideband) = ENCODE_ZEROS;	// one more entry than offsets [13]
	pZeroBlockSideband += sizeof(EncodeFormat); /**[13]**/

	while(--nSidebands) {
		*pZeroBlock32++ = 0;	// 0xFF000000
		*pZeroBlock32++ = 0;	// 0x00FF0000
		*pZeroBlock32++ = 0;	// 0x0000FF00
		*pZeroBlock32++ = sizeof(EncodeFormat);	// 0x000000FF [13]

		*((EncodeFormat*)pZeroBlockSideband) = ENCODE_ZEROS;	/**[13]**/
		pZeroBlockSideband += sizeof(EncodeFormat);				/**[13]**/
		//*pZeroBlockSideband++ = ENCODE_ZEROS; [13]
	}
	return(pZeroBlock);
}


/*******************************************************
**	NCScbmCloseFileView() -	Closes a previously opened file view
**
**	Notes:
**	-	File could be local or remote
**	-	Blocks until file can be opened
********************************************************/

#ifdef NCSJPC_ECW_SUPPORT
NCSError	NCScbmCloseFileViewEx_ECW(NCSFileView *pNCSFileView,			/**[07]**/ /**[11]**/
						  BOOLEAN bFreeCachedFile)				/**[07]**/
#else
NCSError	NCScbmCloseFileViewEx(NCSFileView *pNCSFileView,			/**[07]**/ /**[11]**/
						  BOOLEAN bFreeCachedFile)				/**[07]**/
#endif
{
	NCSFile *pNCSFile;

	if (!pNCSEcwInfo) {
		NCSecwInitInternal();
	}
 
	if( pNCSFileView ) {					//[14] Check if files are already closed
		BOOLEAN bFound = FALSE;
#ifndef NCSJPC_ECW_SUPPORT
		NCSMutexBegin(&pNCSEcwInfo->mutex);
#endif
		
		pNCSFile = pNCSEcwInfo->pNCSFileList;
		while(pNCSFile) {
			NCSFileView *pTmp = pNCSFile->pNCSFileViewList;
			while(pTmp) {
				if(pTmp == pNCSFileView) {
					bFound = TRUE;
					break;
				}
				pTmp = pTmp->pNextNCSFileView;
			}	
			pNCSFile = pNCSFile->pNextNCSFile;
		}
		if(bFound) {
			pNCSFile = pNCSFileView->pNCSFile;


			NCSEcwStatsLock();
			NCSEcwStatsDecrement(&pNCSEcwInfo->pStatistics->nFileViewsOpen, 1);
			NCSEcwStatsUnLock();

			NCScbmCloseFileViewCompletely(&(pNCSFile->pNCSFileViewList), pNCSFileView);

			if((pNCSFile->nUsageCount == 1) && bFreeCachedFile) {	/**[07]**/
				pNCSFile->bValid = FALSE;							/**[07]**/
			}														/**[07]**/
			NCSecwCloseFile(pNCSFile);
		}
#ifndef NCSJPC_ECW_SUPPORT
		NCSMutexEnd(&pNCSEcwInfo->mutex);
#endif
	}

	return(0);		// FileView was closed
}

#ifdef NCSJPC_ECW_SUPPORT
NCSError	NCScbmCloseFileView_ECW(NCSFileView *pNCSFileView) /**[11]**/
#else
NCSError	NCScbmCloseFileView(NCSFileView *pNCSFileView) /**[11]**/
#endif
{
	return(NCScbmCloseFileViewEx(pNCSFileView, FALSE));	/**[07]**/
}

/*
**	Flush file view from cache. Does NOT mutex - caller must do the mutex
*/

int	NCScbmCloseFileViewCompletely(NCSFileView **ppNCSFileViewList, NCSFileView *pNCSFileView)
{

	if( pNCSFileView->pRefreshCallback )
		NCScbmFileViewGoToQuietState(pNCSFileView);

	if( pNCSFileView->pQmfRegion ) {
		// Mark blocks as no longer in use
		if( pNCSFileView->nCacheMethod == NCS_CACHE_VIEW )
			NCScbmFileViewRequestBlocks(pNCSFileView, pNCSFileView->pQmfRegion, NCSECW_BLOCK_CANCEL);
		// shut down the view
		erw_decompress_end_region(pNCSFileView->pQmfRegion);
		pNCSFileView->pQmfRegion = NULL;
	}
	if(pNCSFileView->info.pBandList) {	/**[06]**/
		NCSFree(pNCSFileView->info.pBandList);			// [02] we keep a local band list for the view
		pNCSFileView->info.pBandList = NULL;
	}
	if(pNCSFileView->pending.pBandList) {	/**[06]**/
		NCSFree(pNCSFileView->pending.pBandList);		// [02] and for the pending view
		pNCSFileView->pending.pBandList = NULL;
	}
		
	if(!pNCSFileView->pNCSFile->bLocalFile /**[12]**/ && !pNCSFileView->pNCSFile->bSendInProgress && 
	   (pNCSFileView->pNCSFile->nRequestsXmitPending || pNCSFileView->pNCSFile->nCancelsXmitPending)) {
		NCScbmNetFileXmitRequests(NCS_SUCCESS, NULL, pNCSFileView->pNCSFile );
	}

	// Remove this file view from the view List
	if( *ppNCSFileViewList == pNCSFileView )
		*ppNCSFileViewList = pNCSFileView->pNextNCSFileView;
	if( pNCSFileView->pNextNCSFileView )
		pNCSFileView->pNextNCSFileView->pPrevNCSFileView = pNCSFileView->pPrevNCSFileView;
	if( pNCSFileView->pPrevNCSFileView )
		pNCSFileView->pPrevNCSFileView->pNextNCSFileView = pNCSFileView->pNextNCSFileView;
	NCSFree(pNCSFileView);
	return(0);
}

/*******************************************************
**	NCScbmFileViewToQuietState()	- Force the view into Quiet state (cancel iDWTs in progress, etc)
**
**	Notes:
**	(1)	-	Don't call this more that you have to - it can be slow
**	(2) -	If can't get to quiet state inside NCSECW_BLOCKING_TIME_MS,
**			there is a serious error and it forces the view to quiet state (normally
**			should only take a few milliseconds at most to get into quiet state)
**	(3)	-	Should be called in MUTEX except during process shutdown
**
********************************************************/
#define QUIET_SLEEP_WAIT	10		// wait 10ms each time for it to go to quiet
static void NCScbmFileViewGoToQuietState(NCSFileView *pNCSFileView)
{
	INT32	nWait = NCSECW_QUIET_WAIT_TIME_MS;
//MessageBox(NULL, "Goto Quiet", "DEBUG", MB_OK);
	if( pNCSFileView->eCallbackState == NCSECW_VIEW_SET ) {		// OK to shut it down straight away
		pNCSFileView->eCallbackState = NCSECW_VIEW_QUIET;
		pNCSFileView->bGoToQuietState = FALSE;
		return;
	}

	while( pNCSFileView->eCallbackState != NCSECW_VIEW_QUIET && nWait > 0
		   && !pNCSEcwInfo->bShutdown
		   && NCSThreadIsRunning(&(pNCSEcwInfo->pIDWT->tIDWT)) ) {// if in shutdown, threads have died so don't wait for them

NCSMutexEnd(&(pNCSEcwInfo->mutex));		/**[10]**/
		pNCSFileView->bGoToQuietState = TRUE;
		NCSSleep(QUIET_SLEEP_WAIT);		// wait a short time - should not take long to go to quiet state
		nWait -= QUIET_SLEEP_WAIT;
NCSMutexBegin(&(pNCSEcwInfo->mutex));	/**[10]**/
	}
	if(nWait <= 0) {
		// Timed out - kill thread, since it's most likely locked up on the global mutex
		if(NCSThreadIsRunning(&(pNCSEcwInfo->pIDWT->tIDWT))) {
			NCSThreadTerminate(&(pNCSEcwInfo->pIDWT->tIDWT));
		}
		pNCSEcwInfo->pIDWT->eIDWTState = NCSECW_THREAD_DEAD;
	}
}

/*******************************************************
**	NCScbmGetViewFileInfo() -	Returns the information about a file opened using OpenView
**
**	Notes:
**	-	Just returns information about the file, not the current SetView in this view into that file
********************************************************/
#ifdef NCSJPC_ECW_SUPPORT
NCSError NCScbmGetViewFileInfo_ECW(NCSFileView *pNCSFileView, NCSFileViewFileInfo **ppNCSFileViewFileInfo) /**[11]**/
#else
NCSError NCScbmGetViewFileInfo(NCSFileView *pNCSFileView, NCSFileViewFileInfo **ppNCSFileViewFileInfo) /**[11]**/
#endif
{
	if( !pNCSFileView || !pNCSFileView->pNCSFile || !pNCSFileView->pNCSFile->pTopQmf ) {
		*ppNCSFileViewFileInfo = NULL;
		return(NCS_INVALID_PARAMETER); /*[15]*/
	}
	*ppNCSFileViewFileInfo = (NCSFileViewFileInfo *) pNCSFileView->pNCSFile->pTopQmf->pFileInfo;
	return(0);
}

/*******************************************************
**	NCScbmGetViewFileInfoEx() -	Returns the information about a file opened using OpenView
**
**	Notes:
**	-	Just returns information about the file, not the current SetView in this view into that file
********************************************************/
#ifdef NCSJPC_ECW_SUPPORT
NCSError NCScbmGetViewFileInfoEx_ECW(NCSFileView *pNCSFileView, NCSFileViewFileInfoEx **ppNCSFileViewFileInfo) /**[11]**/
#else
NCSError NCScbmGetViewFileInfoEx(NCSFileView *pNCSFileView, NCSFileViewFileInfoEx **ppNCSFileViewFileInfo) /**[11]**/
#endif
{
	if( !pNCSFileView || !pNCSFileView->pNCSFile || !pNCSFileView->pNCSFile->pTopQmf ) {
		*ppNCSFileViewFileInfo = NULL;
		return(NCS_INVALID_PARAMETER); /*[15]*/
	}
	*ppNCSFileViewFileInfo = (NCSFileViewFileInfoEx *) pNCSFileView->pNCSFile->pTopQmf->pFileInfo;
	return(0);
}

/*******************************************************
**	NCScbmGetViewFileInfo() -	Returns the information about the current SetView
**
**	Notes:
**	-	Only use this inside a callback, to determine which SetView is currently being processed
********************************************************/
#ifdef NCSJPC_ECW_SUPPORT
NCSError NCScbmGetViewInfo_ECW(NCSFileView *pNCSFileView, NCSFileViewSetInfo **ppNCSFileViewSetInfo) /**[11]**/
#else
NCSError NCScbmGetViewInfo(NCSFileView *pNCSFileView, NCSFileViewSetInfo **ppNCSFileViewSetInfo) /**[11]**/
#endif
{
	*ppNCSFileViewSetInfo = &(pNCSFileView->info);
	return(0);
}

