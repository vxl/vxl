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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCPacketHeader.cpp $
** CREATED:  18/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCPacketHeader class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCPacket.h"
#include "NCSJPCTilePartHeader.h"
#include "NCSJPCEPHMarker.h"
#include "NCSJPCSOPMarker.h"
#include "NCSJPCCodeBlock.h"
#include "NCSJPCProgression.h"
#include "NCSJPC.h"
#include "math.h"
#include "NCSJPCFileIOStream.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCPacket::CNCSJPCPacket()
{
	m_nLength = 0;
	m_nOffset = 0;
	m_nDataLength = 0;
	m_nDataOffset = 0;
	m_nPacket = (NCSJPCPacketId)-1;
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCPacket::ParseHeader(CNCSJPC &JPC, 
										   CNCSJPCIOStream &Stream, 
										   CNCSJPCProgression *pProgression, 
										   bool bParseData,
										   bool *pbNonZeroLength)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		CNCSJPCTilePartHeader *pMainTP = JPC.GetTile(pProgression->m_nCurTile);

		if(pMainTP) {
			CNCSJPCComponent *pComponent = pMainTP->m_Components[pProgression->m_nCurComponent];
			CNCSJPCResolution *pResolution = pComponent->m_Resolutions[pProgression->m_nCurResolution];
			CNCSJPCPrecinct *pPrecinct = pResolution->m_Precincts.find(pProgression->m_nCurPrecinctX, pProgression->m_nCurPrecinctY);

			m_nPacket = pProgression->m_nCurPacket;
			m_nOffset = Stream.Tell();
			m_nLength = 0;

			if(pPrecinct->m_bZeroSize == false) {
				if(pComponent->m_CodingStyle.m_Scod.bSOPMarkers) {
					Stream.Mark();
					CNCSJPCSOPMarker SOP;
					NCSJP2_CHECKIO_ERROR(SOP.Parse(JPC, Stream));
					if(SOP.m_bValid == false) {
						Stream.Rewind();
					} else {
						Stream.UnMark();
					}
				}
					/** Non-Zero length packet */
				bool	bNonZeroLength;
				if(pbNonZeroLength == NULL) {
					pbNonZeroLength = &bNonZeroLength;
				}

				NCSJP2_CHECKIO(UnStuff(*pbNonZeroLength));
				if(*pbNonZeroLength) {
					pPrecinct->CreateSubBands(false);
					for(int eBand = (pProgression->m_nCurResolution == 0 ? NCSJPC_LL : NCSJPC_HL); 
							eBand <= (pProgression->m_nCurResolution == 0 ? NCSJPC_LL : NCSJPC_HH); 
							eBand++) {
						CNCSJPCSubBand *pSubBand = pPrecinct->m_SubBands[eBand];
						CNCSJPCTagTree *pLITree = &pSubBand->m_LayerInclusion;
						INT32 nCBWide = pSubBand->GetNumCBWide();
						INT32 nCBHigh = pSubBand->GetNumCBHigh();

						for(INT32 nCBY = 0; nCBY < nCBHigh; nCBY++) {
							for(INT32 nCBX = 0; nCBX < nCBWide; nCBX++) {
								CNCSJPCCodeBlock *pCB = &(*pSubBand->m_pCodeBlocks)[nCBX + nCBWide * nCBY];
								bool bCBIncluded = false;

								if(pCB->m_bZeroSize) {
									continue;
								}
								if(pCB->m_nPasses == 0) {
									if(pLITree->Parse(Stream, nCBX, nCBY, pProgression->m_nCurLayer + 1, bCBIncluded) == false) {
										Error = Stream.GetError();
										break;
									}
								} else {
									NCSJP2_CHECKIO(UnStuff(bCBIncluded));
								}
								if(bCBIncluded) {
										// read in the zero-plane tree node for this codeblock
									CNCSJPCTagTree *pZPTree = &pSubBand->m_ZeroPlanes;

									if(pCB->m_nLayerIncluded > pProgression->m_nCurLayer) {
										pCB->m_nLayerIncluded = pProgression->m_nCurLayer;
									}

									if(pCB->m_nPasses == 0) {
										int zbpl = 1; 
										for (;;) {
											bool bBit;
											if(pZPTree->Parse(Stream, nCBX, nCBY, zbpl, bBit) == false) {
												Error = Stream.GetError();
												break;
											}
											if(bBit) {
												break;
											}
											zbpl++;
										}
										pCB->m_nZeroBits = zbpl - 1;
									}
//									if(pCB->m_nZeroBits > 74) {
//										printf("HERE");
//									}
									//
									// Work out how many encoding passes are included in this packet.  This is bit stuffed.
									//
									bool bBit;
									INT32 nPassesIncluded = 0;

									NCSJP2_CHECKIO(UnStuff(bBit));
									if(bBit == 0) {
										// 0
										nPassesIncluded = 1;
									} else {
										// 1
										NCSJP2_CHECKIO(UnStuff(bBit));
										if(bBit == 0) {
											// 10
											nPassesIncluded = 2;
										} else {
											// 11
											NCSJP2_CHECKIO(UnStuff(bBit));
											if(bBit == 0) {
												// 110
												NCSJP2_CHECKIO(UnStuff(bBit));
												if(bBit == 0) {
													// 1100
													nPassesIncluded = 3;
												} else {
													// 1101
													nPassesIncluded = 4;
												}
											} else {
												// 111
												NCSJP2_CHECKIO(UnStuff(bBit));
												if(bBit == 0) {
													// 1110
													nPassesIncluded = 5;
												} else {
													// 1111
													UINT8 nByte = 0;
													int i;
													for(i = 0; i < 5; i++) {
														NCSJP2_CHECKIO(UnStuff(bBit));
														nByte = (nByte << 1) | (bBit ? 0x1 : 0x0);
													}
													if(nByte != 0x1f) {
														// 1111 00000 - 1111 11110, 6-36
														nPassesIncluded = 6 + nByte;
													} else {
														// 1111 11111
														nByte = 0;
														for(i = 0; i < 7; i++) {
															NCSJP2_CHECKIO(UnStuff(bBit));
															nByte = (nByte << 1) | (bBit ? 0x1 : 0x0);
														}
														nPassesIncluded = 37 + nByte;
													}
												}
											}
										}
									}
									if(pComponent->m_CodingStyle.m_SPcod.m_Scb & CNCSJPCT1Coder::PASS_TERMINATION) {
										// Multiple codeword segments
										for(int s = 0; s < nPassesIncluded; s++) {
											UINT32 nSegLength = 0;
											UINT32 nPasses = 1;
											if(s == 0) {
												NCSJP2_CHECKIO_ERROR(ParseSegment(Stream, pCB, nPasses, nSegLength));
											} else {
												INT32 nBits = pCB->m_nLblock + NCSLog2(nPasses);
												for(int i = 0; i < nBits; i++) {
													bool bBit;
													NCSJP2_CHECKIO(UnStuff(bBit));
													nSegLength = (nSegLength << 1) | (bBit ? 0x1 : 0x0);
												}
											}
											CNCSJPCSegment Seg;
											Seg.m_nIndex = (UINT16)pCB->m_Segments.size() + s;
											Seg.m_nPasses = nPasses;
											Seg.m_nLength = nSegLength;
											pCB->m_NextSegments.push_back(Seg);
											m_nDataLength += nSegLength;
										}
									} else if(pComponent->m_CodingStyle.m_SPcod.m_Scb & CNCSJPCT1Coder::SELECTIVE_CODING_BYPASS) {
										// Multiple codeword segments
										INT32 nCBPasses = 0;
										UINT32 s;

										for(s = 0; s < pCB->m_Segments.size(); s++) {
											nCBPasses += pCB->m_Segments[s].m_nPasses;
										}

										INT32 nPassesLeft = nPassesIncluded;
										s = 0;
										while(nPassesLeft > 0) {
											CNCSJPCSegment Seg;
											
											INT32 nSegMaxPasses = (nCBPasses < 10) ? (10 - nCBPasses) : ((nCBPasses % 3) ? 2 : 1);
											INT32 nSegPasses = NCSMin(nPassesLeft, nSegMaxPasses);
											Seg.m_nIndex = (nCBPasses < 10) ? 0 : (1 + 2 * (nCBPasses - 10) / 3);//((nCBPasses % 3) ? (1 + (nCBPasses - 10) % 2) : (1 + (nCBPasses - 10) % 2 + 1));
											if(pCB->m_Segments.size() > Seg.m_nIndex) {
												nSegPasses = NCSMin(nSegPasses, ((Seg.m_nIndex == 0) ? 10 : ((Seg.m_nIndex & 0x1) ? 2 : 1)) - pCB->m_Segments[Seg.m_nIndex].m_nPasses);
											}
											UINT32 nSegLength = 0;
											if(s == 0) {
												NCSJP2_CHECKIO_ERROR(ParseSegment(Stream, pCB, nSegPasses, nSegLength));
											} else {
												INT32 nBits = pCB->m_nLblock + NCSLog2(nSegPasses);
												for(int i = 0; i < nBits; i++) {
													bool bBit;
													NCSJP2_CHECKIO(UnStuff(bBit));
													nSegLength = (nSegLength << 1) | (bBit ? 0x1 : 0x0);
												}
											}
											Seg.m_nPasses = nSegPasses;
											Seg.m_nLength = nSegLength;
											pCB->m_NextSegments.push_back(Seg);
											m_nDataLength += nSegLength;
											nCBPasses += nSegPasses;
											nPassesLeft -= nSegPasses;
											s++;
										}
									} else {
										// Single codeword segments
										UINT32 nSegLength = 0;
										NCSJP2_CHECKIO_ERROR(ParseSegment(Stream, pCB, nPassesIncluded, nSegLength));
										CNCSJPCSegment Seg;
										Seg.m_nPasses = nPassesIncluded;
										Seg.m_nLength = nSegLength;
										pCB->m_NextSegments.push_back(Seg);
										
										m_nDataLength += nSegLength;									
									}
									pCB->m_nPasses += nPassesIncluded;
								}
							}
						}
					}
				}
				NCSJP2_CHECKIO(ResetBitStuff());

				if(pComponent->m_CodingStyle.m_Scod.bEPHMarkers) {
					Stream.Mark();
					CNCSJPCEPHMarker EPH;
					NCSJP2_CHECKIO_ERROR(EPH.Parse(JPC, Stream));
					if(EPH.m_bValid == false) {
						Stream.Rewind();
					} else {
						Stream.UnMark();
					}
				}
				m_nLength = (INT32)(Stream.Tell() - m_nOffset);
				if(bParseData) {
					Error = ParseBody(JPC, Stream, pProgression, *pbNonZeroLength);
				}
			}
		}
	NCSJP2_CHECKIO_END();

	return(Error);
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCPacket::UnParseHeader(CNCSJPC &JPC, 
										CNCSJPCIOStream &Stream, 
										CNCSJPCProgression *pProgression, 
										bool bParseData)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		CNCSJPCTilePartHeader *pMainTP = JPC.GetTile(pProgression->m_nCurTile);

		if(pMainTP) {
			CNCSJPCComponent *pComponent = pMainTP->m_Components[pProgression->m_nCurComponent];
			CNCSJPCResolution *pResolution = pComponent->m_Resolutions[pProgression->m_nCurResolution];
			CNCSJPCPrecinct *pPrecinct = pResolution->m_Precincts.find(pProgression->m_nCurPrecinctX, pProgression->m_nCurPrecinctY);

			m_nPacket = pProgression->m_nCurPacket;
			m_nOffset = Stream.Tell();
			m_nLength = 0;

			if(pPrecinct->m_bZeroSize == false) {
				JPC.m_pEncoderOffsetFile->Seek((pResolution->m_nEncoderPLTStartIndex + pProgression->m_nCurLayer * pResolution->GetNumPrecinctsWide() * pResolution->GetNumPrecinctsHigh() + pProgression->m_nCurPrecinctX + pProgression->m_nCurPrecinctY * pResolution->GetNumPrecinctsWide()) * sizeof(UINT64), CNCSJPCIOStream::START);
				JPC.m_pEncoderOffsetFile->WriteUINT64(JPC.m_EncoderFiles[pResolution->m_nResolution]->Tell());
			
				if(pComponent->m_CodingStyle.m_Scod.bSOPMarkers) {
					CNCSJPCSOPMarker SOP;
					SOP.m_nNsop = m_nPacket % 65536;
					SOP.m_bValid = true;
					NCSJP2_CHECKIO_ERROR(SOP.UnParse(JPC, Stream));
				}
				if(pProgression->m_nCurLayer == 0) {
					// First layer, setup LI and ZP tag trees
					for(int eBand = (pProgression->m_nCurResolution == 0 ? NCSJPC_LL : NCSJPC_HL); 
							eBand <= (pProgression->m_nCurResolution == 0 ? NCSJPC_LL : NCSJPC_HH); 
							eBand++) {
						CNCSJPCSubBand *pSubBand = pPrecinct->m_SubBands[eBand];
						CNCSJPCTagTree *pLITree = &pSubBand->m_LayerInclusion;

						INT32 nCBWide = pSubBand->GetNumCBWide();
						INT32 nCBHigh = pSubBand->GetNumCBHigh();

						for(INT32 nCBY = 0; nCBY < nCBHigh; nCBY++) {
							for(INT32 nCBX = 0; nCBX < nCBWide; nCBX++) {
								CNCSJPCCodeBlock *pCB = &(*pSubBand->m_pCodeBlocks)[nCBX + nCBWide * nCBY];
								if(pCB->m_bZeroSize) {
									continue;
								}
								UINT16 nSegs = (UINT16)pCB->m_Segments.size();
								if(nSegs) {
									for(UINT32 l = 0; l < nSegs; l++) {
										if(pCB->m_Segments[l].m_nPasses) {
											pCB->m_nLayerIncluded = l;
											pLITree->SetValue(nCBX, nCBY, pCB->m_nLayerIncluded);
											break;
										}
									}
								} else {
									pLITree->SetValue(nCBX, nCBY, 99999);
								}
								pSubBand->m_ZeroPlanes.SetValue(nCBX, nCBY, pCB->m_nZeroBits);
								pCB->m_nLblock = 3;
							}
						}
					}
				}

				bool bHavePasses = false;
				for(int eBand = (pProgression->m_nCurResolution == 0 ? NCSJPC_LL : NCSJPC_HL); 
						eBand <= (pProgression->m_nCurResolution == 0 ? NCSJPC_LL : NCSJPC_HH) && bHavePasses == false; 
						eBand++) {
					CNCSJPCSubBand *pSubBand = pPrecinct->m_SubBands[eBand];
					INT32 nCBWide = pSubBand->GetNumCBWide();
					INT32 nCBHigh = pSubBand->GetNumCBHigh();

					for(INT32 nCBY = 0; nCBY < nCBHigh && (bHavePasses == false); nCBY++) {
						for(INT32 nCBX = 0; nCBX < nCBWide; nCBX++) {
							CNCSJPCCodeBlock *pCB = &(*pSubBand->m_pCodeBlocks)[nCBX + nCBWide * nCBY];
							if(pCB->m_Segments.size() && pCB->m_Segments[pProgression->m_nCurLayer].m_nPasses) {
								bHavePasses = true;
								break;
							}
						}
					}
				}
				NCSJP2_CHECKIO(Stuff(bHavePasses));
				if(bHavePasses) {
					for(int eBand = (pProgression->m_nCurResolution == 0 ? NCSJPC_LL : NCSJPC_HL); 
							eBand <= (pProgression->m_nCurResolution == 0 ? NCSJPC_LL : NCSJPC_HH); 
							eBand++) {
						CNCSJPCSubBand *pSubBand = pPrecinct->m_SubBands[eBand];
						CNCSJPCTagTree *pLITree = &pSubBand->m_LayerInclusion;
						INT32 nCBWide = pSubBand->GetNumCBWide();
						INT32 nCBHigh = pSubBand->GetNumCBHigh();
						INT32 nCBY;
						
//						for(nCBY = 0; nCBY < nCBHigh; nCBY++) {
//							for(INT32 nCBX = 0; nCBX < nCBWide; nCBX++) {
//								CNCSJPCCodeBlock *pCB = &(*pSubBand->m_pCodeBlocks)[nCBX + nCBWide * nCBY];
//								if(pCB->m_bZeroSize) {
//									continue;
//								}
//								if(pCB->m_Segments.size() && pCB->m_Segments[pProgression->m_nCurLayer].m_nPasses) {
//									if(pCB->m_nLayerIncluded == 0xffffffff) {
//										pCB->m_nLayerIncluded = pProgression->m_nCurLayer;
//									}
//									pLITree->SetValue(nCBX, nCBY, pCB->m_nLayerIncluded);
//								} else {
//									pLITree->SetValue(nCBX, nCBY, 999);
//								}
//							}
//						}
						for(nCBY = 0; nCBY < nCBHigh; nCBY++) {
							for(INT32 nCBX = 0; nCBX < nCBWide; nCBX++) {
								CNCSJPCCodeBlock *pCB = &(*pSubBand->m_pCodeBlocks)[nCBX + nCBWide * nCBY];
								
								if(pCB->m_bZeroSize) {
									continue;
								}
								CNCSJPCSegment &Seg = pCB->m_Segments[pProgression->m_nCurLayer];

								if(pCB->m_nPasses == 0) {//pCB->m_Segments.size() == 0) {
									if(pLITree->UnParse(Stream, nCBX, nCBY, pProgression->m_nCurLayer + 1) == false) {
										Error = Stream.GetError();
										break;
									}
								} else {
									NCSJP2_CHECKIO(Stuff(Seg.m_nPasses != 0/* in this layer*/));
								}
								if(Seg.m_nPasses != 0 /*included in this layer */) {
									if(pCB->m_nLayerIncluded == pProgression->m_nCurLayer) {
										// First layer this CB was included, output ZP tree
										pSubBand->m_ZeroPlanes.UnParse(Stream, nCBX, nCBY, 99999);
									}
									UINT32 nData = 0;
									INT32 nBits = 0;
									if (Seg.m_nPasses == 1) {
										nData = 0;
										nBits = 1;
									} else if (Seg.m_nPasses==2) {
										nData = 2;
										nBits = 2;
									} else if (Seg.m_nPasses<=5) {
										nData = 0xc | (Seg.m_nPasses - 3);
										nBits = 4;
									} else if (Seg.m_nPasses<=36) {
										nData = 0x1e0 | (Seg.m_nPasses - 6);
										nBits = 9;
									} else if (Seg.m_nPasses<=164) {
										nData = 0xff80 | (Seg.m_nPasses - 37);
										nBits = 16;
									}
									while(nBits != 0) {
										NCSJP2_CHECKIO(Stuff(((nData >> (nBits - 1)) & 0x1) ? true : false));
										nBits--;
									}	

									UnParseSegment(Stream, pCB, Seg.m_nPasses, Seg.m_nLength);
									pCB->m_nPasses += Seg.m_nPasses;
								}
							}
						}
					}
				}
				NCSJP2_CHECKIO(FlushBitStuff());
				if(pComponent->m_CodingStyle.m_Scod.bEPHMarkers) {
					CNCSJPCEPHMarker EPH;
					EPH.m_bValid = true;
					EPH.UnParse(JPC, Stream);
				}
				m_nLength = (INT32)(Stream.Tell() - m_nOffset);
				if(bParseData) {
					if(bHavePasses) {
						Error = UnParseBody(JPC, Stream, pProgression, true);
					}
					if(Error == NCS_SUCCESS) {
						// Write the packet length as a UINT32
//						if(!pResolution->m_EncoderPLTFiles[pProgression->m_nCurLayer]) {
//							pResolution->OpenEncoderLayerPLTFile(pProgression->m_nCurLayer, true);
//						}
//						if(pResolution->m_EncoderPLTFiles[pProgression->m_nCurLayer]->WriteUINT32(m_nLength + m_nDataLength) == false) {
//							Error = pResolution->m_EncoderPLTFiles[pProgression->m_nCurLayer]->GetError();
//						}
//						if(pComponent->m_pTilePart->m_pEncoderPLTFile == NULL) {
//							pComponent->m_pTilePart->OpenEncoderPLTFile(true);
//						}
//						pComponent->m_pTilePart->m_pEncoderPLTFile->Seek(pResolution->m_EncoderPLTIndex[pProgression->m_nCurLayer] * sizeof(UINT32), CNCSJPCIOStream::START);
//						pResolution->m_EncoderPLTIndex[pProgression->m_nCurLayer]++;
						JPC.m_pEncoderPLTFile->Seek((pResolution->m_nEncoderPLTStartIndex + pProgression->m_nCurLayer * pResolution->GetNumPrecinctsWide() * pResolution->GetNumPrecinctsHigh() + pProgression->m_nCurPrecinctX + pProgression->m_nCurPrecinctY * pResolution->GetNumPrecinctsWide()) * sizeof(UINT32), CNCSJPCIOStream::START);
						if(JPC.m_pEncoderPLTFile->WriteUINT32(m_nLength + m_nDataLength) == false) {
							Error = JPC.m_pEncoderPLTFile->GetError();
						}

					}
				}
			}
		}
	NCSJP2_CHECKIO_END();

	return(Error);
}

