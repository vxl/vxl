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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCResolution.cpp $
** CREATED:  12/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCResolution class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCResample.h"

#ifndef NCSJPCDCSHIFTNODE_H
#include "NCSJPCDCShiftNode.h"
#endif // NCSJPCDCSHIFTNODE_H
#ifndef NCSJPCPALETTENODE_H
#include "NCSJPCPaletteNode.h"
#endif // NCSJPCPALETTENODE_H
#ifndef NCSJPCTILEPARTHEADER_H
//#include "NCSJPCTilePartHeader.h"
#endif // NCSJPCTILEPARTHEADER_H
#ifndef NCSJP2FILEVIEW_H
#include "NCSJP2FileView.h"
#endif //NCSJP2FILEVIEW_H
//#include "NCSJPCEcwpIOStream.h"
#ifndef NCSJPCYCBCR2RGBNODE_H
#include "NCSJPCYCbCr2RGBNode.h"
#endif // NCSJPCYCBCR2RGBNODE_H
#ifndef NCSJPCICCNODE_H
#include "NCSJPCICCNode.h"
#endif // NCSJPCICCNODE_H


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//#define BILINEAR_INTERP_2x2(o, t, v00, v01, v10, v11, r, dx, dy) \
//									v00 = v00 + (v01 - v00) * (dx);\
//									v10 = v10 + (v11 - v10) * (dx);\
//									v00 = v00 + (v10 - v00) * (dy);\
//									o = (t)(v00 + r)

#define BILINEAR_INTERP_2x2_FLT(o, v00, v01, v10, v11, dx, dy) \
									o = (IEEE4)((v00 + (v01 - v00) * (dx)) + \
											((v10 + (v11 - v10) * (dx)) - \
											 (v00 + (v01 - v00) * (dx))) * (dy))
#define BILINEAR_INTERP_2x2_INT(o, v00, v01, v10, v11, dx, dy) \
								{ \
									float f;\
									BILINEAR_INTERP_2x2_FLT(f, v00, v01, v10, v11, dx, dy);\
									o = NCSfloatToInt32_RM(f); \
								}
#define BILINEAR_INTERP_2(v0, v1, d)	((v0) + ((v1) - (v0)) * (d))

CNCSJPCResample::Context::Context()
{
	m_pView = NULL;
}

CNCSJPCResample::Context::~Context()
{
}

CNCSJPCResample::Context::TileInput::TileInput()
{
	m_pMCT = NULL;
	m_pCSC = NULL;
}

CNCSJPCResample::Context::TileInput::ChannelInput::ChannelInput()
{				
	m_nResSample = 0;
	m_dResSampleFactorX = 0;
	m_dResSampleFactorY = 0;

	m_nResolutionWidth = 0;
	m_nResolutionHeight = 0;	

	m_nMax = 0;
	m_nMin = 0;

	m_pInput = NULL;
}

	/** Default constructor, initialises members */
CNCSJPCResample::CNCSJPCResample()
{
	m_pJPC = NULL;
	m_bClamp = true;
}

	/** Virtual destructor */
CNCSJPCResample::~CNCSJPCResample()
{
}

