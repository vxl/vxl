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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCTilePartHeader.cpp $
** CREATED:  13/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCTilePartHeader class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPC.h"
#include "NCSJPCMCTNode.h"
#include "NCSJP2File.h"

CNCSJPCNode::CNCSJPCNodeTracker CNCSJPCTilePartHeader::sm_Tracker("CNCSJPCTilePartHeader", sizeof(CNCSJPCTilePartHeader));

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCTilePartHeader::CNCSJPCTilePartHeader(CNCSJPC *pJPC)
{
	sm_Tracker.Add();
	m_pJPC = pJPC;
	m_bValid = false;

	m_nCurLayer = 0;
	m_nCurResolution = 0;
	m_nCurComponent = 0;
	m_nCurPrecinctX = 0;
	m_nCurPrecinctY = 0;
	m_nCurTX = 0;
	m_nCurTY = 0;
	m_nIncTX = 0;
	m_nIncTY = 0;
	m_bFilePPTs = false;
	m_bSimpleStructure = false;
 
	m_nZtp = pJPC->m_nNextZtp;
	pJPC->m_nNextZtp += 1;
}

// Destructor
CNCSJPCTilePartHeader::~CNCSJPCTilePartHeader()
{
	while(m_Components.size() > 0) {
		CNCSJPCComponent *pC = m_Components[0];
		m_Components.erase(m_Components.begin());
		delete pC;
	}

	for( UINT32 i=0; i < m_TileParts.size(); i++ ) {
		delete m_TileParts[i];
		m_TileParts[i] = NULL;
	}

//	CloseEncoderPLTFile(true);

//	if(m_pJPC->m_pTmpDir) {
//		char szTmpDir[MAX_PATH];
//		sprintf(szTmpDir, "%s\\t%ld", m_pJPC->m_pTmpDir, m_SOT.m_nIsot);
//		NCSRemoveDir(szTmpDir);
//	}				

	sm_Tracker.Remove();
}

/*bool CNCSJPCTilePartHeader::OpenEncoderPLTFile(bool bWrite)
{
	bool bRet = true;
	if(m_pEncoderPLTFile == NULL) {
		char szRelTmpDir[MAX_PATH+1];
				
		sprintf(szRelTmpDir, "%s\\t%ld\\l.tmp", 
							m_pJPC->m_pTmpDir, 
							m_SOT.m_nIsot);

		m_pEncoderPLTFile = new CNCSJPCFileIOStream();
		if(m_pEncoderPLTFile) {
			CNCSError Error = m_pEncoderPLTFile->Open(szRelTmpDir, bWrite);
			bRet = (Error == NCS_SUCCESS);
		} else {
			bRet = false;
		}
	}
	return(bRet);
}

bool CNCSJPCTilePartHeader::CloseEncoderPLTFile(bool bDelete)
{
	bool bRet = true;
	if(m_pEncoderPLTFile) {
		char *pPLTFilename = bDelete ? NCSStrDup(CHAR_STRING(m_pEncoderPLTFile->GetName())) : NULL;
		bRet = m_pEncoderPLTFile->Close() == NCS_SUCCESS;
		if(bDelete && pPLTFilename) {
			NCSDeleteFile(pPLTFilename);
			NCSFree(pPLTFilename);
		}
		delete m_pEncoderPLTFile;
		m_pEncoderPLTFile = NULL;
	}
	return(bRet);
}*/

