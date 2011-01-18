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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCPaletteNode.cpp $
** CREATED:  20/03/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCPaletteNode class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"
#include "NCSJPCPaletteNode.h"
#include "NCSJPCTilePartHeader.h"

CNCSJPCNode::CNCSJPCNodeTracker CNCSJPCPaletteNode::sm_Tracker("CNCSJPCPaletteNode", sizeof(CNCSJPCPaletteNode));

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCPaletteNode::CNCSJPCPaletteNode()
{
	sm_Tracker.Add();
}

// Destructor
CNCSJPCPaletteNode::~CNCSJPCPaletteNode()
{
	sm_Tracker.Remove();
}

CNCSJPCPaletteNode::Context::Context()
{
	m_eType = NCSCT_UINT8;
	m_pEntries = (void*)NULL;
}

CNCSJPCPaletteNode::Context::~Context()
{
	if(m_pEntries) {
		NCSFree(m_pEntries);
	}
}

// Link a node as the unput to this node.
bool CNCSJPCPaletteNode::Link(ContextID nCtx, UINT16 nInputs, CNCSJPCNode *pInput, CNCSJP2File::CNCSJP2HeaderBox &Header, INT32 iChannel)
{
	bool bRet = CNCSJPCNode::Link(nCtx, nInputs, pInput);

	if(bRet) {
		Context *pCtx = (Context*)GetContext(nCtx);

		if(pCtx) {
			INT16 nCol = Header.m_ComponentMapping.m_Mapping[iChannel].m_iPalette;
			UINT8 nBytes = Header.m_Palette.m_Bits[nCol].m_nBits / 8 + ((Header.m_Palette.m_Bits[nCol].m_nBits % 8) ? 1 : 0);
			UINT32 x;

			switch(nBytes) {
				case 1:	 
						if(Header.m_Palette.m_Bits[nCol].m_bSigned) {
							pCtx->m_eType = NCSCT_INT8;
							pCtx->m_pEntries = NCSMalloc(sizeof(INT8) * Header.m_Palette.m_nEntries, FALSE);
							for(x = 0; x < Header.m_Palette.m_nEntries; x++) {
								((INT8*)pCtx->m_pEntries)[x] = *((INT8*)Header.m_Palette.m_Entries[x][nCol]);
							}
						} else {
							pCtx->m_eType = NCSCT_UINT8;
							pCtx->m_pEntries = NCSMalloc(sizeof(UINT8) * Header.m_Palette.m_nEntries, FALSE);
							for(x = 0; x < Header.m_Palette.m_nEntries; x++) {
								((UINT8*)pCtx->m_pEntries)[x] = *(UINT8*)(Header.m_Palette.m_Entries[x][nCol]);
							}
						}
					break;
				case 2:	 
						if(Header.m_Palette.m_Bits[nCol].m_bSigned) {
							pCtx->m_eType = NCSCT_INT16;
							pCtx->m_pEntries = NCSMalloc(sizeof(INT16) * Header.m_Palette.m_nEntries, FALSE);
							for(x = 0; x < Header.m_Palette.m_nEntries; x++) {
								((INT16*)pCtx->m_pEntries)[x] = *((INT16*)Header.m_Palette.m_Entries[x][nCol]);
							}
						} else {
							pCtx->m_eType = NCSCT_UINT16;
							pCtx->m_pEntries = NCSMalloc(sizeof(UINT16) * Header.m_Palette.m_nEntries, FALSE);
							for(x = 0; x < Header.m_Palette.m_nEntries; x++) {
								((UINT16*)pCtx->m_pEntries)[x] = *((UINT16*)Header.m_Palette.m_Entries[x][nCol]);
							}
						}
					break;
				case 3:	 
				case 4:	 
						if(Header.m_Palette.m_Bits[nCol].m_bSigned) {
							pCtx->m_eType = NCSCT_INT32;
							pCtx->m_pEntries = NCSMalloc(sizeof(INT32) * Header.m_Palette.m_nEntries, FALSE);
							for(x = 0; x < Header.m_Palette.m_nEntries; x++) {
								((INT32*)pCtx->m_pEntries)[x] = *((INT32*)Header.m_Palette.m_Entries[x][nCol]);
							}
						} else {
							pCtx->m_eType = NCSCT_UINT32;
							pCtx->m_pEntries = NCSMalloc(sizeof(UINT32) * Header.m_Palette.m_nEntries, FALSE);
							for(x = 0; x < Header.m_Palette.m_nEntries; x++) {
								((UINT32*)pCtx->m_pEntries)[x] = *((UINT32*)Header.m_Palette.m_Entries[x][nCol]);
							}
						}
					break;
				case 5:	 
				case 6:	 
				case 7:	 
				case 8:	 
						if(Header.m_Palette.m_Bits[nCol].m_bSigned) {
							pCtx->m_eType = NCSCT_INT64;
							pCtx->m_pEntries = NCSMalloc(sizeof(INT64) * Header.m_Palette.m_nEntries, FALSE);
							for(x = 0; x < Header.m_Palette.m_nEntries; x++) {
								((INT64*)pCtx->m_pEntries)[x] = *((INT64*)Header.m_Palette.m_Entries[x][nCol]);
							}
						} else {
							pCtx->m_eType = NCSCT_UINT64;
							pCtx->m_pEntries = NCSMalloc(sizeof(UINT64) * Header.m_Palette.m_nEntries, FALSE);
							for(x = 0; x < Header.m_Palette.m_nEntries; x++) {
								((UINT64*)pCtx->m_pEntries)[x] = *((UINT64*)Header.m_Palette.m_Entries[x][nCol]);
							}
						}
					break;
			}
		}
	}
	return(bRet);
}

