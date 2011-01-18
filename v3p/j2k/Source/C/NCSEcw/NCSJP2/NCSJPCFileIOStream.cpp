/********************************************************
** Copyright 2002 Earth Resource Mapping Ltd.
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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCFileIOStream.cpp $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCFileIOStream class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCFileIOStream.h"
#ifdef NCSJPC_ECW_SUPPORT
#include "NCSEcw.h"
#endif

#define NCSJPC_USE_IO_BUFFER

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
NCSError (NCS_CALL *CNCSJPCFileIOStream::sm_pOpenACB)(char *szFileName, void **ppClientData) = NULL;
NCSError (NCS_CALL *CNCSJPCFileIOStream::sm_pOpenWCB)(wchar_t *szFileName, void **ppClientData) = NULL;
NCSError (NCS_CALL *CNCSJPCFileIOStream::sm_pCloseCB)(void *pClientData) = NULL;
NCSError (NCS_CALL *CNCSJPCFileIOStream::sm_pReadCB)(void *pClientData, void *pBuffer, UINT32 nLength) = NULL;
NCSError (NCS_CALL *CNCSJPCFileIOStream::sm_pSeekCB)(void *pClientData, UINT64 nOffset) = NULL;
NCSError (NCS_CALL *CNCSJPCFileIOStream::sm_pTellCB)(void *pClientData, UINT64 *pOffset) = NULL;
 
UINT32 CNCSJPCFileIOStream::sm_nMaxIOCache = 1024;

// Constructor
CNCSJPCFileIOStream::CNCSJPCFileIOStream()
{
	// Initialise the members
	m_hFile = NCS_NULL_FILE_HANDLE;
	m_pIOCache = NULL;
	m_nIOWriteCache = 0;
	m_iIOReadCache = 0;

	NCSecwGetConfig(NCSCFG_JP2_FILEIO_CACHE_MAXMEM, &sm_nMaxIOCache);
	m_nMaxIOCache = sm_nMaxIOCache;
}

// Destructor
CNCSJPCFileIOStream::~CNCSJPCFileIOStream()
{
	// Close the file, Close() will clean up.
	Close();
}


#ifdef NCS_BUILD_UNICODE
// Open the stream on the specified full path name.
CNCSError CNCSJPCFileIOStream::Open(wchar_t*pFileName, bool bWrite)
{
	*(CNCSError*)this = NCSFileOpen(pFileName, bWrite ? NCS_FILE_READ_WRITE : NCS_FILE_READ, &m_hFile);
	
	if(GetErrorNumber() == NCS_SUCCESS) {
		*(CNCSError*)this = CNCSJPCIOStream::Open(pFileName, bWrite);
		m_pIOCache = (UINT8*)NCSMalloc(m_nMaxIOCache, FALSE);
		m_nIOWriteCache = 0;
		m_iIOReadCache = m_nMaxIOCache;
		m_nFileSize = NCSFileSizeBytes(m_pName);
	}
	return(*(CNCSError*)this);
}
#endif
// Open the stream on the specified full path name.
CNCSError CNCSJPCFileIOStream::Open(char *pFileName, bool bWrite)
{
	*(CNCSError*)this = NCSFileOpen(OS_STRING(pFileName), bWrite ? (NCS_FILE_READ_WRITE|NCS_FILE_CREATE) : NCS_FILE_READ, &m_hFile);
	
	if(GetErrorNumber() == NCS_SUCCESS) {
		*(CNCSError*)this = CNCSJPCIOStream::Open(pFileName, bWrite);
		m_pIOCache = (UINT8*)NCSMalloc(m_nMaxIOCache, FALSE);
		m_nIOWriteCache = 0;
		m_iIOReadCache = m_nMaxIOCache;
		if(bWrite) {
			m_nFileSize = 0;
		} else {
			m_nFileSize = NCSFileSizeBytes(m_pName);
		}
	}
	return(*(CNCSError*)this);
}

// Close the stream, cleans up.
CNCSError CNCSJPCFileIOStream::Close()
{
	WriteFlush();
	*(CNCSError*)this = NCSFileClose(m_hFile);
	m_hFile = NCS_NULL_FILE_HANDLE;
	*(CNCSError*)this = CNCSJPCIOStream::Close();

	NCSFree(m_pIOCache);
	m_pIOCache = NULL;
	m_nIOWriteCache = 0;
	m_iIOReadCache = 0;

	return(*(CNCSError*)this);
}

// Seek the stream to the specified offset, from the specified origin
bool CNCSJPCFileIOStream::Seek(INT64 offset, CNCSJPCIOStream::Origin origin)
{
	if(offset == 0 && origin == CURRENT) {
		return(Seek());
	}
	if(origin == START && m_nOffset == offset) {
		return(Seek());
	}
	
	WriteFlush();
	ReadFlush();

	if(origin == START) {
		m_nOffset = offset;
	} else if(origin == CURRENT) {
		m_nOffset += offset;
	} else {
		m_nOffset = NCSFileTellNative(m_hFile);
	}
	if(m_bWrite) {
		m_nFileSize = NCSMax(m_nFileSize, m_nOffset);
	}
#ifdef NCSJPC_USE_IO_BUFFER
	if(NCSFileSeekNative(m_hFile, m_nOffset, START) == -1) {
		*(CNCSError*)this = NCS_FILEIO_ERROR;
		m_nOffset = -1;
	} else {
		*(CNCSError*)this = NCS_SUCCESS;
	}
#else
	if(NCSFileSeekNative(m_hFile, offset, origin) == -1) {
		*(CNCSError*)this = NCS_FILEIO_ERROR;
		m_nOffset = -1;
	} else {
		*(CNCSError*)this = NCS_SUCCESS;
	}
#endif
	return(GetErrorNumber() == NCS_SUCCESS ? true : false);
}

// Is stream seekable
bool CNCSJPCFileIOStream::Seek()
{
	// Yes we can seek
	return(true);
}

INT64 CNCSJPCFileIOStream::Tell()
{
	WriteFlush();
	return(CNCSJPCIOStream::Tell());
}

// Get the size of the file in bytes, return -1 on error.
INT64 CNCSJPCFileIOStream::Size()
{
//	if(m_nFileSize != -1) {
		return(m_nFileSize);
//	} else {
//		WriteFlush();
//		if(m_bWrite) {
//			NCSFileFlush(m_hFile);
//		}
//		return(NCSFileSizeBytes(m_pName));
//	}
}


// Read some data from the stream
// Note, any changes to Read() need to be reflected in ReadUINT8() below
bool CNCSJPCFileIOStream::Read(void* buffer, UINT32 count)
{
	bool bRet = false;

	if(m_nMaxIOCache == 0) {
		NCSError eError = NCSFileRead(m_hFile, buffer, count, NULL);
		if(eError == NCS_SUCCESS) {
			m_nOffset += count;
			bRet = true;
		} else {
			bRet = false;
			*(CNCSError*)this = eError;
		}
	} else {
		if(m_iIOReadCache < m_nMaxIOCache) {
			UINT32 nThisRead = count;
			if(nThisRead == 1) {
				((UINT8*)buffer)[0] = m_pIOCache[m_iIOReadCache];
				m_nOffset += 1;
				m_iIOReadCache += 1;
				bRet = true;
			} else {
				nThisRead = NCSMin(nThisRead, m_nMaxIOCache - m_iIOReadCache);
				memcpy(buffer, m_pIOCache + m_iIOReadCache, nThisRead);
				count -= nThisRead;
				m_nOffset += nThisRead;
				m_iIOReadCache += nThisRead;
				if(count) {
					bRet = Read((UINT8*)buffer + nThisRead, count);
				} else {
					bRet = true;
				} 
			}
		} else {
			UINT32 nThisRead = (UINT32)NCSMin(m_nMaxIOCache, Size() - m_nOffset);
			NCSError eError;
			if(nThisRead == 0 && Size() == m_nOffset) {
				eError = NCS_FILE_EOF;
			} else {
				eError = NCSFileRead(m_hFile, m_pIOCache, (UINT32)NCSMin(m_nMaxIOCache, Size() - m_nOffset), NULL);
			}
			if(eError == NCS_SUCCESS) {
				m_iIOReadCache = 0;
				bRet = Read(buffer, count);
			} else {
				bRet = false;
				*(CNCSError*)this = eError;
			}
		}
	}
	return(bRet);
}

// Write some data to the stream
bool CNCSJPCFileIOStream::Write(void* buffer, UINT32 count)
{
	if(count + m_nIOWriteCache > m_nMaxIOCache) {
		WriteFlush();
	}
	if(count >= m_nMaxIOCache) {
		// Single write too big for cache, write straight out.
		NCSError eError = NCSFileWrite(m_hFile, buffer, count, &count);
		if(eError == NCS_SUCCESS) {
			m_nOffset += count;
			m_nFileSize = NCSMax(m_nFileSize, m_nOffset);
			return(true);
		}
		*(CNCSError*)this = eError;
		return(false);
	} else {
		// Copy to cache
		memcpy(m_pIOCache + m_nIOWriteCache, buffer, count);
		m_nIOWriteCache += count;
		return(true);
	}
}

// Read a UINT8 from the stream
// This is a specialised version of the generic Read() call above
bool CNCSJPCFileIOStream::ReadUINT8(UINT8 &nValue)
{
	bool bRet = false;

	if(m_nMaxIOCache == 0) {
		NCSError eError = NCSFileRead(m_hFile, &nValue, sizeof(nValue), NULL);
		if(eError == NCS_SUCCESS) {
			m_nOffset += sizeof(nValue);
			bRet = true;
		} else {
			bRet = false;
			*(CNCSError*)this = eError;
		}
	} else {
		if(m_iIOReadCache < m_nMaxIOCache) {
			nValue = m_pIOCache[m_iIOReadCache];
			m_nOffset += sizeof(nValue);
			m_iIOReadCache += sizeof(nValue);
			bRet = true; 
		} else {
			UINT32 nThisRead = (UINT32)NCSMin(m_nMaxIOCache, Size() - m_nOffset);
			NCSError eError;
			if(nThisRead == 0 && Size() == m_nOffset) {
				eError = NCS_FILE_EOF;
			} else {
				eError = NCSFileRead(m_hFile, m_pIOCache, (UINT32)NCSMin(m_nMaxIOCache, Size() - m_nOffset), NULL);
			}
			if(eError == NCS_SUCCESS) {
				m_iIOReadCache = 0;
				bRet = ReadUINT8(nValue);
			} else {
				bRet = false;
				*(CNCSError*)this = eError;
			}
		}
	}
	return(bRet);
}

// Set the IO callback functions to use instead of the normal crt IO routines.
CNCSError CNCSJPCFileIOStream::SetIOCallbacks(NCSError (NCS_CALL *pOpenACB)(char *szFileName, void **ppClientData),
#ifdef NCS_BUILD_UNICODE
									NCSError (NCS_CALL *pOpenWCB)(wchar_t *szFileName, void **ppClientData),
#endif
									NCSError (NCS_CALL *pCloseCB)(void *pClientData),
									NCSError (NCS_CALL *pReadCB)(void *pClientData, void *pBuffer, UINT32 nLength),
									NCSError (NCS_CALL *pSeekCB)(void *pClientData, UINT64 nOffset),
									NCSError (NCS_CALL *pTellCB)(void *pClientData, UINT64 *pOffset))
{
	sm_pOpenACB = pOpenACB;
#ifdef NCS_BUILD_UNICODE
	sm_pOpenWCB = pOpenWCB;
#endif
	sm_pCloseCB = pCloseCB;
	sm_pReadCB = pReadCB;
	sm_pSeekCB = pSeekCB;
	sm_pTellCB = pTellCB;
#ifdef NCSJPC_ECW_SUPPORT
	return(NCSecwSetIOCallbacks_ECW(pOpenACB, pCloseCB, pReadCB, pSeekCB, pTellCB));
#else // NCSJPC_ECW_SUPPORT
	return(NCS_SUCCESS);
#endif
}

UINT32 CNCSJPCFileIOStream::GetIOCacheSize()
{
	return(m_nMaxIOCache);
}

void CNCSJPCFileIOStream::SetIOCacheSize(UINT32 nBytes)
{
	m_nMaxIOCache = nBytes;
}
