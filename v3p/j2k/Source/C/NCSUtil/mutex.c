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
** FILE:   	NCSUtil\mutex.c
** CREATED:	Thu Feb 25 09:19:00 WST 1999
** AUTHOR: 	Simon Cope
** PURPOSE:	NCS Mutex routines
** EDITS:
** [01] sjc 05Jul00 Fixed problem with GlobalLock - need to set DACL since
**					it's created in the IIS service in some instances
** [02]  ny 31Oct00 Merge WinCE/PALM SDK changes
** [03] sjc 05Jan01 Skip SetSecurityInfo() if running debug in purify
** [04] jmp 18Jan01 Don't load advapi32.dll for WIN9X, as FreeLibrary
**				    function misbehaves when called from within DllMain.
** [05] sjc 22Mar01 Fixed problem with GlobalLock under Purifying debugging session
** [06] tfl 01Apr05 Removed Purify references
*******************************************************/

#define _WIN32_WINNT 0x400
 
#include "NCSMutex.h"
#ifndef NCSMALLOC_H
#include "NCSMalloc.h"
#endif /* NCSMALLOC_H */

//#ifdef WIN32	//Pocket PC 2002
#if (defined(WIN32) && !defined(_WIN32_WCE))
#include <aclapi.h>
static BOOL (APIENTRY*pTryEnterCriticalSection)(LPCRITICAL_SECTION lpCriticalSection) = NULL;
#endif

#include "NCSUtil.h"

#ifdef POSIX
pthread_mutex_t *NCSMutexGetNative(NCSMutex *pMutex)
{
	return &pMutex->m;
}
#endif

/*
** Initialise the mutex structure.  Call once per mutex before any calls to Begin/End mutex.
*/
void NCSMutexInit(NCSMutex *pMutex)
{
//#ifndef LINUX
	// TODO : FIXME
	NCSMemSet(pMutex, 0, sizeof(NCSMutex));
//#endif

#ifdef WIN32

	InitializeCriticalSection(&(pMutex->cs));

	if(!pTryEnterCriticalSection && NCSGetPlatform() != NCS_WINDOWS_9X) {
		pTryEnterCriticalSection = (BOOL(APIENTRY*)(LPCRITICAL_SECTION))GetProcAddress(GetModuleHandle(NCS_T("Kernel32.dll")), "TryEnterCriticalSection");
	}
	if(!pTryEnterCriticalSection) {
		// No TryEnterCriticalSection(), so we have to emulate
		pMutex->pTryCS = (NCSMutexTryCS*)NCSMalloc(sizeof(NCSMutexTryCS), TRUE);
		pMutex->pTryCS->nLockCount = -1;
		InitializeCriticalSection(&pMutex->pTryCS->cs);
	}

#elif defined PALM
#elif defined MACINTOSH
#	ifdef MAC_PREEMPTIVE
		//OSStatus result = 
		MPCreateCriticalRegion( &(pMutex->cs) ); 
#	endif //MAC_PREEMPTIVE
#elif defined POSIX

	pthread_mutexattr_t a;
	pthread_mutexattr_init(&a);
#ifdef LINUX
	pthread_mutexattr_settype(&a, PTHREAD_MUTEX_RECURSIVE_NP);
#else
	pthread_mutexattr_settype(&a, PTHREAD_MUTEX_RECURSIVE);
#endif
	pthread_mutex_init(&(pMutex->m), (pthread_mutexattr_t*)&a);

#else	/* WIN32 */
ERROR: NCSMutexInit() needs to be written in NCSUtil/mutex.c
#endif	/* WIN32 */

	NCSMemSet(&(pMutex->msStats), 0, sizeof(NCSMutexStats));
}

