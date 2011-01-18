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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCSubBand.cpp $
** CREATED:  13/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCSubBand class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCSubBand.h"
#include "NCSJPCCodeBlock.h"
#include "NCSJPCPrecinct.h"
#include "NCSJPCResolution.h"
#include "NCSJPCComponent.h"
#include "NCSJPCTilePartHeader.h"

CNCSJPCNode::CNCSJPCNodeTracker CNCSJPCSubBand::sm_Tracker("CNCSJPCSubBand", sizeof(CNCSJPCSubBand));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

	/** Default constructor, initialises members */
CNCSJPCSubBand::CNCSJPCSubBand(CNCSJPCPrecinct *pPrecinct, NCSJPCSubBandType eType)
{
	sm_Tracker.Add();
	m_pPrecinct = pPrecinct;
	m_eType = eType;

	UINT32 nCB = GetNumCBWide() * GetNumCBHigh();

	m_LayerInclusion.SetDimensions(GetNumCBWide(), GetNumCBHigh());
	m_ZeroPlanes.SetDimensions(GetNumCBWide(), GetNumCBHigh());
	m_pCodeBlocks = new std::vector<class CNCSJPCCodeBlock>(nCB);
	//m_pCodeBlocks->resize(nCB);

	for(UINT32 c = 0; c < nCB; c++) {
		(*m_pCodeBlocks)[c].Init(this, c);
//		m_CodeBlocks[c] = new CNCSJPCCodeBlock(this, c);
	}
}

	/** Virtual destructor */
CNCSJPCSubBand::~CNCSJPCSubBand()
{
//	if(m_CodeBlocks) {
//		delete[] m_CodeBlocks;
//	}
	if(m_pCodeBlocks) {
		m_pCodeBlocks->clear();
		delete m_pCodeBlocks;
	}
	sm_Tracker.Remove();
//	while(m_CodeBlocks.size() > 0) {
//		CNCSJPCCodeBlock *pCB = m_CodeBlocks[0];
//		m_CodeBlocks.erase(m_CodeBlocks.begin());
//		delete pCB;
//	}
}

// Get X0 of this node.
INT32 CNCSJPCSubBand::GetX0(CNCSJPCResolution *pResolution, INT32 nPrecinct, NCSJPCSubBandType eType)
{
	INT32 px = 0;
	if(pResolution->GetNumPrecinctsWide()) {
		px = nPrecinct % pResolution->GetNumPrecinctsWide();
	}
	return(GetX0(pResolution, px, 0, eType));
}

// Get X0 of this node.
INT32 CNCSJPCSubBand::GetX0(CNCSJPCResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY, NCSJPCSubBandType eType)
{
	if(eType == NCSJPC_LL) {
			// LL SubBand coordinates == Precinct coordinates (r==0)
		return(CNCSJPCPrecinct::GetX0(pResolution, nPrecinctX, nPrecinctY));
	} else {
		INT32 nXOb = (eType == NCSJPC_HL || eType == NCSJPC_HH) ? 1 : 0;
		INT32 nLevel = (INT32)pResolution->m_pComponent->m_Resolutions.size() - pResolution->m_nResolution - 1;
		INT32 nPX0 = NCSCeilDiv((pResolution->m_pComponent->GetX0() - (1 << nLevel) * nXOb), NCS2Pow(nLevel + 1));
		INT32 nPrecinctWidth = pResolution->GetPrecinctWidth();
		INT32 nRX0 = nPrecinctWidth * (pResolution->GetX0() / nPrecinctWidth);
		return(NCSMax(NCSCeilDiv((nRX0 + nPrecinctX * nPrecinctWidth), 2), nPX0));
	}
}

INT32 CNCSJPCSubBand::GetX0()
{
	if(!m_X0.Cached()) {
		m_X0 = GetX0(m_pPrecinct->m_pResolution, m_pPrecinct->m_nPrecinct, m_eType);
	}
	return(m_X0);
}

// Get Y0 of this node.
INT32 CNCSJPCSubBand::GetY0(CNCSJPCResolution *pResolution, INT32 nPrecinct, NCSJPCSubBandType eType)
{
	INT32 py = 0;
	if(pResolution->GetNumPrecinctsWide()) {
		py = nPrecinct / pResolution->GetNumPrecinctsWide();
	}
	return(GetY0(pResolution, 0, py, eType));
}

// Get Y0 of this node.
INT32 CNCSJPCSubBand::GetY0(CNCSJPCResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY, NCSJPCSubBandType eType)
{
	if(eType == NCSJPC_LL) {
			// LL SubBand coordinates == Precinct coordinates (r==0)
		return(CNCSJPCPrecinct::GetY0(pResolution, nPrecinctX, nPrecinctY));
	} else {
		INT32 nYOb = (eType == NCSJPC_LH || eType == NCSJPC_HH) ? 1 : 0;
		INT32 nLevel = (INT32)pResolution->m_pComponent->m_Resolutions.size() - pResolution->m_nResolution - 1;
		INT32 nPY0 = NCSCeilDiv((pResolution->m_pComponent->GetY0() - (1 << nLevel) * nYOb), NCS2Pow(nLevel + 1));
		INT32 nPrecinctHeight = pResolution->GetPrecinctHeight();
		INT32 nRY0 = nPrecinctHeight * (pResolution->GetY0() / nPrecinctHeight);
		return(NCSMax(NCSCeilDiv((nRY0 + nPrecinctY * nPrecinctHeight), 2), nPY0));
	}
}

