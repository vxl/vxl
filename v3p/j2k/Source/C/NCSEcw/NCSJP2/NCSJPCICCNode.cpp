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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCICCNode.cpp $
** CREATED:  20/03/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCICC2RGBNode class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCICCNode.h"
#include "NCSJPC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCICC2RGBNode::CNCSJPCICC2RGBNode(CNCSJPC &JPC, void *pICCProfile, UINT32 nLen)
{
#ifdef NCSJPC_USE_LCMS
	m_ICCProfile = cmsOpenProfileFromMem(pICCProfile, nLen);
	m_sRGBProfile = cmsCreate_sRGBProfile();
	UINT16 nComponents = (UINT16)JPC.m_SIZ.m_Components.size();
	
	if(JPC.m_SIZ.m_Components[0].m_bSigned ||
	   JPC.m_SIZ.m_Components[NCSMin(1, nComponents - 1)].m_bSigned ||
	   JPC.m_SIZ.m_Components[NCSMin(2, nComponents - 1)].m_bSigned ||
	   JPC.GetTile()->m_Components[0]->m_CodingStyle.m_SPcod.m_eTransformation != CNCSJPCCodingStyleParameter::REVERSIBLE_5x3) {
		m_Transform = cmsCreateTransform(m_ICCProfile,
			nComponents >= 3 ? TYPE_RGB_DBL : TYPE_GRAY_DBL,
										m_sRGBProfile,
										TYPE_RGB_DBL,
										INTENT_PERCEPTUAL,
										0);  
	} else {
		m_Transform = cmsCreateTransform(m_ICCProfile,
			nComponents >= 3 ? TYPE_RGB_16 : TYPE_GRAY_16,
										m_sRGBProfile,
										TYPE_RGB_16,
										INTENT_PERCEPTUAL,
										0); 
	}
	m_pJPC = &JPC;
#endif //NCSJPC_USE_LCMS
}

// Destructor
CNCSJPCICC2RGBNode::~CNCSJPCICC2RGBNode()
{
#ifdef NCSJPC_USE_LCMS
	if(m_Transform != NULL) {
		cmsDeleteTransform(m_Transform);
	}
	if(m_ICCProfile != NULL) {
		cmsCloseProfile(m_ICCProfile);
	}
	if(m_sRGBProfile != NULL) {
		cmsCloseProfile(m_sRGBProfile);
	}
#endif //NCSJPC_USE_LCMS
}

