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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCCodeBlock.cpp $
** CREATED:  21/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCCodeBlock class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPC.h"
#include "NCSJPCCodeBlock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NCSTimeStampUs CNCSJPCCodeBlock::sm_usTotal;
NCSTimeStampUs CNCSJPCCodeBlock::sm_usLast;
UINT64 CNCSJPCCodeBlock::sm_nTotalSamples;
UINT64 CNCSJPCCodeBlock::sm_nLastSamples;
CNCSJPCNode::CNCSJPCNodeTracker CNCSJPCCodeBlock::sm_Tracker("CNCSJPCCodeBlock", sizeof(CNCSJPCCodeBlock));

CNCSJPCT1Coder CNCSJPCCodeBlock::sm_Coder;

CNCSJPCCodeBlock::CNCSJPCCodeBlock() 
{ 
	m_nZeroBits = 0;
	sm_Tracker.Add(); 
}

CNCSJPCCodeBlock::CNCSJPCCodeBlock(const CNCSJPCCodeBlock &s)
{
	sm_Tracker.Add();
	*this = s;
}

CNCSJPCCodeBlock::CNCSJPCCodeBlock(CNCSJPCSubBand *pSubBand, UINT32 nCB) 
{  
	sm_Tracker.Add(); 
	Init(pSubBand, nCB); 
}

	/** Virtual destructor */
CNCSJPCCodeBlock::~CNCSJPCCodeBlock()
{
	for(UINT32 s = 0; s < m_Segments.size(); s++) {
		if(m_Segments[s].m_pData) {
			sm_Tracker.RemoveMem(m_Segments[s].m_nLength);
			NCSFree(m_Segments[s].m_pData);
			m_Segments[s].m_pData = NULL;
		}
	}
	sm_Tracker.Remove();
}

void CNCSJPCCodeBlock::Init(CNCSJPCSubBand *pSubBand, UINT32 nCB)
{
	m_pSubBand = pSubBand;
	m_nCB = nCB;
	m_nLblock = 3;
	m_nLayerIncluded = 0xffffffff;
	m_nPasses = 0;
	m_nZeroBits = 0;

	INT32 nCBWidth = pSubBand->GetCBWidth();
	INT32 x = m_nCB % pSubBand->GetNumCBWide();	
	INT32 nCBHeight = pSubBand->GetCBHeight();
	INT32 y = m_nCB / pSubBand->GetNumCBWide();		
	INT32 nSBX0 = pSubBand->GetX0();
	INT32 nSBY0 = pSubBand->GetY0();

	m_X0 = NCSMax(nCBWidth * (nSBX0 / nCBWidth) + x * nCBWidth, nSBX0);
	m_X1 = NCSMin(nCBWidth * (nSBX0 / nCBWidth) + (x + 1) * nCBWidth, pSubBand->GetX1());
	m_Y0 = NCSMax(nCBHeight * (nSBY0 / nCBHeight) + y * nCBHeight, nSBY0);
	m_Y1 = NCSMin(nCBHeight * (nSBY0 / nCBHeight) + (y + 1) * nCBHeight, pSubBand->GetY1());

	m_bZeroSize = ((m_X0.m_Value < m_X1.m_Value) && (m_Y0.m_Value < m_Y1.m_Value)) ? false : true;
}

// Read a line from the codeblock.
bool CNCSJPCCodeBlock::ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent)
{
	nCtx;iComponent;//Keep compiler happy
#ifdef NCSJPC_PROFILE
	NCSTimeStampUs tsBegin = CNCSJPCBuffer::SystemInfo::GetTime();
#endif
	bool bRet = false;
	
	if(m_nPasses) {
		if(!m_DecBuf.GetPtr()) {
			Decode(pDst->GetType());	
		} 
	}
	if(m_DecBuf.GetPtr()) {
		bRet = m_DecBuf.Read(pDst);
	} else {
		pDst->Clear();
		bRet = true;
	}
#ifdef NCSJPC_PROFILE
	sm_usLast = (CNCSJPCBuffer::SystemInfo::GetTime() - tsBegin);
	sm_usTotal += sm_usLast;
#endif
	sm_nLastSamples = pDst->GetWidth();
	sm_nTotalSamples += sm_nLastSamples;
	return(bRet);
}

