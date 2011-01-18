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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2Box.cpp $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2Box class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJP2Box::CNCSJP2Box()
{
	// Initialise the base box class members
	m_nBoxOffset = 0;
	m_nXLBox = 0;
	m_nTBox = 0;
	m_nDBoxOffset = 0;
	m_nLDBox = 0;
	m_bValid = false;
	m_bHaveBox = false;
	m_bIsSuperBox = false;
}

CNCSJP2Box::CNCSJP2Box(const CNCSJP2Box &s)
{
	m_nBoxOffset = s.m_nBoxOffset;
	m_nXLBox = s.m_nXLBox;
	m_nTBox = s.m_nTBox;
	m_nDBoxOffset = s.m_nDBoxOffset;
	m_nLDBox = 0;
	m_bValid = s.m_bValid;
	m_bHaveBox = s.m_bHaveBox;
}

// Destructor
CNCSJP2Box::~CNCSJP2Box()
{
}

// Setup the box(es) that must preceed this one in the JP2 file.
CNCSError CNCSJP2Box::SetPrevBoxes(CNCSJP2Box *pFirst, ...)
{
	va_list va;
	
	va_start(va, pFirst);

	CNCSJP2Box *pBox = pFirst;

	while(pBox) {	// NULL terminated varargs list
		m_Prev.push_back(pBox);
		pBox = va_arg(va, CNCSJP2Box*);
	}
	va_end(va);
	return(NCS_SUCCESS);
}

// Setup the box(es) that must proceed this one in the JP2 File.
CNCSError CNCSJP2Box::SetNextBoxes(CNCSJP2Box *pFirst, ...)
{
	va_list va;
	
	va_start(va, pFirst);

	CNCSJP2Box *pBox = pFirst;

	while(pBox) {	// NULL terminated varargs list
		m_Next.push_back(pBox);
		pBox = va_arg(va, CNCSJP2Box*);
	}
	va_end(va);
	return(NCS_SUCCESS);
}

void CNCSJP2Box::UpdateXLBox(void)
{
	m_nXLBox = 2 * sizeof(UINT32);
}

// Parse the box in from the JP2 file.
CNCSError CNCSJP2Box::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	CNCSJP2BoxList::iterator pCur = m_Prev.begin();

	while(pCur != m_Prev.end()) {  // Make sure box follows the correct box(es)
		if((*pCur)->m_bHaveBox == false) {
			Error = NCS_FILE_INVALID;
			break;
		}
		pCur++;
	}
	if(Error == NCS_SUCCESS) {	// Make sure box proceeds the correct box(es)
		pCur = m_Next.begin();

		while(pCur != m_Next.end()) {
			if((*pCur)->m_bHaveBox == true) {
				Error = NCS_FILE_INVALID;
				break;
			}
			pCur++;
		}

		if(Error == NCS_SUCCESS) {
			if(Stream.Mark()) { // Mark the stream, so we can rewind on an error.
				UINT32 nLen;

					// Store absolute offset of box in stream
				m_nBoxOffset = Stream.Tell();

					// Box length.
				if(Stream.ReadUINT32(nLen)) {
			
						// Box type
					if(Stream.ReadUINT32(m_nTBox)) {
						//
						// If the 32 bit nLen is equal to 1, then there is a 64bit length field present.
						//
						if(nLen == 1) {
							// Read in the 64bit length
							if(Stream.ReadUINT64(m_nXLBox)) {
								// Calculate the Data length within the box.
								m_nLDBox = m_nXLBox - 16;
							}
						} else {
							if(nLen == 0) {
								// Box consists of the rest of the stream
								m_nXLBox = 8 + Stream.Size() - Stream.Tell();
							} else {
								// No 64bit length present
								m_nXLBox = nLen;
							}
							m_nLDBox = m_nXLBox - 8;
						}
						if(Stream.GetError() == NCS_SUCCESS) {
							m_nDBoxOffset = Stream.Tell();
							// The type matches, or we don't care.  Unmark the stream.
							Stream.UnMark();
							// Got the box, so set the flag indicating we have it.
							m_bHaveBox = true;
						}
					}
				}
			}
			Error = Stream.GetError();
		}
	}
	return(Error);
}

// UnParse the box out to the stream.
CNCSError CNCSJP2Box::UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	// If length is > 2^32, need to write out the 64bit XLBox field.
	if(m_nXLBox > 0xffffffff) {
		// Will be writing XLBox, so write out 1 for the LBox field.
		Stream.WriteUINT32(1);
	} else {
		Stream.WriteUINT32((UINT32)m_nXLBox);
	}
	if(Stream.GetError() == NCS_SUCCESS) {
		// Write out the box type;
		if(Stream.WriteUINT32(m_nTBox) && m_nXLBox > 0xffffffff) {
			// Write out the XLBox field.
			Stream.WriteUINT64(m_nXLBox);
		}
		Error = Stream.GetError();
	}
	return(Error);
}
