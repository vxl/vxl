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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCIOStream.cpp $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCIOStream class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCIOStream.h"
#ifdef NCSJPC_ECW_SUPPORT
#include "NCSEcw.h"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
 
// Constructor
CNCSJPCIOStream::CNCSJPCIOStream()
{
	// Initialise the members
#ifdef NCS_BUILD_UNICODE
	m_pName = (wchar_t*)NULL;
#else
	m_pName = (char*)NULL;
#endif
	m_bWrite = false;
	m_nThisBytes = 0x0;
	m_nBitsLeft = 0;
	m_nOffset = 0;
}

// Destructor
CNCSJPCIOStream::~CNCSJPCIOStream()
{
	// Close the file, Close() will clean up.
	Close();
}


#ifdef NCS_BUILD_UNICODE
// Open the stream on the specified full path name.
CNCSError CNCSJPCIOStream::Open(wchar_t*pName, bool bWrite)
{
    m_pName = (wchar_t *) malloc((wcslen(pName)+1) * sizeof(wchar_t));
    wcscpy(m_pName, pName);

//	m_pName = wcsdup(pName);
	m_bWrite = bWrite;
	if(bWrite) {
		m_nBitsLeft = 8;
	}
	m_nOffset = 0;
	return(CNCSError(NCS_SUCCESS));
}
#endif
// Open the stream on the specified full path name.
CNCSError CNCSJPCIOStream::Open(char *pName, bool bWrite)
{
#ifdef NCS_BUILD_UNICODE
	m_pName = wcsdup(OS_STRING(pName));
#else
    m_pName = (char *) malloc(strlen(pName)+1);
    strcpy(m_pName, pName);
//	m_pName = strdup(pName);
#endif
	m_bWrite = bWrite;
	if(bWrite) {
		m_nBitsLeft = 8;
	}
	m_nOffset = 0;
	return(CNCSError(NCS_SUCCESS));
}

// Close the stream, cleans up.
CNCSError CNCSJPCIOStream::Close()
{
	NCSFree(m_pName);
	m_pName = NULL;
	m_bWrite = false;
	m_nThisBytes = 0x0;
	m_nBitsLeft = 0;
	m_nOffset = 0;
	return(CNCSError(NCS_SUCCESS));
}

// Seek the stream to the specified offset, from the specified origin
bool CNCSJPCIOStream::Seek(INT64 offset, CNCSJPCIOStream::Origin origin)
{
	return(false);
}

// Is stream seekable
bool CNCSJPCIOStream::Seek()
{
	return(false);
}

// Get the current stream pointer position, return -1 on error
INT64 CNCSJPCIOStream::Tell()
{
	return(m_nOffset);
}

// Get the size of the stream in bytes, return -1 on error.
INT64 CNCSJPCIOStream::Size()
{
	return(0);
}

// Mark the stream, so we can rewind() to the current position easily.
bool CNCSJPCIOStream::Mark()
{
	INT64 offset;
	
	offset = Tell();
	if(offset != -1) {
		// Keep a stack of marks
		m_Marks.push_back(offset);
	} else {
		*(CNCSError*)this = NCS_FILEIO_ERROR;
	}
	return(GetErrorNumber() == NCS_SUCCESS ? true : false);
}

// Rewind to the previous mark
bool CNCSJPCIOStream::Rewind()
{
	if(m_Marks.size() != 0) {
		INT64 offset;
		offset = m_Marks[m_Marks.size() - 1];
		UnMark();	// Pop this mark from the stack
		return(Seek(offset, START));
	} else {
		*(CNCSError*)this = NCS_FILEIO_ERROR;
	}
	return(GetErrorNumber() == NCS_SUCCESS ? true : false);
}

// Unmark the stream, removes the previous mark from the stack
bool CNCSJPCIOStream::UnMark()
{
	if(m_Marks.size() != 0) {
		m_Marks.pop_back();
	} else {
		*(CNCSError*)this = NCS_FILEIO_ERROR;
	}
	return(GetErrorNumber() == NCS_SUCCESS ? true : false);
}

// Stream if open for writing
bool CNCSJPCIOStream::IsWrite()
{
	return(m_bWrite);
}

// Read some data from the stream
bool CNCSJPCIOStream::Read(void* buffer, UINT32 count)
{
	return(false);
}

// Write some data to the stream
bool CNCSJPCIOStream::Write(void* buffer, UINT32 count)
{
	return(false);
}

