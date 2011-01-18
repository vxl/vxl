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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCResolution.cpp $
** CREATED:  12/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCResolution class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

//
// 5X3 Filter lifting
//
//Step2     1   3   5   7     
//           \ /|\ /|\ /|\
//Step1       0 | 2 | 4 | 6            
//           /|\|/|\|/|\|/|\
//Index:  2 1 0 1 2 3 4 5 6 7
//
//Step2 == Odd Output
//Step1 == Even Output
//Step0-1 == 2State.
//
// 9X7 Filter lifting
//
//Step6   3   7   11         
//         \ /|\ /|\         
//Step5     2 | 6 | 10          
//           \|/|\|/|\       
//Step4       1 | 5 | 9         
//             \|/|\|/|\   
//Step3         0 | 4 | 8       
//             /|\|/|\|/|\          
//Index:    4 3 2 1 0 1 2 3 4 5 6 7
//
// Step6 == Odd output
// Step5 == Even Output
// Step2-5 == 4State.

#include "NCSJPCResolution.h"
#include "NCSJPCComponent.h"
#include "NCSJPCTilePartHeader.h"
#include "NCSJPC.h"
#include "NCSJPCCodeBlock.h"
#include "NCSUtil.h"
#include "NCSJPCFileIOStream.h"

CNCSJPCNode::CNCSJPCNodeTracker CNCSJPCResolution::sm_Tracker("CNCSJPCResolution", sizeof(CNCSJPCResolution));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define LIFT_9X7_K	1.230174104914001f
// Don't ask where this comes from, the spec says 1/K!
#define LIFT_9X7_1K 1.62578613134411f
#define LIFT_9X7_g	0.443506852043971f
#define LIFT_9X7_y	0.882911075530934f
#define LIFT_9X7_B	-0.052980118572961f
#define LIFT_9X7_a	-1.586134342059924f


CNCSJPCResolution::Context::Context()
{
	m_nStateU0 = 0;
	m_nStateU1 = 0;
	m_nStateV = 0;
}

CNCSJPCResolution::Context::~Context()
{
}

	/** Default constructor, initialises members */
CNCSJPCResolution::CNCSJPCResolution(CNCSJPCComponent *pComponent, UINT8 nResolution)
{
	sm_Tracker.Add();
	m_pComponent = pComponent;
	m_nResolution = nResolution;
	m_nCompressionRatio = 1;
	m_nEncoderPLTStartIndex = 0;
	m_Precincts.Init(this);
//	m_Precincts.resize(GetNumPrecinctsWide() * GetNumPrecinctsHigh());
}

	/** Virtual destructor */
CNCSJPCResolution::~CNCSJPCResolution()
{
//	std::map<UINT32, CNCSJPCPrecinct *>::iterator pCur = m_Precincts.begin();
//	std::map<UINT32, CNCSJPCPrecinct *>::iterator pEnd = m_Precincts.end();

//	while(pCur != pEnd) {
//		CNCSJPCPrecinct *pP = pCur->second;
//		pCur = m_Precincts.erase(pCur);
//		delete pP;
//	}
//	while(m_Precincts.size() > 0) {
//		CNCSJPCPrecinct *pP = m_Precincts[m_Precincts.size() - 1];
//		m_Precincts.pop_back();
//		delete pP;
//	}
//	int l;
//	for(l = 0; l < m_EncoderPLTFiles.size(); l++) {
//		CloseEncoderLayerPLTFile(l, true);
//	}
//	for(l = 0; l < m_EncoderFiles.size(); l++) {
//		CloseEncoderLayerFile(l, true);
//	}
	sm_Tracker.Remove();
}

// Get X0 of this node.
INT32 CNCSJPCResolution::GetX0()
{
	if(!m_X0.Cached()) {
		m_X0 = NCSCeilDiv(m_pComponent->GetX0(), NCS2Pow(m_pComponent->m_CodingStyle.m_SPcod.m_nLevels - m_nResolution));
	}
	return(m_X0);
}

// Get Y0 of this node.
INT32 CNCSJPCResolution::GetY0()
{
	if(!m_Y0.Cached()) {
		m_Y0 = NCSCeilDiv(m_pComponent->GetY0(), NCS2Pow(m_pComponent->m_CodingStyle.m_SPcod.m_nLevels - m_nResolution));
	}
	return(m_Y0);
}

// Get X1 of this node.
INT32 CNCSJPCResolution::GetX1()
{
	if(!m_X1.Cached()) {
		m_X1 = NCSCeilDiv(m_pComponent->GetX1(), NCS2Pow(m_pComponent->m_CodingStyle.m_SPcod.m_nLevels - m_nResolution));
	}
	return(m_X1);
}

// Get Y1 of this node.
INT32 CNCSJPCResolution::GetY1()
{
	if(!m_Y1.Cached()) {
		m_Y1 = NCSCeilDiv(m_pComponent->GetY1(), NCS2Pow(m_pComponent->m_CodingStyle.m_SPcod.m_nLevels - m_nResolution));
	}
	return(m_Y1);
}

// Get Precinct Width.
UINT32 CNCSJPCResolution::GetPrecinctWidth()
{
	if(!m_PrecinctWidth.Cached()) {
		m_PrecinctWidth = NCS2Pow(m_pComponent->m_pTilePart->GetPPx(m_pComponent->m_iComponent, m_nResolution));
	}
	return(m_PrecinctWidth);
}

// Get Precinct Height.
UINT32 CNCSJPCResolution::GetPrecinctHeight()
{
	if(!m_PrecinctHeight.Cached()) {
		m_PrecinctHeight = NCS2Pow(m_pComponent->m_pTilePart->GetPPy(m_pComponent->m_iComponent, m_nResolution));
	}
	return(m_PrecinctHeight);
}

// Get number of precincts wide.
UINT32 CNCSJPCResolution::GetNumPrecinctsWide()
{
	if(!m_NumPrecinctsWide.Cached()) {
		if(GetWidth() == 0) {
			m_NumPrecinctsWide = 0;
		} else {
			INT32 n = GetPrecinctWidth();
			m_NumPrecinctsWide = (INT32)(NCSCeilDiv(GetX1(), n) - NCSFloorDiv(GetX0(), n));
		}
	}
	return(m_NumPrecinctsWide);
}

// Get number of precincts high.
UINT32 CNCSJPCResolution::GetNumPrecinctsHigh()
{
	if(!m_NumPrecinctsHigh.Cached()) {
		if(GetHeight() == 0) {
			m_NumPrecinctsHigh = 0;
		} else {
			INT32 n = GetPrecinctHeight();
			m_NumPrecinctsHigh = (INT32)(NCSCeilDiv(GetY1(), n) - NCSFloorDiv(GetY0(), n));
		}
	}
	return(m_NumPrecinctsHigh);
}

// Get cb Width.
UINT32 CNCSJPCResolution::GetCBWidth()
{
	if(!m_CBWidth.Cached()) {
		m_CBWidth = m_pComponent->m_pTilePart->GetCBWidth(m_pComponent->m_iComponent, m_nResolution);
	}
	return(m_CBWidth);
}

// Get cb Height.
UINT32 CNCSJPCResolution::GetCBHeight()
{
	if(!m_CBHeight.Cached()) {
		m_CBHeight = m_pComponent->m_pTilePart->GetCBHeight(m_pComponent->m_iComponent, m_nResolution);
	}
	return(m_CBHeight);
}

// Get CPRL/PCRL/RPCL Progression resolution multiplier.
INT32 CNCSJPCResolution::GetResMultiplier()
{
	if(!m_ResMultiplier.Cached()) {
		m_ResMultiplier = NCS2Pow(m_pComponent->m_CodingStyle.m_SPcod.m_nLevels - m_nResolution);
	}
	return(m_ResMultiplier);
}

// Get CPRL/PCRL/RPCL Progression XRsiz multiplier.
INT32 CNCSJPCResolution::GetXRsizResMultiplier()
{
	if(!m_XRsizResMultiplier.Cached()) {
		m_XRsizResMultiplier = m_pComponent->m_pTilePart->m_pJPC->m_SIZ.m_Components[m_pComponent->m_iComponent].m_nXRsiz * GetResMultiplier();
	}
	return(m_XRsizResMultiplier);
}

// Get CPRL/PCRL/RPCL Progression YRsiz multiplier.
INT32 CNCSJPCResolution::GetYRsizResMultiplier()
{
	if(!m_YRsizResMultiplier.Cached()) {
		m_YRsizResMultiplier = m_pComponent->m_pTilePart->m_pJPC->m_SIZ.m_Components[m_pComponent->m_iComponent].m_nYRsiz * GetResMultiplier();
	}
	return(m_YRsizResMultiplier);
}

// Get CPRL/PCRL/RPCL Progression DivX factor.
INT32 CNCSJPCResolution::GetDivX()
{
	if(!m_DivX.Cached()) {
		m_DivX = GetXRsizResMultiplier() * GetPrecinctWidth();
	}
	return(m_DivX);
}

// Get CPRL/PCRL/RPCL Progression DivY factor.
INT32 CNCSJPCResolution::GetDivY()
{
	if(!m_DivY.Cached()) {
		m_DivY = GetYRsizResMultiplier() * GetPrecinctHeight();
	}
	return(m_DivY);
}

// Get CPRL/PCRL/RPCL Progression X origin check OK test result.
bool CNCSJPCResolution::XOriginCheckOk()
{
	if(!m_XOriginCheckOk.Cached()) {
		m_XOriginCheckOk = ((GetX0() * GetResMultiplier()) % (GetPrecinctWidth() * GetResMultiplier()) != 0);
	}
	return(m_XOriginCheckOk);
}

// Get CPRL/PCRL/RPCL Progression Y origin check OK test result.
bool CNCSJPCResolution::YOriginCheckOk()
{
	if(!m_YOriginCheckOk.Cached()) {
		m_YOriginCheckOk = ((GetY0() * GetResMultiplier()) % (GetPrecinctHeight() * GetResMultiplier()) != 0);
	}
	return(m_YOriginCheckOk);
}

// Get PX0 of this node.
INT32 CNCSJPCResolution::GetPX0()
{
	if(!m_PX0.Cached()) {
		m_PX0 = CNCSJPCPrecinct::GetX0(this, 0, 0);
	}
	return(m_PX0);
}

// Get PY0 of this node.
INT32 CNCSJPCResolution::GetPY0()
{
	if(!m_PY0.Cached()) {
		m_PY0 = CNCSJPCPrecinct::GetY0(this, 0, 0);
	}
	return(m_PY0);
}

// Read a line from the Resolution.
bool CNCSJPCResolution::ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent)
{
	bool bRet = false;
	Context *pCtx = (CNCSJPCResolution::Context*)GetContext(nCtx);

	if(m_nResolution == 0) {
//		bRet = ReadSubBandLine(nCtx, pDst, NCSJPC_LL);
		CNCSJPCBuffer tmp;
		tmp.Assign(pDst->GetX0() + GetX0(), 
				   pDst->GetY0() + GetY0(), 
				   pDst->GetX1() + GetX0(), 
				   pDst->GetY1() + GetY0(), 
				   pDst->GetType(), 
				   pDst->GetPtr());
		bRet = ReadSubBandLine(nCtx, &tmp, NCSJPC_LL);
		tmp.Release();
	} else {
		CNCSJPCBuffer tmp;
		tmp.Assign(pDst->GetX0() + GetX0(), 
				   pDst->GetY0() + GetY0(), 
				   pDst->GetX1() + GetX0(), 
				   pDst->GetY1() + GetY0(), 
				   pDst->GetType(), 
				   pDst->GetPtr());
		if(pDst->TestFlags(CNCSJPCBuffer::AT_LOCKED)) {
			tmp.SetFlag(CNCSJPCBuffer::AT_LOCKED);
		}
		bRet = SR_2D(nCtx, pDst->GetX0() + GetX0(), pDst->GetX1() + GetX0(), pDst->GetY0() + GetY0(), pDst->GetY1() + GetY0(), tmp);
		if(tmp.GetPtr() != pDst->GetPtr()) {
			if(pDst->TestFlags(CNCSJPCBuffer::AT_LOCKED) == false) {
				pDst->Assign(pDst->GetType(),
							tmp.GetPtr(),
							tmp.GetStep(),
							false);
			}
		}
		tmp.Release();
	}
	return(bRet);
}

// Write a line to the Resolution.
bool CNCSJPCResolution::WriteLine(ContextID nCtx, CNCSJPCBuffer *pSrc, UINT16 iComponent)
{
	bool bRet = false;
	Context *pCtx = (CNCSJPCResolution::Context*)GetContext(nCtx);

	if(m_nResolution == 0) {
		if(m_EncoderLL.GetPtr() == NULL) {
			// Create the subband buffers, these cover a full CB height row
			m_EncoderLL.Alloc(GetX0(), GetY0(),
							  GetWidth(), (UINT32)GetCBHeight(), 
							  pSrc->GetType());
		}
		memcpy(m_EncoderLL.GetPtr(GetX0(), GetY0() + (pSrc->GetY0() - GetY0()) % GetCBHeight()), pSrc->GetPtr(), Size(pSrc->GetType(), pSrc->GetWidth()));
		bRet = WriteSubBandLine(nCtx, pSrc, NCSJPC_LL);
	} else {
		if(m_EncoderLL.GetPtr() == NULL) {
			// Create the subband buffers, these cover a full CB height row
			m_EncoderLL.Alloc((UINT32)NCSCeilDiv(GetX0(), 2), (UINT32)NCSCeilDiv(GetY0(), 2),
							  (UINT32)NCSCeilDiv(GetWidth(), 2), (UINT32)1, 
							  pSrc->GetType());
			m_EncoderLH.Alloc((UINT32)NCSCeilDiv(GetX0(), 2), (UINT32)NCSCeilDiv(GetY0(), 2), 
							  (UINT32)NCSCeilDiv(GetWidth(), 2), (UINT32)GetCBHeight(), 
							  pSrc->GetType());
			m_EncoderHL.Alloc((UINT32)NCSCeilDiv(GetX0(), 2) - 1, (UINT32)NCSCeilDiv(GetY0(), 2), 
							  (UINT32)NCSCeilDiv(GetWidth(), 2)+1, (UINT32)GetCBHeight(), 
							  pSrc->GetType());
			m_EncoderHH.Alloc((UINT32)NCSCeilDiv(GetX0(), 2) - 1, (UINT32)NCSCeilDiv(GetY0(), 2), 
							  (UINT32)NCSCeilDiv(GetWidth(), 2)+1, (UINT32)GetCBHeight(), 
							  pSrc->GetType());
		}

		bRet = SD_2D(nCtx, pSrc);
	}
	if(pSrc->GetY0() == GetY1() - 1) {
		// Free up wavelet buffers
		m_EncoderLL.Free();
		m_EncoderLH.Free();
		m_EncoderHL.Free();
		m_EncoderHH.Free();
		pCtx->m_State.SetBuffers(0);

//		UINT32 l;
		// Last output line
//		for(l = 0; l < m_EncoderFiles.size(); l++) {
//			CloseEncoderLayerFile(l, false);
//		}
	}
	return(bRet);
}

