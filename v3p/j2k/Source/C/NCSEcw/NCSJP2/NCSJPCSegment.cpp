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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCSegment.cpp $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCSegment class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCSegment.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCSegment::CNCSJPCSegment() 
{
	m_nIndex = 0;
	m_nPasses = 0;
	m_nLength = 0;
	m_pData = NULL;
}

// Copy constructor
CNCSJPCSegment::CNCSJPCSegment(const CNCSJPCSegment &s) 
{
	m_nIndex = s.m_nIndex;
	m_nPasses = s.m_nPasses;
	m_nLength = s.m_nLength;
	m_pData = s.m_pData;
	((CNCSJPCSegment&)s).m_pData = NULL;
	((CNCSJPCSegment&)s).m_nLength = 0;
}

// Virtual destructor
CNCSJPCSegment::~CNCSJPCSegment()
{
	NCSFree(m_pData);
}