// Parse the tile-part header from the JPC codestream.
CNCSError CNCSJPCTilePartHeader::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;
	CNCSJPCMarker marker;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		INT64	nBegin = Stream.Tell() - sizeof(UINT16);	// Start of SOT header
		NCSJP2_CHECKIO_ERROR(m_SOT.Parse(JPC, Stream));

		if(m_SOT.m_bValid) {
			bool bRecalcSizes = false;

			m_nCurTX = GetX0();
			m_nCurTY = GetY0();

			CNCSJPCProgression::m_nCurTile = m_SOT.m_nIsot;

			if(m_SOT.m_nTPsot == 0) {
				// THis is the first tile-part for this tile, so
				// setup the tile-part's coding styles

				m_Components.resize(JPC.m_SIZ.m_nCsiz);
				m_Regions.clear();

				for(int c = 0; c < JPC.m_SIZ.m_nCsiz; c++) {
					m_Components[c] = new CNCSJPCComponent(this, c);
					m_Components[c]->m_CodingStyle = JPC.m_CodingStyles[c];
					m_Components[c]->m_QuantizationStyle = JPC.m_QuantizationStyles[c];
				}
				bRecalcSizes = true;
				m_POC = JPC.m_POC;
				m_Regions = JPC.m_RGNs;
			}
			CNCSJPCTilePartHeader *pMainTP = JPC.GetTile(m_nCurTile);
			// First tile part, have COD, COC, QCD, QCC, RGN, POC, PPT, PLT, COM
			// Additional tile part, only have POC, PPT, PLT, COM
				
			while(Error == NCS_SUCCESS && (Stream.Tell() < (INT64)(m_SOT.m_nPsot ? (nBegin + m_SOT.m_nPsot) : (Stream.Size() - sizeof(UINT16))))) {
				NCSJP2_CHECKIO_ERROR(marker.Parse(JPC, Stream));

				switch(marker.m_eMarker) {
					case CNCSJPCMarker::COD:
							if(m_SOT.m_nTPsot == 0 && !m_COD.m_bHaveMarker) {
								(CNCSJPCMarker&)m_COD = marker;
								NCSJP2_CHECKIO_ERROR(m_COD.Parse(JPC, Stream));
								// Setup default coding style for each component
								for(int c = 0; c < JPC.m_SIZ.m_nCsiz; c++) {
									m_Components[c]->m_CodingStyle = m_COD;
								}
								bRecalcSizes = true;
							} else {
								// COD only valid in first tile-part
								Error = NCS_FILEIO_ERROR;
							}
						break;
					
					case CNCSJPCMarker::COC:
							if(m_SOT.m_nTPsot == 0) {
								CNCSJPCCOCMarker COC;
								(CNCSJPCMarker&)COC = marker;
								NCSJP2_CHECKIO_ERROR(COC.Parse(JPC, Stream));
									// Over-ride coding style for this component
								m_Components[COC.m_nCcoc]->m_CodingStyle = COC;
								bRecalcSizes = true;
							} else {
								// COD *must* be before COC
								// COC only valid in first tile-part
								Error = NCS_FILEIO_ERROR;
							}
						break;

					case CNCSJPCMarker::QCD:
							if(m_SOT.m_nTPsot == 0 && !m_QCD.m_bHaveMarker) {
								(CNCSJPCMarker&)m_QCD = marker;
								NCSJP2_CHECKIO_ERROR(m_QCD.Parse(JPC, Stream));
								// Setup default coding style for each component
								for(int c = 0; c < JPC.m_SIZ.m_nCsiz; c++) {
									m_Components[c]->m_QuantizationStyle = m_QCD;
								}
							} else {
								// Not first tile-part
								Error = NCS_FILEIO_ERROR;
							}
						break;

					case CNCSJPCMarker::QCC:
							if(m_SOT.m_nTPsot == 0) {
								CNCSJPCQCCMarker QCC;
								(CNCSJPCMarker&)QCC = marker;
								NCSJP2_CHECKIO_ERROR(QCC.Parse(JPC, Stream));
									// Over-ride quantization style for this component
								m_Components[QCC.m_nCqcc]->m_QuantizationStyle = QCC;
							} else {
								// Not first tile-part
								Error = NCS_FILEIO_ERROR;
							}
						break;			

					case CNCSJPCMarker::RGN:
							if(m_SOT.m_nTPsot == 0) {
								m_Regions.resize(JPC.m_SIZ.m_nCsiz);
								CNCSJPCRGNMarker RGN;	
								(CNCSJPCMarker&)RGN = marker;
								NCSJP2_CHECKIO_ERROR(RGN.Parse(JPC, Stream));
								m_Regions[RGN.m_nCrgn] = RGN;
							} else {
								// Not first tile-part
								Error = NCS_FILEIO_ERROR;
							}
						break;

					case CNCSJPCMarker::POC:
							{
								CNCSJPCPOCMarker POC;
								(CNCSJPCMarker&)POC = marker;
								NCSJP2_CHECKIO_ERROR(POC.Parse(JPC, Stream));
								for(UINT32 i = 0; i < POC.m_Progressions.size(); i++) {
									pMainTP->m_POC.m_Progressions.push_back(POC.m_Progressions[i]);
									pMainTP->m_POC.m_bValid = true;
								}
							}
						break;

					case CNCSJPCMarker::PPT:
							if(JPC.m_PPMs.size() == 0) {
								m_bFilePPTs = true;
								CNCSJPCPPTMarker PPT;
								(CNCSJPCMarker&)PPT = marker;
								NCSJP2_CHECKIO_ERROR(PPT.Parse(JPC, Stream));
								m_PPTs.resize(NCSMax(m_PPTs.size(), (UINT32)PPT.m_nZppt + 1));
								m_PPTs[PPT.m_nZppt] = PPT;
							} else {
								// Can't have PPT & PPM!
								Error = NCS_FILEIO_ERROR;
							}
						break;

					case CNCSJPCMarker::PLT:
							{
								
								CNCSJPCPLTMarker PLT;
								(CNCSJPCMarker&)PLT = marker;
								NCSJP2_CHECKIO_ERROR(PLT.Parse(JPC, Stream));
								m_PLTs.resize(NCSMax(m_PLTs.size(), (UINT32)PLT.m_nZplt + 1));
								m_PLTs[PLT.m_nZplt] = PLT;							
							}
						break;

					case CNCSJPCMarker::COM:
							{
								CNCSJPCCOMMarker COM;
								(CNCSJPCMarker&)COM = marker;
								NCSJP2_CHECKIO_ERROR(COM.Parse(JPC, Stream));
							}
						break;

					case CNCSJPCMarker::SOD:
							if(!m_SOD.m_bHaveMarker) {
								if(this != pMainTP) {
									UINT32 i;
									// Store all PLTs & PPTs in the MainTP
									for(i = 0; i < m_PLTs.size(); i++) {
										pMainTP->m_PLTs.push_back(m_PLTs[i]);
									}
									m_PLTs.clear();

									for(i = 0; i < m_PPTs.size(); i++) {
										pMainTP->m_PPTs.push_back(m_PPTs[i]);
									}
									m_PPTs.clear();
								}
								if(m_bFilePPTs || JPC.m_bFilePPMs) {
									// Too hard to use PLTs with PPM/PPT
									pMainTP->m_PLTs.clear();
								}
								if(bRecalcSizes) {
									RecalcSizes();
									bRecalcSizes = false;
								}
								if(m_SOT.m_nTPsot == 0) {
									CNCSJPCProgression::Start(this);
								}
								(CNCSJPCMarker&)m_SOD = marker;
								NCSJP2_CHECKIO_ERROR(m_SOD.Parse(JPC, Stream));
								NCSJP2_CHECKIO(Seek((m_SOT.m_nPsot ? (nBegin + m_SOT.m_nPsot) : (Stream.Size() - sizeof(UINT16))), CNCSJPCIOStream::START));
							} else {
								// only 1 SOD allowed!
								Error = NCS_FILEIO_ERROR;
							}
						break;

					default:
							Error = NCS_FILEIO_ERROR;
						break;
				}
			}			
			if(Error == NCS_SUCCESS) {
				if(bRecalcSizes) {
					RecalcSizes();
					bRecalcSizes = false;
				}
//				if(!m_SOD.m_bHaveMarker && Stream.IsPacketStream()) {
//					m_Packets.resize(GetNrPackets());
//					for(UINT32 p = 0; p < m_Packets.size(); p++) {
//						m_Packets[p].m_nPacket = p;
//					}
//					m_Packets.m_bHaveOffsets = true;
//				}
				m_bValid = true;
			}
		} else {
			Error = NCS_FILE_INVALID;
		}
	NCSJP2_CHECKIO_END();