bool CNCSJPCResolution::UnLink(ContextID nCtx, UINT16 nInputs)
{
	bool bRet = m_Precincts.UnLink(nCtx, nInputs);
	bRet &= CNCSJPCNode::UnLink(nCtx, nInputs);
	return(bRet);
}

bool CNCSJPCResolution::WritePrecinctLayerPLT(CNCSJPC &JPC,
											  CNCSJPCIOStream &Stream,
											  CNCSJPCTilePartHeader &TilePart,
											  CNCSJPCPLTMarker &PLT,
											  CNCSJPCPacketLengthType &Len,
											  INT64 &nSOTOffset,
											  UINT32 &nBytes,
											  UINT32 p,
											  UINT32 l)
{
	bool bRet = true;

	CNCSJPCIOStream *pStream = JPC.m_pEncoderPLTFile;
	if(pStream) {
		bRet &= pStream->Seek((m_nEncoderPLTStartIndex + l * GetNumPrecinctsWide() * GetNumPrecinctsHigh() + p) * sizeof(UINT32), CNCSJPCIOStream::START);

		if(bRet) {
			PLT.AllocLengths();

			if(bRet) {
				UINT32 nThisBytes = 0;
				bRet &= pStream->ReadUINT32(Len.m_nHeaderLength);

				// Stuffed into 7 bits, top bit is continuation bit, so work out how many lots of 7 bits required
				if(Len.m_nHeaderLength < 128) {	//2^7
					nThisBytes = 1;
				} else if(Len.m_nHeaderLength < 16384) { //2^14
					nThisBytes = 2;
				} else if(Len.m_nHeaderLength < 2097152) {  //2^21
					nThisBytes = 3;
				} else if(Len.m_nHeaderLength < 268435456) {	//2^28
					nThisBytes = 4;
				} else {
					nThisBytes = 5;
				}

				if(nBytes + nThisBytes > 65535) {//len stored as UINT16, so need multiple PLTs if more bytes 2^16-1
					PLT.m_nLength = nBytes;
					bRet &= (PLT.UnParse(JPC, Stream) == NCS_SUCCESS);
								
					if(PLT.m_nZplt == 255) {
						// Can only have 256 PLT's in a tile part.
						// This unfortunately means we can only have ~16MB or 8M packets
						// in each tile part, and only 255 tile parts in a codestream.
						// If we have > 2130576384 packets (~2^31), you need to use multiple tiles.
						// With a 64x64 packet, that's around 3,000,000x3,000,000xRGB.
						// Rather than use multiuple tile parts and suffer from artifacts, 							// the preferred method to circumvent this limitation is to go to 							// larger precincts, upto 32kx32k is supported.  This also gets around a similar
						// issue with ECWP only supporting 32bit "block" numbers (ie 2^32 packet indices).
						// If we have >255 PLTs, then we need to write out multiple tile
						// parts, and update Psot in each one but the last.
						bRet &= TilePart.m_SOD.UnParse(JPC, Stream) == NCS_SUCCESS;
						INT64 nOffset = Stream.Tell();

						// Seek back and update Psot
						bRet &= Stream.Seek(nSOTOffset + 3 * sizeof(UINT16), CNCSJPCIOStream::START);
						bRet &= Stream.WriteUINT32((UINT32)(nOffset - nSOTOffset));
						bRet &= Stream.Seek(nOffset, CNCSJPCIOStream::START);
						nSOTOffset = nOffset;
						TilePart.m_SOT.m_nTPsot++;								
						TilePart.UnParse(JPC, Stream);
						PLT.m_nZplt = 0;
					} else {
						PLT.m_nZplt++;
					}
					PLT.FreeLengths();
					PLT.AllocLengths();

					nBytes = 3;
				}
				nBytes += nThisBytes;
				PLT.AddLength(Len);
			}
		}
	}
	return(bRet);
}

bool CNCSJPCResolution::WriteLayerPLTs(CNCSJPC &JPC,
									   CNCSJPCIOStream &Stream,
									   CNCSJPCTilePartHeader &TilePart,
									   CNCSJPCPLTMarker &PLT,
									   CNCSJPCPacketLengthType &Len,
									   INT64 &nSOTOffset,
									   UINT32 &nBytes,
									   UINT32 l)
{
	bool bRet = true;

	CNCSJPCIOStream *pStream = JPC.m_pEncoderPLTFile;
	if(pStream) {
		INT64 nLen = GetNumPrecinctsWide() * GetNumPrecinctsHigh() * sizeof(UINT32);//pStream->Size();
		bRet &= pStream->Seek((m_nEncoderPLTStartIndex + l * GetNumPrecinctsWide() * GetNumPrecinctsHigh()) * sizeof(UINT32), CNCSJPCIOStream::START);

		if(bRet) {
			PLT.AllocLengths();

			while(bRet && nLen > 0) {
				UINT32 nThisBytes = 0;
				bRet &= pStream->ReadUINT32(Len.m_nHeaderLength);

				// Stuffed into 7 bits, top bit is continuation bit, so work out how many lots of 7 bits required
				if(Len.m_nHeaderLength < 128) {	//2^7
					nThisBytes = 1;
				} else if(Len.m_nHeaderLength < 16384) { //2^14
					nThisBytes = 2;
				} else if(Len.m_nHeaderLength < 2097152) {  //2^21
					nThisBytes = 3;
				} else if(Len.m_nHeaderLength < 268435456) {	//2^28
					nThisBytes = 4;
				} else {
					nThisBytes = 5;
				}

				if(nBytes + nThisBytes > 65535) {//len stored as UINT16, so need multiple PLTs if more bytes 2^16-1
					PLT.m_nLength = nBytes;
					bRet &= (PLT.UnParse(JPC, Stream) == NCS_SUCCESS);
								
					if(PLT.m_nZplt == 255) {
						// Can only have 256 PLT's in a tile part.
						// This unfortunately means we can only have ~16MB or 8M packets
						// in each tile part, and only 255 tile parts in a codestream.
						// If we have > 2130576384 packets (~2^31), you need to use multiple tiles.
						// With a 64x64 packet, that's around 3,000,000x3,000,000xRGB.
						// Rather than use multiuple tile parts and suffer from artifacts, 							// the preferred method to circumvent this limitation is to go to 							// larger precincts, upto 32kx32k is supported.  This also gets around a similar
						// issue with ECWP only supporting 32bit "block" numbers (ie 2^32 packet indices).
						// If we have >255 PLTs, then we need to write out multiple tile
						// parts, and update Psot in each one but the last.
						bRet &= TilePart.m_SOD.UnParse(JPC, Stream) == NCS_SUCCESS;
						INT64 nOffset = Stream.Tell();

						// Seek back and update Psot
						bRet &= Stream.Seek(nSOTOffset + 3 * sizeof(UINT16), CNCSJPCIOStream::START);
						bRet &= Stream.WriteUINT32((UINT32)(nOffset - nSOTOffset));
						bRet &= Stream.Seek(nOffset, CNCSJPCIOStream::START);
						nSOTOffset = nOffset;
						TilePart.m_SOT.m_nTPsot++;								
						TilePart.UnParse(JPC, Stream);
						PLT.m_nZplt = 0;
					} else {
						PLT.m_nZplt++;
					}
					PLT.FreeLengths();
					PLT.AllocLengths();

					nBytes = 3;
				}
				nBytes += nThisBytes;
				PLT.AddLength(Len);
				nLen -= sizeof(UINT32);
			}
		}
	}
	return(bRet);
}

bool CNCSJPCResolution::WritePrecinctLayerPacket(CNCSJPC &JPC,
												 CNCSJPCIOStream &Stream,
												 UINT32 p,
												 UINT32 l)
{
	bool bRet = true;

	CNCSJPCIOStream *pLengthStream = JPC.m_pEncoderPLTFile;
	CNCSJPCIOStream *pPacketStream = JPC.m_EncoderFiles[m_nResolution];
	CNCSJPCIOStream *pOffsetStream = JPC.m_pEncoderOffsetFile;
	
	if(pLengthStream && pPacketStream && pOffsetStream) {
		p += m_nEncoderPLTStartIndex + l * GetNumPrecinctsWide() * GetNumPrecinctsHigh();
		bRet &= pLengthStream->Seek(p * sizeof(UINT32), CNCSJPCIOStream::START);
		bRet &= pOffsetStream->Seek(p * sizeof(UINT64), CNCSJPCIOStream::START);

		UINT64 nOffset = 0; 
		UINT32 nLen = 0;

		bRet &= pOffsetStream->ReadUINT64(nOffset);
		bRet &= pPacketStream->Seek(nOffset, CNCSJPCIOStream::START);
		bRet &= pLengthStream->ReadUINT32(nLen);

		while(bRet && nLen > 0) {
			UINT8 buf[4096];
			UINT32 nThis = NCSMin(sizeof(buf), nLen);
			bRet &= pPacketStream->Read(buf, (UINT32)nThis);
			bRet &= Stream.Write(buf, (UINT32)nThis);
			nLen -= nThis;
		}
	}
	return(bRet);
}

bool CNCSJPCResolution::WriteLayerPackets(CNCSJPC &JPC,
										  CNCSJPCIOStream &Stream,
										  UINT32 l)
{
	bool bRet = true;

	UINT32 nPackets = GetNumPrecinctsWide() * GetNumPrecinctsHigh();
	for(UINT32 p = 0; p < nPackets; p++) {
		bRet &= WritePrecinctLayerPacket(*m_pComponent->m_pTilePart->m_pJPC, Stream, p, l);
	}
	return(bRet);
}

// PRIVATE

// Read a line from a subband.
bool CNCSJPCResolution::ReadSubBandLine(ContextID nCtx, CNCSJPCBuffer *pDst, NCSJPCSubBandType eSubBandType)
{
	CNCSJPCResolution *pInput = (GetContext(nCtx)->NrInputs() > 0) ? (CNCSJPCResolution*)GetContext(nCtx)->GetInput(0) : NULL;
	INT32 nInputX = pDst->GetX0();
	INT32 nInputY = pDst->GetY0();
	UINT32 nInputWidth = pDst->GetWidth();
	Type eType = pDst->GetType();
	void *pBuffer = pDst->GetPtr();

	bool bRet = false;
	INT32 nMaxHeight = (pInput ? pInput->GetHeight() : GetHeight());
	INT32 nMaxWidth = (pInput ? pInput->GetWidth() : GetWidth());
	INT32 nReadX0 = nInputX;
	INT32 nReadX1 = nInputX + (INT32)nInputWidth;
	INT32 nReadWidth = nReadX1 - nReadX0;
	INT32 nReadY0 = nInputY;

	if(nReadWidth == 0) {
		return(true);
	}
	if(eSubBandType == NCSJPC_LL && m_nResolution != 0) {
		if(nMaxWidth <= 0 || nMaxHeight <= 0) {
			pDst->Clear();
			return(true);
		}
		CNCSJPCBuffer tmp;
		tmp.Assign(nReadX0 - pInput->GetX0(), nReadY0 - pInput->GetY0(), (UINT32)nReadWidth, (UINT32)1, eType, (UINT8*)pBuffer + Size(eType, nReadX0 - nInputX));
		bRet = CNCSJPCNode::ReadLine(nCtx, &tmp, NCSJPC_LL);
		if(tmp.GetPtr() != pDst->GetPtr()) {
			pDst->Assign(pDst->GetType(),
						 tmp.GetPtr(),//-(nReadX0 - nInputX), 0),
						 tmp.GetStep());
		}
		tmp.Release();
	} else {
		INT32 nCurY = 0;
		INT32 nBufferOffset = nReadX0-nInputX;

		INT32 nPrecinctsHigh = GetNumPrecinctsHigh();
		INT32 nPrecinctsWide = GetNumPrecinctsWide();
		if(nInputY < CNCSJPCSubBand::GetY0(this, 0, 0, eSubBandType) || nInputY >= CNCSJPCSubBand::GetY1(this, 0, nPrecinctsHigh, eSubBandType)) {
			pDst->Clear();
			return(true);
		}
		if(nPrecinctsHigh && nPrecinctsWide && nReadWidth /*&& nMaxWidth*/) {
			INT32 nY0 = CNCSJPCSubBand::GetY0(this, 0, 0, eSubBandType);
			INT32 nY1 = CNCSJPCSubBand::GetY1(this, 0, 0, eSubBandType);
			int py = 0;
			nCurY = nY0;
			
			if(nReadY0 >= nY1) {
				UINT32 nSBHeight = CNCSJPCSubBand::GetY1(this, 0, 1, eSubBandType) - CNCSJPCSubBand::GetY0(this, 0, 1, eSubBandType);
				if(nSBHeight) {
					nCurY = nY1;
					py++;
					py += (nReadY0 - nCurY) / nSBHeight;
					nCurY += (py - 1) * nSBHeight;
				}
			}
			for(; py < nPrecinctsHigh; py++) {
				INT32 nNewY = nCurY;
				CNCSJPCPrecinct *pPrecinct = m_Precincts.find(0, py);
				
				if(pPrecinct && pPrecinct->m_SubBands[eSubBandType]) {
					nNewY += pPrecinct->m_SubBands[eSubBandType]->GetHeight();
				} else {
					nNewY = CNCSJPCSubBand::GetY1(this, 0, py, eSubBandType);
				}
				if(nReadY0 >= nCurY && nReadY0 < nNewY) {
					break;
				}
				nCurY = nNewY;
			}
			if(py < nPrecinctsHigh) {
				INT32 nCurX = 0;
				INT32 nX0 = CNCSJPCSubBand::GetX0(this, 0, py, eSubBandType);
				INT32 nX1 = CNCSJPCSubBand::GetX1(this, 0, py, eSubBandType);
				int px = 0;
			
				if(nReadX0 < nX0) {
					nReadWidth -= nX0 - nReadX0;
					nBufferOffset += nX0 - nReadX0;
					nReadX0 = nX0;
				}
				nCurX = nX0;

				if(nReadX0 >= nX1 && nReadWidth) {
					UINT32 nSBWidth = CNCSJPCSubBand::GetX1(this, 1, 0, eSubBandType) - CNCSJPCSubBand::GetX0(this, 1, 0, eSubBandType);
					if(nSBWidth) {
						nCurX = nX1;
						px++;
						px += (nReadX0 - nCurX) / nSBWidth;
						nCurX += (px - 1) * nSBWidth;
					}
				}

				for(; px < nPrecinctsWide && nReadWidth; px++) {
					INT32 nSBWidth;
					CNCSJPCPrecinct *pPrecinct = m_Precincts.find(px, py);

					if(pPrecinct && pPrecinct->m_SubBands[eSubBandType]) {
						nSBWidth = pPrecinct->m_SubBands[eSubBandType]->GetWidth();
					} else {
						nSBWidth = CNCSJPCSubBand::GetX1(this, px, py, eSubBandType) -nCurX;
					}

					if(nReadX0 >= nCurX && nReadX0 < nCurX + nSBWidth) {
						CNCSJPCPrecinct *pPrecinct = m_Precincts.find(px, py);

						if(pPrecinct == NULL) {
							pPrecinct = new CNCSJPCPrecinct(this, px + py * nPrecinctsWide);
							pPrecinct->AddRef();
							m_Precincts.insert(px, py, pPrecinct);

							bool bComplete = false;
							CNCSJPCTilePartHeader *pMainTP = m_pComponent->m_pTilePart;
							pMainTP->Start(pMainTP);
							while(!bComplete) {
								pMainTP->Iterate(pMainTP, NULL, bComplete);
							}
						}

						UINT32 nSBReadWidth = NCSMin(nReadWidth, nSBWidth - (nReadX0 - nCurX));

						CNCSJPCBuffer tmp;
						tmp.Assign(nReadX0 - nCurX, nReadY0 - nCurY, nSBReadWidth, (UINT32)1,
								   eType, ((UINT8*)pBuffer) + Size(eType, nBufferOffset));
						bRet = pPrecinct->ReadLine(nCtx, &tmp, eSubBandType);	
						tmp.Release();

						if(bRet != true) {
							*(CNCSError*)this = *(CNCSError*)pPrecinct;
							break;
						}
						nReadX0 += nSBReadWidth;
						nBufferOffset += nSBReadWidth;
						nReadWidth -= nSBReadWidth;
					}
					nCurX += nSBWidth;
				}
			}
		}
		if(nReadWidth) {
			memset(((UINT8*)pBuffer) + Size(eType, nBufferOffset), 0, (size_t)Size(eType, nReadWidth));
			bRet = true;
		}
	}	
	return(bRet);
}

