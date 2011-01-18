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
** FILE:   	ncscbmnet.c
** CREATED:	1 March 1999
** AUTHOR: 	SNS
** PURPOSE:	NCS Client Block Manager network functions
** EDITS:
** [01] sns	03Mar99 Created file
** [02] sns 17May99 Handling invalid SetViews without crashing
** [03] dmm 30Aug99 Added synchronise logic
** [04] sjc 30Dec99 Added global lock for stats seg
** [05] sjc 30Dec99 Counter handling mods to fix asserts
** [06] sjc 30Dec99 Fixed Invalid requests bug
** [07] ny  12May00	Added connection status error propagation
** [08] sjc 15May00 Moved IDWT Queue inside mutex
** [09] sjc 05Jul00 Fixed memory leak of first returned packed (HDR)
** [10] rar 22-Jan-01 Mac port changes
** [11] rar 17-May-01 Modified NCScbmNetFileReceiveRequests() to include a returned error
**						code.  And cancel reading of ECW file on error.
** [12] rar 02-Sep-03 Added option to try ecwp re-connection if connection lost
** [13] tfl 02-Jul-04 Added additional file extensions to checking code in NCSEcwNetBreakdownURL
 *******************************************************/

#include "NCSTypes.h"

#include "NCSEcw.h"
#if !defined(_WIN32_WCE)
	#ifdef WIN32 //[10]
		#define _CRTDBG_MAP_ALLOC
		#include "crtdbg.h"
	#endif
#endif
#include "NCSUtil.h"
#include "NCSMalloc.h"

/*
** Define this if you want to run the RELEASE client against the DEBUG
** IWS server.
*/
#undef NCS_USE_DEBUG_SERVER

#if defined(_DEBUG)||defined(NCS_USE_DEBUG_SERVER)
#define NCS_IIS_DLL_NAME	"/ecwp/ecwpd.dll"
#else
#define NCS_IIS_DLL_NAME	"/ecwp/ecwp.dll"
#endif

#ifdef NCS_MINDEP_BUILD
void NCScnetDestroy(pNCSnetClient pClient) {};
BOOLEAN NCScnetSendPacket(pNCSnetClient pClient, void *pPacket, int nLength, void *pUserdata, BOOLEAN *bIsConnected) { return(FALSE); };
void NCScnetSetRecvCB(pNCSnetClient pClient, NCSnetPacketRecvCB *pRecvCB, void *pUserdata) {};
void NCScnetSetSentCB(pNCSnetClient pClient, NCSnetPacketSentCB *pSentCB) {};
NCSError NCScnetCreateEx(char *szURL, void **ppClient, void *pPacketIn, int nLengthIn, void **ppPacketOut, INT32 *pLengthOut, NCSnetPacketRecvCB *pRecvCB, void *pRecvUserdata, char *pszIISDLLName) { return(NCS_INVALID_PARAMETER); }
void NCScnetSetIISDLLName(char *szDLLname) {};
#endif //NCS_MINDEP_BUILD

static void	NCScbmNetFileReceiveRequests(NCSPacket *pPacket, INT32	nLength, pNCSnetClient pClientNetID, NCSFile *pNCSFile, NCSError eError ); /**[11]**/
static BOOLEAN NCScbmNetReceivedBlock(NCSFile *pNCSFile, NCSBlockId nBlock);

/*******************************************************
**	NCSecwNetBreakdownUrl()	- Returns pointers into the URL to each
**	part.  Returns TRUE if the file is a valid ECW file (or URL
**	to a ECW file) or FALSE if it is not a valid ECW file or URL.
**
**	Notes:
**
**	If Protocol or Host are NULL, then must be a local file
**	If a pointer is returned as NULL, then LENGTH for that pointer is undefined
**	These are pointers into the original string, so the strings are not null terminated
********************************************************/

#ifdef NCSJPC_ECW_SUPPORT
BOOLEAN NCSecwNetBreakdownUrl_ECW( char *szUrlPath,
						   char **ppProtocol, int *pnProtocolLength,
						   char **ppHost,	  int *pnHostLength,
						   char **ppFilename, int *pnFilenameLength)
#else
BOOLEAN NCSecwNetBreakdownUrl( char *szUrlPath,
						   char **ppProtocol, int *pnProtocolLength,
						   char **ppHost,	  int *pnHostLength,
						   char **ppFilename, int *pnFilenameLength)
