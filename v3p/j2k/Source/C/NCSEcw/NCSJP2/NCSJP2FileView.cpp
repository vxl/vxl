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
** [01] 07Sep04 tfl  Changes for parameters to support world files
** [02] 11Nov04 tfl  Changes to ensure rotation data persists correctly
** [03] 21Sep05 sjc  Use upstream NCSJP2 view tiling logic with >4kx4k views
** [04] 12Dec05 tfl  Change to reset m_nNextLine = 0 on every SetView call on an ECW file
** [05] 14Dec05 tfl  Fixes from Simon to logic on transition between tiled/untiled view
** [06] 19Jan06 tfl  Misc fixes, including reducing vector of tiled views to a single view
and fixing GetFileViewSetInfo for the tiled case
** [07] 13Mar06 tfl  Memory leak fix from Simon
*******************************************************/

#include "NCSJP2FileView.h"
#include "NCSJPCMCTNode.h"
#include "NCSJPCDCShiftNode.h"
#include "NCSJPCYCbCr2RGBNode.h"
#include "NCSJPCResample.h"
#include "NCSJPCEcwpIOStream.h"
#include "NCSJPCCodeBlock.h"
#include "NCSJPCT1Coder.h"
#include "NCSEcw.h"
#include "../../NCSGDT2/NCSGDTEpsg.h"

#define ECW_COMPRESS_SDK_LIMIT	((500 * 1024 * 1024) ^ ECW_COMPRESS_SDK_LIMIT_MASK)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Initialise static
CNCSJP2FileView::CNCSJP2FileViewVector CNCSJP2FileView::sm_Views;

// Constructor
CNCSJP2FileView::CNCSJP2FileView()
{
	CNCSJPCGlobalLock _Lock;
	
	if(sm_Views.size() == 0 && CNCSJP2File::sm_Files.size() == 0) {// && (!pNCSEcwInfo || (pNCSEcwInfo && (pNCSEcwInfo->pNCSFileList == NULL)))) {
		//	NCSecwInitInternal();
		if(sm_Views.IsRunning() == false) {
			sm_Views.Spawn(NULL, false);
		}
	}
	
	m_pFile = NULL;
#ifdef NCSJPC_ECW_SUPPORT
	m_pECWFileView = NULL;
	m_pECWCompressionTask = NULL;
#endif // NCSJPC_ECW_SUPPORT
	m_pDatum = NULL;
	m_pProjection = NULL;
	m_pFilename = NULL;
	m_bIsProgressive = FALSE;
	m_bIsOpen = FALSE;
	m_bHaveValidSetView = FALSE;
	m_bSetViewModeIsWorld = FALSE;
	m_pnSetViewBandList = NULL;
	m_pResampler = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nNumberOfBands = 0;
	m_dCompressionRate = 10;
	m_dTargetCompressionRate = 10;
	m_dCellIncrementX = 1.0;
	m_dCellIncrementY = 1.0;
	m_dOriginX = 0.0;
	m_dOriginY = 0.0;
	m_CellSizeUnits = ECW_CELL_UNITS_METERS;
	m_eCellType = NCSCT_UINT8;
	
	/** Initialise m_CurrentView */
	memset(&m_CurrentView, 0, sizeof(m_CurrentView));
	
	m_bIsProgressive = false;
	m_bHaveValidSetView = false;
	m_bSetViewModeIsWorld = false;
	m_bIsOpen = false;
	m_bIsWrite = false;
	m_bPendingView = false;
	m_bAutoScaleUp = false;
	memset(&m_PendingView, 0, sizeof(m_PendingView));
	
	m_nNextLine = 0;
	m_nBlocksAtLastRefreshUpdate = 0;
	m_bInRefreshUpdate = false;
	m_bCancelRead = false;
	m_pRefreshCallback = NULL;
	m_pCompressClient = NULL;
	m_bLowMemCompress = false;
	
	m_pOutputNode = NULL;
	
	m_bCompressMTRead = true;//NCSGetNumPhysicalCPUs() > 1;
	
	UpdateFileViewInfo();
	
	m_bTiledView = false;
	m_pTiledView = NULL;
	
	sm_Views.push_back(this);
}

// Destructor
CNCSJP2FileView::~CNCSJP2FileView()
{
	CNCSJPCGlobalLock _Lock;
	
	Close(true);
	
	std::vector<CNCSJP2FileView *>::iterator pCur = sm_Views.begin();
	std::vector<CNCSJP2FileView *>::iterator pEnd = sm_Views.end();
	
	while(pCur != pEnd) {
		if(*pCur == this) {
			sm_Views.erase(pCur);
			break;
		}
		pCur++;
	}
	//			if(sm_Views.size() == 0 && CNCSJP2File::sm_Files.size() == 0) {// && pNCSEcwInfo->pNCSFileList == NULL) {
	//				Shutdown();//NCSecwShutdown();
	//			}
	DeleteDynamicNodes();
	
	if(sm_Views.size() == 0 && CNCSJP2File::sm_Files.size() == 0 && pNCSEcwInfo->pNCSFileList == NULL) {
		Shutdown();//NCSecwShutdown();
	}
	
	if( m_pResampler ) {
		delete m_pResampler;
		m_pResampler = NULL;
	}
}

void CNCSJP2FileView::DeleteDynamicNodes(void)
{
	CNCSJPCGlobalLock _Lock;
	
	UINT32 nNodes = (UINT32)m_DynamicNodes.size();
	
	for(UINT32 d = 0; d < nNodes; d++) {
		delete m_DynamicNodes[d];
	}
	m_DynamicNodes.clear();
	/*	while(m_DynamicNodes.size() > 0) {
	CNCSJPCNode *pNode = m_DynamicNodes[0];
	m_DynamicNodes.erase(m_DynamicNodes.begin());
	delete pNode;
}*/
}

#ifdef NCS_BUILD_UNICODE
// Open the JP2 file for Parsing/UnParsing.
CNCSError CNCSJP2FileView::Open(wchar_t *pURLPath, bool bProgressiveDisplay, bool bWrite)
{
	CNCSError Error = NCS_INVALID_PARAMETER;
	CNCSJPCGlobalLock _Lock;
	
	Close(true);
	if(!bWrite) {
#ifdef NCSJPC_ECW_SUPPORT
		wchar_t *exts[] = { L".jp2", L".j2k", L".jpx", L".jpc", L".j2c", L".jpf" };
		bool bTestECW = true;
		
		for(int i = 0; i < sizeof(exts) / sizeof(exts[0]); i++) {
			if(wcsicmp(pURLPath + wcslen(pURLPath) - wcslen(exts[i]), exts[i]) == 0) {
				bTestECW = false;
				break;
			}
		}
		if(bTestECW) {
			Error = NCScbmOpenFileView_ECW(CHAR_STRING(pURLPath),
				&m_pECWFileView,
				bProgressiveDisplay ? CNCSJP2FileView::sRefreshCallback : NULL);
		}
#endif
		if(Error != NCS_SUCCESS) {
			Error = CNCSJP2File::sOpen(&m_pFile, pURLPath);
		}
		if(Error == NCS_SUCCESS) {
			m_bIsOpen = true;
			m_bIsWrite = bWrite;
			m_pResampler = new CNCSJPCResample;
			m_bIsProgressive = bProgressiveDisplay;
			SetFileParameters();
			UpdateFileViewInfo();
		}
	} else {
		Error = OpenForWrite(CHAR_STRING(pURLPath), NULL, bProgressiveDisplay);
	}
	return(Error);
}
#endif

// Open the JP2 file for Parsing/UnParsing.
CNCSError CNCSJP2FileView::Open(char *pURLPath, bool bProgressiveDisplay, bool bWrite)
{
	CNCSError Error = NCS_INVALID_PARAMETER;
	CNCSJPCGlobalLock _Lock;
	
	Close(true);
	if(!bWrite) {
#ifdef NCSJPC_ECW_SUPPORT
		char *exts[] =  ERS_JP2_DATASET_EXTS;
		bool bTestECW = true;
		
		for(int i = 0; i < sizeof(exts) / sizeof(exts[0]); i++) {
			if(stricmp(pURLPath + strlen(pURLPath) - strlen(exts[i]), exts[i]) == 0) {
				bTestECW = false;
				break;
			}
		}
		if(bTestECW) {
			Error = NCScbmOpenFileView_ECW(pURLPath,
				&m_pECWFileView,
				bProgressiveDisplay ? CNCSJP2FileView::sRefreshCallback : NULL);
			memset(&m_CurrentView, 0, sizeof(m_CurrentView));
		}
#endif
		if(Error != NCS_SUCCESS) {
			Error = CNCSJP2File::sOpen(&m_pFile, OS_STRING(pURLPath));
		}
		if(Error == NCS_SUCCESS) {
			m_bIsOpen = true;
			m_bIsProgressive = bProgressiveDisplay;
			m_pResampler = new CNCSJPCResample;
			SetFileParameters();
			UpdateFileViewInfo();
		} else {
			delete m_pFile;
			m_pFile = NULL;
		}
	} else {
		Error = OpenForWrite(pURLPath, NULL, bProgressiveDisplay);
	}
	return(Error);
}

// Open the JP2 file for Parsing/UnParsing.
CNCSError CNCSJP2FileView::Open(CNCSJPCIOStream *pStream, bool bProgressiveDisplay)
{
	CNCSError Error = NCS_INVALID_PARAMETER;
	CNCSJPCGlobalLock _Lock;
	
	Close(true);
	if(!pStream->IsWrite()) {
#ifdef NCSJPC_ECW_SUPPORT
		char *exts[] =  ERS_JP2_DATASET_EXTS;
		bool bTestECW = true;
		char *pURLPath = CHAR_STRING(pStream->GetName());
		
		for(int i = 0; i < sizeof(exts) / sizeof(exts[0]); i++) {
			if(stricmp(pURLPath + strlen(pURLPath) - strlen(exts[i]), exts[i]) == 0) {
				bTestECW = false;
				break;
			}
		}
		if(bTestECW) {
			//FIXME - ECW from IO Stream hooks needed here
			Error = NCScbmOpenFileView_ECW(pURLPath,
				&m_pECWFileView,
				bProgressiveDisplay ? CNCSJP2FileView::sRefreshCallback : NULL);
			memset(&m_CurrentView, 0, sizeof(m_CurrentView));
		}
#endif
		if(Error != NCS_SUCCESS) {
			Error = CNCSJP2File::sOpen(&m_pFile, pStream);
		}
		if(Error == NCS_SUCCESS) {
			m_bIsOpen = true;
			m_bIsProgressive = bProgressiveDisplay;
			m_pResampler = new CNCSJPCResample;
			SetFileParameters();
			UpdateFileViewInfo();
		} else {
			delete m_pFile;
			m_pFile = NULL;
		}
	} else {
		Error = OpenForWrite(NULL, pStream, bProgressiveDisplay);
	}
	return(Error);
}

