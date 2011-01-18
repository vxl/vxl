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
** FILE:   	NCScnet3\NCSRequest.cpp
** CREATED:	12-4-2001
** AUTHOR: 	Russell Rogers
** PURPOSE:	
** EDITS:
** [01]
*******************************************************/

// NCSRequest.cpp: implementation of the CNCSRequest class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <winsock2.h>
#endif

#include <iostream>
#include <sstream>
#include <ctype.h>
#include <assert.h>

#include "NCSRequest.h"
#include "NCSUtil.h"
#include "NCSBase64.h"
#include "NCSCrypto.h"

#define _DEBUG
#ifdef MACINTOSH
	#define NCS_SEND_TIMEOUT 0
#else
#ifdef _DEBUG
	#define NCS_SEND_TIMEOUT 5000000
#else
	#define NCS_SEND_TIMEOUT 5000
#endif
#endif //MACINTOSH

using namespace std;

/** Dialog to prompt for Username and Password.
 */
bool NCSGetPasswordDlg( const char *pszSite, const char *pszRealm, string &sUsername, string &sPassword, bool &bRememberAuth );

/** @class CNCSRequest
 *  @brief Generic class for performing HTTP requests.
 *
 *  Provides functionally to allow for the sending and recieving of HTTP requests.
 *
 *  @author	Russell Rogers
 *  @date	10 April 2001
 *  @par Edits:
 *
 */

string CNCSRequest::sm_sUserName;
string CNCSRequest::sm_sPassword;
bool CNCSRequest::sm_bRememberAuth = true;
CNCSMutex CNCSRequest::sm_mAuthMutex;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSRequest::CNCSRequest()
{
	m_pSocket=0;
	m_sUrl="";

	m_sMethod="";
	m_sBody="";
	m_sHttpVer="HTTP/1.0";
	m_nSendStatus = 0;
	
	m_psServerName=0;
	m_pnServerPort=0;
	m_pnID=0;
	m_pProxy = 0;
	m_psIISDLLName=0;
	
	m_nAuthRetries=0;

	m_bPolling = false;
	
	m_nSendTimeOut = NCS_SEND_TIMEOUT;
	
}

CNCSRequest::~CNCSRequest()
{
	Disconnect();

	if( NCSThreadIsRunning(&m_tSendRequestThread) ) {
		NCSThreadTerminate( &m_tSendRequestThread );
	}
    
	if( m_pSocket ) {
		delete m_pSocket;
	}
	m_pSocket = NULL;
	m_responseHeader.Clear();

}

//////////////////////////////////////////////////////////////////////
// Member function
//////////////////////////////////////////////////////////////////////

void CNCSRequest::SetShared( string *psServerName, UINT32 *pnServerPort, UINT64 *pnID, CNCSProxy *pProxy, string *psIISDLLName ) {
	m_psServerName =	psServerName;
	m_pnServerPort =	pnServerPort;
	m_pnID =			pnID;
	m_psIISDLLName =	psIISDLLName;
	m_pProxy =			pProxy;
}

bool CNCSRequest::Connect( ) {
	_ASSERT( m_psServerName != 0 );
	_ASSERT( m_pnServerPort != 0 );
	
	if( Connected() ) Disconnect();
	
	if( m_pProxy->m_bUseProxy ) {
        if( m_pSocket ) delete m_pSocket;
		m_pSocket = new CNCSSocket();
		if( m_pSocket && !m_pSocket->Connect(m_pProxy->m_sProxyName, m_pProxy->m_nProxyPort) ) {
			//cerr << "Connection failed: " << m_pProxy->m_sProxyName << endl;
			return false;
		}
		// remove Host field from request header
		m_header.Set( "Host", m_pSocket->ConnectedTo() );
		//m_header.Remove( "Host" );
		
#ifdef DEBUGINFO 
		cout << "connect: " << m_pProxy->m_sProxyName << endl;
#endif
#ifdef _DEBUG
		DEBUGPRINT("connect\n" );
#endif
	}
	else {
        if( m_pSocket ) delete m_pSocket;
		m_pSocket = new CNCSSocket();
		if( m_pSocket && !m_pSocket->Connect(*m_psServerName, *m_pnServerPort) ) {
			//cerr << "Connection failed: " << *m_psServerName << endl;
			return false;
		}
		// add Host field to request header
		m_header.Set( "Host", m_pSocket->ConnectedTo() );
		
#ifdef DEBUGINFO 
		cout << "connect: " << *m_psServerName << endl;
#endif
	}
	
	
	return true;
	
}

