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
** FILE:   	fileio_decompress.c
** CREATED:	1998
** AUTHOR: 	SNS
** PURPOSE:	Decompress File IO routines
** EDITS:
** [01] sns	10sep98	Created file
** [02] sjc 26Oct00 Added support for user-define IO callbacks.
** [03] rar	17-Jan-01 Mac port changes
 *******************************************************/

#include "NCSEcw.h" 
#include "NCSFileIO.h"

#ifdef WIN32
#include <winbase.h>
#endif

/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*
**	Decompress File IO routines.
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/

//[02] - begin
static NCSError (NCS_CALL *pFileOpenUserCB)(char *szFileName, void **ppClientData) = NULL;
static NCSError (NCS_CALL *pFileCloseUserCB)(void *pClientData) = NULL;	
static NCSError (NCS_CALL *pFileReadUserCB)(void *pClientData, void *pBuffer, UINT32 nLength) = NULL;
static NCSError (NCS_CALL *pFileSeekUserCB)(void *pClientData, UINT64 nOffset) = NULL;
static NCSError (NCS_CALL *pFileTellUserCB)(void *pClientData, UINT64 *pOffset) = NULL;

/*
**	NCSecwSetIOCallbacks()
**
** Allows user to register callback functions to be used for ECW File IO.
** This can be used to read from memory etc., instead of disk.
*/
#ifdef NCSJPC_ECW_SUPPORT
NCSError NCSecwSetIOCallbacks_ECW(NCSError (NCS_CALL *pOpenCB)(char *szFileName, void **ppClientData),
							  NCSError (NCS_CALL *pCloseCB)(void *pClientData),
							  NCSError (NCS_CALL *pReadCB)(void *pClientData, void *pBuffer, UINT32 nLength),
							  NCSError (NCS_CALL *pSeekCB)(void *pClientData, UINT64 nOffset),
							  NCSError (NCS_CALL *pTellCB)(void *pClientData, UINT64 *pOffset))
#else
NCSError NCSecwSetIOCallbacks(NCSError (NCS_CALL *pOpenCB)(char *szFileName, void **ppClientData),
							  NCSError (NCS_CALL *pCloseCB)(void *pClientData),
							  NCSError (NCS_CALL *pReadCB)(void *pClientData, void *pBuffer, UINT32 nLength),
							  NCSError (NCS_CALL *pSeekCB)(void *pClientData, UINT64 nOffset),
							  NCSError (NCS_CALL *pTellCB)(void *pClientData, UINT64 *pOffset))
#endif
{
	if(pOpenCB && pCloseCB && pReadCB && pSeekCB && pTellCB) {
		pFileOpenUserCB = pOpenCB;
		pFileCloseUserCB = pCloseCB;
		pFileReadUserCB = pReadCB;
		pFileSeekUserCB = pSeekCB;
		pFileTellUserCB = pTellCB;

		return(NCS_SUCCESS);
	} else {
		return(NCS_INVALID_PARAMETER);
	}
}
//[02] - end

/*
**	EcwFileOpenForRead()
**	Open file for reading. Returns TRUE if error, otherwise fills in pFile handle,
**	which *might* (in Windows anyway) be NULL and still valid - so you have to
**	track open files using another variable. File is always opened in shared, read, binary, mode.
*/

BOOLEAN EcwFileOpenForRead(char *szFilename, ECWFILE *pFile)
{
	if(pFileOpenUserCB) {	//[02]
		return((*pFileOpenUserCB)(szFilename, &(pFile->pClientData)) == NCS_SUCCESS ? FALSE : TRUE);
	}

	if(NCSFileOpen(OS_STRING(szFilename), NCS_FILE_READ, &(pFile->hFile)) == NCS_SUCCESS) {
		return(FALSE);
	} else {
		return(TRUE);
	}
}

/*
**	EcwFileClose()	- closes a ECW file handle that had been opened for reading
*/
BOOLEAN EcwFileClose(ECWFILE hFile)								// Closes file
{
	if(pFileCloseUserCB) {	//[02]
		return((*pFileCloseUserCB)(hFile.pClientData) == NCS_SUCCESS ? FALSE : TRUE);
	}

	if(NCSFileClose(hFile.hFile) == NCS_SUCCESS) {
		return(FALSE);
	} else {
		return(TRUE);
	}
}


/*
**	EcwFileRead()
**	Reads nLength bytes into pBuffer from the File handle. Returns FALSE
**	if all went well, or TRUE if something went wrong.
*/
BOOLEAN EcwFileRead(ECWFILE hFile, void *pBuffer, UINT32 nLength)
{
	if(pFileReadUserCB) {	//[02]
		return((*pFileReadUserCB)(hFile.pClientData, pBuffer, nLength) == NCS_SUCCESS ? FALSE : TRUE);
	}

	if(NCSFileRead(hFile.hFile, pBuffer, nLength, &nLength) == NCS_SUCCESS) {
		return(FALSE);
	} else {
		return(TRUE);
	}
}

/*
**	EcwFileSetPos()
**	Returns the current seek position as a UINT64 value, and FALSE
**	if all went well, or TRUE if something went wrong.  Windows has
**	a particularly nasty way of handling with long file offsets.
*/
BOOLEAN EcwFileSetPos(ECWFILE hFile, UINT64 nOffset)
{
	if(pFileSeekUserCB) {	//[02]
		return((*pFileSeekUserCB)(hFile.pClientData, nOffset) == NCS_SUCCESS ? FALSE : TRUE);
	}

	if(NCSFileSeekNative(hFile.hFile, nOffset, NCS_FILE_SEEK_START) == (INT64)nOffset) {
		return(FALSE);
	} else {
		return(TRUE);
	}
}


