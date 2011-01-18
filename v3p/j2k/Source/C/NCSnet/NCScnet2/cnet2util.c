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
** FILE:   	NCScnet2\cnet2util.c
** CREATED:	
** AUTHOR: 	Simon Cope
** PURPOSE:	cnet utilities
** EDITS: 
** [01] 15Oct99 dmm - abstracted proxy settings to NCSnetProxyInfo structure 
					  added Netscape proxy determination
					  fixed handling of proxy strings with preceding protocol specifiers
** [02] 25Feb00 sjc HttpSendRequest with timeout to get around wininet.dll bugs
** [03] 02Mar00 sjc Polling GET support
** [04] 22Jun00  ny Windows2000 do not support MB_TASKMODAL, therefore, removed
** [05] 26Jun00 sjc Bugfix for W2K - username detection has changed.
** [06] 08mar01 jmp Added try catch block around pHttpSendRequest
** [07] 22Oct01 sjc Added exported funcs to access "our" wininet.dll from GDT lib.
** [08] 30Jan02 mjs When the client networking is used in a service (inetinfo)
**					load the newer WinHttp5 library instead of wininet. 
**					It fixes resource leaks encountered in defect 636 (and others).
** [09] 01DEC03 rar NCScnetCleanupGlobals now deletes the "MaxConnectionsPerServer" and 
**						"MaxConnectionsPer1_0Server" registry kesy if the "DeleteMaxConnValues"
**						key is set to 1.
** [10] 20Apr05 tfl Renamed file to cnet2util.c to resolve clash with NCSUtil/util.c
** [11] 05May05 tfl Modified declaration of mPollingServerMutex in line with changes to NCScnet.c
**						(as suggested by Manifold)
** [12] 13May05 tfl Fixed spurious code in NCSRunningInService
 *******************************************************/

#define _WIN32_WINNT 0x400
#include "cnet.h"
#include <lmcons.h>		/* for UNLEN in GetCurrentUserName() */
//#define ISSP_LEVEL 32
#define SECURITY_WIN32
#include <security.h>

#if !defined(_WIN32_WCE)
	#include "NCSWinHttp.h"
#else
	#include <Winsock.h>
#endif

#include "stdio.h" 
#include "windows.h" 
#ifndef _WIN32_WCE
#include "process.h"
#endif  
#include "resource.h" 

/*
** windows socket initialisation globals
*/
HINTERNET hInternet = NULL;
HINSTANCE hWininetDLL = NULL;
INT32 nClientCount = 0;

BOOL (CALLBACK *pHttpSendRequest)(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength);
BOOL (CALLBACK *pHttpAddRequestHeaders)(HINTERNET hConnect, LPCSTR lpszHeaders, DWORD dwHeadersLength, DWORD dwModifiers); /** ny **/
DWORD (CALLBACK *pInternetErrorDlg)(HWND hWnd, HINTERNET hRequest, DWORD dwError, DWORD dwFlags, LPVOID *lppvData);
BOOL (CALLBACK *pInternetSetOption)(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength);
HINTERNET (CALLBACK *pHttpOpenRequest)(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR FAR *lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext);
HINTERNET (CALLBACK *pInternetConnect)(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext);
BOOL (CALLBACK *pHttpQueryInfo)(HINTERNET hRequest, DWORD dwInfoLevel, LPVOID lpBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex);
BOOL (CALLBACK *pInternetCloseHandle)(HINTERNET hInternet);
BOOL (CALLBACK *pInternetWriteFile)(HINTERNET hFile, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite, LPDWORD lpdwNumberOfBytesWritten);
BOOL (CALLBACK *pInternetReadFile)(HINTERNET hFile, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead);
BOOL (CALLBACK *pInternetReadFileEx)(IN HINTERNET hFile, OUT LPINTERNET_BUFFERS lpBuffersOut, IN DWORD dwFlags, IN DWORD dwContext);
BOOL (CALLBACK *pInternetQueryOption)(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer, LPDWORD lpdwBufferLength);
HINTERNET (CALLBACK *pInternetOpen)(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags);

NCSMutex mSocketStartup;
NCSMutex mPollingServerMutex = NCS_NULL_MUTEX;	/**[03]**/ /**[11]**/
static char **ppPollingServerArray = (char**)NULL;		/**[03]**/
static INT32 nPollingServerArray = 0;					/**[03]**/

BOOLEAN NCSRunningInService() {
	char chName[MAX_PATH];
	return ((BOOLEAN)(NCSIsService()
			|| ((GetModuleFileNameA(NULL, chName, MAX_PATH) 
					&& _strlwr(chName) 
					&& (strstr(chName, "inetinfo.exe")
						|| strstr(chName, "dllhost.exe") 
						|| strstr(chName, "mtx.exe")
						|| strstr(chName, "w3wp.exe"))
				))));
}

/*
** global proxy settings
*/
static NCSnetProxyInfo ProxyInfo = { FALSE, "", 0, "", "", "", "" };

void NCScnetSetProxy( char *pProxy, INT32 nPort, char *pProxyBypass ) {
	ProxyInfo.bUseProxy = TRUE;
	ProxyInfo.nPort = (short)nPort;
	strncpy( ProxyInfo.szHost, pProxy, 1023 ); ProxyInfo.szHost[1023] = '\0';
	strncpy( ProxyInfo.szBypassList, pProxyBypass, 1023 ); ProxyInfo.szBypassList[1023] = '\0';
}

