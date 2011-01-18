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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCSODMarker.cpp $
** CREATED:  11/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCSODMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCSODMarker.h"
#include "NCSJPC.h"
#include "NCSJPCResolution.h"
#include "NCSJPCCodeBlock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCSODMarker::CNCSJPCSODMarker()
{
	// Initialise the base marker class members
	m_nDataOffset = 0;
	m_eMarker = SOD;
	m_nLength = 2;
}

// Destructor
CNCSJPCSODMarker::~CNCSJPCSODMarker()
{
}

INT64 CNCSJPCSODMarker::FindSOP(CNCSJPCIOStream &Stream, UINT64 &nBytesLeft)
{
	UINT8 buf[1024];
	UINT16 SOP = 0;
	//UINT8 Byte = 0;
	INT64 nBegin = Stream.Tell();
	UINT16 nLength = (UINT16)NCSMin(sizeof(buf), nBytesLeft);

	while(Stream.Read(buf, nLength) && nBytesLeft) {
		UINT16 i = 0;
		while(SOP != CNCSJPCMarker::SOP && i < nLength) {
			nBytesLeft--;
			SOP = (SOP << 8) | buf[i++];
		}
		if(SOP == CNCSJPCMarker::SOP) {
			INT64 iSOP = Stream.Tell() - (nLength - i) - sizeof(UINT16);
			Stream.Seek(iSOP + sizeof(UINT16), CNCSJPCIOStream::START);
			return(iSOP);
		}
		nLength = (UINT16)NCSMin(sizeof(buf), nBytesLeft);
		nBegin = Stream.Tell();
	}
/*
	while(SOP != CNCSJPCMarker::SOP && nBytesLeft && Stream.ReadUINT8(Byte)) {
		nBytesLeft--;
		SOP = (SOP << 8) | Byte;
	}
	if(SOP == CNCSJPCMarker::SOP) {
		return(Stream.Tell() - sizeof(UINT16));
	}
*/
	return(-1);
}
// Parse the marker in from the JP2 file.
CNCSError CNCSJPCSODMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;
	m_bHaveMarker = true;
	m_bValid = true;
	m_nDataOffset = Stream.Tell();

	CNCSJPCTilePartHeader *pMainTP = JPC.GetTile();
//pMainTP->m_PLTs.clear();
	if(/*true ||*/ pMainTP->m_PLTs.size() == 0 || (pMainTP->m_PLTs.size() && pMainTP->m_PLTs[0].m_bDynamic) || Stream.Seek() == false) {
		UINT64 nBytes = (JPC.m_pCurrentTilePart->m_SOT.m_nPsot ? (JPC.m_pCurrentTilePart->m_SOT.m_nOffset + JPC.m_pCurrentTilePart->m_SOT.m_nPsot) : (Stream.Size() - sizeof(UINT16))) - Stream.Tell();

		CNCSJPCPacketLengthType Length;

		// No Packet Length markers, or non-seekable stream, parse the headers in now
#ifdef NOTUSED
		if(false /* Currently SOP index parsing broken so just skip it */ && pMainTP->m_pJPC->m_bFilePPMs == false && pMainTP->m_bFilePPTs == false && 
		   pMainTP->m_Components[pMainTP->m_nCurComponent]->m_CodingStyle.m_Scod.bSOPMarkers) {
			INT64 nPrev = FindSOP(Stream, nBytes);
			if(nPrev > 0) {
				INT64 nCur;
				pMainTP->m_PLTs.resize(1);
				CNCSJPCPLTMarker &PLT = pMainTP->m_PLTs[0];
				PLT.m_bDynamic = true;
				PLT.AllocLengths();
//				std::vector<CNCSJPCPacketLengthType> *pLengths = PLT.GetLengths();

				while((nCur = FindSOP(Stream, nBytes)) > 0) {
					Length.m_nHeaderLength = (UINT32)(nCur - nPrev);
					PLT.AddLength(Length);
//					pLengths->push_back(Length);
//					CNCSJPCPLTMarker::sm_Tracker.AddMem(sizeof(CNCSJPCPacketLengthType));
//					PLT.m_nLengths++;
					nPrev = nCur;
				}
				Length.m_nHeaderLength = (UINT32)(JPC.m_pCurrentTilePart->m_SOT.m_nPsot + JPC.m_pCurrentTilePart->m_SOT.m_nOffset - nPrev);
				PLT.AddLength(Length);
//				pLengths->push_back(Length);
//				CNCSJPCPLTMarker::sm_Tracker.AddMem(sizeof(CNCSJPCPacketLengthType));
//				PLT.m_nLengths++;
			}
		} else
#endif
			if(pMainTP->m_bFilePPTs) {
			// Just skip SOD for now, all tiles with PPTs parsed at end of main header
			Stream.Seek(JPC.m_pCurrentTilePart->m_SOT.m_nPsot + JPC.m_pCurrentTilePart->m_SOT.m_nOffset, CNCSJPCIOStream::START);
		} else {
			bool bComplete = false;
			CNCSJPCPacketList Packets;
			Error = Packets.Parse(JPC, Stream, bComplete, nBytes);
			if(Error == NCS_SUCCESS) {
				Error = Packets.GenDynamicPLTs(pMainTP);
				Packets.clear();
			}
		}
	}
	return(NCS_SUCCESS);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCSODMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	return(CNCSJPCMarker::UnParse(JPC, Stream));
}
