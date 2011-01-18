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
** FILE:   	NCSMalloc.h
** CREATED:	Thu Feb 25 09:19:00 WST 1999
** AUTHOR: 	Simon Cope
** PURPOSE:	NCS Memory Pool types and protos.
** EDITS:
 *******************************************************/

#ifndef NCSMEMPOOL_H
#define NCSMEMPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NCSDEFS_H
#include "NCSDefs.h"
#endif
#ifndef NCSARRAY_H
#include "NCSArray.h"
#endif
#ifndef NCSMUTEX_H
#include "NCSMutex.h"
#endif
#ifndef NCSTIMESTAMP_H
#include "NCSTimeStamp.h"
#endif
#ifndef NCSTYPES_H
#include "NCSTypes.h"
#endif

/*
** Typedefs
*/

typedef struct {
	INT32 nElementsInUse;
#ifdef NCS_64BIT
	INT64 iLastFreeElement;
#else
	INT32 iLastFreeElement;
#endif
	void *pElements;
	BOOLEAN *pbElementInUse;
} NCSPoolNode;

typedef struct {
	UINT32 iElementSize;
	UINT32 nElementsPerNode;
	UINT32 nNodes;
	UINT32 nElementsInUse;

	UINT32 nAddNodes;
	NCSTimeStampMs tsAddNodeTime;
	
	UINT32 nRemoveNodes;
	NCSTimeStampMs tsRemoveNodeTime;
	
	UINT32 nAllocElements;
	NCSTimeStampMs tsAllocElementTime;
	
	UINT32 nFreeElements;
	NCSTimeStampMs tsFreeElementTime;

	NCSMutexStats msPool;
} NCSPoolStats;

typedef struct {
	NCSMutex		mMutex;
	NCSPoolStats	psStats;
	UINT32			nMaxElements;
	BOOLEAN			bCollectStats;
	NCSPoolNode		*pNodes;
} NCSPool;

/*
** Prototypes.
*/
NCSPool *NCSPoolCreate(UINT32 iElementSize, UINT32 nElements);
void *NCSPoolAlloc(NCSPool *pPool, BOOLEAN bClear);
void NCSPoolFree(NCSPool *pPool, void *pPtr);
void NCSPoolDestroy(NCSPool *pPool);
void NCSPoolSetMaxSize(NCSPool *pPool, UINT32 nMaxElements);
NCSPoolStats NCSPoolGetStats(NCSPool *pPool);
void NCSPoolEnableStats(NCSPool *pPool);
void NCSPoolDisableStats(NCSPool *pPool);

#ifdef __cplusplus
}
#endif

#endif /* NCSMEMPOOL_H */
