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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCMarker.cpp $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCMarker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCMarker::CNCSJPCMarker()
{
	// Initialise the base marker class members
	m_eMarker = UNKNOWN;
	m_nOffset = 0;
	m_nLength = 0;
	m_bHaveMarker = false;
	m_bValid = false;
}

// Destructor
CNCSJPCMarker::~CNCSJPCMarker()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	&JPC;//Keep compiler happy
		// Marker.
	UINT16 t16;

	m_nOffset = Stream.Tell();

	if(Stream.ReadUINT16(t16)) {
		m_eMarker = (Type)t16;
		m_bHaveMarker = true;
	}
	return(Stream.GetError());
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	&JPC;//Keep compiler happy
	m_nOffset = Stream.Tell();
	Stream.WriteUINT16((UINT16)m_eMarker);
	return(Stream.GetError());
}

// Assignment operator
//CNCSJPCMarker& CNCSJPCMarker::operator=( const CNCSJPCMarker& src )
void CNCSJPCMarker::operator=( const class CNCSJPCMarker& src )
{
	m_eMarker = src.m_eMarker;
	m_nOffset = src.m_nOffset;
	m_nLength = src.m_nLength;
	m_bHaveMarker = src.m_bHaveMarker;
	m_bValid = src.m_bValid;
//	return(*this);
}
