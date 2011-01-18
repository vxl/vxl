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
** FILE:   	NCScnet3\NCSProxy.cpp
** CREATED:	12-4-2001
** AUTHOR: 	Russell Rogers
** PURPOSE:	
** EDITS:
** [01]
*******************************************************/

// NCSProxy.cpp: implementation of the CNCSProxy class.
//
//////////////////////////////////////////////////////////////////////

#include "NCSProxy.h"

#ifdef WIN32
#include <Wininet.h>
#include <lmcons.h>		/* for UNLEN in GetCurrentUserName() */
#elif defined(POSIX)
#	ifdef MACOSX
#		include <ctype.h>
#	else
#		include <wctype.h>
#	endif
#endif // WIN32
#include "NCSBase64.h"
#include "NCSCrypto.h"

#ifdef LINUX
//#define __USE_XOPEN
#include <stdio.h> // for cuserid
#endif

/** Dialog to prompt for Username and Password.
 */
bool NCSGetPasswordDlg( const char *pszSite, const char *pszRealm, string &sUsername, string &sPassword, bool &bRememberAuth );

/** @class CNCSProxy
 *  @brief Handles proxy detection and use.
 *
 *  This class is used to detect if a proxy is being used (either
 *  by netscape or IE).  It also check which urls should bypass the
 *  proxy.
 *
 *  @author	Russell Rogers
 *  @date	10 April 2001
 *  @par Edits:
 *
 */

//////////////////////////////////////////////////////////////////////
// Utilitiy functions
//////////////////////////////////////////////////////////////////////

string &IntToString( int i, string &str ) {
	char tmpBuffer[256] = "";
	
	sprintf(tmpBuffer, "%d", i);

	str = tmpBuffer;

	return str;
}

void StringToUpper( const string &src, string &dest ) {
	dest = "";
	for( int i=0; i < src.size(); i++ ) {
		dest += toupper(src[i]);
	}
}

//////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////

int CNCSProxy::sm_nProxyPort = NCS_DEFAULT_HTTP_PORT;
bool CNCSProxy::sm_bUseGlobalProxy = false;
string CNCSProxy::sm_sProxyName;
string CNCSProxy::sm_sBypassList;
CNCSMutex CNCSProxy::sm_mProxy;

void CNCSProxy::SetGlobalProxy( char *szProxy, int nPort, char *szNoProxy )
{
    sm_mProxy.Lock();
    
    if( szProxy && szNoProxy && nPort ) {
        sm_nProxyPort = nPort;
        sm_sProxyName = szProxy;
        sm_sBypassList = szNoProxy;
        sm_bUseGlobalProxy = true;
    }
    
    sm_mProxy.UnLock();
}

#ifdef MACOSX
string CNCSProxy::sm_sCredentials;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSProxy::CNCSProxy( string sServerName )
{
	m_nProxyPort = NCS_DEFAULT_HTTP_PORT;
	m_sProxyName = "";
	m_bUseProxy = false;
	m_bPromptUser = false;

	m_sUsername = "";
	m_sPassword = "";
	m_bAuthenticate = false;
	        
	if( Find() ) {
		if( !InBypassList( sServerName ) ) {
			m_bUseProxy = true;
		}
	}
}

CNCSProxy::~CNCSProxy()
{

}

//////////////////////////////////////////////////////////////////////
// Member functions
//////////////////////////////////////////////////////////////////////

