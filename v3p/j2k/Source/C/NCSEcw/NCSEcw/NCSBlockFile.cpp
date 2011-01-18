/********************************************************** 
** Copyright 2004 Earth Resource Mapping Ltd.
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
** FILE:   	NCSBlockFile.c
** CREATED:	30 June 2004
** AUTHOR: 	Simon Cope
** PURPOSE:	C++ class wrapper for a "block" file
** EDITS:
*******************************************************/

#include "NCSBlockFile.h"

CNCSBlockFile::CNCSBlockFile()
{
	m_pJP2File = NULL;
	m_pJP2MemImage = NULL;
	m_pNCSFile = NULL;
#ifdef NCS_ODBC
	m_hODBCEnv = NULL;
	m_hODBCCon = NULL;
	m_hODBCCmd = NULL;
#endif
}

CNCSBlockFile::~CNCSBlockFile()
{
	Close();
}

CNCSError CNCSBlockFile::Open(char *pFilename, 
							  bool bHeaderOnly)
{
	CNCSError Error;

	if( pFilename ) m_sFileName = pFilename;

	Error = NCSecwOpenFile(&m_pNCSFile, pFilename, bHeaderOnly ? FALSE : TRUE, TRUE);
	if(Error != NCS_SUCCESS) {
		m_pJP2File = new CNCSJP2File();

		Error = m_pJP2File->Open(pFilename);
		if(Error != NCS_SUCCESS) {
			delete m_pJP2File;
			m_pJP2File = NULL;
#ifdef NCS_ODBC
			SQLRETURN   retcode;

				  /*Allocate environment handle */
			retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hODBCEnv);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			   /* Set the ODBC version environment attribute */
			   retcode = SQLSetEnvAttr(m_hODBCEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0); 

			   if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				  /* Allocate connection handle */
				  retcode = SQLAllocHandle(SQL_HANDLE_DBC, m_hODBCEnv, &m_hODBCCon); 

				  if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					 /* Set login timeout to 5 seconds. */
					 //SQLSetConnectAttr(m_hODBCCon, SQL_LOGIN_TIMEOUT, (void*)5, 0);

					 /* Connect to data source */
					 retcode = SQLConnect(m_hODBCCon, 
										  NCS_T("localhost"), SQL_NTS,
										  NCS_T("root"), SQL_NTS,
										  NCS_T(""), SQL_NTS);

					 if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO){
						/* Allocate statement handle */
						retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hODBCCon, &m_hODBCCmd); 
					 }
				  }
			   }
			}
			if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
				Close();
				Error = NCS_FILE_OPEN_FAILED;
			} else {
				Error = NCS_SUCCESS;
			}
#endif
		}
	}
	return(Error);
}

CNCSError CNCSBlockFile::Close()
{
	if(m_pNCSFile) {
		if(m_pNCSFile->nUsageCount == 1) {
			// Ref count is currently 1, clear valid flag (indicated to ECW SDK to close the file for real,
			// rather than cache it internally
			m_pNCSFile->bValid = FALSE;
		}
		NCSecwCloseFile(m_pNCSFile);
		m_pNCSFile = NULL;
	}
	if(m_pJP2File) {
		m_pJP2File->Close(true);
		delete m_pJP2File;
		m_pJP2File = NULL;
	}
	if( m_pJP2MemImage ) {
		NCSFree( m_pJP2MemImage );
		m_pJP2MemImage = NULL;
	}
#ifdef NCS_ODBC
	if (m_hODBCCmd != NULL) {
		SQLFreeHandle(SQL_HANDLE_STMT, m_hODBCCmd);
		m_hODBCCmd = NULL;
	}
	if(m_hODBCCon != NULL) {
		SQLDisconnect(m_hODBCCon);
		SQLFreeHandle(SQL_HANDLE_DBC, m_hODBCCon);
		m_hODBCCon = NULL;
	}
	if(m_hODBCEnv != NULL) {
	   SQLFreeHandle(SQL_HANDLE_ENV, m_hODBCEnv);
	}
#endif
	return(NCS_SUCCESS);
}

bool CNCSBlockFile::HaveBlockTable()
{
	if(m_pNCSFile && m_pNCSFile->bReadOffsets && m_pNCSFile->pTopQmf &&
			(m_pNCSFile->pTopQmf->p_block_offsets || m_pNCSFile->pTopQmf->bRawBlockTable)) {
		return(true);
	} else if(m_pJP2File) {
		return(true);
#ifdef NCS_ODBC
	} else if(m_hODBCCmd != NULL && m_hODBCCon != NULL && m_hODBCEnv != NULL) {
	   return(true);
#endif
	}
	return(false);
}

