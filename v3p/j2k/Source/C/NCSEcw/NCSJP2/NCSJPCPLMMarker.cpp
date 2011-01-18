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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCPLMMarker.cpp $
** CREATED:  18/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCPLMMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCPLMMarker.h"
#include "NCSJPC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCPLMMarker::CNCSJPCPLMMarker()
{
	// Initialise the base marker class members
	m_eMarker = PLM;
	m_nZplm = 0;
}

// Destructor
CNCSJPCPLMMarker::~CNCSJPCPLMMarker()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCPLMMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	m_bHaveMarker = true;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(ReadUINT16(m_nLength));
		NCSJP2_CHECKIO(ReadUINT8(m_nZplm));

		INT32 nLen = m_nLength - sizeof(UINT16) - sizeof(UINT8);
		INT64 nBegin = Stream.Tell();

		while(Stream.Tell() < (nBegin + nLen) && Error == NCS_SUCCESS) {
			if(JPC.m_PLMs.size() != 0 && JPC.m_PLMs.back().m_TileParts.size() != 0 && JPC.m_PLMs.back().m_TileParts.back().m_nNplm != 0) {
				// Nead to complete previous PLM...
				CNCSJPCPacketLengthType length;
				while(Stream.Tell() < (nBegin + nLen) && JPC.m_PLMs.back().m_TileParts.back().m_nNplm-- && Error == NCS_SUCCESS) {
					if(length.Parse(Stream)) {
						JPC.m_PLMs.back().m_TileParts.back().m_Lengths.push_back(length);
					} else {
						Error = Stream;
						break;
					}
				}
			} else {
				TilePartPacketLength temp;

				NCSJP2_CHECKIO(ReadUINT8(temp.m_nNplm));
				CNCSJPCPacketLengthType length;

				while(Stream.Tell() < (nBegin + nLen) && temp.m_nNplm-- && Error == NCS_SUCCESS) {
					if(length.Parse(Stream)) {
						temp.m_Lengths.push_back(length);
					} else {
						Error = Stream;
						break;
					}
				}
				m_TileParts.push_back(temp);
			}
		}
		if(Error == NCS_SUCCESS) {
			m_bValid = true;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

#ifndef NCSJPC_LEAN_AND_MEAN
// UnParse the marker out to the stream.
CNCSError CNCSJPCPLMMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCMarker::UnParse(JPC, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		UINT64 nBegin = Stream.Tell();
		NCSJP2_CHECKIO(WriteUINT16(m_nLength));
		NCSJP2_CHECKIO(WriteUINT8(m_nZplm));

		for(UINT32 tp = 0; tp < m_TileParts.size(); tp++) {
			UINT8 nNplm = 0;
			UINT32 pk;
			
				// Work out how long Nplm is
			for(pk = 0; pk < m_TileParts[tp].m_Lengths.size(); pk++) {
				UINT64 nLen = m_TileParts[tp].m_Lengths[pk].m_nHeaderLength;
				while(nLen > 0) {
					nNplm++;
					nLen = nLen >> 7;
				}
			}
			NCSJP2_CHECKIO(WriteUINT8(nNplm));

				// Write out each packet length, packed 7 bits at a time
			for(pk = 0; pk < m_TileParts[tp].m_Lengths.size(); pk++) {
				if(m_TileParts[tp].m_Lengths[pk].UnParse(Stream) == false) {
					Error = Stream;
					break;
				}
			}
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}
#endif //!NCSJPC_LEAN_AND_MEAN
