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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCDCShiftNode.cpp $
** CREATED:  20/03/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCDCShiftNode class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCDCShiftNode.h"
#include "NCSJPCTilePartHeader.h"
#include "NCSJPC.h"

CNCSJPCNode::CNCSJPCNodeTracker CNCSJPCDCShiftNode::sm_Tracker("CNCSJPCDCShiftNode", sizeof(CNCSJPCDCShiftNode));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCDCShiftNode::CNCSJPCDCShiftNode()
{
	sm_Tracker.Add();
}

// Destructor
CNCSJPCDCShiftNode::~CNCSJPCDCShiftNode()
{
	sm_Tracker.Remove();
}

// Initialise this node.
bool CNCSJPCDCShiftNode::Init(ContextID nCtx, 
							  class CNCSJPC &JPC, 
							  UINT16 iComponent)
{
	Context *pCtx = (Context*)GetContext(nCtx);
	pCtx->m_nSsiz = JPC.m_SIZ.m_Components[iComponent].m_nBits;
	return(true);
}

template< class T > void DCShift_T(T *pSrc,
								   T *pDst,
								   UINT32 x,
								   UINT32 nWidth,
								   T nAdd,
								   T nMax)
{
	if(nAdd == 128 && nMax == 255) {
		for(; x < nWidth; x++) {
			T v = pSrc[x] + (T)128;
			if(v < 0) {
				pDst[x] = (T)0;
			} else if(v > 255) {
				pDst[x] = (T)255;
			} else {
				pDst[x] = v;
			}
		}
	} else {
		for(; x < nWidth; x++) {
			T v = pSrc[x] + (T)nAdd;
			if(v < 0) {
				pDst[x] = (T)0;
			} else if(v > nMax) {
				pDst[x] = (T)nMax;
			} else {
				pDst[x] = v;
			}
		}
	}
}

