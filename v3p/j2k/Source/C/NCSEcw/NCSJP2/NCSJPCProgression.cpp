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

#include "NCSJPCProgression.h"
#include "NCSJPCTilePartHeader.h"
#include "NCSJPC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCProgression::CNCSJPCProgression()
{
	m_nCurTile = 0;
	m_nCurLayer = 0;
	m_nCurResolution = 0;
	m_nCurComponent = 0;
	m_nCurPrecinctX = 0;
	m_nCurPrecinctY = 0;
	m_nCurPacket = 0;
	m_nCurTX = 0;
	m_nCurTY = 0;
	m_nIncTX = 0;
	m_nIncTY = 0;
}

// Destructor
CNCSJPCProgression::~CNCSJPCProgression()
{
}

// Get current POC PO if present
CNCSJPCPOCMarker::ProgressionOrder *CNCSJPCProgression::CurrentPO(CNCSJPCTilePartHeader *pMainTP)
{
	if(pMainTP && pMainTP->m_POC.m_bValid && pMainTP->m_POC.m_Progressions.size() > 0) {
		if(pMainTP->m_POC.m_nCurProgression >= pMainTP->m_POC.m_Progressions.size()) {
			pMainTP->m_POC.m_nCurProgression = 0;
		}
		return(&pMainTP->m_POC.m_Progressions[pMainTP->m_POC.m_nCurProgression]);
	}
	return(NULL);
}

// Increment the POC PO if present
CNCSError CNCSJPCProgression::IncrementPO(CNCSJPCTilePartHeader *pMainTP, bool &bComplete)
{
	bComplete;//Keep compiler happy
	if(pMainTP && pMainTP->m_POC.m_bValid) {
		pMainTP->m_POC.m_nCurProgression++;
//		if(pMainTP->m_POC.m_nCurProgression >= pMainTP->m_POC.m_Progressions.size()) {
//			pMainTP->m_POC.m_nCurProgression = 0;
//		} else {
				// Not really finished yet, more POC's to go
				// FIXME = do we need to recalc m_nCurLayer?
//			bComplete = false;
//		}

//		CNCSJPCPOCMarker::ProgressionOrder *pPO = CurrentPO(pMainTP);
//		return(Start(pMainTP, pPO ? pPO->m_nCSpoc : 0, pPO ? pPO->m_nRSpoc : 0));
	}
	return(NCS_SUCCESS);
}

CNCSError CNCSJPCProgression::Start(CNCSJPCTilePartHeader *pMainTP)
{
	for(UINT32 c = 0; c < pMainTP->m_Components.size(); c++) {
		UINT32 nResolutions = pMainTP->m_Components[c]->m_CodingStyle.m_SPcod.m_nLevels + 1;
		for(UINT32 r = 0; r < nResolutions; r++) {
			CNCSJPCResolution *pResolution = pMainTP->m_Components[c]->m_Resolutions[r];
			pResolution->m_Precincts.ResetProgressionLayer();
//			UINT32 nPrecincts = pResolution->GetNumPrecinctsWide() * pResolution->GetNumPrecinctsHigh();
//			for(UINT32 p = 0; p < nPrecincts; p++) {
//				CNCSJPCPrecinct *pPrecinct = pResolution->m_Precincts.find(p);
//				if(pPrecinct) {
//					pPrecinct->m_nProgressionLayer = 0;
//				}
//			}
		}
	}
	pMainTP->m_POC.m_nCurProgression = 0;
	CNCSJPCPOCMarker::ProgressionOrder *pPO = CurrentPO(pMainTP);
	return(Start(pMainTP, pPO ? pPO->m_nCSpoc : 0, pPO ? pPO->m_nRSpoc : 0));
}

