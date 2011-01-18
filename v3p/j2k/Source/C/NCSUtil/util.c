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
 ** FILE:   	NCSUtil/util.c
 ** CREATED:	Tue Mar 2 09:19:00 WST 1999
 ** AUTHOR: 	Simon Cope
 ** PURPOSE:	NCS utility lib main
 ** EDITS:
 ** [01] ny 17May00	Added routine NCSGetUserAndDomainName()
 **					to determine current username and domain name
 ** [02] ny 23May00	Added cross platform code for
 **					NCSGetUserAndDomainName
 ** [03] ny 27Jun00	Added code for NCSGetWebServerID
 ** [04] ny 30Jun00	Clean up NCSGetUserAndDomainName prototype
 ** [05] ny 31Oct00	Merge WinCE/PALM SDK changes
 ** [06] ny 03Nov00	Mac port changes
 ** [07] ny 23Nov00  Mac port changes (Added NCSstrcasecmp)
 ** [08] sjc 05Apr01 Changed to QPC so we don't need to link with WINMM.dll.
 ** [09] sjc 28Aug01 Added NCSIsService() to detect if running as a service under WinNT/2K/XP Server.
 ** [10] ajd 01Jul02 Added NCSNanVal() to generate NaNs
 ** [11] tfl 01Apr05 Removed Purify references and utility functions
 *******************************************************/

#include "NCSUtil.h"
#include "NCSLog.h"
#include <stdio.h>
#include <float.h>
#include <string.h> /**[03]**/
#include <ctype.h>
#include <math.h>

#ifdef WIN32
#include <Oleauto.h>
#endif //WIN32

#ifdef SOLARIS
#include <errno.h>
#include <ieeefp.h>
#endif // SOLARIS

#ifdef LINUX
#include <sys/time.h>
#endif

#ifdef MACINTOSH
#	ifdef MAC_PREEMPTIVE
#		include <Multiprocessing.h>
#		include <MacTypes.h>
#		include <DriverServices.h>
#		include <Types.h>
#		include <stdio.h>
#		include <stdlib.h>
//#		include <sioux.h>
#		include <math64.h> 
#	endif //MAC_PREEMPTIVE
#include <extras.h>
#endif //MACINTOSH

static INT32 nInitialised = FALSE;

INT32 NCSUtilInitialised() {
	return nInitialised;
}

void NCSUtilInit(void)
{
	if(nInitialised == 0) {
		NCSMallocInit();
		NCSThreadInit();
		NCSErrorInit();
		NCSPrefInit();
		NCSLogInit();

		NCSGlobalLockInit();
	}
	nInitialised++;
}

void NCSUtilFini(void)
{
	nInitialised--;

	if(nInitialised == 0) {
		NCSGlobalLockFini();

		NCSLogFini();
		NCSPrefFini();
		NCSErrorFini();
		NCSThreadFini();
		NCSMallocFini();
	}
}

