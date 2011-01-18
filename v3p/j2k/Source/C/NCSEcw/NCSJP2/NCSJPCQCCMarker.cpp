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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCQCCMarker.cpp $
** CREATED:  13/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCQCCMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCQCCMarker.h"
#include "NCSJPC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCQCCMarker::CNCSJPCQCCMarker()
{
	// Initialise the base marker class members
	m_eMarker = QCC;
}

CNCSJPCQCCMarker::CNCSJPCQCCMarker(const class CNCSJPCQCDMarker& src )
{
	*this = src;
}

// Destructor
CNCSJPCQCCMarker::~CNCSJPCQCCMarker()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCQCCMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	m_bHaveMarker = true;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		UINT8 t8;
		INT32 nLength;
		NCSJP2_CHECKIO(ReadUINT16(m_nLength));	
		if(JPC.m_SIZ.m_nCsiz < 257) {
			NCSJP2_CHECKIO(ReadUINT8(t8));
			m_nCqcc = t8;
			nLength = m_nLength - 1;
		} else {
			NCSJP2_CHECKIO(ReadUINT16(m_nCqcc));
			nLength = m_nLength - 2;
		}
		NCSJP2_CHECKIO_ERROR(((CNCSJPCQuantizationParameter*)this)->Parse(JPC, Stream, nLength));

		if(Error == NCS_SUCCESS) {
			m_bValid = true;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCQCCMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCMarker::UnParse(JPC, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT16(m_nLength));
		if(JPC.m_SIZ.m_nCsiz < 257) {
			NCSJP2_CHECKIO(WriteUINT8((UINT8)m_nCqcc));
		} else {
			NCSJP2_CHECKIO(WriteUINT16(m_nCqcc));
		}
		NCSJP2_CHECKIO_ERROR(CNCSJPCQuantizationParameter::UnParse(JPC, Stream));
	NCSJP2_CHECKIO_END();
	return(Error);
}

// Assignment operator
class CNCSJPCQCCMarker& CNCSJPCQCCMarker::operator=( const class CNCSJPCQCDMarker& src )
{
	*(CNCSJPCQuantizationParameter*)this = src;
	return(*this);
}
