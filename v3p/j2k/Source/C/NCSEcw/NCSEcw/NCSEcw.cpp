/********************************************************** 
** Copyright 1998 Earth Resource Mapping Ltd.
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
** FILE:   	ncsecw.c
** CREATED:	1 March 1999
** AUTHOR: 	SNS
** PURPOSE:	Glue functions to access ECW files from the NCS system
** EDITS:
** [01] sns	01-Mar-99 Created file
** [02] sns 02-Apr-99 Not doing Mutex locks during process shutdown
** [03] sns 09-May-99 Added thread logic for IDWT callbacks
** [04] sns 14-May-99 Adding better file comparison logic for caching
** [05] sns 17-May-99 Updated cache counting logic if closing library while file(s) open
** [06] dmm 22-Jun-99 removed unnecessary calls to _CRT_INIT in DllMain
** [07] sjc 22-Jul-99 Only read offsets on local file if asked (otherwise open can take a _long_ time)
** [08] sjc 22-Jul-99 Check flags to make sure cached file is acceptable
** [09] dmm 27-Jul-99 Changed NCSecwOpenFile to return NCSError
** [10] sjc 30-Dec-99 Added global lock on stats seg
** [11] mjs 10-Feb-00 Turned into a COM dll, hence added new STDAPI calls for com compatability
** [12] mjs 11-Feb-00 Added NCSGetMajorVersion, NCSGetMinorVersion
** [13] sjc 30-Mar-00 Modified for SHM stats with support for old Shared DSEG
** [14] mjs 13-Jul-00 Removed NCSecwInit() from DLMain, because it does a load library and
**					  it is not safe to call LoadLibrary from DLLMain, (it hangs on some platforms).
** [15] sjc 20-Jul-00 Removed v1.0 stats sharing code - could hang on some systems
** [16] sjc 11-Sep-00 Modified registration to work as a restricted user under win2k
** [17]  ny 17-Nov-00 Merged WinCE/PALM SDK changes
** [18] sjc 04-Feb-02 Added MAXOPEN
** [19] sjc 19-Feb-02 Added purge & other tunables
** [20] rar 19-Jun-02 Added mutex around NCScbmReadFileBlockLocal() and NCScbmGetFileBlockSizeLocal() to
**						prevent the IWS and ecw_img trying to read from the same file at the same time.
** [21] rar 02-Sep-03 Added option to try ecwp re-connection if connection lost
** [22] sjc 28-Jun-04 Added JP2 options
** [23] tfl 23-May-05 Changed initialisation code to cope with libecwj2 and static lib scenarios
** [24] tfl 15-Sep-05 Added change to cached block pointer update logic due to possibility of self-cycling (change from Bill Binko)
*******************************************************/

#define NCSECWGLOBAL
// need this for COINIT_MULTITHREADED under VS.NET 2003
#define _WIN32_WINNT 0x0500

#include "NCSEcw.h"				/**[17]**/
#if !defined(_WIN32_WCE)
	#include <string.h>
#endif

#if defined(WIN32)&&!defined(NCS_MINDEP_BUILD)					/**[17]**/
#if !defined(NCSECW_STATIC_LIBS) && !defined(LIBECWJ2) && !defined(_WIN32_WCE)
	#include "stdafx.h"				/**[17]**/
	#include <initguid.h>			/**[17]**/
	#include "NCSEcwCom.h"			/**[17]**/
	#include "NCSEcw_i.c"			/**[17]**/
	#include "resource.h"		
	#include "ComNCSRenderer.h"		/**[17]**/
	#include <WinBase.h>			/**[17]**/
#elif !defined(NCSECW_STATIC_LIBS) && !defined(LIBECWJ2) && defined(_WIN32_WCE)
	#include "..\NCSEcwCE\stdafx.h"				
	#include <initguid.h>		
//	#include "..\NCSEcwCE\NCSEcw.h"		
//	#include "..\NCSEcwCE\NCSEcw_i.c"		
	#include "..\NCSEcw\NCSEcwCom.h"		
	#include "..\NCSEcw\NCSEcw_i.c"		
	#include "..\NCSEcwCE\resource.h"		
	//#include "..\NCSEcwCE\ComNCSRenderer.h"	
	#include "..\NCSECW\ComNCSRenderer.h"	
#endif
#elif defined MACINTOSH
	#include <ctype.h>
#elif defined POSIX
	#include <unistd.h>			/**[18]**/
#endif							/**[17]**/

#include "NCSJP2FileView.h"

static UINT32	NCSecwSemiUniqueId(char *szUrlPath);
//static int NCSecwGetFileModifiedTime(NCSFile *pNCSFile, DATETIME *tModifiedTime); /**[17]**/

#if defined (WIN32) && !defined(NCSECW_STATIC_LIBS) && !defined(LIBECWJ2) && !defined(NCS_MINDEP_BUILD)
CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_NCSRenderer, CComNCSRenderer)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

#ifdef _WIN32_WCE
extern "C" BOOL WINAPI DllMain(HANDLE hInstance, DWORD dwReason, LPVOID lpReserved)
#else
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
#endif
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
			_Module.Init(ObjectMap, (HINSTANCE)hInstance, &LIBID_NCSECWLib);
#if !defined(_WIN32_WCE) || !(_WIN32_WCE <= 211)
		    DisableThreadLibraryCalls((HINSTANCE)hInstance);
#endif
			//NCSecwInitInternal();
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			// We do the iDWT detach in the main process shutdown.
			// If you add anything here, you may need to remove
			// the call to DisableThreadLibraryCalls() above.
			break;
		case DLL_PROCESS_DETACH:
			_Module.Term();
