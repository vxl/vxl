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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2BitsPerComponentBox.cpp $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2BitsPerComponentBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2BitsPerComponentBox::sm_nTBox = 'bpcc';

// Constructor
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2BitsPerComponentBox::CNCSJP2BitsPerComponentBox()
{
	m_nTBox = sm_nTBox;
}

// Destructor
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2BitsPerComponentBox::~CNCSJP2BitsPerComponentBox()
{
}

void CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2BitsPerComponentBox::UpdateXLBox(void)
{
	CNCSJP2Box::UpdateXLBox();
	m_nXLBox += m_Bits.size() * sizeof(UINT8);
}

// Parse the box in from the stream
CNCSError CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2BitsPerComponentBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		// For each component in the image, read in the bit depth
		for(int c = 0; c < JP2File.m_Header.m_ImageHeader.m_nComponents; c++) {			
			CNCSJPCComponentDepthType ci;
			
			NCSJP2_CHECKIO_ERROR(ci.Parse(JP2File.m_Codestream, Stream));
			// Store the bit depth into the m_Bits member.
			m_Bits.push_back(ci);
		}
		m_bValid = true;
	NCSJP2_CHECKIO_END();
	return(Error);
}

// Unparse the box out to the stream.
CNCSError CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2BitsPerComponentBox::UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	// Setup the base box members

	// Write out the base box
	Error = CNCSJP2Box::UnParse(JP2File, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		// For each component, write out the bit depth.
		for(UINT32 c = 0; c < m_Bits.size(); c++) {
			m_Bits[c].UnParse(JP2File.m_Codestream, Stream);
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}
