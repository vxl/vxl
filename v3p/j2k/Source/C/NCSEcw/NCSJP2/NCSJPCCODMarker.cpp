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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCCODMarker.cpp $
** CREATED:  11/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCCODMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCCODMarker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCCODMarker::CNCSJPCCODMarker()
{
	// Initialise the base marker class members
	m_eMarker = COD;

	m_Scod.bDefinedPrecincts = false;
	m_Scod.bEPHMarkers = false;
	m_Scod.bSOPMarkers = false;

	m_SGcod.m_bMCT = false;
	m_SGcod.m_nLayers = 0;
}

// Destructor
CNCSJPCCODMarker::~CNCSJPCCODMarker()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCCODMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	m_bHaveMarker = true;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(ReadUINT16(m_nLength));
		UINT8 t8;
		NCSJP2_CHECKIO(ReadUINT8(t8));
		if(t8 & (0x01 << 0)) {
			m_Scod.bDefinedPrecincts = true;
		}
		if(t8 & (0x01 << 1)) {
			m_Scod.bSOPMarkers = true;
		}
		if(t8 & (0x01 << 2)) {
			m_Scod.bEPHMarkers = true;
		}

		Error = m_SGcod.m_ProgressionOrder.Parse(JPC, Stream);
		if(Error == NCS_SUCCESS) {
			NCSJP2_CHECKIO(ReadUINT16(m_SGcod.m_nLayers));
			NCSJP2_CHECKIO(ReadUINT8(t8));
			if(t8) {
				m_SGcod.m_bMCT = true;
			} else {
				m_SGcod.m_bMCT = false;
			}
			Error = m_SPcod.Parse(JPC, Stream, m_Scod.bDefinedPrecincts);			

			if(Error == NCS_SUCCESS) {
				m_bValid = true;
			}
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCCODMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCMarker::UnParse(JPC, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT16(m_nLength));

		UINT8 t8 = 0;
		if(m_Scod.bDefinedPrecincts) {
			t8 |= 0x01 << 0;
		}
		if(m_Scod.bSOPMarkers) {
			t8 |= 0x01 << 1;
		}
		if(m_Scod.bEPHMarkers) {
			t8 |= 0x01 << 2;
		}
		NCSJP2_CHECKIO(WriteUINT8(t8));
		Error = m_SGcod.m_ProgressionOrder.UnParse(JPC, Stream);
		if(Error == NCS_SUCCESS) {
			NCSJP2_CHECKIO(WriteUINT16(m_SGcod.m_nLayers));
			NCSJP2_CHECKIO(WriteUINT8(m_SGcod.m_bMCT ? 0x1 : 0x0));
			Error = m_SPcod.UnParse(JPC, Stream, m_Scod.bDefinedPrecincts);
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// Assignment operator
class CNCSJPCCODMarker& CNCSJPCCODMarker::operator=( const class CNCSJPCCOCMarker& src )
{
	m_nOffset = src.m_nOffset;
	m_nLength = src.m_nLength;
	m_Scod.bDefinedPrecincts = src.m_bScoc;
	m_SPcod = src.m_SPcoc;
	return(*this);
}