// Open the JP2 file for Parsing/UnParsing.
CNCSError CNCSJP2FileView::OpenForWrite(char *pURLPath, CNCSJPCIOStream *pStream, bool bProgressiveDisplay)
{
	CNCSError Error = NCS_INVALID_PARAMETER;
	CNCSJPCGlobalLock _Lock;
	
	Close(true);
	{
		INT32 b;
		INT32 nLimit = ECW_COMPRESS_SDK_LIMIT;
		
		/*
		** Validate the parameters that were set by the client
		*/
		if((m_dTargetCompressionRate < 1.0) ||
			/* Bad physical dimensions */
			(m_nWidth == 0) ||
			(m_nHeight == 0) ||
			(m_nNumberOfBands == 0) ||
			/* Bad compression format */
			((m_eColorSpace != NCSCS_GREYSCALE) &&
			(m_eColorSpace != NCSCS_YUV) &&
			(m_eColorSpace != NCSCS_MULTIBAND) &&
			(m_eColorSpace != NCSCS_sRGB)) ||
			/* Bad cell increment */
			(m_dCellIncrementX == 0.0) ||
			(m_dCellIncrementY == 0.0) ||
			/* Bad units */
			((m_CellSizeUnits != ECW_CELL_UNITS_METERS) && 
			(m_CellSizeUnits != ECW_CELL_UNITS_DEGREES) && 
			(m_CellSizeUnits != ECW_CELL_UNITS_FEET)) ||
			(m_Bands.size() == 0)/* ||
								 //FIXME
								 (m_eCellType != NCSCT_UINT8 && m_eCellType != NCSCT_UINT16 && m_eCellType != NCSCT_IEEE4)*/) {
								 return(NCS_INVALID_PARAMETER);
		}
		if((CNCSJP2File::sm_nKeySize ^ ECW_COMPRESS_SDK_LIMIT_MASK) == 0) {
			UINT32 nMask = CNCSJP2File::sm_nKeySize;
			/*if(((INT64)m_nWidth * (INT64)m_nHeight * (INT64)m_nNumberOfBands) > (nLimit ^ nMask)) {
				Close(TRUE);
				return(NCS_INPUT_SIZE_EXCEEDED);
			}*/
			nLimit = ECW_COMPRESS_SDK_LIMIT;
		} else {
			CNCSJP2File::sm_nKeySize = ECW_COMPRESS_SDK_LIMIT_MASK;
		}
		m_pFilename = NCSStrDup(pURLPath ? pURLPath : CHAR_STRING(pStream->GetName()));
		
#ifdef NCSJPC_ECW_SUPPORT
		if(pURLPath && stricmp(pURLPath + strlen(pURLPath) - strlen(ERS_WAVELET_DATASET_EXT), ERS_WAVELET_DATASET_EXT) == 0) {
			// ECW only supports unsigned 8 bit.
			for(b = 0; b < m_nNumberOfBands; b++) {
				if(m_Bands[b].bSigned || m_Bands[b].nBits != 8) {
					return(NCS_INVALID_PARAMETER);
				}
			}
			INT32 nBlockSizeX = X_BLOCK_SIZE;
			INT32 nBlockSizeY = Y_BLOCK_SIZE;
			
			// Bump block size to keep # blocks under 2^32
			while((m_nWidth / (2 * nBlockSizeX)) * (m_nHeight / (2 * nBlockSizeY)) * 2 * sizeof(UINT64) > 0x7fffffff) {
				nBlockSizeX *= 2;
				if((m_nWidth / (2 * nBlockSizeX)) * (m_nHeight / (2 * nBlockSizeY)) * 2 * sizeof(UINT64) > 0x7fffffff) {
					nBlockSizeY *= 2;
				}
			}
			if((m_pECWCompressionTask = (EcwCompressionTask*)NCSMalloc(sizeof(EcwCompressionTask), TRUE)) != NULL) {
				UINT16 num_levels = 0;
				char *pTmp;
				
				m_pECWCompressionTask->pClient = this;
				m_pECWCompressionTask->eInputError = NCS_SUCCESS;
				m_pECWCompressionTask->OutputHandle = NCS_NULL_FILE_HANDLE;
				
				m_pECWCompressionTask->eInternalFormat = (CompressFormat)m_eColorSpace;
				
				/* force output number of bands to desired values */
				if( m_eColorSpace == NCSCS_GREYSCALE) {
					/* always write one band */
					m_nNumberOfBands = 1;		
				} else if( m_eColorSpace == NCSCS_YUV ) {
					/* always write RGB encode as three bands in YUV format */
					m_nNumberOfBands = 3;		
				} else if( m_eColorSpace == NCSCS_MULTIBAND ) {
					/* for COMPRESS_MULTI we should leave things alone */	
				} else if( m_eColorSpace == NCSCS_sRGB ) {
					m_nNumberOfBands = 3;		
					m_pECWCompressionTask->eInternalFormat = (CompressFormat)NCSCS_YUV;
				}
				INT32 i;
				
				strcpy(m_pECWCompressionTask->szTmpDir, m_pFilename);
				pTmp = &(m_pECWCompressionTask->szTmpDir[strlen(m_pECWCompressionTask->szTmpDir) - 1]);
				while((pTmp >= m_pECWCompressionTask->szTmpDir) && *pTmp) {
					if((*pTmp == '\\') || (*pTmp == '/')) {
						*pTmp = '\0';
						break;
					}
					pTmp--;
				}
				if(pTmp < m_pECWCompressionTask->szTmpDir) {
					strcpy(m_pECWCompressionTask->szTmpDir, "");
				}
				/* Open the output ECW file */
				Error = NCSFileOpen(OS_STRING(m_pFilename), 
					NCS_FILE_READ_WRITE|NCS_FILE_CREATE, 
					&m_pECWCompressionTask->OutputHandle);
				if( Error != NCS_SUCCESS ) {
					return(Error);
				}
				/*
				**	Set up the compression QMF structure
				*/
				NCSecwConfig(TRUE,FALSE);			// Don't want any input compressed files with texture dither
				
				// Set up the QMF tree for compression. It will return the number
				// of levels, and a full tree set up. Level 0 will be the smallest level;
				// level "num_levels" will be the fake file level.
				
				Error = setup_qmf_tree(&(m_pECWCompressionTask->pQmf), 
					m_pECWCompressionTask, 
					nBlockSizeX, nBlockSizeY,
					&num_levels,
					m_nWidth, m_nHeight, m_nNumberOfBands,
					m_nNumberOfBands, COMPRESS_HINT_INTERNET,
					(IEEE4)m_dTargetCompressionRate, (CompressFormat)m_pECWCompressionTask->eInternalFormat, TRUE);
				
				if( !m_pECWCompressionTask->pQmf || Error != NCS_SUCCESS ) {
					if(Error != NCS_SUCCESS) {
						return(Error);
					} else {
						return(NCS_COULDNT_OPEN_COMPRESSION);
					}
				}
				/*
				** Set up a FileInfo structure so the header logic can write datum/projection etc
				*/
				m_pECWCompressionTask->pQmf->pFileInfo = (ECWFileInfoEx *) NCSMalloc( sizeof( ECWFileInfoEx ), TRUE);
				if(!m_pECWCompressionTask->pQmf->pFileInfo) {
					return NCS_COULDNT_ALLOC_MEMORY;
				}
				m_pECWCompressionTask->pQmf->pFileInfo->nCompressionRate = (UINT16) m_dTargetCompressionRate;
				m_pECWCompressionTask->pQmf->pFileInfo->nSizeX = m_nWidth;
				m_pECWCompressionTask->pQmf->pFileInfo->nSizeY = m_nHeight;
				m_pECWCompressionTask->pQmf->pFileInfo->nBands = m_nNumberOfBands;
				m_pECWCompressionTask->pQmf->pFileInfo->eCellSizeUnits = m_CellSizeUnits;
				m_pECWCompressionTask->pQmf->pFileInfo->fCellIncrementX = m_dCellIncrementX;
				m_pECWCompressionTask->pQmf->pFileInfo->fCellIncrementY = m_dCellIncrementY;
				m_pECWCompressionTask->pQmf->pFileInfo->fOriginX = m_dOriginX;
				m_pECWCompressionTask->pQmf->pFileInfo->fOriginY = m_dOriginY;
				m_pECWCompressionTask->pQmf->pFileInfo->eColorSpace = m_eColorSpace;
				m_pECWCompressionTask->pQmf->pFileInfo->szDatum = NCSStrDup(m_pDatum);
				if(!m_pECWCompressionTask->pQmf->pFileInfo->szDatum) {
					return NCS_COULDNT_ALLOC_MEMORY;
				}
				m_pECWCompressionTask->pQmf->pFileInfo->szProjection = NCSStrDup(m_pProjection);
				if(!m_pECWCompressionTask->pQmf->pFileInfo->szProjection) {
					return NCS_COULDNT_ALLOC_MEMORY;
				}
				if(strcmp(m_pECWCompressionTask->pQmf->pFileInfo->szProjection,"GEODETIC") == 0) {
					m_pECWCompressionTask->pQmf->pFileInfo->eCellSizeUnits = ECW_CELL_UNITS_DEGREES;
				}
				m_bLowMemCompress = m_pECWCompressionTask->pQmf->p_file_qmf->bLowMemCompress ? true : false;
				
				if(m_bCompressMTRead && NCSThreadSpawn(&(m_pECWCompressionTask->tThread), 
					CNCSJP2FileView::sECWCompressThreadReadFunc, 
					(void*)m_pECWCompressionTask, TRUE)) {
					/* Do buffer alloc in 2 parts to better organise memory */
					INT32 e;
					
					m_pECWCompressionTask->pQueuePool = NCSPoolCreate(sizeof(EcwCompressionQueueNode), m_pECWCompressionTask->pQmf->p_file_qmf->bLowMemCompress ? 2 : MAX_THREAD_LINES);
					m_pECWCompressionTask->pReadQueue = NCSQueueCreate(m_pECWCompressionTask->pQueuePool, sizeof(NCSQueue), sizeof(EcwCompressionQueueNode));
					m_pECWCompressionTask->pFreeQueue = NCSQueueCreate(m_pECWCompressionTask->pQueuePool, sizeof(NCSQueue), sizeof(EcwCompressionQueueNode));
					
					for(e = 0; e < (m_pECWCompressionTask->pQmf->p_file_qmf->bLowMemCompress ? 2 : MAX_THREAD_LINES); e++) {
						EcwCompressionQueueNode *pNode = (EcwCompressionQueueNode*)NCSQueueCreateNode(m_pECWCompressionTask->pFreeQueue);
						
						pNode->ppLines = (IEEE4**)NCSMalloc(m_nNumberOfBands * sizeof(IEEE4*), FALSE);
						if(!pNode->ppLines) {
							return NCS_COULDNT_ALLOC_MEMORY;
						}
						
						for(i = 0; i < m_nNumberOfBands; i++) {
							pNode->ppLines[i] = (IEEE4*)NCSMalloc(m_nWidth * sizeof(IEEE4), FALSE);
							if(!pNode->ppLines[i]) {
								return NCS_COULDNT_ALLOC_MEMORY;
							}
						}
						NCSQueueAppendNode(m_pECWCompressionTask->pFreeQueue, (NCSQueueNode*)pNode);
					}
				} else {
				/*
				** Alloc temp buffer for conversion
					*/
					if(m_pECWCompressionTask->eInternalFormat != (CompressFormat)m_eColorSpace ||
						m_eCellType != NCSCT_IEEE4) {
						m_pECWCompressionTask->ppLines = (IEEE4**)NCSMalloc(m_nNumberOfBands * sizeof(IEEE4*), FALSE);
						if(!m_pECWCompressionTask->ppLines) {
							return NCS_COULDNT_ALLOC_MEMORY;
						}
						for(i = 0; i < m_nNumberOfBands; i++) {
							m_pECWCompressionTask->ppLines[i] = (IEEE4*)NCSMalloc(m_nWidth * sizeof(IEEE4), FALSE);
							if(!m_pECWCompressionTask->ppLines[i]) {
								return NCS_COULDNT_ALLOC_MEMORY;
							}
						}
					}
					m_bCompressMTRead = FALSE;
				}
				m_pECWCompressionTask->pReadCallback = sECWCompressRead;
				m_pECWCompressionTask->pStatusCallback = sECWCompressStatus;
				m_pECWCompressionTask->pCancelCallback = sECWCompressCancel;
				
				m_bIsOpen = true;
				m_bIsWrite = true;
				Error = NCS_SUCCESS; 
			} else {
				return(NCS_COULDNT_ALLOC_MEMORY);
			}
		} else
#endif //ifdef NCSJPC_ECW_SUPPORT
		{
			// JPEG2000
			if( m_nWidth < 2 || m_nHeight < 2 ) {
				Error = NCS_INPUT_SIZE_TOO_SMALL;
			} else {
				m_pFile = new CNCSJP2File;
				
				SetFileParameters();
				
				if(m_bCompressMTRead) {
					m_bCompressMTRead = Spawn(NULL, true);
				}
				//		m_bLowMemCompress = m_pFile->m_Codestream.m_bLowMemCompression;
				if(m_bCompressMTRead) {
					m_CompressFreeQueue.resize(m_bLowMemCompress ? 2 : MAX_THREAD_LINES);
				} else {
					m_CompressFreeQueue.resize(1);
				}
				for(UINT32 i = 0; i < (UINT32)m_CompressFreeQueue.size(); i++) {
					m_CompressFreeQueue[i] = new CNCSJPCBufferVector();
					m_CompressFreeQueue[i]->resize((m_eColorSpace == NCSCS_YUV) ? 3 : m_nNumberOfBands);
				}
				if(pURLPath) {
					Error = m_pFile->Open(pURLPath, true);
				} else {
					Error = m_pFile->Open(*pStream);
				}
				if(Error == NCS_SUCCESS) {
					NCSFileViewFileInfoEx Info;
					Info.nSizeX = m_nWidth;
					Info.nSizeY = m_nHeight;
					Info.nBands = m_nNumberOfBands;	
					Info.nCompressionRate = (UINT16)m_dCompressionRate;	
					Info.fCellIncrementX = m_dCellIncrementX;
					Info.fCellIncrementY = m_dCellIncrementY;
					Info.fOriginX = m_dOriginX;
					Info.fOriginY = m_dOriginY;
					Info.szDatum = NCSStrDup(m_pDatum);
					Info.szProjection = NCSStrDup(m_pProjection);
					Info.eCellSizeUnits = m_CellSizeUnits;
					Info.eCellType = m_eCellType;
					Info.fCWRotationDegrees = m_dCWRotationDegrees; //[02]
					Info.eColorSpace = m_eColorSpace;
					if(Info.eColorSpace == NCSCS_sRGB) {
						// YUV enables MCT
						//FIXME - no MCT if not 3+ bands, or bands not same dims/bits/sign
						Info.eColorSpace = NCSCS_YUV;
					}
					if(Info.eColorSpace == NCSCS_YUV) {
						m_pOutputNode = new CNCSJPCMCTNode;
					} else {
						m_pOutputNode = m_pFile;
					}
					Info.pBands = (NCSFileBandInfo*)NCSMalloc(Info.nBands * sizeof(NCSFileBandInfo), TRUE);
					for(UINT32 b = 0; b < Info.nBands; b++) {
						Info.pBands[b].bSigned = m_Bands[b].bSigned;
						Info.pBands[b].nBits = m_Bands[b].nBits;
						Info.pBands[b].szDesc = NCSStrDup(m_Bands[b].szDesc);
						
						if(m_pOutputNode && Info.eColorSpace == NCSCS_YUV) {
							m_pOutputNode->AddInput(this, m_pFile);
						}
					}
					for(UINT32 i = 0; i < m_OtherBoxes.size(); i++) {
						m_pFile->m_OtherBoxes.push_back(m_OtherBoxes[i]);
					}
					m_pFile->SetCompress(Info);
					NCSFreeFileInfoEx(&Info);
					
					m_bIsWrite = true;
					m_bIsOpen = true;
				} else {
					delete m_pFile;
					m_pFile = NULL;
				}
			}
		}
		if(m_bIsOpen) {
			NCSecwConfig(TRUE,FALSE);
		}
	}
	m_nNextLine = 0;
	return(Error);
}

// Close the JP2 file
CNCSError CNCSJP2FileView::Close(bool bFreeCache)
{
	CNCSError Error;
	CNCSJPCGlobalLock _Lock;
	
	if(m_bIsOpen) {
#ifdef NCSJPC_ECW_SUPPORT
		if(m_pECWCompressionTask) {
			EcwCompressionQueueNode *pNode;
			INT32 i;
			
			if(m_pECWCompressionTask->OutputHandle != NCS_NULL_FILE_HANDLE) {
				if(m_pECWCompressionTask->pQmf) {
					//			pClient->nOutputSize = (UINT64)NCSFileTellNative(m_pECWCompressionTask->OutputHandle);
				}
				/*
				**	Close the file as soon as we can, so not left open/unflushed pending user prompts etc
				*/
				NCSFileClose(m_pECWCompressionTask->OutputHandle);
				m_pECWCompressionTask->OutputHandle = NCS_NULL_FILE_HANDLE;
			}
			
			/*
			**	Everything went well, so we now create the ERS header file for the ERW file
			*/
			
			m_dCompressionRate = m_dCompressionRate;
			if(m_pECWCompressionTask->pQmf) {
				m_pECWCompressionTask->pQmf->compression_factor = (IEEE4)m_dCompressionRate;
			}
			
			if( m_pECWCompressionTask->pQmf )
				delete_qmf_levels(m_pECWCompressionTask->pQmf);
			m_pECWCompressionTask->pQmf = NULL;
			
			m_pECWCompressionTask->bKillThread = TRUE;
			
			if(m_pECWCompressionTask->pReadQueue) {
				/* Implicitly threaded */
				NCSThreadResume(&(m_pECWCompressionTask->tThread));
				
				while(NCSThreadIsRunning(&(m_pECWCompressionTask->tThread))) {
					m_CompressFreeEvent.Set();
					NCSSleep(50);
				}
			}
			if(m_pECWCompressionTask->pReadQueue) {
				while((pNode = (EcwCompressionQueueNode*)NCSQueueRemoveNode(m_pECWCompressionTask->pReadQueue, (NCSQueueNode*)NULL)) != NULL) {
					for(i = 0; i < m_nNumberOfBands; i++) {
						NCSFree(pNode->ppLines[i]);
					}
					NCSFree(pNode->ppLines);
					NCSQueueDestroyNode(m_pECWCompressionTask->pReadQueue, (NCSQueueNode*)pNode);
				}
				NCSQueueDestroy(m_pECWCompressionTask->pReadQueue);
			}
			if(m_pECWCompressionTask->pFreeQueue) {
				while((pNode = (EcwCompressionQueueNode*)NCSQueueRemoveNode(m_pECWCompressionTask->pFreeQueue, (NCSQueueNode*)NULL)) != NULL) {
					for(i = 0; i < m_nNumberOfBands; i++) {
						NCSFree(pNode->ppLines[i]);
					}
					NCSFree(pNode->ppLines);
					NCSQueueDestroyNode(m_pECWCompressionTask->pFreeQueue, (NCSQueueNode*)pNode);
				}
				NCSQueueDestroy(m_pECWCompressionTask->pFreeQueue);
			}
			if(m_pECWCompressionTask->pQueuePool) {
				NCSPoolDestroy(m_pECWCompressionTask->pQueuePool);
			}
			if(m_pECWCompressionTask->ppLines) {
				for(i = 0; i < m_nNumberOfBands; i++) {
					NCSFree(m_pECWCompressionTask->ppLines[i]);
				}
				NCSFree(m_pECWCompressionTask->ppLines);
			}
			NCSFree(m_pECWCompressionTask);
			m_pECWCompressionTask = (EcwCompressionTask*)NULL;
			
			Error = NCS_SUCCESS;
		}
		if(m_pECWFileView) {
			Error = NCScbmCloseFileViewEx_ECW(m_pECWFileView, bFreeCache);
			m_pECWFileView = NULL;
		}
#endif // NCSJPC_ECW_SUPPORT
		
		if (m_pTiledView)
		{
			m_pTiledView->Close(bFreeCache);
			delete m_pTiledView;
			m_pTiledView = NULL; //[07]
		}
		
		if(m_pFile) {
			if(m_pResampler) {
				m_pResampler->UnLink(this, m_pFile->m_FileInfo.nBands);
			}
			if(m_bHaveValidSetView) {
				for(UINT32 b = 0; b < m_pFile->m_ChannelComponentIndex.size(); b++) {
					for(INT32 t = 0; t < m_pFile->m_Codestream.GetNumXTiles() * m_pFile->m_Codestream.GetNumYTiles(); t++) {
						CNCSJPCTilePartHeader *pMainTP = m_pFile->m_Codestream.GetTile(t);
						if(pMainTP) {
							UINT32 nDummy;
							pMainTP->m_Components[m_pFile->m_ChannelComponentIndex[b]]->TraversePyramid(CNCSJPCResample::CalculateResolutionLevel(pMainTP->m_Components[m_pFile->m_ChannelComponentIndex[b]],
								m_CurrentView.nTopX, m_CurrentView.nLeftY,
								m_CurrentView.nBottomX, m_CurrentView.nRightY,
								m_CurrentView.nSizeX, m_CurrentView.nSizeY),
								CNCSJPCComponent::UNREF|
								CNCSJPCComponent::CANCEL, 
								m_CurrentView.nTopX, m_CurrentView.nLeftY, 
								m_CurrentView.nBottomX, m_CurrentView.nRightY, 
								m_CurrentView.nSizeX, m_CurrentView.nSizeY,
								nDummy, nDummy);
						}
					}
				}
			}
			m_pFile->Close(bFreeCache);
			if(m_pFile->m_pStream == NULL) {
				delete m_pFile;
			}			
			if(m_pOutputNode != m_pFile) {
				delete m_pOutputNode;
			}
			m_pOutputNode = NULL;
			if( m_pResampler ) {
				delete m_pResampler;
				m_pResampler = NULL;
			}
			m_pFile = NULL;
		}
		UINT32 i;
		for(i = 0; i < m_CompressFreeQueue.size(); i++) {
			delete m_CompressFreeQueue[i];
		}
		m_CompressFreeQueue.clear();
		for(i = 0; i < m_CompressReadQueue.size(); i++) {
			delete m_CompressReadQueue[i];
		}
		m_CompressReadQueue.clear();
		if(m_bIsWrite) {	
			/* end if no error */
			NCSecwConfig(FALSE,FALSE);			// Revert to normal processing
		}
		/** re-Initialise m_CurrentView */
		NCSFree(m_CurrentView.pBandList);
		memset(&m_CurrentView, 0, sizeof(m_CurrentView));
		
		m_bHaveValidSetView = false;
		m_bIsProgressive = false;
		m_bPendingView = false;
		NCSFree(m_PendingView.pBandList);
		memset(&m_PendingView, 0, sizeof(m_PendingView));
		
		UpdateFileViewInfo();
		DeleteDynamicNodes();
		m_InputBuf.Free();
		m_bIsOpen = false;
		m_bIsWrite = false;
		
		m_nWidth = 0;
		m_nHeight = 0;
		m_nNumberOfBands = 0;
		m_dCompressionRate = 10;
		m_dTargetCompressionRate = 10;
		m_dCellIncrementX = 1.0;
		m_dCellIncrementY = 1.0;
		m_dOriginX = 0.0;
		m_dOriginY = 0.0;
		m_CellSizeUnits = ECW_CELL_UNITS_METERS;
		
		m_eColorSpace = NCSCS_sRGB;
		m_eCellType = NCSCT_UINT8;
	}
	return(Error);
}

