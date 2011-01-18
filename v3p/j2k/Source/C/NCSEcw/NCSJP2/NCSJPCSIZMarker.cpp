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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCSIZMarker.cpp $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCSIZMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCSIZMarker.h"
#include "NCSUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCSIZMarker::CNCSJPCSIZMarker()
{
	// Initialise the base marker class members
	m_eMarker = SIZ;

	m_nRsiz = 0;
	m_nXsiz = 0;
	m_nYsiz = 0;
	m_nXOsiz = 0;
	m_nYOsiz = 0;
	m_nXTsiz = 0;
	m_nYTsiz = 0;
	m_nXTOsiz = 0;
	m_nYTOsiz = 0;
	m_nCsiz = 0;
}

// Destructor
CNCSJPCSIZMarker::~CNCSJPCSIZMarker()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCSIZMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	m_bHaveMarker = true;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(ReadUINT16(m_nLength));
		NCSJP2_CHECKIO(ReadUINT16(m_nRsiz));
		NCSJP2_CHECKIO(ReadUINT32(m_nXsiz));
		NCSJP2_CHECKIO(ReadUINT32(m_nYsiz));
		NCSJP2_CHECKIO(ReadUINT32(m_nXOsiz));
		NCSJP2_CHECKIO(ReadUINT32(m_nYOsiz));
		NCSJP2_CHECKIO(ReadUINT32(m_nXTsiz));
		NCSJP2_CHECKIO(ReadUINT32(m_nYTsiz));
		NCSJP2_CHECKIO(ReadUINT32(m_nXTOsiz));
		NCSJP2_CHECKIO(ReadUINT32(m_nYTOsiz));
		NCSJP2_CHECKIO(ReadUINT16(m_nCsiz));

		for(int c = 0; c < m_nCsiz; c++) {
			ComponentInfo ci;

			NCSJP2_CHECKIO_ERROR(ci.Parse(JPC, Stream));
			m_Components.push_back(ci);
		}
		if(Error == NCS_SUCCESS) {
			m_bValid = true;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCSIZMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCMarker::UnParse(JPC, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT16(m_nLength));
		NCSJP2_CHECKIO(WriteUINT16(m_nRsiz));
		NCSJP2_CHECKIO(WriteUINT32(m_nXsiz));
		NCSJP2_CHECKIO(WriteUINT32(m_nYsiz));
		NCSJP2_CHECKIO(WriteUINT32(m_nXOsiz));
		NCSJP2_CHECKIO(WriteUINT32(m_nYOsiz));
		NCSJP2_CHECKIO(WriteUINT32(m_nXTsiz));
		NCSJP2_CHECKIO(WriteUINT32(m_nYTsiz));
		NCSJP2_CHECKIO(WriteUINT32(m_nXTOsiz));
		NCSJP2_CHECKIO(WriteUINT32(m_nYTOsiz));
		NCSJP2_CHECKIO(WriteUINT16(m_nCsiz));

		for(int c = 0; c < m_nCsiz; c++) {
			NCSJP2_CHECKIO_ERROR(m_Components[c].UnParse(JPC, Stream));
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// Get X0, the component top left in reference grid
INT32 CNCSJPCSIZMarker::GetComponentX0(INT32 iComponent)
{
	return(NCSCeilDiv(m_nXOsiz, m_Components[iComponent].m_nXRsiz));
}

// Get X1, the component bottom right in reference grid
INT32 CNCSJPCSIZMarker::GetComponentX1(INT32 iComponent)
{
	return(NCSCeilDiv(m_nXsiz, m_Components[iComponent].m_nXRsiz));
}

// Get Y0, the component top left in reference grid
INT32 CNCSJPCSIZMarker::GetComponentY0(INT32 iComponent)
{
	return(NCSCeilDiv(m_nYOsiz, m_Components[iComponent].m_nYRsiz));
}

// Get Y1, the component bottom right in reference grid
INT32 CNCSJPCSIZMarker::GetComponentY1(INT32 iComponent)
{
	return(NCSCeilDiv(m_nYsiz, m_Components[iComponent].m_nYRsiz));
}

// Get the component width in reference grid
INT32 CNCSJPCSIZMarker::GetComponentWidth(INT32 iComponent)
{
	return(GetComponentX1(iComponent) - GetComponentX0(iComponent));
}

// Get the component height in reference grid
INT32 CNCSJPCSIZMarker::GetComponentHeight(INT32 iComponent)
{
	return(GetComponentY1(iComponent) - GetComponentY0(iComponent));
}

// Get NumXTiles
INT32 CNCSJPCSIZMarker::GetNumXTiles()
{
	return(NCSCeilDiv((m_nXsiz - m_nXTOsiz), m_nXTsiz));
}

// Get NumYTiles
INT32 CNCSJPCSIZMarker::GetNumYTiles()
{
	return(NCSCeilDiv((m_nYsiz - m_nYTOsiz), m_nYTsiz));
}

// Get the Tile P index from the tile index
INT32 CNCSJPCSIZMarker::GetTilePFromIndex(INT32 iIndex)
{
	return(iIndex % GetNumXTiles());
}

// Get the Tile Q index from the tile index
INT32 CNCSJPCSIZMarker::GetTileQFromIndex(INT32 iIndex)
{
	return(iIndex / GetNumXTiles());
}

// Get the Tile Index from the tile PQ index
INT32 CNCSJPCSIZMarker::GetTileIndexFromPQ(INT32 iPIndex, INT32 iQIndex)
{
	return(iPIndex + iQIndex * GetNumXTiles());
}

// Constructor
CNCSJPCSIZMarker::ComponentInfo::ComponentInfo()
{
	m_nXRsiz = 1;
	m_nYRsiz = 1;
}

// Destructor
CNCSJPCSIZMarker::ComponentInfo::~ComponentInfo()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCSIZMarker::ComponentInfo::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCComponentDepthType::Parse(JPC, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(ReadUINT8(m_nXRsiz));
		NCSJP2_CHECKIO(ReadUINT8(m_nYRsiz));
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCSIZMarker::ComponentInfo::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCComponentDepthType::UnParse(JPC, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT8(m_nXRsiz));
		NCSJP2_CHECKIO(WriteUINT8(m_nYRsiz));
	NCSJP2_CHECKIO_END();
	return(Error);
}