//FIXME
	if(Error != NCS_SUCCESS) {
		m_SOT.m_bValid = false;
	}
	return(Error);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCTilePartHeader::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO_ERROR(m_SOT.UnParse(JPC, Stream));
	NCSJP2_CHECKIO_END();
	return(Error);
}

CNCSError CNCSJPCTilePartHeader::RecalcSizes()
{
	m_bSimpleStructure = (m_POC.m_bValid == false) && (m_pJPC->m_POC.m_bValid == false);

	for(UINT32 c = 0; c < m_Components.size(); c++) {
		CNCSJPCComponent *pComponent = m_Components[c];
		CNCSJPCComponent *pPrevComponent = (c > 0) ? m_Components[c - 1] : NULL;
		UINT32 nResolutions = pComponent->m_CodingStyle.m_SPcod.m_nLevels + 1;

		if(pPrevComponent && m_bSimpleStructure) {
			if(nResolutions != pPrevComponent->m_CodingStyle.m_SPcod.m_nLevels + 1) {
				m_bSimpleStructure = false;
			}
			if(pComponent->m_CodingStyle.m_SGcod.m_nLayers != pPrevComponent->m_CodingStyle.m_SGcod.m_nLayers) {
				m_bSimpleStructure = false;
			}
			if(pComponent->GetWidth() != pPrevComponent->GetWidth() ||
			   pComponent->GetHeight() != pPrevComponent->GetHeight()) {
				m_bSimpleStructure = false;
			}
			if(pComponent->GetX0() != pPrevComponent->GetX0() ||
			   pComponent->GetY0() != pPrevComponent->GetY0()) {
				m_bSimpleStructure = false;
			}
		}
		pComponent->m_Resolutions.resize(nResolutions);
		for(UINT32 r = 0; r < pComponent->m_Resolutions.size(); r++) {
			pComponent->m_Resolutions[r] = new CNCSJPCResolution(pComponent, r);

			if(pPrevComponent && m_bSimpleStructure) {
				if(pComponent->m_Resolutions[r]->GetPrecinctWidth() != pPrevComponent->m_Resolutions[r]->GetPrecinctWidth() ||
				   pComponent->m_Resolutions[r]->GetPrecinctHeight() != pPrevComponent->m_Resolutions[r]->GetPrecinctHeight()) {
					m_bSimpleStructure = false;
				}
			}
		}
	}
	return(NCS_SUCCESS);
}