// Link a node as the input to this node.
bool CNCSJPCResample::Link(ContextID nCtx, CNCSJPC &JPC, CNCSJP2FileView *pFileView, UINT16 iComponent, UINT16 iChannel, CNCSJP2File::CNCSJP2HeaderBox &Header)
{
	Context *pCtx = (Context*)GetContext(nCtx);
	NCSFileViewSetInfo *pView = (NCSFileViewSetInfo*)pFileView->GetFileViewSetInfo();
	INT32 nTiles = JPC.GetNumXTiles() * JPC.GetNumYTiles();

	m_pJPC = &JPC;

	pCtx->m_Tiles.resize(nTiles);//>m_InputNodes.resize(nTiles);

	for(INT32 nTile = 0; nTile < nTiles; nTile++) {
		CNCSJPCTilePartHeader *pTilePart = JPC.GetTile(nTile);

		if(pTilePart) {
			CNCSJPCComponent *pComponent = pTilePart->m_Components[iComponent];

			Context::TileInput &TI = pCtx->m_Tiles[nTile];

			TI.m_CI.resize(NCSMax(iChannel + 1, (UINT16)TI.m_CI.size()));


			Context::TileInput::ChannelInput &CI = TI.m_CI[iChannel];

			if(Header.m_Palette.m_bValid && 
			   Header.m_ComponentMapping.m_bValid && 
			   Header.m_ComponentMapping.m_Mapping[iComponent].m_nType != CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ComponentMappingBox::DIRECT) {
				pComponent->m_bPaletted = true;
			}
			UINT8 nBits = pTilePart->m_pJPC->m_SIZ.m_Components[iComponent].m_nBits;

			if(pTilePart->m_pJPC->m_SIZ.m_Components[iComponent].m_bSigned) {
				CI.m_nMax = (UINT32)(1 << (nBits - 1)) - 1;
				CI.m_nMin = (-1 * (signed)CI.m_nMax - 1);
			} else {
				CI.m_nMax = (UINT32)(1 << nBits) - 1;
				CI.m_nMin = 0;
			}

			INT32 nXRsiz = JPC.m_SIZ.m_Components[iComponent].m_nXRsiz;
			INT32 nYRsiz = JPC.m_SIZ.m_Components[iComponent].m_nYRsiz;

			double dViewSampleX = (pView->nBottomX - pView->nTopX + 1) / (double)(pView->nSizeX);
			double dViewSampleY = (pView->nRightY - pView->nLeftY + 1) / (double)(pView->nSizeY);

			int r = CalculateResolutionLevel(pComponent, 
											pView->nTopX, pView->nLeftY, 
											pView->nBottomX, pView->nRightY, 
											pView->nSizeX, pView->nSizeY);

			CI.m_nResSample = NCS2Pow((UINT32)pComponent->m_Resolutions.size() - (r + 1));

			pCtx->m_pView = pView;
			//pCtx->m_pComponent = pComponent;
			//pCtx->m_nResolution = r;
			CI.m_dResSampleFactorX = dViewSampleX / CI.m_nResSample;//(dResSample * nXRsiz);
			CI.m_dResSampleFactorY = dViewSampleY / CI.m_nResSample;//(dResSample * nYRsiz);

			CI.m_nResolutionWidth = pComponent->m_Resolutions[r]->GetWidth() * nXRsiz;
			CI.m_nResolutionHeight = pComponent->m_Resolutions[r]->GetHeight() * nYRsiz;

			bool bInitCSC = false;
			CNCSJPCNode *pInput = NULL;

			if(pTilePart->m_Components[iComponent]->m_CodingStyle.m_SGcod.m_bMCT && JPC.m_SIZ.m_nCsiz >= 3) {
				//pCtx->m_MCTs.resize(nTiles);
				if(TI.m_pMCT == NULL) {
					TI.m_pMCT = new CNCSJPCMCTNode;
					pFileView->AddDynamicNode(TI.m_pMCT);
				}
			}
			if(Header.m_ColorSpecification.m_bValid && 
			   ((Header.m_ColorSpecification.m_eMethod == CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::ENUMERATED_COLORSPACE &&
				 Header.m_ColorSpecification.m_eEnumeratedColorspace == CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::YCbCr) ||
				(Header.m_ColorSpecification.m_eMethod == CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::RESTRICTED_ICC_PROFILE) ||
				(Header.m_ColorSpecification.m_eMethod == CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::ICC_PROFILE))) {
				// Input component indexes 
				std::vector<UINT16> Components;
				Components.resize(3);
				//pCtx->m_CSCs.resize(nTiles);

				if(Header.m_ChannelDefinition.m_bValid) {
					for(int i = 0; i < Header.m_ChannelDefinition.m_nEntries; i++) {
						CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::ChannelDefinition def = Header.m_ChannelDefinition.m_Definitions[i];

						if(def.m_eType == CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::COLOR) {
							switch(def.m_eAssociation) {
								case CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::Y:
										Components[0] = def.m_iChannel;
										if(def.m_iChannel == iComponent && TI.m_pCSC == NULL) {
											bInitCSC = true;
										}
									break;

								case CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::Cb:
										Components[1] = def.m_iChannel;
										if(def.m_iChannel == iComponent && TI.m_pCSC == NULL) {
											bInitCSC = true;
										}
									break;
								
								case CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::Cr:
										Components[2] = def.m_iChannel;
										if(def.m_iChannel == iComponent && TI.m_pCSC == NULL) {
											bInitCSC = true;
										}
									break;
							}
						}
					}
				} else {
					Components[0] = 0;
					Components[1] = 1;
					Components[2] = 2;
					if(iComponent < 3 && TI.m_pCSC == NULL) {
						bInitCSC = true;
					}
				}
				if(bInitCSC) {
					BOOLEAN bManageICC = TRUE;
					NCSecwGetConfig(NCSCFG_JP2_MANAGE_ICC, &bManageICC);
					if(!bManageICC) {
						bInitCSC = FALSE;
					}
				}
				if(bInitCSC) {
					if(Header.m_ColorSpecification.m_eMethod == CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::RESTRICTED_ICC_PROFILE ||
						Header.m_ColorSpecification.m_eMethod == CNCSJP2File::CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::ICC_PROFILE) {
						TI.m_pCSC = new CNCSJPCICC2RGBNode(JPC,
														   Header.m_ColorSpecification.m_pICCProfile,
														   Header.m_ColorSpecification.m_nICCLen);
					} else {
						// Need to do a color space conversion for this output channel
						TI.m_pCSC = new CNCSJPCYCbCr2RGBNode;
					}
					pFileView->AddDynamicNode(TI.m_pCSC);
					if(pTilePart->m_Components.size() == 3) {
						TI.m_pCSC->Link(nCtx, 3,
												BuildInputChain(nCtx, 
																JPC, 
																pFileView, 
																Components[0],
																0, 
																Header, 
																pTilePart),
												BuildInputChain(nCtx, 
																JPC, 
																pFileView, 
																Components[1],
																1, 
																Header, 
																pTilePart),
												BuildInputChain(nCtx, 
																JPC, 
																pFileView, 
																Components[2],
																2, 
																Header, 
																pTilePart));
					} else {
						TI.m_pCSC->Link(nCtx, 1,
												BuildInputChain(nCtx, 
																JPC, 
																pFileView, 
																Components[0],
																0, 
																Header, 
																pTilePart));
					}
				}
				if(TI.m_pCSC) {
					for(UINT32 i = 0; i < Components.size(); i++) {
						if(Components[i] == iComponent) {
							pInput = TI.m_pCSC;
							m_bClamp = true;
							break;
						}
					}
				}
			}
			
			if(!pInput) {
				pInput = BuildInputChain(nCtx,
										 JPC, 
										 pFileView, 
										 iComponent,
										 iChannel, 
										 Header, 
										 pTilePart);
			}
			//for(int c = (pTilePart->m_pMCT ? 0 : iComponent); c <= (pTilePart->m_pMCT ? 2 : iComponent); c++) 
			CNCSJPCTilePartHeader *pTile0 = JPC.GetTile(0);
			INT32 nX0 = pTilePart->GetX0() - pTile0->GetX0();
			INT32 nY0 = pTilePart->GetY0() - pTile0->GetY0();
			INT32 nX1 = nX0 + pTilePart->GetWidth();
			INT32 nY1 = nY0 + pTilePart->GetHeight();
			// Offset to current tile, and clip to valid tile region
			CNCSJPCNode *pOffset = new CNCSJPCNode();
			//INT32 nResSample = NCS2Pow(pCtx->m_pComponent->m_Resolutions.size() - (pCtx->m_nResolution + 1));

			pOffset->Assign((INT32)(nX0 / (double)CI.m_nResSample), 
							(INT32)(nY0 / (double)CI.m_nResSample),
							(INT32)(nX1 / (double)CI.m_nResSample), 
							(INT32)(nY1 / (double)CI.m_nResSample),
							BT_INT16,
							NULL);
			pOffset->Link(nCtx, 1, pInput);		
			pFileView->AddDynamicNode(pOffset);
			CI.m_pInput = pOffset;
		}
	}

	return(true);
}