#endif
{
	int	nMatch1=0;
	int nExtCount=0;
	BOOLEAN bECWPS = FALSE;
	//TODO: put this in sync with the allowable extensions defined in NCSJP2FileView.cpp
	char *JP2Exts[] = ERS_JP2_DATASET_EXTS; /*[13]*/

	if (!pNCSEcwInfo) {
		NCSecwInitInternal();
	}

	//Check if the protocol is ecwp: or ecwps: (SSL)
	if( strnicmp("ecwps:", szUrlPath, 6)==0 ) {
		bECWPS = TRUE;
		nMatch1 = 0;
	} else if( strnicmp("ecwp:", szUrlPath, 5)==0 ) {
		bECWPS = FALSE;
		nMatch1 = 0;
	} else {
		bECWPS = FALSE;
		nMatch1 = 1;
	}

	//nMatch1 = strnicmp("ECWP",szUrlPath,4);

	if( nMatch1 ) {
		// must be a local file
		if (!NCSIsLocalFile(szUrlPath))
			return(FALSE);
		*ppProtocol = *ppHost = NULL;
		*ppFilename = szUrlPath;
		*pnFilenameLength = (int)strlen(szUrlPath);

		//start [13]

		//if( *pnFilenameLength < (int) strlen(ERS_WAVELET_DATASET_EXT) )
		//	return( FALSE );		// not an ECW file
		nMatch1 = strnicmp(ERS_WAVELET_DATASET_EXT, 
						   &szUrlPath[*pnFilenameLength - strlen(ERS_WAVELET_DATASET_EXT)],
						   strlen(ERS_WAVELET_DATASET_EXT));
		if (nMatch1 == 0) return(TRUE); //an ECW file

		for (nExtCount = 0; nExtCount < sizeof(JP2Exts)/sizeof(JP2Exts[0]); nExtCount++)
		{
			nMatch1 = strnicmp(JP2Exts[nExtCount],
						 &szUrlPath[*pnFilenameLength - strlen(JP2Exts[nExtCount])],
						 strlen(JP2Exts[nExtCount]));
			if (nMatch1 == 0) return(TRUE); //valid extension found
		}
		return(FALSE); //valid extension wasn't found
		//end [13]
	}
	else {
		char	*pUrl;
		int		nSlashes = 3;

		*ppProtocol = szUrlPath;
		if( bECWPS ) {
			*pnProtocolLength = 6;
		}else {
			*pnProtocolLength = 5;
		}

		pUrl = *ppHost = &szUrlPath[*pnProtocolLength];
		while( *pUrl && nSlashes ) {
			if( *pUrl == '/' || *pUrl == '\\' ) {
				if( *pUrl == '\\' )
					*pUrl = '/';
				--nSlashes;		// skip over the "//host/" part of the string
			}
			pUrl++;
		}
		*pnHostLength = (int)(pUrl - *ppHost);
		*ppFilename = pUrl;	
		*pnFilenameLength = (int)strlen(szUrlPath);
	}
	return(TRUE);
}