// Write a BufferType line to the output.
bool CNCSJPCCodeBlock::WriteLine(ContextID nCtx, 
								 CNCSJPCBuffer *pSrc, 
								 UINT16 iComponent)
{
	nCtx;iComponent;//Keep compiler happy
	bool bRet = true;
	CNCSJPCResolution *pResolution = m_pSubBand->m_pPrecinct->m_pResolution;
	CNCSJPCBuffer tmp;
	tmp.Assign(0, 0, 
			   GetWidth(), 
			   GetHeight(), 
			   pSrc->GetType(), 
			   m_pSubBand->GetPtr(GetX0(), GetY0()), 
			   m_pSubBand->GetStep());
		// Last scanline, encode block
	CNCSJPCComponent *pComponent = pResolution->m_pComponent;

	bRet &= sm_Coder.Encode(m_pSubBand->m_eType, 
							pComponent->m_QuantizationStyle.m_Sqc.m_nGuardBits + pComponent->m_QuantizationStyle.m_SPqc[m_pSubBand->m_eType].m_nReversible - 1, 
							m_nZeroBits, 
							m_Segments, 
							&tmp,
							pResolution->m_nCompressionRatio,
							pResolution->m_pComponent->m_CodingStyle.m_SGcod.m_nLayers);
	UINT32 nSegs = (UINT32)m_Segments.size();
	for(UINT32 s = 0; s < nSegs; s++) {
		sm_Tracker.AddMem(m_Segments[s].m_nLength);
	}
	//printf("%ld %ld (%ld)\r\n", m_Segments[0].m_nLength, m_Segments[0].m_nPasses, m_nZeroBits);
//	if(m_nZeroBits > 16) {
//		printf("HERE");
//	}
//	char buf[1024];
//	sprintf(buf, "CB %ld src %ld sb %ld\r\n", GetY0(), pSrc->GetY0(), m_pSubBand->GetY0());
//	OutputDebugStringA(buf);
	return(bRet);
}

// Decode the codeblock.
CNCSError CNCSJPCCodeBlock::Decode(Type eType)
{
	if(!m_pSubBand) {
		return(NCS_INVALID_PARAMETER);
	}
	if(!m_pSubBand->m_pPrecinct) {
		return(NCS_INVALID_PARAMETER);
	}
	if(!m_pSubBand->m_pPrecinct->m_pResolution) {
		return(NCS_INVALID_PARAMETER);
	}
	if(!m_pSubBand->m_pPrecinct->m_pResolution->m_pComponent) {
		return(NCS_INVALID_PARAMETER);
	}
	CNCSJPCComponent *pComponent = m_pSubBand->m_pPrecinct->m_pResolution->m_pComponent;

	int Mb = 0;

	if(pComponent->m_CodingStyle.m_SPcod.m_eTransformation == CNCSJPCCodingStyleParameter::REVERSIBLE_5x3) {
		Mb = pComponent->m_QuantizationStyle.m_Sqc.m_nGuardBits + pComponent->m_QuantizationStyle.m_SPqc[m_pSubBand->m_eType].m_nReversible - 1;
	} else {
		if(pComponent->m_QuantizationStyle.m_Sqc.m_eStyle == CNCSJPCQuantizationParameter::SCALAR_DERIVED) {
			Mb = pComponent->m_QuantizationStyle.m_Sqc.m_nGuardBits + pComponent->m_QuantizationStyle.m_SPqc[NCSJPC_LL].m_Irreversible.m_nExponent - 1;
		} else {
	//		INT32 i = 1 + (m_pSubBand->m_eType - 1) + 3 * (m_pSubBand->m_pPrecinct->m_pResolution->m_nResolution - 1);
			Mb = pComponent->m_QuantizationStyle.m_Sqc.m_nGuardBits + pComponent->m_QuantizationStyle.m_SPqc[/*i*/m_pSubBand->m_eType].m_Irreversible.m_nExponent - 1;
		}
	}
/*	if(m_nCB == 0 && m_pSubBand->m_pPrecinct->m_nPrecinct == 0 && m_pSubBand->m_pPrecinct->m_pResolution->m_nResolution == 5 && m_pSubBand->m_pPrecinct->m_pResolution->m_pComponent->m_iComponent == 0) {
		for(int s = 0; s < m_Segments.size(); s++) {
			for(int i = 0; i < m_Segments[s].m_nLength; i++) {
				printf("%lx", 	m_Segments[s].m_pData[i]);
			}
			printf("\r\n\r\n");
		}
		printf("\r\n");
		fgetc(stdin);
	}
*/
	if(m_DecBuf.Alloc(0, 0, GetWidth(), GetHeight(), eType)) {
		UINT8 roiShift = 0;
		for(UINT32 r = 0; r < pComponent->m_pTilePart->m_Regions.size(); r++) {
			if(pComponent->m_pTilePart->m_Regions[r].m_nCrgn == pComponent->m_iComponent) {
				roiShift = pComponent->m_pTilePart->m_Regions[r].m_nSPrgn;
				break;
			}
		}
		if(sm_Coder.Decode(m_pSubBand->m_eType,
							roiShift, 
							(UINT8)Mb,
							m_nZeroBits,
							m_Segments,
							&m_DecBuf,
							pComponent->m_CodingStyle.m_SPcod.m_Scb,
							pComponent->m_QuantizationStyle,
							pComponent->m_pTilePart->m_pJPC->m_SIZ.m_Components[pComponent->m_iComponent].m_nBits,
							pComponent->m_CodingStyle.m_SPcod.m_nLevels + m_pSubBand->m_pPrecinct->m_pResolution->m_nResolution,
							m_pSubBand->m_pPrecinct->m_pResolution->m_nResolution,
							pComponent->m_pTilePart->m_pJPC->m_fReconstructionParameter) == false) {
			return(NCS_ECW_ERROR);
		}
	}
	return(NCS_SUCCESS);
}

