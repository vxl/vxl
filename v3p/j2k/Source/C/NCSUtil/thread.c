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
 ** FILE:   	NCSUtil/thread.h
 ** CREATED:	Fri Feb 26 09:19:00 WST 1999
 ** AUTHOR: 	Simon Cope
 ** PURPOSE:	NCS Thread handling.
 ** EDITS:
 ** [01] sjc 10May99 Reworked thread code to clean up better on exit,
 **					and prevent hangs.
 ** [02]  ny 01Aug00 Added new function NCSThreadTerminate()
 ** [03]  ny 31Oct00 Merge WinCE/PALM SDK changes
 ** [04] sjc 11Dec00 Added exception handler for all threads
 ** [05] rar	19-Jan-01 Mac port changes
 ** [06]  ny 02May01	Added GetSysID() method
 ** [07] rar 09Oct01 Mac port change
 ** [08] tfl 15Sep05 Fixed thread updates due to bad init defaults in some environs (from Bill Binko)
 *******************************************************/

#ifdef WIN32
// For SwitchToThread()
#define _WIN32_WINNT 0x0500
#endif //WIN32

#include "NCSUtil.h"
#include "NCSLog.h"
#include "NCSBuildNumber.h"

#include <string.h>

#ifdef POSIX

#include <sys/errno.h>
#include <pthread.h>

pthread_attr_t sAttributes;
#endif

/*
 ** Internal thread info.
 */
typedef struct {
	NCSThread		tid;
#ifdef WIN32

	DWORD			dwTID;
	HANDLE			hThread;

#elif defined(PALM)

#elif defined(MACINTOSH)
	MacThreadID		hThread;
#	ifdef MAC_PREEMPTIVE
	//MPTaskID		hThread;
	MPSemaphoreID 	suspend_condition;
	BOOLEAN			bSuspended;
#	else
	//ThreadID		hThread;
	//MPTimerID		timerID;
#	endif //MAC_PREEMPTIVE
#elif defined(POSIX)

	pthread_t		thread;
	pthread_cond_t	suspend_condition;
	BOOLEAN			bSuspended;
	BOOLEAN			bThreadStarted;
	NCSMutex		mSuspendMutex;

#else 
ERROR: define NCSThread for this platform
#endif

	       NCSThreadStats	tsStats;
       BOOLEAN			bCollectStats;
       BOOLEAN			bThreadRunning;
       void *pStartData;
} NCSThreadInfo;

/*
 ** Thread start data.
 */
typedef struct {
	NCSThread		*pThread;
	void			(*pFunc)(void*);
	void			*pData;
#if defined MACINTOSH && defined MAC_PREEMPTIVE
	NCSThreadInfo   *pThreadInfo;
#endif
#ifdef POSIX
	NCSThreadInfo   *pThreadInfo;
#endif
} NCSThreadStartData;


/*
 ** Static functions.
 */
static NCSThreadInfo *NCSThreadGetInfo(NCSThread *);
static NCSThreadInfo *NCSThreadGetCurrentInfo(void);


/*
 ** Static Data.
 */
static NCSThreadLSKey	ThreadIDKey;
#ifdef LINUX
static NCSMutex			mMutex = { NCS_NULL_MUTEX };
#else
static NCSMutex			mMutex = { NCS_NULL_MUTEX };
#endif
static INT32			nThreadsInitialised = 0;
static NCSThreadInfo	**ppThreadInfos = (NCSThreadInfo**)NULL;
static INT32			nThreadInfos = 0;
static NCSThread		tidNextTID = (NCSThread)1;

/*
 ** Local prototypes.
 */
static int NCSThreadStartFunc(NCSThreadStartData *pStartData);

/*
 ** Initialise thread logic.
 */
void NCSThreadInit(void)
{
	if(nThreadsInitialised == 0) {
		NCSMutexInit(&mMutex);

		ThreadIDKey = NCSThreadLSAlloc();
		ppThreadInfos = (NCSThreadInfo**)NULL;
		nThreadInfos = 0;
#ifdef POSIX
		pthread_attr_init(&sAttributes);
		//pthread_attr_setstacksize(&sAttributes, 1024*1024);
		pthread_attr_setschedpolicy(&sAttributes, SCHED_RR);
		//pthread_attr_setscope(&sAttributes, PTHREAD_SCOPE_SYSTEM);
		//		{
		//			struct sched_param param;
		//			pthread_attr_getparam(&sAttributes, &param);
		//			param.
		//			sched_setscheduler(getpid(), SCHED_RR, &param);
		//		}
#endif // POSIX
	}
	NCSMutexBegin(&mMutex);
	nThreadsInitialised++;
	NCSMutexEnd(&mMutex);
}

/*
 ** Finalise thread logic.
 */
void NCSThreadFini(void)
{
	NCSMutexBegin(&mMutex);
	nThreadsInitialised--;
	NCSMutexEnd(&mMutex);

	if(nThreadsInitialised == 0) {
		NCSMutexBegin(&mMutex);
		while(nThreadInfos) {
			NCSThreadFreeInfo(&(ppThreadInfos[0]->tid));
		}
		NCSMutexEnd(&mMutex);

		NCSThreadLSFree(ThreadIDKey);
		NCSMutexFini(&mMutex);
#ifdef POSIX
		pthread_attr_destroy(&sAttributes);
#endif // POSIX
	}
}

/*
 ** Spawn a thread.
 */
