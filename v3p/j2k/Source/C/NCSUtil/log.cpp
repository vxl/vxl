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
** FILE:   	log.cpp
** CREATED:	Mon May 31 09:19:00 WST 1999
** AUTHOR: 	Doug Mansell
** PURPOSE:	NCS server logging implementation
** EDITS:
** [01] sjc 24Feb00 Only open log file if something to log out
** [02]  ny 31Oct00 Merge WinCE/PALM SDK changes
** [03]  ny 08Feb01 Added Version Number string to log files
** [04] rar 14Mar02 Minor changes required to support logging in NCSMalloc, NCSRealloc.
** [05] rar 06Jun02 Changed so that when logging in server the log file is not kept open.
 *******************************************************/

#include "NCSBuildNumber.h"	//[03]
#include "NCSUtil.h"

#ifdef PALM
#include <unix_stdarg.h>
#else
#include <stdarg.h>

#ifndef _WIN32_WCE
#include <time.h>

#if (defined(WIN32) && !defined(_WIN32_WCE))
#include <crtdbg.h>
#endif
#endif
#endif

#if defined(MACINTOSH)||defined(SOLARIS)
#include <string.h>
#endif

#include "NCSLog.h"

class CNCSLogInternal
{
private:
	ENCSLogLevel m_eLevel;
  
	FILE *m_pFile;

  
public:
	CNCSLogInternal();
	~CNCSLogInternal();
	BOOLEAN Open(char *szLogFile, ENCSLogLevel eLevel = LOG_LOW);
	ENCSLogLevel &Level() { return m_eLevel; };
	void Log(ENCSLogLevel eLevel, char *szFormat, ... );				// wrapper to a call to vLog
	void vLog(ENCSLogLevel eLevel, char *szFormat, va_list args);
	void Close();

	static BOOLEAN m_bDontLog; //[02]
	BOOLEAN DontLogCheck(); //[02]
};

static CNCSLogInternal Log;
static BOOLEAN bIsServerLog = FALSE;
BOOLEAN CNCSLogInternal::m_bDontLog = FALSE; //[04]

extern "C" void NCSLog(ENCSLogLevel eLevel, char *szFormat, ...)
{
	va_list args;
	va_start(args, szFormat);

	Log.vLog(eLevel, szFormat, args);

	va_end(args);
}

BOOLEAN CNCSLogInternal::DontLogCheck() //[04]
{
#ifdef _WIN32_WCE
	return TRUE;
#else
	if( NCSUtilInitialised() ) return m_bDontLog;
	else return TRUE;
#endif //_WIN32_WCE
}

/*extern "C" void NCSLogGetConfig(char **pLogName, ENCSLogLevel *pLogLevel)
{
#ifndef MACINTOSH
	CNCSLogInternal::m_bDontLog = TRUE; //[04]

	if (bIsServerLog) {
		NCSPrefGetString(NCS_SERVER_LOG_FILE_NAME_PREF, pLogName);
		if (NCSPrefGetInt(NCS_SERVER_LOG_FILE_LEVEL_PREF, (INT32 *) pLogLevel) != NCS_SUCCESS) {
			*pLogLevel = (ENCSLogLevel) 0;
		}
	}
	else {
		NCSPrefGetUserString(NCS_LOG_FILE_NAME_PREF, pLogName);
		if (NCSPrefGetUserInt(NCS_LOG_FILE_LEVEL_PREF, (INT32 *) pLogLevel) != NCS_SUCCESS) {
			*pLogLevel = (ENCSLogLevel) 0;
		}	
	}

	CNCSLogInternal::m_bDontLog = FALSE; //[04]
#endif
}*/

extern "C" void NCSLogSetServer(BOOLEAN bValue)
{
	bIsServerLog = bValue;
}

CNCSLogInternal::CNCSLogInternal()
{
	m_pFile = NULL;
}


CNCSLogInternal::~CNCSLogInternal()
{
	Close();
}


BOOLEAN CNCSLogInternal::Open(char *szFilename, ENCSLogLevel eLevel /* = LOG_LOW */)
{
#ifndef NO_STDIO
	if (szFilename && strlen(szFilename))
		m_pFile = fopen(szFilename, "a+c");		// c == commit to disk (not just OS) on fflush
	m_eLevel = eLevel;
	if (m_pFile!=NULL && !bIsServerLog) {					//[03]
		fprintf(m_pFile,					//[03]
				"Version Number : %s\n",	//[03]
				NCS_VERSION_STRING);		//[03]
	}										//[03]
	return (m_pFile != NULL);
#else	/* NO_STDIO */
	return(FALSE);
#endif	/* NO_STDIO */
}


void CNCSLogInternal::Log(ENCSLogLevel eLevel, char *szFormat, ... )
{
	va_list args;
	va_start(args, szFormat);

	vLog(eLevel, szFormat, args);

	va_end(args);
}


void CNCSLogInternal::vLog(ENCSLogLevel eLevel, char *szFormat, va_list args)
{
	char *pLogName = (char*)NULL;
	ENCSLogLevel eConfigLevel = LOG_LOW;

	if( DontLogCheck() ) return; //[04]

	if(!m_pFile) {

		//NCSLogGetConfig(&pLogName, &eConfigLevel);
		m_eLevel = eConfigLevel; //[05]
		/* [05] if(eLevel <= eConfigLevel) {
			Open(pLogName, eConfigLevel);
		}
		NCSFree(pLogName);*/
	}
	if (eLevel > m_eLevel) {
		NCSFree(pLogName);
		return;
	}


	// construct complete line in a buffer so we can write it atomically
	char szBuffer[1024];
	memset(szBuffer, 0, sizeof(szBuffer));

#ifdef _WIN32_WCE

	SYSTEMTIME ptm;
	GetLocalTime(&ptm);

	int nLen1 = sprintf(szBuffer, "%02d%02d%02d %02d:%02d:%02d %d : ", 
		ptm.wYear%100, ptm.wMonth+1, ptm.wDay, ptm.wHour, ptm.wMinute, ptm.wSecond, eLevel);

#else	/* _WIN32_WCE */

	struct tm *ptm;
	time_t long_time;
	time(&long_time);
	ptm = localtime(&long_time);

	int nLen1 = sprintf(szBuffer, "%02d%02d%02d %02d:%02d:%02d %d : ", 
		ptm->tm_year%100, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, eLevel);

#endif	/* _WIN32_WCE */

	int nLen2 = vsnprintf(szBuffer + nLen1, sizeof(szBuffer) - nLen1 - 10, szFormat, args);

	if (nLen2 == -1)
	{
		// couldn't fit it all in the buffer
		strcat(szBuffer, "...");		// indicate truncation
		nLen2 = sizeof(szBuffer) - 10 - nLen1 + 3;		// length of _vsnprintf + "..."
	}
	strcat(szBuffer, "\r\n");
	va_end(args);

#ifdef _DEBUG
	_RPT0(_CRT_WARN, szBuffer);			// send to crt debug too
#endif

	if(!m_pFile) {
		if(eLevel <= eConfigLevel) {
			Open(pLogName, eConfigLevel);
		}
		NCSFree(pLogName);
	}

	if (m_pFile == NULL)				// are we actually logging to file?
		return;

	fwrite(szBuffer, nLen1 + nLen2 + 2, 1, m_pFile);
	fflush(m_pFile);

	if(bIsServerLog)
		Close();			//[05]
}


void CNCSLogInternal::Close()
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}

