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
** FILE:   	NCSUtil\pool.c
** CREATED:	Thu Feb 25 09:19:00 WST 1999
** AUTHOR: 	Simon Cope
** PURPOSE:	NCS Memory allocation routines
** EDITS:
** [01] 21Dec99 sjc	Added NULL ptr check
** [02] 31Oct00  ny	Merge WinCE/PALM SDK changes
 *******************************************************/

#include "NCSMemPool.h"

/*
** Prototypes
*/
static void NCSPoolFreeNodeContents(NCSPoolNode *pNode);
static BOOLEAN NCSPoolInitNodeContents(NCSPool *pPool, NCSPoolNode *pNode);
static NCSPoolNode *NCSPoolAddNode(NCSPool *pPool);
static void NCSPoolRemoveNode(NCSPool *pPool, NCSPoolNode *pNode);
static void *NCSPoolGetElement(NCSPool *pPool);
static void NCSPoolFreeElement(NCSPool *pPool, void *pElement);

/*
** Create a new memory pool.
*/
NCSPool *NCSPoolCreate(UINT32 iElementSize, UINT32 nElementsPerNode)
{
	NCSPool *pPool = (NCSPool*)NULL;

	if(NULL != (pPool = (NCSPool*)NCSMalloc(sizeof(NCSPool), TRUE))) {
		pPool->psStats.iElementSize = iElementSize;
		pPool->psStats.nElementsPerNode = nElementsPerNode;
		pPool->nMaxElements = 0; 
		NCSMutexInit(&(pPool->mMutex));
	}
	return(pPool);
}

/*
** Destroy a memory pool.
*/
void NCSPoolDestroy(NCSPool *pPool)
{
	if(pPool) {
		INT32 iNode;

		NCSMutexBegin(&(pPool->mMutex));

		iNode = (INT32)pPool->psStats.nNodes;

		while(iNode-- > 0) {
			NCSPoolRemoveNode(pPool, pPool->pNodes);
		}
		
		NCSMutexEnd(&(pPool->mMutex));
		NCSMutexFini(&(pPool->mMutex));
		NCSFree(pPool);
	}
}

/*
** Set pool max elements
*/
void NCSPoolSetMaxSize(NCSPool *pPool, 
					   UINT32 nMaxElements)
{
	if(pPool) {
		pPool->nMaxElements = nMaxElements;
	}
}
/*
** Alloc an element in the pool.
*/
void *NCSPoolAlloc(NCSPool *pPool, BOOLEAN bClear)
{
	void *pData;
	NCSTimeStampMs tsStart = 0;
	
	if(pPool->bCollectStats) {
		tsStart = NCSGetTimeStampMs();
	}

	NCSMutexBegin(&(pPool->mMutex));

	if(NULL != (pData = NCSPoolGetElement(pPool))) {
		if(bClear) {
			NCSMemSet(pData, 0, pPool->psStats.iElementSize);
		}
	} else {
		pData = NCSMalloc(pPool->psStats.iElementSize, bClear);
	}
	if(pPool->bCollectStats) {
		pPool->psStats.nAllocElements += 1;
		pPool->psStats.tsAllocElementTime += NCSGetTimeStampMs() - tsStart;
	}
	NCSMutexEnd(&(pPool->mMutex));
	
	return(pData);
}

/*
** Free an element in the pool.
*/
void NCSPoolFree(NCSPool *pPool, void *pPtr)
{
	if(pPtr) {
		NCSTimeStampMs tsStart = 0;
		
		if(pPool->bCollectStats) {
			tsStart = NCSGetTimeStampMs();
		}
		NCSMutexBegin(&(pPool->mMutex));

		NCSPoolFreeElement(pPool, pPtr);

		if(pPool->bCollectStats) {
			pPool->psStats.nFreeElements += 1;
			pPool->psStats.tsFreeElementTime += NCSGetTimeStampMs() - tsStart;
		}
		NCSMutexEnd(&(pPool->mMutex));
	}
}

/*
** Get pool stats.
*/
NCSPoolStats NCSPoolGetStats(NCSPool *pPool)
{
	pPool->psStats.msPool = NCSMutexGetStats(&(pPool->mMutex));
	return(pPool->psStats);
}

/*
** Enable Pool stats collection
*/
void NCSPoolEnableStats(NCSPool *pPool)
{
	pPool->bCollectStats = TRUE;
	NCSMutexEnableStats(&(pPool->mMutex));
}

/*
** Disable Pool stats collection
*/
void NCSPoolDisableStats(NCSPool *pPool)
{
	pPool->bCollectStats = FALSE;
	NCSMutexDisableStats(&(pPool->mMutex));
}

/*
** Static local utility functions
*/

static BOOLEAN NCSPoolInitNodeContents(NCSPool *pPool, NCSPoolNode *pNode)
{
	if(pNode) {
		pNode->iLastFreeElement = 0;
		if(NULL != (pNode->pElements = (void*)NCSMalloc(pPool->psStats.nElementsPerNode * pPool->psStats.iElementSize, FALSE))) {
			if(NULL != (pNode->pbElementInUse = (BOOLEAN*)NCSMalloc(pPool->psStats.nElementsPerNode * sizeof(BOOLEAN), TRUE))) {
				return(TRUE);
			}
			NCSPoolFreeNodeContents(pNode);
		}
	}
	return(FALSE);
}