/*
** Finalise a mutex (destroy resources).  Do not use after this.
*/
void NCSMutexFini(NCSMutex *pMutex)
{
#ifdef WIN32

	DeleteCriticalSection(&(pMutex->cs));
	if(pMutex->pTryCS) {
		DeleteCriticalSection(&pMutex->pTryCS->cs);
		NCSFree(pMutex->pTryCS);
	}
#elif defined PALM
#elif defined MACINTOSH
#	ifdef MAC_PREEMPTIVE
		//OSStatus result = 
		MPDeleteCriticalRegion( pMutex->cs ); 
#	endif //MAC_PREEMPTIVE
#elif defined POSIX
	
	pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_destroy(&(pMutex->m));
	pMutex->m = m;

#else	/* WIN32 */
ERROR: NCSMutexFini() needs to be written in NCSUtil/mutex.c
#endif	/* WIN32 */
}

/*
** Enter/Lock/Begin a mutex.
*/
void NCSMutexBegin(NCSMutex *pMutex)
{
	NCSTimeStampMs tsStart = 0;

	if(pMutex->bCollectStats) {
		tsStart = NCSGetTimeStampMs();
	}
#ifdef WIN32

	if(pMutex->pTryCS) {
		// lock TryCS, prevents anything else from attempting to lock.
		EnterCriticalSection(&pMutex->pTryCS->cs);
		while(pMutex->pTryCS->nLockCount >= 0 && (DWORD)pMutex->pTryCS->dwOwnerTID != GetCurrentThreadId()) {
			// Locked already, spin
			LeaveCriticalSection(&pMutex->pTryCS->cs);
			NCSSleep(1);
			EnterCriticalSection(&pMutex->pTryCS->cs);
		}
		EnterCriticalSection(&(pMutex->cs));
		pMutex->pTryCS->nLockCount++;
		pMutex->pTryCS->dwOwnerTID = GetCurrentThreadId();
		LeaveCriticalSection(&pMutex->pTryCS->cs);
	} else {
		EnterCriticalSection(&(pMutex->cs));
	}
#elif defined	PALM
#elif defined	MACINTOSH
#	ifdef MAC_PREEMPTIVE
		//OSStatus result = 
		MPEnterCriticalRegion( pMutex->cs, kDurationForever /*Duration timeout*/ ); 
#	endif //MAC_PREEMPTIVE
#elif defined POSIX

	pthread_mutex_lock(&(pMutex->m));

#else	/* WIN32 */
ERROR: NCSMutexBegin() needs to be written in NCSUtil/mutex.c
#endif	/* WIN32 */

		   if(pMutex->bCollectStats) {
		pMutex->msStats.tsStartTemp = NCSGetTimeStampMs();
		pMutex->msStats.tsBeginTime += pMutex->msStats.tsStartTemp - tsStart;
		pMutex->msStats.nLocks++;
	}
}

/*
** Enter/Lock/Begin a mutex - return TRUE on success, or FALSE if mutex already locked by another thread.
*/

BOOLEAN NCSMutexTryBegin(NCSMutex *pMutex)
{
	BOOLEAN bRet = FALSE;
	NCSTimeStampMs tsStart = 0;

	if(pMutex->bCollectStats) {
		tsStart = NCSGetTimeStampMs();
	}
#ifdef WIN32

	if(pMutex->pTryCS) {
		EnterCriticalSection(&pMutex->pTryCS->cs);
		if(pMutex->pTryCS->nLockCount < 0 || (pMutex->pTryCS->nLockCount >= 0 && (DWORD)pMutex->pTryCS->dwOwnerTID == GetCurrentThreadId())) {
			// not locked
			EnterCriticalSection(&(pMutex->cs));
			pMutex->pTryCS->nLockCount++;
			pMutex->pTryCS->dwOwnerTID = GetCurrentThreadId();

			bRet = TRUE;
		}
		LeaveCriticalSection(&pMutex->pTryCS->cs);
	} else {
		bRet = (BOOLEAN)(*pTryEnterCriticalSection)(&(pMutex->cs));
	}

#elif defined	PALM
#elif defined	MACINTOSH
#	ifdef MAC_PREEMPTIVE
		if ( MPEnterCriticalRegion( pMutex->cs, kDurationImmediate) == 0) {
			bRet = TRUE;
		} else {
			bRet = FALSE;
		}

#	endif //MAC_PREEMPTIVE
#elif defined POSIX

		bRet = (pthread_mutex_trylock(&(pMutex->m)) == 0) ? TRUE : FALSE;

#else	/* WIN32 */
ERROR: NCSMutexBegin() needs to be written in NCSUtil/mutex.c
#endif	/* WIN32 */

   if(pMutex->bCollectStats) {
		pMutex->msStats.tsStartTemp = NCSGetTimeStampMs();
		pMutex->msStats.tsBeginTime += pMutex->msStats.tsStartTemp - tsStart;
		pMutex->msStats.nLocks++;
   }
   return(bRet);
}

