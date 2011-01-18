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
** FILE:   	NCSUtil\NCSMutex.cpp
** CREATED:	Fri May 12 2001
** AUTHOR: 	Simon Cope
** PURPOSE:	NCS Mutex class
** EDITS:
*******************************************************/

#include "NCSUtil.h"
#include "NCSMutex.h"

CNCSMutex::CNCSMutex() 
{ 
	NCSMutexInit(&m_Mutex); 
}

//CNCSMutex::CNCSMutex(const CNCSMutex &mMutex)
//{ 
//	NCSMutexInit(&m_Mutex); 
//}

CNCSMutex::~CNCSMutex() 
{ 
	NCSMutexFini(&m_Mutex); 
}

void CNCSMutex::Lock(void) 
{ 
	NCSMutexBegin(&m_Mutex); 
}

bool CNCSMutex::TryLock(void) 
{ 
	return(NCSMutexTryBegin(&m_Mutex) ? true : false); 
}

void CNCSMutex::UnLock(void) 
{ 
	NCSMutexEnd(&m_Mutex); 
}

#ifdef POSIX
pthread_mutex_t *CNCSMutex::GetNative()
{
	return NCSMutexGetNative(&m_Mutex);
}
#endif
