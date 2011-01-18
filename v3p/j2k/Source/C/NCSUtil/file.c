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
** FILE:   	NCSUtil\file.c
** CREATED:	Tue 09/14/1999
** AUTHOR: 	Simon Cope
** PURPOSE:	64bit file IO wrapper functions
** EDITS:
** [01] sjc 24Feb00 Added NCSGetFileVersion() call to return file version
** [02]  ny 30Oct00 Merge CE/Palm modification in preparation for Macintosh port
** [03]  ny  14Nov00 Mac port modification
** [04] mjs	30Apr01 Changed NCSFileTell -> NCSFileTellNative && NCSFileTellStdIO to NCSFileTell
**					NCSFileSeek -> NCSFileSeekNative && NCSFileSeekStdIO -> NCSFileSeek
**					to fix backwards compatibility issues.
** [05] sjc 12Nov01 Added versioncompare func
** [06] tfl 12Dec05 Added <time.h> to get rid of warning
 *******************************************************/

#include "NCSUtil.h"

#if !defined( _WIN32_WCE )
#	if !defined( MACOSX ) && !defined( POSIX )
#		include <direct.h>
#	endif
#include <errno.h>
#endif

#ifdef WIN32
#include "shlwapi.h"
#endif // WIN32

#ifdef MACOSX
#	include "CoreServices/CoreServices.h"
#endif

#ifndef _WIN32_WCE
#include <time.h> /* [06] */
#endif

NCSError NCSFileTouch(NCS_FILE_HANDLE hFile) 
{
	if( hFile ) {
#ifdef WIN32
		FILETIME ft;
		SYSTEMTIME st;
		BOOL f;

		GetSystemTime(&st);              // gets current time
		SystemTimeToFileTime(&st, &ft);  // converts to file time format
		f = SetFileTime(hFile,              // sets last-write time for file
			(LPFILETIME) NULL, (LPFILETIME) NULL, &ft);
		if( !f ) { //failed
			return NCS_FILE_IO_ERROR;
		}
#endif
	}
	return NCS_SUCCESS;
}