void NCSSleep(NCSTimeStampMs tsSleepTime)
{
#ifdef WIN32
	Sleep((DWORD)tsSleepTime);

#elif defined PALM

	SysTaskDelay((Int32)((SysTicksPerSecond() * tsSleepTime) / 1000));

#elif defined MACINTOSH
#ifdef MAC_PREEMPTIVE
	Duration sleepDuration = (Duration)tsSleepTime * durationMillisecond;
	AbsoluteTime absTNow = UpTime();
	AbsoluteTime absTEnd = AddDurationToAbsolute( sleepDuration, absTNow );

	OSStatus result = MPDelayUntil ( &absTEnd /*AbsoluteTime * expirationTime*/ );
#else
	NCSTimeStampMs tNow = NCSGetTimeStampMs();
	NCSTimeStampMs tEnd = tNow + tsSleepTime;
	/*ThreadID currentThreadID;
	  OSErr err = GetCurrentThread ( &currentThreadID );
	  if( currentThreadID == 100 ) {
	  EventRecord theEvent;
	  Boolean result;
	  RgnHandle mouseRgn=NULL;
	  while( NCSGetTimeStampMs() < tEnd ) {
	  NCSThreadYield();
	  result = WaitNextEvent ( everyEvent, &theEvent, (UInt32)1, mouseRgn );
	  NCSThreadYield();
	  }
	  } else {*/
	NCSThreadYield();
	while( NCSGetTimeStampMs() < tEnd ) {
		NCSThreadYield();
	}
	//}
#endif //MAC_PREEMPTIVE
#elif defined POSIX

	NCSTimeStampMs tNow = NCSGetTimeStampMs();
	NCSTimeStampMs tEnd = tNow + tsSleepTime;
	struct timespec tv = { 0, 0 };

	while(1) {
		NCSThreadYield();
		if((tNow = NCSGetTimeStampMs()) >= tEnd) {
			break;
		}
		tv.tv_nsec = MIN(tEnd - tNow, 100) * 1000 * 1000;
		nanosleep(&tv, (struct timespec*)NULL);
	}

#else	/* WIN32 */
ERROR: NCSSleep() needs coding in NCSUtil/util.c
#endif	/* WIN32 */
}

void NCSFormatSizeText(INT64 nSizeBytes, char *buf)
{
	if(nSizeBytes < 1024) {
#ifdef WIN32
		sprintf(buf, "%I64d bytes", nSizeBytes);
#elif defined SOLARIS
		sprintf(buf, "%lld bytes", nSizeBytes);
#elif defined MACINTOSH
		sprintf(buf, "%I64d bytes", nSizeBytes);
#elif defined HPUX
		sprintf(buf, "%lld bytes", nSizeBytes);
#elif defined LINUX
		sprintf(buf, "%lld bytes", nSizeBytes);
#elif defined MACOSX
		sprintf(buf, "%lld bytes", nSizeBytes);
#else
#error whats the 64bit format specifier?
#endif // WIN32
	} else if(nSizeBytes < 1024 * 1024) {
		sprintf(buf, "%.1lf KB", (double)nSizeBytes / 1024.0);
	} else if(nSizeBytes < 1024 * 1024 * 1024) {
		nSizeBytes /= 1024;
		sprintf(buf, "%.1lf MB", (double)nSizeBytes / 1024.0);
	} else if(nSizeBytes < (INT64)1024 * 1024 * 1024 * 1024) {
		nSizeBytes /= (1024 * 1024);
		sprintf(buf, "%.1lf GB", (double)nSizeBytes / 1024.0);
	} else {
		nSizeBytes /= (1024 * 1024 * 1024);
		sprintf(buf, "%.1lf TB", (double)nSizeBytes / 1024.0);
	}
}

NCSTimeStampMs NCSGetTimeStampMs(void)
{
#ifdef WIN32
	static NCSTimeStampMs tsLast = 0;
	static NCSTimeStampMs tsAdd = 0;
	NCSTimeStampMs tsLocalLast = tsLast;
	NCSTimeStampMs tsLocalAdd = tsAdd;
	NCSTimeStampMs tsNow;

	tsNow = (NCSTimeStampMs)GetTickCount();

	if(tsNow < tsLocalLast) {		// wrapped
		tsLocalAdd += (NCSTimeStampMs)0x100000000;		
		tsAdd = tsLocalAdd;
	}
	tsLast = tsNow;
	return(tsNow + tsLocalAdd);

#elif defined PALM

	return((1000 * TimGetTicks()) / SysTicksPerSecond());

#elif defined MACINTOSH
#if TARGET_API_MAC_CARBON
	return ((INT64)(1000 * (INT64)TickCount()) / (INT64)60);
#else
	return ((INT64)(1000 * (INT64)TickCount()) / CLOCKS_PER_SEC);
#endif //TARGET_API_MAC_CARBON
#elif defined POSIX

	struct timeval t;
	gettimeofday(&t, NULL);
	//printf("TS: %lld\n", ((UINT64)t.tv_sec * 1000000 + t.tv_usec) / 1000);
	return(((UINT64)t.tv_sec * 1000000 + t.tv_usec) / 1000);

#else
ERROR: code NCSGetTimeStampMs() in NCSUtil/util.c
#endif	/* WIN32 */

}