// Write a BufferType line to the given subband.
bool CNCSJPCResolution::WriteSubBandLine(ContextID nCtx, 
										 CNCSJPCBuffer *pSrc, 
										 NCSJPCSubBandType eSubBandType)
{
	bool bRet = true;

	if(eSubBandType == NCSJPC_LL && m_nResolution != 0) {
		CNCSJPCResolution *pInput = m_pComponent->m_Resolutions[m_nResolution - 1];
		bRet = pInput->WriteLine(nCtx, pSrc, 0);
	} else {
		INT32 nPrecinctsWide = GetNumPrecinctsWide();
		INT32 nPrecinctHeight = GetPrecinctHeight();

		INT32 nSrcY0 = pSrc->GetY0();
		INT32 px;
		INT32 py = (nSrcY0 - (GetY0() / (m_nResolution ? 2 : 1))) / (nPrecinctHeight / (m_nResolution ? 2 : 1));
		//nSrcY0 -= py * (nPrecinctHeight / (m_nResolution ? 2 : 1));
		bool bFinal = nSrcY0 == CNCSJPCSubBand::GetY1(this, py * nPrecinctsWide, eSubBandType) - 1 && (eSubBandType == NCSJPC_HL || eSubBandType == NCSJPC_LL);

		if(bFinal || nSrcY0 % GetCBHeight() == GetCBHeight() - 1 || nSrcY0 == CNCSJPCSubBand::GetY1(this, py * nPrecinctsWide, eSubBandType) - 1) {
			for(px = 0; px < nPrecinctsWide; px++) {
				CNCSJPCPrecinct *pPrecinct = m_Precincts.find(px, py);
				if(pPrecinct == NULL) {
					pPrecinct = new CNCSJPCPrecinct(this, px + py * nPrecinctsWide);
					pPrecinct->AddRef();
					m_Precincts.insert(px, py, pPrecinct);
				}
				CNCSJPCSubBand *pSubBand = pPrecinct->m_SubBands[eSubBandType];

				nSrcY0 -= pSubBand->GetY0();

				switch(eSubBandType) {
					case NCSJPC_LL:
							pSubBand->Assign(pSubBand->GetX0(), 
											 pSubBand->GetY0(), 
											 pSubBand->GetWidth(), 
											 pSubBand->GetHeight(), 
											 pSrc->GetType(), 
											 m_EncoderLL.GetPtr(pSubBand->GetX0()/*GetX0()*/, GetY0()-((nSrcY0 - (nSrcY0 % GetCBHeight())) % GetPrecinctHeight())), 
											 m_EncoderLL.GetStep());
						break;
					case NCSJPC_LH:
							pSubBand->Assign(pSubBand->GetX0(), 
											 pSubBand->GetY0(), 
											 pSubBand->GetWidth(), 
											 pSubBand->GetHeight(), 
											 pSrc->GetType(), 
											 m_EncoderLH.GetPtr(pSubBand->GetX0()/*NCSCeilDiv(GetX0(), 2)*/,  NCSCeilDiv(GetY0(), 2)-((nSrcY0 - (nSrcY0 % GetCBHeight())) % (GetPrecinctHeight() / 2))), 
											 m_EncoderLH.GetStep());
						break;
					case NCSJPC_HL:
							pSubBand->Assign(pSubBand->GetX0(), 
											 pSubBand->GetY0(), 
											 pSubBand->GetWidth(), 
											 pSubBand->GetHeight(), 
											 pSrc->GetType(), 
											 m_EncoderHL.GetPtr(pSubBand->GetX0()/*NCSCeilDiv(GetX0(), 2)*/,  NCSCeilDiv(GetY0(), 2)-((nSrcY0 - (nSrcY0 % GetCBHeight())) % (GetPrecinctHeight() / 2))), 
											 m_EncoderHL.GetStep());
						break;
					case NCSJPC_HH:
							pSubBand->Assign(pSubBand->GetX0(), 
											 pSubBand->GetY0(), 
											 pSubBand->GetWidth(), 
											 pSubBand->GetHeight(), 
											 pSrc->GetType(), 
											 m_EncoderHH.GetPtr(pSubBand->GetX0()/*NCSCeilDiv(GetX0(), 2)*/, NCSCeilDiv(GetY0(), 2)-((nSrcY0 - (nSrcY0 % GetCBHeight())) % (GetPrecinctHeight() / 2))), 
											 m_EncoderHH.GetStep());
						break;
				}

				bRet &= pSubBand->WriteLine(nCtx, pSrc, eSubBandType);

				if(bFinal) {//nSrcY0 == pSubBand->GetY1() - 1 && (eSubBandType == NCSJPC_HL || eSubBandType == NCSJPC_LL)) {		
					CNCSJPCTilePartHeader *pTP = m_pComponent->m_pTilePart;
					for(pTP->m_nCurLayer = 0; pTP->m_nCurLayer < pTP->m_COD.m_SGcod.m_nLayers; pTP->m_nCurLayer++) {
						pTP->m_nCurTile = pTP->m_SOT.m_nIsot;
						pTP->m_nCurResolution = m_nResolution;
						pTP->m_nCurComponent = m_pComponent->m_iComponent;
						pTP->m_nCurPrecinctX = pPrecinct->m_nPrecinct % nPrecinctsWide;
						pTP->m_nCurPrecinctY = pPrecinct->m_nPrecinct / nPrecinctsWide;
						CNCSJPCPacket packet;
//						if(m_EncoderFiles[pTP->m_nCurLayer] == NULL) {
//							bRet &= OpenEncoderLayerFile(pTP->m_nCurLayer, true);
/*							char szRelTmpDir[MAX_PATH+1];

							sprintf(szRelTmpDir, "%s\\t%ld\\c%ld\\r%ld", 
												pTP->m_pJPC->m_pTmpDir, 
												pTP->m_SOT.m_nIsot, 
												m_pComponent->m_iComponent, 
												m_nResolution);
							NCSMakeDir(szRelTmpDir, TRUE);

							sprintf(szRelTmpDir, "%s\\t%ld\\c%ld\\r%ld\\l%ld.tmp0", 
												pTP->m_pJPC->m_pTmpDir, 
												pTP->m_SOT.m_nIsot, 
												m_pComponent->m_iComponent, 
												m_nResolution, 
												pTP->m_nCurLayer);
							
							m_EncoderFiles[pTP->m_nCurLayer] = new CNCSJPCFileIOStream();
							CNCSError Error = m_EncoderFiles[pTP->m_nCurLayer]->Open(szRelTmpDir, true);
							if(Error != NCS_SUCCESS) {
								return(false);
							}*/
//						}
						packet.UnParseHeader(*pTP->m_pJPC, 
											*m_pComponent->m_pTilePart->m_pJPC->m_EncoderFiles[m_nResolution/*pTP->m_nCurLayer*/],//FIXME
											pTP, true);
						pTP->m_nCurPacket++;
					}
					pPrecinct->UnRef();
					m_Precincts.remove(px, py);
				}
			}
		}
#ifdef NOTUSED
		for(px = 0; px < nPrecinctsWide; px++) {
			CNCSJPCPrecinct *pPrecinct = m_Precincts[px + py * nPrecinctsWide];
			if(pPrecinct == NULL) {
				m_Precincts.insert(pair <px + py * nPrecinctsWide, new CNCSJPCPrecinct(this, px + py * nPrecinctsWide)>);
				pPrecinct = m_Precincts[px + py * nPrecinctsWide];
			}
			CNCSJPCSubBand *pSubBand = pPrecinct->m_SubBands[eSubBandType];
			bRet &= pSubBand->WriteLine(nCtx, pSrc, eSubBandType);

			if(bFinal) {//nSrcY0 == pSubBand->GetY1() - 1 && (eSubBandType == NCSJPC_HL || eSubBandType == NCSJPC_LL)) {		
				CNCSJPCTilePartHeader *pTP = m_pComponent->m_pTilePart;

				pTP->m_nCurTile = pTP->m_SOT.m_nIsot;
				pTP->m_nCurLayer = 0;
				pTP->m_nCurResolution = m_nResolution;
				pTP->m_nCurComponent = m_pComponent->m_iComponent;
				//FIXME = write precinct to codestream first
				pTP->m_nCurPrecinct = pPrecinct->m_nPrecinct;
				CNCSJPCPacket packet;
				packet.UnParseHeader(*pTP->m_pJPC, 
									*m_pEncoderFiles,
									pTP, true);
				pTP->m_nCurPacket++;
				delete m_Precincts[px + py * nPrecinctsWide];
				m_Precincts[px + py * nPrecinctsWide] = NULL;
			}
		} 
#endif
	}
	return(bRet);
}


bool CNCSJPCResolution::INTERLEAVE_2D(ContextID nCtx,
										INT32 u0,
										INT32 u1,
										INT32 v0,
										INT32 v1,
										CNCSJPCBuffer &a)
{
	INT32 ub, vb;
	bool bRet = true;
	INT32 vbMIN = NCSCeilDiv(v0, 2);
	INT32 vbMAX = NCSCeilDiv(v1, 2);
	INT32 ubMINL = NCSCeilDiv(u0, 2);
	INT32 ubMAXL = NCSCeilDiv(u1, 2);
	INT32 ubMINH = NCSFloorDiv(u0, 2);
	INT32 ubMAXH = NCSFloorDiv(u1, 2);

	for(vb = vbMIN; vb < vbMAX; vb++) {
		CNCSJPCBuffer LL(ubMINL, vb, (UINT32)(ubMAXL - ubMINL), (UINT32)1, a.GetType());
		CNCSJPCBuffer HL(ubMINH, vb, (UINT32)(ubMAXH - ubMINH), (UINT32)1, a.GetType());

		bRet = ReadSubBandLine(nCtx, &LL, NCSJPC_LL) && ReadSubBandLine(nCtx, &HL, NCSJPC_HL);
		if(bRet) {
			switch(a.GetType()) {
				case BT_INT16:
					{
						INT16 *pA = (INT16*)a.GetPtr(0, 2 * vb);
						INT16 *pSrcLL = (INT16*)LL.GetPtr(0, vb);
						INT16 *pSrcHL = (INT16*)HL.GetPtr(0, vb);
						
						for(ub = ubMINH; ub < ubMINL; ub++) {
							pA[2 * ub + 1] = pSrcHL[ub];
						}
						for(; ub < ubMAXH; ub++) {
							pA[2 * ub] = pSrcLL[ub];
							pA[2 * ub + 1] = pSrcHL[ub];
						}
						for(; ub < ubMAXL; ub++) {
							pA[2 * ub] = pSrcLL[ub];
						}
					}
					break;
				case BT_INT32:
					{
						INT32 *pA = (INT32*)a.GetPtr(0, 2 * vb);
						INT32 *pSrcLL = (INT32*)LL.GetPtr(0, vb);
						INT32 *pSrcHL = (INT32*)HL.GetPtr(0, vb);
						
						for(ub = ubMINH; ub < ubMINL; ub++) {
							pA[2 * ub + 1] = pSrcHL[ub];
						}
						for(; ub < ubMAXH; ub++) {
							pA[2 * ub] = pSrcLL[ub];
							pA[2 * ub + 1] = pSrcHL[ub];
						}
						for(; ub < ubMAXL; ub++) {
							pA[2 * ub] = pSrcLL[ub];
						}
					}
					break;
				case BT_IEEE4:
					{
						IEEE4 *pA = (IEEE4*)a.GetPtr(0, 2 * vb);
						IEEE4 *pSrcLL = (IEEE4*)LL.GetPtr(0, vb);
						IEEE4 *pSrcHL = (IEEE4*)HL.GetPtr(0, vb);
						
						for(ub = ubMINH; ub < ubMINL; ub++) {
							pA[2 * ub + 1] = pSrcHL[ub];
						}
						for(; ub < ubMAXH; ub++) {
							pA[2 * ub] = pSrcLL[ub];
							pA[2 * ub + 1] = pSrcHL[ub];
						}
						for(; ub < ubMAXL; ub++) {
							pA[2 * ub] = pSrcLL[ub];
						}
					}
					break;
			}
		}
	}
	vbMIN = NCSFloorDiv(v0, 2);
	vbMAX = NCSFloorDiv(v1, 2);

	for(vb = vbMIN; vb < vbMAX; vb++) {
		CNCSJPCBuffer LH(ubMINL, vb, (UINT32)(ubMAXL - ubMINL), (UINT32)1, a.GetType());
		CNCSJPCBuffer HH(ubMINH, vb, (UINT32)(ubMAXH - ubMINH), (UINT32)1, a.GetType());

		bRet = ReadSubBandLine(nCtx, &LH, NCSJPC_LH) && ReadSubBandLine(nCtx, &HH, NCSJPC_HH);
		if(bRet) {
			switch(a.GetType()) {
				case BT_INT16:
					{
						INT16 *pA = (INT16*)a.GetPtr(0, 2 * vb + 1);
						INT16 *pSrcLH = (INT16*)LH.GetPtr(0, vb);
						INT16 *pSrcHH = (INT16*)HH.GetPtr(0, vb);
						
						for(ub = ubMINH; ub < ubMINL; ub++) {
							pA[2 * ub + 1] = pSrcHH[ub];
						}
						for(; ub < ubMAXH; ub++) {
							pA[2 * ub] = pSrcLH[ub];
							pA[2 * ub + 1] = pSrcHH[ub];
						}
						for(; ub < ubMAXL; ub++) {
							pA[2 * ub] = pSrcLH[ub];
						}
					}
				break;
				case BT_INT32:
					{
						INT32 *pA = (INT32*)a.GetPtr(0, 2 * vb + 1);
						INT32 *pSrcLH = (INT32*)LH.GetPtr(0, vb);
						INT32 *pSrcHH = (INT32*)HH.GetPtr(0, vb);
						
						for(ub = ubMINH; ub < ubMINL; ub++) {
							pA[2 * ub + 1] = pSrcHH[ub];
						}
						for(; ub < ubMAXH; ub++) {
							pA[2 * ub] = pSrcLH[ub];
							pA[2 * ub + 1] = pSrcHH[ub];
						}
						for(; ub < ubMAXL; ub++) {
							pA[2 * ub] = pSrcLH[ub];
						}
					}
				break;
				case BT_IEEE4:
					{
						IEEE4 *pA = (IEEE4*)a.GetPtr(0, 2 * vb + 1);
						IEEE4 *pSrcLH = (IEEE4*)LH.GetPtr(0, vb);
						IEEE4 *pSrcHH = (IEEE4*)HH.GetPtr(0, vb);
						
						for(ub = ubMINH; ub < ubMINL; ub++) {
							pA[2 * ub + 1] = pSrcHH[ub];
						}
						for(; ub < ubMAXH; ub++) {
							pA[2 * ub] = pSrcLH[ub];
							pA[2 * ub + 1] = pSrcHH[ub];
						}
						for(; ub < ubMAXL; ub++) {
							pA[2 * ub] = pSrcLH[ub];
						}
					}
				break;
			}
		}
	}
	return(bRet);
}