//For windows CE, must call NCSEcwShutdown from program before it is closes
#if !defined(_WIN32_WCE)
//			if (pNCSEcwInfo) {
//				pNCSEcwInfo->bShutdown = TRUE;
//				NCSecwShutdownInternal();
//			}
#endif
			break;
    }

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hRes = _Module.RegisterServer(TRUE);

	if(!SUCCEEDED(hRes)) {
		HMODULE hDll;
		LONG (APIENTRY *pRegOverridePredefKey)(HKEY hKey, HKEY hNewHKey);

		if((hDll = LoadLibrary(NCS_T("advapi32.dll"))) != NULL) {
			if((pRegOverridePredefKey = 
#ifdef _WIN32_WCE
				(LONG(APIENTRY*)(HKEY, HKEY))GetProcAddress(hDll, NCS_T("RegOverridePredefKey"))
#else
				(LONG(APIENTRY*)(HKEY, HKEY))GetProcAddress(hDll, "RegOverridePredefKey")
#endif
				) != NULL) {
				HKEY hNewCRKey;

				if(RegOpenKeyEx(HKEY_CURRENT_USER, NCS_T("SOFTWARE\\Classes"), 0, KEY_ALL_ACCESS, &hNewCRKey) == ERROR_SUCCESS) {
					if(pRegOverridePredefKey(HKEY_CLASSES_ROOT, hNewCRKey) == ERROR_SUCCESS) {
						// registers object, typelib and all interfaces in typelib
						hRes = _Module.RegisterServer(TRUE);

						pRegOverridePredefKey(HKEY_CLASSES_ROOT, NULL);
					}
					RegCloseKey(hNewCRKey);
				}
			}
			FreeLibrary(hDll);
		}
	}
	return(hRes);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	HRESULT hRes = S_OK;
	HMODULE hDll;
	LONG (APIENTRY *pRegOverridePredefKey)(HKEY hKey, HKEY hNewHKey);

	if((hDll = LoadLibrary(NCS_T("advapi32.dll"))) != NULL) {
		if((pRegOverridePredefKey = 
#ifdef _WIN32_WCE
			(LONG(APIENTRY*)(HKEY, HKEY))GetProcAddress(hDll, NCS_T("RegOverridePredefKey"))
#else
			(LONG(APIENTRY*)(HKEY, HKEY))GetProcAddress(hDll, "RegOverridePredefKey")
#endif
			) != NULL) {
			HKEY hNewCRKey;

			if(RegOpenKeyEx(HKEY_CURRENT_USER, NCS_T("SOFTWARE\\Classes"), 0, KEY_ALL_ACCESS, &hNewCRKey) == ERROR_SUCCESS) {
				if(pRegOverridePredefKey(HKEY_CLASSES_ROOT, hNewCRKey) == ERROR_SUCCESS) {
					// registers object, typelib and all interfaces in typelib
					hRes = _Module.UnregisterServer(TRUE);

					pRegOverridePredefKey(HKEY_CLASSES_ROOT, NULL);
				}
				RegCloseKey(hNewCRKey);
			}
		}
		FreeLibrary(hDll);
	}
	if(SUCCEEDED(hRes)) {
		hRes = _Module.UnregisterServer(TRUE);
	}
	return(hRes);
}

#elif defined MACINTOSH

/*int __initialize()
{
	NCSecwInit();
	return TRUE;
}

int __terminate()
{
	pNCSEcwInfo->bShutdown = TRUE;
	NCSecwShutdown();
	return TRUE;
}*/

#endif //defined (WIN32) && !defined(NCSECW_STATIC_LIBS) && !defined(LIBECWJ2)

/*******************************************************
**	Init / shutdown DLL
********************************************************/


/*******************************************************
**	NCSecwInitInternal/ShutdownInternal() - Inits/shutsdown the NCSecw library
**	WARNING! ONLY CALL THESE if you are not linking to the DLL,
**	but are including the code directly inside your application
********************************************************/
extern "C"
//#ifdef NCSJPC_ECW_SUPPORT
//void NCSecwInit_ECW()
//#else
void NCSecwInitInternal()
//#endif
{
	if( !pNCSEcwInfo ) {
		NCSecwStatistics	*pStatistics = (NCSecwStatistics *)NULL;// &cNCSEcwSharedStatistics;
		NCSGlobalLockInfo *pLock;		/**[10]**/

		pLock = NCSGlobalLock(ECW_STATS_LOCK_NAME);		/**[10]**/

		pNCSEcwInfo = (NCSEcwInfo *) NCSMalloc(sizeof(NCSEcwInfo), TRUE);	/**[17]**/
		pNCSEcwInfo->pIDWT = (NCSidwt *) NCSMalloc(sizeof(NCSidwt), TRUE);	/**[17]**/

		pNCSEcwInfo->pNCSFileList = NULL;
#ifdef WIN32
		if(!pStatistics) {		/**[13]**/
			// No NCSecw.dll in system dir, so use shared memory for stats instead.

			pNCSEcwInfo->pStatisticsSHM = NCSMapSHM(sizeof(NCSecwStatistics), TRUE, NCS_STATUS_STRUCT_NAME_VERSION_2);

			if(pNCSEcwInfo->pStatisticsSHM == NULL) {
				// Fails if SHM already exists, which means it's V2 with a global lock
				pNCSEcwInfo->pStatisticsSHM = NCSMapSHM(sizeof(NCSecwStatistics), FALSE, NCS_STATUS_STRUCT_NAME_VERSION_2);
				pNCSEcwInfo->nStatsStructVersion = ECW_STATS_STRUCT_VERSION_2;
				pNCSEcwInfo->StatsLockKey = NCSThreadLSAlloc();
			} else {
				// Worked, which means no V2 SHM exists currently, free it and create a V3 one
				NCSUnmapSHM(pNCSEcwInfo->pStatisticsSHM);
				pNCSEcwInfo->pStatisticsSHM = NCSMapSHM(sizeof(NCSecwStatistics), FALSE, NCS_STATUS_STRUCT_NAME_VERSION);
				pNCSEcwInfo->nStatsStructVersion = ECW_STATS_STRUCT_VERSION;
			}
			pStatistics = (NCSecwStatistics*)pNCSEcwInfo->pStatisticsSHM->pData;
			
		}
#elif defined(MACINTOSH)||defined(POSIX)
		pStatistics = (NCSecwStatistics *)NCSMalloc(sizeof(NCSecwStatistics), TRUE);
		pNCSEcwInfo->nStatsStructVersion = ECW_STATS_STRUCT_VERSION;
#endif

		pNCSEcwInfo->pStatistics = pStatistics;	// shared statistics across applications
		NCSGlobalUnlock(pLock);		/**[10]**/

		pNCSEcwInfo->bEcwpReConnect = FALSE; //[21]
		pNCSEcwInfo->bJP2ICCManage = TRUE;	//[22]
		pNCSEcwInfo->nMaxJP2FileIOCache = 1024; //[22]
		pNCSEcwInfo->nMaxProgressiveViewSize = NCSECW_MAX_VIEW_SIZE_TO_CACHE; /**[25]**/
		pNCSEcwInfo->nForceFileReopen = FALSE;	// normally want to merge file opens for optimization
		pNCSEcwInfo->bForceLowMemCompress = FALSE;
		pNCSEcwInfo->tLastCachePurge = NCSGetTimeStampMs();
		pNCSEcwInfo->nAggressivePurge = 0;		// larger number means be more aggressive next purge
		pNCSEcwInfo->pIDWT->eIDWTState = NCSECW_THREAD_DEAD;
		pNCSEcwInfo->nMaximumOpen = NCSECW_MAX_UNUSED_CACHED_FILES;		/**[18]**/
#ifdef POSIX
		pNCSEcwInfo->nMaximumOpen = MIN(pNCSEcwInfo->nMaximumOpen, (UINT32)sysconf(_SC_OPEN_MAX) / 4);	/**[18]**/
#endif // POSIX

#ifdef WIN32
		pNCSEcwInfo->m_hSuspendEvent = NULL;		
#endif // WIN32

		pNCSEcwInfo->nPurgeDelay = NCSECW_PURGE_DELAY_MS;					/**[19]**/
		pNCSEcwInfo->nFilePurgeDelay = NCSECW_FILE_PURGE_TIME_MS;			/**[19]**/
		pNCSEcwInfo->nMinFilePurgeDelay = NCSECW_FILE_MIN_PURGE_TIME_MS;	/**[19]**/
		pNCSEcwInfo->nMaxOffsetCache = NCSECW_MAX_OFFSET_CACHE;				/**[19]**/

		if( pStatistics->nApplicationsOpen == 0 ) {
#ifdef MACINTOSH
			// This causes problems on the Mac, so we'll disable caching until we get
			// the threading/IPS/Shared memory support in place
			pStatistics->nMaximumCacheSize = NCSPhysicalMemorySize() / 4;
#else
			pStatistics->nMaximumCacheSize = NCSPhysicalMemorySize() / 4;
#endif
			// FIXME!! Should remember these by storing them in the registry
			pStatistics->nBlockingTime		= NCSECW_BLOCKING_TIME_MS;
			pStatistics->nRefreshTime		= NCSECW_REFRESH_TIME_MS;
		}
		NCSEcwStatsLock();
		NCSEcwStatsIncrement(&pStatistics->nApplicationsOpen, 1);
		NCSEcwStatsUnLock();

#if defined LIBECWJ2 || defined NCSECW_STATIC_LIBS
		NCSUtilInit();
		NCScnetInit();
#endif
		NCSMutexInit(&pNCSEcwInfo->mutex);
#ifdef NCSJPC_ECW_SUPPORT
		NCScbmInitThreadIDWT(pNCSEcwInfo->pIDWT);
#endif // NCSJPC_ECW_SUPPORT
		pNCSEcwInfo->bShutdown = FALSE;
	}
}

