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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2SignatureBox.cpp $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2SignatureBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2SignatureBox::sm_nTBox = 'jP  ';
UINT32 CNCSJP2File::CNCSJP2SignatureBox::sm_JP2Signature = 0x0d0a870a;//'\r\n\x87\n';

// Constructor
CNCSJP2File::CNCSJP2SignatureBox::CNCSJP2SignatureBox()
{
	// Initialise members
	m_nTBox = sm_nTBox;
	m_nSignature = sm_JP2Signature;
}

// Destructor
CNCSJP2File::CNCSJP2SignatureBox::~CNCSJP2SignatureBox()
{
}

void CNCSJP2File::CNCSJP2SignatureBox::UpdateXLBox(void)
{
	CNCSJP2Box::UpdateXLBox();
	m_nXLBox += sizeof(UINT32);
}


// Parse the box from the stream
CNCSError CNCSJP2File::CNCSJP2SignatureBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
//		UINT8 buf[4];
		// Read in the signature
//		NCSJP2_CHECKIO(Read(buf, 4));
		UINT32 buf;
		NCSJP2_CHECKIO(ReadUINT32(buf));
		
		if(memcmp(&sm_JP2Signature, &buf, sizeof(buf)) == 0) {
			// Signature is valid
			m_bValid = true;
		} else {
			Error = NCS_FILE_INVALID;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the box to the stream
CNCSError CNCSJP2File::CNCSJP2SignatureBox::UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJP2Box::UnParse(JP2File, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT32(sm_JP2Signature));
	NCSJP2_CHECKIO_END();
	return(Error);
}