BOOLEAN NCSThreadSpawn(NCSThread *pThread, void (*pFunc)(void*), void *pData, BOOLEAN bCreateSuspended)
{
#ifdef WIN32
	NCSThreadStartData *pStartData;
	//MessageBox(NULL, "HERE", "DEBUG", MB_OK);
	if((pStartData = (NCSThreadStartData*)NCSMalloc(sizeof(NCSThreadStartData), TRUE)) != NULL) {
#ifdef _WIN32_WCE
		HANDLE hThread;
#elif WIN64
		uintptr_t hThread;
#else /* _WIN32_WCE */
		unsigned long hThread;
#endif /* _WIN32_WCE */
		NCSThreadInfo *pThreadInfo = (NCSThreadInfo*)NCSMalloc(sizeof(NCSThreadInfo), TRUE);

		NCSMutexBegin(&mMutex);
		pThreadInfo->bThreadRunning = FALSE; /**[08]**/
		pThreadInfo->tid = tidNextTID++;
		NCSMutexEnd(&mMutex);

		*pThread = pThreadInfo->tid;
		pStartData->pThread = &(pThreadInfo->tid);//pThread;
		pStartData->pFunc = pFunc;
		pStartData->pData = pData;

		pThreadInfo->pStartData = pStartData;
		pThreadInfo->bCollectStats = FALSE;

		/*
		 ** Create the thread.  If we create is suspended, we know pThread->hThread is valid BEFORE
		 ** the thread starts, so it can be passed to the thread as a parameter.
		 */
#ifdef _WIN32_WCE

		hThread = CreateThread((void*)NULL,
				0,
				(unsigned(__stdcall*)(void*))NCSThreadStartFunc,
				pStartData,
				CREATE_SUSPENDED,
				(unsigned*)&(pThreadInfo->dwTID));

#else	/* _WIN32_WCE */

		hThread = (unsigned long)_beginthreadex((void*)NULL,
				0,
				(unsigned(__stdcall*)(void*))NCSThreadStartFunc,
				pStartData,
				CREATE_SUSPENDED,
				(unsigned*)&(pThreadInfo->dwTID));
#endif	/* _WIN32_WCE */

		if(hThread > 0) {
			if(pThread) {
				*pThread = pThreadInfo->tid;
			}
			pThreadInfo->hThread = (HANDLE)hThread;
			pThreadInfo->bThreadRunning = TRUE;

			NCSMutexBegin(&mMutex);
			NCSArrayAppendElement(ppThreadInfos, nThreadInfos, &pThreadInfo);
			NCSLog(LOG_HIGH, "Spawn: ID 0x%lx, Handle 0x%lx", pThreadInfo->dwTID, pThreadInfo->hThread);
			NCSMutexEnd(&mMutex);

			if(!bCreateSuspended) {
				/*
				 ** Resume thread.
				 */
				ResumeThread(pThreadInfo->hThread);
			}
			return(TRUE);
		}
		NCSFree(pThreadInfo);
	}
	return(FALSE);
#elif defined(PALM)

#elif defined(MACINTOSH)

	OSStatus	errWhatTaskErr;
	NCSThreadInfo *pThreadInfo = (NCSThreadInfo*)NCSMalloc(sizeof(NCSThreadInfo), TRUE);
	MacThreadID hThread;
	NCSThreadStartData *pStartData = NULL;
	if( pThreadInfo ) {

		NCSMutexBegin(&mMutex);
		pThreadInfo->tid = tidNextTID++;
		pThreadInfo->bThreadRunning = FALSE; /**[08]**/
		NCSMutexEnd(&mMutex);

		*pThread = pThreadInfo->tid;

#ifdef MAC_PREEMPTIVE
		{
			OSStatus status = MPCreateSemaphore ( 1, 0, &pThreadInfo->suspend_condition);
			pStartData = (NCSThreadStartData*)NCSMalloc(sizeof(NCSThreadStartData), TRUE);
			if( status != noErr && pStartData ) {
				NCSFree(pThreadInfo);
				if( pStartData ) NCSFree( pStartData );
				return(FALSE);
			}
			pThreadInfo->bSuspended = FALSE;

			pStartData->pThread = &(pThreadInfo->tid);//pThread;
			pStartData->pFunc = pFunc;
			pStartData->pData = pData;
			pStartData->pThreadInfo = pThreadInfo;

			errWhatTaskErr = MPCreateTask( (TaskProc)NCSThreadStartFunc,
					(void *)pStartData,
					(ByteCount)10*1024/*stackSize*/,
					0/*MPQueueID notifyQueue*/,
					(void *) NULL/*terminationParameter1*/,
					(void *) NULL/* terminationParameter2*/,
					0/*MPTaskOptions options*/,
					(MPTaskID *) &hThread
					);
		}
#else
#if TARGET_API_MAC_CARBON
		errWhatTaskErr = NewThread ( kCooperativeThread, 
				(ThreadEntryTPP) NewThreadEntryUPP( pFunc ), 
				(void *)pData, 
				0, //stack size
				kNewSuspend, 
				0, 
				&hThread );
#else
		errWhatTaskErr = NewThread ( kCooperativeThread, 
				(ThreadEntryProcPtr) pFunc, 
				(void *)pData, 
				0, //stack size
				kNewSuspend, 
				0, 
				&hThread );
#endif
#endif //MAC_PREEMPTIVE

		if( errWhatTaskErr == noErr ) {
			pThreadInfo->hThread = hThread;
			pThreadInfo->bThreadRunning = TRUE;

			NCSMutexBegin(&mMutex);
			NCSArrayAppendElement(ppThreadInfos, nThreadInfos, &pThreadInfo);
			NCSMutexEnd(&mMutex);

#ifdef MAC_PREEMPTIVE
			//Wait until the created thread has been suspended before continuing
			while( !pThreadInfo->bSuspended	) NCSSleep( 10 );
#endif //MAC_PREEMPTIVE
			if(!bCreateSuspended) {
				NCSThreadResume(pThread);
			}

			return(TRUE);
		}
		else {
			NCSFree(pThreadInfo);
			NCSFree(pStartData);
			return(FALSE);
		}
	}
	return(FALSE);

#elif defined POSIX

	int nError = 0;	
	NCSThreadStartData *pStartData = NULL;
	pthread_cond_t c = PTHREAD_COND_INITIALIZER;
	NCSThreadInfo *pThreadInfo = (NCSThreadInfo*)NCSMalloc(sizeof(NCSThreadInfo), TRUE);

	NCSMutexBegin(&mMutex);
	pThreadInfo->tid = tidNextTID++;
	pThreadInfo->bThreadRunning = FALSE; /**[08]**/
	NCSMutexEnd(&mMutex);

	*pThread = pThreadInfo->tid;

	pThreadInfo->suspend_condition = c;
	pThreadInfo->bSuspended = bCreateSuspended;
	pThreadInfo->bThreadRunning = TRUE;
	pthread_cond_init(&(pThreadInfo->suspend_condition), NULL);
	NCSMutexInit(&pThreadInfo->mSuspendMutex);
	NCSMutexBegin(&pThreadInfo->mSuspendMutex);

	pStartData = (NCSThreadStartData*)NCSMalloc(sizeof(NCSThreadStartData), TRUE);

	pStartData->pThread = &(pThreadInfo->tid);//pThread;
	pStartData->pFunc = pFunc;
	pStartData->pData = pData;
	pStartData->pThreadInfo = pThreadInfo;
	//pStartData->bStartSuspended = bCreateSuspended;

	pThreadInfo->pStartData = pStartData;

	if(0 == (nError = pthread_create(&(pThreadInfo->thread),
					&sAttributes, 
					(void *(*)(void*))NCSThreadStartFunc, pStartData))) {

		NCSMutexBegin(&mMutex);
		NCSArrayAppendElement(ppThreadInfos, nThreadInfos, &pThreadInfo);
		NCSMutexEnd(&mMutex);

		NCSMutexEnd(&pThreadInfo->mSuspendMutex);
		// Wait for thread to start
		while(!pThreadInfo->bThreadStarted) {
			NCSThreadYield();
		}
		return(TRUE);
	} else {
		NCSMutexEnd(&pThreadInfo->mSuspendMutex);
		NCSMutexFini(&pThreadInfo->mSuspendMutex);
		NCSFree(pThreadInfo);
		NCSFree(pStartData);
		return(FALSE);
	}

#else
ERROR: Need to code NCSThreadSpawn() in NCSUtil/thread.c
#endif

}

