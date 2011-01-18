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
** FILE:   	NCSLog.h
** CREATED:	Mon May 31 09:19:00 WST 1999
** AUTHOR: 	Doug Mansell
** PURPOSE:	NCS server logging interface
** EDITS:
** [01] 06-06-02 Changed the Registry key names to match IWS 1.7
** [02] 08-12-05 tfl lint fixes
 *******************************************************/


#ifndef NCSLOG_H
#define NCSLOG_H

#ifndef NCSTYPES_H
#include "NCSTypes.h"
#endif // NCSTYPES_H

#ifdef __cplusplus 
extern "C" {
#endif

#define NCS_SERVER_LOG_FILE_NAME_PREF		"Log Filename"		//[01]
#define NCS_SERVER_LOG_FILE_LEVEL_PREF		"Log Level"			//[01]
#define NCS_LOG_FILE_NAME_PREF				"IWS Log Filename"
#define NCS_LOG_FILE_LEVEL_PREF				"IWS Log Level"

typedef enum
{
	LOG_LOW = 0,		// lowest log level
	LOG_MED,			// more informational
	LOG_HIGH,			// and more

	LOG_HIGHER
						// ... with room for even more
} ENCSLogLevel;


void NCSLogGetConfig(char **pLogName, ENCSLogLevel *pLogLevel);
void NCSLog(ENCSLogLevel eLevel, char *szFormat, ...);
void NCSLogSetServer(BOOLEAN bValue);

void NCSLogInit(void);
void NCSLogFini(void);

#ifdef __cplusplus 
}

#ifndef NCSOBJECT_H
#include "NCSObject.h"
#endif // NCSOBJECT_H

#ifndef NCSMUTEX_H
#include "NCSMutex.h"
#endif // NCSMUTEX_H

#ifndef NCSTHREAD_H
#include "NCSThread.h"
#endif // NCSTHREAD_H

/**
 * Logging utility class for server.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.23 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/09 06:11:11 $ 
 */
class NCS_EXPORT CNCSLog: public CNCSObject {
public:
	/**
	 * Enum of the different log levels available.  LOG_DEBUG only appears in the log file for a DEBUG build.
	 */
	typedef enum {
		LOG_LEVEL0	= LOG_LOW,		// Always logged
		LOG_LEVEL1	= LOG_MED,		// Level 1 messages
		LOG_LEVEL2	= LOG_HIGH,		// Level 2 messages
		LOG_LEVEL3	= LOG_HIGHER,	// Level 3 messages

		LOG_DEBUG					// DEBUG build only
	} NCSLogLevel;

	/**
	 * Default Constructor.
	 */
	CNCSLog();
	virtual ~CNCSLog();

	/**
	 * Log a string to the log file.  String is also written out using OutputDebugString() 
	 * (on WIN32) or fprintf(stderr) (other platforms) under a DEBUG build
	 * 
	 * @param		 pBuf			String to log to the log file
	 */
	static void Log(char *pBuf);
	/**
	 * Log a formatted error if the loggin level meets the specified one.
	 * 
	 * @param        eLevel			Log level that must be met before message should be logged.
	 * @param		 pFormat		printf() style format string to log
	 * @param		 ...			Argument list to match pFormat format string
	 */
	static void Log(NCSLogLevel eLevel, char *pFormat, ...);
	/**
	 * Lof a formatted error and file/line entry to the log file if the logging level meets the specified one.
	 * 
	 * @param        pFile			File name where Log() is called (usually __FILE__ is used)
	 * @param		 nLine			Line number in file where Log() is called (usually __LINE__ is used)
	 * @param        eLevel			Log level that must be met before message should be logged.
	 * @param		 pFormat		printf() style format string to log
	 * @param		 ...			Argument list to match pFormat format string
	 */
	static void Log(char *pFile, int nLine, NCSLogLevel eLevel, char *pFormat, ...);
	/**
	 * Get the current log level.
	 * 
	 * @return       NCSLogLevel	Current log level as set in the registry
	 */
	static NCSLogLevel GetLogLevel(void);
	/**
	 * Update the current log level and log file name from the registry (WIN32).
	 * 
	 */
	static void UpdateLogConfig(void);
	static void UpdateLogConfig(NCSLogLevel eLogLevel, char *pLogFile);

private:
	/**
	 * Mutex for accessing the log level and log file name, which can change at runtime.
	 * 
	 * @see				#sm_eLogLevel
	 * @see				#sm_szLogFile
	 */
	static CNCSMutex	sm_Mutex;
	/**
	 * Current log level.
	 */
	static NCSLogLevel	sm_eLogLevel;
	/**
	 * Current log file name.
	 */
	static char			sm_szLogFile[MAX_PATH];
	//static int			sm_nRefCount;
	
public:
	class NCS_EXPORT CNCSUpdateLogConfigThread: public CNCSThread {
	public:
		CNCSUpdateLogConfigThread() {
#ifndef WIN32
			// This will break on Win32 because this happense from DllMain(), the thread doesn't actually create until later 
			// and it all goes pear shaped if the dll gets unloaded straight away because the Stop() hangs in the destructor.
			Spawn(this, false);
#endif
		}

		virtual ~CNCSUpdateLogConfigThread() {
			Stop();
		}

		void Work( void *pData );
	};

	static CNCSUpdateLogConfigThread *pUpdateLogConfigThread;

	/**
	 * CNCSThread Work() function, used to update config from registry.
	 * 
	 * @param        pParameter	(UNUSED)
	 */
	//void				Work(void *pParameter);
};

#define LOGERROR0( t, e )	CNCSLog::Log(__FILE__, __LINE__, CNCSLog::LOG_LEVEL0, "Error: %s %s(%ld)", t, NCSGetErrorText(e), e)
#define LOGERROR1( t, e )	CNCSLog::Log(__FILE__, __LINE__, CNCSLog::LOG_LEVEL1, "Error: %s %s(%ld)", t, NCSGetErrorText(e), e)

#endif //__cplusplus

#endif //NCSLOG_H

