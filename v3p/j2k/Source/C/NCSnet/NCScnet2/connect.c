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
** FILE:   	NCScnet2\connect.c
** CREATED:	
** AUTHOR: 	Simon Cope
** PURPOSE:	connection stuff
** EDITS:
** [01] sjc 24Feb00 Added support for polling GET connections
** [02] ny  07Feb00 Authentication
** [03] ny  03Mar00 Encryption of basic authentication scheme
** [04] ny  04Apr00 Enable the overiding of connection type via registry key
**					Remove all "max-age = 0" HTTP Header information
**					Added "Expire : 0" HTTP Header information
** [05] ny  06Apr00 Enable the overiding of "max-age=0" via registry key
**					Enable the overiding of "Expire:0" via registry key
** [06] ny  04Jul00 Comment out the encryption of the base64 digest message
**					Append "\r\n" to the WWW-Authorization string
** [07] ny  19Jul00 Change to timeout error
** [08] ny  20Jul00 Intialize string to NULL, this was
**					causing the client to send garbage to
**					the server if all the if-them-else
**					conditions are not met
** [09] ny  22Nov00 Fix wrong usage of Authorization field.
** [10] jmp 08mar01 extra parameter added to NCScnetHttpSendRequest function
** [11] rar 07Aug01	Added strlen to calls to NCScnetHttpSendRequest
 *******************************************************/




#include <string.h>        /**[02]**/
#include "cnet.h"
#include "NCSBase64.h"     /**[02]**/
#include <NCSCrypto.h> /**[03]**/

#if !defined(_WIN32_WCE)
	#include "NCSWinHttp.h"
#endif

static NCSError NCScnetQueryStatus(NCScnetConnection *pConn, BOOLEAN bSendSocket);

#ifndef _WIN32_WCE
PCCERT_CONTEXT g_pCertContext = NULL;
HCERTSTORE g_hCertStore = NULL;
NCSMutex g_mCertMutex;
#endif