/*
** Leave/Unlock/End a mutex
*/
void NCSMutexEnd(NCSMutex *pMutex)
{
	NCSTimeStampMs tsStart = 0, tsEnd = 0;

	if(pMutex->bCollectStats) {
		tsStart = NCSGetTimeStampMs();
		pMutex->msStats.nUnLocks++;
	}

#ifdef WIN32

	if(pMutex->pTryCS) {
		EnterCriticalSection(&pMutex->pTryCS->cs);
		pMutex->pTryCS->nLockCount--;
		if(pMutex->pTryCS->nLockCount < 0) {
			pMutex->pTryCS->dwOwnerTID = 0;
		}
		LeaveCriticalSection(&(pMutex->cs));
		LeaveCriticalSection(&pMutex->pTryCS->cs);
	} else {
		LeaveCriticalSection(&(pMutex->cs));
	}
#elif defined PALM
#elif defined MACINTOSH
#	ifdef MAC_PREEMPTIVE
		//OSStatus result = 
		MPExitCriticalRegion ( pMutex->cs );
#	endif //MAC_PREEMPTIVE
#elif defined POSIX

	pthread_mutex_unlock(&(pMutex->m));

#else	/* WIN32 */
ERROR: NCSMutexBegin() needs to be written in NCSUtil/mutex.c
#endif 	/* WIN32 */
	
	if(pMutex->bCollectStats) {
		tsEnd = NCSGetTimeStampMs();
		pMutex->msStats.tsTotalLockedTime += tsEnd - pMutex->msStats.tsStartTemp;
		pMutex->msStats.tsEndTime += tsEnd - tsStart;
	}
}

/*
** Return MutexStats structure.
*/
NCSMutexStats NCSMutexGetStats(NCSMutex *pMutex)
{
	return(pMutex->msStats);
}

/*
** Enable Stats Collection
*/
void NCSMutexEnableStats(NCSMutex *pMutex)
{
	pMutex->bCollectStats = TRUE;
}

/*
** Disable Stats Collection
*/
void NCSMutexDisableStats(NCSMutex *pMutex)
{
	pMutex->bCollectStats = FALSE;
}

/*
** GlobalLock
*/

NCSGlobalLockInfo *pGlobalLocks = (NCSGlobalLockInfo*)NULL;
INT32			nGlobalLocks = 0;
NCSMutex		mGlobalLockMutex = { NCS_NULL_MUTEX };
INT32			nGlobalLockInit = 0;

void NCSGlobalLockInit(void)
{
	if(nGlobalLockInit == 0) {
		NCSMutexInit(&mGlobalLockMutex);
	}
	nGlobalLockInit++;
}

