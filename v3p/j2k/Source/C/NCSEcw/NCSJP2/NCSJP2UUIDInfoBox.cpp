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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2UUIDInfoBox.cpp $
** CREATED:  04/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2UUIDInfoBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2UUIDInfoBox::sm_nTBox = 'uinf';

// Constructor
CNCSJP2File::CNCSJP2UUIDInfoBox::CNCSJP2UUIDInfoBox()
{
	// Initialise members
	m_nTBox = sm_nTBox;

	// This is a superbox, setup the boxes it can contain, and the order they should appear in.
	SetBoxes(&m_List,
			 &m_DE,
			 NULL);
}

// Destructor
CNCSJP2File::CNCSJP2UUIDInfoBox::~CNCSJP2UUIDInfoBox()
{
}
