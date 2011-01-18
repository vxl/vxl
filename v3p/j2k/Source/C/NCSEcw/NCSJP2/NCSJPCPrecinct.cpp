/********************************************************
** Copyright 2003 Earth Resource Mapping Ltd.
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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCPrecinct.cpp $
** CREATED:  12/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCPrecinct class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

// shut compiler up!
#ifdef WIN32
	#pragma warning(disable:4786)
	#pragma warning(disable:4251)
#endif

#include "NCSUtil.h"
#include "NCSJPCPrecinct.h"
#include "NCSJPCResolution.h"
#include "NCSJPCComponent.h"
#include "NCSJPCTilePartHeader.h"
#include "NCSJPC.h"
#include "NCSJPCMemoryIOStream.h"

CNCSJPCNode::CNCSJPCNodeTracker CNCSJPCPrecinct::sm_Tracker("CNCSJPCPrecinct", sizeof(CNCSJPCPrecinct));
UINT32 CNCSJPCPrecinct::sm_nZeroRefs = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

	/** Default constructor, initialises members */
CNCSJPCPrecinct::CNCSJPCPrecinct(CNCSJPCResolution *pResolution, UINT32 nPrecinct, bool bCreateSubBands)
{
	sm_Tracker.Add();
	m_pResolution = pResolution;
	m_nPrecinct = nPrecinct;
	m_nRefs = 0;
	sm_nZeroRefs++;
	m_nProgressionLayer = 0;

	CNCSJPC *pJPC = m_pResolution->m_pComponent->m_pTilePart->m_pJPC;

	CNCSJPCTilePartHeader *pMainTP = pJPC->GetTile(m_pResolution->m_pComponent->m_pTilePart->m_SOT.m_nIsot);
	m_Packets.resize(m_pResolution->m_pComponent->m_CodingStyle.m_SGcod.m_nLayers, (UINT32)-1);

	memset(m_SubBands, 0, sizeof(m_SubBands));

	if(pJPC->m_pStream->Seek() == false || m_pResolution->m_pComponent->m_pTilePart->m_PLTs.size() == 0 || bCreateSubBands) {
		CreateSubBands(false);
	}
	m_bZeroSize = ((GetX0() < GetX1()) && (GetY0() < GetY1())) ? false : true;
}

	/** Virtual destructor */
CNCSJPCPrecinct::~CNCSJPCPrecinct()
{
	DestroySubBands();
	sm_Tracker.Remove();
	sm_nZeroRefs--;
}

// Get X0 of this node.
INT32 CNCSJPCPrecinct::GetX0(CNCSJPCResolution *pResolution, INT32 nPrecinct)
{
	INT32 px = 0;
	if(pResolution->GetNumPrecinctsWide()) {
		px = nPrecinct % pResolution->GetNumPrecinctsWide();
	}
	return(GetX0(pResolution, px, 0));
}

// Get X0 of this node.
INT32 CNCSJPCPrecinct::GetX0(CNCSJPCResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY)
{
	INT32 nPrecinctWidth = pResolution->GetPrecinctWidth();
	INT32 nRX0 = nPrecinctWidth * (pResolution->GetX0() / nPrecinctWidth);
	return(NCSMax(pResolution->GetX0(), NCSMin(pResolution->GetX1(), nRX0 + nPrecinctX * nPrecinctWidth)));
}

// Get X0 of this node.
INT32 CNCSJPCPrecinct::GetX0()
{
	if(!m_X0.Cached()) {
		m_X0 = GetX0(m_pResolution, m_nPrecinct);
	}
	return(m_X0);
}

// Get Y0 of this node.
INT32 CNCSJPCPrecinct::GetY0(CNCSJPCResolution *pResolution, INT32 nPrecinct)
{
	INT32 py = 0;
	if(pResolution->GetNumPrecinctsWide()) {
		py = nPrecinct / pResolution->GetNumPrecinctsWide();
	}
	return(GetY0(pResolution, 0, py));
}

