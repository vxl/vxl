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
** PURPOSE:	NCS Memory handling routines.
** EDITS:
** [01] sjc 30Apr00 Merged Mac SDK port
** [02] sjc 15Jan02 Solaris port mods
** [03] tfl 02Jul04 Added util function to convert enum cell type to cell size
 *******************************************************/

#ifndef NCSMALLOC_H
#define NCSMALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NCSTYPES_H
#include "NCSTypes.h"
#endif

#ifdef MACINTOSH

#include <stdlib.h>
#include <string.h>

#elif defined MACOSX
#include <stdlib.h>
#include <string.h>

#else /* MACINTOSH */

#include <malloc.h>

#if defined SOLARIS || defined LINUX || defined HPUX
#include <string.h>
#include <alloca.h>
#endif /* MACINTOSH */

#endif // SOLARIS

typedef struct {
#ifdef WIN32
	HANDLE hMem;
	HANDLE hSize;
#endif
	void	*pData;
	UINT64  nSize;
} NCSSHMInfo;

/*
** Prototypes.
*/
void NCSMallocInit(void);
void NCSMallocFini(void);
void *NCSMalloc(UINT32 iSize, BOOLEAN bClear);
void *NCSRealloc(void *pPtr, UINT32 iSize, BOOLEAN bClear);
void NCSFree(void *pPtr);
INT32 NCSPhysicalMemorySize(void);
INT32 NCSCellSize(NCSEcwCellType eCellType);
UINT16 NCSByteSwap16(UINT16 n);
UINT32 NCSByteSwap32(UINT32 n);
UINT64 NCSByteSwap64(UINT64 n);
void NCSByteSwapRange16(UINT16 *pDst, UINT16 *pSrc, INT32 nValues);
void NCSByteSwapRange32(UINT32 *pDst, UINT32 *pSrc, INT32 nValues);
void NCSByteSwapRange64(UINT64 *pDst, UINT64 *pSrc, INT32 nValues);

#ifndef PALM
#define NCSMemSet(p, v, c)	memset((p), (v), (c))
#define NCSMemMove(d, s, c)	memmove((d), (s), (c))
#else	/* PALM */
#define NCSMemSet(p, v, c)	MemSet((void*)(p), (Int32)(c), (UInt8)(v))
#define NCSMemMove(d, s, c)	MemMove((void*)(d), (const void*)(s), (Int32)(c))
#endif	/* PALM */

/*
** Shared memory
*/
NCSSHMInfo *NCSMapSHM(UINT64 nMapSize, BOOLEAN bCreateUnique, char *pMapName);
void NCSUnmapSHM(NCSSHMInfo *pInfo);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif	/* NCSMALLOC_H */
