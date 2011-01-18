/********************************************************
** Copyright 1999 Earth Resource Mapping Ltd.
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
** FILE:   	NCScnet2\packet.c
** CREATED:	
** AUTHOR: 	Simon Cope
** PURPOSE:	packet stuff
** EDITS:
** [01] sjc 25Feb00 Added support for polling GET connections
** [02] rar 14May01 Client sends ping to server every 60secs (if no other sends done).  So server can
**					tell when a client dies.
 *******************************************************/

#include "cnet.h"

#if !defined(_WIN32_WCE)
	#include "NCSWinHttp.h"
#endif

/*
** Send a packet
*/
NCSError NCScnetSendRawPacket(NCScnetConnection *pConn,
							  void *pData,
							  INT32 nDataLength,
							  BOOLEAN *pbCancelSend)
{
	void *pFullData;
	NCSError eError = NCS_SUCCESS;
	pbCancelSend;//Keep compiler happy

	if(NCScnetConnected(pConn, TRUE)) {
		NCScnetDisconnect(pConn, TRUE);
	}

	if((pFullData = NCSMalloc(nDataLength + sizeof(nDataLength), FALSE)) != NULL) {
		INT32 nRetryCount = 2;

		memcpy(pFullData, &nDataLength, sizeof(nDataLength));
		memcpy((UINT8*)pFullData + sizeof(nDataLength), pData, nDataLength);

		while(nRetryCount-- > 0) {
			eError = NCScnetConnectToServer(pConn,
											TRUE,
											pFullData,
											nDataLength + sizeof(nDataLength));
			if(eError == NCS_SUCCESS) {
				pConn->tsLastSend = NCSGetTimeStampMs();		/**[02]**/
				break;
			}
		}
		NCSFree(pFullData);

		if(eError == NCS_SUCCESS) {
			NCScnetDisconnect(pConn, TRUE);
			return(eError);
		}
	} else {
		eError = NCS_COULDNT_ALLOC_MEMORY;
	}
	return(eError);
}