NCSTimeStampUs NCSGetTimeStampUs(void)
{
#ifdef WIN32
	static NCSTimeStampMs tsLast = 0;
	static NCSTimeStampMs tsAdd = 0;
	NCSTimeStampMs tsLocalLast = tsLast;
	NCSTimeStampMs tsLocalAdd = tsAdd;
	NCSTimeStampMs tsNow;
	LARGE_INTEGER iFrequency;

	if(QueryPerformanceFrequency(&iFrequency)) {
		LARGE_INTEGER iCount;

		if(QueryPerformanceCounter(&iCount)) {
			tsNow = (iCount.QuadPart * 1000000) / iFrequency.QuadPart;
		} else {
			tsNow = (NCSTimeStampMs)GetTickCount() * 1000;
		}
	} else {
		tsNow = (NCSTimeStampMs)GetTickCount() * 1000;
	}
	if(tsNow < tsLocalLast) {		// wrapped
		tsLocalAdd += (NCSTimeStampMs)0x100000000;		
		tsAdd = tsLocalAdd;
	}
	tsLast = tsNow;
	return(tsNow + tsLocalAdd);
#else
	return(NCSGetTimeStampMs() * 1000);
#endif
}

NCSPlatform NCSGetPlatform(void)
{
#ifdef WIN32
	OSVERSIONINFO osv;

	osv.dwOSVersionInfoSize = sizeof(osv);

	if(GetVersionEx(&osv)) {
		if(osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
			return(NCS_WINDOWS_9X);
		}
		else if (osv.dwPlatformId == VER_PLATFORM_WIN32_NT) {
			return(NCS_WINDOWS_NT);
#if defined(_WIN32_WCE)
		} else if (osv.dwPlatformId == VER_PLATFORM_WIN32_CE) {
			return (NCS_WINDOWS_CE);
#endif
		}
	}
#elif defined MACINTOSH 

	return(NCS_MACINTOSH);

#elif defined PALM

	return(NCS_PALM);

#elif defined SOLARIS

	return(NCS_SOLARIS);

#elif defined LINUX

	return(NCS_LINUX);

#elif defined HPUX

	return(NCS_HPUX);

#elif defined MACOSX

	return(NCS_MACINTOSH_OSX);

#else	/* WIN32 */
#error - NCSGetPlatform()
#endif	/* WIN32 */
	return(NCS_UNKNOWN);
}

char *NCSStrDup(char *pString)
{
	char *cp;

	if (pString == NULL) {
		cp = NCSStrDup("");
	} else {
		cp = (char *)NCSMalloc((unsigned)strlen(pString)+1, 1);
		(void)strcpy(cp, pString);
	}

	return(cp);
}

NCSTChar *NCSStrDupT(NCSTChar *pString)
{
	NCSTChar *cp;

	if (pString == NULL) {
		cp = NCSStrDupT(NCS_T(""));
	} else {
#ifdef NCS_BUILD_UNICODE
		cp = (NCSTChar *)NCSMalloc((unsigned)wcslen(pString)+1, 1);
		(void)wcscpy(cp, pString);
#else
		cp = (NCSTChar *)NCSMalloc((unsigned)strlen(pString)+1, 1);
		(void)strcpy(cp, pString);
#endif
	}

	return(cp);
}

BOOLEAN NCSIsLocalFile(char *string)
{
	if (!string) return(FALSE);
	if (strlen(string)>2) {
		if (string[1] == ':') return(TRUE);								// Mapped drive name
		if ((string[0] == '\\') && (string[1] == '\\')) return(TRUE);	// UNC name
#ifdef MACINTOSH
		if (!_strnicmp(string, "http:", 5)) return(FALSE);				// URL
		if (!_strnicmp(string, "ecwp:", 5)) return(FALSE);				// ECW protocol
#else
		if (!strnicmp(string, "http:", 5)) return(FALSE);				// URL
		if (!strnicmp(string, "ecwp:", 5)) return(FALSE);				// ECW protocol
#endif
		return(FALSE);
	}
	else {
		return(FALSE);
	}
}