// Get Y0 of this node.
INT32 CNCSJPCPrecinct::GetY0(CNCSJPCResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY)
{
	INT32 nPrecinctHeight = pResolution->GetPrecinctHeight();
	INT32 nRY0 = nPrecinctHeight * (pResolution->GetY0() / nPrecinctHeight);
	return(NCSMax(pResolution->GetY0(), NCSMin(pResolution->GetY1(), nRY0 + nPrecinctY * nPrecinctHeight)));
}

// Get Y0 of this node.
INT32 CNCSJPCPrecinct::GetY0()
{
	if(!m_Y0.Cached()) {
		m_Y0 = GetY0(m_pResolution, m_nPrecinct);
	}
	return(m_Y0);
}

// Get X1 of this node.
INT32 CNCSJPCPrecinct::GetX1(CNCSJPCResolution *pResolution, INT32 nPrecinct)
{
	INT32 px = 0;
	if(pResolution->GetNumPrecinctsWide()) {
		px = nPrecinct % pResolution->GetNumPrecinctsWide();
	}
	return(GetX1(pResolution, px, 0));
}

// Get X1 of this node.
INT32 CNCSJPCPrecinct::GetX1(CNCSJPCResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY)
{
	INT32 nPrecinctWidth = pResolution->GetPrecinctWidth();
	INT32 nRX0 = nPrecinctWidth * (pResolution->GetX0() / nPrecinctWidth);
	return(NCSMin(pResolution->GetX1(), NCSMax(pResolution->GetX0(), nRX0 + (nPrecinctX + 1) * nPrecinctWidth)));
}

// Get X1 of this node.
INT32 CNCSJPCPrecinct::GetX1()
{
	if(!m_X1.Cached()) {
		m_X1 = GetX1(m_pResolution, m_nPrecinct);
	}
	return(m_X1);
}

// Get Y1 of this node.
INT32 CNCSJPCPrecinct::GetY1(CNCSJPCResolution *pResolution, INT32 nPrecinct)
{
	INT32 py = 0;
	if(pResolution->GetNumPrecinctsWide()) {
		py = nPrecinct / pResolution->GetNumPrecinctsWide();
	}
	return(GetY1(pResolution, 0, py));
}

// Get Y1 of this node.
INT32 CNCSJPCPrecinct::GetY1(CNCSJPCResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY)
{
	INT32 nPrecinctHeight = pResolution->GetPrecinctHeight();
	INT32 nRY0 = nPrecinctHeight * (pResolution->GetY0() / nPrecinctHeight);
	return(NCSMin(pResolution->GetY1(), NCSMax(pResolution->GetY0(), nRY0 + (nPrecinctY + 1) * nPrecinctHeight)));
}

// Get Y1 of this node.
INT32 CNCSJPCPrecinct::GetY1()
{
	if(!m_Y1.Cached()) {
		m_Y1 = GetY1(m_pResolution, m_nPrecinct);
	}
	return(m_Y1);
}

// Create the subbands
bool CNCSJPCPrecinct::CreateSubBands(bool bRead)
{
	bool bReadCB = false;

	for(int eBand = (m_pResolution->m_nResolution == 0 ? NCSJPC_LL : NCSJPC_HL); 
						eBand <= (m_pResolution->m_nResolution == 0 ? NCSJPC_LL : NCSJPC_HH); 
						eBand++) {
//		if(m_SubBands.size() < (UINT32)eBand + 1) {
//			m_SubBands.resize((UINT32)eBand + 1);
//		}
		if(m_SubBands[eBand] == NULL) {
			m_SubBands[eBand] = new CNCSJPCSubBand(this, (NCSJPCSubBandType)eBand);
			bReadCB = true;
		}
	}
	if(bReadCB && bRead) {
		return(ReadPackets());
	}
	return(true);
}