/*
** Receive a packet
*/
NCSError NCScnetRecvRawPacket(NCScnetConnection *pConn,
							  void **ppData,
							  INT32 *pDataLength,
							  BOOLEAN *pbCancelRecv)
{
	void *pData = (void*)NULL;
	char *pChunk = NULL;

	UINT32 nCommand = 0;
	INT32 nLength = 0;

	INT32 nTotalLength = 0;
	INT32 nTotalRead = 0;

	BOOLEAN bAgain = FALSE;
#ifdef ECWP_POLLING_SUPPORT
	BOOLEAN bPollServer = TRUE;		/**[01]**/
#endif

	do {
#ifdef ECWP_POLLING_SUPPORT
		if(pConn->bIsPollingConnection && bPollServer) {	/**[01]**/
			DWORD dwError = 200;							/**[01]**/
			do { 											/**[01]**/
				DWORD nErrorLength = sizeof(dwError);		/**[01]**/
				DWORD nFieldNr = 0;							/**[01]**/
				NCSError eError;
															/**[01]**/
				eError = NCScnetPollServer(pConn);					/**[01]**/
				if(eError == NCS_SUCCESS) {					/**[01]**/
#if !defined(_WIN32_WCE)
					if (NCSWinHttpIsService()) {
                        if (NCSWinHttpGetConnectionStatusCode(pConn->hRecvResource, &dwError)) {
                            if(dwError == 204) NCSSleep(250);   
                        } else {
                            eError = NCS_NET_PACKET_RECV_FAILURE;
                        }
                    }
                    else {
#endif
                        if(pHttpQueryInfo(pConn->hRecvResource,     /**[01]**/
                                      HTTP_QUERY_STATUS_CODE |      /**[01]**/
                                        HTTP_QUERY_FLAG_NUMBER,     /**[01]**/
                                      &dwError,                     /**[01]**/
                                      &nErrorLength,                /**[01]**/
                                      &nFieldNr)) {                 /**[01]**/
                            if(dwError == 204) {                    /**[01]**/
    //NCScnetDisconnect(pConn, FALSE);      
                                NCSSleep(250);                      /**[01]**/
        //NCScnetConnectToServer(pConn, FALSE, (void*)NULL, 0);
                            }
                        } else {
                            eError = NCS_NET_PACKET_RECV_FAILURE;
                        }
#if !defined(_WIN32_WCE)
                    }
#endif
				}
				if(eError != NCS_SUCCESS) {
					NCSFree(pData);
					if(!pConn->bEndThreads) {
						NCSLog(LOG_MED, "NCScnetRecvRawPacket died: nTotalLength = %d, nTotalRead = %d, nCommand = %d, nLength = %d", nTotalLength, nTotalRead, nCommand, nLength);
					}
					return eError;
				}											/**[01]**/
											/**[01]**/
			} while(dwError == 204);						/**[01]**/
			bPollServer = FALSE;							/**[01]**/
		}
#endif
		if(NCScnetRecvData(pConn, FALSE, &nCommand, sizeof(nCommand), (BOOLEAN*)NULL) != sizeof(nCommand)) 
			break;

		if ((bAgain = (BOOLEAN)(nCommand == 0)) != FALSE)
			continue;						/* ignore pings */

		nLength = (nCommand & 0xffffff);
		nCommand >>= 24;

		if (nCommand == NCSNET_PACKET) {
			NCSLog(LOG_HIGH, "NCScnetRecvRawPacket: NCSNET_PACKET, nLength = %d\n", nLength);
			pChunk = NULL;
			nTotalLength = nLength;
			pData = NCSRealloc(pData, nTotalLength, FALSE);
			nTotalRead = NCScnetRecvData(pConn, FALSE, pData, nTotalLength, pbCancelRecv);
#ifdef ECWP_POLLING_SUPPORT
			bPollServer = TRUE;		/**[01]**/
#endif
		}
		else if (nCommand == NCSNET_CHUNKED_PACKET) {
			NCSLog(LOG_HIGH, "NCScnetRecvRawPacket: NCSNET_CHUNKED_PACKET, nLength = %d\n", nLength);
			nTotalLength = nLength;
			pData = NCSRealloc(pData, nTotalLength, FALSE);
			pChunk = pData;
			nTotalRead = 0;
		}
		else if (nCommand == NCSNET_CHUNK) {
			NCSLog(LOG_HIGH, "NCScnetRecvRawPacket: NCSNET_CHUNK, nLength = %d\n", nLength);
//			NCSSleep(1000);				/* simulate a slow connection for post-send cancel testing :) */
			if (pChunk == NULL) {
				/* if this happens the server is probably sending 'padding' (or is in error) */
				NCSLog(LOG_HIGH, "NCScnetRecvRawPacket: discarding unexpected chunk\n");
				pData = NCSRealloc(pData, nLength, FALSE);
				if (NCScnetRecvData(pConn, FALSE, pData, nLength, pbCancelRecv) != nLength)
					break;
				bAgain = TRUE;
			} else {
				if ((nLength + nTotalRead > nTotalLength) ||
					(NCScnetRecvData(pConn, FALSE, pChunk, nLength, pbCancelRecv) != nLength))
					break;

				pChunk += nLength;
				nTotalRead += nLength;
			}
#ifdef ECWP_POLLING_SUPPORT
			bPollServer = TRUE;		/**[01]**/
#endif
		} else {
			NCSLog(LOG_HIGH, "NCScnetRecvRawPacket: unknown nCommand (%d)\n", nCommand);
			break;
		}
	} while ((nTotalRead < nTotalLength  &&  pChunk) || bAgain);

	if (nTotalLength == 0  ||  nTotalRead < nTotalLength) {
		NCSFree(pData);
		if(!pConn->bEndThreads) {
			NCSLog(LOG_MED, "NCScnetRecvRawPacket died: nTotalLength = %d, nTotalRead = %d, nCommand = %d, nLength = %d", nTotalLength, nTotalRead, nCommand, nLength);
		}
		return NCS_NET_PACKET_RECV_FAILURE;
	}
	*pDataLength = nTotalLength;
	*ppData = pData;

	return(NCS_SUCCESS);
}