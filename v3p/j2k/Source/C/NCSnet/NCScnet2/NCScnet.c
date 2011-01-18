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
** FILE:   	NCScnet2\NCScnet.c
** CREATED:	
** AUTHOR: 	Simon Cope
** PURPOSE:	
** EDITS:
** [01] sjc 01Oct99 Added support for tracking protocol version
**					between client and server
** [02] sjc 24Feb00 Added support for polling GET connections
** [03] sjc 24Feb00 Fixed hang-on-close bug with mutex race condition
** [04] sjc 24Feb00 Give warning message if old wininet.dll
** [05] ny  07Feb00 Authentication
** [06] ny  04Apr00 Get User Agent from registry
** [07] ny  26Apr00 Check for zero length packet being received
**					and change to polling mode on the fly and retry
**					(retry only for initial connection)
** [08] ny  11May00 Added checks to see if the connection to the
**					server is still valid
** [09] sjc 15May00 Modified Netscape proxy config handling - don't Autoconfig with IE settings.
** [10]  ny 17May00 Overwrite return value from XXXX if user is IUSR (not cross-platform compatible)
** [11]  ny 01Aug00 Added code to terminate thread by force
**					to solve the problem of IE crashing
**					when it exits and there are still threads
**					running
** [12]  ny 06Sep00 Added code to set pConn->bIsConnected to TRUE, if not
**                  when in polling mode, it will report a lost of connection
** [13] sjc 18Sep00 Removed disconnect timer, do inline instead.
** [14] sjc 05Dec00 Skip proxy check if running inside IIS
** [15] jmp 15Jan01 Altered invalid wininet.dll message text for Manual Netscape Navigator plugin.
** [16] jmp 06Mar01 Added generic post url message, opens url and returns received buffer from fn
** [17] jmp 08Mar01 Added check to wait for NCScnetHttpSendRequestProc thread to finish.
** [18] rar 14May01 Client sends ping to server every 60secs (if no other sends done).  So server can
**					tell when a client dies.
** [19] rar 17-May-01 Modified NCScbmNetFileReceiveRequests() to include a returned error
**						code.  And cancel reading of ECW file on error.
** [20] sjc 18May01 Modified [10] to check process name instead, would fail if anon user wasn't IUSR_*
** [21] jmp 13Jun01 Packet points to shared memory, should be released within NCSEcw functions.
** [22] sjc 28Aug01 Use NCSIsService() to check if it's a service before hitting HKCU.\
** [23] sjc 12Nov01 Added versioncompare func to NCSUtil.lib
** [24] rar 15Jan02 Moved the wininet initialization code out of NCScnetCreateEx and into NCScnetInitWininet.
** [25] rar 18Feb02 Modified NCSDoInitialTransfer to timeout, by running in a new thread, if not completed
**						in time specified by NCS_INITIAL_TRANSFER_TIMEOUT.
** [26] rar 16Apr02 Reset the Connection ID if NCScnetDoInitialTransfer() times-out and the connection goes
**						into polling mode.
** [27] rar 18Jun02 Made the buffer size larger in NCScnetPostURL so that it is not reallocing too much
** [28] sjc 14Jul03 Removed proxy warning popups for polling mode & proxied connection.
** [29] tfl 01Apr05 Removed Purify references
** [30] tfl 05May05 Added changes suggested by Manifold to fix static build
** [31] tfl 02May06 Prevented restart message being raised more than once per load
 *******************************************************/
#define _WIN32_WINNT 0x400
#include "cnet.h"
#if !defined(_WIN32_WCE)
#include "winver.h"
#else
#include "winbase.h"
#endif
#include "NCSDefs.h"
#include "NCSTimer.h" /**[08]**/
#include <lmcons.h> /**[10]**/
#include <string.h> /**[10]**/

#if !defined(_WIN32_WCE)
	#include "NCSWinHttp.h"
#endif

static char *szDefaultIISDLLName =
#ifdef _DEBUG
	"/ecwp/ecwpd.dll";
#else
	"/ecwp/ecwp.dll";
#endif

static BOOLEAN bHaveRaisedRestartMessage = FALSE;

//
// Function specifically required for use by the IWS Proxy
//
void NCSUseWinHTTP( BOOL bValue )
{
#if !defined(_WIN32_WCE)
	bUseWinHTTP = bValue;
#endif
}

/*
** Forward protos
*/

static NCSError NCScnetDoInitialTransfer(NCScnetConnection *pConn, void *pPacketOut, INT32 nPacketOutLength, void **ppPacketIn, INT32 *pPacketInLength, INT32 tsTimeOut); //[25]
//static NCSError NCScnetDoInitialTransfer(NCScnetConnection *pConn, void *pPacketOut,	INT32 nPacketOutLength, void **ppPacketIn, INT32 *pPacketInLength);
static BOOLEAN NCScnetStartThreads(NCScnetConnection *pConn);
static void NCScnetStopThreads(NCScnetConnection *pConn);
static void NCScnetWaitForThreadsToExit(NCScnetConnection *pConn);

#ifdef _DEBUG
static void NCScnetSocketKillerThread(void *pDummy);
#endif

#define NCS_RECV_THREAD_TIMEOUT 30000	/**[08]**/

#define NCS_INITIAL_TRANSFER_TIMEOUT (NCS_CONNECTION_ATTEMP_TIMEOUT_MS) //[25]

BOOLEAN NCScnetVerifyURL(char *szURL)
{
	if (!szURL) return(FALSE);
	return (BOOLEAN)((strnicmp(szURL, "ecwp://", 7) == 0) || (strnicmp(szURL, "ecwps://", 8) == 0));
}

