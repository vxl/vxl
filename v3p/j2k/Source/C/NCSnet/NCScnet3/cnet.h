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
** FILE:   	NCScnet2\cnet.h
** CREATED:	
** AUTHOR: 	Russell Rogers
** PURPOSE:	cnet private header
** EDITS:
** [01]
*******************************************************/

#ifndef CNET_H
#define CNET_H

#ifdef WIN32
#include <winsock2.h>
#endif

#ifdef POSIX
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#ifndef INADDR_NONE
#define INADDR_NONE ((in_addr_t)-1)
#endif
#endif // POSIX

#include "NCSGetRequest.h"
#include "NCSPostRequest.h"
#include "NCSException.h"

#include "NCSUtil.h"
#include "NCScnet.h"
#include "NCSnet.h"
#include "NCSLog.h"

#if defined(WIN32)
#include <crtdbg.h>
#endif
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
//
// cnet.h: interface for the CNCScnet class.
//
//////////////////////////////////////////////////////////////////////

class CNCScnet {
public:
	CNCScnet();
	virtual ~CNCScnet();

	NCSError Create(char *szUrl, char *pszIISDLLName);

	NCSError Start(void **ppClient, void *pPacketIn, int nLengthIn, void **ppPacketOut,
		INT32 *pLengthOut, NCSnetPacketRecvCB *pRecvCB, void *pRecvUserdata);

	CNCSGetRequest *m_pRecvCon;
	CNCSPostRequest *m_pSendCon;

private:
	NCSError StartupConnections( void **ppClient, void *pPacketIn, int nLengthIn, 
					void **ppPacketOut, INT32 *pLengthOut );
	NCSError DoInitialTransfer(void *pPacketOut,	INT32 nPacketOutLength, void **ppPacketIn, INT32 *pPacketInLength);
	BOOLEAN StartThreads();
	void StopThreads();
	NCSError InitializeSettings();
	BOOLEAN SetID();
	void DisconnectFromServer();
	void Destroy();
	
	void BreakUpUrl( char *szUrl );

	CNCSProxy *m_pProxy;

	string m_sServerName;
	UINT32 m_nServerPort;
	UINT64 m_nID;
	string m_sIISDLLName;

};

#endif /* CNET_H */
