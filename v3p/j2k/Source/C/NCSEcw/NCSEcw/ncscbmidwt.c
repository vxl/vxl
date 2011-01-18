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
** FILE:   	ncscbmidwt.c
** CREATED:	9 May 1999
** AUTHOR: 	SNS
** PURPOSE:	NCS CBM inverse DWT thread and processing functions
** EDITS:
** [01] sns	09-May-99 Created file
** [02] sns 17-May-99 Cancelling reads if too many SetViews are pending
** [03] sns 17-May-99 Allows NULL entries for queued SetViews
** [04] sjc 15-May-00 NCScbmQueueIDWTCallback() called from 2 threads now
**					  - so need to do check all the time 
** [05]  ny 23-Nov-00 Mac port changes
** [06] sjc 22-May-01 Removed - this is done in Read*() calls already, was causing FMR & FMW hits & crashes.
** [07] jmp 23-Jul-01 Put in fix for bug where IDWT thread can be left in a suspended state after a SetView
**					  Using an Event object instead of ThreadSuspend/Resume.
** [08] rar 10-Oct-01 Mac port changes to required to support changes above [07]
** [09] sjc 11-Aug-04 Bugfix, force another refresh callback if read not called during last one
 *******************************************************/

#include "NCSEcw.h"
#if !defined(_WIN32_WCE)
	#ifdef WIN32
		#define _CRTDBG_MAP_ALLOC
		#include "crtdbg.h"
	#endif
#endif
#include "NCSLog.h"


static void NCScbmThreadIDWT( NCSidwt *pIDWT ); /**[05]**/
static void NCScbmThreadIDWTRequeue(NCSFileView *pNCSFileView);

/*******************************************************
**	NCScbmInitThreadIDWT()	- Inits (but does not start IDWT thread)
**
**
********************************************************/
void NCScbmInitThreadIDWT(NCSidwt *pIDWT)
{
	pIDWT;//Keep compiler happy
}
/*******************************************************
**	NCScbmFinishThreadIDWT() - Finishes IDWT thread, including telling it that it is no longer needed
**
********************************************************/
void NCScbmFinishThreadIDWT(NCSidwt *pIDWT)
{
	int	nWait = 5000;
	// we try and shutdown the iDWT thread. Give up after 5 seconds - something horrible has gone wrong

	while( pIDWT->eIDWTState != NCSECW_THREAD_DEAD ) {
		if( !NCSThreadIsRunning(&(pIDWT->tIDWT)) )
			break;		// thread is now dead
#ifdef WIN32 //[08]
		SetEvent(pNCSEcwInfo->m_hSuspendEvent);					/**[07]**/
#else
		NCSThreadResume(&(pNCSEcwInfo->pIDWT->tIDWT));		/**[07]**/
#endif
		NCSSleep(100);		// wait to get the thread to pay attention
		nWait -= 100;
		if( nWait < 0 )
			break;		// at this point, we have a serious problem
	}
	if(NCSThreadIsRunning(&(pIDWT->tIDWT)) ) {
		NCSLog(LOG_LOW, "Terminating iDWT thread after wait timeout: 0x%lx\n", pIDWT->tIDWT);
		NCSThreadTerminate(&(pIDWT->tIDWT));
	}

#ifdef WIN32 //[08]
	CloseHandle(pNCSEcwInfo->m_hSuspendEvent);					/**[07]**/
#endif

}

