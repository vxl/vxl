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
** FILE:   	NCScnet3\NCSSocket.cpp
** CREATED:	12-4-2001
** AUTHOR: 	Russell Rogers
** PURPOSE:	
** EDITS:
** [01]
*******************************************************/

// NSCSocket.cpp: implementation of the CNSCSocket class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <winsock2.h>
#endif

#include <assert.h>
#include "NCSSocket.h"
#include "cnet.h"

int CNCSSocket::s_initCount = 0;
bool CNCSSocket::s_bSockInit = false;
CNCSMutex CNCSSocket::s_initMutex;
int CNCSSocket::s_nIdCount=0;

#ifdef MACINTOSH
#if TARGET_API_MAC_CARBON
OTClientContextPtr CNCSSocket::s_pClientContext = NULL;
#endif //TARGET_API_MAC_CARBON
#endif //MACINTOSH

/** @class CNCSSocket
 *  @brief A client TCP/IP stream network socket.
 *
 *  Provides a class interface for a client program to use
 *  standard Berkley sockets for TCP/IP streaming.
 *
 *  @author	Russell Rogers
 *  @date	10 April 2001
 *  @par Edits:
 *
 */

#ifdef MACINTOSH

/////////////////////////////////////////////////////////////////////

static pascal void YieldingNotifier(void* contextPtr, OTEventCode code, 
									   OTResult result, void* cookie)
	// This simple notifier checks for kOTSyncIdleEvent and
	// when it gets one calls the Thread Manager routine
	// YieldToAnyThread.  Open Transport sends kOTSyncIdleEvent
	// whenever it's waiting for something, eg data to arrive
	// inside a sync/blocking OTRcv call.  In such cases, we
	// yield the processor to some other thread that might
	// be doing useful work.
{
	//#pragma unused(contextPtr)
	#pragma unused(result)
	#pragma unused(cookie)
	OSStatus junk;
	CNCSSocket *pTheSocket = (CNCSSocket *)contextPtr;
	
	switch (code) {
		case kOTSyncIdleEvent:
			junk = YieldToAnyThread();
			//MoreAssert(junk == noErr); //Assertion fails if:  YieldToAnyThread failed
			break;
		default:
			// do nothing
			break;
	}
}

/////////////////////////////////////////////////////////////////////

EndpointRef CNCSSocket::InitEndpoint() {
	OSStatus 	err				= noErr;
	OSStatus 	junk;
	Ptr			transferBuffer 	= nil;
	TCall 		sndCall;
	DNSAddress 	hostDNSAddress;
	OTFlags 	junkFlags;
	OTResult 	lookResult;

	// Now open a TCP endpoint.
	OTConfigurationRef configRef = NULL;
	configRef = OTCreateConfiguration(kTCPName);
	_ASSERT( configRef != NULL );
#if TARGET_API_MAC_CARBON
	m_OTEndpoint = OTOpenEndpointInContext(configRef, 0, nil, &err,s_pClientContext);
#else
	m_OTEndpoint = OTOpenEndpoint(configRef, 0, nil, &err);
#endif
	
	// If the endpoint opens successfully...
	
	if (err == noErr) {

		// Establish the modes of operation.  This sample uses
		// sync/blocking mode, with sync idle events that yield
		// time using the Thread Manager.

		junk = OTSetSynchronous(m_OTEndpoint);
		if( junk != noErr ) return kOTInvalidEndpointRef;
		//MoreAssert(junk == noErr); //Assertion Fails if: OTSetSynchronous failed 
		
		junk = OTSetBlocking(m_OTEndpoint);
		if( junk != noErr ) return kOTInvalidEndpointRef;
		//MoreAssert(junk == noErr); //Assertion Fails if: OTSetBlocking failed
		
//#ifndef MAC_PREEMPTIVE		
		junk = OTInstallNotifier(m_OTEndpoint, NewOTNotifyUPP(YieldingNotifier), this);
		if( junk != noErr ) return kOTInvalidEndpointRef;
		//MoreAssert(junk == noErr); //Assertion Fails if: OTInstallNotifier failed
//#endif

		junk = OTUseSyncIdleEvents(m_OTEndpoint, true);
		if( junk != noErr ) return kOTInvalidEndpointRef;
		//MoreAssert(junk == noErr); //Assertion Fails if: OTUseSyncIdleEvents failed

		// Bind the endpoint.  Because we're an outgoing connection,
		// we don't have to bind it to a specific address.
		
		err = OTBind(m_OTEndpoint, nil, nil);
		m_bBound = (err == noErr);
	}
	
	if( m_bBound ) return m_OTEndpoint;
	else return kOTInvalidEndpointRef;
}