/*
 ** Free thread local info (handle etc).
 */
void NCSThreadFreeInfo(NCSThread *pThread)
{
	if(pThread) {
		INT32 i;

		NCSMutexBegin(&mMutex);
		for(i = 0; i < nThreadInfos; i++) {
			if(ppThreadInfos[i] && ppThreadInfos[i]->tid == *pThread) {
#ifdef WIN32
				ppThreadInfos[i]->dwTID = 0;
				if(ppThreadInfos[i]->hThread) {
					CloseHandle(ppThreadInfos[i]->hThread);
					ppThreadInfos[i]->hThread = 0;
				}
#elif defined(PALM)
#elif defined(MACINTOSH)
#elif defined(POSIX)
				void *value_pair = (void*)NULL;
				pthread_join(ppThreadInfos[i]->thread, &value_pair);
				NCSMutexFini(&ppThreadInfos[i]->mSuspendMutex);
#else
#error NCSThreadFreeInfo()
#endif
				if( ppThreadInfos[i]->pStartData ) NCSFree(ppThreadInfos[i]->pStartData);
				NCSFree(ppThreadInfos[i]);
				NCSArrayRemoveElement(ppThreadInfos, nThreadInfos, i);
				break;
			}
		}
		NCSMutexEnd(&mMutex);
	}
}

/*
 ** Exit thread & free resources.
 */
void NCSThreadExit(INT32 dwExitId)
{
	NCSThreadInfo *pThreadInfo;

	if(NULL != (pThreadInfo = NCSThreadGetCurrentInfo())) {
		pThreadInfo->bThreadRunning = FALSE;

		if(pThreadInfo->bCollectStats) {
			pThreadInfo->tsStats.tsTotalRunning = NCSGetTimeStampMs() - pThreadInfo->tsStats.tsStart;
		}

		NCSMutexBegin(&mMutex);
#ifdef WIN32
		pThreadInfo->dwTID = 0;
		if(pThreadInfo->hThread) {
			OSVERSIONINFO osv;
			memset(&osv, 0, sizeof(osv));
			osv.dwOSVersionInfoSize = sizeof(osv);

			if(GetVersionEx(&osv) && (osv.dwPlatformId == VER_PLATFORM_WIN32_NT)) {
				CloseHandle(pThreadInfo->hThread);
			}
			pThreadInfo->hThread = 0;
		}
#elif defined(MACINTOSH)
		pThreadInfo->bThreadRunning = FALSE;
		pThreadInfo->hThread = 0;
#endif
		NCSMutexEnd(&mMutex);
	}
#ifdef WIN32

#ifdef _WIN32_WCE
	ExitThread(dwExitId);
#else	/* _WIN32_WCE */
	_endthreadex(dwExitId);
#endif	/* _WIN32_WCE */

#elif defined(PALM)
#elif defined(MACINTOSH)
#ifdef MAC_PREEMPTIVE
	MPExit( (OSStatus)dwExitId ); 
#else
	//NCSThreadTerminate(&pThreadInfo->hThread);
#endif // MAC_PREEMPTIVE
#elif defined(POSIX)
	pthread_exit((void *)NULL);
#else
ERROR: Need to code NCSThreadExit() in NCSUtil/thread.c
#endif
}

/*
 ** Suspend a thread
 */
void NCSThreadSuspend(void)
{
	NCSThreadInfo *pThreadInfo = NCSThreadGetCurrentInfo();

	if(pThreadInfo) {
		if(pThreadInfo->bCollectStats) {
			pThreadInfo->tsStats.tsSuspendStart = NCSGetTimeStampMs();
			pThreadInfo->tsStats.nSuspends += 1;
		}

#ifdef WIN32

		SuspendThread(pThreadInfo->hThread);

#elif defined(PALM)
#elif defined(MACINTOSH)
#ifdef MAC_PREEMPTIVE
		{	
			OSStatus eResult;
			pThreadInfo->bSuspended = TRUE;
			eResult = MPWaitOnSemaphore ( pThreadInfo->suspend_condition, kDurationForever );
			pThreadInfo->bSuspended = FALSE;
		}
#else
		{
			OSErr eResult = SetThreadState( kCurrentThreadID, 
					kStoppedThreadState, 
					kNoThreadID );
			//NCSThreadYield();
		}
#endif // MAC_PREEMPTIVE
#elif defined(POSIX)

		{
			NCSMutexBegin(&pThreadInfo->mSuspendMutex);

			pThreadInfo->bSuspended = TRUE;
			pthread_cond_wait(&(pThreadInfo->suspend_condition), &pThreadInfo->mSuspendMutex.m);
			pThreadInfo->bSuspended = FALSE;

			//pthread_cond_wait re-aquires the mutex once it resumes
			NCSMutexEnd(&pThreadInfo->mSuspendMutex);
		}
#else
ERROR: Need to code NCSThreadSuspend() in NCSUtil/thread.c
#endif
	}
}

/*
 ** Resume a thread.
 */
void NCSThreadResume(NCSThread *pThread)
{
	NCSThreadInfo *pThreadInfo;
	NCSMutexBegin(&mMutex);

	if(NULL != (pThreadInfo = NCSThreadGetInfo(pThread))) {
		if(pThreadInfo->bCollectStats) {
			pThreadInfo->tsStats.tsTotalSuspended = NCSGetTimeStampMs() - pThreadInfo->tsStats.tsSuspendStart;
			pThreadInfo->tsStats.nResumes += 1;
		}

#ifdef WIN32

		ResumeThread(pThreadInfo->hThread);

#elif defined(PALM)
#elif defined(MACINTOSH)

#ifdef MAC_PREEMPTIVE
		if( pThreadInfo->bSuspended ) {
			MPSignalSemaphore( pThreadInfo->suspend_condition );		
		}
#else
		{
			OSErr eResult;
			ThreadState threadState;

			eResult = GetThreadState(pThreadInfo->hThread, &threadState);
			/* If we can get the thread state, go for it! */
			if (eResult == noErr)
			{
				/* If it's not stopped, something is wrong. */
				if (threadState != kStoppedThreadState) {
					//DebugStr("\pWake-up thread is in the wrong state!");
				} else {
					/* Should be sleeping, mark it for wake up! */
					eResult = SetThreadState( pThreadInfo->hThread, 
							kReadyThreadState, 
							kNoThreadID );
				}
			}
			//} while ( eResult != noErr );
			if( eResult == noErr ) {
				eResult = YieldToThread(pThreadInfo->hThread);
				if( eResult != noErr ) {
					printf("bugger.\n");
				}
			}
	}
#endif // MAC_PREEMPTIVE

#elif defined POSIX
	NCSMutexBegin(&pThreadInfo->mSuspendMutex);
	if( pThreadInfo->bSuspended ) {
		pthread_cond_signal(&pThreadInfo->suspend_condition);
	}
	NCSMutexEnd(&pThreadInfo->mSuspendMutex);
#else
ERROR: Need to code NCSThreadResume() in NCSUtil/thread.c
#endif
}
	NCSMutexEnd(&mMutex);
}