#ifdef NOTDEF
/*
** Takes an address in the form of "hostname:xx" and fills the sockaddr_in structure
**
** 'hostname' can be a name or an IP address
** ':xx' is optional and defaults to nDefaultPort if not present
**
** returns NCS_SUCCESS, or NCS_COULDNT_RESOLVE_HOST
*/
NCSError NCScnetMakeSockAddr(char *szHostAndPort, 
		  					 struct sockaddr_in *pAddr, 
							 short nDefaultPort,
							 BOOLEAN bPopupError)
{
	if(pAddr && szHostAndPort) {
		short nPort;
		char *pHost;
		char *pPort;
		BOOLEAN bRetVal = TRUE;

		pHost = NCSStrDup(szHostAndPort);
		pPort = pHost;

		if (pHost == NULL)
			return NCS_COULDNT_ALLOC_MEMORY;

		while (*pPort && *pPort != ':') {
			pPort++;
		}

		if (*pPort == ':') {
			*pPort = '\0';
			nPort = atoi(pPort + 1);
		} else {
			nPort = nDefaultPort;
		}

		pAddr->sin_family = AF_INET;
		pAddr->sin_port = htons(nPort);
		pAddr->sin_addr.S_un.S_addr = inet_addr(pHost);

		if (pAddr->sin_addr.S_un.S_addr == INADDR_NONE) {
			// failed to convert pHost, look it up instead
			struct hostent *pHostent = gethostbyname(pHost);

			if (pHostent && pHostent->h_addr_list  &&  *(pHostent->h_addr_list)) {
				memcpy(&pAddr->sin_addr.S_un.S_addr, *(pHostent->h_addr_list), 4);
			} else {
				if(bPopupError) {
					char msg[1024];
					sprintf(msg, "Could not get network address for %s", pHost);
					//MessageBox(NULL, msg, NCS_MESSAGE_BOX_TITLE, MB_OK|MB_ICONERROR|MB_TASKMODAL);
				}
				bRetVal = FALSE;
			}
		}
		NCSFree(pHost);
		return bRetVal ? NCS_SUCCESS : NCS_NET_COULDNT_RESOLVE_HOST;
	}
	return NCS_INVALID_ARGUMENTS;
}
#endif

static char *GetProgramFilesDir()
{
	HKEY hKey;
	char chBuffer[MAX_PATH] = { '\0' };

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
					OS_STRING("Software\\Microsoft\\Windows\\CurrentVersion"),	// ProgramFilesDir
					 0,	
					 KEY_QUERY_VALUE,
					 &hKey) == ERROR_SUCCESS) {
		DWORD nBufferSize = sizeof(chBuffer);
		DWORD dwType;
		if (RegQueryValueEx(hKey, 
							OS_STRING("ProgramFilesDir"), 
							NULL, 
							&dwType, 
							(unsigned char *) chBuffer, 
							&nBufferSize) != ERROR_SUCCESS) {
#ifdef _DEBUG
				MessageBox(NULL, NCS_T("Couldn't get program files dir"), OS_STRING(NCS_MESSAGE_BOX_TITLE), MB_OK|MB_ICONERROR|MB_TASKMODAL);
#endif
		}
		RegCloseKey(hKey);
	}

	return NCSStrDup(chBuffer);
}


static char *GetCurrentUserName()
{
#if !defined(_WIN32_WCE)
	if(NCSGetPlatform() == NCS_WINDOWS_9X) {
		return(NCSStrDup("default"));
	} else {
		NCSTChar szNameBuf[MAX_PATH];
		DWORD nSize = sizeof(szNameBuf); 
		HKEY hKey;

		if(RegOpenKeyEx(HKEY_CURRENT_USER,		/**[05]**/
						NCS_T("Software\\Netscape\\Netscape Navigator\\biff"),	// ProgramFilesDir
						 0,	
						 KEY_QUERY_VALUE,
						 &hKey) == ERROR_SUCCESS) {
			DWORD dwType;
			if (RegQueryValueEx(hKey, 
								NCS_T("CurrentUser"), 
								NULL, 
								&dwType, 
								(unsigned char *) szNameBuf, 
								&nSize) == ERROR_SUCCESS) {
				NCSTChar *p = szNameBuf;

				while(*p != NCS_T('\0')) {
					if(*p == NCS_T('.')) {
						*p = NCS_T('_');
					}
					p++;
				}
				RegCloseKey(hKey);
				return(NCSStrDup(CHAR_STRING(szNameBuf)));
			}
			RegCloseKey(hKey);
		}

		nSize = sizeof(szNameBuf);
		GetUserName(szNameBuf, &nSize);

		return(NCSStrDup(CHAR_STRING(szNameBuf)));
	}
#else
		return(NCSStrDup("win ce"));
#endif
}


/*
** returns an unquoted copy of the input string
** returns NULL if the input string is not quoted
** returns an empty string if passed "\"\""
*/
static char *strdupunquote(char *pSrc)
{
	char *pDest = NULL;

	int nLen = (int)strlen(pSrc);
	if (pSrc[0] == '\"'  &&  pSrc[nLen - 1] == '\"') {
		if (nLen == 2) {
			pDest = NCSMalloc(1, TRUE);		// empty string
		} else if (nLen > 2) {
			pDest = NCSMalloc(nLen - 1, TRUE);
			if (pDest)
				strncpy(pDest, pSrc + 1, nLen - 2);
		}
	}
	return pDest;
}


/*
** Extract Netscape proxy settings by parsing current user's netscape prefs file
**
** WIN32
*/
void NCScnetGetNetscapeProxyInfo(char *pProxyName, char *pBypassList)
{
	char *szUser = GetCurrentUserName();
	char *szProgramFiles = GetProgramFilesDir();

	char szName[MAX_PATH];
	FILE *fpPrefs;

	int nProxyType = 0;
	int nPort = 0;
	char *szHost = NULL;
	char *szAutoURL = NULL;
	char *szBypass = NULL;

	/* prefs file lives in "%program files%\netscape\user\%username%" even when netscape is installed elsewhere */
	if (_snprintf(szName, sizeof(szName), "%s\\Netscape\\Users\\%s\\prefs.js", szProgramFiles, szUser) > 0 &&
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
		MessageBox(NULL, OS_STRING(szError), OS_STRING(NCS_MESSAGE_BOX_TITLE), MB_OK|MB_ICONERROR); /**[04]**/
	}
#endif

	NCSFree(szProgramFiles);
	NCSFree(szUser);

	/* return and free the stuff we're not keeping (differs depending on proxy config) */

	if (nProxyType == 1) {
		UINT32 i;

		pProxyName[0] = '\0';
		if(szHost && szHost[0] != '\0') {
			sprintf(pProxyName, "http=http://%s:%d", szHost, nPort ? nPort : 80);
		}

		pBypassList[0] = '\0';
		if(szBypass && szBypass[0] != '\0') {
			strcpy(pBypassList, szBypass);
			for(i = 0; i < strlen(pBypassList); i++) {
				if(pBypassList[i] == ',') {
					pBypassList[i] = ' ';
				}
			}
		}
	} else {
		pProxyName[0] = '\0';
		pBypassList[0] = '\0';
	}

	NCSFree(szAutoURL);
	NCSFree(szHost);
	NCSFree(szBypass);
}