bool CNCSRequest::Connected() {
	if( m_pSocket != 0 ) 
		return m_pSocket->Connected();
	else return false;
}

void CNCSRequest::Disconnect() {
	if( m_pSocket ) {
		m_pSocket->Disconnect();
	}
}

bool CNCSRequest::Open() {
	_ASSERT( m_sMethod != "" );
	_ASSERT( m_sHttpVer != "" );
	_ASSERT( Connected() );
	
	string strTemp;
	
	strTemp = m_sMethod;
	strTemp += " ";
	strTemp += GetUrl();
	strTemp += " ";
	strTemp += m_sHttpVer;
	strTemp += "\r\n";
	
#ifdef DEBUGINFO 
	cout << "open: " << strTemp;
#endif
	
	if( !m_pSocket->Write( strTemp ) ){
		//cerr << "Open: Write failed." << m_pSocket->ConnectedTo() << endl;
		return false;
	}
	
	return true;
}

bool CNCSRequest::SetMethod( string method ) {
	
	StringToUpper( method, m_sMethod );
	
	if( !(m_sMethod == "GET" || m_sMethod == "POST") ) { // check method is valid
		m_sMethod = "";
		return false;
	}
	
	m_sMethod=method;
	return true;
}

string CNCSRequest::GetUrl() {
	string sUrl;

	if( m_sUrl == "" ) {
		char *pszRequest = new char[ m_psIISDLLName->size() + MAX_PATH + 10 ];
		if( m_pProxy->m_bUseProxy ) {
			sprintf(pszRequest, 
#if defined( WIN32 )
					"http://%s:%d%s?%I64u,%ld,%ld",
#elif defined(POSIX) || defined( MACINTOSH )
					"http://%s:%d%s?%llu,%ld,%ld",
#endif // WIN32
				m_psServerName->c_str(), *m_pnServerPort, m_psIISDLLName->c_str(), *m_pnID, (long)3,//NCS_PROTOCOL_VERSION,
				(long)(m_bPolling?TRUE:FALSE) );
		}
		else {
			sprintf(pszRequest, 
#if defined( WIN32 )
				"%s?%I64u,%ld,%ld", 
#elif defined(POSIX) || defined( MACINTOSH )
				"%s?%llu,%ld,%ld",
#endif // WIN32
				m_psIISDLLName->c_str(), *m_pnID, (long)NCS_PROTOCOL_VERSION,
				(long)(m_bPolling?TRUE:FALSE) );
		}
		
		sUrl = pszRequest;
		delete [] pszRequest;
	}
	else {
		return m_sUrl;
	}

	return sUrl;
}

bool CNCSRequest::SetHttpVer( string http_ver ) {
	m_sHttpVer=http_ver;
	return true;
}

string &CNCSRequest::CompleteHeader( string &str ) {
	
	m_header.ToString(str);
	str += "\r\n";
	
	return str;
}


