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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCMCTNode.cpp $
** CREATED:  20/03/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCMCTNode class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCMCTNode.h"
#include "NCSJPCTilePartHeader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSJPCNode::CNCSJPCNodeTracker CNCSJPCMCTNode::sm_Tracker("CNCSJPCMCTNode", sizeof(CNCSJPCMCTNode));

// Constructor
CNCSJPCMCTNode::CNCSJPCMCTNode()
{
	sm_Tracker.Add();
}

// Destructor
CNCSJPCMCTNode::~CNCSJPCMCTNode()
{
	sm_Tracker.Remove();
}

// Read a line from the input node.
bool CNCSJPCMCTNode::ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent)
{
	bool bRet = false;
	Context *pCtx = (Context*)GetContext(nCtx);

	if(pDst && pCtx->NrInputs() >= 3) {
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

		CNCSJPCBuffer *pBuf0 = pDst;
		CNCSJPCBuffer *pBuf1 = pCtx->GetInputBuffer(1, (UINT8)nBuffer);
		CNCSJPCBuffer *pBuf2 = pCtx->GetInputBuffer(2, (UINT8)nBuffer);
		bRet = true;

		if(bRet) {
			UINT32 nWidth = pDst->GetWidth();
			void *pPtr0 = pBuf0->GetPtr();
			void *pPtr1 = pBuf1->GetPtr();
			void *pPtr2 = pBuf2->GetPtr();
			void *pDst0 = pDst->GetPtr();

			switch(pDst->GetType()) {
				case CNCSJPCBuffer::BT_INT16:
						if(bDoRead) {
							bRet = ReadInputLines(nCtx, pCtx, pBuf0, pBuf1, pBuf2);
							if(bRet) {
								UINT32 x = 0;
								INT16 *pI1Buffer = (INT16*)pCtx->GetInputBuffer(0, (UINT8)nBuffer)->GetPtr();//pCtx->m_pI1Buffer;
#ifdef NCSJPC_X86_MMI
								if(CNCSJPCBuffer::sm_SystemInfo.m_bSSE2Present) {
									UINT32 n = nWidth / 8;

									for(x = 0; x < n; x++) {
										__m128i mm0 = _mm_load_si128(&((__m128i*)pPtr0)[x]);//Y0
										__m128i mm1 = _mm_load_si128(&((__m128i*)pPtr1)[x]);//Y1
										__m128i mm2 = _mm_load_si128(&((__m128i*)pPtr2)[x]);//Y2

										__m128i mm4 = _mm_adds_epi16(mm2, mm1);//mm4=Y2+Y1
										__m128i mm3 = _mm_srai_epi16(mm4, 2);//mm3=(Y2+Y1)>>2
										mm4 = _mm_subs_epi16(mm0, mm3);//mm4=Y0-((Y2 + Y1)>>2)
										_mm_store_si128(&((__m128i*)pI1Buffer)[x], mm4);
									}
									x *= 8;
#ifdef NCSJPC_X86_MMI_MMX
									_mm_empty();
								} else if(CNCSJPCBuffer::sm_SystemInfo.m_bMMXPresent) {
									UINT32 n = nWidth / 4;

									for(x = 0; x < n; x++) {
										__m64 mm0 = ((__m64*)pPtr0)[x];//Y0
										__m64 mm1 = ((__m64*)pPtr1)[x];//Y1
										__m64 mm2 = ((__m64*)pPtr2)[x];//Y2

										__m64 mm4 = _mm_adds_pi16(mm2, mm1);//mm4=Y2+Y1
										__m64 mm3 = _mm_srai_pi16(mm4, 2);//mm3=(Y2+Y1)>>2
										mm4 = _mm_subs_pi16(mm0, mm3);//mm4=Y0-((Y2 + Y1)>>2)
										((__m64*)pI1Buffer)[x] = mm4;
									}
									x *= 4;
									_mm_empty();
#endif NCSJPC_X86_MMI_MMX
								}
#endif
								for(; x < nWidth; x++) {
									INT16 Y0 = ((INT16*)pPtr0)[x];
									INT16 Y1 = ((INT16*)pPtr1)[x];
									INT16 Y2 = ((INT16*)pPtr2)[x];
											
									pI1Buffer[x] = Y0 - ((Y2 + Y1) >> 2);
								}
							}
							bDoRead= false;
						} else {
							bRet = true;
						}					
						if(bRet) {
							UINT32 x = 0;
							INT16 *pI1Buffer = (INT16*)pCtx->GetInputBuffer(0, (UINT8)nBuffer)->GetPtr();//pCtx->m_pI1Buffer;

							switch(iComponent) {
								case 1:
										pCtx->GetInputBuffer(0, (UINT8)nBuffer)->Read(pDst);
									break;

								case 2:
										pPtr2 = pPtr1;
										//fallthrough
								case 0:
#ifdef NCSJPC_X86_MMI
										if(CNCSJPCBuffer::sm_SystemInfo.m_bSSE2Present) {
											UINT32 n = nWidth / 8;

											if((UINT64)pDst0 % 16 == 0) {
												for(x = 0; x < n; x++) {
													__m128i mm0 = _mm_load_si128(&((__m128i*)pI1Buffer)[x]);
													__m128i mm2 = _mm_load_si128(&((__m128i*)pPtr2)[x]);//Y2

													__m128i mm4 = _mm_adds_epi16(mm2, mm0);
													_mm_store_si128(&((__m128i*)pDst0)[x], mm4);
												}
											} else {
												for(x = 0; x < n; x++) {
													__m128i mm0 = _mm_load_si128(&((__m128i*)pI1Buffer)[x]);
													__m128i mm2 = _mm_load_si128(&((__m128i*)pPtr2)[x]);//Y2

													__m128i mm4 = _mm_adds_epi16(mm2, mm0);
													_mm_storeu_si128(&((__m128i*)pDst0)[x], mm4);
												}
											}
											x *= 8;
#ifdef NCSJPC_X86_MMI_MMX
											_mm_empty();
										} else if(CNCSJPCBuffer::sm_SystemInfo.m_bMMXPresent) {
											UINT32 n = nWidth / 4;

											for(x = 0; x < n; x++) {
												__m64 mm0 = ((__m64*)pI1Buffer)[x];
												__m64 mm2 = ((__m64*)pPtr2)[x];//Y2

												__m64 mm4 = _mm_adds_pi16(mm2, mm0);
												((__m64*)pDst0)[x] = mm4;
											}
											x *= 4;
											_mm_empty();
#endif NCSJPC_X86_MMI_MMX
										}
#endif
										for(; x < nWidth; x++) {
											INT16 Y2 = ((INT16*)pPtr2)[x];
											
											((INT16*)pDst0)[x] = Y2 + pI1Buffer[x];
										}
									break;
							}
						}
					break;
				case CNCSJPCBuffer::BT_INT32:
							// RCT
						if(bDoRead) {
							bRet = ReadInputLines(nCtx, pCtx, pBuf0, pBuf1, pBuf2);
								
							if(bRet) {
								UINT32 x = 0;
								INT32 *pI1Buffer = (INT32*)pCtx->GetInputBuffer(0, (UINT8)nBuffer)->GetPtr();//pCtx->m_pI1Buffer;
#ifdef NCSJPC_X86_MMI
								if(CNCSJPCBuffer::sm_SystemInfo.m_bSSE2Present) {
									UINT32 n = nWidth / 4;

									for(x = 0; x < n; x++) {
										__m128i mm0 = _mm_load_si128(&((__m128i*)pPtr0)[x]);//Y0
										__m128i mm1 = _mm_load_si128(&((__m128i*)pPtr1)[x]);//Y1
										__m128i mm2 = _mm_load_si128(&((__m128i*)pPtr2)[x]);//Y2

										__m128i mm4 = _mm_add_epi32(mm2, mm1);//mm4=Y2+Y1
										__m128i mm3 = _mm_srai_epi32(mm4, 2);//mm3=(Y2+Y1)>>2
										mm4 = _mm_sub_epi32(mm0, mm3);//mm4=Y0-((Y2 + Y1)>>2)
										_mm_store_si128(&((__m128i*)pI1Buffer)[x], mm4);
									}
									x *= 4;
			//						_mm_empty();
								}
#endif //NCSJPC_X86_MMI
								for(; x < nWidth; x++) {
									INT32 Y0 = ((INT32*)pPtr0)[x];
									INT32 Y1 = ((INT32*)pPtr1)[x];
									INT32 Y2 = ((INT32*)pPtr2)[x];
									
									pI1Buffer[x] = Y0 - ((Y2 + Y1) >> 2);
								}
							}
						} else {
							bRet = true;
						}
						if(bRet) {
							UINT32 x = 0;
							INT32 *pI1Buffer = (INT32*)pCtx->GetInputBuffer(0, (UINT8)nBuffer)->GetPtr();//pCtx->m_pI1Buffer;

							switch(iComponent) {
								case 1:
										bRet = pCtx->GetInputBuffer(0, (UINT8)nBuffer)->Read(pDst);
									break;
								case 2:
										pPtr2 = pPtr1;
										//fallthrough
								case 0:
#ifdef NCSJPC_X86_MMI
										if(CNCSJPCBuffer::sm_SystemInfo.m_bSSE2Present) {
											UINT32 n = nWidth / 4;

											if((UINT64)pDst0 % 16 == 0) {
												for(x = 0; x < n; x++) {
													__m128i mm0 = _mm_load_si128(&((__m128i*)pI1Buffer)[x]);
													__m128i mm2 = _mm_load_si128(&((__m128i*)pPtr2)[x]);//Y2

													__m128i mm4 = _mm_add_epi32(mm2, mm0);
													_mm_store_si128(&((__m128i*)pDst0)[x], mm4);
												}
											} else {
												for(x = 0; x < n; x++) {
													__m128i mm0 = _mm_load_si128(&((__m128i*)pI1Buffer)[x]);
													__m128i mm2 = _mm_load_si128(&((__m128i*)pPtr2)[x]);//Y2

													__m128i mm4 = _mm_add_epi32(mm2, mm0);
													_mm_storeu_si128(&((__m128i*)pDst0)[x], mm4);
												}
											}
											x *= 4;
				//							_mm_empty();
										}
#endif
										for(; x < nWidth; x++) {
											INT32 Y2 = ((INT32*)pPtr2)[x];
												
											((INT32*)pDst0)[x] = Y2 + pI1Buffer[x];
										}
									break;
							}
						}
					break;
				case CNCSJPCBuffer::BT_IEEE4:
						// ICT - RGB<>YUV
						pBuf0 = pCtx->GetInputBuffer(0, (UINT8)nBuffer);
						pPtr0 = pBuf0->GetPtr();
						if(bDoRead) {
							bRet = ReadInputLines(nCtx, pCtx, pBuf0, pBuf1, pBuf2);
							bDoRead = false;
						}
						if(bRet) {
							switch(iComponent) {
								case 0:
										{
											for(UINT32 x = 0; x < nWidth; x++) {
												IEEE4 Y = ((IEEE4*)pPtr0)[x];
												IEEE4 V = ((IEEE4*)pPtr2)[x];
												((IEEE4*)pDst0)[x] = Y + V * 1.402f;
											}
										}
									break;
								case 1:
										{
											for(UINT32 x = 0; x < nWidth; x++) {
												IEEE4 Y = ((IEEE4*)pPtr0)[x];
												IEEE4 U = ((IEEE4*)pPtr1)[x];
												IEEE4 V = ((IEEE4*)pPtr2)[x];
												((IEEE4*)pDst0)[x] = Y - 0.344136f * U - 0.714136f * V;
											}
										}
									break;
								case 2:
										{
											for(UINT32 x = 0; x < nWidth; x++) {
												IEEE4 Y = ((IEEE4*)pPtr0)[x];
												IEEE4 U = ((IEEE4*)pPtr1)[x];
												((IEEE4*)pDst0)[x] = Y + 1.772f * U;
											}
										}
									break;
							}
						}
					break;
			}						
		} else {
			*(CNCSError*)this = NCS_COULDNT_ALLOC_MEMORY;
		}
	}
	return(bRet);
}