/*******************************************************
**	NCScbmQueueIDWTCallback()	- Queues a callback for this view
**
**	Notes:
**	(1)	-	This queues a callback to the application for this File View
**	(2)	-	The callback request is posted to the IDWT thread
**	(3)	-	The IDWT thread is started the first time this call is ever made
**			(so if the application never uses the callback application interface, the
**			IDWT thread is never started)
**	(4)	-	When starting the thread the first time, it is started SUSPENDED and we wake it
**			after adding the callback request to the queue
**
**	FIXME!! Currently queue order is ignored - always LIFO for speed
**
********************************************************/
void NCScbmQueueIDWTCallback(NCSFileView *pNCSFileView,			// queue a callback for this file view
								NCSEcwQueueInsertOrder eOrder)	// insert LIFO or FIFO
{
	NCSidwt *pIDWT;
	eOrder;//Keep compiler happy
	if( pNCSEcwInfo->bShutdown )
		return;			// Test this before the MUTEX, in case mutex's cross lock during shutdown

	pIDWT = pNCSEcwInfo->pIDWT;

	NCSMutexBegin(&pNCSEcwInfo->mutex);

	/*
	**	If shutting down or trying to get the view quiet, don't process this callback request
	*/
	if( pNCSEcwInfo->bShutdown
	 || pNCSFileView->bGoToQuietState ) {		// do another shutdown test, this time in the mutex
		// The world is shutting down. Ignore this setview request and quietly exit
		pNCSFileView->eCallbackState = NCSECW_VIEW_QUIET;
		NCSMutexEnd(&pNCSEcwInfo->mutex);
		return;
	}

	/*
	**	If thread not started yet, then start it
	*/
	if( pIDWT->eIDWTState == NCSECW_THREAD_DEAD ) {
#ifdef WIN32 //[08]
		pNCSEcwInfo->m_hSuspendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);	/**[07]**/
#endif
		if( !NCSThreadSpawn(&(pIDWT->tIDWT), (void(*)(void *))NCScbmThreadIDWT, pIDWT, FALSE) ) { /**[05]**/
			// thread is dead, and won't start. Just ignore - this is a fatal error
			NCSMutexEnd(&pNCSEcwInfo->mutex);
			return;
		}
		pIDWT->eIDWTState = NCSECW_THREAD_SUSPENDED;
	}

	/*
	**	Add the callback request to the thread queue.
	**	Note that the IDWT structure is init'd to zero at start, so
	**	the realloc works as expected on the first call (turns into a malloc)
	**	The queue expands if there are more requests pending that the current queue length
	*/
//[04] #ifdef _DEBUG
	{
		// First, do a debug pass to see if the view is already in the list. If so, we have
		// a problem.
		INT32	nCurrent = 0;
		while(nCurrent < pIDWT->nQueueNumber ) {
			if( pIDWT->ppNCSFileView[nCurrent] == pNCSFileView) {
//[04]				_ASSERT(0);
				NCSMutexEnd(&pNCSEcwInfo->mutex);
				return;
			}
			nCurrent++;
		}
	}
//[04]#endif
	pIDWT->nQueueNumber += 1;
	if( pIDWT->nQueueNumber >= pIDWT->nQueueAllocLength ) {
		pIDWT->nQueueAllocLength += NCSECW_IDWT_QUEUE_GRANULARITY;
		pIDWT->ppNCSFileView = (NCSFileView **) 
			NCSRealloc(pIDWT->ppNCSFileView, sizeof(NCSFileView *) * pIDWT->nQueueAllocLength, FALSE);
	}
	pIDWT->ppNCSFileView[pIDWT->nQueueNumber - 1] = pNCSFileView;
	pNCSFileView->eCallbackState = NCSECW_VIEW_QUEUED;

	/*
	**	If the thread was suspended, go and kick it into life to process the callback
	**	FIXME!! There is a potential problem here:
	**		a.	iDWT thread marks itself as SUSPENDED
	**		b.	iDWT exits mutex lock, and a thread swap happens to this thread
	**		c.	This thread does a Resume, but the other thread is still marked
	**			as running, so no resume happens
	**		e.	This thread exits mutex lock, and a thread swap happens to the iDWT thread
	**		d.	iDWT goes to sleep
	**	At this point, the thread is suspended, and won't wake because this thread thinks
	**	that it did wake the other thread. The problem corrects on the next call into
	**	this thread, but still a problem.
	*/
	if(pIDWT->eIDWTState == NCSECW_THREAD_SUSPENDED ) {		/**[07]**/
#ifdef WIN32  //[08]
		SetEvent(pNCSEcwInfo->m_hSuspendEvent);				/**[07]**/
#else
		NCSThreadResume(&(pIDWT->tIDWT));
#endif
	}
	NCSMutexEnd(&pNCSEcwInfo->mutex);

	return;
}

/*******************************************************
**	NCScbmThreadIDWT()	- The thread that processes iDWT's for callback Views
**
**	Notes:
**	(1)	-	Started first time a callback request is posted to the queue for the thread
**	(2)	-	The thread processes outstanding callback requests, then suspends
**			when nothing left to process
**	(3)	-	If in SHUTDOWN mode, cleans up, and dies
**
********************************************************/


