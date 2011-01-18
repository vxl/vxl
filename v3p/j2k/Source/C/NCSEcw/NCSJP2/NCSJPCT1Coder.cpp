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
S
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCT1Coder.cpp $
** CREATED:  21/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCT1Coder class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCT1Coder.h"
#include "NCSJPCSubBand.h"
#include "NCSJPCCodeBlock.h"

#ifdef WIN32
#define T1_INLINE __forceinline
#else
#define T1_INLINE NCS_INLINE
#endif

const int CNCSJPCT1Coder::SELECTIVE_CODING_BYPASS	= (0x1 << 0);
const int CNCSJPCT1Coder::PASS_RESET_CTX			= (0x1 << 1);
const int CNCSJPCT1Coder::PASS_TERMINATION			= (0x1 << 2);
const int CNCSJPCT1Coder::VERTICAL_CAUSAL_CTX		= (0x1 << 3);
const int CNCSJPCT1Coder::PREDICTABLE_TERMINATION	= (0x1 << 4);
const int CNCSJPCT1Coder::SEGMENT_SYMBOLS			= (0x1 << 5);

//
// Significance propogation bits
//
// D0 V0 D1
// H0 XX H1
// D2 V1 D3
//
#define T1_SIG_D1 0x0001
#define T1_SIG_D3 0x0002
#define T1_SIG_D2 0x0004
#define T1_SIG_D0 0x0008
#define T1_SIG_V0 0x0010
#define T1_SIG_H1 0x0020
#define T1_SIG_V1 0x0040
#define T1_SIG_H0 0x0080

// All horizontal+vertical bits
#define T1_SIG_HV (T1_SIG_V0|T1_SIG_H1|T1_SIG_V1|T1_SIG_H0)
// All 8 neighbour hor+ver+diag
#define T1_SIG_HVD (T1_SIG_HV|T1_SIG_D1|T1_SIG_D3|T1_SIG_D2|T1_SIG_D0)

// This pixel significant
#define T1_SIG 0x1000
#define T1_REFINE 0x2000
#define T1_VISIT 0x4000

//
// Sign bits
//
//    V0 
// H0 XX H1
//    V1 
//
#define T1_SGN_V0 0x0100
#define T1_SGN_H1 0x0200
#define T1_SGN_V1 0x0400
#define T1_SGN_H0 0x0800
#define T1_SGN (T1_SGN_V0|T1_SGN_H1|T1_SGN_V1|T1_SGN_H0)

#define T1_NUMCTXS_AGG 1
#define T1_NUMCTXS_ZC 9
#define T1_NUMCTXS_MAG 3
#define T1_NUMCTXS_SC 5
#define T1_NUMCTXS_UNI 1

#define T1_CTXNO_AGG 0
#define T1_CTXNO_ZC (T1_CTXNO_AGG+T1_NUMCTXS_AGG)
#define T1_CTXNO_MAG (T1_CTXNO_ZC+T1_NUMCTXS_ZC)
#define T1_CTXNO_SC (T1_CTXNO_MAG+T1_NUMCTXS_MAG)
#define T1_CTXNO_UNI (T1_CTXNO_SC+T1_NUMCTXS_SC)
#define T1_NUMCTXS (T1_CTXNO_UNI+T1_NUMCTXS_UNI)


CNCSJPCBuffer CNCSJPCT1Coder::sm_Data;
CNCSJPCBuffer CNCSJPCT1Coder::sm_Flags;
UINT8 CNCSJPCT1Coder::m_lut_ctxno_zc0[256];
UINT8 CNCSJPCT1Coder::m_lut_ctxno_zc1[256];
UINT8 CNCSJPCT1Coder::m_lut_ctxno_zc2[256];
UINT8 CNCSJPCT1Coder::m_lut_ctxno_zc3[256];
UINT8 *CNCSJPCT1Coder::m_plut_ctxno_zc;
UINT8 CNCSJPCT1Coder::m_lut_ctxno_sc[256];
UINT8 CNCSJPCT1Coder::m_lut_spb[256];
INT8 CNCSJPCT1Coder::sm_nBitPlane;
INT32 CNCSJPCT1Coder::sm_one;
INT32 CNCSJPCT1Coder::sm_half;
INT32 CNCSJPCT1Coder::sm_oneplushalf;
UINT8 CNCSJPCT1Coder::sm_Scb;
bool bVCC = false;
bool bBypass = false;
NCSTimeStampUs CNCSJPCT1Coder::sm_usTotal;
NCSTimeStampUs CNCSJPCT1Coder::sm_usLast;
UINT64 CNCSJPCT1Coder::sm_nTotalSamples;
UINT64 CNCSJPCT1Coder::sm_nLastSamples;
CNCSJPCMemoryIOStream CNCSJPCT1Coder::sm_Stream;
UINT32 nSigValues = 0;
bool bAllSig = false;

CNCSJPCT1Coder::CNCSJPCT1Coder()
{
	InitLUTs();
	m_pEncTmpBuf = NULL;
}

CNCSJPCT1Coder::~CNCSJPCT1Coder()
{
	NCSFree(m_pEncTmpBuf);
}

bool CNCSJPCT1Coder::GetBit()
{
	bool bBit;
	sm_Stream.UnStuff(bBit);
	return(bBit);
}


T1_INLINE UINT8 CNCSJPCT1Coder::GetCtxNo_ZC(T1_FLAG_TYPE f) 
{
	return m_plut_ctxno_zc[f&T1_SIG_HVD];
}

T1_INLINE UINT8 CNCSJPCT1Coder::GetCtxNo_SC(T1_FLAG_TYPE f) 
{
	return m_lut_ctxno_sc[(f&(T1_SIG_HV|T1_SGN))>>4];
}

T1_INLINE INT32 CNCSJPCT1Coder::GetSPB(T1_FLAG_TYPE f) 
{
	return m_lut_spb[(f&(T1_SIG_HV|T1_SGN))>>4];
}

T1_INLINE UINT8 CNCSJPCT1Coder::GetCtxNoMag(T1_FLAG_TYPE f) 
{
	if(!(f&T1_REFINE)){
		return(T1_CTXNO_MAG+((f&(T1_SIG_HVD))?1:0));
	} else {
		return(T1_CTXNO_MAG+2);
	}
}

__inline void CNCSJPCT1Coder::UpdateFlags(T1_FLAG_TYPE *fp, int s) 
{
	T1_FLAG_TYPE *wp=fp-(sm_Flags.GetStep());
	T1_FLAG_TYPE *ep=fp+(sm_Flags.GetStep());
	if(bVCC) {
	//	wp[-1]|=T1_SIG_D3;
		wp[1]|=T1_SIG_D1;
	//	ep[-1]|=T1_SIG_D2;
		ep[1]|=T1_SIG_D0;
		if(s) {
			*wp|=T1_SGN_H1|T1_SIG_H1;
			*ep|=T1_SGN_H0|T1_SIG_H0;
	//		fp[-1]|=T1_SGN_V1|T1_SIG_V1;
			*fp|=T1_SIG;
			fp[1]|=T1_SGN_V0|T1_SIG_V0;
		} else {
			*wp|=T1_SIG_H1;
			*ep|=T1_SIG_H0;
	//		fp[-1]|=T1_SIG_V1;
			*fp|=T1_SIG;
			fp[1]|=T1_SIG_V0;
		}
	} else {
		wp[-1]|=T1_SIG_D3;
		wp[1]|=T1_SIG_D1;
		ep[-1]|=T1_SIG_D2;
		ep[1]|=T1_SIG_D0;
		if(s) {
			*wp|=T1_SGN_H1|T1_SIG_H1;
			*ep|=T1_SGN_H0|T1_SIG_H0;
			fp[-1]|=T1_SGN_V1|T1_SIG_V1;
			*fp|=T1_SIG;
			fp[1]|=T1_SGN_V0|T1_SIG_V0;
		} else {
			*wp|=T1_SIG_H1;
			*ep|=T1_SIG_H0;
			fp[-1]|=T1_SIG_V1;
			*fp|=T1_SIG;
			fp[1]|=T1_SIG_V0;
		}
	}
	nSigValues++;
}

