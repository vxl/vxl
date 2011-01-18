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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCTLMMarker.cpp $
** CREATED:  18/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCTLMMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCTLMMarker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCTLMMarker::CNCSJPCTLMMarker()
{
	// Initialise the base marker class members
	m_eMarker = TLM;
	m_nZtlm = 0;
	m_Stlm.m_nSP = 0;
	m_Stlm.m_nST = 0;
}

// Destructor
CNCSJPCTLMMarker::~CNCSJPCTLMMarker()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCTLMMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;
	&JPC;//Keep compiler happy
	m_bHaveMarker = true;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(ReadUINT16(m_nLength));
		NCSJP2_CHECKIO(ReadUINT8(m_nZtlm));

		UINT8 t8;

		NCSJP2_CHECKIO(ReadUINT8(t8));
		m_Stlm.m_nSP = t8 >> 6;
		m_Stlm.m_nST = (t8 >> 4) & 0x3;
	
		UINT16 nSegs = 0;

		if(m_Stlm.m_nSP == 0) {
			if(m_Stlm.m_nST == 0) {
				nSegs = (m_nLength - 4) / 2;
			} else if(m_Stlm.m_nST == 1) {
				nSegs = (m_nLength - 4) / 3;
			} else {
				nSegs = (m_nLength - 4) / 4;
			}
		} else {
			if(m_Stlm.m_nST == 0) {
				nSegs = (m_nLength - 4) / 4;
			} else if(m_Stlm.m_nST == 1) {
				nSegs = (m_nLength - 4) / 5;
			} else {
				nSegs = (m_nLength - 4) / 6;
			}
		}
		for(int c = 0; c < nSegs; c++) {
			PointerSegment seg = { 0, 0 };

			if(m_Stlm.m_nST == 1) {
				NCSJP2_CHECKIO(ReadUINT8(t8));
				seg.m_nTtlm = t8;
			} else if(m_Stlm.m_nST == 2) {
				NCSJP2_CHECKIO(ReadUINT16(seg.m_nTtlm));
			}
			if(m_Stlm.m_nSP == 0) {
				UINT16 t16;
				NCSJP2_CHECKIO(ReadUINT16(t16));
				seg.m_nPtlm = t16;
			} else if(m_Stlm.m_nSP == 1) {
				NCSJP2_CHECKIO(ReadUINT32(seg.m_nPtlm));
			}
			m_Pointers.push_back(seg);
		}
		if(Error == NCS_SUCCESS) {
			m_bValid = true;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCTLMMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCMarker::UnParse(JPC, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT16(m_nLength));
		NCSJP2_CHECKIO(WriteUINT8(m_nZtlm));
		NCSJP2_CHECKIO(WriteUINT8((m_Stlm.m_nSP << 6) | (m_Stlm.m_nST << 4)));
	
		for(UINT32 c = 0; c < m_Pointers.size(); c++) {
			if(m_Stlm.m_nST == 1) {
				NCSJP2_CHECKIO(WriteUINT8((UINT8)m_Pointers[c].m_nTtlm));
			} else if(m_Stlm.m_nST == 2) {
				NCSJP2_CHECKIO(WriteUINT16(m_Pointers[c].m_nTtlm));
			}
			if(m_Stlm.m_nSP == 0) {
				NCSJP2_CHECKIO(WriteUINT16((UINT16)m_Pointers[c].m_nPtlm));
			} else if(m_Stlm.m_nSP == 1) {
				NCSJP2_CHECKIO(WriteUINT32(m_Pointers[c].m_nPtlm));
			}
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}
