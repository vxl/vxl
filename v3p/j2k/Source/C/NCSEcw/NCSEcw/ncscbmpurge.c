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
** FILE:   	ncscbmpurge.c
** CREATED:	1 March 1999
** AUTHOR: 	SNS
** PURPOSE:	NCS Client Block Manager cache purge functions
** EDITS:
** [01] sns	20-Mar-99 Created file
** [02] sns 02-apr-99 Updated logic to handle many files - first purging only unpacked blocks
** [03] sjc 30-Dec-99 Added global lock on stats seg
** [04] rar	17-Jan-01 Mac port changes
 *******************************************************/


#include "NCSEcw.h"
#if !defined(_WIN32_WCE)
	#ifdef WIN32 //[04]
		#define _CRTDBG_MAP_ALLOC
		#include "crtdbg.h"
	#endif
#endif


/*******************************************************
**	NCScbmPurgeCache() - Exceeded cache limits, so
**	try and purge down to 1/2 cache size
**
**	NOTES:
**	(1)	You MUST mutex make this call while the structures
**		are mutexed
**
**	(2) Cache purging is potentially quite complex, because:
**		-	we may not be able to get down to 1/2 cache,
**			if there are lots of file views open.
**		-	We want to avoid thrashing (purging/loading)
**		-	We want to purge larger level blocks before
**			purging lower level blocks.
**
**	(3)	The main cache size problem is the unpacked blocks,
**		so we ditch them first.
**
**	(4)	We can't just purge all blocks from one file, ideally
**		we want to purge blocks level N all files, level N-1 all
**		files, level N-2 all files, and so on down to level 0.
**		Because blocks are in a linked list, we DON'T want
**		to re-scan the linked lists because it will be way too
**		slow.  Net result: a double recursion function,
**		which recurses levels, and files, e.g. (going down
**		the levels). In the following example, 3 files, with:
**		File 1: Levels 0 to 2
**		File 2: Levels 0 to 1
**		File 3: Levels 0 to 4
**
**		File 1, Level 0
**		 File 2, Level 0
**		  File 3, Level 0
**		File 1, Level 1
**		 File 2, Level 1
**		  File 3, Level 1
**		File 1, Level 2
**		 File 3, Level 2
**		File 3, Level 3
**		File 3, Level 4
**
**
**	(5) If, at the end of purge, we did not achieve the desired
**		purge amount (down to 1/2 cache), the nAggressivePurge
**		is incremented. This means that time through on the purge,
**		a more draconian purge will be applied. This number can
**		continue to increase. It will revert to zero once a purge
**		managed to get cache size down to a reasonable level.
**
**	FIXME!! If all this is still not enough, we may have to
**	add a inverse DWT begin/end state, so we can purge
**	unpacked blocks even though they are within active views.
**
********************************************************/
void NCScbmPurgeCache(NCSFileView *pView)
{
	pView;//Keep compiler happy
	NCSMutexBegin(&pNCSEcwInfo->mutex);
	{
		NCSFile *pNCSFile = pNCSEcwInfo->pNCSFileList;
		NCSTimeStampMs	tLastCachePurge;
		UINT16	nMaxLevelCount;		// Relative to 1 not ZERO
		UINT32	nOldSize;
		NCSEcwStatsLock();

		nOldSize = (pNCSEcwInfo->pStatistics->nUnpackedBlocksCacheSize + pNCSEcwInfo->pStatistics->nPackedBlocksCacheSize);

		NCSEcwStatsUnLock();

	// DEBUG PERFORMANCE TESTING
	// NCSMutexEnd(&pNCSEcwInfo->mutex);
	//	return;

		if( !pNCSFile ) {
			NCSMutexEnd(&pNCSEcwInfo->mutex);
			return;
		}
		// Don't bother with purging if not yet over cache size limits
		if( (pNCSEcwInfo->pStatistics->nUnpackedBlocksCacheSize + pNCSEcwInfo->pStatistics->nPackedBlocksCacheSize)
			< pNCSEcwInfo->pStatistics->nMaximumCacheSize ) {
			NCSMutexEnd(&pNCSEcwInfo->mutex);
			return;
		}

		// To stop cache thrashing, only purge if some amount of time has passed (1 second)
		// Time in MS can wrap every 49 days, so we have to check for less than or greater than
		tLastCachePurge = NCSGetTimeStampMs();
		if( (pNCSEcwInfo->tLastCachePurge < tLastCachePurge) 
			&& ((pNCSEcwInfo->tLastCachePurge + pNCSEcwInfo->nPurgeDelay) > tLastCachePurge) ) {
			NCSMutexEnd(&pNCSEcwInfo->mutex);
			return;
		}

		// Initital a cache purge process, and note time of this purge
		pNCSEcwInfo->tLastCachePurge = tLastCachePurge;

		// First, get block pointers for all levels for all files. This requires
		// one scan of the cached block lists. We then do a second scan to clean the blocks
		// up from each file; this time working across files for max level, max level-1 and
		// so on.
		nMaxLevelCount = 0;
		while(pNCSFile) {
			UINT16	nFileLevelCount = 0;
			NCSFileCachedBlock	*pNextBlock = pNCSFile->pFirstCachedBlock;
			NCSFileCachedBlock	*pPreviousBlock = NULL;
			QmfLevelStruct		*pQmf = pNCSFile->pTopQmf;
			NCSFileCachePurge	*pLevelCachePurge = pNCSFile->pNCSCachePurge;
			NCSBlockId			nMaxLevelBlockNumber;

			nMaxLevelBlockNumber = 0;
			while( pNextBlock ) {
				// If block is not in this level, increment level until block is in the level
				// and update the current level with pointer to block at start of this block
				// Note that this code is set up to handle situations where there are no blocks
				// at a level, in which case the pointer is to the previous level's block
				while(pNextBlock->nBlockNumber >= nMaxLevelBlockNumber) {
					pLevelCachePurge->pPreviousBlock = pPreviousBlock;	// block before this level's 1st block
					pLevelCachePurge->pLevelBlocks = pNextBlock;		// this or smaller levels first block
					nFileLevelCount += 1;
					nMaxLevelBlockNumber = pQmf->nFirstBlockNumber + (pQmf->nr_x_blocks * pQmf->nr_y_blocks);
					pQmf = pQmf->p_larger_qmf;
					// It could possibly happen (although not if everything is working correctly)
					// that there are no blocks at this level cached, but there are at a higher level.
					// If so, we set the pointer at this level to NULL, indicating nothing here.
					// This might happen in the future if we use this caching for server-side file IO
					if( pNextBlock->nBlockNumber >= nMaxLevelBlockNumber )
						pLevelCachePurge->pLevelBlocks = NULL;
					pLevelCachePurge += 1;
				}
				pPreviousBlock = pNextBlock;
				pNextBlock = pNextBlock->pNextCachedBlock;
			}
			if( nMaxLevelCount < nFileLevelCount )
				nMaxLevelCount = nFileLevelCount;
			pNCSFile->nCachePurgeLevelCount = nFileLevelCount;
			pNCSFile = pNCSFile->pNextNCSFile;
		}

		// Now purge cache, starting from the absolutely largest level for any file, and working
		// down to smaller levels
		while(nMaxLevelCount--) {
			pNCSFile = pNCSEcwInfo->pNCSFileList;
			while(pNCSFile) {
				NCSFileCachePurge	*pLevelCachePurge;
				// only process this level for this file if file has at least this many levels IN CACHE
				if( pNCSFile->nCachePurgeLevelCount > nMaxLevelCount ) {
					NCSFileCachedBlock	*pNextBlock;
					QmfLevelStruct		*pQmf = pNCSFile->pTopQmf;
					NCSBlockId			nMaxLevelBlockNumber;
					UINT32				nPurgeAggression;

					// work out how aggressive we should be on purges
					// This depends on if the file is local or remote, and
					// if previous purges achieved the desired results
					if( pNCSFile->bLocalFile )
						nPurgeAggression = pNCSEcwInfo->nAggressivePurge * 2;
					else
						nPurgeAggression = pNCSEcwInfo->nAggressivePurge;

					pLevelCachePurge = pNCSFile->pNCSCachePurge + nMaxLevelCount;	// index into this level
					pNextBlock = pLevelCachePurge->pLevelBlocks;
					// find the QMF for this level
					while(pQmf->level < nMaxLevelCount)
						pQmf = pQmf->p_larger_qmf;
					nMaxLevelBlockNumber = pQmf->nFirstBlockNumber + (pQmf->nr_x_blocks * pQmf->nr_y_blocks);

					// only process this level if there are blocks at this level
					while(pNextBlock) {
						if(pNextBlock->nBlockNumber >= nMaxLevelBlockNumber)
							break;		// finished with blocks at this level

						// Consider removing this block. If in use, leave it alone
						// If not in use:
						//		-	remove the Unpacked data if there.
						//		-	remove the Packed data ONLY IF:
						//			A local file and low hit count
						//			A remote file and high hit count
						//		-	remove the cache node if both unpacked & packed are NULL
						if( pNextBlock->nUsageCount == 0 /*|| nPurgeAggression > pNextBlock->nHitCount*/) {
							NCSEcwStatsLock();

							if( pNextBlock->pUnpackedECWBlock ) {
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
								DisposeHandle( pNextBlock->pUnpackedECWBlock );
#else
								NCSFree(pNextBlock->pUnpackedECWBlock);
#endif
								pNextBlock->pUnpackedECWBlock = NULL;
								NCSEcwStatsDecrement(&pNCSEcwInfo->pStatistics->nUnpackedBlocksCacheSize, pNextBlock->nUnpackedECWBlockLength);
							}
							if( pNextBlock->pPackedECWBlock ) {
								if( pNextBlock->nUsageCount == 0 && nPurgeAggression > pNextBlock->nHitCount ) {
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
									DisposeHandle( pNextBlock->pPackedECWBlock );
#else
									NCSFree(pNextBlock->pPackedECWBlock);
#endif
									pNextBlock->pPackedECWBlock = NULL;
									NCSEcwStatsDecrement(&pNCSEcwInfo->pStatistics->nPackedBlocksCacheSize, pNextBlock->nPackedECWBlockLength);
								}
							}
							NCSEcwStatsUnLock();
							// We could do the free block here, but will do it later
						}
						pNextBlock = pNextBlock->pNextCachedBlock;
					}
				}
		
				pNCSFile = pNCSFile->pNextNCSFile;

				// stop if have purged enough. If being aggressive, clean all files at this level
				if( !pNCSEcwInfo->nAggressivePurge
				 &&	((pNCSEcwInfo->pStatistics->nUnpackedBlocksCacheSize + pNCSEcwInfo->pStatistics->nPackedBlocksCacheSize)
					< (pNCSEcwInfo->pStatistics->nMaximumCacheSize/2)) )
						break;			// done enough purging
			}
			if( (pNCSEcwInfo->pStatistics->nUnpackedBlocksCacheSize + pNCSEcwInfo->pStatistics->nPackedBlocksCacheSize)
				< (pNCSEcwInfo->pStatistics->nMaximumCacheSize/2) )
					break;			// done enough purging
		}


		// Now remove any nodes that are unactive, with no data loaded.
		// We could do this in the above list (faster), but it gets a little
		// complex to track the level pointers, so keeping it in a separate
		// loop for now, for simplicity.
		// We have to keep blocks that have a bRequested flag, but Usage == 0,
		// as these are block requests that are currently being cancelled.

		pNCSFile = pNCSEcwInfo->pNCSFileList;
		while(pNCSFile) {
			NCSFileCachedBlock *pPreviousBlock = NULL;
			NCSFileCachedBlock	*pBlock = pNCSFile->pFirstCachedBlock;
			while(pBlock) {
				if( !pBlock->bRequested && !pBlock->nUsageCount && !pBlock->pPackedECWBlock && !pBlock->pUnpackedECWBlock ) {
					NCSFileCachedBlock *pNextBlock = pBlock->pNextCachedBlock;

					// delete the block. We MUST clean up the working pointers as well
					if( pNCSFile->pWorkingCachedBlock == pBlock )
						pNCSFile->pWorkingCachedBlock = pNextBlock;

					if( pNCSFile->pLastReceivedCachedBlock == pBlock )
						pNCSFile->pLastReceivedCachedBlock = pNextBlock;

					if( pPreviousBlock )
						pPreviousBlock->pNextCachedBlock = pNextBlock;
					else {
						pNCSFile->pFirstCachedBlock = pNextBlock;
					}

					NCSPoolFree(pNCSFile->pBlockCachePool, pBlock);
					pBlock = pNextBlock;
				}
				else {
					pPreviousBlock = pBlock;
					pBlock = pBlock->pNextCachedBlock;
				}
			}

			pNCSFile = pNCSFile->pNextNCSFile;
		}


		// If running a high aggression factor, start purging files. Time files can live for
		// are dependant on how aggressive we currently are for cache RAM.
		{
			NCSTimeStampMs	tPurgeTime;

			tPurgeTime = pNCSEcwInfo->nFilePurgeDelay / (pNCSEcwInfo->nAggressivePurge+1);
			if( tPurgeTime < pNCSEcwInfo->nMinFilePurgeDelay)
				tPurgeTime = pNCSEcwInfo->nMinFilePurgeDelay;

			pNCSFile = pNCSEcwInfo->pNCSFileList;
			while(pNCSFile) {
				if( pNCSFile->nUsageCount == 0 ) {
					if( (pNCSFile->tLastSetViewTime > tLastCachePurge) 
					 || ((pNCSFile->tLastSetViewTime + tPurgeTime) < tLastCachePurge) ) {
						NCSFile	*pNextNCSFile = pNCSFile->pNextNCSFile;
						// Purge the file
						NCSecwCloseFileCompletely(pNCSFile);
						pNCSFile = pNextNCSFile;
						continue;			// and try the next file
					}
				}
				pNCSFile = pNCSFile->pNextNCSFile;
			}
		}

		// See how we did. If achieved a good purge, then decrement the purge counter by /2
		// next time (so it won't go from an all-out purge down to light purge, but instead
		// try and reach a reasonable rate

		if( (pNCSEcwInfo->pStatistics->nUnpackedBlocksCacheSize + pNCSEcwInfo->pStatistics->nPackedBlocksCacheSize)
			< (pNCSEcwInfo->pStatistics->nMaximumCacheSize/2) ) {
			pNCSEcwInfo->nAggressivePurge /= 2;		// move back towards a lighter purge strategy
		}
		else {
			pNCSEcwInfo->nAggressivePurge += 1;		// next time try a higher purge strategy
		}

#ifdef NEVEREVER
		{
			char	msg[1000];
			tLastCachePurge = NCSGetTimeStampMs() - tLastCachePurge;
			sprintf(msg,"Cache purge: %d ms, aggression = %d, old size %d KB, new size %d KB\n",
				(int) tLastCachePurge,
				(int) pNCSEcwInfo->nAggressivePurge,
				nOldSize / 1024,
				(int) ((pNCSEcwInfo->pStatistics->nUnpackedBlocksCacheSize +
						pNCSEcwInfo->pStatistics->nPackedBlocksCacheSize) / 1024)
				);
			MessageBox(NULL,msg,"NCS Cache Purge", MB_OK);
	//		printf(msg);
		}
#endif
	}
	NCSMutexEnd(&pNCSEcwInfo->mutex);
	return;
}

