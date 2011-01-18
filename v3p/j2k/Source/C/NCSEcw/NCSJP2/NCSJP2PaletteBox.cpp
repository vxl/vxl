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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2PaletteBox.cpp $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2PaletteBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2PaletteBox::sm_nTBox = 'pclr';

// Constructor
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2PaletteBox::CNCSJP2PaletteBox()
{
	// Initialise the members
	m_nTBox = sm_nTBox;

	m_nEntries = 0;
	m_nComponents = 0;
}

// Destructor - cleanup
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2PaletteBox::~CNCSJP2PaletteBox()
{
	for(int e = 0; e < m_nEntries; e++) {
		PaletteEntry pe = m_Entries[e];
		for(int c = 0; c < m_nComponents; c++) {
			NCSFree(pe[c]);
		}
	}
}

// Parse the box in from the stream.
CNCSError CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2PaletteBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		// Get the palette fields in
		NCSJP2_CHECKIO(ReadUINT16(m_nEntries));
		NCSJP2_CHECKIO(ReadUINT8(m_nComponents));

		for(int c = 0; c < m_nComponents; c++) {
			CNCSJPCComponentDepthType depth;

			NCSJP2_CHECKIO_ERROR(depth.Parse(JP2File.m_Codestream, Stream));
			m_Bits.push_back(depth);
		}
		for(int e = 0; e < m_nEntries && Error == NCS_SUCCESS; e++) {
			PaletteEntry pe;

			for(int c = 0; c < m_nComponents; c++) {
				UINT8 nBytes = m_Bits[c].m_nBits / 8 + ((m_Bits[c].m_nBits % 8) ? 1 : 0);
				void *pEntry = NCSMalloc(nBytes, TRUE);
				pe.push_back(pEntry);
				NCSJP2_CHECKIO(Read(pEntry, nBytes)); // FIXME - spec isn't clear on byteorder on the palette entries
			}
			m_Entries.push_back(pe);
		}
		
		m_bValid = true;
	NCSJP2_CHECKIO_END();
	return(Error);
}

#ifndef NCSJPC_LEAN_AND_MEAN
// UnParse the color spec box out to the stream.
CNCSError CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2PaletteBox::UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	// Setup the base box.
	m_nTBox = sm_nTBox;
	m_nXLBox = 8 + sizeof(UINT16) + sizeof(UINT8) + m_nComponents * sizeof(UINT8);
	
	for(int c = 0; c < m_nComponents; c++) {
		UINT8 nBytes = m_Bits[c].m_nBits / 8 + ((m_Bits[c].m_nBits % 8) ? 1 : 0);
		m_nXLBox += nBytes * m_nEntries;
	}
		// Write out the base box first
	Error = CNCSJP2Box::UnParse(JP2File, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);

		// Get the palette fields in
		NCSJP2_CHECKIO(WriteUINT16(m_nEntries));
		NCSJP2_CHECKIO(WriteUINT8(m_nComponents));

		for(int c = 0; c < m_nComponents; c++) {
			m_Bits[c].UnParse(JP2File.m_Codestream, Stream);
		}
		for(int e = 0; e < m_nEntries && Error == NCS_SUCCESS; e++) {
			PaletteEntry pe = m_Entries[e];

			for(int c = 0; c < m_nComponents; c++) {
				UINT8 nBytes = m_Bits[c].m_nBits / 8 + ((m_Bits[c].m_nBits % 8) ? 1 : 0);
				void *pEntry = pe[c];
				NCSJP2_CHECKIO(Write(pEntry, nBytes)); // FIXME - spec isn't clear on byteorder on the palette entries
			}
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}
#endif //!NCSJPC_LEAN_AND_MEAN