/** Send http request header and data.
* A new thread is spawned to do the send so that a time out interval
* can be enforced.
*/
HttpSendRequestResult CNCSRequest::Send( char *body, INT32 nlength, NCSTimeStampMs tsTimeOut ) {
	
	bool Result = false;
    //tsTimeOut=0;
	
	m_sBody.assign( body, nlength );
	
	if((tsTimeOut != 0) && NCSThreadSpawn(&m_tSendRequestThread, SendRequestThread, this, FALSE)) {
		NCSTimeStampMs tsEndTime = NCSGetTimeStampMs() + tsTimeOut;
		
		while(NCSThreadIsRunning(&m_tSendRequestThread) && (NCSGetTimeStampMs() < tsEndTime)) {
			NCSSleep(50);
		}
		if(NCSGetTimeStampMs() >= tsEndTime) {
			// Timed out!
			NCSThreadTerminate( &m_tSendRequestThread );
			return NCS_HTTP_SEND_TIMEOUT;
		}
	} else {
		Send();
	}
	
	if( !(m_nSendStatus == 200 || m_nSendStatus == 204) ) {
		return NCS_HTTP_SEND_FAILED;
	}
	
	return(NCS_HTTP_SEND_SUCCESS);
}


INT32 CNCSRequest::Send( ) {
	_ASSERT( m_pSocket != 0 );
	_ASSERT( m_sMethod != "" );
	//_ASSERT( m_sUrl != "" );
	
	
	//_ASSERT( Connected() );
	if( !Connected() ) {
		m_nSendStatus = 0;
		return 0;
	}
	
	INT32 nCode;
	string strTemp="";
	string strInt="";
	m_nSendStatus = 0;
	
	if( m_sBody.size() > 0 ) m_header.Set("Content-Length", IntToString( m_sBody.size(), strInt ) );
	CompleteHeader( strTemp );
	
#ifdef DEBUGINFO
	cout << strTemp << endl;
#endif
	
	// Attach the header to the body
	strTemp += m_sBody;
	
	if( !m_pSocket->Write( strTemp ) ){
		//cerr << "Send: Write Failed." << m_pSocket->ConnectedTo() << endl;
		m_nSendStatus = 0;
		return 0;
	}
	
	
	nCode = ReadStatus();
	m_nLastStatus = nCode;
	
	if( nCode != 0 ) ReadHeader();
	
	m_nSendStatus = nCode;
	return nCode;	
}

INT32 CNCSRequest::Send( string &data ) {
	
	m_sBody = data;
	
	return Send( );
}


bool CNCSRequest::SendARequest( string sUrl, string sMethod, string sHeader, string sData ) {
	if( Connected() ) Disconnect();
	
	string sServerName;
	string sUrlEnd, sPort;
	bool bResult=false;
	UINT32 nPort=80;

	int nStart = strlen("http://");
	int nEnd = sUrl.find( "/", nStart ); // find end of server name
        if( nEnd == -1 ) return false;
	sServerName = sUrl.substr( nStart, nEnd-nStart );
	sUrlEnd = sUrl.substr( nEnd, sUrl.size() );

	// extract port from server name
	nStart = sServerName.find( ":", 0 );
	if( nStart == string::npos ) { // no port
		nPort = 80;
	} else {
		nEnd = sServerName.size(); // find end of port number
		sPort = sServerName.substr( nStart+1, nEnd );
		nPort = atoi( sPort.c_str() );
		sServerName.erase( nStart, nEnd );
	}
	
	m_pProxy = new CNCSProxy( sServerName );
	m_psServerName = &sServerName;
	
	m_pnServerPort = &nPort;
	m_sMethod = sMethod;

	if( m_pProxy->m_bUseProxy )
		m_sUrl = sUrl;
	else m_sUrl = sUrlEnd;

    //printf("Url=%s\nBody=%s\n", m_sUrl.c_str(), sData.c_str() );
    
    //escape the url (remove any whitespace)
    int nSpacePos = m_sUrl.find( " ", 0 );
    while( nSpacePos != string::npos ) {
        //replace the space with "%20"
        m_sUrl.replace(nSpacePos, 1, "%20");
        
        nSpacePos = m_sUrl.find( " ", nSpacePos );
    }

    //printf("Url=%s\nBody=%s\n", m_sUrl.c_str(), sData.c_str() );

	bool bRedirect = false;
	int nRedirectCount = 0;
	
	do { // Support server redirects
		bRedirect = false;
		nRedirectCount++; // prevent infinite loop in redirections (max 5)
		
		m_responseHeader.Clear();
		m_header.Clear();
		m_header.Set( "Connection", "close" );
		m_header.Set( sHeader );
		
		if( Connect() ) {
			if( Open() ) {
				int nResult = Send( sData );
				
				switch( nResult ) {
				case 200:
				case 204:
					bResult = true;
					break;
				case 300: // if response code is a redirect
				case 301:
				case 302:
				case 303:
				{
					string sLocation;
					// check for a Location field in the response header
					if( m_responseHeader.Get( "Location", sLocation ) ) {
						bRedirect = true;
						m_sUrl = sLocation;
					} else {
						bRedirect = false;
						bResult = false;
					}
					break;
				}
				default:
					bResult = false;
					break;
				}
			}
		}
	} while( bRedirect && (nRedirectCount < 5) );
	
	delete m_pProxy;
	return bResult;
}