// Read a line from the input node.
bool CNCSJPCDCShiftNode::ReadLine(ContextID nCtx, 
								  CNCSJPCBuffer *pDst,
								  UINT16 iComponent)
{
	Context *pCtx = (Context*)GetContext(nCtx);
	bool bRet = false;
	bool bFound = false;
	UINT32 nBuffer = 0;

	pCtx->SetInputBuffers(0, 1);
	CNCSJPCBuffer *pBuffer = pCtx->FindInputBuffer(0, nBuffer, *pDst, bFound, pDst->GetType());
	bRet = CNCSJPCNode::ReadLine(nCtx, pBuffer, iComponent);
	if(bRet) {
		INT64 nAdd = ((INT64)0x1) << (pCtx->m_nSsiz - 1);
		INT64 nMax = (((INT64)0x1) << pCtx->m_nSsiz) - 1;
		UINT32 nWidth = pDst->GetWidth();
		UINT32 x = 0;

		switch(pDst->GetType()) {
			case BT_INT16:
#ifdef NCSJPC_X86_MMI
					if(CNCSJPCBuffer::sm_SystemInfo.m_bSSE2Present) {
						INT16 *pSrc = (INT16*)pBuffer->GetPtr();
						INT16 *pD = (INT16*)pDst->GetPtr();
						UINT32 n = nWidth / 8;

						__m128i mm0 = _mm_set1_epi16(nAdd);
						__m128i mm1 = _mm_set1_epi16(nMax);
						__m128i mm2 = _mm_set1_epi16(0);

						if((UINT64)pD % 16 == 0) {
							for(x = 0; x < n; x++) {
								__m128i mm3 = _mm_load_si128(&((__m128i*)pSrc)[x]);
								__m128i mm4 = _mm_adds_epi16(mm3, mm0);
								mm3 = _mm_min_epi16(mm4, mm1);
								mm4 = _mm_max_epi16(mm3, mm2);
								_mm_store_si128(&((__m128i*)pD)[x], mm4);
							}
						} else {
							for(x = 0; x < n; x++) {
								__m128i mm3 = _mm_load_si128(&((__m128i*)pSrc)[x]);
								__m128i mm4 = _mm_adds_epi16(mm3, mm0);
								mm3 = _mm_min_epi16(mm4, mm1);
								mm4 = _mm_max_epi16(mm3, mm2);
								_mm_storeu_si128(&((__m128i*)pD)[x], mm4);
							}
						}
						x *= 8;
#ifdef NCSJPC_X86_MMI_MMX
						_mm_empty();
					} else if(CNCSJPCBuffer::sm_SystemInfo.m_bSSEPresent) {
						// min&max require SSE
						INT16 *pSrc = (INT16*)pBuffer->GetPtr();
						INT16 *pD = (INT16*)pDst->GetPtr();
						UINT32 n = nWidth / 4;

						__m64 mm0 = _mm_set1_pi16(nAdd);
						__m64 mm1 = _mm_set1_pi16(nMax);
						__m64 mm2 = _mm_set1_pi16(0);

						for(; x < n; x++) {
							__m64 mm3 = ((__m64*)pSrc)[x];
							__m64 mm4 = _mm_adds_pi16(mm3, mm0);
							mm3 = _mm_min_pi16(mm4, mm1);
							mm4 = _mm_max_pi16(mm3, mm2);
							((__m64*)pD)[x] = mm4;
						}
						x *= 4;
						_mm_empty();
#endif NCSJPC_X86_MMI_MMX
					}
#endif //NCSJPC_X86_MMI
					DCShift_T((INT16*)pBuffer->GetPtr(), (INT16*)pDst->GetPtr(), x, nWidth, (INT16)nAdd, (INT16)nMax);
				break;
			case BT_INT32:
					DCShift_T((INT32*)pBuffer->GetPtr(), (INT32*)pDst->GetPtr(), x, nWidth, (INT32)nAdd, (INT32)nMax);
				break;
			case BT_INT64:
					DCShift_T((INT64*)pBuffer->GetPtr(), (INT64*)pDst->GetPtr(), x, nWidth, (INT64)nAdd, (INT64)nMax);
				break;
			case BT_IEEE4:
//				{//FIXME
//					IEEE4 *pSrc = (IEEE4*)pBuffer->GetPtr();
//					for(INT32 x = 0; x < nWidth; x++) {
//						pSrc[x] = (INT32)(pSrc[x] + 0.5);
//					}
//				}
#ifdef NCSJPC_X86_MMI
					if(CNCSJPCBuffer::sm_SystemInfo.m_bSSEPresent) {
						IEEE4 *pSrc = (IEEE4*)pBuffer->GetPtr();
						IEEE4 *pD = (IEEE4*)pDst->GetPtr();
						UINT32 n = 4 * (nWidth / 4);

						__m128 mm0 = _mm_set1_ps((IEEE4)nAdd);
						__m128 mm1 = _mm_set1_ps((IEEE4)nMax);
						__m128 mm2 = _mm_set1_ps((IEEE4)0);

						if((UINT64)pD % 16 == 0) {
							for(x = 0; x < n; x += 4) {
								__m128 mm3 = _mm_load_ps(&(pSrc[x]));
								__m128 mm4 = _mm_add_ps(mm3, mm0);
								mm3 = _mm_min_ps(mm4, mm1);
								mm4 = _mm_max_ps(mm3, mm2);
								_mm_store_ps(&(pD[x]), mm4);
							}
						} else {
							for(x = 0; x < n; x += 4) {
								__m128 mm3 = _mm_load_ps(&(pSrc[x]));
								__m128 mm4 = _mm_add_ps(mm3, mm0);
								mm3 = _mm_min_ps(mm4, mm1);
								mm4 = _mm_max_ps(mm3, mm2);
								_mm_storeu_ps(&(pD[x]), mm4);
							}
						}
					//	_mm_empty();
					}
#endif
					DCShift_T((IEEE4*)pBuffer->GetPtr(), (IEEE4*)pDst->GetPtr(), x, nWidth, (IEEE4)nAdd, (IEEE4)nMax);
				break;
		}
	}
	return(bRet);
}

CNCSJPCNode::Context *CNCSJPCDCShiftNode::GetContext(ContextID nCtx, bool bAutoConstruct)
{
	CNCSJPCNode::Context *pCtx = CNCSJPCNode::GetContext(nCtx, false);
	if(!pCtx && bAutoConstruct) {
		pCtx = new Context;
		SetContext(nCtx, pCtx);
	}
	return(pCtx);
}