/*
 ** Get current thread.
 */
NCSThread *NCSThreadGetCurrent(void)
{
#ifdef WIN32
	NCSThread *pThread = (NCSThread*)NCSThreadLSGetValue(ThreadIDKey);

	if(!pThread) {
		/*
		 ** Non NCSThread, create an NCSThreadInfo struct for it.
		 */
		NCSThreadInfo *pThreadInfo = (NCSThreadInfo*)NCSMalloc(sizeof(NCSThreadInfo), TRUE);

		if(pThreadInfo) {
			NCSMutexBegin(&mMutex);

			pThreadInfo->tid = tidNextTID++;

#ifdef WIN32
#ifdef _WIN32_WCE
			pThreadInfo->hThread = GetCurrentThread(); // FIXME - no DuplicateHandle()?
#else /* _WIN32_WCE */
			DuplicateHandle(GetCurrentProcess(),
					GetCurrentThread(),
					GetCurrentProcess(),
					&(pThreadInfo->hThread),
					0,
					TRUE,
					DUPLICATE_SAME_ACCESS);
#endif /* _WIN32_WCE */
#endif /* WIN32 */
			pThreadInfo->bThreadRunning = TRUE;
			pThreadInfo->pStartData = NULL;

			NCSArrayAppendElement(ppThreadInfos, nThreadInfos, &pThreadInfo);
			pThread = &(pThreadInfo->tid);
			NCSThreadLSSetValue(ThreadIDKey, pThread);

			NCSMutexEnd(&mMutex);
		}
	}
	return(pThread);
#elif defined(MACINTOSH)
#ifdef MAC_PREEMPTIVE
	MPTaskID  currentThread = MPCurrentTaskID();

	INT32 i;
	NCSThreadInfo *pThreadInfo = (NCSThreadInfo*)NULL;
	NCSThread *tid;
	NCSMutexBegin(&mMutex);
	for(i = 0; i < nThreadInfos; i++) {
		if(ppThreadInfos[i]->hThread == currentThread) {
			pThreadInfo = ppThreadInfos[i];
			break;
		}
	}	
	tid = &(pThreadInfo->tid);
	NCSMutexEnd(&mMutex);
	return(tid);	
#else
	ThreadID hThread;
	OSStatus errWhatTaskErr = GetCurrentThread( &hThread );

	if( errWhatTaskErr == noErr ) {
		INT32 i;
		NCSThreadInfo *pThreadInfo = (NCSThreadInfo*)NULL;
		NCSThread *tid;
		NCSMutexBegin(&mMutex);
		for(i = 0; i < nThreadInfos; i++) {
			if(ppThreadInfos[i]->hThread == hThread) {
				pThreadInfo = ppThreadInfos[i];
				break;
			}
		}	
		tid = &(pThreadInfo->tid);
		NCSMutexEnd(&mMutex);
		return(tid);
	}
	else return 0;
#endif // MAC_PREEMPTIVE
#elif defined POSIX
	pthread_t t = pthread_self();
	INT32 i;
	NCSThreadInfo *pThreadInfo = (NCSThreadInfo*)NULL;
	NCSThread *tid;
	NCSMutexBegin(&mMutex);
	for(i = 0; i < nThreadInfos; i++) {
		if(ppThreadInfos[i]->thread == t) {
			pThreadInfo = ppThreadInfos[i];
			break;
		}
	}	
	tid = &(pThreadInfo->tid);
	NCSMutexEnd(&mMutex);
	return(tid);
#endif
}

/*
 ** Is thread running
 */
BOOLEAN NCSThreadIsRunning(NCSThread *pThread)
{
	BOOLEAN bThreadRunning = FALSE;
	NCSThreadInfo *pThreadInfo;
	NCSMutexBegin(&mMutex);

	if(NULL != (pThreadInfo = NCSThreadGetInfo(pThread))) {
		bThreadRunning = pThreadInfo->bThreadRunning;

#ifdef WIN32
		{
			DWORD dwExitCode = 0;

			if(bThreadRunning) {
				GetExitCodeThread(pThreadInfo->hThread, &dwExitCode);

				if(dwExitCode != STILL_ACTIVE) {
					bThreadRunning = FALSE;
				}
			}
		}
#elif defined POSIX
		{
			int  policy;
			struct sched_param param;

			if(pthread_getschedparam(pThreadInfo->thread,  
						&policy,
						&param) == ESRCH) {
				bThreadRunning = FALSE;
			}
		}
#elif defined MACINTOSH
#ifdef MAC_PREEMPTIVE
		/*if(bThreadRunning) {
		  OSStatus result = MPSetTaskType ( pThreadInfo->hThread, 0);
		  if( result == kMPTaskStoppedErr ) {
		  bThreadRunning = pThreadInfo->bThreadRunning = FALSE;
		  }
		  }*/
#else
		{
			OSErr err;
			ThreadState state;

			if(bThreadRunning) {
				err = GetThreadState(pThreadInfo->hThread, &state);

				if(err == threadNotFoundErr || state == kStoppedThreadState) {
					bThreadRunning = FALSE;
					if( err == threadNotFoundErr ) { // thread has exited
						pThreadInfo->hThread = 0;
					}
				}
			}
		}
#endif //MAC_PREEMPTIVE
#endif
	}
	NCSMutexEnd(&mMutex);
	return(bThreadRunning);
}


/*
 ** Is thread suspended
 */