// Get the memory image of the file.
void *CNCSBlockFile::GetMemImage(UINT32 &nLength)
{
	if(m_pNCSFile) {
		if(m_pNCSFile->pTopQmf->pHeaderMemImage) {
			nLength = m_pNCSFile->pTopQmf->nHeaderMemImageLen;
			return(m_pNCSFile->pTopQmf->pHeaderMemImage);
		}
	} else if(m_pJP2File) {
		if( m_pJP2MemImage ) {
			NCSFree( m_pJP2MemImage );
		}
		m_pJP2MemImage = m_pJP2File->GetMemImage(nLength);
		return m_pJP2MemImage;
	}
	return(NULL);
}

// Get length of a packet in the file.
UINT32 CNCSBlockFile::GetPacketLength(NCSBlockId nPacket)
{
	if(HaveBlockTable()) {
		if(m_pNCSFile) {
			UINT32 nLength = 0;
			NCScbmGetFileBlockSizeLocal(m_pNCSFile, nPacket, &nLength, NULL);
			return(nLength);
		} else if(m_pJP2File) {
			return(m_pJP2File->GetPacketLength(nPacket));
		}
	}
	return(0);
}

// Read a packet from the file.
void *CNCSBlockFile::GetPacket(NCSBlockId nPacket, UINT32 &nLength)
{
	if(HaveBlockTable()) {
		if(m_pNCSFile) {
			return((void*)NCScbmReadFileBlockLocal_ECW(m_pNCSFile, nPacket, &nLength));
		} else if(m_pJP2File) {
			return(m_pJP2File->GetPacket(nPacket, nLength));
		}
	}
	return(NULL);
}

// Get number of a packet in the file.
UINT32 CNCSBlockFile::GetNrPackets()
{
	if(m_pNCSFile) {
		return(get_qmf_tree_nr_blocks(m_pNCSFile->pTopQmf) - 2);
	} else if(m_pJP2File) {
		return(m_pJP2File->GetNrPackets());
	}
	return(0);
}

NCSFileViewFileInfoEx *CNCSBlockFile::GetFileInfo()
{
	if(m_pNCSFile) {
		return(m_pNCSFile->pTopQmf->pFileInfo);
	} else if(m_pJP2File) {
		return(&m_pJP2File->m_FileInfo);
	} 
	return(NULL);
}

//extern "C" UINT8 *NCScbmConstructZeroBlock(QmfLevelStruct *p_qmf, UINT32 *pLength);

// Create a zero blocks for ECW
// 
// NCScbmConstructZeroBlock does not create the zero block correctly?
// It crashes the browser.
//
UINT8 *ConstructZeroBlock(QmfLevelStruct *p_qmf, UINT32 &nSize )
{
	UINT32	nSidebands;
	UINT8	nSideband;
	UINT8	*pZeroBlock, *pZeroBlockSideband;
	UINT8	*pZeroBlock32;

	if( p_qmf->level )
		nSidebands = p_qmf->nr_sidebands - 1;
	else
		nSidebands = p_qmf->nr_sidebands;
	nSidebands = nSidebands * p_qmf->nr_bands;
	// we need room for N-1 UINT32's of sidebands, and N bytes of compression (zero block flags)
	nSize = (sizeof(UINT32) * (nSidebands-1)) + nSidebands;
	pZeroBlock = (UINT8*)NCSMalloc(nSize, FALSE);
	if( !pZeroBlock )
		return( NULL );
	pZeroBlock32 = pZeroBlock;
	pZeroBlockSideband = pZeroBlock + (sizeof(UINT32) * (nSidebands - 1));
	*pZeroBlockSideband++ = ENCODE_ZEROS;	// one more entry than offsets

	nSideband = (UINT8)nSidebands;
	while(--nSideband) {
		*pZeroBlock32++ = 0;	// 0xFF000000
		*pZeroBlock32++ = 0;	// 0x00FF0000
		*pZeroBlock32++ = 0;	// 0x0000FF00
		*pZeroBlock32++ = ((UINT8)nSidebands - nSideband);	// 0x000000FF
		*pZeroBlockSideband++ = ENCODE_ZEROS;
	}
	return(pZeroBlock);
}

void *CNCSBlockFile::GetZeroPacket(UINT32 &nLength)
{
	if(m_pNCSFile) {
		//return((void*)NCScbmConstructZeroBlock(m_pNCSFile->pTopQmf->p_larger_qmf, &nLength));
		return((void*)ConstructZeroBlock(m_pNCSFile->pTopQmf->p_larger_qmf, nLength));
	} else if(m_pJP2File) {
		return((void*)NCSMalloc(1, TRUE));	
	}
	return(NULL);
}