CNCSError CNCSJPCPacket::ParseBody(class CNCSJPC &JPC, 
										 CNCSJPCIOStream &Stream, 
										 class CNCSJPCProgression *pProgression,
										 bool bNonZeroLength)
{
	CNCSError Error;
	CNCSJPCTilePartHeader *pMainTP;
	
	if(bNonZeroLength && (pMainTP = JPC.GetTile(pProgression->m_nCurTile)) != NULL) {
		CNCSJPCComponent *pComponent = pMainTP->m_Components[pProgression->m_nCurComponent];

		if((JPC.m_bFilePPMs || pMainTP->m_bFilePPTs) && pComponent->m_CodingStyle.m_Scod.bSOPMarkers) {
			Stream.Mark();
			CNCSJPCSOPMarker SOP;
			Error = SOP.Parse(JPC, Stream);
			if(SOP.m_bValid == false || Error != NCS_SUCCESS) {
				Stream.Rewind();
			} else {
				Stream.UnMark();
			}
		}
		UINT64 nCurOffset = Stream.Tell();
		m_nDataOffset = nCurOffset;

		CNCSJPCResolution *pResolution = pComponent->m_Resolutions[pProgression->m_nCurResolution];
		CNCSJPCPrecinct *pPrecinct = pResolution->m_Precincts.find(pProgression->m_nCurPrecinctX, pProgression->m_nCurPrecinctY);

		for(int eBand = (pProgression->m_nCurResolution == 0 ? NCSJPC_LL : NCSJPC_HL); 
			eBand <= (pProgression->m_nCurResolution == 0 ? NCSJPC_LL : NCSJPC_HH); 
			eBand++) {
			CNCSJPCSubBand *pSubBand = pPrecinct->m_SubBands[eBand];
			INT32 nCBWide = pSubBand->GetNumCBWide();
			INT32 nCBHigh = pSubBand->GetNumCBHigh();
				
			for(INT32 nCBY = 0; nCBY < nCBHigh; nCBY++) {
				for(INT32 nCBX = 0; nCBX < nCBWide; nCBX++) {
					CNCSJPCCodeBlock *pCB = &(*pSubBand->m_pCodeBlocks)[nCBX + nCBWide * nCBY];
					if(pCB && pCB->m_nLayerIncluded <= pProgression->m_nCurLayer) {
						INT32 nRead = pCB->ReadNewSegs(Stream);
						if(nRead < 0) {
							//Error
							Error = Stream;
						} else {
							nCurOffset += nRead;
						}
					}
				}
			}
		}
	}
	return(Error);
}