/*
** This stub now exported to allow the same code to be linked both statically and 
** against either the NCSEcw, NCSUtil, and NCScnet DLLs or the libecwj2 DLL.
** It is expected that applications linking statically will explicitly call 
** NCSecwShutdown and NCSecwInit to destroy and initialise library resources. [23]
*/
extern "C" void NCSecwInit()
{
//#if defined LIBECWJ2 || defined NCSECW_STATIC_LIBS 
	NCSecwInitInternal();
//#endif
}

/*
**	At this point, we must assume that threads are potentially dead, and mutex's might
**	be locked. So we basically blow through, bypassing usual lock states.
**  For windows CE must call NCSecwShutdown from your program before exiting
*/
extern "C"
//#ifdef NCSJPC_ECW_SUPPORT
//void NCSecwShutdown_ECW()
//#else
void NCSecwShutdownInternal()
//#endif
{
	if( pNCSEcwInfo ) {
		BOOLEAN bTrueShutdown;
		NCSGlobalLockInfo *pLock;		/**[10]**/

#if defined(_WIN32_WCE)
		pNCSEcwInfo->bShutdown = TRUE;
#endif

		bTrueShutdown = pNCSEcwInfo->bShutdown;
		pNCSEcwInfo->bShutdown = TRUE;
		if( !bTrueShutdown )
			NCSMutexBegin(&pNCSEcwInfo->mutex);		// mutex if an explicit shutdown (thread still alive)

		CNCSJP2FileView::Shutdown();

		NCSEcwStatsLock();

		// force shutdown of all files that are opened
		while(pNCSEcwInfo->pNCSFileList) {
			NCSFile *pNCSFile = pNCSEcwInfo->pNCSFileList;
			if( pNCSFile->nUsageCount ) 
				NCSEcwStatsDecrement(&pNCSEcwInfo->pStatistics->nFilesOpen, 1);
			else
				NCSEcwStatsDecrement(&pNCSEcwInfo->pStatistics->nFilesCached, 1);		// [05] Decrement usage count
			NCSecwCloseFileCompletely(pNCSEcwInfo->pNCSFileList);
		}
		if( pNCSEcwInfo->pStatistics) {
			NCSEcwStatsDecrement(&pNCSEcwInfo->pStatistics->nApplicationsOpen, 1);
#if defined(MACINTOSH)||defined(POSIX)
			NCSFree(pNCSEcwInfo->pStatistics);
			pNCSEcwInfo->pStatistics = NULL;
#endif		
		}
		NCSEcwStatsUnLock();

		pLock = NCSGlobalLock(ECW_STATS_LOCK_NAME);		/**[10]**/
		if(pNCSEcwInfo->pStatisticsSHM) {				/**[13]**/
			NCSUnmapSHM(pNCSEcwInfo->pStatisticsSHM);	
			pNCSEcwInfo->pStatisticsSHM = NULL;
		}
#ifdef WIN32
		if(pNCSEcwInfo->StatsLockKey) {
			NCSThreadLSFree(pNCSEcwInfo->StatsLockKey);
		}
#endif //WIN32
		NCSGlobalUnlock(pLock);		/**[10]**/

		NCScbmFinishThreadIDWT(pNCSEcwInfo->pIDWT);
		if(pNCSEcwInfo->pIDWT->ppNCSFileView)
			NCSFree(pNCSEcwInfo->pIDWT->ppNCSFileView);
		NCSFree(pNCSEcwInfo->pIDWT);

		if( !bTrueShutdown )
			NCSMutexEnd(&pNCSEcwInfo->mutex);		// mutex if an explicit shutdown (thread still alive)

		NCSMutexFini(&pNCSEcwInfo->mutex);

#if defined LIBECWJ2 || defined NCSECW_STATIC_LIBS
		NCScnetShutdown();
		NCSUtilFini();
#endif
		NCSFree(pNCSEcwInfo);
		pNCSEcwInfo = NULL;
	}
		
#if defined(WIN32) && !defined(_WIN32_WCE) && defined(NCS_BUILD_WITH_COM) && !defined(NCS_MINDEP_BUILD)//&&!defined(NCSJPC_ECW_SUPPORT)
	CComNCSRenderer::ShutDown();
#endif
}

