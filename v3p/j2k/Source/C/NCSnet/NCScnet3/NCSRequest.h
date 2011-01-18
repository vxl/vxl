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
** FILE:   	NCScnet3\NCSRequest.h
** CREATED:	12-4-2001
** AUTHOR: 	Russell Rogers
** PURPOSE:	
** EDITS:
** [01]
*******************************************************/

// NCSRequest.h: interface for the CNCSRequest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(NCSREQUEST_H)
#define NCSREQUEST_H

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // WIN32

#include <iostream>
#include <sstream>
#include <ctype.h>
#include <assert.h>
#include <string>
#include <vector>
#include "cnetdefs.h"
#include "NCSSocket.h"
#include "NCSProxy.h"
#include "NCScnet.h"

// displayes extra info
#ifdef _DEBUG
#define DEBUGINFO
//#define cout cerr
#endif

typedef enum {
	NCS_HTTP_SEND_FAILED = 0,
	NCS_HTTP_SEND_SUCCESS = 1,
	NCS_HTTP_SEND_TIMEOUT = 2
} HttpSendRequestResult;

//using namespace std;

typedef struct {
	string name;
	string value;
} HeaderField;

typedef std::vector<HeaderField> HeaderList;
typedef HeaderList::iterator HeaderListIterator;


class CNCSRequest  
{
public:
	CNCSRequest();
	virtual ~CNCSRequest();
	
	void SetShared( string *psServerName, UINT32 *pnServerPort, UINT64 *pnID, CNCSProxy *pProxy, string *psIISDLLName );
	NCSError ConnectToServer(void *pData, INT32 nDataLength);
	void Disconnect();

	bool SendARequest( string sUrl, string sMethod, string sHeader, string sData );
	int Read( char *body, int nSize );

protected:
	bool Connect();
	
	bool Connected();
	
	HttpSendRequestResult Send( char *body, INT32 nlength, NCSTimeStampMs tsTimeOut );

	INT32 Send();
	
	int Read( string &body, int size );
	bool Read( char &c );
	bool Read( UINT32 &n );
	bool Read( INT64 &n );
	
	string &CompleteHeader( string &str );
	
	bool ReadHeader();
	int ReadStatus();
	INT32 Send( string &body );
	bool SetMethod( string method );
	string GetUrl();
	bool SetHttpVer( string http_ver );
	bool Open();
	bool Authenticate();
	void ProxyAuthenticate();

        static string sm_sUserName;
        static string sm_sPassword;
        static bool sm_bRememberAuth;
        static CNCSMutex sm_mAuthMutex;
        
	int 		m_nAuthRetries;

	CNCSSocket *m_pSocket;
	
	string		m_sMethod;
	string		m_sBody;
	string		m_sHttpVer;
	
	INT32		m_nSendStatus;
	
	bool		m_bPolling;

	string		*m_psServerName;
	UINT32		*m_pnServerPort;
	UINT64		*m_pnID;
	CNCSProxy	*m_pProxy;
	string		*m_psIISDLLName;
	
	INT32		m_nSendTimeOut;

private:
	static void SendRequestThread( void *data ) {
		CNCSRequest *request = (CNCSRequest *)data;
		request->Send();
#ifndef MAC_PREEMPTIVE
		NCSThreadYield();
#endif
	}

	NCSThread	m_tSendRequestThread;
	string		m_sUrl;

private: // header stuff
	class CNCSHeader {
	public:
		CNCSHeader();
		virtual ~CNCSHeader();
		
		bool Clear();
		string &ToString( string &str );
		bool Remove( const string &name );
		bool Set( const string &strHeader );
		bool Set( const string &name, const string &value );
		bool Set( const HeaderField &field );
		bool const Get( const string &name, string &value );
		bool const Get( const string &name, int &value );
		HeaderListIterator const Find( const string &name );
		HeaderListIterator const End(void) { return(m_header.end()); }
		
		int const Size();
		
	private:
		HeaderList m_header;
	};
	
	
	public:
		CNCSHeader	m_header;
		CNCSHeader	m_responseHeader;
		INT32		m_nLastStatus;
};


#endif // !defined(NCSREQUEST_H)
