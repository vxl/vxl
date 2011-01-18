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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCMainHeader.cpp $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCMainHeader class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPC.h"
#include "NCSBuildNumber.h"
#include "NCSUtil.h"
#include "NCSJPCFileIOStream.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCMainHeader::CNCSJPCMainHeader()
{
	m_bValid = false;
	m_bFilePPMs = false;

	m_COM.m_pComment = NCSStrDup("ECW JPEG 2000 SDK v" NCS_VERSION_STRING_DOT_DEL);
	m_COM.m_eRegistration = CNCSJPCCOMMarker::IS_8859_LATIN;
	m_COM.m_nLength = 2 * sizeof(UINT16) + (UINT16)strlen((char*)m_COM.m_pComment);
	m_COM.m_bValid = true;

	m_pTmpDir = NULL;

	m_pEncoderPLTFile = NULL;
	m_pEncoderOffsetFile = NULL;
}

// Destructor
CNCSJPCMainHeader::~CNCSJPCMainHeader()
{
	CloseEncoderFiles(true);
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCMainHeader::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;
	CNCSJPCMarker marker;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		Error = marker.Parse(JPC, Stream);
		if(Error == NCS_SUCCESS) {
			if(marker.m_eMarker == CNCSJPCMarker::SOC) {
				(CNCSJPCMarker&)m_SOC = marker;
				NCSJP2_CHECKIO_ERROR(m_SOC.Parse(JPC, Stream));
				NCSJP2_CHECKIO_ERROR(marker.Parse(JPC, Stream));
				
				if(marker.m_eMarker == CNCSJPCMarker::SIZ) {
					(CNCSJPCMarker&)m_SIZ = marker;
					NCSJP2_CHECKIO_ERROR(m_SIZ.Parse(JPC, Stream));

					//initialize JPC.m_Tiles
					if( JPC.m_Tiles.size() == 0 ) {
						INT32 nNumTiles = JPC.GetNumXTiles() * JPC.GetNumYTiles();
						JPC.m_Tiles.reserve( nNumTiles );
						for( int i=0; i < nNumTiles; i++ ) {
							JPC.m_Tiles.push_back(NULL);
						}
					}

					while(Error == NCS_SUCCESS && !m_EOC.m_bValid) {
						NCSJP2_CHECKIO_ERROR(marker.Parse(JPC, Stream));

						switch(marker.m_eMarker) {
							case CNCSJPCMarker::COD:
									if(!m_COD.m_bHaveMarker) {
										(CNCSJPCMarker&)m_COD = marker;
										NCSJP2_CHECKIO_ERROR(m_COD.Parse(JPC, Stream));
										// Setup default coding style for each component
										m_CodingStyles.resize(m_SIZ.m_nCsiz, m_COD);
									} else {
										Error = NCS_FILEIO_ERROR;
									}
								break;
							
							case CNCSJPCMarker::COC:
									if(m_COD.m_bValid) {
										CNCSJPCCOCMarker COC;
										(CNCSJPCMarker&)COC = marker;
										NCSJP2_CHECKIO_ERROR(COC.Parse(JPC, Stream));
											// Over-ride coding style for this component
										m_CodingStyles[COC.m_nCcoc] = COC;
									} else {
										// COD *must* be before COC
										Error = NCS_FILEIO_ERROR;
									}
								break;

							case CNCSJPCMarker::QCD:
									if(!m_QCD.m_bHaveMarker) {
										(CNCSJPCMarker&)m_QCD = marker;
										NCSJP2_CHECKIO_ERROR(m_QCD.Parse(JPC, Stream));
										// Setup default coding style for each component
										m_QuantizationStyles.resize(m_SIZ.m_nCsiz, m_QCD);
									} else {
										Error = NCS_FILEIO_ERROR;
									}
								break;

							case CNCSJPCMarker::QCC:
									if(m_QCD.m_bValid) {
										CNCSJPCQCCMarker QCC;
										(CNCSJPCMarker&)QCC = marker;
										NCSJP2_CHECKIO_ERROR(QCC.Parse(JPC, Stream));
											// Over-ride quantization style for this component
										m_QuantizationStyles[QCC.m_nCqcc] = QCC;
									} else {
										// QCD *must* be before QCC
										Error = NCS_FILEIO_ERROR;
									}
								break;

							case CNCSJPCMarker::RGN:
									{
										CNCSJPCRGNMarker RGN;
										(CNCSJPCMarker&)RGN = marker;									
										NCSJP2_CHECKIO_ERROR(RGN.Parse(JPC, Stream));
										m_RGNs.resize(m_SIZ.m_nCsiz);
										m_RGNs[RGN.m_nCrgn] = RGN;
									}
								break;

							case CNCSJPCMarker::POC:
									if(!m_POC.m_bHaveMarker) {
										(CNCSJPCMarker&)m_POC = marker;
										NCSJP2_CHECKIO_ERROR(m_POC.Parse(JPC, Stream));
									} else {
										Error = NCS_FILEIO_ERROR;
									}
								break;

							case CNCSJPCMarker::PPM:
									if(true) 
									{
										m_bFilePPMs = true;
										CNCSJPCPPMMarker PPM;
										(CNCSJPCMarker&)PPM = marker;									
										NCSJP2_CHECKIO_ERROR(PPM.Parse(JPC, Stream));
										m_PPMs.resize(NCSMax(m_PPMs.size(), (UINT32)PPM.m_nZppm + 1));
										m_PPMs[PPM.m_nZppm] = PPM;
									} else {
										Error = NCS_FILEIO_ERROR;
									}
								break;

							case CNCSJPCMarker::TLM:
									{
										CNCSJPCTLMMarker TLM;
										(CNCSJPCMarker&)TLM = marker;									
										NCSJP2_CHECKIO_ERROR(TLM.Parse(JPC, Stream));
										m_TLMs.resize(NCSMax(m_TLMs.size(), (UINT32)TLM.m_nZtlm + 1));
										m_TLMs[TLM.m_nZtlm] = TLM;
									}
								break;

							case CNCSJPCMarker::PLM:
									{
										CNCSJPCPLMMarker PLM;
										(CNCSJPCMarker&)PLM = marker;									
										NCSJP2_CHECKIO_ERROR(PLM.Parse(JPC, Stream));
										m_PLMs.resize(NCSMax(m_PLMs.size(), (UINT32)PLM.m_nZplm + 1));
										m_PLMs[PLM.m_nZplm] = PLM;
									}
								break;

							case CNCSJPCMarker::CRG:
									if(!m_CRG.m_bHaveMarker) {
										(CNCSJPCMarker&)m_CRG = marker;
										NCSJP2_CHECKIO_ERROR(m_CRG.Parse(JPC, Stream));
									} else {
										// Can only have one CRG marker in the codestream
										Error = NCS_FILEIO_ERROR;
									}
								break;

							case CNCSJPCMarker::SOT:
									{
										CNCSJPCTilePartHeader *pTP = new CNCSJPCTilePartHeader(&JPC);
										if(pTP) {
											(CNCSJPCMarker&)pTP->m_SOT = marker;
											JPC.m_pCurrentTilePart = pTP;
											NCSJP2_CHECKIO_ERROR(pTP->Parse(JPC, Stream));

											if( pTP->m_SOT.m_nTPsot == 0 ) { //first tile part
												if( JPC.m_Tiles[pTP->m_SOT.m_nIsot] == NULL ) {
													JPC.m_Tiles[pTP->m_SOT.m_nIsot] = pTP;
												} else {
													// Got TP twice, just ignore.
													delete pTP;
												}
											} else if( JPC.m_Tiles[pTP->m_SOT.m_nIsot] ) { // other parts
												if( (JPC.m_Tiles[pTP->m_SOT.m_nIsot]->m_TileParts.size() >= (pTP->m_SOT.m_nTPsot)) &&
													(JPC.m_Tiles[pTP->m_SOT.m_nIsot]->m_TileParts[pTP->m_SOT.m_nTPsot] != NULL) )
												{
													// Got TP twice, just ignore.
													delete pTP;
												} else {
													JPC.m_Tiles[pTP->m_SOT.m_nIsot]->m_TileParts.push_back(pTP);
												}
											} else {
												// this shouldn't happen
												Error = NCS_COULDNT_ALLOC_MEMORY;
											}

										} else {
											Error = NCS_COULDNT_ALLOC_MEMORY;
										}
									}
								break;

							case CNCSJPCMarker::COM:
									{
										CNCSJPCCOMMarker COM;
										(CNCSJPCMarker&)COM = marker;
										NCSJP2_CHECKIO_ERROR(COM.Parse(JPC, Stream));
									}
								break;

							case CNCSJPCMarker::EOC:
									if(!m_EOC.m_bHaveMarker) {
										(CNCSJPCMarker&)m_EOC = marker;
										NCSJP2_CHECKIO_ERROR(m_EOC.Parse(JPC, Stream));
									} else {
										Error = NCS_FILEIO_ERROR;
									}
								break;

							default:
									if(marker.m_eMarker >= 0xff30 && marker.m_eMarker <= 0xff3f) {
										// Skip
									} else {
										Error = NCS_FILEIO_ERROR;
									}
								break;
						}
					}
				//	if(Error == NCS_SUCCESS) {
						if(m_SOC.m_bValid && m_SIZ.m_bValid && m_COD.m_bValid && 
							(JPC.m_pStream->IsPacketStream() || 
							 (JPC.m_pCurrentTilePart && JPC.m_pCurrentTilePart->m_SOT.m_bValid))) {
							m_bValid = true;
							Error = NCS_SUCCESS;
						}
				//	}
					CNCSJPCTilePartHeader *pTP = JPC.GetTile(0);
					while(pTP) {
						if(pTP->m_bFilePPTs) {
							JPC.m_pCurrentTilePart = pTP;
				 			bool bComplete = false;
							CNCSJPCPacketList Packets;
							Error = Packets.Parse(JPC, Stream, bComplete, (UINT64)-1);
							if(Error == NCS_SUCCESS) {
								Error = Packets.GenDynamicPLTs(pTP);
								Packets.clear();
							}
						}
						pTP = JPC.GetTile(pTP->m_SOT.m_nIsot + 1);
					}
				} else {
					Error = NCS_FILE_INVALID;
				}
			} else {
				Error = NCS_FILE_INVALID;
			}
		}
		if(Error == NCS_SUCCESS) {
			for(int y = 0; y < GetNumYTiles(); y++) {
				for(int x = 0; x < GetNumXTiles(); x++) {
					CNCSJPCTilePartHeader *pTP = JPC.GetTile(x + y * GetNumXTiles());
					if(pTP) {
						pTP->m_nCurPacket = pTP->GetFirstPacketNr();
					}	
				}
			}
		}
	NCSJP2_CHECKIO_END();

	return(Error);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCMainHeader::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO_ERROR(m_SOC.UnParse(JPC, Stream));
		NCSJP2_CHECKIO_ERROR(m_SIZ.UnParse(JPC, Stream));
		NCSJP2_CHECKIO_ERROR(m_COD.UnParse(JPC, Stream));
		NCSJP2_CHECKIO_ERROR(m_QCD.UnParse(JPC, Stream));
		NCSJP2_CHECKIO_ERROR(m_COM.UnParse(JPC, Stream));
		if(false && m_COD.m_SGcod.m_bMCT) {
			CNCSJPCQCCMarker QCC = m_QuantizationStyles[1];
			QCC.m_nCqcc = 1;
	//		if(
			QCC.m_nLength = 5 + 3 * JPC.m_CodingStyles[1].m_SPcod.m_nLevels;
			NCSJP2_CHECKIO_ERROR(QCC.UnParse(JPC, Stream));
			QCC = m_QuantizationStyles[2];
			QCC.m_nCqcc = 2;
			NCSJP2_CHECKIO_ERROR(QCC.UnParse(JPC, Stream));
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// Get X0, the component top left in reference grid
INT32 CNCSJPCMainHeader::GetComponentX0(INT32 iComponent)
{
	return(m_SIZ.GetComponentX0(iComponent));
}

// Get X1, the component bottom right in reference grid
INT32 CNCSJPCMainHeader::GetComponentX1(INT32 iComponent)
{
	return(m_SIZ.GetComponentX1(iComponent));
}

// Get Y0, the component top left in reference grid
INT32 CNCSJPCMainHeader::GetComponentY0(INT32 iComponent)
{
	return(m_SIZ.GetComponentY0(iComponent));
}

// Get Y1, the component bottom right in reference grid
INT32 CNCSJPCMainHeader::GetComponentY1(INT32 iComponent)
{
	return(m_SIZ.GetComponentY1(iComponent));
}

// Get the component width in reference grid
INT32 CNCSJPCMainHeader::GetComponentWidth(INT32 iComponent)
{
	return(m_SIZ.GetComponentWidth(iComponent));
}

// Get the component width in reference grid
INT32 CNCSJPCMainHeader::GetComponentHeight(INT32 iComponent)
{
	return(m_SIZ.GetComponentHeight(iComponent));
}

// Get NumXTiles
INT32 CNCSJPCMainHeader::GetNumXTiles()
{
	return(m_SIZ.GetNumXTiles());
}

// Get NumYTiles
INT32 CNCSJPCMainHeader::GetNumYTiles()
{
	return(m_SIZ.GetNumYTiles());
}

// Get the Tile P index from the tile index
INT32 CNCSJPCMainHeader::GetTilePFromIndex(INT32 iIndex)
{
	return(m_SIZ.GetTilePFromIndex(iIndex));
}

// Get the Tile Q index from the tile index
INT32 CNCSJPCMainHeader::GetTileQFromIndex(INT32 iIndex)
{
	return(m_SIZ.GetTileQFromIndex(iIndex));
}

// Get the Tile Index from the tile PQ index
INT32 CNCSJPCMainHeader::GetTileIndexFromPQ(INT32 iPIndex, INT32 iQIndex)
{
	return(m_SIZ.GetTileIndexFromPQ(iPIndex, iQIndex));
}

bool CNCSJPCMainHeader::OpenEncoderFiles(bool bWrite)
{
	bool bRet = true;
	char szRelTmpDir[MAX_PATH+1];

	if(m_pEncoderPLTFile == NULL) {
		sprintf(szRelTmpDir, "%s" NCS_FILE_SEP "l.tmp", 
							m_pTmpDir);

		m_pEncoderPLTFile = new CNCSJPCFileIOStream();
		if(m_pEncoderPLTFile) {
//			if(!bWrite) {
//				((CNCSJPCFileIOStream*)m_pEncoderPLTFile)->SetIOCacheSize(0);
//			} else {
//				((CNCSJPCFileIOStream*)m_pEncoderPLTFile)->SetIOCacheSize(32768);				
//			}
			CNCSError Error = m_pEncoderPLTFile->Open(szRelTmpDir, bWrite);
			bRet = (Error == NCS_SUCCESS);
		} else {
			bRet = false;
		}
	}
	if(m_pEncoderOffsetFile == NULL) {
		sprintf(szRelTmpDir, "%s" NCS_FILE_SEP "o.tmp", 
							m_pTmpDir);

		m_pEncoderOffsetFile = new CNCSJPCFileIOStream();
		if(m_pEncoderOffsetFile) {
//			if(!bWrite) {
//				((CNCSJPCFileIOStream*)m_pEncoderOffsetFile)->SetIOCacheSize(0);
//			} else {
//				((CNCSJPCFileIOStream*)m_pEncoderOffsetFile)->SetIOCacheSize(32768);
//			}	
			CNCSError Error = m_pEncoderOffsetFile->Open(szRelTmpDir, bWrite);
			bRet = (Error == NCS_SUCCESS);
		} else {
			bRet = false;
		}
	}
	m_EncoderFiles.resize(m_COD.m_SPcod.m_nLevels + 1);
	
	for(INT32 r = 0; r < m_COD.m_SPcod.m_nLevels + 1; r++) {
		m_EncoderFiles[r] = new CNCSJPCFileIOStream();
	
		sprintf(szRelTmpDir, "%s" NCS_FILE_SEP "r%ldp.tmp", 
							m_pTmpDir, r);
		if(!bWrite) {
			((CNCSJPCFileIOStream*)m_EncoderFiles[r])->SetIOCacheSize(0);
		} else {	
			((CNCSJPCFileIOStream*)m_EncoderFiles[r])->SetIOCacheSize(32768);
		}	
		CNCSError Error = m_EncoderFiles[r]->Open(szRelTmpDir, bWrite);
		bRet &= (Error == NCS_SUCCESS);
	}
	return(bRet);
}

bool CNCSJPCMainHeader::CloseEncoderFiles(bool bDelete)
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
	if(m_pEncoderOffsetFile) {
		char *pPLTFilename = bDelete ? NCSStrDup(CHAR_STRING(m_pEncoderOffsetFile->GetName())) : NULL;
		bRet = m_pEncoderOffsetFile->Close() == NCS_SUCCESS;
		if(bDelete && pPLTFilename) {
			NCSDeleteFile(pPLTFilename);
			NCSFree(pPLTFilename);
		}
		delete m_pEncoderOffsetFile;
		m_pEncoderOffsetFile = NULL;
	}
	for(INT32 r = 0; r < (INT32)m_EncoderFiles.size(); r++) {
		char *pPLTFilename = bDelete ? NCSStrDup(CHAR_STRING(m_EncoderFiles[r]->GetName())) : NULL;
		bRet = m_EncoderFiles[r]->Close() == NCS_SUCCESS;
		if(bDelete && pPLTFilename) {
			NCSDeleteFile(pPLTFilename);
			NCSFree(pPLTFilename);
		}
		delete m_EncoderFiles[r];
		m_EncoderFiles[r] = NULL;
	}
	m_EncoderFiles.clear();
	return(bRet);
}
