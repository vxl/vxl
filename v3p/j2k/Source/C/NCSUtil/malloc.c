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
** FILE:   	NCSUtil\malloc.c
** CREATED:	Thu Feb 25 09:19:00 WST 1999
** AUTHOR: 	Simon Cope
** PURPOSE:	NCS Memory allocation routines
** EDITS:
**	[01] 23/09/99	mjb	Added test for 98 before running SetSecurityInfo
**	[02] 31Oct00	 ny	Merge WinCE/PALM SDK changes
**  [03] 06Jan01	sjc Don't call SetSecurityInfo() when purifying
**  [04] 18Jan01    jmp Don't load advapi32.dll for WIN9X, as FreeLibrary
**					    function misbehaves when called from within DllMain.
**  [05] 19Jan01	rar Mac port changes
**	[06] 01Jun04	tfl Changed NCSPhysicalMemorySize to cap output
**  [07] 02Jul04	tfl Added util function to convert cell type enum to cell size in bytes
**	[08] 01Apr05	tfl	Removed various Purify references
 *******************************************************/

#include "NCSMalloc.h"
#if defined(WIN32) && !defined(_WIN32_WCE)
#include <aclapi.h>
#endif

#ifdef NCSMALLOCLOG
#	include "NCSLog.h"
#endif

#include "NCSDefs.h"
#include "NCSMisc.h"
#include "NCSUtil.h"

#undef ONE_MEG
#define ONE_MEG (1024 * 1024)
#undef ONE_GIG
#define ONE_GIG (1024I64 * 1024 * 1024) //[06]

#ifdef MACINTOSH
#include <stdio.h>

static void *MACmalloc( size_t cb );
static void *MACcalloc( size_t c, size_t cb );
static void *MACrealloc(void *pPtr, size_t cb, BOOLEAN bClear);
static void MACfree(void *pPtr);

#elif defined PALM

typedef struct {
	MemHandle 	hRecord;
	UInt16		Index;
} PalmHeapHeader;

static UInt16 nPalmHeapCardId = 0;
static LocalID idPalmHeap = 0;
static DmOpenRef refPalmHeap = 0;

#elif defined POSIX

#include <unistd.h>

#endif

void NCSMallocInit(void)
{
#ifdef NOTDEF
PALM
	if(DmCreateDatabase(nPalmHeapCardId, "NCS DynamicHeap", NCS_PALM_CREATOR_ID, 'NCSH', FALSE) == errNone) {
		if(idPalmHeap = DmFindDatabase(nPalmHeapCardId, "NCS DynamicHeap")) {
			refPalmHeap = DmOpenDatabase(nPalmHeapCardId, idPalmHeap, dmModeReadWrite);
		}
	}
#endif
}

void NCSMallocFini(void)
{
#ifdef PALM
	if(refPalmHeap != 0) {
		DmCloseDatabase(refPalmHeap);
	}
	if(idPalmHeap) {
		DmDeleteDatabase(nPalmHeapCardId, idPalmHeap);
	}
#endif
}

#ifdef MACINTOSH  //[05]
void *vmalloc( size_t cb )
	{
	void	*p = malloc( cb );
	if(!p)
		{
		//
		//	Out of Memory --
		//
		printf( "Out of Memory\n");	// remove and handle error better here
		CompactMem( cb );
		PurgeMem( cb );
		p = malloc( cb );
		if(!p)
			{
			//	worse case exit
			printf( "Out of Memory 2\n");
			//ExitToShell();	// nothing we can do!
			return NULL;
			}
		}
	return p;
	}
void *vcalloc( size_t c, size_t cb )
	{
	void	*p = calloc( c, cb );
	if(!p)
		{
		//
		//	Out of Memory --
		//
		//printf( "Out of Memory\n");	// remove and handle error better here
		CompactMem( cb*c );
		PurgeMem( cb*c );
		p = calloc( c, cb );
		if(!p)
			{
			//	worse case exit
			//ExitToShell();	// nothing we can do!
			return NULL;
			}
		}
	return p;
	}
#endif