/*******************************************************
**	NCScbmNetFileOpen() - open file from server
**	returns TRUE if the request failed for some reason
**	Internal routine - called by NCSecwFileOpen()
**	and returns the memory image of the file header
**	and updates the pNCSFile structure with the network Client ID
**	and the NCS Server Client UID.  If an error, NULL
**	is returned as the memory image, and the NCSFile is left
**	in a clean state.
**
*******************************************************/
NCSError NCScbmNetFileOpenInternal(UINT8 **ppHeaderMemImage, 
								   UINT32 *pnHeaderMemImageLen, 
								   pNCSnetClient *pClientID, 
								   NCSClientUID		*pClientUID,
								   NCSnetPacketSentCB *pSentCB,
								   NCSnetPacketRecvCB *pRecvCB,
								   void *pCBData, 
								   char *szUrlPath) //[12]
{
	INT32		iSize;
	NCSClientUID nUID = 0;
	NCSPacketType ptType = NCSPT_CONNECTION_OPEN;
	int			iOIPacketLength = 0;
	NCSPacket	*pPacket;
	NCSPacket	*pOIPacket = NULL;
	NCSFileHeader	*pHeaderMemImage = NULL;
	UINT32		iPacketSize;
	UINT8		nVersion = NCS_PACKET_VERSION;
	NCSError	nResult = NCS_UNKNOWN_ERROR;
	char		*pszProxyServer = (char*)NULL;
	UINT16 iHeaderLength=0; //[12]

	if (!ppHeaderMemImage)
		return(NCS_INVALID_ARGUMENTS);
	*ppHeaderMemImage = NULL;

	iSize = NCS_OPEN_PACKET_BASE_SIZE + (INT32)strlen(szUrlPath) + 1;
	pPacket = (NCSPacket*) NCSMalloc(iSize, FALSE);
	if (!pPacket)
		return(NCS_FILE_NO_MEMORY);

	// Construct the open packet
	NCS_PACKET_PACK_BEGIN(pPacket);
	NCS_PACKET_PACK_ELEMENT(iSize);
	NCS_PACKET_PACK_ELEMENT(nUID);
	NCS_PACKET_PACK_ELEMENT(ptType);
	NCS_PACKET_PACK_ELEMENT(nVersion);

	if( strnicmp( szUrlPath, "ecwps:", 6 ) == 0 ) {
		//Check if the protocol is ecwps
		//If using SSL the url in the open packet must start with ecwp: not ecwps: as IWS
		// doesn't know what to do if it is ecwps:
		char szEcwpUrlPath[MAX_PATH] = "ecwp:";
		strcat( &szEcwpUrlPath[5], &szUrlPath[6] );

		NCS_PACKET_PACK_ELEMENT_SIZE(szEcwpUrlPath[0], strlen(szEcwpUrlPath) + 1);

	} else {
		NCS_PACKET_PACK_ELEMENT_SIZE(szUrlPath[0], strlen(szUrlPath) + 1);
	}

	NCS_PACKET_PACK_END(pPacket);

	NCScnetSetIISDLLName(NCS_IIS_DLL_NAME);

	if( NCSPrefGetUserString(NCS_ECW_PROXY_KEY, &pszProxyServer) != NCS_SUCCESS ) {
		pszProxyServer = NULL;
	}

	if( pszProxyServer && (pszProxyServer[0]!=0) ) {
		char szServer[MAX_PATH];

		sprintf(szServer, "ecwp://%s", pszProxyServer);
#ifdef _DEBUG
		nResult = NCScnetCreateEx(szServer, pClientID, pPacket, iSize, (void**)&pOIPacket, (int*)&iOIPacketLength, (void	(*)(void*, INT32, void*, void*, NCSError))NCScbmNetFileReceiveRequests, pCBData, "/ecwp/ecwp.dll");
#else
		nResult = NCScnetCreateEx(szServer, pClientID, pPacket, iSize, (void**)&pOIPacket, (int*)&iOIPacketLength, (void	(*)(void*, INT32, void*, void*, NCSError))NCScbmNetFileReceiveRequests, pCBData, "/ecwp/ecwp.dll");
#endif
		NCSFree(pszProxyServer);
	} else {
#ifdef _DEBUG
		nResult = NCScnetCreateEx(szUrlPath, pClientID, pPacket, iSize, (void**)&pOIPacket, (int*)&iOIPacketLength, (void	(*)(void*, INT32, void*, void*, NCSError))NCScbmNetFileReceiveRequests, pCBData, "/ecwp/ecwp.dll");
#else
		nResult = NCScnetCreateEx(szUrlPath, pClientID, pPacket, iSize, (void**)&pOIPacket, (int*)&iOIPacketLength, (void	(*)(void*, INT32, void*, void*, NCSError))NCScbmNetFileReceiveRequests, pCBData, "/ecwp/ecwp.dll");
#endif
	}
	NCSFree(pPacket);
	if(nResult != NCS_SUCCESS) {
		return (nResult);
	}
	if (!*pClientID ) {
		return(NCS_ECW_ERROR);
	}
	if( iOIPacketLength == 0 || pOIPacket == NULL ) {
		// got a response, but not one we liked, so abort with error (can this actually happen?)
		if( pOIPacket )
			NCSFree(pOIPacket);
		NCScnetDestroy(*pClientID);
		*pClientID = NULL;
		return(NCS_ECW_ERROR);
	}
	NCS_PACKET_UNPACK_BEGIN(pOIPacket);
	NCS_PACKET_UNPACK_ELEMENT(iPacketSize);
	NCS_PACKET_UNPACK_ELEMENT(*pClientUID);
	NCS_PACKET_UNPACK_ELEMENT(ptType);

	if(ptType == NCSPT_CONNECTION_OPEN_INFO) {

		NCS_PACKET_UNPACK_ELEMENT(nVersion);
		NCS_PACKET_UNPACK_ELEMENT(iHeaderLength);

		if(iHeaderLength) {
			pHeaderMemImage = NCSMalloc(iHeaderLength, FALSE);
			if (pHeaderMemImage) {
				NCS_PACKET_UNPACK_ELEMENT_SIZE(pHeaderMemImage[0], iHeaderLength);
			}
			else
				nResult = NCS_FILE_NO_MEMORY;
		}
	}
	else if (ptType == NCSPT_CONNECTION_OPEN_FAIL) {
		UINT8 nFailType;
		UINT32 nLimit;
		static NCSError nError[] = {
			NCS_SERVER_ERROR,
			NCS_FILE_OPEN_FAILED,
			NCS_FILE_LIMIT_REACHED,
			NCS_FILE_SIZE_LIMIT_REACHED,
			NCS_CLIENT_LIMIT_REACHED,
			NCS_INCOMPATIBLE_PROTOCOL_VERSION
		};

		NCS_PACKET_UNPACK_ELEMENT(nFailType);
		NCS_PACKET_UNPACK_ELEMENT(nLimit);

		// map from packet based connection failure codes to NCSError codes
		nResult = (/*nFailType >= NCSOF_SERVER_FAILURE  && */ nFailType < NCSOF_END_MARKER) ? nError[nFailType] : NCS_UNKNOWN_ERROR;
	}
	else
		nResult = NCS_PACKET_TYPE_ILLEGAL;
		
	NCS_PACKET_UNPACK_END(pOIPacket);
	
	NCSFree(pOIPacket);	/**[09]**/

	if( !pHeaderMemImage ) {
		NCScnetDestroy(*pClientID);
		*pClientID = NULL;
	}
	else {
		NCScnetSetSentCB(*pClientID, pSentCB);
		NCScnetSetRecvCB(*pClientID, pRecvCB, pCBData);
		nResult = NCS_SUCCESS;
		*ppHeaderMemImage = pHeaderMemImage;
		*pnHeaderMemImageLen = iHeaderLength; //[12]
	}
	return(nResult);
}