/*
** Connect receive channel to server
*/
NCSError NCScnetConnectToServer(NCScnetConnection *pConn,
								BOOLEAN bSendSocket,
								void *pData,
								INT32 nDataLength)
{
	/*
	** szLastQueryString
	**
	*/
	char szLastQueryString[2048] = { '\0' }; /** [02] **/
	/*
	** szUserName, szPassword
	**
	*/
	char szUserName[256] = { '\0' }; /** [02] **/
	char szPassword[256] = { '\0' }; /** [02] **/

	/* Client Connection Information*/
	INT32 nConnectionType=0;		/**[04]**/
	char *pszContentType;		/**[04]**/
	char *pszUserAgent;			/**[04]**/
	char *pszMaxAge;			/**[05]**/
	char *pszExpire;			/**[05]**/
	char szContentType[1024] = { "image/gif" };	/**[04]**/
	char szUserAgent[1024] = { "NCS/1.0" };		/**[04]**/
	char szMaxAge[1024] = { '\0' };		/**[05]**/
	char szExpire[1024] = { '\0' };		/**[05]**/

	NCSError eError = NCS_SUCCESS;

	if( NCSPrefSetUserKeyLock(NULL) == NCS_SUCCESS ) {

		/* Retrieve Content Type and User Agent from registry settings */
		if (NCSPrefGetUserString("IWS Client User Agent",&pszUserAgent)==NCS_SUCCESS) {	/**[04]**/
			strcpy(szUserAgent,pszUserAgent);				/**[04]**/
			NCSFree(pszUserAgent);							/**[04]**/
			NCSLog(LOG_LOW, "NCScnetConnectToServer : Reading registry key [\"IWS Client User Agent\"] with %s",szUserAgent);
		} else {											/**[04]**/
			strcpy(szUserAgent,"NCS/1.0");					/**[04]**/
		}													/**[04]**/
		if (NCSPrefGetUserString("IWS Client Content Type",&pszContentType)==NCS_SUCCESS) {							/**[04]**/
			strcpy(szContentType,pszContentType);			/**[04]**/
			NCSFree(pszContentType);						/**[04]**/
			NCSLog(LOG_LOW, "NCScnetConnectToServer : Reading registry key [\"IWS Client Content Type\"] with %s",szContentType);
		} else {											/**[04]**/
			strcpy(szContentType,"image/gif");				/**[04]**/
		}													/**[04]**/
		if (NCSPrefGetUserString("IWS Client Max Age",&pszMaxAge)==NCS_SUCCESS) {							/**[05]**/
			strcpy(szMaxAge,pszMaxAge);						/**[05]**/
			NCSFree(pszMaxAge);								/**[05]**/
			NCSLog(LOG_LOW, "NCScnetConnectToServer : Reading registry key [\"IWS Client Max Age\"] with %s",szMaxAge);
		}	/*	No default value, Simon wanted it removed but
				Mark wanted it optional, this is the
				compromise (code might be difficult to understand)
			*/												/**[05]**/
		if (NCSPrefGetUserString("IWS Client Expire",&pszExpire)==NCS_SUCCESS) {							/**[05]**/
			strcpy(szExpire,pszExpire);						/**[05]**/
			NCSFree(pszExpire);								/**[05]**/
			NCSLog(LOG_LOW, "NCScnetConnectToServer : Reading registry key [\"IWS Client Expire\"] with %s",szExpire);
		}	/*	No default value, Simon wanted it added but
				Mark wanted it optional, this is the
				compromise (code might be difficult to understand)
			*/

		NCSPrefUserUnLock();
	}


	if(!NCScnetConnected(pConn, bSendSocket)) {
		HINTERNET hConnection;

		NCScnetCheckIsPollingServer(pConn);

		if( !bSendSocket && !NCSRunningInService() ) {
			if (NCSPrefGetUserInt("IWS Client Connection Type",&nConnectionType)==NCS_SUCCESS) {														/**[04]**/
				if (nConnectionType==1)														/**[04]**/
					pConn->bIsPollingConnection = TRUE;										/**[04]**/
				NCSLog(LOG_LOW,
					   "NCScnetConnectToServer : Reading registry key [\"IWS Connection Type\"] with %d",
					   nConnectionType);
			}				/**[04]**/
		}

		NCSLog(LOG_HIGH, "NCScnetConnectToServer(): Not connected");

		if(pConn->szUserName != NULL && pConn->szPassword != NULL) {
			strcpy( szUserName, pConn->szUserName );
			strcpy( szPassword, pConn->szPassword );
		}

#if !defined(_WIN32_WCE)
		if (NCSWinHttpIsService()) {


			hConnection = NCSWinHttpConnect(hInternet,
									   pConn->szServerName,
									   (INTERNET_PORT)pConn->nServerPort,
									   0);
		}
		else {
#endif
			hConnection = pInternetConnect(hInternet,
									   pConn->szServerName,
									   (INTERNET_PORT)pConn->nServerPort,
									   NULL,
									   NULL,
									   INTERNET_SERVICE_HTTP,
									   0, 0);
#if !defined(_WIN32_WCE)
		}
#endif
		if(hConnection != NULL) {
			char *pDllName = NULL;
			char *pRequest;
			HINTERNET hResource;

			NCSLog(LOG_HIGH, "NCScnetConnectToServer(): InternetConnect(%s:%ld) SUCCESS", pConn->szServerName, pConn->nServerPort);

			/* can override the default ISAPI dll names via the registry :) */
			if (NCSPrefGetUserString("IWS Client Network Dll", &pDllName) != NCS_SUCCESS) {
				pDllName = NCSStrDup(pConn->pszIISDLLName);
			}
			pRequest = NCSMalloc((UINT32)strlen(pDllName) + MAX_PATH + 10, TRUE);


reopen_request:
			/*
			** Must be inside auth_resend so we can 
			** change pConn->bIsPollingConnection
			*/
			sprintf(pRequest, "%s?%I64u,%ld,%ld",
							  pDllName,
							  pConn->nID, 
							  (long)NCS_PROTOCOL_VERSION,
							  (long)pConn->bIsPollingConnection);		/**[01]**/

#if !defined(_WIN32_WCE)
			if (NCSWinHttpIsService()) {
				hResource = NCSWinHttpOpenRequest(hConnection, 
												  bSendSocket ? "POST" : "GET",
												  pRequest, 
												  bSendSocket ? "HTTP/1.0" : "HTTP/1.1",
												  NULL, 
												  0, 
												  0);
			}
			else
#endif
			{
				unsigned long nOpenFlags = 0;
				if( pConn->bUseSSL ) {
					// if using SSL
					nOpenFlags = INTERNET_FLAG_KEEP_CONNECTION |
										 INTERNET_FLAG_DONT_CACHE |
										 INTERNET_FLAG_PRAGMA_NOCACHE | 
										 INTERNET_FLAG_NO_CACHE_WRITE |
										 INTERNET_FLAG_SECURE |
										 INTERNET_FLAG_IGNORE_CERT_CN_INVALID |
										 INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |
										 INTERNET_FLAG_RELOAD;
				} else {
					// Default: not using SSL
					nOpenFlags = INTERNET_FLAG_KEEP_CONNECTION |
										 INTERNET_FLAG_DONT_CACHE |
										 INTERNET_FLAG_PRAGMA_NOCACHE | 
										 INTERNET_FLAG_NO_CACHE_WRITE |
										 INTERNET_FLAG_RELOAD;
				}
				hResource = pHttpOpenRequest(hConnection,
									     bSendSocket ? "POST" : "GET",
										 pRequest,
										 bSendSocket ? "HTTP/1.0" : "HTTP/1.1",
										 NULL,
										 0,
										 nOpenFlags, 
										 0);
			}
			if(hResource) {
				char szHeaders[1024] = {'\0'};

				NCSLog(LOG_HIGH, "NCScnetConnectToServer(): OpenRequest(%s, %s) SUCCESS", bSendSocket ? "POST" : "GET", bSendSocket ? "HTTP/1.0" : "HTTP/1.1");

resend_request:
				if(bSendSocket) {
					if ((strlen(szUserName)!=0)&&(strlen(szPassword)!=0))
						if (strlen(szMaxAge)>0) { /**[05]**/
							if (strlen(szExpire)>0) /* Need to becareful about szExpire being NULL, i.e. premature "\r\n\r\n"*/
								sprintf(szHeaders, 
										"Cache-Control: no-cache, no-store, %s\r\nPragma: no-cache\r\n%s\r\nHost: %s\r\nUser-Agent: %s\r\nContent-Type: %s", /**[04]**/
										szMaxAge,szExpire,pConn->szServerName,szUserAgent,szContentType);/** [02] **//**[04]**//**[05]**/
							else
								sprintf(szHeaders, 
										"Cache-Control: no-cache, no-store, %s\r\nPragma: no-cache\r\nHost: %s\r\nUser-Agent: %s\r\nContent-Type: %s", /**[04]**/
										szMaxAge,pConn->szServerName,szUserAgent,szContentType);/** [02] **//**[04]**//**[05]**/
						} else {
							if (strlen(szExpire)>0) /* Need to becareful about szExpire being NULL, i.e. premature "\r\n\r\n"*/
								sprintf(szHeaders, 
										"Cache-Control: no-cache, no-store\r\nPragma: no-cache\r\n%s\r\nHost: %s\r\nUser-Agent: %s\r\nContent-Type: %s", /**[04]**/
										szExpire,pConn->szServerName,szUserAgent,szContentType);/** [02] **//**[04]**//**[05]**/
							else
								sprintf(szHeaders, 
										"Cache-Control: no-cache, no-store\r\nPragma: no-cache\r\nHost: %s\r\nUser-Agent: %s\r\nContent-Type: %s", /**[04]**/
										pConn->szServerName,szUserAgent,szContentType);/** [02] **//**[04]**//**[05]**/
						}
					else
						if (strlen(szMaxAge)>0) { /**[05]**/
							if (strlen(szExpire)>0) /* Need to becareful about szExpire being NULL, i.e. premature "\r\n\r\n"*/
								sprintf(szHeaders, 
										"Cache-Control: no-cache, no-store, %s\r\nPragma: no-cache\r\n%s\r\nContent-length: %d\r\nUser-Agent: %s\r\nContent-Type: %s", /**[04]**/
										szMaxAge,szExpire,nDataLength,szUserAgent,szContentType);/**[04]**/
							else
								sprintf(szHeaders, 
										"Cache-Control: no-cache, no-store, %s\r\nPragma: no-cache\r\nContent-length: %d\r\nUser-Agent: %s\r\nContent-Type: %s", /**[04]**/
										szMaxAge,nDataLength,szUserAgent,szContentType);/**[04]**/
						} else {
							if (strlen(szExpire)>0) /* Need to becareful about szExpire being NULL, i.e. premature "\r\n\r\n"*/
								sprintf(szHeaders, 
										"Cache-Control: no-cache, no-store\r\nPragma: no-cache\r\n%s\r\nContent-length: %d\r\nUser-Agent: %s\r\nContent-Type: %s", /**[04]**/
										szExpire,nDataLength,szUserAgent,szContentType);/**[04]**/
							else
								sprintf(szHeaders, 
										"Cache-Control: no-cache, no-store\r\nPragma: no-cache\r\nContent-length: %d\r\nUser-Agent: %s\r\nContent-Type: %s", /**[04]**/
										nDataLength,szUserAgent,szContentType);/**[04]**/
						}

				} else {
					if (strlen(szMaxAge)>0) { /**[05]**/
						if (strlen(szExpire)>0) /* Need to becareful about szExpire being NULL, i.e. premature "\r\n\r\n"*/
							sprintf(szHeaders, 
									"Cache-Control: no-cache, no-store, %s\r\nPragma: no-cache\r\n%s\r\nHost: %s\r\nUser-Agent: %s\r\nContent-Type: %s", /**[04]**/
									szMaxAge,szExpire,pConn->szServerName,szUserAgent,szContentType);/**[04]**/
						else
							sprintf(szHeaders, 
									"Cache-Control: no-cache, no-store, %s\r\nPragma: no-cache\r\nHost: %s\r\nUser-Agent: %s\r\nContent-Type: %s", /**[04]**/
									szMaxAge,pConn->szServerName,szUserAgent,szContentType);/**[04]**/
					} else {
						if (strlen(szExpire)>0) /* Need to becareful about szExpire being NULL, i.e. premature "\r\n\r\n"*/
							sprintf(szHeaders, 
									"Cache-Control: no-cache, no-store\r\nPragma: no-cache\r\n%s\r\nHost: %s\r\nUser-Agent: %s\r\nContent-Type: %s", /**[04]**/
									szExpire,pConn->szServerName,szUserAgent,szContentType);/**[04]**/
						else
							sprintf(szHeaders, 
									"Cache-Control: no-cache, no-store\r\nPragma: no-cache\r\nHost: %s\r\nUser-Agent: %s\r\nContent-Type: %s", /**[04]**/
									pConn->szServerName,szUserAgent,szContentType);/**[04]**/
					}
				}
				if ((strlen(szUserName)!=0)&&(strlen(szPassword)!=0)) {                /**[02]**/
					char szHeader[1024] = {'\0'};                                               /**[02]**/
					char szBase64Msg[1024] = {'\0'};                                   /**[02]**/
					char szCredentials[1024] = {'\0'};                                 /**[02]**/
					DWORD szLength;                                                    /**[02]**/
					//char szCipherText[1024] = {'\0'};                                  /**[03]**/

					strcat(szCredentials,szUserName);                                  /**[02]**/
					strcat(szCredentials,":");                                         /**[02]**/
					strcat(szCredentials,szPassword);                                  /**[02]**/

					NCSEncodeBase64Message(szCredentials,szBase64Msg);                 /**[02]**/
//					NCSEncryptData(szBase64Msg,szCipherText);                          /**[03]**/
//					sprintf(szHeader,"WWW-Authorization: Basic %s\r\n",szCipherText); /**[03]**/
					sprintf(szHeader,"\r\nAuthorization: Basic %s\r\n",szBase64Msg); /**[03]**/ /**[09]**/
					szLength = (DWORD)strlen(szHeader);                                       /**[02]**/
					strcat(szHeaders,szHeader);                                        /**[02]**/
				}                                                                      /**[02]**/

				switch(NCScnetHttpSendRequest(&(pConn->tSendRequestThread),			/**[10]**/
											  hResource,
											  OS_STRING(szHeaders),
											  (DWORD)strlen(szHeaders),//-1L, //[11]
											  pData,
											  nDataLength,
#ifdef NO_TIMEOUT
											  0)){//(bSendSocket ? 30000 : (pConn->bIsPollingConnection ? 30000 : 10000))) ) {	/**[01]**/
#else
											  (bSendSocket ? 30000 : (pConn->bIsPollingConnection ? 30000 : 10000))) ) {	/**[01]**/
#endif

					case NCS_HTTP_SEND_TIMEOUT:																		/**[01]**/
						if( !bSendSocket ) {	
							if(pConn->bIsPollingConnection || NCSRunningInService() ) {														/**[01]**/
								NCSLog(LOG_MED, "NCScnetConnectToServer(): POLLING SendRequest(%s) FAILED Win32 error %d", szHeaders, GetLastError());		/**[01]**/
								eError = NCS_NET_HEADER_SEND_TIMEOUT;												/**[07]**/
								hResource = NULL;																	/**[01]**/
							} else {																				/**[01]**/
								pConn->bIsPollingConnection = TRUE;													/**[01]**/
								hResource = NULL;																	/**[01]**/
								NCSLog(LOG_HIGH, "NCScnetConnectToServer(): Switching to polling connection");		/**[01]**/
								goto reopen_request;																/**[01]**/
							}
						} else {
							hResource = NULL;
							NCSLog(LOG_HIGH, "NCScnetConnectToServer(): SendRequest(%s) FAILED Win32 error %d", szHeaders, GetLastError());
							eError = NCS_NET_HEADER_SEND_TIMEOUT;
						}
						break;
					case NCS_HTTP_SEND_INVALID_CA:
						{
							DWORD dwFlags;
							DWORD dwBuffLen = sizeof(dwFlags);

							/*DWORD dwError;
							// Make sure to check return code from InternetErrorDlg
							// user may click either OK or Cancel. In case of Cancel
							// request should not be resumbitted.
							dwError = pInternetErrorDlg (GetDesktopWindow(),
								hResource,
								ERROR_INTERNET_INVALID_CA,
								FLAGS_ERROR_UI_FILTER_FOR_ERRORS |
								FLAGS_ERROR_UI_FLAGS_GENERATE_DATA |
								FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS,
								NULL);
							if( dwError == ERROR_SUCCESS ) {
								goto resend_request;
							} else {
								eError = NCS_NET_403_FORBIDDEN;
							}*/
							
							pInternetQueryOption(hResource, INTERNET_OPTION_SECURITY_FLAGS,
								(LPVOID)&dwFlags, &dwBuffLen);
							
							dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
							dwFlags |= SECURITY_FLAG_IGNORE_REVOCATION;

							pInternetSetOption(hResource, INTERNET_OPTION_SECURITY_FLAGS,
								&dwFlags, sizeof (dwFlags) );
							goto resend_request;							
						}
						break;
					case NCS_HTTP_SEND_CLIENT_AUTH_CERT_NEEDED:
						{
							// Return ERROR_SUCCESS regardless of clicking on OK or Cancel
							/*if( pInternetErrorDlg( GetDesktopWindow(), 
								hResource,
								ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED,
								FLAGS_ERROR_UI_FILTER_FOR_ERRORS       |
								FLAGS_ERROR_UI_FLAGS_GENERATE_DATA     |
								FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS, 
								NULL) == ERROR_SUCCESS )
							{
								goto resend_request;
							}*/
#ifndef _WIN32_WCE							
							NCSMutexBegin(&g_mCertMutex);
							if( !pConn->pCertContext && !pConn->bCertSuccess ) {

								if( !g_hCertStore ) g_hCertStore = CertOpenSystemStore((HCRYPTPROV)NULL, NCS_T("MY"));

								if( g_hCertStore ) {

									//First try the global certificate
									if( !pConn->pCertContext && g_pCertContext ) {
										pConn->pCertContext = CertDuplicateCertificateContext(g_pCertContext);
									} else {
										if( pConn->pCertContext ) CertFreeCertificateContext( pConn->pCertContext );
										
										pConn->pCertContext = NCSCertificateChooser(g_hCertStore);
									}

									if( pConn->pCertContext ) {
										int res = pInternetSetOption(hResource, 
											INTERNET_OPTION_CLIENT_CERT_CONTEXT, 
											(void *) pConn->pCertContext, sizeof(CERT_CONTEXT));

										if (res) {
											NCSMutexEnd(&g_mCertMutex);
											goto resend_request;
										}
									}
								}
							} else if( pConn->bCertSuccess ) {
								if( pConn->pCertContext ) {
									int res = pInternetSetOption(hResource, 
										INTERNET_OPTION_CLIENT_CERT_CONTEXT, 
										(void *) pConn->pCertContext, sizeof(CERT_CONTEXT));

									if (res) {
										NCSMutexEnd(&g_mCertMutex);
										goto resend_request;
									}
								}
							}
							NCSMutexEnd(&g_mCertMutex);
#endif // _WIN32_WCE
							eError = NCS_NET_403_FORBIDDEN;
						}
						break;

					case NCS_HTTP_SEND_SUCCESS:
							{
								DWORD dwErrorCode;
								DWORD dwError;

								dwErrorCode = hResource ? ERROR_SUCCESS : GetLastError();

								NCSLog(LOG_HIGH, "NCScnetConnectToServer(): SendRequest(%s) SUCCESS", szHeaders);

								// Cant authenicate if we are inside a server, cant pop up a dialog box, so just fail.
#if !defined(_WIN32_WCE)
								if (!NCSWinHttpIsService()) {
#endif
									
									dwError = pInternetErrorDlg((HWND)GetDesktopWindow(),
										hResource,
										dwErrorCode,
										FLAGS_ERROR_UI_FILTER_FOR_ERRORS |
										FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS |
										FLAGS_ERROR_UI_FLAGS_GENERATE_DATA,
										NULL);
									
									if(dwError == ERROR_INTERNET_FORCE_RETRY) {
										/** [02] START **/
										char text[1024] = {'\0'};
										DWORD nErrorLength = sizeof(text);
										DWORD nFieldNr = 0;
										
										if(pHttpQueryInfo(hResource,
											HTTP_QUERY_WWW_AUTHENTICATE,
											text,
											&nErrorLength,
											&nFieldNr)) {
											char *pdest = strstr(text,"Image Web Server");
											
											if (pdest!=NULL) {
											/*
											** Before closing the current handle, we must extract the query string for the auth_resend
											** section or else the new query string will not contain the User/Password string.
												*/
												DWORD nErrorLength;
												DWORD nFieldNr;
												
												nErrorLength = sizeof(szUserName);
												pInternetQueryOption((HINTERNET)hResource, INTERNET_OPTION_USERNAME, (void*)szUserName, &nErrorLength);
												nErrorLength = sizeof(szPassword);
												pInternetQueryOption((HINTERNET)hResource, INTERNET_OPTION_PASSWORD, (void*)szPassword, &nErrorLength);
												
												
												/* --------------------------------------- */
												nErrorLength = sizeof(szLastQueryString);
												nFieldNr = 0;
												pHttpQueryInfo(hResource,
													HTTP_QUERY_RAW_HEADERS_CRLF,
													szLastQueryString,
													&nErrorLength,
													&nFieldNr);
												pInternetCloseHandle(hResource); 
												NCSLog(LOG_HIGH, "NCScnetConnectToServer(): Image Web Server Auth_Resend");
												goto reopen_request;
											}
										}
										/** [02] END **/
										NCSLog(LOG_HIGH, "NCScnetConnectToServer(): Resend");
										goto resend_request;
									}

#if !defined(_WIN32_WCE)
								}
#endif
								if(bSendSocket) {
									pConn->hSendConn = hConnection;
									pConn->hSendResource = hResource;
								} else {
									pConn->hRecvConn = hConnection;
									pConn->hRecvResource = hResource;
								}

								eError = NCScnetQueryStatus(pConn, bSendSocket);

								if(eError == NCS_SUCCESS) {
#ifndef _WIN32_WCE
									NCSMutexBegin(&g_mCertMutex);
									if( pConn->pCertContext && (g_pCertContext != pConn->pCertContext) ) {
										if (g_pCertContext) {
											CertFreeCertificateContext(g_pCertContext);
											g_pCertContext = NULL;
										}
										g_pCertContext = CertDuplicateCertificateContext(pConn->pCertContext);
									}
									pConn->bCertSuccess = TRUE;
									NCSMutexEnd(&g_mCertMutex);
#endif
									NCScnetAddPollingServer(pConn);
								}
							}
						break;

					case NCS_HTTP_SEND_FAILED:
							NCSLog(LOG_MED, "NCScnetConnectToServer(): SendRequest(%s) FAILED Win32 error %d", szHeaders, GetLastError());
							eError = NCS_NET_HEADER_SEND_FAILURE;
						break;
				}
			} else {
				NCSLog(LOG_MED, "NCScnetConnectToServer(): OpenRequest(%s, %s) FAILED Win32 error %d", bSendSocket ? "POST" : "GET", bSendSocket ? "HTTP/1.0" : "HTTP/1.1", GetLastError());

				eError = NCS_NET_COULDNT_CONNECT;
			}

			NCSFree(pDllName);
			NCSFree(pRequest);

			if(eError != NCS_SUCCESS) {
				NCScnetDisconnect(pConn, bSendSocket);
			}
		} else {
			NCSLog(LOG_MED, "NCScnetConnectToServer(): InternetConnect(%s:%ld) FAILED Win32 error %d", pConn->szServerName, pConn->nServerPort, GetLastError());

			eError = NCS_NET_COULDNT_CONNECT;
		}
	} else {
		NCSLog(LOG_LOW, "NCScnetConnectToServer(): Already connected!");

		eError = NCS_NET_ALREADY_CONNECTED;
	}
	return eError;
}

