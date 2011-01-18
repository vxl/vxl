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
** FILE:     $Archive: /NCS/Source/include/NCSJPCIOStream.h $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCIOStream class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCIOSTREAM_H
#define NCSJPCIOSTREAM_H

#ifndef NCSJPCTYPES_H
#include "NCSJPCTypes.h"
#endif // !NCSJPCTYPES_H

// NCS 64bit FileIO routines
#ifndef NCSFILEIO_H
#include "NCSFileIO.h"
#endif // !NCSFILEIO_H

// Various NCS Headers
#include "NCSDefs.h"
#include "NCSError.h"
//#include "NCSUtil.h"

// STD vector template
#include <vector>

/**
 * Some time-saving CHECKIO Macros.
 * Ideally we'd use exceptions instead, but they aren't portable to all compilers/platforms,
 * so revert to a low-tech solution.
 * 
 * @param        Error		Reference to error to construct from
 */
#define NCSJP2_CHECKIO_BEGIN(e, s)	while(e == NCS_SUCCESS) { \
												CNCSError *pErrorNCSJP2_CHECKIO = &e;\
												CNCSJPCIOStream *pStreamNCSJP2_CHECKIO = &s
#define NCSJP2_CHECKIO(a)		if(pStreamNCSJP2_CHECKIO->a == false) { *pErrorNCSJP2_CHECKIO = *pStreamNCSJP2_CHECKIO; break; }
#define NCSJP2_CHECKIO_ERROR(a)	*pErrorNCSJP2_CHECKIO = a; if(*pErrorNCSJP2_CHECKIO != NCS_SUCCESS) break
#define NCSJP2_CHECKIO_END()		break; }

/**
 * CNCSJPCIOStream class - 64bit IO "Stream" class.
 * This class is the IO mechanism used to access JP2 files.
 * << and >> Operators are not implemented as we can't use exception handling for portability reasons.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.16 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
 */
class NCSJPC_EXPORT_ALL CNCSJPCIOStream: public CNCSError {
public:
		/** Anonymous enum for Seek() origin. */
	typedef enum {
			/** Origin is start of file */
		START		= NCS_FILE_SEEK_START,
			/** Origin is from current position in file */
		CURRENT		= NCS_FILE_SEEK_CURRENT,
			/** Origin is from the end of the file */
		END			= NCS_FILE_SEEK_END
	} Origin;

		/** Default constructor, initialises members */
	CNCSJPCIOStream();
		/** Virtual destructor, releases members */
	virtual ~CNCSJPCIOStream();

		/** 
		 * Get the current CNCSError.
		 * @return      CNCSError	CNCSError object representing current error status of the Stream.
		 */
	virtual CNCSError GetError() { return((CNCSError)*this); };
#ifdef NCS_BUILD_UNICODE
		/** 
		 * Get the current filename for the stream.
		 * @return      _NCSTChar *	const pointer to filename for the stream if available, else NULL.
		 */
	virtual const wchar_t *GetName() { 
			return((const wchar_t*)m_pName); 
		};
		/** 
		 * Open the stream on the specified file.
		 * @param		pName		Full Name of JP2 stream being parsed
		 * @param		bWrite		Open for writing flag.
		 * @return      CNCSError	NCS_SUCCESS, or error code on failure.
		 */
	virtual CNCSError Open(wchar_t *pName, bool bWrite = false);
#else
		/** 
		 * Get the current filename for the stream.
		 * @return      _NCSTChar *	const pointer to filename for the stream if available, else NULL.
		 */
	virtual const char *GetName() { return(m_pName); };
#endif
		/** 
		 * Open the stream on the specified file.
		 * @param		pName		Full Name of JP2 stream being parsed
		 * @param		bWrite		Open for writing flag.
		 * @return      CNCSError	NCS_SUCCESS, or error code on failure.
		 */
	virtual CNCSError Open(char *pName, bool bWrite = false);
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
		 * Mark the current position in the file, so we can rewind it if required.
		 * Note: maintains a stack of marked positions.
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL Mark();
		/** 
		 * Rewind the file to the previously marked position.
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL Rewind();
		/** 
		 * UnMark the previously marked file position.
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL UnMark();

		/** Is file open for writing 
		 * @return		bool		true is file os open for writing, otherwise false
		 */
	virtual bool NCS_FASTCALL IsWrite();

		/** Is the stream a packet stream?  This means the SOD segment will be missing from
		 * the stream as parsed, and must be requested separately (ie, ECWP, JPIP)
		 * @return		bool		true this is a packet stream, otherwise false
		 */
	virtual bool NCS_FASTCALL IsPacketStream() { return(false); }

