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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCComponent.cpp $
** CREATED:  12/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCComponent class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCComponent.h"
#include "NCSJP2FileView.h"
#include "NCSJPCTilePartHeader.h"
#include "NCSJPC.h"
#include "NCSEcw.h"

#include "NCSJPCEcwpIOStream.h"

const UINT32 CNCSJPCComponent::CREATE		= (0x1 << 0);
const UINT32 CNCSJPCComponent::ADDREF		= (0x1 << 1);
const UINT32 CNCSJPCComponent::UNREF		= (0x1 << 2);
const UINT32 CNCSJPCComponent::STATS		= (0x1 << 3);
const UINT32 CNCSJPCComponent::REQUEST	= (0x1 << 4);
const UINT32 CNCSJPCComponent::CANCEL		= (0x1 << 5);


CNCSJPCNode::CNCSJPCNodeTracker CNCSJPCComponent::sm_Tracker("CNCSJPCComponent", sizeof(CNCSJPCComponent));

extern "C" BOOLEAN IntersectRects(INT32 tlx1, INT32 tly1, INT32 brx1, INT32 bry1, INT32 tlx2, INT32 tly2, INT32 brx2, INT32 bry2);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSJPCComponent::Context::Context()
{
	m_nXCMul = 1;
	m_nYCMul = 1;
}

CNCSJPCComponent::Context::~Context()
{
}

	/** Default constructor, initialises members */
CNCSJPCComponent::CNCSJPCComponent(CNCSJPCTilePartHeader *pTilePart, 
								   UINT16 iComponent)
{
	sm_Tracker.Add();
	m_iComponent = iComponent;
	m_pTilePart = pTilePart;
	m_bPaletted = false;
}

	/** Virtual destructor */
CNCSJPCComponent::~CNCSJPCComponent()
{
	while(m_Resolutions.size() > 0) {
		CNCSJPCResolution  *pR = m_Resolutions[0];
		m_Resolutions.erase(m_Resolutions.begin());
		delete pR;
	}
	sm_Tracker.Remove();
}

// Get X0 of this node.
INT32 CNCSJPCComponent::GetX0()
{
	if(!m_X0.Cached()) {
		m_X0 = NCSCeilDiv(m_pTilePart->GetX0(), m_pTilePart->m_pJPC->m_SIZ.m_Components[m_iComponent].m_nXRsiz);
	}
	return(m_X0);
}

// Get Y0 of this node.
INT32 CNCSJPCComponent::GetY0()
{
	if(!m_Y0.Cached()) {
		m_Y0 = NCSCeilDiv(m_pTilePart->GetY0(), m_pTilePart->m_pJPC->m_SIZ.m_Components[m_iComponent].m_nYRsiz);
	}
	return(m_Y0);
}

// Get X1 of this node.
INT32 CNCSJPCComponent::GetX1()
{
	if(!m_X1.Cached()) {
		m_X1 = NCSCeilDiv(m_pTilePart->GetX1(),m_pTilePart->m_pJPC->m_SIZ.m_Components[m_iComponent].m_nXRsiz);
	}
	return(m_X1);
}

// Get Y1 of this node.
INT32 CNCSJPCComponent::GetY1()
{
	if(!m_Y1.Cached()) {
		m_Y1 = NCSCeilDiv(m_pTilePart->GetY1(), m_pTilePart->m_pJPC->m_SIZ.m_Components[m_iComponent].m_nYRsiz);
	}
	return(m_Y1);
}

CNCSJPCNode::Context *CNCSJPCComponent::GetContext(ContextID nCtx, bool bAutoConstruct)
{
	CNCSJPCNode::Context *pCtx = CNCSJPCNode::GetContext(nCtx, false);
	if(!pCtx && bAutoConstruct) {
		pCtx = new Context;
		SetContext(nCtx, pCtx);
	}
	return(pCtx);
}

// Link a node as the unput to this node.
bool CNCSJPCComponent::Link(ContextID nCtx, 
							UINT32 nResolution,
							INT32 nXCMul,
							INT32 nYCMul)
{
	Context *pCtx = (Context*)GetContext(nCtx);
	for(INT32 r = nResolution; r >= 0; r--) {
		m_Resolutions[r]->Link(nCtx, 1, r == 0 ? NULL : m_Resolutions[r - 1]);
	}
	pCtx->m_nXCMul = nXCMul;
	pCtx->m_nYCMul = nYCMul;
	return(CNCSJPCNode::Link(nCtx, 1, m_Resolutions[nResolution]));
}