BOOLEAN NCSPathExists(char *pPath, BOOLEAN *pIsDirectory)
{
	BOOLEAN bPathExists = FALSE;

#if defined(WIN32)||defined(POSIX)
#ifdef _WIN32_WCE

	BY_HANDLE_FILE_INFORMATION Info;
	HANDLE hFile;

	if(hFile = CreateFile(OS_STRING(pPath), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL)) {
		if(GetFileInformationByHandle(hFile, &Info)) {
			if(Info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				*pIsDirectory = TRUE;
				bPathExists = TRUE;
			} else if(Info.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) {
				*pIsDirectory = FALSE;
				bPathExists = TRUE;	
			}
		}
		CloseHandle(hFile);
	}

#else /* _WIN32_WCE */
	struct stat buffer;

	char *pPathCopy = NCSStrDup(pPath);

	// stat fails on win32 if the dir path ends in a backslash - go figure!
	if (pPathCopy[strlen(pPath) - 1] == '\\') {
		pPathCopy[strlen(pPath) - 1] = '\0';
	}

	if (stat( (const char *)pPathCopy, &buffer) != -1) {

		if (((buffer.st_mode)&S_IFMT) == S_IFDIR) {
			*pIsDirectory = TRUE;
			bPathExists = TRUE;
		} else if (((buffer.st_mode)&S_IFMT) == S_IFREG) {
			*pIsDirectory = FALSE;
			bPathExists = TRUE;
		} else { 
			bPathExists = FALSE;
		}
	}
	else {
		bPathExists = FALSE;
	}

	NCSFree(pPathCopy);
#endif	/* _WIN32_WCE */
#elif defined PALM

	bPathExists = TRUE;

#elif defined MACINTOSH

	bPathExists = TRUE;

#else
#ERROR: define NCSPathExists() for this platform
#endif	/* WIN32||LINUX */

	return bPathExists;
}

// Following code stolen and modified from ER Mapper library
void NCSDegreesToDMSString(char **ppDegreeString, double dDecimalDegrees, UINT8 nPrecision)
{
	INT32 sign = 1;
	INT32 degrees;
	INT32 minutes;
	double seconds;
	double ten_fac;
	INT32 i;
	char TmpBuffer[256];
	char *Buffer = TmpBuffer;

	Buffer[0] = '\0';
	if (dDecimalDegrees < 0.0) {
		sign = -1;
		dDecimalDegrees *= -1;
	}

	degrees = (INT32)dDecimalDegrees;

	dDecimalDegrees = (dDecimalDegrees - (double)degrees) * 60.0;
	minutes = (INT32)dDecimalDegrees;

	dDecimalDegrees = (dDecimalDegrees - (double)minutes) * 60.0;
	seconds = dDecimalDegrees;

	if (sign == -1) {
		*Buffer++ = '-';
	}

	ten_fac = 1.0;
	for (i=0; i<nPrecision; i++) {
		ten_fac *= 10.0;
	}

	seconds = floor(seconds * ten_fac + 0.5) / ten_fac;
	if (seconds >= 60.0) {
		seconds -= 60.0;
		minutes++;
	}
	if (minutes >= 60) {
		minutes -= 60;
		degrees ++;
	}
	sprintf(Buffer, "%d:%d:%.*f", degrees, minutes, nPrecision, seconds);

	/*
	 ** strip trailing zeros up to last one before the decimal point
	 */
	i = (INT32)strlen(Buffer) - 1;
	while (i > 0 && Buffer[i] == '0' && Buffer[i-1] != '.') {
		Buffer[i--] = '\0';
	}

	*ppDegreeString = NCSStrDup(Buffer);
}