#ifdef NOTDEF
BOOLEAN NCScnetGetIEProxyInfo(NCSnetProxyInfo *pInfo)
{
	HKEY hKey;

	char *pProxyServer = NULL;
	char *pProxyOverride = NULL;
	char *pAutoConfigURL = NULL;

	BOOLEAN bRetVal = FALSE;

	/* dig proxy settings out of registry, this is good for IE4 and IE5 "lan settings" -
	   IE5 with multiple dial up configurations is untested */

	if(RegOpenKeyEx(HKEY_CURRENT_USER,
					"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
					 0,	
					 KEY_QUERY_VALUE,
					 &hKey) == ERROR_SUCCESS) {
		LPBYTE szValueBuffer[MAX_PATH];
		DWORD dwType;
		DWORD dwBufferSize;

		/* autoconfig overrides other proxy settings, check for it first: */

		dwBufferSize = sizeof(szValueBuffer);
		if (RegQueryValueEx(hKey, 
							"AutoConfigURL",
							NULL,
							&dwType,
							(unsigned char *) szValueBuffer,
							&dwBufferSize) == ERROR_SUCCESS) {
			if (dwType == REG_SZ) {
				pAutoConfigURL = NCSStrDup((char *)szValueBuffer);
				bRetVal = TRUE;
			}
		} else {
			dwBufferSize = sizeof(szValueBuffer);
			if (RegQueryValueEx(hKey, 
								   "ProxyEnable", 
								   NULL,
								   &dwType,
								   (unsigned char *) szValueBuffer,
								   &dwBufferSize) == ERROR_SUCCESS) {
				if(szValueBuffer[0]) {
					dwBufferSize = sizeof(szValueBuffer);
					if (RegQueryValueEx(hKey, 
										"ProxyServer", 
										NULL, 
										&dwType, 
										(unsigned char *) szValueBuffer, 
										&dwBufferSize) == ERROR_SUCCESS) {
						if (dwType == REG_SZ) {
							pProxyServer = NCSStrDup((char *)szValueBuffer);
							bRetVal = TRUE;

							dwBufferSize = sizeof(szValueBuffer);
							if (RegQueryValueEx(hKey,
												"ProxyOverride",
												NULL,
												&dwType,
												(unsigned char *) szValueBuffer,
												&dwBufferSize) == ERROR_SUCCESS) {
								if (dwType == REG_SZ) {
									pProxyOverride = NCSStrDup((char *)szValueBuffer);
								}
							}
						}
					}
				}
			}
		}

		// check autoconfig option
		RegCloseKey(hKey);
	} 

	if (pProxyServer)
	{
		/* value may look like "...;http=hostname:port;..."  (semi-colon separated) or just "hostname:port",
		   some people like to define their proxies with http:// in front of the hostname */
		char *pStart;
		char *pEnd;
		char *p;
		
		if (strchr(pProxyServer, '=')) {	// check to see if proxies specified individually
			if(pStart = strstr(pProxyServer, "http="))
				pStart += 5;
			else {
				pProxyServer[0] = '\0';		// no http proxy specified
				pStart = pProxyServer;
			}
		} else {
			pStart = (char *) pProxyServer;
		}
		
		/* search for end of string or ' ' or ';'*/
		pEnd = pStart;
		
		while (*pEnd  &&  *pEnd != ' '  &&  *pEnd != ';') {
			pEnd++;
		}
		*pEnd = '\0';

		/* strip out unneccessary preceding protocol specifier (like "http://") by searching for / */
		p = strrchr(pStart, '/');
		if (p)
			pStart = p + 1;

		/* now split up host:port into pInfo */
		if (strlen(pStart)) {
			pInfo->szHost = NCSMalloc(strlen(pStart) + 1, TRUE);
			if (pInfo->szHost) {
				sscanf(pStart, "%[^:]:%d", pInfo->szHost, &pInfo->nPort);
			}
		}
		pInfo->szBypassList = pProxyOverride;

		NCSFree(pProxyServer);
	} 
	
	pInfo->szAutoURL = pAutoConfigURL;
	
	return bRetVal;
}


void NCScnetProxyInit()
{
	BOOL bNetscape = FALSE;
	char chName[MAX_PATH];

	if (GetModuleFileNameA(NULL, chName, MAX_PATH)) {
		_strlwr(chName);
		bNetscape = (strstr(chName, "netscape.exe") != NULL);
	}

	memset(&ProxyInfo, 0, sizeof(ProxyInfo));

	if (bNetscape) {
		NCScnetGetNetscapeProxyInfo(&ProxyInfo);
	} else {
		NCScnetGetIEProxyInfo(&ProxyInfo);
	}

	if (ProxyInfo.szAutoURL && !FIsService()) {
		MessageBox(NULL, "The Image Web Server plugin is unable to determine your HTTP proxy settings because your browser is using Automatic Proxy Configuration.", NCS_MESSAGE_BOX_TITLE, MB_OK|MB_ICONINFORMATION|MB_TASKMODAL);
	}
}


void NCScnetProxyDeInit()
{
	if (ProxyInfo.szAutoURL)
		NCSFree(ProxyInfo.szAutoURL);
	if (ProxyInfo.szBypassList)
		NCSFree(ProxyInfo.szBypassList);
	if (ProxyInfo.szHost)
		NCSFree(ProxyInfo.szHost);
	if (ProxyInfo.szUsername)
		NCSFree(ProxyInfo.szUsername);
	if (ProxyInfo.szPassword)
		NCSFree(ProxyInfo.szPassword);
}