// Read a line from the input node.
bool CNCSJPCPaletteNode::ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent)
{
	bool bRet = CNCSJPCNode::ReadLine(nCtx, pDst, iComponent);
	if(bRet) {
		Context *pCtx = (Context*)GetContext(nCtx);
		UINT32 x;
		UINT32 nWidth = pDst->GetWidth();
		void *pBuffer = pDst->GetPtr();

		switch(pDst->GetType()) {
			case CNCSJPCBuffer::BT_INT16:
					switch(pCtx->m_eType) {
						case NCSCT_INT8:
								for(x = 0; x < nWidth; x++) {
									((INT16*)pBuffer)[x] = ((INT8*)pCtx->m_pEntries)[((INT16*)pBuffer)[x]];
								}
							break;
						case NCSCT_UINT8:
								for(x = 0; x < nWidth; x++) {
									((INT16*)pBuffer)[x] = ((UINT8*)pCtx->m_pEntries)[((INT16*)pBuffer)[x]];
								}
							break;
						case NCSCT_INT16:
								for(x = 0; x < nWidth; x++) {
									((INT16*)pBuffer)[x] = ((INT16*)pCtx->m_pEntries)[((INT16*)pBuffer)[x]];
								}
							break;
						case NCSCT_UINT16:
								for(x = 0; x < nWidth; x++) {
									((INT16*)pBuffer)[x] = ((UINT16*)pCtx->m_pEntries)[((INT16*)pBuffer)[x]];
								}
							break;
						case NCSCT_INT32:
								for(x = 0; x < nWidth; x++) {
									((INT16*)pBuffer)[x] = ((INT32*)pCtx->m_pEntries)[((INT16*)pBuffer)[x]];
								}
							break;
						case NCSCT_UINT32:
								for(x = 0; x < nWidth; x++) {
									((INT16*)pBuffer)[x] = ((UINT32*)pCtx->m_pEntries)[((INT16*)pBuffer)[x]];
								}
							break;
						case NCSCT_INT64:
								for(x = 0; x < nWidth; x++) {
									((INT16*)pBuffer)[x] = (INT16)((INT64*)pCtx->m_pEntries)[((INT16*)pBuffer)[x]];
								}
							break;
						case NCSCT_UINT64:
								for(x = 0; x < nWidth; x++) {
									((INT16*)pBuffer)[x] = (INT16)((UINT64*)pCtx->m_pEntries)[((INT16*)pBuffer)[x]];
								}
							break;
					}
				break;

			case CNCSJPCBuffer::BT_INT32:
					switch(pCtx->m_eType) {
						case NCSCT_INT8:
								for(x = 0; x < nWidth; x++) {
									((INT32*)pBuffer)[x] = ((INT8*)pCtx->m_pEntries)[((INT32*)pBuffer)[x]];
								}
							break;
						case NCSCT_UINT8:
								for(x = 0; x < nWidth; x++) {
									((INT32*)pBuffer)[x] = ((UINT8*)pCtx->m_pEntries)[((INT32*)pBuffer)[x]];
								}
							break;
						case NCSCT_INT16:
								for(x = 0; x < nWidth; x++) {
									((INT32*)pBuffer)[x] = ((INT16*)pCtx->m_pEntries)[((INT32*)pBuffer)[x]];
								}
							break;
						case NCSCT_UINT16:
								for(x = 0; x < nWidth; x++) {
									((INT32*)pBuffer)[x] = ((UINT16*)pCtx->m_pEntries)[((INT32*)pBuffer)[x]];
								}
							break;
						case NCSCT_INT32:
								for(x = 0; x < nWidth; x++) {
									((INT32*)pBuffer)[x] = ((INT32*)pCtx->m_pEntries)[((INT32*)pBuffer)[x]];
								}
							break;
						case NCSCT_UINT32:
								for(x = 0; x < nWidth; x++) {
									((INT32*)pBuffer)[x] = ((UINT32*)pCtx->m_pEntries)[((INT32*)pBuffer)[x]];
								}
							break;
						case NCSCT_INT64:
								for(x = 0; x < nWidth; x++) {
									((INT32*)pBuffer)[x] = (INT32)((INT64*)pCtx->m_pEntries)[((INT32*)pBuffer)[x]];
								}
							break;
						case NCSCT_UINT64:
								for(x = 0; x < nWidth; x++) {
									((INT32*)pBuffer)[x] = (INT32)((UINT64*)pCtx->m_pEntries)[((INT32*)pBuffer)[x]];
								}
							break;
					}
				break;

			case CNCSJPCBuffer::BT_IEEE4:
					switch(pCtx->m_eType) {
						case NCSCT_INT8:
								for(x = 0; x < nWidth; x++) {
									((IEEE4*)pBuffer)[x] = ((INT8*)pCtx->m_pEntries)[(INT32)((IEEE4*)pBuffer)[x]];
								}
							break;
						case NCSCT_UINT8:
								for(x = 0; x < nWidth; x++) {
									((IEEE4*)pBuffer)[x] = ((UINT8*)pCtx->m_pEntries)[(INT32)((IEEE4*)pBuffer)[x]];
								}
							break;
						case NCSCT_INT16:
								for(x = 0; x < nWidth; x++) {
									((IEEE4*)pBuffer)[x] = ((INT16*)pCtx->m_pEntries)[(INT32)((IEEE4*)pBuffer)[x]];
								}
							break;
						case NCSCT_UINT16:
								for(x = 0; x < nWidth; x++) {
									((IEEE4*)pBuffer)[x] = ((UINT16*)pCtx->m_pEntries)[(INT32)((IEEE4*)pBuffer)[x]];
								}
							break;
						case NCSCT_INT32:
								for(x = 0; x < nWidth; x++) {
									((IEEE4*)pBuffer)[x] = (IEEE4)((INT32*)pCtx->m_pEntries)[(INT32)((IEEE4*)pBuffer)[x]];
								}
							break;
						case NCSCT_UINT32:
								for(x = 0; x < nWidth; x++) {
									((IEEE4*)pBuffer)[x] = (IEEE4)((UINT32*)pCtx->m_pEntries)[(INT32)((IEEE4*)pBuffer)[x]];
								}
							break;
						case NCSCT_INT64:
								for(x = 0; x < nWidth; x++) {
									((IEEE4*)pBuffer)[x] = (IEEE4)((INT64*)pCtx->m_pEntries)[(INT32)((IEEE4*)pBuffer)[x]];
								}
							break;
						case NCSCT_UINT64:
								for(x = 0; x < nWidth; x++) {
#ifdef _WIN32_WCE
									((IEEE4*)pBuffer)[x] = (IEEE4)((INT64*)pCtx->m_pEntries)[(INT32)((IEEE4*)pBuffer)[x]];
#else
									((IEEE4*)pBuffer)[x] = (IEEE4)((UINT64*)pCtx->m_pEntries)[(INT32)((IEEE4*)pBuffer)[x]];
#endif
								}
							break;
					}
				break;
		}
	}
	return(bRet);
}



CNCSJPCNode::Context *CNCSJPCPaletteNode::GetContext(ContextID nCtx, bool bAutoConstruct)
{
	CNCSJPCNode::Context *pCtx = CNCSJPCNode::GetContext(nCtx, false);
	if(!pCtx && bAutoConstruct) {
		pCtx = new Context;
		SetContext(nCtx, pCtx);
	}
	return(pCtx);
}
