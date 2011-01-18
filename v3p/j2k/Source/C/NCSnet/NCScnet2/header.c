/********************************************************
** Copyright 1999 Earth Resource Mapping Pty Ltd.
** This document contains unpublished source code of
** Earth Resource Mapping Pty Ltd. This notice does
** not indicate any intention to publish the source
** code contained herein.
**
** FILE:   	NCScnet2\header.c
** CREATED:	
** AUTHOR: 	Simon Cope
** PURPOSE:	HTTP header send/recv
** EDITS:
** [01] sjc 01Oct99 Track protocol version
 *******************************************************/

#include "cnet.h"

#ifdef NOTDEF
/*
** construct and send HTTP header to server
*/
BOOLEAN NCScnetSendHeader(NCScnetConnection *pConn,
						  BOOLEAN bSendSocket,
						  INT32 nSendLength)
{
	if(pConn) {
		char szRequest[300];
		int nReqLen;
		int nBytesSent;
		char *pDllName;

		/* can override the default ISAPI dll names via the registry :) */
		if (NCSPrefGetUserString("IWS Client Network Dll", &pDllName) != NCS_SUCCESS) {
			pDllName = strdup(szIISDLLName);
		}

		if(bSendSocket)	{
			/*
			** Up-Link always uses HTTP 1.0 to get through proxies
			*/
			nReqLen = _snprintf(szRequest, 299, "POST %s%s%s?%I64u,%ld HTTP/1.0\r\nCache-Control:no-cache,no-store,max-age=0\r\nContent-length:%d\r\nUser-Agent:NCS/1.0\r\n\r\n",
								pConn->bViaProxy ? "http://" : "",
								pConn->bViaProxy ? pConn->szHostName : "",
								pDllName,
								pConn->nID,
								(long)NCS_PROTOCOL_VERSION,	/**[01]**/
								nSendLength + sizeof(nSendLength));
		} else {
			/*
			** Downlink always uses HTTP 1.1
			*/
			nReqLen = _snprintf(szRequest, 299, "GET %s%s%s?%I64u,%ld HTTP/1.1\r\nCache-Control:no-cache,no-store,max-age=0\r\nHost:%s\r\nUser-Agent:NCS/1.0\r\n\r\n",
								pConn->bViaProxy ? "http://" : "",
								pConn->bViaProxy ? pConn->szHostName : "",
								pDllName,
								pConn->nID, 
								(long)NCS_PROTOCOL_VERSION,	/**[01]**/
								pConn->szHostName);
		}

		NCSFree(pDllName);

		if(nReqLen > 0) {
			nBytesSent = NCScnetSendData(pConn, bSendSocket, szRequest, nReqLen, (BOOLEAN*)NULL);

			if(nBytesSent == nReqLen) {
				return(TRUE);
			}
		}
	}
	return(FALSE);
}
#endif


#ifdef NOTDEF
/*
** Recv a header back from the server and decode it
*/
NCSError NCScnetRecvHeader(NCScnetConnection *pConn,
						  BOOLEAN bSendSocket)
{
	char pLine[1024];
	INT32 nLineLen = 0;
//	BOOLEAN bRVal = TRUE;
	NCSError eError = NCS_UNKNOWN_ERROR;

	char pResp[40];
	INT32 nPeeked;

	BOOLEAN bFirstLine = TRUE;
	BOOLEAN bEntity = FALSE;

	while(!bEntity)	{
		nPeeked = NCScnetRecvData(pConn, bSendSocket, pResp, 40, TRUE, (BOOLEAN*)NULL);

		if(nPeeked > 0) {
			INT32 nIdx = 0;

			// look for a CRLF pair in the data just read
			BOOLEAN bEOL = FALSE;
			while((nIdx < nPeeked) && (!bEOL))	{
				if((nIdx <= nPeeked-2)  &&  (pResp[nIdx] == '\r') && (pResp[nIdx+1] == '\n')) {
					nIdx += 2;
					bEOL = TRUE;
				} else {
					nIdx++;
				}
			}

			// add line fragment to total line
			if((nLineLen + nIdx) > 1023) {
				// todo: error, line too long
				_RPT1(_CRT_ASSERT, "ReadResponse(%s): line too long\n", bSendSocket ? "Writer" : "Reader");
			} else {
				INT32 nRead;

				memcpy(pLine + nLineLen, pResp, nIdx);
				nLineLen += nIdx;
				pLine[nLineLen] = '\0';
				
				nRead = NCScnetRecvData(pConn, bSendSocket, pResp, nIdx, FALSE, (BOOLEAN*)NULL);
				if(nRead != nIdx) {
					/* socket error... */
					return NCS_NET_HEADER_RECV_FAILURE;		
				}
			}

			if(bEOL) {
//MessageBox(NULL, pLine, bSendSocket ? "HTTP Send Response" : "HTTP Recv Response", MB_OK);
				if(bFirstLine) {
					int nMajor, nMinor, nResult;

					if(sscanf(pLine, "HTTP/%d.%d %d", &nMajor, &nMinor, &nResult) != 3) {
						_RPT2(_CRT_ERROR, "ReadResponse(%s): first line = '%s'?\n", bSendSocket ? "Writer" : "Reader", pLine);
						eError = NCS_NET_BAD_RESPONSE;
					} else {
						_RPT4(_CRT_WARN, "ReadResponse(%s): major=%d, minor=%d, result=%d\n", bSendSocket ? "Writer" : "Reader", nMajor, nMinor, nResult);
						
						if(nResult == 200 || nResult == 204) {
							eError = NCS_SUCCESS;
						} else if (nResult == 401) {
							eError = NCS_NET_401_UNAUTHORISED;
						} else if (nResult == 403) {
							eError = NCS_NET_403_FORBIDDEN;
						} else if (nResult == 407) {
							eError = NCS_NET_407_PROXYAUTH;
						} else if (nResult == 500) {
							eError = NCS_SERVER_ERROR;
						} else {
							/* maybe like a 30x redirection? */
							eError = NCS_NET_UNEXPECTED_RESPONSE;
							{
								/* todo: remove this messagebox when we've sussed these responses */
								char szError[50];
								sprintf(szError, "Unexpected HTTP response code %d\n", nResult);
								MessageBox(NULL, szError, "IWS Network", MB_OK);
							}
						};
					}
					bFirstLine = FALSE;
				}

				nLineLen = 0;
				if((pLine[0] == '\r') && (pLine[1] == '\n')) {
					bEntity = TRUE;		// now we've read up to the entity object.
				}
			}
		} else if((nPeeked == 0) && bSendSocket && (WSAGetLastError() == WSAECONNRESET)) {
			return(NCS_SUCCESS);
		} else {
				// socket closed, or socket error
			_RPT3(_CRT_WARN, "ReadResponse(%s): socket error(%d), read (%s)\n", bSendSocket ? "Writer" : "Reader", WSAGetLastError(), nLineLen ? pLine:"nothing");
			return NCS_NET_HEADER_RECV_FAILURE;
		}
	}
	return eError;
}

#endif