bool CNCSProxy::Find() {
	char chName[MAX_PATH];
	string sProxyName = "";
	string sBypassList = "";
        
        if( sm_bUseGlobalProxy ) {
            sm_mProxy.Lock();
            
            m_nProxyPort = sm_nProxyPort;
            m_sProxyName = sm_sProxyName;
            sBypassList = sm_sBypassList;
            
            sm_mProxy.UnLock();
            
            // create bypass list
            CreateBypassList( sBypassList );
            
            return true;
        }
#ifdef WIN32	
	else if(GetModuleFileNameA(NULL, chName, MAX_PATH) &&	_strlwr(chName) && strstr(chName, "netscape.exe") ) { // netscape
		
		if( !GetNetscapeProxyInfo(sProxyName, sBypassList) )
			return false;		
	} else if(GetModuleFileNameA(NULL, chName, MAX_PATH) &&	_strlwr(chName) &&  // direct
		(strstr(chName, "inetinfo.exe") || strstr(chName, "dllhost.exe")  || strstr(chName, "mtx.exe"))) {
		// Running inside IIS
		// use DIRECT!
		return false;
	} else { // ie
		if( !GetIEProxyInfo( sProxyName, sBypassList ) )
			return false;
	}
#elif defined( POSIX )
        else {
            return false;
        }
#endif

	m_nProxyPort = ExtractPort( sProxyName );
	m_sProxyName = sProxyName;
	DisplayProxyWarning();
	
	// create bypass list
	CreateBypassList( sBypassList );

	return true;
}

INT32 CNCSProxy::ExtractPort( string &sProxyName ) {
	string sPort;
	INT32 nPort=0;

	int nPos = sProxyName.rfind(":", sProxyName.size() );

	if( nPos != -1 ) {
		sPort = sProxyName.substr( nPos+1, sProxyName.size() );
		sProxyName.erase( nPos, sProxyName.size() );
		nPort = atoi( sPort.c_str() );
	} else {
		nPort = NCS_DEFAULT_HTTP_PORT;
	}
	
	return nPort;
}

bool CNCSProxy::CreateBypassList( string &sBypassList ) {

	// break into individual address using space or ',' as seperator
	int i=0;
	string sName="";
	string sUBypassList;

	// clear list
	m_lsBypassList.clear();

	StringToUpper( sBypassList, sUBypassList );

	for(i=0; i < sUBypassList.size(); i++) {
		if( isspace( sUBypassList[i] ) || sUBypassList[i] == ',' || sUBypassList[i] == ';' ) {
			// check not already in list
			if( (sName != "") && !InBypassList( sName ) ) {
				// add to list
				m_lsBypassList.push_back( sName );
			}
			sName = ""; //clear
		}
		else {
			sName += sUBypassList[i];
		}
	}

	// add final entry
	if( sName != "" ) {
		// check not already in list
		if( !InBypassList( sName ) ) {
			// add to list
			m_lsBypassList.push_back( sName );
		}
	}

	// add LOCALHOST
        string sTempHost = "127.0.0.1";
	if( !InBypassList( sTempHost ) ) m_lsBypassList.push_back( "127.0.0.1" );
        sTempHost = "LOCALHOST";
	if( !InBypassList( sTempHost ) ) m_lsBypassList.push_back( "LOCALHOST" );

	return true;
}

bool CNCSProxy::InBypassList( string &sHost ) {
	BypassListIterator iPos;
	string sUHost;
	int p0=0, n0=0;

	// convert sHost to upper case
	StringToUpper( sHost, sUHost );
	
	// search list for match.
	for( iPos = m_lsBypassList.begin(); iPos < m_lsBypassList.end(); iPos++ ) {
		// compare the end of the host name with the bypasslist host
		p0 = (sUHost.size() > iPos->size())?((sUHost.size()) - iPos->size()) : 0;
		n0 = sUHost.size();
#ifdef LINUX
//		if( !stricmp(sUHost.get( p0, n0, *iPos ), (*iPos).get()) == 0 ) return true;
		fprintf(stderr, "CNCSProxy::InBypassList() not yet implemented\n");
		return false;
#else
		if( sUHost.compare( p0, n0, *iPos ) == 0 ) return true;
#endif
	}

	return false;
}


