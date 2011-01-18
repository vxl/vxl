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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCCOMMarker.cpp $
** CREATED:  18/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCCOMMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCCOMMarker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCCOMMarker::CNCSJPCCOMMarker()
{
	// Initialise the base marker class members
	m_eMarker = COM;
	m_pComment = NULL;
}

// Copy Constructor
CNCSJPCCOMMarker::CNCSJPCCOMMarker(const CNCSJPCCOMMarker &s)
{
	// Initialise the base marker class members
	m_eMarker = COM;
	m_pComment = NULL;
	if(s.m_pComment) {
		m_nLength = s.m_nLength;
		m_eRegistration = s.m_eRegistration;
		m_pComment = NCSMalloc(m_nLength - 2 * sizeof(UINT16), FALSE);
		memcpy(m_pComment, s.m_pComment, m_nLength - 2 * sizeof(UINT16));
	}
}

// Destructor
CNCSJPCCOMMarker::~CNCSJPCCOMMarker()
{
	NCSFree(m_pComment);
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCCOMMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	&JPC;//Keep compiler happy
	CNCSError Error;

	m_bHaveMarker = true;
	
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(ReadUINT16(m_nLength));
		UINT16 t16;
		NCSJP2_CHECKIO(ReadUINT16(t16));
		if(t16 == 0) {
			m_eRegistration = BINARY;
		} else if(t16 == 1) {
			m_eRegistration = IS_8859_LATIN;
		} else {
			Error = NCS_FILEIO_ERROR;
		}
		if(Error == NCS_SUCCESS) {
			UINT8 t8;
			for(UINT32 i = 0; i < m_nLength - 2 * sizeof(UINT16); i++) {
				NCSJP2_CHECKIO(ReadUINT8(t8));
			}
			m_bValid = true;
		}
	NCSJP2_CHECKIO_END();
	return(NCS_SUCCESS);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCCOMMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;
	if(m_bValid && m_pComment && (m_nLength - 2 * sizeof(UINT16) > 0)) {
		Error = CNCSJPCMarker::UnParse(JPC, Stream);
		
		NCSJP2_CHECKIO_BEGIN(Error, Stream);
		switch(m_eRegistration) {
			case BINARY:
					NCSJP2_CHECKIO(WriteUINT16(m_nLength));
					NCSJP2_CHECKIO(WriteUINT16(0));
					NCSJP2_CHECKIO(Write((void *)m_pComment, m_nLength - 2 * sizeof(UINT16)));
				break;
			case IS_8859_LATIN:
					NCSJP2_CHECKIO(WriteUINT16(m_nLength));
					NCSJP2_CHECKIO(WriteUINT16(1));
					NCSJP2_CHECKIO(Write((void *)m_pComment, m_nLength - 2 * sizeof(UINT16)));
				break;
		}
		NCSJP2_CHECKIO_END();
	}
	return(NCS_SUCCESS);
}