#define MOD(n, d) ((n) - (d) * NCSFloorDiv((n), (d)))

INT32 PSE0(INT32 i, INT32 i0, INT32 i1) 
{
	if(i >= i0 && i < i1) {
		return(i);
	}
	if(i1 - i0 == 1) {
		return(i0);
	}
	INT32 n2Width = 2 * (i1 - i0 - 1);
	INT32 nMOD = MOD(i - i0, n2Width);

	return(i0 + NCSMin(nMOD, n2Width - nMOD));
}

template< class T > void PSE_EXTEND(INT32 nStart, INT32 nEnd, INT32 nX0, INT32 nX1, T *pA)
{
	for(INT32 i = nEnd - 1; i >= nStart; i--) {
		pA[i] = pA[PSE0(i, nX0, nX1)];
	}
}

bool CNCSJPCResolution::HOR_SR(ContextID nCtx,
							   CNCSJPCBuffer &a,
							   INT32 u0,
							   INT32 u1,
							   INT32 v0,
							   INT32 v1)
{ 
	bool bRet = true;
	UINT8 nBorder = 2 + 3 * CNCSJPCBuffer::Border(a.GetType());
 	INT32 nX0 = GetX0();
	INT32 nX1 = GetX1();

	switch(a.GetType()) {
		case BT_INT16:	
				bRet = INTERLEAVE_2D(nCtx, u0-nBorder, u1+nBorder, v0, v1, a);
				if(bRet) {
					for(INT32 v = v0; v < v1; v++) {
						INT32 uMIN = NCSFloorDiv(u0, 2);
						INT32 uMAX = NCSFloorDiv(u1, 2);
						INT16 *pA = (INT16*)a.GetPtr(0, v);

							// PSE
						PSE_EXTEND(u0 - nBorder, nX0, nX0, nX1, pA);
						PSE_EXTEND(nX1, u1 + nBorder, nX0, nX1, pA);

						if(GetWidth() == 1) {
							if(GetX0() & 0x1) {
								a.Add(0, 1);
							}
						} else { 
							INT32 nNext = pA[2 * uMIN] - NCSFloorDiv(pA[2 * uMIN - 1] + pA[2 * uMIN + 1] + 2, 4);
							INT32 n;
							for(n = uMIN; n < uMAX; n++) {
								INT32 n2n = 2 * n;
								pA[n2n] = nNext;
								nNext = pA[n2n + 2] - NCSFloorDiv(pA[n2n - 1 + 2] + pA[n2n + 1 + 2] + 2, 4);
								pA[n2n + 1] = pA[n2n + 1] + NCSFloorDiv(pA[n2n] + nNext, 2);
							}
							pA[2 * n] = nNext;
						}
					}
				}
			break;
		case BT_INT32:	
				bRet = INTERLEAVE_2D(nCtx, u0-nBorder, u1+nBorder, v0, v1, a); 
				if(bRet) {
					for(INT32 v = v0; v < v1; v++) {
						INT32 uMIN = NCSFloorDiv(u0, 2);
						INT32 uMAX = NCSFloorDiv(u1, 2);
						INT32 *pA = (INT32*)a.GetPtr(0, v);

							// PSE
						PSE_EXTEND(u0 - nBorder, nX0, nX0, nX1, pA);
						PSE_EXTEND(nX1, u1 + nBorder, nX0, nX1, pA);

						if(GetWidth() == 1) {
							if(GetX0() & 0x1) {
								a.Add(0, 1);
							}
						} else { 
							INT32 nNext = pA[2 * uMIN] - NCSFloorDiv(pA[2 * uMIN - 1] + pA[2 * uMIN + 1] + 2, 4);
							INT32 n;
							for(n = uMIN; n < uMAX; n++) {
								INT32 n2n = 2 * n;
								pA[n2n] = nNext;
								nNext = pA[n2n + 2] - NCSFloorDiv(pA[n2n - 1 + 2] + pA[n2n + 1 + 2] + 2, 4);
								pA[n2n + 1] = pA[n2n + 1] + NCSFloorDiv(pA[n2n] + nNext, 2);
							}
							pA[2 * n] = nNext;
						}
					}
				}
			break;
		case BT_IEEE4:	
				bRet = INTERLEAVE_2D(nCtx, u0-nBorder, u1+nBorder, v0, v1, a); 
				if(bRet) {
					for(INT32 v = v0; v < v1; v++) {
						INT32 uMIN = NCSFloorDiv(u0, 2);
						INT32 uMAX = NCSFloorDiv(u1, 2);
						IEEE4 *pA = (IEEE4*)a.GetPtr(0, v);

							// PSE
						PSE_EXTEND(u0 - nBorder, nX0, nX0, nX1, pA);
						PSE_EXTEND(nX1, u1 + nBorder, nX0, nX1, pA);

						if(GetWidth() != 1) {
							//Step6   3   7   11         
							//         \ /|\ /|\         
							//Step5     2 | 6 | 10          
							//           \|/|\|/|\       
							//Step4       1 | 5 | 9         
							//             \|/|\|/|\   
							//Step3         0 | 4 | 8       
							//             /|\|/|\|/|\          
							//Index:    4 3 2 1 0 1 2 3 4 5 6 7
							for(INT32 n = uMIN - 3; n <= uMAX; n++) {
								INT32 n2n = 2 * n;
								pA[n2n + 2] *= LIFT_9X7_K;
								pA[n2n + 3] *= LIFT_9X7_1K;
								pA[n2n + 2] -= LIFT_9X7_g * (pA[n2n + 1] + pA[n2n + 3]);
								pA[n2n + 1] -= LIFT_9X7_y * (pA[n2n] + pA[n2n + 2]);
								pA[n2n]		-= LIFT_9X7_B * (pA[n2n - 1] + pA[n2n + 1]);
								pA[n2n - 1] -= LIFT_9X7_a * (pA[n2n - 2] + pA[n2n]);
							}
						}
					}
				}
			break;
	}

	return(bRet);
}

CNCSJPCBuffer *CNCSJPCResolution::GET_STATE_BUFFER(ContextID nCtx,
												   INT32 u0, 
												   INT32 u1, 
												   INT32 v,
												   CNCSJPCBuffer::Type eType)
{
	Context *pCtx = (CNCSJPCResolution::Context*)GetContext(nCtx);
	UINT32 nBuffer;
	bool bFound = false;
	bool bRet = true;
	CNCSJPCRect rect(pCtx->m_nStateU0, v, pCtx->m_nStateU1, v + 1);

	CNCSJPCBuffer *pBuffer = pCtx->m_State.FindBuffer(nBuffer, 
													  rect, 
													  bFound, 
													  eType);
	if(!bFound && pBuffer) {
		CNCSJPCBuffer tmp;
		INT32 nY = PSE0(pBuffer->GetY0(), GetY0(), GetY1());
		tmp.Assign(pBuffer->GetX0(), nY, pBuffer->GetX1(), nY + 1, eType, pBuffer->GetPtr());
		bRet &= HOR_SR(nCtx, tmp, u0, u1, nY, nY + 1);
		tmp.Release();
	}
	return(bRet ? pBuffer : NULL);
}

bool CNCSJPCResolution::VER_SR_INPUT2(ContextID nCtx,
									  INT32 u0, INT32 u1,
									  CNCSJPCBuffer *pDst,
									  CNCSJPCBuffer::Type eType)
{
	Context *pCtx = (CNCSJPCResolution::Context*)GetContext(nCtx);
	bool bRet = true;
	UINT8 nBorder = 2;
	CNCSJPCBuffer *pState2U = NULL;
	CNCSJPCBuffer *pState1U = NULL;
	CNCSJPCBuffer *pState0 = NULL;
	CNCSJPCBuffer *pState1D = NULL;
	CNCSJPCBuffer *pState2D = NULL;
	CNCSJPCBuffer *pState3D = NULL;
	INT32 nY0 = GetY0();

	if(pCtx->m_nStateV - 2 >= nY0) {
		pState2U = GET_STATE_BUFFER(nCtx, u0, u1, pCtx->m_nStateV - 2, eType);
	}
	if(pCtx->m_nStateV - 1 >= nY0) {
		pState1U = GET_STATE_BUFFER(nCtx, u0, u1, pCtx->m_nStateV - 1, eType);
	}
	if(pCtx->m_nStateV >= nY0) {
		pState0 = GET_STATE_BUFFER(nCtx, u0, u1, pCtx->m_nStateV, eType);
	} 
	if(pCtx->m_nStateV + 1 >= nY0) {
		pState1D = GET_STATE_BUFFER(nCtx, u0, u1, pCtx->m_nStateV + 1, eType);
	}
	if(eType == BT_IEEE4) {
		if(pCtx->m_nStateV + 2 >= nY0) {
			pState2D = GET_STATE_BUFFER(nCtx, u0, u1, pCtx->m_nStateV + 2, eType);
		}
		pState3D = GET_STATE_BUFFER(nCtx, u0, u1, pCtx->m_nStateV + 3, eType);
		if(pCtx->m_nStateV + 2 < nY0) {
			pState2D = GET_STATE_BUFFER(nCtx, u0, u1, pCtx->m_nStateV + 2, eType);
		}
	}
	if(pCtx->m_nStateV + 1 < nY0) {
		pState1D = GET_STATE_BUFFER(nCtx, u0, u1, pCtx->m_nStateV + 1, eType);
	}
	if(pCtx->m_nStateV < nY0) {
		pState0 = GET_STATE_BUFFER(nCtx, u0, u1, pCtx->m_nStateV, eType);
	} 
	if(pCtx->m_nStateV - 1 < nY0) {
		pState1U = GET_STATE_BUFFER(nCtx, u0, u1, pCtx->m_nStateV - 1, eType);
	}
	if(pCtx->m_nStateV - 2 < nY0) {
		pState2U = GET_STATE_BUFFER(nCtx, u0, u1, pCtx->m_nStateV - 2, eType);
	}

	if(bRet) {
		INT32 nCurrY = pCtx->m_nStateV;

		switch(eType) {
			case BT_INT16:
				{
					INT16 *pA1UP = (INT16*)pState1U->GetPtr(0, pState1U->GetY0());
					INT16 *pA2UP = (INT16*)pState2U->GetPtr(0, pState2U->GetY0());
					INT16 *pA = (INT16*)pState0->GetPtr(0, pState0->GetY0());
					INT16 *pA1DOWN = (INT16*)pState1D->GetPtr(0, pState1D->GetY0());
					INT32 u = u0;

#ifdef NCSJPC_X86_MMI_MMX
					for(; u < u1 && u & 0x3; u++) {
						pA[u] -= NCSFloorDiv(pA1UP[u] + pA1DOWN[u] + 2, 4);
						pA1UP[u] += NCSFloorDiv(pA2UP[u] + pA[u], 2);
					}

					if(CNCSJPCBuffer::sm_SystemInfo.m_bMMXPresent) {
						INT32 n = 4 * (u1 / 4);
						__m64 mm7 = _mm_set1_pi16(2);

						//pA[u] -= NCSFloorDiv(pA1UP[u] + pA1DOWN[u] + 2, 4);
						//pA1UP[u] += NCSFloorDiv(pA2UP[u] + pA[u], 2);
						
						for(; u < n; u += 4) {
							__m64 mm0_0 = *((__m64*)&(pA[u]));
							__m64 mm1_1U = *((__m64*)&(pA1UP[u]));
							__m64 mm2_2U = *((__m64*)&(pA2UP[u]));
							__m64 mm3_1D = *((__m64*)&(pA1DOWN[u]));

							__m64 mm5 = _mm_adds_pi16(mm1_1U, mm3_1D);	// mm5 = pA1UP[u] + pA1DOWN[u]
							__m64 mm6 = _mm_adds_pi16(mm5, mm7);		// mm6 = mm5+2
							mm5 = _mm_srai_pi16(mm6, 2);				// mm5 = mm6 / 4
							mm6 = _mm_subs_pi16(mm0_0, mm5);			// mm6 = pA - mm5
							*((__m64*)&(pA[u])) = mm6;					// pA = mm6
							mm5 = _mm_adds_pi16(mm2_2U, mm6);			// mm5 = pA2UP + pA
							__m64 mm4 = _mm_srai_pi16(mm5, 1);			// mm4 = mm5 / 2
							mm5 = _mm_adds_pi16(mm1_1U, mm4);			// mm5 = pA1U + mm4
							*((__m64*)&(pA1UP[u])) = mm5;				// pA1UP = mm5
						}
						_mm_empty();
					}
#endif //NCSJPC_X86_MMI
					for(; u < u1; u++) {
						pA[u] -= NCSFloorDiv(pA1UP[u] + pA1DOWN[u] + 2, 4);
						pA1UP[u] += NCSFloorDiv(pA2UP[u] + pA[u], 2);
					}
				}
				break;
			case BT_INT32:
				{
					INT32 *pA1UP = (INT32*)pState1U->GetPtr(0, pState1U->GetY0());
					INT32 *pA2UP = (INT32*)pState2U->GetPtr(0, pState2U->GetY0());
					INT32 *pA = (INT32*)pState0->GetPtr(0, pState0->GetY0());
					INT32 *pA1DOWN = (INT32*)pState1D->GetPtr(0, pState1D->GetY0());

					for(INT32 u = u0; u < u1; u++) {
						pA[u] -= NCSFloorDiv(pA1UP[u] + pA1DOWN[u] + 2, 4);
						pA1UP[u] += NCSFloorDiv(pA2UP[u] + pA[u], 2);
					}
				}
				break;
			case BT_IEEE4:
				{
					IEEE4 *pA1UP = (IEEE4*)pState1U->GetPtr(0, pState1U->GetY0());
					IEEE4 *pA2UP = (IEEE4*)pState2U->GetPtr(0, pState2U->GetY0());
					IEEE4 *pA = (IEEE4*)pState0->GetPtr(0, pState0->GetY0());
					IEEE4 *pA1DOWN = (IEEE4*)pState1D->GetPtr(0, pState1D->GetY0());
					IEEE4 *pA2DOWN = (IEEE4*)pState2D->GetPtr(0, pState2D->GetY0());
					IEEE4 *pA3DOWN = (IEEE4*)pState3D->GetPtr(0, pState3D->GetY0());
					INT32 u = u0;

#ifdef NOTYET
NCSJPC_X86_MMI
					for(; u < u1 && u < (1 + u0 / 4) * 4; u++) {
						pA2DOWN[u] *= LIFT_9X7_K;
						pA3DOWN[u] *= LIFT_9X7_1K;//1.0f / LIFT_9X7_K;
						pA2DOWN[u] -= LIFT_9X7_g * (pA1DOWN[u] + pA3DOWN[u]);
						pA1DOWN[u] -= LIFT_9X7_y * (pA[u] + pA2DOWN[u]);
						pA[u]      -= LIFT_9X7_B * (pA1UP[u] + pA1DOWN[u]);
						pA1UP[u]   -= LIFT_9X7_a * (pA2UP[u] + pA[u]);
					}
					if(CNCSJPCBuffer::sm_SystemInfo.m_bSSEPresent) {
						INT32 n = 4 * (u1 / 4);
						__m64 mm7 = _mm_set1_pi16(2);

						//pA2DOWN[u] *= LIFT_9X7_K;
						//pA3DOWN[u] *= LIFT_9X7_1K;//1.0f / LIFT_9X7_K;
						//pA2DOWN[u] -= LIFT_9X7_g * (pA1DOWN[u] + pA3DOWN[u]);
						//pA1DOWN[u] -= LIFT_9X7_y * (pA[u] + pA2DOWN[u]);
						//pA[u]      -= LIFT_9X7_B * (pA1UP[u] + pA1DOWN[u]);
						//pA1UP[u]   -= LIFT_9X7_a * (pA2UP[u] + pA[u]);
						
						for(; u < n; u += 4) {
							__m128 mm0_0 = _mm_load_ps((float*)&(pA[u]));
							__m128 mm1_1U = _mm_load_ps((float*)&(pA1UP[u]));
							__m128 mm2_2U = _mm_load_ps((float*)&(pA2UP[u]));
							__m128 mm3_1D = _mm_load_ps((float*)&(pA1DOWN[u]));
							__m128 mm4_2D = _mm_load_ps((float*)&(pA2DOWN[u]));
							__m128 mm5_3D = _mm_load_ps((float*)&(pA3DOWN[u]));

							__m128 mm7 = _mm_set1_ps(LIFT_9X7_K);
							mm4_2D = _mm_mul_ps(mm4_2D, mm7);
							mm7 = _mm_set1_ps(LIFT_9X7_1K);
							mm5_3D = _mm_mul_ps(mm5_3D, mm7);
							mm7 = _mm_set1_ps(LIFT_9X7_g);
							mm4_2D = _mm_sub_ps(mm4_2D, _mm_mul_ps(mm7, _mm_add_ps(mm3_1D, mm5_3D)));
							mm7 = _mm_set1_ps(LIFT_9X7_y);
							mm3_1D = _mm_sub_ps(mm3_1D, _mm_mul_ps(mm7, _mm_add_ps(mm0_0, mm4_2D)));
							mm7 = _mm_set1_ps(LIFT_9X7_B);
							mm0_0 = _mm_sub_ps(mm0_0, _mm_mul_ps(mm7, _mm_add_ps(mm1_1U, mm3_1D)));
							mm7 = _mm_set1_ps(LIFT_9X7_a);
							mm1_1U = _mm_sub_ps(mm1_1U, _mm_mul_ps(mm7, _mm_add_ps(mm2_2U, mm0_0)));

							_mm_store_ps((float*)&(pA[u]), mm0_0);
							_mm_store_ps((float*)&(pA1UP[u]), mm1_1U);
							_mm_store_ps((float*)&(pA2UP[u]), mm2_2U);
							_mm_store_ps((float*)&(pA1DOWN[u]), mm3_1D);
							_mm_store_ps((float*)&(pA2DOWN[u]), mm4_2D);
							_mm_store_ps((float*)&(pA3DOWN[u]), mm5_3D);
						}
						_mm_empty();
					}
#endif //NCSJPC_X86_MMI
					for(; u < u1; u++) {
						pA2DOWN[u] *= LIFT_9X7_K;
						pA3DOWN[u] *= LIFT_9X7_1K;//1.0f / LIFT_9X7_K;
						pA2DOWN[u] -= LIFT_9X7_g * (pA1DOWN[u] + pA3DOWN[u]);
						pA1DOWN[u] -= LIFT_9X7_y * (pA[u] + pA2DOWN[u]);
						pA[u]      -= LIFT_9X7_B * (pA1UP[u] + pA1DOWN[u]);
						pA1UP[u]   -= LIFT_9X7_a * (pA2UP[u] + pA[u]);
					}
				}				
				break;
		}
		if(bRet) {
			if(pDst) {
			//	pDst->Assign(pDst->GetType(),
			//				pState1U->GetPtr(pDst->GetX0(), pDst->GetY0()),
			//				pState1U->GetStep());
				bRet = pState1U->Read(pDst);
			}
			pCtx->m_nStateV += 2;
		}
	}
	return(bRet);
}