static NCSError NCScnetQueryStatus(NCScnetConnection *pConn,
								   BOOLEAN bSendSocket)
{
	DWORD dwError;
	DWORD nErrorLength = sizeof(dwError);
	DWORD nFieldNr = 0;
	BOOL  bHaveErrorCode = FALSE;

#if !defined(_WIN32_WCE)
	if (NCSWinHttpIsService()) {
		if (NCSWinHttpGetConnectionStatusCode(bSendSocket ? pConn->hSendResource : pConn->hRecvResource, &dwError)) {
			bHaveErrorCode = TRUE;
		} else {
			dwError = GetLastError();
		}
	}
	else {
#endif
		if(pHttpQueryInfo(bSendSocket ? pConn->hSendResource : pConn->hRecvResource,
						  HTTP_QUERY_STATUS_CODE |
							HTTP_QUERY_FLAG_NUMBER,
						  &dwError,
						  &nErrorLength,
						  &nFieldNr)) {
			bHaveErrorCode = TRUE;
		}
#if !defined(_WIN32_WCE)
	}
#endif

	if (bHaveErrorCode) {
		switch(dwError) {
			case 200:
			case 204:
#ifdef NOTYET
				/*
				** Doesn't seem to be a very reliable test for keep-alives
				*/ 
					if(!bSendSocket && !pConn->bNoKeepAlives) {
						char text[1024] = {'\0'};
						nErrorLength = sizeof(text);

						if(pHttpQueryInfo(bSendSocket ? pConn->hSendResource : pConn->hRecvResource,
											  HTTP_QUERY_CONNECTION,
											  text,
											  &nErrorLength,
											  &nFieldNr)) {
							if(stricmp(text, "Keep-Alive") != 0) {
								pConn->bNoKeepAlives = TRUE;
							}
						} else {
							pConn->bNoKeepAlives = TRUE;
						}
					}
#endif
					NCSLog(LOG_HIGH, "NCScnetQueryStatus(): HTTP 200/204 Response");
					return(NCS_SUCCESS);
				break;
			/** [02] START **/
			case 400:
					{
#if !defined(_WIN32_WCE)
						if (NCSWinHttpIsService()) {
							LPCSTR pStatusText;
							if (NCSWinHttpGetConnectionStatusText(bSendSocket ? pConn->hSendResource : pConn->hRecvResource, &pStatusText)) {
								NCSLog(LOG_HIGH, "NCScnetQueryStatus(): HTTP 400 Response: %s", pStatusText);
								if (pStatusText)NCSFree((void*)pStatusText);
							}
						}
						else {
#endif
							char text[1024] = {'\0'};
							nErrorLength = sizeof(text);

							if(pHttpQueryInfo(bSendSocket ? pConn->hSendResource : pConn->hRecvResource,
												  HTTP_QUERY_STATUS_TEXT,
												  text,
												  &nErrorLength,
												  &nFieldNr)) {
								NCSLog(LOG_HIGH, "NCScnetQueryStatus(): HTTP 400 Response: %s", text);
							}
#if !defined(_WIN32_WCE)
						}
#endif
					}
					return(NCS_NET_UNEXPECTED_RESPONSE);
				break;
			/** [02] END **/
			case 401:
					NCSLog(LOG_HIGH, "NCScnetQueryStatus(): HTTP 401 Response: Unauthorised");
					return(NCS_NET_401_UNAUTHORISED);
				break;

			case 403:
					NCSLog(LOG_HIGH, "NCScnetQueryStatus(): HTTP 403 Response: Forbidden");
					return(NCS_NET_403_FORBIDDEN);
				break;

			case 407:
					NCSLog(LOG_HIGH, "NCScnetQueryStatus(): HTTP 407 Response: Proxy Authentication required");
					return(NCS_NET_407_PROXYAUTH);
				break;
			
			case 500:
					NCSLog(LOG_HIGH, "NCScnetQueryStatus(): HTTP 500 Response: Server Error");
					return(NCS_SERVER_ERROR);
				break;

			default: 
					{
						char text[1024] = {'\0'};
						nErrorLength = sizeof(text);

#if !defined(_WIN32_WCE)
						if (NCSWinHttpIsService()) {
							LPCSTR pStatusText;
							if (NCSWinHttpGetConnectionStatusText(bSendSocket ? pConn->hSendResource : pConn->hRecvResource, &pStatusText)) {
								NCSLog(LOG_HIGH, "NCScnetQueryStatus(): HTTP %s Response: %s", dwError, pStatusText);
								if (pStatusText) NCSFree((void*)pStatusText);
							}
						}
						else {
#endif
							if(pHttpQueryInfo(bSendSocket ? pConn->hSendResource : pConn->hRecvResource,
												  HTTP_QUERY_STATUS_TEXT,
												  text,
												  &nErrorLength,
												  &nFieldNr)) {
								NCSLog(LOG_HIGH, "NCScnetQueryStatus(): HTTP %s Response: %s", dwError, text);
							}
#if !defined(_WIN32_WCE)
						}
#endif
					}
					NCSLog(LOG_HIGH, "NCScnetQueryStatus(): HTTP Unexpected Response");
					return(NCS_NET_UNEXPECTED_RESPONSE);
				break;
		}
	}
	NCSLog(LOG_HIGH, "NCScnetQueryStatus(): HttpQueryInfo() failed");
	return(NCS_NET_HEADER_RECV_FAILURE);
}

