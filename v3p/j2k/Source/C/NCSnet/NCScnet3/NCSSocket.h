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
** FILE:   	NCScnet3\NCSSocket.h
** CREATED:	12-4-2001
** AUTHOR: 	Russell Rogers
** PURPOSE:	
** EDITS:
** [01]
*******************************************************/

// NSCSocket.h: interface for the CNCSSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(NCSSOCKET_H)
#define NCSSOCKET_H

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // WIN32

#include "NCSUtil.h"

#include <vector>
#include <string>
#include <iostream>

#ifdef WIN32
#include <winsock2.h>

#elif defined(POSIX)
#include <sys/types.h>
#include <sys/socket.h>

#elif defined(MACINTOSH)

#	ifdef TARGET_API_MAC_CARBON
#		include <carbon.h>
#	else
#		include <OpenTransport.h>
#		include <OpenTransportProtocol.h>
#		include <OpenTransportProviders.h>
#	endif //TARGET_API_MAC_CARBON

#	include <Types.h>
#	include <SIOUX.h>

/*#include "Sockets.h"

// Define macros for name compatibilty of function names
#define getpeername		socket_getpeername
#define connect			socket_connect
#define shutdown		socket_shutdown
#define recv			socket_recv
#define send			socket_send*/

#else
#error "NCSSocket not supported on this platform"
#endif

//#include "NCSException.h"

#define NCS_SSIZE_MAX 1024*1024

using namespace std;

class CNCSSocket {

public:
	CNCSSocket();//const string& host, unsigned short port);
#if !defined( MACINTOSH )
	CNCSSocket(const int handle);
#endif
	virtual ~CNCSSocket();
	unsigned short ConnectedAtPort();
	string ConnectedTo();
	bool Connected();
	bool Connect(const string& host, unsigned short port);
	bool Disconnect();
	int Read(string& buffer, int numChars);
	int Read(char * buffer, int numChars);
	bool Write(const string& data);
	bool Write(const char * data, int numChars);
	
	static bool init(void);
	static void deinit(void);
	
	static CNCSMutex s_initMutex;
	static int  s_initCount;
	static bool s_bSockInit;
	static int s_nIdCount;
	
	string m_sLastError;
	NCSError m_eLastError;
	
private:

	bool	m_bInitialized;
	
#ifdef MACINTOSH
	EndpointRef InitEndpoint();
	EndpointRef m_OTEndpoint;
	string m_sConnectedTo;
	Boolean m_bBound;
	int m_nId;
#if TARGET_API_MAC_CARBON
	static OTClientContextPtr s_pClientContext;
#endif //TARGET_API_MAC_CARBON
#else
	int m_handle; /**< socket handle */
	struct hostent *m_pConnectedTo; /**< describes the host that the socket is connected to */
#endif

	bool m_connected; /**< specify if connected to a socket */
	int m_connectedPort; /**< port on host that the socket is connected to */

};


#endif //NCSSOCKET_H
