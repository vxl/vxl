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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCMQCoder.cpp $
** CREATED:  21/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCMQCoder class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCMQCoder.h"
#include "NCSMisc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const CNCSJPCMQCoder::State CNCSJPCMQCoder::sm_States[47*2] = {
	{0x5601, 0, 2, 3},
	{0x5601, 1, 3, 2},
	{0x3401, 0, 4, 12},
	{0x3401, 1, 5, 13},
	{0x1801, 0, 6, 18},
	{0x1801, 1, 7, 19},
	{0x0ac1, 0, 8, 24},
	{0x0ac1, 1, 9, 25},
	{0x0521, 0, 10, 58},
	{0x0521, 1, 11, 59},
	{0x0221, 0, 76, 66},
	{0x0221, 1, 77, 67},
	{0x5601, 0, 14, 13},
	{0x5601, 1, 15, 12},
	{0x5401, 0, 16, 28},
	{0x5401, 1, 17, 29},
	{0x4801, 0, 18, 28},
	{0x4801, 1, 19, 29},
	{0x3801, 0, 20, 28},
	{0x3801, 1, 21, 29},
	{0x3001, 0, 22, 34},
	{0x3001, 1, 23, 35},
	{0x2401, 0, 24, 36},
	{0x2401, 1, 25, 37},
	{0x1c01, 0, 26, 40},
	{0x1c01, 1, 27, 41},
	{0x1601, 0, 58, 42},
	{0x1601, 1, 59, 43},
	{0x5601, 0, 30, 29},
	{0x5601, 1, 31, 28},
	{0x5401, 0, 32, 28},
	{0x5401, 1, 33, 29},
	{0x5101, 0, 34, 30},
	{0x5101, 1, 35, 31},
	{0x4801, 0, 36, 32},
	{0x4801, 1, 37, 33},
	{0x3801, 0, 38, 34},
	{0x3801, 1, 39, 35},
	{0x3401, 0, 40, 36},
	{0x3401, 1, 41, 37},
	{0x3001, 0, 42, 38},
	{0x3001, 1, 43, 39},
	{0x2801, 0, 44, 38},
	{0x2801, 1, 45, 39},
	{0x2401, 0, 46, 40},
	{0x2401, 1, 47, 41},
	{0x2201, 0, 48, 42},
	{0x2201, 1, 49, 43},
	{0x1c01, 0, 50, 44},
	{0x1c01, 1, 51, 45},
	{0x1801, 0, 52, 46},
	{0x1801, 1, 53, 47},
	{0x1601, 0, 54, 48},
	{0x1601, 1, 55, 49},
	{0x1401, 0, 56, 50},
	{0x1401, 1, 57, 51},
	{0x1201, 0, 58, 52},
	{0x1201, 1, 59, 53},
	{0x1101, 0, 60, 54},
	{0x1101, 1, 61, 55},
	{0x0ac1, 0, 62, 56},
	{0x0ac1, 1, 63, 57},
	{0x09c1, 0, 64, 58},
	{0x09c1, 1, 65, 59},
	{0x08a1, 0, 66, 60},
	{0x08a1, 1, 67, 61},
	{0x0521, 0, 68, 62},
	{0x0521, 1, 69, 63},
	{0x0441, 0, 70, 64},
	{0x0441, 1, 71, 65},
	{0x02a1, 0, 72, 66},
	{0x02a1, 1, 73, 67},
	{0x0221, 0, 74, 68},
	{0x0221, 1, 75, 69},
	{0x0141, 0, 76, 70},
	{0x0141, 1, 77, 71},
	{0x0111, 0, 78, 72},
	{0x0111, 1, 79, 73},
	{0x0085, 0, 80, 74},
	{0x0085, 1, 81, 75},
	{0x0049, 0, 82, 76},
	{0x0049, 1, 83, 77},
	{0x0025, 0, 84, 78},
	{0x0025, 1, 85, 79},
	{0x0015, 0, 86, 80},
	{0x0015, 1, 87, 81},
	{0x0009, 0, 88, 82},
	{0x0009, 1, 89, 83},
	{0x0005, 0, 90, 84},
	{0x0005, 1, 91, 85},
	{0x0001, 0, 90, 86},
	{0x0001, 1, 91, 87},
	{0x5601, 0, 92, 92},
	{0x5601, 1, 93, 93},
};

INT32 CNCSJPCMQCoder::sm_nIndex = 0;
UINT8 *CNCSJPCMQCoder::sm_pB = NULL;
UINT8 CNCSJPCMQCoder::sm_ShiftLut[4096];

