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
** FILE:   	NCScnet3\NCSGetRequest.cpp
** CREATED:	12-4-2001
** AUTHOR: 	Russell Rogers
** PURPOSE:	
** EDITS:
** [01]
*******************************************************/


// NCSGetRequest.cpp: implementation of the CNCSGetRequest class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <winsock2.h>
#endif

#include <iostream>
#include "NCSGetRequest.h"
#include "NCSLog.h"
#include "NCSnet.h"
#include "cnet.h"

#ifdef _DEBUG
	#define NCS_RECV_THREAD_TIMEOUT 300000
#else
	#define NCS_RECV_THREAD_TIMEOUT 3000
#endif

using namespace std;

/** @class CNCSGetRequest
 *  @brief Implements the GET http method.
 *
 *  This class creates a GET type http conection with the server
 *  and is used to recieve the data packet.  May operate in two
 *  modes streaming or polling.  The streaming method is the prefered
 *  prefered method and works by keeping the connection with the
 *  server continously open and recieving packets as they are sent.
 *  The polling method is only used when it is not possible to use
 *  the streaming method as it is much slower.  In polling mode a new
 *  GET connection is created each time a packet is required.  This
 *  has considerable over heads compared to the streaming due to having
 *  to re-connect each time and also because a request must be sent
 *  for each packet recieved.
 *
 *  @author	Russell Rogers
 *  @date	10 April 2001
 *  @par Edits:
 *
 */

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSGetRequest::CNCSGetRequest( ) : CNCSRequest( )
{
	SetMethod( "GET" );
	
	m_bEndThreads=true;

	m_pRecvCB=NULL;
	//m_tRecvThread=NULL;
	m_pRecvCBData=NULL;
	
	m_bPolling = false;
	m_nServerVersion=0;
	
	//m_nSendTimeOut = 10000;

}

CNCSGetRequest::~CNCSGetRequest()
{
	Disconnect();
	StopThread();
	WaitForThreadsToExit();	
}

//////////////////////////////////////////////////////////////////////
// Member function
//////////////////////////////////////////////////////////////////////

void CNCSGetRequest::SwitchToPolling( ) {
	
	m_bPolling = true;
	//m_nSendTimeOut = 10000;
	
	DEBUGPRINT("Switching to polling mode.\n");
#ifdef DEBUGINFO
	cout << "Switching to polling mode." << endl;
#endif
}


NCSError CNCSGetRequest::RecvRawPacket( void **ppData, INT32 *pDataLength, bool *pbCancelRecv ) {

	void *pData = (void*)NULL;
	char *pChunk = NULL;

	UINT32 nCommand;
	INT32 nLength;

	INT32 nTotalLength = 0;
	INT32 nTotalRead = 0;

	bool bAgain = false;
	bool bPollServer = true;

	do {
		if( m_bPolling && bPollServer) {
			NCSError eError;
			do {
				Disconnect();
				eError = ConnectToServer( NULL, 0 );
			} while ( eError != NCS_SUCCESS || m_nLastStatus == 204 );
			bPollServer = false;
		}
		
		if( !Read( nCommand ) )  { // if no connection
			//return NCS_NET_COULDNT_CONNECT;
			break;
		}
		
		if (bAgain = (nCommand == 0))	
			continue;						/* ignore pings */

		nLength = (nCommand & 0xffffff);
		nCommand >>= 24;

		if (nCommand == NCSNET_PACKET) {
			pChunk = NULL;
			nTotalLength = nLength;
			pData = NCSRealloc(pData, nTotalLength, FALSE);
			nTotalRead = RecvData(pData, nTotalLength, pbCancelRecv);
			bPollServer = true;
		}
		else if (nCommand == NCSNET_CHUNKED_PACKET) {
			nTotalLength = nLength;
			pData = NCSRealloc(pData, nTotalLength, FALSE);
			pChunk = (char *)pData;
			nTotalRead = 0;
		}
		else if (nCommand == NCSNET_CHUNK) {
//			NCSSleep(1000);				/* simulate a slow connection for post-send cancel testing :) */
			if (pChunk == NULL) {
				/* if this happens the server is probably sending 'padding' (or is in error) */
				pData = NCSRealloc(pData, nLength, FALSE);
				if (RecvData(pData, nLength, pbCancelRecv) != nLength)
					break;
				bAgain = true;
			} else {
				if ((nLength + nTotalRead > nTotalLength) ||
					(RecvData(pChunk, nLength, pbCancelRecv) != nLength))
					break;

				pChunk += nLength;
				nTotalRead += nLength;
			}
			bPollServer = true;

		} else {
			break;
		}

#ifdef DEBUGINFO 
		cout << "**Recv Packet " << nTotalRead << endl;
#endif
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
		DEBUGPRINT("**Recv Packet\n");
#endif

	} while ((nTotalRead < nTotalLength  &&  pChunk) || bAgain);

	if (nTotalLength == 0  ||  nTotalRead < nTotalLength) {
		NCSFree(pData);
		if(!m_bEndThreads) {
		}
		return NCS_NET_PACKET_RECV_FAILURE;
	}
	*pDataLength = nTotalLength;
	*ppData = pData;

	return(NCS_SUCCESS);
}