// Destroy the subbands
bool CNCSJPCPrecinct::DestroySubBands()
{
	delete m_SubBands[NCSJPC_LL];
	m_SubBands[NCSJPC_LL] = NULL;
	delete m_SubBands[NCSJPC_LH];
	m_SubBands[NCSJPC_LH] = NULL;
	delete m_SubBands[NCSJPC_HL];
	m_SubBands[NCSJPC_HL] = NULL;
	delete m_SubBands[NCSJPC_HH];
	m_SubBands[NCSJPC_HH] = NULL;
	return(true);
}

// Read a line from the Precinct.
bool CNCSJPCPrecinct::ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent)
{
	bool bRet = false;
	CreateSubBands(true);

	bRet = m_SubBands[(NCSJPCSubBandType)iComponent]->ReadLine(nCtx, pDst, 0);
	if(bRet == false) {
		*(CNCSError*)this = (CNCSError)*m_SubBands[(NCSJPCSubBandType)iComponent];
	}
	return(bRet);
}

bool CNCSJPCPrecinct::ReadPackets()
{
	CNCSJPCTilePartHeader *pMainTP = m_pResolution->m_pComponent->m_pTilePart->m_pJPC->GetTile(m_pResolution->m_pComponent->m_pTilePart->m_SOT.m_nIsot);
	
	for(UINT32 l = 0; l < m_Packets.size(); l++) {
/*		UINT32 nLength = 0;
		void *pImage = pMainTP->m_pJPC->GetPacket(m_Packets[l] + pMainTP->GetFirstPacketNr(), &nLength);
		if(pImage) {
			CNCSJPCMemoryIOStream Stream(false);
			CNCSError Error = Stream.Open(pImage, nLength);
			if(Error == NCS_SUCCESS) {
				CNCSJPCPacket Header;
				CNCSJPCProgression p;

				p.m_nCurTile = m_pResolution->m_pComponent->m_pTilePart->m_nCurTile;
				p.m_nCurComponent = m_pResolution->m_pComponent->m_iComponent;
				p.m_nCurResolution = m_pResolution->m_nResolution;
				p.m_nCurPrecinct = m_nPrecinct;
				p.m_nCurLayer = l;
				p.m_nCurPacket = m_Packets[l];
				m_Error = Header.Parse(*(pMainTP->m_pJPC), Stream, &p);
			} else {
				m_Error = Error;
			}
			NCSFree(pImage);
		} else {
*/	
		CNCSJPCPacket *pPacket = pMainTP->GetPacketHeader(m_Packets[l]);
			if(pPacket) {
		//	char buf[1024];
		//	sprintf(buf, "(%ld) %I64d %ld %I64d %ld\r\n", pPacket->m_nPacket, pPacket->m_nOffset, pPacket->m_nLength, pPacket->m_nDataOffset, pPacket->m_nDataLength);
		//	fprintf(stdout, buf);
				if(pPacket->m_nOffset != 0) {
					if(pMainTP->m_pJPC->m_pStream->Seek(pPacket->m_nOffset, CNCSJPCIOStream::START)) {
						CNCSJPCProgression p;

						p.m_nCurTile = m_pResolution->m_pComponent->m_pTilePart->m_nCurTile;
						p.m_nCurComponent = m_pResolution->m_pComponent->m_iComponent;
						p.m_nCurResolution = m_pResolution->m_nResolution;
						p.m_nCurPrecinctX = m_nPrecinct % m_pResolution->GetNumPrecinctsWide();
						p.m_nCurPrecinctY = m_nPrecinct / m_pResolution->GetNumPrecinctsWide();
						p.m_nCurLayer = l;
						p.m_nCurPacket = m_Packets[l];
						pPacket->m_nDataLength = 0;
						pPacket->m_nLength = 0;
						if(pMainTP->m_pJPC->m_bFilePPMs || pMainTP->m_bFilePPTs) {
							bool bNonZeroLength = false;
							m_Error = pPacket->ParseHeader(*(pMainTP->m_pJPC), *(pMainTP->m_pJPC->m_pStream), &p, false, &bNonZeroLength);
							if(m_Error == NCS_SUCCESS) {
								pMainTP->m_pJPC->m_pStream->Seek(pPacket->m_nDataOffset, CNCSJPCIOStream::START);
								m_Error = pPacket->ParseBody(*(pMainTP->m_pJPC), *(pMainTP->m_pJPC->m_pStream), &p, bNonZeroLength);
							}
						} else {
							m_Error = pPacket->ParseHeader(*(pMainTP->m_pJPC), *(pMainTP->m_pJPC->m_pStream), &p, true);
						}
					} else {
						m_Error = NCS_FILE_SEEK_ERROR;
						delete pPacket;
						break;
					}
				}
				delete pPacket;
			} else {
				m_Error = NCS_INVALID_PARAMETER;
				break;
			}
//		}
		if(m_Error != NCS_SUCCESS) {
			break;
		}
	}
	return(m_Error == NCS_SUCCESS);
}

