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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCYCbCr2RGBNode.cpp $
** CREATED:  20/03/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCYCbCr2RGBNode class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCYCbCr2RGBNode.h"
//#include "NCSJPCTilePartHeader.h"

//Y = (0.257 * R) + (0.504 * G) + (0.098 * B) + 16
//Cr = V = (0.439 * R) - (0.368 * G) - (0.071 * B) + 128
//Cb = U = -(0.148 * R) - (0.291 * G) + (0.439 * B) + 128

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCYCbCr2RGBNode::CNCSJPCYCbCr2RGBNode()
{
}

// Destructor
CNCSJPCYCbCr2RGBNode::~CNCSJPCYCbCr2RGBNode()
{
}

// Read a line from the input node.
bool CNCSJPCYCbCr2RGBNode::ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent)
{
	bool bRet = false;
	Context *pCtx = GetContext(nCtx);

	if(pCtx->NrInputs() >= 3) {
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
			}
			if(bRet) {
				UINT32 x;
				UINT32 nWidth = pDst->GetWidth();
				void *pDst0 = pDst->GetPtr();
				void *pPtr0 = pBuf0->GetPtr();
				void *pPtr1 = pBuf1->GetPtr();
				void *pPtr2 = pBuf2->GetPtr();

				switch(pDst->GetType()) {
					case CNCSJPCBuffer::BT_INT16:
							FLT_TO_INT_INIT();
							switch(iComponent) {
								case 0:
										
										for(x = 0; x < nWidth; x++) {
											//R' = 1.164*(Y' - 16) + 1.596*(Cr' - 128)
											((INT16*)pDst0)[x] = (INT16)NCSfloatToInt32_RM(1.164f*(((INT16*)pPtr0)[x] - 16) + 1.596f*(((INT16*)pPtr2)[x] - 128));
										}
									break;
								case 1:
										for(x = 0; x < nWidth; x++) {													
											//G' = 1.164*(Y' - 16) - 0.813*(Cr' - 128) - 0.392*(Cb' - 128)
											((INT16*)pDst0)[x] = (INT16)NCSfloatToInt32_RM(1.164f*(((INT16*)pPtr0)[x] - 16) - 0.813f*(((INT16*)pPtr2)[x] - 128) - 0.391f*(((INT16*)pPtr1)[x] - 128));
										}
									break;
								case 2:
										for(x = 0; x < nWidth; x++) {
											//B' = 1.164*(Y' - 16) + 2.017*(Cb' - 128)
											((INT16*)pDst0)[x] = (INT16)NCSfloatToInt32_RM(1.164f*(((INT16*)pPtr0)[x] - 16) + 2.018f*(((INT16*)pPtr1)[x] - 128));
										}
									break;
							}
							FLT_TO_INT_FINI();
						break;

					case CNCSJPCBuffer::BT_INT32:
							switch(iComponent) {
								case 0:
										for(x = 0; x < nWidth; x++) {
											//R' = 1.164*(Y' - 16) + 1.596*(Cr' - 128)
											((INT32*)pDst0)[x] = NCSfloatToInt32_RM(1.164f*(((INT32*)pPtr0)[x] - 16) + 1.596f*(((INT32*)pPtr2)[x] - 128));
										}
									break;
								case 1:
										for(x = 0; x < nWidth; x++) {													
											//G' = 1.164*(Y' - 16) - 0.813*(Cr' - 128) - 0.392*(Cb' - 128)
											((INT32*)pDst0)[x] = NCSfloatToInt32_RM(1.164f*(((INT32*)pPtr0)[x] - 16) - 0.813f*(((INT32*)pPtr2)[x] - 128) - 0.392f*(((INT32*)pPtr1)[x] - 128));
										}
									break;
								case 2:
										for(x = 0; x < nWidth; x++) {
											//B' = 1.164*(Y' - 16) + 2.017*(Cb' - 128)
											((INT32*)pDst0)[x] = NCSfloatToInt32_RM(1.164f*(((INT32*)pPtr0)[x] - 16) + 2.017f*(((INT32*)pPtr1)[x] - 128));
										}
									break;
							}
						break;

					case CNCSJPCBuffer::BT_IEEE4:
							switch(iComponent) {
								case 0:
										for(x = 0; x < nWidth; x++) {
											//R' = 1.164*(Y' - 16) + 1.596*(Cr' - 128)
											((IEEE4*)pDst0)[x] = (IEEE4)(1.164*(((IEEE4*)pPtr0)[x] - 16) + 1.596*(((IEEE4*)pPtr2)[x] - 128));
										}
									break;
								case 1:
										for(x = 0; x < nWidth; x++) {													
											//G' = 1.164*(Y' - 16) - 0.813*(Cr' - 128) - 0.392*(Cb' - 128)
											((IEEE4*)pDst0)[x] = (IEEE4)(1.164*(((IEEE4*)pPtr0)[x] - 16) - 0.813*(((IEEE4*)pPtr2)[x] - 128) - 0.392*(((IEEE4*)pPtr1)[x] - 128));
										}
									break;
								case 2:
										for(x = 0; x < nWidth; x++) {
											//B' = 1.164*(Y' - 16) + 2.017*(Cb' - 128)
											((IEEE4*)pDst0)[x] = (IEEE4)(1.164*(((IEEE4*)pPtr0)[x] - 16) + 2.017*(((IEEE4*)pPtr1)[x] - 128));
										}
									break;
							}
						break;
				}	
			}
		} else {
			*(CNCSError*)this = NCS_COULDNT_ALLOC_MEMORY;
		}
	}
	return(bRet);
}