std::vector<CNCSBlockFile::ResolutionLevel> CNCSBlockFile::GetLevels()
{
	std::vector<ResolutionLevel> Levels;

	if(m_pNCSFile) {
		Levels.resize(m_pNCSFile->pTopQmf->p_file_qmf->nr_levels);
		
		UINT32 nPacketNum = 0;

		QmfLevelStruct *pLevelQMF = m_pNCSFile->pTopQmf;
		for(UINT8 r = 0; (r < Levels.size()) && pLevelQMF; r++) {
			ResolutionLevel &Level = Levels[r];
			
			Level.m_nLevel = r;
			Level.m_nWidth = pLevelQMF->x_size*2;
			Level.m_nHeight = pLevelQMF->y_size*2;
			Level.m_nBlockWidth = pLevelQMF->x_block_size*2;
			Level.m_nBlockHeight = pLevelQMF->y_block_size*2;
			Level.m_nBlocksWide = pLevelQMF->nr_x_blocks;
			Level.m_nBlocksHigh = pLevelQMF->nr_y_blocks;
			
			Level.m_nNrPackets = Level.m_nBlocksWide * Level.m_nBlocksHigh;
			Level.m_nFirstPacketNr = nPacketNum;

			nPacketNum += Level.m_nNrPackets;

			pLevelQMF = pLevelQMF->p_larger_qmf;
		}
	} else if(m_pJP2File) {
		CNCSJPCTilePartHeader *pTP0 = m_pJP2File->m_Codestream.GetTile(0);
		Levels.resize(pTP0->m_Components[0]->m_Resolutions.size());

		UINT32 nPacketNum = pTP0->GetFirstPacketNr();
		UINT32 nNumComponents = (UINT32)pTP0->m_Components.size();

		for(UINT8 r = 0; r < Levels.size(); r++) {
			ResolutionLevel &Level = Levels[r];
			CNCSJPCResolution *pResolution = pTP0->m_Components[0]->m_Resolutions[r];
			
			Level.m_nLevel = r;
			Level.m_nWidth = pResolution->GetWidth();
			Level.m_nHeight = pResolution->GetHeight();
			Level.m_nBlockWidth = pResolution->GetPrecinctWidth();
			Level.m_nBlockHeight = pResolution->GetPrecinctHeight();
			Level.m_nBlocksWide = pResolution->GetNumPrecinctsWide();
			Level.m_nBlocksHigh = pResolution->GetNumPrecinctsHigh();

			Level.m_nNrPackets = Level.m_nBlocksWide * Level.m_nBlocksHigh * nNumComponents;
			Level.m_nFirstPacketNr = nPacketNum;

			nPacketNum += Level.m_nNrPackets;
		}

		_ASSERT( nPacketNum == pTP0->GetNrPackets() );

	}
	return(Levels);
}

bool CNCSBlockFile::AdvancedSecurityCompatible( bool &bIsJP2, CNCSJPCProgressionOrderType::Type &po, std::string &sError ) {
	bIsJP2 = false;

	if(m_pJP2File) {
		bIsJP2 = true;

		// Must be only one tile
		if( (m_pJP2File->m_Codestream.GetNumXTiles() * m_pJP2File->m_Codestream.GetNumYTiles()) == 1 ) {
			CNCSJPCTilePartHeader *pTP0 = m_pJP2File->m_Codestream.GetTile(0);
			// Must be all one progression order
			if( pTP0 && pTP0->m_Components[0] && (pTP0->m_POC.m_Progressions.size() == 0) ) {
				CNCSJPCCODMarker::StyleParameters *pStyle = &pTP0->m_Components[0]->m_CodingStyle.m_SGcod;
				
				po = pStyle->m_ProgressionOrder.m_eType;
				//Check that the progression order is one of LRCP/RLCP/RPCL
				if( (po == CNCSJPCProgressionOrderType::LRCP) ||
					(po == CNCSJPCProgressionOrderType::RLCP) ||
					(po == CNCSJPCProgressionOrderType::RPCL) )
				{
					// Must have only one quality layer
					if( pStyle->m_nLayers <= 1 ) { 
						return true;
					} else {
						sError = "it contains more than 1 quality layer";
					}
				} else {
					sError = "its progression order is not LRCP, RLCP or RPCL";
				}
			} else {
				sError = "it contains more than one progression order";
			}
		} else {
			sError = "it contains more than one tile";
		}

		return false;
	} else if(m_pNCSFile) {
		//ECW
		return true;
	}

	return false;
};

NCSFileType CNCSBlockFile::GetFileType()
{
	if(m_pNCSFile) {
		//ECW
		return NCS_FILE_ECW;
	} else if(m_pJP2File) {
		//JP2
		return NCS_FILE_JP2;
	} else {
		//unknown
		return NCS_FILE_UNKNOWN;
	}
}
