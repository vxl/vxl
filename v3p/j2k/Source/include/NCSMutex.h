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
** FILE:   	NCSMutex.h
** CREATED:	Thu Feb 25 09:19:00 WST 1999
** AUTHOR: 	Simon Cope
** PURPOSE:	NCS Mutex handling.
** EDITS:
** [01] 03Nov00	 ny	Merged WinCE/PALM SDK changes
** [02] 12Apr01	 ny	Added copy constructor for CNCSMutex
 *******************************************************/

#ifndef NCSMUTEX_H
#define NCSMUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NCSDEFS_H
#include "NCSDefs.h"
#endif
#ifndef NCSTYPES_H
#include "NCSTypes.h"
#endif
#ifndef NCSTIMESTAMP_H
#include "NCSTimeStamp.h"
#endif

#ifdef POSIX
#include <pthread.h>
#endif

#if defined( MACINTOSH ) && defined( MAC_PREEMPTIVE )
#	include <Multiprocessing.h>
#endif

typedef struct {
	INT64		   nLocks;					/* Number of times mutex has been locked	*/
	INT64		   nUnLocks;				/* Number of times mutex has been unlocked	*/
	NCSTimeStampMs tsBeginTime;				/* Time spent locking mutex					*/
	NCSTimeStampMs tsEndTime;				/* Time spent unlocking mutex				*/
	NCSTimeStampMs tsTotalLockedTime;		/* Total time spent locked					*/
	NCSTimeStampMs tsStartTemp;				/* Temp to calculate tsTotalLockedTime		*/	
} NCSMutexStats;

#ifdef WIN32
typedef struct {
	CRITICAL_SECTION cs;
	INT32			nLockCount;
	DWORD			dwOwnerTID;
} NCSMutexTryCS;
#endif // WIN32

typedef struct {
#ifdef WIN32
	CRITICAL_SECTION cs;
	NCSMutexTryCS *pTryCS;
#elif defined MACINTOSH
	MPCriticalRegionID cs;
// FIXME:  No mutex yet!
#elif defined PALM
// FIXME:  No mutex yet!
#elif defined POSIX
	pthread_mutex_t m;
#else	/* PALM */
ERROR: Define NCSMutex for this platform
#endif 	/* WIN32 */
	NCSMutexStats msStats;
	BOOLEAN		  bCollectStats;
} NCSMutex;

typedef struct {
#ifdef WIN32
	HANDLE	hLock;
#elif defined(PALM)
	int		hLock;
#elif defined(MACINTOSH)
	int		hLock;
#elif defined(POSIX)
	int		hLock;
#else	/* WIN32 */
#error NCSGlobalLockInfo
#endif	/* WIN32 */
	char	*pLockName;
} NCSGlobalLockInfo;

/*
** Defines.
*/
#ifdef WIN32

#define NCS_NULL_MUTEX { 0, 0, 0, 0, 0, 0 }

#elif defined MACINTOSH

#define NCS_NULL_MUTEX { 0, 0, 0, 0, 0 }

#elif defined SOLARIS

#define NCS_NULL_MUTEX PTHREAD_MUTEX_INITIALIZER

#elif defined HPUX

#define NCS_NULL_MUTEX { PTHREAD_MUTEX_INITIALIZER }

#elif defined LINUX

#define NCS_NULL_MUTEX  PTHREAD_MUTEX_INITIALIZER 

#elif defined MACOSX
#define NCS_NULL_MUTEX  PTHREAD_MUTEX_INITIALIZER 

#else /* MACINTOSH */

ERROR: Define NCS_NULL_MUTEX for this platform

#endif /* WIN32 */

/*
** Prototypes.
*/
void NCSMutexInit(NCSMutex *pMutex);
void NCSMutexFini(NCSMutex *pMutex);
void NCSMutexBegin(NCSMutex *pMutex);
BOOLEAN NCSMutexTryBegin(NCSMutex *pMutex);
void NCSMutexEnd(NCSMutex *pMutex);
NCSMutexStats NCSMutexGetStats(NCSMutex *pMutex);
void NCSMutexEnableStats(NCSMutex *pMutex);
void NCSMutexDisableStats(NCSMutex *pMutex);
#ifdef POSIX
pthread_mutex_t *NCSMutexGetNative(NCSMutex *pMutex);
#endif

/* Global (SYSTEM Wide) Locks */
void NCSGlobalLockInit(void);
void NCSGlobalLockFini(void);
NCSGlobalLockInfo *NCSGlobalLock(char *pLockName);
void NCSGlobalUnlock(NCSGlobalLockInfo *pInfo);

#ifdef __cplusplus
}

#ifndef NCS_NO_UTIL_LINK
// CNCSMutex
NCS_GC class NCS_EXPORT CNCSMutex {
private:
	NCSMutex	m_Mutex;
public:
	CNCSMutex();
	//CNCSMutex(const CNCSMutex &mMutex); /**[02]**/
	virtual ~CNCSMutex();

	virtual void Lock(void);
	virtual bool TryLock(void);
	virtual void UnLock(void);
#ifdef POSIX
	pthread_mutex_t *GetNative();
#endif
};

// CNCSMutexLock
NCS_GC class NCS_EXPORT CNCSMutexLock {

private:
	CNCSMutex	*m_pMutex;
public:
	CNCSMutexLock(CNCSMutex *pmMutex) {
		if( pmMutex ) {
			m_pMutex = pmMutex;
			m_pMutex->Lock();
		} else {
			m_pMutex = NULL;
		}
	}
	virtual ~CNCSMutexLock() {
		if( m_pMutex ) {
			m_pMutex->UnLock();
		}
	}
};

#endif //NCS_NO_UTIL_LINK

#endif
#endif /* NCSMUTEX_H */