/*
** Close the sockets
*/
void NCScnetDisconnect(NCScnetConnection *pConn,
					   BOOLEAN bSendSocket)
{
#if !defined(_WIN32_WCE)
	BOOL bIsService = NCSWinHttpIsService();
#endif
	NCSLog(LOG_HIGH, "NCScnetDisconnect(): Entry");
	
	if(pConn) {
		if(bSendSocket) {
			if(pConn->hSendResource != NULL) {
				NCSLog(LOG_HIGH, "NCScnetDisconnect(): InternetCloseHandle(hSendResource: 0x%lx)", pConn->hSendResource);
#if !defined(_WIN32_WCE)
				if (bIsService) {
					NCSWinHttpCloseHandle(pConn->hSendResource);
				} else {
					pInternetCloseHandle(pConn->hSendResource); 
				}
#else
				pInternetCloseHandle(pConn->hSendResource); 
#endif
				pConn->hSendResource = NULL;
			}
			if(pConn->hSendConn != NULL) {
				NCSLog(LOG_HIGH, "NCScnetDisconnect(): InternetCloseHandle(hSendConn: 0x%lx)", pConn->hSendConn);
#if !defined(_WIN32_WCE)
				if (bIsService) {
					NCSWinHttpCloseHandle(pConn->hSendConn);
				} else {
					pInternetCloseHandle(pConn->hSendConn); 
				}
#else
				pInternetCloseHandle(pConn->hSendConn); 
#endif
				pConn->hSendConn = NULL;
			}
		} else {
			if(pConn->hRecvResource != NULL) {
				NCSLog(LOG_HIGH, "NCScnetDisconnect(): InternetCloseHandle(hRecvResource: 0x%lx)", pConn->hRecvResource);
#if !defined(_WIN32_WCE)
				if (bIsService) {
					NCSWinHttpCloseHandle(pConn->hRecvResource);
				} else {
					pInternetCloseHandle(pConn->hRecvResource); 
				}
#else
		pInternetCloseHandle(pConn->hRecvResource); 
#endif
				pConn->hRecvResource = NULL;
			}
			if(pConn->hRecvConn != NULL) {
				NCSLog(LOG_HIGH, "NCScnetDisconnect(): InternetCloseHandle(hRecvConn: 0x%lx)", pConn->hRecvConn);
#if !defined(_WIN32_WCE)
				if (bIsService) {
					NCSWinHttpCloseHandle(pConn->hRecvConn);
				} else {
					pInternetCloseHandle(pConn->hRecvConn);
				}				 
#else
					pInternetCloseHandle(pConn->hRecvConn);
#endif
				pConn->hRecvConn = NULL;
			}
		}
	}
	NCSLog(LOG_HIGH, "NCScnetDisconnect(): Exit");
}