NCSError NCScnetInitWininet() { //[24]
	NCSError eError = NCS_SUCCESS;
	char *pszUserAgent;			/**[06]**/

	// If we are a service, use the winhttp dll, else use the wininet dll.
#if !defined(_WIN32_WCE)
	if (NCSWinHttpIsService()) {
		hWininetDLL = (HINTERNET)NCSWinHttpLoad(FALSE);
		
		if (!hWininetDLL) {
			return NCS_COULDNT_LOAD_WININET;
		}
		else {
			if (hInternet == NULL) {
				hInternet = NCSWinHttpOpen(NCS_HTTP_AGENT, 0, NULL, NULL, 0);
				if(hInternet) {
					INT32 nConns = NCS_MIN_NR_INET_CONNECTIONS;
					NCSWinHttpSetMaxConns(hInternet, nConns);
					return NCS_SUCCESS;
				}
				return NCS_WININET_FAILURE;
			}
		}
	}
	else {
#endif	//_WIN32_WCE
	if(hInternet == NULL) {
		char chName[MAX_PATH];
		INT32 nConns = NCS_MIN_NR_INET_CONNECTIONS;
		BOOLEAN bNeedDllCopy = TRUE;
		char *pDLLName = (char*)NULL;

		if( NCSPrefSetUserKeyLock("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings") == NCS_SUCCESS ) {
			if(NCSPrefGetUserString("IWS Client Network Wininet Dll", &pDLLName) != NCS_SUCCESS) {
				pDLLName = NULL;
			}
			NCSPrefUserUnLock();
		}
		if( !pDLLName ) {
#if defined(_WIN32_WCE)
			pDLLName = NCSStrDup(NCS_WININET_CE_DLL_NAME);
#else
			pDLLName = NCSStrDup(NCS_WININET_DLL_NAME);
#endif
		}

		if( (eError = NCSPrefSetUserKeyLock("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings")) == NCS_SUCCESS ) {
			// Delete the MaxConnectionsPerServer value that may have been previously set
			if(NCSPrefGetUserInt("MaxConnectionsPerServer", &nConns) == NCS_SUCCESS) {
				if(nConns == NCS_MIN_NR_INET_CONNECTIONS) {
					NCSPrefDeleteValueUser("MaxConnectionsPerServer");
					NCSPrefDeleteValueUser("MaxConnectionsPer1_0Server");
				}
			}

			NCSPrefUserUnLock();
		}

		if( (eError == NCS_SUCCESS) && ((eError = NCSPrefCreateUserKeyLock(NCSPREF_DEFAULT_BASE_KEY)) == NCS_SUCCESS ))
		{
			if(eError != NCS_SUCCESS) {													/**[20]**/
				// See if we're running in IIS.  If so, ignore the error.					/**[20]**/
				if(NCSIsService()/**[22]**/ || (GetModuleFileName(NULL, OS_STRING(chName), MAX_PATH) &&	_strlwr(chName) &&			/**[20]**/
					(strstr(chName, "inetinfo.exe") || strstr(chName, "dllhost.exe") || strstr(chName, "mtx.exe")))) {	/**[20]**/
					eError = NCS_SUCCESS;													/**[20]**/
				}																			/**[20]**/
			}																				/**[20]**/
			if(eError == NCS_SUCCESS) {
				if(hWininetDLL == NULL) {
					char *pBaseDir = (char*)NULL;
					char *pDestPath;

					if(NCSPrefGetString(NCS_CLIENTBASEDIR_REG_KEY, &pBaseDir) != NCS_SUCCESS) {
						if(NCSPrefGetUserString(NCS_CLIENTBASEDIR_REG_KEY, &pBaseDir) != NCS_SUCCESS) {
							NCSTChar szSysDir[MAX_PATH];

#if !defined(_WIN32_WCE)
							if(GetSystemDirectory(szSysDir, sizeof(szSysDir)) == 0) {
#endif
								NCSTCpy(szSysDir, NCS_T("C:"));
#if !defined(_WIN32_WCE)
							}
#endif
							szSysDir[2] = NCS_T('\0');
							NCSTCat(szSysDir, NCS_T("\\"));
							NCSTCat(szSysDir, OS_STRING(NCS_CLIENT_DIR_NAME));
							pBaseDir = NCSStrDup(CHAR_STRING(szSysDir));
						}
					}
					
					pDestPath = NCSMalloc((UINT32)strlen(pBaseDir) + (UINT32)strlen(pDLLName) + 2, FALSE);
					sprintf(pDestPath, "%s\\%s", pBaseDir, pDLLName);

					/*
					** Check the version number of wininet.dll
					** Complain if it's not 5.00.2614.3500 or
					** higher. [04]
					*/


					{
						UINT16 nMajor;
						UINT16 nMinor;
						UINT16 nRevision;
						UINT16 nBuild;

						if(NCSFileGetVersion(pDLLName,
											 &nMajor,
											 &nMinor,
											 &nRevision,
											 &nBuild)) {

							if(NCSVersionCompare(nMajor, nMinor, nRevision, nBuild, 5, 00, 2614, 3500) < 0) {
								char msg[1024];
								BOOLEAN bAlreadyShownMessage = FALSE;

								sprintf(msg, "The Internet Client Library (wininet.dll) installed on this computer\n"
											 "is version %ld.%ld.%ld.%ld.  The minimum supported version is\n"
											 "5.00.2614.3500 - you may encounter problems accessing imagery unless\n"
											 "a newer version is installed.  This can be obtained by installing\n"
											 "Internet Explorer v5 or newer on this computer.",
											 nMajor, nMinor, nRevision, nBuild);

								NCSLog(LOG_MED, "NCScnetCreate(): %s", msg);		

								if(NCSPrefGetUserBoolean(NCS_WININET_SHOW_MESSAGE_KEY, &bAlreadyShownMessage) != NCS_SUCCESS) {
									bAlreadyShownMessage = FALSE;
								}
								if(!bAlreadyShownMessage) {
#ifdef NCS_BUILD_UNICODE
									MessageBox(NULL, OS_STRING(msg), OS_STRING(NCS_MESSAGE_BOX_TITLE), MB_OK|MB_ICONINFORMATION);
#else
									MessageBox(NULL, msg, NCS_MESSAGE_BOX_TITLE, MB_OK|MB_ICONINFORMATION|MB_TASKMODAL|MB_SERVICE_NOTIFICATION);
#endif
									NCSPrefSetUserBoolean(NCS_WININET_SHOW_MESSAGE_KEY, TRUE);
								}	
							}
						}
					}

					if(GetModuleHandle(OS_STRING(pDLLName)) == NULL) {
						/*
						** WinInet.dll not loaded, don't need to copy dll
						*/
						bNeedDllCopy = FALSE;
					}
					if(bNeedDllCopy) {
#if !defined(_WIN32_WCE)
						NCSTChar szSysDllName[MAX_PATH];

						NCSLog(LOG_HIGH, "NCScnetCreate(): Need to copy wininet DLL");

						if(GetSystemDirectory(szSysDllName, MAX_PATH)) {
							NCSTCat(szSysDllName, NCS_T("\\"));
							NCSTCat(szSysDllName, OS_STRING(pDLLName));
	
							/*
							** Create Client directory if doesn't exist
							*/
							if(!CreateDirectory(OS_STRING(pBaseDir), (LPSECURITY_ATTRIBUTES)NULL)) {
								NCSLog(LOG_HIGH, "NCScnetCreate(): CreateDirectory(%s) FAILED", pBaseDir);
							}
							/*
							** Copy %SYSTEM%\wininet.dll to Client\wininet.dll
							*/
							if(!CopyFile(szSysDllName, OS_STRING(pDestPath), FALSE)) {
								NCSLog(LOG_HIGH, "NCScnetCreate(): CopyFile(%s, %s) FAILED", szSysDllName, pDestPath);
							}

							/*
							** Try loading Client\wininet.dll
							*/
							hWininetDLL = LoadLibrary(OS_STRING(pDestPath));
							if(!hWininetDLL) {
								NCSLog(LOG_HIGH, "NCScnetCreate(): LoadLibrary(%s) FAILED", pDestPath);
							}
						}
#else
#endif
					} else {
						/*
						** Don't need the copied wininet.dll anymore.
						*/
						if(!DeleteFile(OS_STRING(pDestPath))) {
							NCSLog(LOG_HIGH, "NCScnetCreate(): DeleteFile(%s) FAILED", pDestPath);
						}
						/*
						** Delete Client directory if empty.
						*/
						if(!RemoveDirectory(OS_STRING(pBaseDir))) {
							NCSLog(LOG_HIGH, "NCScnetCreate(): RemoveDirectory(%s) FAILED", pDestPath);
						}
					}
					if(!hWininetDLL) {
						/*
						** No wininet.dll loaded, load wininet.dll using standard path search
						*/

						hWininetDLL = LoadLibrary(OS_STRING(pDLLName));

						if(hWininetDLL && bNeedDllCopy) {
							/*
							** Copy/Load of Client\wininet.dll failed and we've fallen through to 
							** %SYSTEM%\wininet.dll, show rerun application message.
							*/
							if(!NCSIsService()/**[22]**/ && (GetModuleFileName(NULL, OS_STRING(chName), MAX_PATH) &&	_strlwr(chName) && //Don't display if running in IIS
								!(strstr(chName, "inetinfo.exe") || strstr(chName, "dllhost.exe") || strstr(chName, "mtx.exe"))) && !bHaveRaisedRestartMessage) {

#ifdef NCS_BUILD_UNICODE
								MessageBox(NULL, 
										   NCS_T("Your settings have been updated to support the ECWP protocol and\r\n")
										   NCS_T("you need to restart this application to use the new settings."),
										   OS_STRING(NCS_MESSAGE_BOX_TITLE), MB_OK|MB_ICONINFORMATION);
#else
								MessageBox(NULL, 
										   "Your settings have been updated to support the ECWP protocol and\r\n"
										   "you need to restart this application to use the new settings.",
										   NCS_MESSAGE_BOX_TITLE, MB_OK|MB_ICONINFORMATION|MB_TASKMODAL|MB_SERVICE_NOTIFICATION);
#endif
								bHaveRaisedRestartMessage = TRUE;
							}

						}
						if(!hWininetDLL) {
							NCSLog(LOG_HIGH, "NCScnetCreate(): LoadLibrary(%s) FAILED", pDLLName);
						}
					}
					NCSFree(pBaseDir);
					NCSFree(pDestPath);
				}
				if(hWininetDLL) {
					pHttpSendRequest = (BOOL(CALLBACK *)(HINTERNET, LPCSTR, DWORD, LPVOID, DWORD))
														GetProcAddress(hWininetDLL, "HttpSendRequestA");
					if (pHttpSendRequest == NULL)
						MessageBox(NULL, NCS_T("Unable to get proc address: pHttpSendRequest"), NCS_T("NCScnetCreateEx"), MB_OK);
					
					pHttpAddRequestHeaders = (BOOL(CALLBACK *)(HINTERNET, LPCSTR, DWORD, DWORD))
														GetProcAddress(hWininetDLL, "HttpAddRequestHeadersA"); /** [05] **/
					pInternetErrorDlg = (DWORD(CALLBACK *)(HWND, HINTERNET, DWORD, DWORD, LPVOID*))
														GetProcAddress(hWininetDLL, "InternetErrorDlg");
					pInternetSetOption = (BOOL(CALLBACK *)(HINTERNET, DWORD, LPVOID, DWORD))
														GetProcAddress(hWininetDLL, "InternetSetOptionA");
					pHttpOpenRequest = (HINTERNET(CALLBACK *)(HINTERNET, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPCSTR FAR *, DWORD, DWORD_PTR))
														GetProcAddress(hWininetDLL, "HttpOpenRequestA");
					pInternetConnect = (HINTERNET(CALLBACK *)(HINTERNET, LPCSTR, INTERNET_PORT, LPCSTR, LPCSTR, DWORD, DWORD, DWORD_PTR))
														GetProcAddress(hWininetDLL, "InternetConnectA");
					pHttpQueryInfo = (BOOL(CALLBACK *)(HINTERNET, DWORD, LPVOID, LPDWORD, LPDWORD))
														GetProcAddress(hWininetDLL, "HttpQueryInfoA");
					pInternetCloseHandle = (BOOL(CALLBACK *)(HINTERNET))
														GetProcAddress(hWininetDLL, "InternetCloseHandle");
					pInternetWriteFile = (BOOL(CALLBACK *)(HINTERNET, LPCVOID, DWORD, LPDWORD))
														GetProcAddress(hWininetDLL, "InternetWriteFile");
					pInternetReadFile = (BOOL(CALLBACK *)(HINTERNET, LPVOID, DWORD, LPDWORD))
														GetProcAddress(hWininetDLL, "InternetReadFile");
					pInternetReadFileEx = (BOOL(CALLBACK *)(HINTERNET, LPINTERNET_BUFFERS, DWORD, DWORD))
														GetProcAddress(hWininetDLL, "InternetReadFileExA");
					pInternetQueryOption = (BOOL(CALLBACK *)(HINTERNET, DWORD, LPVOID, LPDWORD))
														GetProcAddress(hWininetDLL, "InternetQueryOptionA");
					pInternetOpen = (HINTERNET(CALLBACK *)(LPCSTR, DWORD, LPCSTR, LPCSTR, DWORD))
														GetProcAddress(hWininetDLL, "InternetOpenA");

					if (NCSPrefGetUserString("IWS Client User Agent",&pszUserAgent)==NCS_SUCCESS) {	/**[06]**/
						NCSLog(LOG_LOW, "NCScnetCreate : Reading registry key [\"IWS Client User Agent\"] with %s",pszUserAgent);
					} else {
						pszUserAgent = NCSStrDup(NCS_HTTP_AGENT);
					}
					hInternet = NCSWinInetInternetOpen(pszUserAgent, 
													   INTERNET_OPEN_TYPE_PROXY,		/**[06]**/
													   NULL,					/**[06]**/
													   NULL,					/**[06]**/
													   0);

					NCSFree(pszUserAgent);							/**[06]**/

					if(hInternet) {
						nConns = NCS_MIN_NR_INET_CONNECTIONS;
#ifndef _WIN32_WCE
						pInternetSetOption(NULL,
										   INTERNET_OPTION_MAX_CONNS_PER_SERVER,
										   (void*)&nConns,
										   sizeof(nConns));
						pInternetSetOption(NULL,
										   INTERNET_OPTION_MAX_CONNS_PER_1_0_SERVER,
										   (void*)&nConns,
										   sizeof(nConns));
#endif // _WIN32_WCE
						pInternetSetOption(hInternet, 
										   INTERNET_OPTION_SETTINGS_CHANGED,
										   (void*)NULL, 0);
					} else {
						NCSLog(LOG_LOW, "NCScnetCreate(): InternetOpen() FAILED");
						eError = NCS_WININET_FAILURE;
					}
				} else {
					NCSLog(LOG_LOW, "NCScnetCreate(): Could not load wininet DLL");
					eError = NCS_COULDNT_LOAD_WININET;
				}
			}

			NCSPrefUserUnLock();
		}

		NCSFree(pDLLName);

		if(eError != NCS_SUCCESS) {
			NCScnetCleanupGlobals();
			NCSMutexEnd(&mSocketStartup);
			return(eError);
		}
	}

#if !defined(_WIN32_WCE)
	} // else if NCSWinHttpIsService
#endif	//_WIN32_WCE
	

	return NCS_SUCCESS;
}