INT32 CNCSJPCCodeBlock::ReadNewSegs(CNCSJPCIOStream &Stream)
{
	INT32 nRead = 0;
	UINT32 nNewSegs = (UINT32)m_NextSegments.size();
	if(nNewSegs) {
		for(UINT32 i = 0; i < nNewSegs; i++) {
			UINT32 iSeg = m_NextSegments[i].m_nIndex;

			if(m_Segments.size() < iSeg + 1) {
				m_Segments.resize(iSeg + 1);
			}
			CNCSJPCSegment &Seg = m_Segments[iSeg];
			CNCSJPCSegment &NextSeg = m_NextSegments[i];

				// To make the MQ Decoder faster, we always append 0xffff on the end here.
				// This saves a lot of special case handling later inside some very
				// tight loops...
			Seg.m_nIndex = (UINT16)iSeg;
			Seg.m_pData = (UINT8*)NCSRealloc(Seg.m_pData, Seg.m_nLength + NextSeg.m_nLength + 2, FALSE);
			if(Stream.Read(Seg.m_pData + Seg.m_nLength, NextSeg.m_nLength) == false) {
				// Read error;
				Seg.m_pData[Seg.m_nLength] = 0xff;
				Seg.m_pData[Seg.m_nLength + 1] = 0xff;
				nRead = -1;
				break;
			}
			Seg.m_nLength += NextSeg.m_nLength;
			Seg.m_pData[Seg.m_nLength] = 0xff;
			Seg.m_pData[Seg.m_nLength + 1] = 0xff;
			Seg.m_nPasses = Seg.m_nPasses + NextSeg.m_nPasses;

			nRead += NextSeg.m_nLength;

			sm_Tracker.AddMem(NextSeg.m_nLength);
		}
		m_NextSegments.clear();
		// Force T1 re-decode
		m_DecBuf.Free();
	}
	return(nRead);
}

INT32 CNCSJPCCodeBlock::WriteNewSegs(CNCSJPCIOStream &Stream)
{
	INT32 nWritten = 0;
	UINT32 nNewSegs = (UINT32)m_NextSegments.size();
	if(nNewSegs) {
		for(UINT32 i = 0; i < nNewSegs; i++) {
			CNCSJPCSegment &Seg = m_NextSegments[i];

			if(Stream.Write(Seg.m_pData, Seg.m_nLength) == false) {
				// Read error;
				nWritten = -1;
				break;
			}
			nWritten += Seg.m_nLength;

			sm_Tracker.RemoveMem(Seg.m_nLength);
		}
		m_NextSegments.clear();
	}
	return(nWritten);
}
