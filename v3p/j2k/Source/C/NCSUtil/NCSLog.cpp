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
** FILE:     $Archive: /NCS/Source/C/NCSServerUtil/NCSLog.cpp $
** CREATED:  28/08/2001 4:56:40 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSLog class implementation for server
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

//#import "C:\WinNT\System32\inetsrv\logscrpt.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids

#include "NCSUtil.h"

#if defined(WIN32) && !defined(_WIN32_WCE)
#include <time.h>
#include <aclapi.h>
#endif
#include "NCSLog.h"

#ifndef NCSPREFS_H
#include "NCSPrefs.h"
#endif // NCSPREFS_H

#ifdef POSIX
#define _snprintf snprintf
#define _vsnprintf vsnprintf
#endif

extern "C" void NCSLogSetServer(BOOLEAN bValue);

// Initialise the static class members
CNCSMutex				CNCSLog::sm_Mutex;
#ifdef POSIX
CNCSLog::NCSLogLevel	CNCSLog::sm_eLogLevel = CNCSLog::LOG_LEVEL2;
#else
CNCSLog::NCSLogLevel	CNCSLog::sm_eLogLevel = CNCSLog::LOG_LEVEL0;
#endif
char					CNCSLog::sm_szLogFile[MAX_PATH] = { '\0' };
//int						CNCSLog::sm_nRefCount = 0;
CNCSLog::CNCSUpdateLogConfigThread		*CNCSLog::pUpdateLogConfigThread = NULL;


void NCSLogInit()
{
	CNCSLog::pUpdateLogConfigThread = new CNCSLog::CNCSUpdateLogConfigThread();
}

void NCSLogFini()
{
	if( CNCSLog::pUpdateLogConfigThread ) delete CNCSLog::pUpdateLogConfigThread;
	CNCSLog::pUpdateLogConfigThread = NULL;
}



//
// Constructor
//
CNCSLog::CNCSLog()
{
//	sm_Mutex.Lock();
//	if( sm_nRefCount == 0 ) {

	if(pUpdateLogConfigThread && !pUpdateLogConfigThread->IsRunning()) {
		pUpdateLogConfigThread->Spawn((void*)&pUpdateLogConfigThread, false);
	}
	UpdateLogConfig();

	NCSLogSetServer(TRUE);

/*	// Spawn worker thread
	Spawn(this, false);
	}
	sm_nRefCount++;
	sm_Mutex.UnLock();*/
}

CNCSLog::~CNCSLog() {
/*	sm_Mutex.Lock();
	sm_nRefCount--;
	if( sm_nRefCount == 0 ) {
		Stop();
	}
	sm_Mutex.UnLock();*/
}

//
// Write a log entry
//
void CNCSLog::Log(CNCSLog::NCSLogLevel eLevel, char *pFormat, ...)
{
    if(GetLogLevel() >= eLevel
#ifdef DEBUG
			|| eLevel == CNCSLog::LOG_DEBUG
#endif
			) {
		char buf[4096];
		va_list va;
		va_start(va, pFormat);

		_snprintf(buf, sizeof(buf), "%d : ", (int)eLevel);
		int nLen = (int)strlen(buf);

#ifdef POSIX
		// Under WIN32 %I64 is used to print a 64bit int but the posix
		//  conversion is %ll.  So we must change all occurances of %I64 to %ll.
		if( pFormat && (strlen( pFormat ) > 4) ) {
			int nLength = strlen( pFormat );
			char *pFormatNew = new char[nLength+1];
			int i=0;
			int j=0;
			pFormatNew[0] = pFormat[0];
			pFormatNew[1] = pFormat[1];
			pFormatNew[2] = pFormat[2];			
			for( i=3, j=3; i < nLength; i++, j++ ) {
				if( pFormat[i-3] == '%' && pFormat[i-2] == 'I' && pFormat[i-1] == '6' && pFormat[i] == '4' ) {
					// we have found an occurance of "%I64"
					pFormatNew[j-2] = pFormatNew[j-1] = 'l';
					pFormatNew[j] = pFormat[++i];
				} else {
					pFormatNew[j] = pFormat[i];
				}
			}
			pFormatNew[j] = '\0';
			_vsnprintf(buf + nLen, sizeof(buf) - nLen, pFormatNew, va);

			delete [] pFormatNew;
		} else
#endif
        {
			_vsnprintf(buf + nLen, sizeof(buf) - nLen, pFormat, va);
		}


		Log(buf);

		va_end(va);
	}
}