#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 * Creates new socket connection for given host and port number.
 *
 * @param host [IN] name of the host to connect to.
 * @param port [IN] port number to connect to.
 * @see Connect()
 */
CNCSSocket::CNCSSocket()//const string& host, unsigned short port)
//: m_connected(false), m_connectedPort(port)
{
	m_connected = false;
	m_connectedPort = 80;
	m_bInitialized = false;
	
#ifdef WIN32
	m_bInitialized = s_bSockInit;
#elif defined(MACOSX) || defined(POSIX) 
    m_bInitialized = true;
#endif
	//Connect(host, port);
}

#if !defined( MACINTOSH )
/**
 * Constructor.
 * Set class to use existing open socket connection.
 *
 * @param handle [IN] socket handle that was returned by a previous call to socket()
 * @see Connect()
 */
CNCSSocket::CNCSSocket(const int handle)
: m_handle(handle), m_connected(true)
{
	//init();
	struct sockaddr_in info;
#if defined(POSIX) && !defined(HPUX)
#	ifdef MACOSX
	int tempSize = sizeof(struct sockaddr);
#	else
	socklen_t tempSize = sizeof(struct sockaddr);
#	endif
#else
	int tempSize = sizeof(struct sockaddr);
#endif
	getpeername(handle, (struct sockaddr*)&info, &tempSize);
	m_connectedPort = info.sin_port;
	//m_connectedTo = inet_ntoa(info.sin_addr);

}
#endif

/**
 * Destructor.
 * If socket still connected, disconnect it.
 *
 * @see Connected()
 * @see Disconnect()
 */
CNCSSocket::~CNCSSocket()
{
	if(Connected())
		Disconnect();
		
	//deinit();
}

//////////////////////////////////////////////////////////////////////
// Public member functios
//////////////////////////////////////////////////////////////////////

bool CNCSSocket::init(void) {
	CNCSMutexLock lockFunction(&s_initMutex);
		
	if( !s_bSockInit ) {
#if defined( WIN32 )
	
		WSAData wsaData;
		
		if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
			fprintf(stderr, "WSAStartup failed.\n");
			exit(1);
		}
		s_bSockInit = true;
#elif defined( MACINTOSH )
		OSStatus 	err = noErr;
		
#	if TARGET_API_MAC_CARBON
		//err = InitOpenTransportInContext(kInitOTForApplicationMask, &m_pClientContext);
		err = InitOpenTransportInContext(kInitOTForExtensionMask, &s_pClientContext);
#	else
		err = InitOpenTransport();//kInitOTForApplicationMask, nil);
#	endif
		if( err == noErr ) {
			s_bSockInit = true;
		}
#elif defined( POSIX )
		//m_bSockInit = true;
#else
#error "CNCSSocket::init not defined for platform"
#endif
	}
	s_initCount++;
	
	return s_bSockInit;
}

void CNCSSocket::deinit(void) {
	CNCSMutexLock lockFunction(&s_initMutex);
	
	s_initCount--;
#ifdef MACINTOSH
	if( s_initCount == 0 && s_bSockInit ) {
#	if TARGET_API_MAC_CARBON
		CloseOpenTransportInContext(s_pClientContext);
#	else
		CloseOpenTransport();
#	endif
		s_bSockInit = false;
	}
#endif
}



/** 
 * Returns the port to which this socket is connected.
 *
 * @see m_connectedPort
 * @return port number that socket is connected to
 * @par
 * PRECONDITION: must be connected
 */
unsigned short CNCSSocket::ConnectedAtPort()
{
	return( m_connectedPort );
}

/** 
 * Returns the host to which this socket is connected.
 *
 * @see m_pConnectedTo
 * @return name of host connected to
 * @par
 * PRECONDITION: must be connected
 */