BOOLEAN NCSThreadIsSuspended(NCSThread *pThread)
{
	BOOLEAN bSuspended = FALSE;
	NCSThreadInfo *pThreadInfo;
	NCSMutexBegin(&mMutex);

	if(NULL != (pThreadInfo = NCSThreadGetInfo(pThread))) {

#ifdef WIN32

		// Suspend returns previous suspend count.
		if(SuspendThread(pThreadInfo->hThread) > 0) {
			bSuspended = TRUE;
		}
		// Resume the thread.
		ResumeThread(pThreadInfo->hThread);
#elif defined(PALM)
#elif defined(MACINTOSH)
#ifdef MAC_PREEMPTIVE
		bSuspended = pThreadInfo->bSuspended;
#else
		bSuspended = !NCSThreadIsRunning(pThread);
#endif //MAC_PREEMPTIVE
#elif defined(POSIX)
		bSuspended = pThreadInfo->bSuspended;
#else
ERROE: Code NCSThreadIsSuspended() in NCSUtil/thread.c
#endif

	}
	NCSMutexEnd(&mMutex);
	return(bSuspended);
}

/*
 ** Yield remainder of timeslice to other threads
 */
BOOLEAN NCSThreadYield(void)
{
#ifdef WIN32
	Sleep(0);//SwitchToThread());
#elif defined(PALM)
#elif defined(MACINTOSH)
#ifdef MAC_PREEMPTIVE
	MPYield();
#else
	YieldToAnyThread();
#endif //MAC_PREEMPTIVE
#elif defined(SOLARIS)
	//sched_yield();
	yield();
#elif defined(HPUX)
	sched_yield();
#elif defined(LINUX)
	sched_yield();
	//pthread_yield();
#elif defined(MACOSX)
	pthread_yield_np();
#else
ERROR: NCSThreadYield() needs coding in NCSUtil/thread.c
#endif
	       return(FALSE);
}

/*
 ** Get thread stats
 */
NCSThreadStats NCSThreadGetStats(NCSThread *pThread)
{
	NCSThreadInfo *pThreadInfo;
	NCSThreadStats tsStats = { 0 };

	NCSMutexBegin(&mMutex);
	if(NULL != (pThreadInfo = NCSThreadGetInfo(pThread))) {
		if(pThreadInfo->bThreadRunning) {
			pThreadInfo->tsStats.tsTotalRunning = NCSGetTimeStampMs() - pThreadInfo->tsStats.tsStart;
		}
		NCSMutexEnd(&mMutex);
		return(pThreadInfo->tsStats);
	}
	NCSMutexEnd(&mMutex);
	return(tsStats);
}

/*
 ** Enable Thread stats collection
 */
void NCSThreadEnableStats(NCSThread *pThread)
{
	NCSThreadInfo *pThreadInfo;

	NCSMutexBegin(&mMutex);
	if(NULL != (pThreadInfo = NCSThreadGetInfo(pThread))) {
		pThreadInfo->bCollectStats = TRUE;
	}
	NCSMutexEnd(&mMutex);
}

/*
 ** Disable Thread stats collection
 */
void NCSThreadDisableStats(NCSThread *pThread)
{
	NCSThreadInfo *pThreadInfo;
	NCSMutexBegin(&mMutex);
	if(NULL != (pThreadInfo = NCSThreadGetInfo(pThread))) {
		pThreadInfo->bCollectStats = FALSE;
	}
	NCSMutexEnd(&mMutex);
}

/*
 ** Alloc a TLS key
 */
NCSThreadLSKey NCSThreadLSAlloc(void)
{
#ifdef WIN32

	return((NCSThreadLSKey)TlsAlloc());

#elif defined(PALM)
	return((NCSThreadLSKey)0);
#elif defined(MACINTOSH)
	return((NCSThreadLSKey)0);
#elif defined(POSIX)
	pthread_key_t *key = NCSMalloc(sizeof(pthread_key_t), TRUE);

	if(pthread_key_create(key, NULL) == 0) {
		return(key);
	} else {
		NCSFree(key);
		return(NULL);
	}
#else
ERROR: Need to code NCSThreadLSAlloc() in NCSUtil/thread.c
#endif
}

/*
 ** Free a TLS key
 */
void NCSThreadLSFree(NCSThreadLSKey Key)
{
#ifdef WIN32

	TlsFree((DWORD)Key);

#elif defined(PALM)
#elif defined(MACINTOSH)
#elif defined(POSIX)
	if(Key != NULL) {
		pthread_key_delete(*Key);
		NCSFree(Key);
	}
#else
ERROR: Need to code NCSThreadLSFree() in NCSUtil/thread.c
#endif
}

/*
 ** Set TLS void* value for this thread.
 */
void NCSThreadLSSetValue(NCSThreadLSKey Key, void *pValue)
{
#ifdef WIN32

	TlsSetValue((DWORD)Key, (LPVOID)pValue);

#elif defined(PALM)
#elif defined(MACINTOSH)
#elif defined(POSIX)
	pthread_setspecific(*Key, pValue);
#else
ERROR: Need to code NCSThreadLSSetValue() in NCSUtil/thread.c
#endif
}

/*
 ** Get TLS void* value for this thread.
 */
void *NCSThreadLSGetValue(NCSThreadLSKey Key)
{
#ifdef WIN32

	return((void*)TlsGetValue((DWORD)Key));

#elif defined(PALM)
	return((void*)NULL);
#elif defined(MACINTOSH)
	return((void*)NULL);
#elif defined(POSIX)
	return(pthread_getspecific(*Key));
#else
ERROR: Need to code NCSThreadLSSetValue() in NCSUtil/thread.c
#endif
}

/* 
 ** Set Thread priority
 */
BOOLEAN NCSThreadSetPriority(NCSThread *pThread,
		NCSThreadPriority pri)
{
	NCSThreadInfo *pThreadInfo;
	NCSMutexBegin(&mMutex);

	pThreadInfo = NCSThreadGetInfo(pThread);
	if(pThreadInfo) {
#ifdef WIN32
		switch(pri) {
			case NCS_THREAD_PRI_IDLE:
				SetThreadPriority(pThreadInfo->hThread, 
						THREAD_PRIORITY_IDLE);
				break;
			case NCS_THREAD_PRI_BELOW_NORMAL:
				SetThreadPriority(pThreadInfo->hThread, 
						THREAD_PRIORITY_BELOW_NORMAL);
				break;
			case NCS_THREAD_PRI_NORMAL:
			default:
				SetThreadPriority(pThreadInfo->hThread, 
						THREAD_PRIORITY_NORMAL);
				break;
			case NCS_THREAD_PRI_ABOVE_NORMAL:
				SetThreadPriority(pThreadInfo->hThread, 
						THREAD_PRIORITY_ABOVE_NORMAL);
				break;
			case NCS_THREAD_PRI_REALTIME:
				SetThreadPriority(pThreadInfo->hThread, 
						THREAD_PRIORITY_TIME_CRITICAL);
				break;
		}
#elif defined(PALM)
#elif defined(MACINTOSH)
#elif defined(POSIX)
#else
ERROR: Need to code NCSThreadSetPriority() in NCSUtil/thread.c
#endif
	   	NCSMutexEnd(&mMutex);
		return(TRUE);
	}
	NCSMutexEnd(&mMutex);
	return(FALSE);
}

