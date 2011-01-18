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
** FILE:   	NCScnet3\NCScnet.cpp
** CREATED:	12-4-2001
** AUTHOR: 	Russell Rogers, Simon Cope
** PURPOSE:	
** EDITS:
** [01]
*******************************************************/

#include "cnet.h"
#ifdef WIN32
#include <windows.h>
#include "winver.h"
#elif defined(POSIX)

#elif defined(MACINTOSH)
#include <extras.h>

#endif // WIN32
#include "NCSTimer.h"
#include <string>
#include "NCSBase64.h"
#include "NCSCrypto.h"
#include "NCSProxy.h"
#include <iostream>
#include "NCSUtil.h"

using namespace std;

// Globals
static INT32 nClientCount = 0;
NCSMutex mSocketStartup;

//////////////////////////////////////////////////////////////////////
// NCScnet entry point functions
//////////////////////////////////////////////////////////////////////

BOOLEAN NCScnetVerifyURL(char *szURL)
{
	if (!szURL) return(FALSE);
	return (strnicmp(szURL, "ecwp://", 7) == 0);
}

NCSError NCScnetCreateEx(char *szURL, void **ppClient, void *pPacketIn, int nLengthIn, 
						 void **ppPacketOut, INT32 *pLengthOut, NCSnetPacketRecvCB *pRecvCB, 
						 void *pRecvUserdata, char *pszIISDLLName)
{
	CNCScnet *pConn;
	NCSError eError=NCS_SUCCESS;

	if(!szURL || !pPacketIn || !ppPacketOut || !pLengthOut || !ppClient)
		return NCS_INVALID_ARGUMENTS;
	
	//Create connections
	pConn = new CNCScnet();
	if( pConn ) {
		eError = pConn->Create(szURL, pszIISDLLName);
		if( eError != NCS_SUCCESS ) return eError;
	}

	if(!pConn) {
		return(NCS_COULDNT_ALLOC_MEMORY);
	}
	else {
		eError = pConn->Start(ppClient, pPacketIn, nLengthIn, ppPacketOut, pLengthOut, pRecvCB, pRecvUserdata);
		if( eError != NCS_SUCCESS ) {
			delete pConn;
			*ppClient = NULL;
		}
	}

	return eError;

}

NCSError NCScnetCreate(char *szURL, void **ppClient, void *pPacketIn, int nLengthIn, void **ppPacketOut,
					   INT32 *pLengthOut, NCSnetPacketRecvCB *pRecvCB, void *pRecvUserdata)
{
	char *szIISDLLName =
#ifdef _DEBUG
		"/ecwp/ecwp.dll";
#else
	"/ecwp/ecwpd.dll";
#endif
	
	return(NCScnetCreateEx(szURL, ppClient, pPacketIn, nLengthIn, ppPacketOut, pLengthOut, pRecvCB, pRecvUserdata, szIISDLLName));
}


void NCScnetDestroy(void *pConnection)
{
	CNCScnet *pConn = (CNCScnet*)pConnection;
	if(pConn) {		
		delete(pConn);
	}
}

void NCScnetSetRecvCB(void *pConnection, NCSnetPacketRecvCB *pRecvCB, void *pUserdata)
{
	CNCScnet *pConn = (CNCScnet *)pConnection;
	
	if(pConn) {
		pConn->m_pRecvCon->SetRecvCB( pRecvCB, pUserdata );
	}
}

void NCScnetSetSentCB(void *pConnection, NCSnetPacketSentCB *pSentCB)
{
	CNCScnet *pConn = (CNCScnet *)pConnection;
	
		  if(pConn) {
			  pConn->m_pSendCon->SetSentCB( pSentCB );
		  }
}

BOOLEAN NCScnetSendPacket(void *pConnection, void *pPacket, int nLength, void *pUserdata, BOOLEAN *bIsConnected)
{
	CNCScnet *pConn = (CNCScnet *)pConnection;
	
	bool bConnected;
	bool result=false;
	
	if( pConn ) {
		result = pConn->m_pSendCon->SendPacket(pPacket, nLength, pUserdata, &bConnected );
		*bIsConnected = bConnected ? TRUE:FALSE;
	}
	return(result?TRUE:FALSE);
	
}

void NCScnetSetSendBlocking(void *pConnection, BOOLEAN bOn) // redundant
{
	DEBUGPRINT("NCScnetSetSendBlocking does nothing.");
}