T1_INLINE void CNCSJPCT1Coder::DecSigPassStep(T1_FLAG_TYPE *fp, int *dp) 
{
	if (!(*fp&(T1_SIG|T1_VISIT)) && (*fp&T1_SIG_HVD)) {
		if(bBypass) {
			if (CNCSJPCT1Coder::GetBit()) {
				int v=CNCSJPCT1Coder::GetBit();//^GetSPB(*fp);
				*dp=v?-sm_oneplushalf:sm_oneplushalf;
				UpdateFlags(fp, v);
			}
		} else {
			if (CNCSJPCMQCoder::Decode(GetCtxNo_ZC(*fp))) {
				int v=CNCSJPCMQCoder::Decode(GetCtxNo_SC(*fp))^GetSPB(*fp);
				*dp=v?-sm_oneplushalf:sm_oneplushalf;
				UpdateFlags(fp, v);
			}
		}
		*fp|=T1_VISIT;
	}
}

void CNCSJPCT1Coder::DecSigPass(int w, int h) {
	//int nDataStep = sm_Data.GetStep();
	int nFlagsStep = sm_Flags.GetStep();

	for (int k=0; k<h; k+=4) {
		int max_j = NCSMin(4, h - k);
		INT32 *pData = (INT32*)sm_Data.GetPtr(0, k >> 2);
		T1_FLAG_TYPE *pFlags = (T1_FLAG_TYPE*)sm_Flags.GetPtr(k+1, 1);

		if(max_j == 4) {
			for (int sm_i=0; sm_i<w; sm_i++, pFlags += nFlagsStep) {
				bVCC = (sm_Scb & VERTICAL_CAUSAL_CTX) ? true : false;DecSigPassStep(pFlags, pData + sm_i * 4);bVCC = false;
				DecSigPassStep(pFlags + 1, pData + sm_i * 4 + 1);
				DecSigPassStep(pFlags + 2, pData + sm_i * 4 + 2);
				DecSigPassStep(pFlags + 3, pData + sm_i * 4 + 3);
			}
		} else {
			for (int i=0; i<w; i++, pFlags += nFlagsStep) {
				for (int j=0; j< max_j; j++) {
					if(j == 0) bVCC = (sm_Scb & VERTICAL_CAUSAL_CTX) ? true : false;;
					DecSigPassStep(pFlags + j, pData + i * 4 + j);
					bVCC = false;
				}
			}
		}
	}
}

T1_INLINE void CNCSJPCT1Coder::DecRefPassStep(T1_FLAG_TYPE *fp, int *dp)
{
	if ((*fp&(T1_SIG|T1_VISIT))==T1_SIG) {
		int v;
		if(bBypass) {
			v = CNCSJPCT1Coder::GetBit();
		} else {
			v = CNCSJPCMQCoder::Decode(GetCtxNoMag(*fp));
		}
		int t=v?sm_half:-sm_half;
		*dp+=*dp<0?-t:t;
		*fp|=T1_REFINE;
	}
}

void CNCSJPCT1Coder::DecRefPass(int w, int h) {
	int nFlagsStep = sm_Flags.GetStep();
	
	for (int k=0; k<h; k+=4) {
		T1_FLAG_TYPE *pFlags = (T1_FLAG_TYPE*)sm_Flags.GetPtr(k+1, 1);
		INT32 *pData = (INT32*)sm_Data.GetPtr(0, k >> 2);

		for (int i=0; i<w; i++, pFlags += nFlagsStep) {
			bVCC = (sm_Scb & VERTICAL_CAUSAL_CTX) ? true : false;;DecRefPassStep(pFlags, pData + i * 4);bVCC = false;
			DecRefPassStep(pFlags + 1, pData + i * 4 + 1);
			DecRefPassStep(pFlags + 2, pData + i * 4 + 2);
			DecRefPassStep(pFlags + 3, pData + i * 4 + 3);
		}
	}
}

T1_INLINE void CNCSJPCT1Coder::DecRefPassStepBit0(T1_FLAG_TYPE *fp)
{
	if ((*fp&(T1_SIG|T1_VISIT))==T1_SIG) {
		CNCSJPCMQCoder::Decode(GetCtxNoMag(*fp));
	}
}

void CNCSJPCT1Coder::DecRefPassBit0(int w, int h) {
	int nFlagsStep = sm_Flags.GetStep();
	
	for (int k=0; k<h; k+=4) {
		T1_FLAG_TYPE *pFlags = (T1_FLAG_TYPE*)sm_Flags.GetPtr(k+1, 1);

		for (int i=0; i<w; i++, pFlags += nFlagsStep) {
			bVCC = (sm_Scb & VERTICAL_CAUSAL_CTX) ? true : false;;DecRefPassStepBit0(pFlags);bVCC = false;
			DecRefPassStepBit0(pFlags + 1);
			DecRefPassStepBit0(pFlags + 2);
			DecRefPassStepBit0(pFlags + 3);
		}
	}
}

T1_INLINE void CNCSJPCT1Coder::DecClnPassStep(T1_FLAG_TYPE *fp, int *dp) {
	if (!(*fp&(T1_SIG|T1_VISIT))) {
		if (CNCSJPCMQCoder::Decode(GetCtxNo_ZC(*fp))) {
			int v = CNCSJPCMQCoder::Decode(GetCtxNo_SC(*fp))^GetSPB(*fp);
			*dp=v?-sm_oneplushalf:sm_oneplushalf;
			UpdateFlags(fp, v);
		}
	} else {
		*fp&=~T1_VISIT;
	}
}

void CNCSJPCT1Coder::DecClnPassStepPartial(T1_FLAG_TYPE *fp, int *dp) {
	int v = CNCSJPCMQCoder::Decode(GetCtxNo_SC(*fp))^GetSPB(*fp);
	*dp=v?-sm_oneplushalf:sm_oneplushalf;
	UpdateFlags(fp, v);
}

void CNCSJPCT1Coder::DecClnPass(int w, int h, bool segsym) {
// 	int nDataStep = sm_Data.GetStep();
	int nFlagsStep = sm_Flags.GetStep();
	int i, j, k, agg, runlen;
	for (k=0; k<h; k+=4) {
		int max_j = NCSMin(4, h - k);
		INT32 *pData = (INT32*)sm_Data.GetPtr(0, k >> 2);
		T1_FLAG_TYPE *pFlags = (T1_FLAG_TYPE*)sm_Flags.GetPtr(k+1, 1);

		if (max_j == 4) {
			for (i=0; i<w; i++, pData += 4, pFlags += nFlagsStep) {        
				agg = !(*(pFlags)&(T1_SIG|T1_VISIT|T1_SIG_HVD) ||
						*(pFlags + 1)&(T1_SIG|T1_VISIT|T1_SIG_HVD) ||
						*(pFlags + 2)&(T1_SIG|T1_VISIT|T1_SIG_HVD) ||
						*(pFlags + 3)&(T1_SIG|T1_VISIT|T1_SIG_HVD));
				if (agg) {
					if (!CNCSJPCMQCoder::Decode(T1_CTXNO_AGG)) {
						continue;
					}
					runlen=CNCSJPCMQCoder::Decode(T1_CTXNO_UNI);
					runlen=(runlen<<1)|CNCSJPCMQCoder::Decode(T1_CTXNO_UNI);
//					switch(runlen) {
//						case 0:	DecClnPassStepPartial(pFlags, pData);
//						case 1:	DecClnPassStep(pFlags + 1, pData + 1, true && (1==runlen));
//						case 2:	DecClnPassStep(pFlags + 2, pData + 2, true && (2==runlen));
//						case 3:	DecClnPassStep(pFlags + 3, pData + 3, true && (3==runlen));
//					}
					switch(runlen) {
						case 0:	bVCC = (sm_Scb & VERTICAL_CAUSAL_CTX) ? true : false;DecClnPassStepPartial(pFlags, pData);bVCC = false;
								DecClnPassStep(pFlags + 1, pData + 1);
								DecClnPassStep(pFlags + 2, pData + 2);
								DecClnPassStep(pFlags + 3, pData + 3);
							break;
						case 1:
								DecClnPassStepPartial(pFlags + 1, pData + 1);
								DecClnPassStep(pFlags + 2, pData + 2);
								DecClnPassStep(pFlags + 3, pData + 3);
							break;
						case 2:
								DecClnPassStepPartial(pFlags + 2, pData + 2);
								DecClnPassStep(pFlags + 3, pData + 3);
							break;
						case 3:
								DecClnPassStepPartial(pFlags + 3, pData + 3);
							break;
					}
				} else {
					runlen=0;
					bVCC = (sm_Scb & VERTICAL_CAUSAL_CTX) ? true : false;DecClnPassStep(pFlags, pData);bVCC = false;
					DecClnPassStep(pFlags + 1, pData + 1);
					DecClnPassStep(pFlags + 2, pData + 2);
					DecClnPassStep(pFlags + 3, pData + 3);
				}
//				for (j=runlen; j<4; j++) {
//					DecClnPassStep(pFlags + j * nFlagsStep, pData + j * nDataStep, orient, oneplushalf, agg && (j==runlen));
//				}
			}
		} else {
			for (i=0; i<w; i++, pData += 4, pFlags += nFlagsStep) {
				for (j=0; j<max_j; j++) {
					if(j == 0) bVCC = (sm_Scb & VERTICAL_CAUSAL_CTX) ? true : false;
					DecClnPassStep(pFlags + j, pData + j);
					bVCC = false;
				}
			}
		}
	}
	if (segsym) {
		int v=0;
		v=CNCSJPCMQCoder::Decode(T1_CTXNO_UNI);
		v=(v<<1)|CNCSJPCMQCoder::Decode(T1_CTXNO_UNI);
		v=(v<<1)|CNCSJPCMQCoder::Decode(T1_CTXNO_UNI);
		v=(v<<1)|CNCSJPCMQCoder::Decode(T1_CTXNO_UNI);
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
		if (v!=0xa) {//FIXME
			fprintf(stderr, "warning: bad segmentation symbol\n");
		}
#endif // NCS_BUILD_WITH_STDERR_DEBUG_INFO
	}
}

