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
** FILE:     $Archive: /NCS/Source/include/NCSJPCT1Coder.h $
** CREATED:  12/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCT1Coder class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCT1CODER_H
#define NCSJPCT1CODER_H

#include "NCSJPCTypes.h"
#ifndef NCSJPCNODE_H
#include "NCSJPCNode.h"
#endif // NCSJPCNODE_H
#ifndef NCSJPCMQCODER_H
#include "NCSJPCMQCoder.h"
#endif // NCSJPCMQCODER_H
#include "NCSJPCQCDMarker.h"
#include "NCSJPCMemoryIOStream.h"
#ifndef NCSJPCSEGMENT_H
#include "NCSJPCSegment.h"
#endif // NCSJPCSEGMENT_H

#define NCSJPC_ENCODE_BLOCK_SIZE 64
	/**
	 * CNCSJPCT1Coder class - the JPC T1 Coder.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.20 $ $Author: simon $ $Date: 2005/01/17 09:11:24 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCT1Coder: public CNCSJPCMQCoder {
public:
	typedef UINT32 T1_FLAG_TYPE;
#define NCSJPC_T1_FLAG_BUFTYPE	CNCSJPCBuffer::BT_INT32
	
	const static int PREDICTABLE_TERMINATION;
	const static int PASS_RESET_CTX;
	const static int SEGMENT_SYMBOLS;
	const static int PASS_TERMINATION;
	const static int VERTICAL_CAUSAL_CTX;
	const static int SELECTIVE_CODING_BYPASS;

	CNCSJPCT1Coder();
	virtual ~CNCSJPCT1Coder();

	bool Decode(NCSJPCSubBandType eSBType,
				UINT8 roiShift, 
				UINT8 nBits,
				UINT8 nZeroBits,
				std::vector<CNCSJPCSegment> &m_Segments,
				CNCSJPCBuffer *pDecBuf,
				int Flags,
				CNCSJPCQCDMarker &Quantization,
				UINT8 nComponentBits,
				UINT16 nLevels,
				UINT8 nResolution,
				IEEE4 fReconstructionParameter);
	bool Encode(NCSJPCSubBandType eSBType, 
				UINT8 nBitsTotal,
				UINT8 &nZeroBits,
				std::vector<CNCSJPCSegment> &Segments,
				CNCSJPCBuffer *pDecBuf,
				UINT16 nRatio,
				UINT16 nLayers);
	static NCSTimeStampUs sm_usTotal;
	static NCSTimeStampUs sm_usLast;
	static UINT64 sm_nTotalSamples;
	static UINT64 sm_nLastSamples;

private:
	static UINT8 m_lut_ctxno_zc0[256];
	static UINT8 m_lut_ctxno_zc1[256];
	static UINT8 m_lut_ctxno_zc2[256];
	static UINT8 m_lut_ctxno_zc3[256];
	static UINT8 *m_plut_ctxno_zc;
	static UINT8 m_lut_ctxno_sc[256];
	static UINT8 m_lut_spb[256];

	UINT8 *m_pEncTmpBuf;
	static INT8 sm_nBitPlane;
	static UINT8 sm_Scb;
	static INT32 sm_one;
	static INT32 sm_half;
	static INT32 sm_oneplushalf;
	static INT32 sm_i;
	static void DecSigPass(int w, int h);
	static void DecSigPassStep(T1_FLAG_TYPE *fp, int *dp);
	static void DecRefPass(int w, int h);
	static void DecRefPassBit0(int w, int h);
	static void DecRefPassStep(T1_FLAG_TYPE *fp, int *dp);
	static void DecRefPassStepBit0(T1_FLAG_TYPE *fp);
	static void DecClnPass(int w, int h, bool segsym);
	static void DecClnPassStep(T1_FLAG_TYPE *fp, int *dp);
	static void DecClnPassStepPartial(T1_FLAG_TYPE *fp, int *dp);

	static void EncSigPassStep(T1_FLAG_TYPE *fp, int *dp);
	static void EncSigPass(int w, int h);
	static void EncRefPassStep(T1_FLAG_TYPE *fp, int *dp) ;
	static void EncRefPassStepSIG(T1_FLAG_TYPE *fp, int *dp) ;
	static void EncRefPass(int w, int h);
	static void EncClnPassStep(T1_FLAG_TYPE *fp, int *dp);
	static void EncClnPassStepPartial(T1_FLAG_TYPE *fp, int *dp);
	static void EncClnPass(int w, int h);

	static UINT8 GetCtxNo_ZC(T1_FLAG_TYPE f);
	static UINT8 GetCtxNo_SC(T1_FLAG_TYPE f);
	static INT32 GetSPB(T1_FLAG_TYPE f);
	static UINT8 GetCtxNoMag(T1_FLAG_TYPE f);
	static void UpdateFlags(T1_FLAG_TYPE *fp, int s);
	static void EncUpdateFlags(T1_FLAG_TYPE *fp, int s);

	UINT8 InitCtxNoZC(T1_FLAG_TYPE f, int orient);
	UINT8 InitCtxNoSC(T1_FLAG_TYPE f);
	INT32 InitSPB(T1_FLAG_TYPE f);
	void InitLUTs();

	static UINT32 sm_EncData[(NCSJPC_ENCODE_BLOCK_SIZE) * 4 * (1 + NCSJPC_ENCODE_BLOCK_SIZE / 4)];
	static T1_FLAG_TYPE sm_EncFlags[(NCSJPC_ENCODE_BLOCK_SIZE+2) * (NCSJPC_ENCODE_BLOCK_SIZE+2)];

	static CNCSJPCBuffer sm_Data;
	static CNCSJPCBuffer sm_Flags;
	static CNCSJPCMemoryIOStream sm_Stream;
	static bool GetBit();
};

#endif // !NCSJPCT1CODER_H
