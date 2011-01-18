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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCProgressionOrderType.cpp $
** CREATED:  18/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCProgressionOrderType class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCProgressionOrderType.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCProgressionOrderType::CNCSJPCProgressionOrderType()
{
	// Initialise the base marker class members
	m_eType = LRCP;
}

// Destructor
CNCSJPCProgressionOrderType::~CNCSJPCProgressionOrderType()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCProgressionOrderType::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	&JPC;//Keep compiler happy
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		UINT8 t8;

		NCSJP2_CHECKIO(ReadUINT8(t8));
		switch(t8) {
			case 0x0: m_eType = LRCP; break;
			case 0x1: m_eType = RLCP; break;
			case 0x2: m_eType = RPCL; break;
			case 0x3: m_eType = PCRL; break;
			case 0x4: m_eType = CPRL; break;
			default: Error = NCS_FILEIO_ERROR; break;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCProgressionOrderType::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	&JPC;//Keep compiler happy
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		UINT8 t8 = 0;
		
		switch(m_eType) {
			case LRCP: t8 = 0x0;	break;
			case RLCP: t8 = 0x1;	break;
			case RPCL: t8 = 0x2;	break;
			case PCRL: t8 = 0x3;	break;
			case CPRL: t8 = 0x4;	break;
		}
		NCSJP2_CHECKIO(WriteUINT8(t8));
	NCSJP2_CHECKIO_END();
	return(Error);
}