bool CNCSJPCT1Coder::Decode(NCSJPCSubBandType eSBType, 
								UINT8 roiShift, 
								UINT8 nBits,
								UINT8 nZeroBits,
								std::vector<CNCSJPCSegment> &Segments,
								CNCSJPCBuffer *pDecBuf,
								int Flags,
								CNCSJPCQCDMarker &Quantization,
								UINT8 nComponentBits,
								UINT16 nLevels,
								UINT8 nResolution,
								IEEE4 fReconstructionParameter) 
{
	CNCSJPCGlobalLock _Lock;
	nLevels;//Keep compiler happy

	NCSTimeStampUs tsBegin = CNCSJPCBuffer::SystemInfo::GetTime();
	UINT32 nWidth = pDecBuf->GetWidth();
	UINT32 nHeight = pDecBuf->GetHeight();
	nSigValues = 0;
	UINT8 orient = CNCSJPCSubBand::GetOrient(eSBType);
	sm_Scb = (UINT8)Flags;

	if(nWidth * 4 > sm_Data.GetWidth() || (1 + nHeight / 4) > sm_Data.GetHeight()) {
		sm_Data.Alloc(0, 0, nWidth * 4, (1 + nHeight / 4), CNCSJPCBuffer::BT_INT32);
	}
	sm_Data.Clear();

	UINT32 w = 2 + 4 * (nHeight / 4) + ((nHeight % 4) ? 4 : 0);
	UINT32 h = nWidth + 2;
	if(h > sm_Flags.GetHeight() || w > sm_Flags.GetWidth()) {
		sm_Flags.Alloc(0, 0, w, h, NCSJPC_T1_FLAG_BUFTYPE);
	}
	sm_Flags.Clear();

	bBypass = false;

	int passno = nZeroBits * 3;
	CNCSJPCMQCoder::ResetStates();
	CNCSJPCMQCoder::SetState(T1_CTXNO_UNI, 0, 46);
	CNCSJPCMQCoder::SetState(T1_CTXNO_AGG, 0, 3);
	CNCSJPCMQCoder::SetState(T1_CTXNO_ZC, 0, 4);

	switch(orient) {
		case 0: m_plut_ctxno_zc = m_lut_ctxno_zc0; break;
		case 1: m_plut_ctxno_zc = m_lut_ctxno_zc1; break;
		case 2: m_plut_ctxno_zc = m_lut_ctxno_zc2; break;
		case 3: m_plut_ctxno_zc = m_lut_ctxno_zc3; break;
	}
	bAllSig = false;

//	INT16 nMSBs = NCSMin(nBits, nBits - ((passno - (int)nZeroBits * 3 + 2) / 3));
	UINT16 nMSBs = nZeroBits;
	for (UINT32 segno=0; segno<Segments.size(); segno++) {
		CNCSJPCSegment &Seg = Segments[segno];
		if(/*Seg.m_nLength &&*/ Seg.m_pData) {
			if(((passno - nZeroBits * 3) >= 10) && (((passno + 2) % 3) != 2) && (Flags & SELECTIVE_CODING_BYPASS)) {
				bBypass = true;
				sm_Stream.Open(Seg.m_pData, Seg.m_nLength+2);
			} else {
				bBypass = false;
				CNCSJPCMQCoder::InitDec((UINT8*)Seg.m_pData, Seg.m_nLength, bBypass);
			}
			

			for (UINT32 nSegPass=0; nSegPass < Seg.m_nPasses; ++nSegPass) {
				sm_nBitPlane = (INT8)(1+roiShift+nBits - 1 - (nZeroBits + (passno + nSegPass - (int)nZeroBits * 3 + 2) / 3));
	//if(m_nBitPlane <= 7) break;
				sm_one = 1 << sm_nBitPlane;
				sm_half = sm_one >> 1;
				sm_oneplushalf = sm_one | sm_half;

				int passtype = (passno + nSegPass + 2) % 3;
			//	bool bAllSig = nSigValues == nWidth * nWidth;

				switch (passtype) {
					case 0:
		//				if(!bAllSig) {
							DecSigPass(nWidth, nHeight);
							nMSBs++;
		//					bAllSig = (nSigValues == nWidth * nWidth);
		//				}
						break;
					case 1:
//						if(false&&sm_nBitPlane == 0) {
//							DecRefPassBit0(nWidth, nHeight);
//						} else {
							DecRefPass(nWidth, nHeight);
//						}
						break;
					case 2:
		//				if(!bAllSig) {
							DecClnPass(nWidth, nHeight, (Flags & SEGMENT_SYMBOLS) ? true : false);
		//					bAllSig = (nSigValues == nWidth * nWidth);
		//				}
						break;
				}
				if(Flags & CNCSJPCT1Coder::PASS_RESET_CTX) {
					CNCSJPCMQCoder::ResetStates();
					CNCSJPCMQCoder::SetState(T1_CTXNO_UNI, 0, 46);
					CNCSJPCMQCoder::SetState(T1_CTXNO_AGG, 0, 3);
					CNCSJPCMQCoder::SetState(T1_CTXNO_ZC, 0, 4);
				}
				if(bBypass) {
			//		sm_Stream.ResetBitStuff();
//					while(sm_Stream.m_nBitsLeft) {
//						GetBit();
//					}
				}
			}
			if(bBypass) {
//				while(CNCSJPCMQCoder::sm_nCT) {
//					CNCSJPCT1Coder::GetBit();
//				}
				sm_Stream.Close();
			}
		}
		passno += Seg.m_nPasses;
	}
	//UINT32 nStep = sm_Data.GetStep();
	switch(pDecBuf->GetType()) {
		case CNCSJPCBuffer::BT_INT16:
			if(roiShift == 0) {
				for(UINT32 y = 0; y < nHeight; y++) {
					INT16 *pDst = (INT16*)pDecBuf->GetPtr(0, y);
					INT32 *pData = (INT32*)sm_Data.GetPtr(y % 4, y / 4);
					for(UINT32 x = 0; x < nWidth; x++) {
						INT32 v = pData[x * 4] >> 1;
						if(v < 0) {
							v += 1;
						}
						pDst[x] = (INT16)v;
					}
				}
			} else {
				INT32 nMax = 1 << roiShift;
				for(UINT32 y = 0; y < nHeight; y++) {
					INT16 *pDst = (INT16*)pDecBuf->GetPtr(0, y);
					INT32 *pData = (INT32*)sm_Data.GetPtr(y % 4, y / 4);
					for(UINT32 x = 0; x < nWidth; x++) {
						INT32 v = pData[x * 4] >> 1;
						INT32 v2 = abs(v);
//						if(v2 >= nMax) {
//							v2 >>= roiShift;
//							v = v<0?(1-v2):v2;
//						}
						if(v2 >= nMax) {
							v >>= roiShift;
						}
						if(v < 0) {
							v += 1;
						}
						pDst[x] = (INT16)v;
					}
				}
			}
			break;
		case CNCSJPCBuffer::BT_INT32:
			if(roiShift == 0) {
				for(UINT32 y = 0; y < nHeight; y++) {
					INT32 *pDst = (INT32*)pDecBuf->GetPtr(0, y);
					INT32 *pData = (INT32*)sm_Data.GetPtr(y % 4, y / 4);
					for(UINT32 x = 0; x < nWidth; x++) {
						INT32 v = pData[x * 4] >> 1;
						if(v < 0) {
							v += 1;
						}
						pDst[x] = v;
					}
				}
			} else {
				INT32 nMax = 1 << roiShift;
				for(UINT32 y = 0; y < nHeight; y++) {
					INT32 *pDst = (INT32*)pDecBuf->GetPtr(0, y);
					INT32 *pData = (INT32*)sm_Data.GetPtr(y % 4, y / 4);
					for(UINT32 x = 0; x < nWidth; x++) {
						INT32 v = pData[x * 4] >> 1;
						INT32 v2 = abs(v);
						if(v2 >= nMax) {
							v >>= roiShift;
						}
						if(v < 0) {
							v += 1;
						}
						pDst[x] = (INT32)v;
					}
				}
			}
			break;
		case CNCSJPCBuffer::BT_IEEE4:
			{
				INT16 nMantissa = 0;
				INT16 nExponent = 0;

				if(Quantization.m_Sqc.m_eStyle == CNCSJPCQuantizationParameter::SCALAR_DERIVED) {
					nExponent = Quantization.m_SPqc[NCSJPC_LL].m_Irreversible.m_nExponent;// - nLevels;
					nMantissa = Quantization.m_SPqc[NCSJPC_LL].m_Irreversible.m_nMantissa;
				} else if(eSBType == NCSJPC_LL) {
					nMantissa = Quantization.m_SPqc[eSBType].m_Irreversible.m_nMantissa;
					nExponent = Quantization.m_SPqc[eSBType].m_Irreversible.m_nExponent;
				} else {
					INT32 i = 1 + (eSBType - 1) + 3 * (nResolution - 1);
					nMantissa = Quantization.m_SPqc[i].m_Irreversible.m_nMantissa;
					// FIXME - not 100% on this one, but by some fluke it seems to work for now... 
					nExponent = Quantization.m_SPqc[eSBType].m_Irreversible.m_nExponent;
				}
				float fScale = 1.0f;
				switch(eSBType) {
					case NCSJPC_LL:
							fScale = (float)NCS2PowS(nComponentBits - nExponent) * (1.0f + (float)nMantissa / NCSJPC_2POW11);
						break;
					case NCSJPC_LH:
							fScale = (float)NCS2PowS(nComponentBits + 0 - nExponent) * (1.0f + (float)nMantissa / NCSJPC_2POW11);
						break;
					case NCSJPC_HL:
							fScale = (float)NCS2PowS(nComponentBits + 0 - nExponent) * (1.0f + (float)nMantissa / NCSJPC_2POW11);
						break;
					case NCSJPC_HH:
							fScale = (float)NCS2PowS(nComponentBits + 0 - nExponent) * (1.0f + (float)nMantissa / NCSJPC_2POW11);
						break;
				}
//				UINT16 nMSBs = nBits - 1 - (nZeroBits + (passno - (int)nZeroBits * 3 + 2) / 3);

//				IEEE4 fOffset = (3.0f / 8.0f) * (IEEE4)NCS2PowS(sm_nBitPlane - 1);//nBits - nMSBs);
				IEEE4 fOffset = fReconstructionParameter * (IEEE4)NCS2PowS(nBits - nMSBs);//nBits - (passno-1)/3);
//				IEEE4 fOffset = (3.0f / 8.0f) * (IEEE4)NCS2PowS(sm_nBitPlane - 1);
//				IEEE4 fOffset = (1.0f / 2.0f) * (IEEE4)NCS2PowS(nBits - nComponentBits);

				if(roiShift == 0) {
					for(UINT32 y = 0; y < nHeight; y++) {
						IEEE4 *pDst = (IEEE4*)pDecBuf->GetPtr(0, y);
						INT32 *pData = (INT32*)sm_Data.GetPtr(y % 4, y / 4);
						for(UINT32 x = 0; x < nWidth; x++) {
							IEEE4 v = (IEEE4)(pData[x * 4] >> 1);
							if(v > 0.0) {
//								v *= fScale;
								v = (v + fOffset) * fScale;
							} else if(v < 0.0) {
//								v += 1;
//								v *= fScale;
								v = (v - fOffset + 1) * fScale;
							}
							pDst[x] = v;
						}
					}
				} else {
					INT32 nMax = 1 << roiShift;
					for(UINT32 y = 0; y < nHeight; y++) {
						IEEE4 *pDst = (IEEE4*)pDecBuf->GetPtr(0, y);
						INT32 *pData = (INT32*)sm_Data.GetPtr(y % 4, y / 4);
						for(UINT32 x = 0; x < nWidth; x++) {
							INT32 v = pData[x * 4] >> 1;
							INT32 v2 = abs(v);
							if(v2 >= nMax) {
								v >>= roiShift;
							}
							if(v > 0) {
								pDst[x] = (v + fOffset) * fScale;
							} else if(v < 0) {
								pDst[x] = (v - fOffset + 1) * fScale;
							} else {
								pDst[x] = (IEEE4)v;
							}
						}
					}
				}
			}
			break;
	}

	sm_usLast = CNCSJPCBuffer::SystemInfo::GetTime() - tsBegin;
	sm_usTotal += sm_usLast;
	sm_nLastSamples = nWidth * nHeight;
	sm_nTotalSamples += sm_nLastSamples;
	return(true);
}