bool CNCSJPCResolution::VER_SR(ContextID nCtx,
							   INT32 u0,
							   INT32 u1,
							   INT32 v0,
							   INT32 v1,
							   CNCSJPCBuffer &Dst)
{
	Context *pCtx = (CNCSJPCResolution::Context*)GetContext(nCtx);
	bool bRet = false;
	UINT8 nBorder = 2 + 3 * CNCSJPCBuffer::Border(Dst.GetType());

	if(GetHeight() == 1) {
		CNCSJPCBuffer a(u0-nBorder, v0, u1+nBorder, v1, Dst.GetType());
		bRet = HOR_SR(nCtx, a, u0, u1, v0, v1);

		if(bRet) {
			if(Dst.GetType() != BT_IEEE4 && (GetY0() & 0x1)) {
				bRet = a.Add(0, 1);// Divide by 2
			}
			if(bRet) {
				bRet = a.Read(&Dst);
			}
		}
		return(bRet);
	} else {
		if(v0 < pCtx->m_nStateV - 2 || pCtx->m_State.GetBuffer(0) == NULL || pCtx->m_nStateU0 != u0 - nBorder || pCtx->m_nStateU1 != u1 + nBorder) {
			pCtx->m_State.SetBuffers(0);
			if(Dst.GetType() == BT_IEEE4) {
				pCtx->m_State.SetBuffers(6);
				INT32 nStateV0 = (v0 / 2) * 2;
				pCtx->m_nStateU0 = u0 - nBorder;
				pCtx->m_nStateU1 = u1 + nBorder;
				pCtx->m_nStateV = nStateV0-4;
			} else {
				pCtx->m_State.SetBuffers(4);
				INT32 nStateV0 = (v0 / 2) * 2;
				pCtx->m_nStateU0 = u0 - nBorder;
				pCtx->m_nStateU1 = u1 + nBorder;
				pCtx->m_nStateV = nStateV0-2;
			}
			bRet = VER_SR_INPUT2(nCtx, u0, u1, NULL, Dst.GetType());
		}
		while(pCtx->m_nStateV <= v0) {
				bRet = VER_SR_INPUT2(nCtx, u0, u1, NULL, Dst.GetType());
		}
		if(v0 % 2 == 1) {
			bRet = VER_SR_INPUT2(nCtx, u0, u1, &Dst, Dst.GetType());
		} else {
			UINT32 nBuffer;
			bool bFound;
			CNCSJPCRect rect(Dst.GetX0() - nBorder, Dst.GetY0(), Dst.GetX1() + nBorder, Dst.GetY1());
			CNCSJPCBuffer *pSrc = pCtx->m_State.FindBuffer(nBuffer, rect, bFound, Dst.GetType());
			
		//	Dst.Assign(Dst.GetType(),
		//			   pSrc->GetPtr(Dst.GetX0(), Dst.GetY0()),
		//			   pSrc->GetStep());
			bRet = pSrc->Read(&Dst);
		}
		return(bRet);
	}
}

bool CNCSJPCResolution::SR_2D(ContextID nCtx,
							  INT32 u0,
							  INT32 u1,
							  INT32 v0,
							  INT32 v1,
							  CNCSJPCBuffer &Dst)
{
	Context *pCtx = (CNCSJPCResolution::Context*)GetContext(nCtx);

	bool bRet = true;
	if(pCtx->m_OutputBuffer.IsValid() == false || 
	   (pCtx->m_OutputBuffer.IsValid() && pCtx->m_OutputBuffer != Dst)) {
		pCtx->m_OutputBuffer.Alloc(Dst.GetX0(), Dst.GetY0(), Dst.GetX1(), Dst.GetY1(), Dst.GetType());
		bRet = VER_SR(nCtx, u0, u1, v0, v1, pCtx->m_OutputBuffer);
	}
	if(Dst.TestFlags(CNCSJPCBuffer::AT_LOCKED)) {
		pCtx->m_OutputBuffer.Read(&Dst);
	} else {
		Dst.Assign(Dst.GetX0(), Dst.GetY0(), 
				Dst.GetX1(), Dst.GetY1(), 
				Dst.GetType(), 
				pCtx->m_OutputBuffer.GetPtr(Dst.GetX0(), Dst.GetY0()), 
				pCtx->m_OutputBuffer.GetStep());
	}
	return(bRet);
}

