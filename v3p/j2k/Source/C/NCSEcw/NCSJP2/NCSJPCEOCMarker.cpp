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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCEOCMarker.cpp $
** CREATED:  11/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCEOCMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCEOCMarker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCEOCMarker::CNCSJPCEOCMarker()
{
	// Initialise the base marker class members
	m_eMarker = EOC;
	m_nLength = 2;
}

// Destructor
CNCSJPCEOCMarker::~CNCSJPCEOCMarker()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCEOCMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	&JPC;&Stream;//Keep compiler happy
	m_bHaveMarker = true;
	m_bValid = true;
	return(NCS_SUCCESS);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCEOCMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	return(CNCSJPCMarker::UnParse(JPC, Stream));
}
