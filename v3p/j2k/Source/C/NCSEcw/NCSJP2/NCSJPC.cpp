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
** 
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPC.cpp $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPC class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
**           [01] 05May05 tfl  Commented out NCSUtilInit and NCSUtilFini in ctor/dtor per Manifold suggestion
 *******************************************************/

#define NCS_INLINE_FUNCS
#include "NCSJPC.h"
#include "NCSJPCCodeBlock.h"
#include "NCSUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPC::CNCSJPC()
{
	//NCSUtilInit(); /**[01]**/
	m_pStream = NULL;
	m_pCurrentTilePart = NULL;
	m_nNextZtp = 0;
	m_bLowMemCompression = false;
	m_fReconstructionParameter = 0.0f;
	{
		IEEE8 dV = 0.0;
		if(NCSPrefGetUserDouble("NCSJP2_RECONSTRUCTION_PARAMETER", &dV) == NCS_SUCCESS) {
			m_fReconstructionParameter = (IEEE4)dV;
		}
	}
}

// Destructor
CNCSJPC::~CNCSJPC()
{
	for(UINT32 i = 0; i < m_Tiles.size(); i++) {
		if( m_Tiles[i] ) delete m_Tiles[i];
		m_Tiles[i] = NULL;
	}
	//NCSUtilFini(); /**[01]**/
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPC::Parse(CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	m_pStream = &Stream;
	Error = CNCSJPCMainHeader::Parse(*this, Stream);
	if(Error == NCS_SUCCESS) {
	//	m_bValid = true;
	}
	return(Error);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPC::UnParse(CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCMainHeader::UnParse(*this, Stream);
	if(Error == NCS_SUCCESS) {

	}
	return(Error);
}

// Get the Tile TRX0 coordinate in reference grid from the tile index
CNCSJPCTilePartHeader *CNCSJPC::GetTile(INT32 iIndex, INT32 iPart)
{
	if(iIndex == -1) {
		iIndex = m_pCurrentTilePart->m_SOT.m_nIsot;
		iPart = 0;
	}

	if(m_pCurrentTilePart && m_pCurrentTilePart->m_SOT.m_nIsot == iIndex && m_pCurrentTilePart->m_SOT.m_nTPsot == iPart) {
		return(m_pCurrentTilePart);
	} else if( (iIndex < (INT32)m_Tiles.size()) && (iPart == 0) ) {
		return(m_Tiles[iIndex]);
	} else if( (iIndex < (INT32)m_Tiles.size()) && m_Tiles[iIndex] && ((INT32)m_Tiles[iIndex]->m_TileParts.size() >= iPart) ) {
		return m_Tiles[iIndex]->m_TileParts[iPart-1];
	} else {
		return(NULL);
	}

}


UINT32 CNCSJPC::GetPacketLength(UINT32 nPacket)
{
	CNCSJPCPacket *pPacket = GetPacketHeader(nPacket);
	if(pPacket) {
		UINT32 nLen = (UINT32)(pPacket->m_nLength + pPacket->m_nDataLength);
		delete pPacket;
		return(nLen);
	}
	return(0);
}

void *CNCSJPC::GetPacket(UINT32 nPacket, UINT32 *pLength)
{
	CNCSJPCPacket *pPacket = GetPacketHeader(nPacket);
	if(pPacket) {
		*pLength = (UINT32)(pPacket->m_nLength + pPacket->m_nDataLength);
		void *pData = NCSMalloc((UINT32)(pPacket->m_nLength + pPacket->m_nDataLength), FALSE);
		if(pData) {
			if(m_pStream->Seek(pPacket->m_nOffset, CNCSJPCIOStream::START) &&
			   m_pStream->Read(pData, (UINT32)pPacket->m_nLength) &&
			   m_pStream->Seek(pPacket->m_nDataOffset, CNCSJPCIOStream::START) &&
			   m_pStream->Read((UINT8*)pData + pPacket->m_nLength, (UINT32)pPacket->m_nDataLength)) {
			} else {
				NCSFree(pData);
				pData = NULL;
#ifdef WIN32
				*pLength = GetLastError();
#endif
			}
			delete pPacket;
			return(pData);
		}
	}
	return(NULL);
}

CNCSJPCPacket *CNCSJPC::GetPacketHeader(UINT32 nPacket)
{
	CNCSJPCPacket *pPacket = NULL;
	CNCSJPCTilePartHeader *pTP = GetTile(0);
	UINT32 nP = nPacket;

	while(pTP && pTP->GetNrPackets() <= nP) {
		nP -= pTP->GetNrPackets();
		pTP = GetTile(pTP->m_SOT.m_nIsot + 1);
	}
	if(pTP) {
		pPacket = pTP->GetPacketHeader(nPacket);
	}
/*	if(pPacket) {
		FILE *p = fopen("c:\\ecwp.txt", "a+");
		char buf[1024];
		sprintf(buf, "(%ld) %I64d %ld %I64d %ld\r\n", pPacket->m_nPacket, pPacket->m_nOffset, pPacket->m_nLength, pPacket->m_nDataOffset, pPacket->m_nDataLength);
		fprintf(p, buf);
		fclose(p);
	} else {
		FILE *p = fopen("c:\\temp\\ecwp.txt", "a+");
		char buf[1024];
		sprintf(buf, "(%ld) NOT FOUND\r\n", nPacket);
		fprintf(p, buf);
		fclose(p);
	}
*/	return(pPacket);
}

bool CNCSJPC::FindPacketRCPL(UINT32 nPacket, 
							 UINT16 &nTile,
							 UINT8 &nResolution, 
							 UINT16 &nComponent, 
							 UINT32 &nPrecinctX, 
							 UINT32 &nPrecinctY, 
							 UINT16 &nLayer)
{
	CNCSJPCTilePartHeader *pTP = GetTile(0);
	while(pTP) {
		if(pTP->GetFirstPacketNr() <= nPacket && pTP->GetFirstPacketNr() + pTP->GetNrPackets() > nPacket) {
			// Packet # in this tile part
			UINT32 nComponents = (UINT32)pTP->m_Components.size();
			for(UINT32 c = 0; c < nComponents; c++) {
				CNCSJPCComponent *pComponent = pTP->m_Components[c];
				UINT32 nResolutions = (UINT32)pComponent->m_Resolutions.size();
				for(UINT32 r = 0; r < nResolutions; r++) {
					CNCSJPCResolution *pResolution = pComponent->m_Resolutions[r];
					UINT32 nPrecinctsWide = pResolution->GetNumPrecinctsWide();
					UINT32 nPrecinctsHigh = pResolution->GetNumPrecinctsHigh();

					if(nPrecinctsWide && nPrecinctsHigh) {
						CNCSJPCPrecinct *pPrecinct = pResolution->m_Precincts.findPacketPrecinct(nPacket);
						if(pPrecinct) {
							nTile = pTP->m_SOT.m_nIsot;
							nResolution = r;
							nComponent = c;
							nPrecinctX = pPrecinct->m_nPrecinct % nPrecinctsWide;
							nPrecinctY = pPrecinct->m_nPrecinct / nPrecinctsWide;

							UINT32 nLayers = (UINT32)pPrecinct->m_Packets.size();
							for(UINT32 l = 0; l < nLayers; l++) {
								if(pPrecinct->m_Packets[l] == nPacket) {
									nLayer = l;
									break;
								}
							}
							return(true);
						}
					}
/*					for(UINT32 py = 0; py < nPrecinctsHigh; py++) {
						if(!pResolution->m_Precincts.empty(py)) {
							for(UINT32 px = 0; px < nPrecinctsWide; px++) {
								const CNCSJPCPrecinct *pPrecinct = pResolution->m_Precincts.find(px, py);
								if(pPrecinct) {
									UINT32 nLayers = (UINT32)pPrecinct->m_Packets.size();
									for(UINT32 l = 0; l < nLayers; l++) {
										if(pPrecinct->m_Packets[l] == nPacket) {
											nTile = pTP->m_SOT.m_nIsot;
											nResolution = r;
											nComponent = c;
											nPrecinctX = px;
											nPrecinctY = py;
											nLayer = l;
											return(true);
										}
									}
								}
							}
						}
					}*/
				}
			}
		}
		pTP = GetTile(pTP->m_SOT.m_nIsot + 1);
	}
	return(false);
}