//UINT8 PassLUT[1+32*3];
//INT32 Lengths[1+32*3];

#define T1_SIGN_MASK 0x80000000
#define T1_VALUE_MASK 0x7fffffff
#define T1_ENCODER_BUFSIZE 32768

#define CBSIZE NCSJPC_ENCODE_BLOCK_SIZE
#define CBSTEP (CBSIZE * 4)
#define FLAG_STEP (CBSIZE+2)

UINT32 CNCSJPCT1Coder::sm_EncData[(CBSIZE) * 4 * (1 + CBSIZE / 4)];
CNCSJPCT1Coder::T1_FLAG_TYPE CNCSJPCT1Coder::sm_EncFlags[(CBSIZE+2) * (CBSIZE+2)];

__inline void CNCSJPCT1Coder::EncUpdateFlags(T1_FLAG_TYPE *fp, int s) 
{
	CNCSJPCT1Coder::T1_FLAG_TYPE *wp=fp-(FLAG_STEP);
	CNCSJPCT1Coder::T1_FLAG_TYPE *ep=fp+(FLAG_STEP);

	wp[-1]|=T1_SIG_D3;
	wp[1]|=T1_SIG_D1;
	ep[-1]|=T1_SIG_D2;
	ep[1]|=T1_SIG_D0;
	if(s) {
		*wp|=T1_SGN_H1|T1_SIG_H1;
		*ep|=T1_SGN_H0|T1_SIG_H0;
		fp[-1]|=T1_SGN_V1|T1_SIG_V1;
		*fp|=T1_SIG;
		fp[1]|=T1_SGN_V0|T1_SIG_V0;
	} else {
		*wp|=T1_SIG_H1;
		*ep|=T1_SIG_H0;
		fp[-1]|=T1_SIG_V1;
		*fp|=T1_SIG;
		fp[1]|=T1_SIG_V0;
	}

	nSigValues++;
}

