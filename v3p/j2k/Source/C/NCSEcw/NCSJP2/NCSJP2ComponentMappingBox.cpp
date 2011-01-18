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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2ComponentMappingBox.cpp $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2ComponentMappingBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ComponentMappingBox::sm_nTBox = 'cmap';

// Constructor
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ComponentMappingBox::CNCSJP2ComponentMappingBox()
{
	// Initialise the members
	m_nTBox = sm_nTBox;

	m_nEntries = 0;
}

// Destructor - cleanup
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ComponentMappingBox::~CNCSJP2ComponentMappingBox()
{
}

// Parse the box in from the stream.
CNCSError CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ComponentMappingBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		m_nEntries = (UINT16)m_nLDBox / (sizeof(UINT16) + sizeof(UINT8) + sizeof(UINT8));

		// Get the component mapping fields in
		for(int e = 0; e < m_nEntries; e++) {
			ComponentMapping cm;

			NCSJP2_CHECKIO(ReadUINT16(cm.m_iComponent));
			UINT8 t8;
			NCSJP2_CHECKIO(ReadUINT8(t8)); cm.m_nType = (Type)t8;
			NCSJP2_CHECKIO(ReadUINT8(cm.m_iPalette));
			m_Mapping.push_back(cm);
		}
		m_bValid = true;
	NCSJP2_CHECKIO_END();
	return(Error);
}

#ifndef NCSJPC_LEAN_AND_MEAN
// UnParse the color spec box out to the stream.
CNCSError CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ComponentMappingBox::UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	// Setup the base box.
	m_nTBox = sm_nTBox;
	m_nXLBox = 8 + m_nEntries * sizeof(ComponentMapping);
	
		// Write out the base box first
	Error = CNCSJP2Box::UnParse(JP2File, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		// Put the component mapping fields out
		for(int e = 0; e < m_nEntries; e++) {
			NCSJP2_CHECKIO(WriteUINT16(m_Mapping[e].m_iComponent));
			NCSJP2_CHECKIO(WriteUINT8(m_Mapping[e].m_nType));
			NCSJP2_CHECKIO(WriteUINT8(m_Mapping[e].m_iPalette));
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}
#endif //!NCSJPC_LEAN_AND_MEAN
