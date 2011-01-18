/**********************************************************
** Copyright 1998 Earth Resource Mapping Ltd.
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
** FILE:   	compress.cpp
** CREATED:	2004
** AUTHOR: 	SJC
** PURPOSE:	Entry points for NCSECWC.dll library
** EDITS:
 *******************************************************/

#include "NCSEcw.h"
#include "NCSECWCompress.h"
#include "NCSFile.h"

#ifndef ECW_COMPRESS_SDK_VERSION
#include "NCSJP2File.h"
#endif

/**************************************************************************/

/* Allocate a new CompressionClient structure and fill in defaults */
extern "C" NCSEcwCompressClient *NCSEcwCompressAllocClient(void)
{
	NCSEcwCompressClient *pClient;

	if((pClient = (NCSEcwCompressClient*)NCSMalloc(sizeof(NCSEcwCompressClient), TRUE)) != NULL) {
		pClient->fTargetCompression	= 10;
		pClient->eCompressFormat	= COMPRESS_UINT8;
		pClient->eCompressHint		= COMPRESS_HINT_INTERNET;
		pClient->nBlockSizeX		= X_BLOCK_SIZE;
		pClient->nBlockSizeY		= Y_BLOCK_SIZE;

		pClient->fCellIncrementX = 1.0;
		pClient->fCellIncrementY = 1.0;
		pClient->fOriginX = 0.0;
		pClient->fOriginY = 0.0;
		pClient->eCellSizeUnits = ECW_CELL_UNITS_METERS;

		strcpy(pClient->szDatum, "RAW");
		strcpy(pClient->szProjection, "RAW");
	}
	return(pClient);
}

/* Free the CompressionClient structure */
extern "C" NCSError NCSEcwCompressFreeClient(NCSEcwCompressClient *pClient)
{
	NCSError rval = NCS_SUCCESS;
				
	if(pClient) {
		if(pClient->pTask) {
			rval = NCSEcwCompressClose(pClient);
			if(rval != NCS_SUCCESS) {
				return(rval);
			}
		}
		NCSFree(pClient);
		return(NCS_SUCCESS);
	}
	return(NCS_INVALID_PARAMETER);
}

/* Open the compression */
extern "C" NCSError NCSEcwCompressOpen(NCSEcwCompressClient *pClient,
							BOOLEAN bCalculateSizesOnly)
{
	NCSError eError;
	CNCSFile *pFile = new CNCSFile;

	if(pClient->szOutputFilename[0] == '\0') {
		strcpy(pClient->szOutputFilename, pClient->szInputFilename);
		char *pTmp = &(pClient->szOutputFilename[strlen(pClient->szOutputFilename) - 1]);

		while((pTmp >= pClient->szOutputFilename) && *pTmp) {
			if(*pTmp == '.') {
				strcpy(pTmp, ERS_WAVELET_DATASET_EXT);
				break;
			}
			pTmp--;
		}
		if(pTmp <= pClient->szOutputFilename) {
			strcat(pClient->szOutputFilename, ERS_WAVELET_DATASET_EXT);
		}
	}
	pClient->nInputSize = pClient->nInOutSizeX * pClient->nInOutSizeY * pClient->nInputBands;

	if(bCalculateSizesOnly) {
		// this is a guess only for now
		pClient->nOutputSize = (((UINT64) pClient->nInOutSizeX) * ((UINT64) pClient->nInOutSizeY) 
						  * ((UINT64) pClient->nInputBands)) / (UINT64) pClient->fTargetCompression;
		eError = NCS_SUCCESS;
	} else {
		NCSFileViewFileInfoEx Info;
		Info.eCellSizeUnits = pClient->eCellSizeUnits;
		Info.eCellType = NCSCT_IEEE4;
		Info.fCellIncrementX = pClient->fCellIncrementX;
		Info.fCellIncrementY = pClient->fCellIncrementY;
		Info.fOriginX = pClient->fOriginX;
		Info.fOriginY = pClient->fOriginY;
		Info.nBands = pClient->nInputBands;
		Info.nCompressionRate = (UINT16)pClient->fTargetCompression;
		Info.nSizeX = pClient->nInOutSizeX;
		Info.nSizeY = pClient->nInOutSizeY;
		Info.pBands = NULL;
		Info.szDatum = pClient->szDatum;
		Info.szProjection = pClient->szProjection;
		Info.eColorSpace = (NCSFileColorSpace)pClient->eCompressFormat;
		pFile->SetFileInfo(Info);
#ifndef ECW_COMPRESS_SDK_VERSION
		CNCSJP2File::SetKeySize();
#endif
		eError = pFile->Open(pClient->szOutputFilename, FALSE, TRUE);
		if(eError == NCS_SUCCESS) {
			pFile->SetCompressClient(pClient);
			pClient->pTask = (EcwCompressionTask*)pFile;
			pClient->nOutputBands = pFile->m_nNumberOfBands;
		}
	}
	return(eError);
}

	/* Run the compression - pReadCallback() called to read input lines */
