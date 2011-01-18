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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCCRGMarker.cpp $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCCRGMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCCRGMarker.h"
#include "NCSJPC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCCRGMarker::CNCSJPCCRGMarker()
{
	// Initialise the base marker class members
	m_eMarker = CRG;
}

// Destructor
CNCSJPCCRGMarker::~CNCSJPCCRGMarker()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCCRGMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	m_bHaveMarker = true;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(ReadUINT16(m_nLength));

		for(int i = 0; i < JPC.m_SIZ.m_nCsiz; i++) {
			Registration reg;
			NCSJP2_CHECKIO(ReadUINT16(reg.m_nXcrg));
			NCSJP2_CHECKIO(ReadUINT16(reg.m_nYcrg));
			m_Offsets.push_back(reg);
		}
		if(Error == NCS_SUCCESS) {
			m_bValid = true;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

#ifndef NCSJPC_LEAN_AND_MEAN
// UnParse the marker out to the stream.
CNCSError CNCSJPCCRGMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCMarker::UnParse(JPC, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT16(m_nLength));
		
		for(int i = 0; i < JPC.m_SIZ.m_nCsiz; i++) {
			NCSJP2_CHECKIO(WriteUINT16(m_Offsets[i].m_nXcrg));
			NCSJP2_CHECKIO(WriteUINT16(m_Offsets[i].m_nYcrg));
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}
#endif //!NCSJPC_LEAN_AND_MEAN