/*
** Malloc some memory of given size, clear if requested, and return pointer.
*/
void *NCSMalloc(UINT32 iSize, BOOLEAN bClear)
{
	void *pMem=NULL;

	if(iSize > 0) {
		if(bClear) {
			/*
			** Return a calloc'd block.
			*/
#ifdef MACINTOSH
			return(MACcalloc(iSize, 1));
#elif defined PALM
			void *p = (void*)NULL;
			if(refPalmHeap != 0) {
				MemHandle h;
				UInt16 Index = dmMaxRecordIndex;
				PalmHeapHeader *pHdr;
				
				h = DmNewRecord(refPalmHeap, &Index, iSize + sizeof(PalmHeapHeader));
				if(h != 0) {
					pHdr = (PalmHeapHeader*)MemHandleLock(h);
					if(pHdr != NULL) {
						pHdr->hRecord = h;
						pHdr->Index = Index;
						
						p = ((UINT8*)pHdr) + sizeof(PalmHeapHeader);
					} 
				}
			} else {
				p = MemPtrNew(iSize);
			}
			if(p) {
				MemSet(p, (Int32)iSize, 0);
			} else {
				NCSGetLastErrorTextMsgBox(NCS_COULDNT_ALLOC_MEMORY, 0);
			}
			return(p);

#elif defined _WIN32_WCE && (_WIN32_WCE <= 211)
			void *p = malloc(iSize);
			if(p) {
				memset(p, 0, iSize);
			}
			return(p);
#else
			pMem = ((void*)calloc(1, iSize));
			if( pMem ) return pMem;
#ifdef NCSMALLOCLOG
			else NCSLog(LOG_LOW, "NCSMalloc: calloc failed (%d bytes).", iSize); //[06]
#endif //NCSMALLOCLOG

#endif	/* MACINTOSH */
		} else {
			/*
			** Return a malloc'd block.
			*/
#ifdef MACINTOSH
			return(MACmalloc(iSize));
#elif defined PALM
			void *p;
			if(refPalmHeap != 0) {
				MemHandle h;
				UInt16 Index = dmMaxRecordIndex;
				
				h = DmNewRecord(refPalmHeap, &Index, iSize + sizeof(PalmHeapHeader));
				if(h != 0) {
					p = MemHandleLock(h);
					
					if(p != NULL) {
						((PalmHeapHeader*)p)->hRecord = h;
						((PalmHeapHeader*)p)->Index = Index;
						
						(UInt8*)p += sizeof(PalmHeapHeader);
					} 
				}
			} else {
				p = MemPtrNew(iSize);
			}
			return(p);
#else	/* PALM */

			pMem = ((void*)malloc(iSize));
			if( pMem ) return pMem;
#ifdef NCSMALLOCLOG
			else NCSLog(LOG_LOW, "NCSMalloc: malloc failed (%d bytes).", iSize); //[06]
#endif //NCSMALLOCLOG

#endif	/* MACINTOSH */
		}
	}
	return((void*)NULL);
}

/*
** Realloc a chunk of memory to given size, clear extra if requested and lager, 
** return new pointer.  If pPtr is NULL, Malloc a new chunk.
*/
void *NCSRealloc(void *pPtr, UINT32 iSize, BOOLEAN bClear)
{
	if(pPtr) {
		if(iSize > 0) {
			UINT32 iOldSize;
			void *pNew;
			/* Get the old size */
#ifdef POSIX
			// No way to determine old size under Solaris libc
			iOldSize = iSize;
#else
			iOldSize = (UINT32)_msize(pPtr);
#endif
			/* realloc block */
			pNew = realloc(pPtr, iSize);
#ifdef NCSMALLOCLOG
			if( !pNew  ) NCSLog(LOG_LOW, "NCSRealloc: realloc failed (%d bytes).", iSize); //[06]
#endif //NCSMALLOCLOG

			/* if realloc ok, clear end of new block if bigger than old block */
			if(pNew && bClear && (iSize > iOldSize)) {
				memset((UINT8*)pNew + iOldSize, 0, iSize - iOldSize);
			}
			return(pNew);
		} else {
			NCSFree(pPtr);
		}
	} else {
		/* NCSMalloc() a block */
		void *pMem=NULL;
		pMem = ((void*)malloc(iSize));
#ifdef NCSMALLOCLOG
		if( !pMem ) NCSLog(LOG_LOW, "NCSMalloc: malloc failed (%d bytes).", iSize); //[06]
#endif //NCSMALLOCLOG
		return(pMem);
	}
	return((void*)NULL);
}

