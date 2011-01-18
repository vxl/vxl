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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCCodingStyleParameter.cpp $
** CREATED:  11/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCCodingStyleParameter class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCCodingStyleParameter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCCodingStyleParameter::CNCSJPCCodingStyleParameter()
{
	m_nLevels = 0;
	m_eTransformation = IRREVERSIBLE_9x7;
	m_nXcb = 0;
	m_nYcb = 0;
	m_Scb = 0;
}

// Destructor
CNCSJPCCodingStyleParameter::~CNCSJPCCodingStyleParameter()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCCodingStyleParameter::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream, bool bDefinedPrecincts)
{
	&JPC;//Keep compiler happy
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(ReadUINT8(m_nLevels));
		NCSJP2_CHECKIO(ReadUINT8(m_nXcb));  m_nXcb += 2;
		NCSJP2_CHECKIO(ReadUINT8(m_nYcb));  m_nYcb += 2;
			
		if(m_nXcb > 10 || m_nYcb > 10 || m_nXcb + m_nYcb > 12) {
			Error = NCS_FILEIO_ERROR;
		} else {
			UINT8 t8;
			// Coding style 
			NCSJP2_CHECKIO(ReadUINT8(m_Scb));
			// Filter type
			NCSJP2_CHECKIO(ReadUINT8(t8));
			if(t8 == 0x0) {
				m_eTransformation = IRREVERSIBLE_9x7;
			} else if(t8 == 0x1) {
				m_eTransformation = REVERSIBLE_5x3;
			} else {
				Error = NCS_FILEIO_ERROR;
			}
			if(Error == NCS_SUCCESS) {
				if(bDefinedPrecincts) {
					for(int l = 0; l < m_nLevels + 1; l++) {
						NCSJP2_CHECKIO(ReadUINT8(t8));
						PrecinctSize sz;
						sz.m_nPPx = t8 & 0x0f;
						sz.m_nPPy = (t8 & 0xf0) >> 4;
	
						m_PrecinctSizes.push_back(sz);
					}
				}
			}
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCCodingStyleParameter::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream, bool bDefinedPrecincts)
{
	&JPC;//Keep compiler happy
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT8(m_nLevels));
		NCSJP2_CHECKIO(WriteUINT8(m_nXcb - 2));
		NCSJP2_CHECKIO(WriteUINT8(m_nYcb - 2));
		NCSJP2_CHECKIO(WriteUINT8(m_Scb));
				
		UINT8 t8;
		switch(m_eTransformation) {
			case IRREVERSIBLE_9x7: NCSJP2_CHECKIO(WriteUINT8(0x0)); break;
			case REVERSIBLE_5x3:  NCSJP2_CHECKIO(WriteUINT8(0x1)); break;
			default:
					Error = NCS_FILEIO_ERROR;
				break;
		}
		if(Error == NCS_SUCCESS) {
			if(bDefinedPrecincts) {
				for(int l = 0; l < m_nLevels + 1; l++) {
					t8 = (m_PrecinctSizes[l].m_nPPx & 0x0f) | ((m_PrecinctSizes[l].m_nPPy & 0xf) << 4);
					NCSJP2_CHECKIO(WriteUINT8(t8));
				}
			}
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}