bool CNCSJPCResolution::SD_2D_OUTPUT4(ContextID nCtx, 
									  INT32 u0, 
									  INT32 u1, 
									  INT32 nV, 
									  CNCSJPCBuffer::Type eType)
{
	Context *pCtx = (CNCSJPCResolution::Context*)GetContext(nCtx);
	bool bRet = true;
	bool bFound = false;
	UINT32 nBuffer = 0;

	INT32 v0 = GetY0();
	INT32 v1 = GetY1();

	// LL HL
	// LH HH
	CNCSJPCBuffer LL;
	LL.Assign(NCSCeilDiv(u0, 2), 
			  NCSCeilDiv(nV, 2), 
			  (UINT32)(NCSCeilDiv(u1, 2) - NCSCeilDiv(u0, 2)), 
			  (UINT32)1, 
			  eType,
			  m_EncoderLL.GetPtr(NCSCeilDiv(u0, 2), NCSCeilDiv(v0, 2)));
	CNCSJPCBuffer HL;
	HL.Assign(NCSFloorDiv(u0, 2), 
			  NCSCeilDiv(nV, 2), 
			  (UINT32)(NCSFloorDiv(u1, 2) - NCSFloorDiv(u0, 2)), 
			  (UINT32)1, 
			  eType,
			  m_EncoderHL.GetPtr(NCSCeilDiv(u0, 2), NCSCeilDiv(v0, 2) + NCSCeilDiv(nV - v0, 2) % GetCBHeight()));
	CNCSJPCBuffer LH;
	LH.Assign(NCSCeilDiv(u0, 2), 
			  NCSFloorDiv(nV, 2), 
			  (UINT32)(NCSCeilDiv(u1, 2) - NCSCeilDiv(u0, 2)), 
			  (UINT32)1, 
			  eType,
			  m_EncoderLH.GetPtr(NCSCeilDiv(u0, 2), NCSCeilDiv(v0, 2) + NCSFloorDiv(nV - v0, 2) % GetCBHeight()));
	CNCSJPCBuffer HH;
	HH.Assign(NCSFloorDiv(u0, 2), 
			  NCSFloorDiv(nV, 2), 
			  (UINT32)(NCSFloorDiv(u1, 2) - NCSFloorDiv(u0, 2)), 
			  (UINT32)1, 
			  eType,
			  m_EncoderHH.GetPtr(NCSCeilDiv(u0, 2), NCSCeilDiv(v0, 2) + NCSFloorDiv(nV - v0, 2) % GetCBHeight()));

	UINT32 nWidth = GetWidth();
	UINT32 nHeight = GetHeight();

	if(eType == BT_INT16) {
		CNCSJPCRect r1(u0 - 2, PSE0(nV - 2, v0, v1), (UINT32)(u1 - u0) + 4, (UINT32)1);
		CNCSJPCRect r2(u0 - 2, PSE0(nV - 1, v0, v1), (UINT32)(u1 - u0) + 4, (UINT32)1);
		CNCSJPCRect r3(u0 - 2, PSE0(nV, v0, v1), (UINT32)(u1 - u0) + 4, (UINT32)1);
		CNCSJPCRect r4(u0 - 2, PSE0(nV + 1, v0, v1), (UINT32)(u1 - u0) + 4, (UINT32)1);
		CNCSJPCRect r5(u0 - 2, PSE0(nV + 2, v0, v1), (UINT32)(u1 - u0) + 4, (UINT32)1);
		
		INT16 *pSrc2Up = (INT16*)pCtx->m_State.FindBuffer(nBuffer, (CNCSJPCRect&)r1, bFound, eType)->GetPtr(u0, PSE0(nV - 2, v0, v1));
		INT16 *pSrc1Up = (INT16*)pCtx->m_State.FindBuffer(nBuffer, (CNCSJPCRect&)r2, bFound, eType)->GetPtr(u0, PSE0(nV - 1, v0, v1));
		INT16 *pSrc0 = (INT16*)pCtx->m_State.FindBuffer(nBuffer, (CNCSJPCRect&)r3, bFound, eType)->GetPtr(u0, PSE0(nV, v0, v1));
		INT16 *pSrc1Down = (INT16*)pCtx->m_State.FindBuffer(nBuffer, (CNCSJPCRect&)r4, bFound, eType)->GetPtr(u0, PSE0(nV + 1, v0, v1));
		INT16 *pSrc2Down = (INT16*)pCtx->m_State.FindBuffer(nBuffer, (CNCSJPCRect&)r5, bFound, eType)->GetPtr(u0, PSE0(nV + 2, v0, v1));

		INT16 *pLL = (INT16*)LL.GetPtr();
		INT16 *pHL = (INT16*)HL.GetPtr();
		INT16 *pLH = (INT16*)LH.GetPtr();
		INT16 *pHH = (INT16*)HH.GetPtr();

		
		//
		// 5X3 Filter lifting
		//
		//Step2     1   3   5   7     
		//           \ /|\ /|\ /|\
		//Step1       0 | 2 | 4 | 6            
		//           /|\|/|\|/|\|/|\
		//Index:  2 1 0 1 2 3 4 5 6 7
		//
		//Step2 == Odd Output
		//Step1 == Even Output
		//Step0-1 == 2State.
		//

		INT32 n = 0;

#ifdef NCSJPC_X86_MMI
		if(CNCSJPCBuffer::sm_SystemInfo.m_bSSE2Present && (nWidth / 8) > 1) {
			UINT32 nSubWidth = (nWidth / 8) - 1;

			//      -2    -1     0     1     3     4
			// S2U               
			// S1U               
			// S0   P00   P01    LL    HL
			// S1D  P01   P11    LH    HH
			// S2D

			__m128i *pSrc2Up_128i = (__m128i*)(pSrc2Up-2);
			__m128i *pSrc1Up_128i = (__m128i*)(pSrc1Up-2);
			__m128i *pSrc0_128i = (__m128i*)(pSrc0-2);
			__m128i *pSrc1Down_128i = (__m128i*)(pSrc1Down-2);
			__m128i *pSrc2Down_128i = (__m128i*)(pSrc2Down-2);

			__m128i mm1_VS1U = _mm_sub_epi16(pSrc1Up_128i[0], _mm_srai_epi16(_mm_add_epi16(pSrc2Up_128i[0], pSrc0_128i[0]), 1));
			__m128i mm0_VS1D = _mm_sub_epi16(pSrc1Down_128i[0], _mm_srai_epi16(_mm_add_epi16(pSrc0_128i[0], pSrc2Down_128i[0]), 1));
			__m128i mm2_VS0 = _mm_add_epi16(pSrc0_128i[0], _mm_srai_epi16(_mm_add_epi16(_mm_add_epi16(mm1_VS1U, mm0_VS1D), _mm_set1_epi16(2)), 2));
			
			//            -2    -1     0     1     3     4     5     6     7
			// S2U               
			// S1U               
			// mm2_VS0    0     1      2     3     4     5     6     7     0     1      2     3     4     5     6     7
			// mm0_VS1D   0     1      2     3     4     5     6     7     0     1      2     3     4     5     6     7
			// S2D

	//		INT16 nVert01 = pSrc1Down[n2n] - NCSFloorDiv(pSrc0[n2n] + pSrc2Down[n2n], 2);
	//		INT16 nVert00 = pSrc0[n2n] + NCSFloorDiv(pSrc1Up[n2n] - NCSFloorDiv(pSrc2Up[n2n] + pSrc0[n2n], 2) + nVert01, 4);
		
			pHL[-1] = (INT16)_mm_extract_epi16(mm2_VS0, 1) - NCSFloorDiv((INT16)_mm_extract_epi16(mm2_VS0, 0) + (INT16)_mm_extract_epi16(mm2_VS0, 2), 2);
			pHH[-1] = (INT16)_mm_extract_epi16(mm0_VS1D, 1) - NCSFloorDiv((INT16)_mm_extract_epi16(mm0_VS1D, 0) + (INT16)_mm_extract_epi16(mm0_VS1D, 2), 2);
		
			for(; n < (INT32)nSubWidth; n++) {
				__m128i mm1_VS1U_next = _mm_sub_epi16(pSrc1Up_128i[n+1], _mm_srai_epi16(_mm_add_epi16(pSrc2Up_128i[n+1], pSrc0_128i[n+1]), 1));
				__m128i mm0_VS1D_next = _mm_sub_epi16(pSrc1Down_128i[n+1], _mm_srai_epi16(_mm_add_epi16(pSrc0_128i[n+1], pSrc2Down_128i[n+1]), 1));
				__m128i mm2_VS0_next = _mm_add_epi16(pSrc0_128i[n+1], _mm_srai_epi16(_mm_add_epi16(_mm_add_epi16(mm1_VS1U_next, mm0_VS1D_next), _mm_set1_epi16(2)), 2));

				//FIXME - this could be much better
				pHL[n * 4] = (INT16)_mm_extract_epi16(mm2_VS0, 3) - NCSFloorDiv((INT16)_mm_extract_epi16(mm2_VS0, 2) + (INT16)_mm_extract_epi16(mm2_VS0, 4), 2);
				pLL[n * 4] = (INT16)_mm_extract_epi16(mm2_VS0, 2) + NCSFloorDiv(pHL[n * 4 - 1] + pHL[n * 4 + 0] + 2, 4);
				pHL[n * 4 + 1] = (INT16)_mm_extract_epi16(mm2_VS0, 5) - NCSFloorDiv((INT16)_mm_extract_epi16(mm2_VS0, 4) + (INT16)_mm_extract_epi16(mm2_VS0, 6), 2);
				pLL[n * 4 + 1] = (INT16)_mm_extract_epi16(mm2_VS0, 4) + NCSFloorDiv(pHL[n * 4 + 0] + pHL[n * 4 + 1] + 2, 4);
				pHL[n * 4 + 2] = (INT16)_mm_extract_epi16(mm2_VS0, 7) - NCSFloorDiv((INT16)_mm_extract_epi16(mm2_VS0, 6) + (INT16)_mm_extract_epi16(mm2_VS0_next, 0), 2);
				pLL[n * 4 + 2] = (INT16)_mm_extract_epi16(mm2_VS0, 6) + NCSFloorDiv(pHL[n * 4 + 1] + pHL[n * 4 + 2] + 2, 4);
				pHL[n * 4 + 3] = (INT16)_mm_extract_epi16(mm2_VS0_next, 1) - NCSFloorDiv((INT16)_mm_extract_epi16(mm2_VS0_next, 0) + (INT16)_mm_extract_epi16(mm2_VS0_next, 2), 2);
				pLL[n * 4 + 3] = (INT16)_mm_extract_epi16(mm2_VS0_next, 0) + NCSFloorDiv(pHL[n * 4 + 2] + pHL[n * 4 + 3] + 2, 4);

				pHH[n * 4] = (INT16)_mm_extract_epi16(mm0_VS1D, 3) - NCSFloorDiv((INT16)_mm_extract_epi16(mm0_VS1D, 2) + (INT16)_mm_extract_epi16(mm0_VS1D, 4), 2);
				pLH[n * 4] = (INT16)_mm_extract_epi16(mm0_VS1D, 2) + NCSFloorDiv(pHH[n * 4 - 1] + pHH[n * 4 + 0] + 2, 4);
				pHH[n * 4 + 1] = (INT16)_mm_extract_epi16(mm0_VS1D, 5) - NCSFloorDiv((INT16)_mm_extract_epi16(mm0_VS1D, 4) + (INT16)_mm_extract_epi16(mm0_VS1D, 6), 2);
				pLH[n * 4 + 1] = (INT16)_mm_extract_epi16(mm0_VS1D, 4) + NCSFloorDiv(pHH[n * 4 + 0] + pHH[n * 4 + 1] + 2, 4);
				pHH[n * 4 + 2] = (INT16)_mm_extract_epi16(mm0_VS1D, 7) - NCSFloorDiv((INT16)_mm_extract_epi16(mm0_VS1D, 6) + (INT16)_mm_extract_epi16(mm0_VS1D_next, 0), 2);
				pLH[n * 4 + 2] = (INT16)_mm_extract_epi16(mm0_VS1D, 6) + NCSFloorDiv(pHH[n * 4 + 1] + pHH[n * 4 + 2] + 2, 4);
				pHH[n * 4 + 3] = (INT16)_mm_extract_epi16(mm0_VS1D_next, 1) - NCSFloorDiv((INT16)_mm_extract_epi16(mm0_VS1D_next, 0) + (INT16)_mm_extract_epi16(mm0_VS1D_next, 2), 2);
				pLH[n * 4 + 3] = (INT16)_mm_extract_epi16(mm0_VS1D_next, 0) + NCSFloorDiv(pHH[n * 4 + 2] + pHH[n * 4 + 3] + 2, 4);

				mm1_VS1U = mm1_VS1U_next;
				mm0_VS1D = mm0_VS1D_next;
				mm2_VS0 = mm2_VS0_next; 
			}
			n *= 4;
#ifdef NCSJPC_X86_MMI_MMX
			_mm_empty();
		} else if(CNCSJPCBuffer::sm_SystemInfo.m_bSSEPresent && (nWidth / 4) > 1) {
			UINT32 nSubWidth = (nWidth / 4) - 1;

			//      -2    -1     0     1     3     4
			// S2U               
			// S1U               
			// S0   P00   P01    LL    HL
			// S1D  P01   P11    LH    HH
			// S2D

			__m64 *pSrc2Up_128i = (__m64*)(pSrc2Up-2);
			__m64 *pSrc1Up_128i = (__m64*)(pSrc1Up-2);
			__m64 *pSrc0_128i = (__m64*)(pSrc0-2);
			__m64 *pSrc1Down_128i = (__m64*)(pSrc1Down-2);
			__m64 *pSrc2Down_128i = (__m64*)(pSrc2Down-2);

			__m64 mm1_VS1U = _mm_sub_pi16(pSrc1Up_128i[0], _mm_srai_pi16(_mm_add_pi16(pSrc2Up_128i[0], pSrc0_128i[0]), 1));
			__m64 mm0_VS1D = _mm_sub_pi16(pSrc1Down_128i[0], _mm_srai_pi16(_mm_add_pi16(pSrc0_128i[0], pSrc2Down_128i[0]), 1));
			__m64 mm2_VS0 = _mm_add_pi16(pSrc0_128i[0], _mm_srai_pi16(_mm_add_pi16(_mm_add_pi16(mm1_VS1U, mm0_VS1D), _mm_set1_pi16(2)), 2));
			
			//            -2    -1     0     1     3     4     5     6     7
			// S2U               
			// S1U               
			// mm2_VS0    0     1      2     3     0     1      2     3        
			// mm0_VS1D   0     1      2     3     0     1      2     3      
			// S2D

			pHL[-1] = (INT16)_mm_extract_pi16(mm2_VS0, 1) - NCSFloorDiv((INT16)_mm_extract_pi16(mm2_VS0, 0) + (INT16)_mm_extract_pi16(mm2_VS0, 2), 2);
			pHH[-1] = (INT16)_mm_extract_pi16(mm0_VS1D, 1) - NCSFloorDiv((INT16)_mm_extract_pi16(mm0_VS1D, 0) + (INT16)_mm_extract_pi16(mm0_VS1D, 2), 2);
		
			for(; n < (INT32)nSubWidth; n++) {
				__m64 mm1_VS1U_next = _mm_sub_pi16(pSrc1Up_128i[n+1], _mm_srai_pi16(_mm_add_pi16(pSrc2Up_128i[n+1], pSrc0_128i[n+1]), 1));
				__m64 mm0_VS1D_next = _mm_sub_pi16(pSrc1Down_128i[n+1], _mm_srai_pi16(_mm_add_pi16(pSrc0_128i[n+1], pSrc2Down_128i[n+1]), 1));
				__m64 mm2_VS0_next = _mm_add_pi16(pSrc0_128i[n+1], _mm_srai_pi16(_mm_add_pi16(_mm_add_pi16(mm1_VS1U_next, mm0_VS1D_next), _mm_set1_pi16(2)), 2));

				pHL[n * 2] = (INT16)_mm_extract_pi16(mm2_VS0, 3) - NCSFloorDiv((INT16)_mm_extract_pi16(mm2_VS0, 2) + (INT16)_mm_extract_pi16(mm2_VS0_next, 0), 2);
				pLL[n * 2] = (INT16)_mm_extract_pi16(mm2_VS0, 2) + NCSFloorDiv(pHL[n * 2 - 1] + pHL[n * 2] + 2, 4);
				pHL[n * 2 + 1] = (INT16)_mm_extract_pi16(mm2_VS0_next, 1) - NCSFloorDiv((INT16)_mm_extract_pi16(mm2_VS0_next, 0) + (INT16)_mm_extract_pi16(mm2_VS0_next, 2), 2);
				pLL[n * 2 + 1] = (INT16)_mm_extract_pi16(mm2_VS0_next, 0) + NCSFloorDiv(pHL[n * 2] + pHL[n * 2 + 1] + 2, 4);

				pHH[n * 2] = (INT16)_mm_extract_pi16(mm0_VS1D, 3) - NCSFloorDiv((INT16)_mm_extract_pi16(mm0_VS1D, 2) + (INT16)_mm_extract_pi16(mm0_VS1D_next, 0), 2);
				pLH[n * 2] = (INT16)_mm_extract_pi16(mm0_VS1D, 2) + NCSFloorDiv(pHH[n * 2 - 1] + pHH[n * 2] + 2, 4);
				pHH[n * 2 + 1] = (INT16)_mm_extract_pi16(mm0_VS1D_next, 1) - NCSFloorDiv((INT16)_mm_extract_pi16(mm0_VS1D_next, 0) + (INT16)_mm_extract_pi16(mm0_VS1D_next, 2), 2);
				pLH[n * 2 + 1] = (INT16)_mm_extract_pi16(mm0_VS1D_next, 0) + NCSFloorDiv(pHH[n * 2] + pHH[n * 2 + 1] + 2, 4);
			
				mm0_VS1D = mm0_VS1D_next;
				mm2_VS0 = mm2_VS0_next;
			}
			n *= 2;
			_mm_empty();
#endif NCSJPC_X86_MMI_MMX
		}
#endif //NCSJPC_X86_MMI
		nWidth = NCSCeilDiv(nWidth, 2);

		INT32 n2n = n * 2 - 2;
			
		INT16 nPrevVert01 = pSrc1Down[n2n] - NCSFloorDiv(pSrc0[n2n] + pSrc2Down[n2n], 2);
		INT16 nPrevVert11 = pSrc1Down[n2n+1] - NCSFloorDiv(pSrc0[n2n+1] + pSrc2Down[n2n+1], 2);
		INT16 nPrevVert00 = pSrc0[n2n] + NCSFloorDiv(pSrc1Up[n2n] - NCSFloorDiv(pSrc2Up[n2n] + pSrc0[n2n], 2) + nPrevVert01 + 2, 4);
		INT16 nPrevVert10 = pSrc0[n2n+1] + NCSFloorDiv(pSrc1Up[n2n+1] - NCSFloorDiv(pSrc2Up[n2n+1] + pSrc0[n2n+1], 2) + nPrevVert11 + 2, 4);

		n2n += 2;
		
		INT16 nVert01 = pSrc1Down[n2n] - NCSFloorDiv(pSrc0[n2n] + pSrc2Down[n2n], 2);
		INT16 nVert00 = pSrc0[n2n] + NCSFloorDiv(pSrc1Up[n2n] - NCSFloorDiv(pSrc2Up[n2n] + pSrc0[n2n], 2) + nVert01 + 2, 4);

		for(; n < (INT32)nWidth; n++) {
			n2n = 2 * n;

			INT16 nVert11 = pSrc1Down[n2n+1] - NCSFloorDiv(pSrc0[n2n+1] + pSrc2Down[n2n+1], 2);
			INT16 nVert10 = pSrc0[n2n+1] + NCSFloorDiv(pSrc1Up[n2n+1] - NCSFloorDiv(pSrc2Up[n2n+1] + pSrc0[n2n+1], 2) + nVert11 + 2, 4);

			INT16 nNextVert01 = pSrc1Down[n2n+2] - NCSFloorDiv(pSrc0[n2n+2] + pSrc2Down[n2n+2], 2);
			INT16 nNextVert00 = pSrc0[n2n+2] + NCSFloorDiv(pSrc1Up[n2n+2] - NCSFloorDiv(pSrc2Up[n2n+2] + pSrc0[n2n+2], 2) + nNextVert01 + 2, 4);

			pHL[n] = nVert10 - NCSFloorDiv(nVert00 + nNextVert00, 2);
			pHH[n] = nVert11 - NCSFloorDiv(nVert01 + nNextVert01, 2);
			pLL[n] = nVert00 + NCSFloorDiv(nPrevVert10 - NCSFloorDiv(nPrevVert00 + nVert00, 2) + pHL[n] + 2, 4);
			pLH[n] = nVert01 + NCSFloorDiv(nPrevVert11 - NCSFloorDiv(nPrevVert01 + nVert01, 2) + pHH[n] + 2, 4);

			nPrevVert01 = nVert01;
			nPrevVert11 = nVert11;
			nPrevVert00 = nVert00;
			nPrevVert10 = nVert10;
			nVert01 = nNextVert01;
			nVert00 = nNextVert00;
		}
	} else {
		CNCSJPCRect r1(u0 - 2, PSE0(nV - 2, v0, v1), (UINT32)(u1 - u0) + 4, (UINT32)1);
		CNCSJPCRect r2(u0 - 2, PSE0(nV - 1, v0, v1), (UINT32)(u1 - u0) + 4, (UINT32)1);
		CNCSJPCRect r3(u0 - 2, PSE0(nV, v0, v1), (UINT32)(u1 - u0) + 4, (UINT32)1);
		CNCSJPCRect r4(u0 - 2, PSE0(nV + 1, v0, v1), (UINT32)(u1 - u0) + 4, (UINT32)1);
		CNCSJPCRect r5(u0 - 2, PSE0(nV + 2, v0, v1), (UINT32)(u1 - u0) + 4, (UINT32)1);
		
		INT32 *pSrc2Up = (INT32*)pCtx->m_State.FindBuffer(nBuffer, (CNCSJPCRect&)r1, bFound, eType)->GetPtr(u0, PSE0(nV - 2, v0, v1));
		INT32 *pSrc1Up = (INT32*)pCtx->m_State.FindBuffer(nBuffer, (CNCSJPCRect&)r2, bFound, eType)->GetPtr(u0, PSE0(nV - 1, v0, v1));
		INT32 *pSrc0 = (INT32*)pCtx->m_State.FindBuffer(nBuffer, (CNCSJPCRect&)r3, bFound, eType)->GetPtr(u0, PSE0(nV, v0, v1));
		INT32 *pSrc1Down = (INT32*)pCtx->m_State.FindBuffer(nBuffer, (CNCSJPCRect&)r4, bFound, eType)->GetPtr(u0, PSE0(nV + 1, v0, v1));
		INT32 *pSrc2Down = (INT32*)pCtx->m_State.FindBuffer(nBuffer, (CNCSJPCRect&)r5, bFound, eType)->GetPtr(u0, PSE0(nV + 2, v0, v1));

		INT32 *pLL = (INT32*)LL.GetPtr();
		INT32 *pHL = (INT32*)HL.GetPtr();
		INT32 *pLH = (INT32*)LH.GetPtr();
		INT32 *pHH = (INT32*)HH.GetPtr();

		
		//
		// 5X3 Filter lifting
		//
		//Step2     1   3   5   7     
		//           \ /|\ /|\ /|\
		//Step1       0 | 2 | 4 | 6            
		//           /|\|/|\|/|\|/|\
		//Index:  2 1 0 1 2 3 4 5 6 7
		//
		//Step2 == Odd Output
		//Step1 == Even Output
		//Step0-1 == 2State.
		//

		UINT32 n = 0;
		nWidth = NCSCeilDiv(nWidth, 2);

		UINT32 n2n = n * 2 - 2;
			
		INT32 nPrevVert01 = pSrc1Down[n2n] - NCSFloorDiv(pSrc0[n2n] + pSrc2Down[n2n], 2);
		INT32 nPrevVert11 = pSrc1Down[n2n+1] - NCSFloorDiv(pSrc0[n2n+1] + pSrc2Down[n2n+1], 2);
		INT32 nPrevVert00 = pSrc0[n2n] + NCSFloorDiv(pSrc1Up[n2n] - NCSFloorDiv(pSrc2Up[n2n] + pSrc0[n2n], 2) + nPrevVert01 + 2, 4);
		INT32 nPrevVert10 = pSrc0[n2n+1] + NCSFloorDiv(pSrc1Up[n2n+1] - NCSFloorDiv(pSrc2Up[n2n+1] + pSrc0[n2n+1], 2) + nPrevVert11 + 2, 4);

		n2n += 2;
		
		INT32 nVert01 = pSrc1Down[n2n] - NCSFloorDiv(pSrc0[n2n] + pSrc2Down[n2n], 2);
		INT32 nVert00 = pSrc0[n2n] + NCSFloorDiv(pSrc1Up[n2n] - NCSFloorDiv(pSrc2Up[n2n] + pSrc0[n2n], 2) + nVert01 + 2, 4);

		for(; n < nWidth; n++) {
			n2n = 2 * n;

			INT32 nVert11 = pSrc1Down[n2n+1] - NCSFloorDiv(pSrc0[n2n+1] + pSrc2Down[n2n+1], 2);
			INT32 nVert10 = pSrc0[n2n+1] + NCSFloorDiv(pSrc1Up[n2n+1] - NCSFloorDiv(pSrc2Up[n2n+1] + pSrc0[n2n+1], 2) + nVert11 + 2, 4);

			INT32 nNextVert01 = pSrc1Down[n2n+2] - NCSFloorDiv(pSrc0[n2n+2] + pSrc2Down[n2n+2], 2);
			INT32 nNextVert00 = pSrc0[n2n+2] + NCSFloorDiv(pSrc1Up[n2n+2] - NCSFloorDiv(pSrc2Up[n2n+2] + pSrc0[n2n+2], 2) + nNextVert01 + 2, 4);

			pHL[n] = nVert10 - NCSFloorDiv(nVert00 + nNextVert00, 2);
			pHH[n] = nVert11 - NCSFloorDiv(nVert01 + nNextVert01, 2);
			pLL[n] = nVert00 + NCSFloorDiv(nPrevVert10 - NCSFloorDiv(nPrevVert00 + nVert00, 2) + pHL[n] + 2, 4);
			pLH[n] = nVert01 + NCSFloorDiv(nPrevVert11 - NCSFloorDiv(nPrevVert01 + nVert01, 2) + pHH[n] + 2, 4);

			nPrevVert01 = nVert01;
			nPrevVert11 = nVert11;
			nPrevVert00 = nVert00;
			nPrevVert10 = nVert10;
			nVert01 = nNextVert01;
			nVert00 = nNextVert00;
		}
	}
	
	if(nV < v1 - 1) {
        		if(u1 % 2 == 0) {
			bRet &= WriteSubBandLine(nCtx, &HH, NCSJPC_HH);
		} else {
			CNCSJPCBuffer tmp;
			tmp.Assign(HH.GetX0(), HH.GetY0(), HH.GetWidth() - 1, HH.GetHeight(), HH.GetType(), HH.GetPtr());
			bRet &= WriteSubBandLine(nCtx, &tmp, NCSJPC_HH);
		}
		bRet &= WriteSubBandLine(nCtx, &LH, NCSJPC_LH);
	}
	if(u1 % 2 == 0) {
		bRet &= WriteSubBandLine(nCtx, &HL, NCSJPC_HL);
	} else {
		CNCSJPCBuffer tmp;
		tmp.Assign(HL.GetX0(), HL.GetY0(), HL.GetWidth() - 1, HL.GetHeight(), HL.GetType(), HL.GetPtr());
		bRet &= WriteSubBandLine(nCtx, &tmp, NCSJPC_HL);
	}
	bRet &= WriteSubBandLine(nCtx, &LL, NCSJPC_LL);
	return(bRet);
}

