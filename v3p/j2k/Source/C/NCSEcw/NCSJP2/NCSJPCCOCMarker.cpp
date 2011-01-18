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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCCOCMarker.cpp $
** CREATED:  11/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCCOCMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCCOCMarker.h"
#include "NCSJPC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCCOCMarker::CNCSJPCCOCMarker()
{
	// Initialise the base marker class members
	m_eMarker = COC;

	m_nCcoc = 0;
	m_bScoc = false;
}

/** Copy constructor */
CNCSJPCCOCMarker::CNCSJPCCOCMarker(const class CNCSJPCCODMarker &src)
{
	*this = src;
}


// Destructor
CNCSJPCCOCMarker::~CNCSJPCCOCMarker()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCCOCMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	m_bHaveMarker = true;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		UINT8 t8 = 0;
		NCSJP2_CHECKIO(ReadUINT16(m_nLength));

		if(JPC.m_SIZ.m_nCsiz < 257) {
			NCSJP2_CHECKIO(ReadUINT8(t8));
			m_nCcoc = t8;
		} else {
			NCSJP2_CHECKIO(ReadUINT16(m_nCcoc));
		}
		NCSJP2_CHECKIO(ReadUINT8(t8));
		if(t8 & 0x1) {
			m_bScoc = true;
		}
		Error = m_SPcoc.Parse(JPC, Stream, m_bScoc);			

		if(Error == NCS_SUCCESS) {
			m_bValid = true;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCCOCMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCMarker::UnParse(JPC, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT16(m_nLength));

		if(JPC.m_SIZ.m_nCsiz < 257) {
			NCSJP2_CHECKIO(WriteUINT8((UINT8)m_nCcoc));
		} else {
			NCSJP2_CHECKIO(WriteUINT16(m_nCcoc));
		}
		NCSJP2_CHECKIO(WriteUINT8(m_bScoc ? 0x1 : 0x0));

		Error = m_SPcoc.UnParse(JPC, Stream, m_bScoc);
	NCSJP2_CHECKIO_END();
	return(Error);
}

// Assignment operator
class CNCSJPCCOCMarker& CNCSJPCCOCMarker::operator=( const class CNCSJPCCODMarker& src )
{
	m_nOffset = src.m_nOffset;
	m_nLength = src.m_nLength;
	m_bScoc = src.m_Scod.bDefinedPrecincts;
	m_SPcoc = src.m_SPcod;
	return(*this);
}

