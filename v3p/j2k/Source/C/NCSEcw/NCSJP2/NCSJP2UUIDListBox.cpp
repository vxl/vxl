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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2UUIDListBox.cpp $
** CREATED:  04/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2UUIDListBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2UUIDInfoBox::CNCSJP2UUIDListBox::sm_nTBox = 'ulst';

// Constructor
CNCSJP2File::CNCSJP2UUIDInfoBox::CNCSJP2UUIDListBox::CNCSJP2UUIDListBox()
{
	// Initialise the members
	m_nTBox = sm_nTBox;

	m_nEntries = 0;
}

// Destructor - cleanup
CNCSJP2File::CNCSJP2UUIDInfoBox::CNCSJP2UUIDListBox::~CNCSJP2UUIDListBox()
{
}

// Parse the box in from the stream.
CNCSError CNCSJP2File::CNCSJP2UUIDInfoBox::CNCSJP2UUIDListBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		// Get the fields in
		NCSJP2_CHECKIO(ReadUINT16(m_nEntries));
		for(int e = 0; e < m_nEntries; e++) {
			NCSUUID uuid;

			NCSJP2_CHECKIO(Read(&uuid.m_UUID, sizeof(uuid.m_UUID)));
			m_UUIDs.push_back(uuid);
		}

		m_bValid = true;
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the color spec box out to the stream.
CNCSError CNCSJP2File::CNCSJP2UUIDInfoBox::CNCSJP2UUIDListBox::UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	// Setup the base box.
	m_nTBox = sm_nTBox;
	m_nXLBox = 8 + sizeof(UINT16) + m_nEntries * sizeof(m_UUIDs[0].m_UUID);
	
		// Write out the base box first
	Error = CNCSJP2Box::UnParse(JP2File, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT16(m_nEntries));
		for(int c = 0; c < m_nEntries; c++) {
			NCSJP2_CHECKIO(Write(m_UUIDs[c].m_UUID, sizeof(m_UUIDs[0].m_UUID)));
		}	
	NCSJP2_CHECKIO_END();
	return(Error);
}