T1_INLINE void CNCSJPCT1Coder::EncSigPassStep(T1_FLAG_TYPE *fp, int *dp)
{
	if ((*fp&T1_SIG_HVD) && !(*fp&(T1_SIG|T1_VISIT))) {
		if(*dp&sm_one) {
			CNCSJPCMQCoder::Encode(1, GetCtxNo_ZC(*fp));
//			PassLUT[passno]++;
			if(*dp & T1_SIGN_MASK) {
				CNCSJPCMQCoder::Encode(1^GetSPB(*fp), GetCtxNo_SC(*fp));
				EncUpdateFlags(fp, 1);
			} else {
				CNCSJPCMQCoder::Encode(0^GetSPB(*fp), GetCtxNo_SC(*fp));
				EncUpdateFlags(fp, 0);
			}
		} else {
			CNCSJPCMQCoder::Encode(0, GetCtxNo_ZC(*fp));
		}
		*fp|=T1_VISIT;
	}
}

void CNCSJPCT1Coder::EncSigPass(int w, int h)
{
	int i, j, k;
	for (k=0; k<h; k+=4) {
		int max_j = NCSMin(4, h - k);
		INT32 *pData = (INT32*)&sm_EncData[(k >> 2) * CBSTEP];
		T1_FLAG_TYPE *pFlags = (T1_FLAG_TYPE*)&sm_EncFlags[k+1 + 1*FLAG_STEP];

		if(max_j == 4) {
			for (i=0; i<w; i++, pData += 4, pFlags += FLAG_STEP) {
				EncSigPassStep(pFlags + 0, pData + 0);
				EncSigPassStep(pFlags + 1, pData + 1);
				EncSigPassStep(pFlags + 2, pData + 2);
				EncSigPassStep(pFlags + 3, pData + 3);
			}
		} else {
			for (i=0; i<w; i++,pData += 4, pFlags += FLAG_STEP) {
				for (j=0; j<max_j; j++) {
					EncSigPassStep(pFlags + j, pData + j);
				}
			}
		}
	}
}

T1_INLINE void CNCSJPCT1Coder::EncRefPassStep(T1_FLAG_TYPE *fp, int *dp) 
{
	if((*fp&(T1_SIG|T1_VISIT)) == T1_SIG) {
//		CNCSJPCMQCoder::Encode((*dp >> sm_nBitPlane) & 1, GetCtxNoMag(*fp));
		if(*dp&sm_one) {
			CNCSJPCMQCoder::Encode(1, GetCtxNoMag(*fp));
//			PassLUT[passno]++;
		} else {
			CNCSJPCMQCoder::Encode(0, GetCtxNoMag(*fp));
		}
		*fp|=T1_REFINE;
	}
}

T1_INLINE void CNCSJPCT1Coder::EncRefPassStepSIG(T1_FLAG_TYPE *fp, int *dp) 
{
//	CNCSJPCMQCoder::Encode((*dp >> sm_nBitPlane) & 1, GetCtxNoMag(*fp));
	if(*dp&sm_one) {
		CNCSJPCMQCoder::Encode(1, GetCtxNoMag(*fp));
	} else {
		CNCSJPCMQCoder::Encode(0, GetCtxNoMag(*fp));
	}
	*fp|=T1_REFINE;
}

void CNCSJPCT1Coder::EncRefPass(int w, int h) 
{
	int i, j, k;
	for (k=0; k<h; k+=4) {
		int max_j = NCSMin(4, h - k);

		INT32 *pData = (INT32*)&sm_EncData[(k >> 2) * CBSTEP];
		T1_FLAG_TYPE *pFlags = (T1_FLAG_TYPE*)&sm_EncFlags[k+1 + 1*FLAG_STEP];

		if(max_j == 4) {
			if(bAllSig) {
				for (i=0; i<w; i++, pData += 4, pFlags += FLAG_STEP) {
					EncRefPassStepSIG(pFlags + 0, pData + 0);
					EncRefPassStepSIG(pFlags + 1, pData + 1);
					EncRefPassStepSIG(pFlags + 2, pData + 2);
					EncRefPassStepSIG(pFlags + 3, pData + 3);
				}
			} else {
				for (i=0; i<w; i++, pData += 4, pFlags += FLAG_STEP) {
					EncRefPassStep(pFlags + 0, pData + 0);
					EncRefPassStep(pFlags + 1, pData + 1);
					EncRefPassStep(pFlags + 2, pData + 2);
					EncRefPassStep(pFlags + 3, pData + 3);
				}
			}
		} else {
			for (i=0; i<w; i++,pData += 4, pFlags += FLAG_STEP) {
				for (j=0; j<max_j; j++) {
					EncRefPassStep(pFlags + j, pData + j);
				}
			}
		}
	}
}

T1_INLINE void CNCSJPCT1Coder::EncClnPassStep(T1_FLAG_TYPE *fp, int *dp)
{
//    if (partial) goto label_partial;

	if (!(*fp&(T1_SIG|T1_VISIT))) {
		if (*dp&sm_one) {
			CNCSJPCMQCoder::Encode(1, GetCtxNo_ZC(*fp));
//label_partial:
//			PassLUT[passno]++;
			if(*dp & T1_SIGN_MASK) {
				CNCSJPCMQCoder::Encode(1^GetSPB(*fp), GetCtxNo_SC(*fp));
				EncUpdateFlags(fp, 1);
			} else {
				CNCSJPCMQCoder::Encode(0^GetSPB(*fp), GetCtxNo_SC(*fp));
				EncUpdateFlags(fp, 0);
			}
		} else {
			CNCSJPCMQCoder::Encode(0, GetCtxNo_ZC(*fp));
		}
	}
	*fp&=~T1_VISIT;

}
T1_INLINE void CNCSJPCT1Coder::EncClnPassStepPartial(T1_FLAG_TYPE *fp, int *dp)
{
	if(*dp & T1_SIGN_MASK) {
		CNCSJPCMQCoder::Encode(1^GetSPB(*fp), GetCtxNo_SC(*fp));
		EncUpdateFlags(fp, 1);
	} else {
		CNCSJPCMQCoder::Encode(0^GetSPB(*fp), GetCtxNo_SC(*fp));
		EncUpdateFlags(fp, 0);
	}
	*fp&=~T1_VISIT;
}

void CNCSJPCT1Coder::EncClnPass(int w, int h)
{
	int i, j, k;
	for (k=0; k<h; k+=4) {
		int max_j = NCSMin(4, h - k);

		INT32 *pData = (INT32*)&sm_EncData[(k >> 2) * CBSTEP];
		T1_FLAG_TYPE *pFlags = (T1_FLAG_TYPE*)&sm_EncFlags[k+1 + 1*FLAG_STEP];

		if (max_j == 4) {
			for (i=0; i<w; i++,pData += 4, pFlags += FLAG_STEP) {
				int agg=!(pFlags[0]&(T1_SIG|T1_VISIT|T1_SIG_HVD) ||
						pFlags[1]&(T1_SIG|T1_VISIT|T1_SIG_HVD) ||
						pFlags[2]&(T1_SIG|T1_VISIT|T1_SIG_HVD) ||
						pFlags[3]&(T1_SIG|T1_VISIT|T1_SIG_HVD));
				if (agg) {
					int runlen;
					for (runlen=0; runlen<4; runlen++) {
						if (pData[runlen] & sm_one) break;
					}
					CNCSJPCMQCoder::Encode(runlen!=4, T1_CTXNO_AGG);
					if (runlen==4) {
						continue;
					}
					CNCSJPCMQCoder::Encode(runlen>>1, T1_CTXNO_UNI);
					CNCSJPCMQCoder::Encode(runlen&1, T1_CTXNO_UNI);
					
					EncClnPassStepPartial(pFlags + runlen, pData + runlen);
					for(j = runlen + 1; j < 4; j++) {
						EncClnPassStep(pFlags + j, pData + j);
					}

				} else {
					EncClnPassStep(pFlags + 0, pData + 0);
					EncClnPassStep(pFlags + 1, pData + 1);
					EncClnPassStep(pFlags + 2, pData + 2);
					EncClnPassStep(pFlags + 3, pData + 3);
				}
			}
		} else {
			for (i=0; i<w; i++,pData += 4, pFlags += FLAG_STEP) {
				for (j = 0; j < max_j; j++) {
					EncClnPassStep(pFlags + j, pData + j);
				}
			}
		}
	}
}

