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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCSOCMarker.cpp $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCSOCMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCSOCMarker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCSOCMarker::CNCSJPCSOCMarker()
{
	// Initialise the base marker class members
	m_eMarker = SOC;
	m_nLength = 2;
}

// Destructor
CNCSJPCSOCMarker::~CNCSJPCSOCMarker()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCSOCMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	&JPC;&Stream;//Keep compiler happy
	m_bHaveMarker = true;
	m_bValid = true;
	return(NCS_SUCCESS);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCSOCMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	return(CNCSJPCMarker::UnParse(JPC, Stream));
}