CNCSError CNCSJP2FileView::SetTiledView(NCSFileViewSetInfo &View,
										UINT32 nReadLine)
{
	CNCSError Error;
	
	//UINT32 nViews = 1;//View.nSizeX / NCSECW_MAX_VIEW_SIZE_TO_CACHE + ((View.nSizeX % NCSECW_MAX_VIEW_SIZE_TO_CACHE) ? 1 : 0);
	
//	if (m_pTiledView) 
//	{
//		m_pTiledView->Close(false);
//		delete m_pTiledView; //[07]
//		m_pTiledView = NULL;
//	}
	double dScaleX = (View.nBottomX - View.nTopX + 1) / (double)View.nSizeX;
	double dScaleY = (View.nRightY - View.nLeftY + 1) / (double)View.nSizeY;
	double dWorldScaleX = (View.fBottomX - View.fTopX + 1) / (double)View.nSizeX;
	double dWorldScaleY = (View.fRightY - View.fLeftY + 1) / (double)View.nSizeY;
	
	UINT32 u = nReadLine / NCSJP2_STRIP_HEIGHT;
	UINT32 nWidth = View.nSizeX;
	UINT32 nHeight = View.nSizeY;
	
	
	if(!m_pTiledView) {
		m_pTiledView = new CNCSJP2FileView();
		if(m_pECWFileView) {	/**[03]**/
			Error = m_pTiledView->Open(m_pECWFileView->pNCSFile->szUrlPath,
				m_bIsProgressive,
				false);
		} else {
			Error = m_pTiledView->Open((NCSTChar *)m_pFile->m_pStream->GetName(),
				m_bIsProgressive,
				false);
		}
		if (m_bIsProgressive && Error == NCS_SUCCESS)
		{
			m_pTiledView->SetRefreshCallback(m_pRefreshCallback);
		}
	}
	UINT32 nThisViewWidth = View.nSizeX;//NCSMin(NCSECW_MAX_VIEW_SIZE_TO_CACHE, View.nSizeX - v * NCSECW_MAX_VIEW_SIZE_TO_CACHE);
	UINT32 nThisViewHeight = NCSMin(NCSJP2_STRIP_HEIGHT, View.nSizeY - nReadLine);
	Error = m_pTiledView->SetView(View.nBands, View.pBandList,
		View.nTopX,// + dScaleX * v * NCSECW_MAX_VIEW_SIZE_TO_CACHE,
		(UINT32)(View.nLeftY + dScaleY * u * NCSJP2_STRIP_HEIGHT),
		View.nBottomX,//View.nTopX + dScaleX * v * NCSECW_MAX_VIEW_SIZE_TO_CACHE + dScaleX * (nThisViewWidth - 1), 
		(UINT32)(View.nLeftY + dScaleY * u * NCSJP2_STRIP_HEIGHT + dScaleY * (nThisViewHeight - 1)),
		nThisViewWidth, nThisViewHeight, 
		View.fTopX,// + dWorldScaleX * v * NCSECW_MAX_VIEW_SIZE_TO_CACHE, 
		View.fLeftY + dWorldScaleY * u,
		View.fBottomX,//View.fTopX + dWorldScaleX * v * NCSECW_MAX_VIEW_SIZE_TO_CACHE + dWorldScaleX * (nThisViewWidth - 1), 
		View.fLeftY + dWorldScaleY * u * NCSJP2_STRIP_HEIGHT + dWorldScaleY * (nThisViewHeight - 1));
	return(Error);
}

// Set a view into the JP2 file for reading.
CNCSError CNCSJP2FileView::SetView(UINT32 nBands, UINT32 *pBandList, 
								   UINT32 nDatasetTLX, UINT32 nDatasetTLY,
								   UINT32 nDatasetBRX, UINT32 nDatasetBRY,
								   UINT32 nWidth, UINT32 nHeight,
								   IEEE8 dWorldTLX, IEEE8 dWorldTLY,
								   IEEE8 dWorldBRX, IEEE8 dWorldBRY)
{
	UINT32 b;
	CNCSJPCGlobalLock _Lock;
	const NCSFileViewFileInfoEx *pInfo = GetFileInfo();
	
	// Validate the view dimensions
	if(nBands == 0 || nBands > pInfo->nBands) {
		m_bHaveValidSetView = false;
		return(NCS_TOO_MANY_BANDS);
	}
	
	// Make sure the view isn't too large in progressive mode
	
	UINT32 nMaxProgressiveSize = 0;
	NCSecwGetConfig(NCSCFG_MAX_PROGRESSIVE_VIEW_SIZE, &nMaxProgressiveSize);
	
	if(((nWidth > nMaxProgressiveSize && nHeight > NCSJP2_STRIP_HEIGHT) || nHeight > nMaxProgressiveSize)
		&& m_bIsProgressive) {/**[03]**/
		return(NCS_PROGRESSIVE_VIEW_TOO_LARGE);
	}
	
	if(!pBandList) {
		m_bHaveValidSetView = false;
		return(NCS_INVALID_ARGUMENTS);
	}
	// Validate the band list
	for(b = 0; b < nBands; b++) {
		if(pBandList[b] >= pInfo->nBands) {
			m_bHaveValidSetView = false;
			return(NCS_INVALID_BAND_NR);
		}
	}
	if(nWidth == 0 || nHeight == 0) {
		m_bHaveValidSetView = false;
		return(NCS_ZERO_SIZE);
	}
	if(nDatasetBRX < nDatasetTLX || nDatasetBRY < nDatasetTLY) {
		m_bHaveValidSetView = false;
		return(NCS_FILE_INVALID_SETVIEW);	
	}
	if(nDatasetTLX < 0 || nDatasetTLX >= pInfo->nSizeX ||
		nDatasetTLY < 0 || nDatasetTLY >= pInfo->nSizeY || 
		nDatasetBRX < 0 || nDatasetBRX >= pInfo->nSizeX ||
		nDatasetBRY < 0 || nDatasetBRY >= pInfo->nSizeY) {
		m_bHaveValidSetView = false;
		return(NCS_REGION_OUTSIDE_FILE);
	}
	
	if(m_bInRefreshUpdate && m_pFile) {
		m_PendingView.nBands = nBands;
		NCSFree(m_PendingView.pBandList);
		m_PendingView.pBandList = (UINT32*)NCSMalloc(sizeof(INT32) * nBands, false);
		for(b = 0; b < nBands; b++) {
			m_PendingView.pBandList[b] = pBandList[b];
		}
		
		m_PendingView.nTopX = nDatasetTLX;
		m_PendingView.nLeftY = nDatasetTLY;
		m_PendingView.nBottomX = nDatasetBRX;
		m_PendingView.nRightY = nDatasetBRY;
		m_PendingView.nSizeX = nWidth;
		m_PendingView.nSizeY = nHeight;
		
		m_PendingView.fTopX = dWorldTLX;
		m_PendingView.fLeftY = dWorldTLY;
		m_PendingView.fBottomX = dWorldBRX;
		m_PendingView.fRightY = dWorldBRY;
		
		m_bPendingView = true;
		//	char buf[1024];
		//	sprintf(buf, "SP: %ld,%ld %ld,%ld (%ldx%ld)\r\n", m_PendingView.nTopX, m_PendingView.nLeftY, m_PendingView.nBottomX, m_PendingView.nRightY, m_PendingView.nSizeX, m_PendingView.nSizeY);
		//	OutputDebugStringA(buf);
		
		//m_bCancelRead = true;
		return(NCS_SUCCESS);
	}
	INT32 nOldTopX = m_CurrentView.nTopX;
	INT32 nOldLeftY = m_CurrentView.nLeftY;
	INT32 nOldBottomX = m_CurrentView.nBottomX;
	INT32 nOldRightY = m_CurrentView.nRightY;
	UINT32 nOldWidth = m_CurrentView.nSizeX;
	UINT32 nOldHeight = m_CurrentView.nSizeY;
	std::vector<UINT16> OldComponents;
	if(m_pFile) {
		OldComponents = m_pFile->m_ChannelComponentIndex;
	}
	
	m_CurrentView.nBands = nBands;
	NCSFree(m_CurrentView.pBandList);
	m_CurrentView.pBandList = (UINT32*)NCSMalloc(sizeof(INT32) * nBands, false);
	for(b = 0; b < nBands; b++) {
		m_CurrentView.pBandList[b] = pBandList[b];
	}
	
	m_CurrentView.nTopX = nDatasetTLX;
	m_CurrentView.nLeftY = nDatasetTLY;
	m_CurrentView.nBottomX = nDatasetBRX;
	m_CurrentView.nRightY = nDatasetBRY;
	m_CurrentView.nSizeX = nWidth;
	m_CurrentView.nSizeY = nHeight;
	
	m_CurrentView.nBlocksInView = 0;
	m_CurrentView.nBlocksAvailable = 0;
	m_CurrentView.nBlocksAvailableAtSetView = 0;
	m_CurrentView.nMissedBlocksDuringRead = 0;
	
	m_CurrentView.fTopX = dWorldTLX;
	m_CurrentView.fLeftY = dWorldTLY;
	m_CurrentView.fBottomX = dWorldBRX;
	m_CurrentView.fRightY = dWorldBRY;
	
#ifdef NCSJPC_ECW_SUPPORT
	if(m_pECWFileView) {
		CNCSError Error;
		m_nNextLine = 0; // [04]
		
		if((nWidth > nMaxProgressiveSize && nHeight > NCSJP2_STRIP_HEIGHT) || nHeight > nMaxProgressiveSize) {/**[03]**/
			m_bTiledView = true;
			Error = SetTiledView(m_CurrentView, 0);
		} else {
			m_bTiledView = false; // [05]
			Error = NCScbmSetFileViewEx_ECW(m_pECWFileView,
				nBands, pBandList, 
				nDatasetTLX, nDatasetTLY, 
				nDatasetBRX, nDatasetBRY, 
				nWidth, nHeight, 
				dWorldTLX, dWorldTLY, 
				dWorldBRX, dWorldBRY);			
		}
		if(Error == NCS_SUCCESS) {
			m_bHaveValidSetView = true;
		} else {
			m_bHaveValidSetView = false;
		}
		UpdateFileViewInfo();
		return(Error);
	}
#endif
	if(m_pFile) {
		CNCSError Error;
		
		//	char buf[1024];
		//	sprintf(buf, "SV: %ld,%ld %ld,%ld (%ldx%ld)\r\n", m_CurrentView.nTopX, m_CurrentView.nLeftY, m_CurrentView.nBottomX, m_CurrentView.nRightY, m_PendingView.nSizeX, m_CurrentView.nSizeY);
		//	OutputDebugStringA(buf);
		
		m_nNextLine = 0;
		bool bPrevViewValid = m_bHaveValidSetView;
		m_bHaveValidSetView = true;
		
		UpdateFileViewInfo();
		m_pResampler->UnLink(this, m_pFile->m_FileInfo.nBands);
		DeleteDynamicNodes();
		
		m_tsLastBlockTime = NCSGetTimeStampMs();
		
		if((nWidth > nMaxProgressiveSize && nHeight > NCSJP2_STRIP_HEIGHT) || nHeight > nMaxProgressiveSize) {
			m_bTiledView = true;
			Error = SetTiledView(m_CurrentView, 0);
		} else {
			m_bTiledView = false; // [05]
			// Setup the components, link the view subset in.
			for(UINT32 c = 0; c < m_pFile->m_FileInfo.nBands; c++) {
				m_pResampler->Link(this, 
					m_pFile->m_Codestream, 
					this, 
					m_pFile->m_ChannelComponentIndex[c], 
					c, 
					m_pFile->m_Header);
			}
		}
		
		for(INT32 t = 0; t < m_pFile->m_Codestream.m_SIZ.GetNumXTiles() * m_pFile->m_Codestream.m_SIZ.GetNumYTiles(); t++) {
			CNCSJPCTilePartHeader *pMainTP = m_pFile->m_Codestream.GetTile(t);
			
			if(pMainTP) {
				if(nOldWidth != 0 && nOldHeight != 0 && bPrevViewValid) {
					for(b = 0; b < OldComponents.size(); b++) {
						UINT32 nDummy;
						pMainTP->m_Components[OldComponents[b]]->TraversePyramid(CNCSJPCResample::CalculateResolutionLevel(pMainTP->m_Components[OldComponents[b]],
							nOldTopX, nOldLeftY,
							nOldBottomX, nOldRightY,
							nOldWidth, nOldHeight),
							CNCSJPCComponent::UNREF|
							CNCSJPCComponent::CANCEL, 
							nOldTopX, nOldLeftY, 
							nOldBottomX, nOldRightY, 
							nOldWidth, nOldHeight,
							nDummy, nDummy);
					}
				}
				
				if(m_pFile->m_Codestream.m_pStream->Seek() && (pMainTP->m_PLTs.size() != 0 || m_pFile->m_pStream->IsPacketStream())) {
					bool bComplete = false;
					pMainTP->Start(pMainTP);
					//CNCSJPCPacketList Packets;
					while(!bComplete) {
						pMainTP->Iterate(pMainTP, NULL, bComplete);
					}
				}
			}
		}
		
		if(m_pFile->m_pStream->IsPacketStream()) {
			Error = ((CNCSJPCEcwpIOStream*)m_pFile->m_pStream)->SendRequests();
		}
		m_nBlocksAtLastRefreshUpdate = 0;
		m_tsLastRefreshUpdate = NCSGetTimeStampMs();
		//		if(!m_bInRefreshUpdate && m_bIsProgressive) {
		// Progressive update, but setview not called from refreshupdate(), cancel next read.
		//			m_bCancelRead = true;
		//		}
		UpdateViewStats();
		sm_Views.m_Event.Set();
		return(Error);
	}
	return(NCS_FILE_NOT_OPEN);	
}

// Read the next line in BIL format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineBIL(UINT8 **ppOutputLine)
{
	return(ReadLineBIL(NCSCT_UINT8, m_CurrentView.nBands, (void**)ppOutputLine));
}
// Read the next line in BIL format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineBIL(UINT16 **ppOutputLine)
{
	return(ReadLineBIL(NCSCT_UINT16, m_CurrentView.nBands, (void**)ppOutputLine));
}
// Read the next line in BIL format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineBIL(UINT32 **ppOutputLine)
{
	return(ReadLineBIL(NCSCT_UINT32, m_CurrentView.nBands, (void**)ppOutputLine));
}
// Read the next line in BIL format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineBIL(UINT64 **ppOutputLine)
{
	return(ReadLineBIL(NCSCT_UINT64, m_CurrentView.nBands, (void**)ppOutputLine));
}
// Read the next line in BIL format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineBIL(INT8 **ppOutputLine)
{
	return(ReadLineBIL(NCSCT_INT8, m_CurrentView.nBands, (void**)ppOutputLine));
}
// Read the next line in BIL format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineBIL(INT16 **ppOutputLine)
{
	return(ReadLineBIL(NCSCT_INT16, m_CurrentView.nBands, (void**)ppOutputLine));
}
// Read the next line in BIL format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineBIL(INT32 **ppOutputLine)
{
	return(ReadLineBIL(NCSCT_INT32, m_CurrentView.nBands, (void**)ppOutputLine));
}
// Read the next line in BIL format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineBIL(INT64 **ppOutputLine)
{
	return(ReadLineBIL(NCSCT_INT64, m_CurrentView.nBands, (void**)ppOutputLine));
}
// Read the next line in BIL format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineBIL(IEEE4 **ppOutputLine)
{
	return(ReadLineBIL(NCSCT_IEEE4, m_CurrentView.nBands, (void**)ppOutputLine));
}
// Read the next line in BIL format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineBIL(IEEE8 **ppOutputLine)
{
	return(ReadLineBIL(NCSCT_IEEE8, m_CurrentView.nBands, (void**)ppOutputLine));
}