CNCSJPCMQCoder::CHiLoUnion CNCSJPCMQCoder::sm_C;
#ifdef NCSJPC_USE_CDP_OPT
INT32 CNCSJPCMQCoder::sm_D;
#endif // NCSJPC_USE_CDP_OPT
UINT16 CNCSJPCMQCoder::sm_A;
UINT16 CNCSJPCMQCoder::sm_nCT;
CNCSJPCMQCoder::State CNCSJPCMQCoder::sm_Contexts[NCSJPC_MQC_NUMCTXS];
//UINT8 CNCSJPCMQCoder::sm_SymbolStream[16384];//FIXME
//UINT32 CNCSJPCMQCoder::sm_nSymbolStream;

CNCSJPCMQCoder::CNCSJPCMQCoder()
{
	ResetStates();

	for(int c = 0; c <= 8; c++) {
		for(UINT32 n = 0; n < 256; n++) {
			if(n & 0x80) {
				sm_ShiftLut[n | (c << 8)] = (UINT8)NCSMin(c, 0);
			} else if(n & 0x40) {
				sm_ShiftLut[n | (c << 8)] = (UINT8)NCSMin(c, 1);
			} else if(n & 0x20) {
				sm_ShiftLut[n | (c << 8)] = (UINT8)NCSMin(c, 2);
			} else if(n & 0x10) {
				sm_ShiftLut[n | (c << 8)] = (UINT8)NCSMin(c, 3);
			} else if(n & 0x08) {
				sm_ShiftLut[n | (c << 8)] = (UINT8)NCSMin(c, 4);
			} else if(n & 0x04) {
				sm_ShiftLut[n | (c << 8)] = (UINT8)NCSMin(c, 5);
			} else if(n & 0x02) {
				sm_ShiftLut[n | (c << 8)] = (UINT8)NCSMin(c, 6);
			} else if(n & 0x01) {
				sm_ShiftLut[n | (c << 8)] = (UINT8)NCSMin(c, 7);
			} else if(n == 0x00) {
				sm_ShiftLut[n | (c << 8)] = (UINT8)NCSMin(c, 8);
			}
		}
	}

}

CNCSJPCMQCoder::~CNCSJPCMQCoder()
{
}

void CNCSJPCMQCoder::InitDec(UINT8 *pData, UINT32 nLen, bool bBypass)
{
	sm_nIndex = -(INT32)nLen;
	sm_pB = pData + nLen;
	if(bBypass) {
		sm_nCT = 8;
		sm_C.m_CHiLo.m_CLow = sm_pB[sm_nIndex];
	} else {
		sm_C.m_C24 = sm_pB[sm_nIndex] << 16;
		ByteIn();
		sm_C.m_C24 <<= 7;
		sm_nCT -= 7;
		sm_A = 0x8000;
#ifdef NCSJPC_USE_CDP_OPT
		sm_D = NCSMin(sm_A - 0x8000, sm_C.m_CHiLo.m_CHigh);
		sm_A -= sm_D;
		sm_C.m_CHiLo.m_CHigh -= sm_D;
#endif //NCSJPC_USE_CDP_OPT
	}
}

void CNCSJPCMQCoder::InitEnc(UINT8 *pData, UINT32 nLen)
{
//	sm_nSymbolStream = 0;
	sm_A = 0x8000;
	sm_C.m_C24 = 0;
	sm_nIndex = -(INT32)nLen;
	sm_pB = pData + nLen;
sm_pB[sm_nIndex] = 0;
	sm_nCT = 12;
	if (sm_pB[sm_nIndex]==0xff) {
		sm_nCT=13;
	}
}

/// <summary>
/// Flush encoded data.
/// </summary>
void CNCSJPCMQCoder::Flush()
{
	SetBits();
	sm_C.m_C24<<=sm_nCT;
	ByteOut();
	sm_C.m_C24<<=sm_nCT;
	ByteOut();
	if (sm_pB[sm_nIndex]!=0xff) {
		sm_nIndex++;
	} else {
		//Discard B
		sm_pB[sm_nIndex] = 0;
	}
}

void CNCSJPCMQCoder::SetBits()
{
	unsigned int tempc=sm_C.m_C24 + sm_A;
	sm_C.m_C24 |= 0xffff;
	if (sm_C.m_C24 >= tempc) {
		sm_C.m_C24 -= 0x8000;
	}
}
void CNCSJPCMQCoder::ResetStates() 
{
	for(int i = 0; i < NCSJPC_MQC_NUMCTXS; i++) {
		sm_Contexts[i] = sm_States[0];
	}
}