void CNCSProxy::DisplayProxyWarning() {
/*
#if defined (MACINTOSH) || defined (POSIX)
	cout << "CNCSProxy::DisplayProxyWarning() NOT implemented for this platform.";
#else
	BOOLEAN bAlreadyShownMessage = FALSE;
	
	if(NCSPrefGetUserBoolean(NCS_PROXY_SHOW_MESSAGE_KEY, &bAlreadyShownMessage) != NCS_SUCCESS) {
		bAlreadyShownMessage = FALSE;
	}
	if(!bAlreadyShownMessage) {
		char buf[1024];
		sprintf(buf, "You are connecting to an Image Web Server through\n"
			"the HTTP Proxy server \"%s\".  Certain proxy servers\n"
			"may degrade performance.  See the Image Web Server web\n"
			"site at http://www.earthetc.com for more information on\n"
			"proxies.  You will not see this message again.", 
			m_sProxyName.c_str());
#ifdef WIN32
		MessageBox(NULL, buf, NCS_MESSAGE_BOX_TITLE, MB_OK|MB_ICONINFORMATION|MB_TASKMODAL);
#else
		fprintf(stderr, buf);
#endif // WIN32
		NCSPrefCreateUserKey(NCSPREF_DEFAULT_BASE_KEY);
		NCSPrefSetUserBoolean(NCS_PROXY_SHOW_MESSAGE_KEY, TRUE);
	}
#endif
*/
}

#define CreateKey()  char szKeyName[256] = "Proxy: "; \
					string sPort; \
					IntToString( m_nProxyPort, sPort ); \
					strcat(szKeyName, m_sProxyName.c_str()); \
					strcat(szKeyName, ":"); \
					strcat(szKeyName, sPort.c_str() )

bool CNCSProxy::GetProxyAuthentication( string &sProxyAuth ) {

#if defined( WIN32 ) || defined( MACOSX )
	string sUserName, sPassword;
        bool bRemeberAuth = true;
	char *szBase64Msg;

	if( m_bPromptUser == false ) {
#ifdef MACOSX
                sm_mProxy.Lock();
                if( sm_sCredentials.size() > 0 ) {
                    char szHeader[1024] = {'\0'};
                    sprintf(szHeader,"Proxy-Authorization: Basic %s",sm_sCredentials.c_str());
                    sProxyAuth = szHeader;
                }
                sm_mProxy.UnLock();
#else
		// create key
		CreateKey();
		if( NCSPrefGetUserString( szKeyName, &szBase64Msg ) == NCS_SUCCESS ) {
			char szHeader[1024] = {'\0'};
			sprintf(szHeader,"Proxy-Authorization: Basic %s",szBase64Msg);
			sProxyAuth = szHeader;
			//m_header.Set( szHeader );
			NCSFree( szBase64Msg );
		}
#endif
		m_bPromptUser = true;
		return true;
	}
	else {
		if( NCSGetPasswordDlg( m_sProxyName.c_str(), "Proxy Authentication", sUserName, sPassword, bRemeberAuth ) ) {
			// encrypt password
			if( (sUserName.size()!=0)&& (sPassword.size()!=0)) {
				char szBase64Msg[1024] = {'\0'};
				char szCredentials[1024] = {'\0'};
				char szHeader[1024] = {'\0'};
				
				strcat(szCredentials,sUserName.c_str());
				strcat(szCredentials,":");
				strcat(szCredentials,sPassword.c_str());
				
				NCSEncodeBase64Message(szCredentials,szBase64Msg);
				// add to request header
				sprintf(szHeader,"Proxy-Authorization: Basic %s",szBase64Msg);
				sProxyAuth = szHeader;
				//m_header.Set( szHeader );
#ifdef MACOSX
                                sm_mProxy.Lock();
                                sm_sCredentials = szBase64Msg;
                                sm_mProxy.UnLock();
#else
				NCSPrefCreateUserKey(NCSPREF_DEFAULT_BASE_KEY);			
				// create key
				CreateKey();
				NCSPrefSetUserString(szKeyName, szBase64Msg );
#endif
			}
			m_bPromptUser = false;
			return true;
		}
		else return false;
	}
#else
	return false;
#endif
}