// Increment the progression order from the current location, parsing packet headers in the process.
CNCSError CNCSJPCTilePartHeader::Iterate(CNCSJPCTilePartHeader *pMainTP, void *pData, bool &bComplete)
{
	m_bSeekable = m_pJPC->m_pStream->Seek();

	if((!m_pJPC->m_pStream->IsPacketStream() || m_bSeekable == false) && pData) {
		m_bParsingHeaders = true;
	} else {
		m_bParsingHeaders = false;
	}
	if(m_bParsingHeaders || CurrentPO(this)) {
		m_bDynPrecinct = true;
	} else {
		m_bDynPrecinct = false;
	}
	return(CNCSJPCProgression::Iterate(pMainTP, pData, bComplete));
}


bool CNCSJPCTilePartHeader::StepRow(CNCSJPCComponent *pComponent,
									CNCSJPCResolution *pResolution)
{
	if(!m_bDynPrecinct) {
		if(pResolution->m_Precincts.empty(m_nCurPrecinctY)) {
			if(pComponent->m_CodingStyle.m_SGcod.m_ProgressionOrder.m_eType == CNCSJPCProgressionOrderType::RPCL) {
				if(!m_bSimpleStructure) {
					return(false);
				}
				m_nCurPacket += (pResolution->GetNumPrecinctsWide() - m_nCurPrecinctX) * (NCSJPCPacketId)(m_Components.size() * m_Components[0]->m_CodingStyle.m_SGcod.m_nLayers);
			} else {
				m_nCurPacket += pResolution->GetNumPrecinctsWide() - m_nCurPrecinctX;
			}
			return(true);
		}
	}
	return(false);
}