static void NCSPoolFreeNodeContents(NCSPoolNode *pNode)
{
	if(pNode) {
		NCSFree(pNode->pbElementInUse);
		NCSFree(pNode->pElements);
	}
}

static NCSPoolNode *NCSPoolAddNode(NCSPool *pPool)
{
	NCSPoolNode *pNode = (NCSPoolNode*)NULL;
	NCSTimeStampMs tsStart = 0;
	
	if(pPool->bCollectStats) {
		tsStart = NCSGetTimeStampMs();
	}
	NCSArrayAppendElement(pPool->pNodes, pPool->psStats.nNodes, (NCSPoolNode*)NULL);
	
	pNode = &(pPool->pNodes[pPool->psStats.nNodes - 1]);

	if(NCSPoolInitNodeContents(pPool, pNode)) {
		if(pNode->pElements && pNode->pbElementInUse) {
			if(pPool->bCollectStats) {
				pPool->psStats.nAddNodes++;
				pPool->psStats.tsAddNodeTime += NCSGetTimeStampMs() - tsStart;
			}
		} else {
			NCSPoolRemoveNode(pPool, pNode);
			pNode = (NCSPoolNode*)NULL;
		}
	}
	return(pNode);
}

static void NCSPoolRemoveNode(NCSPool *pPool, NCSPoolNode *pNode)
{
	if(pNode) {
		UINT32 iNode;
		NCSTimeStampMs tsStart = 0;
		
		if(pPool->bCollectStats) {
			tsStart = NCSGetTimeStampMs();
		}
		NCSPoolFreeNodeContents(pNode);

		for(iNode = 0; iNode < pPool->psStats.nNodes; iNode++) {
			if(pNode == &(pPool->pNodes[iNode])) {
				NCSArrayRemoveElement(pPool->pNodes, pPool->psStats.nNodes, iNode);

				if(pPool->bCollectStats) {
					pPool->psStats.nRemoveNodes++;
				}
				break;
			}
		}
		if(pPool->bCollectStats) {
			pPool->psStats.tsRemoveNodeTime += NCSGetTimeStampMs() - tsStart;
		}
	}
}

static void *NCSPoolGetElement(NCSPool *pPool)
{
	UINT32 iNode;
	NCSPoolNode *pNode;

	if((pPool->nMaxElements != 0) && 
	   (pPool->psStats.nElementsInUse >= pPool->nMaxElements)) {
		return((void*)NULL);
	}
	for(iNode = 0; iNode < pPool->psStats.nNodes; iNode++) {
#ifdef NCS_64BIT
		INT64 iElement;
#else
		INT32 iElement;
#endif
		INT32 nElements;
		BOOLEAN *pbElementInUse;

		pNode = &(pPool->pNodes[iNode]);
		nElements = (INT32)pPool->psStats.nElementsPerNode;

		if(pNode->nElementsInUse == nElements) {
			continue;
		}
		pbElementInUse = pNode->pbElementInUse;

		for(iElement = pNode->iLastFreeElement; iElement < nElements; iElement++) {
			if(pbElementInUse[iElement] == FALSE) {
				pbElementInUse[iElement] = TRUE;
				pNode->nElementsInUse += 1;
				pNode->iLastFreeElement = iElement;

				return((void*)&(((UINT8*)(pNode->pElements))[iElement * pPool->psStats.iElementSize]));
			}
		}
	}
	if(NULL != (pNode = NCSPoolAddNode(pPool))) {
		pNode->pbElementInUse[0] = TRUE;
		pNode->nElementsInUse += 1;

		return(pNode->pElements);
	}
	return((void*)NULL);
}


static void NCSPoolFreeElement(NCSPool *pPool, void *pElement)
{
	UINT32 iNode;
	NCSPoolNode *pNode;

	for(iNode = 0; iNode < pPool->psStats.nNodes; iNode++) {
#ifdef NCS_64BIT
		INT64 iElement;
#else
		INT32 iElement;
#endif
		INT32 nElements;

		pNode = &(pPool->pNodes[iNode]);
		nElements = (INT32)pPool->psStats.nElementsPerNode;

		if((pElement >= pNode->pElements) && (
				(UINT8*)pElement < (UINT8*)pNode->pElements + pPool->psStats.iElementSize * nElements)) {
#ifdef NCS_64BIT
			iElement = (INT64)((INT64)pElement - (INT64)pNode->pElements) / (INT64)pPool->psStats.iElementSize;
#else
			iElement = (INT32)((INT32)pElement - (INT32)pNode->pElements) / (INT32)pPool->psStats.iElementSize;
#endif

			pNode->pbElementInUse[iElement] = FALSE;

			pNode->nElementsInUse -= 1;
			pNode->iLastFreeElement = MIN(iElement, pNode->iLastFreeElement);

			/*
			** Leave at least 1 node in the pool at all time, in case we're pumped empty.
			** It'll get freed on the NCSPoolDestroy();
			*/
			if((pNode->nElementsInUse == 0) && (pPool->psStats.nNodes > 1)) {
				NCSPoolRemoveNode(pPool, pNode);
			}
			pElement = (void*)NULL;
			break;
		}
	}
	NCSFree(pElement);
}