// Read the next line in BIL format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineBIL(NCSEcwCellType eType, UINT16 nBands, void **ppOutputLine, UINT32 *pLineSteps)
{
	NCSEcwReadStatus eRet = NCSECW_READ_FAILED;
	CNCSJPCGlobalLock _Lock(false);
	
	_Lock.Lock();
	
	if(m_nNextLine >= m_CurrentView.nSizeY) {
		_Lock.UnLock();
		return(eRet);
	}
	if(m_bTiledView) {
		CNCSError Error;
		if(m_nNextLine && (m_nNextLine % NCSJP2_STRIP_HEIGHT) == 0) {
			Error = SetTiledView(m_CurrentView, m_nNextLine);
		}
		void **ppBands = (void**)NCSMalloc(sizeof(void*) * nBands, TRUE);
		if(ppBands) {
			for(UINT16 b = 0; b < nBands; b++) {
				switch(eType) {
				case NCSCT_INT8:
				case NCSCT_UINT8:
					ppBands[b] = (UINT8*)ppOutputLine[b];// + v * NCSECW_MAX_VIEW_SIZE_TO_CACHE * (pLineSteps ? pLineSteps[b] : 1);
					break;
				case NCSCT_INT16:
				case NCSCT_UINT16:
					ppBands[b] = (UINT16*)ppOutputLine[b];// + v * NCSECW_MAX_VIEW_SIZE_TO_CACHE * (pLineSteps ? pLineSteps[b] : 1);
					break;
				case NCSCT_INT32:
				case NCSCT_UINT32:
				case NCSCT_IEEE4:
					ppBands[b] = (UINT32*)ppOutputLine[b];// + v * NCSECW_MAX_VIEW_SIZE_TO_CACHE * (pLineSteps ? pLineSteps[b] : 1);
					break;
				case NCSCT_INT64:
				case NCSCT_UINT64:
				case NCSCT_IEEE8:
					ppBands[b] = (UINT64*)ppOutputLine[b];// + v * NCSECW_MAX_VIEW_SIZE_TO_CACHE * (pLineSteps ? pLineSteps[b] : 1);
					break;
				default: break;
				}
			}
			eRet = m_pTiledView->ReadLineBIL(eType, nBands, ppBands, pLineSteps);
		}
		NCSFree(ppBands);
		m_nNextLine++;
		_Lock.UnLock();
		return(eRet);
	}
#ifdef NCSJPC_ECW_SUPPORT
	if(m_pECWFileView) {
		if(pLineSteps) {
			_Lock.UnLock();
			return(NCSECW_READ_FAILED);
		}
		_Lock.UnLock();
		eRet = NCScbmReadViewLineBILEx_ECW(m_pECWFileView, eType, ppOutputLine);
		return(eRet);
	}
#endif // NCSJPC_READ_SUPPORT
	{
		if(m_pFile && m_bHaveValidSetView) {
			bool bRet = false;
			
			if(m_bCancelRead) {
				_Lock.UnLock();
				return(NCSECW_READ_CANCELLED);
			}
			CNCSJPCTilePartHeader *pTile0 = m_pFile->m_Codestream.GetTile(0);
			
			for(UINT32 c = 0; c < nBands; c++) {
				UINT32 iChannel = m_CurrentView.pBandList[NCSMin(c, m_CurrentView.nBands - 1)];
				
				CNCSJPCBuffer::Type eBufType = CNCSJPCBuffer::BT_INT16;
				
				if(pTile0->m_Components[m_pFile->m_ChannelComponentIndex[iChannel]]->m_CodingStyle.m_SPcod.m_eTransformation == CNCSJPCCodingStyleParameter::REVERSIBLE_5x3) {
					if(m_pFile->m_Codestream.m_SIZ.m_Components[m_pFile->m_ChannelComponentIndex[iChannel]].m_nBits > 14) {
						eBufType = CNCSJPCBuffer::BT_INT32;
					} else {
						eBufType = CNCSJPCBuffer::BT_INT16;
					}
				} else {
					eBufType = CNCSJPCBuffer::BT_IEEE4;
				}
				//			INT32 nStep = 0;
				//			void *pBuf = CNCSJPCNode::BufAlloc(eBufType, &nStep, m_CurrentView.nSizeX);
				m_InputBuf.Alloc(0, m_nNextLine, m_CurrentView.nSizeX, (UINT32)1, eBufType);
				
				if(m_InputBuf.GetPtr()) {
					if(m_pFile->m_pStream->IsPacketStream() && !m_bIsProgressive && m_CurrentView.nBlocksAvailable != m_CurrentView.nBlocksInView) {
						UpdateViewStats();
						while(m_CurrentView.nBlocksAvailable != m_CurrentView.nBlocksInView) {
							// Requested but not received yet
							NCSTimeStampMs	tNow  = NCSGetTimeStampMs();
							
							// Note: nBlockingTime == -1 is INDEFINITE delay.
							if( (tNow < m_tsLastBlockTime)	// ms timer wrapped
								|| ((pNCSEcwInfo->pStatistics->nBlockingTime != 0xffffffff) && (tNow > (m_tsLastBlockTime + pNCSEcwInfo->pStatistics->nBlockingTime)))) {
								break;			// Too much time has passed, so quit waiting
							}
							_Lock.UnLock();
							NCSSleep(500);			// Wait for a while to see if this (and possibly other) blocks turn up
							_Lock.Lock();
							UpdateViewStats();
						}
					}
					bRet = m_pResampler->ReadLine(this, &m_InputBuf, iChannel);
					if(bRet) {
						INT32 nAdd = 0;
						INT32 nShift = 0;
						UINT8 nBits = m_pFile->m_Codestream.m_SIZ.m_Components[m_pFile->m_ChannelComponentIndex[iChannel]].m_nBits;
						nShift = nBits;
						// No DC Shift if signed, so have to clip the buffer to the valid data range
						UINT64 nMax = (UINT64)(1 << (nBits - 1)) - 1;;
						bool bComponentSigned = (m_pFile->m_Codestream.m_SIZ.m_Components[m_pFile->m_ChannelComponentIndex[iChannel]].m_bSigned);
						bool bOutputSigned = false;

						switch (eType)
						{
						case NCSCT_INT8: 
						case NCSCT_INT16: 
						case NCSCT_INT32: 
						case NCSCT_INT64:
							bOutputSigned = true;
							// unsigned image, but asking for signed output
							// so lose 1 bit 
							if (!bComponentSigned) nShift += 1;
							break;
						case NCSCT_UINT8:
						case NCSCT_UINT16:
						case NCSCT_UINT32:
						case NCSCT_UINT64:
							// signed image but asking for unsigned output, offset by negative minimum
							if (bComponentSigned) nAdd -= (-1 * (signed)nMax - 1);
							break;
						default: break;
						}
						switch(eType) {
						case NCSCT_UINT8: 
						case NCSCT_INT8:
							nShift -= 8;
							break;
						case NCSCT_UINT16: 
						case NCSCT_INT16: 
							nShift -= 16;
							break;
						case NCSCT_UINT32: 
						case NCSCT_INT32: 
							nShift -= 32;
							break;
						case NCSCT_UINT64:
						case NCSCT_INT64:
							nShift -= 64;
							break;
						case NCSCT_IEEE4:
							nShift -= 23;
							break;
						case NCSCT_IEEE8:
							nShift -= 52;
							break;
						default: break;
						}
						if(m_InputBuf.GetType() == CNCSJPCBuffer::BT_IEEE4) {
							//m_InputBuf.OffsetShift(nAdd, nBits, nBits - (m_bAutoScaleUp ? nShift : NCSMax(0, nShift)));
							m_InputBuf.OffsetShift(nAdd, nBits, nBits - NCSMax(0, nShift));
						} else {
							m_InputBuf.Add(nAdd, NCSMax(0,nShift));
							if (m_bAutoScaleUp)
							{
								IEEE4 fScale = 0.0;
								switch (eType)
								{
								case NCSCT_UINT8:
								case NCSCT_INT8:
								case NCSCT_UINT16:
								case NCSCT_INT16:
									fScale = (IEEE4) (1 << (8 * NCSCellSize(eType) - (bOutputSigned?1:0))) - 1;
									fScale /= nMax;
									if (fScale > 1.0) m_InputBuf.Multiply((IEEE4)fScale);
									break;
								default:
									break;

								}
							}
						}
					
					} 
					if(bRet) {					
						bRet = false;
						switch(eType) {
						default:
						case NCSCT_UINT8: 
							bRet = m_InputBuf.Read((UINT8*)ppOutputLine[c], pLineSteps ? pLineSteps[c] : 1);
							break;
						case NCSCT_UINT16: 
							bRet = m_InputBuf.Read((UINT16*)ppOutputLine[c], pLineSteps ? pLineSteps[c] : 1);
							break;
						case NCSCT_UINT32: 
							bRet = m_InputBuf.Read((UINT32*)ppOutputLine[c], pLineSteps ? pLineSteps[c] : 1);
							break;
						case NCSCT_UINT64: 
							bRet = m_InputBuf.Read((UINT64*)ppOutputLine[c], pLineSteps ? pLineSteps[c] : 1);
							break;
						case NCSCT_INT8: 
							bRet = m_InputBuf.Read((INT8*)ppOutputLine[c], pLineSteps ? pLineSteps[c] : 1);
							break;
						case NCSCT_INT16: 
							bRet = m_InputBuf.Read((INT16*)ppOutputLine[c], pLineSteps ? pLineSteps[c] : 1);
							break;
						case NCSCT_INT32: 
							bRet = m_InputBuf.Read((INT32*)ppOutputLine[c], pLineSteps ? pLineSteps[c] : 1);
							break;
						case NCSCT_INT64: 
							bRet = m_InputBuf.Read((INT64*)ppOutputLine[c], pLineSteps ? pLineSteps[c] : 1);
							break;
						case NCSCT_IEEE4: 
							bRet = m_InputBuf.ReadFLT((IEEE4*)ppOutputLine[c], pLineSteps ? pLineSteps[c] : 1);
							break;
						case NCSCT_IEEE8: 
							bRet = m_InputBuf.ReadFLT((IEEE8*)ppOutputLine[c], pLineSteps ? pLineSteps[c] : 1);
							break;
						}
					}
					//CNCSJPCNode::BufFree(pBuf);
				} else {
					bRet = false;
				}
				if(!bRet) {
					break;
				}
			}
			m_nNextLine++;
			
			if(bRet) {
				eRet = NCSECW_READ_OK;
			}
			m_pFile->Purge();
		}
	}
	_Lock.UnLock();
	return(eRet);
}

// Read the next line in RGB UINT8 triplet format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineRGB(UINT8 *pRGBTriplet)
{
	NCSEcwReadStatus eRet;
#ifdef NCSJPC_ECW_SUPPORT
	if(m_pECWFileView) {
		if(m_bTiledView) {
			CNCSError Error;
			if(m_nNextLine && (m_nNextLine % NCSJP2_STRIP_HEIGHT) == 0) {
				Error = SetTiledView(m_CurrentView, m_nNextLine);
			}
			eRet = m_pTiledView->ReadLineRGB(pRGBTriplet /*+ v * NCSECW_MAX_VIEW_SIZE_TO_CACHE * 3*/);
			m_nNextLine++;
		} else {
			eRet = NCScbmReadViewLineRGB_ECW(m_pECWFileView, pRGBTriplet);
		}
		return(eRet);
	}
#endif // NCSJPC_ECW_SUPPORT
	{
		CNCSJPCGlobalLock _Lock;
		UINT32 nLineStep[3] = { 3, 3, 3 };
		void *pOutputLine[3] = { pRGBTriplet, pRGBTriplet + 1, pRGBTriplet + 2 };
		eRet = ReadLineBIL(NCSCT_UINT8, 3, pOutputLine, nLineStep);
	}
	return(eRet);
}

// Read the next line in BGR UINT8 triplet format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineBGR(UINT8 *pBGRTriplet)
{
	NCSEcwReadStatus eRet;
#ifdef NCSJPC_ECW_SUPPORT
	if(m_pECWFileView) {
		if(m_bTiledView) {
			CNCSError Error;
			if(m_nNextLine && (m_nNextLine % NCSJP2_STRIP_HEIGHT) == 0) {
				Error = SetTiledView(m_CurrentView, m_nNextLine);
			}
			eRet = m_pTiledView->ReadLineBGR(pBGRTriplet/* + v * NCSECW_MAX_VIEW_SIZE_TO_CACHE * 3*/);
			m_nNextLine++;
		} else {
			eRet = NCScbmReadViewLineBGR_ECW(m_pECWFileView, pBGRTriplet);
		}
		return(eRet);
	}
#endif // NCSJPC_ECW_SUPPORT
	{
		CNCSJPCGlobalLock _Lock;
		UINT32 nLineStep[3] = { 3, 3, 3 };
		void *pOutputLine[3] = { pBGRTriplet + 2, pBGRTriplet + 1, pBGRTriplet + 0 };
		eRet = ReadLineBIL(NCSCT_UINT8, 3, pOutputLine, nLineStep);
	}
	return(eRet);
}

// Read the next line in RGBA UINT32 format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineRGBA(UINT32 *pRGBA)
{
	NCSEcwReadStatus eRet;
#ifdef NCSJPC_ECW_SUPPORT
	if(m_pECWFileView) {
		if(m_bTiledView) {
			CNCSError Error;
			if(m_nNextLine && (m_nNextLine % NCSJP2_STRIP_HEIGHT) == 0) {
				Error = SetTiledView(m_CurrentView, m_nNextLine);
			}
			eRet = m_pTiledView->ReadLineRGBA(pRGBA/* + v * NCSECW_MAX_VIEW_SIZE_TO_CACHE*/);
			m_nNextLine++;
		} else {
			eRet = NCScbmReadViewLineRGBA_ECW(m_pECWFileView, pRGBA);
		}
		return(eRet);
	}
#endif // NCSJPC_ECW_SUPPORT
	{
		CNCSJPCGlobalLock _Lock;
		UINT32 nLineStep[4] = { 4, 4, 4, 4 };
		void *pOutputLine[4] = { ((UINT8*)pRGBA), ((UINT8*)pRGBA) + 1, ((UINT8*)pRGBA) + 2, ((UINT8*)pRGBA) + 3 };
		eRet = ReadLineBIL(NCSCT_UINT8, (m_CurrentView.nBands == 2 || m_CurrentView.nBands == 4) ? 4 : 3, pOutputLine, nLineStep);
	}
	return(eRet);
}

// Read the next line in BGRA UINT32 format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineBGRA(UINT32 *pBGRA)
{
	NCSEcwReadStatus eRet;
#ifdef NCSJPC_ECW_SUPPORT
	if(m_pECWFileView) {
		if(m_bTiledView) {
			CNCSError Error;
			if(m_nNextLine && (m_nNextLine % NCSJP2_STRIP_HEIGHT) == 0) {
				Error = SetTiledView(m_CurrentView, m_nNextLine);
			}
			eRet = m_pTiledView->ReadLineBGRA(pBGRA/* + v * NCSECW_MAX_VIEW_SIZE_TO_CACHE*/);
			m_nNextLine++;
		} else {
			eRet = NCScbmReadViewLineBGRA_ECW(m_pECWFileView, pBGRA);
		}
		return(eRet);
	}
#endif // NCSJPC_ECW_SUPPORT
	{
		CNCSJPCGlobalLock _Lock;
		UINT32 nLineStep[4] = { 4, 4, 4, 4 };
		void *pOutputLine[4] = { ((UINT8*)pBGRA) + 2, ((UINT8*)pBGRA) + 1, ((UINT8*)pBGRA), ((UINT8*)pBGRA) + 3 };
		eRet = ReadLineBIL(NCSCT_UINT8, (m_CurrentView.nBands == 2 || m_CurrentView.nBands == 4) ? 4 : 3, pOutputLine, nLineStep);
	}
	return(eRet);
}

// Read the next line in ARGB UINT32 format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineARGB(UINT32 *pARGB)
{
	NCSEcwReadStatus eRet;
#ifdef NCSJPC_ECW_SUPPORT
	if(m_pECWFileView) {
		eRet = NCSECW_READ_FAILED;
		return(eRet);
	}
#endif // NCSJPC_ECW_SUPPORT
	{
		CNCSJPCGlobalLock _Lock;
		UINT32 nLineStep[4] = { 4, 4, 4, 4 };
		void *pOutputLine[4] = { ((UINT8*)pARGB) + 1, ((UINT8*)pARGB) + 2, ((UINT8*)pARGB) + 3, ((UINT8*)pARGB) };
		eRet = ReadLineBIL(NCSCT_UINT8, (m_CurrentView.nBands == 2 || m_CurrentView.nBands == 4) ? 4 : 3, pOutputLine, nLineStep);
	}
	return(eRet);
}