/*
 **	For a gien number 125.223466 it will format it to look like 125:55:33N
 */
NCSError NCSFormatCoordStringsLL(double dLatitude, double dLongitude, char **ppLatString, char **ppLongString )
{
	char *coord_ll_NS = "N";
	char *coord_ll_EW = "E";
	char Buffer [100];
	char *pDMSString = NULL;

	if (dLatitude != 0.0) {
		if (dLatitude < 0.0) {
			dLatitude = -dLatitude; 
			coord_ll_NS = "S";
			NCSDegreesToDMSString(&pDMSString, dLatitude, 2);
			sprintf(Buffer, "%s%s", pDMSString, coord_ll_NS );
		}
		else {
			NCSDegreesToDMSString(&pDMSString, dLatitude, 2);
			sprintf(Buffer, "%s%s", pDMSString, coord_ll_NS );
		}
	} else {
		strcpy(Buffer, "0:0:0.0N");
	}

	*ppLatString = NCSStrDup(Buffer);


	if( dLongitude != 0.0 ) {       
		/* Normalise longitude around 180 degrees */
		int nr_revs = 0;
		nr_revs = ((int)dLongitude)/360;
		if (dLongitude < 0.0) {
			dLongitude = dLongitude + (((double)(nr_revs+1)) * 360.0);
			nr_revs = ((int)dLongitude)/360;
		}
		dLongitude = dLongitude - (((double)nr_revs) * 360.0);
		if ((((int)dLongitude)/180)%2 == 0) {
			coord_ll_EW = "E";
		} else {
			dLongitude = 360.0 - dLongitude;
			coord_ll_EW = "W";
		}

		NCSFree(pDMSString);
		NCSDegreesToDMSString(&pDMSString, dLongitude, 2);

		sprintf(Buffer,"%s%s", pDMSString, coord_ll_EW );
	} else {                               
		strcpy(Buffer, "0:0:0.0E");
	}

	*ppLongString = NCSStrDup(Buffer);

	return NCS_SUCCESS;
}

/*
 **	For a gien number 125.223466 it will format it to look like 125:55:33N
 */
NCSError NCSFormatCoordStringsEN(double dEasting, double dNorthing, char **ppEastString, char **ppNorthString )
{
	char    e_str_buf[256];
	char    n_str_buf[256];

	if( dEasting != 0.0 ) {
		sprintf(e_str_buf, "%.2lf%s", dEasting, "E");
	} else {                           
		strcpy(e_str_buf, "0.0E");
	}
	if( dNorthing != 0.0 ) { 
		sprintf(n_str_buf, "%.2lf%s", dNorthing, "N");
	} else {                            
		strcpy(n_str_buf, "0.0N");  
	}
	*ppEastString = NCSStrDup(e_str_buf); 
	*ppNorthString = NCSStrDup(n_str_buf);

	return NCS_SUCCESS;
}

BOOLEAN NCSIsNaN(IEEE8 dDoubleValue)
{
#ifdef WIN32
	return (BOOLEAN)_isnan(dDoubleValue);
#else
	return (BOOLEAN)isnan(dDoubleValue);
#endif
}

BOOLEAN NCSIsService(void)	/**[09]**/
{
#ifdef WIN32
	HKEY hKey;

	if(RegOpenKeyEx(HKEY_CURRENT_USER,
				OS_STRING("Software"),
				0,	
				KEY_QUERY_VALUE,
				&hKey) != ERROR_SUCCESS) {
		return(TRUE);
	}
	RegCloseKey(hKey);
#endif
	return(FALSE);
}

BOOLEAN g_bPrcessIsIWS = FALSE;

BOOLEAN NCSIsIWS(void)
{
	return g_bPrcessIsIWS;
}

void NCSSetIsIWS( BOOLEAN bValue )
{
	g_bPrcessIsIWS = bValue;
}