NCSError NCScnetCreateEx(char *szURL, 
					     void **ppClient, 
					     void *pPacketIn, 
					     int nLengthIn, 
					     void **ppPacketOut, 
					     int *pLengthOut, 
					     NCSnetPacketRecvCB *pRecvCB, 
					     void *pRecvUserdata,
					     char *pszIISDLLName)
{
	return NCScnetCreateProxy(szURL, ppClient, pPacketIn, nLengthIn, ppPacketOut, pLengthOut, pRecvCB, 
					     pRecvUserdata, pszIISDLLName, NULL, NULL );
}

NCSError NCScnetCreateProxy(char *szURL, 
					     void **ppClient, 
					     void *pPacketIn, 
					     int nLengthIn, 
					     void **ppPacketOut, 
					     int *pLengthOut, 
					     NCSnetPacketRecvCB *pRecvCB, 
					     void *pRecvUserdata,
					     char *pszIISDLLName,
						 char *pszUsername,
						 char *pszPassword)
{
	NCSError eError = NCS_SUCCESS;
	NCScnetConnection *pConn;
	//char *pszUserAgent;			/**[06]**/
	BOOLEAN bSwitchToPollingModeRetry = FALSE;	/**[07]**/

#ifdef _DEBUG
	//MessageBox(NULL, "NCScnetCreate()", NCS_MESSAGE_BOX_TITLE, MB_OK|MB_ICONINFORMATION|MB_TASKMODAL|MB_SERVICE_NOTIFICATION);
#endif
	if(!szURL || !pPacketIn || !ppPacketOut || !pLengthOut || !ppClient)
		return NCS_INVALID_ARGUMENTS;

	/* startup winsock */
	NCSMutexBegin(&mSocketStartup);

	eError = NCScnetInitWininet(); //[24]
	if( eError != NCS_SUCCESS ) {
		NCSMutexEnd(&mSocketStartup);
		return eError;
	}

	pConn = NCSMalloc(sizeof(NCScnetConnection), TRUE);

	if(!pConn) {
		NCSMutexEnd(&mSocketStartup);
		NCSLog(LOG_LOW, "NCScnetCreate(): Could not allocate %ld bytes", sizeof(NCScnetConnection));
		return(NCS_COULDNT_ALLOC_MEMORY);
	} else {
		char *pSrc = NULL;
		char *pDest = pConn->szServerName;
		int nCount = 0;

		pConn->pCertContext = NULL;
		pConn->bCertSuccess = FALSE;
		pConn->bNoKeepAlives = TRUE;

		pConn->bUseSSL = FALSE; // Don't use SSL by default

		if(strnicmp(szURL, "ecwps://", 8) == 0) {
			pSrc = szURL + 8;				/* skip "ecwps://" */
			pConn->bUseSSL = TRUE;
		} else {
			pSrc = szURL + 7;				/* skip "ecwp://" */
		}

		nClientCount++;
		NCSMutexEnd(&mSocketStartup);

		while (*pSrc  &&  (*pSrc != '/')  &&  (nCount < MAX_PATH-2)) {
			*pDest = *pSrc;
			nCount++;
			pDest++;
			pSrc++;
		}
		*pDest = '\0';

		pDest = strstr(pConn->szServerName, ":");

		if(pDest) {
			*pDest = '\0';
			pDest++;
			pConn->nServerPort = atoi(pDest);
		}
		if(pConn->nServerPort == 0) {
			if( pConn->bUseSSL ) {
				pConn->nServerPort = INTERNET_DEFAULT_HTTPS_PORT;
			} else {
				pConn->nServerPort = NCS_DEFAULT_HTTP_PORT;
			}
		}

		NCSMutexInit(&(pConn->mSendMutex));

		pConn->pRecvCB = pRecvCB;
		pConn->pRecvCBData = pRecvUserdata;
		pConn->pszIISDLLName = NCSStrDup(pszIISDLLName);
		pConn->bIsConnected = FALSE; //initally false, set to true once the connection has been established

		if( pszUsername != NULL && pszPassword != NULL ) {
			pConn->szUserName = NCSStrDup( pszUsername );
			pConn->szPassword = NCSStrDup( pszPassword );
		} else {
			pConn->szUserName = NULL;
			pConn->szPassword = NULL;
		}

		{
			INTERNET_PROXY_INFO *pProxy;
			DWORD nLen = sizeof(INTERNET_PROXY_INFO);

#if !defined(_WIN32_WCE)
			if (NCSWinHttpIsService()) {
				pConn->bViaProxy = (BOOLEAN)NCSWinHttpIsConnectionViaProxy(hInternet);
			}
			else {
#endif
				pInternetQueryOption(hInternet, INTERNET_OPTION_PROXY, (void*)NULL, &nLen);
				
				if(GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
					if((pProxy = NCSMalloc(nLen, TRUE)) != NULL) {
						if(pInternetQueryOption(hInternet, INTERNET_OPTION_PROXY, pProxy, &nLen)) {
							pConn->bViaProxy = (pProxy->dwAccessType == INTERNET_OPEN_TYPE_PROXY) ? TRUE : FALSE;
							
							if(pConn->bViaProxy) {
#ifdef NOTUSED	//[28]
								BOOLEAN bAlreadyShownMessage = FALSE;
								
								if(NCSPrefGetUserBoolean(NCS_PROXY_SHOW_MESSAGE_KEY, &bAlreadyShownMessage) != NCS_SUCCESS) {
									bAlreadyShownMessage = FALSE;
								}
								if(!bAlreadyShownMessage ) {
									char buf[1024];
									sprintf(buf, "You are connecting to the Image Web Server \"%s\" through\n"
										"the HTTP Proxy server \"%s\".  Certain proxy servers may degrade\n"
										"performance.  See the Image Web Server web site at http://www.earthetc.com\n"
										"for more information on proxies.  You will not see this message again.", 
										pConn->szServerName, pProxy->lpszProxy);
#if !defined(_WIN32_WCE)
									MessageBox(NULL, buf, NCS_MESSAGE_BOX_TITLE, MB_OK|MB_ICONINFORMATION|MB_TASKMODAL|MB_SERVICE_NOTIFICATION);
									NCSPrefSetUserBoolean(NCS_PROXY_SHOW_MESSAGE_KEY, TRUE);
#else
									MessageBox(NULL, OS_STRING(buf), OS_STRING(NCS_MESSAGE_BOX_TITLE), MB_OK|MB_ICONINFORMATION);
									NCSPrefSetUserBoolean(NCS_PROXY_SHOW_MESSAGE_KEY, TRUE);
#endif
								}
#endif // NOTUSED //[28]
								NCSLog(LOG_HIGH, "NCScnetCreate(): Connected via proxy %s (bypass %s) to server %s", (char*)pProxy->lpszProxy, pProxy->lpszProxyBypass ? (char*)pProxy->lpszProxyBypass : "", pConn->szServerName);							}
						} else {
							NCSLog(LOG_LOW, "NCScnetCreate(): Could not query proxy information");
#ifdef NCS_BUILD_UNICODE
							MessageBox(NULL, NCS_T("Could not query proxy information"), OS_STRING(NCS_MESSAGE_BOX_TITLE), MB_OK|MB_ICONERROR);
#else
							MessageBox(NULL, "Could not query proxy information", NCS_MESSAGE_BOX_TITLE, MB_OK|MB_ICONERROR|MB_TASKMODAL|MB_SERVICE_NOTIFICATION);
							
#endif
						}
						NCSFree(pProxy);
					}
				}
#if !defined(_WIN32_WCE)
			} // if NCSWinHttpIsService()
#endif
		}

		NCScnetSetID(pConn);

		NCSLog(LOG_HIGH, "NCScnetCreate(): Attempting to connect down link (GET)");

		eError = NCScnetConnectToServer(pConn, FALSE, (void*)NULL, 0);
		if(eError == NCS_SUCCESS) {
			NCSLog(LOG_HIGH, "NCScnetCreate(): Connected down link (GET)");
			NCSLog(LOG_HIGH, "NCScnetCreate(): Attempting initial transfer");

			eError = NCScnetDoInitialTransfer(pConn,
											  pPacketIn,
											  nLengthIn,
											  ppPacketOut,
											  pLengthOut,
											  NCS_INITIAL_TRANSFER_TIMEOUT ); //[25]
			if (eError == NCS_SUCCESS) {

				pConn->bIsConnected = TRUE;	/**[08]**/

				NCSLog(LOG_HIGH, "NCScnetCreate(): Initial transfer SUCCESS");
				NCSLog(LOG_HIGH, "NCScnetCreate(): Attempting to start threads");

				if(NCScnetStartThreads(pConn)) {
					NCSLog(LOG_HIGH, "NCScnetCreate(): Thread start SUCCESS");
					*ppClient = pConn;
					return NCS_SUCCESS;
				} else {
					NCSLog(LOG_LOW, "NCScnetCreate(): Thread start failed");
				}
			} else if (eError == NCS_NET_PACKET_RECV_ZERO_LENGTH) {		/**[07]**/
				/*
				** Make sure we are not already in polling mode. No point
				** retry if we are already in polling mode. Issue an error
				** instead.
				*/

				if (!pConn->bIsPollingConnection && !NCSRunningInService() ) {						/**[07]**/
					pConn->bIsPollingConnection = TRUE;					/**[07]**/
					bSwitchToPollingModeRetry = TRUE;					/**[07]**/
				} else {												/**[07]**/
					eError = NCS_NET_PACKET_RECV_FAILURE;				/**[07]**/
				}														/**[07]**/
			} else {
				NCSLog(LOG_LOW, "NCScnetCreate(): Initial transfer failed");
			}
			NCScnetDisconnectFromServer(pConn);
		} else {
			NCSLog(LOG_LOW, "NCScnetCreate(): Down link connection failed (GET)");
		}
		/*
		** We need to re-connect to the server with pConn->bIsPollingConnection == TRUE
		** Because we are only attempting to do this just ONCE, I am duplicating the
		** above code. Should this design/policy decision change in future, we should
		** probably add a MAX_SWITCH_TO_POLLING_MODE_RETRIES and loop through that.
		**
		*/
		if (bSwitchToPollingModeRetry) {												/**[07]**/
			//Reset the pConn ID so that it does not use a connection on the server that
			//has already been created by the previous call to NCScnetDoInitialTransfer()
			NCScnetSetID(pConn); //[26]

			eError = NCScnetConnectToServer(pConn, FALSE, (void*)NULL, 0);				/**[07]**/
			if(eError == NCS_SUCCESS) {													/**[07]**/
				NCSLog(LOG_HIGH, "NCScnetCreate(): Connected down link (GET)");			/**[07]**/
				NCSLog(LOG_HIGH, "NCScnetCreate(): Attempting initial transfer");		/**[07]**/
				eError = NCScnetDoInitialTransfer(pConn,								/**[07]**/
												pPacketIn,								/**[07]**/
												nLengthIn,								/**[07]**/
												ppPacketOut,							/**[07]**/
												pLengthOut,								/**[07]**/
												NCS_INITIAL_TRANSFER_TIMEOUT*2); //[25]
				if (eError == NCS_SUCCESS) {											/**[07]**/
					NCSLog(LOG_HIGH, "NCScnetCreate(): Initial transfer SUCCESS");		/**[07]**/
					NCSLog(LOG_HIGH, "NCScnetCreate(): Attempting to start threads");	/**[07]**/
					pConn->bIsConnected = TRUE;											/**[12]**/
					if(NCScnetStartThreads(pConn)) {									/**[07]**/
						NCSLog(LOG_HIGH, "NCScnetCreate(): Thread start SUCCESS");		/**[07]**/
						*ppClient = pConn;												/**[07]**/
						return NCS_SUCCESS;												/**[07]**/
					} else {															/**[07]**/
						NCSLog(LOG_LOW, "NCScnetCreate(): Thread start failed");		/**[07]**/
					}																	/**[07]**/
				} else {																/**[07]**/
					NCSLog(LOG_LOW, "NCScnetCreate(): Initial transfer failed");		/**[07]**/
				}																		/**[07]**/
				NCScnetDisconnectFromServer(pConn);										/**[07]**/
			} else {																	/**[07]**/
				NCSLog(LOG_LOW, "NCScnetCreate(): Down link connection failed (GET)");	/**[07]**/
			}																			/**[07]**/
		}																				/**[07]**/
		NCScnetDestroy(pConn);
		NCSLog(LOG_HIGH, "NCScnetCreate(): After NCScnetDestroy");
		*ppClient = NULL;
		//NCSLog(LOG_HIGH, "NCScnetCreate(): After assign NULL");
		//NCSMutexBegin(&mSocketStartup);
		//NCSLog(LOG_HIGH, "NCScnetCreate(): After NCSMutexBegin");
		//nClientCount--;
		//NCSLog(LOG_HIGH, "NCScnetCreate(): After decrement");
		//NCSMutexEnd(&mSocketStartup);
		//NCSLog(LOG_HIGH, "NCScnetCreate(): After Mutex");
		return eError;
	}
}