void NCScnetSetIISDLLName(char *szDLLName) // redundant
{
    char *szDefaultIISDLLName =
#ifdef _DEBUG
"/ecwp/ecwp.dll";
#else
"/ecwp/ecwpd.dll";
#endif

	szDefaultIISDLLName = szDLLName;
	DEBUGPRINT("NCScnetSetIISDLLName does nothing.");
}

void NCScnetInit()
{
//#ifndef WIN32
	NCSMutexInit(&mSocketStartup);
	CNCSSocket::init();
//#endif // WIN32
}
// only necessary if you plan to NCScnetDestroy within the process detach
//
void NCScnetShutdown() // redundant
{
//#ifdef WIN32
//	int i=0;
//	DEBUGPRINT("NCScnetShutdown does nothing.");
//#else
	CNCSSocket::deinit();
	NCSMutexFini(&mSocketStartup);
//#endif
}

NCSError NCScnetPostURL(char *szURL, char *szBody, char *szHeaders, char **ppAccept,
							   BOOLEAN bIsPost, BOOLEAN bUseCache, UINT8**szResponse, int *nRespLength,
							   UINT32 *pnStatusCode, UINT32 *pnContentLength) {
	NCSError eError = NCS_SUCCESS;
	char *p = 0;
	char *pOpenVerb = NULL;

	CNCSRequest request;
	string sMethod;
	string sHeader = szHeaders;

	// Add accept types to header
	if( ppAccept ) {
		string sAccept = "";
		if( *ppAccept != NULL ) { // add first accept type
			sAccept += *ppAccept;
			ppAccept++;
		}
		while( *ppAccept != NULL ) { // add all remaining accept types
			sAccept += ", ";
			sAccept += *ppAccept;
			ppAccept++;
		}
		if( sAccept != "" ) {
			request.m_header.Set( "Accept", sAccept );
		}
	}

	if(bIsPost)
		sMethod = "POST";
	else
		sMethod = "GET";

	if( szURL && (szURL[0] != 0) && request.SendARequest( szURL, sMethod, sHeader, szBody ) ) {
		int nBytesRead = 0;
		char *pData = NULL;
		
		//Check if response has a content length
		int nContentLength = 0;
		if( request.m_responseHeader.Get( "Content-Length", nContentLength ) ) {
			if( pnContentLength ) {
				*pnContentLength = nContentLength;
			}
			
			pData = (char*)NCSMalloc(nContentLength, TRUE);
			
			while( nBytesRead < nContentLength ) {
				unsigned long nThisRead=0;
				
				nThisRead = request.Read( (char*)pData+nBytesRead, nContentLength );
				if(nThisRead > 0) {
					nBytesRead += nThisRead;
				} else {
					break;
				}				
			}
			
		} else {
			pData = (char*)NCSMalloc(1024, TRUE);
			
			while(TRUE) {
				unsigned long nThisRead=0;
				
				nThisRead = request.Read( (char*)pData+nBytesRead, 1024 );
				
				if(nThisRead != 0) {
					nBytesRead += nThisRead;
					
					pData = (char*)NCSRealloc((void*)pData, nBytesRead + 1024, TRUE);
				} else {
					break;
				}
			}
		}
		
		*szResponse = (UINT8*)pData;
		*nRespLength = nBytesRead;

		if( pnStatusCode ) *pnStatusCode = request.m_nLastStatus;

	}
	else {
		eError = NCS_NET_COULDNT_CONNECT;
	}
	return(eError);
}


/** @class CNCScnet
 *  @brief manages the GET and POST connections used to comunicate with server.
 *
 *	Creates and initializes the GET and POST connections that are used to
 *  send and recieve packets with the server.
 *
 *  @author	Russell Rogers
 *  @date	10 April 2001
 *  @par Edits:
 *
 */

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCScnet::CNCScnet() {
	m_nID=0;
	m_sServerName="";
	m_nServerPort=0;

	m_pRecvCon = 0;
	m_pSendCon = 0;
	m_pProxy = 0;
}