int CNCSRequest::ReadStatus() {
	// read first line of response to get status
	string strStatus="";
	char c=0;
	int nCode=0;
	
	if( !Read(c) ) return 0;
	while( c != '\n' ) {
		if( c != '\r' )	strStatus += c;
		if( !Read(c) ) return 0;
	}
	
#ifdef DEBUGINFO
	cout << strStatus << endl << endl;
#endif
	
	int nVerPos=0;
	int nCodePos=0;
	nVerPos = strStatus.find( " ", 0 );
	string strVer = strStatus.substr(0, nVerPos);
	nCodePos = strStatus.find( " ", nVerPos+1 );
	string strCode = strStatus.substr( nVerPos+1, nCodePos-nVerPos-1 );
	string strReason = strStatus.substr( nCodePos+1, strStatus.size()-nCodePos-1 );
	
	nCode = atoi( strCode.c_str() );
	
	return nCode;
}

bool CNCSRequest::ReadHeader() {
	string strHeader="";
	char c;
	while( ((strHeader.size() > 4)?(strHeader.compare( strHeader.size()-4, 4, string("\r\n\r\n") ) != 0) : true )) {
		Read(c);
		strHeader += c;
	}
	
	m_responseHeader.Clear();
	m_responseHeader.Set( strHeader );

#ifdef DEBUGINFO
	string sTemp;
	cout << m_responseHeader.ToString( sTemp ) << endl;
#endif

	return true;
}

int CNCSRequest::Read( string &body, int nSize ) {
	
	char *szTemp = new char[nSize];
	
	int retval = Read( szTemp, nSize );
	body = szTemp;
	
	delete szTemp;
	return retval;
}

int CNCSRequest::Read( char *body, int nSize ) {
	if( !Connected() ) return 0;
	
	string tmp;
	
	return m_pSocket->Read( body, nSize );
	
}

bool CNCSRequest::Read( char &c ) {
	int count;
	
	count = Read( &c, 1 );
	
	if( count == 1 ) {
		return true;
	}
	else return false;
}

bool CNCSRequest::Read( UINT32 &n ) {	
	union {
		UINT32 n;
		char bytes[4];
	} uData;
	
	int count = Read( uData.bytes, 4 );
	
	if( count == 4 ) {
#ifdef NCSBO_MSBFIRST
		// byte swap
		n = NCSByteSwap32(uData.n);
#else
		n = uData.n;
#endif
		return true;
	}
	else return false;	
}

bool CNCSRequest::Read( INT64 &n ) {
	union {
		INT64 n;
		char bytes[8];
	} uData;
	
	int count = Read( uData.bytes, 8 );
	
	if( count == 8 ) {
#ifdef NCSBO_MSBFIRST
		// byte swap
		n = NCSByteSwap64(uData.n);
#else
		n = uData.n;
#endif
		return true;
	}
	else return false;	
}