// Read the next line in ABGR UINT32 format from the current view into the file.
NCSEcwReadStatus CNCSJP2FileView::ReadLineABGR(UINT32 *pABGR)
{
	NCSEcwReadStatus eRet;
#ifdef NCSJPC_ECW_SUPPORT
	if(m_pECWFileView) {
		eRet = NCSECW_READ_FAILED;
		return(eRet);
	}
#endif // NCSJPC_ECW_SUPPORT
	{
		CNCSJPCGlobalLock _Lock;
		UINT32 nLineStep[4] = { 4, 4, 4 };
		void *pOutputLine[4] = { ((UINT8*)pABGR) + 3, ((UINT8*)pABGR) + 2, ((UINT8*)pABGR) + 1, ((UINT8*)pABGR) };
		eRet = ReadLineBIL(NCSCT_UINT8, (m_CurrentView.nBands == 2 || m_CurrentView.nBands == 4) ? 4 : 3, pOutputLine, nLineStep);
	}
	return(eRet);
}

// Write the next line in BIL format into the file.
CNCSError CNCSJP2FileView::WriteLineBIL(NCSEcwCellType eType, 
										UINT16 nBands, 
										void **ppOutputLine, 
										UINT32 *pLineSteps)
{
	CNCSJPCGlobalLock _Lock;
	INT32 b;
	CNCSJPC &JPC = (CNCSJPC&)m_pFile->m_Codestream;
	CNCSJPCTilePartHeader *pTile = JPC.GetTile(0);
	CNCSJPCBuffer OutputBuffer;
	UINT32 x;
	bool bRet = true;
	
	for(b = 0; b < nBands; b++) {
		UINT32 nWidth = JPC.GetComponentWidth(b);
		INT32 nStep = pLineSteps ? pLineSteps[b] : 1;
		INT32 nSub = JPC.m_SIZ.m_Components[b].GetDCShift();//JPC.m_SIZ.m_Components[b].m_bSigned ? 0 : (0x1 << (JPC.m_SIZ.m_Components[b].m_nBits - 1));
		
		if(JPC.m_SIZ.m_Components[b].m_nBits <= 12) {
			OutputBuffer.Alloc(0, m_nNextLine, nWidth, (UINT32)1, CNCSJPCBuffer::BT_INT16);
			INT16 *pBuf = (INT16*)OutputBuffer.GetPtr();
			void *pOutputLine = ppOutputLine[b];
			switch(eType) {
			case NCSCT_UINT8:
				for(x = 0; x < nWidth; x++) {
					pBuf[x] = (INT16)((UINT8*)pOutputLine)[x * nStep] - nSub;
				}
				break;
			case NCSCT_UINT16:
				for(x = 0; x < nWidth; x++) {
					pBuf[x] = (INT16)((UINT16*)pOutputLine)[x * nStep] - nSub;
				}
				break;
			case NCSCT_UINT32:
				for(x = 0; x < nWidth; x++) {
					pBuf[x] = (INT16)((UINT32*)pOutputLine)[x * nStep] - nSub;
				}
				break;
			case NCSCT_UINT64:
				for(x = 0; x < nWidth; x++) {
					pBuf[x] = (INT16)((UINT64*)pOutputLine)[x * nStep] - nSub;
				}
				break;
			case NCSCT_INT8:
				for(x = 0; x < nWidth; x++) {
					pBuf[x] = (INT16)((INT8*)pOutputLine)[x * nStep] - nSub;
				}
				break;
			case NCSCT_INT16:
				for(x = 0; x < nWidth; x++) {
					pBuf[x] = (INT16)((INT16*)pOutputLine)[x * nStep] - nSub;
				}
				break;
			case NCSCT_INT32:
				for(x = 0; x < nWidth; x++) {
					pBuf[x] = (INT16)((INT32*)pOutputLine)[x * nStep] - nSub;
				}
				break;
			case NCSCT_INT64:
				for(x = 0; x < nWidth; x++) {
					pBuf[x] = (INT16)((INT64*)pOutputLine)[x * nStep] - nSub;
				}
				break;
			case NCSCT_IEEE4:
				FLT_TO_INT_INIT();
				INT32 nMin = JPC.m_SIZ.m_Components[b].GetMinValue();
				INT32 nMax = JPC.m_SIZ.m_Components[b].GetMaxValue();
				x = 0;
#ifdef NCSJPC_X86_MMI_MMX
				if(CNCSJPCBuffer::sm_SystemInfo.m_bSSEPresent && nWidth > 4 && nStep == 1) {
					UINT32 nSubWidth = nWidth / 4;
					
					__m128 mm7_fMin = _mm_set1_ps((float)nMin);
					__m128 mm6_fMax = _mm_set1_ps((float)nMax);
					__m64 mm4_nSub = _mm_set1_pi16(nSub);
					
					for(; x < nSubWidth; x++) {
						__m128 mm0 = _mm_loadu_ps(&(((IEEE4*)pOutputLine)[x*4]));
						__m128 mm1 = _mm_max_ps(mm7_fMin, _mm_min_ps(mm6_fMax, mm0));
						__m64 mm2 = _mm_cvtps_pi16__INLINE(mm1);
						__m64 mm3 = _mm_sub_pi16(mm2, mm4_nSub);
						
						((__m64*)pBuf)[x] = mm3;
					}
					_mm_empty();
					x *= 4;
					
				}
#endif
				for(; x < nWidth; x++) {
					IEEE4 v = ((IEEE4*)pOutputLine)[x * nStep];
					v = NCSMax(nMin, NCSMin(nMax, v));
					pBuf[x] = (INT16)NCSfloatToInt32_RM(v) - nSub;
				}
				FLT_TO_INT_FINI();
				break;
			case NCSCT_IEEE8:
				FLT_TO_INT_INIT();
				INT32 nMin = JPC.m_SIZ.m_Components[b].GetMinValue();
				INT32 nMax = JPC.m_SIZ.m_Components[b].GetMaxValue();
				for(x = 0; x < nWidth; x++) {
					IEEE4 v = (IEEE4)((IEEE8*)pOutputLine)[x * nStep];
					v = NCSMax(nMin, NCSMin(nMax, v));
					pBuf[x] = (INT16)NCSfloatToInt32_RM(v) - nSub;
				}
				FLT_TO_INT_FINI();
				break;
			default: break;
			}
		} else {
			OutputBuffer.Alloc(0, m_nNextLine, nWidth, (UINT32)1, CNCSJPCBuffer::BT_INT32);
			INT32 *pBuf = (INT32*)OutputBuffer.GetPtr();
			void *pOutputLine = ppOutputLine[b];
			switch(eType) {
				
			case NCSCT_UINT8:
				for(x = 0; x < nWidth; x++) {
					pBuf[x] = (INT32)((UINT8*)pOutputLine)[x * nStep] - nSub;
				}
				break;
			case NCSCT_UINT16:
				for(x = 0; x < nWidth; x++) {
					pBuf[x] = (INT32)((UINT16*)pOutputLine)[x * nStep] - nSub;
				}
				break;
			case NCSCT_UINT32:
				for(x = 0; x < nWidth; x++) {
					pBuf[x] = (INT32)((UINT32*)pOutputLine)[x * nStep] - nSub;
				}
				break;
			case NCSCT_UINT64:
				for(x = 0; x < nWidth; x++) {
					pBuf[x] = (INT32)((UINT64*)pOutputLine)[x * nStep] - nSub;
				}
				break;
			case NCSCT_INT8:
				for(x = 0; x < nWidth; x++) {
					pBuf[x] = (INT32)((INT8*)pOutputLine)[x * nStep] - nSub;
				}
				break;
			case NCSCT_INT16:
				for(x = 0; x < nWidth; x++) {
					pBuf[x] = (INT32)((INT16*)pOutputLine)[x * nStep] - nSub;
				}
				break;
			case NCSCT_INT32:
				for(x = 0; x < nWidth; x++) {
					pBuf[x] = (INT32)((INT32*)pOutputLine)[x * nStep] - nSub;
				}
				break;
			case NCSCT_INT64:
				for(x = 0; x < nWidth; x++) {
					pBuf[x] = (INT32)((INT64*)pOutputLine)[x * nStep] - nSub;
				}
				break;
			case NCSCT_IEEE4:
				FLT_TO_INT_INIT();
				INT32 nMin = JPC.m_SIZ.m_Components[b].GetMinValue();
				INT32 nMax = JPC.m_SIZ.m_Components[b].GetMaxValue();
				for(x = 0; x < nWidth; x++) {
					IEEE4 v = ((IEEE4*)pOutputLine)[x * nStep];
					v = NCSMax(nMin, NCSMin(nMax, v));
					pBuf[x] = (INT32)NCSfloatToInt32_RM(v) - nSub;
				}
				FLT_TO_INT_FINI();
				break;
			case NCSCT_IEEE8:
				FLT_TO_INT_INIT();
				INT32 nMin = JPC.m_SIZ.m_Components[b].GetMinValue();
				INT32 nMax = JPC.m_SIZ.m_Components[b].GetMaxValue();
				for(x = 0; x < nWidth; x++) {
					IEEE4 v = (IEEE4)((IEEE8*)pOutputLine)[x * nStep];
					v = NCSMax(nMin, NCSMin(nMax, v));
					pBuf[x] = (INT32)NCSfloatToInt32_RM(v) - nSub;
				}
				FLT_TO_INT_FINI();
				break;
			default: break;
			}
		}
		bRet &= m_pOutputNode->WriteLine(this, &OutputBuffer, b);
	}
	m_nNextLine++;
	return(bRet ? CNCSError() : CNCSError(NCS_COULDNT_PERFORM_COMPRESSION));
}

// Get current ViewSetinfo structure.
/*const*/ NCSFileViewSetInfo *CNCSJP2FileView::GetFileViewSetInfo()
{
	CNCSJPCGlobalLock _Lock;
#ifdef NCSJPC_ECW_SUPPORT
	if (m_pECWFileView) {
		if(m_bIsProgressive) {
			NCSFileViewSetInfo *pInfo = NULL;
			NCSError eError = NCScbmGetViewInfo_ECW(m_pECWFileView, &pInfo);
			
			/*
			Change below makes progressive mode work for non-tiled views, by updating
			the view statistics so that refresh callbacks can be fired properly.
			here was a problem where the pClientData pointer was inconsistent.
			This logic will need to be fixed for tiled and progressive view case, which 
			currently fails with a view too large error.
			*/
			
			m_CurrentView.nBlocksAvailable = pInfo->nBlocksAvailable;
			m_CurrentView.nBlocksAvailableAtSetView = pInfo->nBlocksAvailableAtSetView;
			m_CurrentView.nBlocksInView = pInfo->nBlocksInView;
			m_CurrentView.nMissedBlocksDuringRead = pInfo->nMissedBlocksDuringRead;
		}
		return(&m_CurrentView);
	}
#endif
	if(m_pFile) {
		return(&m_CurrentView);
	}
	return(NULL);
}

// Get current FileInfo structure.
/*const*/ NCSFileViewFileInfoEx *CNCSJP2FileView::GetFileInfo()
{
	CNCSJPCGlobalLock _Lock;
#ifdef NCSJPC_ECW_SUPPORT
	if(m_pECWFileView) {
		NCSFileViewFileInfoEx *pInfo = NULL;
		NCSError eError = NCScbmGetViewFileInfoEx_ECW(m_pECWFileView, &pInfo);
		pInfo->fCWRotationDegrees = 0.0;//[02]
		return(pInfo);
	} else if(m_pECWCompressionTask && m_pECWCompressionTask->pQmf) {
		return(m_pECWCompressionTask->pQmf->pFileInfo);
	}
#endif
	if(m_pFile) {
		return(&m_pFile->m_FileInfo);
	}
	return(NULL);
}

// Pass in the physical parameters to use for the file
CNCSError CNCSJP2FileView::SetFileInfo(NCSFileViewFileInfoEx &Info)
{
	CNCSJPCGlobalLock _Lock;
	UINT32 b;
	
	m_nWidth = Info.nSizeX;
	m_nHeight = Info.nSizeY;
	m_nNumberOfBands = Info.nBands;
	if(Info.eColorSpace == NCSCS_YUV) {
		m_nNumberOfBands = NCSMax(m_nNumberOfBands, 3);
	}
	m_dTargetCompressionRate = Info.nCompressionRate;
	m_dCompressionRate = Info.nCompressionRate;
	m_dCellIncrementX = Info.fCellIncrementX;
	m_dCellIncrementY = Info.fCellIncrementY;
	m_dOriginX = Info.fOriginX;
	m_dOriginY = Info.fOriginY;
	NCSFree(m_pDatum);
	m_pDatum = NCSStrDup(Info.szDatum);
	NCSFree(m_pProjection);
	m_pProjection = NCSStrDup(Info.szProjection);
	m_CellSizeUnits = Info.eCellSizeUnits;
	
	m_eColorSpace = Info.eColorSpace;
	m_eCellType = Info.eCellType;
	for(b = 0; b < m_Bands.size(); b++) {
		NCSFree(m_Bands[b].szDesc);
	}
	m_Bands.resize(m_nNumberOfBands);
	for(b = 0; b < (UINT32)m_nNumberOfBands; b++) {
		if(Info.pBands) {
			m_Bands[b].nBits = Info.pBands[b].nBits;
			m_Bands[b].bSigned = Info.pBands[b].bSigned;
			m_Bands[b].szDesc = NCSStrDup(Info.pBands[b].szDesc);
		} else {
			char buf[32];
			m_Bands[b].nBits = 8;
			m_Bands[b].bSigned = FALSE;
			
			switch(Info.eColorSpace) {
			case NCSCS_sRGB:
			case NCSCS_YUV:
				switch(b) {
				case 0:
					strcpy(buf, NCS_BANDDESC_Red);
					break;
				case 1:
					strcpy(buf, NCS_BANDDESC_Green);
					break;
				case 2:
					strcpy(buf, NCS_BANDDESC_Blue);
					break;
				}
				break;
				case NCSCS_GREYSCALE:
					strcpy(buf, NCS_BANDDESC_Greyscale);
					break;
				default:
					sprintf(buf, NCS_BANDDESC_Band, b + 1);
					break;
			}
			m_Bands[b].szDesc = NCSStrDup(buf);
		}
	}
	//SetParameter(JP2_COMPRESS_LAYERS, (UINT32)10);
	//SetParameter(JP2_COMPRESS_TILE_WIDTH, (UINT32)1024);
	//SetParameter(JP2_COMPRESS_TILE_HEIGHT, (UINT32)1024);
	m_dCWRotationDegrees = Info.fCWRotationDegrees; //[02]
	
	return(NCS_SUCCESS);
}

CNCSJP2Box *CNCSJP2FileView::GetUUIDBox(NCSUUID uuid, CNCSJP2Box *pLast)
{
	if(m_pFile && m_pFile->m_pStream && !m_pFile->m_pStream->IsWrite()) {
		return(m_pFile->GetUUIDBox(uuid, (CNCSJP2File::CNCSJP2UUIDBox*)pLast));
	}
	return(NULL);
}

CNCSJP2Box *CNCSJP2FileView::GetXMLBox(CNCSJP2Box *pLast)
{
	if(m_pFile && m_pFile->m_pStream && !m_pFile->m_pStream->IsWrite()) {
		return(m_pFile->GetXMLBox((CNCSJP2File::CNCSJP2XMLBox*)pLast));
	}
	return(NULL);
}

CNCSJP2Box *CNCSJP2FileView::GetBox(UINT32 nTBox, CNCSJP2Box *pLast)
{
	if(m_pFile && m_pFile->m_pStream && !m_pFile->m_pStream->IsWrite()) {
		return(m_pFile->GetBox(nTBox, pLast));
	}
	return(NULL);
}

// Add a box to be written to the file on compression.
CNCSError CNCSJP2FileView::AddBox(CNCSJP2Box *pBox)
{
	m_OtherBoxes.push_back(pBox);
	return(NCS_SUCCESS);
}

CNCSJPCIOStream *CNCSJP2FileView::GetStream()
{
	if(m_pFile) {
		return(m_pFile->m_pStream);
	}
	return(NULL);
}