		/** Packet stream subclasses overload this to process received packets on a 
		 * regular basis
		 */
	virtual void ProcessReceivedPackets() {};
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
	virtual NCS_INLINE bool NCS_FASTCALL ReadUINT8(UINT8 &Buffer) {
				return(Read(&Buffer, sizeof(Buffer)));
			};
		/** 
		 * Read a UINT16 from the stream.
		 * @param		buffer		Buffer to receive the data
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL ReadUINT16(UINT16 &Buffer);
		/** 
		 * Read a UINT32 from the stream, byteswapped if necessary.
		 * @param		buffer		Buffer to receive the data
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL ReadUINT32(UINT32 &Buffer);
		/** 
		 * Read a UINT64 from the stream, byteswapped if necessary.
		 * @param		buffer		Buffer to receive the data
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL ReadUINT64(UINT64 &Buffer);
		/** 
		 * Read a INT8 from the stream.
		 * @param		buffer		Buffer to receive the data
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual NCS_INLINE bool NCS_FASTCALL ReadINT8(INT8 &Buffer) {
				return(Read(&Buffer, sizeof(Buffer)));
			};
		/** 
		 * Read a INT16 from the stream.
		 * @param		buffer		Buffer to receive the data
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL ReadINT16(INT16 &Buffer);
		/** 
		 * Read a INT32 from the stream, byteswapped if necessary.
		 * @param		buffer		Buffer to receive the data
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL ReadINT32(INT32 &Buffer);
		/** 
		 * Read a INT64 from the stream, byteswapped if necessary.
		 * @param		buffer		Buffer to receive the data
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL ReadINT64(INT64 &Buffer);
		/** 
		 * Read a IEEE4 float from the stream, byteswapped if necessary.
		 * @param		buffer		Buffer to receive the data
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL ReadIEEE4(IEEE4 &Buffer);
		/** 
		 * Read a IEEE8 double from the stream, byteswapped if necessary.
		 * @param		buffer		Buffer to receive the data
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL ReadIEEE8(IEEE8 &Buffer);
		/** 
		 * Write a UINT8 to the stream.
		 * @param		buffer		Buffer to write to the stream
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL WriteUINT8(UINT8 nValue);
		/** 
		 * Write a UINT16 to the stream, byteswapped if necessary.
		 * @param		buffer		Buffer to write to the stream
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL WriteUINT16(UINT16 nValue);
		/** 
		 * Write a UINT32 to the stream, byteswapped if necessary.
		 * @param		buffer		Buffer to write to the stream
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL WriteUINT32(UINT32 nValue);
		/** 
		 * Write a UINT64 to the stream, byteswapped if necessary.
		 * @param		buffer		Buffer to write to the stream
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL WriteUINT64(UINT64 nValue);
		/** 
		 * Write a INT8 to the stream.
		 * @param		buffer		Buffer to write to the stream
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL WriteINT8(INT8 nValue);
		/** 
		 * Write a INT16 to the stream, byteswapped if necessary.
		 * @param		buffer		Buffer to write to the stream
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL WriteINT16(INT16 nValue);
		/** 
		 * Write a INT32 to the stream, byteswapped if necessary.
		 * @param		buffer		Buffer to write to the stream
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL WriteINT32(INT32 nValue);
		/** 
		 * Write a INT64 to the stream, byteswapped if necessary.
		 * @param		buffer		Buffer to write to the stream
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL WriteINT64(INT64 nValue);
		/** 
		 * Write a IEEE4 float to the stream, byteswapped if necessary.
		 * @param		buffer		Buffer to write to the stream
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL WriteIEEE4(IEEE4 fValue);
		/** 
		 * Write a IEEE8 double to the stream, byteswapped if necessary.
		 * @param		buffer		Buffer to write to the stream
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL WriteIEEE8(IEEE8 dValue);

		/** 
		 * Reset values for bit (Un)stuffing.
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL ResetBitStuff();
		/** 
		 * Flush values for bit (Un)stuffing.
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL FlushBitStuff();
		/** 
		 * Stuff a bit to the JPC bitdata stream.
		 * @param		bool		Buffer to write bit value to stream from.
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL Stuff(bool bBit);
		/** 
		 * UnStuff a bit from the JPC bitdata stream.
		 * @param		bool		Buffer to read bit from stream to.
		 * @return      bool		true, or false on failure.  Instance inherits CNCSError object containing error value.
		 */
	virtual bool NCS_FASTCALL UnStuff(bool &bBit);

protected:
		/** name stream is open on */
#ifdef NCS_BUILD_UNICODE
	wchar_t			*m_pName;
#else
	char 			*m_pName;
#endif
		/** Stream is open for writing */
	bool			m_bWrite;
		/** Stack of marks in the stream */
	std::vector<INT64>	m_Marks;
		/** File pointer */
	INT64			m_nOffset;

		/** Current byte(s) when bit stuffing/unstuffing */
	UINT16	m_nThisBytes;
		/** # of bits left */
	UINT8	m_nBitsLeft;
};

typedef std::vector<CNCSJPCIOStream *> CNCSJPCIOStreamVector;

#endif // !NCSJPCIOSTREAM_H