bool CNCSJPCT1Coder::Encode(NCSJPCSubBandType eSBType, 
							UINT8 nBitsTotal,
							UINT8 &nZeroBits,
							std::vector<CNCSJPCSegment> &Segments,
							CNCSJPCBuffer *pDecBuf,
							UINT16 nRatio,
							UINT16 nLayers)
{
	CNCSJPCGlobalLock _Lock;

	int passno=0;
	int passtype;
	INT32 nMax = CNCSJPCBuffer::NCS_MININT32;

	UINT32 nWidth = pDecBuf->GetWidth();
	UINT32 nHeight = pDecBuf->GetHeight();

	nSigValues = 0;
	memset(sm_EncFlags, 0, sizeof(sm_EncFlags));

	INT32 nDecStep = pDecBuf->GetStep(); 

	if(pDecBuf->GetType() == CNCSJPCBuffer::BT_INT16) {
		if(nRatio == 1) {
			for(UINT32 y = 0; y < nHeight; y++) {
				INT16 *pSrc = (INT16*)pDecBuf->GetPtr() + y * nDecStep;
				INT32 *pDst = (INT32*)&sm_EncData[(y % 4) +  (y / 4) * CBSTEP];//sm_Data.GetPtr(y % 4, y / 4);

				for(UINT32 x = 0; x < nWidth; x++) {
					INT16 v = pSrc[x];
					if(v < 0) {
						nMax=NCSMax(nMax, (-v));
						pDst[x * 4] = (-v) | T1_SIGN_MASK;//((v < 0) ? T1_SIGN_MASK : 0);
					} else {
						nMax=NCSMax(nMax, v);
						pDst[x * 4] = v;
					}
	//				INT16 v = pSrc[x];
	//				nMin=NCSMin(nMin, v);
	//				nMax=NCSMax(nMax, v);
	//				pDst[x * 4] = abs(v) | (v & T1_SIGN_MASK);//((v < 0) ? T1_SIGN_MASK : 0);
				}
			}
		} else {
			IEEE4 fRatio = 1.0f / nRatio;

#ifdef NCSJPC_X86_MMI
			if(CNCSJPCBuffer::sm_SystemInfo.m_bSSE2Present && (nWidth % 8) == 0) {
				UINT32 nSubWidth = nWidth / 8;

				__m128 mm7 = _mm_set1_ps(fRatio);
				__m128i mm4 = _mm_set1_epi16((short)CNCSJPCBuffer::NCS_MININT16);

				for(UINT32 y = 0; y < nHeight; y++) {
					INT16 *pSrc = (INT16*)pDecBuf->GetPtr() + y * nDecStep;
					INT32 *pDst = (INT32*)&sm_EncData[(y % 4) +  (y / 4) * CBSTEP];//sm_Data.GetPtr(y % 4, y / 4);

					for(UINT32 x = 0; x < nSubWidth; x++) {
						_mm_prefetch((const char*)&((__m64*)pSrc)[x], _MM_HINT_T2);

						__m128i mm1 = _mm_unpacklo_epi16(_mm_setzero_si128(), _mm_loadu_si128(&((__m128i*)pSrc)[x]));
						mm1 = _mm_srai_epi32(mm1, 16);
						__m128 mm2 = _mm_cvtepi32_ps(mm1);
						mm2 = _mm_mul_ps(mm2, mm7);
						__m128i mm3 = _mm_cvtps_epi32(mm2);
						mm1 = _mm_unpackhi_epi16(_mm_setzero_si128(), _mm_loadu_si128(&((__m128i*)pSrc)[x]));
						mm1 = _mm_srai_epi32(mm1, 16);
						mm2 = _mm_cvtepi32_ps(mm1);
						mm2 = _mm_mul_ps(mm2, mm7);
						__m128i mm5 = _mm_cvtps_epi32(mm2);

						// mm5 = l0 h0 l1 h1 l2 h2 l3 h4
						mm5 = _mm_packs_epi32(mm3, mm5);
						mm5 = _mm_mullo_epi16(mm5, _mm_set1_epi16(nRatio));

							// Create T1_SIGN_MASK mask in mm3
#pragma warning(disable: 4310)
						mm3 = _mm_and_si128(mm5, _mm_set1_epi16((INT16)0x8000)); // generate sign bits, 0x1000 or 0x0000
#pragma warning(default: 4310)
						// Do ABS
						mm5 = _mm_max_epi16(mm5, _mm_mullo_epi16(mm5, _mm_set1_epi16(-1)));
							// Update max value
						mm4 = _mm_max_epi16(mm5, mm4);
							// Or in T1_SIGN_MASK
						mm5 = _mm_or_si128(mm5, mm3);

						pDst[x * 8 * 4] = (INT16)_mm_extract_epi16(mm5, 0);
						pDst[(x * 8 + 1) * 4] = (INT16)_mm_extract_epi16(mm5, 1);
						pDst[(x * 8 + 2) * 4] = (INT16)_mm_extract_epi16(mm5, 2);
						pDst[(x * 8 + 3) * 4] = (INT16)_mm_extract_epi16(mm5, 3);
						pDst[(x * 8 + 4) * 4] = (INT16)_mm_extract_epi16(mm5, 4);
						pDst[(x * 8 + 5) * 4] = (INT16)_mm_extract_epi16(mm5, 5);
						pDst[(x * 8 + 6) * 4] = (INT16)_mm_extract_epi16(mm5, 6);
						pDst[(x * 8 + 7) * 4] = (INT16)_mm_extract_epi16(mm5, 7);
					}
				}
				nMax = NCSMax((INT16)_mm_extract_epi16(mm4, 0), NCSMax((INT16)_mm_extract_epi16(mm4, 1), NCSMax((INT16)_mm_extract_epi16(mm4, 2), NCSMax((INT16)_mm_extract_epi16(mm4, 3), NCSMax((INT16)_mm_extract_epi16(mm4, 4), NCSMax((INT16)_mm_extract_epi16(mm4, 5), NCSMax((INT16)_mm_extract_epi16(mm4, 6), (INT16)_mm_extract_epi16(mm4, 7))))))));
#ifdef NCSJPC_X86_MMI_MMX
				_mm_empty();
			} else if(CNCSJPCBuffer::sm_SystemInfo.m_bSSEPresent && (nWidth % 4) == 0) {
				UINT32 nSubWidth = nWidth / 4;
				
				__m128 mm7 = _mm_set1_ps(fRatio);
				__m64 mm4 = _mm_set1_pi16((short)CNCSJPCBuffer::NCS_MININT16);

				for(UINT32 y = 0; y < nHeight; y++) {
					INT16 *pSrc = (INT16*)pDecBuf->GetPtr() + y * nDecStep;
					INT32 *pDst = (INT32*)&sm_EncData[(y % 4) +  (y / 4) * CBSTEP];//sm_Data.GetPtr(y % 4, y / 4);

					for(UINT32 x = 0; x < nSubWidth; x++) {
						_mm_prefetch((const char*)&((__m64*)pSrc)[x], _MM_HINT_T2);

						__m128 mm1 = _mm_cvtpi16_ps__INLINE(((__m64*)pSrc)[x]);
						__m128 mm2 = _mm_mul_ps(mm1, mm7);
						__m64 mm3 = _mm_cvtps_pi16__INLINE(mm2);
						__m64 mm5 = _mm_mullo_pi16(mm3, _mm_set1_pi16(nRatio));

							// Create T1_SIGN_MASK mask in mm3
#pragma warning(disable: 4310)
						mm3 = _mm_and_si64(mm5, _mm_set1_pi16((INT16)0x8000)); // generate sign bits, 0x1000 or 0x0000
#pragma warning(default: 4310)
							// Do ABS
						mm5 = _mm_max_pi16(mm5, _mm_mullo_pi16(mm5, _mm_set1_pi16(-1)));
							// Update max value
						mm4 = _mm_max_pi16(mm5, mm4);
							// Or in T1_SIGN_MASK
						mm5 = _mm_or_si64(mm5, mm3);

						pDst[x * 4 * 4] = (INT16)_mm_extract_pi16(mm5, 0);
						pDst[(x * 4 + 1) * 4] = (INT16)_mm_extract_pi16(mm5, 1);
						pDst[(x * 4 + 2) * 4] = (INT16)_mm_extract_pi16(mm5, 2);
						pDst[(x * 4 + 3) * 4] = (INT16)_mm_extract_pi16(mm5, 3);
					}
				}
				nMax = NCSMax((INT16)_mm_extract_pi16(mm4, 3), NCSMax((INT16)_mm_extract_pi16(mm4, 2), NCSMax((INT16)_mm_extract_pi16(mm4, 1), (INT16)_mm_extract_pi16(mm4, 0))));
				_mm_empty();
#endif NCSJPC_X86_MMI_MMX
			} else {
#endif //NCSJPC_X86_MMI
				FLT_TO_INT_INIT();
				for(UINT32 y = 0; y < nHeight; y++) {
					INT16 *pSrc = (INT16*)pDecBuf->GetPtr() + y * nDecStep;
					INT32 *pDst = (INT32*)&sm_EncData[(y % 4) +  (y / 4) * CBSTEP];//sm_Data.GetPtr(y % 4, y / 4);

					for(UINT32 x = 0; x < nWidth; x++) {
						INT16 v = (INT16)(nRatio * NCSfloatToInt32_RM(pSrc[x] * fRatio));//pSrc[x];
						if(v < 0) {
							nMax = NCSMax(nMax, -v);
							pDst[x * 4] = (-v) | T1_SIGN_MASK;
						} else {
							nMax=NCSMax(nMax, v);
							pDst[x * 4] = v;//((v < 0) ? T1_SIGN_MASK : 0);
						}
					}
				}
				FLT_TO_INT_FINI();
#ifdef NCSJPC_X86_MMI
			}
#endif //NCSJPC_X86_MMI
		}
	} else {
		if(nRatio == 1) {
			for(UINT32 y = 0; y < nHeight; y++) {
				INT32 *pSrc = (INT32*)pDecBuf->GetPtr() + y * nDecStep;
				INT32 *pDst = (INT32*)&sm_EncData[(y % 4) +  (y / 4) * CBSTEP];//sm_Data.GetPtr(y % 4, y / 4);

				for(UINT32 x = 0; x < nWidth; x++) {
					INT32 v = pSrc[x];
					if(v < 0) {
						nMax=NCSMax(nMax, (-v));
						pDst[x * 4] = (-v) | T1_SIGN_MASK;//((v < 0) ? T1_SIGN_MASK : 0);
					} else {
						nMax=NCSMax(nMax, v);
						pDst[x * 4] = v;
					}
	//				INT16 v = pSrc[x];
	//				nMin=NCSMin(nMin, v);
	//				nMax=NCSMax(nMax, v);
	//				pDst[x * 4] = abs(v) | (v & T1_SIGN_MASK);//((v < 0) ? T1_SIGN_MASK : 0);
				}
			}
		} else {
			IEEE4 fRatio = 1.0f / nRatio;

			FLT_TO_INT_INIT();
			for(UINT32 y = 0; y < nHeight; y++) {
				INT32 *pSrc = (INT32*)pDecBuf->GetPtr() + y * nDecStep;
				INT32 *pDst = (INT32*)&sm_EncData[(y % 4) +  (y / 4) * CBSTEP];//sm_Data.GetPtr(y % 4, y / 4);

				for(UINT32 x = 0; x < nWidth; x++) {
					INT32 v = nRatio * NCSfloatToInt32_RM(pSrc[x] * fRatio);//pSrc[x];
					if(v < 0) {
						nMax = NCSMax(nMax, -v);
						pDst[x * 4] = (-v) | T1_SIGN_MASK;
					} else {
						nMax=NCSMax(nMax, v);
						pDst[x * 4] = v;//((v < 0) ? T1_SIGN_MASK : 0);
					}
				}
			}
			FLT_TO_INT_FINI();
		}
	}

	UINT8 nBits = (UINT8)(nMax ? (NCSLog2(nMax) + 1) : 0);
	nZeroBits = nBitsTotal - nBits;

	sm_nBitPlane = nBits - 1;
	passtype=2;

	CNCSJPCMQCoder::ResetStates();
	CNCSJPCMQCoder::SetState(T1_CTXNO_UNI, 0, 46);
	CNCSJPCMQCoder::SetState(T1_CTXNO_AGG, 0, 3);
	CNCSJPCMQCoder::SetState(T1_CTXNO_ZC, 0, 4);

	int orient = CNCSJPCSubBand::GetOrient(eSBType);

	switch(orient) {
		case 0: m_plut_ctxno_zc = m_lut_ctxno_zc0; break;
		case 1: m_plut_ctxno_zc = m_lut_ctxno_zc1; break;
		case 2: m_plut_ctxno_zc = m_lut_ctxno_zc2; break;
		case 3: m_plut_ctxno_zc = m_lut_ctxno_zc3; break;
	}
	if(!m_pEncTmpBuf) {
		m_pEncTmpBuf = (UINT8*)NCSMalloc(T1_ENCODER_BUFSIZE, TRUE);
	}
	bAllSig = false;
		// Num passes per layer
	IEEE4 fSegPasses = (sm_nBitPlane * 3 + 1) / (IEEE4)nLayers;
	IEEE4 fTotalSegPasses = 0.0f;

	Segments.resize(nLayers);
	UINT16 nPassesOutput = 0;
	UINT32 nSegOffset = 0;
	int nLayer;
	CNCSJPCMQCoder::InitEnc(m_pEncTmpBuf, T1_ENCODER_BUFSIZE);
			
	for(nLayer = 0; nLayer < nLayers; nLayer++) {
		Segments[nLayer].m_nLength = 0;	
		Segments[nLayer].m_pData = NULL;
		Segments[nLayer].m_nPasses = 0;

		fTotalSegPasses += fSegPasses;
		UINT16 nThisSegPasses = (UINT16)NCSFloor(fTotalSegPasses - nPassesOutput);

		if(nLayer == nLayers - 1) {
			// Last layer, make sure we output all remaining passes!
			nThisSegPasses = (nBits - 1) * 3 + 1 - nPassesOutput;
		}
		if(nThisSegPasses >= 1) {//nLayer >= NCSFloor((nZeroBits * 3) / fSegPasses)) {
			
			for (int nSegPass = 0; sm_nBitPlane >= 0 && nSegPass < nThisSegPasses; passno++, nSegPass++) {
				sm_one = 1 << sm_nBitPlane;
				sm_half = sm_one >> 1;
				sm_oneplushalf = sm_one | sm_half;

				switch (passtype) {
					case 0:
							if(!bAllSig) {
								EncSigPass(nWidth, nHeight);
							}
						break;
					case 1:
							EncRefPass(nWidth, nHeight);
						break;
					case 2:
							if(!bAllSig) {
								EncClnPass(nWidth, nHeight);
								bAllSig = (nSigValues == nWidth * nHeight);
							}
						break;
				}
				if (++passtype==3) {
					passtype=0;
					sm_nBitPlane--;
				}
			}
		}
			if((nLayer == nLayers - 1) || (passno == (nBits - 1) * 3 + 1)) {
				CNCSJPCMQCoder::Flush();
			}
			//printf("%ld (%ld).\r\n", passno, 4096 + CNCSJPCMQCoder::sm_nIndex);
#ifdef DUMP_LUTS
			static int nCB = 0;
			FILE *pFile = fopen("C:\\temp\\blocks.csv", "a+");
			if(pFile) {
				fprintf(pFile, "Pass%ld", nCB);
				for(int i = 0; i < 1+32*3;i++) {
					fprintf(pFile, ",%ld", PassLUT[i]);
				}
				fprintf(pFile, "\n");
				fprintf(pFile, "Len%ld", nCB);
				for(int i = 0; i < 1+32*3;i++) {
					fprintf(pFile, ",%ld", Lengths[i]);
				}
				fprintf(pFile, "\n");
				fclose(pFile);
			}
			nCB++;
#endif
			Segments[nLayer].m_nLength = NCSMax(0, (T1_ENCODER_BUFSIZE + CNCSJPCMQCoder::sm_nIndex - 1) - (signed)nSegOffset);	
			if(Segments[nLayer].m_nLength || (passno - nPassesOutput)) {
				
				if((nLayer < nLayers - 1)) {
					if(m_pEncTmpBuf[nSegOffset + Segments[nLayer].m_nLength/*-1*/] == 0xff) {
						//Don't split escaped data 
						Segments[nLayer].m_nLength += 1;
					}
					if(CNCSJPCMQCoder::sm_nCT >= 5) {
						Segments[nLayer].m_nLength += 4;
					} else {
						Segments[nLayer].m_nLength += 5;
					} 
				}
				nSegOffset += Segments[nLayer].m_nLength;
				Segments[nLayer].m_nPasses = (UINT8)(passno - nPassesOutput);
				nPassesOutput = (UINT16)passno;
			}
	}
	nSegOffset= 0;
	for(nLayer = 0; nLayer < nLayers; nLayer++) {
	//	int nSegLen = Segments[nLayer].m_nLength;
		if(nLayer == nLayers - 1) {
			// Adjust final length to end of MQ coder buffer
			Segments[nLayer].m_nLength = (T1_ENCODER_BUFSIZE + CNCSJPCMQCoder::sm_nIndex - 1) - nSegOffset;
		}
		while(Segments[nLayer].m_nLength > 0 && ((m_pEncTmpBuf[nSegOffset + Segments[nLayer].m_nLength/*-1*/] == 0xff) || ((INT32)(nSegOffset + Segments[nLayer].m_nLength) > T1_ENCODER_BUFSIZE + CNCSJPCMQCoder::sm_nIndex - 1))) {
			Segments[nLayer].m_nLength--;
		}
		Segments[nLayer].m_pData = (UINT8*)NCSMalloc(Segments[nLayer].m_nLength, FALSE);
		memcpy(Segments[nLayer].m_pData, m_pEncTmpBuf+1 + nSegOffset, Segments[nLayer].m_nLength);
		nSegOffset += Segments[nLayer].m_nLength;
	}
//	fprintf(stdout, "len %ld\r\n", nSegOffset);
	return(true);
}

