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
** FILE:   	NCSUtil.h
** CREATED:	Thu Feb 25 09:19:00 WST 1999
** AUTHOR: 	Simon Cope
** PURPOSE:	NCS Utility library header.
** EDITS:
** [01] ny  23May00 Modified NCSGetUserAndDomainName to be cross platform
** [02] ny  30Oct00 Merge CE/Palm modification in preparation for Macintosh port
** [03] ny  23Nov00 Mac port changes (Added NCSstrcasecmp)
** [04] sjc 28Aug01 Added NCSIsService() to detect if running as a service under WinNT/2K/XP Server.
** [05] sjc 11Mar02 Added NCSIsWebService() to detect if running as a web service under WinNT/2K/XP Server.
** [06] ajd 01Jul02 Added NCSNanVal() to generate NaNs
** [07] sjc 12Feb03 Added NCSLog2() to calculate log2() function
** [08] tfl 06Sep05 Added NCSErrorFiniThread() proto to quell compiler warning
 *******************************************************/

#ifndef NCSUTIL_H
#define NCSUTIL_H

// FIXME					/**[02]**/
#ifdef __PALM__				/**[02]**/
#ifndef EMULATION_LEVEL		/**[02]**/
#define EMULATION_LEVEL 0	/**[02]**/
#endif						/**[02]**/
#endif						/**[02]**/

#ifndef __PALM__			/**[02]**/
#include <math.h>
#endif /**[02]**/

#ifndef NCSDEFS_H
#include "NCSDefs.h"
#endif
#ifndef NCSTYPES_H
#include "NCSTypes.h"
#endif
#ifndef NCSDYNAMICLIB_H
#include "NCSDynamicLib.h"
#endif
#ifndef NCSERRORS_H
#include "NCSErrors.h"
#endif
#ifndef NCSMALLOC_H
#include "NCSMalloc.h"
#endif
#ifndef NCSARRAY_H
#include "NCSArray.h"
#endif
#ifndef NCSTIMESTAMP_H
#include "NCSTimeStamp.h"
#endif
#ifndef NCSMUTEX_H
#include "NCSMutex.h"
#endif
#ifndef NCSMEMPOOL_H
#include "NCSMemPool.h"
#endif
#ifndef NCSUTIL_H
#include "NCSUtil.h"
#endif
#ifndef NCSTHREAD_H
#include "NCSThread.h"
#endif
#ifndef NCSQUEUE_H
#include "NCSQueue.h"
#endif
#ifndef NCSPACKETS_H
#include "NCSPackets.h"
#endif
#ifndef NCSTIMER_H
#include "NCSTimer.h"
#endif
#ifndef NCSPREFS_H
#include "NCSPrefs.h"
#endif
#ifndef NCSFILEIO_H
#include "NCSFileIO.h"
#endif
#ifndef NCSMISC_H
#include "NCSMisc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

INT32 NCSUtilInitialised();
void NCSUtilInit(void);
void NCSUtilFini(void);
void NCSErrorFiniThread(void); /**[08]**/
void NCSSleep(NCSTimeStampMs tsSleepTime);
char *NCSStrDup(char *pString);
NCSTChar *NCSStrDupT(NCSTChar *pString);
NCSError NCSFormatCoordStringsEN(double dEasting, double dNorthing, char **ppEastString, char **ppNorthString);
NCSError NCSFormatCoordStringsLL(double dEasting, double dNorthing, char **ppEastString, char **ppNorthString);
UINT32   NCSGetNumCPUs(void);
UINT32 NCSGetNumPhysicalCPUs(void);
#ifdef WIN32
BOOLEAN NCSGetUserAndDomainName(char *UserName, LPDWORD cchUserName, char* DomainName, LPDWORD cchDomainName);
UINT32 NCSGetWebServerID(const char *szPath, DWORD nPathLen);
#endif
BOOLEAN NCSIsNaN(IEEE8 dDoubleValue);
BOOLEAN NCSIsService(void);		/**[04]**/

BOOLEAN NCSIsIWS(void);
void NCSSetIsIWS( BOOLEAN bValue );

double NCSNanVal(void);	/**[06]**/
BOOLEAN NCSDoubleIsNan(double v);

#ifdef WIN32
#ifdef _WIN32_WCE

#if (_WIN32_WCE <= 211)
#define strnicmp(a, b, c)	_memicmp(a, b, c)
#define stricmp(a, b)		_memicmp(a, b, MIN(strlen(a), strlen(b)))
#else
#define strnicmp(a, b, c)	_strnicmp(a, b, c)
#define stricmp(a, b)		_stricmp(a, b)
#endif

#endif /* _WIN32_CE */

#define vsnprintf			_vsnprintf
#define snprintf			_snprintf

#define NCSMessageBox(Window, Text, Caption, Type) ::MessageBox(Window, Text, Caption, Type)
#define NCSMessageBoxA(Window, Text, Caption, Type) ::MessageBoxA(Window, Text, Caption, Type)

#elif defined __PALM__

#define stricmp(a, b)		StrCaselessCompare(a, b)
#define strnicmp(a, b, c)	StrNCaselessCompare(a, b, c)
#define strcmp(a, b)		StrCompare(a, b)
#define strncmp(a, b)		StrNCompare(a, b)
#define strcpy(a, b) 		StrCopy(a, (const char*)b)
#define strncpy(a, b, c)	StrNCopy(a, (const char*)b, c)
#define strlen(a)			StrLen(a)
#define calloc(a, b) 		NCSMalloc(a * b, TRUE)
#define malloc(a)			NCSMalloc(a, FALSE)
#define realloc(a, b)		NCSRealloc((void*)a, b, TRUE)
#define free(a)				NCSFree(a)
#define memcpy(a, b, c)		MemMove(a, b, c)
#define memset(a, b, c)		MemSet(a, c, b)
#define isupper(a)			((((a) >= 'A') && ((a) <= 'Z')) ? TRUE : FALSE)
#define tolower(a)			((a) -= ('A' - 'a'))
int sscanf(const char *string, const char *format, ...);
__inline double atof(char *s) 
{
	double d;
	sscanf(s, "%f", &d);
	return(d);
}


#elif defined MACINTOSH

#if TARGET_API_MAC_CARBON
#	define _stricmp stricmp
#	define _strnicmp strnicmp
#	define _itoa itoa
#else
#	define strncascmp 		_stricmp
#	define NCSstrcasecmp 	_stricmp
#	define strnicmp		_strnicmp
#	define stricmp			_stricmp
#endif //TARGET_API_MAC_CARBON

#elif defined POSIX

#include <string.h>
#define stricmp(a, b) 		strcasecmp(a, b)
#define _stricmp(a, b)		strcasecmp(a, b)
#define strnicmp(a, b, c)	strncasecmp(a, b, c)
#define _snprintf snprintf
#define _vsnprintf vsnprintf

#else /* __PALM__ */

#define stricmp(a, b) 		strcasecmp(a, b)
#define strnicmp(a, b, c)	strncasecmp(a, b, c)

#endif /* WIN32 */

#ifdef __cplusplus
}
#endif

#endif /* NCSUTIL_H */