/*
** returns TRUE if the given hostname should bypass the proxy
**
** FIXME: currently only good for bypassing local addresses
** (Explorer can have wildcards on FQDNs, or on IP addresses...)
*/
BOOLEAN NCScnetProxyBypass(char *szHostname)
{
	BOOLEAN bResult = FALSE;
	char *szHost;

	if (!ProxyInfo.szBypassList)
		return FALSE;

	szHost = NCSStrDup(szHostname);
	if (!szHost)
		return FALSE;

	/* strip optional trailing port specifier from hostname */
	{
		char *p = szHost;
		while (*p && *p != ':')
			p++;
		*p = '\0';
	}

	if (strstr(ProxyInfo.szBypassList, "<local>") || strstr(ProxyInfo.szBypassList, "localhost")) {
		/* count the '.'s in the hostname to determine if it's local (this is how ie/wininet does it!) */
		int nDots = 0;
		char *p = szHost;
		while (*p) {
			if (*p == '.')
				nDots++;
			p++;
		}
		if (nDots == 0  ||  strcmp(szHost, "127.0.0.1") == 0  ||  stricmp(szHost, "localhost") == 0)
			bResult = TRUE;
	}
	NCSFree(szHost);
	return bResult;
}
#endif

#ifdef NOTDEF
/*
** Obtains the HTTP proxy server settings and fills out the sockaddr_in structure
**
** returns TRUE if successful
*/
BOOLEAN NCScnetGetProxySockAddr(struct sockaddr_in *pAddr,
								char *pProxyName,
								int nProxyNameLen)
{	
	BOOLEAN bRetVal = FALSE;

	if (ProxyInfo.szHost && ProxyInfo.nPort)
	{
		bRetVal = (NCScnetMakeSockAddr(ProxyInfo.szHost, pAddr, ProxyInfo.nPort, TRUE) == NCS_SUCCESS);
		if(bRetVal && pProxyName) {
			strncpy(pProxyName, ProxyInfo.szHost, nProxyNameLen - 1);
			pProxyName[nProxyNameLen - 1] = '\0';
		}
	} 

	return bRetVal;
}
#endif

/*
** Set cnet ID
*/
BOOLEAN NCScnetSetID(NCScnetConnection *pConn)
{
	static INT64 nBaseID = 0;
	static UINT32 nIDCount = 0;

	if(nBaseID == 0) {
		/* attempt to initialise base id (ie: top 32 bits of id) to client machine's IP address */
		char szHostname[80];
		HOSTENT *pHostent;

		if(gethostname(szHostname, 80) == 0  && (pHostent = gethostbyname(szHostname)) != NULL)	{
			/* go through all addresses for this machine and use one that isn't 127.0.0.1 */
			UINT32 *pList = (UINT32 *) pHostent->h_addr_list;

			while(pList  && (*pList == htonl(0x7f000001))) {
				pList++;
			}
			if(pList) {
				nBaseID = ((INT64) (*pList)) << 32;
			}
		}
	}

	if(nBaseID == 0) {
		return FALSE;
	} else {
		pConn->nID = (nBaseID & 0xffffffff00000000) | (NCSGetTimeStampMs() & 0xfff0) | nIDCount++;
		_RPT2(_CRT_WARN, "SetID: ID = 0x%x,%x\n", (UINT32) (pConn->nID >> 32), (UINT32) (pConn->nID & 0xffffffff));
		return TRUE;
	}
}

/*
** [03] NCScnetCheckIsPollingServer()
**
** Check if the server for this connection is in the polling server list
** if it is, set the flag in the connection structure.
*/
void NCScnetCheckIsPollingServer(NCScnetConnection *pConn)
{
	if(!pConn->bIsPollingConnection) {
		if (!NCSRunningInService() ) {
		INT32 i;

		NCSMutexBegin(&mPollingServerMutex);

		for(i = 0; i < nPollingServerArray; i++) {
			if(!strcmp(ppPollingServerArray[i], pConn->szServerName)) {
				pConn->bIsPollingConnection = TRUE;
			}
		}
		NCSMutexEnd(&mPollingServerMutex);
		}
	}
}

/*
** [03] NCScnetAddPollingServer()
**
** If the server fo rthis connection isn't in the polling server
** list, add it.
*/
void NCScnetAddPollingServer(NCScnetConnection *pConn)
{
	if(pConn->bIsPollingConnection) {
		INT32 i;

		NCSMutexBegin(&mPollingServerMutex);

		for(i = 0; i < nPollingServerArray; i++) {
			if(!strcmp(ppPollingServerArray[i], pConn->szServerName)) {
				break;
			}
		}
		if(i == nPollingServerArray) {
			char *pServer = NCSStrDup(pConn->szServerName);

			NCSArrayAppendElement(ppPollingServerArray, 
								  nPollingServerArray,
								  &pServer);
		}
		NCSMutexEnd(&mPollingServerMutex);
	}
}

void NCScnetCleanupGlobals(void)
{
	NCSMutexBegin(&mSocketStartup);

	if(hInternet != NULL) {
#ifdef _DEBUG
	//	MessageBox(NULL, "NCScnetCleanupGlobals(): InternetCloseHandle(hInternet);", NCS_MESSAGE_BOX_TITLE, MB_OK|MB_ICONINFORMATION|MB_TASKMODAL);
#endif
#if !defined(_WIN32_WCE)
		if (NCSWinHttpIsService()) {
			NCSWinHttpCloseHandle(hInternet);
		}
		else {
#endif
			pInternetCloseHandle(hInternet);
#if !defined(_WIN32_WCE)
		}
#endif
		hInternet = NULL;
	}

#if !defined(_WIN32_WCE)
	if (NCSWinHttpIsService()) {
		NCSWinHttpUnload();
		hWininetDLL = NULL;
	}
	else {
#endif
		if(hWininetDLL != NULL) {
			FreeLibrary(hWininetDLL);
			hWininetDLL = NULL;
		}
#if !defined(_WIN32_WCE)
	}
#endif
	NCSMutexBegin(&mPollingServerMutex);
	while(nPollingServerArray != 0) {
		NCSArrayRemoveElement(ppPollingServerArray, 
							  nPollingServerArray,
							  0);
	}
	NCSMutexEnd(&mPollingServerMutex);

	{ // start [09]
		BOOLEAN bDeleteMaxConnValues = FALSE;
		if( NCS_SUCCEEDED( NCSPrefSetUserKeyLock(NCSPREF_DEFAULT_BASE_KEY) ) ) {
			if( NCSPrefGetUserBoolean("DeleteMaxConnValues", &bDeleteMaxConnValues) == NCS_SUCCESS ) {
				NCSPrefUserUnLock();

				if( bDeleteMaxConnValues )
				{
					if( NCS_SUCCEEDED( NCSPrefSetUserKeyLock("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings") ) ) {
						NCSPrefDeleteValueUser("MaxConnectionsPerServer");
						NCSPrefDeleteValueUser("MaxConnectionsPer1_0Server");
						NCSPrefUserUnLock();
					}
				}
			} else {
				NCSPrefUserUnLock();
			}
		}
	} // end [09]


	NCSMutexEnd(&mSocketStartup);
#ifndef _WIN32_WCE
	//Clean up Certificate globals
	NCSMutexBegin( &g_mCertMutex );
	if(g_pCertContext) {
		CertFreeCertificateContext(g_pCertContext);
		g_pCertContext = NULL;
	}
	if( g_hCertStore ) {
		CertCloseStore( g_hCertStore, 0 );
		g_hCertStore = NULL;
	}
	NCSMutexEnd( &g_mCertMutex );
#endif
}

