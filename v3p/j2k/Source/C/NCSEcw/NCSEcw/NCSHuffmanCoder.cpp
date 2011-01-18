/********************************************************
** Copyright 2002 Earth Resource Mapping Ltd.
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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSEcw/NCSHuffmanCoder.cpp $
** CREATED:  11/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  Huffman Coder class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSHuffmanCoder.h"

const UINT32 CNCSHuffmanCoder::NUM_SYMBOL_VALUES = 65536;
const UINT16 CNCSHuffmanCoder::SIGN_MASK = NCS_HUFFMAN_SIGN_MASK;
const UINT16 CNCSHuffmanCoder::RUN_MASK = NCS_HUFFMAN_RUN_MASK;
const INT16 CNCSHuffmanCoder::MAX_BIN_VALUE = 16383;
const INT16 CNCSHuffmanCoder::MIN_BIN_VALUE = -16383;
const UINT16 CNCSHuffmanCoder::VALUE_MASK = NCS_HUFFMAN_VALUE_MASK;
const UINT16 CNCSHuffmanCoder::MAX_RUN_LENGTH = NCS_HUFFMAN_MAX_RUN_LENGTH;
const UINT8 CNCSHuffmanCoder::SMALL_SYMBOL = 0x40;
const UINT8 CNCSHuffmanCoder::SMALL_SHIFT = 10;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSHuffmanCoder::CCodeNode::CCodeNode() 
{
	m_Children.m_P.m_p0Child = NULL;
	m_Children.m_P.m_p1Child = NULL;

	memset(&m_Symbol, 0, sizeof(m_Symbol));

	m_nFrequency = 0;
	m_pNext = (CCodeNode*)NULL;  
	m_nCode = 0;
	m_nCodeBits = 0;
	m_bInHistogram = false;
}

CNCSHuffmanCoder::CCodeNode::CCodeNode(UINT8 **ppPacked, 
									   UINT32 &nNodes) 
{
	Unpack(ppPacked, nNodes);
}

CNCSHuffmanCoder::CCodeNode::~CCodeNode() 
{
	delete m_Children.m_P.m_p0Child;
	delete m_Children.m_P.m_p1Child;
}

void CNCSHuffmanCoder::CCodeNode::Pack(UINT8 **ppPacked, 
									   UINT32 &nNodes)
{
	if(m_Children.m_P.m_p0Child == NULL) {
		nNodes++;

		UINT16 nSymbol = m_Symbol.nValue;

		if((nSymbol & VALUE_MASK) <= 0x0f ) {
			nSymbol = SMALL_SYMBOL | (nSymbol >> 10) | (nSymbol & 0x0f);
			**ppPacked = (UINT8)nSymbol;
			(*ppPacked)++;
		} else {
			**ppPacked = 0x80;
			(*ppPacked)++;

			**ppPacked = (UINT8)(nSymbol & 0xff);
			(*ppPacked)++;
			**ppPacked = (UINT8)((nSymbol >> 8) & 0xff);
			(*ppPacked)++;
		}
	} else {
		nNodes++;
		**ppPacked = 0;
		(*ppPacked)++;
		m_Children.m_P.m_p0Child->Pack(ppPacked, nNodes);
		m_Children.m_P.m_p1Child->Pack(ppPacked, nNodes);
	}
}

CNCSHuffmanCoder::CCodeNode *CNCSHuffmanCoder::CCodeNode::Unpack(UINT8 **ppPacked, 
																 UINT32 &nNodes) 
{
	nNodes--;
	if(nNodes == 0) {
		return(NULL);
	}
	UINT8 nByte = (*(*ppPacked)++);

	if( nByte == 0 ) {
		m_Children.m_P.m_p0Child = new CCodeNode(ppPacked, nNodes);
		m_Children.m_P.m_p1Child = new CCodeNode(ppPacked, nNodes);
		memset(&m_Symbol, 0, sizeof(m_Symbol));
	}
	else {
		m_Children.m_P.m_p0Child = NULL;
		m_Children.m_P.m_p1Child = NULL;

		UINT16 nValue;
		if( nByte & SMALL_SYMBOL ) {
			nValue = (((UINT16)(nByte & 0x30)) << SMALL_SHIFT) | (nByte & 0x0f);
		}
		else {
			nValue = ((UINT16)**ppPacked) | (((UINT16)(*((*ppPacked) + 1))) << 8);
			*ppPacked += 2;
		}
		if(nValue & NCS_HUFFMAN_RUN_MASK) {
			m_Symbol.bZeroRun = TRUE;
			m_Symbol.nValue = (nValue & NCS_HUFFMAN_MAX_RUN_LENGTH) - 1;
		} else {
			m_Symbol.bZeroRun = FALSE;

			if(nValue & SIGN_MASK) {
#ifdef NCSBO_MSBFIRST
				m_Symbol.nValue = (INT16)NCSByteSwap16(((INT16)(nValue & VALUE_MASK)) * -1);
#else
				m_Symbol.nValue = ((INT16)(nValue & VALUE_MASK)) * -1;
#endif
			} else {
#ifdef NCSBO_MSBFIRST
				m_Symbol.nValue = (INT16)NCSByteSwap16((INT16)nValue);
#else
				m_Symbol.nValue = ((INT16)nValue);
#endif
			}
		}
	}
	return (this);
}

void CNCSHuffmanCoder::CCodeNode::SetCode(UINT32 nCode, UINT8 nCodeBits)
{
	if(m_Children.m_P.m_p0Child) {
		m_Children.m_P.m_p0Child->SetCode((nCode << 1), nCodeBits + 1);
		m_Children.m_P.m_p1Child->SetCode((nCode << 1) | 0x1, nCodeBits + 1);
	} else {
		m_nCode = nCode;
		m_nCodeBits = nCodeBits;
	}
}

CNCSHuffmanCoder::CTree::CTree() 
{
	m_pRoot = NULL;
}

CNCSHuffmanCoder::CTree::CTree(UINT8 **ppPacked) 
{
	m_pRoot = NULL;
	Unpack(ppPacked);
}

CNCSHuffmanCoder::CTree::~CTree()
{
	delete m_pRoot;
}

CNCSError CNCSHuffmanCoder::CTree::Pack(UINT8 **ppPacked,
										INT16 *pUnPacked,
										UINT32 nSymbols)
{
	UINT32 i;
	std::map<INT16, CCodeNode*>::iterator pEnd = m_Histogram.end();

	for(i=0; i < nSymbols; i++) {
		UINT16 nSymbol = pUnPacked[i];
		std::map<INT16, CCodeNode*>::iterator Leaf = m_Histogram.find(nSymbol);
		CCodeNode *pLeaf = (Leaf != pEnd) ? (*Leaf).second : NULL;
			
		if (pLeaf == NULL) {
			pLeaf = new CCodeNode();
			pLeaf->m_bInHistogram = TRUE;
			pLeaf->m_Symbol.nValue = nSymbol;
			pLeaf->m_nFrequency = 1;
			const std::pair<const INT16, CCodeNode*> pr(nSymbol, pLeaf);
			m_Histogram.insert(pr);
		} else if(pLeaf->m_nFrequency == 0) {
			pLeaf->m_Symbol.nValue = nSymbol;
			pLeaf->m_nFrequency = 1;
		} else {
			pLeaf->m_nFrequency++;
		}
	}
	std::map<INT16, CCodeNode*>::iterator pCur = m_Histogram.begin();

	while(pCur != pEnd) {
		CCodeNode *pLeaf = (*pCur).second;

		if(pLeaf && (pLeaf->m_nFrequency != 0)) {
			if((m_pRoot == NULL) || (pLeaf->m_nFrequency <= m_pRoot->m_nFrequency)) {
				pLeaf->m_pNext = m_pRoot;
				m_pRoot = pLeaf;
			} else {
				CCodeNode *pList = m_pRoot;
				while((pList->m_pNext != NULL) && (pLeaf->m_nFrequency > pList->m_pNext->m_nFrequency) )
					pList = pList->m_pNext;
				pLeaf->m_pNext = pList->m_pNext;
				pList->m_pNext = pLeaf;
			}
		}
		pCur++;
	}
	while (m_pRoot->m_pNext != NULL) {
		CCodeNode *pLeaf = new CCodeNode();
		pLeaf->m_Children.m_P.m_p0Child = m_pRoot;
		pLeaf->m_Children.m_P.m_p1Child = m_pRoot->m_pNext;
		pLeaf->m_nFrequency = pLeaf->m_Children.m_P.m_p0Child->m_nFrequency + pLeaf->m_Children.m_P.m_p1Child->m_nFrequency;
		m_pRoot = m_pRoot->m_pNext->m_pNext;       
		
		if((m_pRoot == NULL) || (pLeaf->m_nFrequency <= m_pRoot->m_nFrequency)) {
			pLeaf->m_pNext = m_pRoot;
			m_pRoot = pLeaf;
		} else {
			CCodeNode *pList = m_pRoot;

			while((pList->m_pNext != NULL) && (pLeaf->m_nFrequency > pList->m_pNext->m_nFrequency))
				pList = pList->m_pNext;
			pLeaf->m_pNext = pList->m_pNext;
			pList->m_pNext = pLeaf;
		}
	}
	m_pRoot->SetCode(0, 0);

	UINT8 *pBase = *ppPacked;
	UINT32 nNodes = 0;
	*ppPacked += 2;
	m_pRoot->Pack(ppPacked, nNodes);

	pBase[0] = (UINT8)(nNodes & 0xff);
	pBase[1] = (UINT8)((nNodes >> 8) & 0xff);

	return(NCS_SUCCESS);
}

CNCSError CNCSHuffmanCoder::CTree::Unpack(UINT8 **ppPacked)
{
	UINT32 nNodes = (((UINT32)**ppPacked) | (((UINT32)(*((*ppPacked) + 1))) << 8)) + 1;
	*ppPacked += 2;
	m_pRoot = new CCodeNode(ppPacked, nNodes);
	if(m_pRoot) {
		return(NCS_SUCCESS);
	} else {
		return(NCS_COULDNT_ALLOC_MEMORY);
	}
}

// Constructor
CNCSHuffmanCoder::CNCSHuffmanCoder()
{
	m_pTree = NULL;
}

// Destructor
CNCSHuffmanCoder::~CNCSHuffmanCoder()
{
	delete m_pTree;
}

CNCSError CNCSHuffmanCoder::Pack(UINT8 *pPacked, 
								 UINT32 *pPackedLength,
								 INT16 *pUnPacked,
								 UINT32 nRawLength)
{
	UINT8 *pOutput = pPacked;
	register UINT32	nWordCount = nRawLength / 2;

	m_pTree = new CTree();
	m_pTree->Pack(&pOutput, pUnPacked, nWordCount);
	UINT32 i;
	UINT8 nBitsUsed = 0;
	UINT8 nByte = 0;

	for(i = 0; i < nWordCount; i++) {
		CCodeNode *pNode = m_pTree->m_Histogram[pUnPacked[i]];
		UINT8 b;
		UINT8 nBits = pNode->m_nCodeBits;
		UINT32 nCode = pNode->m_nCode;

		for(b = 0; b < nBits; b++) {
			nByte |= ((nCode >> (nBits - 1 - b)) & 0x1) << nBitsUsed;
			nBitsUsed++;
			if(nBitsUsed == 8) {
				nBitsUsed = 0; 
				*pOutput++ = nByte;
				nByte = 0;
			}
		}
	}
	if(nBitsUsed != 0) {
		*pOutput++ = nByte;
	}
	*pPackedLength = (UINT32)(pOutput - pPacked + 1);
	return(NCS_SUCCESS);
}

// UnParse the marker out to the stream.
CNCSError CNCSHuffmanCoder::UnPack(UINT8 *pPacked,
								   INT16 *pUnPacked,
								   UINT32 nRawLength)
{
	register UINT32	nWordCount = nRawLength / 2;
	register INT16 *pOutput = (INT16*)pUnPacked;
	register UINT32 nBitsUsed = 0;

	m_pTree = new CTree(&pPacked);

	if(!m_pTree) {
		return(NCS_COULDNT_ALLOC_MEMORY);
	}
	
	while(nWordCount--) {
		// Decode a packed Huffman value
		CCodeNode *pNode = m_pTree->m_pRoot;
		while (pNode->m_Children.m_P.m_p0Child != NULL) {
			pNode = pNode->m_Children.m_Children[(pPacked[nBitsUsed >> 3] >> (nBitsUsed & 0x7)) & 0x1];
			nBitsUsed++;
		}
				 
		if(pNode->m_Symbol.bZeroRun) {
			register UINT16 nZero;
			register UINT16	nZeroRun = pNode->m_Symbol.nValue;
			
			if( nZeroRun >= nWordCount ) {
				nZero = (UINT16)nWordCount + 1;	
				nWordCount = 0;			
			} else {
				nZero = nZeroRun + 1;
				nWordCount -= nZeroRun;		
			}
			memset(pOutput, 0, nZero * sizeof(INT16));
			pOutput += nZero;
		} else {
			*pOutput++ = pNode->m_Symbol.nValue;
		}
	}
	delete m_pTree;
	m_pTree = NULL;
	return(NCS_SUCCESS);
}



extern "C" NCSError unpack_huffman(UINT8 *pPacked, 
								  INT16 *pUnPacked,
								  UINT32 nRawLength)
{
	CNCSHuffmanCoder HC;
	CNCSError Error = HC.UnPack(pPacked, pUnPacked, nRawLength);
	return(Error.GetErrorNumber());
}

extern "C" void unpack_huffman_init_state(NCSHuffmanState *pState, UINT8 **ppPacked)
{
	pState->pTree = (void*)new CNCSHuffmanCoder::CTree(ppPacked);
	pState->nBitsUsed = 0;
}

extern "C" void unpack_huffman_fini_state(NCSHuffmanState *pState)
{
	delete (CNCSHuffmanCoder::CTree*)pState->pTree;
	pState->pTree = (void*)NULL;
	pState->nBitsUsed = 0;
}

extern "C" NCSHuffmanSymbol *unpack_huffman_symbol(UINT8 **ppPacked,
												   NCSHuffmanState *pState)
{
	register UINT32 nBitsUsed = pState->nBitsUsed;
	register CNCSHuffmanCoder::CCodeNode *pNode = ((CNCSHuffmanCoder::CTree*)pState->pTree)->m_pRoot;
	register UINT8 *pEncoded = *ppPacked;

	while (pNode->m_Children.m_P.m_p0Child != 0) {
		pNode = pNode->m_Children.m_Children[(pEncoded[nBitsUsed >> 3] >> (nBitsUsed & 0x7)) & 0x1];
		nBitsUsed++;
	}

	pState->nBitsUsed = nBitsUsed;
	return(&pNode->m_Symbol);
}

extern "C" UINT32 pack_huffman(UINT8 *pPacked, INT16 *pUnPacked, UINT32 nRawLength)
{
	UINT32 nLen;
	CNCSHuffmanCoder HC;
	CNCSError Error = HC.Pack(pPacked, &nLen, pUnPacked, nRawLength);
	return(nLen);
}