NCSError CNCScnet::Create( char *szUrl, char *pszIISDLLName ) {
	m_sIISDLLName=pszIISDLLName;

	NCSMutexBegin(&mSocketStartup);
	nClientCount++;
#ifdef _DEBUG
	char szCount[256];
	sprintf( szCount, "%d\n", nClientCount );
	DEBUGPRINT( szCount );
#endif
	NCSMutexEnd(&mSocketStartup);

	m_pRecvCon = new CNCSGetRequest();
	if( !m_pRecvCon ) {
		Destroy();
		return NCS_COULDNT_ALLOC_MEMORY;
	}
	
	m_pSendCon = new CNCSPostRequest();	
	if( !m_pSendCon ) {
		Destroy();
		return NCS_COULDNT_ALLOC_MEMORY;
	}

	CNCSSocket::init();  // start-up winsock services
	//Done when first socket created

	if( !SetID() ) {
		Destroy();
		return NCS_UNKNOWN_ERROR;
	}
	BreakUpUrl( szUrl );

	//Create proxy information
	m_pProxy = new CNCSProxy(m_sServerName);
	if( !m_pProxy ) {
		Destroy();
		return NCS_COULDNT_ALLOC_MEMORY;
	}

	//Setup up variables that are shared between send and recv connection
	m_pRecvCon->SetShared( &m_sServerName, &m_nServerPort, &m_nID, m_pProxy, &m_sIISDLLName );
	m_pSendCon->SetShared( &m_sServerName, &m_nServerPort, &m_nID, m_pProxy, &m_sIISDLLName );
	
	//Setup connections settings from registry
	return InitializeSettings();
}

CNCScnet::~CNCScnet() {
	Destroy();
}

//////////////////////////////////////////////////////////////////////
// Member function
//////////////////////////////////////////////////////////////////////

void CNCScnet::BreakUpUrl( char *szUrl ) {
	//Get server name and port from szURl
	string sURL = szUrl;
	string sServerPort = "";
	
	int nPos=0;
	
	nPos = sURL.find( "/", 7 );	/* skip "ecwp://" */
	m_sServerName = sURL.substr( 7, nPos-7 );
	
	nPos = sURL.find( ":", nPos );
	if( nPos != -1 ) {
		sServerPort = sURL.substr( nPos+1, sURL.size() );
		m_nServerPort = atoi( sServerPort.c_str() );
	}
	
	if (m_nServerPort == 0) {
		m_nServerPort = NCS_DEFAULT_HTTP_PORT;
	}

}

void CNCScnet::Destroy() {
	if( m_pSendCon ) delete m_pSendCon;
	if( m_pRecvCon ) delete m_pRecvCon;
	if( m_pProxy ) delete m_pProxy;

	NCSMutexBegin(&mSocketStartup);
	nClientCount--;
	NCSMutexEnd(&mSocketStartup);
}

NCSError CNCScnet::Start(void **ppClient, void *pPacketIn, int nLengthIn, void **ppPacketOut,
						 INT32 *pLengthOut, NCSnetPacketRecvCB *pRecvCB, void *pRecvUserdata) {

	NCSError eError = NCS_SUCCESS;
	
	m_pRecvCon->SetRecvCB( pRecvCB, pRecvUserdata );
	
	//m_pRecvCon->SwitchToPolling(); // TEMP: to test server polling mode

	eError = StartupConnections( ppClient, pPacketIn, nLengthIn, ppPacketOut, pLengthOut );
	if( eError != NCS_SUCCESS ) {
		// Switch to polling mode and retry if necessary
		//m_pRecvCon->SwitchToPolling();
		//StartupConnections( ppClient, pPacketIn, nLengthIn, ppPacketOut, pLengthOut );
	}
	return eError;
}