NCSError NCSMakeDir( const char *pDirName, BOOLEAN bCreateTree )
{
#if !defined(WIN32)||(defined(WIN32)&&!defined(_WIN32_WCE))
	NCSError eError = NCS_SUCCESS;

	unsigned int i=0;

	if( pDirName ) {
		if( bCreateTree ) { // Create the whole path
			eError = NCSMakeDir( pDirName, FALSE );
			if( eError == NCS_FILE_MKDIR_PATH_NOT_FOUND ) {
				char szDirName[MAX_PATH];
				char cLastChar = 'a';

				// copy Dir Name as we are going to change it (slightly)
				strcpy( szDirName, pDirName );

				// Remove trailing \ or /
				cLastChar = szDirName[ strlen( szDirName )-1 ];
				while( cLastChar == '\\' || cLastChar == '/' ) {
					szDirName[ strlen( szDirName )-1 ] = '\0';
					cLastChar = szDirName[ strlen( szDirName )-1 ];
				}
				// Create the parent directories
				for( i=0; i < strlen( szDirName ); i++ ) {
					if( (i > 2) && ((szDirName[i] == '\\') || (szDirName[i] == '/')) ) {
						// create dir
						szDirName[i] = '\0';
						eError = NCSMakeDir( szDirName, FALSE );
						if( (eError == NCS_FILE_MKDIR_PATH_NOT_FOUND) ||
							(eError == NCS_UNKNOWN_ERROR ) )
						{
							return eError;
						}
						szDirName[i] = '\\';
					}
				}
				// Create the final directory
				eError = NCSMakeDir( szDirName, FALSE );
			}
			return eError;
		} else {
#ifdef WIN32
			if( (_mkdir(pDirName) == 0) ) {
#else
			if( (mkdir( pDirName, S_IRWXU) == 0) ) {
#endif
				//directory created
				return NCS_SUCCESS;
			} else if( errno == EEXIST ) {
				//directory already exists
				return NCS_FILE_MKDIR_EXISTS;
			} else if( errno == ENOENT ) {
				//path does not exist
				return NCS_FILE_MKDIR_PATH_NOT_FOUND;
			} else {
				// Unknown error
				return NCS_UNKNOWN_ERROR;
			}
		}
	} else {
		return NCS_INVALID_ARGUMENTS;
	}
#else
	return NCS_UNKNOWN_ERROR;
#endif
}

NCSError NCSRemoveDir( const char *pDirName)
{
#ifdef LINUX
	if(rmdir(pDirName) == 0) {
#elif defined(_WIN32_WCE)
	if(RemoveDirectory(OS_STRING(pDirName)) != 0) {
#else
	if(rmdir(pDirName) == 0) {
#endif
		return(NCS_SUCCESS);
	} else {
		return(NCS_FILEIO_ERROR);
	}
}

/* 
** Seek to given position
*/
INT64 NCSFileSeekNative(NCS_FILE_HANDLE hFile,
				  INT64 nOffset,
				  NCS_FILE_ORIGIN origin)
{
#if defined WIN32
	LARGE_INTEGER li;

	li.QuadPart = nOffset;

	li.LowPart = SetFilePointer(hFile,						// file handle
						li.LowPart,							// low 32 bits
						&li.HighPart,						// high 32 bits
						origin);						// relative to start of file

	if( li.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR )
		return( -1 );
	else
		return( li.QuadPart );
#elif defined PALM
	switch(origin) {
		case NCS_FILE_SEEK_START:
				hFile->iOffset = nOffset;
			break;
		case NCS_FILE_SEEK_CURRENT:
				hFile->iOffset += nOffset;
			break;
		case NCS_FILE_SEEK_END:
				hFile->iOffset = hFile->nDBSize + nOffset - 1;
	}
	return(hFile->iOffset);
#elif defined MACINTOSH
	UINT32	myOffset;
	int		result; 
			
	myOffset = U32SetU(nOffset);
			
	if(!myOffset) result = 0;
			
	result = SetFPos((short)hFile, fsFromStart, myOffset);
			
	if(!result) return myOffset;
	else return (0);
#elif defined SOLARIS
	return (INT64) llseek(hFile, (offset_t)nOffset, origin);
#elif defined LINUX
	return (INT64) lseek64(hFile, (__off64_t)nOffset, origin);
#elif defined HPUX
	return (INT64) lseek64(hFile, (off64_t)nOffset, origin);
#elif defined IRIX
	return (INT64) lseek64(hFile, (off64_t)nOffset, origin);
#elif defined MACOSX
    return (INT64) lseek(hFile, nOffset, origin);
#endif
}

/*
** Get current file position
*/
INT64 NCSFileTellNative(NCS_FILE_HANDLE hFile)
{
#if defined WIN32
	LARGE_INTEGER li;

	li.QuadPart = 0;

	li.LowPart = SetFilePointer(hFile,						// file handle
						li.LowPart,							// low 32 bits
						&li.HighPart,						// high 32 bits
						FILE_CURRENT);						// relative to present position

	if( li.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR )
		return(-1);
	else
		return(li.QuadPart);

#elif defined MACINTOSH

	long  myOffset;
	OSErr	result;
		
	result = GetFPos((short)hFile, &myOffset);
			
	if (result==noErr)
		return ((INT64)myOffset);
	else
		return ((INT64)-1);

#elif defined PALM
	return(hFile->iOffset);
#elif defined SOLARIS
	INT64 nOffset = llseek(hFile, (offset_t)0, SEEK_CUR);
	return(nOffset);
#elif defined IRIX
	return((INT64)telli64(hFile));
#elif defined LINUX
	return (INT64) lseek64(hFile, (__off64_t)0, SEEK_CUR);
#elif defined HPUX
	return (INT64) lseek64(hFile, (off64_t)0, SEEK_CUR);
#elif defined MACOSX
	return (INT64) lseek(hFile, (long)0, SEEK_CUR);
#else
#error ERROR: EcwFileGetPos() routine is not defined for this platform
#endif
}

#if !(defined(NO_STDIO)||defined(IRIX)||defined(POSIX))

INT64 NCSFileSeek(int hFile,
				  	   INT64 nOffset,
				  	   int origin)
{
#ifdef WIN32

	return((INT64)_lseeki64(hFile, (__int64)nOffset, origin));

#elif defined MACINTOSH
	
	long fpos, size;

	if (GetEOF((short) hFile, &size) != noErr)
		return EOF;
	(void) GetFPos((short) hFile, &fpos);

	switch (origin) {
	case SEEK_CUR:
		if (nOffset + fpos > size)
			SetEOF((short) hFile, nOffset + fpos);
		if (SetFPos((short) hFile, fsFromMark, nOffset) != noErr)
			return EOF;
		break;
	case SEEK_END:
		if (nOffset > 0)
			SetEOF((short) hFile, nOffset + size);
		if (SetFPos((short) hFile, fsFromStart, nOffset + size) != noErr)
			return EOF;
		break;
	case SEEK_SET:
		if (nOffset > size)
			SetEOF((short) hFile, nOffset);
		if (SetFPos((short) hFile, fsFromStart, nOffset) != noErr)
			return EOF;
		break;
	}

	return (INT64)(GetFPos((short) hFile, &fpos) == noErr ? fpos : EOF);
	
#else	/* WIN32 */

#error: NCSFileSeek()
#endif	/* WIN32 */
}

/*
** Get current file position
*/
INT64 NCSFileTell(int hFile)
{
#ifdef WIN32

	return((INT64)_telli64(hFile));

#elif defined MACINTOSH
/*   Simon's code
	UINT32		fileLocation;
	fgetpos(hFile,(unsigned long *)&fileLocation);
	return((UINT64)fileLocation);
*/ 
	long fpos, size;

	if (GetEOF((short) hFile, &size) != noErr)
		return EOF;
	return (INT64)size;
	//return (INT64) GetFPos((short) hFile, &fpos);

#else	/* WIN32 */
#error	NCSFileTell()
#endif	/* WIN32 */
}

#elif defined(_WIN32_WCE)

#undef NCSFileSeek
INT64 NCSFileSeek(int hFile,
				  	   INT64 nOffset,
				  	   int origin)
{
	return(-1);
}

#undef NCSFileTell
INT64 NCSFileTell(int hFile)
{
	return(0);
}

#endif	/* !(NO_STDIO||SOLARIS||IRIX) */

NCSError NCSFileOpen(const NCSTChar *szFilename, int iFlags, NCS_FILE_HANDLE *phFile)
{
#ifdef WIN32
	DWORD dwMode = GENERIC_READ;
	DWORD dwCreate = OPEN_EXISTING;

	if(iFlags & NCS_FILE_READ) dwMode = GENERIC_READ;
	if(iFlags & NCS_FILE_READ_WRITE) dwMode = GENERIC_READ|GENERIC_WRITE;
	if(iFlags & NCS_FILE_CREATE) dwCreate = CREATE_ALWAYS;
	if(iFlags & NCS_FILE_CREATE_UNIQUE) dwCreate = CREATE_NEW;
	if(iFlags & NCS_FILE_APPEND) dwCreate = OPEN_ALWAYS;

	*phFile = CreateFile(szFilename,			        // file name
						 dwMode,						// Generic read mode 
						 FILE_SHARE_READ,				// Let anyone access and share the file
						 NULL,							// No security info (so can't be inherited by child process)
						 dwCreate,						// File must exist to be opened
						 FILE_FLAG_RANDOM_ACCESS,		// Going to be doing lots of random access
						 NULL);							// And no template file for attributes
	if( *phFile == INVALID_HANDLE_VALUE ) {
		return( NCS_FILE_OPEN_FAILED );
	} else {
		return( NCS_SUCCESS );
	}

#elif defined MACINTOSH
#if __POWERPC__
	
	int i,length, result;
	Str255		pascalString;
	FSSpec		fileSpec;
		//	We have a C string, we need a PASCAL string.
	length = strlen(szFilename) + 1;
	for(i = 1; i < length; ++i)
		pascalString[i] = szFilename[i - 1];
	pascalString[0] = strlen(szFilename);
			
	//	Create a File Specification Record, then create a File
	result = FSMakeFSSpec(0,0,pascalString,&fileSpec);	// return is meaningless, since the only possible error doesn't effect processing in this case
			
	switch(result) {
		case noErr:
				// we could dRes pFile here, but we are the only user
				result =FSpOpenDF(&fileSpec, fsRdPerm, (short *)phFile);
				if(result) return NCS_FILE_OPEN_FAILED;
				else return NCS_SUCCESS;
			break;
		default:
			    return NCS_SUCCESS;
		    break;
	}

#else	/* __POWERPC__ */

	int i,length, result;
	Str255		pascalString;
	//	We have a C string, we need a PASCAL string.
	length = strlen(szFilename) + 1;
	for(i = 1; i < length; ++i)
		pascalString[i] = szFilename[i - 1];
	pascalString[0] = strlen(szFilename);
		
	result =FSOpen(pascalString, 0, (short *)phFile);
	if(result) return TRUE;
	else return FALSE;

#endif	/* __POWERPC__ */
#elif defined PALM

	NCS_FILE_HANDLE hFile;
	Err eErr;
	UInt32 nMode = 0;
	
	if(hFile = (NCS_FILE_HANDLE)NCSMalloc(sizeof(NCS_FILE_HANDLE_STRUCT), TRUE)) {
		hFile->dbID = DmFindDatabase(0, szFilename);
		
		if(hFile->dbID) {
	   		Char nameP[dmDBNameLength];
	   		UInt16 attributes;
	   		UInt16 version;
	   		UInt32 crDate;
	   		UInt32 modDate;
	   		UInt32 bckUpDate;
	   		UInt32 modNum;
	   		LocalID appInfoID;
	   		LocalID sortInfoID;
	   		UInt32 type;
	   		UInt32 creator;
					
	   		DmDatabaseInfo(0, hFile->dbID, nameP,
	   					   &attributes, 
	   					   &version,
	   					   &crDate,
	   					   &modDate,
	   					   &bckUpDate,
	   					   &modNum,
	   					   &appInfoID,
	   					   &sortInfoID,
	   					   &type,
	   					   &creator);
	   					   
	   		if(creator == NCS_PALM_CREATOR_ID) {
	   			if(hFile->dbRef = DmOpenDatabase(0, hFile->dbID, dmModeReadOnly|dmModeShowSecret)) {
	   				UInt32 nRecords;
	   				UInt32 nTotalBytes;
	   				UInt32 nDataBytes;
	   				
	   				eErr = DmDatabaseSize(0, hFile->dbID, &nRecords, &nTotalBytes, &nDataBytes);
	   				
	   				if(eErr == errNone) {
	   					MemHandle hRecord;
	   					
	   					hFile->nRecords = nRecords;
	   					hFile->nDBSize = nDataBytes;
#ifdef NOTDEF	   					
	   					if(hRecord = DmGetRecord(hFile->dbRef, 0)) {
	   						MemPtr pData;
	   						
							if(pData = MemHandleLock(hRecord)) {
								hFile->nRecordSize = ((UINT16*)pData)[0];
							
								MemHandleUnlock(hRecord);
							}
							DmReleaseRecord(hFile->dbRef, 0, false);
						}
#endif
						*phFile = hFile;
						return(NCS_SUCCESS);
	   				}
	   				DmCloseDatabase(hFile->dbRef);
	   				return(NCSPalmGetNCSError(eErr));
	   			}
	   		}
		}
	} else {
		return(NCS_COULDNT_ALLOC_MEMORY);
	}
/*	
	if(iFlags & NCS_FILE_READ) nMode = fileModeReadOnly|fileModeAnyTypeCreator;
	if(iFlags & NCS_FILE_READ_WRITE) nMode = fileModeUpdate|fileModeAnyTypeCreator;
	if(iFlags & NCS_FILE_CREATE) nMode = fileModeReadWrite|fileModeAnyTypeCreator;
	if(iFlags & NCS_FILE_CREATE_UNIQUE) nMode = fileModeReadWrite|fileModeDontOverwrite|fileModeAnyTypeCreator;
	if(iFlags & NCS_FILE_APPEND) nMode = fileModeAppend|fileModeAnyTypeCreator;
	
	*phFile = FileOpen(0, (char*)szFilename, 0, 0, nMode, &eErr);
	
	return(NCSPalmGetNCSError(eErr));			   
*/					
#elif defined(POSIX)

	int flags = O_RDONLY;

	if(iFlags & NCS_FILE_READ) flags = O_RDONLY;
	if(iFlags & NCS_FILE_READ_WRITE) flags = O_RDWR;
	if(iFlags & NCS_FILE_CREATE) flags |= O_CREAT;
	if(iFlags & NCS_FILE_CREATE_UNIQUE) flags |= O_CREAT|O_EXCL;
	if(iFlags & NCS_FILE_APPEND) flags |= O_APPEND;

#if defined SOLARIS || (defined(HPUX) && !defined(__LP64__))
	// Enable 64bit!
	flags |= O_LARGEFILE;
#endif

#ifdef HPUX
	*phFile = open64((const char*)CHAR_STRING(szFilename), (int)flags);

#ifdef NOTDEF
	if (*phFile < 0) {
		fprintf(stderr, "Error opening file : %ld\n", errno); 
		if (errno == EOVERFLOW) {
			fprintf(stderr, "The named file is a regular file and the size "
                          "of the file cannot be represented correctly in an object of "
                          "size off_t.");
		}
	}
#endif

#else
	*phFile = open((const char*)CHAR_STRING(szFilename), (int)flags, S_IRUSR|S_IWUSR);
#endif
	if(*phFile != -1) {
		return(NCS_SUCCESS);
	} else {
		return(NCS_FILE_OPEN_FAILED);
	}

#else	/* SOLARIS||IRIX */
#error ERROR  EcwFileCreate() routine is not defined for this platform
#endif	/* WIN32 */
}

NCSError NCSFileClose(NCS_FILE_HANDLE hFile)
{
#ifdef WIN32

	if( CloseHandle(  hFile ) )
		return(NCS_FILE_CLOSE_ERROR);
	else
		return(NCS_SUCCESS);

#elif defined MACINTOSH

	FSClose((short)hFile);
	hFile = NULL;
	return NCS_SUCCESS;
			
#elif defined PALM

	if(hFile && hFile->dbRef) {
		if(hFile->hRecord) {
			if(hFile->pData) {
				MemHandleUnlock(hFile->hRecord);
			}
			DmReleaseRecord(hFile->dbRef, hFile->iRecord, false);
		}
		DmCloseDatabase(hFile->dbRef);
	}
	NCSFree((void*)hFile);
	
#elif defined(POSIX)

	if(close(hFile) == 0) {
		return(NCS_SUCCESS);
	} else {
		return(NCS_FILE_CLOSE_ERROR);
	}

#else	/* SOLARIS||IRIX */
#error ERROR: EcwFileOpen() routine is not defined for this platform
#endif	/* WIN32 */
}

NCSError NCSFileRead(NCS_FILE_HANDLE hFile, void *pBuffer, UINT32 nLength, UINT32* pRead)
{
#ifdef WIN32
	
	BOOLEAN	bError;
	DWORD	nBytesRead;

	bError = (BOOLEAN)ReadFile(  hFile,								// handle of file to read
								pBuffer,							// pointer to buffer that receives data
								nLength,							// number of bytes to read
								&nBytesRead,							// pointer to number of bytes read
								NULL);								// pointer to structure for data
	if(pRead) {
		*pRead = (UINT32)nBytesRead;
	}
	if( bError != 0 && nBytesRead == nLength )
		return( NCS_SUCCESS );
	else
		return( NCS_FILE_IO_ERROR );

#elif defined MACINTOSH
	
	int		iOReturn;
	long	myLength;
			
	UINT32  theOffset;
	BOOLEAN	result;
			
	result = GetFPos((short)hFile, (long *)&theOffset);
			
	myLength = nLength;
	iOReturn = FSRead((short)hFile, (long *)&nLength,pBuffer);
	switch (iOReturn) {
		case noErr:
				return NCS_SUCCESS;
			break;
		case eofErr:
				return ((myLength == nLength)?NCS_FILE_IO_ERROR:NCS_SUCCESS);
			break;
		case rfNumErr:
			    return (NCS_FILE_IO_ERROR);
		    break;
		default:
				return NCS_FILE_IO_ERROR;
			break;
	}

#elif defined PALM

	Err eErr;
	INT16 iStartRecord = hFile->iOffset / hFile->nRecordSize + 1;
	INT16 iEndRecord = (hFile->iOffset + nLength) / hFile->nRecordSize + 1;
	INT16 iRecord = hFile->iOffset / hFile->nRecordSize + 1;
	INT64 nRead = 0;
	INT32 nToRead = nLength;
	
	for(iRecord = iStartRecord; iRecord <= iEndRecord; iRecord++) {
		if(hFile->iRecord != iRecord) {
			if(hFile->hRecord) {
				if(hFile->pData) {
					MemHandleUnlock(hFile->hRecord);
					hFile->pData = NULL;
				}
				DmReleaseRecord(hFile->dbRef, hFile->iRecord, false);
				hFile->iRecord = 0;
				hFile->hRecord = NULL;
			}
			hFile->iRecord = iRecord;
			if(hFile->hRecord = DmGetRecord(hFile->dbRef, hFile->iRecord)) {
				hFile->pData = MemHandleLock(hFile->hRecord);
				if(hFile->pData == NULL) {
					DmReleaseRecord(hFile->dbRef, hFile->iRecord, false);
				}
			}
			if(!hFile->hRecord || !hFile->pData) {
				return(NCS_FILE_IO_ERROR);
			}
		}
		if(hFile->pData) {
			INT32 nThisRead = MIN(nToRead, hFile->nRecordSize - (hFile->iOffset % hFile->nRecordSize));
				 
			memcpy((UINT8*)pBuffer + nRead, 
				   (void*)((UINT8*)hFile->pData + hFile->iOffset % hFile->nRecordSize), 
				   nThisRead);
						   
			hFile->iOffset += nThisRead;
			nToRead -= nThisRead;
			nRead += nThisRead;
		}
	}
	if(nRead == nLength) {
		*pRead = nRead;
		return(NCS_SUCCESS);
	}
/*		
	nRead = FileRead(hFile, pBuffer, (Int32)(nLength & 0x7fffffff) , 1, &eErr);
	
	*pRead = (UINT32)nRead;
	
	return(NCSPalmGetNCSError(eErr));
*/
#elif defined(POSIX)

	int nThisRead = read(hFile, (void*)pBuffer, (unsigned int)nLength);
	if(pRead) {
		*pRead = nThisRead;
	}
	if(nThisRead == nLength) {
		return(NCS_SUCCESS);
	} else {
		return(NCS_FILE_IO_ERROR);
	}

#else	/* SOLARIS||IRIX */
#error ERROR: EcwReadFile() routine is not defined for this platform
#endif	/* WIN32 */
}

NCSError NCSFileWrite(NCS_FILE_HANDLE hFile, void* pBuffer, UINT32 nLength, UINT32* pWritten)
{
#ifdef WIN32

	BOOLEAN	bError;
	DWORD	nBytesWritten;

	bError = (BOOLEAN)WriteFile(hFile,								// handle of file to read
								pBuffer,							// pointer to buffer that receives data
								nLength,							// number of bytes to read
								&nBytesWritten,							// pointer to number of bytes read
								NULL);								// pointer to structure for data
	if(pWritten) {
		*pWritten = (UINT32)nBytesWritten;
	}
	if( bError != 0 && nBytesWritten == nLength )
		return( NCS_SUCCESS );
	else
		return( NCS_FILE_IO_ERROR );

#elif defined PALM

/*
	Err eErr;
	Int32 nWritten = 0;
	
	nWritten = FileWrite(hFile, pBuffer, (Int32)(nLength & 0x7fffffff) , 1, &eErr);
	FileFlush(hFile);
	*pWritten = (UINT32)nWritten;
	
	return(NCSPalmGetNCSError(eErr));
*/

#elif defined(POSIX)
	int nThisWrite = write(hFile, (void*)pBuffer, (unsigned int)nLength);
	if(pWritten) {
		*pWritten = nThisWrite;
	}

	if(nThisWrite == nLength) {
		return(NCS_SUCCESS);
	} else {
		return(NCS_FILE_IO_ERROR);
	}

#elif defined MACINTOSH

	long count = (long)nLength;
	if(pWritten) {
		*pWritten = 0;
	}
	return (FSRead((short) hFile, (long*) &count, (char*) pBuffer) == noErr ?
	    NCS_SUCCESS : NCS_FILE_IO_ERROR);


#else	/* WIN32 */
#error NCSFileWrite()
#endif	/* WIN32 */
}

NCSError NCSFileFlush(NCS_FILE_HANDLE hFile)
{
#ifdef WIN32
	FlushFileBuffers(hFile);
#else
	fsync(hFile);
#endif
	return(NCS_SUCCESS);
}

INT32 NCSFileModTime(char *pFilename)
{
    struct stat statbuf;

    if (stat(pFilename, &statbuf) != 0) {
        return(-1); 
    }
    return((INT32)statbuf.st_mtime);
}

/*
** Get the size of the given file
*/
INT64 NCSFileSizeBytes(NCSTChar *pFilename)
{
#ifdef WIN32
#ifdef _WIN32_WCE
	DWORD dwLow;
	DWORD dwHigh;
	HANDLE hFile;

	if(hFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL)) {
		dwLow = GetFileSize(hFile, &dwHigh);
		
		if(dwLow == 0xffffffff && GetLastError() != NO_ERROR) {
			CloseHandle(hFile);
			return(-1);
		} else {
			CloseHandle(hFile);
			return((UINT64)dwLow | ((UINT64)dwHigh << 32));
		}
	} else {
		return(-1);
	}

#else	/* _WIN32_WCE */
	struct _stati64 statbuf;
#ifdef NCS_BUILD_UNICODE
#define _tstati64 _wstati64
#else
#define _tstati64 _stati64
#endif

    if(_tstati64(pFilename, &statbuf) != 0) {
		DWORD dwLow;
		DWORD dwHigh;
		HANDLE hFile;

#ifdef NCS_BUILD_UNICODE
		if(hFile = CreateFileW(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL)) {
#else
		if(hFile = CreateFileA(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL)) {
#endif
			dwLow = GetFileSize(hFile, &dwHigh);
			
			if(dwLow == 0xffffffff && GetLastError() != NO_ERROR) {
				CloseHandle(hFile);
				return(-1);
			} else {
				CloseHandle(hFile);
				return((UINT64)dwLow | ((UINT64)dwHigh << 32));
			}
		} else {
			return(-1);
		}
    }
    return((INT64)statbuf.st_size);
#undef _tstati64
#endif

#elif defined POSIX

#ifdef _LARGEFILE64_SOURCE
    struct stat64 statbuf;

    if (stat64(CHAR_STRING(pFilename), &statbuf) != 0) {
        return(-1); 
    }
    return((INT64)statbuf.st_size);
#else
    struct stat statbuf;

    if (stat(CHAR_STRING(pFilename), &statbuf) != 0) {
        return(-1); 
    }
    return((INT64)statbuf.st_size);
#endif


#elif defined IRIX

	struct stat64 statbuf;

    if (stat(CHAR_STRING(pFilename), &statbuf) != 0) {
        return(-1);
    }
	return((INT64)statbuf.st_size);

#elif defined PALM
	
	NCS_FILE_HANDLE hFile;
	
	if(NCSFileOpen(pFilename, NCS_FILE_READ, &hFile) == NCS_SUCCESS) {
		INT32 nFileSize = hFile->nDBSize;
		
		NCSFileClose(hFile);
		return((INT64)nFileSize);
	}
	return(-1);

#else	/* WIN32 */
#error 	NCSFileSizeBytes();
#endif	/* WIN32 */
}

/*
** Get the size of the given file
*/
INT64 NCSFreeDiskSpaceBytes(char *pDirName, INT64 *pTotal)
{
#ifdef WIN32
	NCS_FUNCADDR pGetDiskFreeSpaceEx;
	//WINBASEAPI BOOL (WINAPI *pGetDiskFreeSpaceEx)(LPCSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

	char szPath[MAX_PATH];

	strcpy(szPath, pDirName);
	if(szPath[0] == '\\' && szPath[strlen(szPath) - 1] != '\\') {
		// UNC dir must have trailing backslash
		strcat(szPath, "\\");
	}

	pGetDiskFreeSpaceEx = (NCS_FUNCADDR)GetProcAddress( GetModuleHandle(NCS_T("kernel32.dll")),
							"GetDiskFreeSpaceExA");

	if(pGetDiskFreeSpaceEx) {
		ULARGE_INTEGER i64FreeBytesToCaller;
		ULARGE_INTEGER i64TotalBytes;
		ULARGE_INTEGER i64FreeBytes;

		if(((BOOL (WINAPI *)(LPCSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER))pGetDiskFreeSpaceEx)(szPath,
					(PULARGE_INTEGER)&i64FreeBytesToCaller,
					(PULARGE_INTEGER)&i64TotalBytes,
					(PULARGE_INTEGER)&i64FreeBytes)) {
			
			if(pTotal) {
				*pTotal = (INT64)i64TotalBytes.QuadPart;
			}
			return((INT64)i64FreeBytesToCaller.QuadPart);
		} else {
			return(-1);
		}
	} else {
#if !defined(_WIN32_WCE)
		DWORD dwSectPerClust;
		DWORD dwBytesPerSect;
		DWORD dwFreeClusters;
		DWORD dwTotalClusters;

		if(GetDiskFreeSpace(OS_STRING(szPath), 
							&dwSectPerClust, 
							&dwBytesPerSect,
							&dwFreeClusters, 
							&dwTotalClusters)) {
			if(pTotal) {
				*pTotal = (INT64)dwBytesPerSect * (INT64)dwSectPerClust * (INT64)dwTotalClusters;
			}
			return((INT64)dwBytesPerSect * (INT64)dwSectPerClust * (INT64)dwFreeClusters);
		} else {
			return(-1);
		}
#elif defined(_WIN32_WCE)
		ULARGE_INTEGER nFreeBytesToCaller;
		ULARGE_INTEGER nTotalBytes;
		ULARGE_INTEGER nTotalFreeBytes;

		if (GetDiskFreeSpaceEx(OS_STRING(szPath), &nFreeBytesToCaller, &nTotalBytes, &nTotalFreeBytes)) {
			if(pTotal) {
				*pTotal = (INT64) nFreeBytesToCaller.QuadPart;
			}
			return((INT64)nFreeBytesToCaller.QuadPart);
		} else {
			return (-1);
		}
#endif
	}
#else // WIN32
	return(-1);
#endif
}

/*
** Get a temporary file name
**
** Can pass in Dir to use instead of %TEMP%
** Can pass in prefix to use instead of NCS
** Can pass in file extension to use instead of .tmp
*/
char *NCSGetTempFileName(char *pDir,
						 char *pPrefix,
						 char *pExt)
{
	char *pTmpName = NULL;

#if defined PALM

	return(NULL);

#else	/* MACINTOSH */
	
    char buf[MAX_PATH];

    if(pDir == (char *)NULL || (pDir && strlen(pDir) == 0)) {
		pDir = NCSGetTempDirectory();
    } else {
		pDir = NCSStrDup(pDir);
	}
	if(pExt == NULL) {
		pExt = ".tmp";
	}
	if(pPrefix == NULL) {
		pPrefix = "NCS";
	}

#ifdef WIN32
	{
		int i = 0;
#ifndef _WIN32_WCE
		srand( (unsigned)time( NULL ) );
#endif	
		while(i < 65535) {
			sprintf(buf, "%s\\%s%lx%lx%s", pDir, pPrefix, rand(), rand(), pExt);
#if defined(_WIN32_WCE)||defined(NCS_MINDEP_BUILD)
			if(NCSFileSizeBytes(buf) < 0) {
#else
			if(PathFileExistsA(buf) == FALSE) {
#endif
				pTmpName = NCSStrDup(buf);
				break;
			} 
			i++;
		}
	}
	NCSFree((void*)pDir);
	
#elif defined( MACOSX )
    sprintf(buf, "%sXXXXXX", pPrefix ? pPrefix : "NCS");

    pTmpName = NCSMalloc(strlen(pDir) + strlen(buf) + strlen(pExt) + 3, FALSE);
    sprintf(pTmpName, "%s/%s", pDir, buf);

    mktemp(pTmpName);

    NCSFree((void*)pDir);
    strcat(pTmpName, pExt);	// FIXME: Is this really going to be unique?

#else

	sprintf(buf, "%sXXXXXX", pPrefix ? pPrefix : "NCS");

	pTmpName = NCSMalloc(strlen(pDir) + strlen(buf) + strlen(pExt) + 3, FALSE);
	sprintf(pTmpName, "%s/%s", pDir, buf);

    mktemp(pTmpName);

	NCSFree((void*)pDir);
	strcat(pTmpName, pExt);	// FIXME: Is this really going to be unique?

#endif
#endif

    return(pTmpName);
}

/*
** Get name of temp directory
**
** WIN32:
** Default: %TEMP% env variable
** 9x:		C:\Windows\Temp
** NT:		C:\Temp
** CE:		\Temp
*/
char *NCSGetTempDirectory(void)
{
#ifdef _WIN32_WCE

	return(NCSStrDup("\\Temp"));

#elif defined WIN32

	NCSTChar winbuf[MAX_PATH];

	if(GetTempPath((DWORD)MAX_PATH, winbuf) == 0) {
		if(GetSystemDirectory(winbuf, MAX_PATH)) {
			if(NCSGetPlatform() == NCS_WINDOWS_NT) {
				/* eg, c:\Temp */
				winbuf[3] = '\0';
				NCSTCat(winbuf, NCS_T("Temp"));
			} else {
				/* eg, c:\Windows\Temp */
				NCSTCat(winbuf, NCS_T("Temp"));
			}
		}
	}
	if((winbuf[0] != '\0') && (winbuf[NCSTLen(winbuf) - 1] == '\\')) {
		winbuf[NCSTLen(winbuf) - 1] = '\0';
	}
	return(NCSStrDup(CHAR_STRING(winbuf)));

#elif defined PALM

	return(NCSStrDup(""));

#elif defined MACOSX

        FSRef tempRef;
        UInt8 szPath[1024] = "";
        
        
        if( FSFindFolder( kUserDomain, kTemporaryFolderType, kDontCreateFolder, &tempRef ) == noErr ) {
            if( FSRefMakePath( &tempRef, szPath, 1024 ) == noErr ) {
            }
        }
         

        return( NCSStrDup(szPath) );

#elif defined POSIX

	return(NCSStrDup("/tmp"));

#else	/* PALM */
	char *szTmpDir = getenv("TMP");
	if (szTmpDir != NULL)
		return NCSStrDup(szTmpDir);
	else return NCSStrDup("/tmp");

#endif
}

/*
** Get File Version information [01]
**
*/
BOOLEAN NCSFileGetVersion(char *pFileName,
						  UINT16 *pMajor,
						  UINT16 *pMinor,
						  UINT16 *pRevision,
						  UINT16 *pBuild)
{
	BOOLEAN bRVal = FALSE;

	if(pFileName) {
#if defined(_WIN32_WCE)||defined(NCS_MINDEP_BUILD)

	return(FALSE);

#elif defined WIN32
		DWORD dwVISize;
		DWORD dwZero;

		dwVISize = GetFileVersionInfoSize(OS_STRING(pFileName), &dwZero);

		if(dwVISize != 0) {
			LPVOID lpData = NCSMalloc(dwVISize, TRUE);

			if(lpData) {
				if(GetFileVersionInfo(OS_STRING(pFileName),
									  0,
									  dwVISize,
									  lpData)) {
					VS_FIXEDFILEINFO *pVI = (VS_FIXEDFILEINFO*)NULL;
					UINT dwSize;

					if(VerQueryValue(lpData, 
									 NCS_T("\\"), 
									 (LPVOID*)&pVI,
									 &dwSize) && pVI) {
				
						if(pMajor) {
							*pMajor = (UINT16)(pVI->dwFileVersionMS >> 16);
						}
						if(pMinor) {
							*pMinor = (UINT16)(pVI->dwFileVersionMS & 0xffff);
						}
						if(pRevision) {
							*pRevision = (UINT16)(pVI->dwFileVersionLS >> 16);
						}
						if(pBuild) {
							*pBuild = (UINT16)(pVI->dwFileVersionLS & 0xffff);
						}
						bRVal = TRUE;
					}
				}
				NCSFree(lpData);
			}
		}
#elif defined(PALM)||defined(MACINTOSH)||defined(POSIX)

	bRVal = FALSE;

#else
FIXME: NCSGetFileVersion()
#endif
	}
	return(bRVal);
}

int NCSVersionCompare(UINT16 nMajor1, UINT16 nMinor1, UINT16 nRevision1, UINT16 nBuild1,	//[05]
					  UINT16 nMajor2, UINT16 nMinor2, UINT16 nRevision2, UINT16 nBuild2)
{
	if(nMajor1 > nMajor2) {
		return(1);		
	} else if(nMajor1 == nMajor2) {
		if(nMinor1 > nMinor2) {
			return(1);
		} else if(nMinor1 == nMinor2) {
			if(nRevision1 > nRevision2) {
				return(1);				
			} else if(nRevision1 == nRevision2) {
				if(nBuild1 > nBuild2) {
					return(1);
				} else if(nBuild1 == nBuild2) {
					return(0);
				}
				
			}
			
		}
		
	}
	return(-1);
}

BOOLEAN NCSRenameFile(char *pSrc, char *pDst)
{
#ifdef _WIN32_WCE
	return(MoveFile(OS_STRING(pSrc), OS_STRING(pDst)));
#else
	return((BOOLEAN)(rename(pSrc, pDst) == 0));	
#endif
}

BOOLEAN NCSDeleteFile(char *pFileName)
{
	if(pFileName) {
#ifdef WIN32
	
		if(DeleteFile(OS_STRING(pFileName))) {
			return(TRUE);
		} 
#ifndef _WIN32_WCE
		else {
			// check if the filename is wildcard
			WIN32_FIND_DATA FindFileData;
			HANDLE hFindFile;
			
			NCSTChar szPath[MAX_PATH];
			int i=0;

			NCSTCpy( szPath, OS_STRING(pFileName) );
			for( i = (int)NCSTLen( szPath )-1; i > 0; i-- ) {
				if( szPath[i] == '\\' ) {
					szPath[i+1] = '\0';
					break;
				}
			}
			if( i == 0 ) return FALSE;

			hFindFile = FindFirstFile(OS_STRING(pFileName), &FindFileData );
			// Erase all the files matching the wildcard
			if( hFindFile != INVALID_HANDLE_VALUE ) {
				do {
					if( !(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) { // check if not a directory
						//Erase the file
						NCSTChar szFileName[MAX_PATH];
						NCSTCpy( szFileName, szPath );
						NCSTCat( szFileName, FindFileData.cFileName );

						DeleteFile( szFileName );
					}
				} while( FindNextFile( hFindFile, &FindFileData) );

				FindClose( hFindFile );

				return TRUE;
			}

			return FALSE;
		}
#endif // ifndef _WIN32_WCE

#elif defined MACINTOSH

		Str255		pascalString;
		int 			i, length;
		OSErr		result;
		
		//	We have a C string, we need a PASCAL string.
		length = strlen(pFileName) + 1;
		for(i = 1; i < length; ++i)
			pascalString[i] = pFileName[i - 1];
		pascalString[0] = strlen(pFileName);
		
		result = HDelete( 0,  0, pascalString );
		if( result =! noErr ) return(FALSE);
		
		return(TRUE);

#elif defined PALM

		return((BOOLEAN)(FileDelete(0, pFileName) == errNone ? TRUE : FALSE));

#elif defined POSIX
		if(unlink(pFileName) == 0) {
			return(TRUE);
		}
#else	/* MACINTOSH */
	
		if(rmfile(pFileName) == 0) {
			return(TRUE);
		}

#endif	/* WIN32 */
	}
	return(FALSE);
}

NCSError NCSFileReadUINT8_MSB(NCS_FILE_HANDLE hFile, UINT8 *pBuffer)
{
	UINT32 nSize = sizeof(*pBuffer);
	return(NCSFileRead(hFile, pBuffer, nSize, &nSize));
}

NCSError NCSFileReadUINT8_LSB(NCS_FILE_HANDLE hFile, UINT8 *pBuffer)
{
	UINT32 nSize = sizeof(*pBuffer);
	return(NCSFileRead(hFile, pBuffer, nSize, &nSize));
}

NCSError NCSFileReadUINT16_MSB(NCS_FILE_HANDLE hFile, UINT16 *pBuffer)
{
	UINT32 nSize = sizeof(*pBuffer);
	NCSError eError = NCSFileRead(hFile, pBuffer, nSize, &nSize);
#ifdef NCSBO_LSBFIRST
	*pBuffer = NCSByteSwap16(*pBuffer);
#endif
	return(eError);
}

NCSError NCSFileReadUINT16_LSB(NCS_FILE_HANDLE hFile, UINT16 *pBuffer)
{
	UINT32 nSize = sizeof(*pBuffer);
	NCSError eError = NCSFileRead(hFile, pBuffer, nSize, &nSize);
#ifdef NCSBO_MSBFIRST
	*pBuffer = NCSByteSwap16(*pBuffer);
#endif
	return(eError);
}

NCSError NCSFileReadUINT32_MSB(NCS_FILE_HANDLE hFile, UINT32 *pBuffer)
{
	UINT32 nSize = sizeof(*pBuffer);
	NCSError eError = NCSFileRead(hFile, pBuffer, nSize, &nSize);
#ifdef NCSBO_LSBFIRST
	*pBuffer = NCSByteSwap32(*pBuffer);
#endif
	return(eError);
}

NCSError NCSFileReadUINT32_LSB(NCS_FILE_HANDLE hFile, UINT32 *pBuffer)
{
	UINT32 nSize = sizeof(*pBuffer);
	NCSError eError = NCSFileRead(hFile, pBuffer, nSize, &nSize);
#ifdef NCSBO_MSBFIRST
	*pBuffer = NCSByteSwap32(*pBuffer);
#endif
	return(eError);
}

NCSError NCSFileReadUINT64_MSB(NCS_FILE_HANDLE hFile, UINT64 *pBuffer)
{
	UINT32 nSize = sizeof(*pBuffer);
	NCSError eError = NCSFileRead(hFile, pBuffer, nSize, &nSize);
#ifdef NCSBO_LSBFIRST
	*pBuffer = NCSByteSwap64(*pBuffer);
#endif
	return(eError);
}

NCSError NCSFileReadUINT64_LSB(NCS_FILE_HANDLE hFile, UINT64 *pBuffer)
{
	UINT32 nSize = sizeof(*pBuffer);
	NCSError eError = NCSFileRead(hFile, pBuffer, nSize, &nSize);
#ifdef NCSBO_MSBFIRST
	*pBuffer = NCSByteSwap64(*pBuffer);
#endif
	return(eError);
}

NCSError NCSFileReadIEEE4_MSB(NCS_FILE_HANDLE hFile, IEEE4 *pBuffer)
{
	UINT32 nSize = sizeof(*pBuffer);
	NCSError eError = NCSFileRead(hFile, pBuffer, nSize, &nSize);
#ifdef NCSBO_LSBFIRST
	NCSByteSwapRange32((UINT32*)pBuffer, (UINT32*)pBuffer, 1);
#endif
	return(eError);
}

NCSError NCSFileReadIEEE4_LSB(NCS_FILE_HANDLE hFile, IEEE4 *pBuffer)
{
	UINT32 nSize = sizeof(*pBuffer);
	NCSError eError = NCSFileRead(hFile, pBuffer, nSize, &nSize);
#ifdef NCSBO_MSBFIRST
	NCSByteSwapRange32((UINT32*)pBuffer, (UINT32*)pBuffer, 1);
#endif
	return(eError);
}

NCSError NCSFileReadIEEE8_MSB(NCS_FILE_HANDLE hFile, IEEE8 *pBuffer)
{
	UINT32 nSize = sizeof(*pBuffer);
	NCSError eError = NCSFileRead(hFile, pBuffer, nSize, &nSize);
#ifdef NCSBO_LSBFIRST
	NCSByteSwapRange64((UINT64*)pBuffer, (UINT64*)pBuffer, 1);
#endif
	return(eError);
}

NCSError NCSFileReadIEEE8_LSB(NCS_FILE_HANDLE hFile, IEEE8 *pBuffer)
{
	UINT32 nSize = sizeof(*pBuffer);
	NCSError eError = NCSFileRead(hFile, pBuffer, nSize, &nSize);
#ifdef NCSBO_MSBFIRST
	NCSByteSwapRange64((UINT64*)pBuffer, (UINT64*)pBuffer, 1);
#endif
	return(eError);
}

NCSError NCSFileWriteUINT8_MSB(NCS_FILE_HANDLE hFile, UINT8 nValue)
{
	UINT32 nSize = sizeof(nValue);
	return(NCSFileWrite(hFile, &nValue, nSize, &nSize));
}

NCSError NCSFileWriteUINT8_LSB(NCS_FILE_HANDLE hFile, UINT8 nValue)
{
	UINT32 nSize = sizeof(nValue);
	return(NCSFileWrite(hFile, &nValue, nSize, &nSize));
}

NCSError NCSFileWriteUINT16_MSB(NCS_FILE_HANDLE hFile, UINT16 nValue)
{
	UINT32 nSize = sizeof(nValue);
#ifdef NCSBO_LSBFIRST
	nValue = NCSByteSwap16(nValue);
#endif
	return(NCSFileWrite(hFile, &nValue, nSize, &nSize));
}

NCSError NCSFileWriteUINT16_LSB(NCS_FILE_HANDLE hFile, UINT16 nValue)
{
	UINT32 nSize = sizeof(nValue);
#ifdef NCSBO_MSBFIRST
	nValue = NCSByteSwap16(nValue);
#endif
	return(NCSFileWrite(hFile, &nValue, nSize, &nSize));
}

NCSError NCSFileWriteUINT32_MSB(NCS_FILE_HANDLE hFile, UINT32 nValue)
{
	UINT32 nSize = sizeof(nValue);
#ifdef NCSBO_LSBFIRST
	nValue = NCSByteSwap32(nValue);
#endif
	return(NCSFileWrite(hFile, &nValue, nSize, &nSize));
}

NCSError NCSFileWriteUINT32_LSB(NCS_FILE_HANDLE hFile, UINT32 nValue)
{
	UINT32 nSize = sizeof(nValue);
#ifdef NCSBO_MSBFIRST
	nValue = NCSByteSwap32(nValue);
#endif
	return(NCSFileWrite(hFile, &nValue, nSize, &nSize));
}

NCSError NCSFileWriteUINT64_MSB(NCS_FILE_HANDLE hFile, UINT64 nValue)
{
	UINT32 nSize = sizeof(nValue);
#ifdef NCSBO_LSBFIRST
	nValue = NCSByteSwap64(nValue);
#endif
	return(NCSFileWrite(hFile, &nValue, nSize, &nSize));
}

NCSError NCSFileWriteUINT64_LSB(NCS_FILE_HANDLE hFile, UINT64 nValue)
{
	UINT32 nSize = sizeof(nValue);
#ifdef NCSBO_MSBFIRST
	nValue = NCSByteSwap64(nValue);
#endif
	return(NCSFileWrite(hFile, &nValue, nSize, &nSize));
}

NCSError NCSFileWriteIEEE4_MSB(NCS_FILE_HANDLE hFile, IEEE4 fValue)
{
	UINT32 nSize = sizeof(fValue);
#ifdef NCSBO_LSBFIRST
	NCSByteSwapRange32((UINT32*)&fValue, (UINT32*)&fValue, 1);
#endif
	return(NCSFileWrite(hFile, &fValue, nSize, &nSize));
}

NCSError NCSFileWriteIEEE4_LSB(NCS_FILE_HANDLE hFile, IEEE4 fValue)
{
	UINT32 nSize = sizeof(fValue);
#ifdef NCSBO_MSBFIRST
	NCSByteSwapRange32((UINT32*)&fValue, (UINT32*)&fValue, 1);
#endif
	return(NCSFileWrite(hFile, &fValue, nSize, &nSize));
}

NCSError NCSFileWriteIEEE8_MSB(NCS_FILE_HANDLE hFile, IEEE8 dValue)
{
	UINT32 nSize = sizeof(dValue);
#ifdef NCSBO_LSBFIRST
	NCSByteSwapRange64((UINT64*)&dValue, (UINT64*)&dValue, 1);
#endif
	return(NCSFileWrite(hFile, &dValue, nSize, &nSize));
}

NCSError NCSFileWriteIEEE8_LSB(NCS_FILE_HANDLE hFile, IEEE8 dValue)
{
	UINT32 nSize = sizeof(dValue);
#ifdef NCSBO_MSBFIRST
	NCSByteSwapRange64((UINT64*)&dValue, (UINT64*)&dValue, 1);
#endif
	return(NCSFileWrite(hFile, &dValue, nSize, &nSize));
}
