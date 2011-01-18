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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2ColorSpecificationBox.cpp $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2ColorSpecificationBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::sm_nTBox = 'colr';

// Constructor
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::CNCSJP2ColorSpecificationBox()
{
	// Initialise the members
	m_nTBox = sm_nTBox;

	m_eMethod = ENUMERATED_COLORSPACE;
	m_nPrecedence = 0;
	m_nApproximation = 0;
	m_eEnumeratedColorspace = sRGB;
	m_pICCProfile = (void*)NULL;
	m_nICCLen = 0;
}

// Destructor - cleanup
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::~CNCSJP2ColorSpecificationBox()
{
	NCSFree(m_pICCProfile);
}

void CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::UpdateXLBox(void)
{
	CNCSJP2Box::UpdateXLBox();
	m_nXLBox += 3 * sizeof(UINT8);
	if(m_eMethod == ENUMERATED_COLORSPACE) {
		m_nXLBox += sizeof(UINT32);
	} else {
		m_nXLBox += m_nICCLen;
	}
}

// Parse the box in from the stream.
CNCSError CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		// Get the color spec fields in
		UINT8 t8;
		NCSJP2_CHECKIO(ReadUINT8(t8)); m_eMethod = (Type)t8;
		NCSJP2_CHECKIO(ReadUINT8(m_nPrecedence));
		NCSJP2_CHECKIO(ReadUINT8(m_nApproximation));

		if(m_eMethod == ENUMERATED_COLORSPACE) {
			UINT32 t32;
			// Enumerated colorspace, read it in.
			NCSJP2_CHECKIO(ReadUINT32(t32)); 
			m_eEnumeratedColorspace = (EnumeratedType)t32;
		} else if(m_eMethod == RESTRICTED_ICC_PROFILE || m_eMethod == ICC_PROFILE) {
			// Have an ICC profile, need to alloc memory & read it in.
			m_nICCLen = (UINT32)m_nLDBox - 3;
			m_pICCProfile = NCSMalloc(m_nICCLen, TRUE);
			if(m_pICCProfile) {
				NCSJP2_CHECKIO(Read(m_pICCProfile, m_nICCLen));
			} else {
				Error = NCS_COULDNT_ALLOC_MEMORY;
			}
		} else {
			Error = NCS_FILE_INVALID;
		}
		m_bValid = true;
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the color spec box out to the stream.
CNCSError CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

		// Write out the base box first
	Error = CNCSJP2Box::UnParse(JP2File, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
			// Now write out the color spec fields
		NCSJP2_CHECKIO(WriteUINT8(m_eMethod));
		NCSJP2_CHECKIO(WriteUINT8(m_nPrecedence));
		NCSJP2_CHECKIO(WriteUINT8(m_nApproximation));

		if(m_eMethod == ENUMERATED_COLORSPACE) {
			// Enumerated, write out enum value
			NCSJP2_CHECKIO(WriteUINT32(m_eEnumeratedColorspace));
		} else if(m_eMethod == RESTRICTED_ICC_PROFILE) {
			// ICC profile, write that out
			if(m_pICCProfile) {
				NCSJP2_CHECKIO(Write(m_pICCProfile, (UINT32)m_nICCLen));
			} else {
				Error = NCS_FILE_INVALID;
			}
		} else {
			Error = NCS_FILE_INVALID;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}