NCSError NCScnetCreate(char *szURL, 
					   void **ppClient, 
					   void *pPacketIn, 
					   int nLengthIn, 
					   void **ppPacketOut, 
					   int *pLengthOut, 
					   NCSnetPacketRecvCB *pRecvCB, 
					   void *pRecvUserdata)
{
	char *szIISDLLName =
#ifdef _DEBUG
	"/ecwp/ecwp.dll";
#else
	"/ecwp/ecwp.dll";
#endif

	return(NCScnetCreateEx(szURL, 
						   ppClient, 
						   pPacketIn, 
						   nLengthIn, 
						   ppPacketOut, 
						   pLengthOut, 
						   pRecvCB, 
						   pRecvUserdata,
						   szIISDLLName));
}

void NCScnetDestroy(void *pConnection)
{
	NCScnetConnection *pConn = (NCScnetConnection*)pConnection;
	if(pConn) {
		NCScnetStopThreads(pConn);
		NCSLog(LOG_HIGH, "NCScnetDestroy(): Disconnecting from server");
		NCScnetDisconnectFromServer(pConn);
		NCSLog(LOG_HIGH, "NCScnetDestroy(): Stopping threads");
		NCScnetStopThreads(pConn);
		NCSLog(LOG_HIGH, "NCScnetDestroy(): Waiting for thread exit");
		NCScnetWaitForThreadsToExit(pConn);

		NCSLog(LOG_HIGH, "NCScnetDestroy(): Destroying connection mutex");
		NCSFree(pConn->pszIISDLLName);

		NCSFree(pConn->szUserName);
		NCSFree(pConn->szPassword);

		if (pConn->pCertContext) {
#ifndef _WIN32_WCE
			CertFreeCertificateContext(pConn->pCertContext);
#endif
			pConn->pCertContext = NULL;
		}

		NCSMutexFini(&(pConn->mSendMutex));
		NCSFree(pConn);

		NCSMutexBegin(&mSocketStartup);
		nClientCount--;
		if(nClientCount == 0) {
			NCSLog(LOG_HIGH, "NCScnetDestroy(): Cleaning up globals");
			NCScnetCleanupGlobals();
			NCSLog(LOG_HIGH, "NCScnetDestroy(): Finished cleaning up globals");
		}	
		NCSLog(LOG_HIGH, "NCScnetDestroy(): Before MutexEnd");
		NCSMutexEnd(&mSocketStartup);
		NCSLog(LOG_HIGH, "NCScnetDestroy(): After MutexEnd");
	}
}


