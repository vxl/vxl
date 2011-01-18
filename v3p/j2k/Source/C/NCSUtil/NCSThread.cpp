/********************************************************
** Copyright 2001 Earth Resource Mapping Ltd.
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
** FILE:   	NCSUtil/NCSThread.cpp
** CREATED:	Fri May 12 2001
** AUTHOR: 	Simon Cope
** PURPOSE:	NCS Thread class.
** EDITS:
 *******************************************************/

#include "NCSUtil.h"

void CNCSThread::ThreadFunc(void *pDataParam)
{
	//void **pStartData = (void**)pDataParam;
	class CNCSThread *pThis = (class CNCSThread *)pDataParam;
	void *pData = pThis->m_pData;
	//NCSFree(pStartData);
	pThis->Work(pData);
}

CNCSThread::CNCSThread() 
{ 
	m_Thread = NCS_NULL_THREAD_ID;
	m_bRun = false;
	m_pData = NULL;
}

CNCSThread::~CNCSThread() 
{
	if( Terminate() ) {
		FreeInfo();
	}
}

bool CNCSThread::Spawn(void *pData, bool bCreateSuspended) {
	//void **pStartData = (void **)NCSMalloc(2 * sizeof(void*), TRUE);
	//pStartData[0] = (void*)this;
	//pStartData[1] = pData;
	m_pData = pData;
	m_bRun = true;
	return(NCSThreadSpawn(&m_Thread, CNCSThread::ThreadFunc, (void*)this, bCreateSuspended)==TRUE);
}

void CNCSThread::FreeInfo() {
	NCSThreadFreeInfo(&m_Thread);
}

void CNCSThread::Exit(INT32 dwExitId) {
	NCSThreadExit(dwExitId);
}

void CNCSThread::Suspend(void) {
	NCSThreadSuspend();
}

void CNCSThread::Resume() {
	NCSThreadResume(&m_Thread);
}

NCSThread *CNCSThread::GetCurrent(void) {
	return (NCSThreadGetCurrent());
}

NCSThreadStats CNCSThread::GetStats() {
	return(NCSThreadGetStats(&m_Thread));
}

void CNCSThread::EnableStats() {
	NCSThreadEnableStats(&m_Thread);
}

void CNCSThread::DisableStats() {
	NCSThreadDisableStats(&m_Thread);
}

bool CNCSThread::IsRunning() {
	return (NCSThreadIsRunning(&m_Thread)==TRUE);
}

bool CNCSThread::IsSuspended() {
	return (NCSThreadIsSuspended(&m_Thread)==TRUE);
}

bool CNCSThread::ThreadYield(void) {
	return (NCSThreadYield()==TRUE);
}

bool CNCSThread::SetPriority(NCSThreadPriority pri) {
	return (NCSThreadSetPriority(&m_Thread,pri)==TRUE);
}

NCSThreadPriority CNCSThread::GetPriority() {
	return (NCSThreadGetPriority(&m_Thread));
}

bool CNCSThread::Terminate() {
	return (NCSThreadTerminate(&m_Thread)==TRUE);
}

NCSThread CNCSThread::GetID(void) {
	return(m_Thread);
}

#ifdef WIN32
DWORD CNCSThread::GetSysID(void) {
	return(NCSThreadGetSysID(&m_Thread));
}
#elif defined MACINTOSH
MacThreadID CNCSThread::GetSysID(void) {
	return(NCSThreadGetSysID(&m_Thread));
}
#elif defined POSIX
pthread_t *CNCSThread::GetSysID(void) {
	return(NCSThreadGetSysID(&m_Thread));
}
#else 
#error CNCSThread::GetSysID
#endif // WIN32

bool CNCSThread::Run(void)
{
	return(m_bRun);
}

bool CNCSThread::Stop(bool bWait)
{
	if(m_bRun) {
		m_bRun = false;

		while(bWait && IsRunning()) {
			Resume();
			NCSSleep(1);	
		}
		return(!IsRunning());
	} else {
		return(false);
	}
}
