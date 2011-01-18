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

#ifndef NCSJPCMEMORYIOSTREAM_H
#define NCSJPCMEMORYIOSTREAM_H

#ifndef NCSJPCIOSTREAM_H
#include "NCSJPCIOStream.h"
#endif // NCSJPCIOSTREAM_H

/**
 * CNCSJPCFileIOStream class - 64bit File-based IO "Stream" class.
 * This class is the IO mechanism used to access JP2 files.
 * << and >> Operators are not implemented as we can't use exception handling for portability reasons.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.6 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
 */
class NCSJPC_EXPORT_ALL CNCSJPCMemoryIOStream: public CNCSJPCIOStream {
public:
		/** Default constructor, initialises members */
	CNCSJPCMemoryIOStream(bool bSeekable = true);
		/** Virtual destructor, releases members */
	virtual ~CNCSJPCMemoryIOStream();

		/** 
		 * Open the stream on the specified memory.
		 * @param		pMemory		Pointer to memory image
		 * @param		nMemoryLength	Memory image length
		 * @return      CNCSError	NCS_SUCCESS, or error code on failure.
		 */
	virtual CNCSError Open(const void *pMemory, UINT32 nMemoryLength);
		/** 
		 * Open the memory stream from the specified IOStream.  IOStream is left seeked forward by nMemoryLength
		 * @param		Stream		Stream to open from
		 * @param		nMemoryLength	Memory image length
		 * @return      CNCSError	NCS_SUCCESS, or error code on failure.
		 */
	virtual CNCSError Open(CNCSJPCIOStream &Stream, UINT32 nMemoryLength);
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
		
		/** Is the stream a packet stream?  This means the SOD segment will be missing from
		 * the stream as parsed, and must be requested separately (ie, ECWP, JPIP)
		 * @return		bool		true this is a packet stream, otherwise false
		 */
	virtual bool NCS_FASTCALL IsPacketStream() { return(false); }
protected:
	void *m_pMemory;
	UINT32 m_nMemoryLength;
	bool	m_bSeekable;
	bool	m_bOwnMemory;
};

#endif // !NCSJPCMEMORYIOSTREAM_H