bool CNCSJPCTilePartHeader::Step(CNCSJPCComponent *pComponent,
								 CNCSJPCResolution *pResolution,
								 CNCSJPCPrecinct *pPrecinct)
{
	bool bRet = true;
//	CNCSJPCComponent *pComponent = m_Components[m_nCurComponent];
	CNCSJPCPacketList *pPacketList = (CNCSJPCPacketList*)m_pData;

//	if(pComponent) {
//		CNCSJPCResolution *pResolution = pComponent->m_Resolutions[m_nCurResolution];
		
//		if(pResolution && pResolution->m_Precincts.size() > m_nCurPrecinct) {
//			CNCSJPCPrecinct *pPrecinct = pResolution->m_Precincts.find(m_nCurPrecinctX, m_nCurPrecinctY);

			if(!m_bDynPrecinct) {
				if(pPrecinct) {
					if(pPrecinct->m_nProgressionLayer <= m_nCurLayer) {
						pPrecinct->m_Packets[m_nCurLayer] = m_nCurPacket++;
						pPrecinct->m_nProgressionLayer = m_nCurLayer + 1;
					}
				} else {
					m_nCurPacket++;
				}
			} else {
				if(!pPrecinct) {
					pPrecinct = new CNCSJPCPrecinct(pResolution, m_nCurPrecinctX + m_nCurPrecinctY * pResolution->GetNumPrecinctsWide());
					pResolution->m_Precincts.insert(m_nCurPrecinctX, m_nCurPrecinctY, pPrecinct);
					pPrecinct->AddRef();
				}
				if(m_bParsingHeaders) {
					if(pPrecinct->m_nProgressionLayer <= m_nCurLayer) {
						m_Error = ((CNCSJPCPacketList*)m_pData)->ParseHeader(m_bDone, m_nCurPacket);
						pPrecinct->m_nProgressionLayer = m_nCurLayer + 1;
						m_nCurPacket++;
					}
					bRet = (m_Error == NCS_SUCCESS);
				} else {
					if(pPrecinct) {
						if(pPrecinct->m_nProgressionLayer <= m_nCurLayer) {
							pPrecinct->m_Packets[m_nCurLayer] = m_nCurPacket++;
							pPrecinct->m_nProgressionLayer = m_nCurLayer + 1;
						}
					} else {
						m_nCurPacket++;
					}
				}
				if(m_nCurLayer == pComponent->m_CodingStyle.m_SGcod.m_nLayers - 1 && 
				   m_bSeekable != false && m_POC.m_nCurProgression >= m_POC.m_Progressions.size()) {
					pPrecinct->UnRef();
					// FIXME - call purge method in JP2File here
					if(pPrecinct->NrRefs() == 0) {
						pResolution->m_Precincts.remove(m_nCurPrecinctX, m_nCurPrecinctY);
					}
				}
			}
//		}
//	}
	return(true);
}

// Get the Tile Index
INT32 CNCSJPCTilePartHeader::GetIndex()
{
	return(m_SOT.m_nIsot);
}

// Get the Tile P index
INT32 CNCSJPCTilePartHeader::GetP()
{
	return(m_pJPC->m_SIZ.GetTilePFromIndex(GetIndex()));
}

// Get the Tile Q index
INT32 CNCSJPCTilePartHeader::GetQ()
{
	return(m_pJPC->m_SIZ.GetTileQFromIndex(GetIndex()));
}

// Get the Tile TX0 coordinate in reference grid
INT32 CNCSJPCTilePartHeader::GetX0()
{
	if(!m_X0.Cached()) {
		m_X0 = NCSMax(m_pJPC->m_SIZ.m_nXTOsiz + GetP() * m_pJPC->m_SIZ.m_nXTsiz, m_pJPC->m_SIZ.m_nXOsiz);
	}
	return(m_X0);
}

// Get the Tile TY0 coordinate in reference grid 
INT32 CNCSJPCTilePartHeader::GetY0()
{
	if(!m_Y0.Cached()) {
		m_Y0 = NCSMax(m_pJPC->m_SIZ.m_nYTOsiz + GetQ() * m_pJPC->m_SIZ.m_nYTsiz, m_pJPC->m_SIZ.m_nYOsiz);
	}
	return(m_Y0);
}

// Get the Tile TX1 coordinate in reference grid
INT32 CNCSJPCTilePartHeader::GetX1()
{
	if(!m_X1.Cached()) {
		m_X1 = NCSMin(m_pJPC->m_SIZ.m_nXTOsiz + (GetP() + 1) * m_pJPC->m_SIZ.m_nXTsiz, m_pJPC->m_SIZ.m_nXsiz);
	}
	return(m_X1);
}

// Get the Tile TY1 coordinate in reference grid
INT32 CNCSJPCTilePartHeader::GetY1()
{
	if(!m_Y1.Cached()) {
		m_Y1 = NCSMin(m_pJPC->m_SIZ.m_nYTOsiz + (GetQ() + 1) * m_pJPC->m_SIZ.m_nYTsiz, m_pJPC->m_SIZ.m_nYsiz);
	}
	return(m_Y1);
}