bool CNCSJPCResolution::SD_2D(ContextID nCtx,
							  CNCSJPCBuffer *pSrc)
{
	Context *pCtx = (CNCSJPCResolution::Context*)GetContext(nCtx);
	bool bRet = true;

	INT32 u0 = GetX0();
	INT32 u1 = GetX1();
	INT32 v0 = GetY0();
	INT32 v1 = GetY1();
	INT32 nSrcX0 = NCSMax(pSrc->GetX0(), GetX0());
	INT32 nSrcX1 = NCSMin(pSrc->GetX1(), GetX1());
	INT32 nSrcY0 = pSrc->GetY0();
	UINT32 nSrcWidth = NCSMin(pSrc->GetWidth(), GetWidth());

	if(pCtx->m_State.GetBuffer(0) == NULL) {
		pCtx->m_State.SetBuffers(5);
	}
	bool bFound = false;
	UINT32 nBuffer = 0;
	CNCSJPCRect rect(nSrcX0 - 2, nSrcY0, nSrcWidth + 4, (UINT32)1);
	CNCSJPCBuffer *pThis = pCtx->m_State.FindBuffer(nBuffer, rect, bFound, pSrc->GetType());
	if(pThis) {
		void *pDst = pThis->GetPtr(nSrcX0, nSrcY0);
		memcpy(pDst, pSrc->GetPtr(u0, nSrcY0), Size(pSrc->GetType(), nSrcWidth));

		pDst = pThis->GetPtr(0, nSrcY0);
		if(pSrc->GetType() == BT_INT16) {
			PSE_EXTEND(u0-2, u0, u0, u1, (INT16*)pDst);
			PSE_EXTEND(u1, u1 + 2, u0, u1, (INT16*)pDst);
		} else {
			PSE_EXTEND(u0-2, u0, u0, u1, (INT32*)pDst);
			PSE_EXTEND(u1, u1 + 2, u0, u1, (INT32*)pDst);
		}
	}
// here it determines whether it can perform lifting or  it should wait for another line
	if(nSrcY0 >= v0 + 2 && (nSrcY0 % 2 == 0)) {
		INT32 nV = nSrcY0 - (2);
		bRet = SD_2D_OUTPUT4(nCtx, nSrcX0, nSrcX1, nV, pSrc->GetType());
	}
	if(nSrcY0 >= v1 - 1) {
		if(nSrcY0 % 2 == 0) {
			INT32 nV = nSrcY0;
			bRet = SD_2D_OUTPUT4(nCtx, nSrcX0, nSrcX1, nV, pSrc->GetType());
		} else {
			INT32 nV = nSrcY0 - 1;
			bRet = SD_2D_OUTPUT4(nCtx, nSrcX0, nSrcX1, nV, pSrc->GetType());
		}
	}
	return(bRet);
}

#ifdef NOTUSED
bool CNCSJPCResolution::InvRow(CNCSJPCBuffer *pInputs, CNCSJPCBuffer *pOutput, bool bLowFirst)
{
	UINT32 nInputWidth = (pOutput->GetWidth() >> 1) + (pOutput->GetWidth() & 0x1 ? 1 : 0);
	bool bRet = false;

	if(pInputs && pOutput) {
		UINT32 x = 0;
		UINT8 nBorder = CNCSJPCBuffer::Border(pOutput->GetType());

		switch(pOutput->GetType()) {
			case CNCSJPCBuffer::BT_INT16:
#ifdef NCSJPC_X86_MMI
				
					if(CNCSJPCBuffer::sm_SystemInfo.m_bSSEPresent) {
						INT16 *pLow = (INT16*)pInputs->GetPtr() + nBorder;
						INT16 *pHigh = (INT16*)pInputs->GetPtr() + pInputs->GetStep() + nBorder;
						INT16 *pDst = (INT16*)pOutput->GetPtr();
						UINT32 n = nInputWidth / 4;
						
						__m64 mm5 = _mm_set1_pi16(2);

						if(!bLowFirst) {
							INT16 nPrevEven = pLow[x - 1] - ((pHigh[x - 1] + pHigh[x] + 2) >> 2);
							__m64 mm7 = _mm_set1_pi16(nPrevEven);

							for(;x < n; x++) {
								__m64 mm0 = ((__m64*)(pLow))[x];
								__m64 mm1 = ((__m64*)(pHigh))[x];
								__m64 mm2 = ((__m64*)(pHigh + 1))[x];
								__m64 mm3 = _mm_adds_pi16(mm1, mm2);
								__m64 mm4 = _mm_adds_pi16(mm3, mm5);
								mm3 = _mm_srai_pi16 (mm4, 2);
								__m64 mm6 = _mm_subs_pi16(mm0, mm3); //mm6 now has 4 even outputs
								mm3 = _mm_shuffle_pi16(mm6, _MM_SHUFFLE(2, 1, 0, 0));
								int v = _mm_extract_pi16(mm7 , 3);
								mm7 = _mm_insert_pi16(mm3, v, 0); //mm7 holds 4 previous even outputs (x - 1)
								mm0 = _mm_adds_pi16(mm6, mm7);
								mm7 = mm6;
								mm3 = _mm_srai_pi16(mm0, 1);
								mm0 = _mm_adds_pi16(mm1, mm3); // mm0 holds 4 odd outputs
				
								mm1 = _mm_unpacklo_pi16(mm0, mm6);
								((__m64*)(pDst))[x*2] = mm1;
								mm1 = _mm_unpackhi_pi16(mm0, mm6);
								((__m64*)(pDst))[x*2+1] = mm1;
							}
						} else {
/*							INT16 nPrevEven = pLow[x - 1] - ((pHigh[x - 1] + pHigh[x] + 2) >> 2);
							__m64 mm7 = _mm_set1_pi16(nPrevEven);

							for(;x < n; x++) {
								__m64 mm0 = ((__m64*)(pLow))[x];
								__m64 mm1 = ((__m64*)(pHigh))[x];
								__m64 mm2 = ((__m64*)(pHigh + 1))[x];
								__m64 mm3 = _mm_adds_pi16(mm1, mm2);
								__m64 mm4 = _mm_adds_pi16(mm3, mm5);
								mm3 = _mm_srai_pi16 (mm4, 2);
								__m64 mm6 = _mm_subs_pi16(mm0, mm3); //mm6 now has 4 even outputs
								mm3 = _mm_shuffle_pi16(mm6, _MM_SHUFFLE(2, 1, 0, 0));
								int v = _mm_extract_pi16(mm7 , 3);
								mm7 = _mm_insert_pi16(mm3, v, 0); //mm7 holds 4 previous even outputs (x - 1)
								mm0 = _mm_adds_pi16(mm6, mm7);
								mm7 = mm6;
								mm3 = _mm_srai_pi16(mm0, 1);
								mm0 = _mm_adds_pi16(mm1, mm3); // mm0 holds 4 odd outputs
				
								mm1 = _mm_unpacklo_pi16(mm0, mm6);
								((__m64*)(pDst))[x*2] = mm1;
								mm1 = _mm_unpackhi_pi16(mm0, mm6);
								((__m64*)(pDst))[x*2+1] = mm1;
							}*/
						}
						x *= 4;
						_mm_empty();
					}
#endif // NCSJPC_X86_MMI
					return(TInvRow_I((INT16*)pInputs->GetPtr() + nBorder,
									(INT16*)pInputs->GetPtr() + pInputs->GetStep() + nBorder,
									(INT16*)pOutput->GetPtr(),
									x,
									nInputWidth,
									bLowFirst));
				break;
			case CNCSJPCBuffer::BT_INT32:
					return(TInvRow_I((INT32*)pInputs->GetPtr() + nBorder,
									(INT32*)pInputs->GetPtr() + pInputs->GetStep() + nBorder,
									(INT32*)pOutput->GetPtr(),
									x,
									nInputWidth,
									bLowFirst));
				break;
	
		}
	}
	return(bRet);
}