void NCScnetSetRecvCB(void *pConnection, NCSnetPacketRecvCB *pRecvCB, void *pUserdata)
{
	NCScnetConnection *pConn = pConnection;

	NCSLog(LOG_HIGH, "NCScnetSetRecvCB()");

	if(pConn) {
		pConn->pRecvCB = pRecvCB;
		pConn->pRecvCBData = pUserdata;
	}
}

void NCScnetSetSentCB(void *pConnection, NCSnetPacketSentCB *pSentCB)
{
	NCScnetConnection *pConn = pConnection;

	NCSLog(LOG_HIGH, "NCScnetSetSentCB()");

	if(pConn) {
		pConn->pSentCB = pSentCB;
	}
}

BOOLEAN NCScnetSendPacket(void *pConnection, void *pPacket, int nLength, void *pUserdata, BOOLEAN *bIsConnected)/**[08]**/
{
	NCScnetConnection *pConn = pConnection;

	NCSLog(LOG_HIGH, "NCScnetSendPacket(0x%lx, 0x%lx, %ld, 0x%lx)", pConnection, pPacket, nLength, pUserdata);

	if(pConn) {
		NCSMutexBegin(&(pConn->mSendMutex));

		if(pConn->pSendPacket) {
			NCSLog(LOG_LOW, "NCScnetSendPacket(0x%lx, 0x%lx, %ld, 0x%lx): Packet already loaded for send", pConnection, pPacket, nLength, pUserdata);
			NCSMutexEnd(&(pConn->mSendMutex));
			return(FALSE);
		}
		pConn->pSendPacket = pPacket;
		pConn->nSendPacketLength = nLength;
		pConn->pSendCBData = pUserdata;

		NCSLog(LOG_HIGH, "NCScnetSendPacket(0x%lx, 0x%lx, %ld, 0x%lx): Resuming send thread", pConnection, pPacket, nLength, pUserdata);

		//NCSThreadResume(&(pConn->tSendThread));
		pConn->bSendResume = TRUE;				/**[18]**/

		NCSMutexEnd(&(pConn->mSendMutex));
		*bIsConnected = pConn->bIsConnected;	/**[08]**/
		if (!pConn->bIsConnected)				/**[08]**/
			return(FALSE);						/**[08]**/
		return(TRUE);
	}
	NCSLog(LOG_LOW, "NCScnetSendPacket(0x%lx, 0x%lx, %ld, 0x%lx): NULL Connection", pConnection, pPacket, nLength, pUserdata);
	return(FALSE);
}

void NCScnetSetSendBlocking(void *pConnection, BOOLEAN bOn)
{
	NCSLog(LOG_HIGH, "NCScnetSetSendBlocking(0x%lx, %s)", pConnection, bOn ? "TRUE" : "FALSE");
}

void NCScnetSetIISDLLName(char *szDLLName)
{
	NCSLog(LOG_HIGH, "NCScnetSetIISDLLName(%s)", szDLLName);
	szDefaultIISDLLName = szDLLName;
}

/** [30] Changes suggested by Manifold */
extern NCSMutex mSocketStartup;
extern NCSMutex mPollingServerMutex;
void NCScnetInit()
{
	NCSLog(LOG_HIGH,"NCScnetInit()");
	NCSMutexInit(&mPollingServerMutex);
	NCSMutexInit(&mSocketStartup);
	NCSMutexInit(&g_mCertMutex);
}
/** end [30] */

// only necessary if you plan to NCScnetDestroy within the process detach
//
void NCScnetShutdown()
{
	NCSLog(LOG_HIGH, "NCScnetShutdown()");
/** [30] Further changes suggested by Manifold */
	NCSMutexFini(&g_mCertMutex);
	NCSMutexFini(&mSocketStartup);
	NCSMutexFini(&mPollingServerMutex);
/** end [30] */	
}

#ifdef _DEBUG
void NCScnetSetTimeout(BOOLEAN bTimeout)
{
	bTimeout;//Keep compiler happy
}
#endif

typedef struct {
	NCScnetConnection *pConn;
	void *pPacketOut;
	INT32 nPacketOutLength;
	void **ppPacketIn;
	INT32 *pPacketInLength;
	NCSError *peError;
	NCSEvent	event;
} NCScnetDoInitialTransferInfo; //[25]

static void NCScnetDoInitialTransferThread(void *pData) //[25]
{
	NCScnetDoInitialTransferInfo *info = (NCScnetDoInitialTransferInfo *)pData;

	//BOOLEAN bHavePacket = FALSE;
	//NCSTimeStampMs tsExpireAt = NCSGetTimeStampMs() + NCS_CONNECTION_ATTEMP_TIMEOUT_MS;
	UINT64 nConnID = 0;

	{																		/**[01]**/
		UINT8 nServerVersion = 1;											/**[01]**/
																			/**[01]**/
		if(NCScnetRecvData(info->pConn, 											/**[01]**/
						   FALSE, 											/**[01]**/
						   &nServerVersion, 								/**[01]**/
						   sizeof(nServerVersion), 							/**[01]**/
						   (BOOLEAN*)NULL) == sizeof(nServerVersion)) {		/**[01]**/

			info->pConn->nServerVersion = nServerVersion;

			NCSLog(LOG_HIGH, "NCScnetDoInitialTransfer(): Server protocol version is %ld", (INT32)nServerVersion);
			/*
			** Now that protocol version negotiation is out of the way,
			** we can do the version specific stuff to maintain compatibility.
			*/
			if(nServerVersion >= 2) {										/**[01]**/
				/*
				** V2 protocol
				** 
				** Read server generated UINT64 ConnID out.
				*/
				if(NCScnetRecvData(info->pConn, 									/**[01]**/
								   FALSE, 									/**[01]**/
								   &nConnID, 								/**[01]**/
								   sizeof(nConnID), 						/**[01]**/
								   (BOOLEAN*)NULL) == sizeof(nConnID)) {	/**[01]**/

					NCSLog(LOG_HIGH, "NCScnetDoInitialTransfer(): Connection ID from server is %I64d", (INT64)nConnID);
					info->pConn->nID = nConnID;									/**[01]**/
				} else {													/**[01]**/
					NCSLog(LOG_MED, "NCScnetDoInitialTransfer(): Zero length data packet received");				/**[07]**/
					*(info->peError) = NCS_NET_PACKET_RECV_ZERO_LENGTH;														/**[07]**/
				}															/**[01]**/
			}																/**[01]**/

		} else {															/**[01]**/
			NCSLog(LOG_MED, "NCScnetDoInitialTransfer(): Zero length data packet received");						/**[07]**/
			*(info->peError) = NCS_NET_PACKET_RECV_ZERO_LENGTH;																/**[07]**/
		}																	/**[01]**/
	}																		/**[01]**/
	if(nConnID == 0) {														/**[01]**/
		if (*(info->peError)!=NCS_NET_PACKET_RECV_ZERO_LENGTH) {																/**[07]**/
			NCSLog(LOG_MED, "NCScnetDoInitialTransfer(): Failed to receive connection ID from server");
			*(info->peError) = NCS_NET_RECV_TIMEOUT;										/**[01]**/
		}																											/**[07]**/
	}																	/**[01]**/

	if( info->event ) NCSEventSet( info->event );

}