// Get the PPx from the component resolution
UINT32 CNCSJPCTilePartHeader::GetPPx(UINT16 iComponent, UINT8 nResolution)
{
	return(m_Components[iComponent]->m_CodingStyle.m_Scod.bDefinedPrecincts ? m_Components[iComponent]->m_CodingStyle.m_SPcod.m_PrecinctSizes[nResolution].m_nPPx : 15);
}

// Get the PPy from the component resolution
UINT32 CNCSJPCTilePartHeader::GetPPy(UINT16 iComponent, UINT8 nResolution)
{
	return(m_Components[iComponent]->m_CodingStyle.m_Scod.bDefinedPrecincts ? m_Components[iComponent]->m_CodingStyle.m_SPcod.m_PrecinctSizes[nResolution].m_nPPy : 15);
}

// Get the xcb' from the resolution
UINT32 CNCSJPCTilePartHeader::GetXCBPrime(UINT16 iComponent, UINT8 nResolution)
{
	return(NCSMin(m_Components[iComponent]->m_CodingStyle.m_SPcod.m_nXcb, GetPPx(iComponent, nResolution) - (nResolution == 0 ? 0 : 1)));
}

// Get the ycb' from the resolution
UINT32 CNCSJPCTilePartHeader::GetYCBPrime(UINT16 iComponent, UINT8 nResolution)
{
	return(NCSMin(m_Components[iComponent]->m_CodingStyle.m_SPcod.m_nYcb, GetPPy(iComponent, nResolution) - (nResolution == 0 ? 0 : 1)));
}

// Get the CB Width from the resolution
UINT32 CNCSJPCTilePartHeader::GetCBWidth(UINT16 iComponent, UINT8 nResolution)
{
	return(NCS2Pow(GetXCBPrime(iComponent, nResolution)));
}

// Get the CB Height from the resolution
UINT32 CNCSJPCTilePartHeader::GetCBHeight(UINT16 iComponent, UINT8 nResolution)
{
	return(NCS2Pow(GetYCBPrime(iComponent, nResolution)));
}