bool CNCSJP2FileView::UpdateFileViewInfo(void)
{
	bool bRet = false;
	CNCSJPCGlobalLock _Lock;
	const NCSFileViewFileInfoEx *pInfo = GetFileInfo();
	
	if(m_bIsOpen && pInfo) {
		NCSFree(m_pFilename);
#ifdef NCSJPC_ECW_SUPPORT
		if(m_pECWFileView) {
			m_dCompressionRate = pInfo->nCompressionRate;
			m_pFilename = NCSStrDup(m_pECWFileView->pNCSFile->szUrlPath);
		} else {
#endif // NCSJPC_ECW_SUPPORT
			((NCSFileViewFileInfoEx*)pInfo)->nCompressionRate = (UINT16)(pInfo->nSizeX * pInfo->nSizeY * pInfo->nBands / (IEEE8)m_pFile->m_pStream->Size());
			m_pFilename = NCSStrDup(CHAR_STRING(m_pFile->m_pStream->GetName()));
#ifdef NCSJPC_ECW_SUPPORT
		}
#endif // NCSJPC_ECW_SUPPORT
		bRet = SetFileInfo((NCSFileViewFileInfoEx&)*pInfo) == NCS_SUCCESS;
	} else {
		m_nWidth = 0;				
		m_nHeight = 0;
		m_nNumberOfBands = 0;
		m_dCompressionRate = 0.0;
		
		/* Projection Info */
		m_CellSizeUnits = ECW_CELL_UNITS_METERS;
		m_dCellIncrementX = 1.0;
		m_dCellIncrementY = 1.0;
		m_dOriginX = 0.0;
		m_dOriginY = 0.0;
		NCSFree(m_pDatum);
		m_pDatum = NULL;
		NCSFree(m_pProjection);
		m_pProjection =	NULL;
		
		NCSFree(m_pFilename);
		m_pFilename = NULL;
		m_eCellType = NCSCT_UINT8;
		for(UINT32 b = 0; b < m_Bands.size(); b++) {
			NCSFree(m_Bands[b].szDesc);
		}
		m_Bands.resize(m_nNumberOfBands);
	}
	if(m_bIsOpen && m_bHaveValidSetView) {
		const NCSFileViewSetInfo *pInfo = GetFileViewSetInfo();
		
		m_nSetViewNrBands = pInfo->nBands;
		m_pnSetViewBandList = (INT32*)pInfo->pBandList;
		m_nSetViewWidth = pInfo->nSizeX;
		m_nSetViewHeight = pInfo->nSizeY;
		m_dSetViewWorldTLX = pInfo->fTopX;
		m_dSetViewWorldTLY = pInfo->fLeftY;
		m_dSetViewWorldBRX = pInfo->fBottomX;
		m_dSetViewWorldBRY = pInfo->fRightY;
		m_nSetViewDatasetTLX = pInfo->nTopX;
		m_nSetViewDatasetTLY = pInfo->nLeftY;
		m_nSetViewDatasetBRX = pInfo->nBottomX;
		m_nSetViewDatasetBRY = pInfo->nRightY;
	} else {
		m_nSetViewNrBands = 0;
		m_pnSetViewBandList = NULL;
		m_nSetViewWidth = 0;
		m_nSetViewHeight = 0;
		m_dSetViewWorldTLX = 0;
		m_dSetViewWorldTLY = 0;
		m_dSetViewWorldBRX = 0;
		m_dSetViewWorldBRY = 0;
		m_nSetViewDatasetTLX = 0;
		m_nSetViewDatasetTLY = 0;
		m_nSetViewDatasetBRX = 0;
		m_nSetViewDatasetBRY = 0;
	}
	return(bRet);
}

CNCSJP2File *CNCSJP2FileView::GetFile()
{
	return(m_pFile);
}

void CNCSJP2FileView::UpdateViewStats()
{
	if(m_bHaveValidSetView) {
		m_pFile->m_pStream->ProcessReceivedPackets();
		
		m_CurrentView.nBlocksInView = 0;
		m_CurrentView.nBlocksAvailable = 0;
		for(INT32 t = 0; t < m_pFile->m_Codestream.GetNumXTiles() * m_pFile->m_Codestream.GetNumYTiles(); t++) {
			CNCSJPCTilePartHeader *pMainTP = m_pFile->m_Codestream.GetTile(t);
			
			if(pMainTP) {
				for(UINT32 b = 0; b < m_pFile->m_ChannelComponentIndex.size(); b++) {
					pMainTP->m_Components[m_pFile->m_ChannelComponentIndex[b]]->TraversePyramid(CNCSJPCResample::CalculateResolutionLevel(pMainTP->m_Components[m_pFile->m_ChannelComponentIndex[b]],
						m_CurrentView.nTopX, m_CurrentView.nLeftY,
						m_CurrentView.nBottomX, m_CurrentView.nRightY,
						m_CurrentView.nSizeX, m_CurrentView.nSizeY),
						CNCSJPCComponent::STATS, 
						m_CurrentView.nTopX, 
						m_CurrentView.nLeftY, 
						m_CurrentView.nBottomX, 
						m_CurrentView.nRightY, 
						m_CurrentView.nSizeX,
						m_CurrentView.nSizeY,
						m_CurrentView.nBlocksInView, 
						m_CurrentView.nBlocksAvailable);
				}
			}
		}
		m_pFile->Purge();
	}
}
#ifdef NCSJPC_ECW_SUPPORT
NCSFileView *CNCSJP2FileView::GetNCSFileView()
{
	return(m_pECWFileView);
}
#endif // NCSJPC_ECW_SUPPORT

// Set refresh callback funnction 
CNCSError CNCSJP2FileView::SetRefreshCallback(NCSEcwReadStatus (*pCallback)(NCSFileView*))
{
	m_pRefreshCallback = pCallback;
	return(NCS_SUCCESS);
}

CNCSError CNCSJP2FileView::SetCompressClient(struct NCSEcwCompressClient *pCompressClient)
{
	m_pCompressClient = pCompressClient;
	return(NCS_SUCCESS);
}

NCSEcwReadStatus CNCSJP2FileView::FireRefreshUpdate(CNCSJPCGlobalLock &_Lock)
{
	m_bInRefreshUpdate = true;
	m_bCancelRead = false;
	m_nNextLine = 0;
	
	_Lock.UnLock();
	NCSEcwReadStatus eStatus = RefreshUpdateEx((NCSFileViewSetInfo*)GetFileViewSetInfo());
	_Lock.Lock();
	
	std::vector<CNCSJP2FileView *>::iterator pCur = sm_Views.begin();
	std::vector<CNCSJP2FileView *>::iterator pEnd = sm_Views.end();
	
	while(pCur != pEnd && *pCur != this) {
		pCur++;
	}
	if(pCur != pEnd) {
		m_bInRefreshUpdate = false;
		m_bCancelRead = false;
		if(eStatus == NCSECW_READ_OK && m_nNextLine != 0) {
			// Only update this if something was read
			m_nBlocksAtLastRefreshUpdate = m_CurrentView.nBlocksAvailable;
			m_tsLastRefreshUpdate = NCSGetTimeStampMs();
		}
	}
	return(eStatus);
}

NCSEcwReadStatus CNCSJP2FileView::RefreshUpdateEx(NCSFileViewSetInfo *pViewSetInfo)
{
	if(m_pRefreshCallback) {
		return(m_pRefreshCallback((NCSFileView*)this));
	}
	return(NCSECW_READ_FAILED);
}

CNCSJP2FileView *CNCSJP2FileView::FindJP2FileView(NCSFileView *pNCSFileView)
{
#ifdef NCSJPC_ECW_SUPPORT
	CNCSJP2FileView *pView = NULL;
	CNCSJPCGlobalLock _Lock;
	
	for(UINT32 i = 0; i < sm_Views.size(); i++) {
		if(sm_Views[i] == (CNCSJP2FileView*)pNCSFileView || sm_Views[i]->m_pECWFileView == pNCSFileView) {
			pView = sm_Views[i];
			break;
		}
	}
	return(pView);
#else
	return((CNCSJP2FileView*)pNCSFileView);
#endif // NCSJPC_ECW_SUPPORT
}

CNCSJP2FileView::CNCSJP2FileViewVector::~CNCSJP2FileViewVector()
{
	Stop();
}

void CNCSJP2FileView::CNCSJP2FileViewVector::Work(void *pData)
{
	while(Run()) {
		{
			CNCSJPCGlobalLock _Lock(false);
			if(_Lock.TryLock()) {
				std::vector<CNCSJP2FileView *>::iterator pCur = begin();
				std::vector<CNCSJP2FileView *>::iterator pEnd = end();
				
				while(pCur != pEnd) {
					CNCSJP2FileView *pView = *pCur;
					
					// Can check this outside the Mutex OK.
					if(pView->m_bIsOpen && pView->m_bIsProgressive && pView->m_bHaveValidSetView) {
						
						if(pView->m_pFile) {
							pView->UpdateViewStats();
							
							if(pView->m_nBlocksAtLastRefreshUpdate != pView->m_CurrentView.nBlocksAvailable &&
								((pView->m_tsLastRefreshUpdate + (NCSecwGetStatistics() ? NCSecwGetStatistics()->nRefreshTime : NCSECW_REFRESH_TIME_MS) < NCSGetTimeStampMs()) ||
								(pView->m_CurrentView.nBlocksAvailable == pView->m_CurrentView.nBlocksInView))) {
								pView->FireRefreshUpdate(_Lock);
								/*							pView->m_bInRefreshUpdate = true;
								pView->m_bCancelRead = false;
								pView->m_nNextLine = 0;
								_Lock.UnLock();
								NCSEcwReadStatus eStatus = pView->RefreshUpdateEx((NCSFileViewSetInfo*)pView->GetFileViewSetInfo());
								_Lock.Lock();
								pCur = begin();
								pEnd = end();
								while(pCur != pEnd && *pCur != pView) {
								pCur++;
								}
								if(pCur == pEnd) {
								pView = NULL;
								}
								if(pView) {
								pView->m_bInRefreshUpdate = false;
								pView->m_bCancelRead = false;
								if(eStatus == NCSECW_READ_OK && pView->m_nNextLine != 0) {
								// Only update this if something was read
								pView->m_nBlocksAtLastRefreshUpdate = pView->m_CurrentView.nBlocksAvailable;
								}
								}
								*/
							}
							if(pView) {
								if(pView->m_bPendingView) {
									UINT32 *pBandList = pView->m_PendingView.pBandList;
									pView->m_PendingView.pBandList = NULL;
									pView->m_bPendingView = false;
									pView->SetView(pView->m_PendingView.nBands,
										pBandList,
										pView->m_PendingView.nTopX,
										pView->m_PendingView.nLeftY,
										pView->m_PendingView.nBottomX,
										pView->m_PendingView.nRightY,
										pView->m_PendingView.nSizeX,
										pView->m_PendingView.nSizeY,
										pView->m_PendingView.fTopX,
										pView->m_PendingView.fLeftY,
										pView->m_PendingView.fBottomX,
										pView->m_PendingView.fRightY);
									NCSFree(pBandList);
									
								}
							}
						}
					}
					if(pCur != pEnd) {
						pCur++;
					}
				}
				_Lock.UnLock();
			}
		}
		// Wait() outside _Lock's scope!
		m_Event.Wait(50);
		//NCSecwGetStatistics() ? NCSecwGetStatistics()->nRefreshTime : NCSECW_REFRESH_TIME_MS);
		//		NCSSleep(NCSecwGetStatistics() ? NCSecwGetStatistics()->nRefreshTime : NCSECW_REFRESH_TIME_MS);
	}
}

void CNCSJP2FileView::Shutdown()
{
	sm_Views.Stop();
	CNCSJP2File::Shutdown();
	CNCSGDTEPSG::Release();
}

void CNCSJP2FileView::SetKeySize(void)
{
	CNCSJP2File::SetKeySize();
}

void CNCSJP2FileView::GetStatistic(CNCSJP2FileView::Statistic eType, NCSTimeStampMs &Val)
{
	switch(eType) {
	case ST_CODEBLOCK_DECODER_US:
		Val = CNCSJPCT1Coder::sm_usTotal;
		break;
	case ST_CODEBLOCK_READ_US:
		Val = CNCSJPCCodeBlock::sm_usTotal;
		break;
	case ST_BUF_ALLOC_US:
		Val = CNCSJPCBuffer::sm_usAlloc;
		break;
	case ST_BUF_FREE_US:
		Val = CNCSJPCBuffer::sm_usFree;
		break;
	case ST_BUF_COPY_US:
		Val = CNCSJPCBuffer::sm_usCopy;
		break;
	case ST_BUF_CLEAR_US:
		Val = CNCSJPCBuffer::sm_usClear;
		break;
	case ST_COMPRESS_TOTAL_MS:
		Val = m_tsCompressEnd - m_tsCompressStart;
		break;
	default: break;
	}
}

void CNCSJP2FileView::GetStatistic(CNCSJP2FileView::Statistic eType, UINT64 &Val)
{
	switch(eType) {
	case ST_CODEBLOCK_DECODER_SAMPLES:
		Val = CNCSJPCT1Coder::sm_nTotalSamples;
		break;
	case ST_CODEBLOCK_READ_SAMPLES:
		Val = CNCSJPCCodeBlock::sm_nTotalSamples;
		break;
	case ST_BUF_ALLOC_BYTES:
		Val = CNCSJPCBuffer::sm_nAllocBytes;
		break;
	case ST_BUF_COPY_BYTES:
		Val = CNCSJPCBuffer::sm_nCopyBytes;
		break;
	case ST_BUF_CLEAR_BYTES:
		Val = CNCSJPCBuffer::sm_nClearBytes;
		break;
	case ST_COMPRESS_OUTPUT_SIZE:
		Val = NCSFileSizeBytes(OS_STRING(m_pFilename));
		break;
	default: break;
	}
}

void CNCSJP2FileView::SetParameter(Parameter eType)
{
	CNCSJPCGlobalLock _Lock;
	switch(eType) {
	case JP2_COMPRESS_PROFILE_BASELINE_0:
	case JP2_COMPRESS_PROFILE_BASELINE_1:
	case JP2_COMPRESS_PROFILE_BASELINE_2:
		m_Parameters.push_back(ParameterValuePair(eType));
		break;
	case JP2_COMPRESS_PROFILE_NITF_BIIF_NPJE:
		m_Parameters.push_back(ParameterValuePair(eType));
		SetParameter(JP2_COMPRESS_PROGRESSION_LRCP);
		SetParameter(JP2_COMPRESS_LEVELS, (UINT32)5);
		SetParameter(JP2_COMPRESS_LAYERS, (UINT32)19);
		SetParameter(JP2_COMPRESS_TILE_WIDTH, (UINT32)1024);
		SetParameter(JP2_COMPRESS_TILE_HEIGHT, (UINT32)1024);
		SetParameter(JP2_COMPRESS_PRECINCT_WIDTH, (UINT32)32768);
		SetParameter(JP2_COMPRESS_PRECINCT_HEIGHT, (UINT32)32768);
		break;
	case JP2_COMPRESS_PROFILE_NITF_BIIF_EPJE:
		m_Parameters.push_back(ParameterValuePair(eType));
		SetParameter(JP2_COMPRESS_PROGRESSION_RLCP);
		SetParameter(JP2_COMPRESS_LEVELS, (UINT32)5);
		SetParameter(JP2_COMPRESS_LAYERS, (UINT32)19);
		SetParameter(JP2_COMPRESS_TILE_WIDTH, (UINT32)1024);
		SetParameter(JP2_COMPRESS_TILE_HEIGHT, (UINT32)1024);
		SetParameter(JP2_COMPRESS_PRECINCT_WIDTH, (UINT32)32768);
		SetParameter(JP2_COMPRESS_PRECINCT_HEIGHT, (UINT32)32768);
		break;
	case JP2_COMPRESS_PROGRESSION_LRCP:
	case JP2_COMPRESS_PROGRESSION_RLCP:
	case JP2_COMPRESS_PROGRESSION_RPCL:
		m_Parameters.push_back(ParameterValuePair(eType));
		break;
	default: break;
	}
}

void CNCSJP2FileView::SetParameter(Parameter eType, bool bBool)
{
	CNCSJPCGlobalLock _Lock;
	switch(eType) {
	case JP2_COMPRESS_INCLUDE_SOP:
	case JP2_COMPRESS_INCLUDE_EPH:
		m_Parameters.push_back(ParameterValuePair(eType, bBool));
		break;
	case JP2_COMPRESS_CODESTREAM_ONLY:
		m_Parameters.push_back(ParameterValuePair(eType, bBool));
		break;
	case JP2_COMPRESS_MT_READ:
		m_Parameters.push_back(ParameterValuePair(eType, bBool));
		break;
	case JPC_DECOMPRESS_AUTOSCALE_UP:
		m_bAutoScaleUp = bBool;
		break;
	default: break;
	}
}