/* 
 ** Get Thread priority
 */
NCSThreadPriority NCSThreadGetPriority(NCSThread *pThread)
{
	NCSThreadInfo *pThreadInfo;
	NCSMutexBegin(&mMutex);
	pThreadInfo = NCSThreadGetInfo(pThread);
	if(pThreadInfo) {
#ifdef WIN32
		switch(GetThreadPriority(pThreadInfo->hThread)) {
			case THREAD_PRIORITY_IDLE:
					NCSMutexEnd(&mMutex);
				return(NCS_THREAD_PRI_IDLE);
				break;
			case THREAD_PRIORITY_BELOW_NORMAL:
					NCSMutexEnd(&mMutex);
				return(NCS_THREAD_PRI_BELOW_NORMAL);
				break;
			case THREAD_PRIORITY_NORMAL:
			default:
					NCSMutexEnd(&mMutex);
				return(NCS_THREAD_PRI_NORMAL);
				break;
			case THREAD_PRIORITY_ABOVE_NORMAL:
					NCSMutexEnd(&mMutex);
				return(NCS_THREAD_PRI_ABOVE_NORMAL);
				break;
			case THREAD_PRIORITY_TIME_CRITICAL:
					NCSMutexEnd(&mMutex);
				return(NCS_THREAD_PRI_REALTIME);
				break;
		}	

#elif defined(PALM)||defined(MACINTOSH)||defined(POSIX)
		NCSMutexEnd(&mMutex);
		return(NCS_THREAD_PRI_NORMAL);
#else
ERROR: Need to code NCSThreadGetPriority() in NCSUtil/thread.c
#endif
	}
	NCSMutexEnd(&mMutex);
	return(NCS_THREAD_PRI_NORMAL);
}

/*
 ** Local functions.
 */

/*
 ** Get current thread info.
 */
static NCSThreadInfo *NCSThreadGetCurrentInfo(void)
{
	NCSThread *pThread;

	if(NULL != (pThread = NCSThreadGetCurrent())) {
		return(NCSThreadGetInfo(pThread));
	}
	return((NCSThreadInfo*)NULL);
}

/*
 ** Get thread info for given thread id.
 */
static NCSThreadInfo *NCSThreadGetInfo(NCSThread *pThread)
{
	INT32 i;
	NCSThreadInfo *pThreadInfo = (NCSThreadInfo*)NULL;

	if(ppThreadInfos) {
		for(i = 0; i < nThreadInfos; i++) {
			if(ppThreadInfos[i]->tid == *pThread) {
				pThreadInfo = ppThreadInfos[i];
				break;
			}
		}	
	}
	return(pThreadInfo);
}

/*
 ** Thread start func.
 */
static int NCSThreadStartFunc(NCSThreadStartData *pStartData)
{
#ifdef WIN32 // [05]
	static char msg[16384] = { '\0' };			/**[04]**/

	__try {										/**[04]**/
#endif	/* WIN32 */
		if(pStartData) {
			NCSThreadInfo *pThreadInfo;
			void (*pFunc)(void*);
			void *pData;

#if defined(MACINTOSH) && defined(MAC_PREEMPTIVE)
			//NCSThreadSuspend();
			OSStatus eResult;
			pStartData->pThreadInfo->bSuspended = TRUE;
			eResult = MPWaitOnSemaphore ( pStartData->pThreadInfo->suspend_condition, kDurationForever );
			pStartData->pThreadInfo->bSuspended = FALSE;
#endif //defined(MACINTOSH) && defined(MAC_PREEMPTIVE)

#ifdef POSIX
			NCSMutexBegin(&pStartData->pThreadInfo->mSuspendMutex);
			pStartData->pThreadInfo->bThreadStarted = TRUE;
			if( pStartData->pThreadInfo->bSuspended ) {
				pthread_cond_wait(&(pStartData->pThreadInfo->suspend_condition), &pStartData->pThreadInfo->mSuspendMutex.m);
				pStartData->pThreadInfo->bSuspended = FALSE;
			}
			NCSMutexEnd(&pStartData->pThreadInfo->mSuspendMutex);
#endif
			NCSMutexBegin(&mMutex);
			if(NULL != (pThreadInfo = NCSThreadGetInfo(pStartData->pThread))) {
				if(pThreadInfo->bCollectStats) {
					pThreadInfo->tsStats.tsStart = NCSGetTimeStampMs();
				}

				pFunc = pStartData->pFunc;
				pData = pStartData->pData;

				NCSThreadLSSetValue(ThreadIDKey, pStartData->pThread);
				NCSFree(pStartData);

				pThreadInfo->pStartData = NULL;
				NCSMutexEnd(&mMutex);

				(*pFunc)(pData);
			} else {
				NCSMutexEnd(&mMutex);
			}
		}
#ifdef WIN32 // [05]
	} __except (NCSDbgGetExceptionInfoMsg(_exception_info(), msg)) {														/**[04]**/
		char chName[MAX_PATH];
		char extended_msg[16384] = { '\0' };

		if(GetModuleFileName(NULL, OS_STRING(chName), MAX_PATH) &&	_strlwr(chName) && (strstr(chName, "inetinfo.exe") || strstr(chName, "dllhost.exe"))) {
			sprintf(extended_msg,
					"(thread) Image Web Server Version : %s\n%s",
					NCS_VERSION_STRING,msg);
			NCSLog(LOG_LOW, extended_msg, NCS_UNKNOWN_ERROR, (char*)NCSGetErrorText(NCS_UNKNOWN_ERROR));									/**[04]**/
#ifdef _WIN32_WCE
			//MessageBox(NULL, OS_STRING(extended_msg), NCS_T("Image Web Server Exception"), MB_ICONERROR|MB_OK); /**[04]**/
#else
			//MessageBox(NULL, OS_STRING(extended_msg), NCS_T("Image Web Server Exception"), MB_ICONERROR|MB_OK|MB_SERVICE_NOTIFICATION|MB_SYSTEMMODAL); /**[04]**/
#endif
		} else {
			sprintf(extended_msg,
					"(thread) ECW Version : %s\n%s",
					NCS_VERSION_STRING,msg);
			NCSLog(LOG_LOW, extended_msg, NCS_UNKNOWN_ERROR, (char*)NCSGetErrorText(NCS_UNKNOWN_ERROR));									/**[04]**/
#ifdef NCS_BUILD_UNICODE
			MessageBox(NULL, OS_STRING(extended_msg), NCS_T("ECW Exception"), MB_ICONERROR|MB_OK); /**[04]**/
#else
			MessageBox(NULL, OS_STRING(extended_msg), NCS_T("ECW Exception"), MB_ICONERROR|MB_OK|MB_SERVICE_NOTIFICATION|MB_SYSTEMMODAL); /**[04]**/
#endif
		}
	}
#endif	/* WIN32 */																														/**[04]**/
	NCSThreadExit(0);
	return(0);
}

