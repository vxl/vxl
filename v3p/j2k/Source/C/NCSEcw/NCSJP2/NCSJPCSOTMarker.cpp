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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCSOTMarker.cpp $
** CREATED:  11/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCSOTMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCSOTMarker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCSOTMarker::CNCSJPCSOTMarker()
{
	// Initialise the base marker class members
	m_eMarker = SOT;
	m_nLength = 10;

	m_nIsot = 0;
	m_nPsot = 0;
	m_nTPsot = 0;
	m_nTNsot = 0;
}

// Destructor
CNCSJPCSOTMarker::~CNCSJPCSOTMarker()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCSOTMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;
	&JPC;//Keep compiler happy

	m_bHaveMarker = true;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(ReadUINT16(m_nLength));
		NCSJP2_CHECKIO(ReadUINT16(m_nIsot));
		NCSJP2_CHECKIO(ReadUINT32(m_nPsot));
		NCSJP2_CHECKIO(ReadUINT8(m_nTPsot));
		NCSJP2_CHECKIO(ReadUINT8(m_nTNsot));

		if(Error == NCS_SUCCESS) {
			m_bValid = true;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCSOTMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCMarker::UnParse(JPC, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT16(m_nLength));
		NCSJP2_CHECKIO(WriteUINT16(m_nIsot));
		NCSJP2_CHECKIO(WriteUINT32(m_nPsot));
		NCSJP2_CHECKIO(WriteUINT8(m_nTPsot));
		NCSJP2_CHECKIO(WriteUINT8(m_nTNsot));
	NCSJP2_CHECKIO_END();
	return(Error);
}