void CNCSJPCPrecinct::AddRef() 
{ 
	if(m_nRefs == 0) {
		sm_nZeroRefs--;
	}
	m_nRefs++;
}

void CNCSJPCPrecinct::UnRef() 
{ 
	if(m_nRefs > 0) {
		m_nRefs--;
		if(m_nRefs == 0) {
			sm_nZeroRefs++;
		}
	}
}

UINT32 CNCSJPCPrecinct::NrRefs()  
{ 
	return(m_nRefs);
};

bool CNCSJPCPrecinct::HaveZeroRefs()
{
	return(sm_nZeroRefs ? true : false);
}

CNCSJPCPrecinctMap::CNCSJPCPrecinctMap() 
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_pResolution = NULL;
}

CNCSJPCPrecinctMap::~CNCSJPCPrecinctMap() 
{
}

void CNCSJPCPrecinctMap::Init(class CNCSJPCResolution *pResolution) 
{
	m_pResolution = pResolution;
	m_nWidth = pResolution->GetNumPrecinctsWide();
	m_nHeight = pResolution->GetNumPrecinctsHigh();

	m_Rows.resize(NCSMax(1, m_nHeight));
}

void CNCSJPCPrecinctMap::ResetProgressionLayer(void)
{
	for(UINT32 y = 0; y < m_nHeight; y++) {
		CNCSJPCPrecinctMapRow &Row = m_Rows[y];

		if(!Row.m_Columns.empty()) {
			CNCSJPCPrecinctMapRow::CNCSJPCPrecinctMapColumnIterator pColCur = Row.m_Columns.begin();
			CNCSJPCPrecinctMapRow::CNCSJPCPrecinctMapColumnIterator pColEnd = Row.m_Columns.end();

			while(pColCur != pColEnd) {
				(*pColCur).second->m_nProgressionLayer = 0;
				pColCur++;
			}
		}
	}
}

bool CNCSJPCPrecinctMap::UnLink(CNCSJPCNode::ContextID nCtx, UINT16 nInputs) 
{
	bool bRet = true;
	for(UINT32 n = 0; n < (UINT32)NCSMax(1, nInputs); n++) {
		for(UINT32 y = 0; y < m_nHeight; y++) {
			CNCSJPCPrecinctMapRow &Row = m_Rows[y];

			if(!Row.m_Columns.empty()) {
				CNCSJPCPrecinctMapRow::CNCSJPCPrecinctMapColumnIterator pColCur = Row.m_Columns.begin();
				CNCSJPCPrecinctMapRow::CNCSJPCPrecinctMapColumnIterator pColEnd = Row.m_Columns.end();

				while(pColCur != pColEnd) {
					bRet &= (*pColCur).second->UnLink(nCtx, n);
					pColCur++;
				}
			}
		}
	}
	return(bRet);
}

bool CNCSJPCPrecinctMap::empty(UINT32 y)
{
	return(m_Rows[y].m_Columns.empty());
}

