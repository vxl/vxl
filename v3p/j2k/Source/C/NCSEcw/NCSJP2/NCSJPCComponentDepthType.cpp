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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCComponentDepthType.cpp $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCComponentDepthType class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCComponentDepthType.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCComponentDepthType::CNCSJPCComponentDepthType()
{
	m_nBits = 0;
	m_bSigned = false;
}

// Destructor
CNCSJPCComponentDepthType::~CNCSJPCComponentDepthType()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCComponentDepthType::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	&JPC;//Keep compiler happy
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(ReadUINT8(m_nBits));
	
		if(m_nBits != 255) {
			if(m_nBits & 0x80) {
				m_bSigned = true;
			} else {
				m_bSigned = false;
			}
			m_nBits = (m_nBits & 0x7f) + 1;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCComponentDepthType::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{	
	&JPC;//Keep compiler happy
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		if(m_nBits != 255) {
			UINT8 nSsiz = m_nBits - 1;
			if(m_bSigned) {
				nSsiz |= 0x80;
			}
			NCSJP2_CHECKIO(WriteUINT8(nSsiz));
		} else {
			NCSJP2_CHECKIO(WriteUINT8(255));
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}
