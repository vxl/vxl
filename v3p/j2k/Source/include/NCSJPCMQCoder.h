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
** FILE:     $Archive: /NCS/Source/include/NCSJPCMQCoder.h $
** CREATED:  12/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCMQCoder class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCMQCODER_H
#define NCSJPCMQCODER_H

#include "NCSJPCTypes.h"

//
// Defining this uses a "Common Decoding Path" optimisation from the 
// JPEG2000 book by Taubman and MArcellin.  Benchmarking indicates
// this is SLOWER on on Pentium4 type CPU, as the small efficiency gain
// from one less if test in the CDP path is more than offset by the 
// significant additional complexity in the non-CDP path, so it is 
// NOT defined by default.
//
#undef NCSJPC_USE_CDP_OPT

//
// Inline certain functions for performance improvements,
// including SetCurCtx() and Renormd().
//
#define NCSJPC_MQ_INLINE NCS_INLINE
#define NCSJPC_MQC_NUMCTXS 19

	/**
	 * CNCSJPCMQCoder class - the JPC MQ Coder.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.13 $ $Author: simon $ $Date: 2005/07/13 02:51:07 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCMQCoder {
public:
	CNCSJPCMQCoder();
	virtual ~CNCSJPCMQCoder();

	static void InitDec(UINT8 *pData, UINT32 nLen, bool bFirstSeg);
	static void InitEnc(UINT8 *pData, UINT32 nLen);
	static void Flush();
#ifdef NCSJPC_USE_CDP_OPT
	static NCSJPC_MQ_INLINE INT32 Decode(UINT8 nCtx) {
				UINT16 nQEval = sm_Contexts[nCtx].nQEval;
				INT32 d = sm_Contexts[nCtx].nD;

				sm_D -= nQEval;
				if(sm_D < 0) {
					UINT8 n;
					sm_A += sm_D;
					sm_C.m_CHiLo.m_CHigh += sm_D;

					if((INT16)sm_C.m_CHiLo.m_CHigh >= 0) {
						
						// MPS Exchange
						if(sm_A < nQEval) {
							d = !d;
							n = sm_Contexts[nCtx].nNLPS;				
						} else {
							n = sm_Contexts[nCtx].nNMPS;
						}
					} else {
						sm_C.m_CHiLo.m_CHigh += nQEval;
						//LPSExchange
						if(sm_A < nQEval) {
							n = sm_Contexts[nCtx].nNMPS;
						} else {
							d = !d;
							n = sm_Contexts[nCtx].nNLPS;
						}
						sm_A = nQEval;
					}
					sm_Contexts[nCtx] = sm_States[n];

					Renormd();

					if(sm_A - 0x8000 < sm_C.m_CHiLo.m_CHigh) {
						sm_D = sm_A - 0x8000;
						sm_A -= sm_D;
						sm_C.m_CHiLo.m_CHigh -= sm_D;
					} else {
						sm_D = sm_C.m_CHiLo.m_CHigh;
						sm_A -= sm_C.m_CHiLo.m_CHigh;
						sm_C.m_CHiLo.m_CHigh = 0;
					}
					return(d);
				}
				return(d);
			};
#else // NCSJPC_USE_CDP_OPT
	static NCSJPC_MQ_INLINE INT32 Decode(UINT8 nCtx) {
				UINT16 nQEval = sm_Contexts[nCtx].nQEval;
				INT32 d = sm_Contexts[nCtx].nD;

				sm_A = sm_A - nQEval;

				if (sm_C.m_CHiLo.m_CHigh >= nQEval) {
					sm_C.m_CHiLo.m_CHigh = sm_C.m_CHiLo.m_CHigh - nQEval;

					if (/*(INT16)sm_A >= 0) {*/sm_A < 0x8000) {
						// MPS Exchange
						if (sm_A < nQEval) {
							d = !d;
							sm_Contexts[nCtx] = sm_States[sm_Contexts[nCtx].nNLPS];
						} else {
							sm_Contexts[nCtx] = sm_States[sm_Contexts[nCtx].nNMPS];
						}
						// Renormd MPS
						RenormDec();
					}
				} else {
					//LPSExchange
					if (sm_A < nQEval) {
						sm_Contexts[nCtx] = sm_States[sm_Contexts[nCtx].nNMPS];
					} else {
						d = !d;
						sm_Contexts[nCtx] = sm_States[sm_Contexts[nCtx].nNLPS];
					}
					sm_A = nQEval;
					// Renormd LPS
					RenormDec();
				}
				return(d);
			};
#endif // NCSJPC_USE_CDP_OPT
//	static NCSJPC_MQ_INLINE void Encode(INT32 d, UINT8 nCtx)
//	{
	//	sm_SymbolStream[sm_nSymbolStream++] = d;
	//	sm_SymbolStream[sm_nSymbolStream++] = nCtx;
//	}

		static NCSJPC_MQ_INLINE void Encode(INT32 d, UINT8 nCtx)
			{
				if (sm_Contexts[nCtx].nD == d) {
					UINT16 nQEval = sm_Contexts[nCtx].nQEval;
					sm_A = sm_A - nQEval;
					if (sm_A < 0x8000) {
						if (sm_A >= nQEval) {
							sm_C.m_C24 += nQEval;
						} else {
							sm_A = nQEval;
						}
						sm_Contexts[nCtx] = sm_States[sm_Contexts[nCtx].nNMPS];
						RenormEnc();
					} else {
						sm_C.m_C24 += nQEval;
					}
				} else {
					UINT16 nQEval = sm_Contexts[nCtx].nQEval;
					sm_A = sm_A - nQEval;

					if (sm_A < nQEval) {
						sm_C.m_C24 += nQEval;
					} else {
						sm_A = nQEval;
					}
					sm_Contexts[nCtx] = sm_States[sm_Contexts[nCtx].nNLPS];
					RenormEnc();
				}
			};
	static void ResetStates();
	static void SetState(UINT8 nCtx, int msb, int prob);
	static INT32 GetBit();
	static void SetBits();
//private:
	struct CHiLo {
#ifdef NCSBO_MSBFIRST
		UINT16 m_CHigh;
		UINT8 m_CLow;
		UINT8 __pad[1];
#else
		UINT8 __pad[1];
		UINT8	m_CLow;
		UINT16 m_CHigh;
#endif
	};

	static union CHiLoUnion {
		struct CHiLo m_CHiLo;
		UINT32 m_C24;
	} sm_C;

#ifdef NCSJPC_USE_CDP_OPT
	static INT32 sm_D;
#endif // NCSJPC_USE_CDP_OPT
	static UINT16 sm_A;
	static UINT16 sm_nCT;
	static INT32 sm_nIndex;
	static UINT8 *sm_pB;
//static UINT8 sm_SymbolStream[16384];//FIXME
//static UINT32 sm_nSymbolStream;
	// The extra pad bytes helps with alignment and makes it faster
	typedef struct State {
		UINT16	nQEval;
		UINT16	nD;
		UINT8	nNMPS; 
		UINT8	nNLPS;
		UINT8	__pad[2];
	} State;

	static State sm_Contexts[NCSJPC_MQC_NUMCTXS];
	static const State sm_States[47*2];
	static UINT8 sm_ShiftLut[4096];

	static void ByteIn();
	static void ByteOut();
	static void RenormDec();
	static void RenormEnc();
};

#endif // !NCSJPCMQCODER_H