static NCSError NCScnetDoInitialTransfer(NCScnetConnection *pConn, //[25]
										void *pPacketOut,
										INT32 nPacketOutLength,
										void **ppPacketIn,
										INT32 *pPacketInLength,
										INT32 tsTimeOut)
{
	NCSError eError = NCS_SUCCESS;

	//Construct InitialTransfer info
	NCScnetDoInitialTransferInfo info;

	info.pConn = pConn;
	info.pPacketOut = pPacketOut;
	info.nPacketOutLength = nPacketOutLength;
	info.ppPacketIn = ppPacketIn;
	info.pPacketInLength = pPacketInLength;
	info.peError = &eError;
	info.event = NCSEventCreate();

	if(info.event && (tsTimeOut != 0) && NCSThreadSpawn(&(pConn->tDoInitialTransferThread), NCScnetDoInitialTransferThread, &info, FALSE)) {
		if( !NCSEventWait(info.event, tsTimeOut) ) {
			// Timed out!
			NCSThreadTerminate( &(pConn->tDoInitialTransferThread) );

			if(info.event) NCSEventDestroy(info.event);
			return NCS_NET_PACKET_RECV_ZERO_LENGTH;
		}
	} else {
		NCScnetDoInitialTransferThread(&info);
	}

	if(info.event) {
		NCSEventDestroy(info.event);
		info.event = NULL;
	}

	// Moved out of NCScnetDoInitialTransferThread
#ifdef ECWP_POLLING_SUPPORT
	if( *(info.peError) == NCS_SUCCESS && info.pConn->bIsPollingConnection && (pConn->nServerVersion >= 3)) {			/**[03]**/
#ifdef NOTUSED //[28]
		BOOLEAN bAlreadyShownMessage = FALSE;
		char szRegKey[MAX_PATH];
#endif
		/* 
		** V3 protocol
		** 
		** Reconnect the GET link with the server generated 
		** connection ID.
		*/
		NCScnetDisconnect(info.pConn, FALSE);
		NCScnetConnectToServer(info.pConn, FALSE, (void*)NULL, 0);

#ifdef NOTUSED //[28]

#if !defined(_WIN32_WCE)
		if (!NCSWinHttpIsService() ) {
#endif	//_WIN32_WCE
			/*
			** Reg key for message is of the form
			**
			** NCS_POLLING_SHOW_MESSAGE_KEY:server.domain
			**
			** This is so the user gets a warning for every server they visit,
			** but only once.
			*/
			sprintf(szRegKey, "%s:%s", NCS_POLLING_SHOW_MESSAGE_KEY, info.pConn->szServerName);

			if(NCSPrefGetUserBoolean(szRegKey, &bAlreadyShownMessage) != NCS_SUCCESS) {
				bAlreadyShownMessage = FALSE;
			}
			if(!bAlreadyShownMessage) {
				char buf[1024];
				sprintf(buf, "You are connecting to the Image Web Server \"%s\" through\n"
							 "a network configuration that does not support streaming HTTP connections.\n"
							 "This will increase network traffic and degrade performance.  See the\n"
							 "Image Web Server web site at http://www.earthetc.com for more information.\n"
							 "You will not see this message again.", 
							 info.pConn->szServerName);
				NCSPrefCreateUserKey(NCSPREF_DEFAULT_BASE_KEY);
				NCSPrefSetUserBoolean(szRegKey, TRUE);
#if !defined(_WIN32_WCE)
				NCSMessageBoxNoWait(NULL, buf, NCS_MESSAGE_BOX_TITLE, MB_OK|MB_ICONINFORMATION|MB_TASKMODAL|MB_SERVICE_NOTIFICATION);
#else
				NCSMessageBoxNoWait(NULL, buf, NCS_MESSAGE_BOX_TITLE, MB_OK|MB_ICONINFORMATION);
#endif
			}
#if !defined(_WIN32_WCE)
		}
#endif	//_WIN32_WCE

#endif // NOTUSED //[28]
		NCSLog(LOG_HIGH, "NCScnetDoInitialTransfer(): Connected via POLLING proxy to server %s", pConn->szServerName);

	}
#endif
	
	if( *(info.peError) == NCS_SUCCESS) {
		if(info.pPacketOut && (info.nPacketOutLength != 0)) {
			NCSLog(LOG_HIGH, "NCScnetDoInitialTransfer(): Sending initial packet 0x%lx, 0x%lx, %ld", info.pConn, info.pPacketOut, info.nPacketOutLength);
			
			*(info.peError) = NCScnetSendRawPacket(info.pConn, info.pPacketOut, info.nPacketOutLength, (BOOLEAN*)NULL);
			if(*(info.peError) == NCS_SUCCESS) {
				NCSLog(LOG_HIGH, "NCScnetDoInitialTransfer(): Send initial packet 0x%lx, 0x%lx, %ld SUCCESS", info.pConn, info.pPacketOut, info.nPacketOutLength);
				NCSLog(LOG_HIGH, "NCScnetDoInitialTransfer(): Receiving initial packet 0x%lx", info.pConn);
			
				*(info.peError) = NCScnetRecvRawPacket(info.pConn, info.ppPacketIn, info.pPacketInLength, (BOOLEAN*)NULL);
				/* Packet back from server */
				
				if((*(info.peError) != NCS_SUCCESS) || (*(info.ppPacketIn) == NULL) || (*(info.pPacketInLength) == 0)) {
					NCSFree(*(info.ppPacketIn));
					if(*(info.peError) == NCS_SUCCESS) {
						NCSLog(LOG_MED, "NCScnetDoInitialTransfer(): Receive initial packet 0x%lx FAILED", info.pConn);
				
						*(info.peError) = NCS_NET_PACKET_RECV_FAILURE;
					}
				} else {
					UINT32 nPacketSize;
					NCS_PACKET_UNPACK_BEGIN(*(info.ppPacketIn));
					NCS_PACKET_UNPACK_ELEMENT(nPacketSize);
					NCS_PACKET_UNPACK_ELEMENT(info.pConn->nPollingPingID);
					NCS_PACKET_UNPACK_END(*(info.ppPacketIn));
					
					NCSLog(LOG_HIGH, "NCScnetDoInitialTransfer(): Receive initial packet 0x%lx SUCCESS", info.pConn);
				}
			} else {
				NCSLog(LOG_MED, "NCScnetDoInitialTransfer(): Sending initial packet 0x%lx, 0x%lx, %ld FAILED", info.pConn, info.pPacketOut, info.nPacketOutLength);
			}
		} else {
			*(info.peError) = NCS_INVALID_ARGUMENTS;
		}
	}

	return eError;
}


NCSError NCScnetSendSynchronise(NCScnetConnection *pConn, NCSSequenceNr nSequence)		/**[18]**/
{
	UINT32 iPacketSize = NCS_PACKET_BASE_SIZE + sizeof(NCSSequenceNr);
	NCSPacket *pSyncPacket = (NCSPacket *)NCSMalloc(iPacketSize, FALSE);
	NCSPacketType ptType = NCSPT_SYNCHRONISE;
	NCSError eError;

	NCS_PACKET_PACK_BEGIN(pSyncPacket);
	NCS_PACKET_PACK_ELEMENT(iPacketSize);
	NCS_PACKET_PACK_ELEMENT(pConn->nPollingPingID);
	NCS_PACKET_PACK_ELEMENT(ptType);
	NCS_PACKET_PACK_ELEMENT(nSequence);
	NCS_PACKET_PACK_END(pSyncPacket);

	eError = NCScnetSendRawPacket( pConn, pSyncPacket, iPacketSize, NULL );

	//if(eError != NCS_SUCCESS) {
	NCSFree(pSyncPacket);
	//}
	return(eError);
}

#define PINGFREQUENCY 60000		/**[18]**/
#define WAITSLEEP 100			/**[18]**/

