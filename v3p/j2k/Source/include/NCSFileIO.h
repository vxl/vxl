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
** FILE:   	include\NCSFileIO.h
** CREATED:	Tue 09/14/1999
** AUTHOR: 	Simon Cope
** PURPOSE:	64bit file IO wrapper function header
** EDITS:
** [01] sjc 24Feb00 Added NCSGetFileVersion() call to return file version
** [02] sjc 30Apr00 Merged Mac SDK port
** [03]  ny 30Oct00 Merged WinCE/Palm SDK port
 *******************************************************/

#ifndef NCSFILEIO_H
#define NCSFILEIO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NCSTYPES_H
#include "NCSDefs.h"
#endif
#ifndef NCSMALLOC_H
#include "NCSMalloc.h"
#endif

#ifndef NCSERRORS_H
#include "NCSErrors.h"
#endif

#ifdef LINUX
#ifndef __USE_FILE_OFFSET64
#define __USE_FILE_OFFSET64
#endif
//#define __USE_LARGEFILE64
#endif

#ifdef WIN32

#if !defined(_WIN32_WCE)	//Pocket PC 2002
	#include <io.h>
	#include <fcntl.h>
#endif 

#elif defined PALM

#include <FileStream.h>

#elif defined _WIN32_WCE

#include <stdlib.h>

#elif defined MACINTOSH

#include <stdlib.h>
#include <fcntl.h>

#elif defined SOLARIS || defined HPUX || defined LINUX

#include <sys/types.h>
#include <unistd.h>
#include <strings.h>

#endif


#ifdef WIN32
#ifndef _WIN32_WCE

#include <sys/types.h>
#include <sys/stat.h>

#else	/* _WIN32_WCE */

//#if (_WIN32_WCE < 300)	//< Pocket PC 2002
#define NO_STDIO
//#endif

#endif	/* _WIN32_WCE */

#define NCS_FILE_SEP "\\"
#define NCS_FILE_SEP_T NCS_T("\\")

#elif defined MACINTOSH

#include <types.h>
#include <stat.h>

#define NCS_FILE_SEP "/"
#define NCS_FILE_SEP_T NCS_T("/")

#elif defined PALM

#include <PalmTypes.h>
#define NO_STDIO

#define NCS_FILE_SEP "/"
#define NCS_FILE_SEP_T NCS_T("/")

#elif defined SOLARIS || defined HPUX || defined LINUX

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define NCS_FILE_SEP "/"
#define NCS_FILE_SEP_T NCS_T("/")

#elif defined MACOSX
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define NCS_FILE_SEP "/"
#define NCS_FILE_SEP_T NCS_T("/")

#else	/* WIN32 */

#error - need to include types.h and stat.h for this platform

#endif	/* WIN32 */


#ifndef PALM
#include <stdio.h>
#endif	/* PALM */

#define NCS_FILE_READ			(1 << 0)
#define NCS_FILE_READ_WRITE		(1 << 1)
#define NCS_FILE_CREATE			(1 << 2)
#define NCS_FILE_CREATE_UNIQUE	(1 << 3)
#define NCS_FILE_APPEND			(1 << 4)
#define NCS_FILE_TEMPORARY		(1 << 5)
#define NCS_FILE_ASYNCIO		(1 << 6)

#ifdef WIN32

typedef	HANDLE					NCS_FILE_HANDLE;
#define NCS_NULL_FILE_HANDLE	NULL
typedef DWORD					NCS_FILE_ORIGIN; 
#define NCS_FILE_SEEK_START		FILE_BEGIN
#define NCS_FILE_SEEK_CURRENT	FILE_CURRENT
#define NCS_FILE_SEEK_END		FILE_END

#elif defined MACINTOSH

typedef INT16					NCS_FILE_HANDLE;
#define NCS_NULL_FILE_HANDLE	NULL

typedef enum {
	NCS_FILE_SEEK_START		= 0,
	NCS_FILE_SEEK_CURRENT	= 1,
	NCS_FILE_SEEK_END		= 2
} NCS_FILE_ORIGIN;

#elif defined PALM

typedef	HANDLE					NCS_FILE_HANDLE;
typedef DWORD					NCS_FILE_ORIGIN; 
#define NCS_FILE_SEEK_START		FILE_BEGIN
#define NCS_FILE_SEEK_CURRENT	FILE_CURRENT
#define NCS_FILE_SEEK_END		FILE_END