/*
** Free (return to heap) a chunk of memory.
*/
void NCSFree(void *pPtr)
{
	if(pPtr) {
#ifdef MACINTOSH
		MACfree(pPtr);
#elif defined PALM
		if(refPalmHeap != 0) {
			MemHandle h;
			UInt16 Index;
				
			(UInt8*)pPtr -= sizeof(PalmHeapHeader);
			
			h = ((PalmHeapHeader*)pPtr)->hRecord;
			Index = ((PalmHeapHeader*)pPtr)->Index;
		
			MemHandleUnlock(h);
			DmRemoveRecord(refPalmHeap, Index); 		
		} else {
			MemPtrFree(pPtr);
		}
#else	/* PALM */
		free(pPtr);
#endif	/* MACINTOSH */
	}
}
#include <stdio.h>
/*
** Return the amount of physical memory
*/
INT32 NCSPhysicalMemorySize(void)
{
#ifdef WIN32
	INT64 nTotalPhysicalMemory = 0;
	
#ifndef _WIN32_WCE
	HMODULE hLib = LoadLibraryA("kernel32.dll");
	if(hLib) {
		FARPROC pProc = GetProcAddress(hLib,"GlobalMemoryStatusEx");

		if(pProc) {
			MEMORYSTATUSEX MemoryStatus;
			MemoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
			pProc(&MemoryStatus);
			nTotalPhysicalMemory = MemoryStatus.ullTotalPhys;
		}
		FreeLibrary(hLib);
	}
#endif // _WIN32_WCE
	if(nTotalPhysicalMemory == 0) {
		MEMORYSTATUS MemoryStatus;
		MemoryStatus.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&MemoryStatus);
		/* return the amount of physical memory available (free) */
		/* round up to 4MB boundary to account for kernel memory */
		//[06] Cap return value at 2GB to avoid INT32 overflow issues
		//Can't extend this function to return INT64 due to backwards compatibility concerns
		nTotalPhysicalMemory = MemoryStatus.dwTotalPhys;
	}
	if(nTotalPhysicalMemory % (4 * ONE_MEG)) {
		nTotalPhysicalMemory /= (4 * ONE_MEG);
		nTotalPhysicalMemory++;
	} else {
		nTotalPhysicalMemory /= (4 * ONE_MEG);
	}
	nTotalPhysicalMemory *= (4 * ONE_MEG);
	nTotalPhysicalMemory = (nTotalPhysicalMemory >= (2 * ONE_GIG))?(2 * ONE_GIG - 1):nTotalPhysicalMemory;

	return (INT32)nTotalPhysicalMemory;
	//end [06]

#else	/* WIN32 */
#ifdef MACINTOSH

	INT32  contigiousSpace;
	INT32	memorySpace;
	
	//	Don't ask me why this is named "Purge."  It doesn't purge anything.
	//PurgeSpace(&memorySpace,&contigiousSpace);
	
	contigiousSpace = TempMaxMem(&memorySpace);

	return (contigiousSpace/2) ;	

#else	/* MACINTOSH */
#ifdef IRIX

	inventory_t *p_invent;
	UINT32 bytes = 0;

	setinvent();
	while(p_invent = getinvent()) {
		if((p_invent->inv_class == INV_MEMORY) &&
		   (p_invent->inv_type == INV_MAIN)) {
			bytes += p_invent->inv_state;
		}
	}
	endinvent();
	bytes += 4096;  /* for Onyx */
	return(bytes);
	
#else	/* IRIX */
#ifdef SOLARIS
	
	return(sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE));

#else	/* SOLARIS */

#ifdef LINUX
	return(sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE));
#elif defined MACOSX
	return(128*1024*1024);

#else

#ifdef HPUX
#include <sys/param.h>
#include <sys/pstat.h>

	struct pst_static pss;
	if (pstat_getstatic (&pss, sizeof(pss), 1, 0) >= 0) {

//fprintf(stderr, "Memory : %lld\n", pss.physical_memory * pss.page_size);

		if (pss.physical_memory > (LONG_MAX/2)/pss.page_size)
			return LONG_MAX/2;
		else
			return pss.physical_memory * pss.page_size;

	}
	// Apparently on older systems, pstat may fail, so guess there is 64 Mb.
	return 64 * 1024 * 1024;
#else

#ifdef PALM
	
	UInt16 i;
	UInt32 nTotalRam = 0;
	
	for(i = 0; i < MemNumCards(); i++) {
		UInt32 nThisCardRam = 0;
		 
		if(MemCardInfo(i, 0, 0, 0, 0, 0, &nThisCardRam, 0) == errNone) {
			nTotalRam += nThisCardRam;
		} 
	}
	return((INT32)nTotalRam);
	