// Read a line from the Resolution.
bool CNCSJPCComponent::ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent)
{
	bool bRet = false;
	Context *pCtx = (Context*)GetContext(nCtx);

	INT32 nXCMul = pCtx->m_nXCMul;
	INT32 nYCMul = pCtx->m_nYCMul;

	pDst->SetFlag(CNCSJPCBuffer::AT_LOCKED);

	if(nXCMul == 1 && nYCMul == 1) {
		// Not supersampling here, do nothing
		return(CNCSJPCNode::ReadLine(nCtx, pDst, iComponent));
	} else {
		if(nXCMul == 1) {
			// Only supersampling in Y
			CNCSJPCBuffer tmp;
			
			tmp.Assign(pDst->GetX0(), pDst->GetY0() / nYCMul, pDst->GetWidth(), pDst->GetHeight(), pDst->GetType(), pDst->GetPtr());
			tmp.SetFlag(CNCSJPCBuffer::AT_LOCKED);
			bRet = CNCSJPCNode::ReadLine(nCtx, &tmp, iComponent);
			tmp.Release();
		} else {
			INT32 nX0 = NCSFloorDiv(pDst->GetX0(), nXCMul);
			INT32 nY0 = NCSFloorDiv(pDst->GetY0(), nYCMul);
			INT32 nX1 = NCSCeilDiv(pDst->GetX1(), nXCMul);
			INT32 nY1 = NCSCeilDiv(pDst->GetY1(), nYCMul);
			CNCSJPCRect rect(nX0, nY0, nX1, nY1);
			//CNCSJPCBuffer tmp(nX0, nY0, nX1, nY1, pDst->GetType());
			pCtx->SetInputBuffers(0, 1);
			UINT32 nBuffer = 0;

			CNCSJPCBuffer *pBuffer = pCtx->FindInputBuffer(0, nBuffer, rect, bRet, pDst->GetType());
			if(pBuffer) {
pBuffer->SetFlag(CNCSJPCBuffer::AT_LOCKED);
				bRet = CNCSJPCNode::ReadLine(nCtx, pBuffer, iComponent);
				nX1 = pDst->GetX1();

				switch(pDst->GetType()) {
					case BT_INT16:
						{
							INT16 *pSrc = (INT16*)pBuffer->GetPtr(0, nY0);
							INT16 *pBuf = (INT16*)pDst->GetPtr(0, pDst->GetY0());
							for(INT32 x = pDst->GetX0(); x < nX1; x++) {
								pBuf[x] = pSrc[x / nXCMul];
							}
						}
						break;
					case BT_INT32:
						{
							INT32 *pSrc = (INT32*)pBuffer->GetPtr(0, nY0);
							INT32 *pBuf = (INT32*)pDst->GetPtr(0, pDst->GetY0());
							for(INT32 x = pDst->GetX0(); x < nX1; x++) {
								pBuf[x] = pSrc[x / nXCMul];
							}
						}
						break;
					case BT_IEEE4:
						{
							IEEE4 *pSrc = (IEEE4*)pBuffer->GetPtr(0, nY0);
							IEEE4 *pBuf = (IEEE4*)pDst->GetPtr(0, pDst->GetY0());
							for(INT32 x = pDst->GetX0(); x < nX1; x++) {
								pBuf[x] = pSrc[x / nXCMul];
							}
						}
						break;
				}
			}
		}
	}
	pDst->UnSetFlags(CNCSJPCBuffer::AT_LOCKED);
	return(bRet);
}

// Write a line to the Component.
bool CNCSJPCComponent::WriteLine(ContextID nCtx, CNCSJPCBuffer *pSrc, UINT16 iComponent)
{
	return(m_Resolutions[m_Resolutions.size() - 1]->WriteLine(nCtx, pSrc, iComponent));
}