typedef struct NCS_FILE_HANDLE_STRUCT {
	LocalID 		dbID;		// Local DB ID
	DmOpenRef 		dbRef;		// DB Ref handle
	UINT16 			nRecordSize;// Size of records
	UINT16			nRecords;	// Nr of Records
	UINT32			nDBSize;	// Total DB Data size
	UINT32			iOffset;	// Current offset
	
	// Cached current record
	UINT16			iRecord;	// Index of current record
	MemHandle 		hRecord;	// Handle of current record
	MemPtr 			pData;		// Data in current record
} *NCS_FILE_HANDLE;

typedef enum {
	NCS_FILE_SEEK_START		= 0,
	NCS_FILE_SEEK_CURRENT	= 1,
	NCS_FILE_SEEK_END		= 2
} NCS_FILE_ORIGIN;

#else	/* PALM */

/* STDIO */
typedef	int						NCS_FILE_HANDLE;
#define NCS_NULL_FILE_HANDLE	-1
typedef int						NCS_FILE_ORIGIN;
#define NCS_FILE_SEEK_START		SEEK_SET
#define NCS_FILE_SEEK_CURRENT	SEEK_CUR
#define NCS_FILE_SEEK_END		SEEK_END

#endif	/* WIN32 */

INT64 NCSFileSeekNative(NCS_FILE_HANDLE handle, INT64 offset, NCS_FILE_ORIGIN origin);
INT64 NCSFileTellNative(NCS_FILE_HANDLE handle);

#if	defined(NO_STDIO)||defined(SOLARIS)||defined(IRIX)||defined(HPUX)||defined(LINUX)
#define NCSFileSeek			NCSFileSeekNative
#define NCSFileTell			NCSFileTellNative
#else	/* NO_STDIO||SOLARIS||IRIX */
INT64 NCSFileSeek(int handle, INT64 offset, int origin);
INT64 NCSFileTell(int handle);
#endif	/* NO_STDIO||SOLARIS||IRIX */

NCSError NCSFileOpen(const NCSTChar *szFilename, int iFlags, NCS_FILE_HANDLE *phFile);
NCSError NCSFileClose(NCS_FILE_HANDLE handle);
NCSError NCSFileRead(NCS_FILE_HANDLE handle, void* buffer, UINT32 count, UINT32* pRead);
NCSError NCSFileWrite(NCS_FILE_HANDLE handle, void* buffer, UINT32 count, UINT32* pWrote);
NCSError NCSFileTouch(NCS_FILE_HANDLE hFile);
NCSError NCSFileFlush(NCS_FILE_HANDLE hFile);
//
// These routines read/write the specified type from/to a file.  The type in the file is the byte order
// specifiec by the routine name.  The value will be swapped to/from the native CPU byte order as
// appropriate.
//
// eg, NCSFileReadUINT16_LSB() - will read a UINT16 (LSB format on disk) from the file, and swap it to 
// the native CPU's byte order.
//
NCSError NCSFileReadUINT8_MSB(NCS_FILE_HANDLE hFile, UINT8 *pBuffer);
NCSError NCSFileReadUINT8_LSB(NCS_FILE_HANDLE hFile, UINT8 *pBuffer);
NCSError NCSFileReadUINT16_MSB(NCS_FILE_HANDLE hFile, UINT16 *pBuffer);
NCSError NCSFileReadUINT16_LSB(NCS_FILE_HANDLE hFile, UINT16 *pBuffer);
NCSError NCSFileReadUINT32_MSB(NCS_FILE_HANDLE hFile, UINT32 *pBuffer);
NCSError NCSFileReadUINT32_LSB(NCS_FILE_HANDLE hFile, UINT32 *pBuffer);
NCSError NCSFileReadUINT64_MSB(NCS_FILE_HANDLE hFile, UINT64 *pBuffer);
NCSError NCSFileReadUINT64_LSB(NCS_FILE_HANDLE hFile, UINT64 *pBuffer);
NCSError NCSFileReadIEEE4_MSB(NCS_FILE_HANDLE hFile, IEEE4 *pBuffer);
NCSError NCSFileReadIEEE4_LSB(NCS_FILE_HANDLE hFile, IEEE4 *pBuffer);
NCSError NCSFileReadIEEE8_MSB(NCS_FILE_HANDLE hFile, IEEE8 *pBuffer);
NCSError NCSFileReadIEEE8_LSB(NCS_FILE_HANDLE hFile, IEEE8 *pBuffer);
NCSError NCSFileWriteUINT8_MSB(NCS_FILE_HANDLE hFile, UINT8 nValue);
NCSError NCSFileWriteUINT8_LSB(NCS_FILE_HANDLE hFile, UINT8 nValue);
NCSError NCSFileWriteUINT16_MSB(NCS_FILE_HANDLE hFile, UINT16 nValue);
NCSError NCSFileWriteUINT16_LSB(NCS_FILE_HANDLE hFile, UINT16 nValue);
NCSError NCSFileWriteUINT32_MSB(NCS_FILE_HANDLE hFile, UINT32 nValue);
NCSError NCSFileWriteUINT32_LSB(NCS_FILE_HANDLE hFile, UINT32 nValue);
NCSError NCSFileWriteUINT64_MSB(NCS_FILE_HANDLE hFile, UINT64 nValue);
NCSError NCSFileWriteUINT64_LSB(NCS_FILE_HANDLE hFile, UINT64 nValue);
NCSError NCSFileWriteIEEE4_MSB(NCS_FILE_HANDLE hFile, IEEE4 fValue);
NCSError NCSFileWriteIEEE4_LSB(NCS_FILE_HANDLE hFile, IEEE4 fValue);
NCSError NCSFileWriteIEEE8_MSB(NCS_FILE_HANDLE hFile, IEEE8 dValue);
NCSError NCSFileWriteIEEE8_LSB(NCS_FILE_HANDLE hFile, IEEE8 dValue);
/*
** Currently macros to keep dll size down.
*/
#ifdef NO_STDIO

