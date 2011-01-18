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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2ResolutionBox.cpp $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2ResolutionBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ResolutionBox::sm_nTBox = 'res\040';

// Constructor
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ResolutionBox::CNCSJP2ResolutionBox()
{
	// Initialise members
	m_nTBox = sm_nTBox;

	// This is a superbox, setup the boxes it can contain, and the order they should appear in.
	SetBoxes(&m_Capture,
			 &m_DefaultDisplay,
			 NULL);
}

// Destructor
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ResolutionBox::~CNCSJP2ResolutionBox()
{
}