static void NCScbmThreadIDWT( NCSidwt *pIDWT ) /**[05]**/
{
	if( pNCSEcwInfo->bShutdown ) {
		pIDWT->eIDWTState = NCSECW_THREAD_DEAD;
#ifdef WIN32
		NCSThreadExit(0);
#else
		return;
#endif
	}
	NCSMutexBegin(&pNCSEcwInfo->mutex);

	while( TRUE ) {
		pIDWT->eIDWTState = NCSECW_THREAD_ALIVE;		
		/*
		**	See if there is any work for us. If so, remove it from the queue and process it,
		*/
		if( pIDWT->nQueueNumber ) {
			NCSFileView *pNCSFileView;
			pIDWT->nQueueNumber -= 1;
			pNCSFileView = pIDWT->ppNCSFileView[pIDWT->nQueueNumber];
			if( pNCSFileView ) {	// [03] can have NULL (canceled) views in the list
				if( pNCSFileView->bGoToQuietState ) {		// View is being reset, so ignore the read
					pNCSFileView->eCallbackState = NCSECW_VIEW_QUIET;
				}
				else {
					// See if should cancel this read because of pending setviews.
					// We stop cancelling if we get a continual stream of SetViews,
					// so *something* is done every so often
					if( pNCSFileView->nPending >= NCSECW_MAX_SETVIEW_PENDING
					 && pNCSFileView->nCancelled < NCSECW_MAX_SETVIEW_CANCELS ) {
						// [02] cancel this SetView and queue the pending one instead
						pNCSFileView->nCancelled += 1;
						pNCSFileView->eCallbackState = NCSECW_VIEW_QUIET;
						NCScbmThreadIDWTRequeue(pNCSFileView);
					}
					else {
						NCSEcwReadStatus eStatus;
						// process the iDWT up in the application (which will call back in with readline calls)
						// must do this outside the MUTEX
						pNCSFileView->eCallbackState = NCSECW_VIEW_IDWT;
						NCSMutexEnd(&pNCSEcwInfo->mutex);
						eStatus = (pNCSFileView->pRefreshCallback)(pNCSFileView);
#ifdef NOTDEF	//[06] - removed - done in Read*() calls already, was causing FMR & FMW hits						
						if( pNCSFileView->bGoToQuietState ) {		// View is being reset, so ignore the read
							pNCSFileView->eCallbackState = NCSECW_VIEW_QUIET;
						}
#endif						
						NCSMutexBegin(&pNCSEcwInfo->mutex);
						// If the view was closed, it may no longer exist, so we can only
						// trust the pointer if there was no error
						if( eStatus != NCSECW_READ_CANCELLED ) {
							pNCSFileView->eCallbackState = NCSECW_VIEW_QUIET;
							pNCSFileView->nCancelled = 0;			// this one was not cancelled
							if(pNCSFileView->pQmfRegion->nCounter == 0) {
								// [09] App didn't read anything, so force another refresh callback
								pNCSFileView->info.nMissedBlocksDuringRead++;
							}
							NCScbmThreadIDWTRequeue(pNCSFileView);
						}
					}	// [02]
				}
			}	// [03]
		}
		/*
		**	If more work outstanding, release the mutex, and loop again
		*/
		if( pIDWT->nQueueNumber ) {
#ifdef MACINTOSH
			NCSThreadYield();
#endif
			continue;		// process the next view
		}

		/*
		**	Nothing left to do.  Suspend the thread and change state to sleeping
		*/
		pIDWT->eIDWTState = NCSECW_THREAD_SUSPENDED;
		NCSMutexEnd(&pNCSEcwInfo->mutex);

#ifdef WIN32 //[08]
		//replacing the suspend with a wait.
		//NCSThreadSuspend();											/**[07]**/
		WaitForSingleObject(pNCSEcwInfo->m_hSuspendEvent, INFINITE);	/**[07]**/
#else
		NCSThreadSuspend();
#endif

		if( pNCSEcwInfo->bShutdown ) {
			pIDWT->eIDWTState = NCSECW_THREAD_DEAD;
#ifdef WIN32
			NCSThreadExit(0);
#else
			return;
#endif
			//_endthreadex(0);
		}
		NCSMutexBegin(&pNCSEcwInfo->mutex);
	}
}


