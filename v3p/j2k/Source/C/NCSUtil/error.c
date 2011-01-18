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
** FILE:   	NCSUtil\error.c
** CREATED:	Thu Feb 25 09:19:00 WST 1999
** AUTHOR: 	Simon Cope
** PURPOSE:	NCSError routines
** EDITS:
** [01] 06Apr00 mjs Added new var-args error functionality.
** [02] 12Oct00 sjc Check NCSError range	
** [03] 30Oct00  ny Merge CE/Palm modification in preparation for Macintosh port	
 *******************************************************/

#define _WIN32_WINNT 0x400
#define NCSERROR_DEFINE_GLOBALS
#ifdef PALM					/**[03]**/
#define ERROR_CHECK_LEVEL 2		/**[03]**/
#endif							/**[03]**/
#ifndef PALM				/**[03]**/
#include <stdio.h>
#endif							/**[03]**/
#include "NCSErrors.h"
#include "NCSThread.h"
#include "NCSUtil.h"

#if defined(_WIN32_WCE)
	#include "NCSUtil.h"
#endif

#define NCS_MAX_ERROR_LEN 2048

/*
** Structure to hold error info, stashed in tls
*/
typedef struct NCSErrorInfo {
	NCSError nError;
	char NCSErrorTextBuffer[NCS_MAX_ERROR_LEN];
} NCSErrorInfo;

/*
** Thread local storage key
*/
#ifdef MACINTOSH
static NCSErrorInfo g_LastErrorInfo;
#else
static NCSThreadLSKey nThreadErrorKey;
#endif

/*
** Thread Init. Only called internally.
*/
void NCSErrorInitThread()
{
#ifdef MACINTOSH
	g_LastErrorInfo.nError = NCS_SUCCESS;
	g_LastErrorInfo.NCSErrorTextBuffer[0] = '\0';
#else
	NCSThreadLSSetValue(nThreadErrorKey, (void *)NULL);
#endif
}

void NCSErrorFiniThread()
{
#ifdef MACINTOSH
	g_LastErrorInfo.nError = NCS_SUCCESS;
	g_LastErrorInfo.NCSErrorTextBuffer[0] = '\0';
#else
	NCSErrorInfo *pErrorInfo = (NCSErrorInfo *)NCSThreadLSGetValue(nThreadErrorKey);
	if (pErrorInfo) {
		NCSFree(pErrorInfo);
	}
#endif
}

/*
** Init. Should only be called on dll-load
*/
void NCSErrorInit(void)
{
#ifdef MACINTOSH
	g_LastErrorInfo.nError = NCS_SUCCESS;
	g_LastErrorInfo.NCSErrorTextBuffer[0] = '\0';
#else
	nThreadErrorKey = NCSThreadLSAlloc();
#endif

}

/*
** Finish. Should only be called on dll-unload
*/
void NCSErrorFini(void)
{
#ifdef MACINTOSH
#else
	// Clean up the main thread.
	NCSErrorFiniThread(); 
	NCSThreadLSFree(nThreadErrorKey);
#endif

}

/*
** Return the text for the given error enum value. Deprecated as of version 1.5
*/
const char *NCSGetErrorText(NCSError eError)
{
	return(NCSErrorTextArray[(eError >= NCS_MAX_ERROR_NUMBER) ? NCS_INVALID_ERROR_ENUM : eError/**[02]**/]);
}

/*
** Return the text for the given error enum value. To be used as of v1.5.
** also sets the variable in thread local storage to be retrieved by
** NCSGetLastErrorText() or NCSGetLastErrorTextMsgBox().
*/
const char * NCSFormatErrorText(NCSError nError, ...)
{
	va_list args;
	char *pFormat;
	
#ifdef MACINTOSH
	NCSErrorInfo *pErrorInfo = &g_LastErrorInfo;
#else
	// Get the current threads error info
	
	NCSErrorInfo *pErrorInfo = (NCSErrorInfo *)NCSThreadLSGetValue(nThreadErrorKey);

	// If there isnt one, create it (first time in this thread an error has been called).
	if (!pErrorInfo) {
		pErrorInfo = (NCSErrorInfo *)NCSMalloc(sizeof(NCSErrorInfo) , TRUE);

		NCSThreadLSSetValue(nThreadErrorKey, (void *)pErrorInfo);

	}
#endif

	pErrorInfo->nError = nError;
	
	// Check nError range [02]
	nError = (nError >= NCS_MAX_ERROR_NUMBER) ? NCS_INVALID_ERROR_ENUM : nError;

	pFormat = NCSErrorTextArrayEx[nError];
	va_start(args, nError);
	vsprintf(pErrorInfo->NCSErrorTextBuffer, pFormat, args);
	va_end(args);
	
	return(pErrorInfo->NCSErrorTextBuffer);

}