static void NCScnetSendThread(void *pData)
{
	NCScnetConnection *pConn = pData;

	NCSLog(LOG_HIGH, "NCScnetSendThread(): Startup");

	while(!pConn->bEndThreads) {
		void *pPacket;
		INT32 nPacketLength;

		if( pConn->bSendResume && !pConn->bEndThreads ) { /**[18]**/

			pConn->bSendResume = FALSE; /**[18]**/

			NCSMutexBegin(&(pConn->mSendMutex));
			pPacket = pConn->pSendPacket;
			nPacketLength = pConn->nSendPacketLength;
			pConn->pSendPacket = (void*)NULL;
			pConn->nSendPacketLength = 0;
			NCSMutexEnd(&(pConn->mSendMutex));
			
			if(pPacket && !pConn->bEndThreads ) {
				BOOLEAN bOK;
				
				NCSLog(LOG_HIGH, "NCScnetSendThread(): Sending packet 0x%lx, 0x%lx, %ld", pConn, pPacket, nPacketLength);
				
				bOK = (BOOLEAN)(NCScnetSendRawPacket(pConn,
											pPacket,
											nPacketLength,
											&(pConn->bEndThreads)) == NCS_SUCCESS);
				
				if(pConn->pSentCB && !pConn->bEndThreads) {
					if(!pConn->bEndThreads) {
						NCSLog(LOG_HIGH, "NCScnetSendThread(): %s", bOK ? "Success" : "Failed");
						
						(*pConn->pSentCB)(bOK ? NCS_SUCCESS : NCS_NET_PACKET_SEND_FAILURE, 
							pPacket, 
							pConn->pSendCBData);
					}
				} else {
					//NCSFree(pPacket);									/**[21]**/
					pPacket = 0;
				}
			}
		} 
		else if( pConn->nServerVersion >= 4 && !pConn->bEndThreads ) {	/**[18]**/
			NCSError eError=NCS_SUCCESS;
			// send sync (ping) packet every PINGFREQUENCY secs
			if( ((NCSGetTimeStampMs() - pConn->tsLastSend) > PINGFREQUENCY) && !pConn->bEndThreads) { /**[18]**/
				// send ping
				eError = NCScnetSendSynchronise( pConn, pConn->tsLastSend );
				if( eError != NCS_SUCCESS && !pConn->bEndThreads ) { // try sending ping once more time then giveup
					eError = NCScnetSendSynchronise( pConn, pConn->tsLastSend );
				}
				/*if( eError != NCS_SUCCESS && !pConn->bEndThreads) { // if sending of ping was unsuccessfull connection to server must be lost.
					pConn->bIsConnected = FALSE;
					(*pConn->pRecvCB)(NULL, 0, pConn, pConn->pRecvCBData, NCS_NET_PACKET_RECV_FAILURE);		//[19]
					NCScnetStopThreads(pConn);							//[19]
				}*/
			}
		}
		NCSSleep( WAITSLEEP ); /**[18]**/
	}
	NCSLog(LOG_HIGH, "NCScnetSendThread(): Exiting");
	NCSThreadExit(0);
}

#ifdef NOTUSED	/**[13]**/
void NCSUpdateConnectionStatus(void *pData)	/**[08]**/
{																							/**[08]**/
	NCScnetConnection *pConn = pData;
	pConn->bIsConnected = FALSE;													/**[08]**/
}																							/**[08]**/
#endif

static void NCScnetRecvThread(void *pData)
{
	NCScnetConnection *pConn = pData;
//	NCSTimeStampMs tsPeriod = NCS_RECV_THREAD_TIMEOUT;	/**[08]**/
//	NCSTimer *timer;									/**[08]**/

	NCSLog(LOG_HIGH, "NCScnetRecvThread(): Startup");

	while(!pConn->bEndThreads && pConn->bIsConnected) {
		void *pPacket;
		INT32 nPacketLength;
		NCSError eError;

		eError = NCScnetRecvRawPacket(pConn, 
									  &pPacket,
									  &nPacketLength, 
									  &(pConn->bEndThreads));
		if((eError == NCS_SUCCESS) && pPacket && (nPacketLength != 0)) {
			if(pConn->pRecvCB) {
				if(!pConn->bEndThreads) {
					NCSLog(LOG_HIGH, "NCScnetRecvThread(): Calling receive callback");

					(*pConn->pRecvCB)(pPacket, 
									  nPacketLength, 
									  pConn, 
									  pConn->pRecvCBData,
									  NCS_SUCCESS);				/**[19]**/
				} else {
					NCSLog(LOG_HIGH, "NCScnetRecvThread(): Got a packet, ending threads");
				}
			} else {
				NCSLog(LOG_HIGH, "NCScnetRecvThread(): Got a packet, no receive callback!");
				NCSFree(pPacket);
			}
		}
		if ((eError != NCS_SUCCESS) && !pConn->bEndThreads) {
			NCSTimeStampMs tsDisconnectAt = 0;		/**[13]**/

#ifdef _DEBUG
			//MessageBox(NULL, "Reconnecting", NCS_MESSAGE_BOX_TITLE, MB_OK|MB_ICONERROR|MB_TASKMODAL|MB_SERVICE_NOTIFICATION);
#endif

			NCScnetDisconnect(pConn, FALSE);

//			eError = NCScnetConnectToServer(pConn, FALSE, (void*)NULL, 0);
//			if (eError != NCS_SUCCESS) {
//				NCSLog(LOG_HIGH, "NCScnetRecvThread(): Reconnection FAILED: %s", NCSGetErrorText(eError));
//				break;
//			} 

//			tsPeriod = NCS_RECV_THREAD_TIMEOUT;									/**[08][13]**/
//			timer = NCSTimerCreate(tsPeriod,									/**[08][13]**/
//								   NCSUpdateConnectionStatus,					/**[08][13]**/
//								   pConn);										/**[08][13]**/
//			NCSTimerStart(timer);												/**[08][13]**/
			
			tsDisconnectAt = NCSGetTimeStampMs() + NCS_RECV_THREAD_TIMEOUT;		/**[13]**/

			while(!pConn->bEndThreads && 
					((eError = NCScnetConnectToServer(pConn, FALSE, (void*)NULL, 0)) != NCS_SUCCESS) &&	pConn->bIsConnected) {	/**[08]**/
				NCSLog(LOG_HIGH, "NCScnetRecvThread(): Reconnection FAILED : %s", NCSGetErrorText(eError));
				NCSSleep(250);

				if(NCSGetTimeStampMs() >= tsDisconnectAt) {						/**[13]**/
					pConn->bIsConnected = FALSE;								/**[13]**/
					(*pConn->pRecvCB)(NULL, 0, pConn, pConn->pRecvCBData, NCS_NET_PACKET_RECV_FAILURE);		/**[19]**/
					NCScnetStopThreads(pConn);							/**[19]**/
				}																/**[13]**/
			}
//			NCSTimerDestroy(timer);												/**[08][13]**/
		}
	}
	NCSThreadExit(0);
}

static BOOLEAN NCScnetStartThreads(NCScnetConnection *pConn)
{
	pConn->bEndThreads = FALSE;
	pConn->bSendResume = TRUE;

	if(NCSThreadSpawn(&(pConn->tSendThread),
					  NCScnetSendThread,
					  (void*)pConn,
					  FALSE)) {
		if(NCSThreadSpawn(&(pConn->tRecvThread),
						  NCScnetRecvThread,
						  (void*)pConn,
						  FALSE)) {
#ifdef _CLOSE_TEST
			NCSThread *pKiller = NCSMalloc(sizeof(NCSThread), TRUE);
			NCSThreadSpawn(pKiller, NCScnetSocketKillerThread, (void*) pConn, FALSE);
#endif
			return(TRUE);
		} 
	} 
	NCScnetStopThreads(pConn);
	return(FALSE);
}

static void NCScnetStopThreads(NCScnetConnection *pConn)
{
	if(pConn) {
		pConn->bEndThreads = TRUE;

		//pConn->bSendResume = TRUE;					/**[18]**/
		//NCSThreadResume(&(pConn->tSendThread));	/**[18]**/
	}
}

