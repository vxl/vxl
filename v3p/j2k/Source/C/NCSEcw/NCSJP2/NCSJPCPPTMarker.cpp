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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCPPTMarker.cpp $
** CREATED:  19/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCPPTMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCPPTMarker.h"
#include "NCSJPC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCPPTMarker::CNCSJPCPPTMarker()
{
	// Initialise the base marker class members
	m_eMarker = PPT;
	m_nZppt = 0;
}

// Destructor
CNCSJPCPPTMarker::~CNCSJPCPPTMarker()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCPPTMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	&JPC;//Keep compiler happy
	CNCSError Error;

	m_bHaveMarker = true;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(ReadUINT16(m_nLength));
		NCSJP2_CHECKIO(ReadUINT8(m_nZppt));

		NCSJP2_CHECKIO(Seek(m_nLength - (sizeof(UINT16) + sizeof(UINT8))));

		if(Error == NCS_SUCCESS) {
			m_bValid = true;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

#ifndef NCSJPC_LEAN_AND_MEAN
// UnParse the marker out to the stream.
CNCSError CNCSJPCPPTMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCMarker::UnParse(JPC, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT16(m_nLength));
		NCSJP2_CHECKIO(WriteUINT8(m_nZppt));

//		for(UINT32 i = 0; i < m_Headers.size(); i++) {
//			NCSJP2_CHECKIO_ERROR(m_Headers[i].UnParse(JPC, Stream));
//		}
	NCSJP2_CHECKIO_END();
	return(Error);
}
#endif //!NCSJPC_LEAN_AND_MEAN