// Get the Packet Info for the specified packet number
CNCSJPCPacket *CNCSJPCTilePartHeader::GetPacketHeader(UINT32 nPacket)
{
	CNCSJPCTilePartHeader *pMainTP = m_pJPC->GetTile(m_SOT.m_nIsot);
	CNCSJPCTilePartHeader *pTP = pMainTP;
	CNCSJPC &JPC = *pMainTP->m_pJPC;
	CNCSJPCIOStream &Stream = *pMainTP->m_pJPC->m_pStream;

	if(m_PLTs.size()) {
		pTP = pMainTP;
		UINT64 nHeaderLength = 0;
		UINT64 nDataLength = 0;
		UINT32 p = nPacket - GetFirstPacketNr();
		UINT32 nPLT = 0;
		UINT32 nPLTs = (UINT32)m_PLTs.size();

		if(m_PLTIndex.size() == 0) {
			UINT32 p = 0;
			m_PLTIndex.resize(1 + GetNrPackets() / 32768);
			for(UINT32 nPLT = 0; nPLT < nPLTs; nPLT++) {
				CNCSJPCPLTMarker &PLT = m_PLTs[nPLT];
				if(PLT.m_nLengths > 0) {
					CNCSJP2File::Purge();
					// Will parse upto and including PLT.m_nLengths
					if(PLT.ParseLength(JPC, Stream, PLT.m_nLengths - 1)) {
						bool bCarryForward = false;
						std::vector<CNCSJPCPacketLengthType> *pLengths = PLT.GetLengths();

						if(nPLTs > 1 && nPLT < nPLTs - 1) {
							m_PLTs[nPLT+1].m_nFirstPacket += PLT.m_nLengths + PLT.m_nFirstPacket;
							m_PLTs[nPLT+1].m_nHeaderLengthOffset += PLT.m_nHeaderLengthOffset;
							m_PLTs[nPLT+1].m_nDataLengthOffset += PLT.m_nDataLengthOffset;
							if(PLT.m_bCumulativeLengths) {
								m_PLTs[nPLT+1].m_nHeaderLengthOffset += (*pLengths)[PLT.m_nLengths - 1].m_nHeaderLength;
								m_PLTs[nPLT+1].m_nDataLengthOffset += (*pLengths)[PLT.m_nLengths - 1].m_nDataLength;
							} else {
								bCarryForward = true;
							}
						}
						if(bCarryForward) {
							for(UINT32 i = p; i < p + PLT.m_nLengths; i++) {
								if(i % 32768 == 0) {
									m_PLTIndex[i / 32768] = nPLT;
								}
								m_PLTs[nPLT+1].m_nHeaderLengthOffset += (*pLengths)[i-p].m_nHeaderLength;
								m_PLTs[nPLT+1].m_nDataLengthOffset += (*pLengths)[i-p].m_nDataLength;
							}
						} else {
							for(UINT32 i = 1 + p / 32768; i <= (p + PLT.m_nLengths - 1) / 32768; i++) {
								m_PLTIndex[i] = nPLT;
							}
						}
						p += PLT.m_nLengths;
					} else {
						break;
					}
				}
			}	
		}
		if(m_PLTs.size() != 0) {
			nPLT = m_PLTIndex[p / 32768];
			p -= m_PLTs[nPLT].m_nFirstPacket;
			nHeaderLength += m_PLTs[nPLT].m_nHeaderLengthOffset;
			nDataLength += m_PLTs[nPLT].m_nDataLengthOffset;
		}
		for(; nPLT < nPLTs; nPLT++) {
			CNCSJPCPLTMarker &PLT = m_PLTs[nPLT];
			CNCSJP2File::Purge();
			// Will parse upto and including PLT.m_nLengths
			if(PLT.ParseLength(JPC, Stream, PLT.m_nLengths - 1)) {
				std::vector<CNCSJPCPacketLengthType> *pLengths = PLT.GetLengths();

				for(UINT32 i = 0; i < PLT.m_nLengths; i++) {
					if(PLT.m_bCumulativeLengths) {
						if(p < PLT.m_nLengths) {
							if(p > 0) {
								nHeaderLength += (*pLengths)[p - 1].m_nHeaderLength;
								nDataLength += (*pLengths)[p - 1].m_nDataLength;
//								fprintf(stdout, "Packet %ld PLT %ld P %ld\r\n", nPacket, nPLT, p);
								i = p;
								p = 0;
							}
						} else {
							nHeaderLength += (*pLengths)[PLT.m_nLengths - 1].m_nHeaderLength;
							nDataLength += (*pLengths)[PLT.m_nLengths - 1].m_nDataLength;	
							p -= PLT.m_nLengths;
							break;
						}
					}
					CNCSJPCPacketLengthType &Length = (*pLengths)[i];
					if(p == 0) {
						CNCSJPCPacket *pPH = new CNCSJPCPacket;
					
						pPH->m_nPacket = nPacket;
						pPH->m_nLength = Length.m_nHeaderLength;
						pPH->m_nDataLength = Length.m_nDataLength;
						if(PLT.m_bCumulativeLengths && i > 0) {
							pPH->m_nLength -= (*pLengths)[i-1].m_nHeaderLength;
							pPH->m_nDataLength -= (*pLengths)[i-1].m_nDataLength;
						}
						if(pMainTP->m_pJPC->m_bFilePPMs) {
							while(pTP && nDataLength >= pTP->m_SOT.m_nPsot - (pTP->m_SOD.m_nDataOffset - pTP->m_SOT.m_nOffset)) {
								nDataLength -= pTP->m_SOT.m_nPsot - (UINT32)(pTP->m_SOD.m_nDataOffset - pTP->m_SOT.m_nOffset);
								pTP = JPC.GetTile(pMainTP->m_SOT.m_nIsot, pTP->m_SOT.m_nTPsot + 1);
							}
							if(pTP) {
								pPH->m_nDataOffset = pTP->m_SOD.m_nDataOffset + nDataLength;

								UINT32 nZtp = pTP->m_nZtp;
								UINT32 i = 0;
								while(nZtp >= (UINT32)JPC.m_PPMs[i].m_TileParts.size()) {
									nZtp -= (UINT32)JPC.m_PPMs[i].m_TileParts.size();
									i++;
								}
								pPH->m_nOffset = JPC.m_PPMs[i].m_TileParts[nZtp].m_nOffset + nHeaderLength;
							}
						} else if(pMainTP->m_bFilePPTs) {
							while(pTP && nDataLength >= pTP->m_SOT.m_nPsot - (pTP->m_SOD.m_nDataOffset - pTP->m_SOT.m_nOffset)) {
								nDataLength -= pTP->m_SOT.m_nPsot - (UINT32)(pTP->m_SOD.m_nDataOffset - pTP->m_SOT.m_nOffset);
								pTP = JPC.GetTile(pMainTP->m_SOT.m_nIsot, pTP->m_SOT.m_nTPsot + 1);
							}
							if(pTP) {
								pPH->m_nDataOffset = pTP->m_SOD.m_nDataOffset + nDataLength;

								UINT32 i = 0;
								while(nHeaderLength >= pMainTP->m_PPTs[i].m_nLength - (sizeof(UINT16) + sizeof(UINT8))) {
									nHeaderLength -= pMainTP->m_PPTs[i].m_nLength - (sizeof(UINT16) + sizeof(UINT8));
									i++;
								}
								pPH->m_nOffset = pMainTP->m_PPTs[i].m_nOffset + 2 * sizeof(UINT16) + sizeof(UINT8) + nHeaderLength;
							}
						} else {
							UINT64 nOffset = nHeaderLength + nDataLength;
							while(pTP && nOffset >= pTP->m_SOT.m_nPsot - (pTP->m_SOD.m_nDataOffset - pTP->m_SOT.m_nOffset)) {
								nOffset -= pTP->m_SOT.m_nPsot - (pTP->m_SOD.m_nDataOffset - pTP->m_SOT.m_nOffset);
								pTP = JPC.GetTile(pMainTP->m_SOT.m_nIsot, pTP->m_SOT.m_nTPsot + 1);
							}
							if(pTP) {
								pPH->m_nOffset = pTP->m_SOD.m_nDataOffset + nOffset;
								pPH->m_nDataOffset = 0;//pPH->m_nOffset + pPH->m_nLength;
							}
						}
						return(pPH);
					}
					p--;
			//		PLT.ParseLength(JPC, Stream, i);
			//		CNCSJPCPacketLengthType &Length = PLT.m_Lengths[i];
					nHeaderLength += Length.m_nHeaderLength;
					nDataLength += Length.m_nDataLength;
				}
			} else {
				break;
			}
		}
	}
	CNCSJPCPacket *pPacket = new CNCSJPCPacket;
	pPacket->m_nPacket = nPacket;
	return(pPacket);
}