string CNCSSocket::ConnectedTo()
{
#ifdef MACINTOSH
	return m_sConnectedTo;
#else
	return( m_pConnectedTo->h_name );
#endif
}

/**
 * Check if socket connected to host.
 *
 * @see m_connected
 * @return true if connected, false if not
 */
bool CNCSSocket::Connected()
{
	return( m_connected );
}

/** 
 * Connects to host, at port.
 * If already connected, will disconnect old connection.
 *
 * @param host name of host to connect to
 * @param port port number to connect to
 * @see Connected()
 * @see Disconnect()
 * @see m_pConnectedTo
 * @see m_handle
 * @see m_connected
 */
bool CNCSSocket::Connect(const string& host, unsigned short port)
{
	if( !m_bInitialized ) return false;
	
	if(Connected())
		Disconnect();

#ifdef MACINTOSH
	DNSAddress 	hostDNSAddress;
	TCall 		sndCall;
	OSStatus 	err = noErr;
	string		sHostPort;
	char		szTmp[20];

	if( m_OTEndpoint == kOTInvalidEndpointRef ) { // no endpoint
		//throw( CNCSException( "CNCSSocket::Connect, Invalid Endpoint", NCS_NET_COULDNT_CONNECT ) );
		m_sLastError = "CNCSSocket::Connect, Invalid Endpoint";
		m_eLastError = NCS_NET_COULDNT_CONNECT;
		return false;
	}
	
	sHostPort = host;
	sHostPort += ":";
	_itoa( port, szTmp, 10 );
	sHostPort += szTmp;
	
	// Initialise the sndCall structure and call OTConnect.  We nil
	// out most of the fields in the sndCall structure because
	// we don't want any special options or connection data.
	// The important field of the sndCall is the addr TNetBuf,
	// which we initialise to the
	
	sndCall.addr.buf 	= (UInt8 *) &hostDNSAddress;
	sndCall.addr.len 	= OTInitDNSAddress( &hostDNSAddress, (char *)(sHostPort.c_str()) );
	sndCall.opt.buf 	= nil;		// no connection options
	sndCall.opt.len 	= 0;
	sndCall.udata.buf 	= nil;		// no connection data
	sndCall.udata.len 	= 0;
	sndCall.sequence 	= 0;		// ignored by OTConnect
	
	err = OTConnect(m_OTEndpoint, &sndCall, nil);

	if( err != noErr ) {		
		//throw( CNCSException( "CNCSSocket::Connect, OTConnect failed", NCS_NET_COULDNT_CONNECT ) );
		m_sLastError = "CNCSSocket::Connect, OTConnect failed";
		m_eLastError = NCS_NET_COULDNT_CONNECT;
		return false;
	}
	
	m_sConnectedTo = host;
#else
	
	// determine whether host is a name or an IP, and fill _data accordingly
	struct in_addr iahost;
	
	iahost.s_addr = inet_addr(host.c_str());
	if(iahost.s_addr == INADDR_NONE)
	{
		// it's not an IP address. Hopefully it's a name
		m_pConnectedTo = gethostbyname(host.c_str());

		if( m_pConnectedTo == NULL ) {
			//throw( CNCSException( "CNCSSocket::Connect, gethostbyname", NCS_NET_COULDNT_RESOLVE_HOST ) );
			m_sLastError = "CNCSSocket::Connect, gethostbyname";
			m_eLastError = NCS_NET_COULDNT_RESOLVE_HOST;
			return false;
		}
	}
	else
	{
		// it's an IP
		m_pConnectedTo = gethostbyaddr((const char *)&iahost, sizeof(struct in_addr), AF_INET);
		if( m_pConnectedTo == NULL ) {
			//throw( CNCSException( "CNCSSocket::Connect, gethostbyaddr()", NCS_NET_COULDNT_RESOLVE_HOST ) );
			m_sLastError = "CNCSSocket::Connect, gethostbyaddr()";
			m_eLastError = NCS_NET_COULDNT_RESOLVE_HOST;
			return false;
		}
	}
	
	m_connectedPort = port;
	
	m_handle = socket(PF_INET, SOCK_STREAM, 0);
	if(m_handle == -1) {
		//throw( CNCSException( "CNCSSocket::Connect, gethostbyaddr()", NCS_NET_COULDNT_RESOLVE_HOST ) );
			m_sLastError = "CNCSSocket::Connect, gethostbyaddr()";
			m_eLastError = NCS_NET_COULDNT_RESOLVE_HOST;
			return false;
		}
	
	struct sockaddr_in cHost;
	cHost.sin_family      = PF_INET;
	cHost.sin_port        = htons(port);
	cHost.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr*)m_pConnectedTo->h_addr)));
	
	if(connect(m_handle, (struct sockaddr*)&cHost, sizeof(struct sockaddr)) == -1) {
		//throw( CNCSException( "CNCSSocket::Connect, connect()", NCS_NET_COULDNT_CONNECT ) );
		m_sLastError = "CNCSSocket::Connect, connect()";
		m_eLastError = NCS_NET_COULDNT_CONNECT;
		return false;
	}
	