// UnLink a node as the unput to this node.
bool CNCSJPCResample::UnLink(ContextID nCtx, UINT16 nInputs)
{
	Context *pCtx = (Context*)GetContext(nCtx);
	UINT32 n;

	for(n = 0; n < pCtx->m_Tiles.size(); n++) {
		if(pCtx->m_Tiles[n].m_pMCT) {
			pCtx->m_Tiles[n].m_pMCT->UnLink(nCtx);
		}
		if(pCtx->m_Tiles[n].m_pCSC) {
			pCtx->m_Tiles[n].m_pCSC->UnLink(nCtx);
		}
	}
	m_pJPC = NULL; 
	m_bClamp = true;
	return(CNCSJPCNodeTiler::UnLink(nCtx, nInputs));
}

// Read a line from the Resolution.
bool CNCSJPCResample::ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent)
{
	bool bRet = false;
	Context *pCtx = (Context*)GetContext(nCtx);
	Context::TileInput::ChannelInput &CI = pCtx->m_Tiles[0].m_CI[iComponent];

	if(CI.m_dResSampleFactorX == 1.0 && CI.m_dResSampleFactorY == 1.0) {
		// 1:1, just read input.

		CNCSJPCBuffer tmp;
		tmp.Assign(NCSFloor(pCtx->m_pView->nTopX / (double)CI.m_nResSample),
				   NCSFloor(pCtx->m_pView->nLeftY / (double)CI.m_nResSample) + pDst->GetY0(),
				   pDst->GetWidth(), (UINT32)1,
				   pDst->GetType(), 
				   pDst->GetPtr());

		bRet = CNCSJPCNodeTiler::ReadLine(nCtx, 
										&tmp,
										iComponent);
		tmp.Release();
		if(!bRet) {
			*(CNCSError*)this = CNCSJPCNode::GetError(nCtx);
		}
	} else  {
		double dYMul = (pCtx->m_pView->nRightY - pCtx->m_pView->nLeftY) / (double)(pCtx->m_pView->nSizeY * CI.m_nResSample);
		INT32 nInputWidth = 0;
		for(UINT32 t = 0; t < GetNumNodesWide(); t++) {
			nInputWidth += pCtx->m_Tiles[t].m_CI[iComponent].m_nResolutionWidth;
		}
		nInputWidth = NCSMin(nInputWidth, NCSCeilDiv(pCtx->m_pView->nBottomX+1, CI.m_nResSample) - NCSFloorDiv(pCtx->m_pView->nTopX, CI.m_nResSample));
		INT32 nY0 = NCSfloatToInt32(NCSFloorDiv(pCtx->m_pView->nLeftY, CI.m_nResSample) + (float)dYMul * pDst->GetY0());
		INT32 nY1 = nY0 + 1;
		
		CNCSJPCBuffer Buf0(NCSFloorDiv(pCtx->m_pView->nTopX, CI.m_nResSample), nY0,
						   (UINT32)nInputWidth, (UINT32)1,
						   pDst->GetType());
						   
		CNCSJPCBuffer Buf1(NCSFloorDiv(pCtx->m_pView->nTopX, CI.m_nResSample), nY1,
						   (UINT32)nInputWidth, (UINT32)1,
						   pDst->GetType());
						   			
		bRet = CNCSJPCNodeTiler::ReadLine(nCtx, &Buf0, iComponent);
		if(bRet) {
			bRet = CNCSJPCNodeTiler::ReadLine(nCtx, &Buf1, iComponent); 
			if(bRet) {
				UINT32 x;
				IEEE4 y = (IEEE4)(NCSFloorDiv(pCtx->m_pView->nLeftY, CI.m_nResSample) + (float)dYMul * pDst->GetY0() - nY0);
				UINT32 nWidth = pDst->GetWidth();
				void *pBuf0 = Buf0.GetPtr();
				void *pBuf1 = Buf1.GetPtr();
				void *pDst0 = pDst->GetPtr();
				double dXMul = (pCtx->m_pView->nBottomX - pCtx->m_pView->nTopX) / (double)(pCtx->m_pView->nSizeX * CI.m_nResSample);

				switch(pDst->GetType()) {
					case CNCSJPCBuffer::BT_INT16:
						FLT_TO_INT_INIT();
							for(x = 0; x < nWidth; x++) {
								double dX0 = x * dXMul;
								INT32 nX0 = NCSdoubleToInt32_RM(dX0-0.5);
								INT32 nX1 = nX0 + 1;
								dX0 -= nX0;

								BILINEAR_INTERP_2x2_INT(((INT16*)pDst0)[x],
														((INT16*)pBuf0)[nX0],
														((INT16*)pBuf0)[nX1],
														((INT16*)pBuf1)[nX0],
														((INT16*)pBuf1)[nX1],
														dX0,
														y);
							}
						FLT_TO_INT_FINI();
						break;
					case CNCSJPCBuffer::BT_INT32:
						FLT_TO_INT_INIT();
							for(x = 0; x < nWidth; x++) {
								double dX0 = x * dXMul;
								INT32 nX0 = NCSdoubleToInt32_RM(dX0-0.5);
								INT32 nX1 = nX0 + 1;
								dX0 -= nX0;

								BILINEAR_INTERP_2x2_INT(((INT32*)pDst0)[x],
														((INT32*)pBuf0)[nX0],
														((INT32*)pBuf0)[nX1],
														((INT32*)pBuf1)[nX0],
														((INT32*)pBuf1)[nX1],
														dX0,
														y);
							}
						FLT_TO_INT_FINI();
						break;
					case CNCSJPCBuffer::BT_IEEE4:
						{
							for(x = 0; x < nWidth; x++) {
								double dX0 = x * dXMul;
								INT32 nX0 = NCSdoubleToInt32_RM(dX0-0.5);
								INT32 nX1 = nX0 + 1;
								dX0 -= nX0;

								BILINEAR_INTERP_2x2_FLT(((IEEE4*)pDst0)[x],
														((IEEE4*)pBuf0)[nX0],
														((IEEE4*)pBuf0)[nX1],
														((IEEE4*)pBuf1)[nX0],
														((IEEE4*)pBuf1)[nX1],
														dX0,
														y);
							}
						}
						break;
				}
			} 
		}
		if(!bRet) {
			*(CNCSError*)this = CNCSJPCNode::GetError(nCtx);
		}
	}
	if(m_bClamp) {
		pDst->Clip(CI.m_nMin, CI.m_nMax);
	}							
	return(bRet);

}