static void NCScnetWaitForThreadsToExit(NCScnetConnection *pConn)
{
	if(pConn && pConn->bEndThreads) {
		NCSTimeStampMs tsAbortTime = NCSGetTimeStampMs() + 10000; /**[03] - exit anyway after 10s to stop hang **/

		//pConn->bSendResume = TRUE;					/**[18]**/
		//NCSThreadResume(&(pConn->tSendThread));	/**[18]**/

		while((NCSThreadIsRunning(&(pConn->tSendThread))  ||
			   NCSThreadIsRunning(&(pConn->tRecvThread))  ||
			   NCSThreadIsRunning(&(pConn->tSendRequestThread))) &&				/**[17]**/
			  (tsAbortTime > NCSGetTimeStampMs())) {		/**[03]**/
			NCSSleep(250);
		}
		if (NCSThreadIsRunning(&(pConn->tSendRequestThread))) {									/**[17]**/
			NCSLog(LOG_LOW,"NCScnetWaitForThreadsToExit : Terminating SendRequest thread");		/**[17]**/
			if (!NCSThreadTerminate(&(pConn->tSendRequestThread)))								/**[17]**/
				NCSLog(LOG_LOW,																	/**[17]**/
					   "NCScnetWaitForThreadsToExit : Unable to terminating receive thread");	/**[17]**/
		}																						/**[17]**/
		if (NCSThreadIsRunning(&(pConn->tSendThread))) {										/**[11]**/
			NCSLog(LOG_LOW,																		/**[11]**/
				   "NCScnetWaitForThreadsToExit : Terminating send thread");					/**[11]**/
			if (!NCSThreadTerminate(&(pConn->tSendThread)))										/**[11]**/
				NCSLog(LOG_LOW,																	/**[11]**/
					   "NCScnetWaitForThreadsToExit : Unable to terminate send thread");;		/**[11]**/
		}																						/**[11]**/
		if (NCSThreadIsRunning(&(pConn->tRecvThread))) {																			/**[11]**/
			NCSLog(LOG_LOW,"NCScnetWaitForThreadsToExit : Terminating receive thread");			/**[11]**/
			if (!NCSThreadTerminate(&(pConn->tRecvThread)))										/**[11]**/
				NCSLog(LOG_LOW,																	/**[11]**/
					   "NCScnetWaitForThreadsToExit : Unable to terminating receive thread");	/**[11]**/
		}																						/**[11]**/


		//pConn->bEndThreads = FALSE;
	}
}


//#ifdef NCS_POST_VERSION2
NCSError NCScnetPostURL(char *szURL, char *szBody, char *szHeaders, char **ppAccept, BOOLEAN bIsPost, BOOLEAN bUseCache, UINT8**szResponse, int *nRespLength, UINT32 *pnStatusCode, UINT32 *pnContentLength)		/**[16]**/
{
	NCSError eError = NCS_SUCCESS;
	HINTERNET hInternetL;
	char *p = 0;
	char *pOpenVerb = NULL;
	//char *ppLAccept[] = { "text/*", "*", NULL };


	// Start up wininet (if not already)
	NCSMutexBegin(&mSocketStartup);
	eError = NCScnetInitWininet(); //[24]
	if( eError != NCS_SUCCESS ) return eError;
	NCSMutexEnd(&mSocketStartup);

	NCSWinInetAddClient();

	*szResponse = NULL;
	*nRespLength = 0;

	if(hInternetL = pInternetOpen("NCS/1.0",
								INTERNET_OPEN_TYPE_PRECONFIG,
								NULL,
								NULL,
								0)) {
		HINTERNET hConnection;
		char szServerName[MAX_PATH+1];
		INTERNET_PORT nServerPort = 0;
		int nUrlOffset = 0;
		strncpy(szServerName, szURL + strlen("http://"), MAX_PATH);

		p = szServerName;
		while(*p != '/' && *p != '\0') {
			p++;
		}
		*p = '\0';
		
		nUrlOffset = (int)(p - szServerName + strlen("http://"));

		p = strstr(szServerName, ":");

		if(p) {
			*p = '\0';
			p++;
			nServerPort = (INTERNET_PORT)atoi(p);
		}
		if(nServerPort == 0) {
			nServerPort = NCS_DEFAULT_HTTP_PORT;
		}

		if(hConnection = pInternetConnect(hInternetL,
										 szServerName,
										 nServerPort,
										 NULL,
										 NULL,
										 INTERNET_SERVICE_HTTP,
										 0, 0)) {
			HINTERNET hResource;

			if (bIsPost)
				pOpenVerb = NCSStrDup("POST");
			else {
				pOpenVerb = NCSStrDup("GET");
				szBody = NULL;
			}

reopen_request:
			if(hResource = pHttpOpenRequest(hConnection,
										   pOpenVerb,
										   szURL + nUrlOffset,//strlen("http://") + strlen(szServerName) + 1,
										   "HTTP/1.0",
										   NULL,
										   (const char**)ppAccept,
										   (bUseCache ?
												(INTERNET_FLAG_KEEP_CONNECTION)
												: (INTERNET_FLAG_KEEP_CONNECTION |
												   INTERNET_FLAG_DONT_CACHE | 
												   INTERNET_FLAG_PRAGMA_NOCACHE | 
												   INTERNET_FLAG_RELOAD |
												   INTERNET_FLAG_NO_CACHE_WRITE)),
										   0 ) ) {

				//add request headers
				if (szHeaders)
					pHttpAddRequestHeaders(hResource, szHeaders, (DWORD)-1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
				
				if(pHttpSendRequest(hResource,
								   "", 0,
								   szBody, szBody ? (DWORD)strlen(szBody):-1) == TRUE) {

					int nBytesRead = 0;
#define MEMBLOCKSIZE 1024*100
					UINT8 *pData = (UINT8*)NCSMalloc(MEMBLOCKSIZE, TRUE); //[27]

					// Retry code
					DWORD dwErrorCode;
					DWORD dwError;
					DWORD dwBufLen = sizeof(dwErrorCode);

					dwErrorCode = hResource ? ERROR_SUCCESS : GetLastError();

					dwError = pInternetErrorDlg((HWND)GetDesktopWindow(),
												hResource,
												dwErrorCode,
												FLAGS_ERROR_UI_FILTER_FOR_ERRORS |
						 						 FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS |
												 FLAGS_ERROR_UI_FLAGS_GENERATE_DATA,
												NULL);
																	  
					if(dwError == ERROR_INTERNET_FORCE_RETRY) {
						NCSWinInetInternetCloseHandle(hResource); 
						goto reopen_request;
					}
					
					// Get status code
					if( pnStatusCode ) {
						pHttpQueryInfo(hResource,
									  HTTP_QUERY_STATUS_CODE |
										HTTP_QUERY_FLAG_NUMBER,
									  &dwErrorCode,
									  &dwBufLen,
									  0);
						*pnStatusCode = (UINT32)dwErrorCode;
					}

					// Get content length
					if( pnContentLength ) {
						pHttpQueryInfo(hResource, 
									 HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
									 (LPVOID)&dwErrorCode,
									 &dwBufLen,
									 0);
						*pnContentLength = (UINT32)dwErrorCode;
					}

					// Read data from internet until none left to read
					while(TRUE) {
						unsigned long nThisRead;

						if(pInternetReadFile(hResource,
											((UINT8*)pData) + nBytesRead,
											MEMBLOCKSIZE, //[27]
											&nThisRead)) {
							if(nThisRead != 0) {
								nBytesRead += nThisRead;

								pData = (UINT8*)NCSRealloc((void*)pData, nBytesRead + MEMBLOCKSIZE, TRUE); //[27]
							} else {
								// EOF
								eError = NCS_SUCCESS;
								break;
							}
						} else {
							// ERROR
							eError = NCS_WININET_FAILURE;
							break;
						}
					}
					*szResponse = pData;
					*nRespLength = nBytesRead;
				} else {
					eError = NCS_NET_HEADER_SEND_FAILURE;
				}
				pInternetCloseHandle(hResource);
			} else {
				eError = NCS_NET_COULDNT_CONNECT;
			}
			pInternetCloseHandle(hConnection);
		} else {
			eError = NCS_NET_COULDNT_CONNECT;
		}
		pInternetCloseHandle(hInternetL);
		if (pOpenVerb)
			NCSFree(pOpenVerb);
	} else {
		eError = NCS_NET_COULDNT_CONNECT;
	}

	NCSWinInetRemoveClient();
	return(eError);
}
//#endif


#ifdef _CLOSE_TEST
static void NCScnetSocketKillerThread(void *pDummy)
{
	NCScnetConnection *pConn = (NCScnetConnection *) pDummy;
	NCSTimeStampMs nTimeout = 0;
	
	while (pConn->bEndThreads == FALSE)
	{
		if (nTimeout == 0) {
			nTimeout = NCSGetTimeStampMs() + ((rand() % 20000) + 5000);
		} else if (NCSGetTimeStampMs() > nTimeout) {
			_RPT0(_CRT_WARN, "killing recv socket now\n");
			NCScnetDisconnect(pConn, FALSE);
			nTimeout = 0;
		}
		Sleep(50);
	}
}
#endif
