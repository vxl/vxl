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
** FILE:   	NCScnet3\NCSPostRequest.h
** CREATED:	12-4-2001
** AUTHOR: 	Russell Rogers
** PURPOSE:	
** EDITS:
** [01]
*******************************************************/

// NCSPostRequest.h: interface for the CNCSPostRequest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(NCSPOSTREQUEST_H)
#define NCSPOSTREQUEST_H

#include "NCSRequest.h"
#include "NCSUtil.h"
#include "NCSThread.h"

//typedef void NCSnetPacketSentCB(NCSError, void *, void *);					// eError, pPacket, pUserData

class CNCSPostRequest : public CNCSRequest, public CNCSThread
{
public:
	CNCSPostRequest();
	virtual ~CNCSPostRequest();

	bool StartThread(void *pConn);
	void StopThread();
	void WaitForThreadsToExit();

	bool SendPacket(void *pPacket, int nLength, void *pUserdata, bool *bIsConnected);
	NCSError SendRawPacket(void *pData, INT32 nDataLength);
	
	void SetSentCB( NCSnetPacketSentCB	*pSentCB );

private:

	void Work(void *pData);
	NCSError SendSynchronise(NCSSequenceNr nSequence);

	NCSTimeStampMs		m_tsLastSend;

	bool				m_bEndThreads;

	bool				m_bPacketToSend;

	NCSnetPacketSentCB	*m_pSentCB;
	void				*m_pSendCBData;
	NCSMutex			m_mSendMutex;

	void				*m_pSendPacket;
	INT32				m_nSendPacketLength;

};

#endif // !defined(NCSPOSTREQUEST_H)