#ifdef NCSJPC_USE_LCMS
// Read a line from the input node.
bool CNCSJPCICC2RGBNode::ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent)
{
	bool bRet = false;
	Context *pCtx = GetContext(nCtx);
	
	if(m_Transform && pCtx->NrInputs() >= 3) {
		bool bDoRead = false;
		int nInput;
		UINT32 nBuffer = 0;
		CNCSJPCRect Rect(pDst->GetX0(), pDst->GetY0(), pDst->GetWidth(), (UINT32)1);

		for(nInput = 0; nInput < 3; nInput++) {
			pCtx->SetInputBuffers(nInput, 2);
			bool bFound = false;
			pCtx->FindInputBuffer(nInput, nBuffer, Rect, bFound, pDst->GetType());
			if(!bFound) {
				bDoRead = true;
			}
		}

		CNCSJPCBuffer *pBuf0 = pCtx->GetInputBuffer(0, (UINT8)nBuffer);
		CNCSJPCBuffer *pBuf1 = pCtx->GetInputBuffer(1, (UINT8)nBuffer);
		CNCSJPCBuffer *pBuf2 = pCtx->GetInputBuffer(2, (UINT8)nBuffer);
		bRet = true;

		if(pBuf0 && pBuf1 && pBuf2) {
			if(bDoRead) {
				bRet = ReadInputLines(nCtx, 3, (UINT32)0, pBuf0, (UINT32)1, pBuf1, (UINT32)2, pBuf2);
				if(bRet) {
					UINT32 x;
					UINT32 nWidth = pDst->GetWidth();
					void *pPtr0 = pBuf0->GetPtr();
					void *pPtr1 = pBuf1->GetPtr();
					void *pPtr2 = pBuf2->GetPtr();
					UINT16 *pRGB = (UINT16*)NCSMalloc(sizeof(INT16) * nWidth * 3, FALSE);
					INT8 n0 = 16 - m_pJPC->m_SIZ.m_Components[0].m_nBits;
					INT8 n1 = 16 - m_pJPC->m_SIZ.m_Components[1].m_nBits;
					INT8 n2 = 16 - m_pJPC->m_SIZ.m_Components[2].m_nBits;

					if(m_pJPC->m_SIZ.m_Components[0].m_bSigned ||
					   m_pJPC->m_SIZ.m_Components[1].m_bSigned ||
					   m_pJPC->m_SIZ.m_Components[2].m_bSigned ||
					   pDst->GetType() == BT_IEEE4) {

						IEEE8 *pRGB = (IEEE8*)NCSMalloc(sizeof(IEEE8) * nWidth * 3, FALSE);
						INT32 nAdd0 = 0;
						INT32 nAdd1 = 0;
						INT32 nAdd2 = 0;
						IEEE8 dDiv0 = NCS2Pow(m_pJPC->m_SIZ.m_Components[0].m_nBits);
						IEEE8 dDiv1 = NCS2Pow(m_pJPC->m_SIZ.m_Components[1].m_nBits);
						IEEE8 dDiv2 = NCS2Pow(m_pJPC->m_SIZ.m_Components[2].m_nBits);
						if(m_pJPC->m_SIZ.m_Components[0].m_bSigned) {
							nAdd0 = 0x1 << (m_pJPC->m_SIZ.m_Components[0].m_nBits - 1);
						}
						if(m_pJPC->m_SIZ.m_Components[1].m_bSigned) {
							nAdd1 = 0x1 << (m_pJPC->m_SIZ.m_Components[1].m_nBits - 1);
						}
						if(m_pJPC->m_SIZ.m_Components[2].m_bSigned) {
							nAdd2 = 0x1 << (m_pJPC->m_SIZ.m_Components[2].m_nBits - 1);
						}
						pBuf0->Add(nAdd0);
						pBuf1->Add(nAdd1);
						pBuf2->Add(nAdd2);

						switch(pDst->GetType()) {
							case CNCSJPCBuffer::BT_INT16:
									for(x = 0; x < nWidth; x++) {
										pRGB[x * 3 + 0] = ((INT16*)pPtr0)[x] / dDiv0;
										pRGB[x * 3 + 1] = ((INT16*)pPtr1)[x] / dDiv1;
										pRGB[x * 3 + 2] = ((INT16*)pPtr2)[x] / dDiv2;
									}
								break;

							case CNCSJPCBuffer::BT_INT32:
									for(x = 0; x < nWidth; x++) {
										pRGB[x * 3 + 0] = ((INT32*)pPtr0)[x] / dDiv0;
										pRGB[x * 3 + 1] = ((INT32*)pPtr1)[x] / dDiv1;
										pRGB[x * 3 + 2] = ((INT32*)pPtr2)[x] / dDiv2;
									}
								break;

							case CNCSJPCBuffer::BT_IEEE4:
									for(x = 0; x < nWidth; x++) {
										pRGB[x * 3 + 0] = ((IEEE4*)pPtr0)[x] / dDiv0;
										pRGB[x * 3 + 1] = ((IEEE4*)pPtr1)[x] / dDiv1;
										pRGB[x * 3 + 2] = ((IEEE4*)pPtr2)[x] / dDiv2;
									}
								break;
						}	
						cmsDoTransform(m_Transform, pRGB, pRGB, nWidth);
						switch(pDst->GetType()) {
							case CNCSJPCBuffer::BT_INT16:
									for(x = 0; x < nWidth; x++) {
										((INT16*)pPtr0)[x] = (INT16)(pRGB[x * 3 + 0] * dDiv0 - nAdd0);
										((INT16*)pPtr1)[x] = (INT16)(pRGB[x * 3 + 1] * dDiv1 - nAdd1);
										((INT16*)pPtr2)[x] = (INT16)(pRGB[x * 3 + 2] * dDiv2 - nAdd2);
									}
								break;

							case CNCSJPCBuffer::BT_INT32:
									for(x = 0; x < nWidth; x++) {
										((INT32*)pPtr0)[x] = (INT32)(pRGB[x * 3 + 0] * dDiv0 - nAdd0);
										((INT32*)pPtr1)[x] = (INT32)(pRGB[x * 3 + 1] * dDiv1 - nAdd1);
										((INT32*)pPtr2)[x] = (INT32)(pRGB[x * 3 + 2] * dDiv2 - nAdd2);
									}
								break;

							case CNCSJPCBuffer::BT_IEEE4:
									for(x = 0; x < nWidth; x++) {
										((IEEE4*)pPtr0)[x] = (IEEE4)(pRGB[x * 3 + 0] * dDiv0 - nAdd0);
										((IEEE4*)pPtr1)[x] = (IEEE4)(pRGB[x * 3 + 1] * dDiv1 - nAdd1);
										((IEEE4*)pPtr2)[x] = (IEEE4)(pRGB[x * 3 + 2] * dDiv2 - nAdd2);
									}		
								break;
						}	
						NCSFree(pRGB);
					} else {
						switch(pDst->GetType()) {
							case CNCSJPCBuffer::BT_INT16:
									for(x = 0; x < nWidth; x++) {
										pRGB[x * 3 + 0] = ((UINT16*)pPtr0)[x]<<n0;
										pRGB[x * 3 + 1] = ((UINT16*)pPtr1)[x]<<n1;
										pRGB[x * 3 + 2] = ((UINT16*)pPtr2)[x]<<n2;
									}
								break;

							case CNCSJPCBuffer::BT_INT32:
									for(x = 0; x < nWidth; x++) {
										pRGB[x * 3 + 0] = (INT16)((INT32*)pPtr0)[x]<<n0;
										pRGB[x * 3 + 1] = (INT16)((INT32*)pPtr1)[x]<<n1;
										pRGB[x * 3 + 2] = (INT16)((INT32*)pPtr2)[x]<<n2;
									}
								break;
						}	
						cmsDoTransform(m_Transform, pRGB, pRGB, nWidth);
						switch(pDst->GetType()) {
							case CNCSJPCBuffer::BT_INT16:
									for(x = 0; x < nWidth; x++) {
										((INT16*)pPtr0)[x] = pRGB[x * 3 + 0]>>n0;
										((INT16*)pPtr1)[x] = pRGB[x * 3 + 1]>>n1;
										((INT16*)pPtr2)[x] = pRGB[x * 3 + 2]>>n2;
									}
								break;

							case CNCSJPCBuffer::BT_INT32:
									for(x = 0; x < nWidth; x++) {
										((INT32*)pPtr0)[x] = pRGB[x * 3 + 0]>>n0;
										((INT32*)pPtr1)[x] = pRGB[x * 3 + 1]>>n1;
										((INT32*)pPtr2)[x] = pRGB[x * 3 + 2]>>n2;
									}
								break;
						}	
						NCSFree(pRGB);
					}
				}
			}
			switch(iComponent) {
				case 0: pBuf0->Read(pDst); break;
				case 1: pBuf1->Read(pDst); break;
				case 2: pBuf2->Read(pDst); break;
			}
		} else {
			*(CNCSError*)this = NCS_COULDNT_ALLOC_MEMORY;
		}
	} else {
		bRet = CNCSJPCNode::ReadLine(nCtx, pDst, iComponent);
	}
	return(bRet);
}
#endif //NCSJPC_USE_LCMS