NCSError CNCScnet::InitializeSettings() {
	 
	 //char szUserName[256] = { '\0' };
	 //char szPassword[256] = { '\0' };
	 
	 /* Client Connection Information*/
	 char *pszContentType;
	 char *pszUserAgent;
	 char *pszMaxAge;
	 char *pszExpire;
	 char szContentType[1024] = { '\0' };
	 char szUserAgent[1024] = { '\0' };
	 char szMaxAge[1024] = { '\0' };
	 char szExpire[1024] = { '\0' };
	 
	 //char szSendHeaders[1024] = {'\0'};
	 //char szRecvHeaders[1024] = {'\0'};
	 
	 /* can override the default ISAPI dll names via the registry :) */
	 char *pDllName=0;
	 if (NCSPrefGetUserString("IWS Client Network Dll", &pDllName) == NCS_SUCCESS && pDllName != 0) {
		 m_sIISDLLName = pDllName;
	 }
	 else {
#ifdef MACINTOSH
		 m_sIISDLLName = "/ecwp/ecwp.dll";
#else
		 m_sIISDLLName = "/ecwp/ecwp.dll";
#endif
	 }
	 
	 /* Retrieve Content Type and User Agent from registry settings */
	 if (NCSPrefGetUserString("IWS Client User Agent",&pszUserAgent)==NCS_SUCCESS) {
		 strcpy(szUserAgent,pszUserAgent);
		 NCSFree(pszUserAgent);
		 NCSLog(LOG_LOW, "NCScnetConnectToServer : Reading registry key [\"IWS Client User Agent\"] with %s",szUserAgent);
	 } else {
		 strcpy(szUserAgent,"NCS/1.0");
	 }
	 if (NCSPrefGetUserString("IWS Client Content Type",&pszContentType)==NCS_SUCCESS) {
		 strcpy(szContentType,pszContentType);
		 NCSFree(pszContentType);
		 NCSLog(LOG_LOW, "NCScnetConnectToServer : Reading registry key [\"IWS Client Content Type\"] with %s",szContentType);
	 } else {
		 strcpy(szContentType,"image/gif");
	 }
	 if (NCSPrefGetUserString("IWS Client Max Age",&pszMaxAge)==NCS_SUCCESS) {
		 strcpy(szMaxAge,pszMaxAge);
		 NCSFree(pszMaxAge);
		 NCSLog(LOG_LOW, "NCScnetConnectToServer : Reading registry key [\"IWS Client Max Age\"] with %s",szMaxAge);
		 }	/*	No default value, Simon wanted it removed but
			Mark wanted it optional, this is the
			compromise (code might be difficult to understand)
	 */
	 if (NCSPrefGetUserString("IWS Client Expire",&pszExpire)==NCS_SUCCESS) {
		 strcpy(szExpire,pszExpire);
		 NCSFree(pszExpire);
		 NCSLog(LOG_LOW, "NCScnetConnectToServer : Reading registry key [\"IWS Client Expire\"] with %s",szExpire);
		 }	/*	No default value, Simon wanted it added but
			Mark wanted it optional, this is the
			compromise (code might be difficult to understand)
	 */
	 
	 //Clear Headers
	 m_pRecvCon->m_header.Clear();
	 m_pSendCon->m_header.Clear();
	 
	 //Setup Send Header
	 if (strlen(szMaxAge)>0) {
		 m_pSendCon->m_header.Set( "Cache-Control", (string("no-cache, no-store, ") += szMaxAge) );
	 }
	 else {
		 m_pSendCon->m_header.Set( "Cache-Control", "no-cache, no-store" );
	 }
	 if (strlen(szExpire)>0) {// Need to becareful about szExpire being NULL, i.e. premature "\r\n\r\n"
		 m_pSendCon->m_header.Set( szExpire );
	 }
	 m_pSendCon->m_header.Set( "Pragma", "no-cache" );
	 m_pSendCon->m_header.Set( "Content-Type", szContentType );
	 m_pSendCon->m_header.Set( "User-Agent", szUserAgent );
	 
	 //Setup Recv Header
	 if (strlen(szMaxAge)>0) { /**[05]**/
		 m_pRecvCon->m_header.Set( "Cache-Control", (string("no-cache, no-store, ") += szMaxAge) );
	 }
	 else {
		 m_pRecvCon->m_header.Set( "Cache-Control", "no-cache, no-store" );
	 }
	 if (strlen(szExpire)>0) {/* Need to becareful about szExpire being NULL, i.e. premature "\r\n\r\n"*/
		 m_pRecvCon->m_header.Set( szExpire );
	 }
	 m_pRecvCon->m_header.Set( "Pragma", "no-cache" );
	 m_pRecvCon->m_header.Set( "Content-Type", szContentType );
	 m_pRecvCon->m_header.Set( "User-Agent", szUserAgent );
	 //pRecvCon->m_header.Set( "Connection", "keep-alive" );
	 	
	 INT32 nConnectionType=0;
	 if (NCSPrefGetUserInt("IWS Client Connection Type",&nConnectionType)==NCS_SUCCESS) {
		 if (nConnectionType==1)
			 m_pRecvCon->SwitchToPolling( );
	 }
	 
	 return NCS_SUCCESS;
}


NCSError CNCScnet::StartupConnections( void **ppClient, void *pPacketIn, int nLengthIn, void **ppPacketOut, INT32 *pLengthOut ) {

	NCSError eError = NCS_SUCCESS;
	UINT8 nServerVersion;

	eError = m_pRecvCon->ConnectToServer((void*)NULL, 0);
	if(eError == NCS_SUCCESS) {		
		eError = m_pRecvCon->GetInfoFromServer( &m_nID, &nServerVersion );
		if( eError == NCS_SUCCESS ) {
			eError = DoInitialTransfer(pPacketIn, nLengthIn, ppPacketOut, pLengthOut);
			if (eError == NCS_SUCCESS) {
				if(StartThreads()) {
					*ppClient = this;
					return NCS_SUCCESS;
				}
			}
			DisconnectFromServer();
		}
	}
	return eError;
}