void CNCSJP2FileView::SetParameter(Parameter eType, UINT32 nValue)
{
	CNCSJPCGlobalLock _Lock;
	switch(eType) {
	case JP2_COMPRESS_PRECINCT_WIDTH:
	case JP2_COMPRESS_PRECINCT_HEIGHT:
		if(nValue >= 32 && nValue <= NCS2Pow(15) && NCSIsPow2(nValue)) {
			m_Parameters.push_back(ParameterValuePair(eType, nValue));
		}
		break;
	case JP2_COMPRESS_TILE_WIDTH:
	case JP2_COMPRESS_TILE_HEIGHT:
		if(nValue >= 32 && NCSIsPow2(nValue)) {
			m_Parameters.push_back(ParameterValuePair(eType, nValue));
		}
		break;
	case JP2_COMPRESS_LEVELS:
		if(nValue >= 0 && nValue <= 32) {
			m_Parameters.push_back(ParameterValuePair(eType, nValue));
		}
		break;
	case JP2_COMPRESS_LAYERS:
	case JP2_DECOMPRESS_LAYERS:
		if(nValue >= 1 && nValue <= 65535) {
			m_Parameters.push_back(ParameterValuePair(eType, nValue));
		}
		break;
	case JP2_GEODATA_USAGE:
		if(nValue == JP2_GEODATA_USE_NONE ||
			nValue == JP2_GEODATA_USE_PCS_ONLY ||
			nValue == JP2_GEODATA_USE_GML_ONLY ||
			nValue == JP2_GEODATA_USE_WLD_ONLY ||
			nValue == JP2_GEODATA_USE_PCS_GML ||
			nValue == JP2_GEODATA_USE_PCS_WLD ||
			nValue == JP2_GEODATA_USE_GML_PCS ||
			nValue == JP2_GEODATA_USE_GML_WLD ||
			nValue == JP2_GEODATA_USE_WLD_PCS ||
			nValue == JP2_GEODATA_USE_WLD_GML ||
			nValue == JP2_GEODATA_USE_PCS_GML_WLD ||
			nValue == JP2_GEODATA_USE_PCS_WLD_GML ||
			nValue == JP2_GEODATA_USE_GML_PCS_WLD ||
			nValue == JP2_GEODATA_USE_GML_WLD_PCS ||
			nValue == JP2_GEODATA_USE_WLD_PCS_GML ||
			nValue == JP2_GEODATA_USE_WLD_GML_PCS)
		{
			//					m_Parameters.push_back(ParameterValuePair(eType,nValue));
			CNCSJP2File::sm_nGeodataUsage = nValue;
		}
		break;
	default: break;
	}
}

void CNCSJP2FileView::SetParameter(Parameter eType, IEEE4 fValue)
{
	CNCSJPCGlobalLock _Lock;
	switch(eType) {
	case JPC_DECOMPRESS_RECONSTRUCTION_PARAMETER:
		if(fValue >= 0.0f && fValue < 1.0f) {
			m_Parameters.push_back(ParameterValuePair(eType, fValue));
		}
		break;
	case JP2_GEODATA_PRECISION_EPSILON:
		if(fValue >= 0.0f) {
			CNCSJP2File::CNCSJP2PCSBox::SetPrecision(fValue);
		}
	default: break;
	}
}

void CNCSJP2FileView::SetGeodataUsage(GeodataUsage nGeodataUsage)
{
	CNCSJP2File::sm_nGeodataUsage = (UINT32) nGeodataUsage;
}

void CNCSJP2FileView::SetFileParameters()
{
	if(m_pFile) {
		for(UINT32 i = 0; i < m_Parameters.size(); i++) {
			switch(m_Parameters[i].m_eParam) {
			case JP2_COMPRESS_PROFILE_BASELINE_0:
				m_pFile->m_eCompressionProfile = CNCSJPC::BASELINE_0;
				break;
			case JP2_COMPRESS_PROFILE_BASELINE_1:
				m_pFile->m_eCompressionProfile = CNCSJPC::BASELINE_1;
				break;
			case JP2_COMPRESS_PROFILE_BASELINE_2:
				m_pFile->m_eCompressionProfile = CNCSJPC::BASELINE_2;
				break;
			case JP2_COMPRESS_PROFILE_NITF_BIIF_NPJE:
				m_pFile->m_eCompressionProfile = CNCSJPC::NITF_BIIF_NPJE;
				break;
			case JP2_COMPRESS_PROFILE_NITF_BIIF_EPJE:
				m_pFile->m_eCompressionProfile = CNCSJPC::NITF_BIIF_EPJE;
				break;				
			case JP2_COMPRESS_LEVELS:
				m_pFile->m_nCompressionLevels = *((UINT32*)m_Parameters[i].m_pValue);
				break;
			case JP2_COMPRESS_LAYERS:
				m_pFile->m_nCompressionLayers = *((UINT32*)m_Parameters[i].m_pValue);
				break;
			case JP2_COMPRESS_PRECINCT_WIDTH:
				m_pFile->m_nCompressionPrecinctWidth = *((UINT32*)m_Parameters[i].m_pValue);
				break;
			case JP2_COMPRESS_PRECINCT_HEIGHT:
				m_pFile->m_nCompressionPrecinctHeight = *((UINT32*)m_Parameters[i].m_pValue);
				break;
			case JP2_COMPRESS_TILE_WIDTH:
				m_pFile->m_nCompressionTileWidth = *((UINT32*)m_Parameters[i].m_pValue);
				break;
			case JP2_COMPRESS_TILE_HEIGHT:
				m_pFile->m_nCompressionTileHeight = *((UINT32*)m_Parameters[i].m_pValue);
				break;
			case JP2_DECOMPRESS_LAYERS:
				m_pFile->m_nDecompressionLayers = *((UINT32*)m_Parameters[i].m_pValue);
				break;
			case JP2_COMPRESS_INCLUDE_SOP:
				m_pFile->m_bCompressionIncludeSOP = *((bool*)m_Parameters[i].m_pValue);
				break;
			case JP2_COMPRESS_INCLUDE_EPH:
				m_pFile->m_bCompressionIncludeEPH = *((bool*)m_Parameters[i].m_pValue);
				break;
			case JP2_COMPRESS_CODESTREAM_ONLY:
				m_pFile->m_bCodestreamOnly = *((bool*)m_Parameters[i].m_pValue);
				break;
			case JP2_COMPRESS_PROGRESSION_LRCP:
				m_pFile->m_eCompressionOrder = CNCSJPCProgressionOrderType::LRCP;
				break;
			case JP2_COMPRESS_PROGRESSION_RLCP:
				m_pFile->m_eCompressionOrder = CNCSJPCProgressionOrderType::RLCP;
				break;
			case JP2_COMPRESS_PROGRESSION_RPCL:
				m_pFile->m_eCompressionOrder = CNCSJPCProgressionOrderType::RPCL;
				break;
			case JPC_DECOMPRESS_RECONSTRUCTION_PARAMETER:
				m_pFile->m_Codestream.m_fReconstructionParameter = *((IEEE4*)m_Parameters[i].m_pValue);
				break;
			case JP2_COMPRESS_MT_READ:
				m_bCompressMTRead = *((bool*)m_Parameters[i].m_pValue);
				break;
			case JP2_GEODATA_USAGE:
			case JP2_GEODATA_PRECISION_EPSILON:
				break;
			default: break;
			}
		}
	}
}

CNCSJP2FileView::ParameterValuePair::ParameterValuePair()
{
	m_eParam = (CNCSJP2FileView::Parameter)0;
	m_pValue = NULL;
}

CNCSJP2FileView::ParameterValuePair::ParameterValuePair(const ParameterValuePair &Src)
{
	m_eParam = Src.m_eParam;
	m_pValue = NULL;
	
	switch(m_eParam) {
	case JP2_COMPRESS_LEVELS:
	case JP2_COMPRESS_LAYERS:
	case JP2_COMPRESS_PRECINCT_WIDTH:
	case JP2_COMPRESS_PRECINCT_HEIGHT:
	case JP2_COMPRESS_TILE_WIDTH:
	case JP2_COMPRESS_TILE_HEIGHT:
	case JP2_DECOMPRESS_LAYERS:
		m_pValue = NCSMalloc(sizeof(UINT32), FALSE);
		*((UINT32*)m_pValue) = *((UINT32*)Src.m_pValue);
		break;
	case JP2_COMPRESS_INCLUDE_SOP:
	case JP2_COMPRESS_INCLUDE_EPH:
	case JP2_COMPRESS_CODESTREAM_ONLY:
	case JP2_COMPRESS_MT_READ:
	case JPC_DECOMPRESS_AUTOSCALE_UP:
		m_pValue = NCSMalloc(sizeof(bool), FALSE);
		*((bool*)m_pValue) = *((bool*)Src.m_pValue);
		break;
	default: break;
	}
}

CNCSJP2FileView::ParameterValuePair::ParameterValuePair(Parameter eParam)
{
	m_eParam = eParam;
	m_pValue = NULL;
}

CNCSJP2FileView::ParameterValuePair::ParameterValuePair(Parameter eParam, bool bValue)
{
	m_eParam = eParam;
	m_pValue = NCSMalloc(sizeof(bool), FALSE);
	*((bool*)m_pValue) = bValue;
}

CNCSJP2FileView::ParameterValuePair::ParameterValuePair(Parameter eParam, UINT32 nValue)
{
	m_eParam = eParam;
	m_pValue = NCSMalloc(sizeof(UINT32), FALSE);
	*((UINT32*)m_pValue) = nValue;
}

CNCSJP2FileView::ParameterValuePair::ParameterValuePair(Parameter eParam, IEEE4 fValue)
{
	m_eParam = eParam;
	m_pValue = NCSMalloc(sizeof(IEEE4), FALSE);
	*((IEEE4*)m_pValue) = fValue;
}

CNCSJP2FileView::ParameterValuePair::~ParameterValuePair()
{
	NCSFree(m_pValue);
}


void CNCSJP2FileView::ResetStatistic(Statistic eType)
{
	switch(eType) {
	case ST_CODEBLOCK_DECODER_US:
		CNCSJPCT1Coder::sm_usTotal = 0;
		break;
	case ST_CODEBLOCK_DECODER_SAMPLES:
		CNCSJPCT1Coder::sm_nTotalSamples = 0;
		break;
	case ST_CODEBLOCK_READ_US:
		CNCSJPCCodeBlock::sm_usTotal = 0;
		break;
	case ST_CODEBLOCK_READ_SAMPLES:
		CNCSJPCCodeBlock::sm_nTotalSamples = 0;
		break;
	case ST_DCSHIFT_US:
	case ST_MCT_US:
	case ST_DWT_ROW_US:
	case ST_DWT_COL_US:
	case ST_DWT_SAMPLES:
	case ST_VIEW_SETVIEW_US:
	case ST_VIEW_READLINE_US:
	case ST_VIEW_RESAMPLE_US:
	case ST_IO_READ_US:
	case ST_IO_WRITE_US:
	case ST_IO_SEEK_US:
		break;
	case ST_BUF_ALLOC_US:
		CNCSJPCBuffer::sm_usAlloc = 0;
		break;
	case ST_BUF_FREE_US:
		CNCSJPCBuffer::sm_usFree = 0;
		break;
	case ST_BUF_ALLOC_BYTES:
		CNCSJPCBuffer::sm_nAllocBytes = 0;
		break;
	case ST_BUF_COPY_US:
		CNCSJPCBuffer::sm_usCopy = 0;
		break;
	case ST_BUF_COPY_BYTES:
		CNCSJPCBuffer::sm_nCopyBytes = 0;
		break;
	case ST_BUF_CLEAR_US:
		CNCSJPCBuffer::sm_usClear = 0;
		break;
	case ST_BUF_CLEAR_BYTES:
		CNCSJPCBuffer::sm_nClearBytes = 0;
		break;
	default: break;
	}
}

void CNCSJP2FileView::ResetStatistics()
{
	ResetStatistic(ST_CODEBLOCK_DECODER_US);
	ResetStatistic(ST_CODEBLOCK_DECODER_SAMPLES);
	ResetStatistic(ST_CODEBLOCK_READ_US);
	ResetStatistic(ST_CODEBLOCK_READ_SAMPLES);
	ResetStatistic(ST_DCSHIFT_US);
	ResetStatistic(ST_DWT_ROW_US);
	ResetStatistic(ST_DWT_COL_US);
	ResetStatistic(ST_DWT_SAMPLES);
	ResetStatistic(ST_VIEW_SETVIEW_US);
	ResetStatistic(ST_VIEW_READLINE_US);
	ResetStatistic(ST_VIEW_RESAMPLE_US);
	ResetStatistic(ST_IO_READ_US);
	ResetStatistic(ST_IO_WRITE_US);
	ResetStatistic(ST_IO_SEEK_US);
	ResetStatistic(ST_BUF_ALLOC_US);
	ResetStatistic(ST_BUF_FREE_US);
	ResetStatistic(ST_BUF_ALLOC_BYTES);
	ResetStatistic(ST_BUF_COPY_US);
	ResetStatistic(ST_BUF_COPY_BYTES);
	ResetStatistic(ST_BUF_CLEAR_US);
	ResetStatistic(ST_BUF_CLEAR_BYTES);
}

/**
* Return the percentage of image remaining to be downloaded.
* A number from 0 to 100 representing the amount of image left to be downloaded
* @return The percentage complete value
*/
INT32 CNCSJP2FileView::GetPercentComplete()
{
	const NCSFileViewSetInfo *pViewInfo = GetFileViewSetInfo();
	if(pViewInfo) {
		if(pViewInfo->nBlocksInView) {
			//			int nBlocks = pViewInfo->nBlocksAvailable;
			return (INT32)NCSMin(100, (pViewInfo->nBlocksAvailable / (double)pViewInfo->nBlocksInView) * 100);
		} else {
			return(100);
			//			int nBlocks = (pViewInfo->nBlocksAvailable - pViewInfo->nBlocksAvailableAtSetView);
			//			return (INT32)((nBlocks / (double)(pViewInfo->nBlocksInView - pViewInfo->nBlocksAvailableAtSetView)) * 100);
		}
	}
	return 0;
}

/**
* Return the percentage of image remaining to be downloaded.
* A number from 0 to 100 representing the amount of image left to be downloaded
* @return The percentage complete value
*/
INT32 CNCSJP2FileView::GetPercentCompleteTotalBlocksInView()
{
	return(GetPercentComplete());
	//	const NCSFileViewSetInfo *pViewInfo = GetFileViewSetInfo();
	//	if(pViewInfo) {
	//		return (short)((pViewInfo->nBlocksAvailable / (double)pViewInfo->nBlocksInView) * 100);
	//	}
	//	return 0;
}

/**
* Get the EPSG code associated with this view's projection and datum, if any.
* @return		INT32			EPSG code, or 0 if none applies.
*/
INT32 CNCSJP2FileView::GetEPSGCode()
{
	INT32 nEPSG;
	const NCSFileViewFileInfoEx *pInfo = GetFileInfo();
	if (pInfo)
		if (CNCSJP2FileView::GetEPSGCode(pInfo->szProjection, pInfo->szDatum, &nEPSG) == NCS_SUCCESS)
			return nEPSG;
		return 0;
}

#ifdef NCSJPC_ECW_SUPPORT

NCSEcwReadStatus CNCSJP2FileView::sRefreshCallback(NCSFileView *pNCSFileView)
{
	CNCSJPCGlobalLock _Lock;
	
	std::vector<CNCSJP2FileView *>::iterator pCur = sm_Views.begin();
	std::vector<CNCSJP2FileView *>::iterator pEnd = sm_Views.end();
	
	while(pCur != pEnd) {
		CNCSJP2FileView *pView = *pCur;
		if(pView->m_pECWFileView == pNCSFileView) {
			NCSEcwReadStatus eError = pView->FireRefreshUpdate(_Lock);//pView->RefreshUpdateEx((NCSFileViewSetInfo*)pView->GetFileViewSetInfo());
			return(eError);
		}
		pCur++;
	}	
	return(NCSECW_READ_FAILED);
}