char szItemSelected[80]; // receives name of item to delete.
int nCount=0;
char **pCertificateList = NULL;
int nItem=0;
 
BOOL CALLBACK SelectItemProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
	HWND hwndList;
	lParam;//Keep compiler happy

    switch (message) 
    { 
        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
				case IDOK:
                    hwndList = GetDlgItem(hwndDlg, IDC_CERT_LIST); 
                    nItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);
 
                case IDCANCEL: 
                    EndDialog(hwndDlg, wParam); 
                    return TRUE; 
            }
			break;

		case WM_INITDIALOG:
		{
			int i=0;
			char *pNextValue = pCertificateList[0];
            hwndList = GetDlgItem(hwndDlg, IDC_CERT_LIST); 
 
            // Initialize the list box
 
            for (i = 0; (i < nCount) && pNextValue ; i++) 
            { 
                /*int nIndex = (int)*/SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) pNextValue); 
                //SendMessage(hwndList, LB_SETITEMDATA, i, (LPARAM) i);

				pNextValue = pCertificateList[i+1];
            }

			SendMessage(hwndList, LB_SETCURSEL, 0, 0);
            SetFocus(hwndList); 
            return FALSE; 
		}
    } 
    return FALSE; 
} 

#ifndef _WIN32_WCE
PCCERT_CONTEXT NCSCertificateChooser(HCERTSTORE hCertStore)
{
	HMODULE hModule = NULL;
	int nItemSelected = 0;
	int i=0;
	int j=0;

#define MAXLISTSIZE 20

	PCCERT_CONTEXT pContextList[MAXLISTSIZE+1]; //List has a maximum of MAXLISTSIZE certificates
	PCCERT_CONTEXT pContext = NULL;
	char pszNameString[256];
	pContextList[0] = NULL;

	//Get list of certificates
	pContext = CertEnumCertificatesInStore( hCertStore, NULL );
	/*pContext = CertFindCertificateInStore( hCertStore, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 
	  0, dwFindType, 
	  const void *pvFindPara, 
	  PCCERT_CONTEXT pPrevCertContext 
	);*/


	while( pContext && (i<MAXLISTSIZE)  )
	{
		pContextList[i] = CertDuplicateCertificateContext(pContext);
		pContext = CertEnumCertificatesInStore( hCertStore, pContext );
		i++;
	}
	if( i == MAXLISTSIZE ) pContextList[MAXLISTSIZE] = NULL;

	if( i < 2 ) {
		return pContextList[0];
	}
	pContext = NULL;
		
	pCertificateList = NCSMalloc( sizeof(char*)*(i+1), TRUE );
	if( pCertificateList ) {
		for( j=0; j < i; j++ ) {
			if( CertGetNameString(
			   pContextList[j],
			   CERT_NAME_SIMPLE_DISPLAY_TYPE,
			   0,
			   NULL,
			   OS_STRING(pszNameString),
			   128) )
			{
				pCertificateList[j] = NCSStrDup( pszNameString );
			} else {
				pCertificateList[j] = NCSStrDup( "" );
			}

		}
	}
	nCount = i;

#ifdef _DEBUG
	hModule = GetModuleHandle(OS_STRING("NCScnetd.dll"));
#else
	hModule = GetModuleHandle(OS_STRING("NCScnet.dll"));
#endif
    if (DialogBox(hModule, 
         MAKEINTRESOURCE(IDD_CERT_CHOOSER), 
         GetActiveWindow(), (DLGPROC)SelectItemProc)==IDOK) 
    {

        // Complete the command; szItemName contains the 
        // name of the item to delete. 
		nItemSelected = nItem;
		pContext = pContextList[nItemSelected];		
    }
    else 
    {
        // Cancel the command.
		nItemSelected = -1;
    }

	//Free the strings in memory
	//Free unused certificate contexts
	for( j=0; j < i; j++ ) {
		if( pCertificateList[j] ) {
			NCSFree( pCertificateList[j] );
			pCertificateList[j] = NULL;
		}
		if( (j != nItemSelected) && pContextList[j] )
		{
			CertFreeCertificateContext( pContextList[j] );
			pContextList[j] = NULL;
		}
	}
	NCSFree( pCertificateList );


	return pContext;
}
#endif

