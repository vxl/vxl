/********************************************************** 
** Copyright 1998 Earth Resource Mapping Ltd.
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
** FILE:   	NCSHuffmanCoder.h
** CREATED:	30 Jun 2004
** AUTHOR: 	Simon Cope
** PURPOSE:	CNCSHuffmanCoder class header
** EDITS:
*******************************************************/

#ifndef NCSHUFFMANCODER_H
#define NCSHUFFMANCODER_H

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif

#include "NCSTypes.h"

#ifdef __cplusplus

#include <map>

extern "C" {
#endif // __cplusplus

typedef struct _NCSHuffmanSymbol {
	UINT16 nValue;
	BOOLEAN bZeroRun;
} NCSHuffmanSymbol;

#ifdef __cplusplus

#include "NCSError.h"

/**
 * CNCSHuffmanCoder class - ECW Huffman Coder/Decoder.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.4 $ $Author: simon $ $Date: 2005/01/17 09:07:54 $ 
 */
class CNCSHuffmanCoder {
public:
	const static UINT32 NUM_SYMBOL_VALUES;
	const static UINT16 SIGN_MASK;
	const static UINT16 RUN_MASK;
	const static INT16 MAX_BIN_VALUE;
	const static INT16 MIN_BIN_VALUE;
	const static UINT16 VALUE_MASK;
	const static UINT16 MAX_RUN_LENGTH;
	const static UINT8 SMALL_SYMBOL;
	const static UINT8 SMALL_SHIFT;

	/*
	**	Huffman structures and definitions
	*/
	class CTree;

	class CCodeNode {  
	public:
		typedef struct {
			class CCodeNode *m_p0Child;
			class CCodeNode *m_p1Child;
		} A;
		typedef union {
			A				m_P;
			class CCodeNode *m_Children[2];
		} U;
		U m_Children;

		NCSHuffmanSymbol m_Symbol;
		UINT32	m_nFrequency;
		class CCodeNode	*m_pNext;  
		UINT32 	m_nCode;
		UINT8	m_nCodeBits;
		bool	m_bInHistogram;

		CCodeNode();
		CCodeNode(UINT8 **ppPacked, UINT32 &nNodes);
		virtual ~CCodeNode();

		void Pack(UINT8 **ppPacked, UINT32 &nNodes);
		CCodeNode *Unpack(UINT8 **ppPacked, UINT32 &nNodes);
		void SetCode(UINT32 nCode, UINT8 nCodeBits);
	};
	class CTree: public CCodeNode {
	public:
		CCodeNode *m_pRoot;
		std::map<INT16, CCodeNode*> m_Histogram;

		CTree();
		CTree(UINT8 **ppPacked);
		virtual ~CTree();

		CNCSError Pack(UINT8 **ppPacked, INT16 *pUnPacked, UINT32 nSymbols);
		CNCSError Unpack(UINT8 **ppPacked);
	protected:
		CCodeNode *UnpackNode(UINT8 **ppPacked, UINT32 &nNodes);
	};

		/** Default constructor. */
	CNCSHuffmanCoder();
		/** virtual default destructor. */
	virtual ~CNCSHuffmanCoder();

	CNCSError Pack(UINT8 *pPacked, UINT32 *pPackedLength, INT16 *pUnPacked, UINT32 nRawLength);
	CNCSError UnPack(UINT8 *pPacked, INT16 *pUnPacked, UINT32 nRawLength);
private:
	CTree *m_pTree;
};

};
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define NCS_HUFFMAN_MAX_RUN_LENGTH 0x7fff
#define NCS_HUFFMAN_SIGN_MASK	0x4000
#define NCS_HUFFMAN_VALUE_MASK	0x3fff
#define NCS_HUFFMAN_RUN_MASK	0x8000

typedef struct {
	void	*pTree;
	UINT32	nBitsUsed;		
} NCSHuffmanState;

NCSError unpack_huffman(UINT8 *pPacked, INT16 *pUnPacked, UINT32 nRawLength);
void unpack_huffman_init_state(NCSHuffmanState *pState, UINT8 **ppPacked);
void unpack_huffman_fini_state(NCSHuffmanState *pState);
NCSHuffmanSymbol *unpack_huffman_symbol(UINT8 **ppPacked, NCSHuffmanState *pState);
static NCS_INLINE BOOLEAN unpack_huffman_symbol_zero_run(NCSHuffmanSymbol *pSymbol) {
				return(pSymbol->bZeroRun);
			}
static NCS_INLINE UINT16 unpack_huffman_symbol_zero_length(NCSHuffmanSymbol *pSymbol) {
				return(pSymbol->nValue);
			};
static NCS_INLINE INT16 unpack_huffman_symbol_value(NCSHuffmanSymbol *pSymbol) {
				return(pSymbol->nValue);
			};

UINT32	pack_huffman(UINT8 *pPacked, INT16 *pUnPacked, UINT32 symbol_stream_length);
#ifdef __cplusplus
};
#endif

#endif /* NCSHUFFMANCODER_H */