bool CNCSJPCResolution::InvCol(CNCSJPCBuffer* Low[5],
							   CNCSJPCBuffer* High[5],
							   CNCSJPCBuffer *pOutput,
							   bool bLowFirst)
{//return(true);
	bool bRet = false;

	if(pOutput) {
		UINT32 x = 0;
		switch(pOutput->GetType()) {
			case CNCSJPCBuffer::BT_INT16:
				{
					INT16 *pLow = (INT16*)Low[1]->GetPtr();
					INT16 *pLo1UP = (INT16*)Low[0]->GetPtr();
					INT16 *pLo1DOWN = (INT16*)Low[2]->GetPtr();
					INT16 *pHigh = (INT16*)High[1]->GetPtr();
					INT16 *pHi1UP = (INT16*)High[0]->GetPtr();
					INT16 *pHi1DOWN = (INT16*)High[2]->GetPtr();
					INT16 *pDst0 = (INT16*)pOutput->GetPtr();
					INT16 *pDst1 = (INT16*)pOutput->GetPtr() + pOutput->GetStep();

#ifdef NCSJPC_X86_MMI
					if(CNCSJPCBuffer::sm_SystemInfo.m_bSSE2Present) {
						UINT32 n = pOutput->GetWidth() / 8;
						__m128i mm7 = _mm_set1_epi16(2);

						if(bLowFirst) {
							for(; x < n; x++) {
								__m128i mm2 = _mm_load_si128(&((__m128i*)pHi1UP)[x]);
								__m128i mm3 = _mm_load_si128(&((__m128i*)pHigh)[x]);

								__m128i mm5 = _mm_adds_epi16(mm2, mm3);
								__m128i mm6 = _mm_adds_epi16(mm5, mm7);
								mm5 = _mm_srai_epi16(mm6, 2);
								__m128i mm0 = _mm_load_si128(&((__m128i*)pLow)[x]);
								mm6 = _mm_subs_epi16(mm0, mm5); //mm6 = nThisEven
								_mm_store_si128(&((__m128i*)pDst0)[x], mm6);
								__m128i mm4 = _mm_load_si128(&((__m128i*)pHi1DOWN)[x]);
								mm5 = _mm_adds_epi16(mm3, mm4);
								mm2 = _mm_adds_epi16(mm5, mm7);
								mm5 = _mm_srai_epi16(mm2, 2);
								__m128i mm1 = _mm_load_si128(&((__m128i*)pLo1DOWN)[x]);
								mm2 = _mm_subs_epi16(mm1, mm5); //mm2 = nNextEven
								mm5 = _mm_adds_epi16(mm6, mm2);
								mm6 = _mm_srai_epi16(mm5, 1);
								mm5 = _mm_adds_epi16(mm3, mm6); //mm5 = Dst0
								_mm_store_si128(&((__m128i*)pDst1)[x], mm5);
							}
						} else {
							for(; x < n; x++) {
								__m128i mm2 = _mm_load_si128(&((__m128i*)pHi1UP)[x]);
								__m128i mm3 = _mm_load_si128(&((__m128i*)pHigh)[x]);

								__m128i mm5 = _mm_adds_epi16(mm2, mm3);
								__m128i mm6 = _mm_adds_epi16(mm5, mm7);
								mm5 = _mm_srai_epi16(mm6, 2);
								__m128i mm0 = _mm_load_si128(&((__m128i*)pLo1UP)[x]);
								mm6 = _mm_subs_epi16(mm0, mm5); //mm6 = nPrevEven
								__m128i mm4 = _mm_load_si128(&((__m128i*)pHi1DOWN)[x]);
								mm5 = _mm_adds_epi16(mm3, mm4);
								mm2 = _mm_adds_epi16(mm5, mm7);
								mm5 = _mm_srai_epi16(mm2, 2);
								__m128i mm1 = _mm_load_si128(&((__m128i*)pLow)[x]);
								mm2 = _mm_subs_epi16(mm1, mm5); //mm2 = nNextEven
								_mm_store_si128(&((__m128i*)pDst1)[x], mm2);
								mm5 = _mm_adds_epi16(mm6, mm2);
								mm6 = _mm_srai_epi16(mm5, 1);
								mm5 = _mm_adds_epi16(mm3, mm6); //mm5 = Dst0
								_mm_store_si128(&((__m128i*)pDst0)[x], mm5);
							}
						}
						x *= 8;
						_mm_empty();
#ifdef NCSJPC_X86_MMI_MMX
					} else if(CNCSJPCBuffer::sm_SystemInfo.m_bMMXPresent) {
						UINT32 n = pOutput->GetWidth() / 4;
						__m64 mm7 = _mm_set1_pi16(2);

						if(bLowFirst) {
							for(; x < n; x++) {
								__m64 mm0 = ((__m64*)(pLow))[x];
								__m64 mm1 = ((__m64*)(pLo1DOWN))[x];
								__m64 mm2 = ((__m64*)(pHi1UP))[x];
								__m64 mm3 = ((__m64*)(pHigh))[x];
								__m64 mm4 = ((__m64*)(pHi1DOWN))[x];

								__m64 mm5 = _mm_adds_pi16(mm2, mm3);
								__m64 mm6 = _mm_adds_pi16(mm5, mm7);
								mm5 = _mm_srai_pi16(mm6, 2);
								mm6 = _mm_subs_pi16(mm0, mm5); //mm6 = nThisEven
								((__m64*)(pDst0))[x] = mm6;
								mm5 = _mm_adds_pi16(mm3, mm4);
								mm2 = _mm_adds_pi16(mm5, mm7);
								mm5 = _mm_srai_pi16(mm2, 2);
								mm2 = _mm_subs_pi16(mm1, mm5); //mm2 = nNextEven
								mm5 = _mm_adds_pi16(mm6, mm2);
								mm6 = _mm_srai_pi16(mm5, 1);
								mm5 = _mm_adds_pi16(mm3, mm6); //mm5 = Dst0
								((__m64*)(pDst1))[x] = mm5;
							}
						} else {
							for(; x < n; x++) {
								__m64 mm0 = ((__m64*)(pLo1UP))[x];
								__m64 mm1 = ((__m64*)(pLow))[x];
								__m64 mm2 = ((__m64*)(pHi1UP))[x];
								__m64 mm3 = ((__m64*)(pHigh))[x];
								__m64 mm4 = ((__m64*)(pHi1DOWN))[x];

								__m64 mm5 = _mm_adds_pi16(mm2, mm3);
								__m64 mm6 = _mm_adds_pi16(mm5, mm7);
								mm5 = _mm_srai_pi16(mm6, 2);
								mm6 = _mm_subs_pi16(mm0, mm5); //mm6 = nPrevEven
								mm5 = _mm_adds_pi16(mm3, mm4);
								mm2 = _mm_adds_pi16(mm5, mm7);
								mm5 = _mm_srai_pi16(mm2, 2);
								mm2 = _mm_subs_pi16(mm1, mm5); //mm2 = nNextEven
								((__m64*)(pDst1))[x] = mm2;
								mm5 = _mm_adds_pi16(mm6, mm2);
								mm6 = _mm_srai_pi16(mm5, 1);
								mm5 = _mm_adds_pi16(mm3, mm6); //mm5 = Dst0
								((__m64*)(pDst0))[x] = mm5;
							}
						}
						x *= 4;
						_mm_empty();
#endif NCSJPC_X86_MMI_MMX
					}
#endif //NCSJPC_X86_MMI
					return(TInvCol_I(pLow, pLo1UP, pLo1DOWN,
									 pHigh, pHi1UP, pHi1DOWN,
									 pDst0, pDst1,
									 x, pOutput->GetWidth(),
									 bLowFirst));
				}
				break;
			case CNCSJPCBuffer::BT_INT32:
				{
					INT32 *pLow = (INT32*)Low[1]->GetPtr();
					INT32 *pLo1UP = (INT32*)Low[0]->GetPtr();
					INT32 *pLo1DOWN = (INT32*)Low[2]->GetPtr();
					INT32 *pHigh = (INT32*)High[1]->GetPtr();
					INT32 *pHi1UP = (INT32*)High[0]->GetPtr();
					INT32 *pHi1DOWN = (INT32*)High[2]->GetPtr();
					INT32 *pDst0 = (INT32*)pOutput->GetPtr();
					INT32 *pDst1 = (INT32*)pOutput->GetPtr() + pOutput->GetStep();

#ifdef NCSJPC_X86_MMI
					if(CNCSJPCBuffer::sm_SystemInfo.m_bSSE2Present) {
						UINT32 n = pOutput->GetWidth() / 4;
						__m128i mm7 = _mm_set1_epi32(2);

						if(bLowFirst) {
							for(; x < n; x++) {
								__m128i mm2 = _mm_load_si128(&((__m128i*)pHi1UP)[x]);
								__m128i mm3 = _mm_load_si128(&((__m128i*)pHigh)[x]);

								__m128i mm5 = _mm_add_epi32(mm2, mm3);
								__m128i mm6 = _mm_add_epi32(mm5, mm7);
								mm5 = _mm_srai_epi32(mm6, 2);
								__m128i mm0 = _mm_load_si128(&((__m128i*)pLow)[x]);
								mm6 = _mm_sub_epi32(mm0, mm5); //mm6 = nThisEven
								_mm_store_si128(&((__m128i*)pDst0)[x], mm6);
								__m128i mm4 = _mm_load_si128(&((__m128i*)pHi1DOWN)[x]);
								mm5 = _mm_add_epi32(mm3, mm4);
								mm2 = _mm_add_epi32(mm5, mm7);
								mm5 = _mm_srai_epi32(mm2, 2);
								__m128i mm1 = _mm_load_si128(&((__m128i*)pLo1DOWN)[x]);
								mm2 = _mm_sub_epi32(mm1, mm5); //mm2 = nNextEven
								mm5 = _mm_add_epi32(mm6, mm2);
								mm6 = _mm_srai_epi32(mm5, 1);
								mm5 = _mm_add_epi32(mm3, mm6); //mm5 = Dst0
								_mm_store_si128(&((__m128i*)pDst1)[x], mm5);
							}
						} else {
							for(; x < n; x++) {
								__m128i mm2 = _mm_load_si128(&((__m128i*)pHi1UP)[x]);
								__m128i mm3 = _mm_load_si128(&((__m128i*)pHigh)[x]);

								__m128i mm5 = _mm_add_epi32(mm2, mm3);
								__m128i mm6 = _mm_add_epi32(mm5, mm7);
								mm5 = _mm_srai_epi32(mm6, 2);
								__m128i mm0 = _mm_load_si128(&((__m128i*)pLo1UP)[x]);
								mm6 = _mm_sub_epi32(mm0, mm5); //mm6 = nPrevEven
								__m128i mm4 = _mm_load_si128(&((__m128i*)pHi1DOWN)[x]);
								mm5 = _mm_add_epi32(mm3, mm4);
								mm2 = _mm_add_epi32(mm5, mm7);
								mm5 = _mm_srai_epi32(mm2, 2);
								__m128i mm1 = _mm_load_si128(&((__m128i*)pLow)[x]);
								mm2 = _mm_sub_epi32(mm1, mm5); //mm2 = nNextEven
								_mm_store_si128(&((__m128i*)pDst1)[x], mm2);
								mm5 = _mm_add_epi32(mm6, mm2);
								mm6 = _mm_srai_epi32(mm5, 1);
								mm5 = _mm_add_epi32(mm3, mm6); //mm5 = Dst0
								_mm_store_si128(&((__m128i*)pDst0)[x], mm5);
							}
						}
						x *= 4;
						_mm_empty();
#ifdef NCSJPC_X86_MMI_MMX
					} else if(CNCSJPCBuffer::sm_SystemInfo.m_bMMXPresent) {
						UINT32 n = pOutput->GetWidth() / 2;
						__m64 mm7 = _mm_set1_pi32(2);

						if(bLowFirst) {
							for(; x < n; x++) {
								__m64 mm0 = ((__m64*)(pLow))[x];
								__m64 mm1 = ((__m64*)(pLo1DOWN))[x];
								__m64 mm2 = ((__m64*)(pHi1UP))[x];
								__m64 mm3 = ((__m64*)(pHigh))[x];
								__m64 mm4 = ((__m64*)(pHi1DOWN))[x];

								__m64 mm5 = _mm_add_pi32(mm2, mm3);
								__m64 mm6 = _mm_add_pi32(mm5, mm7);
								mm5 = _mm_srai_pi32(mm6, 2);
								mm6 = _mm_sub_pi32(mm0, mm5); //mm6 = nThisEven
								((__m64*)(pDst0))[x] = mm6;
								mm5 = _mm_add_pi32(mm3, mm4);
								mm2 = _mm_add_pi32(mm5, mm7);
								mm5 = _mm_srai_pi32(mm2, 2);
								mm2 = _mm_sub_pi32(mm1, mm5); //mm2 = nNextEven
								mm5 = _mm_add_pi32(mm6, mm2);
								mm6 = _mm_srai_pi32(mm5, 1);
								mm5 = _mm_add_pi32(mm3, mm6); //mm5 = Dst0
								((__m64*)(pDst1))[x] = mm5;
							}
						} else {
							for(; x < n; x++) {
								__m64 mm0 = ((__m64*)(pLo1UP))[x];
								__m64 mm1 = ((__m64*)(pLow))[x];
								__m64 mm2 = ((__m64*)(pHi1UP))[x];
								__m64 mm3 = ((__m64*)(pHigh))[x];
								__m64 mm4 = ((__m64*)(pHi1DOWN))[x];

								__m64 mm5 = _mm_add_pi32(mm2, mm3);
								__m64 mm6 = _mm_add_pi32(mm5, mm7);
								mm5 = _mm_srai_pi32(mm6, 2);
								mm6 = _mm_sub_pi32(mm0, mm5); //mm6 = nPrevEven
								mm5 = _mm_add_pi32(mm3, mm4);
								mm2 = _mm_add_pi32(mm5, mm7);
								mm5 = _mm_srai_pi32(mm2, 2);
								mm2 = _mm_sub_pi32(mm1, mm5); //mm2 = nNextEven
								((__m64*)(pDst1))[x] = mm2;
								mm5 = _mm_add_pi32(mm6, mm2);
								mm6 = _mm_srai_pi32(mm5, 1);
								mm5 = _mm_add_pi32(mm3, mm6); //mm5 = Dst0
								((__m64*)(pDst0))[x] = mm5;
							}
						}
						x *= 2;
						_mm_empty();
#endif NCSJPC_X86_MMI_MMX
					}
#endif //NCSJPC_X86_MMI
					return(TInvCol_I(pLow, pLo1UP, pLo1DOWN,
									 pHigh, pHi1UP, pHi1DOWN,
									 pDst0, pDst1,
									 x, pOutput->GetWidth(),
									 bLowFirst));
				}
				break;
		}
		bRet = true;
	}
	return(bRet);
}
#endif // NOTUSED

CNCSJPCNode::Context *CNCSJPCResolution::GetContext(ContextID nCtx, bool bAutoConstruct)
{
	CNCSJPCNode::Context *pCtx = CNCSJPCNode::GetContext(nCtx, false);
	if(!pCtx && bAutoConstruct) {
		pCtx = new Context;
		SetContext(nCtx, pCtx);
	}
	return(pCtx);
}