#else	/* PALM */
#error - NCSPhysicalMemorySize()
#endif	/* PALM */
#endif	/* SOLARIS */
#endif	/* IRIX */
#endif	/* MACINTOSH */
#endif	/* WIN32 */
#endif	/* WIN32 */
#endif	/* WIN32 */


}

/*
** Convert an NCSEcwCellType enum value into a cell sample size in bytes
** - returns 0 if cell type is unrecognised
*/
INT32 NCSCellSize(NCSEcwCellType eCellType)
{
	switch (eCellType)
	{
		case NCSCT_UINT8: return 1;
		case NCSCT_INT8: return 1;
		case NCSCT_UINT16: return 2;
		case NCSCT_INT16: return 2;
		case NCSCT_UINT32: return 4;
		case NCSCT_INT32: return 4;
		case NCSCT_IEEE4: return 4;
		case NCSCT_UINT64: return 8;
		case NCSCT_INT64: return 8;
		case NCSCT_IEEE8: return 8;
		default: return 0;
	}
}


/*
** Map SHM
*/
NCSSHMInfo *NCSMapSHM(UINT64 nMapSize,
  					  BOOLEAN bCreateUnique,
					  char *pMapName)
{
#ifdef WIN32
	HANDLE hSize = NULL;
	char szName[MAX_PATH];
	char szSizeName[MAX_PATH];

	strcpy(szName, pMapName);
	strcpy(szSizeName, pMapName);
	strcat(szSizeName, "SIZE");
#ifdef _DEBUG
	strcat(szName, "DEBUG");
	strcat(szSizeName, "DEBUG");
#endif

#ifdef WIN32
	hSize = CreateFileMapping(INVALID_HANDLE_VALUE,
							  (LPSECURITY_ATTRIBUTES)NULL,
							  (DWORD)PAGE_READWRITE|SEC_COMMIT,
							  (DWORD)0,
							  (DWORD)sizeof(UINT64),
							  (LPCTSTR)szSizeName);
#else
	hSize = CreateFileMapping(INVALID_HANDLE_VALUE,	
							  (LPSECURITY_ATTRIBUTES)NULL,
							  (DWORD)PAGE_READWRITE|SEC_COMMIT,
							  (DWORD)0,
							  (DWORD)sizeof(UINT64),
							  (unsigned short *)szSizeName);	/**[02]**/
#endif
	if(hSize) {
		UINT64 *pSize;
		BOOLEAN bExists = FALSE;

		if(GetLastError() == ERROR_ALREADY_EXISTS) {
			bExists = TRUE;
		}

		if(bCreateUnique && bExists) {
			CloseHandle(hSize);
			return((NCSSHMInfo*)NULL);
		}
	
		pSize = (UINT64*)MapViewOfFile(hSize, 
									   FILE_MAP_ALL_ACCESS,
									   0, 0, 0);
		if(pSize) {
			HANDLE hMem;
			DWORD dwError;

			if(bExists) {
				/* Already mapped, get size of SHM */
				nMapSize = *pSize;
			} else {
				/* Not mapped, set size of SHM */
				*pSize = nMapSize;
			}
			UnmapViewOfFile(pSize);

			if(nMapSize != 0) {
#ifdef WIN32
				hMem = CreateFileMapping(INVALID_HANDLE_VALUE,
										 (LPSECURITY_ATTRIBUTES)NULL,
										 (DWORD)PAGE_READWRITE|SEC_COMMIT,
										 (DWORD)(nMapSize >> 32),   
										 (DWORD)(nMapSize & 0x00000000ffffffff),
										 (LPCTSTR)szName);
#else
				hMem = CreateFileMapping(INVALID_HANDLE_VALUE,
										 (LPSECURITY_ATTRIBUTES)NULL,
										 (DWORD)PAGE_READWRITE|SEC_COMMIT,
										 (DWORD)(nMapSize >> 32),   
										 (DWORD)(nMapSize & 0x00000000ffffffff),
										 (unsigned short *)szName);	/**[02]**/
#endif
				dwError = GetLastError();
				
				if(dwError == ERROR_ALREADY_EXISTS) {
					bExists = TRUE;
				}
				if(bCreateUnique && bExists) {
					CloseHandle(hSize);
					CloseHandle(hMem);
					return((NCSSHMInfo*)NULL);
				}
				if(hMem) {
					void *pData;
				
					pData = MapViewOfFile(hMem, 
										  FILE_MAP_ALL_ACCESS,
										  0, 0, 0);

					if(pData) {
						NCSSHMInfo *pInfo;
		
						pInfo = NCSMalloc(sizeof(NCSSHMInfo), TRUE);

						if(!bExists) {
							/* Clear SHM */
							memset(pData, 0, (UINT32)(nMapSize & 0x00000000ffffffff));
						}
						if(pInfo) {
							FARPROC pProc = NULL;
							HANDLE hLib = NULL;

							pInfo->hMem = hMem;
							pInfo->hSize = hSize;
							pInfo->pData = pData;
							pInfo->nSize = nMapSize;

							{					
							//[01] Have to test to make sure that we are not running on 95
							//if not call the Set Security Info.
							if(NCSGetPlatform() == NCS_WINDOWS_NT) {	/**[04]**/
#if !defined(_WIN32_WCE)
								hLib = LoadLibraryA("advapi32.dll");

								if(hLib)
									pProc = GetProcAddress(hLib,"SetSecurityInfo");

								if(pProc)
								{
									/* change the discretionary access control list to allow inetinfo and ncsservergui process to access */
									pProc(hMem, 
											SE_KERNEL_OBJECT,
											DACL_SECURITY_INFORMATION,
											NULL,
											NULL,
											NULL,	/* this is the DACL - NULL means all access for all */
											NULL);
									pProc(hSize, 
											SE_KERNEL_OBJECT,
											DACL_SECURITY_INFORMATION,
											NULL,
											NULL,
											NULL,	/* this is the DACL - NULL means all access for all */
											NULL);
								}

								if(hLib)
									FreeLibrary(hLib);
#endif
							}

						  }	/**[03]**/

							return(pInfo);
						}
						UnmapViewOfFile(pData);
					}
					CloseHandle(hMem);
				}
			}
		} 
		CloseHandle(hSize);
	}
	
#else	/* WIN32 */

	NCSSHMInfo *pInfo = (NCSSHMInfo*)NULL;
		
	if(NULL != (pInfo = (NCSSHMInfo *)NCSMalloc(sizeof(NCSSHMInfo), TRUE))) {
		pInfo->nSize = nMapSize;

		if(NULL != (pInfo->pData = NCSMalloc(nMapSize, TRUE))) {
			return(pInfo);
		}
		NCSFree(pInfo);
	}

#endif	/* WIN32 */
	return((NCSSHMInfo*)NULL);
}

