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

#include "NCSJPCNodeTiler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

	/** Default constructor, initialises members */
CNCSJPCNodeTiler::CNCSJPCNodeTiler()
{
	m_pCtx = NULL;
}

	/** Virtual destructor */
CNCSJPCNodeTiler::~CNCSJPCNodeTiler()
{
}

//UnLink a node as the unput to this node.
bool CNCSJPCNodeTiler::UnLink(ContextID nCtx, UINT16 nInputs)
{
	bool bRet = true;
	m_pCtx = (Context*)GetContext(nCtx);
	UINT32 nNodes = GetNumNodesWide() * GetNumNodesHigh();
	for(UINT32 n = 0; n < (UINT32)NCSMax(1, nInputs); n++) {
		for(UINT32 i = 0; i < nNodes; i++) {
			CNCSJPCNode *pNode = GetNodePtr(i, (UINT16)n);
			if(pNode) {
				bRet &= pNode->UnLink(nCtx);
			}
		}
	}
	return(CNCSJPCNode::UnLink(nCtx));
}

// Read a line.
bool CNCSJPCNodeTiler::ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent)
{
	bool bRet = false;
	INT32 nCurY = 0;
	INT32 nBufferOffset = 0;
	UINT32 nNodesWide = GetNumNodesWide();
	UINT32 nNodesHigh = GetNumNodesHigh();
	INT32 nX = pDst->GetX0();
	INT32 nY = pDst->GetY0();
	UINT32 nWidth = pDst->GetWidth();
	
	m_pCtx = (Context*)GetContext(nCtx);

	if(nNodesHigh && nNodesWide) {
		CNCSJPCNode *pNode;
		if(nNodesHigh == 1 && nNodesWide == 1) {
			pNode = GetNodePtr(0, iComponent);
			if(pNode && pNode->GetX0() <= nX && pNode->GetX1() >= pDst->GetX1() &&
				pNode->GetY0() <= nY && pNode->GetY1() >= pDst->GetY1()) {
				return(pNode->ReadLine(nCtx, pDst, iComponent));
			}
		}
		pNode = GetNodePtr(0, iComponent);
		if(pNode) {
			INT32 nY0 = pNode->GetY0();
			UINT32 ny;
			for(ny = 0; ny < nNodesHigh; ny++) {
				pNode = GetNodePtr(ny * nNodesWide);
				if(pNode) {
					INT32 nY1 = pNode->GetY1();
					UINT32 nNodeHeight = nY1 - nY0;
					nY0 = nY1;

					if(nY >= nCurY && nY < nCurY + (INT32)nNodeHeight) {
						break;
					}
					nCurY += nNodeHeight;
				} else {
					bRet = false;
					break;
				}
			}
			if(ny < nNodesHigh) {
				INT32 nCurX = 0;
				bRet = false;
				pNode = GetNodePtr(ny * nNodesWide, iComponent);
				if(pNode) {
					INT32 nX0 = pNode->GetX0();
					CNCSJPCBuffer tmp;
					
					for(UINT32 nx = 0; nx < nNodesWide && nWidth; nx++) {
						CNCSJPCNode *pNode = GetNodePtr(nx + ny * nNodesWide, iComponent);
						
						if(pNode) {
							INT32 nX1 = pNode->GetX1();
							UINT32 nNodeWidth = nX1 - nX0;//pCB->GetWidth();
							nX0 = nX1;
							if(nX >= nCurX && nX < nCurX + (INT32)nNodeWidth) {
								UINT32 nNodeReadWidth = NCSMin(nWidth, nNodeWidth - (nX - nCurX));

								tmp.Assign(nX - nCurX, nY - nCurY, nNodeReadWidth, (UINT32)1, pDst->GetType(), (UINT8*)pDst->GetPtr() + pDst->Size(pDst->GetType(), nBufferOffset), pDst->GetStep());

								bRet = pNode->ReadLine(nCtx, 
													   &tmp,
													   iComponent);
								tmp.Release();
								if(!bRet) {
									*(CNCSError*)this = *(CNCSError*)pNode;
									break;
								}
								nX += nNodeReadWidth;
								nBufferOffset += nNodeReadWidth;
								nWidth -= nNodeReadWidth;
							}
							nCurX += nNodeWidth;
						} else {
							bRet = false;
							break;
						}
					}
				} else {
					bRet = false;
				}
			}
		}
	}
	if(nWidth) {
		memset(((UINT8*)pDst->GetPtr()) + Size(pDst->GetType(), nBufferOffset), 0, (size_t)Size(pDst->GetType(), nWidth));
		bRet = true;
	}
	return(bRet);
}

// Write a BufferType line to the output.
bool CNCSJPCNodeTiler::WriteLine(ContextID nCtx, CNCSJPCBuffer *pSrc, UINT16 iComponent)
{
	bool bRet = true;
	UINT32 nNodesWide = GetNumNodesWide();
	UINT32 nNodesHigh = GetNumNodesHigh();

	if(nNodesWide == 1 && nNodesHigh == 1) {
		bRet = GetNodePtr(0)->WriteLine(nCtx, pSrc, iComponent);
	} else {
		UINT32 nNodeHeight = GetNodeHeight();
		INT32 nNodeY = (pSrc->GetY0() - GetY0()) / nNodeHeight;
		UINT32 nNodesOffset = nNodeY * nNodesWide;

		nNodesWide += nNodesOffset;
		for(UINT32 nNode = nNodesOffset; nNode < nNodesWide; nNode++) {
			bRet &= GetNodePtr(nNode)->WriteLine(nCtx, pSrc, iComponent);
		}
	}
	return(bRet);
}