UINT8 CNCSJPCT1Coder::InitCtxNoZC(T1_FLAG_TYPE f, int orient) {
	int h,v,d,n,t,hv;
	n=0;
	h=((f&T1_SIG_H0)!=0)+((f&T1_SIG_H1)!=0);
	v=((f&T1_SIG_V0)!=0)+((f&T1_SIG_V1)!=0);
	d=((f&T1_SIG_D0)!=0)+((f&T1_SIG_D1)!=0)+((f&T1_SIG_D3)!=0)+((f&T1_SIG_D2)!=0);
	switch(orient) {
		case 2:t=h;h=v;v=t;
		case 0:
		case 1:
				if(!h){
					if(!v){
						if(!d){
							n=0;
						} else if(d==1){
							n=1;
						} else { 
							n=2;
						}
					} else if(v==1){
						n=3;
					} else {
						n=4;
					}
				}else if(h==1){
					if(!v){
						if(!d){
							n=5;
						}else{
							n=6;
						}
					} else{
						n=7;
					}
				}else{
					n=8;
				}
			break;
		case 3:
			hv=h+v;
			if(!d){
				if(!hv){
					n=0;
				}else if(hv==1){
					n=1;
				}else{
					n=2;
				}
			}else if(d==1){
				if(!hv){
					n=3;
				}else if(hv==1){ 
					n=4;
				}else{
					n=5;
				}
			}else if(d==2){
				if(!hv){
					n=6;
				}else{
					n=7;
				}
			}else{
				n=8;
			}
		break;
	}
	return (UINT8)(T1_CTXNO_ZC+n);
}