CNCSJPCNode::Context *CNCSJPCResample::GetContext(ContextID nCtx, bool bAutoConstruct)
{
	CNCSJPCNode::Context *pCtx = CNCSJPCNode::GetContext(nCtx, false);
	if(!pCtx && bAutoConstruct) {
		pCtx = new Context;
		SetContext(nCtx, pCtx);
	}
	return(pCtx);
}

UINT8	CNCSJPCResample::CalculateResolutionLevel(CNCSJPCComponent *pComponent,
												  INT32 nTopX, 
												  INT32 nLeftY,
												  INT32 nBottomX,
												  INT32 nRightY,
												  UINT32 nSizeX,
												  UINT32 nSizeY)
{
	if(pComponent->m_bPaletted) {
		// Paletted components are always read at full resolution, 
		// then subsampled to the view.  This is because you only get
		// full lossless decoding at full resolution
		return((UINT8)pComponent->m_Resolutions.size() - 1);
	} else {
		double dViewSampleX = (nBottomX - nTopX + 1) / (double)(nSizeX);
		double dViewSampleY = (nRightY - nLeftY + 1) / (double)(nSizeY);
//	INT32 nXRsiz = pComponent->m_pTilePart->m_pJPC->m_SIZ.m_Components[pComponent->m_iComponent].m_nXRsiz;
//	INT32 nYRsiz = pComponent->m_pTilePart->m_pJPC->m_SIZ.m_Components[pComponent->m_iComponent].m_nYRsiz;

	// If X&Y Rsiz are equal and powers of 2, alter the resolution level we read.
	// Otherwise, an additional node is added elsewhere if Rsiz != 1 and we have an 
	// MCT or Color node.
//	if(nXRsiz == nYRsiz && nXRsiz % 2 == 0) {
//		dViewSampleX /= nXRsiz;
//		dViewSampleY /= nYRsiz;
//	}
		int r = NCSMax(0, (int)pComponent->m_Resolutions.size() - 1);

		while((NCS2Pow((UINT32)pComponent->m_Resolutions.size() - (r + 1)) * 2.0 <= NCSMin(dViewSampleX, dViewSampleY)) && r > 0) {
			r--;
		}
		return(r);
	}
}

