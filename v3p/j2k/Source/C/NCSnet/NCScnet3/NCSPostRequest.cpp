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
** FILE:   	NCScnet3\NCSPostRequest.cpp
** CREATED:	12-4-2001
** AUTHOR: 	Russell Rogers
** PURPOSE:	
** EDITS:
** [01]
*******************************************************/

// NCSPostRequest.cpp: implementation of the CNCSPostRequest class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <winsock2.h>
#endif

#include <iostream>
#include "NCSPostRequest.h"
#include "NCSLog.h"
#include "cnet.h"

using namespace std;

/** @class CNCSPostRequest
 *  @brief Implements the POST http method.
 *
 *  The POST connectiong is used for sending packets to the server.
 *  No response is expected other than a success acknowledgement.
 *
 *  @author	Russell Rogers
 *  @date	10 April 2001
 *  @par Edits:
 *
 */

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSPostRequest::CNCSPostRequest() : CNCSRequest( )
{
	SetMethod( "POST" );
	m_sHttpVer="HTTP/1.0";

	m_bEndThreads=false;

	m_pSentCB = NULL;
	m_pSendCBData = NULL;
	m_pSendPacket = NULL;
	m_nSendPacketLength = 0;
	m_bPolling = false;
	m_bPacketToSend = false;

	NCSMutexInit(&m_mSendMutex);

	//m_nSendTimeOut = 10000;

}

CNCSPostRequest::~CNCSPostRequest()
{
    Disconnect();

	StopThread();
	WaitForThreadsToExit();
	
	NCSMutexFini(&m_mSendMutex);
}

//////////////////////////////////////////////////////////////////////
// Member function
//////////////////////////////////////////////////////////////////////

bool CNCSPostRequest::SendPacket(void *pPacket, int nLength, void *pUserdata, bool *bIsConnected)
{
	NCSMutexBegin(&(m_mSendMutex));
	
	if(m_pSendPacket) {
		NCSMutexEnd(&(m_mSendMutex));
		return(false);
	}

	m_pSendPacket = pPacket;
	m_nSendPacketLength = nLength;
	m_pSendCBData = pUserdata;
	
	// NCSThreadResume(&(m_tSendThread));
	// instead of resume set flag indicating a packet ready to be sent
	m_bPacketToSend = true;

	NCSMutexEnd(&(m_mSendMutex));
	*bIsConnected = Connected();
	if (!bIsConnected)
		return(false);
	return(true);
	return(false);
}

bool CNCSPostRequest::StartThread(void *pConn) {
	m_bEndThreads = false;

	if(Spawn((void*)pConn, FALSE)) {
		return(true);
	} 
	StopThread();
	
	return(false);

}


void CNCSPostRequest::StopThread() {
	m_bEndThreads = true;
}