//
// Write a log entry
//
void CNCSLog::Log(char *pFile, int nLine, CNCSLog::NCSLogLevel eLevel, char *pFormat, ...)
{
    if(GetLogLevel() >= eLevel
#ifdef DEBUG
			|| eLevel == CNCSLog::LOG_DEBUG
#endif
			) {
		char buf[4096];
		va_list va;
		va_start(va, pFormat);
		char *p = pFile + strlen(pFile);
		while(p > pFile && *(p - 1) != '\\') {
			p--;
		}
		_snprintf(buf, sizeof(buf), "%d : File %s, Line %ld, ", (int)eLevel, p, nLine);
		int nLen = (int)strlen(buf);

#ifdef POSIX
		// Under WIN32 %I64 is used to print a 64bit int but the posix
		//  conversion is %ll.  So we must change all occurances of %I64 to %ll.
		if( pFormat && (strlen( pFormat ) > 4) ) {
			int nLength = strlen( pFormat );
			char *pFormatNew = new char[nLength+1];
			int i=0;
			int j=0;
			pFormatNew[0] = pFormat[0];
			pFormatNew[1] = pFormat[1];
			pFormatNew[2] = pFormat[2];
			for( i=3, j=3; i < nLength; i++, j++ ) {
				if( pFormat[i-3] == '%' && pFormat[i-2] == 'I' && pFormat[i-1] == '6' && pFormat[i] == '4' ) {
					// we have found an occurance of "%I64"
					pFormatNew[j-2] = pFormatNew[j-1] = 'l';
					pFormatNew[j] = pFormat[++i];
				} else {
					pFormatNew[j] = pFormat[i];
				}
			}
			pFormatNew[j] = '\0';
			_vsnprintf(buf + nLen, sizeof(buf) - nLen, pFormatNew, va);

			delete [] pFormatNew;
		} else
#endif
        {
			_vsnprintf(buf + nLen, sizeof(buf) - nLen, pFormat, va);
		}
		
		Log(buf);

		va_end(va);
	}
}

//
// Get the current log level
//
CNCSLog::NCSLogLevel CNCSLog::GetLogLevel(void)
{
	return(sm_eLogLevel);
}