NCSError NCScbmNetFileOpen(UINT8 **ppHeaderMemImage, UINT32 *pnHeaderMemImageLen, NCSFile *pNCSFile, char *szUrlPath) //[12]
{
	return(NCScbmNetFileOpenInternal(ppHeaderMemImage, 
									 pnHeaderMemImageLen, 
									 &(pNCSFile->pClientNetID), 
									 &pNCSFile->nClientUID,
									 (NCSnetPacketSentCB*)NCScbmNetFileXmitRequests, 
									 (NCSnetPacketRecvCB*)NCScbmNetFileReceiveRequests, 
									 (void*)pNCSFile, 
									 szUrlPath));
}

/*******************************************************
**	NCScbmNetFileXmitRequests() - Xmit block requests and/or cancels
**
*******************************************************/
void	NCScbmNetFileXmitRequests(NCSError nError, UINT8 *pLastPacketSent, NCSFile *pNCSFile )
{
	NCSecwStatistics *pStatistics = pNCSEcwInfo->pStatistics;
	UINT32		iPacketSize;
	NCSPacketType ptType = NCSPT_CONNECTION_DATA;
	UINT32		iPacketSpaceLeft = NCSECW_MAX_SEND_PACKET_SIZE - NCS_DATA_PACKET_BASE_SIZE;
	UINT16		*pCancelCount, *pRequestCount;
	UINT16		nCancelCount, nRequestCount;
	NCSPacket	*pPacket;
	NCSFileCachedBlock	*pBlock;
	NCSConnectionDataRequestType	ptBlockRange = NCSRT_SINGLE_BLOCKS;
	NCSFile *pNextNCSFile;
	pLastPacketSent;nError;//Keep compiler happy
	
	NCSMutexBegin(&pNCSEcwInfo->mutex);
	// Must make sure the file pointer is still valid.
	// Have to be paranoid here, even if at expense of performance.
	pNextNCSFile = pNCSEcwInfo->pNCSFileList;
	while( pNextNCSFile ) {
		if( pNextNCSFile == pNCSFile )
			break;		// found the file
		pNextNCSFile = pNextNCSFile->pNextNCSFile;
	}
	if( !pNextNCSFile ) {
		// somehow got a callback for a file that no longer exists
		NCSMutexEnd(&pNCSEcwInfo->mutex);
		return;				// nothing more to send for the file
	}

	pNCSFile->bSendInProgress = FALSE;
	if( !pNCSFile->nRequestsXmitPending && !pNCSFile->nCancelsXmitPending ) {
		NCSMutexEnd(&pNCSEcwInfo->mutex);
		return;				// nothing more to send for the file
	}

	if( !pNCSFile->pSendPacket ) {
		// If first send, need to allocate the send packet
		pNCSFile->pSendPacket = (NCSPacket *) NCSMalloc( NCSECW_MAX_SEND_PACKET_SIZE + 1 , FALSE);
		if( !pNCSFile->pSendPacket ) {
			NCSMutexEnd(&pNCSEcwInfo->mutex);
			return;			// ERROR - out of memory. Perhaps we could do a cache flush here?
		}
	}
	pPacket = pNCSFile->pSendPacket;

	// Now construct the packet. Insert fake values for Packet Size, # of Cancels and # of requests
	// and then come back and update them before sending the packet

	iPacketSize = 0;			// Inserted after packet is constructed
	nCancelCount = 0;			// Inserted after packet is constructed
	nRequestCount = 0;			// Inserted after packet is constructed


	NCS_PACKET_PACK_BEGIN(pPacket);
	NCS_PACKET_PACK_ELEMENT(iPacketSize);
	NCS_PACKET_PACK_ELEMENT(pNCSFile->nClientUID);
	NCS_PACKET_PACK_ELEMENT(ptType);
	NCS_PACKET_PACK_ELEMENT(pNCSFile->nClientSequence);
	NCS_PACKET_PACK_ELEMENT(ptBlockRange);

	// Point to Cancel Count location, and insert fake value for now
	pCancelCount = (UINT16 *) pLocalData;
	NCS_PACKET_PACK_ELEMENT(nCancelCount);

	_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

	// Insert Cancels into the packet
	pBlock = pNCSFile->pFirstCachedBlock;
	while( pBlock && pNCSFile->nCancelsXmitPending && iPacketSpaceLeft >= sizeof(NCSBlockId) ) {
		if(pBlock->bRequested && !pBlock->nUsageCount ) {
			NCSEcwStatsLock();

			_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

			nCancelCount += 1;
			pNCSFile->nCancelsXmitPending -= 1;
			NCSEcwStatsDecrement(&pStatistics->nCancelsXmitPending, 1);
			NCSEcwStatsIncrement(&pStatistics->nCancelsSent, 1);
			pBlock->bRequested = FALSE;
			NCS_PACKET_PACK_ELEMENT(pBlock->nBlockNumber);
			iPacketSpaceLeft -= sizeof(NCSBlockId);

			_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

			NCSEcwStatsUnLock();
		}
		pBlock = pBlock->pNextCachedBlock;
	}
	if( !pBlock && (pNCSFile->nCancelsXmitPending != 0) ) {
		_RPT1(_CRT_WARN, "NCScbmNetFileXmitRequests: warning nCancelsXmitPending (=%d) non-zero after scanning all cached blocks\n", pNCSFile->nCancelsXmitPending);
		pNCSFile->nCancelsXmitPending = 0;		// ERROR - reset count to zero
	}

	_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

	// Point to Request Count location, and insert fake value for now
	pRequestCount = (UINT16 *) pLocalData;
	NCS_PACKET_PACK_ELEMENT(nRequestCount);
	// Insert Requests into the packet
	pBlock = pNCSFile->pFirstCachedBlock;
	while( pBlock && pNCSFile->nRequestsXmitPending && iPacketSpaceLeft >= sizeof(NCSBlockId) ) {
		if(!pBlock->bRequested && !pBlock->pPackedECWBlock && pBlock->nUsageCount ) {
			NCSEcwStatsLock();

			_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

			nRequestCount += 1;
			pNCSFile->nRequestsXmitPending -= 1;
			NCSEcwStatsDecrement(&pStatistics->nRequestsXmitPending, 1);
			NCSEcwStatsIncrement(&pStatistics->nRequestsSent, 1);
			pBlock->bRequested = TRUE;
			NCS_PACKET_PACK_ELEMENT(pBlock->nBlockNumber);
			iPacketSpaceLeft -= sizeof(NCSBlockId);

			_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

			NCSEcwStatsUnLock();
		}
		pBlock = pBlock->pNextCachedBlock;
	}
	_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

	if( !pBlock && (pNCSFile->nRequestsXmitPending != 0) ) {
		_ASSERT(pNCSFile->nRequestsXmitPending == 0);
		pNCSFile->nRequestsXmitPending = 0;		// ERROR - reset count to zero
	}

	// Update size and counts
#ifdef NCSBO_MSBFIRST		
	iPacketSize = ((UINT8 *) pLocalData) - ((UINT8 *) pPacket);
	pPacket->iPacketSize = NCSByteSwap32(iPacketSize);
	*pCancelCount = NCSByteSwap16(nCancelCount);
	*pRequestCount = NCSByteSwap16(nRequestCount);
#else
	iPacketSize = (UINT32)(((UINT8 *) pLocalData) - ((UINT8 *) pPacket));
	pPacket->iPacketSize = iPacketSize;
	*pCancelCount = nCancelCount;
	*pRequestCount = nRequestCount;
#endif //MACINTOSH

	NCS_PACKET_PACK_END(pPacket);

	// Send the packet if something to send
	if( nCancelCount || nRequestCount ) {
		BOOLEAN bIsConnected;																			/**[07]**/
		if( NCScnetSendPacket(pNCSFile->pClientNetID, pPacket, iPacketSize, pNCSFile,&bIsConnected) ) { /**[07]**/
			pNCSFile->bSendInProgress = TRUE;
			pNCSFile->nClientSequence += 1;
		} else																							/**[07]**/
			pNCSFile->bIsConnected = FALSE;//bIsConnected;														/**[07]**/
	}

	NCSMutexEnd(&pNCSEcwInfo->mutex);
}