/*
** Unmap SHM
*/
void NCSUnmapSHM(NCSSHMInfo *pInfo)
{
	if(pInfo) {

#ifdef WIN32

		if(pInfo->pData) {
			UnmapViewOfFile(pInfo->pData);
		}
		if(pInfo->hMem) {
			CloseHandle(pInfo->hMem);
		}
		if(pInfo->hSize) {
			CloseHandle(pInfo->hSize);
		}
		
#else	/* WIN32 */

		if(pInfo->pData) {
			NCSFree(pInfo->pData);
		}

#endif	/* WIN32 */

		NCSFree(pInfo);
	}
}

/*	
**	Bytes swapping routines
*/	
UINT16 NCSByteSwap16(UINT16 n)
{
    UINT16          res;
    unsigned char   ch;

    union
        {
        UINT16          a;
        unsigned char   b[2];
        } un;

    un.a = n;
    ch = un.b[0];
    un.b[0] = un.b[1];
    un.b[1] = ch;

    res = un.a;
    return res;
}

void NCSByteSwapRange16(UINT16 *pDst, UINT16 *pSrc, INT32 nValues)
{
	while(nValues-- > 0) {
		*pDst++ = NCSByteSwap16( *pSrc++ );
	}
}

UINT32 NCSByteSwap32(UINT32 n)
{
    UINT32  res;
    UINT16  ch;

    union
        {
        UINT32   a;
        UINT16   b[2];
        } un;

    //
    //  Swap HI/LO word
    //
    un.a = n;

    ch = un.b[0];
    un.b[0] = un.b[1];
    un.b[1] = ch;

    //
    //  Swap Bytes
    //
    un.b[0] = NCSByteSwap16(un.b[0]);
    un.b[1] = NCSByteSwap16(un.b[1]);


    res = un.a;
    return res;
}