CNCSError CNCSJP2FileView::Write()
{
	CNCSError Error = NCS_INVALID_PARAMETER;
	
	m_tsCompressStart = NCSGetTimeStampMs();
	
	m_CompressFreeEvent.Set();
	m_CompressReadEvent.Set();
	//	hFreeEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	//	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	
	if(m_pECWCompressionTask && m_pECWCompressionTask->pQmf) {
	/*
	**	Compress the file
		*/
		NCSThreadResume(&(m_pECWCompressionTask->tThread));
		
		Error = build_qmf_compress_file(m_pECWCompressionTask->pQmf, m_pECWCompressionTask->OutputHandle);
		if(Error == NCS_USER_CANCELLED_COMPRESSION) {
			m_pECWCompressionTask->bKillThread = TRUE;
		}
	} else if(m_pFile) {
		Error = NCS_SUCCESS;
		void **ppInputArray = (void**)NCSMalloc(((m_eColorSpace == NCSCS_YUV) ? 3 : m_nNumberOfBands) * sizeof(void*), FALSE);
		bool bCancel = false;
		
		Resume();
		
		for(INT32 y = 0; y < m_nHeight && !bCancel && Error == NCS_SUCCESS && m_CompressError == NCS_SUCCESS; y++) {
			if(m_bCompressMTRead) {
				m_CompressFreeEvent.Wait(100);
				m_CompressQueueMutex.Lock();
				while(m_CompressFreeQueue.empty()) {
					m_CompressQueueMutex.UnLock();
					NCSThreadYield();
					m_CompressQueueMutex.Lock();
				}
			}
			CNCSJPCBufferVector *pBuffers = *m_CompressFreeQueue.begin();
			if(m_bCompressMTRead) {
				m_CompressFreeQueue.erase(m_CompressFreeQueue.begin());
				m_CompressQueueMutex.UnLock();
			}
			for(INT32 b = 0; b < ((m_eColorSpace == NCSCS_YUV) ? 3 : m_nNumberOfBands); b++) {
				(*pBuffers)[b].Alloc(0, y, (UINT32)m_nWidth, (UINT32)1, CNCSJPCBuffer::BT_IEEE4);
				ppInputArray[b] = (*pBuffers)[b].GetPtr();
			}
			Error = WriteReadLine(y, ppInputArray);
			if(Error == NCS_SUCCESS) {
				if(m_eColorSpace == NCSCS_YUV && m_eCellType == NCSCT_IEEE4) {
					// "C" based API allows the app to send in scanlines in YUV format, so we
					// have to convert them to RGB here first
					IEEE4 *pRY = (IEEE4*)ppInputArray[0];
					IEEE4 *pGU = (IEEE4*)ppInputArray[1];
					IEEE4 *pBV = (IEEE4*)ppInputArray[2];
					INT32 x = 0;
#ifdef NCSJPC_X86_MMI
					if(CNCSJPCBuffer::sm_SystemInfo.m_bSSEPresent && m_nWidth > 4) {
						INT32 nSubWidth = m_nWidth / 4;
						
						for(; x < nSubWidth; x++) {
							__m128 mm0_RY = _mm_loadu_ps(&(((IEEE4*)pRY)[x*4]));
							__m128 mm1_GU = _mm_loadu_ps(&(((IEEE4*)pGU)[x*4]));
							__m128 mm2_BV = _mm_loadu_ps(&(((IEEE4*)pBV)[x*4]));
							
							__m128 mm3_r = _mm_add_ps(mm0_RY, _mm_mul_ps(_mm_set1_ps(1.402f), mm2_BV));
							__m128 mm4_g = _mm_add_ps(_mm_add_ps(mm0_RY, _mm_mul_ps(_mm_set1_ps(-0.34414f), mm1_GU)), _mm_mul_ps(_mm_set1_ps(-0.71414f), mm2_BV));
							__m128 mm5_b = _mm_add_ps(mm0_RY, _mm_mul_ps(_mm_set1_ps(1.772f), mm1_GU));
							
							_mm_storeu_ps(&(((IEEE4*)pRY)[x*4]), mm3_r);
							_mm_storeu_ps(&(((IEEE4*)pGU)[x*4]), mm4_g);
							_mm_storeu_ps(&(((IEEE4*)pBV)[x*4]), mm5_b);
						}
						x *= 4;
					}
#endif
					for(; x < m_nWidth; x++) {
						IEEE4 r = pRY[x] + 1.402f * pBV[x];
						IEEE4 g = pRY[x] + (-0.34414f * pGU[x]) + (-0.71414f * pBV[x]);
						IEEE4 b = pRY[x] + (1.772f * pGU[x]);
						pRY[x] = r;
						pGU[x] = g;
						pBV[x] = b;
					}
				}
				WriteStatus(y);
				bCancel = WriteCancel();
			}
			if(m_bCompressMTRead) {
				m_CompressQueueMutex.Lock();
				m_CompressReadQueue.push_back(pBuffers);
				m_CompressQueueMutex.UnLock();
				m_CompressReadEvent.Set();
				if(bCancel) {
					while(!Stop(false) && IsRunning()) {
						m_CompressReadEvent.Set();
					}
				}
			} else {
				m_CompressError = WriteLineBIL(m_eCellType, (m_eColorSpace == NCSCS_YUV) ? 3 : m_nNumberOfBands, ppInputArray);
			}
			if(bCancel) {
				Error = NCS_USER_CANCELLED_COMPRESSION;
			}
		}
		NCSFree(ppInputArray);
		while(IsRunning()) NCSSleep(50);
	}
	m_tsCompressEnd = NCSGetTimeStampMs();
	return(m_CompressError != NCS_SUCCESS ? m_CompressError : Error);
}

// These are used for write (compression)
// you overload this read method, if you want progressive (pull-through)
CNCSError CNCSJP2FileView::WriteReadLine(UINT32 nNextLine, void **ppInputArray)
{
	if(m_pCompressClient && m_pCompressClient->pReadCallback) {
		return((m_pCompressClient->pReadCallback(m_pCompressClient, nNextLine, (IEEE4**)ppInputArray) == TRUE) ? NCS_SUCCESS : NCS_COULDNT_READ_INPUT_LINE);
	}
	return(NCS_SUCCESS);
}
// you overload this if you want a status callback to show progress during compression 
void CNCSJP2FileView::WriteStatus(UINT32 nCurrentLine)
{
	if(m_pCompressClient && m_pCompressClient->pStatusCallback) {
		m_pCompressClient->pStatusCallback(m_pCompressClient, nCurrentLine);
	}
}
// you overload this if you want to be able to cancel during compression. Return TRUE to cancel
bool CNCSJP2FileView::WriteCancel(void)
{
	if(m_pCompressClient && m_pCompressClient->pCancelCallback) {
		return(m_pCompressClient->pCancelCallback(m_pCompressClient) ? true : false);
	}
	return(false);
}

void CNCSJP2FileView::sECWCompressThreadReadFunc(void *pData)
{
	EcwCompressionTask *pTask = (EcwCompressionTask*)pData;
	
	if(pTask) {
		CNCSJP2FileView *pThis = (CNCSJP2FileView*)pTask->pClient;
		
		while((pTask->nNextInputLine < (UINT32)pThis->m_nHeight) && !pTask->bKillThread) {
			EcwCompressionQueueNode *pNode = (EcwCompressionQueueNode*)NULL;
			
			pThis->m_CompressFreeEvent.Wait(100);
			while(!pNode && !pTask->bKillThread) {
				pNode = (EcwCompressionQueueNode*)NCSQueueRemoveNode(pTask->pFreeQueue, (NCSQueueNode*)NULL);
				if(!pNode) {
					NCSThreadYield();
				}
			}
			if(pNode && !pTask->bKillThread) {
				pTask->eInputError = pThis->WriteReadLine(pTask->nNextInputLine, (void**)pNode->ppLines).GetErrorNumber();
				if(pTask->eInputError != NCS_SUCCESS) {
					return;
				}
				NCSQueueAppendNode(pTask->pReadQueue, (NCSQueueNode*)pNode);
				pThis->m_CompressReadEvent.Set();
				pTask->nNextInputLine++;
			}
		}
	}
}

NCSError CNCSJP2FileView::sECWCompressRead(void *pClient, UINT32 nNextLine, IEEE4 **ppInputArray)
{
	CNCSJP2FileView *pThis = (CNCSJP2FileView*)pClient;
	EcwCompressionTask *pTask = pThis->m_pECWCompressionTask;
	EcwCompressionQueueNode *pNode = (EcwCompressionQueueNode*)NULL;
	INT32 i;
	IEEE4 **ppInputLines = (IEEE4**)NULL;
	
	if(pThis->m_bCompressMTRead) {
		pThis->m_CompressReadEvent.Wait(100);
		while(!pNode) {
			pNode = (EcwCompressionQueueNode*)NCSQueueRemoveNode(pTask->pReadQueue, (NCSQueueNode*)NULL);
			if(!pNode) {
				NCSThreadYield();
			}
			if(!pNode && !NCSThreadIsRunning(&(pTask->tThread)) && (pTask->nNextInputLine < (UINT32)pThis->m_nHeight)) {
			/*
			** Thread has terminated early for some reason
				*/
				if(pTask->eInputError != NCS_SUCCESS) {
					return(pTask->eInputError);
				} else {
					return(NCS_COULDNT_READ_INPUT_LINE);
				}
			}
		}
		if(pNode) {
			ppInputLines = pNode->ppLines;
		}
	} else {
		if(pTask->eInternalFormat == (CompressFormat)pThis->m_eColorSpace && pThis->m_eCellType == NCSCT_IEEE4) {
			NCSError eError = pThis->WriteReadLine(nNextLine, (void**)ppInputArray).GetErrorNumber();
			
			if(eError != NCS_SUCCESS) {
				return(eError);
			}		
		} else {
			NCSError eError = pThis->WriteReadLine(nNextLine, (void**)pTask->ppLines).GetErrorNumber();
			
			if(eError != NCS_SUCCESS) {
				return(eError);
			}		
			ppInputLines = pTask->ppLines;
		}
	}
	
	if(pTask->eInternalFormat == (CompressFormat)pThis->m_eColorSpace) {
		if(ppInputLines) {
			if(pThis->m_eCellType == NCSCT_IEEE4) {
				for(i = 0; i < pThis->m_nNumberOfBands; i++) {
					// can't just fiddle pointers here as we didn't alloc the buffer
					memcpy(ppInputArray[i], ppInputLines[i], pThis->m_nWidth * sizeof(IEEE4));
				}
			} else {
				for(i = 0; i < pThis->m_nNumberOfBands; i++) {
					UINT32 x;
					IEEE4 *pDst = ppInputArray[i];
					UINT8 *pSrc = (UINT8*)ppInputLines[i];
					for(x = 0; x < (UINT32)pThis->m_nWidth; x++) {
						pDst[x] = pSrc[x];
					}
				}
			}
		}
	} else if((pTask->eInternalFormat == COMPRESS_YUV) &&
		((CompressFormat)pThis->m_eColorSpace == COMPRESS_RGB)) {
		IEEE4 *NCS_RESTRICT pY = ppInputArray[0];
		IEEE4 *NCS_RESTRICT pU = ppInputArray[1];
		IEEE4 *NCS_RESTRICT pV = ppInputArray[2];
		INT32 nInOutSizeX = pThis->m_nWidth;
		
		if(pThis->m_eCellType == NCSCT_IEEE4) {
			IEEE4 *NCS_RESTRICT pR = ppInputLines[0];
			IEEE4 *NCS_RESTRICT pG = ppInputLines[1];
			IEEE4 *NCS_RESTRICT pB = ppInputLines[2];
			
			for(i = 0; i < nInOutSizeX; i++) {
				register IEEE4 fRed = *(pR++);
				register IEEE4 fGreen = *(pG++);
				register IEEE4 fBlue = *(pB++);
				
				*(pY++) = ((0.299f * fRed) + (0.587f * fGreen) + (0.114f * fBlue));
				*(pU++) = ((-0.1687f * fRed) + (-0.3313f * fGreen) + (0.5f * fBlue));
				*(pV++) = ((0.5f * fRed) + (-0.4187f * fGreen) + (-0.0813f * fBlue));
			}
		} else {
			UINT8 *NCS_RESTRICT pR = (UINT8*)ppInputLines[0];
			UINT8 *NCS_RESTRICT pG = (UINT8*)ppInputLines[1];
			UINT8 *NCS_RESTRICT pB = (UINT8*)ppInputLines[2];
			
			for(i = 0; i < nInOutSizeX; i++) {
				register IEEE4 fRed = *(pR++);
				register IEEE4 fGreen = *(pG++);
				register IEEE4 fBlue = *(pB++);
				
				*(pY++) = ((0.299f * fRed) + (0.587f * fGreen) + (0.114f * fBlue));
				*(pU++) = ((-0.1687f * fRed) + (-0.3313f * fGreen) + (0.5f * fBlue));
				*(pV++) = ((0.5f * fRed) + (-0.4187f * fGreen) + (-0.0813f * fBlue));
			}
		}
	} else {
		return(NCS_INVALID_PARAMETER);
	}
	
	if(pThis->m_bCompressMTRead && pNode) {
		NCSQueueAppendNode(pTask->pFreeQueue, (NCSQueueNode*)pNode);
		pThis->m_CompressFreeEvent.Set();
	}
	return(NCS_SUCCESS);
}

void CNCSJP2FileView::sECWCompressStatus(void *pClient, UINT32 nCurrentLine)
{
	CNCSJP2FileView *pThis = (CNCSJP2FileView*)pClient;
	pThis->WriteStatus(nCurrentLine);
}

BOOLEAN CNCSJP2FileView::sECWCompressCancel(void *pClient)
{
	CNCSJP2FileView *pThis = (CNCSJP2FileView*)pClient;
	return(pThis->WriteCancel());
}

#endif // NCSJPC_ECW_SUPPORT

void CNCSJP2FileView::Work(void *pDate)
{
	void **ppLines = (void**)NCSMalloc(m_nNumberOfBands * sizeof(void*), FALSE);
	while(Run() && m_CompressError == NCS_SUCCESS && m_pFile->m_nNextLine != m_nHeight) {
		m_CompressReadEvent.Wait(100);
		m_CompressQueueMutex.Lock();
		while(Run() && m_CompressReadQueue.empty()) {
			m_CompressQueueMutex.UnLock();
			//			NCSSleep(50);
			NCSThreadYield();
			//		Suspend();
			m_CompressQueueMutex.Lock();
		}
		if(!Run()) {
			break;
		}
		CNCSJPCBufferVector *pBuffers = *m_CompressReadQueue.begin();
		m_CompressReadQueue.erase(m_CompressReadQueue.begin());
		m_CompressQueueMutex.UnLock();
		
		for(INT32 b = 0; b < m_nNumberOfBands; b++) {
			ppLines[b] = (*pBuffers)[b].GetPtr();
		}
		m_CompressError = WriteLineBIL(m_eCellType, m_nNumberOfBands, ppLines);
		
		m_CompressQueueMutex.Lock();
		m_CompressFreeQueue.push_back(pBuffers);
		m_CompressQueueMutex.UnLock();
		m_CompressFreeEvent.Set();
	}
	NCSFree(ppLines);
}

CNCSError CNCSJP2FileView::GetProjectionAndDatum(const INT32 nEPSGCode, 
												 char **ppProjection, 
												 char **ppDatum)
{
	CNCSJPCGlobalLock _Lock;
	CNCSGDTEPSG& Epsg = *CNCSGDTEPSG::Instance();
	char *pProj = NULL;
	char *pDat = NULL;
	if(Epsg.GetProjectionAndDatum(nEPSGCode, &pProj, &pDat).GetErrorNumber() != NCS_SUCCESS) {
		NCSFree(pProj);
		NCSFree(pDat);
		return(NCS_INCOMPATIBLE_COORDINATE_SYSTEMS);
	}
	NCSFree(*ppProjection);
	NCSFree(*ppDatum);
	*ppProjection = pProj;
	*ppDatum = pDat;
	return(NCS_SUCCESS);
}

CNCSError CNCSJP2FileView::GetEPSGCode(char *szProjection, 
									   char *szDatum, 
									   INT32 *pnEPSG)
{
	CNCSGDTEPSG& Epsg = *CNCSGDTEPSG::Instance();
	NCSError Error = NCS_INCOMPATIBLE_COORDINATE_SYSTEMS;
	*pnEPSG = 0;
	*pnEPSG = Epsg.GetEPSG(szProjection,szDatum);
	//if (*pnEPSG == 0) *pnEPSG = Epsg.GetGCSGeoKey(szDatum);
	if (*pnEPSG == 0)
	{
		char *pColon;
		if (strnicmp(szProjection,"epsg:",5) == 0)
		{
			pColon = strchr(szProjection,':');
			pColon++;
			*pnEPSG = atoi(pColon);
			Error = NCS_SUCCESS;
		}
		else if (strnicmp(szDatum,"epsg:",5) == 0)
		{
			pColon = strchr(szDatum,':');
			pColon++;
			*pnEPSG = atoi(pColon);
			Error = NCS_SUCCESS;
		}
	}
	else Error = NCS_SUCCESS;
	return CNCSError(Error);
}

void CNCSJP2FileView::SetGDTPath(const char *szPath)
{
	CNCSGDTEPSG::SetPath(szPath);
}

char *CNCSJP2FileView::GetGDTPath()
{
	CNCSGDTEPSG::Instance();
	return CNCSGDTEPSG::GetPath();
}

void CNCSJP2FileView::DetectGDTPath()
{
	CNCSGDTEPSG::Instance()->DetectPath();
}

NCSFileType CNCSJP2FileView::GetFileType()
{
	if(m_pECWFileView) {
		//ECW
		return NCS_FILE_ECW;
	} else if(m_pFile) {
		//JP2
		return NCS_FILE_JP2;
	} else {
		//unknown
		return NCS_FILE_UNKNOWN;
	}
}

char *CNCSJP2FileView::GetFileMimeType(NCSFileType fileType)
{
	switch( fileType ) {
	case NCS_FILE_ECW:
		return NCSStrDup("x-image/ecw");
	case NCS_FILE_JP2:
		return NCSStrDup("image/jp2");
	case NCS_FILE_UNKNOWN:
	default:
		return NULL;
	}
}

char *CNCSJP2FileView::GetFileMimeType()
{
	return GetFileMimeType( GetFileType() );
}
