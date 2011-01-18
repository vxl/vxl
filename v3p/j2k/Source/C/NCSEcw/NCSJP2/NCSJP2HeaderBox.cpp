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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2HeaderBox.cpp $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2HeaderBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UINT32 CNCSJP2File::CNCSJP2HeaderBox::sm_nTBox = 'jp2h';

// Constructor
CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2HeaderBox()
{
	// Initialise members
	m_nTBox = sm_nTBox;

	// This is a superbox, setup the boxes it can contain, and the order they should appear in.
	SetBoxes(&m_ImageHeader, 
			 &m_BitsPerComponent, 
			 &m_ColorSpecification, 
			 &m_Palette, 
			 &m_ComponentMapping, 
			 &m_ChannelDefinition, 
			 &m_Resolution,
			 NULL);

	m_ImageHeader.SetPrevBoxes(NULL);
	m_ImageHeader.SetNextBoxes(&m_BitsPerComponent, 
							   &m_ColorSpecification, 
							   &m_Palette, 
							   &m_ComponentMapping, 
							   &m_ChannelDefinition, 
							   &m_Resolution,
							   NULL);
	
	m_BitsPerComponent.SetPrevBoxes(&m_ImageHeader, NULL);
	m_BitsPerComponent.SetNextBoxes(NULL);
	
	m_ColorSpecification.SetPrevBoxes(&m_ImageHeader, NULL);
	m_ColorSpecification.SetNextBoxes(NULL);
	
	m_Palette.SetPrevBoxes(&m_ImageHeader, NULL);
	m_Palette.SetNextBoxes(NULL);
	
	m_ComponentMapping.SetPrevBoxes(&m_ImageHeader, NULL);
	m_ComponentMapping.SetNextBoxes(NULL);

	m_ChannelDefinition.SetPrevBoxes(&m_ImageHeader, NULL);
	m_ChannelDefinition.SetNextBoxes(NULL);

	m_Resolution.SetPrevBoxes(&m_ImageHeader, NULL);
	m_Resolution.SetNextBoxes(NULL);
}

// Destructor
CNCSJP2File::CNCSJP2HeaderBox::~CNCSJP2HeaderBox()
{
}
