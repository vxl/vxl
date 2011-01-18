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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2DataEntryURLBox.cpp $
** CREATED:  04/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2DataEntryURLBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2UUIDInfoBox::CNCSJP2DataEntryURLBox::sm_nTBox = 'url\040';

// Constructor
CNCSJP2File::CNCSJP2UUIDInfoBox::CNCSJP2DataEntryURLBox::CNCSJP2DataEntryURLBox()
{
	// Initialise the members
	m_nTBox = sm_nTBox;

	m_nVersion = 0;
	memset(m_Flags, 0, sizeof(m_Flags));
	m_pLocation = NULL;
}

// Destructor - cleanup
CNCSJP2File::CNCSJP2UUIDInfoBox::CNCSJP2DataEntryURLBox::~CNCSJP2DataEntryURLBox()
{
	NCSFree(m_pLocation);
}

// Parse the box in from the stream.
CNCSError CNCSJP2File::CNCSJP2UUIDInfoBox::CNCSJP2DataEntryURLBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		// Get the fields in
		NCSJP2_CHECKIO(ReadUINT8(m_nVersion));
		NCSJP2_CHECKIO(Read(&m_Flags, sizeof(m_Flags)));
		m_pLocation = (UINT8*)NCSMalloc(sizeof(UINT8) * ((UINT32)m_nLDBox - 4), TRUE);
		NCSJP2_CHECKIO(Read(m_pLocation, (UINT32)m_nLDBox - 4));
		
		m_bValid = true;
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the color spec box out to the stream.
CNCSError CNCSJP2File::CNCSJP2UUIDInfoBox::CNCSJP2DataEntryURLBox::UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	// Setup the base box.
	m_nTBox = sm_nTBox;
	NCSUUID uuid;
	m_nXLBox = 8 + sizeof(UINT8) + 3 * sizeof(uuid.m_UUID) + strlen((char*)m_pLocation) + sizeof(UINT8);

		// Write out the base box first
	Error = CNCSJP2Box::UnParse(JP2File, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT8(m_nVersion));
		NCSJP2_CHECKIO(Write(&m_Flags, sizeof(m_Flags)));
		NCSJP2_CHECKIO(Write(m_pLocation, (UINT32)m_nLDBox - 4));
	NCSJP2_CHECKIO_END();
	return(Error);
}