NCS_FILE_HANDLE NCSFileOpenStdIO(const char *filename, int oflag, int pmode);
int NCSFileCloseStdIO(NCS_FILE_HANDLE handle);
int NCSFileReadStdIO(NCS_FILE_HANDLE handle, void* buffer, int count);
int NCSFileWriteStdIO(NCS_FILE_HANDLE handle, void* buffer, int count);

#else	/* NO_STDIO */
#ifdef WIN32

#define NCSFileOpenStdIO(filename, oflag, pmode)	\
				_open((const char*)filename, (int)oflag, (int)pmode)
#define NCSFileCloseStdIO(handle)	\
				_close(handle)
#define NCSFileReadStdIO(handle, buffer, count)	\
				_read(handle, (void*)buffer, (unsigned int)count)
#define NCSFileWriteStdIO(handle, buffer, count)	\
				_write(handle, (const void*)buffer, (unsigned int)count)

#else	/* WIN32 */

#define NCSFileOpenStdIO(filename, oflag, pmode) open((const char*)filename, (int)oflag, (int)pmode)
#define NCSFileCloseStdIO(handle)				 close(handle)
#define NCSFileReadStdIO(handle, buffer, count)	 read(handle, (void*)buffer, (unsigned int)count)
#define NCSFileWriteStdIO(handle, buffer, count) write(handle, (const void*)buffer, (unsigned int)count)

#endif	/* WIN32 */
#endif 	/* NO_STDIO */

INT32 NCSFileModTime(char *pFilename);
INT64 NCSFileSizeBytes(NCSTChar *pFilename);
INT64 NCSFreeDiskSpaceBytes(char *pDirName, INT64 *pTotal);
char *NCSGetTempDirectory(void);
char *NCSGetTempFileName(char *pDir, char *pPrefix, char *pExt);
BOOLEAN NCSRenameFile(char *pSrc, char *pDst);
BOOLEAN NCSDeleteFile(char *pFileName);
BOOLEAN NCSFileGetVersion(char *pFileName, UINT16 *pMajor, UINT16 *pMinor, UINT16 *pRevision, UINT16 *pBuild);/**[01]**/
int NCSVersionCompare(UINT16 nMajor1, UINT16 nMinor1, UINT16 nRevision1, UINT16 nBuild1, UINT16 nMajor2, UINT16 nMinor2, UINT16 nRevision2, UINT16 nBuild2);
BOOLEAN NCSIsLocalFile(char *string);
BOOLEAN NCSPathExists(char *pPath, BOOLEAN *pIsDirectory);

NCSError NCSMakeDir( const char *pDirName, BOOLEAN bCreateTree );
NCSError NCSRemoveDir( const char *pDirName);

#ifdef __cplusplus
}
#endif

#endif /* NCSFILE_H */