/*
** Thread function to do HttpSendRequest.
** pHttpSendRequest() will return if hRequest handle is closed.
*/
static void NCScnetHttpSendRequestProc(void *pData)	/**[02]**/
{
	NCScnetHttpSendRequestInfo *pInfo = (NCScnetHttpSendRequestInfo*)pData;
	static char msg[16384] = { '\0' };
	char extended_msg[16384] = { '\0' };
	char str[101];
	BOOLEAN bResult=FALSE;
	NCScnetHttpSendRequestResult RVal=NCS_HTTP_SEND_FAILED;

	if(pInfo && !pInfo->bCancel) {
		__try									/**[06]**/
		{
#if !defined(_WIN32_WCE)
			if (NCSWinHttpIsService()) {
				bResult = (BOOLEAN)NCSWinHttpSendRequest(pInfo->hRequest, CHAR_STRING(pInfo->lpszHeaders), pInfo->dwHeadersLength, pInfo->lpOptional, pInfo->dwOptionalLength, pInfo->dwOptionalLength, 0);

				if (!bResult) {
					RVal = NCS_HTTP_SEND_FAILED;
#ifdef NOTDEF
					LPVOID lpMsgBuf;
					FormatMessage( 
						FORMAT_MESSAGE_ALLOCATE_BUFFER | 
						FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						GetLastError(),
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						(LPTSTR) &lpMsgBuf,
						0,
						NULL 
						);
					NCSLog(LOG_LOW, "NCScnet : NCSWinHttpSendRequest() failed , %s", (LPCTSTR)lpMsgBuf);
					LocalFree( lpMsgBuf );
#endif //NOTDEF
				} else {
					RVal = NCS_HTTP_SEND_SUCCESS;
				}
			}
			else {
#endif
				if (pHttpSendRequest) {				/**[06]**/
					bResult = (BOOLEAN)pHttpSendRequest(pInfo->hRequest,
													CHAR_STRING(pInfo->lpszHeaders),
													pInfo->dwHeadersLength,
													pInfo->lpOptional, 
													pInfo->dwOptionalLength);
					if( !bResult ) { 
						//error
						DWORD dwError = GetLastError();
						if (dwError == ERROR_INTERNET_INVALID_CA)
						{
							RVal = NCS_HTTP_SEND_INVALID_CA;
						} else if( dwError == ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED )
						{
							RVal = NCS_HTTP_SEND_CLIENT_AUTH_CERT_NEEDED;
						} else if( dwError == ERROR_INTERNET_SECURITY_CHANNEL_ERROR ) {
							RVal = NCS_HTTP_SEND_FAILED;//NCS_HTTP_SEND_CLIENT_AUTH_CERT_NEEDED;
						} else {
							RVal = NCS_HTTP_SEND_FAILED;
						}

					} else {
						RVal = NCS_HTTP_SEND_SUCCESS;
					}
				}
#if !defined(_WIN32_WCE)
			}
#endif

			if( pInfo && !pInfo->bCancel) {
				pInfo->RVal = RVal;
				if( pInfo->RVal == NCS_HTTP_SEND_SUCCESS ) {
					DWORD dwError = 0;
					DWORD nErrorLength = sizeof(dwError);
					DWORD nFieldNr = 0;
			
#if !defined(_WIN32_WCE)
					if (NCSWinHttpIsService()) {
						if (!NCSWinHttpGetConnectionStatusCode(pInfo->hRequest, &dwError)) {
							//pInfo->bRVal = FALSE;
							pInfo->RVal = NCS_HTTP_SEND_FAILED;
						}
					}
					else {
#endif
						if(!pHttpQueryInfo(pInfo->hRequest,
										  HTTP_QUERY_STATUS_CODE |
										  HTTP_QUERY_FLAG_NUMBER,
										  &dwError,
										  &nErrorLength,
										  &nFieldNr)) {
							//pInfo->bRVal = FALSE;
							pInfo->RVal = NCS_HTTP_SEND_FAILED;
						}
#if !defined(_WIN32_WCE)
					}
#endif
				}
			}
			
			if( pInfo->event ) NCSEventSet( pInfo->event );
			

		} __except (NCSDbgGetExceptionInfoMsg(_exception_info(), msg)) {
			sprintf(str, "ECWP Exception (NCScnetHttpSendRequestProc)::%p", pHttpSendRequest);
			sprintf(extended_msg,
					"ECWP Exception : %s\n%s",
					NCS_VERSION_STRING,msg);
#ifdef NCS_BUILD_UNICODE
			MessageBox(NULL, OS_STRING(extended_msg), OS_STRING(str), MB_ICONERROR|MB_OK);
#else
			MessageBox(NULL, extended_msg, str, MB_ICONERROR|MB_OK|MB_SYSTEMMODAL|MB_SERVICE_NOTIFICATION);
#endif
		}
	}
}

NCScnetHttpSendRequestResult NCScnetHttpSendRequest(NCSThread *tThread,
													HINTERNET hRequest,/**[02]**/
													LPCTSTR lpszHeaders,
													DWORD dwHeadersLength,
													LPVOID lpOptional,
													DWORD dwOptionalLength,
													NCSTimeStampMs tsTimeOut)
{
	NCScnetHttpSendRequestResult Result = NCS_HTTP_SEND_FAILED;
	//NCSThread tThread;
	NCScnetHttpSendRequestInfo Info;

	char str[101];
	static char msg[16384] = { '\0' };

	Info.hRequest = hRequest;
	Info.lpszHeaders = lpszHeaders;
	Info.dwHeadersLength = dwHeadersLength;
	Info.lpOptional = lpOptional;
	Info.bCancel = FALSE;

	Info.dwOptionalLength = dwOptionalLength;
	Info.RVal = NCS_HTTP_SEND_FAILED;
	Info.event = NCSEventCreate();

	if(Info.event && (tsTimeOut != 0) && NCSThreadSpawn(tThread, NCScnetHttpSendRequestProc, &Info, FALSE)) {
		if( !NCSEventWait( Info.event, tsTimeOut ) ) {
			Info.bCancel = TRUE;

			// Timed out!
#if !defined(_WIN32_WCE)
			if (NCSWinHttpIsService()) {
				NCSWinHttpCloseHandle(hRequest);
			}
			else {
#endif
				pInternetCloseHandle(hRequest);
#if !defined(_WIN32_WCE)
			}
#endif
			
			Result = NCS_HTTP_SEND_TIMEOUT;

			// Wait for thread to exit
			while(NCSThreadIsRunning(tThread)) {
				NCSSleep(200);
			}
		} else {
			Result = Info.RVal;
		}
	} else {
		__try									/**[06]**/
		{
			NCScnetHttpSendRequestProc(&Info);
			Result = Info.RVal;

		} __except (NCSDbgGetExceptionInfoMsg(_exception_info(), msg)) {														
			char extended_msg[16384] = { '\0' };
			sprintf(str, "ECWP Exception::%p", pHttpSendRequest);
			sprintf(extended_msg,
				"(ECWP) Image Web Server Version : %s\n%s",
				NCS_VERSION_STRING,msg);
#ifdef NCS_BUILD_UNICODE
			MessageBox(NULL, OS_STRING(extended_msg), OS_STRING(str), MB_ICONERROR|MB_OK);
#else
			MessageBox(NULL, extended_msg, str, MB_ICONERROR|MB_OK|MB_SYSTEMMODAL|MB_SERVICE_NOTIFICATION);
#endif
		}

	}

	if(Info.event) {
		NCSEventDestroy(Info.event);
		Info.event = NULL;
	}
	return(Result);
}

