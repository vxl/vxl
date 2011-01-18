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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCPPMMarker.cpp $
** CREATED:  18/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCPPMMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCPPMMarker.h"
#include "NCSJPC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCPPMMarker::CNCSJPCPPMMarker()
{
	// Initialise the base marker class members
	m_eMarker = PPM;
	m_nZppm = 0;
}

// Destructor
CNCSJPCPPMMarker::~CNCSJPCPPMMarker()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCPPMMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	m_bHaveMarker = true;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(ReadUINT16(m_nLength));
		NCSJP2_CHECKIO(ReadUINT8(m_nZppm));

		INT32 nLen = m_nLength - sizeof(UINT16) - sizeof(UINT8);
		INT64 nBegin = Stream.Tell();

		while(Stream.Tell() < (nBegin + nLen) && Error == NCS_SUCCESS) {
		//	if(JPC.m_PPMs.size() != 0 && JPC.m_PPMs.back().m_TileParts.size() != 0 && JPC.m_PPMs.back().m_TileParts.back().m_nNppm != 0) {
				// Nead to complete previous PPM...
		//		Stream.Seek(NCSMin(nLen, JPC.m_PPMs.back().m_TileParts.back().m_nNppm));
				//JPC.m_PPMs.back().m_TileParts.back().m_nNppm -= NCSMin(nLen, JPC.m_PPMs.back().m_TileParts.back().m_nNppm);
		//	}
//				while(Stream.Tell() < (nBegin + nLen) && JPC.m_PPMs.back().m_TileParts.back().m_nNppm-- && Error == NCS_SUCCESS) {
//					CNCSJPCPacket header;
//					NCSJP2_CHECKIO_ERROR(header.Parse(JPC, Stream, JPC.GetTile()));
//					JPC.m_PPMs.back().m_TileParts.back().m_Headers.push_back(header);
//				}
//			} else {
				TilePartPackets temp;

				if(JPC.m_PPMs.size() != 0 && JPC.m_PPMs.back().m_TileParts.size() != 0 && JPC.m_PPMs.back().m_TileParts.back().m_nNppmLeft != 0) {
					Stream.Seek(JPC.m_PPMs.back().m_TileParts.back().m_nNppmLeft);
					nBegin = Stream.Tell();
				}
				NCSJP2_CHECKIO(ReadUINT32(temp.m_nNppmTotal));
				temp.m_nOffset = Stream.Tell();

				temp.m_nNppmLeft = temp.m_nNppmTotal - (UINT32)NCSMin((nBegin + nLen) - temp.m_nOffset, temp.m_nNppmTotal);
				Stream.Seek(temp.m_nNppmTotal - temp.m_nNppmLeft);
				//temp.m_nNppm -= NCSMin((nBegin + nLen) - temp.m_nOffset, temp.m_nNppm);
//				while(Stream.Tell() < (nBegin + nLen) && temp.m_nNppm-- && Error == NCS_SUCCESS) {
//					CNCSJPCPacket header;
//					NCSJP2_CHECKIO_ERROR(header.Parse(JPC, Stream, JPC.GetTile()));
//					temp.m_Headers.push_back(header);
//				}
				m_TileParts.push_back(temp);
//			}
		}
		if(Error == NCS_SUCCESS) {
			m_bValid = true;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

#ifndef NCSJPC_LEAN_AND_MEAN
// UnParse the marker out to the stream.
CNCSError CNCSJPCPPMMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCMarker::UnParse(JPC, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		UINT64 nBegin = Stream.Tell();
		NCSJP2_CHECKIO(WriteUINT16(m_nLength));
		NCSJP2_CHECKIO(WriteUINT8(m_nZppm));

		for(UINT32 tp = 0; tp < m_TileParts.size(); tp++) {
			UINT8 nNPPM = 0;
			
				// Work out how long NPPM is
//			for(UINT32 pk = 0; pk < m_TileParts[tp].m_Headers.size(); pk++) {
//				UINT64 nLen = m_TileParts[tp].m_Headers[pk].m_nLength;
//				while(nLen > 0) {
//					nNPPM++;
//					nLen = nLen >> 7;
//				}
//			}
			NCSJP2_CHECKIO(WriteUINT8(nNPPM));

				// Write out each packet length, packed 7 bits at a time
//			for(pk = 0; pk < m_TileParts[tp].m_Headers.size(); pk++) {
//				NCSJP2_CHECKIO_ERROR(m_TileParts[tp].m_Headers[pk].UnParse(JPC, Stream));
//			}
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}
#endif //!NCSJPC_LEAN_AND_MEAN