CNCSError CNCSJPCProgression::Start(CNCSJPCTilePartHeader *pMainTP, INT32 nComponent, INT32 nResolution)
{
	m_nCurComponent = (UINT16)nComponent;
	m_nCurResolution = (UINT8)nResolution;
	m_nCurPacket = pMainTP->GetFirstPacketNr();
	m_nCurLayer = 0;
	m_nCurPrecinctX = 0;
	m_nCurPrecinctY = 0;
	m_nCurTX = 0;
	m_nCurTY = 0;

	CNCSJPCPOCMarker::ProgressionOrder *pPO = CurrentPO(pMainTP);

	CNCSJPCProgressionOrderType::Type eType = pPO ? pPO->m_Ppoc.m_eType : pMainTP->m_Components[m_nCurComponent]->m_CodingStyle.m_SGcod.m_ProgressionOrder.m_eType;

	if(eType == CNCSJPCProgressionOrderType::RPCL || eType == CNCSJPCProgressionOrderType::PCRL || eType == CNCSJPCProgressionOrderType::CPRL) {
		m_nCurTX = pMainTP->GetX0();
		m_nCurTY = pMainTP->GetY0();
		m_nCurPrecinctX = CalculatePrecinctX(pMainTP, pMainTP->m_Components[m_nCurComponent], pMainTP->m_Components[m_nCurComponent]->m_Resolutions[m_nCurResolution]);
		m_nCurPrecinctY = CalculatePrecinctY(pMainTP, pMainTP->m_Components[m_nCurComponent], pMainTP->m_Components[m_nCurComponent]->m_Resolutions[m_nCurResolution]);
		
		m_nIncTX = 0x7fffffff;
		m_nIncTY = 0x7fffffff;
				
		for(UINT32 c = 0; c < pMainTP->m_Components.size(); c++) {
			INT32 nResolutions = pMainTP->m_Components[c]->m_CodingStyle.m_SPcod.m_nLevels + 1;
//			INT32 nLevels = pMainTP->m_Components[c]->m_CodingStyle.m_SPcod.m_nLevels;
									
			for(INT32 r = 0; r < nResolutions; r++) {
				INT32 nDivX = pMainTP->m_Components[c]->m_Resolutions[r]->GetDivX();//pMainTP->m_pJPC->m_SIZ.m_Components[c].m_nXRsiz * NCS2Pow(pMainTP->GetPPx(c, r) + nLevels - r)
				INT32 nDivY = pMainTP->m_Components[c]->m_Resolutions[r]->GetDivY();//pMainTP->m_pJPC->m_SIZ.m_Components[c].m_nYRsiz * NCS2Pow(pMainTP->GetPPy(c, r) + nLevels - r);
		
				m_nIncTX = NCSMin(m_nIncTX, nDivX);
				m_nIncTY = NCSMin(m_nIncTY, nDivY);
			}
		}
	}
	return(NCS_SUCCESS);
}