void CNCSJPCComponent::TraversePyramid(UINT32 nResolution, 
										UINT32 Flags,
										INT32 nTopX,
										INT32 nLeftY,
										INT32 nBottomX,
										INT32 nRightY,
										UINT32 nViewSizeX, 
										UINT32 nViewSizeY, 
										UINT32 &nBlocksTotal,
										UINT32 &nBlocksAvailable)
{
	// Create the precinct/subband/codeblock structure for the given view
	INT8 nBorder = 2 + 4 * CNCSJPCBuffer::Border(CNCSJPCBuffer::BT_IEEE4);

	// View extents are inclusive, but JP2 structure is exclusive, so move BR
	nBottomX += 1;
	nRightY += 1;

	if(nTopX == nBottomX || nLeftY == nRightY) {
		return;
	}
		
	bool bCachableView = (nViewSizeX <= NCSECW_MAX_VIEW_SIZE_TO_CACHE || nViewSizeY <= NCSJP2_STRIP_HEIGHT) && nViewSizeY <= NCSECW_MAX_VIEW_SIZE_TO_CACHE;

	// Offset the intersection view by the relative tile position
	CNCSJPCTilePartHeader *pTile0 = m_pTilePart->m_pJPC->GetTile(0);
	INT32 nXOffset = m_pTilePart->GetX0() - pTile0->GetX0();
	INT32 nYOffset = m_pTilePart->GetY0() - pTile0->GetY0();
	nTopX -= nXOffset;
	nBottomX -= nXOffset;
	nLeftY -= nYOffset;
	nRightY -= nYOffset;

	UINT32 nL = (UINT32)m_Resolutions.size() - 1;
	nBottomX = NCSFloorDiv(nBottomX, NCS2Pow(nL - nResolution));
	nTopX = NCSCeilDiv(nTopX, NCS2Pow(nL - nResolution));
	nRightY = NCSCeilDiv(nRightY, NCS2Pow(nL - nResolution));
	nLeftY = NCSFloorDiv(nLeftY, NCS2Pow(nL - nResolution));

	for(INT32 r = nResolution; r >= 0; r--) { 
		/*if(r > (int)nResolution) {
			if(r != 0) {
				nBottomX = NCSFloorDiv(nBottomX, 2);
				nTopX = NCSCeilDiv(nTopX, 2);
				nRightY = NCSCeilDiv(nRightY, 2);
				nLeftY = NCSFloorDiv(nLeftY, 2);
			}
		} else if(r <= (int)nResolution) */{
	//		if(r != 0) { 
				nBottomX = NCSFloorDiv(nBottomX, 2) + nBorder;
				nTopX = NCSCeilDiv(nTopX, 2) - nBorder;
				nRightY = NCSCeilDiv(nRightY, 2) + nBorder;
				nLeftY = NCSFloorDiv(nLeftY, 2) - nBorder;
				nTopX = NCSMax(0, nTopX);
				nLeftY = NCSMax(0, nLeftY);
	//		}
			CNCSJPCResolution *pResolution = m_Resolutions[r];

			INT32 nPY0 = pResolution->GetPY0()/*;CNCSJPCPrecinct::GetY0(pResolution, 0, 0)*/ >> 1;
			//INT32 nPY1 = CNCSJPCPrecinct::GetY1(pResolution, 0, 0) >> 1;
			INT32 nPX0 = pResolution->GetPX0()/*;CNCSJPCPrecinct::GetX0(pResolution, 0, 0)*/ >> 1;
			//INT32 nPX1 = CNCSJPCPrecinct::GetX1(pResolution, 0, 0) >> 1;

	/*		if(r != 0) {
				nPY0 = NCSMax(nPY0, pResolution->GetY0() >> 1);
				nPY1 = NCSMin(nPY1, pResolution->GetY1() >> 1);
				nPX0 = NCSMax(nPX0, pResolution->GetX0() >> 1);
				nPX1 = NCSMin(nPX1, pResolution->GetX1() >> 1);
			} else {
				nPY0 = NCSMax(nPY0, pResolution->GetY0());
				nPY1 = NCSMin(nPY1, pResolution->GetY1());
				nPX0 = NCSMax(nPX0, pResolution->GetX0());
				nPX1 = NCSMin(nPX1, pResolution->GetX1());
			}
	*/		UINT32 nPrecinctsWide = pResolution->GetNumPrecinctsWide();
			UINT32 nPrecinctsHigh = pResolution->GetNumPrecinctsHigh();
			INT32 nX0s = CNCSJPCSubBand::GetX0(pResolution, 0, 0, NCSJPC_HH);
			INT32 nY0s = CNCSJPCSubBand::GetY0(pResolution, 0, 0, NCSJPC_HH);

			INT32 nStartX = -1;
			INT32 nEndX = -1;
			INT32 nStartY = -1;
			INT32 nEndY = -1;

			for(UINT32 x = 0; x < nPrecinctsWide; x++) {
				INT32 nX1s = CNCSJPCSubBand::GetX1(pResolution, x, 0, NCSJPC_HH);
				INT32 nX0 = nX0s - 1;
				nX0 -= nPX0;

				if(nX0 <= nBottomX) {
					INT32 nX1 = nX1s + 1;
					nX1 -= nPX0;

					if(nX1 >= nTopX) {
						if(nX0 < 0) {
							nX1 -= nX0;
							nX0 = 0;
						}
						// if the tile is not above, below, left or right of the view, then it
						// intersects it
						if(!((nX1 <= nTopX) ||	// Left of view
							 (nX0 > nBottomX))) {	// Right of view
							if(nStartX == -1) {
								nStartX = x;
							}
							nEndX = x;
						}
					}
				} else {
					break;
				}
				nX0s = nX1s;
			}
			for(UINT32 y = 0; y < nPrecinctsHigh; y++) {
				INT32 nY1s = CNCSJPCSubBand::GetY1(pResolution, 0, y, NCSJPC_HH);
				INT32 nY0 = nY0s;
				nY0 -= nPY0;

				if(nY0 <= nRightY) {
					INT32 nY1 = nY1s;
					nY1 -= nPY0;

					if(nY1 >= nLeftY) {
						if(nY0 < 0) {
							nY1 -= nY0;
							nY0 = 0;
						}
						// if the tile is not above, below, left or right of the view, then it
						// intersects it
						if(!((nY1 <= nLeftY) ||	// above view
							 (nY0 > nRightY))) {	// below view
							if(nStartY == -1) {
								nStartY = y;
							}
							nEndY = y;
						}
					}
				} else {
					break;
				}
				nY0s = nY1s;
			}
			if(nStartY != -1 && nEndY != -1 && nStartX != -1 && nEndX != -1) {
				for(INT32 y = nStartY; y <= nEndY; y++) {
					for(INT32 x = nStartX; x <= nEndX; x++) {
						CNCSJPCPrecinct *pPrecinct = pResolution->m_Precincts.find(x, y);
						if((Flags & CREATE) && pPrecinct == NULL && bCachableView) {
							UINT32 p = x + y * nPrecinctsWide;
							pPrecinct = new CNCSJPCPrecinct(pResolution, p, m_pTilePart->m_pJPC->m_pStream->IsPacketStream());
							//pPrecinct->AddRef();
							pResolution->m_Precincts.insert(x, y, pPrecinct);
					//pPrecinct->CreateSubBands(true); 
						}
						if(pPrecinct) {
							if(Flags & STATS) {
								if(m_pTilePart->m_pJPC->m_pStream->IsPacketStream()) {
									CNCSJPCEcwpIOStream *pEcwp = (CNCSJPCEcwpIOStream*)m_pTilePart->m_pJPC->m_pStream;
									pEcwp->Lock();
									for(UINT32 l = 0; l < pPrecinct->m_Packets.size(); l++) {
										nBlocksTotal++;
										if(pEcwp->GetPacketStatus(pPrecinct->m_Packets[l]) >= CNCSJPCEcwpIOStream::CNCSJPCPacketStatus::RECEIVED) {
											nBlocksAvailable++;
										}
									}
									pEcwp->UnLock();
								} else {
									nBlocksTotal += (UINT32)pPrecinct->m_Packets.size();
									nBlocksAvailable += (UINT32)pPrecinct->m_Packets.size();
								}
							}
							if(Flags & ADDREF) {
								pPrecinct->AddRef();
							} 
							if(Flags & UNREF) {
								pPrecinct->UnRef();
							}
							if(m_pTilePart->m_pJPC->m_pStream->IsPacketStream()) {
								CNCSJPCEcwpIOStream *pEcwp = (CNCSJPCEcwpIOStream*)m_pTilePart->m_pJPC->m_pStream;
								if(Flags & REQUEST && pPrecinct->NrRefs() > 0) {
									pEcwp->RequestPrecinct(pPrecinct);
								} else if(Flags & CANCEL && pPrecinct->NrRefs() == 0) {
									pEcwp->CancelPrecinct(pPrecinct);
								}
							}
						}
					}
				}
			}
		}
	}
}