void NCSByteSwapRange32(UINT32 *pDst, UINT32 *pSrc, INT32 nValues)
{
	while(nValues-- > 0) {
		*pDst++ = NCSByteSwap32( *pSrc++ );
	}
}

UINT64 NCSByteSwap64(UINT64 n)
{
	register int i;
	UINT8	temp;
	union {
		UINT8	dataChar[8]; /**[02]**/
		UINT64	dataInt;
	} dataChanger;

	dataChanger.dataInt = n;

	for(i = 0; i < 4; ++i) {
		temp = dataChanger.dataChar[i];
		dataChanger.dataChar[i] = dataChanger.dataChar[7-i];
		dataChanger.dataChar[7-i] = temp;
	}
	return(dataChanger.dataInt);
}

void NCSByteSwapRange64(UINT64 *pDst, UINT64 *pSrc, INT32 nValues)
{
	register int count;

	for(count = 0; count < nValues; count++) {
		*pDst++ = NCSByteSwap64(*pSrc++);
	}
}

/*
** Macintosh memory allocation routines
** Try to compact memory if alloc fails.
*/
#define NEW_MEMORY_CODE 
#if defined(MACINTOSH) && defined(NEW_MEMORY_CODE)
#pragma pack(1)
typedef struct _mMallocHdr {
	unsigned long	cBytes;
	unsigned char	arData[4];	// variable size - is cBytes.  [4] bytes is left at the end of the allocation right now
} mMallocHdr, *PmMallocHdr;
#pragma pack()
#endif

#ifdef MACINTOSH
static void *MACmalloc( size_t cb )
{
/*	void *p = malloc( cb + sizeof(mMallocHdr) );
	if(!p) {
		//
		//	Out of Memory --
		//
		printf( "Out of Memory\n");	// remove and handle error better here
		CompactMem( cb + sizeof(mMallocHdr));
		PurgeMem( cb + sizeof(mMallocHdr));
		p = malloc( cb + sizeof(mMallocHdr) );
		if(!p) {
			//	worse case exit
			ExitToShell();	// nothing we can do!
			return NULL;
		}
	}
	((PmMallocHdr)p)->cBytes = cb;
	return ((PmMallocHdr)p)->arData;*/

	void *p = malloc( cb );
	if(!p) {
		//
		//	Out of Memory --
		//
		//printf( "Out of Memory\n");	// remove and handle error better here
		CompactMem( cb );
		PurgeMem( cb );
		p = malloc( cb );
		if(!p) {
			//	worse case exit
			//ExitToShell();	// nothing we can do!
			return NULL;
		}
	}
	return p;
}

static void *MACcalloc( size_t c, size_t cb )
{
/*	void *p = calloc( c, cb + sizeof(mMallocHdr));
	
	if(!p) {
		//
		//	Out of Memory --
		//
		printf( "Out of Memory\n");	// remove and handle error better here
		CompactMem( (cb + sizeof(mMallocHdr)) *c );
		PurgeMem( (cb + + sizeof(mMallocHdr)) *c );
		p = calloc( c, cb + sizeof(mMallocHdr));
		if(!p) {
			//	worse case exit
			ExitToShell();	// nothing we can do!
			return NULL;
		}
	}
	((PmMallocHdr)p)->cBytes = cb;
	return ((PmMallocHdr)p)->arData;
*/
	void *p = calloc( c, cb );
	
	if(!p) {
		//
		//	Out of Memory --
		//
		//printf( "Out of Memory\n");	// remove and handle error better here
		CompactMem( cb  );
		PurgeMem( cb  );
		p = calloc( c, cb );
		if(!p) {
			//	worse case exit
			//ExitToShell();	// nothing we can do!
			return NULL;
		}
	}
	return p;
}

static void *MACrealloc(void *pPtr, size_t iSize, BOOLEAN bClear)
{	
	void *p = realloc(pPtr, iSize);
	if(!p) {
		//
		//	Out of Memory --
		//
		//printf( "Out of Memory\n");	// remove and handle error better here
		CompactMem( iSize  );
		PurgeMem( iSize  );
		p = realloc(pPtr, iSize);
		if(!p) {
			//	worse case exit
			//ExitToShell();	// nothing we can do!
			return NULL;
		}
	}
	
	if( bClear ) {
		//memset((UINT8*)p + iOldSize, 0, iSize - iOldSize);
	}
	
	return p;
	
	
}

static void MACfree(void *pPtr)
{
	if( pPtr ) free(pPtr);
}
#endif	/* MACINTOSH */