CNCSJPCNode *CNCSJPCResample::BuildInputChain(ContextID nCtx, 
											  CNCSJPC &JPC, 
											  CNCSJP2FileView *pFileView, 
											  UINT16 iComponent, 
											  UINT16 iChannel, 
											  CNCSJP2File::CNCSJP2HeaderBox &Header,
											  CNCSJPCTilePartHeader *pTilePart)
{
	Context *pCtx = (Context*)GetContext(nCtx);
	CNCSJPCComponent *pComponent = pTilePart->m_Components[iComponent];
	NCSFileViewSetInfo *pView = (NCSFileViewSetInfo*)pFileView->GetFileViewSetInfo();
	Context::TileInput &TI = pCtx->m_Tiles[pTilePart->m_SOT.m_nIsot];
	Context::TileInput::ChannelInput &CI = TI.m_CI[iChannel];

	INT32 nXRsiz = pTilePart->m_pJPC->m_SIZ.m_Components[iComponent].m_nXRsiz;
	INT32 nYRsiz = pTilePart->m_pJPC->m_SIZ.m_Components[iComponent].m_nYRsiz;

	double dViewSampleX = (pView->nBottomX - pView->nTopX + 1) / (double)(pView->nSizeX);
	double dViewSampleY = (pView->nRightY - pView->nLeftY + 1) / (double)(pView->nSizeY);

	int r = CalculateResolutionLevel(pComponent, 
									pView->nTopX, pView->nLeftY, 
									pView->nBottomX, pView->nRightY, 
									pView->nSizeX, pView->nSizeY);
	UINT32 nResSample = NCS2Pow((UINT32)pComponent->m_Resolutions.size() - (r + 1));

	pComponent->Link(nCtx, r, 
					 nXRsiz,
					 nYRsiz);
	//NCSMax(1, (nResSample * nXRsiz)/(INT32)dViewSampleX), 
	//				 NCSMax(1, (nResSample * nYRsiz)/(INT32)dViewSampleY));
	CNCSJPCNode *pInput = pComponent;
	
		// Multi-component/Intra component transform on first 3 components only.
		// Must be before DC Shift
	if(TI.m_pMCT && iComponent < 3) {
		TI.m_pMCT->Link(nCtx, 3,
						pTilePart->m_Components[0],
						pTilePart->m_Components[1],
						pTilePart->m_Components[2]);
		pInput = TI.m_pMCT;
	}
		// DC Shift before JP2 Palette
	if(JPC.m_SIZ.m_Components[iComponent].m_bSigned == false) {
		CNCSJPCDCShiftNode *pDC = new CNCSJPCDCShiftNode;
		pDC->Link(nCtx, 1, pInput);
		pDC->Init(nCtx, JPC, iComponent);
		pInput = pDC;
		pFileView->AddDynamicNode(pDC);
//		m_bClamp = false;
	}
		// JP2 Palette must be before resample
	if(pComponent->m_bPaletted) {
		CNCSJPCPaletteNode *pPalette = new CNCSJPCPaletteNode();
		pPalette->Link(nCtx, 1, pInput, Header, iChannel);
		pInput = pPalette;
		pFileView->AddDynamicNode(pPalette);
	}

		// Create the precinct/subband/codeblock structure for the given view
	pComponent->TraversePyramid(r, 
								CNCSJPCComponent::CREATE|CNCSJPCComponent::ADDREF|CNCSJPCComponent::REQUEST, 
								pView->nTopX, pView->nLeftY, 
								pView->nBottomX, pView->nRightY, 
								pView->nSizeX, pView->nSizeY,
								pView->nBlocksInView, 
								pView->nBlocksAvailableAtSetView);
	pView->nBlocksAvailable = pView->nBlocksAvailableAtSetView;

	return(pInput);
}

// Get normal Node Width.
UINT32 CNCSJPCResample::GetNodeWidth(UINT16 iComponent)
{
	return(m_pJPC ? m_pJPC->m_SIZ.m_nXTsiz : 0);
}

// Get normal Node Height.
UINT32 CNCSJPCResample::GetNodeHeight(UINT16 iComponent)
{
	return(m_pJPC ? m_pJPC->m_SIZ.m_nYTsiz : 0);
}

// Get number of nodes wide.
UINT32 CNCSJPCResample::GetNumNodesWide(UINT16 iComponent)
{
	return(m_pJPC ? m_pJPC->GetNumXTiles() : 0);
}
// Get number of nodes high.
UINT32 CNCSJPCResample::GetNumNodesHigh(UINT16 iComponent)
{
	return(m_pJPC ? m_pJPC->GetNumYTiles() : 0);
}
// Get pointer to specific node.
CNCSJPCNode *CNCSJPCResample::GetNodePtr(UINT32 nNode, UINT16 iComponent)
{
	if(iComponent < ((Context*)m_pCtx)->m_Tiles[nNode].m_CI.size()) {
		return(((Context*)m_pCtx)->m_Tiles[nNode].m_CI[iComponent].m_pInput);
	} else {
		return(NULL);
	}
}