INT32 CNCSJPCSubBand::GetY0()
{
	if(!m_Y0.Cached()) {
		m_Y0 = GetY0(m_pPrecinct->m_pResolution, m_pPrecinct->m_nPrecinct, m_eType);
	}
	return(m_Y0);
}

// Get X1 of this node.
INT32 CNCSJPCSubBand::GetX1(CNCSJPCResolution *pResolution, INT32 nPrecinct, NCSJPCSubBandType eType)
{
	INT32 px = 0;
	if(pResolution->GetNumPrecinctsWide()) {
		px = nPrecinct % pResolution->GetNumPrecinctsWide();
	}
	return(GetX1(pResolution, px, 0, eType));
}

// Get X1 of this node.
INT32 CNCSJPCSubBand::GetX1(CNCSJPCResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY, NCSJPCSubBandType eType)
{
	if(eType == NCSJPC_LL) {
			// LL SubBand coordinates == Precinct coordinates (r==0)
		return(CNCSJPCPrecinct::GetX1(pResolution, nPrecinctX, nPrecinctY));
	} else {
		INT32 nXOb = (eType == NCSJPC_HL || eType == NCSJPC_HH) ? 1 : 0;
		INT32 nLevel = (INT32)pResolution->m_pComponent->m_Resolutions.size() - pResolution->m_nResolution - 1;
		INT32 nPX1 = NCSCeilDiv((pResolution->m_pComponent->GetX1() - (1 << nLevel) * nXOb), NCS2Pow(nLevel + 1));
		INT32 nPrecinctWidth = pResolution->GetPrecinctWidth();
		INT32 nRX0 = nPrecinctWidth * (pResolution->GetX0() / nPrecinctWidth);
		return(NCSMin(NCSCeilDiv((nRX0 + (nPrecinctX + 1) * nPrecinctWidth), 2), nPX1));
	}
}

INT32 CNCSJPCSubBand::GetX1()
{
	if(!m_X1.Cached()) {
		m_X1 = GetX1(m_pPrecinct->m_pResolution, m_pPrecinct->m_nPrecinct, m_eType);
	}
	return(m_X1);
}

// Get Y1 of this node.
INT32 CNCSJPCSubBand::GetY1(CNCSJPCResolution *pResolution, INT32 nPrecinct, NCSJPCSubBandType eType)
{
	INT32 py = 0;
	if(pResolution->GetNumPrecinctsWide()) {
		py = nPrecinct / pResolution->GetNumPrecinctsWide();
	}
	return(GetY1(pResolution, 0, py, eType));
}

// Get Y1 of this node.
INT32 CNCSJPCSubBand::GetY1(CNCSJPCResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY, NCSJPCSubBandType eType)
{
	if(eType == NCSJPC_LL) {
		// LL SubBand coordinates == Precinct coordinates (r==0)
		return(CNCSJPCPrecinct::GetY1(pResolution, nPrecinctX, nPrecinctY));
	} else {
		INT32 nYOb = (eType == NCSJPC_LH || eType == NCSJPC_HH) ? 1 : 0;
		INT32 nLevel = (INT32)pResolution->m_pComponent->m_Resolutions.size() - pResolution->m_nResolution - 1;
		INT32 nPY1 = NCSCeilDiv((pResolution->m_pComponent->GetY1() - (1 << nLevel) * nYOb), NCS2Pow(nLevel + 1));
		INT32 nPrecinctHeight = pResolution->GetPrecinctHeight();
		INT32 nRY0 = nPrecinctHeight * (pResolution->GetY0() / nPrecinctHeight);
		return(NCSMin(NCSCeilDiv((nRY0 + (nPrecinctY + 1) * nPrecinctHeight), 2), nPY1));
	}
}

INT32 CNCSJPCSubBand::GetY1()
{
	if(!m_Y1.Cached()) {
		m_Y1 = GetY1(m_pPrecinct->m_pResolution, m_pPrecinct->m_nPrecinct, m_eType);
	}
	return(m_Y1);
} 

// Get cb Width.
UINT32 CNCSJPCSubBand::GetCBWidth()
{
	return(m_pPrecinct->m_pResolution->GetCBWidth());
}

// Get cb Height.
UINT32 CNCSJPCSubBand::GetCBHeight()
{
	return(m_pPrecinct->m_pResolution->GetCBHeight());
}

// Get number of cb wide.
UINT32 CNCSJPCSubBand::GetNumCBWide()
{
	if(!m_NumCBWide.Cached()) {
		m_NumCBWide = NCSCeilDiv(GetX1(), GetCBWidth()) - NCSFloorDiv(GetX0(), GetCBWidth());
	}
	return(m_NumCBWide);
}

// Get number of cb high.
UINT32 CNCSJPCSubBand::GetNumCBHigh()
{
	if(!m_NumCBHigh.Cached()) {
		m_NumCBHigh = NCSCeilDiv(GetY1(), GetCBHeight()) - NCSFloorDiv(GetY0(), GetCBHeight());
	}
	return(m_NumCBHigh);
}

CNCSJPCNode *CNCSJPCSubBand::GetNodePtr(UINT32 nNode, UINT16 iComponent) 
{
	iComponent;//Keep compiler happy
	return(&(*m_pCodeBlocks)[nNode]); 
}