/*
** Return the last error number for this thread.
*/
NCSError NCSGetLastErrorNum(void)
{

#ifdef MACINTOSH
	NCSErrorInfo *pErrorInfo = &g_LastErrorInfo;
#else
	NCSErrorInfo *pErrorInfo = (NCSErrorInfo *)NCSThreadLSGetValue(nThreadErrorKey);
#endif

	if (pErrorInfo) {
		return pErrorInfo->nError;
	}
	else {
		return NCS_SUCCESS;
	}

}

/*
** Return the last error text for the given error number.
*/
const char * NCSGetLastErrorText(NCSError nError)
{

#ifdef MACINTOSH
	NCSErrorInfo *pErrorInfo = &g_LastErrorInfo;
#else
	NCSErrorInfo *pErrorInfo = (NCSErrorInfo *)NCSThreadLSGetValue(nThreadErrorKey);
#endif

	if (pErrorInfo && (nError == pErrorInfo->nError)) {
		return pErrorInfo->NCSErrorTextBuffer;
	}
	else {
		// Old fall back mode
		return NCSGetErrorText(nError);
	}
}

void NCSGetLastErrorTextMsgBox(NCSError nError, void *pWindow)
{	
#if defined (_WIN32_WCE) 	//Pocket PC 2002
	MessageBox((HWND)pWindow, OS_STRING(NCSGetLastErrorText(nError)), NCS_T("NCS Error"), MB_ICONERROR|MB_OK);
#elif defined(WIN32)									/**[03]**/
	MessageBox((HWND)pWindow, OS_STRING(NCSGetLastErrorText(nError)), NCS_T("NCS Error"), MB_ICONERROR|MB_OK|MB_SERVICE_NOTIFICATION);
#elif defined PALM											/**[03]**/
	ErrNonFatalDisplayIf(true, NCSGetLastErrorText(nError));	/**[03]**/
#elif defined MACINTOSH
	return;
#elif defined POSIX
	fprintf(stderr, NCSGetLastErrorText(nError));
#else
#ERROR : define NCSGetLastErrorTextMsgBox() for this platform
#endif
}

#ifdef PALM
NCSError NCSPalmGetNCSError(Err eErr)
{
	switch(eErr) {
		case errNone:
		default: return(NCS_SUCCESS); break;
		
		/* FileMgr Errors */
		case fileErrMemError: 			return(NCS_COULDNT_ALLOC_MEMORY); break;
		case fileErrInvalidParam: 		return(NCS_INVALID_PARAMETER); break;
		case fileErrCorruptFile: 		return(NCS_FILE_INVALID); break;
		case fileErrNotFound: 			return(NCS_FILE_NOT_FOUND); break;
		case fileErrTypeCreatorMismatch:return(NCS_FILEIO_ERROR); break;
		case fileErrReplaceError: 		return(NCS_FILE_NO_PERMISSIONS); break;
		case fileErrCreateError: 		return(NCS_FILE_NO_PERMISSIONS); break;
		case fileErrOpenError: 			return(NCS_FILE_OPEN_ERROR); break;
		case fileErrInUse: 				return(NCS_FILE_NO_PERMISSIONS); break;
		case fileErrReadOnly: 			return(NCS_FILE_NO_PERMISSIONS); break;
		case fileErrInvalidDescriptor: 	return(NCS_INVALID_PARAMETER); break;
		case fileErrCloseError: 		return(NCS_FILE_CLOSE_ERROR); break;
		case fileErrOutOfBounds: 		return(NCS_FILE_SEEK_ERROR); break;
		case fileErrPermissionDenied: 	return(NCS_FILE_NO_PERMISSIONS); break;
		case fileErrIOError: 			return(NCS_FILE_IO_ERROR); break;
		case fileErrEOF: 				return(NCS_FILE_EOF); break;
		case fileErrNotStream: 			return(NCS_INVALID_PARAMETER); break;
	}
}	
#endif