/*
** This stub now exported to allow the same code to be linked both statically and 
** against either the NCSEcw, NCSUtil, and NCScnet DLLs or the libecwj2 DLL.
** It is expected that applications linking statically will explicitly call 
** NCSecwShutdown and NCSecwInit to destroy and initialise library resources. [23]
*/
extern "C" void NCSecwShutdown()
{
//#ifdef NCSECW_STATIC_LIBS
	NCSecwShutdownInternal();
//#endif
}


/*******************************************************
**	NCSecwConfig() - configures the ECW/CBM
**	Normally never called. Mainly used for client/server tests
**		nForceFileReopen	- force a new open for each file rather
**							  than using cache. Intended to enable multiple
**							  clients to be simulated for server load tests
**							  DO NOT SET THIS TO TRUE EXCEPT FOR TESTS
**							  as normally the library optimizes file access
**							  by merging them into a single file open
********************************************************/
extern "C"
//#ifdef NCSJPC_ECW_SUPPORT
//void NCSecwConfig_ECW(BOOLEAN bNoTextureDither,BOOLEAN bForceFileReopen)
//#else
void NCSecwConfig(BOOLEAN bNoTextureDither,BOOLEAN bForceFileReopen)
//#endif
{
	if (!pNCSEcwInfo) {
		NCSecwInitInternal();
	}
	//pNCSEcwInfo->bNoTextureDither = bNoTextureDither;
	//pNCSEcwInfo->nForceFileReopen = bForceFileReopen;
	NCSecwSetConfig(NCSCFG_TEXTURE_DITHER, (bNoTextureDither == TRUE) ? FALSE : TRUE);
	NCSecwSetConfig(NCSCFG_FORCE_FILE_REOPEN, bForceFileReopen);
}

/*******************************************************
**	NCSecwSetConfig() - configures the ECW/CBM
**	Normally never called. Mainly used for client/server tests
**
**	NCSCT_TEXTURE_DITHER, BOOLEAN		- Turn on/off texture dither
********************************************************/
extern "C" 
//#ifdef NCSJPC_ECW_SUPPORT
//NCSError NCSecwSetConfig_ECW(NCSEcwConfigType eType, ...)
//#else
NCSError NCSecwSetConfig(NCSEcwConfigType eType, ...)
//#endif
{
	va_list va;
	va_start(va, eType);

	if (!pNCSEcwInfo) {
		NCSecwInitInternal();
	}

	switch(eType) {
		case NCSCFG_TRY_ECWP_RECONNECT:
			pNCSEcwInfo->bEcwpReConnect = va_arg(va, BOOLEAN);
			break;

		case NCSCFG_TEXTURE_DITHER:
			pNCSEcwInfo->bNoTextureDither = (va_arg(va, BOOLEAN) == TRUE) ? FALSE : TRUE;
			break;

		case NCSCFG_FORCE_FILE_REOPEN:
			pNCSEcwInfo->nForceFileReopen = va_arg(va, BOOLEAN);
			break;

		case NCSCFG_CACHE_MAXMEM:
			{
				NCSEcwStatsLock();
				pNCSEcwInfo->pStatistics->nMaximumCacheSize = va_arg(va, UINT32);
				NCSEcwStatsUnLock();
			}
			break;

		case NCSCFG_CACHE_MAXOPEN:
			pNCSEcwInfo->nMaximumOpen = va_arg(va, UINT32);
			break;

		case NCSCFG_BLOCKING_TIME_MS:
			{
				NCSEcwStatsLock();
				pNCSEcwInfo->pStatistics->nBlockingTime = (UINT32)va_arg(va, NCSTimeStampMs);
				NCSEcwStatsUnLock();
			}
			break;

		case NCSCFG_REFRESH_TIME_MS:
			{
				NCSEcwStatsLock();
				pNCSEcwInfo->pStatistics->nRefreshTime = (UINT32)va_arg(va, NCSTimeStampMs);
				NCSEcwStatsUnLock();
			}
			break;

		case NCSCFG_PURGE_DELAY_MS:
				pNCSEcwInfo->nPurgeDelay = va_arg(va, NCSTimeStampMs);
			break;

		case NCSCFG_FILE_PURGE_DELAY_MS:
				pNCSEcwInfo->nFilePurgeDelay = va_arg(va, NCSTimeStampMs);
			break;

		case NCSCFG_MIN_FILE_PURGE_DELAY_MS:
				pNCSEcwInfo->nMinFilePurgeDelay = va_arg(va, NCSTimeStampMs);
			break;

		case NCSCFG_ECWP_PROXY:
			{
				char *pValue = va_arg(va, char *);
				if( pValue[0] == '\0' ) { //if empty string
					//delete the registry key
					NCSPrefDeleteValueUser(NCS_ECW_PROXY_KEY);
				} else { 
					// create the proxy registry key
					NCSPrefSetUserString(NCS_ECW_PROXY_KEY, pValue);
				}
			}
			break;

		case NCSCFG_FORCE_LOWMEM_COMPRESS:
				pNCSEcwInfo->bForceLowMemCompress = va_arg(va, BOOLEAN);
			break;

		case NCSCFG_JP2_MANAGE_ICC:
				pNCSEcwInfo->bJP2ICCManage = va_arg(va, BOOLEAN);
			break;
		
		case NCSCFG_JP2_FILEIO_CACHE_MAXMEM:
				pNCSEcwInfo->nMaxJP2FileIOCache = va_arg(va, UINT32);
			break;

		case NCSCFG_MAX_PROGRESSIVE_VIEW_SIZE:
				pNCSEcwInfo->nMaxProgressiveViewSize = va_arg(va, UINT32);

		default:
				va_end(va);
				return(NCS_INVALID_PARAMETER);
			break;
	}
	va_end(va);
	return(NCS_SUCCESS);
}

