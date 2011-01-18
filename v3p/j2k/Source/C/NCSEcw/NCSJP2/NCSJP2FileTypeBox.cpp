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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2FileTypeBox.cpp $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2FileTypeBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2FileTypeBox::sm_nTBox = 'ftyp';
UINT32 CNCSJP2File::CNCSJP2FileTypeBox::sm_JP2Brand = 0x6a703220;//'jp2\040';

// Constructor
CNCSJP2File::CNCSJP2FileTypeBox::CNCSJP2FileTypeBox()
{
	// Initialise members
	m_nTBox = sm_nTBox;
	m_Brand = sm_JP2Brand;//'jpx\040';	// JPX Brand
	m_nMinV = 0;
	m_CLList.push_back(sm_JP2Brand);	// JPEG2000 Part 1 Compatible
	m_CLList.push_back(0x4a325031/*'J2P1'*/);			// JPEG2000 Part 1 Profile 1 Compliant Codestream
}

// Destructor
CNCSJP2File::CNCSJP2FileTypeBox::~CNCSJP2FileTypeBox()
{
}

void CNCSJP2File::CNCSJP2FileTypeBox::UpdateXLBox(void)
{
	CNCSJP2Box::UpdateXLBox();
	m_nXLBox += (2 + m_CLList.size()) * sizeof(UINT32);
}

// Parse the box in from the stream.
CNCSError CNCSJP2File::CNCSJP2FileTypeBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	m_CLList.clear();

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		// Read in the "brand"
		NCSJP2_CHECKIO(ReadUINT32(m_Brand));

		if(m_Brand == sm_JP2Brand) {
			// Brand is OK
			m_bValid = true;
		}
		// Get the minimim version			
		NCSJP2_CHECKIO(ReadUINT32(m_nMinV));

		// Now read in the "compatibility list".
		// Even if the "brand" is not valid, we may still be able to read the file as a JP2
		// eg, a JPX file.
		UINT32 nCLEntries = (UINT32)((m_nLDBox - 2 * sizeof(UINT32)) / sizeof(UINT32));
				
		// Must be at least one.
		if(nCLEntries >= 1) {
			for(UINT32 i = 0; i < nCLEntries; i++) {
				CNCSJP2CLEntry CL;
								
				if(Stream.ReadUINT32(CL) == false) {
					Error = Stream.GetError();
					break;
				}
				if(CL == sm_JP2Brand) {
					// OK, we can read it!
					m_bValid = true;
				}
				m_CLList.push_back(CL);
			}
		} else {
			Error = NCS_FILE_INVALID;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the box to the stream.
CNCSError CNCSJP2File::CNCSJP2FileTypeBox::UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	// write out the base box
	Error = CNCSJP2Box::UnParse(JP2File, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		// Write out the brand
		NCSJP2_CHECKIO(WriteUINT32(m_Brand));
		NCSJP2_CHECKIO(WriteUINT32(m_nMinV));
		
		CNCSJP2CLList::iterator pCur = m_CLList.begin();

		// Now the compatibility list.
		while(pCur != m_CLList.end()) {
			CNCSJP2CLEntry CL;
			CL = *pCur;
			if(Stream.WriteUINT32(CL) == false) {
				Error = Stream.GetError();
				break;
			}
			pCur++;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}