INT32 CNCSGetRequest::RecvData(void *pData, INT32 nBytesToRead, bool *pbCancelRecv)
{
	int nBytesRead = 0;
	
	while(nBytesRead < nBytesToRead) {
		int nThisRead;
		int nGet = pbCancelRecv ? MIN(NCS_PACKET_CHUNK_SIZE, nBytesToRead - nBytesRead) : (nBytesToRead - nBytesRead);
		
		nThisRead = Read( (char *)pData + nBytesRead, nGet );
		nBytesRead += nThisRead;
		
		if((pbCancelRecv && *pbCancelRecv) || (nThisRead == 0)) {
			break;
		}
	}
	return(nBytesRead);
}

bool CNCSGetRequest::StartThread(void *pConn) {
	m_bEndThreads = false;

	if( Spawn( (void*)pConn, FALSE ) ) {
		return(true);
	}

	StopThread();
	
	return(false);

}


void CNCSGetRequest::StopThread() {
	m_bEndThreads = true;
}

void CNCSGetRequest::WaitForThreadsToExit() {
	if(m_bEndThreads) {
		NCSTimeStampMs tsAbortTime = NCSGetTimeStampMs() + 2000;

		while( (IsRunning()) && (tsAbortTime > NCSGetTimeStampMs()) ) {
			NCSSleep(250);
		}
		if (IsRunning()) {
			Terminate();
		}

	}
}

NCSError CNCSGetRequest::GetInfoFromServer( UINT64 *nID, UINT8 *nServerVersion ) {

	BOOLEAN bHavePacket = FALSE;
	NCSError eError = NCS_SUCCESS;
	NCSTimeStampMs tsExpireAt = NCSGetTimeStampMs() + NCS_CONNECTION_ATTEMP_TIMEOUT_MS;
	INT64 nConnID = 0;
	UINT8 nSVtmp=1;
	
	if( Read( (char &)nSVtmp ) ) {
		
		*nServerVersion = nSVtmp;
		m_nServerVersion = nSVtmp;
		
		/*
		** Now that protocol version negotiation is out of the way,
		** we can do the version specific stuff to maintain compatibility.
		*/
		if(nSVtmp >= 2) {
																/*
																** V2 protocol
																** 
																** Read server generated UINT64 ConnID out.
			*/
			if( Read( nConnID ) ) {					
				*nID = nConnID;
				*m_pnID = nConnID;
				if(nSVtmp >= 3) {
											/* 
											** V3 protocol
											** 
											** Reconnect the GET link with the server generated 
											** connection ID if in polling mode.
					*/
					if( m_bPolling == true ) {
						Disconnect();
						eError = ConnectToServer((void*)NULL, 0);
					}
				}
			} else {
				eError = NCS_NET_PACKET_RECV_ZERO_LENGTH;
			}
		}
		
	} else {
		eError = NCS_NET_PACKET_RECV_ZERO_LENGTH;
	}
	
	if(nConnID == 0) {
		if (eError!=NCS_NET_PACKET_RECV_ZERO_LENGTH) {
			eError = NCS_NET_RECV_TIMEOUT;
		}
	}
	
	return(eError);
}

void CNCSGetRequest::SetRecvCB( NCSnetPacketRecvCB *pRecvCB, void *pRecvCBData ) {
	_ASSERT( pRecvCB != 0 );
	_ASSERT( pRecvCBData != 0 );
	
	m_pRecvCB = pRecvCB;
	m_pRecvCBData = pRecvCBData;
}


void CNCSGetRequest::Work(void *pData)
{
	CNCScnet *pConn = (CNCScnet *)pData;

	while(!m_bEndThreads ) {//&& pConn->pRecvCon->Connected()) {
		void *pPacket;
		INT32 nPacketLength;
		NCSError eError = NCS_SUCCESS;

		eError = RecvRawPacket(&pPacket, &nPacketLength, &(m_bEndThreads));
		
		if((eError == NCS_SUCCESS) && pPacket && (nPacketLength != 0)) {
			if(m_pRecvCB) {
				if(!m_bEndThreads) {

					(*m_pRecvCB)(pPacket, 
									  nPacketLength, 
									  pData, 
									  m_pRecvCBData,
									  NCS_SUCCESS);
				} else {
				}
			} else {
				NCSFree(pPacket);
			}
		}
		else if ((eError != NCS_SUCCESS) && !m_bEndThreads) {
			NCSTimeStampMs tsDisconnectAt = 0;
			Disconnect();
			tsDisconnectAt = NCSGetTimeStampMs() + NCS_RECV_THREAD_TIMEOUT;

			while(!m_bEndThreads && ((eError = ConnectToServer((void*)NULL, 0)) != NCS_SUCCESS) ) {
				NCSSleep(500);

				if(NCSGetTimeStampMs() >= tsDisconnectAt) {
					Disconnect();
					StopThread();
					pConn->m_pSendCon->StopThread();  // terminate send thread
					(*m_pRecvCB)(NULL, 0, pData, m_pRecvCBData, eError);
				}
			}
		}
		
#if defined(MACINTOSH) || defined(COOPERATIVE)
		NCSThreadYield();
#endif
		
		
	}
	Disconnect();
	Exit(0);
}

