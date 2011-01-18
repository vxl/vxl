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
** FILE:   	NCSUtil\main.c
** CREATED:	Fri May 07 16:00:00 WST 1999
** AUTHOR: 	Simon Cope
** PURPOSE:	DllMain()
** EDITS:
 *******************************************************/

#include "NCSThread.h"
#include "NCSUtil.h"

#if defined(WIN32)&&!defined(_LIB)&&!defined(LIBECWJ2)
BOOLEAN WINAPI _CRT_INIT(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved);

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  dwReason, 
                       LPVOID lpReserved
					 )
{
	hModule;// Keep compiler happy
	lpReserved;// Keep compiler happy
    switch (dwReason) {
		case DLL_PROCESS_ATTACH:
				NCSUtilInit();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
				NCSErrorFiniThread();
				NCSThreadFreeInfo(NCSThreadGetCurrent());
			break;
		case DLL_PROCESS_DETACH:
				NCSUtilFini();
			break;
    }
    return(TRUE);
}
#elif defined (MACINTOSH)

/*int __initialize()
{
	NCSUtilInit();
	return (TRUE);
}

int __terminate()
{
	NCSThreadFreeInfo(NCSThreadGetCurrent());
	NCSUtilFini();
	return (TRUE);
}
*/
#else
//ERROR : Shared library entry/exit point needs to be defined
#endif