extern "C" 
//#ifdef NCSJPC_ECW_SUPPORT
//NCSError NCSecwGetConfig_ECW(NCSEcwConfigType eType, ...)
//#else
NCSError NCSecwGetConfig(NCSEcwConfigType eType, ...)
//#endif
{
	va_list va;
	va_start(va, eType);

	if (!pNCSEcwInfo) {
		NCSecwInitInternal();
	}
	switch(eType) {
		case NCSCFG_TRY_ECWP_RECONNECT:
			*(va_arg(va, BOOLEAN*)) = pNCSEcwInfo->bEcwpReConnect;
			break;

		case NCSCFG_TEXTURE_DITHER:
			*(va_arg(va, BOOLEAN*)) = (pNCSEcwInfo->bNoTextureDither == TRUE) ? FALSE : TRUE;
			break;

		case NCSCFG_FORCE_FILE_REOPEN:
				*(va_arg(va, BOOLEAN*)) = pNCSEcwInfo->nForceFileReopen;
			break;

		case NCSCFG_CACHE_MAXMEM:
			{
				NCSEcwStatsLock();
				*(va_arg(va, UINT32*)) = pNCSEcwInfo->pStatistics->nMaximumCacheSize;
				NCSEcwStatsUnLock();
			}
			break;

		case NCSCFG_CACHE_MAXOPEN:
				*(va_arg(va, UINT32*)) = pNCSEcwInfo->nMaximumOpen;
			break;

		case NCSCFG_BLOCKING_TIME_MS:
			{
				NCSEcwStatsLock();
				*(va_arg(va, NCSTimeStampMs*)) = (NCSTimeStampMs)pNCSEcwInfo->pStatistics->nBlockingTime;
				NCSEcwStatsUnLock();
			}
			break;

		case NCSCFG_REFRESH_TIME_MS:
			{
				NCSEcwStatsLock();
				*(va_arg(va, NCSTimeStampMs*)) = (NCSTimeStampMs)pNCSEcwInfo->pStatistics->nRefreshTime;
				NCSEcwStatsUnLock();
			}
			break;

		case NCSCFG_PURGE_DELAY_MS:
				*(va_arg(va, NCSTimeStampMs*)) = pNCSEcwInfo->nPurgeDelay;
			break;

		case NCSCFG_FILE_PURGE_DELAY_MS:
				*(va_arg(va, NCSTimeStampMs*)) = pNCSEcwInfo->nFilePurgeDelay;
			break;

		case NCSCFG_MIN_FILE_PURGE_DELAY_MS:
				*(va_arg(va, NCSTimeStampMs*)) = pNCSEcwInfo->nMinFilePurgeDelay;
			break;

		case NCSCFG_ECWP_PROXY:
			{
				char *pszProxyServer = (char*)NULL;
				NCSPrefGetUserString(NCS_ECW_PROXY_KEY, &pszProxyServer);
				*(va_arg(va, char**)) = pszProxyServer;
			}
			break;

		case NCSCFG_FORCE_LOWMEM_COMPRESS:
				*(va_arg(va, BOOLEAN*)) = pNCSEcwInfo->bForceLowMemCompress;
			break;
		
		case NCSCFG_JP2_MANAGE_ICC:
				*(va_arg(va, BOOLEAN*)) = pNCSEcwInfo->bJP2ICCManage;
			break;

		case NCSCFG_JP2_FILEIO_CACHE_MAXMEM:
				*(va_arg(va, UINT32*)) = pNCSEcwInfo->nMaxJP2FileIOCache;
			break;

		case NCSCFG_MAX_PROGRESSIVE_VIEW_SIZE:
				*(va_arg(va, UINT32*)) = pNCSEcwInfo->nMaxProgressiveViewSize;

		default:
				va_end(va);
				return(NCS_INVALID_PARAMETER);
			break;
	}
	va_end(va);
	return(NCS_SUCCESS);
}