CNCSError CNCSJPCPacket::UnParseBody(class CNCSJPC &JPC, 
										 CNCSJPCIOStream &Stream, 
										 class CNCSJPCProgression *pProgression,
										 bool bNonZeroLength)
{
	CNCSError Error;
	CNCSJPCTilePartHeader *pMainTP;
	
	if(bNonZeroLength && (pMainTP = JPC.GetTile(pProgression->m_nCurTile)) != NULL) {
		UINT64 nCurOffset = Stream.Tell();
		m_nDataOffset = nCurOffset;

		CNCSJPCComponent *pComponent = pMainTP->m_Components[pProgression->m_nCurComponent];
		CNCSJPCResolution *pResolution = pComponent->m_Resolutions[pProgression->m_nCurResolution];
		CNCSJPCPrecinct *pPrecinct = pResolution->m_Precincts.find(pProgression->m_nCurPrecinctX, pProgression->m_nCurPrecinctY);

		for(int eBand = (pProgression->m_nCurResolution == 0 ? NCSJPC_LL : NCSJPC_HL); 
			eBand <= (pProgression->m_nCurResolution == 0 ? NCSJPC_LL : NCSJPC_HH); 
			eBand++) {
			CNCSJPCSubBand *pSubBand = pPrecinct->m_SubBands[eBand];
			INT32 nCBWide = pSubBand->GetNumCBWide();
			INT32 nCBHigh = pSubBand->GetNumCBHigh();
				
			for(INT32 nCBY = 0; nCBY < nCBHigh; nCBY++) {
				for(INT32 nCBX = 0; nCBX < nCBWide; nCBX++) {
					CNCSJPCCodeBlock *pCB = &(*pSubBand->m_pCodeBlocks)[nCBX + nCBWide * nCBY];
					if(pCB && pCB->m_nLayerIncluded <= pProgression->m_nCurLayer) {
						pCB->m_NextSegments.resize(1);
						pCB->m_NextSegments[0] = pCB->m_Segments[pProgression->m_nCurLayer];
						pCB->m_Segments[pProgression->m_nCurLayer].m_nLength = 0;
						pCB->m_Segments[pProgression->m_nCurLayer].m_pData = NULL;
						pCB->m_Segments[pProgression->m_nCurLayer].m_nPasses = 0;
						INT32 nRead = pCB->WriteNewSegs(Stream);
						if(nRead < 0) {
							//Error
							Error = Stream;
						} else {
							nCurOffset += nRead;
						}
					}
				}
			}
		}
		m_nDataLength = (INT32)(Stream.Tell() - m_nDataOffset);
	}
	return(Error);
}