/*******************************************************
**	NCScbmNetFileReceiveRequests() - Receives blocks
**	from the server, and updates file and file views with the
**	information.
**
*******************************************************/

static void	NCScbmNetFileReceiveRequests(NCSPacket *pPacket, INT32	nLength,
										 pNCSnetClient pClientNetID, NCSFile *pNCSFile, NCSError eError )
{
	BOOLEAN		bTriggerRefreshCallbacks = FALSE;
	NCSFileCachedBlock	*pBlock;
	pClientNetID;nLength;//Keep compiler happy
	
	NCSMutexBegin(&pNCSEcwInfo->mutex);

	// Check returned error code
	if( eError != NCS_SUCCESS ) {		/**[11]**/
		pNCSFile->bIsConnected = FALSE;
		//if( pPacket ) free(pPacket);
		NCSMutexEnd(&pNCSEcwInfo->mutex);
		return;
	}

	if( pPacket ) {
		NCSFile	*pNextNCSFile;

		// Must make sure the file pointer is still valie
		pNextNCSFile = pNCSEcwInfo->pNCSFileList;
		while( pNextNCSFile ) {
			if( pNextNCSFile == pNCSFile )
				break;		// found the file
			pNextNCSFile = pNextNCSFile->pNextNCSFile;
		}
		if( pNextNCSFile ) {
			UINT32		iPacketSize;
			NCSPacketType ptType;
			NCSClientUID	nClientUID;

			// have a valid file, so now unpack the blocks read
			NCS_PACKET_UNPACK_BEGIN(pPacket);
			NCS_PACKET_UNPACK_ELEMENT(iPacketSize);
			NCS_PACKET_UNPACK_ELEMENT(nClientUID);
			NCS_PACKET_UNPACK_ELEMENT(ptType);

			switch( ptType ) {
					case NCSPT_BLOCKS: {
						UINT16 nBlocks;
						UINT32 i;

						NCS_PACKET_UNPACK_ELEMENT(nBlocks);
						/*
						** Unpack block data
						*/
						for(i = 0; i < nBlocks; i++) {
							NCSBlockId nBlock;
							UINT32 nBlockLength;
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
							Handle pECWBlock;
							OSErr osErr;
#else
							UINT8 *pECWBlock;
#endif
							NCS_PACKET_UNPACK_ELEMENT(nBlock);
							NCS_PACKET_UNPACK_ELEMENT(nBlockLength);
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
							pECWBlock = TempNewHandle( nBlockLength, &osErr );
							NCS_PACKET_UNPACK_ELEMENT_SIZE((*pECWBlock)[0], nBlockLength);							
#else
							pECWBlock = NCSMalloc(nBlockLength, FALSE);
							NCS_PACKET_UNPACK_ELEMENT_SIZE(pECWBlock[0], nBlockLength);
#endif

							NCSEcwStatsLock();
							NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nBlocksReceived, 1);
							NCSEcwStatsUnLock();

							_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

							pBlock = pNCSFile->pLastReceivedCachedBlock = 
									NCScbmGetCacheBlock(pNCSFile, 
														pNCSFile->pLastReceivedCachedBlock, 
														nBlock, NCSECW_BLOCK_RETURN);
#ifdef NOTDEF
							//_DEBUG
							{
NCSFileCachedBlock	*pCachedBlock = pNCSFile->pFirstCachedBlock;

while(pCachedBlock) {
	if(pCachedBlock->nBlockNumber == nBlock) {
		break;
	}
	if(pCachedBlock->bRequested && pCachedBlock->nUsageCount && !pCachedBlock->pPackedECWBlock && !pCachedBlock->pUnpackedECWBlock) {
		MessageBox(NULL, "requested but don't have", "DEBUG", MB_OK);
	}
	pCachedBlock = pCachedBlock->pNextCachedBlock;
}


							}
#endif
							if( pBlock ) {
								// If this was in the Cancel Xmit list, cancel the cancel request
								if(!pBlock->nUsageCount ) {
									NCSEcwStatsLock();

									if( pBlock->bRequested) {
										pNCSFile->nCancelsXmitPending -= 1;
										NCSEcwStatsDecrement(&pNCSEcwInfo->pStatistics->nCancelsXmitPending, 1);
									} else {
										// block was previously cancelled but still received
										NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nCancelledBlocksReceived, 1);
									}
									NCSEcwStatsUnLock();
								} else if(!pBlock->bRequested && !pBlock->pPackedECWBlock) { // FIXME XXX SJC
									NCSEcwStatsLock();

									pNCSFile->nRequestsXmitPending -= 1;
									NCSEcwStatsDecrement(&pNCSEcwInfo->pStatistics->nRequestsXmitPending, 1);
									NCSEcwStatsUnLock();
								}

								// add ECW block to the list
								pBlock->bRequested = FALSE;
								if( !pBlock->pPackedECWBlock ) {
									if(align_ecw_block(pNCSFile, nBlock, 
													   &pBlock->pPackedECWBlock, 
													   &pBlock->nPackedECWBlockLength, 
													   pECWBlock, 
													   nBlockLength) == 0) {
										NCSEcwStatsLock();
										//if( pNCSEcwInfo->pStatistics )
										NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nPackedBlocksCacheSize, pBlock->nPackedECWBlockLength);
										bTriggerRefreshCallbacks |= 
										NCScbmNetReceivedBlock(pNCSFile, nBlock);	// update FileViews
										NCSEcwStatsUnLock();
									}
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
									DisposeHandle( pECWBlock);
#else
									NCSFree(pECWBlock);//[19]
#endif
								}
								else
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
									DisposeHandle( pECWBlock );
#else
									NCSFree(pECWBlock);	// ERROR - it is already there
#endif
							}
							else {
#if defined( MACINTOSH ) && TARGET_API_MAC_OS8
								DisposeHandle( pECWBlock );
#else
								NCSFree(pECWBlock);
#endif
							}
							_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));
						}
					}
					break;
				case NCSPT_FLOW_CONTROL:
