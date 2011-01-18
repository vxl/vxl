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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCRGNMarker.cpp $
** CREATED:  12/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCRGNMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCRGNMarker.h"
#include "NCSJPC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCRGNMarker::CNCSJPCRGNMarker()
{
	// Initialise the base marker class members
	m_eMarker = RGN;

	m_nCrgn = 0;
	m_eSrgn = IMPLICIT;
	m_nSPrgn = 0;
}

// Destructor
CNCSJPCRGNMarker::~CNCSJPCRGNMarker()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCRGNMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	m_bHaveMarker = true;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(ReadUINT16(m_nLength));

		UINT8 t8;

		if(JPC.m_SIZ.m_nCsiz < 257) {
			NCSJP2_CHECKIO(ReadUINT8(t8));
			m_nCrgn = t8;
		} else {
			NCSJP2_CHECKIO(ReadUINT16(m_nCrgn));
		}
		
		NCSJP2_CHECKIO(ReadUINT8(t8));
		if(t8 == 0x0) {
			m_eSrgn = IMPLICIT;

			NCSJP2_CHECKIO(ReadUINT8(m_nSPrgn));
		} else {
			Error = NCS_FILEIO_ERROR;
		}

		if(Error == NCS_SUCCESS) {
			m_bValid = true;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

#ifndef NCSJPC_LEAN_AND_MEAN
// UnParse the marker out to the stream.
CNCSError CNCSJPCRGNMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCMarker::UnParse(JPC, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT16(m_nLength));

		if(JPC.m_SIZ.m_nCsiz < 257) {
			NCSJP2_CHECKIO(WriteUINT8((UINT8)m_nCrgn));
		} else {
			NCSJP2_CHECKIO(WriteUINT16(m_nCrgn));
		}
		
		NCSJP2_CHECKIO(WriteUINT8((UINT8)m_eSrgn));
		NCSJP2_CHECKIO(WriteUINT8(m_nSPrgn));
	NCSJP2_CHECKIO_END();
	return(Error);
}
#endif //!NCSJPC_LEAN_AND_MEAN