BOOLEAN NCSDoubleIsNan(double v)
{
#ifdef SOLARIS
	fpclass_t c = fpclass(v);
	if((c == FP_SNAN) || (c == FP_QNAN)) {
		return(TRUE);
	}
#endif

#ifdef HPUX
	int c = fpclassify(v);
	if(c == FP_NAN) {
		return(TRUE);
	}
#endif

#ifdef LINUX
	return (isnan(v));
#endif

#ifdef WIN32
	int c = _fpclass(v);

	if((c == _FPCLASS_SNAN) || (c == _FPCLASS_QNAN)) {
		return(TRUE);
	}
#endif
	return(FALSE);
}

double NCSNanVal(void)	/**[10]**/
{
	union {
		struct {
			unsigned lowbits        :32;
			unsigned highbits       :19;
			unsigned qnanbit        : 1;
			unsigned exponent       :11;
			unsigned sign           : 1;
		} lsbfirst_map;
		struct {
			unsigned sign           : 1;
			unsigned exponent       :11;
			unsigned qnanbit        : 1;
			unsigned highbits       :19;
			unsigned lowbits        :32;
		} msbfirst_map;
		double d;
	} u;

	u.d = 0.0;

#if defined(WIN32)||defined(LINUX)
	u.lsbfirst_map.exponent = 0x7FF;
	u.lsbfirst_map.qnanbit = 1;
#else
	u.msbfirst_map.exponent = 0x7FF;
	u.msbfirst_map.qnanbit = 1;
#endif

	return(u.d);
}

#ifdef NOTDEF

typedef enum NCSModule
{
	NCS_MODULE_UTIL = 0,
	NCS_MODULE_ECW,
	NCS_MODULE_JPEG,
	NCS_MODULE_GDT,
	NCS_MODULE_CNET,
	NCS_MODULE_SNET,
	NCS_MODULE_VIEW,
	NCS_MODULE_ADDONS,
	NCS_MODULE_NPVIEW
} NCSModule;

#include "NCSModule.h"

const char *NCSUtilGetModuleName(NCSModule eModuleName)
{
	const char *pName = NULL;

	switch (eModuleName) {
		case NCS_MODULE_UTIL:
		case NCS_MODULE_ECW:
		case NCS_MODULE_JPEG:
		case NCS_MODULE_GDT:
		case NCS_MODULE_CNET:
		case NCS_MODULE_SNET:
		case NCS_MODULE_VIEW:
		case NCS_MODULE_NPVIEW:
			pName =  NCSModuleName[eModuleName];
		default:
			pName = NULL;
			break;
	}

	return pName;
}

#endif

#if (defined(WIN32) && !defined(_WIN32_WCE))
BOOLEAN NCSGetUserAndDomainName(char *UserName, LPDWORD cchUserName, char* DomainName, LPDWORD cchDomainName) /**[04]**/
{
	HANDLE hToken = NULL;

#define MY_BUFSIZE 512  // highly unlikely to exceed 512 bytes
	UCHAR InfoBuffer[ MY_BUFSIZE ];
	DWORD cbInfoBuffer = MY_BUFSIZE;
	SID_NAME_USE snu;

	BOOLEAN bSuccess;

	if(!OpenThreadToken(
				GetCurrentThread(),
				TOKEN_QUERY,
				TRUE,
				&hToken
			   )) {

		if(GetLastError() == ERROR_NO_TOKEN) {

			// 
			// attempt to open the process token, since no thread token
			// exists
			// 

			if(!OpenProcessToken(
						GetCurrentProcess(),
						TOKEN_QUERY,
						&hToken
					    )) return FALSE;

		} else {

			// 
			// error trying to get thread token
			// 

			return FALSE;
		}
	}

	bSuccess = (BOOLEAN)GetTokenInformation(
			hToken,
			TokenUser,
			InfoBuffer,
			cbInfoBuffer,
			&cbInfoBuffer
			);

	if(!bSuccess) {
		if(GetLastError() == ERROR_INSUFFICIENT_BUFFER) {

			// 
			// alloc buffer and try GetTokenInformation() again
			// 

			CloseHandle(hToken);
			return FALSE;

		} else {

			// 
			// error getting token info
			// 

			CloseHandle(hToken);
			return FALSE;
		}
	}

	CloseHandle(hToken);

	return (BOOLEAN)LookupAccountSid(
			NULL,
			((PTOKEN_USER)InfoBuffer)->User.Sid,
			OS_STRING(UserName),
			cchUserName,
			OS_STRING(DomainName),
			cchDomainName,
			&snu
			);
}

