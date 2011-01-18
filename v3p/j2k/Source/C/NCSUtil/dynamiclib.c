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
** FILE:   	NCSUtil\dynamiclib.c
** CREATED:	Thursday 2 March 2000
** AUTHOR: 	Mark Sheridan
** PURPOSE:	Dynamic library loading routines
** EDITS:
	01	30Oct00	 ny	Merge CE/Palm modification in preparation for Macintosh port
 *******************************************************/


#ifdef POSIX
#include <dlfcn.h>
#endif

#include "NCSDefs.h"
#ifdef WIN32
#include "NCSUtil.h" /**[01]**/
#endif
#include "NCSDynamicLib.h"

NCS_DLHANDLE NCSDlOpen( char* pLibraryName )
{
	NCS_DLHANDLE hLibHandle;
	
#if defined WIN32																/**[01]**/
	hLibHandle = LoadLibrary( OS_STRING(pLibraryName) );
#elif defined PALM															/**[01]**/
	if(SysLibLoad(0, NCS_PALM_CREATOR_ID, &hLibHandle) == sysErrLibNotFound) {	/**[01]**/
		hLibHandle = 0;															/**[01]**/
	}																			/**[01]**/
#elif defined MACINTOSH		
	hLibHandle = 0;
#elif defined POSIX
	return(dlopen(pLibraryName, RTLD_NOW));
#else
	ERROR: Platform not supported.
#endif

	return hLibHandle;
}

void NCSDlClose( NCS_DLHANDLE hLibaryHandle )
{

#if defined WIN32						/**[01]**/
	FreeLibrary(hLibaryHandle);
#elif defined PALM					/**[01]**/
	if(hLibraryHandle) {				/**[01]**/
		SysLibRemove(hLibraryHandle);	/**[01]**/
	}									/**[01]**/
#elif defined MACINTOSH
#elif defined POSIX
	if(hLibaryHandle) {
		dlclose(hLibaryHandle);
	}	
#else
	ERROR: Platform not supported.
#endif

	return;
}

NCS_FUNCADDR NCSDlGetFuncAddress(NCS_DLHANDLE hLibaryHandle, char *pFunctionName)
{
	NCS_FUNCADDR hFuncAddress;	/**[01]**/

	if (!hLibaryHandle || !pFunctionName) {
		return NULL;
	}

#if defined WIN32												/**[01]**/
	hFuncAddress =												/**[01]**/
		(NCS_FUNCADDR)GetProcAddress((HMODULE) hLibaryHandle,	/**[01]**/
									 (LPCSTR) pFunctionName );	/**[01]**/
#elif defined PALM											/**[01]**/
	hFuncAddress = 0;											/**[01]**/
#elif defined MACINTOSH
	hFuncAddress = 0;
#elif defined POSIX
	return((NCS_FUNCADDR)dlsym(hLibaryHandle, pFunctionName));
#else
	ERROR: Platform not supported.
#endif

	return hFuncAddress;										/**[01]**/
}

BOOLEAN NCSDlFuncExists(char *pLibraryName, char *pFunctionName)
{
	BOOLEAN bSymExists = FALSE;

    NCS_DLHANDLE hLibHandle = NCSDlOpen( pLibraryName );

    if(hLibHandle) {
        if(NCSDlGetFuncAddress(hLibHandle, pFunctionName)) {
            bSymExists = TRUE;
        }
        NCSDlClose(hLibHandle);
    }

    return(bSymExists);
}

char *NCSDlError(void)
{
	char *pErrorMessage = NULL;

#if defined WIN32				/**[01]**/

	LPVOID lpMsgBuf;
	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, 
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL);
	pErrorMessage = (char *)lpMsgBuf;
#elif defined PALM			/**[01]**/
	pErrorMessage = "NCSDlError()";		/**[01]**/
#elif defined MACINTOSH
	pErrorMessage = "NCSDlError()";
#elif defined POSIX
	pErrorMessage = dlerror();
#else
	ERROR: Platform not supported.
#endif

	return pErrorMessage;
}