/*
** Send data over connection
*/
INT32 NCScnetSendData(NCScnetConnection *pConn,
					  BOOLEAN bSendSocket,
					  void *pData,
					  INT32 nBytesToSend,
					  BOOLEAN *pbCancelSend)
{
	int nBytesSent = 0;
#if !defined(_WIN32_WCE)
	BOOL bIsService = NCSWinHttpIsService();
#endif
	NCSLog(LOG_HIGH, "NCScnetSendData(0x%lx, %s, 0x%lx, %ld): Entry", pConn, bSendSocket ? "TRUE" : "FALSE", pData, nBytesToSend);

	if(pConn) {
		while(nBytesSent < nBytesToSend) {
			DWORD nThisSend;

#if !defined(_WIN32_WCE)
			if (bIsService) {
				if (NCSWinHttpWriteData(bSendSocket ? pConn->hSendResource : pConn->hRecvResource,
									  ((UINT8*)pData) + nBytesSent,
									  pbCancelSend ? MIN(NCS_PACKET_CHUNK_SIZE, nBytesToSend - nBytesSent) : (nBytesToSend - nBytesSent),
									  &nThisSend)) {
				}
				else {
					NCSLog(LOG_MED, "NCScnetSendData(): NCSWinHttpWriteData() FAILED: This %ld Sent %ld, Win32 Error %ld", nBytesSent, nThisSend, GetLastError());
					return(-1);
				}
			}
			else {
#endif
				if(pInternetWriteFile(bSendSocket ? pConn->hSendResource : pConn->hRecvResource,
									  ((UINT8*)pData) + nBytesSent,
									  pbCancelSend ? MIN(NCS_PACKET_CHUNK_SIZE, nBytesToSend - nBytesSent) : (nBytesToSend - nBytesSent),
									  &nThisSend)) {
					nBytesSent += nThisSend;

					NCSLog(LOG_HIGH, "NCScnetSendData(): InternetWriteFile() This %ld Sent %ld", nBytesSent, nThisSend);

					if(pbCancelSend && *pbCancelSend) {
						break;
					}
				} else {
					NCSLog(LOG_MED, "NCScnetSendData(): InternetWriteFile() FAILED: This %ld Sent %ld, Win32 Error %ld", nBytesSent, nThisSend, GetLastError());
					return(-1);
				}
#if !defined(_WIN32_WCE)
			}
#endif
		}
	}
	NCSLog(LOG_HIGH, "NCScnetSendData(0x%lx, %s, 0x%lx, %ld): Exit (Send %ld bytes)", pConn, bSendSocket ? "TRUE" : "FALSE", pData, nBytesToSend, nBytesSent);
	return(nBytesSent);
}

