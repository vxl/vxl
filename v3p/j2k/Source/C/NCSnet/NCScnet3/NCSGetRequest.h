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
** FILE:   	NCScnet3\NCSGetRequest.h
** CREATED:	12-4-2001
** AUTHOR: 	Russell Rogers
** PURPOSE:	
** EDITS:
** [01]
*******************************************************/

// NCSGetRequest.h: interface for the CNCSGetRequest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(NCSGETREQUEST_H)
#define NCSGETREQUEST_H

#include "NCSRequest.h"
#include "NCSUtil.h"
#include "NCSThread.h"

class CNCSGetRequest: public CNCSRequest, public CNCSThread {
public:

	CNCSGetRequest( );
	virtual ~CNCSGetRequest();

	void SwitchToPolling( );

	bool StartThread(void *pConn);
	void StopThread();
	void WaitForThreadsToExit();

	NCSError RecvRawPacket( void **ppData, INT32 *pDataLength, bool *pbCancelRecv );

	NCSError GetInfoFromServer( UINT64 *nID, UINT8 *nServerVersion );

	void SetRecvCB( NCSnetPacketRecvCB *pRecvCB, void *pRecvCBData );
	
	UINT8		m_nServerVersion;

private:
	INT32 RecvData(void *pData, INT32 nBytesToRead, bool *pbCancelRecv);
	void Work(void *pData);

	bool				m_bEndThreads;
	NCSnetPacketRecvCB	*m_pRecvCB;
	void				*m_pRecvCBData;
};

#endif // !defined(NCSGETREQUEST_H)
