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
 ** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2File.cpp $
 ** CREATED:  28/11/2002 3:27:34 PM
 ** AUTHOR:   Simon Cope
 ** PURPOSE:  NCSJP2File class implementation
 ** EDITS:    [xx] ddMmmyy NAME COMMENTS
 **		     [01] 07Sep04 tfl Added six-value world file support
 **			 [02] 17Dec04 tfl Added static function CNCSJP2File::Shutdown()
 **			 [03] 22Dec05 tfl Filename manipulations now use CNCSString
 **			 [04] 19Jan06 tfl Now writes EOC marker correctly
 *******************************************************/

#include "NCSJP2File.h"
#include "NCSECWCompressClient.h"
#include "NCSJPCEcwpIOStream.h"
#include "NCSJPCCodeBlock.h"
#include "NCSJPCBuffer.h"
#include "NCSJPCProgressionOrderType.h"
#include "NCSWorldFile.h"
#include "NCSEcw.h"
#include "../../NCSGDT2/NCSGDTEpsg.h"

#ifdef NCSJPC_USE_LCMS
#include "lcms.h"
#endif //NCSJPC_USE_LCMS

#define JP2_GEODATA_WLD_MASK	4
#define	JP2_GEODATA_GML_MASK	2
#define JP2_GEODATA_PCS_MASK	1

#define DEFAULT_JP2_GEODATA_PRECISION_EPSILON	0.00000001F

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Initialise static
CNCSJP2File::CNCSJP2FileVector CNCSJP2File::sm_Files;
UINT32 CNCSJP2File::sm_nKeySize = ECW_COMPRESS_SDK_LIMIT_MASK;
UINT32 CNCSJP2File::sm_nGeodataUsage = JP2_GEODATA_USE_GML_PCS;

CNCSJPCGlobalLock::CNCSJPCGlobalLock() 
{
	m_bAutoLock = true;
	Lock();
}

CNCSJPCGlobalLock::CNCSJPCGlobalLock(bool bAutoLock) 
{
	m_bAutoLock = bAutoLock;
	if(bAutoLock) {
		Lock();
	}
}

CNCSJPCGlobalLock::~CNCSJPCGlobalLock()
{
	if(m_bAutoLock) {
		UnLock();
	}
}

void CNCSJPCGlobalLock::Lock(void)
{
	CNCSJP2File::sm_Files.Lock();
}

bool CNCSJPCGlobalLock::TryLock(void)
{
	return(CNCSJP2File::sm_Files.TryLock());
}

void CNCSJPCGlobalLock::UnLock(void)
{
	CNCSJP2File::sm_Files.UnLock();
}

// Constructor
CNCSJP2File::CNCSJP2File()
{
	CNCSJPCGlobalLock _Lock;

	// Setup what top-level boxes are in the JP2 file.
	SetBoxes(&m_Signature, 
           &m_FileType, 
           &m_Header, 
           &m_Codestream, 
           &m_IPR,
           NULL);

	// Setup the required order of the boxes within the file.
	m_Signature.SetPrevBoxes(NULL);
	m_Signature.SetNextBoxes(&m_FileType, NULL);
	
	m_FileType.SetPrevBoxes(&m_Signature, NULL);
	m_FileType.SetNextBoxes(&m_Header, NULL);
	
	m_Header.SetPrevBoxes(&m_FileType, NULL);
	m_Header.SetNextBoxes(&m_Codestream, NULL);
	
	m_Codestream.SetPrevBoxes(&m_Header, NULL);
	m_Codestream.SetNextBoxes(NULL);

	m_IPR.SetPrevBoxes(&m_FileType, NULL);
	m_IPR.SetNextBoxes(NULL);

  /** Initialise m_FileInfo */
	NCSInitFileInfoEx(&m_FileInfo);
	
	m_nRefs = 0;
	m_pStream = NULL;
	m_bOurStream = false;

	sm_Files.push_back(this);

	m_eCompressionProfile = CNCSJPC::BASELINE_0;
	m_nCompressionLevels = 0;
	m_nCompressionLayers = 0;
	m_nCompressionPrecinctWidth = 0;
	m_nCompressionPrecinctHeight = 0;
	m_nCompressionTileWidth = 0;
	m_nCompressionTileHeight = 0;
	m_bCompressionIncludeSOP = false;
	m_bCompressionIncludeEPH = true;
	m_bCodestreamOnly = false;

	m_eCompressionOrder = CNCSJPCProgressionOrderType::RPCL;
	m_nDecompressionLayers = CNCSJPCBuffer::NCS_MAXUINT16;

	m_nNextLine = 0;
	//Default geodata usage - was JP2_GEODATA_USE_WLD_GML_PCS
}

// Destructor
CNCSJP2File::~CNCSJP2File()
{
	CNCSJPCGlobalLock _Lock;
	m_nRefs = 0; // Force free

	Close(true, true);

	std::vector<CNCSJP2File *>::iterator pCur = sm_Files.begin();
	std::vector<CNCSJP2File *>::iterator pEnd = sm_Files.end();

	while(pCur != pEnd) {
		if(*pCur == this) {
			sm_Files.erase(pCur);
			break;
		}
		pCur++;
	}
}

#ifdef NCS_BUILD_UNICODE
// Open the file, from the specified full path name.
CNCSError CNCSJP2File::Open(wchar_t *pName, bool bWrite)
{
	CNCSError Error;
	CNCSJPCGlobalLock _Lock;

#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
	printf("Opening file %s\n",CHAR_STRING(pName));
#endif

	if(m_nRefs == 0) {
		// Open the stream.
		int		nProtocolLength, nHostLength, nFilenameLength;
		char *pProtocol = (char *)NULL;
		char *pHost = (char *)NULL;
		char *pFilename = (char *)NULL;
		bool bIsECWP = false;
		if(NCSecwNetBreakdownUrl(CHAR_STRING(pName), &pProtocol, &nProtocolLength,
                             &pHost, &nHostLength,
                             &pFilename, &nFilenameLength)) {
			if(pProtocol && (!strnicmp(pProtocol, "ecwp:", strlen("ecwp:")) || !strnicmp(pProtocol, "ecwps:", strlen("ecwps:")))) {
				bIsECWP = true;
			}
		}
		if(bIsECWP) {
			m_pStream = new CNCSJPCEcwpIOStream(&m_Codestream);
			Error = ((CNCSJPCEcwpIOStream*)m_pStream)->Open(CHAR_STRING(pName));
		} else {
			m_pStream = new CNCSJPCFileIOStream();
			Error = m_pStream->Open(pName, bWrite);
		}
		
		if(Error == NCS_SUCCESS) {
			Error = Open(*m_pStream);
		} else {
			Error = *m_pStream;
		}
		m_bOurStream = true;
	}
	return(Error);
}
#endif

// Open the file, from the specified full path name.
CNCSError CNCSJP2File::Open(char *pName, bool bWrite)
{
	CNCSError Error;
	CNCSJPCGlobalLock _Lock;

	if(m_nRefs == 0) {
    // Open the stream.
		int		nProtocolLength, nHostLength, nFilenameLength;
		char *pProtocol = (char *)NULL;
		char *pHost = (char *)NULL;
		char *pFilename = (char *)NULL;
		bool bIsECWP = false;
		if(NCSecwNetBreakdownUrl(pName, &pProtocol, &nProtocolLength,
                             &pHost, &nHostLength,
                             &pFilename, &nFilenameLength)) {
			if(pProtocol && (!strnicmp(pProtocol, "ecwp:", strlen("ecwp:")) || !strnicmp(pProtocol, "ecwps:", strlen("ecwps:")))) {
				bIsECWP = true;
			}
		}
		if(bIsECWP) {
			m_pStream = new CNCSJPCEcwpIOStream(&m_Codestream);
			Error = ((CNCSJPCEcwpIOStream*)m_pStream)->Open(pName);
		} else {
			m_pStream = new CNCSJPCFileIOStream();
			Error = m_pStream->Open(pName, bWrite);
		}
		if(Error == NCS_SUCCESS) {
			Error = Open(*m_pStream);
		} else {
			Error = *m_pStream;
		}
		m_bOurStream = true;
	}
	return(Error);
}

