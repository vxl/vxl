/********************************************************
** Copyright 2002 Earth Resource Mapping Ltd.
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
** FILE:     $Archive: /NCS/Source/C/NCSUtil/NCSEvent.cpp $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCEvent class implementation
** EDITS:    [01] 28-02-05 RAR Code shifted to NCSUtil and renamed to CNCSEvent
 *******************************************************/

#ifndef NCSUTIL_H
#include "NCSUtil.h"
#endif // NCSUTIL_H

#include "NCSEvent.h"

#if defined( SOLARIS )
#include <time.h>
#endif

NCSEvent NCSEventCreateEx(BOOLEAN bManualReset, BOOLEAN bInitialState, char *pLockName)
{
	CNCSEvent *pEvent = new CNCSEvent(bManualReset?true:false, bInitialState?true:false, pLockName);
	if( pEvent != NULL ) {
		return pEvent;
	}

	return NULL;
}

NCSEvent NCSEventCreate()
{
	CNCSEvent *pEvent = new CNCSEvent();
	if( pEvent != NULL ) {
		return pEvent;
	}

	return NULL;
}


BOOLEAN NCSEventSet(NCSEvent event)
{
	if( event != NULL ) {
		CNCSEvent *pEvent = (CNCSEvent *)event;
		return pEvent->Set()?TRUE:FALSE;
	}

	return FALSE;
}

BOOLEAN NCSEventReset(NCSEvent event)
{
	if( event != NULL ) {
		CNCSEvent *pEvent = (CNCSEvent *)event;
		return pEvent->Reset()?TRUE:FALSE;
	}

	return FALSE;
}

BOOLEAN NCSEventWait(NCSEvent event, NCSTimeStampMs tsTimeout)
{
	if( event != NULL ) {
		CNCSEvent *pEvent = (CNCSEvent *)event;
		return pEvent->Wait(tsTimeout)?TRUE:FALSE;
	}

	return FALSE;
}

void NCSEventDestroy(NCSEvent event)
{
	if( event != NULL ) {
		CNCSEvent *pEvent = (CNCSEvent *)event;
		
		delete pEvent;
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSEvent::CNCSEvent(bool bManualReset, 
						   bool bInitialState, 
						   char *pLockName)
{
#ifdef WIN32
	m_hEvent = CreateEvent(NULL, bManualReset, bInitialState, OS_STRING(pLockName));
#elif defined( SOLARIS )
	pthread_cond_init(&wait_cond, NULL);
	m_bSignaled = bInitialState;
	m_bManualReset = bManualReset;
#else
	if(pLockName) {
		m_pLockName = NCSStrDup(pLockName);
	} else {
		m_pLockName = NULL;
	}
	m_bManualReset = bManualReset;
	m_bSignalled = bInitialState;
#endif
}

CNCSEvent::~CNCSEvent()
{
#ifdef WIN32
	CloseHandle(m_hEvent);
#elif defined( SOLARIS )
	pthread_cond_destroy(&wait_cond);
#else
	NCSFree(m_pLockName);
#endif
}

//////////////////////////////////////////////////////////////////////
// Members
//////////////////////////////////////////////////////////////////////

bool CNCSEvent::Set()
{
#ifdef WIN32
	return(SetEvent(m_hEvent) ? true : false);
#elif defined( SOLARIS )
	m_Mutex.Lock();
	m_bSignaled = true;
	int nResult = pthread_cond_broadcast(&wait_cond);
	m_Mutex.UnLock();
	return (nResult == 0)?true:false;
#else
	if(m_pLockName) {
		NCSGlobalLockInfo *pLock = NCSGlobalLock(m_pLockName);
		if(pLock) {
			m_bSignalled = true;
			NCSGlobalUnlock(pLock);
			return(true);
		}
		return(false);
	} else {
		m_Mutex.Lock();
		m_bSignalled = true;
		m_Mutex.UnLock();
		return(true);
	}
#endif
}
bool CNCSEvent::Reset()
{
#ifdef WIN32
	return(ResetEvent(m_hEvent) ? true : false);
#elif defined( SOLARIS )
	m_Mutex.Lock();
	m_bSignaled = false;
	m_Mutex.UnLock();
	return true;
#else
	bool bRet = false;

	if(m_pLockName) {
		NCSGlobalLockInfo *pLock = NCSGlobalLock(m_pLockName);
		if(pLock) {
			m_bSignalled = false;
			bRet = true;
			NCSGlobalUnlock(pLock);
		}
	} else {
		m_Mutex.Lock();
		m_bSignalled = false;
		bRet = true;
		m_Mutex.UnLock();
	}
	return(bRet);
#endif
}

bool CNCSEvent::Wait(NCSTimeStampMs tsTimeout)
{
	bool bRet = false;
#ifdef WIN32
	switch(WaitForSingleObject(m_hEvent, (DWORD)tsTimeout)) {
		default:
		case WAIT_ABANDONED: 
		case WAIT_TIMEOUT:
				bRet = false;
			break;
		case WAIT_OBJECT_0: 
				bRet = true;
			break;
	}
#elif defined( SOLARIS )
	m_Mutex.Lock();
	if( !m_bSignaled ) {
		int nResult = -1;
		if( tsTimeout >= 0 ) {
			struct timespec abstime;
			NCSTimeStampMs tsEnd = NCSGetTimeStampMs() + tsTimeout;
			abstime.tv_sec = tsEnd/1000;
			abstime.tv_nsec = (tsEnd%1000)*1000*1000;
			nResult = pthread_cond_timedwait(&wait_cond, m_Mutex.GetNative(), &abstime);
		} else {
			nResult = pthread_cond_wait(&wait_cond, m_Mutex.GetNative());	
		}
		if( nResult == 0 ) {
			bRet = true;
			if(!m_bManualReset) {
				m_bSignaled = false;
			}
		} else {
			bRet = false;
		}
	} else {
		bRet = true;
		if(!m_bManualReset) {
			m_bSignaled = false;
		}
	}
	m_Mutex.UnLock();
	return bRet;
#else
	NCSTimeStampMs tsEnd = NCSGetTimeStampMs() + tsTimeout;

	while(bRet == false) {
		if(m_pLockName) {
			NCSGlobalLockInfo *pLock = NCSGlobalLock(m_pLockName);
			if(pLock) {
				if(m_bSignalled) {
					if(!m_bManualReset) {
						m_bSignalled = false;
					}
					bRet = true;
				}
				NCSGlobalUnlock(pLock);
			} else {
				// Error
				break;
			}
		} else {
			m_Mutex.Lock();
			if(m_bSignalled) {
				if(!m_bManualReset) {
					m_bSignalled = false;
				}
				bRet = true;
			}
			m_Mutex.UnLock();
		}
		if(!bRet) {
			//NCSThreadYield();
			NCSSleep(10);
			if(tsTimeout != -1 && NCSGetTimeStampMs() > tsEnd) {
				break;
			}
		}
	}
#endif
	return(bRet);
}