bool CNCSJPCMCTNode::ReadInputLines(ContextID nCtx,
									Context *pCtx, 
									CNCSJPCBuffer *pBuf0, CNCSJPCBuffer *pBuf1, CNCSJPCBuffer *pBuf2)
{
	pCtx;//Keep compiler happy
	return(CNCSJPCNode::ReadInputLines(nCtx, 3, 0, pBuf0, 1, pBuf1, 2, pBuf2));
//	bool bRet = pCtx->GetInput(0)->ReadLine(nCtx, pBuf0, 0);
//	if(bRet) {
//		bRet = pCtx->GetInput(1)->ReadLine(nCtx, pBuf1, 1);
//		if(bRet) {
//			bRet = pCtx->GetInput(2)->ReadLine(nCtx, pBuf2, 2);
//			if(!bRet) {
//				*(CNCSError*)this = *pCtx->GetInput(2);
//			}
//		} else {
//			*(CNCSError*)this = *pCtx->GetInput(1);
//		}
//	} else {
//		*(CNCSError*)this = *pCtx->GetInput(0);
//	}
//	return(bRet);
}

// Write a line.
bool CNCSJPCMCTNode::WriteLine(ContextID nCtx, CNCSJPCBuffer *pSrc, UINT16 iComponent)
{
	bool bRet = false;
	Context *pCtx = (Context*)GetContext(nCtx);

	if(iComponent >= 3) {
		bRet = pCtx->GetInput(iComponent)->WriteLine(nCtx, pSrc, iComponent);
	} else {
		bool bDoWrite = true;
		int nInput;
		UINT32 nBuffer = 0;
		CNCSJPCRect Rect(pSrc->GetX0(), pSrc->GetY0(), pSrc->GetWidth(), (UINT32)1);

		bRet = true;

		for(nInput = 0; nInput < 3; nInput++) {			
			pCtx->SetInputBuffers(nInput, 1);
			CNCSJPCBuffer *pBuf;
			if(nInput == iComponent) {
				bool bFound = false;
				pBuf = pCtx->FindInputBuffer(nInput, nBuffer, Rect, bFound, pSrc->GetType());
				pSrc->Read(pBuf);				
			} else {
				pBuf = pCtx->GetInputBuffer(nInput, 0);
			}
			bDoWrite &= (*pBuf == Rect);
		}
		if(bDoWrite) {
			CNCSJPCBuffer *pBuf0 = pCtx->GetInputBuffer(0, 0);
			CNCSJPCBuffer *pBuf1 = pCtx->GetInputBuffer(1, 0);
			CNCSJPCBuffer *pBuf2 = pCtx->GetInputBuffer(2, 0);
			UINT32 nWidth = pSrc->GetWidth();
			void *pPtr0 = pBuf0->GetPtr();
			void *pPtr1 = pBuf1->GetPtr();
			void *pPtr2 = pBuf2->GetPtr();
			UINT32 x;

			switch(pSrc->GetType()) {
				case BT_INT16:
						for(x = 0; x < nWidth; x++) {
							INT16 i0 = ((INT16*)pPtr0)[x];
							INT16 i1 = ((INT16*)pPtr1)[x];
							INT16 i2 = ((INT16*)pPtr2)[x];
							
							((INT16*)pPtr0)[x] = (INT16)NCSFloorDiv(i0 + 2 * i1 + i2, 4);
							((INT16*)pPtr1)[x] = i2 - i1;
							((INT16*)pPtr2)[x] = i0 - i1;
						}
					break;
				case BT_INT32:
						for(x = 0; x < nWidth; x++) {
							INT32 i0 = ((INT32*)pPtr0)[x];
							INT32 i1 = ((INT32*)pPtr1)[x];
							INT32 i2 = ((INT32*)pPtr2)[x];
							
							((INT32*)pPtr0)[x] = NCSFloorDiv(i0 + 2 * i1 + i2, 4);
							((INT32*)pPtr1)[x] = i2 - i1;
							((INT32*)pPtr2)[x] = i0 - i1;
						}
					break;
			}
			bRet &= pCtx->GetInput(0)->WriteLine(nCtx, pBuf0, 0);
			bRet &= pCtx->GetInput(1)->WriteLine(nCtx, pBuf1, 1);
			bRet &= pCtx->GetInput(2)->WriteLine(nCtx, pBuf2, 2);
		}
	}
	return(bRet);
}