CNCSError CNCSJPCProgression::Iterate(CNCSJPCTilePartHeader *pMainTP, void *pData, bool &bComplete)
{
	bComplete = false;
	CNCSJPCPOCMarker::ProgressionOrder *pPO = CurrentPO(pMainTP);
	INT32 nComponents = (pPO ? NCSMin(pMainTP->m_pJPC->m_SIZ.m_nCsiz, pPO->m_nCEpoc) : pMainTP->m_pJPC->m_SIZ.m_nCsiz);
	UINT32 nResolutionsMax = 1;
	UINT32 nLayersMax = 1;
	INT32 nMainTPX0 = pMainTP->GetX0();
	INT32 nMainTPX1 = pMainTP->GetX1();
	INT32 nMainTPY0 = pMainTP->GetY0();
	INT32 nMainTPY1 = pMainTP->GetY1();

	m_pData = pData;
	m_bDone = false;

	if(pPO && m_nCurComponent == 0 && m_nCurResolution == 0 && m_nCurPrecinctX == 0 && m_nCurPrecinctY == 0 && m_nCurLayer == 0) {
		UINT32 nCurPacket = m_nCurPacket;
		Start(pMainTP, pPO->m_nCSpoc, pPO->m_nRSpoc);
		m_nCurPacket = nCurPacket;
	}

	for(INT32 c = 0; c < nComponents; c++) {
		nResolutionsMax = NCSMax(nResolutionsMax, (UINT32)pMainTP->m_Components[c]->m_CodingStyle.m_SPcod.m_nLevels + 1);
		nLayersMax = NCSMax(nLayersMax, pMainTP->m_Components[c]->m_CodingStyle.m_SGcod.m_nLayers);
	}
	INT32 nResolutions = (pPO ? pPO->m_nREpoc : nResolutionsMax);
	INT32 nLayers = NCSMin(pPO ? pPO->m_nLYEpoc : nLayersMax, nLayersMax);

	switch(pPO ? pPO->m_Ppoc.m_eType : pMainTP->m_Components[m_nCurComponent]->m_CodingStyle.m_SGcod.m_ProgressionOrder.m_eType) {	
		case CNCSJPCProgressionOrderType::LRCP:
				for(; m_nCurLayer < nLayers; m_nCurLayer++) {
					for(; m_nCurResolution < nResolutions; m_nCurResolution++) {
						for(; m_nCurComponent < nComponents; m_nCurComponent++) {
							CNCSJPCComponent *pComponent = pMainTP->m_Components[m_nCurComponent];
							if(m_nCurResolution < pComponent->m_CodingStyle.m_SPcod.m_nLevels + 1) {
								CNCSJPCResolution *pResolution = pComponent->m_Resolutions[m_nCurResolution];
								UINT32 nPrecinctsWide = pResolution->GetNumPrecinctsWide();
								UINT32 nPrecinctsHigh = pResolution->GetNumPrecinctsHigh();
								if(nPrecinctsWide && nPrecinctsHigh) {
									for(; m_nCurPrecinctY < nPrecinctsHigh; m_nCurPrecinctY++) {
										if(!StepRow(pComponent, pResolution)) {
											for(; m_nCurPrecinctX < nPrecinctsWide; m_nCurPrecinctX++) {
												if(!m_bDone) {
													if(Step(pComponent,
															pResolution,
															pResolution->m_Precincts.find(m_nCurPrecinctX, m_nCurPrecinctY)) == false) {
														return(m_Error);
													}
												} else {
													return(m_Error);
												}
											}
										}
										m_nCurPrecinctX = 0;
									}
									m_nCurPrecinctY = 0;
								}
							}
						}
						m_nCurComponent = (pPO ? pPO->m_nCSpoc : 0);
					}
					m_nCurResolution = (pPO ? pPO->m_nRSpoc : 0);
				}
				m_nCurLayer = 0;
			break;

		case CNCSJPCProgressionOrderType::RLCP:
				for(; m_nCurResolution < nResolutions; m_nCurResolution++) {
					for(; m_nCurLayer < nLayers; m_nCurLayer++) {
						for(; m_nCurComponent < nComponents; m_nCurComponent++) {
							CNCSJPCComponent *pComponent = pMainTP->m_Components[m_nCurComponent];
							if(m_nCurResolution < pComponent->m_CodingStyle.m_SPcod.m_nLevels + 1) {
								CNCSJPCResolution *pResolution = pComponent->m_Resolutions[m_nCurResolution];
								UINT32 nPrecinctsWide = pResolution->GetNumPrecinctsWide();
								UINT32 nPrecinctsHigh = pResolution->GetNumPrecinctsHigh();

								if(nPrecinctsWide && nPrecinctsHigh) {
									for(; m_nCurPrecinctY < nPrecinctsHigh; m_nCurPrecinctY++) {
										if(!StepRow(pComponent, pResolution)) {
											for(; m_nCurPrecinctX < nPrecinctsWide; m_nCurPrecinctX++) {
												if(!m_bDone) {
													if(Step(pComponent,
															pResolution,
															pResolution->m_Precincts.find(m_nCurPrecinctX, m_nCurPrecinctY)) == false) {
														return(m_Error);
													}
												} else {
													return(m_Error);
												}
											}
										}
										m_nCurPrecinctX = 0;
									}
									m_nCurPrecinctY = 0;
								}
							}
						}
						m_nCurComponent = (pPO ? pPO->m_nCSpoc : 0);
					}
					m_nCurLayer = 0;
				}
				m_nCurResolution = (pPO ? pPO->m_nRSpoc : 0);
			break;

		case CNCSJPCProgressionOrderType::RPCL:
			if(pMainTP->SimpleStructure()) {
				for(; m_nCurResolution < nResolutions; m_nCurResolution++) {
					UINT32 nPrecinctsWide =	pMainTP->m_Components[0]->m_Resolutions[m_nCurResolution]->GetNumPrecinctsWide();
					UINT32 nPrecinctsHigh =	pMainTP->m_Components[0]->m_Resolutions[m_nCurResolution]->GetNumPrecinctsHigh();
					
					for(; m_nCurPrecinctY < nPrecinctsHigh; m_nCurPrecinctY++) {
						if(!StepRow(pMainTP->m_Components[0], 
									pMainTP->m_Components[0]->m_Resolutions[m_nCurResolution])) {
							for(; m_nCurPrecinctX < nPrecinctsWide; m_nCurPrecinctX++) {
								for(; m_nCurComponent < nComponents; m_nCurComponent++) {
									CNCSJPCComponent *pComponent = pMainTP->m_Components[m_nCurComponent];
									CNCSJPCResolution *pResolution = pComponent->m_Resolutions[m_nCurResolution];
//									INT32 nLevels = pComponent->m_CodingStyle.m_SPcod.m_nLevels;

									for(; m_nCurLayer < nLayers; m_nCurLayer++) {
										if(!m_bDone) {
											if(Step(pComponent,
													pResolution,
													pResolution->m_Precincts.find(m_nCurPrecinctX, m_nCurPrecinctY)) == false) {
												return(m_Error);
											}
										} else {
											return(m_Error);
										}
									}
									m_nCurLayer = 0;
								}
								m_nCurComponent = (pPO ? pPO->m_nCSpoc : 0);
							}
						}
						m_nCurPrecinctX = 0;
					}
					m_nCurPrecinctY = 0;
				}
				m_nCurResolution = (pPO ? pPO->m_nRSpoc : 0);
			} else {
				for(; m_nCurResolution < nResolutions; m_nCurResolution++) {
					for(; m_nCurTY < nMainTPY1; m_nCurTY += (m_nCurTY == nMainTPY0 ? (m_nIncTY - (m_nCurTY % m_nIncTY)) : m_nIncTY)) {
						for(; m_nCurTX < nMainTPX1; m_nCurTX += (m_nCurTX == nMainTPX0 ? (m_nIncTX - (m_nCurTX % m_nIncTX)) : m_nIncTX)) {
							for(; m_nCurComponent < nComponents; m_nCurComponent++) {
								CNCSJPCComponent *pComponent = pMainTP->m_Components[m_nCurComponent];
								if(m_nCurResolution < pComponent->m_CodingStyle.m_SPcod.m_nLevels + 1) {
									CNCSJPCResolution *pResolution = pComponent->m_Resolutions[m_nCurResolution];
									
									if(pResolution->GetNumPrecinctsWide() && pResolution->GetNumPrecinctsHigh()) {
//										INT32 nLevels = pComponent->m_CodingStyle.m_SPcod.m_nLevels;
										INT32 nDivY = pResolution->GetDivY();
											if(m_nCurTY % nDivY == 0 || ((m_nCurTY == nMainTPY0) && pResolution->YOriginCheckOk())) {
											//((pResolution->GetY0() * pResolution->GetResMultiplier()) % (pResolution->GetPrecinctHeight() * pResolution->GetResMultiplier()) != 0))) {
											INT32 nDivX = pResolution->GetDivX();

											if(m_nCurTX % nDivX == 0 || ((m_nCurTX == nMainTPX0) && pResolution->XOriginCheckOk())) {
												//((pResolution->GetX0() * pResolution->GetResMultiplier()) % (pResolution->GetPrecinctWidth() * pResolution->GetResMultiplier()) != 0))) {

												m_nCurPrecinctX = CalculatePrecinctX(pMainTP, pComponent, pResolution);
												m_nCurPrecinctY = CalculatePrecinctY(pMainTP, pComponent, pResolution);
											
												for(; m_nCurLayer < nLayers; m_nCurLayer++) {
													if(!m_bDone) {
														if(Step(pComponent,
																pResolution,
																pResolution->m_Precincts.find(m_nCurPrecinctX, m_nCurPrecinctY)) == false) {
															return(m_Error);
														}
													} else {
														return(m_Error);
													}
												}
												m_nCurLayer = 0;
											}
										}
									}
								}
							}
							m_nCurComponent = (pPO ? pPO->m_nCSpoc : 0);
						}
						m_nCurTX = nMainTPX0;
					}
					m_nCurTY = nMainTPY0;
				}
				m_nCurResolution = (pPO ? pPO->m_nRSpoc : 0);
			}
				
			break;

		case CNCSJPCProgressionOrderType::PCRL:
				for(; m_nCurTY < nMainTPY1; m_nCurTY += (m_nCurTY == nMainTPY0 ? (m_nIncTY - (m_nCurTY % m_nIncTY)) : m_nIncTY)) {
					for(; m_nCurTX < nMainTPX1; m_nCurTX += (m_nCurTX == nMainTPX0 ? (m_nIncTX - (m_nCurTX % m_nIncTX)) : m_nIncTX)) {
						for(; m_nCurComponent < nComponents; m_nCurComponent++) {
							CNCSJPCComponent *pComponent = pMainTP->m_Components[m_nCurComponent];
//							INT32 nLevels = pComponent->m_CodingStyle.m_SPcod.m_nLevels;
		
							for(; m_nCurResolution < nResolutions; m_nCurResolution++) {
								if(m_nCurResolution < pComponent->m_CodingStyle.m_SPcod.m_nLevels + 1) {
									CNCSJPCResolution *pResolution = pComponent->m_Resolutions[m_nCurResolution];
									
									if(pResolution->GetNumPrecinctsWide() && pResolution->GetNumPrecinctsHigh()) {
										INT32 nDivYMask = pResolution->GetDivY() - 1;
										if((m_nCurTY & nDivYMask) == 0 || ((m_nCurTY == nMainTPY0) && pResolution->YOriginCheckOk())) {										
											INT32 nDivXMask = pResolution->GetDivX() - 1;
											if((m_nCurTX & nDivXMask) == 0 || ((m_nCurTX == nMainTPX0) && pResolution->XOriginCheckOk())) {
												m_nCurPrecinctX = CalculatePrecinctX(pMainTP, pComponent, pResolution);
												m_nCurPrecinctY = CalculatePrecinctY(pMainTP, pComponent, pResolution);
											
												for(; m_nCurLayer < nLayers; m_nCurLayer++) {
													if(!m_bDone) {
														if(Step(pComponent,
																pResolution,
																pResolution->m_Precincts.find(m_nCurPrecinctX, m_nCurPrecinctY)) == false) {
															return(m_Error);
														}
													} else {
														return(m_Error);
													}
												}
												m_nCurLayer = 0;
											}
										}
									}
								}
							}
							m_nCurResolution = (pPO ? pPO->m_nRSpoc : 0);
						}
						m_nCurComponent = (pPO ? pPO->m_nCSpoc : 0);
					}
					m_nCurTX = nMainTPX0;
				}
				m_nCurTY = nMainTPY0;
			break;

		case CNCSJPCProgressionOrderType::CPRL:
				for(; m_nCurComponent < nComponents; m_nCurComponent++) {
					CNCSJPCComponent *pComponent = pMainTP->m_Components[m_nCurComponent];
//					INT32 nLevels = pComponent->m_CodingStyle.m_SPcod.m_nLevels;

					for(; m_nCurTY < nMainTPY1; m_nCurTY += (m_nCurTY == nMainTPY0 ? (m_nIncTY - (m_nCurTY % m_nIncTY)) : m_nIncTY)) {
						for(; m_nCurTX < nMainTPX1; m_nCurTX += (m_nCurTX == nMainTPX0 ? (m_nIncTX - (m_nCurTX % m_nIncTX)) : m_nIncTX)) {
							for(; m_nCurResolution < nResolutions; m_nCurResolution++) {
								if(m_nCurResolution < pComponent->m_CodingStyle.m_SPcod.m_nLevels + 1) {
									CNCSJPCResolution *pResolution = pComponent->m_Resolutions[m_nCurResolution];
									
									if(pResolution->GetNumPrecinctsWide() && pResolution->GetNumPrecinctsHigh()) {
										INT32 nDivY = pResolution->GetDivY();

										if(m_nCurTY % nDivY == 0 || ((m_nCurTY == nMainTPY0) && pResolution->YOriginCheckOk())) {
											INT32 nDivX = pResolution->GetDivX();

											if(m_nCurTX % nDivX == 0 || ((m_nCurTX == nMainTPX0) && pResolution->XOriginCheckOk())) {
												m_nCurPrecinctX = CalculatePrecinctX(pMainTP, pComponent, pResolution);
												m_nCurPrecinctY = CalculatePrecinctY(pMainTP, pComponent, pResolution);
											
												for(; m_nCurLayer < nLayers; m_nCurLayer++) {
													if(!m_bDone) {
														if(Step(pComponent,
																pResolution,
																pResolution->m_Precincts.find(m_nCurPrecinctX, m_nCurPrecinctY)) == false) {
															return(m_Error);
														}
													} else {
														return(m_Error);
													}
												}
												m_nCurLayer = 0;
											}
										}
									}
								}
							}
							m_nCurResolution = (pPO ? pPO->m_nRSpoc : 0);
						}
						m_nCurTX = nMainTPX0;
					}
					m_nCurTY = nMainTPY0;
				}
				m_nCurComponent = (pPO ? pPO->m_nCSpoc : 0);
			break;
	}
	if(!pPO) {
		bComplete = true;	
	} else {
		if(pPO->m_nCEpoc >= pMainTP->m_pJPC->m_SIZ.m_nCsiz &&
		   pPO->m_nREpoc >= nResolutionsMax &&
		   pPO->m_nLYEpoc >= nLayersMax) {
			bComplete = true;
		}
	}
	if(bComplete) {
		m_nCurPacket = pMainTP->GetFirstPacketNr();
	}
	IncrementPO(pMainTP, bComplete);
	return(m_Error);
}