/*******************************************************
**	NCSecwOpenFile() - open an NCS pointer to an ECW file
**
**	Notes:
**	a)	If bReadOffsets, p_block_offsets etc will be valid
**	b)	if bReadMemImage, pHeaderMemImage will be valid
**	c)	Can only bReadOffsets and/or bReadMemImage if the szInputFilename
**		points to a local file. So if this is a REMOTE file, then these
**		values are forced to FALSE, regardless of what the caller wanted.
**	d)	If bReadOffsets, then a file handle will be left open for the file,
**		and the NCSecwReadBlock() function call can be used (on this local file)
**	e)	If the file is already open, your re-open must not ask for more that was
**		previously opened (so you can't open the file without asking for block
**		offsets, and then open it again, this time asking for block offsets).
********************************************************/
extern "C"
NCSError NCSecwOpenFile(
					NCSFile **ppNCSFile,
					char *szUrlPath,			// input file name or network path
					BOOLEAN bReadOffsets,		// TRUE if the client wants the block Offset Table
					BOOLEAN bReadMemImage)		// TRUE if the client wants a Memory Image of the Header
{
	NCSFile	*pNCSFile = NULL;
	UINT32	nSemiUniqueId;
	UINT8	*pFileHeaderMemImage=NULL;
	UINT32	nFileHeaderMemImageLen=0;
	char	*pProtocol, *pHost, *pFilename;
	int		nProtocolLength, nHostLength, nFilenameLength;
	pProtocol = (char *)NULL;
	pHost = (char *)NULL;
	pFilename = (char *)NULL;
	
	if (!pNCSEcwInfo) {
		NCSecwInitInternal();
	}

	if (!ppNCSFile)
		return NCS_INVALID_ARGUMENTS;
	*ppNCSFile = NULL;

	NCSMutexBegin(&pNCSEcwInfo->mutex);

	// Get URL info first off. This will also sort out / vs \ problems
	NCSecwNetBreakdownUrl(szUrlPath, &pProtocol, &nProtocolLength,
									 &pHost, &nHostLength,
									 &pFilename, &nFilenameLength);
	nSemiUniqueId = NCSecwSemiUniqueId(szUrlPath);

	// See if the file is already in the cache system
	// unless nForceFileReopen == TRUE, in which case we
	// are running server load tests, so want to open a new
	// file channel

	if( pNCSEcwInfo->nForceFileReopen == FALSE ) {
		pNCSFile = pNCSEcwInfo->pNCSFileList;
		while(pNCSFile) {
			if( pNCSFile->SemiUniqueId == nSemiUniqueId && pNCSFile->bValid) {
				if( stricmp(pNCSFile->szUrlPath, szUrlPath) == 0 ) {
					// See if the file has changed on disk. If so, mark it as invalid and keep looking
					// in cache (the file cache will be removed from cache once no opens are present for it)
#ifdef NOTUSED
// no longer required, file is locked on disk so can't change, this just sucks up CPU cycles...
					if( pNCSFile->bLocalFile ) {
						DATETIME	tModifiedTime;/**[17]**/

						if( NCSecwGetFileModifiedTime(pNCSFile, &tModifiedTime)
						 || tModifiedTime > pNCSFile->tModifiedTime ) {
							NCSEcwStatsLock();
							pNCSFile->bValid = FALSE;
							NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nFilesModified, 1);
							NCSEcwStatsUnlock();
						}
					}
#endif
					if((pProtocol && pHost) ||												/*[08]*/
					   ((!bReadOffsets || (bReadOffsets && pNCSFile->bReadOffsets)) &&		/*[08]*/
						(!bReadMemImage || (bReadMemImage && pNCSFile->bReadMemImage)))) {	/*[08]*/
					
						if( pNCSFile->bValid )
							break;		// found a match, so use the cached file
					}
				}
			}
			pNCSFile = pNCSFile->pNextNCSFile;
		}
	}
	// If we have a match, just return the existing file
	if( pNCSFile ) {
		NCSEcwStatsLock();

		NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nFilesCacheHits, 1);
		if( !pNCSFile->nUsageCount) {
			NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nFilesOpen, 1);
			NCSEcwStatsDecrement(&pNCSEcwInfo->pStatistics->nFilesCached, 1);
		}
		NCSEcwStatsUnLock();

		pNCSFile->nUsageCount += 1;
		NCSMutexEnd(&pNCSEcwInfo->mutex);

		*ppNCSFile = pNCSFile;
		return(NCS_SUCCESS);
	}

	NCSEcwStatsLock();
	// File was not in cache, so have to load it
	NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nFilesCacheMisses, 1);
	NCSEcwStatsUnLock();

	pNCSFile = (NCSFile *) NCSMalloc(sizeof(NCSFile), FALSE);

	if( !pNCSFile ) {
		NCSMutexEnd(&pNCSEcwInfo->mutex);
		return(NCS_FILE_NO_MEMORY);
	}
	pNCSFile->szUrlPath = (char *)NCSMalloc( (UINT32)strlen(szUrlPath) + 1, FALSE);
	if( !pNCSFile->szUrlPath ) {
		NCSFree(pNCSFile);
		NCSMutexEnd(&pNCSEcwInfo->mutex);
		return(NCS_FILE_NO_MEMORY);
	}
	strcpy(pNCSFile->szUrlPath, szUrlPath);

	// Init client specific information
	pNCSFile->pBlockCachePool = NULL;
	pNCSFile->pFirstCachedBlock = NULL;
	pNCSFile->pWorkingCachedBlock = NULL;
	pNCSFile->pLastReceivedCachedBlock = NULL;
	pNCSFile->pSendPacket = NULL;

	pNCSFile->nClientUID = 0;
	pNCSFile->nServerSequence = 0;
	pNCSFile->nClientSequence = 1;
	pNCSFile->pLevel0ZeroBlock = pNCSFile->pLevelnZeroBlock = NULL;
	pNCSFile->pClientNetID = NULL;
	pNCSFile->bSendInProgress = FALSE;
	pNCSFile->nRequestsXmitPending = 0;
	pNCSFile->nCancelsXmitPending = 0;
	pNCSFile->tLastSetViewTime = NCSGetTimeStampMs();

	// init list and file information
	pNCSFile->bValid = TRUE;		// file is currently valid (not changed on disk yet)

	pNCSFile->pNextNCSFile = pNCSFile->pPrevNCSFile = NULL;
	pNCSFile->nUsageCount = 1;
	pNCSFile->SemiUniqueId = nSemiUniqueId;

	pNCSFile->bIsConnected = TRUE;
	pNCSFile->bIsCorrupt = FALSE;
	pNCSFile->bFileIOError = FALSE;
	NCSMutexInit( &pNCSFile->mFileAccess ); //[20]

	// decide if local or remote file
	{
		if( pProtocol && pHost ) {
			NCSError nResult = NCScbmNetFileOpen(&pFileHeaderMemImage, &nFileHeaderMemImageLen, pNCSFile, pProtocol);
			if( nResult != NCS_SUCCESS) {
				NCSFree(pNCSFile->szUrlPath);
				NCSFree(pNCSFile);
				NCSMutexEnd(&pNCSEcwInfo->mutex);
				return(nResult);
			}
			pNCSFile->bLocalFile = FALSE;
		}
		else {
			pFileHeaderMemImage = NULL;
			pNCSFile->bLocalFile = TRUE;
		}
	}

	if( pNCSFile->bLocalFile ) {
		// always read offsets and Memory Image for a local file, even if not asked for
#ifdef NOTDEF		/*[07]*/
		bReadOffsets = TRUE;
#endif
		bReadMemImage = TRUE;
	}
	else {
		// don't read offsets or memory image
		bReadOffsets = FALSE;
		bReadMemImage = FALSE;
	}
	pNCSFile->bReadOffsets = bReadOffsets;
	pNCSFile->bReadMemImage = bReadMemImage;
	pNCSFile->pNCSFileViewList = NULL;
	pNCSFile->pNCSCachedFileViewList = NULL;

	pNCSFile->pOffsetCache = NULL;
	pNCSFile->nOffsetCache = 0;

	pNCSFile->tsLastReconnectTry = 0; //[21]
	pNCSFile->nReconnectCount = 0;

	// Get file modified time
