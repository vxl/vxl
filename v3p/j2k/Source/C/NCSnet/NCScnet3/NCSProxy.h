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
** FILE:   	NCScnet3\NCSProxy.h
** CREATED:	12-4-2001
** AUTHOR: 	Russell Rogers
** PURPOSE:	
** EDITS:
** [01]
*******************************************************/

// NCSProxy.h: interface for the CNCSProxy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(NCSPROXY_H)
#define NCSPROXY_H

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // WIN32

#include "cnetdefs.h"
#include "NCSUtil.h"
//#include <streambuf>
#include <iostream>
#include <sstream>
#include <ctype.h>
#include <assert.h>
#include <string>
#include <vector>
#include <list>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Utilitiy functions
//////////////////////////////////////////////////////////////////////

/**
 * Convert an int into a String.
 */
string &IntToString( int i, string &str );
void StringToUpper( const string &src, string &dest );

//////////////////////////////////////////////////////////////////////

typedef vector<string> BypassList;
typedef BypassList::iterator BypassListIterator;

class CNCSProxy  
{
public:
	CNCSProxy(string sServerName);
	virtual ~CNCSProxy();
	
	string	m_sProxyName;
	INT32	m_nProxyPort;

	string	m_sUsername;
	string	m_sPassword;
	bool	m_bAuthenticate;

	//bool GetProxyAuthentication( string &sUsername, string &sPassword );
	bool GetProxyAuthentication( string &sProxyAuth );
	bool m_bPromptUser;

	bool	m_bUseProxy;
        
        static void SetGlobalProxy( char *szProxy, int nPort, char *szNoProxy );

private:
	bool GetIEProxyInfo(string &sProxyName, string &sBypassList);
	bool Find();
	bool InBypassList( string &sHost );

	bool GetNetscapeProxyInfo(string &sProxyName, string &sBypassList);
	char *GetCurrentUserName();
	char *GetProgramFilesDir();
	char *strdupunquote(char *pSrc);

	void DisplayProxyWarning();
	INT32 ExtractPort( string &sProxyName );

	bool CreateBypassList( string &sBypassList );

	BypassList m_lsBypassList;
        
        static bool sm_bUseGlobalProxy;
        static string sm_sProxyName;
        static int sm_nProxyPort;
        static string sm_sBypassList;
        static CNCSMutex sm_mProxy;

#ifdef MACOSX
        static string sm_sCredentials;
#endif
};

#endif // !defined(NCSPROXY_H)