// FIXME!! Add flow control logic
				break;

				case NCSPT_SYNCHRONISE: /* [03] */
				{
					UINT32 nOutOfSync = 0;
					NCSFileCachedBlock *pCachedBlock = pNCSFile->pFirstCachedBlock;
					NCSEcwStatsLock();

					NCSEcwStatsDecrement(&pNCSEcwInfo->pStatistics->nCancelsXmitPending, pNCSFile->nCancelsXmitPending);
					pNCSFile->nCancelsXmitPending = 0;

					NCSEcwStatsDecrement(&pNCSEcwInfo->pStatistics->nRequestsXmitPending, pNCSFile->nRequestsXmitPending);
					pNCSFile->nRequestsXmitPending = 0;

					NCSEcwStatsUnLock();

					_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

					NCS_PACKET_UNPACK_ELEMENT(pNCSFile->nServerSequence);
					while (pCachedBlock) {
						if (pCachedBlock->bRequested) {
							NCSEcwStatsLock();

//							_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

							if(pCachedBlock->nUsageCount == 0) {	/**[05]**/
								pNCSFile->nCancelsXmitPending++;	/**[05]**/
								NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nCancelsXmitPending, 1);	/**[05]**/
							} else {	/**[05]**/
								pCachedBlock->bRequested = FALSE;
								pNCSFile->nRequestsXmitPending++;
								NCSEcwStatsIncrement(&pNCSEcwInfo->pStatistics->nRequestsXmitPending, 1);
							}	/**[05]**/
							nOutOfSync++;						
//							_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));

							NCSEcwStatsUnLock();
						}
						pCachedBlock = pCachedBlock->pNextCachedBlock;
					}
					if (nOutOfSync && !pNCSFile->bSendInProgress ) { /**[06]**/
						_RPT1(_CRT_WARN, "NCScbmNetFileReceiveRequests: sync: %d blocks out\n", nOutOfSync);
						NCScbmNetFileXmitRequests(NCS_SUCCESS, NULL, pNCSFile);		// fake callback to pick up rerequests
					}
					else {
						_RPT0(_CRT_WARN, "NCScbmNetFileReceiveRequests: in sync\n");
					}
					_ASSERT(NCScbmSanityCheckFileView(pNCSFile->pNCSFileViewList));
				}
				break;

				default:
					// ERROR unknown packet - ignore it
				break;
			}

			NCS_PACKET_UNPACK_END(pPacket);
		}
		NCSFree(pPacket);
	}
	

	// Now go and do the refresh callbacks for all File Views that time has passed
	// for.  We do this after everything so that (a) we unpacked as many blocks
	// as possible before starting the client and (b) we can do it outside the MUTEX
	// in case more blocks are on their way.
	// [08] moved inside mutex - sjc
	if( bTriggerRefreshCallbacks ) {
		NCSFileView	*pNCSFileView = pNCSFile->pNCSFileViewList;
		while( pNCSFileView ) {
			if( pNCSFileView->bTriggerRefreshCallback ) {
				pNCSFileView->bTriggerRefreshCallback = FALSE;
				NCScbmQueueIDWTCallback(pNCSFileView, NCSECW_QUEUE_LIFO);
			}
			pNCSFileView = pNCSFileView->pNextNCSFileView;
		}
	}
	NCSMutexEnd(&pNCSEcwInfo->mutex);
}



