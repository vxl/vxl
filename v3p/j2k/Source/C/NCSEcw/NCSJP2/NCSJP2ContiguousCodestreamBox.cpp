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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2ContiguousCodestreamBox.cpp $
** CREATED:  04/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope 
** PURPOSE:  NCSJP2ContiguousCodestreamBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2ContiguousCodestreamBox::sm_nTBox = 'jp2c';

// Constructor
CNCSJP2File::CNCSJP2ContiguousCodestreamBox::CNCSJP2ContiguousCodestreamBox()
{
	// Initialise members
	m_nTBox = sm_nTBox;
}

// Destructor
CNCSJP2File::CNCSJP2ContiguousCodestreamBox::~CNCSJP2ContiguousCodestreamBox()
{
}

// Parse the box from the stream
CNCSError CNCSJP2File::CNCSJP2ContiguousCodestreamBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(Seek(m_nDBoxOffset, CNCSJPCIOStream::START));
		NCSJP2_CHECKIO_ERROR(CNCSJPC::Parse(Stream));
		NCSJP2_CHECKIO(Seek(m_nDBoxOffset + m_nLDBox, CNCSJPCIOStream::START));
		CNCSJP2Box::m_bValid = true;
		CNCSJPCMainHeader::m_bValid = true;
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the box to the stream
CNCSError CNCSJP2File::CNCSJP2ContiguousCodestreamBox::UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	m_nTBox = sm_nTBox;
	m_nXLBox = 0;

	Error = CNCSJP2Box::UnParse(JP2File, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO_ERROR(CNCSJPC::UnParse(Stream));
	NCSJP2_CHECKIO_END();
	return(Error);
}
