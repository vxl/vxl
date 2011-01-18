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
** FILE:   	NCSThread.h
** CREATED:	Fri Feb 26 09:19:00 WST 1999
** AUTHOR: 	Simon Cope
** PURPOSE:	NCS Thread handling.
** EDITS:
** [01] 03Jan01	 ny	Added CNCSThread wrapper class
** [02] 02May01	 ny	Added GetSysID() method
 *******************************************************/

#ifndef NCSTHREAD_H
#define NCSTHREAD_H

#ifndef NCSTTYPES_H
#include "NCSTypes.h"
#endif

#ifdef WIN32
#ifndef _WIN32_WCE
#include <process.h>
#endif /* _WIN32_WCE */
#include <windows.h>
#ifdef _WIN32_WINNT
#include <winbase.h>
#endif

#elif defined MACINTOSH
#	ifdef MAC_PREEMPTIVE
#		include <Multiprocessing.h>
#	else
#		include <Threads.h>
#	endif //MAC_PREEMPTIVE
#elif defined POSIX
#include <pthread.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NCSTIMESTAMP_H
#include "NCSTimeStamp.h"
#endif
#ifndef NCSMALLOC_H
#include "NCSMalloc.h"
#endif
#ifndef NCSMUTEX_H
#include "NCSMutex.h"
#endif

typedef struct {
	NCSTimeStampMs tsStart;
	NCSTimeStampMs tsTotalRunning;
	NCSTimeStampMs tsSuspendStart;
	NCSTimeStampMs tsTotalSuspended;
	INT64		   nSuspends;
	INT64		   nResumes;
} NCSThreadStats;

typedef enum {
	NCS_THREAD_PRI_IDLE				= 0,
	NCS_THREAD_PRI_BELOW_NORMAL			= 1,
	NCS_THREAD_PRI_NORMAL				= 2,
	NCS_THREAD_PRI_ABOVE_NORMAL			= 3,
	NCS_THREAD_PRI_REALTIME				= 4
} NCSThreadPriority;

typedef UINT32 NCSThread;
#define NCS_NULL_THREAD_ID 0

#if defined(WIN32)||defined(MACINTOSH)
typedef UINT32 NCSThreadLSKey;
#elif defined(POSIX)
typedef pthread_key_t *NCSThreadLSKey;
#endif

void NCSThreadInit(void);
void NCSThreadFini(void);
BOOLEAN NCSThreadSpawn(NCSThread *pThread, void (*pFunc)(void*), void *pData, BOOLEAN bCreateSuspended);
void NCSThreadFreeInfo(NCSThread *pThread);
void NCSThreadExit(INT32 dwExitId);
void NCSThreadSuspend(void);
void NCSThreadResume(NCSThread *pThread);
NCSThread *NCSThreadGetCurrent(void);
NCSThreadStats NCSThreadGetStats(NCSThread *pThread);
void NCSThreadEnableStats(NCSThread *pThread);
void NCSThreadDisableStats(NCSThread *pThread);
BOOLEAN NCSThreadIsRunning(NCSThread *pThread);
BOOLEAN NCSThreadIsSuspended(NCSThread *pThread);
BOOLEAN NCSThreadYield(void);
BOOLEAN NCSThreadSetPriority(NCSThread *pThread, NCSThreadPriority pri);
NCSThreadPriority NCSThreadGetPriority(NCSThread *pThread);

NCSThreadLSKey NCSThreadLSAlloc(void);
void NCSThreadLSFree(NCSThreadLSKey Key);
void NCSThreadLSSetValue(NCSThreadLSKey Key, void *pValue);
void *NCSThreadLSGetValue(NCSThreadLSKey Key);

BOOLEAN NCSThreadTerminate(NCSThread *pThread);
#ifdef WIN32
DWORD NCSThreadGetSysID(NCSThread* t);
#elif defined(MACINTOSH)
#	ifdef MAC_PREEMPTIVE
#		define MacThreadID MPTaskID
#	else
#		define MacThreadID ThreadID
#	endif //MAC_PREEMPTIVE
MacThreadID NCSThreadGetSysID(NCSThread* t);
#elif defined(POSIX)
pthread_t *NCSThreadGetSysID(NCSThread* t);
#else
ERROR: Need NCSThreadGetSysID() prototype in NCSThread.h
#endif //[07]

#ifdef __cplusplus
}

#ifndef NCS_NO_UTIL_LINK

// CNCSThread
class NCS_EXPORT CNCSThread {
private:
	NCSThread	m_Thread;
	bool		m_bRun;
	static void ThreadFunc(void *pDataParam);
	void FreeInfo();

public:
	void *m_pData;

	CNCSThread();
	virtual ~CNCSThread();
	virtual bool Spawn(void *pData=NULL, bool bCreateSuspended=false);
	
	virtual void Exit(INT32 dwExitId);
	virtual void Suspend(void);
	virtual void Resume();
	virtual NCSThread *GetCurrent(void);
	virtual NCSThreadStats GetStats();
	virtual void EnableStats();
	virtual void DisableStats();
	virtual bool IsRunning();
	virtual bool IsSuspended();
	virtual bool ThreadYield(void);
	virtual bool SetPriority(NCSThreadPriority pri);
	virtual NCSThreadPriority GetPriority();
	virtual bool Terminate();
	virtual bool Run();
	virtual bool Stop(bool bWait = true);

	virtual void Work(void *pData) = 0;

	virtual NCSThread GetID(void);

#ifdef WIN32
	virtual DWORD GetSysID(void);
#elif defined MACINTOSH
	virtual MacThreadID GetSysID(void);
#elif defined POSIX
	virtual pthread_t *GetSysID(void);
#endif
};

#endif //NCS_NO_UTIL_LINK

#endif // __cplusplus

#endif /* NCSTHREAD_H */