void CNCSJPCMQCoder::SetState(UINT8 nCtx, int msb, int prob) {
	sm_Contexts[nCtx] = sm_States[msb + (prob << 1)];
}

//
// ByteIn() routine.
//
// Note: Always guaranteed data will terminate with 0xffff by the packet parser,
// this eliminates any need to test for the end condition on the pointer or length.
// Note: Inlining this tends to run slower as we start blowing the instruction cache.
//
void CNCSJPCMQCoder::ByteIn()
{
	UINT32 c = sm_pB[sm_nIndex + 1];

	if (sm_pB[sm_nIndex] != 0xff) {
		sm_nIndex++;
		sm_C.m_CHiLo.m_CLow = (UINT8)c;
		sm_nCT = 8;
	} else {
		if (c <= 0x8f) {
			sm_nIndex++;
			sm_C.m_C24 += c << 9;
			sm_nCT = 7;
		} else {
			sm_C.m_CHiLo.m_CLow = 0xff;
			sm_nCT = 8;
		}
	}
}

/// <summary>
/// Output a byte, doing bit-stuffing if necessary.
/// After a 0xff byte, the next byte must be smaller than 0x90
/// </summary>
void CNCSJPCMQCoder::ByteOut()
{
	if (sm_pB[sm_nIndex]==0xff) {
		sm_nIndex++;
		sm_pB[sm_nIndex] = (UINT8)(sm_C.m_C24 >> 20);
		sm_C.m_C24 &= 0xfffff;
		sm_nCT = 7;
	} else {
		if ((sm_C.m_C24 & 0x8000000)==0) {
			sm_nIndex++;
			sm_pB[sm_nIndex] = (UINT8)(sm_C.m_C24 >> 19);
			sm_C.m_C24 &= 0x7ffff;
			sm_nCT = 8;
		} else {
			sm_pB[sm_nIndex]++;
			if (sm_pB[sm_nIndex]==0xff) {
				sm_C.m_C24 &= 0x7ffffff;
				sm_nIndex++;
				sm_pB[sm_nIndex] = (UINT8)(sm_C.m_C24 >> 20);
				sm_C.m_C24 &= 0xfffff;
				sm_nCT = 7;
			} else {
				sm_nIndex++;
				sm_pB[sm_nIndex] = (UINT8)(sm_C.m_C24 >> 19);
				sm_C.m_C24 &= 0x7ffff;
				sm_nCT = 8;
			}
		}
	}
}

INT32 CNCSJPCMQCoder::GetBit()
{
	INT32 d;
	if(sm_nCT == 0) {
		UINT32 c = sm_pB[sm_nIndex + 1];

		if (sm_pB[sm_nIndex] != 0xff) {
			sm_nIndex++;
			sm_C.m_CHiLo.m_CLow = (UINT8)c;
			sm_nCT = 8;
		} else {
			if(sm_nIndex < 0) {
				sm_nIndex++;
				sm_C.m_C24 += c << 9;
				sm_nCT = 7;
			} else {
				sm_C.m_CHiLo.m_CLow = 0xff;
				sm_nCT = 8;
			}
		}
	}
	d = sm_C.m_CHiLo.m_CLow >> 7;
	sm_C.m_C24 <<= 1;
	sm_nCT--;
	return(d);
}

//
// Renormd() function.
// This uses a shift lookup table optimisation, in the case where
// sm_A needs to be shifted more than one pixel.
//
void CNCSJPCMQCoder::RenormDec()
{
	do {
		if(sm_nCT == 0) {
			ByteIn();
		}
		if(sm_A & 0x4000) {
			// Single bit shift is most common case
			sm_A <<= 1;
			sm_C.m_C24 <<= 1;
			sm_nCT -= 1;
			break;
		} else {
			UINT32 n = sm_ShiftLut[(sm_A  >> 8) | (sm_nCT << 8)];
			sm_A <<= n;
			sm_C.m_C24 <<= n;
			sm_nCT = sm_nCT - (UINT16)n;
		}
	} while(sm_A<0x8000);
}

void CNCSJPCMQCoder::RenormEnc()
{
	do {
		sm_A <<= 1;
		sm_C.m_C24 <<= 1;
		sm_nCT--;
		if(sm_nCT == 0) {
			ByteOut();
		}
	} while(sm_A<0x8000);
}