// Get the number of packets for this tile
UINT32 CNCSJPCTilePartHeader::GetFirstPacketNr()
{
	if(!m_FirstPacketNr.Cached()) {
		UINT32 t;
		UINT32 nPackets = 0;
		for(t = 0; t < m_SOT.m_nIsot; t++) {
			nPackets += m_pJPC->GetTile(t)->GetNrPackets();
		}
		m_FirstPacketNr = nPackets;
	}
	return(m_FirstPacketNr);
}
// Get the number of packets for this tile
UINT32 CNCSJPCTilePartHeader::GetNrPackets()
{
	if(!m_NrPackets.Cached()) {
		UINT32 nPackets = 0;
		for(UINT32 c = 0; c < m_pJPC->m_SIZ.m_nCsiz; c++) {
			CNCSJPCComponent *pComponent = m_Components[c];
			for(int r = 0; r < pComponent->m_CodingStyle.m_SPcod.m_nLevels + 1; r++) {
				CNCSJPCResolution *pResolution = pComponent->m_Resolutions[r];
				nPackets += pResolution->GetNumPrecinctsHigh() * 
							pResolution->GetNumPrecinctsWide() * 
							pComponent->m_CodingStyle.m_SGcod.m_nLayers;
			}
		}
		m_NrPackets = nPackets;
	}
	return(m_NrPackets);
}

// Write a BufferType line to the output.
bool CNCSJPCTilePartHeader::WriteLine(ContextID nCtx, CNCSJPCBuffer *pSrc, UINT16 iComponent)
{
	bool bRet = m_Components[iComponent]->WriteLine(nCtx, pSrc, iComponent);
	
//	if(pSrc->GetY0() == GetY1() - 1 && iComponent == m_pJPC->m_SIZ.m_nCsiz - 1) {
//		CloseEncoderPLTFile(false);
//	}
	return(bRet);
}