NCSError CNCSRequest::ConnectToServer(void *pData, INT32 nDataLength) {
	
	NCSError eError = NCS_SUCCESS;

//	try {
	
		if( !Connected() ) { // not connected
			
			if( Connect( ) ) {
				
				if( Open() ) {
					
					// Set message body				
					HttpSendRequestResult nResult = Send( (char *)pData, nDataLength, m_nSendTimeOut );
					switch( nResult ) {
					case NCS_HTTP_SEND_TIMEOUT:
						eError = NCS_NET_HEADER_SEND_TIMEOUT;
						break;
						
					case NCS_HTTP_SEND_SUCCESS:
                                                m_nAuthRetries = 0;
						eError = NCS_SUCCESS;

						break;
						
					case NCS_HTTP_SEND_FAILED:
						eError = NCS_NET_COULDNT_CONNECT;

						if( m_nLastStatus == 401 ) {
							eError = NCS_NET_401_UNAUTHORISED;
							if( Authenticate() ) {
								//Retry send
								Disconnect();
								eError = ConnectToServer( pData, nDataLength );
							}
						}
						else if( m_nLastStatus == 407 ) {
							DEBUGPRINT("Proxy Auth. Required.\n");
							eError = NCS_NET_407_PROXYAUTH;
							string sProxyAuth;
							if( !m_pProxy->GetProxyAuthentication( sProxyAuth ) ) break; // if user cancels
							m_header.Set( sProxyAuth );
							//Retry send
							Disconnect();
							eError = ConnectToServer( pData, nDataLength );
							m_pProxy->m_bPromptUser = false;
						}
						break;
					}
					
				} else {
					eError = NCS_NET_COULDNT_CONNECT;
				}
				if(eError != NCS_SUCCESS) {
					DEBUGPRINT("Request Send failed\n");
					Disconnect();
				}
			} else {	
				eError = NCS_NET_COULDNT_CONNECT;
			}
		} else { // don't reconnect
			eError = NCS_NET_ALREADY_CONNECTED;		
		}
//	} catch( CNCSException &ex ) {
//		eError = ex.m_eError;
//	}
	
	return eError;
}


bool CNCSRequest::Authenticate() {
        CNCSMutexLock lockMutex( &sm_mAuthMutex );
        
        bool bUseCurrent = false;
        m_nAuthRetries++;
        
	// get username and password
        if( !sm_bRememberAuth ) {
            sm_sUserName = "";
            sm_sPassword = "";
        }
        
        if( (m_nAuthRetries <= 1) && sm_bRememberAuth ) {
            bUseCurrent = true;
        }
        
	// Check authentication realm
	HeaderListIterator hfRealm;
	hfRealm = m_responseHeader.Find("WWW-Authenticate");
	if( hfRealm == m_responseHeader.End() ) return false;
	//if( hfRealm->value.find("Image Web Server Security") == -1 ) return;

	// prompt for password
	if( bUseCurrent || NCSGetPasswordDlg( m_psServerName->c_str(), "Image Web Server Security", sm_sUserName, sm_sPassword, sm_bRememberAuth ) ) {
		
		// encrypt password
		if( (sm_sUserName.size()!=0)&& (sm_sPassword.size()!=0)) {
			char szBase64Msg[1024] = {'\0'};
			char szCredentials[1024] = {'\0'};
			char szHeader[1024] = {'\0'};
			
			strcat(szCredentials,sm_sUserName.c_str());
			strcat(szCredentials,":");
			strcat(szCredentials,sm_sPassword.c_str());
			
			NCSEncodeBase64Message(szCredentials,szBase64Msg);
			// add to request header
			sprintf(szHeader,"Authorization: Basic %s",szBase64Msg);
			m_header.Set( szHeader );
		}
                
                return true;
	} else {
            return false;
        }
}

//////////////////////////////////////////////////////////////////////
// CNCSHeader Member function
//////////////////////////////////////////////////////////////////////

/** @class CNCSHeader
 *  @brief Handles request and response headers.
 *
 *  Allows for the construction of http headers with the ability to
 *  add, find and remove fields.  Also used to parse the response header
 *  to extract header fields returned by the server.
 *
 *  @author	Russell Rogers
 *  @date	10 April 2001
 *  @par Edits:
 *
 */

CNCSRequest::CNCSHeader::CNCSHeader() {
}