#if !defined(NCSECW_STATIC_LIBS) && !defined(LIBECWJ2)
/*
** DLLMain() entry point
*/
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  dwReason, 
                       LPVOID lpReserved
					 )
{
	hModule;//Keep compiler happy
	lpReserved;//Keep compiler happy
    switch (dwReason) {
		case DLL_PROCESS_ATTACH:
				NCSMutexInit(&mSocketStartup);
				NCSMutexInit(&mPollingServerMutex);
				//Initiate the Certificate Globals
#ifndef _WIN32_WCE
				NCSMutexInit( &g_mCertMutex );
#endif
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
				NCScnetCleanupGlobals();
#ifndef _WIN32_WCE
				NCSMutexFini( &g_mCertMutex );
#endif
				NCSMutexFini(&mSocketStartup);
				NCSMutexFini(&mPollingServerMutex);
			break;
    }
    return(TRUE);
}

#endif // !NCSECW_STATIC_LIBS

/**[07] - begin **/

void NCSWinInetAddClient()
{
	NCSMutexBegin(&mSocketStartup);
	NCScnetInitWininet();
	nClientCount++;
	NCSMutexEnd(&mSocketStartup);
}

void NCSWinInetRemoveClient()
{
	NCSMutexBegin(&mSocketStartup);

	nClientCount--;
	if(nClientCount == 0) {
		NCSLog(LOG_HIGH, "NCScnetDestroy(): Cleaning up globals");
		NCScnetCleanupGlobals();
		NCSLog(LOG_HIGH, "NCScnetDestroy(): Finished cleaning up globals");
	}	
	NCSLog(LOG_HIGH, "NCScnetDestroy(): Before MutexEnd");
	NCSMutexEnd(&mSocketStartup);
}

BOOL NCSWinInetHttpSendRequest(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength)
{	
	if(pHttpSendRequest) {
		return((*pHttpSendRequest)(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength));
	} else {
		return(FALSE);
	}
}

BOOL NCSWinInetHttpAddRequestHeaders(HINTERNET hConnect, LPCSTR lpszHeaders, DWORD dwHeadersLength, DWORD dwModifiers)
{
	if(pHttpAddRequestHeaders) {
		return((*pHttpAddRequestHeaders)(hConnect, lpszHeaders, dwHeadersLength, dwModifiers));
	} else {
		return(FALSE);
	}
}

DWORD NCSWinInetInternetErrorDlg(HWND hWnd, HINTERNET hRequest, DWORD dwError, DWORD dwFlags, LPVOID *lppvData)
{
	if(pInternetErrorDlg) {
		return((*pInternetErrorDlg)(hWnd, hRequest, dwError, dwFlags, lppvData));
	} else {
		return(0);
	}
}

BOOL NCSWinInetInternetSetOption(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength)
{
	if(pInternetSetOption) {
		return((*pInternetSetOption)(hInternet, dwOption, lpBuffer, dwBufferLength));
	} else {
		return(FALSE);
	}
}

HINTERNET NCSWinInetHttpOpenRequest(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR FAR *lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext)
{
	if(pHttpOpenRequest) {
		return((*pHttpOpenRequest)(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext));
	} else {
		return((HINTERNET)NULL);
	}
}

HINTERNET NCSWinInetInternetConnect(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext)
{
	if(pInternetConnect) {
		return((*pInternetConnect)(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext));
	} else {
		return((HINTERNET)NULL);
	}
}

BOOL NCSWinInetHttpQueryInfo(HINTERNET hRequest, DWORD dwInfoLevel, LPVOID lpBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex)
{
	if(pHttpQueryInfo) {
		return((*pHttpQueryInfo)(hRequest, dwInfoLevel, lpBuffer, lpdwBufferLength, lpdwIndex));
	} else {
		return(FALSE);
	}
}

BOOL NCSWinInetInternetCloseHandle(HINTERNET hInternet)
{
	if(pInternetCloseHandle) {
		return((*pInternetCloseHandle)(hInternet));
	} else {
		return(FALSE);
	}
}

BOOL NCSWinInetInternetWriteFile(HINTERNET hFile, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite, LPDWORD lpdwNumberOfBytesWritten)
{
	if(pInternetWriteFile) {
		return((*pInternetWriteFile)(hFile, lpBuffer, dwNumberOfBytesToWrite, lpdwNumberOfBytesWritten));
	} else {
		return(FALSE);
	}
}

BOOL NCSWinInetInternetReadFile(HINTERNET hFile, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead)
{
	if(pInternetReadFile) {
		return((*pInternetReadFile)(hFile, lpBuffer, dwNumberOfBytesToRead, lpdwNumberOfBytesRead));
	} else {
		return(FALSE);
	}
}

BOOL NCSWinInetInternetReadFileEx(IN HINTERNET hFile, OUT LPINTERNET_BUFFERS lpBuffersOut, IN DWORD dwFlags, IN DWORD dwContext)
{
	if(pInternetReadFileEx) {
		return((*pInternetReadFileEx)(hFile, lpBuffersOut, dwFlags, dwContext));
	} else {
		return(FALSE);
	}
}

BOOL NCSWinInetInternetQueryOption(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer, LPDWORD lpdwBufferLength)
{
	if(pInternetQueryOption) {
		return((*pInternetQueryOption)(hInternet, dwOption, lpBuffer, lpdwBufferLength));
	} else {
		return(FALSE);
	}
}