CNCSJPCPrecinct *CNCSJPCPrecinctMap::find(UINT32 x, UINT32 y) 
{
	CNCSJPCPrecinctMapRow &Row = m_Rows[y];
	
	if(!Row.m_Columns.empty()) {
		CNCSJPCPrecinctMapRow::CNCSJPCPrecinctMapColumnIterator it = Row.m_Columns.find(x);
		if(it != Row.m_Columns.end()) {
			return((*it).second);
		}
	}
	return(NULL);
}

CNCSJPCPrecinct *CNCSJPCPrecinctMap::find(UINT32 nPrecinct)
{
	UINT32 y = nPrecinct / m_nWidth;
	CNCSJPCPrecinctMapRow &Row = m_Rows[y];

	if(!Row.m_Columns.empty()) {
		UINT32 x = nPrecinct % m_nWidth;
		CNCSJPCPrecinctMapRow::CNCSJPCPrecinctMapColumnIterator it = Row.m_Columns.find(x);
		if(it != Row.m_Columns.end()) {
			return((*it).second);
		}
	}
	return(NULL);
}

CNCSJPCPrecinct *CNCSJPCPrecinctMap::findPacketPrecinct(UINT32 nPacket)
{
	for(UINT32 y = 0; y < m_nHeight; y++) {
		CNCSJPCPrecinctMapRow &Row = m_Rows[y];

		if(!Row.m_Columns.empty()) {
			CNCSJPCPrecinctMapRow::CNCSJPCPrecinctMapColumnIterator pCur = Row.m_Columns.begin();
			CNCSJPCPrecinctMapRow::CNCSJPCPrecinctMapColumnIterator pEnd = Row.m_Columns.end();

			while(pCur != pEnd) {
				CNCSJPCPrecinct *pPrecinct = (*pCur).second;
				if(pPrecinct) {
					std::vector<UINT32> &Packets = pPrecinct->m_Packets;

					UINT32 nPackets = (UINT32)Packets.size();
					for(UINT32 p = 0; p < nPackets; p++) {
						if(Packets[p] == nPacket) {
							return(pPrecinct);
						}
					}
				}
				pCur++;
			}
		}
	}
	return(NULL);
}

void CNCSJPCPrecinctMap::remove(UINT32 x, UINT32 y) 
{
	CNCSJPCPrecinctMapRow &Row = m_Rows[y];
	if(!Row.m_Columns.empty()) {
		CNCSJPCPrecinctMapRow::CNCSJPCPrecinctMapColumnIterator it = Row.m_Columns.find(x);
		if(it != Row.m_Columns.end()) {
			delete (*it).second;
			Row.m_Columns.erase(it);
		}
	}
}

void CNCSJPCPrecinctMap::remove(UINT32 nPrecinct)
{
	remove(nPrecinct % m_nWidth, nPrecinct / m_nWidth);	
}

void CNCSJPCPrecinctMap::remove(CNCSJPCPrecinct *p) {
	remove(p->m_nPrecinct);
}

void CNCSJPCPrecinctMap::insert(UINT32 x, UINT32 y, CNCSJPCPrecinct *p) 
{
	CNCSJPCPrecinctMapRow &Row = m_Rows[y];
	Row.m_Columns.insert(std::pair<const UINT32, CNCSJPCPrecinct*> ((const UINT32)x, p));
}

void CNCSJPCPrecinctMap::insert(UINT32 nPrecinct, CNCSJPCPrecinct *p)
{
	insert(nPrecinct % m_nWidth, nPrecinct / m_nWidth, p);	
}

CNCSJPCPrecinctMap::CNCSJPCPrecinctMapRow::CNCSJPCPrecinctMapRow()
{
}

CNCSJPCPrecinctMap::CNCSJPCPrecinctMapRow::~CNCSJPCPrecinctMapRow()
{
	CNCSJPCPrecinctMapRow::CNCSJPCPrecinctMapColumnIterator pColCur = m_Columns.begin();
	CNCSJPCPrecinctMapRow::CNCSJPCPrecinctMapColumnIterator pColEnd = m_Columns.end();

	while(pColCur != pColEnd) {
		delete (*pColCur).second;
		pColCur++;
	}
}
