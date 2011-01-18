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
** FILE:     $Archive: /NCS/Source/include/NCSJP2IOStream.h $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCIOStream class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCFILEIOSTREAM_H
#define NCSJPCFILEIOSTREAM_H

#ifndef NCSJPCIOSTREAM_H
#include "NCSJPCIOStream.h"
#endif // NCSJPCIOSTREAM_H

#ifndef NCSUTIL_H
#include "NCSUtil.h"
#endif // NCSUTIL_H

/**
 * CNCSJPCFileIOStream class - 64bit File-based IO "Stream" class.
 * This class is the IO mechanism used to access JP2 files.
 * << and >> Operators are not implemented as we can't use exception handling for portability reasons.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.15 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
 */
class NCSJPC_EXPORT_ALL CNCSJPCFileIOStream: public CNCSJPCIOStream {
public:
		/** Default constructor, initialises members */
	CNCSJPCFileIOStream();
		/** Virtual destructor, releases members */
	virtual ~CNCSJPCFileIOStream();

#ifdef NCS_BUILD_UNICODE
		/** 
		 * Open the stream on the specified file.
		 * @param		pFileName	Full path Name of JP2 file being parsed
		 * @param		bWrite		Open for writing flag.
		 * @return      CNCSError	NCS_SUCCESS, or error code on failure.
		 */
	virtual CNCSError Open(wchar_t *pFileName, bool bWrite = false);
#endif
		/** 
		 * Open the stream on the specified file.
		 * @param		pFileName	Full path Name of JP2 file being parsed
		 * @param		bWrite		Open for writing flag.
		 * @return      CNCSError	NCS_SUCCESS, or error code on failure.
		 */
	virtual CNCSError Open(char *pFileName, bool bWrite = false);
		/** 
		 * Close the stream.
		 * @return      CNCSError	NCS_SUCCESS, or error code on failure.
		 */
	virtual CNCSError Close();
		/** 
		 * Seek on the file to the specified location.
		 * @param		offset		Signed 64bit offset to seek by
		 * @param		origin		Origin to calculate new position from.
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL Seek(INT64 offset, Origin origin = CURRENT);
		/** 
		 * Is stream seekable.
		 * @return      bool		true if seekable, or false if not.
		 */
	virtual bool NCS_FASTCALL Seek();
		/** 
		 * Get the current file pointer position.
		 * @return      INT64		Current file pointer position, or -1 on error.
		 */
	virtual INT64 NCS_FASTCALL Tell();
		/** 
		 * Get the total current size of the file, in bytes.
		 * @return      INT64		Size of the file, or -1 on error.
		 */
	virtual INT64 NCS_FASTCALL Size();

		/** 
		 * Read some data from the stream into the supplied buffer.
		 * @param		buffer		Buffer to read the data into
		 * @param		count		How many bytes of data to read.
		 * @param		pRead		How many bytes of data were actually read.
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL Read(void* buffer, UINT32 count);
		/** 
		 * Write some data to the stream.
		 * @param		buffer		Buffer of data to write to the stream
		 * @param		count		How many bytes of data to write to the stream.
		 * @param		pWrote		How many bytes of data were actually written to the stream.
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL Write(void* buffer, UINT32 count);
	
		/** 
		 * Read a UINT8 from the stream.
		 * @param		buffer		Buffer to receive the data
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL ReadUINT8(UINT8 &Buffer);

#ifdef NCS_BUILD_UNICODE
		/** 
		 * Set some IO callback functions to use instead of the normal crt IO routines.
		 * @param		pOpenACB	Open function - CHAR *.
		 * @param		pOpenWCB	Open function - WCHAR_T.
		 * @param		pCloseCB	Close function.
		 * @param		pReadCB		Read function.
		 * @param		pSeekCB		Seek function.
		 * @param		pTellCB		Tell function.
		 * @return      CNCSError	Error return.
		 */
	static CNCSError SetIOCallbacks(NCSError (NCS_CALL *pOpenACB)(char *szFileName, void **ppClientData),
									NCSError (NCS_CALL *pOpenWCB)(wchar_t *szFileName, void **ppClientData),
									NCSError (NCS_CALL *pCloseCB)(void *pClientData),
									NCSError (NCS_CALL *pReadCB)(void *pClientData, void *pBuffer, UINT32 nLength),
									NCSError (NCS_CALL *pSeekCB)(void *pClientData, UINT64 nOffset),
									NCSError (NCS_CALL *pTellCB)(void *pClientData, UINT64 *pOffset));
#else
		/** 
		 * Set some IO callback functions to use instead of the normal crt IO routines.
		 * @param		pOpenCB		Open function.
		 * @param		pCloseCB	Close function.
		 * @param		pReadCB		Read function.
		 * @param		pSeekCB		Seek function.
		 * @param		pTellCB		Tell function.
		 * @return      CNCSError	Error return.
		 */
	static CNCSError SetIOCallbacks(NCSError (NCS_CALL *pOpenCB)(char *szFileName, void **ppClientData),
									NCSError (NCS_CALL *pCloseCB)(void *pClientData),
									NCSError (NCS_CALL *pReadCB)(void *pClientData, void *pBuffer, UINT32 nLength),
									NCSError (NCS_CALL *pSeekCB)(void *pClientData, UINT64 nOffset),
									NCSError (NCS_CALL *pTellCB)(void *pClientData, UINT64 *pOffset));
#endif
	UINT32 GetIOCacheSize();
	void SetIOCacheSize(UINT32 nBytes);

protected:
		/** NCS 64bit file handle */
	NCS_FILE_HANDLE m_hFile;

		/** IOCache buffer */
	UINT8 *m_pIOCache;
		/** Number of bytes in Write cache */
	UINT32 m_nIOWriteCache;
		/** Current Index into read cache */
	UINT32 m_iIOReadCache;
			/** Max IO Cache size */
	UINT32 m_nMaxIOCache;
			/** Global Max IO Cache size */
	static UINT32 sm_nMaxIOCache;
		/** Precalculates file size for read only stream */
	INT64	m_nFileSize;

		/** 
		 * Flush write cache to file.
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual __inline void WriteFlush() {
			if(m_bWrite && m_pIOCache && m_nIOWriteCache) {
				NCSError eError = NCSFileWrite(m_hFile, m_pIOCache, m_nIOWriteCache, NULL);
				if(eError == NCS_SUCCESS) {
					m_nOffset += m_nIOWriteCache;
					m_nFileSize = NCSMax(m_nFileSize, m_nOffset);
					m_nIOWriteCache = 0;
				} else {
					*(CNCSError*)this = eError;
				}
			} 
		}

		/** 
		 * Flush read cache .
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual __inline void ReadFlush() {
				// Current just invalidate the cache.
			m_iIOReadCache = GetIOCacheSize();
		}

	static NCSError (NCS_CALL *sm_pOpenACB)(char *szFileName, void **ppClientData);
	static NCSError (NCS_CALL *sm_pOpenWCB)(wchar_t *szFileName, void **ppClientData);
	static NCSError (NCS_CALL *sm_pCloseCB)(void *pClientData);
	static NCSError (NCS_CALL *sm_pReadCB)(void *pClientData, void *pBuffer, UINT32 nLength);
	static NCSError (NCS_CALL *sm_pSeekCB)(void *pClientData, UINT64 nOffset);
	static NCSError (NCS_CALL *sm_pTellCB)(void *pClientData, UINT64 *pOffset);
};

#endif // !NCSJPCFILEIOSTREAM_H