NCSError CNCScnet::DoInitialTransfer(void *pPacketOut, INT32 nPacketOutLength, void **ppPacketIn, INT32 *pPacketInLength)
{
	NCSError eError = NCS_SUCCESS;
	
	if(pPacketOut && (nPacketOutLength != 0)) {
		//m_pSendCon->m_nAuthRetries = 0;
		eError = m_pSendCon->SendRawPacket( pPacketOut, nPacketOutLength );
		if(eError == NCS_SUCCESS) {
			eError = m_pRecvCon->RecvRawPacket( ppPacketIn, pPacketInLength, (bool*)NULL );
			/* Packet back from server */
			if((eError != NCS_SUCCESS) || (*ppPacketIn == NULL) || (*pPacketInLength == 0)) {
				NCSFree(*ppPacketIn);
				if(eError == NCS_SUCCESS) {
					eError = NCS_NET_PACKET_RECV_FAILURE;
				}
			}
		}
	} else {
		eError = NCS_INVALID_ARGUMENTS;
	}
	
	return(eError);
}

BOOLEAN CNCScnet::StartThreads()
{
	if( m_pSendCon->StartThread(this) )
		if( m_pRecvCon->StartThread(this) )
			return true;
		return false;
}

void CNCScnet::StopThreads()
{
	m_pSendCon->StopThread();
	m_pRecvCon->StopThread();
	m_pSendCon->WaitForThreadsToExit();
	m_pRecvCon->WaitForThreadsToExit();

}


/*
** Disconnect from server
*/
void CNCScnet::DisconnectFromServer()
{
	m_pRecvCon->Disconnect();
	m_pSendCon->Disconnect();
}

/*
** Set cnet ID
*/
BOOLEAN CNCScnet::SetID( )
{
	static INT64 nBaseID = 0;
	static UINT32 nIDCount = 0;

	if(nBaseID == 0) {
		/* attempt to initialise base id (ie: top 32 bits of id) to client machine's IP address */
		char szHostname[256];
#ifdef WIN32
		HOSTENT *pHostent;
#elif defined(POSIX)
		struct hostent *pHostent;
#endif // WIN32

#ifdef MACINTOSH
		// Get ipaddress of local machine
		InetInterfaceInfo info;

		if( OTInetGetInterfaceInfo (&info, kDefaultInetInterface) == noErr )	{
			nBaseID = ((INT64) (info.fAddress)) << 32;
		}
	
#else
		int result = gethostname(szHostname, 256);

		if( result == 0  && (pHostent = gethostbyname(szHostname)) != NULL)	{
			/* go through all addresses for this machine and use one that isn't 127.0.0.1 */
			UINT32 *pList = (UINT32 *) pHostent->h_addr_list;

			while(pList  && (*pList == htonl(0x7f000001))) {
				pList++;
			}
			if(pList) {
				nBaseID = ((INT64) (*pList)) << 32;
			}
		}
#endif
	}

	if(nBaseID == 0) {
		return FALSE;
	} else {
#ifdef POSIX
		m_nID = (nBaseID & (INT64)0x7fffffff00000000LL) | (INT64)(NCSGetTimeStampMs() & 0xfff0) | (INT64)nIDCount++;
#else
		m_nID = (nBaseID & (INT64)0x7fffffff00000000) | (INT64)(NCSGetTimeStampMs() & 0xfff0) | (INT64)nIDCount++;
#endif // POSIX
		_RPT2(_CRT_WARN, "SetID: ID = 0x%x,%x\n", (UINT32) (m_nID >> 32), (UINT32) (m_nID & 0xffffffff));
		return TRUE;
	}
}

#ifdef WIN32
//////////////////////////////////////////////////////////////////////
// DLL functions
//////////////////////////////////////////////////////////////////////

/*
** DLLMain() entry point
*/
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  dwReason, LPVOID lpReserved )
{
    switch (dwReason) {
		case DLL_PROCESS_ATTACH:
				NCSMutexInit(&mSocketStartup);
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
				NCSMutexFini(&mSocketStartup);
			break;
    }
    return(TRUE);
}

#endif // WIN32
