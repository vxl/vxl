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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCPOCMarker.cpp $
** CREATED:  18/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCPOCMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCPOCMarker.h"
#include "NCSJPC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCPOCMarker::CNCSJPCPOCMarker()
{
	// Initialise the base marker class members
	m_eMarker = POC;
	m_nCurProgression = 0;
}

// Destructor
CNCSJPCPOCMarker::~CNCSJPCPOCMarker()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCPOCMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	m_bHaveMarker = true;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(ReadUINT16(m_nLength));

		m_Progressions.clear();

		int nPOC;
		
		if(JPC.m_SIZ.m_nCsiz <257) {
			nPOC = (m_nLength - 2) / 7;	
		} else {
			nPOC = (m_nLength - 2) / 9;
		} 
		for(int c = 0; c < nPOC; c++) {
			ProgressionOrder pc;
			UINT8 t8;

			NCSJP2_CHECKIO(ReadUINT8(pc.m_nRSpoc));
			if(JPC.m_SIZ.m_nCsiz < 257) {
				NCSJP2_CHECKIO(ReadUINT8(t8));
				pc.m_nCSpoc = t8;
			} else {
				NCSJP2_CHECKIO(ReadUINT16(pc.m_nCSpoc));
			}
			NCSJP2_CHECKIO(ReadUINT16(pc.m_nLYEpoc));
			NCSJP2_CHECKIO(ReadUINT8(pc.m_nREpoc));
			if(JPC.m_SIZ.m_nCsiz < 257) {
				NCSJP2_CHECKIO(ReadUINT8(t8));
				pc.m_nCEpoc = (t8 == 0) ? 256 : t8;
			} else {
				NCSJP2_CHECKIO(ReadUINT16(pc.m_nCEpoc));
			}
			NCSJP2_CHECKIO_ERROR(pc.m_Ppoc.Parse(JPC, Stream));
			m_Progressions.push_back(pc);
		}
		if(Error == NCS_SUCCESS) {
			m_bValid = true;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

#ifndef NCSJPC_LEAN_AND_MEAN
// UnParse the marker out to the stream.
CNCSError CNCSJPCPOCMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCMarker::UnParse(JPC, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT16(m_nLength));

		for(UINT32 c = 0; c < m_Progressions.size(); c++) {
			NCSJP2_CHECKIO(WriteUINT8(m_Progressions[c].m_nRSpoc));
			if(JPC.m_SIZ.m_nCsiz < 257) {
				NCSJP2_CHECKIO(WriteUINT8((UINT8)m_Progressions[c].m_nCSpoc));
			} else {
				NCSJP2_CHECKIO(WriteUINT16(m_Progressions[c].m_nCSpoc));
			}
			NCSJP2_CHECKIO(WriteUINT16(m_Progressions[c].m_nLYEpoc));
			NCSJP2_CHECKIO(WriteUINT8(m_Progressions[c].m_nREpoc));
			if(JPC.m_SIZ.m_nCsiz < 257) {
				NCSJP2_CHECKIO(WriteUINT8((UINT8)m_Progressions[c].m_nCEpoc));
			} else {
				NCSJP2_CHECKIO(WriteUINT16(m_Progressions[c].m_nCEpoc));
			}
			NCSJP2_CHECKIO_ERROR(m_Progressions[c].m_Ppoc.UnParse(JPC, Stream));
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}
#endif //!NCSJPC_LEAN_AND_MEAN