/* 
** Receive or peek data from connection 
*/

INT32 NCScnetRecvData(NCScnetConnection *pConn,
					  BOOLEAN bSendSocket,
					  void *pData,
					  INT32 nBytesToRead,
					  BOOLEAN *pbCancelRecv)
{
	int nBytesRead = 0;
#if !defined(_WIN32_WCE)
	BOOL bIsService = NCSWinHttpIsService();
#endif
	if(pConn) {
		while(nBytesRead < nBytesToRead) {
			DWORD nThisRead;
		
#if !defined(_WIN32_WCE)
			if (bIsService) {
				if (NCSWinHttpReadData(bSendSocket ? pConn->hSendResource : pConn->hRecvResource,
									 ((UINT8*)pData) + nBytesRead,
									 pbCancelRecv ? MIN(NCS_PACKET_CHUNK_SIZE, nBytesToRead - nBytesRead) : (nBytesToRead - nBytesRead),
									 &nThisRead)) {
					nBytesRead += nThisRead;
					NCSLog(LOG_HIGH, "NCScnetRecvData(): InternetReadFile() This %ld Read %ld", nBytesRead, nThisRead);
					if((pbCancelRecv && *pbCancelRecv) || (nThisRead == 0)) {
						break;
					}
				}
				else {
					if(NCScnetConnected(pConn, bSendSocket)) {
						NCSLog(LOG_MED, "NCScnetRecvData(): InternetReadFile() FAILED: This %ld Read %ld, Win32 Error %ld", nBytesRead, nThisRead, GetLastError());
					}
					//NCSFree( BuffersOut.lpvBuffer );
					return(-1);
				}
			} else {
#endif
				if(pInternetReadFile(bSendSocket ? pConn->hSendResource : pConn->hRecvResource,
									 ((UINT8*)pData) + nBytesRead,
									 pbCancelRecv ? MIN(NCS_PACKET_CHUNK_SIZE, nBytesToRead - nBytesRead) : (nBytesToRead - nBytesRead),
									 &nThisRead)) {
			
					nBytesRead += nThisRead;
					NCSLog(LOG_HIGH, "NCScnetRecvData(): InternetReadFile() This %ld Read %ld", nBytesRead, nThisRead);
					if((pbCancelRecv && *pbCancelRecv) || (nThisRead == 0)) {
						break;
					}
				} else {
					if(NCScnetConnected(pConn, bSendSocket)) {
						NCSLog(LOG_MED, "NCScnetRecvData(): InternetReadFile() FAILED: This %ld Read %ld, Win32 Error %ld", nBytesRead, nThisRead, GetLastError());
					}
					//NCSFree( BuffersOut.lpvBuffer );
					return(-1);
				}
#if !defined(_WIN32_WCE)
			}
#endif
		}
	}
	return(nBytesRead);
}