void CNCSPostRequest::WaitForThreadsToExit() {
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

void CNCSPostRequest::SetSentCB( NCSnetPacketSentCB	*pSentCB ) {
	_ASSERT( pSentCB != 0 );

	m_pSentCB = pSentCB;
}

NCSError CNCSPostRequest::SendRawPacket(void *pData, INT32 nDataLength) {
	void *pFullData;
	NCSError eError = NCS_SUCCESS;

	NCSMutexBegin(&(m_mSendMutex));
	
	if( Connected() ) {
		Disconnect();
	}

	if(NULL != (pFullData = NCSMalloc(nDataLength + sizeof(nDataLength), FALSE))) {
		INT32 nRetryCount = 2;

		memcpy(pFullData, &nDataLength, sizeof(nDataLength));
		memcpy((UINT8*)pFullData + sizeof(nDataLength), pData, nDataLength);

		while(nRetryCount-- > 0) {

			eError = ConnectToServer(pFullData, nDataLength + sizeof(nDataLength));
			// connect to server and send packet.
			if(eError == NCS_SUCCESS) {
				m_tsLastSend = NCSGetTimeStampMs();
				break;
			}
		}
		NCSFree(pFullData);

		if(eError == NCS_SUCCESS) {
			Disconnect();
			NCSMutexEnd(&(m_mSendMutex));
			return(eError);
		}
	} else {
		eError = NCS_COULDNT_ALLOC_MEMORY;
	}
	NCSMutexEnd(&(m_mSendMutex));
	return(eError);
}

NCSError CNCSPostRequest::SendSynchronise(NCSSequenceNr nSequence)
{
	UINT32 iPacketSize = NCS_PACKET_BASE_SIZE + sizeof(NCSSequenceNr);
	NCSPacket *pSyncPacket = (NCSPacket *)NCSMalloc(iPacketSize, FALSE);
	NCSPacketType ptType = NCSPT_SYNCHRONISE;
	NCSError eError;

	NCS_PACKET_PACK_BEGIN(pSyncPacket);
	NCS_PACKET_PACK_ELEMENT(iPacketSize);
	NCS_PACKET_PACK_ELEMENT(*m_pnID);
	NCS_PACKET_PACK_ELEMENT(ptType);
	NCS_PACKET_PACK_ELEMENT(nSequence);
	NCS_PACKET_PACK_END(pSyncPacket);

	eError = SendRawPacket( pSyncPacket, iPacketSize );

	if(eError != NCS_SUCCESS) {
		NCSFree(pSyncPacket);
	}
	return(eError);
}

#define PINGFREQUENCY 60000
#define WAITSLEEP 1000

void CNCSPostRequest::Work(void *pData)
{
	CNCScnet *pConn = (CNCScnet *)pData;
	NCSError eError = NCS_SUCCESS;

	//NCSTimeStampMs tsLastSend;

	while(!m_bEndThreads) {
		void *pPacket;
		INT32 nPacketLength;
		
		if( m_bPacketToSend ) {
			
			m_bPacketToSend = false;
			
			NCSMutexBegin(&(m_mSendMutex));
			pPacket = m_pSendPacket;
			nPacketLength = m_nSendPacketLength;
			m_pSendPacket = (void*)NULL;
			m_nSendPacketLength = 0;
			NCSMutexEnd(&(m_mSendMutex));
			
			if(pPacket) {
				
#ifdef DEBUGINFO 
				cout << "Send Packet: " << nPacketLength << endl;
#endif
#ifdef _DEBUG
				DEBUGPRINT("!!Send Packet\n");
#endif
				
				eError = SendRawPacket(pPacket, nPacketLength);
				
				if((eError == NCS_SUCCESS) && m_pSentCB) {
					if(!m_bEndThreads) {
						
						(*m_pSentCB)((eError == NCS_SUCCESS) ? NCS_SUCCESS : NCS_NET_PACKET_SEND_FAILURE, pPacket, 
							m_pSendCBData);
					}
				} else {
					NCSFree(pPacket);
				}
			}
		}
		else if( pConn->m_pRecvCon->m_nServerVersion >= 4 && !m_bEndThreads ) {
			NCSError eError=NCS_SUCCESS;
			// send sync (ping) packet every PINGFREQUENCY secs
			if( ((NCSGetTimeStampMs() - m_tsLastSend) > PINGFREQUENCY) && !m_bEndThreads) {
				// send ping
				eError = SendSynchronise( m_tsLastSend );
				if( eError != NCS_SUCCESS && !m_bEndThreads ) { // try sending ping once more time then giveup
					eError = SendSynchronise( m_tsLastSend );
				}
				if( eError != NCS_SUCCESS && !m_bEndThreads) { // if sending of ping was unsuccessfull connection to server must be lost.
					//pConn->bIsConnected = FALSE;
					//(*pConn->pRecvCB)(NULL, 0, pConn, pConn->pRecvCBData, NCS_NET_PACKET_RECV_FAILURE);
					StopThread();
					pConn->m_pRecvCon->StopThread();
				}
			}
		}
		/*} else { // old code
		if( (NCSGetTimeStampMs() - m_tsLastSend) > PINGFREQUENCY ) {
		// send ping
		char szPing[1] = {0};
		eError = SendSynchronise( m_tsLastSend );
		//Might be neccessary to add retry.
		if( eError != NCS_SUCCESS ) m_bEndThreads = true;
		pConn->m_pRecvCon->StopThread();
		break;
		}
	}*/
		
		NCSSleep( WAITSLEEP );
	}
	Disconnect();
	Exit(0);
}
