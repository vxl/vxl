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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCMemoryIOStream.cpp $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCMemoryIOStream class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCMemoryIOStream.h"
#include "NCSUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
 
// Constructor
CNCSJPCMemoryIOStream::CNCSJPCMemoryIOStream(bool bSeekable)
{
	// Initialise the members
	m_pMemory = (void*)NULL;
	m_nMemoryLength = 0;
	m_nOffset = 0;
	m_bSeekable = bSeekable;
	m_bOwnMemory = false;
}

// Destructor
CNCSJPCMemoryIOStream::~CNCSJPCMemoryIOStream()
{
	// Close the file, Close() will clean up.
	Close();
}

// Open the stream on the specified memory.
CNCSError CNCSJPCMemoryIOStream::Open(const void *pMemory, UINT32 nMemoryLength)
{
	if(pMemory && nMemoryLength) {
		m_pMemory = (void*)pMemory;
		m_nMemoryLength = nMemoryLength;
		*(CNCSError*)this = NCS_SUCCESS;
	} else {
		*(CNCSError*)this = NCS_INVALID_PARAMETER;
	}
	if(GetErrorNumber() == NCS_SUCCESS) {
		*(CNCSError*)this = CNCSJPCIOStream::Open("MEMORY", false);
	}
	m_bOwnMemory = false;
	return(*(CNCSError*)this);
}

// Open the stream on the specified memory.
CNCSError CNCSJPCMemoryIOStream::Open(CNCSJPCIOStream &Stream, UINT32 nMemoryLength)
{
	if(nMemoryLength) {
		m_pMemory = NCSMalloc(nMemoryLength, FALSE);
		m_nMemoryLength = nMemoryLength;
		m_bOwnMemory = true;

		if(Stream.Read(m_pMemory, m_nMemoryLength) == false) {
			*(CNCSError*)this = Stream;
		} else {
			*(CNCSError*)this = NCS_SUCCESS;
		}
	} else {
		*(CNCSError*)this = NCS_INVALID_PARAMETER;
	}
	if(GetErrorNumber() == NCS_SUCCESS) {
		*(CNCSError*)this = CNCSJPCIOStream::Open("MEMORY", false);
	}
	return(*(CNCSError*)this);
}

// Close the stream, cleans up.
CNCSError CNCSJPCMemoryIOStream::Close()
{
	if(m_bOwnMemory) {
		NCSFree(m_pMemory);
	}
	m_pMemory = (void*)NULL;
	m_nMemoryLength = 0;
	*(CNCSError*)this = CNCSJPCIOStream::Close();
	return(*(CNCSError*)this);
}

// Seek the stream to the specified offset, from the specified origin
bool CNCSJPCMemoryIOStream::Seek(INT64 offset, CNCSJPCIOStream::Origin origin)
{
	switch(origin) {
		case START:
				m_nOffset = offset;
			break;
		case CURRENT:
				if(offset == 0) {
					return(Seek());
				} else {
					m_nOffset += offset;
					if(m_nOffset < 0) {
						m_nOffset = 0;
					} else if(m_nOffset >= Size()) {
						m_nOffset = Size() - 1;
					}
				}
			break;
		case END:
				m_nOffset = Size() - 1 - offset;
			break;
	}
	return(true);
}

// Is stream seekable
bool CNCSJPCMemoryIOStream::Seek()
{
	// Yes we can seek
	return(m_bSeekable);
}

// Get the size of the file in bytes, return -1 on error.
INT64 CNCSJPCMemoryIOStream::Size()
{
	return(m_nMemoryLength);
}


// Read some data from the stream
bool CNCSJPCMemoryIOStream::Read(void* buffer, UINT32 count)
{
	if(m_nOffset < m_nMemoryLength) {
		if(count == 1) {
			((UINT8*)buffer)[0] = ((UINT8*)m_pMemory)[m_nOffset];
			m_nOffset++;
			return(true);
		} else {
			UINT32 nRead = (UINT32)NCSMax(0, NCSMin(count, m_nMemoryLength - m_nOffset));
			memcpy(buffer, (UINT8*)m_pMemory + m_nOffset, nRead);
			m_nOffset += nRead;

			if(nRead == count) {
				return(true);
			} else {
				*(CNCSError*)this = NCS_INVALID_PARAMETER;
			}
		}
	} else {
		*(CNCSError*)this = NCS_INVALID_PARAMETER;
	}
/*	UINT32 nRead = (UINT32)NCSMax(0, NCSMin(count, Size() - Tell()));
	memcpy(buffer, (UINT8*)m_pMemory + m_nOffset, nRead);
	m_nOffset += nRead;

	if(nRead == count) {
		return(true);
	} else {
		*(CNCSError*)this = NCS_INVALID_PARAMETER;
	}*/
	return(false);
}

// Write some data to the stream
bool CNCSJPCMemoryIOStream::Write(void* buffer, UINT32 count)
{
	return(false);
}