// Open the file, from the specified Stream.
CNCSError CNCSJP2File::Open(CNCSJPCIOStream &Stream)
{
	CNCSError Error;
	CNCSJPCGlobalLock _Lock;

	m_pStream = &Stream;
	m_bOurStream = false;
	if(!Stream.IsWrite()) {
		// How large is the JP2File box?
		m_nXLBox = Stream.Size();
		m_nLDBox = m_nXLBox;

		// Parse in the "info" boxes
		Error = Parse(*this, Stream);
    if(m_FileType.m_bValid == false) {
			// check if it's a jpc file...
			Stream.Seek(0, CNCSJPCIOStream::START);
			Error = m_Codestream.Parse(*this, Stream);
		}
		if(Error == NCS_SUCCESS) {
			if(((CNCSJPCMainHeader*)&m_Codestream)->m_bValid == false) {
				Error = NCS_FILE_INVALID;
			}
			if(Error == NCS_SUCCESS) {
        /** Initialise m_FileInfo */
				NCSFreeFileInfoEx(&m_FileInfo);
				m_FileInfo.nSizeX = m_Codestream.m_SIZ.m_nXsiz - m_Codestream.m_SIZ.m_nXOsiz;
				m_FileInfo.nSizeY = m_Codestream.m_SIZ.m_nYsiz - m_Codestream.m_SIZ.m_nYOsiz;
				bool bSigned = false;

				UINT8 nMaxBits = 0;
				UINT32 c;

				if(m_Header.m_Palette.m_bValid && m_Header.m_ComponentMapping.m_bValid) {
					m_FileInfo.nBands = m_Header.m_ComponentMapping.m_nEntries;
				} else {
					m_FileInfo.nBands = m_Codestream.m_SIZ.m_nCsiz;
				}
				m_FileInfo.pBands = (NCSFileBandInfo*)NCSMalloc(sizeof(NCSFileBandInfo) * m_FileInfo.nBands, TRUE);
			
				m_ChannelComponentIndex.resize(m_FileInfo.nBands);
        // Defualt channel->component mapping
				for(c = 0; c < m_FileInfo.nBands; c++) {
					m_ChannelComponentIndex[c] = NCSMin(c, (UINT16)(m_Codestream.m_SIZ.m_nCsiz - 1));
				}
				if(m_Header.m_Palette.m_bValid && m_Header.m_ComponentMapping.m_bValid) {
          // Palette component mapping
					for(UINT32 c = 0; c < m_Header.m_ComponentMapping.m_Mapping.size(); c++) {
						m_ChannelComponentIndex[c] = m_Header.m_ComponentMapping.m_Mapping[c].m_iComponent;
					}
					for(int b = 0; b < m_FileInfo.nBands; b++) {
						m_FileInfo.pBands[b].nBits = m_Header.m_Palette.m_Bits[m_Header.m_ComponentMapping.m_Mapping[b].m_iPalette].m_nBits;
						nMaxBits = NCSMax(nMaxBits, m_FileInfo.pBands[b].nBits);
						m_FileInfo.pBands[b].bSigned = m_Header.m_Palette.m_Bits[m_Header.m_ComponentMapping.m_Mapping[b].m_iPalette].m_bSigned;
						if(m_FileInfo.pBands[b].bSigned) {
							bSigned = true;
						}
					}
				} else {
					for(int b = 0; b < m_FileInfo.nBands; b++) {
						m_FileInfo.pBands[b].nBits = m_Codestream.m_SIZ.m_Components[b].m_nBits;
						nMaxBits = NCSMax(nMaxBits, m_FileInfo.pBands[b].nBits);
						m_FileInfo.pBands[b].bSigned = m_Codestream.m_SIZ.m_Components[b].m_bSigned;
						if(m_FileInfo.pBands[b].bSigned) {
							bSigned = true;
						}
					}
				}
				CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox *pCSBox = NULL;
						
				while(NULL != (pCSBox = (CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox*)GetBox(CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::sm_nTBox, pCSBox))) {
					if(pCSBox->m_bValid && pCSBox->m_eMethod == CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::ENUMERATED_COLORSPACE) {
						break;
					}
				}
				
				if (pCSBox) 
          {
            const CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::ChannelDefinition *pNullDef = NULL;
            for(c = 0; c < m_FileInfo.nBands; c++) {
              pNullDef = m_Header.m_ChannelDefinition.GetDefinitition(m_FileInfo.nBands - 1);
              if(pNullDef && pNullDef->m_eType == CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::OPACITY && 
                 pNullDef->m_eAssociation == CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::WHOLE_IMAGE) {
                break;
              }
              pNullDef = NULL;
            }
					
						// Have an enumerated colorspace
            switch(pCSBox->m_eEnumeratedColorspace) {
						case CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::sRGB:
						case CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::YCbCr:
							m_FileInfo.eColorSpace = NCSCS_sRGB;
							for(c = 0; c < m_FileInfo.nBands; c++) {
								const CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::ChannelDefinition *pDef = m_Header.m_ChannelDefinition.GetDefinitition(c);
								if(pDef) {
									switch(pDef->m_eType) {
                  case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::COLOR:
                    if(pCSBox->m_eEnumeratedColorspace == CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::sRGB) {
                      switch(pDef->m_eAssociation) {
                      case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::R:
                        m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_Red);
                        break;
                      case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::G:
                        m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_Green);
                        break;
                      case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::B:
                        m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_Blue);
                        break;
                      case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::WHOLE_IMAGE:
                        m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_All);
                        break;
                      default:
                        break;
                      }
                    } else {
                      switch(pDef->m_eAssociation) {
                      case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::Y:
                      case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::Cb:
                      case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::Cr:
                        switch(c) {
                        case 0:
                          m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_Red);
                          break;
                        case 1:
                          m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_Green);
                          break;
                        case 2:
                          m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_Blue);
                          break;
                        }
                        break;
                      case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::WHOLE_IMAGE:
                        m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_All);
                        break;
                      default:
                        break;
                      }
                    }
                    break;
                  case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::OPACITY:
                    switch(pDef->m_eAssociation) {
                    case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::R:
                      m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_RedOpacity);
                      break;
                    case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::G:
                      m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_GreenOpacity);
                      break;
                    case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::B:
                      m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_BlueOpacity);
                      break;
                    case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::WHOLE_IMAGE:
                      m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_AllOpacity);
                      break;
                    default:
                      break;
                    }
                    break;
                  case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::PREMUL_OPACITY:
                    switch(pDef->m_eAssociation) {
                    case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::R:
                      m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_RedOpacityPremultiplied);
                      break;
                    case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::G:
                      m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_GreenOpacityPremultiplied);
                      break;
                    case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::B:
                      m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_BlueOpacityPremultiplied);
                      break;
                    case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::WHOLE_IMAGE:
                      m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_AllOpacityPremultiplied);
                      break;
                    default:
                      break;
                    }
                    break;											
                  default: break;
									}
								} else {
									switch(c) {
                  case 0:
                    m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_Red);
                    break;
                  case 1:
                    m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_Green);
                    break;
                  case 2:
                    m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_Blue);
                    break;
                    //		case 3:
                    //				m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_AllOpacity);
                    //			break;
                  default:
                    break;
									}
								}
							}
							break;
						case CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::GREYSCALE:
							if (m_FileInfo.nBands == 1 || (m_FileInfo.nBands == 2 && pNullDef != NULL)) 
								m_FileInfo.eColorSpace = NCSCS_GREYSCALE;
							else
								m_FileInfo.eColorSpace = NCSCS_MULTIBAND;
							for(c = 0; c < m_FileInfo.nBands; c++) {
								const CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::ChannelDefinition *pDef = m_Header.m_ChannelDefinition.GetDefinitition(c);
								if(pDef) {
									switch(pDef->m_eType) {
                  case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::COLOR:
                    switch(pDef->m_eAssociation) {
                    case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::Y:
                      if(c == 0 && (m_FileInfo.nBands == 1 || (m_FileInfo.nBands == 2 && pNullDef))) {
                        m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_Greyscale);
                      } else if(m_FileInfo.nBands > 1) {
                        char buf[64];
                        sprintf(buf,NCS_BANDDESC_Band,c+1);
                        m_FileInfo.pBands[c].szDesc = NCSStrDup(buf);
                      }
                      break;
                    case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::WHOLE_IMAGE:
                      m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_All);
                      break;
                    default:
                      break;
                    }
                    break;
                  case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::OPACITY:
                    switch(pDef->m_eAssociation) {
                    case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::Y:
                      m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_GreyscaleOpacity);
                      break;
                    case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::WHOLE_IMAGE:
                      m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_AllOpacity);
                      break;
                    default:
                      break;
                    }
                    break;
                  case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::PREMUL_OPACITY:
                    switch(pDef->m_eAssociation) {
                    case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::Y:
                      m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_GreyscaleOpacityPremultiplied);
                      break;															
                    case CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::WHOLE_IMAGE:
                      m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_AllOpacityPremultiplied);
                      break;
                    default:
                      break;
                    }
                    break;											
                  default: break;
									}
								} else if(c == 0 && (m_FileInfo.nBands == 1 || (m_FileInfo.nBands == 2 && pNullDef))) {
									m_FileInfo.pBands[c].szDesc = NCSStrDup(NCS_BANDDESC_Greyscale);
								} else if(m_FileInfo.nBands > 1) {
									char buf[64];
									sprintf(buf,NCS_BANDDESC_Band,c+1);
									m_FileInfo.pBands[c].szDesc = NCSStrDup(buf);
								}
							}
							break;
						default: break;
            }
          } else {
            if(m_Codestream.m_COD.m_SGcod.m_bMCT) {
              m_FileInfo.eColorSpace = NCSCS_sRGB;
            } else {
              m_FileInfo.eColorSpace = NCSCS_MULTIBAND;
            }
          }
				for(c = 0; c < m_FileInfo.nBands; c++) {
					if(!m_FileInfo.pBands[c].szDesc) {
						// Don't know the name
						char buf[32];
						sprintf(buf, NCS_BANDDESC_Band, c + 1);
						m_FileInfo.pBands[c].szDesc = NCSStrDup(buf);
					}
				}
				m_FileInfo.nCompressionRate = (UINT16)((m_FileInfo.nSizeX * m_FileInfo.nSizeY * m_FileInfo.nBands) / m_pStream->Size());
				
        /* Projection Info */
				m_FileInfo.eCellSizeUnits = ECW_CELL_UNITS_METERS;
				m_FileInfo.fCellIncrementX = 1.0;
				m_FileInfo.fCellIncrementY = 1.0;
				m_FileInfo.fOriginX = 0.0;
				m_FileInfo.fOriginY = 0.0;
				m_FileInfo.szDatum = NCSStrDup("RAW");
				m_FileInfo.szProjection = NCSStrDup("RAW");
				m_FileInfo.fCWRotationDegrees = 0.0;

				if(bSigned) {
					if(nMaxBits <= 8) {
						m_FileInfo.eCellType = NCSCT_INT8;
					} else if(nMaxBits <= 16) {
						m_FileInfo.eCellType = NCSCT_INT16;
					} else if(nMaxBits <= 32) {
					 m_FileInfo.eCellType = NCSCT_INT32;
					} else {
						m_FileInfo.eCellType = NCSCT_INT64;
					}
					// FIXME - float?
				} else {
					if(nMaxBits <= 8) {
						m_FileInfo.eCellType = NCSCT_UINT8;
					} else if(nMaxBits <= 16) {
						m_FileInfo.eCellType = NCSCT_UINT16;
					} else if(nMaxBits <= 32) {
						m_FileInfo.eCellType = NCSCT_UINT32;
					} else {
						m_FileInfo.eCellType = NCSCT_UINT64;
					}
				}
        //Look for georeferencing information connected to the image in this file.
        //Three sources: world files, GML XML boxes, and "GeoTIFF" UUID boxes.
        //The priority of the information defaults to world file, then GML box, then 
        //PCS box, however the priority can be set to any of the six possible orders
        //by the SDK user using the JP2_GEODATA_ORDER_ parameters.

        //If all else fails we default to obtaining some registration information from 
        //the resolution boxes.
				UINT32 nGeodataUsage = sm_nGeodataUsage;
				bool bGeodataFound = false;
				while (nGeodataUsage > 0)
          {
            if (nGeodataUsage & JP2_GEODATA_WLD_MASK)
              {
                //Look for a world file
                //Construct world file name
                //Attempt to read world file data
                CNCSWorldFile WorldFile(DEFAULT_JP2_GEODATA_PRECISION_EPSILON);
                char *szFilename = NCSStrDup(CHAR_STRING(Stream.GetName()));
                char *szWorldFilename = NULL;
                if (WorldFile.ConstructWorldFilename(szFilename,&szWorldFilename))
                  {
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
                    fprintf(stderr,"World file name: %s\r\n",szWorldFilename);
#endif
                    if (WorldFile.Read(szWorldFilename))
                      {
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
                        fprintf(stderr,"Processing information from world file\r\n");
#endif
                        WorldFile.PopulateFileInfoEx(m_FileInfo, TRUE);
                        bGeodataFound = true;
                      }
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
                    else fprintf(stderr,"Unable to process information from world file\r\n");
#endif
                    NCSFree(szFilename);
                    NCSFree(szWorldFilename);
					
                  }
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
                else fprintf(stderr,"Unable to construct output path for world file\r\n");
#endif
              }
            if (nGeodataUsage & JP2_GEODATA_GML_MASK)
              {
                //Look for a GML XML box
                CNCSJP2XMLBox *pXMLBox = NULL;
                while (NULL != (pXMLBox = GetXMLBox(pXMLBox))) 
                  {
                    *(CNCSJP2XMLBox*)&m_GML = *pXMLBox;
                    if (Stream.Seek(m_GML.m_nDBoxOffset, CNCSJPCIOStream::START) &&
                        m_GML.Parse(*this, Stream) == NCS_SUCCESS) 
                      {
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
                        fprintf(stderr,"Processing GML info\n");
#endif 
                        m_GML.GetFileInfo(m_FileInfo);
                        bGeodataFound = true;
                        break;
                      }
                    else {
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
                      fprintf(stderr,"XML box found wasn't a valid GML box\n");
#endif
                      m_GML.m_bValid = false;
                    }
                  }
              }
            if (nGeodataUsage & JP2_GEODATA_PCS_MASK)
              {
                //Look for a PCS/EPSG UUID "GeoTIFF" box
                CNCSJP2UUIDBox *pUUIDBox = NULL;
                pUUIDBox = GetUUIDBox(CNCSJP2PCSBox::sm_UUID, pUUIDBox);
                if (pUUIDBox) 
                  {
                    *(CNCSJP2UUIDBox*)&m_PCS = *pUUIDBox;
                    if(Stream.Seek(m_PCS.m_nDBoxOffset, CNCSJPCIOStream::START) &&
                       m_PCS.Parse(*this, Stream) == NCS_SUCCESS) 
                      {
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
                        fprintf(stderr,"Processing PCS/EPSG UUID \"GeoTIFF\" info\n");
#endif
                        m_PCS.GetFileInfo(m_FileInfo);
                        bGeodataFound = true;
                      } 
                    else 
                      {
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
                        fprintf(stderr,"Couldn't parse PCS/EPSG UUID \"GeoTIFF\" box\n");
#endif
                        m_PCS.m_bValid = false;
                      }
                  }

                //"World chunk" box no longer supported, implementation remains, however.
                /*						pUUIDBox = GetUUIDBox(CNCSJP2WorldBox::sm_UUID, pUUIDBox);
                              if (pUUIDBox) 
                              {
                              *(CNCSJP2UUIDBox*)&m_World = *pUUIDBox;
                              if(Stream.Seek(m_World.m_nDBoxOffset, CNCSJPCIOStream::START) &&
                              m_World.Parse(*this, Stream) == NCS_SUCCESS) 
                              {
                              #ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
                              fprintf(stderr,"Processing \"World Chunk\" UUID info\r\n");
                              #endif
                              //m_World.GetFileInfo(m_FileInfo);
                              NCSFileViewFileInfoEx FileInfo;
                              //CNCSAffineTransform Affine;
                              m_World.GetFileInfo(FileInfo);
                              Affine.GetFromFileInfoEx(FileInfo);
                              fprintf(stdout,"Read the following info:\r\n"
                              "fScaleX: %f\r\n"
                              "fRotY: %f\r\n"
                              "fRotX: %f\r\n"
                              "fScaleY: %f\r\n"
                              "fTransX: %f\r\n"
                              "fTransY: %f\r\n",
                              Affine.fScaleX,
                              Affine.fRotY,
                              Affine.fRotX,
                              Affine.fScaleY,
                              Affine.fTransX,
                              Affine.fTransY);
                              "fOriginX: %f\r\n"
                              "fOriginY: %f\r\n"
                              "fCellIncrementX: %f\r\n"
                              "fCellIncrementY: %f\r\n"
                              "fCWRotationDegrees: %f\r\n",
                              FileInfo.fOriginX,
                              FileInfo.fOriginY,
                              FileInfo.fCellIncrementX,
                              FileInfo.fCellIncrementY,
                              FileInfo.fCWRotationDegrees);
                              fflush(stdout);



                              bGeodataFound = true;
                              } 
                              else 
                              {
                              #ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
                              fprintf(stderr,"Couldn't parse \"World Chunk\" UUID box\n");
                              #endif
                              m_World.m_bValid = false;
                              }
                              }*/

              }

            if (bGeodataFound) break;
            nGeodataUsage = nGeodataUsage >> 3; 
          }
				if (!bGeodataFound)
          {
            //Resort to checking the resolution boxes
            if(m_Header.m_Resolution.m_Capture.m_bValid) 
              {
                CNCSJP2HeaderBox::CNCSJP2ResolutionBox::CNCSJP2CaptureResolutionBox &Capture = m_Header.m_Resolution.m_Capture;
                // No proj info, but we have a capture resolution
                m_FileInfo.fCellIncrementX = Capture.m_nHRcN / (double)Capture.m_nHRcD * ::pow(10.0f, Capture.m_nHRcE); 
                m_FileInfo.fCellIncrementY = Capture.m_nVRcN / (double)Capture.m_nVRcD * ::pow(10.0f, Capture.m_nVRcE); 
              } 
            else if(m_Header.m_Resolution.m_DefaultDisplay.m_bValid) 
              {
                CNCSJP2HeaderBox::CNCSJP2ResolutionBox::CNCSJP2DefaultDisplayResolutionBox &Display = m_Header.m_Resolution.m_DefaultDisplay;
                // No proj info, but we have a capture resolution
                m_FileInfo.fCellIncrementX = Display.m_nHRcN / (double)Display.m_nHRcD * ::pow(10.0f, Display.m_nHRcE); 
                m_FileInfo.fCellIncrementY = Display.m_nVRcN / (double)Display.m_nVRcD * ::pow(10.0f, Display.m_nVRcE); 
              }
          }
			}
		}
	} else {
		m_nNextLine = 0;
	}
	return(Error);
}