/*
** Disconnect from server
*/
void NCScnetDisconnectFromServer(NCScnetConnection *pConn)
{
	if(pConn) {
		NCScnetDisconnect(pConn, TRUE);
		NCScnetDisconnect(pConn, FALSE);
	}
}

#ifdef ECWP_POLLING_SUPPORT
/*
** [01]
**
** POll the server - this involves resending the GET request.
** If no Keep-Alives, reconnect.
*/
NCSError NCScnetPollServer(NCScnetConnection *pConn)
{
	NCSError eError = NCS_SUCCESS;
	/* Client Connection Information*/
	char *pszUserAgent;		/**[04]**/
	char szUserAgent[1024] = { '\0' };	/**[04]**/	/**[08]**/
	char *pszMaxAge;		/**[05]**/
	char szMaxAge[1024] = { '\0' };	/**[05]**/		/**[08]**/
	char *pszExpire;		/**[05]**/
	char szExpire[1024] = { '\0' };	/**[05]**/		/**[08]**/

	/* Retrieve Content Type and User Agent from registry settings */
	if (NCSPrefGetUserString("IWS Client User Agent",&pszUserAgent)==NCS_SUCCESS) {							/**[04]**/
		strcpy(szUserAgent,pszUserAgent);				/**[04]**/
		NCSFree(pszUserAgent);							/**[04]**/
		NCSLog(LOG_LOW, "NCScnetPollServer : Reading registry key [\"IWS Client User Agent\"] with %s",szUserAgent);
	} else {											/**[04]**/
		strcpy(szUserAgent,"NCS/1.0");					/**[04]**/
	}													/**[04]**/
	if (NCSPrefGetUserString("IWS Client Max Age",&pszMaxAge)==NCS_SUCCESS) {							/**[05]**/
		strcpy(szMaxAge,pszMaxAge);						/**[05]**/
		NCSFree(pszMaxAge);								/**[05]**/
		NCSLog(LOG_LOW, "NCScnetPollServer : Reading registry key [\"IWS Client Max Age\"] with %s",szMaxAge);
	}	/*	No default value, Simon wanted it removed but
			Mark wanted it optional, this is the
			compromise (code might be difficult to understand)
		*/												/**[05]**/
	if (NCSPrefGetUserString("IWS Client Expire",&pszExpire)==NCS_SUCCESS) {							/**[05]**/
		strcpy(szExpire,pszExpire);						/**[05]**/
		NCSFree(pszExpire);								/**[05]**/
		NCSLog(LOG_LOW, "NCScnetPollServer : Reading registry key [\"IWS Client Expire\"] with %s",szExpire);
	}	/*	No default value, Simon wanted it added but
			Mark wanted it optional, this is the
			compromise (code might be difficult to understand)
		*/												/**[05]**/
	
	if(pConn->bNoKeepAlives) {
		NCScnetDisconnect(pConn, FALSE);
		eError = NCScnetConnectToServer(pConn, FALSE, (void*)NULL, 0);
	} else {
		char szHeaders[1024] = {'\0'};

		if (strlen(szMaxAge)>0)	{																				/**[05]**/
			if (strlen(szExpire)>0)
				sprintf(szHeaders,																					/**[05]**/
						"Cache-Control: no-cache, no-store, %s\r\nPragma: no-cache\r\n%s\r\nHost: %s\r\nUser-Agent: %s",/**[04]**//**[05]**/
						szMaxAge,szExpire,pConn->szServerName,szUserAgent);											/**[05]**/
			else
				sprintf(szHeaders,																					/**[05]**/
						"Cache-Control: no-cache, no-store, %s\r\nPragma: no-cache\r\nHost: %s\r\nUser-Agent: %s",/**[04]**//**[05]**/
						szMaxAge,pConn->szServerName,szUserAgent);											/**[05]**/
		} else {																									/**[05]**/
			if (strlen(szExpire)>0)
				sprintf(szHeaders,																					/**[05]**/
						"Cache-Control: no-cache, no-store\r\nPragma: no-cache\r\n%s\r\nHost: %s\r\nUser-Agent: %s",	/**[04]**//**[05]**/
						szExpire,pConn->szServerName,szUserAgent);													/**[05]**/
			else
				sprintf(szHeaders,																					/**[05]**/
						"Cache-Control: no-cache, no-store\r\nPragma: no-cache\r\nHost: %s\r\nUser-Agent: %s",	/**[04]**//**[05]**/
						pConn->szServerName,szUserAgent);													/**[05]**/
		}		
		switch(NCScnetHttpSendRequest(&(pConn->tSendRequestThread),		/**[10]**/
								      pConn->hRecvResource,
									  OS_STRING(szHeaders),
									  (DWORD)strlen(szHeaders),//-1L,	//[11]
									  (LPVOID)NULL,
									  0,
#ifdef NO_TIMEOUT
									  0)) {
#else
									  pConn->bIsPollingConnection ? 10000 : 10000)) {
#endif
			case NCS_HTTP_SEND_SUCCESS:
					eError = NCScnetQueryStatus(pConn, FALSE);
				break;

			case NCS_HTTP_SEND_TIMEOUT:
					pConn->hRecvResource = NULL;
				// Fallthrough		

			case NCS_HTTP_SEND_FAILED:
				eError = NCS_NET_COULDNT_CONNECT;
				break;
		}
	}
	return(eError);
}
#endif