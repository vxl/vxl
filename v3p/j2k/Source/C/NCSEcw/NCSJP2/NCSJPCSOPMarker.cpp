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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCSOPMarker.cpp $
** CREATED:  18/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCSOPMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCSOPMarker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCSOPMarker::CNCSJPCSOPMarker()
{
	// Initialise the base marker class members
	m_eMarker = SOP;
	m_nLength = 0;
}

// Destructor
CNCSJPCSOPMarker::~CNCSJPCSOPMarker()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCSOPMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCMarker::Parse(JPC, Stream);
	if(Error == NCS_SUCCESS && m_eMarker == SOP) {
		m_bHaveMarker = true;

		NCSJP2_CHECKIO_BEGIN(Error, Stream);
			NCSJP2_CHECKIO(ReadUINT16(m_nLength));
			NCSJP2_CHECKIO(ReadUINT16(m_nNsop));
			m_bValid = true;
		NCSJP2_CHECKIO_END();
	}
	return(NCS_SUCCESS);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCSOPMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	m_nLength = 4;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO_ERROR(CNCSJPCMarker::UnParse(JPC, Stream));

		NCSJP2_CHECKIO(WriteUINT16(m_nLength));
		NCSJP2_CHECKIO(WriteUINT16(m_nNsop));
	NCSJP2_CHECKIO_END();
	return(Error);
}
