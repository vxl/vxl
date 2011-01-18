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
** PURPOSE:	NCS Queue header
** EDITS:
 *******************************************************/

#ifndef NCSQUEUE_H
#define NCSQUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NCSTYPES_H
#include "NCSTypes.h"
#endif
#ifndef NCSDEFS_H
#include "NCSDefs.h"
#endif
#ifndef NCSMUTEX_H
#include "NCSMutex.h"
#endif
#ifndef NCSMEMPOOL_H
#include "NCSMemPool.h"
#endif

/*
** Queue Structures
*/
typedef struct NCSQueueNode {
	struct NCSQueueNode *pPrev;
	struct NCSQueueNode *pNext;
} NCSQueueNode;

typedef struct {
	UINT32				nNodes;
	UINT32				iNodeSize;
	UINT32				nPeakNodes;
	UINT32				nAppends;
	UINT32				nInserts;
	UINT32				nRemoves;
	NCSTimeStampMs		tsAppendTime;
	NCSTimeStampMs		tsInsertTime;
	NCSTimeStampMs		tsRemoveTime;
	NCSMutexStats		msQueue;
	NCSPoolStats		stPool;
} NCSQueueStats;

typedef struct {
	NCSMutex			mMutex;
	NCSQueueStats		qsStats;
	BOOLEAN				bCollectStats;
	struct NCSQueueNode *pFirst;
	struct NCSQueueNode *pLast;
	NCSPool				*pPool;
	BOOLEAN				bOurPool;
} NCSQueue;

/*
** Queue Macros
*/
#define NCSQueueFirstNode(pQueue)	(pQueue)->pFirst
#define NCSQueueLastNode(pQueue)	(pQueue)->pLast
#define NCSQueueNextNode(pCurr)		(pCurr)->pNext
#define NCSQueuePrevNode(pCurr)		(pCurr)->pPrev
#define NCSQueueGetNrNodes(pQueue)	(pQueue)->qsStats.nNodes
#define NCSQueueLock(pQueue)		NCSMutexBegin(&((pQueue)->mMutex))
#define NCSQueueUnLock(pQueue)		NCSMutexEnd(&((pQueue)->mMutex))

/* 
** Queue Prototypes
*/
NCSQueue *NCSQueueCreate(NCSPool *pPool, UINT32 iQueueStructSize, UINT32 iQueueNodeSize);
void NCSQueueDestroy(NCSQueue *pQueue);
NCSQueueNode *NCSQueueCreateNode(NCSQueue *pQueue);
void NCSQueueDestroyNode(NCSQueue *pQueue, NCSQueueNode *pNode);
void NCSQueueAppendNode(NCSQueue *pQueue, NCSQueueNode *pNode);
void NCSQueueInsertNode(NCSQueue *pQueue, NCSQueueNode *pNode, NCSQueueNode *pCurr);
NCSQueueNode *NCSQueueRemoveNode(NCSQueue *pQueue, NCSQueueNode *pNode);
NCSQueueStats NCSQueueGetStats(NCSQueue *pQueue);
void NCSQueueEnableStats(NCSQueue *pQueue);
void NCSQueueDisableStats(NCSQueue *pQueue);

#ifdef __cplusplus
}
#endif

#endif /* NCSQUEUE_H */
