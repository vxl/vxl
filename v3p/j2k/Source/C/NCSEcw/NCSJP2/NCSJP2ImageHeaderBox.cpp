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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2ImageHeaderBox.cpp $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2ImageHeaderBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ImageHeaderBox::sm_nTBox = 'ihdr';

// Constructor
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ImageHeaderBox::CNCSJP2ImageHeaderBox()
{
	// Initialise the members
	m_nTBox = sm_nTBox;

	m_nWidth = 0;
	m_nHeight = 0;
	m_nComponents = 0;
	m_nCompressionType = 0;
	m_nUnknownColorspace = 0;
	m_nIPR = 0;
}

// Destructor
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ImageHeaderBox::~CNCSJP2ImageHeaderBox()
{
}

void CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ImageHeaderBox::UpdateXLBox(void)
{
	CNCSJP2Box::UpdateXLBox();
	m_nXLBox += 2 * sizeof(UINT32) + sizeof(UINT16) + sizeof(UINT8) + 3 * sizeof(UINT8);
}

// Parse in the box from the stream.
CNCSError CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ImageHeaderBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		// Read in the ImageHeader members
		NCSJP2_CHECKIO(ReadUINT32(m_nHeight));
		NCSJP2_CHECKIO(ReadUINT32(m_nWidth));
		NCSJP2_CHECKIO(ReadUINT16(m_nComponents));
		CNCSJPCComponentDepthType::Parse(JP2File.m_Codestream, Stream);
		NCSJP2_CHECKIO(ReadUINT8(m_nCompressionType));
		NCSJP2_CHECKIO(ReadUINT8(m_nUnknownColorspace));
		NCSJP2_CHECKIO(ReadUINT8(m_nIPR));
		
		m_bValid = true;
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the box to the stream
CNCSError CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ImageHeaderBox::UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	m_nWidth = JP2File.m_FileInfo.nSizeX;
	m_nHeight = JP2File.m_FileInfo.nSizeY;
	m_nComponents = JP2File.m_FileInfo.nBands;
	bool bNeedBPCBox = false;

	for(UINT32 b = 1; b < JP2File.m_FileInfo.nBands && JP2File.m_FileInfo.pBands; b++) {
		if(JP2File.m_FileInfo.pBands[b].nBits != JP2File.m_FileInfo.pBands[0].nBits ||
		   JP2File.m_FileInfo.pBands[b].bSigned != JP2File.m_FileInfo.pBands[0].bSigned) {
			bNeedBPCBox = true;
		}
	}
	if(bNeedBPCBox) {
		m_nBits = 255;
	} else {
		m_nBits = JP2File.m_FileInfo.pBands ? JP2File.m_FileInfo.pBands[0].nBits : 8;
	}
	m_bSigned = JP2File.m_FileInfo.pBands ? (JP2File.m_FileInfo.pBands[0].bSigned ? true : false) : false;
	m_nCompressionType = 7;
	m_nUnknownColorspace = 0;
	m_nIPR = 0;
	
		// Write out the base box first
	Error = CNCSJP2Box::UnParse(JP2File, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		// Write out the Imageheader members
		NCSJP2_CHECKIO(WriteUINT32(m_nHeight));
		NCSJP2_CHECKIO(WriteUINT32(m_nWidth));
		NCSJP2_CHECKIO(WriteUINT16(m_nComponents));
		CNCSJPCComponentDepthType::UnParse(JP2File.m_Codestream, Stream);
		NCSJP2_CHECKIO(WriteUINT8(m_nCompressionType));
		NCSJP2_CHECKIO(WriteUINT8(m_nUnknownColorspace));
		NCSJP2_CHECKIO(WriteUINT8(m_nIPR));
	NCSJP2_CHECKIO_END();
	return(Error);
}
