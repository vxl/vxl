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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2CaptureResolutionBox.cpp $
** CREATED:  04/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2CaptureResolutionBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ResolutionBox::CNCSJP2CaptureResolutionBox::sm_nTBox = 'resc';

// Constructor
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ResolutionBox::CNCSJP2CaptureResolutionBox::CNCSJP2CaptureResolutionBox()
{
	// Initialise the members
	m_nTBox = sm_nTBox;

	m_nVRcN = 0;
	m_nVRcD = 0;
	m_nVRcE = 0;
	m_nHRcN = 0;
	m_nHRcD = 0;
	m_nHRcE = 0;
}

// Destructor - cleanup
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ResolutionBox::CNCSJP2CaptureResolutionBox::~CNCSJP2CaptureResolutionBox()
{
}

// Parse the box in from the stream.
CNCSError CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ResolutionBox::CNCSJP2CaptureResolutionBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		// Get the fields in
		NCSJP2_CHECKIO(ReadUINT16(m_nVRcN));
		NCSJP2_CHECKIO(ReadUINT16(m_nVRcD));
		NCSJP2_CHECKIO(ReadUINT16(m_nHRcN));
		NCSJP2_CHECKIO(ReadUINT16(m_nHRcD));
		NCSJP2_CHECKIO(ReadINT8(m_nVRcE));
		NCSJP2_CHECKIO(ReadINT8(m_nHRcE));

		m_bValid = true;
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the color spec box out to the stream.
CNCSError CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ResolutionBox::CNCSJP2CaptureResolutionBox::UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	// Setup the base box.
	m_nTBox = sm_nTBox;
	m_nXLBox = 8 + 4 * sizeof(UINT16) + 2 * sizeof(UINT8);

		// Write out the base box first
	Error = CNCSJP2Box::UnParse(JP2File, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT16(m_nVRcN));
		NCSJP2_CHECKIO(WriteUINT16(m_nVRcD));
		NCSJP2_CHECKIO(WriteUINT16(m_nHRcN));
		NCSJP2_CHECKIO(WriteUINT16(m_nHRcD));
		NCSJP2_CHECKIO(WriteINT8(m_nVRcE));
		NCSJP2_CHECKIO(WriteINT8(m_nHRcE));
	NCSJP2_CHECKIO_END();
	return(Error);
}
