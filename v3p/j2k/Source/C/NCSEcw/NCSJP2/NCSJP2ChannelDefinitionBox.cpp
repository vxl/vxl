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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2ChannelDefinitionBox.cpp $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2ChannelDefinitionBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::sm_nTBox = 'cdef';

// Constructor
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::CNCSJP2ChannelDefinitionBox()
{
	// Initialise the members
	m_nTBox = sm_nTBox;

	m_nEntries = 0;
}

// Destructor - cleanup
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::~CNCSJP2ChannelDefinitionBox()
{
}

const CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::ChannelDefinition *CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::GetDefinitition(UINT16 iChannel) 
{
	for(int i = 0; i < m_nEntries; i++) {
		if(m_Definitions[i].m_iChannel == iChannel) {
			return(&m_Definitions[i]);
		}
	}
	return(NULL);
}

void CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::UpdateXLBox(void)
{
	CNCSJP2Box::UpdateXLBox();
	m_nXLBox += sizeof(UINT16) + m_nEntries * 3 * sizeof(UINT16);
}

// Parse the box in from the stream.
CNCSError CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		// Get the palette fields in
		NCSJP2_CHECKIO(ReadUINT16(m_nEntries));

		for(int c = 0; c < m_nEntries; c++) {
			ChannelDefinition def;
			UINT16 t16;
			
			NCSJP2_CHECKIO(ReadUINT16(def.m_iChannel));
			NCSJP2_CHECKIO(ReadUINT16(t16)); def.m_eType = (Type)t16;
			NCSJP2_CHECKIO(ReadUINT16(t16)); def.m_eAssociation = (Association)t16;
			m_Definitions.push_back(def);
		}
		m_bValid = true;
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the color spec box out to the stream.
CNCSError CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

		// Write out the base box first
	Error = CNCSJP2Box::UnParse(JP2File, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		// Get the palette fields in
		NCSJP2_CHECKIO(WriteUINT16(m_nEntries));

		for(int c = 0; c < m_nEntries; c++) {
			NCSJP2_CHECKIO(WriteUINT16(m_Definitions[c].m_iChannel));
			NCSJP2_CHECKIO(WriteUINT16(m_Definitions[c].m_eType));
			NCSJP2_CHECKIO(WriteUINT16(m_Definitions[c].m_eAssociation));
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}