HINTERNET NCSWinInetInternetOpen(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags)
{
	if(pInternetOpen) {
		HINTERNET hInternet = (HINTERNET)NULL;
		char chName[MAX_PATH];

		if(lpszProxy || lpszProxyBypass) {
			hInternet = (*pInternetOpen)(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
		} else if( ProxyInfo.bUseProxy ) { // if ProxyInfo has been set using NCScnetSetProxy use its values
			char szProxyName[MAX_PATH] = "";
			char szBypassList[MAX_PATH] = "";
			
			UINT32 i;
			szProxyName[0] = '\0';
			if(ProxyInfo.szHost[0] != '\0') {
				sprintf(szProxyName, "http=http://%s:%d", ProxyInfo.szHost, ProxyInfo.nPort ? ProxyInfo.nPort : 80);
			}

			szBypassList[0] = '\0';
			if(ProxyInfo.szBypassList[0] != '\0') {
				strcpy(szBypassList, ProxyInfo.szBypassList);
				for(i = 0; i < strlen(szBypassList); i++) {
					if(szBypassList[i] == ',') {
						szBypassList[i] = ' ';
					}
				}
			}
			if(strlen(szProxyName)) {
				hInternet =	pInternetOpen(lpszAgent,
									  INTERNET_OPEN_TYPE_PROXY,
									  szProxyName,
									  szBypassList,
									  0);
			} else {
				hInternet = pInternetOpen(lpszAgent,
										  INTERNET_OPEN_TYPE_DIRECT,
										  NULL,
										  NULL,
										  0);
			}
		} else if(GetModuleFileNameA(NULL, chName, MAX_PATH) &&	_strlwr(chName) && strstr(chName, "netscape.exe")) {
			char szProxyName[MAX_PATH] = "";
			char szBypassList[MAX_PATH] = "";

			NCSLog(LOG_HIGH, "NCScnetCreate(): Is netscape.exe");

			NCScnetGetNetscapeProxyInfo(szProxyName, szBypassList);

			if(strlen(szProxyName)) {
				hInternet =											/**[06]**/
						pInternetOpen(lpszAgent,					/**[06]**/
									  INTERNET_OPEN_TYPE_PROXY,		/**[06]**/
									  szProxyName,					/**[06]**/
									  szBypassList,					/**[06]**/
									  0);							/**[06]**/
							
				if(hInternet) {
					NCSLog(LOG_HIGH, "NCScnetCreate(): InternetOpen(%s, %s) SUCCESS", szProxyName, szBypassList);
				} else {
					NCSLog(LOG_LOW, "NCScnetCreate(): InternetOpen(%s, %s) FAILED", szProxyName, szBypassList);
				}
			} else {		/**[09] - go direct instead of falling through **/
				hInternet =											/**[06]**/
						pInternetOpen(lpszAgent,					/**[06]**/
									  INTERNET_OPEN_TYPE_DIRECT,	/**[06]**/
									  NULL,							/**[06]**/
									  NULL,							/**[06]**/
									  0);							/**[06]**/
				if(hInternet) {
					NCSLog(LOG_HIGH, "NCScnetCreate(): InternetOpen(DIRECT) SUCCESS");
				} else {
					NCSLog(LOG_LOW, "NCScnetCreate(): InternetOpen(DIRECT) FAILED");
				}
			}
		} else if(NCSIsService()/**[22]**/ || (GetModuleFileNameA(NULL, chName, MAX_PATH) &&	_strlwr(chName) && 
				  (strstr(chName, "inetinfo.exe") || strstr(chName, "dllhost.exe") || strstr(chName, "mtx.exe")))) {	/**[14]**/
			// Running inside IIS
			// use DIRECT!
			NCSLog(LOG_HIGH, "NCScnetCreate(): Is IIS");
			hInternet =												/**[14]**/
						pInternetOpen(lpszAgent,					/**[14]**/
									  INTERNET_OPEN_TYPE_DIRECT,	/**[14]**/
									  NULL,							/**[14]**/
									  NULL,							/**[14]**/
									  0);							/**[14]**/
			if(hInternet) {
				NCSLog(LOG_HIGH, "NCScnetCreate(): InternetOpen(DIRECT) SUCCESS");
			} else {
				NCSLog(LOG_LOW, "NCScnetCreate(): InternetOpen(DIRECT) FAILED");
			}
		} else {
			hInternet =												/**[06]**/
					pInternetOpen(lpszAgent,						/**[06]**/
								  INTERNET_OPEN_TYPE_PRECONFIG,		/**[06]**/
								  NULL,								/**[06]**/
								  NULL,								/**[06]**/
								  0);								/**[06]**/
			if(hInternet) {
				NCSLog(LOG_HIGH, "NCScnetCreate(): InternetOpen(AUTO) SUCCESS");
			} else {
				NCSLog(LOG_LOW, "NCScnetCreate(): InternetOpen(AUTO) FAILED");
			}
		}
		return(hInternet);
	} else {
		return((HINTERNET)NULL);
	}
}

/**[07 - end **/

typedef struct {
	HWND hWnd;
	char *lpText;
	char *lpCaption;
	UINT uType;
} NCSMessageBoxNoWaitData;

/** Function run the the thread spawned by MessageBoxNoWait().
 */
static void NCSMessageBoxNoWaitThread(void *pData) {

	NCSMessageBoxNoWaitData *info = (NCSMessageBoxNoWaitData*)pData;

	if( info ) {
		MessageBox(info->hWnd, OS_STRING(info->lpText), OS_STRING(info->lpCaption), info->uType);

		NCSFree( info->lpText );
		NCSFree( info->lpCaption );
		NCSFree( info );
	}

	NCSThreadExit(0);
}

/** Display a message box without making caller wait for response.
 * Spawns a new thread then calls MessageBox.
 *	HWND hWnd,          // handle to owner window
 *	LPCTSTR lpText,     // text in message box
 *	LPCTSTR lpCaption,  // message box title
 *	UINT uType          // message box style
 */
NCSError NCSMessageBoxNoWait( HWND hWnd, char *pText, char *pCaption, UINT uType ) {
	NCSThread tMessageBoxNoWaitThread; //[09]

	//Construct InitialTransfer info
	NCSMessageBoxNoWaitData *info = (NCSMessageBoxNoWaitData*)NCSMalloc( sizeof(NCSMessageBoxNoWaitData), TRUE );

	info->hWnd = hWnd;
	info->lpText = NCSStrDup( pText );
	info->lpCaption = NCSStrDup( pCaption );
	info->uType = uType;

	if( NCSThreadSpawn(&tMessageBoxNoWaitThread, NCSMessageBoxNoWaitThread, info, FALSE) ) {
		return NCS_SUCCESS;
	} else {
		return NCS_UNKNOWN_ERROR;
	}
}
