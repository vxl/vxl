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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2UUIDBox.cpp $
** CREATED:  04/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2UUIDBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2UUIDBox::sm_nTBox = 'uuid';

// Constructor
CNCSJP2File::CNCSJP2UUIDBox::CNCSJP2UUIDBox()
{
	// Initialise members
	m_nTBox = sm_nTBox;
	m_pData = NULL;
	m_nLength = 0;
}

// Destructor
CNCSJP2File::CNCSJP2UUIDBox::~CNCSJP2UUIDBox()
{
	if(m_pData) {
		delete[] m_pData;
	}
}

// Parse the box from the stream
CNCSError CNCSJP2File::CNCSJP2UUIDBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(Read(m_UUID.m_UUID, sizeof(m_UUID.m_UUID)));
		m_nLength = (INT32)(m_nLDBox - sizeof(m_UUID.m_UUID));
//		m_pData = new UINT8[m_nLength];
//		NCSJP2_CHECKIO(Read(m_pData, m_nLength));
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the box to the stream
CNCSError CNCSJP2File::CNCSJP2UUIDBox::UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	m_nTBox = sm_nTBox;
	m_nXLBox = 8 + m_nLDBox;

	Error = CNCSJP2Box::UnParse(JP2File, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(Write(m_UUID.m_UUID, sizeof(m_UUID.m_UUID)));
		if(m_pData) {
			NCSJP2_CHECKIO(Write(m_pData, (UINT32)m_nLength));
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}