void NCSGlobalLockFini(void)
{
	nGlobalLockInit--;
	if(nGlobalLockInit == 0) {
		NCSMutexBegin(&mGlobalLockMutex);
		while(nGlobalLocks > 0) {
#ifdef WIN32
			CloseHandle(pGlobalLocks[0].hLock);
#endif //WIN32
			NCSFree(pGlobalLocks[0].pLockName);
			NCSArrayRemoveElement(pGlobalLocks, nGlobalLocks, 0);
		}
		NCSMutexEnd(&mGlobalLockMutex);
		NCSMutexFini(&mGlobalLockMutex);
	}
}

NCSGlobalLockInfo *NCSGlobalLock(char *pLockName)
{
#ifdef WIN32
	INT32 i;

	NCSGlobalLockInit();

	NCSMutexBegin(&mGlobalLockMutex);
	for(i = 0; i < nGlobalLocks; i++) {
		if(!strcmp(pGlobalLocks[i].pLockName, pLockName)) {
			break;
		}
	}
	if(i >= nGlobalLocks) {
		NCSGlobalLockInfo Info;
		BOOL bSetInfo = TRUE;

		Info.hLock = CreateMutex((LPSECURITY_ATTRIBUTES)NULL,
								  FALSE,
								  (LPCTSTR)pLockName);
		if(GetLastError() == ERROR_ALREADY_EXISTS) {
			bSetInfo = FALSE;
		}
		if(Info.hLock) {
			Info.pLockName = NCSStrDup(pLockName);
			NCSArrayAppendElement(pGlobalLocks, nGlobalLocks, &Info);
			i = nGlobalLocks - 1;

			{
#if (!defined(_WIN32_WCE))	//Pocket PC 2002
				FARPROC pProc = NULL;			/**[01]**/
				HANDLE hLib = NULL;				/**[01]**/

				if(bSetInfo && NCSGetPlatform() == NCS_WINDOWS_NT) {	/**[04]**/
					hLib = LoadLibrary(NCS_T("advapi32.dll"));					/**[01]**/

					if(hLib) {											/**[01]**/
						pProc = GetProcAddress(hLib,"SetSecurityInfo");	/**[01]**/
						
						if(pProc) {										/**[01]**/
							/* change the discretionary access control list to allow inetinfo and ncsservergui process to access */
#ifdef _MANAGED
							((DWORD(*)(HANDLE, SE_OBJECT_TYPE, SECURITY_INFORMATION, PSID, PSID, PACL, PACL))pProc)
#else
								DWORD dwRes = pProc
#endif
								(
								Info.hLock,								/**[01]**/
									SE_KERNEL_OBJECT,					/**[01]**/
									DACL_SECURITY_INFORMATION,			/**[01]**/
									NULL,								/**[01]**/
									NULL,								/**[01]**/
									NULL,	/* this is the DACL - NULL means all access for all */
									NULL);								/**[01]**/
						}												/**[01]**/
						FreeLibrary(hLib);								/**[01]**/
					}													/**[01]**/
				}
#endif
			}	/**[03]**/
		}
	}
	NCSMutexEnd(&mGlobalLockMutex);

	if(i >= 0 && i < nGlobalLocks) {
		if(WaitForSingleObject(pGlobalLocks[i].hLock,
							   INFINITE) != WAIT_FAILED) {
			return(&pGlobalLocks[i]);
		}
		CloseHandle(pGlobalLocks[i].hLock);
		NCSFree(pGlobalLocks[i].pLockName);
		NCSArrayRemoveElement(pGlobalLocks, nGlobalLocks, i);
	}
#else	/* WIN32 */
#endif	/* WIN32 */
	return((NCSGlobalLockInfo*)NULL);
}

/*
** GlobalUnlock
*/
void NCSGlobalUnlock(NCSGlobalLockInfo *pInfo)
{
	if(pInfo) {
#ifdef WIN32
		if(pInfo->hLock) {
			ReleaseMutex(pInfo->hLock);
//			CloseHandle(pInfo->hLock);
		}
#else	/* WIN32 */
#endif	/* WIN32 */
//		NCSFree(pInfo);
	}
	NCSGlobalLockFini();
}