/*
 ** Terminate running thread (DANGEROUS)
 */
BOOLEAN NCSThreadTerminate(NCSThread *pThread)
{
	BOOLEAN bThreadTerminated = FALSE;
	BOOLEAN bThreadRunning = FALSE;
	NCSThreadInfo *pThreadInfo;
	NCSMutexBegin(&mMutex);

	if(NULL != (pThreadInfo = NCSThreadGetInfo(pThread))) {
		bThreadRunning = pThreadInfo->bThreadRunning;

#ifdef WIN32
		{
			DWORD dwExitCode = 0;

			if(bThreadRunning) {
				bThreadTerminated = (BOOLEAN)TerminateThread(pThreadInfo->hThread, dwExitCode);

			}
		}
#elif defined(MACINTOSH)
		if( pThreadInfo->bThreadRunning && pThreadInfo->hThread ) {
#ifdef MAC_PREEMPTIVE
			OSErr eResult = MPTerminateTask ( pThreadInfo->hThread, (OSStatus)0 /*terminationStatus*/ );
			bThreadTerminated = TRUE;
			pThreadInfo->bThreadRunning = FALSE;
			pThreadInfo->hThread = 0;
#else
			//OSErr eResult = DisposeThread ( pThreadInfo->hThread, NULL, TRUE );
			OSErr eResult = SetThreadState( pThreadInfo->hThread, kStoppedThreadState, kNoThreadID );
			if( eResult == noErr ) {
				bThreadTerminated = TRUE;
				pThreadInfo->bThreadRunning = FALSE;
				pThreadInfo->hThread = 0;
			}
#endif //MAC_PREEMPTIVE
		}
#elif defined(POSIX)
		if(pthread_cancel(pThreadInfo->thread) == 0) {
			bThreadTerminated = TRUE;
		}
#endif
	}
	NCSMutexEnd(&mMutex);
	return (bThreadTerminated);
}

#ifdef WIN32
DWORD NCSThreadGetSysID(NCSThread* t)
{
	DWORD dwTID;
	NCSMutexBegin(&mMutex);
	dwTID = NCSThreadGetInfo(t)->dwTID;
	NCSMutexEnd(&mMutex);
	return(dwTID);
}
#elif defined(MACINTOSH)
MacThreadID NCSThreadGetSysID(NCSThread* t)
{
	MacThreadID hThread;
	NCSMutexBegin(&mMutex);
	hThread = NCSThreadGetInfo(t)->hThread;
	NCSMutexEnd(&mMutex);
	return(hThread);
}
#elif defined(POSIX)
pthread_t *NCSThreadGetSysID(NCSThread* t)
{
	pthread_t *thread;
	NCSMutexBegin(&mMutex);
	thread = &(NCSThreadGetInfo(t)->thread);
	NCSMutexEnd(&mMutex);
	return(thread);
}
#else
ERROR: Need to code NCSThreadGetSysID() in NCSUtil/thread.c
#endif //[07]


#if defined(WIN32)&&defined(_X86_)&&!defined(_WIN32_WCE)

//-------------------------------------------------------------------------------------------------
//
// Copyright C 2001, Intel Corporation . Other brands and names may be claimed as the property of others. 
//
//
// CPU Counting Utility
// Date   : 10/30/2001
// Version: 1.4
// 
//
//
// File Name: CPUCount.cpp
//
// Note: 1) LogicalNum = Number of logical processors per PHYSICAL PROCESSOR.  If you want to count
//       the total number of logical processors, multiply this number with the total number of 
//       physical processors (PhysicalNum)
//
//       2) To detect whether hyper-threading is enabled or not is to see how many logical ID exist 
//       per single physical ID in APIC
//
//       3) For systems that don't support hyper-threading like AMD or PIII and below. the variable
//       LogicalNum will be set to 1 (which means number of logical processors equals to number of
//       physical processors.)
//    
//       4) Hyper-threading cannot be detected when application cannot access all processors in 
//       the system. The number of physical processors will be set to 255.  Make sure to enable ALL 
//       physical processors at startup of windows, and applications calling this function, CPUCount,
//       are NOT restricted to run on any particular logical or physical processors(can run on ALL
//       processors.)
// 
//       5) Windows currently can handle up to 32 processors. 
//
//
//-------------------------------------------------------------------------------------------------


#define HT_BIT             0x10000000     // EDX[28]  Bit 28 is set if HT is supported
#define FAMILY_ID          0x0F00         // EAX[11:8] Bit 8-11 contains family processor ID.
#define PENTIUM4_ID        0x0F00         
#define EXT_FAMILY_ID      0x0F00000      // EAX[23:20] Bit 20-23 contains extended family processor ID
#define NUM_LOGICAL_BITS   0x00FF0000     // EBX[23:16] Bit 16-23 in ebx contains the number of logical
// processors per physical processor when execute cpuid with 
// eax set to 1

#define INITIAL_APIC_ID_BITS  0xFF000000  // EBX[31:24] Bits 24-31 (8 bits) return the 8-bit unique 
// initial APIC ID for the processor this code is running on.
// Default value = 0xff if HT is not supported


// Status Flag
#define HT_NOT_CAPABLE           0
#define HT_ENABLED               1
#define HT_DISABLED              2
#define HT_SUPPORTED_NOT_ENABLED 3
#define HT_CANNOT_DETECT         4

unsigned int  HTSupported(void);
unsigned char LogicalProcPerPhysicalProc(void);
unsigned char GetAPIC_ID(void);
unsigned char CPUCount(unsigned char *,
		unsigned char *);

unsigned int HTSupported(void)
{


	unsigned int Regedx      = 0,
		     Regeax      = 0,
		     VendorId[3] = {0, 0, 0};

	__try    // Verify cpuid instruction is supported
	{
		__asm
		{
			xor eax, eax          // call cpuid with eax = 0
				cpuid                 // Get vendor id string
				mov VendorId, ebx
				mov VendorId + 4, edx
				mov VendorId + 8, ecx

				mov eax, 1            // call cpuid with eax = 1
				cpuid
				mov Regeax, eax      // eax contains family processor type
				mov Regedx, edx      // edx has info about the availability of hyper-Threading

		}
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return(0);                   // cpuid is unavailable
	}

	if (((Regeax & FAMILY_ID) ==  PENTIUM4_ID) || (Regeax & EXT_FAMILY_ID))
		if (VendorId[0] == 'uneG')
			if (VendorId[1] == 'Ieni')
				if (VendorId[2] == 'letn')
					return(Regedx & HT_BIT);    // Genuine Intel with hyper-Threading technology

	return 0;    // Not genuine Intel processor
}