#ifdef WIN32
bool CNCSProxy::GetIEProxyInfo(string &sProxyName, string &sBypassList) { // requires wininet
	
	bool bViaProxy=false;
	
	HINTERNET hInternet;
	INTERNET_PROXY_INFO *pProxy;
	DWORD nLen = sizeof(INTERNET_PROXY_INFO);
	char *pszUserAgent = NCSStrDup(NCS_HTTP_AGENT);
	
	hInternet =	InternetOpenA(pszUserAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(hInternet) {	
		
		InternetSetOption(hInternet, INTERNET_OPTION_SETTINGS_CHANGED, (void*)NULL, 0);
		
		InternetQueryOption(hInternet, INTERNET_OPTION_PROXY, (void*)NULL, &nLen);
		
		if(GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			if(pProxy = (INTERNET_PROXY_INFO *)NCSMalloc(nLen, TRUE)) {
				if(InternetQueryOption(hInternet, INTERNET_OPTION_PROXY, pProxy, &nLen)) {
					bViaProxy = (pProxy->dwAccessType == INTERNET_OPEN_TYPE_PROXY) ? TRUE : FALSE;
					
					if(bViaProxy) {
						sProxyName = CHAR_STRING(pProxy->lpszProxy);
						sBypassList = CHAR_STRING(pProxy->lpszProxyBypass);
						
						// get proxy username and password
						/*HINTERNET hConnection;
						HINTERNET hResource;

						string sServerTemp = sProxyName;
						int nProxyPort = ExtractPort( sServerTemp );

						hConnection = InternetConnect(hInternet,
							   sServerTemp.c_str(),
							   (INTERNET_PORT)nProxyPort,
							   NULL,
							   NULL,
							   INTERNET_SERVICE_HTTP,
							   0, 0);

						if(hConnection != NULL) {
							
							hResource = HttpOpenRequest(hConnection,
									     "POST",
										 "http://www.earthetc.com/",
										 "HTTP/1.1",
										 NULL,
										 0,
										 INTERNET_FLAG_KEEP_CONNECTION |
											INTERNET_FLAG_DONT_CACHE |
											INTERNET_FLAG_PRAGMA_NOCACHE | 
											INTERNET_FLAG_RELOAD, 0);

							DWORD ret = HttpSendRequest(hResource,
												NULL,
												NULL,
												NULL, 
												NULL);

							InternetErrorDlg (GetDesktopWindow(),
								  hResource, ERROR_INTERNET_INCORRECT_PASSWORD,
								  FLAGS_ERROR_UI_FILTER_FOR_ERRORS |
								  FLAGS_ERROR_UI_FLAGS_GENERATE_DATA |
								  FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS,
								  NULL);

							char szUsername[1024];
							char szPassword[1024];
							nLen = 1024;

							DWORD nErrorLength;

							nErrorLength = sizeof(szUsername);
							InternetQueryOption((HINTERNET)hResource, INTERNET_OPTION_USERNAME, (void*)szUsername, &nErrorLength);
							nErrorLength = sizeof(szPassword);
							InternetQueryOption((HINTERNET)hResource, INTERNET_OPTION_PASSWORD, (void*)szPassword, &nErrorLength);
							
							if(InternetQueryOption(hResource, INTERNET_OPTION_PROXY_USERNAME, szUsername, &nLen)) {
								m_bAuthenticate = true;
								//cout << szUsername << endl;
								m_sUsername = szUsername;
								nLen = 256;
								if(InternetQueryOption(hResource, INTERNET_OPTION_PROXY_PASSWORD, szPassword, &nLen)) {
									//cout << szPassword << endl;
									m_sPassword = szPassword;
								}
							}
							DWORD dwError = GetLastError();

							InternetCloseHandle(hConnection);

							HWND		hwnd=0;
							hwnd = GetActiveWindow();
							LPVOID lppvData[256];
							
							InternetErrorDlg( hwnd, hInternet, dwError, NULL, lppvData );
						}*/
					}
				} else {
					MessageBoxA(NULL, 
						"Could not query proxy information", 
						NCS_MESSAGE_BOX_TITLE, MB_OK|MB_ICONERROR|MB_TASKMODAL);
				}
				NCSFree(pProxy);
			}
		}
		InternetCloseHandle( hInternet );
	}
	NCSFree(pszUserAgent);
	
	return bViaProxy;

}
#endif // WIN32

/*
** Extract Netscape proxy settings by parsing current user's netscape prefs file
**
** WIN32
*/
bool CNCSProxy::GetNetscapeProxyInfo(string &sProxyName, string &sBypassList)
{
	char *szUser = GetCurrentUserName();
	char *szProgramFiles = GetProgramFilesDir();

	bool bViaProxy=false;

	char szName[MAX_PATH];
	FILE *fpPrefs;

	int nProxyType = 0;
	int nPort = 0;
	char *szHost = NULL;
	char *szAutoURL = NULL;
	char *szBypass = NULL;

	/* prefs file lives in "%program files%\netscape\user\%username%" even when netscape is installed elsewhere */
#ifdef WIN32
	if( _snprintf(szName, sizeof(szName), "%s\\Netscape\\Users\\%s\\prefs.js", szProgramFiles, szUser) > 0 &&
#else
	if( snprintf(szName, sizeof(szName), "%s\\Netscape\\Users\\%s\\prefs.js", szProgramFiles, szUser) > 0 &&
#endif
		(fpPrefs = fopen(szName, "r")) != NULL)
	{
		while (!feof(fpPrefs)) {
			char chLine[MAX_PATH];
			char chPref[30];
			char chValue[30];

			/* we're interested in the "network.proxy." prefs

			   when "direct connection to the internet" is selected, the pref "network.proxy.type" isn't present 
  		       type == 1 is "use manual proxy configuration"
			   type == 2 is "use automatic proxy configuration"
			*/
			if (fgets(chLine, MAX_PATH, fpPrefs) && 
				sscanf(chLine, "user_pref(\"network.proxy.%[a-z_]\", %[^)]", &chPref[0], &chValue[0]) == 2) 
			{
				if (strcmp(chPref, "http") == 0)
					szHost = strdupunquote(chValue);
				else if (strcmp(chPref, "http_port") == 0)
					nPort = atoi(chValue);
				else if (strcmp(chPref, "no_proxies_on") == 0)
					szBypass = strdupunquote(chValue);
				else if (strcmp(chPref, "autoconfig_url") == 0)
					szAutoURL = strdupunquote(chValue);
				else if (strcmp(chPref, "type") == 0)
					nProxyType = atoi(chValue);
			}
		}
		fclose(fpPrefs);
	} 
#ifdef _DEBUG
	else {
		char szError[MAX_PATH*2];
		sprintf(szError, "couldn't open netscape prefs file (%s)", szName);
		MessageBoxA(NULL, szError, NCS_MESSAGE_BOX_TITLE, MB_OK|MB_ICONERROR); /**[04]**/
	}
#endif

	//NCSFree(szProgramFiles);
	NCSFree(szUser);

	/* return and free the stuff we're not keeping (differs depending on proxy config) */

	if (nProxyType == 1) {
		bViaProxy = true;

		if(szHost && szHost[0] != '\0') {
			char pProxyName[1024];
			sprintf(pProxyName, "%s:%d", szHost, nPort ? nPort : 80);
			sProxyName = pProxyName;
		}

		if(szBypass && szBypass[0] != '\0') {
			sBypassList = szBypass;
			/*strcpy(pBypassList, szBypass);
			for(i = 0; i < strlen(pBypassList); i++) {
				if(pBypassList[i] == ',') {
					pBypassList[i] = ' ';
				}
			}*/
		}
	} else {
		bViaProxy = false;
		sProxyName = "";
		sBypassList = "";
	}

	NCSFree(szAutoURL);
	//NCSFree(szHost);
	NCSFree(szBypass);

	return bViaProxy;
}


char *CNCSProxy::GetCurrentUserName() {
#ifdef WIN32
	if(NCSGetPlatform() == NCS_WINDOWS_9X) {
		return(NCSStrDup("default"));
	} else {
		char szNameBuf[MAX_PATH];
		DWORD nSize = sizeof(szNameBuf); 
		HKEY hKey;

		if(RegOpenKeyExA(HKEY_CURRENT_USER,		/**[05]**/
						"Software\\Netscape\\Netscape Navigator\\biff",	// ProgramFilesDir
						 0,	
						 KEY_QUERY_VALUE,
						 &hKey) == ERROR_SUCCESS) {
			DWORD dwType;
			if (RegQueryValueExA(hKey, 
								"CurrentUser", 
								NULL, 
								&dwType, 
								(unsigned char *) szNameBuf, 
								&nSize) == ERROR_SUCCESS) {
				char *p = szNameBuf;

				while(*p != '\0') {
					if(*p == '.') {
						*p = '_';
					}
					p++;
				}
				RegCloseKey(hKey);
				return(NCSStrDup(szNameBuf));
			}
			RegCloseKey(hKey);
		}

		nSize = sizeof(szNameBuf);
		GetUserNameA(szNameBuf, &nSize);
		return(NCSStrDup(szNameBuf));
	}
#elif defined(POSIX)
#if defined( LINUX ) || defined( MACOSX )
	fprintf(stderr, "CNCSProxy::GetCurrentUserName() using loging instead of cuserid. FIXME\n");
	return(NCSStrDup(getlogin()));
#else
	char buf[L_cuserid];
	cuserid(buf);
	return(NCSStrDup(buf));
#endif
#elif defined(MACINTOSH)
	return(NCSStrDup("none"));
#else
#error "CNCSProxy::GetCurrentUserName not implemented for platform"
#endif
}

char *CNCSProxy::GetProgramFilesDir() {
#ifdef WIN32
	HKEY hKey;
	char chBuffer[MAX_PATH] = { '\0' };

	if(RegOpenKeyExA(HKEY_LOCAL_MACHINE,
					"Software\\Microsoft\\Windows\\CurrentVersion",	// ProgramFilesDir
					 0,	
					 KEY_QUERY_VALUE,
					 &hKey) == ERROR_SUCCESS) {
		DWORD nBufferSize = sizeof(chBuffer);
		DWORD dwType;
		if (RegQueryValueExA(hKey, 
							"ProgramFilesDir", 
							NULL, 
							&dwType, 
							(unsigned char *) chBuffer, 
							&nBufferSize) != ERROR_SUCCESS) {
#ifdef _DEBUG
				MessageBoxA(NULL, "Couldn't get program files dir", NCS_MESSAGE_BOX_TITLE, MB_OK|MB_ICONERROR|MB_TASKMODAL);
#endif
		}
		RegCloseKey(hKey);
	}

	return strdup(chBuffer);
#else
	return(NCSStrDup("/usr/bin"));
#endif // WIN32
}

/*
** returns an unquoted copy of the input string
** returns NULL if the input string is not quoted
** returns an empty string if passed "\"\""
*/
char *CNCSProxy::strdupunquote(char *pSrc) {
	char *pDest = NULL;

	int nLen = strlen(pSrc);
	if (pSrc[0] == '\"'  &&  pSrc[nLen - 1] == '\"') {
		if (nLen == 2) {
			pDest = (char *)NCSMalloc(1, TRUE);		// empty string
		} else if (nLen > 2) {
			pDest = (char *)NCSMalloc(nLen - 1, TRUE);
			if (pDest)
				strncpy(pDest, pSrc + 1, nLen - 2);
		}
	}
	return pDest;
}