UINT32 NCSGetWebServerID(const char *szPath, DWORD nPathLen)	/**[06]**/
{
	UINT32 result = 0;
	int i;
	int index = nPathLen - 1;
	char szNumber[256];
	/* Locate the first "/" from the right hand side */
	for (i=(nPathLen-1);i>=0;i--) {
		if (szPath[i]=='/') {
			index = i;
			break;
		}
	}
	strcpy(szNumber,&(szPath[index+1]));
	sscanf(szNumber,"%d",&result);
	return result;
}

#elif defined(_WIN32_WCE)
BOOLEAN NCSGetUserAndDomainName(char *UserName, LPDWORD cchUserName, char* DomainName, LPDWORD cchDomainName)
{
	return(FALSE);
}
UINT32 NCSGetWebServerID(const char *szPath, DWORD nPathLen)
{
	return(0);
}
#endif

#ifdef MACINTOSH_OLD
int NCSstrcasecmp(const char *s1, const char *s2) /**[07]**/
{
	char *ls1,*ls2;
	int i1,i2,i,result;

	i1  = strlen(s1);
	i2  = strlen(s2);
	ls1 = (char *)malloc(i1+1);
	ls2 = (char *)malloc(i2+1);

	for (i=0;i<i1;i++) {
		ls1[i] = tolower(s1[i]);
	}
	ls1[i1] = '\0';

	for (i=0;i<i2;i++) {
		ls2[i] = tolower(s2[i]);
	}
	ls2[i2] = '\0';

	result = strcmp(ls1,ls2);
	free(ls1);
	free(ls2);
	return result;
}
#endif

#ifdef MACINTOSH_NOT
#if TARGET_API_MAC_CARBON

char *gcvt(double value, int digits, char *buffer)
{
	sprintf( buffer, "%.*f", digits, value );
	return buffer;
}

char * itoa(int val, char *str, int radix)
{
	sprintf( str, "%d", val );
	return str;
}

int stricmp( const char *cA, const char *cB )
{
	char *cAtmp = NULL;
	char *cBtmp = NULL;
	int i=0;

	if( !cA || !cB ) {
		return -1;
	}
	if( strlen( cA ) != strlen( cB ) ) {
		return -1;
	}
	//Convert both string to upper
	cAtmp = NCSStrDup( (char *)cA );
	cBtmp = NCSStrDup( (char *)cB );
	for( i=0; i < strlen( cAtmp ); i++ ) {
		cAtmp[i] = toupper( (int)cAtmp[i] );
		cBtmp[i] = toupper( (int)cBtmp[i] );
	}

	return strcmp( cAtmp, cBtmp );
}

int strnicmp( const char *cA, const char *cB, unsigned long nSize )
{
	char *cAtmp = NULL;
	char *cBtmp = NULL;
	int i=0;

	if( !cA || !cB ) {
		return -1;
	}
	if( (strlen( cA ) < nSize) || (strlen( cB ) < nSize) ) {
		return -1;
	}
	//Convert both string to upper
	cAtmp = NCSStrDup( (char *)cA );
	cBtmp = NCSStrDup( (char *)cB );
	for( i=0; i < strlen( cAtmp ); i++ ) {
		cAtmp[i] = toupper( (int)cAtmp[i] );
		cBtmp[i] = toupper( (int)cBtmp[i] );
	}

	return strncmp( cAtmp, cBtmp, nSize );
}
#endif //TARGET_API_MAC_CARBON
#endif //MACINTOSH