UINT8 CNCSJPCT1Coder::InitCtxNoSC(T1_FLAG_TYPE f) {
	int hc,vc,n;
	n=0;
	hc=NCSMin(((f&(T1_SIG_H1|T1_SGN_H1))==T1_SIG_H1)+((f&(T1_SIG_H0|T1_SGN_H0))==T1_SIG_H0),1)-NCSMin(((f&(T1_SIG_H1|T1_SGN_H1))==(T1_SIG_H1|T1_SGN_H1))+((f&(T1_SIG_H0|T1_SGN_H0))==(T1_SIG_H0|T1_SGN_H0)),1);
	vc=NCSMin(((f&(T1_SIG_V0|T1_SGN_V0))==T1_SIG_V0)+((f&(T1_SIG_V1|T1_SGN_V1))==T1_SIG_V1),1)-NCSMin(((f&(T1_SIG_V0|T1_SGN_V0))==(T1_SIG_V0|T1_SGN_V0))+((f&(T1_SIG_V1|T1_SGN_V1))==(T1_SIG_V1|T1_SGN_V1)),1);
	if(hc<0){
		hc=-hc;vc=-vc;
	}
	if(!hc){
		if(vc==-1){
			n=1;
		}else if(!vc){
			n=0;
		}else{
			n=1;
		}
	}else if(hc==1){
		if(vc==-1){
			n=2;
		}else if(!vc){
			n=3;
		}else{
			n=4;
		}
	}
	return (UINT8)(T1_CTXNO_SC+n);
}

INT32 CNCSJPCT1Coder::InitSPB(T1_FLAG_TYPE f) {
	int hc,vc,n;
	hc=NCSMin(((f&(T1_SIG_H1|T1_SGN_H1))==T1_SIG_H1)+((f&(T1_SIG_H0|T1_SGN_H0))==T1_SIG_H0),1)-NCSMin(((f&(T1_SIG_H1|T1_SGN_H1))==(T1_SIG_H1|T1_SGN_H1))+((f&(T1_SIG_H0|T1_SGN_H0))==(T1_SIG_H0|T1_SGN_H0)),1);
	vc=NCSMin(((f&(T1_SIG_V0|T1_SGN_V0))==T1_SIG_V0)+((f&(T1_SIG_V1|T1_SGN_V1))==T1_SIG_V1),1)-NCSMin(((f&(T1_SIG_V0|T1_SGN_V0))==(T1_SIG_V0|T1_SGN_V0))+((f&(T1_SIG_V1|T1_SGN_V1))==(T1_SIG_V1|T1_SGN_V1)),1);
	if(!hc&&!vc){
		n=0;
	}else{
		n=(!(hc>0||(!hc&&vc>0)));
	}
	return n;
}

//#if defined(_AMD64_) || (defined(_MSC_VER) && _MSC_VER >= 1400)
// Some weird optimiser bug in the AMD64/PSDKv1400 compilers - this isn't performance critical anyway
#pragma optimize( "", off )
//#endif 

void CNCSJPCT1Coder::InitLUTs()
{
	int i;

	for(i=0;i<256;++i) {
		m_lut_ctxno_zc0[i] = InitCtxNoZC(i,0);
	}
	for(i=0;i<256;++i) {
		m_lut_ctxno_zc1[i] = InitCtxNoZC(i,1);
	}
	for(i=0;i<256;++i) {
		m_lut_ctxno_zc2[i] = InitCtxNoZC(i,2);
	}
	for(i=0;i<256;++i) {
		m_lut_ctxno_zc3[i] = InitCtxNoZC(i,3);
	}
	for(i=0;i<256;i++) {
		m_lut_ctxno_sc[i] = InitCtxNoSC(i<<4);
	}
	for(i=0;i<256;++i) {
		m_lut_spb[i] = (UINT8)InitSPB(i<<4);
	}
}

//#if defined(_AMD64_) || (defined(_MSC_VER) && _MSC_VER >= 1400)
#pragma optimize( "", on )
//#endif