//	if( pNCSFile->bLocalFile )
//		(void) NCSecwGetFileModifiedTime(pNCSFile, &pNCSFile->tModifiedTime);

	pNCSFile->pTopQmf = erw_decompress_open( szUrlPath, pFileHeaderMemImage, bReadOffsets, bReadMemImage ); //[21]
	if( !pNCSFile->pTopQmf ) {
		if( pFileHeaderMemImage )
			NCSFree( pFileHeaderMemImage );
		if( pNCSFile->pClientNetID )
			NCScnetDestroy(pNCSFile->pClientNetID);
		NCSFree(pNCSFile->szUrlPath);
		NCSFree(pNCSFile);
		NCSMutexEnd(&pNCSEcwInfo->mutex);
//		_ASSERT( pNCSFile->pTopQmf );
		return(NCS_ECW_ERROR);
	}

	if( !pNCSFile->bLocalFile && pFileHeaderMemImage && (nFileHeaderMemImageLen>0) ) { //[21]
		pNCSFile->pTopQmf->pHeaderMemImage = pFileHeaderMemImage;
		pNCSFile->pTopQmf->nHeaderMemImageLen = nFileHeaderMemImageLen;
	}

	pNCSFile->nUnpackedBlockBandLength = 
		pNCSFile->pTopQmf->x_block_size * pNCSFile->pTopQmf->y_block_size * sizeof(INT16);

	pNCSFile->pNCSCachePurge = (NCSFileCachePurge *) NCSMalloc(sizeof(NCSFileCachePurge) *
		pNCSFile->pTopQmf->nr_levels, FALSE);
	if(!pNCSFile->pNCSCachePurge) {
		return NCS_COULDNT_ALLOC_MEMORY;
	}

	// Point to File Info in the QMF structure
	pNCSFile->pFileInfo = pNCSFile->pTopQmf->pFileInfo;

	// Note the file is now open
	NCSEcwStatsLock();
	NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nFilesOpen, 1);
	NCSEcwStatsUnLock();

	pNCSFile->pNextNCSFile = pNCSEcwInfo->pNCSFileList;
	if( pNCSEcwInfo->pNCSFileList )
		pNCSEcwInfo->pNCSFileList->pPrevNCSFile = pNCSFile;
	pNCSEcwInfo->pNCSFileList = pNCSFile;

	NCSMutexEnd(&pNCSEcwInfo->mutex);
	*ppNCSFile = pNCSFile;
	return(NCS_SUCCESS);
}

/*******************************************************
**	NCSecwCloseFile() - close a NCS file handle to an ECW file
**
** Indicates finished with a NCS file handle. Will only close the file and
** free the memory structures if total usage for this file has dropped to zero
** and the file will be flushed from cache due to too many files open.
**	Returns zero if no error on the close.
********************************************************/
extern "C"
int	NCSecwCloseFile( NCSFile *pNCSFile)
{
	if (!pNCSEcwInfo) {
		NCSecwInitInternal();
	}

	if( pNCSFile ) {
		NCSMutexBegin(&pNCSEcwInfo->mutex);
		pNCSFile->nUsageCount -= 1;
		if( pNCSFile->nUsageCount == 0 ) {
			NCSEcwStatsLock();
			NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nFilesOpen, 1);		// Decrement usage count
			NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nFilesCached, 1);	// Decrement usage count
			// if too many files cached, or file cache is no longer valid
			if( pNCSEcwInfo->pStatistics->nFilesCached > (INT32)pNCSEcwInfo->nMaximumOpen
			 || !pNCSFile->bValid ) {
				NCSecwCloseFileCompletely(pNCSFile);				// Remove from cache
				NCSEcwStatsDecrement(&pNCSEcwInfo->pStatistics->nFilesCached, 1);		// [05] Decrement usage count
			}
			NCSEcwStatsUnLock();
		}
		NCSMutexEnd(&pNCSEcwInfo->mutex);
	}
	return(0);
}

/*
**	Flush file from cache. Does NOT mutex - caller must do the mutex
*/
extern "C"
int	NCSecwCloseFileCompletely( NCSFile *pNCSFile)
{

	// Remove any outstanding file views. Can't call the CloseFileView,
	// as that calls this routine (recursive error)
	while(pNCSFile->pNCSFileViewList) {
		NCSEcwStatsLock();
		NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nFileViewsOpen, 1);
		NCSEcwStatsUnLock();
		NCScbmCloseFileViewCompletely(&(pNCSFile->pNCSFileViewList), pNCSFile->pNCSFileViewList);
	}


	// Remove this file from the File List
	if( pNCSFile->pClientNetID )
		NCScnetDestroy(pNCSFile->pClientNetID);
	if( pNCSEcwInfo->pNCSFileList == pNCSFile )
		pNCSEcwInfo->pNCSFileList = pNCSFile->pNextNCSFile;
	if( pNCSFile->pNextNCSFile )
		pNCSFile->pNextNCSFile->pPrevNCSFile = pNCSFile->pPrevNCSFile;
	if( pNCSFile->pPrevNCSFile )
		pNCSFile->pPrevNCSFile->pNextNCSFile = pNCSFile->pNextNCSFile;
	erw_decompress_close(pNCSFile->pTopQmf);
	NCSFree(pNCSFile->szUrlPath);
	if( pNCSFile->pLevel0ZeroBlock )
		NCSFree( pNCSFile->pLevel0ZeroBlock );
	if( pNCSFile->pLevelnZeroBlock )
		NCSFree( pNCSFile->pLevelnZeroBlock );

	// Free the cached blocks
	{
		NCSFileCachedBlock	*pNextCachedBlock = pNCSFile->pFirstCachedBlock;
		while( pNextCachedBlock ) {
			if( pNextCachedBlock->pPackedECWBlock ) {
				NCSEcwStatsLock();
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
				DisposeHandle( pNextCachedBlock->pPackedECWBlock );
#else
				NCSFree(pNextCachedBlock->pPackedECWBlock);
#endif
				pNextCachedBlock->pPackedECWBlock = NULL;
				NCSEcwStatsDecrement(&pNCSEcwInfo->pStatistics->nPackedBlocksCacheSize, pNextCachedBlock->nPackedECWBlockLength);
				NCSEcwStatsUnLock();
			}
			if( pNextCachedBlock->pUnpackedECWBlock ) {
				NCSEcwStatsLock();
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
				DisposeHandle( pNextCachedBlock->pUnpackedECWBlock );
#else
				NCSFree(pNextCachedBlock->pUnpackedECWBlock);
#endif
				pNextCachedBlock->pUnpackedECWBlock = NULL;
				NCSEcwStatsDecrement(&pNCSEcwInfo->pStatistics->nUnpackedBlocksCacheSize, pNextCachedBlock->nUnpackedECWBlockLength);
				NCSEcwStatsUnLock();
			}
#ifdef POSIX
			/**[24]**/
			if (pNextCachedBlock == pNextCachedBlock->pNextCachedBlock)
			{
				/* Get rid of simple self-cycles here */
                           	pNextCachedBlock = NULL;
			}
			else
			{
				/* And prevent bigger problems here */
				NCSFileCachedBlock *prevBlock = pNextCachedBlock;
				pNextCachedBlock = pNextCachedBlock->pNextCachedBlock;
				prevBlock->pNextCachedBlock = NULL;
			}
#else
			pNextCachedBlock = pNextCachedBlock->pNextCachedBlock;
#endif
			
		}
	}


	// Free cached block pointers pool, which will destroy the pool alloc'd pointers to cached blocks
	// As we do this after freeing the cached blocks, we can just block the entire list out
	// rather than freeing each entry.
	if( pNCSFile->pBlockCachePool )
		NCSPoolDestroy(pNCSFile->pBlockCachePool);

	// Free the SendPacket structure. The network link has allready been shutdown
	if(pNCSFile->pSendPacket)
		NCSFree(pNCSFile->pSendPacket);


	// Free the cache purging structures
	if(pNCSFile->pNCSCachePurge)
		NCSFree(pNCSFile->pNCSCachePurge);

	// Finalize file access mutex
	NCSMutexFini(&pNCSFile->mFileAccess); //[20]

	NCSFree(pNCSFile->pOffsetCache);

	NCSFree(pNCSFile);

	return(0);
}