#endif

	m_connected = true;
	return true;
}

/**
 * If connected, disconnects
 *
 * @see Connected()
 * @see m_handle
 * @see m_connected
 */
bool CNCSSocket::Disconnect()
{
	if(Connected())
	{
#ifdef MACINTOSH
		OSStatus 	err	= noErr;
		OTResult 	lookResult;
		
		lookResult = OTLook(m_OTEndpoint);

		switch (lookResult) {

			case T_DISCONNECT:
				// If we get a T_DISCONNECT event, the remote peer
				// has disconnected the stream in a dis-orderly
				// fashion.  HTTP servers will often just disconnect
				// a connection like this to indicate the end of the
				// data, so all we need do is clear the T_DISCONNECT
				// event on the endpoint.

				err = OTRcvDisconnect(m_OTEndpoint, nil);
				/*if (err = noErr) {
					err = OTSndDisconnect(m_OTEndpoint, nil);
				}*/
				break;
				
			case T_ORDREL:
				// If we get a T_ORDREL event, the remote peer
				// has disconnected the stream in an orderly
				// fashion.  This orderly disconnect indicates that
				// the end of the data.  We respond by clearing
				// the T_ORDREL, and then calling OTSndOrderlyDisconnect
				// to acknowledge the orderly disconnect at
				// the remote peer.
				
				err = OTRcvOrderlyDisconnect(m_OTEndpoint);
				if (err == noErr) {
					err = OTSndOrderlyDisconnect(m_OTEndpoint);
				}
				break;
				
			default:
				// Leave err as kOTLookErr.
				break;
		}


		if ( m_bBound ) {
			err = OTUnbind(m_OTEndpoint);
			if(err != noErr) {
				//throw CNCSException("CNCSSocket::Disconnect OTUnbind failed", NCS_INVALID_CONNECTION);
				m_sLastError = "CNCSSocket::Disconnect OTUnbind failed";
				m_eLastError = NCS_INVALID_CONNECTION;
				return false;
			}
		}
		
		// Clean up.
		if (m_OTEndpoint != kOTInvalidEndpointRef) {
			err = OTCloseProvider(m_OTEndpoint);
			if(err != noErr) {
				//throw CNCSException("CNCSSocket::Disconnect OTCloseProvider failed", NCS_INVALID_CONNECTION);
				m_sLastError = "CNCSSocket::Disconnect OTCloseProvider failed";
				m_eLastError = NCS_INVALID_CONNECTION;
				return false;
			}
		}

#else
		shutdown(m_handle, 2);
#endif
		m_connected = false;
	}
	
	return true;
}

/**
 * Reads from socket.
 * Read up to numChars into a string over a socket connection.
 *
 * @param buffer string to place read data into
 * @param numChars maximum number of bytes to read
 * @return the number of characters actually read.
 * @see Read( char *, int )
 * @par
 * PRECONDITION: must be connected
 * PRECONDITION: 0 < numChars <= SSIZE_MAX
 */
int CNCSSocket::Read(string& buffer, int numChars)
{
	_ASSERT(Connected());
	_ASSERT((0 < numChars) && (numChars <= NCS_SSIZE_MAX));
	
	int retval = 0;
	char *temp = new char[numChars+1];
	
	retval = Read( temp, numChars );
	
	buffer = temp;
	delete[] temp;
	
	return retval;
}