// Close the file
CNCSError CNCSJP2File::Close(bool bFreeCache, bool bFromDestructor)
{
	CNCSError Error;
	CNCSJPCGlobalLock _Lock;	

	if(m_nRefs) {
		m_nRefs--;
	}
	if(m_nRefs == 0 && bFreeCache) {
		if(m_pStream) {
			Error = m_pStream->Close();
			if(m_bOurStream) {
				delete m_pStream;
			}
			m_pStream = NULL;
		}
		NCSFreeFileInfoEx(&m_FileInfo);
		for(UINT32 i = 0; i < m_Codestream.m_Tiles.size(); i++) {
			if( m_Codestream.m_Tiles[i] ) delete m_Codestream.m_Tiles[i];
			m_Codestream.m_Tiles[i] = NULL;
		}
		m_Codestream.m_Tiles.clear();
		m_Codestream.CloseEncoderFiles(true);
		if(m_Codestream.m_pTmpDir) {
			NCSRemoveDir(m_Codestream.m_pTmpDir);
			NCSFree(m_Codestream.m_pTmpDir);
			m_Codestream.m_pTmpDir = NULL;
		}
	}
	return(Error);
}

CNCSError CNCSJP2File::Close(bool bFreeCache)
{
	return(Close(bFreeCache, false));
}

#ifdef NCS_BUILD_UNICODE
CNCSError CNCSJP2File::sOpen(CNCSJP2File **ppFile, wchar_t *pURLPath)
#else
  CNCSError CNCSJP2File::sOpen(CNCSJP2File **ppFile, char *pURLPath)
#endif
{
	CNCSError Error;
	CNCSJPCGlobalLock _Lock;
	
	for(UINT32 i = 0; i < sm_Files.size(); i++) {
		if(sm_Files[i]->m_pStream && sm_Files[i]->m_pStream->GetName() && !NCSTCmp(sm_Files[i]->m_pStream->GetName(), pURLPath) && (sm_Files[i]->m_pStream->IsWrite() == false)) {
			*ppFile = sm_Files[i];
			(*ppFile)->m_nRefs++;
			Error = NCS_SUCCESS;
			break;
		}
	}
	if(!*ppFile) {
		*ppFile = new CNCSJP2File();
		if(*ppFile) {
			Error = (*ppFile)->Open(pURLPath, false);
			if(Error != NCS_SUCCESS) {
				delete *ppFile;
				*ppFile = NULL;
			} else {
				(*ppFile)->m_nRefs++;
			}
		} else {
			Error = NCS_COULDNT_ALLOC_MEMORY;
		}
	}
	return(Error);
}

CNCSError CNCSJP2File::sOpen(CNCSJP2File **ppFile, CNCSJPCIOStream *pStream)
{
	CNCSError Error;
	CNCSJPCGlobalLock _Lock;
	
	for(UINT32 i = 0; i < sm_Files.size(); i++) {
		if(sm_Files[i]->m_pStream && sm_Files[i]->m_pStream->GetName() && !NCSTCmp(sm_Files[i]->m_pStream->GetName(), pStream->GetName()) && (sm_Files[i]->m_pStream->IsWrite() == false)) {
			*ppFile = sm_Files[i];
			(*ppFile)->m_nRefs++;
			Error = NCS_SUCCESS;
			break;
		}
	}
	if(!*ppFile) {
		*ppFile = new CNCSJP2File();
		if(*ppFile) {
			Error = (*ppFile)->Open(*pStream);
			if(Error != NCS_SUCCESS) {
				delete *ppFile;
				*ppFile = NULL;
			} else {
				(*ppFile)->m_nRefs++;
			}
		} else {
			Error = NCS_COULDNT_ALLOC_MEMORY;
		}
	}
	return(Error);
}

// Get X0 of this node.
INT32 CNCSJP2File::GetX0()
{
	if(!m_X0.Cached()) {
		m_X0 = m_Codestream.GetComponentX0(0);
	}
	return(m_X0);
}

// Get Y0 of this node.
INT32 CNCSJP2File::GetY0()
{
	if(!m_Y0.Cached()) {
		m_Y0 = m_Codestream.GetComponentY0(0);
	}
	return(m_Y0);
}

// Get X1 of this node.
INT32 CNCSJP2File::GetX1()
{
	if(!m_X1.Cached()) {
		m_X1 = m_Codestream.GetComponentX1(0);
	}
	return(m_X1);
}

// Get Y1 of this node.
INT32 CNCSJP2File::GetY1()
{
	if(!m_Y1.Cached()) {
		m_Y1 = m_Codestream.GetComponentY1(0);
	}
	return(m_Y1);
}

UINT32 CNCSJP2File::GetPacketLength(NCSBlockId nPacket)
{
	CNCSJPCGlobalLock _Lock;	
	return(m_Codestream.GetPacketLength((UINT32)nPacket));
}

void *CNCSJP2File::GetPacket(NCSBlockId nPacket, UINT32 &nLength)
{
	CNCSJPCGlobalLock _Lock;
	return(m_Codestream.GetPacket((UINT32)nPacket, &nLength));
}

void *CNCSJP2File::GetMemImage_AddBox(void *pImage, UINT32 *pLength, CNCSJP2Box *pBox)
{
	if(pBox->m_bHaveBox && pBox->m_bValid) {
		UINT64 nLength = pBox->m_nXLBox;
		if(nLength == 0) {
			// 0 == rest of stream
			nLength = m_Codestream.m_pStream->Size() - pBox->m_nBoxOffset;
		}
		pImage = NCSRealloc(pImage, (UINT32)(*pLength + nLength), FALSE);
		if(pImage) {
			if(m_Codestream.m_pStream->Seek(pBox->m_nBoxOffset, CNCSJPCIOStream::START) && 
         m_Codestream.m_pStream->Read((UINT8*)pImage + *pLength, (UINT32)nLength)) {
				*pLength += (UINT32)nLength;
			} else {
				NCSFree(pImage);
				pImage = NULL;
			}
		}
	}
	return(pImage);
}

void *CNCSJP2File::GetMemImage_AddMarker(void *pImage, UINT32 *pLength, CNCSJPCMarker *pMarker)
{
	if(pMarker->m_bHaveMarker && pMarker->m_bValid) {
		UINT64 nLength = pMarker->m_nLength + sizeof(UINT16);
		pImage = NCSRealloc(pImage, (UINT32)(*pLength + nLength), FALSE);
		if(pImage) {
			if(m_Codestream.m_pStream->Seek(pMarker->m_nOffset, CNCSJPCIOStream::START) && 
         m_Codestream.m_pStream->Read((UINT8*)pImage + *pLength, (UINT32)nLength)) {
				*pLength += (UINT32)nLength;
			} else {
				NCSFree(pImage);
				pImage = NULL;
			}
		}
	}
	return(pImage);
}