extern "C" NCSError NCSEcwCompress(NCSEcwCompressClient *pClient)
{
	if(pClient && pClient->pTask) {
		CNCSFile *pFile = (CNCSFile*)pClient->pTask;
		return(pFile->Write().GetErrorNumber());
	} else {
		return(NCS_INVALID_PARAMETER);
	}
}

/* Close the compression */
extern "C" NCSError NCSEcwCompressClose(NCSEcwCompressClient *pClient)
{
	NCSError eError = NCS_INVALID_PARAMETER;
	if(pClient && pClient->pTask) {
		CNCSFile *pFile = (CNCSFile*)pClient->pTask;
		NCSTimeStampMs tsTotal = 1;
		pFile->GetStatistic(CNCSFile::ST_COMPRESS_TOTAL_MS, tsTotal);

		eError = pFile->Close(TRUE); 
		pClient->pTask = NULL;
		delete pFile;

		pClient->nOutputSize = NCSFileSizeBytes(OS_STRING(pClient->szOutputFilename));
		pClient->fCompressionSeconds = NCSMax(((IEEE8)tsTotal / 1000.0), 0.0001);
		pClient->fActualCompression = (IEEE4)((IEEE8)(INT64)pClient->nInputSize / (INT64)pClient->nOutputSize);
		pClient->fCompressionMBSec = (IEEE8) ((INT64) (pClient->nInputSize / (1024 * 1024))) / pClient->fCompressionSeconds;
	}
	return(eError);
}

extern "C" NCSError NCSEcwEditReadInfo(char *pFilename, NCSEcwEditInfo **ppInfo)
{
	NCSError eError = NCS_SUCCESS;

	if(ppInfo) {
		NCSEcwEditInfo *pInfo = (NCSEcwEditInfo*)NULL;
		NCSFile *pFile = (NCSFile*)NULL;

		eError = NCSecwOpenFile(&pFile,
								pFilename,
								FALSE, 
								FALSE);

		if(eError == NCS_SUCCESS) {
			pInfo = (NCSEcwEditInfo*)NCSMalloc(sizeof(NCSEcwEditInfo), TRUE);

			if(pInfo) {
				pInfo->nVersion = pFile->pTopQmf->p_file_qmf->version;
				pInfo->bCompressedOffsetTable = pFile->pTopQmf->bRawBlockTable ? FALSE : TRUE;
		
				pInfo->eCellSizeUnits = pFile->pFileInfo->eCellSizeUnits;
				
				pInfo->fCellIncrementX = pFile->pFileInfo->fCellIncrementX;
				pInfo->fCellIncrementY = pFile->pFileInfo->fCellIncrementY;
				pInfo->fOriginX = pFile->pFileInfo->fOriginX;
				pInfo->fOriginY = pFile->pFileInfo->fOriginY;

				pInfo->szDatum = NCSStrDup(pFile->pFileInfo->szDatum);
				pInfo->szProjection = NCSStrDup(pFile->pFileInfo->szProjection);
			} else {
				eError = NCS_COULDNT_ALLOC_MEMORY;
			}
			// Force it to close
			pFile->bValid = FALSE;
			NCSecwCloseFile(pFile);
		}
		*ppInfo = pInfo;
	} else {
		eError = NCS_INVALID_PARAMETER;
	}
	return(eError);
}