#ifdef NOTUSED
// Calculate Precinct 'k' from the current progression (eq. B.20 in JP2 spec)
UINT32 CNCSJPCProgression::CalculatePrecinct(CNCSJPCTilePartHeader *pMainTP,
											 CNCSJPCComponent *pComponent,
											 CNCSJPCResolution *pResolution)
{
	INT32 nXD = pResolution->GetPrecinctWidth();
	INT32 nYD = pResolution->GetPrecinctHeight();

	return(NCSFloorDiv(NCSCeilDiv(m_nCurTX, pResolution->GetXRsizResMultiplier()), nXD) - NCSFloorDiv(pResolution->GetX0(), nXD) + 
			pResolution->GetNumPrecinctsWide() * 
			(NCSFloorDiv(NCSCeilDiv(m_nCurTY, pResolution->GetYRsizResMultiplier()), nYD) - NCSFloorDiv(pResolution->GetY0(), nYD)));
}
#endif

UINT32 CNCSJPCProgression::CalculatePrecinctX(CNCSJPCTilePartHeader *pMainTP,
											 CNCSJPCComponent *pComponent,
											 CNCSJPCResolution *pResolution)
{
	pMainTP;pComponent;//Keep compiler happy
	INT32 nXD = pResolution->GetPrecinctWidth();

	return(NCSFloorDiv(NCSCeilDiv(m_nCurTX, pResolution->GetXRsizResMultiplier()), nXD) - NCSFloorDiv(pResolution->GetX0(), nXD));
}

UINT32 CNCSJPCProgression::CalculatePrecinctY(CNCSJPCTilePartHeader *pMainTP,
											 CNCSJPCComponent *pComponent,
											 CNCSJPCResolution *pResolution)
{
	pMainTP;pComponent;//Keep compiler happy
	INT32 nYD = pResolution->GetPrecinctHeight();

	return(NCSFloorDiv(NCSCeilDiv(m_nCurTY, pResolution->GetYRsizResMultiplier()), nYD) - NCSFloorDiv(pResolution->GetY0(), nYD));
}

bool CNCSJPCProgression::Step(CNCSJPCComponent *pComponent,
							  CNCSJPCResolution *pResolution,
							  CNCSJPCPrecinct *pPrecinct)
{
	pComponent;pResolution;pPrecinct;//Keep compiler happy
	
	m_nCurPacket++;
	m_bDone = true;
	return(true);
}

bool CNCSJPCProgression::StepRow(CNCSJPCComponent *pComponent,
								 CNCSJPCResolution *pResolution)
{
	pComponent;pResolution;//Keep compiler happy
	return(false);
}