CNCSRequest::CNCSHeader::~CNCSHeader() {
}

bool CNCSRequest::CNCSHeader::Set( const string &strHeader ) {
	// paser input string and add header fields
	string str;
	int pos=0;
	HeaderField field;
	
	str = strHeader;
	
	// remove leading spaces
	while( isspace( (int)str[0] ) ) {
		str.erase(0,1);
	}
	
	while( str.size() != 0 ) {
		
		pos = str.find( ":" ); // find name, value seperator
		if( pos == -1 ) return false;
		
		field.name = str.substr( 0, pos ); // copy field name
		str.erase(0, pos+1 ); // remove field name from input string
		
		// remove field name trailing spaces
		while( isspace( (int)field.name[field.name.size()-1] ) ) {
			field.name.erase(field.name.size()-1,1);
		}
		
		// remove leading spaces
		while( isspace( (int)str[0] ) ) {
			str.erase(0,1);
		}
		pos = str.find( "\r\n" ); // find field terminator
		if( pos == -1 ) { // end of string reached
			pos = str.size();
			field.value = str.substr( 0, pos ); // copy field value
			str.erase(0, pos ); // remove field value from input string
		}
		else {
			field.value = str.substr( 0, pos ); // copy field value
			str.erase(0, pos+2 ); // remove field value from input string
		}
		
		// remove field value trailing spaces
		while( isspace( (int)field.value[field.value.size()-1] ) ) {
			field.value.erase(field.value.size()-1,1);
		}
		
		Set( field );
		
		// remove leading spaces
		while( isspace( (int)str[0] ) ) {
			str.erase(0,1);
		}
		
	}
	
	return true;
}

bool CNCSRequest::CNCSHeader::Clear() {
	
	m_header.clear();
	
	return true;
}

string &CNCSRequest::CNCSHeader::ToString( string &str ) {
	HeaderListIterator pos;
	
	str=""; // clear string
	
	for( pos = m_header.begin(); pos < m_header.end(); pos++ ) {
		str += pos->name;
		str += ": ";
		str += pos->value;
		str += "\r\n";
	}
	
	return str;
}

bool CNCSRequest::CNCSHeader::Remove( const string &name ) {
	HeaderListIterator pos;
	
	if( (pos = Find( name )) != End() ) {
		m_header.erase( pos );
	}
	else return false;
	
	return true;
}

bool CNCSRequest::CNCSHeader::Set( const HeaderField &field ) {
	return Set( field.name, field.value );
}

bool CNCSRequest::CNCSHeader::Set( const string &name, const string &value ) {
	HeaderListIterator pos;
	HeaderField field;
	
	if( name.size() < 1 ) return false;  //ensure string is not empty
	
	if( (pos = Find( name )) != End() ) {
		pos->value = value;
	}
	else {
		field.name = name;
		field.value = value;
		m_header.push_back( field );
	}
	
	return true;
}

bool const CNCSRequest::CNCSHeader::Get( const string &name, string &value ) {
	HeaderListIterator pos;
	
	if( (pos = Find( name )) != End() ) {
		value = pos->value;
	}
	else return false;
	
	return true;
}

bool const CNCSRequest::CNCSHeader::Get( const string &name, int &value ) {
	HeaderListIterator pos;
	
	if( (pos = Find( name )) != End() ) {
		value = atoi( pos->value.c_str() );
	}
	else return false;
	
	return true;
}

HeaderListIterator const CNCSRequest::CNCSHeader::Find( const string &name ) {
	HeaderListIterator pos;
	string strA, strB;
	
	for( pos = m_header.begin(); pos < m_header.end(); pos++ ) {
		StringToUpper( name, strA );	// ignore case
		StringToUpper( pos->name, strB );
		
		if( strA.compare( strB ) == 0 ) return pos;
		//if( name.compare( pos->name ) == 0 ) return pos;
	}
	return m_header.end();
}

int const CNCSRequest::CNCSHeader::Size() {
	return m_header.size();
}