/*******************************************************
**	NCScbmThreadIDWTRequeue()	- Requeue this view if need be
**
**	Notes:
**	(1)	-	Only called by the IDWT thread, and in that thread.
**	(2)	-	This routine must be called with the MUTEX locked
**	(3)	-	If the view has a new setview pending, that view is
**			copied to the current view, and the pending flag
**			is clear. In this case, the view is added at the top
**			of the queue (so it will be processed next).
**			If no setview is pending, and the current
**			read did not get all blocks, then a read is re-queued
**			for this view.  We add the view to the bottom of the queue,
**			so this view regeneration will happen last, after any other
**			pending views.
**
********************************************************/
static void NCScbmThreadIDWTRequeue(NCSFileView *pNCSFileView)
{
	if( pNCSFileView->bGoToQuietState )
		return;

	if( pNCSFileView->nPending ) {
		/*
		**	There is a pending SetView, so load it up and run
		**	We can just to a SetView to do this; it will sort everything out
		*/
		pNCSFileView->nPending = 0;						// so the SetView will update correctly

		NCScbmSetFileViewEx(pNCSFileView,
							pNCSFileView->pending.nBands,
							pNCSFileView->pending.pBandList,
							pNCSFileView->pending.nTopX,
							pNCSFileView->pending.nLeftY,
							pNCSFileView->pending.nBottomX,
							pNCSFileView->pending.nRightY,
							pNCSFileView->pending.nSizeX,
							pNCSFileView->pending.nSizeY,
							pNCSFileView->pending.fTopX,
							pNCSFileView->pending.fLeftY,
							pNCSFileView->pending.fBottomX,
							pNCSFileView->pending.fRightY);

	}
	else if( pNCSFileView->info.nMissedBlocksDuringRead ) {
		/*
		**	Otherwise, this is a refresh, so start it up. We know everything
		**	is the same, so we just reset up the QMF structure, and leave everything else
		**	else alone.
		*/

		erw_decompress_end_region(pNCSFileView->pQmfRegion);
		pNCSFileView->pQmfRegion = erw_decompress_start_region( 
				pNCSFileView->pNCSFile->pTopQmf,
				pNCSFileView->info.nBands,
				pNCSFileView->info.pBandList,
				pNCSFileView->info.nTopX,
				pNCSFileView->info.nLeftY,
				pNCSFileView->info.nBottomX,
				pNCSFileView->info.nRightY,
				pNCSFileView->info.nSizeX,
				pNCSFileView->info.nSizeY);
		_ASSERT( pNCSFileView->pQmfRegion );
		if( !pNCSFileView->pQmfRegion )
			return;			// Fatal error
		// Last time a block was received for this file view. Initially set to time of File View
		// Also update last time a Set View was done, to assist cache purging logic
		pNCSFileView->tLastBlockTime = NCSGetTimeStampMs();
		NCScbmPurgeCache(pNCSFileView);	// see if we need to purge cache
		pNCSFileView->bTriggerRefreshCallback = FALSE;
		pNCSFileView->eCallbackState = NCSECW_VIEW_SET;
		pNCSFileView->bIsRefreshView = TRUE;
		pNCSFileView->info.nMissedBlocksDuringRead = 0;		// so will stop once a full read happened
		pNCSFileView->pQmfRegion->pNCSFileView = pNCSFileView;

		/*
		**	There is a remote chance that (a) the previous IDWT missed some blocks,
		**	and (b) those blocks came in during processing and (c) the total number
		**	of these blocks equalled the view blocks. In this situation, the view would
		**	never get a refresh call, so we test for this case here, and queue the refresh
		**	here if need be.
		*/
		if( (pNCSFileView->nCacheMethod == NCS_CACHE_VIEW) && (pNCSFileView->pRefreshCallback) ) {
			if( pNCSFileView->info.nBlocksAvailable == pNCSFileView->info.nBlocksInView )		// [02] all blocks are here
				NCScbmQueueIDWTCallback(pNCSFileView, NCSECW_QUEUE_LIFO);
		}
	}

	return;
}