/*
**	EcwFileGetPos()
**	Returns the current seek position as a UINT64 value, and FALSE
**	if all went well, or TRUE if something went wrong.  Windows has
**	a particularly nasty way of handling with long file offsets.
*/
BOOLEAN EcwFileGetPos(ECWFILE hFile, UINT64 *pOffset)
{
	if(pFileTellUserCB) {	//[02]
		return((*pFileTellUserCB)(hFile.pClientData, pOffset) == NCS_SUCCESS ? FALSE : TRUE);
	}
	*pOffset = NCSFileTellNative(hFile.hFile);
	return(FALSE);
}



/*	*	*	*	*	*	*	*	*	*	*	*	*	*	*
**	ERW HEADER MACHINE INDEPENDANT IO routines.
*	*	*	*	*	*	*	*	*	*	*	*	*	*	*/

/*
**	Read/write functions. Data is always written in machine independant format.
**	All functions return non-zero if an error.
**	These are quite slow, so only use them for reading/writing small numbers
**	of header bytes.
**
** NOTE:  DUE TO A COMPLETE COCKUP THE INTS IN THE HEADER ARE STORED ON DISC AS
**        MSB WHILST FLOATS, THE BLOCK TABLE AND THE REST OF THE DATA IS LSB!
*/


BOOLEAN EcwFileReadUint8(ECWFILE hFile, UINT8 *sym)
{
	if(pFileReadUserCB) {
		return(	EcwFileRead(hFile, sym, sizeof(*sym)));
	} else {
		return( NCSFileReadUINT8_MSB(hFile.hFile, sym) == NCS_SUCCESS ? FALSE : TRUE );
	}
}

BOOLEAN EcwFileReadUint16(ECWFILE hFile, UINT16 *sym16)
{
	if(pFileReadUserCB) {
		UINT16 sym;

		BOOLEAN bRval = EcwFileRead(hFile, &sym, sizeof(sym));
#ifdef NCSBO_LSBFIRST
		NCSByteSwapRange16(&sym, &sym, 1);
#endif // NCSBO_LSBFIRST
		*sym16 = sym;
		return(bRval);
	} else {
		return( NCSFileReadUINT16_MSB(hFile.hFile, sym16) == NCS_SUCCESS ? FALSE : TRUE );
	}
}

BOOLEAN EcwFileReadUint32(ECWFILE hFile, UINT32 *sym32)
{
	if(pFileReadUserCB) {
		UINT32 sym;

		BOOLEAN bRval = EcwFileRead(hFile, &sym, sizeof(sym));
#ifdef NCSBO_LSBFIRST
		NCSByteSwapRange32(&sym, &sym, 1);
#endif // NCSBO_LSBFIRST
		*sym32 = sym;
		return(bRval);
	} else {
		return( NCSFileReadUINT32_MSB(hFile.hFile, sym32) == NCS_SUCCESS ? FALSE : TRUE );
	}
}


BOOLEAN EcwFileReadIeee8(ECWFILE hFile, IEEE8 *symd8)
{
	if(pFileReadUserCB) {
		IEEE8 sym;

		BOOLEAN bRval = EcwFileRead(hFile, &sym, sizeof(sym));
#ifdef NCSBO_MSBFIRST
		NCSByteSwapRange64((UINT64 *)&sym, (UINT64 *)&sym, 1);
#endif // NCSBO_MSBFIRST
		*symd8 = sym;
		return(bRval);
	} else {
		return( NCSFileReadIEEE8_LSB(hFile.hFile, symd8) == NCS_SUCCESS ? FALSE : TRUE );
	}
}


// read a machine independant 4 byte value from a string
UINT32 sread_int32(UINT8 *p_s)
{
#ifdef NCSBO_MSBFIRST
#ifdef NCS_NO_UNALIGNED_ACCESS
	UINT32 n;
	memcpy(&n, p_s, sizeof(UINT32));
	return(n);
#else
	return(*((UINT32*)p_s));
#endif
#else
	#if !defined(_WIN32_WCE)
		return(NCSByteSwap32(*((UINT32*)p_s)));
	#else
		UINT8 *ptr = p_s;
		UINT8 p1 = *ptr++;
		UINT8 p2 = *ptr++;
		UINT8 p3 = *ptr++;
		UINT8 p4 = *ptr;

		UINT32 ps = (p4 << 24) | (p3 << 16) | (p2 << 8) | p1;
		return(NCSByteSwap32(ps));
	#endif
#endif
}

// read a machine independant 2 byte value from a string
UINT16 sread_int16(UINT8 *p_s)
{
#ifdef NCSBO_MSBFIRST
#ifdef NCS_NO_UNALIGNED_ACCESS
	UINT16 n;
	memcpy(&n, p_s, sizeof(UINT16));
	return(n);
#else
	return(*((UINT16*)p_s));
#endif
#else
	return(NCSByteSwap16(*((UINT16*)p_s)));
#endif
}

// Read IEEE8 in machine independant way
void sread_ieee8(IEEE8 *sym, UINT8 *p_s)
{
	memcpy(sym, p_s, sizeof(IEEE8));
#ifdef NCSBO_MSBFIRST
	NCSByteSwapRange64((UINT64 *)sym, (UINT64 *)sym, 1);
#endif
}