extern "C" NCSError NCSEcwEditWriteInfo(char *pFilename, NCSEcwEditInfo *pInfo, void (*pProgressFunc)(UINT64 nTotal, UINT64 nWritten, void *pClientData), BOOLEAN (*pCancelFunc)(void *pClientData), void *pClientData)
{
	NCSError eError = NCS_SUCCESS;

	if(pInfo) {
		// Update HDR
		NCS_FILE_HANDLE hFile = NCS_NULL_FILE_HANDLE;

		eError = NCSFileOpen(OS_STRING(pFilename), NCS_FILE_READ_WRITE, &hFile);
		
		if(eError == NCS_SUCCESS) {
			NCSEcwEditInfo *pECWInfo = (NCSEcwEditInfo*)NULL;
			char buf[ECW_MAX_DATUM_LEN];

			UINT32 nWritten = 0;

			NCSFileSeekNative(hFile, 24, NCS_FILE_SEEK_START);
			
			NCSFileWriteUINT8_MSB(hFile, (UINT8)pInfo->eCellSizeUnits);	// UINT8  CellSizeUnits
			
			NCSFileWriteIEEE8_LSB(hFile, pInfo->fCellIncrementX);		// IEEE8 fCellIncrementX
			NCSFileWriteIEEE8_LSB(hFile, pInfo->fCellIncrementY);		// IEEE8 fCellIncrementY
			NCSFileWriteIEEE8_LSB(hFile, pInfo->fOriginX);				// IEEE8 fOriginX
			NCSFileWriteIEEE8_LSB(hFile, pInfo->fOriginY);				// IEEE8 fOriginY
			
			memset(buf, 0, sizeof(buf));
			strncpy(buf, pInfo->szDatum, sizeof(buf));
			NCSFileWrite(hFile, buf, ECW_MAX_DATUM_LEN, &nWritten);		// char *Datum
			memset(buf, 0, sizeof(buf));
			strncpy(buf, pInfo->szProjection, sizeof(buf));
			NCSFileWrite(hFile, buf, ECW_MAX_DATUM_LEN, &nWritten); // char *Projection
		
			NCSFileClose(hFile);

			eError = NCSEcwEditReadInfo(pFilename, &pECWInfo);
			if(eError == NCS_SUCCESS) {		
				if(pECWInfo->bCompressedOffsetTable && !pInfo->bCompressedOffsetTable) {
					// Decompress block table
					// This means copying the file to a new one...
					INT64 nCurrentSize = NCSFileSizeBytes(OS_STRING(pFilename));
					NCSFile *pFile = (NCSFile*)NULL;

					eError = NCSecwOpenFile(&pFile,
											pFilename,
											FALSE, 
											TRUE);
					if(eError == NCS_SUCCESS) {
						NCS_FILE_HANDLE hFile = pFile->pTopQmf->hEcwFile.hFile;
						UINT32 nCompressedSize = 0;

						NCSFileSeekNative(hFile, pFile->pTopQmf->nHeaderMemImageLen, NCS_FILE_SEEK_START);
						
						eError = NCSFileReadUINT32_MSB(hFile, &nCompressedSize);
						
						if(eError == NCS_SUCCESS) {
							INT64 nNewSize = nCurrentSize - nCompressedSize + get_qmf_tree_nr_blocks(pFile->pTopQmf) * sizeof(UINT64) + 1;
							char szDir[MAX_PATH];
							char *p;
							char szOutputDir[MAX_PATH] = {'\0'};
							INT64 nAvailable;
							BOOLEAN bUsingTempDir = FALSE;

							strcpy(szDir, pFilename);
							p = &(szDir[strlen(szDir) - 1]);
							while(p > szDir && *p != '\\') {
								p--;
							}
							*p = '\0';

							nAvailable = NCSFreeDiskSpaceBytes(szDir, NULL);
							if(nAvailable < nNewSize) {
								// No space to copy file, check $TMP
								char *pTempDir = NCSGetTempDirectory();

								if(pTempDir) {
									nAvailable = NCSFreeDiskSpaceBytes(szDir, NULL);
									if(nAvailable > nNewSize) {
										strcpy(szOutputDir, pTempDir);
										bUsingTempDir = TRUE;
									}
									NCSFree(pTempDir);
								}
							} else {
								strcpy(szOutputDir, szDir);
							}
							if(szOutputDir[0] != '\0') {
								char *pTempFileName = NCSGetTempFileName(szOutputDir,
																		 "ECW",
																		 ERS_WAVELET_DATASET_EXT);

									// Force it to close
								pFile->bValid = FALSE;
								NCSecwCloseFile(pFile);
								
								eError = NCSecwOpenFile(&pFile,
														pFilename,
														TRUE, 
														TRUE);
								hFile = pFile->pTopQmf->hEcwFile.hFile;

								if(eError == NCS_SUCCESS) {
									UINT8 *pBuffer = (UINT8*)NCSMalloc(NCSMax(256*1024, pFile->pTopQmf->nHeaderMemImageLen), FALSE);

									if(pBuffer) {
										NCS_FILE_HANDLE hTempFile = NCS_NULL_FILE_HANDLE;

										eError = NCSFileOpen(OS_STRING(pTempFileName), NCS_FILE_CREATE|NCS_FILE_READ_WRITE, &hTempFile);

										if(eError == NCS_SUCCESS) {
											UINT32 nLen = pFile->pTopQmf->nHeaderMemImageLen;

											NCSFileSeekNative(hFile, 0, NCS_FILE_SEEK_START);

											eError = NCSFileRead(hFile, pBuffer, nLen, &nLen);
											if(eError == NCS_SUCCESS) {
												eError = NCSFileWrite(hTempFile, pBuffer, nLen, &nLen);
												if(eError == NCS_SUCCESS) {
													nLen = get_qmf_tree_nr_blocks(pFile->pTopQmf) * sizeof(UINT64);
	
													eError = NCSFileWriteUINT32_MSB(hTempFile, nLen + 1);
													if(eError == NCS_SUCCESS) {
														eError = NCSFileWriteUINT8_MSB(hTempFile, ENCODE_RAW);
														if(eError == NCS_SUCCESS) {
#ifdef NCSBO_MSBFIRST
															NCSByteSwapRange64((UINT64*)pFile->pTopQmf->p_block_offsets, 
																			   (UINT64*)pFile->pTopQmf->p_block_offsets, nLen / sizeof(pFile->pTopQmf->p_block_offsets[0]));
#endif
															eError = NCSFileWrite(hTempFile, pFile->pTopQmf->p_block_offsets, nLen, &nLen);
#ifdef NCSBO_MSBFIRST
															NCSByteSwapRange64((UINT64*)pFile->pTopQmf->p_block_offsets, 
																			   (UINT64*)pFile->pTopQmf->p_block_offsets, nLen / sizeof(pFile->pTopQmf->p_block_offsets[0]));
#endif
				
															if(eError == NCS_SUCCESS) {
																UINT64 nToWrite;

																NCSFileSeekNative(hFile, pFile->pTopQmf->nHeaderMemImageLen + nCompressedSize + sizeof(UINT32), NCS_FILE_SEEK_START);
															
																nToWrite = nCurrentSize - NCSFileTellNative(hFile);
																while(nToWrite) {
																	UINT32 nThisRead = (UINT32)NCSMin(256*1024, nToWrite);

																	eError = NCSFileRead(hFile, pBuffer, nThisRead, &nThisRead);
																	if(eError != NCS_SUCCESS) {
																		break;
																	}
																	eError = NCSFileWrite(hTempFile, pBuffer, nThisRead, &nThisRead);
																	if(eError != NCS_SUCCESS) {
																		break;
																	}
																	nToWrite -= nThisRead;

																	if(pCancelFunc && (*pCancelFunc)(pClientData) != FALSE) {
																		// Cancelled!
																		break;
																	} else if(pProgressFunc) {
																		(*pProgressFunc)(nNewSize, nNewSize - nToWrite, pClientData);
																	}
																}
																if(nToWrite != 0) {
																	eError = NCS_FILEIO_ERROR;
																}
															}
														}
													}
												}
											}
											NCSFileClose(hTempFile);
										}
										NCSFree(pBuffer);
									} else {
										eError = NCS_COULDNT_ALLOC_MEMORY;
									}
								}
								if(eError == NCS_SUCCESS) {
									// Force it to close
									pFile->bValid = FALSE;
									NCSecwCloseFile(pFile);
									pFile = NULL;
				
									// remove original file
									NCSDeleteFile(pFilename);

									// rename() can rename from a different directory...
									if(NCSRenameFile(pTempFileName, pFilename)) {
										// leave the temp file since we deleted the original!
										eError = NCS_FILEIO_ERROR;
									}
								} else {
									NCSDeleteFile(pTempFileName);
								}
								NCSFree(pTempFileName);
							} else {
								eError = NCS_FILEIO_ERROR;
							}
						}
						if(pFile) {
								// Force it to close
							pFile->bValid = FALSE;
							NCSecwCloseFile(pFile);
						}
					}
				} 
				NCSEcwEditFreeInfo(pECWInfo);
			}
		}
	} else {
		eError = NCS_INVALID_PARAMETER;
	}
	return(eError);

}

extern "C" NCSError NCSEcwEditFreeInfo(NCSEcwEditInfo *pInfo)
{
	if(pInfo) {
		NCSFree(pInfo->szDatum);
		NCSFree(pInfo->szProjection);
		NCSFree(pInfo);
		return(NCS_SUCCESS);
	}
	return(NCS_INVALID_PARAMETER);
}