/*******************************************************
**	NCSecwGetStatistics()	- Returns pointer to statistics
**
**	Notes:
********************************************************/
extern "C"
NCSecwStatistics *NCSecwGetStatistics( void )
{
	if (!pNCSEcwInfo) {
		NCSecwInitInternal();
	}

	if( pNCSEcwInfo )
		if( pNCSEcwInfo->pStatistics )
			return( pNCSEcwInfo->pStatistics );
	return( NULL );
}

extern "C" void NCSecwGlobalLock(void)
{
	NCSMutexBegin(&pNCSEcwInfo->mutex);
}

extern "C" BOOLEAN NCSecwGlobalTryLock(void)
{
	return(NCSMutexTryBegin(&pNCSEcwInfo->mutex));
}

extern "C" void NCSecwGlobalUnLock(void)
{
	NCSMutexEnd(&pNCSEcwInfo->mutex);
}

/*******************************************************
**	NCSecwSemiUniqueId()	- Returns a semi-unique ID for the UrlPath
**
**	Notes:
** [04]
**	(1)	converts case to lowercase
**	(2)	\ and / are summed as the same item
********************************************************/
static UINT32	NCSecwSemiUniqueId(char *szUrlPath)
{
	UINT32	nSemiUniqueId = 1;

	while( TRUE ) {
		char c = *szUrlPath++;

		if( !c )
			break;		// finish on NULL
		if( c == '/' || c == '\\' ) {
			nSemiUniqueId += '/';
		}
		else {
			if( c > 64 && c < 91 ) // isupper doesn't return correct result if using Unicode
				nSemiUniqueId += c - ('A' - 'a');	/**[17]**/
			else
				nSemiUniqueId += c;
		}
	}
	return(nSemiUniqueId);
}



/*******************************************************
**	NCSecwGetFileModifiedTime()	- Returns time file was modifed
**
**	Notes:
**		Only works on local files. Returns error for non-local files
********************************************************/
#ifdef NOTUSED
static int NCSecwGetFileModifiedTime(NCSFile *pNCSFile, DATETIME *tModifiedTime) /**[17]**/
{
#ifdef MACINTOSH
	//DWB	Probably insufficient for test, but we'll attempt to use it.
	*tModifiedTime = (time_t)0;
	return TRUE;
#elif defined(PALM)
	return(0);
#elif defined(_WIN32_WCE)
	return(0);
#else	/* MACINTOSH */
   struct stat StatBuffer;
   int nResult;
   
   nResult = stat(pNCSFile->szUrlPath, &StatBuffer);
   if( nResult ) {
	   *tModifiedTime = 0;		// in case they are using this for time checking
	   return(1);
   }
   *tModifiedTime = StatBuffer.st_mtime;
	return(0);
#endif	/* MACINTOSH */
}
#endif

#include "NCSBuildNumber.h"

extern "C"
void NCSGetLibVersion( INT32 *nMajor, INT32 *nMinor )
{
	// Format of NCS_VERSION_STRING ==> "1,01,1,0\0"
	INT32 dummy1, dummy2;
	const char *pVersionString = NCS_VERSION_STRING;
	sscanf(pVersionString, "%d,%d,%d,%d", nMajor, nMinor, &dummy1, &dummy2);  

	return;
}

void NCSEcwStatsLock(void)
{
#ifdef WIN32
	if(pNCSEcwInfo->nStatsStructVersion <= 2)
		NCSThreadLSSetValue(pNCSEcwInfo->StatsLockKey, (void*)NCSGlobalLock(ECW_STATS_LOCK_NAME));
#else
	NCSMutexBegin(&pNCSEcwInfo->mutex);
#endif
		
}

void NCSEcwStatsUnLock(void)
{
#ifdef WIN32
	if(pNCSEcwInfo->StatsLockKey) {
		NCSGlobalLockInfo *pStatsLock = (NCSGlobalLockInfo*)NCSThreadLSGetValue(pNCSEcwInfo->StatsLockKey);
		if(pStatsLock) {
			NCSGlobalUnlock(pStatsLock);
			NCSThreadLSSetValue(pNCSEcwInfo->StatsLockKey, (void*)NULL);
		}
	}
#else
	NCSMutexEnd(&pNCSEcwInfo->mutex);
#endif
}

void NCSEcwStatsIncrement(NCSEcwStatsType *pVal, INT32 n)
{
#ifdef WIN32
	if(pNCSEcwInfo->nStatsStructVersion >= 3) {
#ifdef _WIN32_WCE
		while(n-- > 0) {
			InterlockedIncrement(pVal);
		}
#else
		InterlockedExchangeAdd(pVal, n);
#endif
	} else
#endif
		*pVal += n;
}

void NCSEcwStatsDecrement(NCSEcwStatsType *pVal, INT32 n)
{
#ifdef WIN32
	if(pNCSEcwInfo->nStatsStructVersion >= 3) {
#ifdef NOTUSED
		INT32 number = n;
		while(number > 0) {
			InterlockedDecrement(pVal);
			number--;
		}
#endif
		InterlockedExchangeAdd(pVal, -n);
	} else
#endif
		*pVal -= n;
}