unsigned char LogicalProcPerPhysicalProc(void)
{
	unsigned int Regebx = 0;
	if (!HTSupported()) return (unsigned char) 1;  // HT not supported
	// Logical processor = 1
	__asm
	{
		mov eax, 1
			cpuid
			mov Regebx, ebx
	}

	return (unsigned char) ((Regebx & NUM_LOGICAL_BITS) >> 16);
}


unsigned char GetAPIC_ID(void)
{

	unsigned int Regebx = 0;
	if (!HTSupported()) return (unsigned char) -1;  // HT not supported
	// Logical processor = 1
	__asm
	{
		mov eax, 1
			cpuid
			mov Regebx, ebx
	}

	return (unsigned char) ((Regebx & INITIAL_APIC_ID_BITS) >> 24);

}


unsigned char CPUCount(unsigned char *LogicalNum,
		unsigned char *PhysicalNum)
{
	unsigned char StatusFlag  = 0;
	SYSTEM_INFO info;


	*PhysicalNum = 0;
	*LogicalNum  = 0;
	info.dwNumberOfProcessors = 0;
	GetSystemInfo (&info);

	// Number of physical processors in a non-Intel system
	// or in a 32-bit Intel system with Hyper-Threading technology disabled
	*PhysicalNum = (unsigned char) info.dwNumberOfProcessors;  

	if (HTSupported())
	{
		unsigned char HT_Enabled = 0;

		*LogicalNum= LogicalProcPerPhysicalProc();

		if (*LogicalNum >= 1)    // >1 Doesn't mean HT is enabled in the BIOS
			// 
		{
			HANDLE hCurrentProcessHandle;
			DWORD  dwProcessAffinity;
			DWORD  dwSystemAffinity;
			DWORD  dwAffinityMask;

			// Calculate the appropriate  shifts and mask based on the 
			// number of logical processors.

			unsigned char i = 1,
				      PHY_ID_MASK  = 0xFF,
				      PHY_ID_SHIFT = 0;

			while (i < *LogicalNum)
			{
				i *= 2;
				PHY_ID_MASK <<= 1;
				PHY_ID_SHIFT++;

			}

			hCurrentProcessHandle = GetCurrentProcess();
			GetProcessAffinityMask(hCurrentProcessHandle, 
					&dwProcessAffinity,
					&dwSystemAffinity);

			// Check if available process affinity mask is equal to the
			// available system affinity mask
			if (dwProcessAffinity != dwSystemAffinity)
			{
				StatusFlag = HT_CANNOT_DETECT;
				*PhysicalNum = (unsigned char)-1;
				return StatusFlag;
			}
			dwAffinityMask = 1;
			while (dwAffinityMask != 0 && dwAffinityMask <= dwProcessAffinity)
			{
				// Check if this CPU is available
				if (dwAffinityMask & dwProcessAffinity)
				{
					if (SetProcessAffinityMask(hCurrentProcessHandle,
								dwAffinityMask))
					{
						unsigned char APIC_ID,
						LOG_ID,
						PHY_ID;

						Sleep(0); // Give OS time to switch CPU

						APIC_ID = GetAPIC_ID();
						LOG_ID  = APIC_ID & ~PHY_ID_MASK;
						PHY_ID  = APIC_ID >> PHY_ID_SHIFT;

						if (LOG_ID != 0)  HT_Enabled = 1;
					}
				}
				dwAffinityMask = dwAffinityMask << 1;
			}
			// Reset the processor affinity
			SetProcessAffinityMask(hCurrentProcessHandle, dwProcessAffinity);

			if (*LogicalNum == 1)  // Normal P4 : HT is disabled in hardware
				StatusFlag = HT_DISABLED;
			else
				if (HT_Enabled) {
					// Total physical processors in a Hyper-Threading enabled system.
					*PhysicalNum = *PhysicalNum / (*LogicalNum);
					StatusFlag = HT_ENABLED;
				}
				else StatusFlag = HT_SUPPORTED_NOT_ENABLED;
		}
	}
	else
	{
		// Processors do not have Hyper-Threading technology
		StatusFlag = HT_NOT_CAPABLE;
		*LogicalNum = 1;
	}
	return StatusFlag;
}
#endif // WIN32 & _X86_


UINT32 NCSGetNumCPUs(void)
{

	UINT32 ncpus = 1;

#ifdef WIN32
#if !defined(_WIN32_WCE)&&defined(_X86_)

	unsigned char LogicalNum   = 0,  // Number of logical CPU per ONE PHYSICAL CPU
		      PhysicalNum  = 0,  // Total number of physical processor
		      HTStatusFlag = 0;  

	HTStatusFlag = CPUCount(&LogicalNum, &PhysicalNum);

	if(PhysicalNum != (unsigned char)-1) {
		ncpus = LogicalNum * PhysicalNum;
	} else 
#endif // !_WIn32_WCE
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		ncpus = (UINT32)si.dwNumberOfProcessors;		
	}
#else
#ifdef SOLARIS
	long n;
	n = sysconf(_SC_NPROCESSORS_ONLN);
	if(n > 0) {
		ncpus = n;
	}
#else
#ifdef HPUX
#include <sys/param.h>
#include <sys/pstat.h>

	struct pst_dynamic psd;
	struct pst_processor *psp;

	if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) != -1) {
		fprintf(stderr, "NCSGetNumCPUs() : value not tested [%d]\n",psd.psd_proc_cnt);
		return (psd.psd_proc_cnt);
	}
#else
#ifdef LINUX
	long n;
	n = sysconf(_SC_NPROCESSORS_ONLN);
	if(n > 0) {
		ncpus = n;
	}
#endif
#endif
#endif
#endif

	return(ncpus);
}

UINT32 NCSGetNumPhysicalCPUs(void) 
{
	UINT32 ncpus = NCSGetNumCPUs();

#if defined(WIN32)&&!defined(_WIN32_WCE)&&defined(_X86_)
	unsigned char LogicalNum   = 0,  // Number of logical CPU per ONE PHYSICAL CPU
		      PhysicalNum  = 0,  // Total number of physical processor
		      HTStatusFlag = 0;  

	HTStatusFlag = CPUCount(&LogicalNum, &PhysicalNum);

	if(PhysicalNum != (unsigned char)-1) {
		ncpus = PhysicalNum;
	}

#endif
	return(ncpus);
}

