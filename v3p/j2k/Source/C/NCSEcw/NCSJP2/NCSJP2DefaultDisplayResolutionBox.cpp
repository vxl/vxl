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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2DefaultDisplayResolutionBox.cpp $
** CREATED:  04/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2DefaultDisplayResolutionBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ResolutionBox::CNCSJP2DefaultDisplayResolutionBox::sm_nTBox = 'resd';

// Constructor
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ResolutionBox::CNCSJP2DefaultDisplayResolutionBox::CNCSJP2DefaultDisplayResolutionBox()
{
	// Initialise the members
	m_nTBox = sm_nTBox;
}

// Destructor - cleanup
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ResolutionBox::CNCSJP2DefaultDisplayResolutionBox::~CNCSJP2DefaultDisplayResolutionBox()
{
}