/**
 * Reads from socket.
 * Read up to numChars into a char buffer over a socket connection.
 *
 * @param buffer char buffer to place read data into
 * @param numChars maximum number of bytes to read
 * @return the number of characters actually read.
 * @see CNCSException::CNCSException()
 * @par
 * PRECONDITION: must be connected
 * PRECONDITION: 0 < numChars <= SSIZE_MAX
 */
int CNCSSocket::Read(char *buffer, int numChars) {
	_ASSERT(Connected());
	_ASSERT((0 < numChars) && (numChars <= NCS_SSIZE_MAX));
	
	int retval = 0;
	
#ifdef MACINTOSH
	OSStatus 	err;
	OTResult 	bytesSent;
	OTResult 	bytesReceived;
	OTFlags 	junkFlags;
	OTResult 	lookResult;
	Ptr   		readBuffer = nil; 

	readBuffer = (Ptr)OTAllocMemInContext(numChars,s_pClientContext);

	if( readBuffer ) {
		bytesReceived = OTRcv(m_OTEndpoint, readBuffer, numChars, &junkFlags);
		if( bytesReceived > 0 ) {
			memcpy(buffer, readBuffer, bytesReceived);
			OTFreeMem(readBuffer);
		}
	}
		
	// OTRcv returns the number of bytes received.  Because we're in
	// synchronous mode, it won't return until it's sent all the
	// bytes, or it gets an error.  
		
	retval = err = bytesReceived;
		
	// handle errors
	if (err < 0 ){// == kOTLookErr) {
		Disconnect();
		//throw( CNCSException( "CNCSSocket::Read, OTRcv()", NCS_NET_PACKET_RECV_FAILURE ) );
		//return err;
	}
	if( retval < 0 ) retval = 0;


#else
	
	retval = recv(m_handle, buffer, numChars, 0);
	if( retval == -1 ) {
		m_connected = false;
		//throw( CNCSException( "CNCSSocket::Read, recv()", NCS_NET_PACKET_RECV_FAILURE ) );
		return -1;
	}

#endif

	return retval;
}

/**
 * Write data to remote host.
 * Writes data contained in a string to the remote host.
 *
 * @param data contains data to be written
 * @see Write( const char*, int )
 */
bool CNCSSocket::Write(const string& data)
{
	_ASSERT(Connected());
	
//	int bs = 0;
//	unsigned int bcount = 0;
	
	return Write( data.c_str(), data.size() );
}

/**
 * Write data to remote host.
 * Writes data contained in a char buffer to the remote host.
 *
 * @param data contains data to be written
 * @param numChars number of bytes in buffer
 */
bool CNCSSocket::Write(const char *data, int numChars) {
	_ASSERT(Connected());
	
#ifdef MACINTOSH
	OSStatus 	err=noErr;
	OTResult 	bytesSent=0;
	OTResult 	lookResult=0;
	Ptr   		writeBuffer = nil;

	writeBuffer = (Ptr)OTAllocMemInContext(numChars,s_pClientContext);

	if( writeBuffer && (numChars > 0) ) {
		memcpy(writeBuffer, data, numChars);
		bytesSent = OTSnd(m_OTEndpoint, (void *)writeBuffer, numChars, 0);
		OTFreeMem(writeBuffer);
	} else return false;
	
	// OTSnd returns the number of bytes sent.  Because we're in
	// synchronous mode, it won't return until it's sent all the
	// bytes, or it gets an error.
	
	if (bytesSent == numChars) {
		err = noErr;
	} else {
		err = bytesSent;
	}
	
	// Handle errors
	if (err < 0) {//err == kOTLookErr) {
		Disconnect();
		//throw( CNCSException( "CNCSSocket::Write, OTSnd()", NCS_NET_PACKET_SEND_FAILURE ) );
		return false;
	}
	
#else
	int bs = 0;
	unsigned int bcount = 0;
	
	while((int)bcount < numChars)
	{
		if((bs = send(m_handle, data+bcount, numChars-bcount, 0)) == -1) {
			m_connected = false;
			//throw( CNCSException( "CNCSSocket::Write, send()", NCS_NET_PACKET_SEND_FAILURE ) );
			return false;
		}
		bcount += bs;
	}
#endif

	return true;
}