/*******************************************************
**	NCScbmNetReceivedBlock() - For each FileView in the
**	File, updates the blocks received (for the fileview)
**	if the block fell into that fileview's view.
**
**	NOTES:
**	(1)	Assumes caller has done the MUTEX (although
**		not really critical for this routine as currently
**		implemented)
**	(2)	Returns TRUE if this block should trigger a refresh
**		in the FileView.
**
********************************************************/
static BOOLEAN NCScbmNetReceivedBlock(NCSFile *pNCSFile, NCSBlockId nBlock)
								  
{
	NCSFileView	*pNCSFileView = pNCSFile->pNCSFileViewList;
	NCSTimeStampMs	tLastBlockTime = NCSGetTimeStampMs();
	BOOLEAN		bTriggerRefreshCallbacks = FALSE;

	while( pNCSFileView ) {
		QmfRegionStruct *pQmfRegion = pNCSFileView->pQmfRegion;
		UINT16					nLevel = 0;
		if( pQmfRegion ) {	// [02] don't process if no region specified at present
			// Traverse from smallest to largest level,
			// working out if the block is in this level of this FileView's region
			while( nLevel <= pQmfRegion->p_largest_qmf->level ) {
				QmfRegionLevelStruct	*pLevel = &(pQmfRegion->p_levels[nLevel]);
				QmfLevelStruct			*pQmf = pLevel->p_qmf;

				UINT32					nStartXBlock, nEndXBlock, nStartYBlock, nEndYBlock;
				UINT32					nFileBlock;

				nStartXBlock	= pLevel->start_x_block;
				nEndXBlock		= (nStartXBlock + pLevel->x_block_count) - 1;
				nStartYBlock	= pLevel->level_start_y / pQmf->y_block_size;
				nEndYBlock		= pLevel->level_end_y   / pQmf->y_block_size;


				nFileBlock = pQmf->nFirstBlockNumber +
							(pQmf->nr_x_blocks * nStartYBlock) + nStartXBlock;
				// if File block is > than nBlock, then nBlock can't be in this FileView
				if( nFileBlock > nBlock )
					break;				// Break out of this FileView's Level loop
				nFileBlock = pQmf->nFirstBlockNumber +
							(pQmf->nr_x_blocks * nEndYBlock) + nEndXBlock;
				if( nFileBlock < nBlock ) {
					nLevel++;
					continue;			// Might be in a larger level, so try next larger level
				}
				// work out if the block is in the row for this level
				nFileBlock = nBlock - pQmf->nFirstBlockNumber;
				nFileBlock = nFileBlock % pQmf->nr_x_blocks;
				if( nFileBlock < nStartXBlock || nFileBlock > nEndXBlock )
					break;				// not in this region, so go on to next fileview

				// We have a valid block loaded for this FileView
				pNCSFileView->info.nBlocksAvailable += 1;

				// Now, if there was a refresh function defined, we see if enough time
				// has passed to warrent telling them that a refresh is worth doing
				// [02] only trigger this if the callback has not already been done or queued
				if( pNCSFileView->pRefreshCallback
				 && pNCSFileView->eCallbackState == NCSECW_VIEW_SET
				 && pNCSFileView->bTriggerRefreshCallback == FALSE ) {
					if( (pNCSFileView->tLastBlockTime > tLastBlockTime)		// 46 day wrap on counter
					 || ((pNCSFileView->tLastBlockTime + pNCSEcwInfo->pStatistics->nRefreshTime) < tLastBlockTime)
					 || (pNCSFileView->info.nBlocksAvailable == pNCSFileView->info.nBlocksInView) ) {
						// note the last time a block came in for this view
						pNCSFileView->tLastBlockTime = tLastBlockTime;
						pNCSFileView->bTriggerRefreshCallback = TRUE;
						bTriggerRefreshCallbacks = TRUE;
					}
				}
				// the File View has a new block available, so quit search for this File View
				break;	
			}
		}	// [02]

		// move on to the next FileView
		pNCSFileView = pNCSFileView->pNextNCSFileView;
	}
	return(bTriggerRefreshCallbacks);
}