void CNCSLog::Log(char *pBuf)
{  
	char szBuffer[4096+512] = { '\0' };

#ifdef _WIN32_WCE

	SYSTEMTIME ptm;
	GetLocalTime(&ptm);

	int nLen1 = sprintf(szBuffer, "%02d%02d%02d %02d:%02d:%02d ", 
		ptm.wYear%100, ptm.wMonth+1, ptm.wDay, ptm.wHour, ptm.wMinute, ptm.wSecond);

#else	/* _WIN32_WCE */

	struct tm *ptm;
	time_t long_time;
	time(&long_time);
	ptm = localtime(&long_time);

	int nLen1 = sprintf(szBuffer, "%02d%02d%02d %02d:%02d:%02d ", 
		ptm->tm_year%100, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

#endif	/* _WIN32_WCE */

#ifdef MACINTOSH
	int nLen2 = snprintf(szBuffer + nLen1, sizeof(szBuffer) - nLen1 - 10, pBuf);
#else
	int nLen2 = _snprintf(szBuffer + nLen1, sizeof(szBuffer) - nLen1 - 10, pBuf);
#endif
	if (nLen2 == -1)
	{
		// couldn't fit it all in the buffer
		strcat(szBuffer, "...");		// indicate truncation
		nLen2 = sizeof(szBuffer) - 10 - nLen1 + 3;		// length of _vsnprintf + "..."
	}
	strcat(szBuffer, "\r\n");

#if defined(_DEBUG) || defined(DEBUG)
#ifdef WIN32
	OutputDebugString(OS_STRING(szBuffer));
#else // WIN32
	fprintf(stderr, szBuffer);
	fflush(stderr);
#endif // WIN32
#endif // _DEBUG

	sm_Mutex.Lock();
	if(sm_szLogFile && strlen(sm_szLogFile)) {
		FILE *pFile;

#ifdef WIN32
		HANDLE hToken = NULL;
		HANDLE hThread = GetCurrentThread();

		// Need to run as SYSTEM to write to the log file, make sure we change back to the correct user afterwards though!
#ifndef _WIN32_WCE
		if(OpenThreadToken(hThread,
						   TOKEN_READ|TOKEN_IMPERSONATE,
						   FALSE,
						   &hToken)) {
			RevertToSelf();
						   
			if((pFile = fopen(sm_szLogFile, "a+c")) != NULL) {
				
			//	_chmod(sm_szLogFile, _S_IREAD|_S_IWRITE);
				fprintf(pFile, szBuffer);
				fflush(pFile);
				fclose(pFile);
			}
			SetThreadToken(&hThread, hToken);
		} else
#endif
#endif //WIN32
		{
			if((pFile = fopen(sm_szLogFile, "a+c")) != NULL) {
				fprintf(pFile, szBuffer);
				fflush(pFile);
				fclose(pFile);
			}
		}
	}
	sm_Mutex.UnLock();

}

void CNCSLog::UpdateLogConfig(NCSLogLevel eLevel, char *pLogFile)
{
	// Update static members
	sm_Mutex.Lock();
	sm_eLogLevel = eLevel;
	if(pLogFile) {
		strcpy(sm_szLogFile, pLogFile);
		NCSFree(pLogFile);
	} else {
		char *pTemp = NCSGetTempDirectory();
		if( pTemp ) {
			strcpy(sm_szLogFile, pTemp);
#ifdef WIN32
			strcat(sm_szLogFile, "\\IWS.log");
#elif POSIX
			strcat(sm_szLogFile, "/IWS.log");
#else
#error not done
#endif
			NCSFree(pTemp);
		}
	}
	sm_Mutex.UnLock();
}

void CNCSLog::UpdateLogConfig(void)
{
	NCSLogLevel eLevel = LOG_LEVEL0;
	char *pLogFile = (char*)NULL;

	// Get registry values, or use defaults if not set
	if( NCSPrefSetMachineKeyLock(NCSPREF_DEFAULT_BASE_KEY) == NCS_SUCCESS ) {
		INT32 nLevel = 0;
		if(NCSPrefGetInt(NCS_SERVER_LOG_FILE_LEVEL_PREF, &nLevel) == NCS_SUCCESS) {
			eLevel = (NCSLogLevel)nLevel;
		} else {
			eLevel = LOG_LEVEL0;
		}
		if(NCSPrefGetString(NCS_SERVER_LOG_FILE_NAME_PREF, &pLogFile) != NCS_SUCCESS) {
			pLogFile = (char*)NULL;
		}

		NCSPrefMachineUnLock();
	}

	// Update static members
	sm_Mutex.Lock();
	sm_eLogLevel = eLevel;
	if(pLogFile) {
		strcpy(sm_szLogFile, pLogFile);
		NCSFree(pLogFile);
	} else {
		char *pTemp = NCSGetTempDirectory();
		if( pTemp ) {
			strcpy(sm_szLogFile, pTemp);
#ifdef WIN32
			strcat(sm_szLogFile, "\\IWS.log");
#elif POSIX
			strcat(sm_szLogFile, "/IWS.log");
#else
#error not done
#endif
			NCSFree(pTemp);
		}
	}
	sm_Mutex.UnLock();
}


void CNCSLog::CNCSUpdateLogConfigThread::Work( void *pData ) {
	while(Run()) {
		CNCSLog::UpdateLogConfig();
		NCSSleep(3000);
	}
}
