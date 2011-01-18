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
** [01] 03Nov00	 ny	Merge WinCE/PALM SDK changes with Mac port changes
 *******************************************************/

#ifndef NCSDYNAMICLIB_H
#define NCSDYNAMICLIB_H

#ifndef NCSTYPES_H
#include "NCSTypes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32

#define NCS_DLEXT ".dll"
#define NCS_DLEXT_NO_DOT "dll"
#define NCS_FUNCADDR FARPROC
#define NCS_DLHANDLE void *

#elif defined PALM

#define NCS_DLEXT ".shl"
#define NCS_DLEXT_NO_DOT "shl"
typedef void (*NCS_FUNCADDR)(void);
#define NCS_DLHANDLE UInt16

#elif defined MACINTOSH

#define NCS_DLEXT ".dll"
#define NCS_DLEXT_NO_DOT "dll"
typedef void (*NCS_FUNCADDR)(void);
#define NCS_DLHANDLE void *

#elif defined POSIX

#ifdef HPUX
#define NCS_DLEXT ".sl"
#define NCS_DLEXT_NO_DOT "sl"
#else
#define NCS_DLEXT ".so"
#define NCS_DLEXT_NO_DOT "so"
#endif
typedef void (*NCS_FUNCADDR)(void);
#define NCS_DLHANDLE void *

#else

	ERROR: Platform not supported.

#endif	/* WIN32 */

NCS_DLHANDLE NCSDlOpen( char* pLibraryName );
void NCSDlClose( NCS_DLHANDLE NCSLibararyHandle );
NCS_FUNCADDR NCSDlGetFuncAddress(NCS_DLHANDLE NCSLibararyHandle, char *pFunctionName);
BOOLEAN NCSDlFuncExists(char *pLibraryName, char *pFunctionName);
char *NCSDlError(void);

#ifdef __cplusplus
}
#endif

#endif
