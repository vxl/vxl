/********************************************************
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
** FILE:   	NCSQueue.h
** CREATED:	Tue Mar 2 09:19:00 WST 1999
** AUTHOR: 	Simon Cope
** PURPOSE:	NCS Queue routines
** EDITS:
 *******************************************************/

#include "NCSQueue.h"

/*
** Create a Queue
*/
NCSQueue *NCSQueueCreate(NCSPool *pPool, UINT32 iQueueStructSize, UINT32 iQueueNodeSize)
{
	NCSQueue *pQueue;

	if((pQueue = (NCSQueue*)NCSMalloc(iQueueStructSize, TRUE)) != NULL) {
		
		NCSMutexInit(&(pQueue->mMutex));

		pQueue->qsStats.iNodeSize = iQueueNodeSize;
		if(pPool) {
			pQueue->pPool = pPool;
		} else {
			pQueue->pPool = NCSPoolCreate(iQueueNodeSize, 1024);
			pQueue->bOurPool = TRUE;
		}
		if(pQueue->pPool) {
			return(pQueue);
		} else {
			NCSQueueDestroy(pQueue);
		}
	}
	return((NCSQueue*)NULL);
}

/*
** Destroy a Queue
*/
void NCSQueueDestroy(NCSQueue *pQueue)
{
	if(pQueue) {
		while(NCSQueueFirstNode(pQueue)) {
			NCSQueueRemoveNode(pQueue, NCSQueueFirstNode(pQueue));
		}

		if(pQueue->pPool && pQueue->bOurPool) {
			NCSPoolDestroy(pQueue->pPool);
		}
		NCSMutexFini(&(pQueue->mMutex));
		NCSFree(pQueue);
	}
}

/*
** Create a QueueNode
*/
NCSQueueNode *NCSQueueCreateNode(NCSQueue *pQueue)
{
	return((NCSQueueNode*)NCSPoolAlloc(pQueue->pPool, FALSE));
}

/*
** Destroy a QueueNode
*/
void NCSQueueDestroyNode(NCSQueue *pQueue, NCSQueueNode *pNode)
{
	NCSPoolFree(pQueue->pPool, pNode);
}

/*
** Append a QueueNode onto the end of the Queue
*/
void NCSQueueAppendNode(NCSQueue *pQueue, NCSQueueNode *pNode)
{
	NCSTimeStampMs tsStart = 0;
	
	if(pQueue->bCollectStats) {
		tsStart = NCSGetTimeStampMs();
	}
	NCSQueueLock(pQueue);

	if(pQueue->pLast) {
		pQueue->pLast->pNext = pNode;
		pNode->pPrev = pQueue->pLast;
		pNode->pNext = (NCSQueueNode*)NULL;
		pQueue->pLast = pNode;
	} else {
		pQueue->pFirst = pNode;
		pQueue->pLast = pNode;
		pNode->pPrev = (NCSQueueNode*)NULL;
		pNode->pNext = (NCSQueueNode*)NULL;
	}
	pQueue->qsStats.nNodes += 1;

	if(pQueue->bCollectStats) {
		pQueue->qsStats.nPeakNodes = MAX(pQueue->qsStats.nNodes, pQueue->qsStats.nPeakNodes);
		pQueue->qsStats.nAppends += 1;
		pQueue->qsStats.tsAppendTime += NCSGetTimeStampMs() - tsStart;
	}
	NCSQueueUnLock(pQueue);
}

/*
** Insert a QueueNode into the Queue after the given node
** NOTE: Queue must be locked around this call to ensure pCurr is valid!!!
*/
void NCSQueueInsertNode(NCSQueue *pQueue, NCSQueueNode *pNode, NCSQueueNode *pCurr)
{
	NCSTimeStampMs tsStart = 0;
	
	if(pQueue->bCollectStats) {
		tsStart = NCSGetTimeStampMs();
	}

	pNode->pPrev = pCurr->pPrev;
	if(pNode->pPrev) {
		pNode->pPrev->pNext = pNode;
	}
	pCurr->pPrev = pNode;
	pNode->pNext = pCurr;

	if(pCurr == NCSQueueFirstNode(pQueue)) {
		pQueue->pFirst = pNode;
	}
	
	pQueue->qsStats.nNodes += 1;

	if(pQueue->bCollectStats) {
		pQueue->qsStats.nPeakNodes = MAX(pQueue->qsStats.nNodes, pQueue->qsStats.nPeakNodes);
		pQueue->qsStats.nInserts += 1;
		pQueue->qsStats.tsInsertTime += NCSGetTimeStampMs() - tsStart;
	}
}

/*
** Remove a QueueNode from the Queue.
** Removes FIRST node if pNode is NULL.
*/
NCSQueueNode *NCSQueueRemoveNode(NCSQueue *pQueue, NCSQueueNode *pNode)
{
	NCSTimeStampMs tsStart = 0;
	
	if(pQueue->bCollectStats) {
		tsStart = NCSGetTimeStampMs();
	}
	NCSQueueLock(pQueue);

	if(NCSQueueFirstNode(pQueue)) {
		if(!pNode) {
			pNode = NCSQueueFirstNode(pQueue);
			pQueue->pFirst = pNode->pNext;
		} else if(pNode == NCSQueueFirstNode(pQueue)) {
			pQueue->pFirst = pNode->pNext;
		}
		if(pNode == NCSQueueLastNode(pQueue)) {
			pQueue->pLast = pNode->pPrev;
		}

		if(pNode->pNext) {
			pNode->pNext->pPrev = pNode->pPrev;
		}
		if(pNode->pPrev) {
			pNode->pPrev->pNext = pNode->pNext;
		}
		pNode->pPrev = (NCSQueueNode*)NULL;
		pNode->pNext = (NCSQueueNode*)NULL;

		pQueue->qsStats.nNodes -= 1;
	} else {
		pNode = (NCSQueueNode*)NULL;
	}
	
	if(pQueue->bCollectStats) {
		pQueue->qsStats.nRemoves += 1;
		pQueue->qsStats.tsRemoveTime += NCSGetTimeStampMs() - tsStart;
	}
	NCSQueueUnLock(pQueue);

	return(pNode);
}

/*
** Get the Queue's Stats.
*/
NCSQueueStats NCSQueueGetStats(NCSQueue *pQueue)
{
	pQueue->qsStats.msQueue = NCSMutexGetStats(&(pQueue->mMutex));
	pQueue->qsStats.stPool = NCSPoolGetStats(pQueue->pPool);
	return(pQueue->qsStats);
}

/*
** Enable Queue stats collection
*/
void NCSQueueEnableStats(NCSQueue *pQueue)
{
	pQueue->bCollectStats = TRUE;
	NCSMutexEnableStats(&(pQueue->mMutex));
	NCSPoolEnableStats(pQueue->pPool);
}

/*
** Disable Queue stats collection
*/
void NCSQueueDisableStats(NCSQueue *pQueue)
{
	pQueue->bCollectStats = FALSE;
	NCSMutexDisableStats(&(pQueue->mMutex));
	NCSPoolDisableStats(pQueue->pPool);
}