void *CNCSJP2File::GetMemImage_AddCodestream(void *pImage, UINT32 *pLength)
{
	if(((CNCSJPCMainHeader)m_Codestream).m_bValid) {
		// Codestream is valid, may or may not be a JP2 file (could be a JPC etc)
		if(m_Codestream.m_bHaveBox && ((CNCSJP2Box)m_Codestream).m_bValid) {
			// If we have the JP2 Box, put the nTBox and length in the image
			UINT64 nLength = 2 * sizeof(UINT32);
			if(pImage) {
				pImage = NCSRealloc(pImage, (UINT32)(*pLength + nLength), FALSE);
			} else {
				pImage = NCSMalloc((UINT32)nLength, FALSE);
			}
			// Length == 0, rest of stream
			memset((UINT8*)pImage + *pLength, 0, sizeof(UINT32));
			*pLength += sizeof(UINT32);
			UINT32 nTBox = m_Codestream.m_nTBox;
#ifdef NCSBO_LSBFIRST
			nTBox = NCSByteSwap32(nTBox);
#endif
			memcpy((UINT8*)pImage + *pLength, &nTBox, sizeof(UINT32));
			*pLength += sizeof(UINT32);
		}
		if((pImage = GetMemImage_AddMarker(pImage, pLength, &m_Codestream.m_SOC)) != NULL &&
       (pImage = GetMemImage_AddMarker(pImage, pLength, &m_Codestream.m_SIZ)) != NULL &&
       (pImage = GetMemImage_AddMarker(pImage, pLength, &m_Codestream.m_COD)) != NULL &&
       (pImage = GetMemImage_AddMarker(pImage, pLength, &m_Codestream.m_QCD)) != NULL &&
       (pImage = GetMemImage_AddMarker(pImage, pLength, &m_Codestream.m_POC)) != NULL &&
       (pImage = GetMemImage_AddMarker(pImage, pLength, &m_Codestream.m_CRG)) != NULL) {
			UINT32 c;

			for(c = 0; c < m_Codestream.m_SIZ.m_nCsiz; c++) {
				CNCSJPCCOCMarker COC = m_Codestream.m_CodingStyles[c];COC.m_eMarker = CNCSJPCMarker::COC;
				CNCSJPCQCCMarker QCC = m_Codestream.m_QuantizationStyles[c];QCC.m_eMarker = CNCSJPCMarker::QCC;

				if(COC.m_nOffset != m_Codestream.m_COD.m_nOffset) {
					pImage = GetMemImage_AddMarker(pImage, pLength, &COC);
					if(!pImage) break;
				}
				if(QCC.m_nOffset != m_Codestream.m_QCD.m_nOffset) {
					pImage = GetMemImage_AddMarker(pImage, pLength, &QCC);
					if(!pImage) break;
				}
			}
			if(pImage) {
				UINT32 r;
				for(r = 0; r < m_Codestream.m_RGNs.size(); r++) {
					pImage = GetMemImage_AddMarker(pImage, pLength, &m_Codestream.m_RGNs[r]);
					if(!pImage) break;
				}
				CNCSJPCTilePartHeader *pTP = m_Codestream.GetTile(0);
				while(pTP && pImage) {
					UINT64 nSOTStart = *pLength;
					if((pImage = GetMemImage_AddMarker(pImage, pLength, &pTP->m_SOT)) != NULL &&
             (pImage = GetMemImage_AddMarker(pImage, pLength, &pTP->m_COD)) != NULL &&
             (pImage = GetMemImage_AddMarker(pImage, pLength, &pTP->m_QCD)) != NULL &&
             (pImage = GetMemImage_AddMarker(pImage, pLength, &pTP->m_POC)) != NULL) {
						for(c = 0; c < pTP->m_Components.size(); c++) {
							if(pTP->m_Components[c]->m_CodingStyle.m_nOffset != m_Codestream.m_CodingStyles[c].m_nOffset) {
								pImage = GetMemImage_AddMarker(pImage, pLength, &pTP->m_Components[c]->m_CodingStyle);
								if(!pImage) break;
							}
							if(pTP->m_Components[c]->m_QuantizationStyle.m_nOffset != m_Codestream.m_QuantizationStyles[c].m_nOffset) {
								pImage = GetMemImage_AddMarker(pImage, pLength, &pTP->m_Components[c]->m_QuantizationStyle);
								if(!pImage) break;
							}
						}
						if(pImage) {
							for(r = 0; r < m_Codestream.m_RGNs.size(); r++) {
								pImage = GetMemImage_AddMarker(pImage, pLength, &pTP->m_Regions[r]);
								if(!pImage) break;
							}
							if(pImage) {
								// Update nPsot, since we don't include the packets or the SOD marker
								UINT32 nPsot = (UINT32)(*pLength - nSOTStart);
#ifdef NCSBO_LSBFIRST
								nPsot = NCSByteSwap32(nPsot);
#endif
								memcpy((UINT8*)pImage + nSOTStart + sizeof(UINT16) * 3, &nPsot, sizeof(nPsot));
							}
						}

					}
					pTP = m_Codestream.GetTile(pTP->m_SOT.m_nIsot + 1);
				}
				pImage = GetMemImage_AddMarker(pImage, pLength, &m_Codestream.m_EOC);
			}
		}
	}
	return(pImage);
}

void *CNCSJP2File::GetMemImage(UINT32 &nLength)
{
	CNCSJPCGlobalLock _Lock;
	void *pImage = (void*)NULL;

	if((pImage = GetMemImage_AddBox(pImage, &nLength, &m_Signature)) != NULL &&
     (pImage = GetMemImage_AddBox(pImage, &nLength, &m_FileType)) != NULL &&
     (pImage = GetMemImage_AddBox(pImage, &nLength, &m_Header)) != NULL &&
     (pImage = GetMemImage_AddBox(pImage, &nLength, &m_IPR)) != NULL) {

		if( m_GML.m_bValid ) {
			pImage = GetMemImage_AddBox(pImage, &nLength, &m_GML);
		}
		if( m_PCS.m_bValid ) {
			pImage = GetMemImage_AddBox(pImage, &nLength, &m_PCS);
		}

		std::vector<CNCSJP2Box *>::iterator pCur = m_OtherBoxes.begin();
		std::vector<CNCSJP2Box *>::iterator pEnd = m_OtherBoxes.end();

		while(pCur != pEnd) {
			pImage = GetMemImage_AddBox(pImage, &nLength, (*pCur));
			if(!pImage) break;
			pCur++;
		}

		if(pImage) {
			// Codestream!
			pImage = GetMemImage_AddCodestream(pImage, &nLength);
		}
	}
	if(!pImage && (m_FileType.m_bValid == false) && ((CNCSJPCMainHeader)m_Codestream).m_bValid) {
		// JPC
		pImage = GetMemImage_AddCodestream(pImage, &nLength);
	}
	return(pImage);
}

// Get number of a packet in the file.
UINT32 CNCSJP2File::GetNrPackets()
{
	CNCSJPCGlobalLock _Lock;
	UINT32 nPackets = 0;
	CNCSJPCTilePartHeader *pTP = m_Codestream.GetTile(0);
	while(pTP) {
		nPackets += pTP->GetNrPackets();
		pTP = m_Codestream.GetTile(pTP->m_SOT.m_nIsot + 1);
	}
	return(nPackets);
}

CNCSJP2File::CNCSJP2UUIDBox *CNCSJP2File::GetUUIDBox(NCSUUID uuid, CNCSJP2UUIDBox *pLast)
{
	while(NULL != (pLast = (CNCSJP2UUIDBox*)GetBox(CNCSJP2UUIDBox::sm_nTBox, pLast))) {
		if(pLast->m_UUID == uuid) {
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
			fprintf(stderr,"Found UUID box\n");
#endif //NCS_BUILD_WITH_STDERR_DEBUG_INFO
			break;
		}
	}
	return(pLast);
}

CNCSJP2File::CNCSJP2XMLBox *CNCSJP2File::GetXMLBox(CNCSJP2XMLBox *pLast)
{
	return((CNCSJP2XMLBox*)GetBox(CNCSJP2XMLBox::sm_nTBox, pLast));
}

bool CNCSJP2File::OverBudget() 
{
	UINT64 nMaxMem = pNCSEcwInfo->pStatistics->nMaximumCacheSize;
  //	if(NCSecwGetConfig(NCSCFG_CACHE_MAXMEM, &nMaxMem) != NCS_SUCCESS) {
  //		nMaxMem = CNCSJPCBuffer::sm_SystemInfo.m_nPhysicalMemBytes / 4;
  //	}
	//nMaxMem = CNCSJPCBuffer::sm_SystemInfo.m_nPhysicalMemBytes / 40;
	UINT64 nUsing = CNCSJPCNode::CNCSJPCNodeTracker::sm_nMemTotal + CNCSJPCBuffer::sm_nAllocBytes;
	return(nUsing > nMaxMem);
}

#define PURGE_STAGE_0		0
#define PURGE_STAGE_1		1
//#define PURGE_STAGE_2		2
//#define PURGE_STAGE_3		3
//#define PURGE_STAGE_4		4

#define PURGE_NR_STAGES		(PURGE_STAGE_1+1)