// Miscellaneous read/write routines, these byteswap as necessary, JP2 files are always MSB byteorder.
bool CNCSJPCIOStream::ReadUINT16(UINT16 &Buffer)
{
	if(Read(&Buffer, sizeof(Buffer))) {
#ifdef NCSBO_LSBFIRST
		Buffer = NCSByteSwap16(Buffer);
#endif
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::ReadUINT32(UINT32 &Buffer)
{
	if(Read(&Buffer, sizeof(Buffer))) {
#ifdef NCSBO_LSBFIRST
		Buffer = NCSByteSwap32(Buffer);
#endif
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::ReadUINT64(UINT64 &Buffer)
{
	if(Read(&Buffer, sizeof(Buffer))) {
#ifdef NCSBO_LSBFIRST
		Buffer = NCSByteSwap64(Buffer);
#endif
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::ReadINT16(INT16 &Buffer)
{
	if(Read(&Buffer, sizeof(Buffer))) {
#ifdef NCSBO_LSBFIRST
		Buffer = NCSByteSwap16(Buffer);
#endif
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::ReadINT32(INT32 &Buffer)
{
	if(Read(&Buffer, sizeof(Buffer))) {
#ifdef NCSBO_LSBFIRST
		Buffer = NCSByteSwap32(Buffer);
#endif
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::ReadINT64(INT64 &Buffer)
{
	if(Read(&Buffer, sizeof(Buffer))) {
#ifdef NCSBO_LSBFIRST
		Buffer = NCSByteSwap64(Buffer);
#endif
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::ReadIEEE4(IEEE4 &Buffer)
{
	if(Read(&Buffer, sizeof(Buffer))) {
#ifdef NCSBO_LSBFIRST
		NCSByteSwapRange32((UINT32*)&Buffer, (UINT32*)&Buffer, 1);
#endif
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::ReadIEEE8(IEEE8 &Buffer)
{
	if(Read(&Buffer, sizeof(Buffer))) {
#ifdef NCSBO_LSBFIRST
		NCSByteSwapRange64((UINT64*)&Buffer, (UINT64*)&Buffer, 1);
#endif
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::WriteUINT8(UINT8 nValue)
{
	if(Write(&nValue, sizeof(nValue))) {
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::WriteUINT16(UINT16 nValue)
{
#ifdef NCSBO_LSBFIRST
	nValue = NCSByteSwap16(nValue);
#endif
	if(Write(&nValue, sizeof(nValue))) {
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::WriteUINT32(UINT32 nValue)
{
#ifdef NCSBO_LSBFIRST
	nValue = NCSByteSwap32(nValue);
#endif
	if(Write(&nValue, sizeof(nValue))) {
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::WriteUINT64(UINT64 nValue)
{
#ifdef NCSBO_LSBFIRST
	nValue = NCSByteSwap64(nValue);
#endif
	if(Write(&nValue, sizeof(nValue))) {
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::WriteINT8(INT8 nValue)
{
	if(Write(&nValue, sizeof(nValue))) {
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::WriteINT16(INT16 nValue)
{
#ifdef NCSBO_LSBFIRST
	nValue = NCSByteSwap16(nValue);
#endif
	if(Write(&nValue, sizeof(nValue))) {
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::WriteINT32(INT32 nValue)
{
#ifdef NCSBO_LSBFIRST
	nValue = NCSByteSwap32(nValue);
#endif
	if(Write(&nValue, sizeof(nValue))) {
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::WriteINT64(INT64 nValue)
{
#ifdef NCSBO_LSBFIRST
	nValue = NCSByteSwap64(nValue);
#endif
	if(Write(&nValue, sizeof(nValue))) {
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::WriteIEEE4(IEEE4 fValue)
{
#ifdef NCSBO_LSBFIRST
	NCSByteSwapRange32((UINT32*)&fValue, (UINT32*)&fValue, 1);
#endif
	if(Write(&fValue, sizeof(fValue))) {
		return(true);
	}
	return(false);
}

bool CNCSJPCIOStream::WriteIEEE8(IEEE8 dValue)
{
#ifdef NCSBO_LSBFIRST
	NCSByteSwapRange64((UINT64*)&dValue, (UINT64*)&dValue, 1);
#endif
	if(Write(&dValue, sizeof(dValue))) {
		return(true);
	}
	return(false);
}

// Reset values for bit (Un)stuffing.
bool CNCSJPCIOStream::ResetBitStuff()
{
	m_nThisBytes = 0x0;
	if(m_bWrite) {
		m_nBitsLeft = 8;
	} else {
		m_nBitsLeft = 0;
	}
	return(true);
}

// Flush values for bit (Un)stuffing.
bool CNCSJPCIOStream::FlushBitStuff()
{
	bool bRet = true;
	if(m_bWrite) {
		if(m_nBitsLeft != 8) {
			m_nThisBytes = m_nThisBytes << m_nBitsLeft;
			bRet &= WriteUINT8((UINT8)m_nThisBytes);
		}
		m_nThisBytes = 0x0;
		m_nBitsLeft = 8;
	}
	return(bRet);
}

// Stuff a bit to the JPC bitdata stream.
bool CNCSJPCIOStream::Stuff(bool bBit)
{
	bool bRet = false;
	m_nThisBytes = (m_nThisBytes << 1) | (UINT16)bBit;
	m_nBitsLeft--;
	if(m_nBitsLeft == 0) {
		bRet = WriteUINT8((UINT8)m_nThisBytes);
		if(bRet) {
			if(m_nThisBytes == 0xff) {
				m_nBitsLeft = 7;
			} else {
				m_nBitsLeft = 8;
			}
			m_nThisBytes = 0x0;
		}
	} else {
		bRet = true;
	}
	return(bRet);
}

// Unstuff a bit from the stream.
bool CNCSJPCIOStream::UnStuff(bool &bBit)
{
	bool bRet = false;
	if(m_nBitsLeft) {
		bBit = (m_nThisBytes & (0x1 << (m_nBitsLeft - 1))) ? true : false;
		m_nBitsLeft--;
		bRet = true;
	} else {
		UINT8 t8;
		bRet = ReadUINT8(t8);
		if(bRet) {
			m_nThisBytes = t8;
			m_nBitsLeft = 8;

			if(t8 == 0xff) {
				bRet = ReadUINT8(t8);
				if(bRet) {
					m_nThisBytes = (m_nThisBytes << 7) | (t8 & 0x7f);
					m_nBitsLeft = 15;
				}
			}
			if(bRet) {
				return(UnStuff(bBit));
			}
		}
	}
	return(bRet);
}