CNCSError CNCSJPCPacket::ParseSegment(CNCSJPCIOStream &Stream, CNCSJPCCodeBlock *pCB, UINT32 nPassesIncluded, UINT32 &Length)
{
	CNCSError Error;
	Length = 0;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		bool bBit = false;
		NCSJP2_CHECKIO(UnStuff(bBit));
		while(bBit == true) {
			pCB->m_nLblock++;
			NCSJP2_CHECKIO(UnStuff(bBit));
		}
		INT32 nBits = pCB->m_nLblock + NCSLog2(nPassesIncluded);
		for(int i = 0; i < nBits; i++) {
			NCSJP2_CHECKIO(UnStuff(bBit));
			Length = (Length << 1) | (bBit ? 0x1 : 0x0);
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

CNCSError CNCSJPCPacket::UnParseSegment(CNCSJPCIOStream &Stream, CNCSJPCCodeBlock *pCB, UINT32 nPassesIncluded, UINT32 &Length)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		INT32 nIncrement = NCSMax(0, NCSLog2(Length) + 1 - (pCB->m_nLblock + NCSLog2(nPassesIncluded)));
									
		INT32 nLblock = nIncrement;//pCB->m_nLblock;
		while(nLblock > 0) {//3) {
			NCSJP2_CHECKIO(Stuff(true));
			nLblock--;
		}
		NCSJP2_CHECKIO(Stuff(false));
		pCB->m_nLblock += nIncrement;
		
		INT32 nBits = pCB->m_nLblock + NCSLog2(nPassesIncluded);
		while(nBits > 0) {
			NCSJP2_CHECKIO(Stuff(((Length >> (nBits - 1)) & 0x1) ? true : false));
			nBits--;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// Default constructor, initialises members 
CNCSJPCPacketList::CNCSJPCPacketList()
{
	m_bHaveOffsets = false;
	m_pJPC = NULL;
	m_pStream = NULL;
	m_nBytes = 0;
	m_nHeaders = -1;
}

// Virtual destructor */
CNCSJPCPacketList::~CNCSJPCPacketList()
{
}

// Parse the headers from the JPC codestream.
// This is done by continuing the progression from the current point
// in the main tile part for this tile.
CNCSError CNCSJPCPacketList::Parse(CNCSJPC &JPC, 
										 CNCSJPCIOStream &Stream, 
										 bool &bComplete, 
										 UINT64 nBytes, 
										 INT32 nHeaders)
{
	CNCSError Error;
	CNCSJPCTilePartHeader *pMainTP = JPC.GetTile();

	if(pMainTP) {
		m_pJPC = &JPC;
		m_pStream = &Stream;

		if(JPC.m_bFilePPMs) {
			UINT32 nZtp = JPC.m_pCurrentTilePart->m_nZtp;
			UINT32 i = 0;
			while(nZtp >= JPC.m_PPMs[i].m_TileParts.size()) {
				nZtp -= (UINT32)JPC.m_PPMs[i].m_TileParts.size();
				i++;
			}
			m_pStream->Seek(JPC.m_PPMs[i].m_TileParts[nZtp].m_nOffset, CNCSJPCIOStream::START);
			if(i > 0 && nZtp == 0 && JPC.m_PPMs[i-1].m_TileParts.back().m_nNppmLeft) {
				m_pStream->Seek(JPC.m_PPMs[i-1].m_TileParts.back().m_nNppmLeft);
			}
			m_nBytes = JPC.m_PPMs[i].m_TileParts[nZtp].m_nNppmTotal;
			m_nHeaders = -1;

			pMainTP->Iterate(pMainTP, this, bComplete);

			if(JPC.m_PPMs[i].m_TileParts[nZtp].m_nNppmLeft && JPC.m_PPMs.size() > i + 1 && JPC.m_PPMs[i+1].m_TileParts.size()) {
				// Next PPM starts with remaining headers
				m_pStream->Seek(JPC.m_PPMs[i+1].m_TileParts[0].m_nOffset, CNCSJPCIOStream::START);
				m_nBytes = JPC.m_PPMs[i].m_TileParts[nZtp].m_nNppmLeft;
				pMainTP->Iterate(pMainTP, this, bComplete);
			}
			Stream.Seek(JPC.m_pCurrentTilePart->m_SOT.m_nOffset + JPC.m_pCurrentTilePart->m_SOT.m_nPsot, CNCSJPCIOStream::START);
		} else if(JPC.m_pCurrentTilePart->m_bFilePPTs) {
			m_nHeaders = -1;
			
			for(UINT32 i = 0; i < pMainTP->m_PPTs.size(); i++) {
				m_pStream->Seek(pMainTP->m_PPTs[i].m_nOffset + 2 * sizeof(UINT16) + sizeof(UINT8), CNCSJPCIOStream::START);
				m_nBytes = pMainTP->m_PPTs[i].m_nLength - (sizeof(UINT16) + sizeof(UINT8));
				pMainTP->Iterate(pMainTP, this, bComplete);
			}
			Stream.Seek(JPC.m_pCurrentTilePart->m_SOT.m_nOffset + JPC.m_pCurrentTilePart->m_SOT.m_nPsot, CNCSJPCIOStream::START);
		} else {
			m_nBytes = nBytes;
			m_nHeaders = nHeaders;

			while(!bComplete && m_nBytes != 0 && m_nHeaders != 0) {
				pMainTP->Iterate(pMainTP, this, bComplete);
			}
		}
		m_bHaveOffsets = true;
	}
	return(Error);
}

// Parse the headers from the JPC codestream.
CNCSError CNCSJPCPacketList::ParseHeader(bool &bComplete, UINT32 nPacket)
{
	CNCSError Error;
	INT32 nHeaders = (m_nHeaders == -1) ? 0x7fffffff : m_nHeaders;
	INT64 nStart = m_pStream->Tell();

	NCSJP2_CHECKIO_BEGIN(Error, *m_pStream);
		CNCSJPCPacket packet;
		NCSJP2_CHECKIO_ERROR(packet.ParseHeader(*m_pJPC, *m_pStream, m_pJPC->GetTile(), m_pJPC->m_bFilePPMs == false && m_pJPC->m_pCurrentTilePart->m_bFilePPTs == false));
		push_back(packet);

		if(m_nHeaders != -1) {
			m_nHeaders--;
		}
		m_nBytes -= m_pStream->Tell() - nStart;
	NCSJP2_CHECKIO_END();

	if(m_nHeaders <= 0 && m_nBytes <= 0) {
		bComplete = true;
	}
	return(Error);
}

CNCSError CNCSJPCPacketList::GenDynamicPLTs(CNCSJPCTilePartHeader *pMainTP)
{
	pMainTP->m_PLTs.resize(1);
	CNCSJPCPLTMarker &PLT = pMainTP->m_PLTs[0];
	PLT.m_bDynamic = true;
	CNCSJPCPacketLengthType Length;
	bool bSOPMarkers = pMainTP->m_Components[0]->m_CodingStyle.m_Scod.bSOPMarkers;
	UINT32 nPackets = (UINT32)m_Packets.size();

	PLT.AllocLengths();

	std::vector<CNCSJPCPacketLengthType> *pLengths = PLT.GetLengths();

	for(UINT32 p = 0; p < nPackets; p++) {
		if(pMainTP->m_pJPC->m_bFilePPMs || pMainTP->m_bFilePPTs) {
			Length.m_nHeaderLength = m_Packets[p].m_nLength;
			Length.m_nDataLength = m_Packets[p].m_nDataLength;
			if(bSOPMarkers && Length.m_nHeaderLength != 0) {
				Length.m_nDataLength += 3 * sizeof(UINT16);
			}
		} else {
			Length.m_nHeaderLength = m_Packets[p].m_nLength + m_Packets[p].m_nDataLength;
		}
		PLT.AddLength(Length);
//		pLengths->push_back(Length);
//		CNCSJPCPLTMarker::sm_Tracker.AddMem(sizeof(CNCSJPCPacketLengthType));
//		PLT.m_nLengths++;
	}
	return(NCS_SUCCESS);
}
	
// UnParse the headers to the JPC codestream.
CNCSError CNCSJPCPacketList::UnParse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	//FIXME
	return(NCS_SUCCESS);
}