void CNCSJP2File::Purge()
{
	if(OverBudget()) {
		CNCSJPCGlobalLock _Lock;
		UINT32 nStage = 0;

		while(OverBudget() && nStage < PURGE_NR_STAGES) {
			for(UINT32 f = 0; f < sm_Files.size() && OverBudget(); f++) {
				CNCSJP2File *pFile = sm_Files[f];
				bool bSeekable = pFile->m_pStream->Seek();
				UINT32 t = 0;
				while(CNCSJPCTilePartHeader *pTP = pFile->m_Codestream.GetTile(t++)) {
					if(nStage == PURGE_STAGE_1) {
						std::vector<CNCSJPCPLTMarker> &PLTs = pTP->m_PLTs;
						for(INT32 i = (INT32)PLTs.size() - 1; i >= 0; i--) {
							if(PLTs[i].m_bDynamic == false && PLTs[i].GetLengths()) {
								// File based PLTs, free up
								PLTs[i].FreeLengths();

								if(!OverBudget()) {
									break;
								}
							}
						}
					} else if(CNCSJPCPrecinct::HaveZeroRefs()) {
						for(UINT32 c = 0; c < pTP->m_Components.size() && OverBudget(); c++) {
							CNCSJPCComponent *pComponent = pTP->m_Components[c];
							for(INT32 r = (INT32)pComponent->m_Resolutions.size() - 1; r >= 0 && OverBudget(); r--) {
								CNCSJPCResolution *pResolution = pComponent->m_Resolutions[r];
								CNCSJPCPrecinctMap &Precincts = pResolution->m_Precincts;
                //	UINT32 nPrecincts = (UINT32)pResolution->m_Precincts.size();
								UINT32 nPrecinctsHigh = pResolution->GetNumPrecinctsHigh();

								for(UINT32 py = 0; py < nPrecinctsHigh /*&& OverBudget()*/; py++) {
									//CNCSJPCPrecinctMap::CNCSJPCPrecinctMapRow &Row = pResolution->m_Precincts.m_Rows[py];

									if(!Precincts.empty(py)) {
										UINT32 nPrecinctsWide = pResolution->GetNumPrecinctsWide();
										
										for(UINT32 px = 0; px < nPrecinctsWide; px++) {
                      //									if(pResolution->m_Precincts.find(p) == pResolution->m_Precincts.end()) {
                      //										continue;
                      //									}
											CNCSJPCPrecinct *pPrecinct = Precincts.find(px, py);
											if(pPrecinct) {
												switch(nStage) {
                        case PURGE_STAGE_0:
                          // delete unused precincts
                          if(bSeekable && pPrecinct->NrRefs() == 0) {
                            Precincts.remove(pPrecinct);
                          } 
                          break;

                          //	case PURGE_STAGE_2:
                          //		{
                          //			int eBand;
                          // delete decompressed codeblock data (still have compressed data)
                          //			for(eBand = (r == 0 ? NCSJPC_LL : NCSJPC_HL); 
                          //				eBand <= (r == 0 ? NCSJPC_LL : NCSJPC_HH); 
                          //				eBand++) {
                          //				CNCSJPCSubBand *pSubBand = pPrecinct->m_SubBands[eBand];
                          //				if(pSubBand && pSubBand->m_pCodeBlocks->size()) {
                          //					UINT32 nCB = pSubBand->GetNumCBWide() * pSubBand->GetNumCBHigh();
                          //					for(UINT32 cb = 0; cb < nCB; cb++) {
                          //						(*pSubBand->m_pCodeBlocks)[cb].m_DecBuf.Free();
                          //					}
                          //				}
                          //			}
                          //		}
                          //		break;

                          //	case PURGE_STAGE_3:
                          // Destroy subbands for non-PacketStream files
                          // This will force a re-read from disk of the image data
                          //			if(bSeekable && pFile->m_pStream->IsPacketStream() == false) {
                          //		pPrecinct->DestroySubBands();
                          //			}
                          //		break;

                          //FIXME: : large local files, trash packets each read
                          //	case PURGE_STAGE_4:
                          // Destroy subbands for any file
                          // This will force a re-read from disk of the image data
                          //			if(bSeekable || pFile->m_pStream->IsPacketStream()) {
                          //		pPrecinct->DestroySubBands();
                          //			}
                          //		break;
												}
                        //												if(!OverBudget()) {
                        //													break;
                        //												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			nStage++;
		}
	}
}

void CNCSJP2File::Shutdown()
{
	CNCSJPCGlobalLock _Lock;

	std::vector<CNCSJP2File *>::iterator pCur = sm_Files.begin();
	//std::vector<CNCSJP2File *>::iterator pEnd = sm_Files.end();

	while(pCur != sm_Files.end()) {
		CNCSJP2File *pCurFile = *pCur;
		pCur = sm_Files.erase(pCur);
		if( pCurFile ) {
			pCurFile->m_nRefs = 0;
			pCurFile->Close(true,true);
			delete pCurFile;
		}
		//pCur++;
	}
}

void CNCSJP2File::CNCSJP2FileVector::CloseAll(void)
{
	while(this->size() != 0) {
		CNCSJP2File *pFile = (*this)[0];
		while(pFile->m_nRefs) {
			pFile->Close(true);
		}
		delete pFile;
	}
}


CNCSError CNCSJP2File::SetCompress(NCSFileViewFileInfoEx &Info)
{
	CNCSError Error;
	if(m_pStream && m_pStream->IsWrite()) {
		UINT32 r;

		NCSCopyFileInfoEx(&m_FileInfo, &Info);

		m_Codestream.m_SOC.m_bValid = true;
		// SIZ required
		m_Codestream.m_SIZ.m_bValid = true;
		m_Codestream.m_SIZ.m_nXsiz = Info.nSizeX;
		m_Codestream.m_SIZ.m_nYsiz = Info.nSizeY;
		if(m_nCompressionTileWidth != 0) {
			m_Codestream.m_SIZ.m_nXTsiz = m_nCompressionTileWidth;
		} else {
			m_Codestream.m_SIZ.m_nXTsiz = Info.nSizeX;
		}
		if(m_nCompressionTileHeight != 0) {
			m_Codestream.m_SIZ.m_nYTsiz = m_nCompressionTileHeight;
		} else {
			m_Codestream.m_SIZ.m_nYTsiz = Info.nSizeY;
		}
		m_Codestream.m_SIZ.m_nCsiz = Info.nBands;

		switch(m_eCompressionProfile) {
    case CNCSJPC::BASELINE_0:
      m_Codestream.m_SIZ.m_nRsiz = 1; // Profile 0
      break;
    case CNCSJPC::BASELINE_1:
    case CNCSJPC::NITF_BIIF_NPJE:
    case CNCSJPC::NITF_BIIF_EPJE:
      m_Codestream.m_SIZ.m_nRsiz = 2; // Profile 1
      break;
    case CNCSJPC::BASELINE_2:
      m_Codestream.m_SIZ.m_nRsiz = 0; // Profile 2 (no restrictions)
      break;
		}

		m_Codestream.m_SIZ.m_Components.resize(m_Codestream.m_SIZ.m_nCsiz);
		UINT32 nBytes = 0;
		for(int c = 0; c < m_Codestream.m_SIZ.m_nCsiz; c++) {
			m_Codestream.m_SIZ.m_Components[c].m_bSigned = Info.pBands ? (Info.pBands[c].bSigned ? true : false) : false;
			m_Codestream.m_SIZ.m_Components[c].m_nBits = Info.pBands ? Info.pBands[c].nBits : 8;
			m_Codestream.m_SIZ.m_Components[c].m_nXRsiz = 1;
			m_Codestream.m_SIZ.m_Components[c].m_nYRsiz = 1;
			nBytes += 2 + 2 * (m_Codestream.m_SIZ.m_Components[c].m_nBits / 12);
		}
		m_Codestream.m_SIZ.m_nLength = 38 + 3 * m_Codestream.m_SIZ.m_nCsiz;

		// COD required
		m_Codestream.m_COD.m_bValid = true;
		if(Info.eColorSpace == NCSCS_YUV) {
			m_Codestream.m_COD.m_SGcod.m_bMCT = true;
		}

		m_Codestream.m_COD.m_SPcod.m_Scb = 0;//CNCSJPCT1Coder::PREDICTABLE_TERMINATION|CNCSJPCT1Coder::PASS_RESET_CTX;
		m_Codestream.m_COD.m_Scod.bDefinedPrecincts = true;
		m_Codestream.m_COD.m_Scod.bEPHMarkers = m_bCompressionIncludeEPH;
		m_Codestream.m_COD.m_Scod.bSOPMarkers = m_bCompressionIncludeSOP;
		if(m_nCompressionLayers != 0) {
			m_Codestream.m_COD.m_SGcod.m_nLayers = m_nCompressionLayers;
		} else {
			m_Codestream.m_COD.m_SGcod.m_nLayers = 1;
		}
		m_Codestream.m_COD.m_SGcod.m_ProgressionOrder.m_eType = m_eCompressionOrder;

		if(m_nCompressionLevels != 0) {
			m_Codestream.m_COD.m_SPcod.m_nLevels = m_nCompressionLevels;
		} else {
			m_Codestream.m_COD.m_SPcod.m_nLevels = NCSMin(NCSLog2((2 * m_Codestream.m_SIZ.m_nXTsiz) / X_BLOCK_SIZE), NCSLog2((2 * m_Codestream.m_SIZ.m_nYTsiz) / X_BLOCK_SIZE));
		}
		m_Codestream.m_COD.m_SPcod.m_eTransformation = CNCSJPCCodingStyleParameter::REVERSIBLE_5x3;
	
		m_Codestream.m_COD.m_SPcod.m_PrecinctSizes.resize(m_Codestream.m_COD.m_SPcod.m_nLevels + 1);
	
		INT32 nPrecinctSizeX = 64;
		INT32 nPrecinctSizeY = 64;

    // Bump block size to keep # blocks under 2^32
		while((Info.nSizeX / (2 * nPrecinctSizeX)) * (Info.nSizeY / (2 * nPrecinctSizeY)) * 2 * sizeof(UINT64) > 0x7fffffff) {
			nPrecinctSizeX *= 2;
			if((Info.nSizeX / (2 * nPrecinctSizeX)) * (Info.nSizeY / (2 * nPrecinctSizeY)) * 2 * sizeof(UINT64) > 0x7fffffff) {
				nPrecinctSizeY *= 2;
			}
		}
		if(m_nCompressionPrecinctHeight >= NCS2Pow(15) ||
		   m_nCompressionPrecinctWidth >= NCS2Pow(15)) {
			m_Codestream.m_COD.m_Scod.bDefinedPrecincts = false;
		} else {
			if(m_nCompressionPrecinctWidth != 0) {
				nPrecinctSizeX = m_nCompressionPrecinctWidth; //field defined in the CSCSJP2FILE 

				for(r = 0; r < (UINT32)(m_Codestream.m_COD.m_SPcod.m_nLevels + 1); r++) {
					m_Codestream.m_COD.m_SPcod.m_PrecinctSizes[r].m_nPPx = NCSLog2(nPrecinctSizeX);
				}
			} else {
				m_Codestream.m_COD.m_SPcod.m_PrecinctSizes[0].m_nPPx = NCSLog2(X_BLOCK_SIZE);
					
				for(r = 1; r < (UINT32)(m_Codestream.m_COD.m_SPcod.m_nLevels + 1); r++) {
					if(r <= 2 && m_Codestream.m_COD.m_SPcod.m_nLevels + 1 > 5) {
						// low res precincts, and > 5 decomposition levels, keep precincts small
						m_Codestream.m_COD.m_SPcod.m_PrecinctSizes[r].m_nPPx = NCSLog2(2*X_BLOCK_SIZE);
					} else {
						m_Codestream.m_COD.m_SPcod.m_PrecinctSizes[r].m_nPPx = NCSLog2(2*nPrecinctSizeX);
					}
				}
			}

			if(m_nCompressionPrecinctHeight != 0) {
				nPrecinctSizeY = m_nCompressionPrecinctHeight;

				for(r = 0; r < (UINT32)(m_Codestream.m_COD.m_SPcod.m_nLevels + 1); r++) {
					m_Codestream.m_COD.m_SPcod.m_PrecinctSizes[r].m_nPPy = NCSLog2(nPrecinctSizeY);
				}
			} else {
				m_Codestream.m_COD.m_SPcod.m_PrecinctSizes[0].m_nPPy = NCSLog2(X_BLOCK_SIZE);
					
				for(r = 1; r < (UINT32)(m_Codestream.m_COD.m_SPcod.m_nLevels + 1); r++) {
					if(r <= 2 && m_Codestream.m_COD.m_SPcod.m_nLevels + 1 > 5) {
						// low res precincts, and > 5 decomposition levels, keep precincts small
						m_Codestream.m_COD.m_SPcod.m_PrecinctSizes[r].m_nPPy = NCSLog2(2*Y_BLOCK_SIZE);
					} else {
						m_Codestream.m_COD.m_SPcod.m_PrecinctSizes[r].m_nPPy = NCSLog2(2*nPrecinctSizeY);
					}
				}
			}
		}

		if(m_eCompressionProfile == CNCSJPC::NITF_BIIF_NPJE || 
       m_eCompressionProfile == CNCSJPC::NITF_BIIF_EPJE) {
			m_Codestream.m_COD.m_SPcod.m_nXcb = NCSLog2(64);
			m_Codestream.m_COD.m_SPcod.m_nYcb = NCSLog2(64);

			if((INT64)nBytes * Info.nSizeX * (1 /* WriteLineBil */ + 3 /* MCT */ + 10 /* DWT input per lev */ + 2 * NCS2Pow(m_Codestream.m_COD.m_SPcod.m_nYcb) /* SB output */) +
         (Info.nSizeX / nPrecinctSizeX) * (sizeof(CNCSJPCPrecinct) + 3 * sizeof(CNCSJPCSubBand) + (nPrecinctSizeX / NCS2Pow(m_Codestream.m_COD.m_SPcod.m_nXcb)) * (nPrecinctSizeY / NCS2Pow(m_Codestream.m_COD.m_SPcod.m_nYcb)) * sizeof(CNCSJPCCodeBlock))
         > (INT64)NCSPhysicalMemorySize() / 4) {
				
				m_Codestream.m_bLowMemCompression = true;
			}
		} else {
			m_Codestream.m_COD.m_SPcod.m_nXcb = NCSLog2(NCSMin(NCSJPC_ENCODE_BLOCK_SIZE, nPrecinctSizeX));
			m_Codestream.m_COD.m_SPcod.m_nYcb = NCSLog2(NCSMin(NCSJPC_ENCODE_BLOCK_SIZE, nPrecinctSizeY));

			while((INT64)nBytes * Info.nSizeX * (1 /* WriteLineBil */ + 3 /* MCT */ + 10 /* DWT input per lev */ + 2 * NCS2Pow(m_Codestream.m_COD.m_SPcod.m_nYcb) /* SB output */) +
            (Info.nSizeX / nPrecinctSizeX) * (sizeof(CNCSJPCPrecinct) + 3 * sizeof(CNCSJPCSubBand) + (nPrecinctSizeX / NCS2Pow(m_Codestream.m_COD.m_SPcod.m_nXcb)) * (nPrecinctSizeY / NCS2Pow(m_Codestream.m_COD.m_SPcod.m_nYcb)) * sizeof(CNCSJPCCodeBlock))
            > (INT64)NCSPhysicalMemorySize() / 4) {
				m_Codestream.m_COD.m_SPcod.m_nYcb--;// = m_Codestream.m_COD.m_SPcod.m_nYcb >> 1;
				if(m_Codestream.m_COD.m_SPcod.m_nYcb <= NCSLog2(4)) {
					//lowmem
					m_Codestream.m_bLowMemCompression = true;
					break;
				}
			}
		}

		m_Codestream.m_COD.m_nLength = m_Codestream.m_COD.m_Scod.bDefinedPrecincts ? (13 + m_Codestream.m_COD.m_SPcod.m_nLevels) : 12;
		m_Codestream.m_CodingStyles.resize(m_Codestream.m_SIZ.m_nCsiz, m_Codestream.m_COD);

		// QCD required
		m_Codestream.m_QCD.m_bValid = true;
		m_Codestream.m_QCD.m_Sqc.m_nGuardBits = 0x1;// No quantization, 2 guard bits
    //	Mb = pComponent->m_QuantizationStyle.m_Sqc.m_nGuardBits + pComponent->m_QuantizationStyle.m_SPqc[m_pSubBand->m_eType].m_nReversible - 1;

		m_Codestream.m_QCD.m_SPqc.resize(1 + 3 * m_Codestream.m_COD.m_SPcod.m_nLevels);
		for(r = 0; r < (UINT32)(1 + 3 * m_Codestream.m_COD.m_SPcod.m_nLevels); r++) {
			if(r == 0) {
				m_Codestream.m_QCD.m_SPqc[r].m_nReversible = 1+m_Codestream.m_SIZ.m_Components[0].m_nBits + 0;// + (m_Codestream.m_COD.m_SGcod.m_bMCT ? 1 : 0);
			} else {
				switch((r - 1) % 3) {
        case 1:
        case 2:
          m_Codestream.m_QCD.m_SPqc[r].m_nReversible = 1+m_Codestream.m_SIZ.m_Components[0].m_nBits + 1;// + (m_Codestream.m_COD.m_SGcod.m_bMCT ? 1 : 0);
          break;
        case 0:
          m_Codestream.m_QCD.m_SPqc[r].m_nReversible = 1+m_Codestream.m_SIZ.m_Components[0].m_nBits + 2;// + (m_Codestream.m_COD.m_SGcod.m_bMCT ? 1 : 0);
          break;
				}
			}
			m_Codestream.m_QCD.m_SPqc[r].m_nReversible += m_Codestream.m_QCD.m_Sqc.m_nGuardBits;
		}
		m_Codestream.m_QCD.m_nLength = 4 + 3 * m_Codestream.m_COD.m_SPcod.m_nLevels;
		m_Codestream.m_QuantizationStyles.resize(m_Codestream.m_SIZ.m_nCsiz, m_Codestream.m_QCD);

    //		for(UINT32 c = 0; c < m_Codestream.m_SIZ.m_nCsiz; c++) {
    //			if((c == 1 || c == 2) && m_Codestream.m_COD.m_SGcod.m_bMCT) {
    //				for(r = 0; r < (UINT32)(1 + 3 * m_Codestream.m_COD.m_SPcod.m_nLevels); r++) {
    //					m_Codestream.m_QuantizationStyles[c].m_SPqc[r].m_nReversible += 1;
    //				}
    //			}
    //		}
		// Tile
		// new filename manipulation code [03] (old code used direct char * fiddling)
		CNCSString sFilename(m_pStream->GetName());
		int nSlash = sFilename.find_last_of(NCS_T("\\/"));

		CNCSString sTmpDir(sFilename.Substr(0, (nSlash > 0)?nSlash:0 ));
		CNCSString sFilenameStub(sFilename.Substr(nSlash + 1,sFilename.length()));
		sFilenameStub += NCS_T("_jp2_");
		m_Codestream.m_pTmpDir = NCSGetTempFileName(const_cast<char *>(sTmpDir.a_str()),
                                                const_cast<char *>(sFilenameStub.a_str()), NULL);
		// end [03]
		NCSMakeDir(m_Codestream.m_pTmpDir, FALSE);

		UINT32 nCurPLTOffset = 0;
		//initialize JPC.m_Tiles
		if( m_Codestream.m_Tiles.size() == 0 ) {
			INT32 nNumTiles = m_Codestream.GetNumXTiles() * m_Codestream.GetNumYTiles();
			m_Codestream.m_Tiles.reserve( nNumTiles );
			for( int i=0; i < nNumTiles; i++ ) {
				m_Codestream.m_Tiles.push_back(NULL);
			}
		}

		for(UINT32 nTile = 0; nTile < (UINT32)(m_Codestream.GetNumXTiles() * m_Codestream.GetNumYTiles()); nTile++) {
			CNCSJPCTilePartHeader *pTP = new CNCSJPCTilePartHeader(&m_Codestream);
			if(pTP) {
				int c;
				pTP->m_SOT.m_bValid = true;
				pTP->m_SOT.m_nLength = 10;
				pTP->m_SOT.m_nIsot = nTile;
				pTP->m_SOT.m_nTPsot = 0;
				pTP->m_SOT.m_nTNsot = 0;
				pTP->m_nCurTile = nTile;
				pTP->m_SOD.m_bValid = true;
				pTP->m_COD = m_Codestream.m_COD;
				pTP->m_QCD = m_Codestream.m_QCD;
				m_Codestream.m_pCurrentTilePart = pTP;

				// Only the first tile part for each tile is needed.
				if( pTP->m_SOT.m_nTPsot == 0 ) { // first tile part
					m_Codestream.m_Tiles[pTP->m_SOT.m_nIsot] = pTP;
				}

				pTP->m_Components.resize(m_Codestream.m_SIZ.m_nCsiz);
				for(c = 0; c < m_Codestream.m_SIZ.m_nCsiz; c++) {
					pTP->m_Components[c] = new CNCSJPCComponent(pTP, c);
					pTP->m_Components[c]->m_CodingStyle = m_Codestream.m_CodingStyles[c];
					pTP->m_Components[c]->m_QuantizationStyle = m_Codestream.m_QuantizationStyles[c];
				}
				pTP->RecalcSizes();

				
				for(c = 0; c < m_Codestream.m_SIZ.m_nCsiz; c++) {
					for(INT32 r = (INT32)pTP->m_Components[c]->m_Resolutions.size() - 1; r >= 0; r--) {
						CNCSJPCResolution *pResolution = pTP->m_Components[c]->m_Resolutions[r];

						UINT16 nLayers = pTP->m_Components[c]->m_CodingStyle.m_SGcod.m_nLayers;

            //	pResolution->m_EncoderFiles.resize(nLayers);
						pResolution->m_nEncoderPLTStartIndex = nCurPLTOffset;
						nCurPLTOffset += pResolution->GetNumPrecinctsWide() * pResolution->GetNumPrecinctsHigh() * nLayers;
            //						pResolution->m_EncoderPLTIndex.resize(nLayers);
            //						pResolution->m_EncoderPLTIndex[0] = pResolution->m_nEncoderPLTStartIndex;
            //						for(INT32 l = 1; l < nLayers; l++) {
            //							pResolution->m_EncoderPLTIndex[l] = pResolution->m_EncoderPLTIndex[l - 1] + pResolution->GetNumPrecinctsWide() * pResolution->GetNumPrecinctsHigh(); 
            //						}
            //						pResolution->m_EncoderPLTFiles.resize(nLayers);

						if(Info.nCompressionRate == 1 || m_Codestream.m_SIZ.m_Components[c].m_nBits == 1) {
							pResolution->m_nCompressionRatio = 1;
						} else {
							UINT16 nRatio = NCSMax(1, (2 * Info.nCompressionRate) / NCS2Pow(pTP->m_Components[c]->m_CodingStyle.m_SPcod.m_nLevels - r));
							if(pTP->m_Components[c]->m_CodingStyle.m_SGcod.m_bMCT) {
								if(pTP->m_Components[c]->m_iComponent == 0) {
									// Y
									nRatio = (UINT16)(nRatio / 2);
								} else {
									// UV
									nRatio = (UINT16)(nRatio * 1.5);
								}
								nRatio = NCSMax(1, nRatio);
							}
							pResolution->m_nCompressionRatio = nRatio;
						}
						if(m_Codestream.m_SIZ.m_nRsiz == 1) {
							// Profile 0 - verify precincts are OK
							if(pResolution->GetWidth() <= 128 && pResolution->GetHeight() <= 128 && (pResolution->GetNumPrecinctsWide() > 1 || pResolution->GetNumPrecinctsHigh() > 1)) {
								// Not profile 0, so force it to profile 1
								m_Codestream.m_SIZ.m_nRsiz = 2;
							}
						}
					}
				}
			}
		}
		m_Codestream.m_EOC.m_bValid = true;

		m_Codestream.m_pStream = m_pStream;

		if(!m_bCodestreamOnly) {
      // How large is the JP2File box?
      //	m_nXLBox = Stream.Size();
      //	m_nLDBox = m_nXLBox;
		
			m_Signature.m_bValid = true;
			m_FileType.m_bValid = true;
			m_Header.m_bValid = true;
			m_Header.m_ImageHeader.m_bValid = true;
			m_Header.m_ColorSpecification.m_bValid = true;
			((CNCSJP2Box&)m_Codestream).m_bValid = true;

			// Setup the required colorspecification box
			switch(m_FileInfo.eColorSpace) {
      default:
      case NCSCS_GREYSCALE:
        m_Header.m_ColorSpecification.m_eEnumeratedColorspace = CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::GREYSCALE;
        break;
      case NCSCS_MULTIBAND:
        // Greyscale CS, cdef box sets band type as UNKNOWN
        m_Header.m_ColorSpecification.m_eEnumeratedColorspace = CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::GREYSCALE;
        break;
      case NCSCS_YUV:
      case NCSCS_sRGB:
        m_Header.m_ColorSpecification.m_eEnumeratedColorspace = CNCSJP2HeaderBox::CNCSJP2ColorSpecificationBox::sRGB;
        break;
			}
			
			if(m_FileInfo.pBands) {
				UINT32 b;

				// Check and setup the bits per component box if required.
				bool bNeedBPCBox = false;

				for(b = 1; b < m_FileInfo.nBands; b++) {
					if(m_FileInfo.pBands[b].nBits != m_FileInfo.pBands[0].nBits ||
					   m_FileInfo.pBands[b].bSigned != m_FileInfo.pBands[0].bSigned) {
						bNeedBPCBox = true;
					}
				}
				if(bNeedBPCBox) {
					m_Header.m_BitsPerComponent.m_bValid = true;
					m_Header.m_BitsPerComponent.m_Bits.resize(m_FileInfo.nBands);
					m_Header.m_ImageHeader.m_nBits = 255;

					for(b = 0; b < m_FileInfo.nBands; b++) {
						m_Header.m_BitsPerComponent.m_Bits[b].m_nBits = m_FileInfo.pBands[b].nBits;
						m_Header.m_BitsPerComponent.m_Bits[b].m_bSigned = m_FileInfo.pBands[b].bSigned ? true : false;
					}
				}
				// Channel definition box
				UINT32 nBand = (UINT32)-1;
				/*if(sscanf(m_FileInfo.pBands[0].szDesc, NCS_BANDDESC_Band, &nBand) != 1)*/ {
					m_Header.m_ChannelDefinition.m_bValid = true;
					m_Header.m_ChannelDefinition.m_Definitions.resize(m_FileInfo.nBands);
					m_Header.m_ChannelDefinition.m_nEntries = m_FileInfo.nBands;
					for(b = 0; b < m_FileInfo.nBands; b++) {
						CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::ChannelDefinition &def = m_Header.m_ChannelDefinition.m_Definitions[b];
						def.m_iChannel = b;

						if(!stricmp(m_FileInfo.pBands[b].szDesc, NCS_BANDDESC_Red)) {
							def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::COLOR;
							def.m_eAssociation = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::R;					
						} else if(!stricmp(m_FileInfo.pBands[b].szDesc, NCS_BANDDESC_Green)) {
							def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::COLOR;
							def.m_eAssociation = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::G;					
						} else if(!stricmp(m_FileInfo.pBands[b].szDesc, NCS_BANDDESC_Blue)) {
							def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::COLOR;
							def.m_eAssociation = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::B;					
						} else if(!stricmp(m_FileInfo.pBands[b].szDesc, NCS_BANDDESC_All)) {
							def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::COLOR;
							def.m_eAssociation = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::WHOLE_IMAGE;
						} else if(!stricmp(m_FileInfo.pBands[b].szDesc, NCS_BANDDESC_RedOpacity)) {
							def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::OPACITY;
							def.m_eAssociation = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::R;					
						} else if(!stricmp(m_FileInfo.pBands[b].szDesc, NCS_BANDDESC_GreenOpacity)) {
							def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::OPACITY;
							def.m_eAssociation = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::G;					
						} else if(!stricmp(m_FileInfo.pBands[b].szDesc, NCS_BANDDESC_BlueOpacity)) {
							def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::OPACITY;
							def.m_eAssociation = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::B;					
						} else if(!stricmp(m_FileInfo.pBands[b].szDesc, NCS_BANDDESC_AllOpacity)) {
							def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::OPACITY;
							def.m_eAssociation = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::WHOLE_IMAGE;					
						} else if(!stricmp(m_FileInfo.pBands[b].szDesc, NCS_BANDDESC_RedOpacityPremultiplied)) {
							def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::PREMUL_OPACITY;
							def.m_eAssociation = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::R;					
						} else if(!stricmp(m_FileInfo.pBands[b].szDesc, NCS_BANDDESC_GreenOpacityPremultiplied)) {
							def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::PREMUL_OPACITY;
							def.m_eAssociation = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::G;					
						} else if(!stricmp(m_FileInfo.pBands[b].szDesc, NCS_BANDDESC_BlueOpacityPremultiplied)) {
							def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::PREMUL_OPACITY;
							def.m_eAssociation = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::B;					
						} else if(!stricmp(m_FileInfo.pBands[b].szDesc, NCS_BANDDESC_AllOpacityPremultiplied)) {
							def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::PREMUL_OPACITY;
							def.m_eAssociation = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::WHOLE_IMAGE;					
						} else if(!stricmp(m_FileInfo.pBands[b].szDesc, NCS_BANDDESC_Greyscale)) {
							def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::COLOR;
							def.m_eAssociation = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::Y;					
						} else if(!stricmp(m_FileInfo.pBands[b].szDesc, NCS_BANDDESC_GreyscaleOpacity)) {
							def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::OPACITY;
							def.m_eAssociation = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::Y;					
						} else if(!stricmp(m_FileInfo.pBands[b].szDesc, NCS_BANDDESC_GreyscaleOpacityPremultiplied)) {
							def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::PREMUL_OPACITY;
							def.m_eAssociation = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::Y;					
						} else {
							if(m_FileInfo.eColorSpace == NCSCS_MULTIBAND) {
								if(b == 0) {
									def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::COLOR;
									def.m_eAssociation = (CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::Association)(b+1);
								} else {
									def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::UNKNOWN;
									def.m_eAssociation = (CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::Association)(b+1);
								}
							} else {
								def.m_eType = CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::COLOR;
								def.m_eAssociation = (CNCSJP2HeaderBox::CNCSJP2ChannelDefinitionBox::Association)(b+1);
							}
						}
					}
				}
			}

			UINT32 nGeodataUsage = sm_nGeodataUsage;
			while (nGeodataUsage > 0)
        {
          if (nGeodataUsage & JP2_GEODATA_WLD_MASK)
            {
              //Write world file
              CNCSWorldFile WorldFile(DEFAULT_JP2_GEODATA_PRECISION_EPSILON);
              char *szFilename = NCSStrDup(CHAR_STRING(m_pStream->GetName()));
              char *szWorldFilename = NULL;
              if (szFilename != NULL && WorldFile.ConstructWorldFilename(szFilename,&szWorldFilename))
                {
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
                  fprintf(stderr,"World file name: %s\r\n",szWorldFilename);
#endif
                  WorldFile.GetFromFileInfoEx(m_FileInfo,TRUE);
                  if (WorldFile.Write(szWorldFilename))
                    {
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
                      fprintf(stderr,"Writing information to world file %s\r\n", szWorldFilename);
#endif	
                    }
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
                  else fprintf(stderr,"Unable to process information from world file %s\r\n",szWorldFilename);
#endif					
                }
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
              else fprintf(stderr,"Unable to construct output path for world file\r\n");
#endif
              NCSFree(szFilename);
              NCSFree(szWorldFilename);
            }
          if (nGeodataUsage & JP2_GEODATA_GML_MASK)
            {
              //Set up GML box output
              NCSStandardizeFileInfoEx(&m_FileInfo);
              m_GML.SetFileInfo(m_FileInfo);
              m_GML.m_bValid = true;
              m_OtherBoxes.push_back(&m_GML);
            }
          if (nGeodataUsage & JP2_GEODATA_PCS_MASK)
            {
              //Set up PCS box output
              NCSStandardizeFileInfoEx(&m_FileInfo);
              m_PCS.SetFileInfo(m_FileInfo);
              m_PCS.m_bValid = true;
              m_OtherBoxes.push_back(&m_PCS);
            }
          nGeodataUsage = nGeodataUsage >> 3;
        }

			Error = UnParse(*this, *m_pStream);
		} else {
			Error = ((CNCSJPC*)&m_Codestream)->UnParse(*m_pStream);
		}
    //		if(Error == NCS_SUCCESS) {
    //			Error = ((CNCSJPC*)&m_Codestream)->UnParse(*m_pStream);
    //		}
	} else {
		Error = NCS_INVALID_PARAMETER;
	}
	return(Error);
}

// Write the next line into the file.
bool CNCSJP2File::WriteLine(ContextID nCtx, 
                            CNCSJPCBuffer *pSrc, 
                            UINT16 iComponent)
{
	CNCSJPC &JPC = (CNCSJPC&)m_Codestream;
	bool bRet = true;

	if(m_nNextLine == 0 && iComponent == 0) {
		bRet &= JPC.OpenEncoderFiles(true);
	}
	bRet &= CNCSJPCNodeTiler::WriteLine(nCtx, pSrc, iComponent);

	if(iComponent == JPC.m_SIZ.m_nCsiz - 1) {
		m_nNextLine++;
	}

	if(m_nNextLine == JPC.GetComponentHeight(0) && iComponent == JPC.m_SIZ.m_nCsiz - 1) {
		UINT32 nTiles = JPC.GetNumXTiles() * JPC.GetNumYTiles();
		std::vector <CNCSJPCTLMMarker> TLMs;
		UINT32 nCurTilePart = 0;

		m_Codestream.CloseEncoderFiles(false);
		m_Codestream.OpenEncoderFiles(false);

		if(m_eCompressionProfile == CNCSJPC::NITF_BIIF_NPJE ||
       m_eCompressionProfile == CNCSJPC::NITF_BIIF_EPJE) {
			// Need to write TLMs
			UINT32 nTileParts = 0;		

			if(m_eCompressionProfile == CNCSJPC::NITF_BIIF_NPJE) {
				nTileParts = nTiles;
			} else {
				nTileParts = nTiles * (JPC.m_COD.m_SPcod.m_nLevels + 1);
			}
			UINT32 nMaxTPInTLM = NCSFloorDiv(65535 - 6, sizeof(UINT16) + sizeof(UINT32));

			TLMs.resize(NCSCeilDiv(nTileParts, nMaxTPInTLM));
			UINT32 nCurTile = 0;

			for(int i = 0; i < (int)TLMs.size(); i++) {
				TLMs[i].m_bValid = true;
				TLMs[i].m_Stlm.m_nST = 2;
				TLMs[i].m_Stlm.m_nSP = 1;
				TLMs[i].m_nZtlm = i;

				TLMs[i].m_Pointers.resize(NCSMin(nTileParts, nMaxTPInTLM));
				TLMs[i].m_nLength = (UINT16)(4 + 6 * TLMs[i].m_Pointers.size());

				for(int j = 0; j < (int)TLMs[i].m_Pointers.size(); j++) {
					TLMs[i].m_Pointers[j].m_nTtlm = nCurTile++;
					TLMs[i].m_Pointers[j].m_nPtlm = 0;
					if(nCurTile >= nTiles) {
						nCurTile = 0;
					}
				}
				nTileParts -= NCSMin(nTileParts, nMaxTPInTLM);
				TLMs[i].UnParse(JPC, *m_pStream);
			}
		}
		if(m_eCompressionProfile == CNCSJPC::NITF_BIIF_EPJE) {
			// EPJE is:
			// - RLCP progression
			// - one tile part per resolution/tile
			// - one self-contained PLT per tile part, 

			for(UINT32 r = 0; r < (UINT32)JPC.GetTile(0)->m_Components[0]->m_Resolutions.size(); r++) {
				for(UINT32 t = 0; t < nTiles; t++) {
					CNCSJPCTilePartHeader *pTP = JPC.GetTile(t);
					CNCSJPCPLTMarker PLT;
					CNCSJPCPacketLengthType Len;
					Len.m_nDataLength = 0;
          //	INT64 nSotOffset = m_pStream->Tell();
					CNCSJPCTilePartHeader TilePart(&JPC);
					UINT32 l;

					TilePart.m_SOT.m_nIsot = pTP->m_SOT.m_nIsot;
					TilePart.m_nCurTile = TilePart.m_SOT.m_nIsot;
					TilePart.m_SOT.m_nTPsot = r;								
					TilePart.m_SOT.m_bValid = true;
					TilePart.m_SOT.m_nLength = 10;
					TilePart.m_SOD.m_bValid = true;

					INT64 nSOTOffset = m_pStream->Tell();

					UINT32 nBytes = 3;
					nSOTOffset = m_pStream->Tell();

					TilePart.UnParse(JPC, *m_pStream);
					PLT.m_nZplt = 0;

          //	pTP->OpenEncoderPLTFile(false);
          //	TilePart.m_pEncoderPLTFile = pTP->m_pEncoderPLTFile;
					
					for(l = 0; l < JPC.m_COD.m_SGcod.m_nLayers; l++) {
						for(int c = 0; c < JPC.m_SIZ.m_nCsiz; c++) {
							CNCSJPCResolution *pResolution = pTP->m_Components[c]->m_Resolutions[r];
							bRet &= pResolution->WriteLayerPLTs(JPC,
                                                  *m_pStream,
                                                  TilePart,
                                                  PLT,
                                                  Len,
                                                  nSOTOffset,
                                                  nBytes, l);	
						}
					}
					if(nBytes != 3) {
						PLT.m_nLength = nBytes;
						bRet &= PLT.UnParse(JPC, *m_pStream) == NCS_SUCCESS;
						PLT.FreeLengths();
					}
          //	TilePart.m_pEncoderPLTFile = NULL;
          //	pTP->CloseEncoderPLTFile(true);

					bRet &= TilePart.m_SOD.UnParse(JPC, *m_pStream) == NCS_SUCCESS;

					for(l = 0; l < JPC.m_COD.m_SGcod.m_nLayers; l++) {
						for(int c = 0; c < JPC.m_SIZ.m_nCsiz; c++) {
							CNCSJPCResolution *pResolution = pTP->m_Components[c]->m_Resolutions[r];
							bRet &= pResolution->WriteLayerPackets(JPC, *m_pStream, l);
						}
					}
					// Update Psot in TLM for this tile part
					INT64 nCurOffset = m_pStream->Tell();
					INT64 nPSot = nCurOffset - nSOTOffset;
					bRet &= m_pStream->Seek((TLMs[nCurTilePart / TLMs[0].m_Pointers.size()].m_nOffset) + 6 + (nCurTilePart % TLMs[0].m_Pointers.size()) * 6 + 2, CNCSJPCIOStream::START);
					bRet &= m_pStream->WriteUINT32((UINT32)nPSot);
					bRet &= m_pStream->Seek(nCurOffset, CNCSJPCIOStream::START);

					// Update Psot in tilepart to actual # bytes in tile part
					bRet &= m_pStream->Seek(nSOTOffset + 3 * sizeof(UINT16), CNCSJPCIOStream::START);
					bRet &= m_pStream->WriteUINT32((UINT32)nPSot);
					bRet &= m_pStream->Seek(nCurOffset, CNCSJPCIOStream::START);

					nCurTilePart++;
				}
			}
		} else {
			for(UINT32 t = 0; t < nTiles; t++) {
				CNCSJPCTilePartHeader *pTP = JPC.GetTile(t);
				CNCSJPCPLTMarker PLT;
				CNCSJPCPacketLengthType Len;
				Len.m_nDataLength = 0;
        //	INT64 nSotOffset = m_pStream->Tell();
				CNCSJPCTilePartHeader TilePart(&JPC);
				UINT32 l, r;

        //	pTP->OpenEncoderPLTFile(false);

				TilePart.m_SOT.m_nIsot = pTP->m_SOT.m_nIsot;
				TilePart.m_nCurTile = TilePart.m_SOT.m_nIsot;
				TilePart.m_SOT.m_bValid = true;
				TilePart.m_SOT.m_nLength = 10;
				TilePart.m_SOD.m_bValid = true;
        //				TilePart.m_pEncoderPLTFile = pTP->m_pEncoderPLTFile;
				INT64 nSOTOffset = m_pStream->Tell();

			
				UINT32 nBytes = 3;
				bRet = pTP->UnParse(JPC, *m_pStream) == NCS_SUCCESS;

				switch(JPC.m_COD.m_SGcod.m_ProgressionOrder.m_eType) {
        case CNCSJPCProgressionOrderType::LRCP:
          for(l = 0; l < JPC.m_COD.m_SGcod.m_nLayers; l++) {
            for(r = 0; r < (UINT32)pTP->m_Components[0]->m_Resolutions.size(); r++) {
              for(int c = 0; c < JPC.m_SIZ.m_nCsiz; c++) {
                CNCSJPCResolution *pResolution = pTP->m_Components[c]->m_Resolutions[r];
                bRet &= pResolution->WriteLayerPLTs(JPC,
                                                    *m_pStream,
                                                    TilePart,
                                                    PLT,
                                                    Len,
                                                    nSOTOffset,
                                                    nBytes, l);	
              }
            }
          }
								
          if(nBytes != 3) {
            PLT.m_nLength = nBytes;
            bRet &= PLT.UnParse(JPC, *m_pStream) == NCS_SUCCESS;
            PLT.FreeLengths();
          }

          bRet &= TilePart.m_SOD.UnParse(JPC, *m_pStream) == NCS_SUCCESS;

          for(l = 0; l < JPC.m_COD.m_SGcod.m_nLayers; l++) {
            for(r = 0; r < (UINT32)pTP->m_Components[0]->m_Resolutions.size(); r++) {
              for(int c = 0; c < JPC.m_SIZ.m_nCsiz; c++) {
                CNCSJPCResolution *pResolution = pTP->m_Components[c]->m_Resolutions[r];
                bRet &= pResolution->WriteLayerPackets(JPC, *m_pStream, l);
              }
            }
          }
          break;

        case CNCSJPCProgressionOrderType::RLCP:
          for(r = 0; r < (UINT32)pTP->m_Components[0]->m_Resolutions.size(); r++) {
            for(l = 0; l < JPC.m_COD.m_SGcod.m_nLayers; l++) {
              for(int c = 0; c < JPC.m_SIZ.m_nCsiz; c++) {
                CNCSJPCResolution *pResolution = pTP->m_Components[c]->m_Resolutions[r];
                bRet &= pResolution->WriteLayerPLTs(JPC, 
                                                    *m_pStream,
                                                    TilePart,
                                                    PLT,
                                                    Len,
                                                    nSOTOffset,
                                                    nBytes, l);	
              }
            }
          }
								
          if(nBytes != 3) {
            PLT.m_nLength = nBytes;
            bRet &= PLT.UnParse(JPC, *m_pStream) == NCS_SUCCESS;
            PLT.FreeLengths();
          }

          bRet &= TilePart.m_SOD.UnParse(JPC, *m_pStream) == NCS_SUCCESS;

          for(r = 0; r < (UINT32)pTP->m_Components[0]->m_Resolutions.size(); r++) {
            for(l = 0; l < JPC.m_COD.m_SGcod.m_nLayers; l++) {
              for(int c = 0; c < JPC.m_SIZ.m_nCsiz; c++) {
                CNCSJPCResolution *pResolution = pTP->m_Components[c]->m_Resolutions[r];
                bRet &= pResolution->WriteLayerPackets(JPC, *m_pStream, l);
              }
            }
          }
          break;

        case CNCSJPCProgressionOrderType::RPCL:
          for(r = 0; r < (UINT32)pTP->m_Components[0]->m_Resolutions.size(); r++) {
            UINT32 nPrecincts = pTP->m_Components[0]->m_Resolutions[r]->GetNumPrecinctsWide() * pTP->m_Components[0]->m_Resolutions[r]->GetNumPrecinctsHigh();
            for(UINT32 p = 0; p < nPrecincts; p++) {
              for(int c = 0; c < JPC.m_SIZ.m_nCsiz; c++) {
                CNCSJPCResolution *pResolution = pTP->m_Components[c]->m_Resolutions[r];
                for(l = 0; l < JPC.m_COD.m_SGcod.m_nLayers; l++) {
                  bRet &= pResolution->WritePrecinctLayerPLT(JPC, 
                                                             *m_pStream,
                                                             TilePart,
                                                             PLT,
                                                             Len,
                                                             nSOTOffset,
                                                             nBytes, 
                                                             p, 
                                                             l);	
                }
              }
            }
          }				
								
          if(nBytes != 3) {
            PLT.m_nLength = nBytes;
            bRet &= PLT.UnParse(JPC, *m_pStream) == NCS_SUCCESS;
            PLT.FreeLengths();
          }

          bRet &= TilePart.m_SOD.UnParse(JPC, *m_pStream) == NCS_SUCCESS;

          for(r = 0; r < (UINT32)pTP->m_Components[0]->m_Resolutions.size(); r++) {
            UINT32 nPrecincts = pTP->m_Components[0]->m_Resolutions[r]->GetNumPrecinctsWide() * pTP->m_Components[0]->m_Resolutions[r]->GetNumPrecinctsHigh();
            for(UINT32 p = 0; p < nPrecincts; p++) {
              for(int c = 0; c < JPC.m_SIZ.m_nCsiz; c++) {
                CNCSJPCResolution *pResolution = pTP->m_Components[c]->m_Resolutions[r];
                for(l = 0; l < JPC.m_COD.m_SGcod.m_nLayers; l++) {
                  bRet &= pResolution->WritePrecinctLayerPacket(JPC, *m_pStream, p, l);
                }
              }
            }
          }
          break;
				}
				INT64 nCurOffset = m_pStream->Tell();
				INT64 nPSot = nCurOffset - nSOTOffset;

				if(m_eCompressionProfile == CNCSJPC::NITF_BIIF_NPJE) {
					// Update TLM
					bRet &= m_pStream->Seek(TLMs[nCurTilePart / TLMs[0].m_Pointers.size()].m_nOffset + 6 + (nCurTilePart % TLMs[0].m_Pointers.size()) * 6 + 2, CNCSJPCIOStream::START);
					bRet &= m_pStream->WriteUINT32((UINT32)nPSot);
					bRet &= m_pStream->Seek(nCurOffset, CNCSJPCIOStream::START);
					nCurTilePart++;
				}
				if(nTiles > 1 || m_eCompressionProfile == CNCSJPC::NITF_BIIF_NPJE) {
					// Update Psot in tilepart to actual # bytes in tile part
					bRet &= m_pStream->Seek(nSOTOffset + 3 * sizeof(UINT16), CNCSJPCIOStream::START);
					bRet &= m_pStream->WriteUINT32((UINT32)nPSot);
					bRet &= m_pStream->Seek(nCurOffset, CNCSJPCIOStream::START);
				}
        //				TilePart.m_pEncoderPLTFile = NULL;
        //		pTP->CloseEncoderPLTFile(true);

			}
		}
	}
	/**[04]**/
	if (m_nNextLine == JPC.GetComponentHeight(0) && iComponent == JPC.m_SIZ.m_nCsiz - 1)
    {
      m_Codestream.m_EOC.UnParse(JPC, *m_pStream);
    }
	return(bRet);
}

// Get normal Node Width.
UINT32 CNCSJP2File::GetNodeWidth(UINT16 iComponent)
{
	return(m_Codestream.m_SIZ.m_nXTsiz);
}

// Get normal Node Height.
UINT32 CNCSJP2File::GetNodeHeight(UINT16 iComponent)
{
	return(m_Codestream.m_SIZ.m_nYTsiz);
}

// Get number of nodes wide.
UINT32 CNCSJP2File::GetNumNodesWide(UINT16 iComponent)
{
	return(m_Codestream.GetNumXTiles());
}
// Get number of nodes high.
UINT32 CNCSJP2File::GetNumNodesHigh(UINT16 iComponent)
{
	return(m_Codestream.GetNumYTiles());
}

CNCSJPCNode *CNCSJP2File::GetNodePtr(UINT32 nNode, UINT16 iComponent) 
{
	return(m_Codestream.GetTile(nNode)); 
}

void CNCSJP2File::SetKeySize()
{
	sm_nKeySize = 0;
}

extern "C" void NCSInitFileInfoEx(NCSFileViewFileInfoEx *pDst)
{
	memset(pDst, 0, sizeof(NCSFileViewFileInfoEx));
	pDst->eCellSizeUnits = ECW_CELL_UNITS_METERS;
	pDst->fCellIncrementX = 1.0;
	pDst->fCellIncrementY = 1.0;
	pDst->fCWRotationDegrees = 0.0;
	pDst->fOriginX = 0.0;
	pDst->fOriginY = 0.0;
	pDst->szDatum = NCSStrDup("RAW");
	pDst->szProjection = NCSStrDup("RAW");
	pDst->eCellType = NCSCT_UINT8;
}

extern "C" void NCSFreeFileInfoEx(NCSFileViewFileInfoEx *pDst)
{
	if (pDst->szDatum != NULL) NCSFree(pDst->szDatum);
	if (pDst->szProjection != NULL) NCSFree(pDst->szProjection);
	if(pDst->pBands) {
		for(UINT32 b = 0; b < pDst->nBands; b++) {
			NCSFree(pDst->pBands[b].szDesc);
		}
		NCSFree(pDst->pBands);
	}
	memset(pDst, 0, sizeof(NCSFileViewFileInfoEx));
}

extern "C" void NCSCopyFileInfoEx(NCSFileViewFileInfoEx *pDst,
                                  NCSFileViewFileInfoEx *pSrc)
{
	NCSFreeFileInfoEx(pDst);
	memcpy(pDst, pSrc, sizeof(NCSFileViewFileInfoEx));
	pDst->szDatum = NCSStrDup(pSrc->szDatum);
	pDst->szProjection = NCSStrDup(pSrc->szProjection);
	if(pSrc->pBands) {
		pDst->pBands = (NCSFileBandInfo*)NCSMalloc(pSrc->nBands * sizeof(NCSFileBandInfo), FALSE);
		for(UINT32 b = 0; b < pSrc->nBands; b++) {
			pDst->pBands[b].nBits = pSrc->pBands[b].nBits;
			pDst->pBands[b].bSigned = pSrc->pBands[b].bSigned;
			pDst->pBands[b].szDesc = NCSStrDup(pSrc->pBands[b].szDesc);
		}
	}
}

extern "C" BOOLEAN NCSIsFileInfoExGeoreferenced(NCSFileViewFileInfoEx *pInfo)
{
	BOOLEAN bRetVal = (!(stricmp(pInfo->szProjection,"RAW") == 0)) ||
    (!(stricmp(pInfo->szDatum,"RAW") == 0)) ||
    (pInfo->fOriginX != 0) ||
    (pInfo->fOriginY != 0) ||
    (pInfo->fCellIncrementX != 1.0) ||
    (pInfo->fCellIncrementY != 1.0) ||
    (pInfo->eCellSizeUnits != ECW_CELL_UNITS_METERS) ||
    (pInfo->fCWRotationDegrees != 0.0);
	return bRetVal;	
}

//Normalises rotation and cell size values so that -PI < rot <= PI, 
//and fCellIncrementX > 0
extern "C" void NCSStandardizeFileInfoEx(NCSFileViewFileInfoEx *pInfo)
{
	if (pInfo->fCellIncrementX < 0)
    {
      pInfo->fCellIncrementX = -(pInfo->fCellIncrementX);
      pInfo->fCellIncrementY = -(pInfo->fCellIncrementY);
      pInfo->fCWRotationDegrees += 180.0; //flip rotation
    } 
	//Shift rotation to between -180 and 180 degrees
	if (pInfo->fCWRotationDegrees <= -180.0)
    {
      pInfo->fCWRotationDegrees -= 180.0;
      pInfo->fCWRotationDegrees = fmod(pInfo->fCWRotationDegrees,360.0);
      pInfo->fCWRotationDegrees += 180.0;
    }
	if (pInfo->fCWRotationDegrees >= 180.0)
    {
      pInfo->fCWRotationDegrees += 180.0;
      pInfo->fCWRotationDegrees = fmod(pInfo->fCWRotationDegrees,360.0);
      pInfo->fCWRotationDegrees -= 180.0;
    }
}

