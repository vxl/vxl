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
** FILE:   	NCScnet2\cnet.h
** CREATED:	
** AUTHOR: 	Simon Cope
** PURPOSE:	cnet private header
** EDITS:
** [01] sjc 01Oct99 Protocol version info
** [02] dmm 18Oct99 added NCSnetProxyInfo struct
** [03] sjc 25Feb00 Added polling GET connection support
** [04] ny  07Feb00 Authentication
** [05] ny  11May00 Added bIsConnected field to the CNET
**					structure so that we can propagate
**					status back to the calling function
** [06] jmp 08Mar01 Added tSendRequestThread to wait for SendRequest thread to exit
**					and NCScnetConnection param to NCScnetHttpSendRequest function
** [07] rar 14May01 Client sends ping to server every 60secs (if no other sends done).  So server can
**					tell when a client dies.
** [08] rar 15Jan02 Moved the wininet initialization code out of NCScnetCreateEx and into NCScnetInitWininet.
** [09] rar 18Feb02 Modified NCSDoInitialTransfer to timeout, by running in a new thread, if not completed
						in time specified by NCS_INITIAL_TRANSFER_TIMEOUT.
 *******************************************************/

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#ifndef CNET_H
#define CNET_H

#include "NCSUtil.h"
#include "NCScnet.h"
#include "NCSnet.h"
#include "NCSlog.h"
#include "NCSBuildNumber.h"
#include "NCSEvent.h"
#include <Wincrypt.h>

#if !defined(_WIN32_WCE)
#include <crtdbg.h>
#endif
#include <stdio.h>

//typedef SIZE_T DWORD_PTR, *PDWORD_PTR;

#if !defined(_WIN32_WCE)
#include "WinInet.h"
#else
	#include <Wininet.h>
#endif

#define ECWP_POLLING_SUPPORT

typedef struct {
	BOOLEAN bUseProxy;
	char szHost[1024];
	short nPort;
	char szAutoURL[1024];
	char szBypassList[1024];
	char szUsername[1024];
	char szPassword[1024];
} NCSnetProxyInfo;


/*
** CNET Struct
*/
typedef struct {
	HINTERNET hSendConn;
	HINTERNET hSendResource;
	HINTERNET hRecvConn;
	HINTERNET hRecvResource;

	char	szServerName[MAX_PATH];
	UINT32	nServerPort;
	BOOLEAN bUseSSL;

	BOOLEAN bViaProxy;
	BOOLEAN bEndThreads;
	BOOLEAN bIsPollingConnection;	/**[03]**/
	BOOLEAN bNoKeepAlives;			/**[03]**/
	BOOLEAN bIsConnected;			/**[05]**/

	UINT64	nID;
	UINT64  nPollingPingID;			/**[07]**/
	UINT8	nServerVersion;			/**[01]**/

	NCSMutex mSendMutex;
	NCSThread tSendThread;

	NCSnetPacketSentCB *pSentCB;
	void *pSendCBData;
	void *pSendPacket;
	INT32 nSendPacketLength;
	BOOLEAN bSendResume;			/**[07]**/
	NCSTimeStampMs tsLastSend;		/**[07]**/
	NCSTimeStampMs tsLastRecv;
	
	NCSThread tRecvThread;
	NCSThread tDoInitialTransferThread; //[09]

	NCSnetPacketRecvCB *pRecvCB;
	void *pRecvCBData;

	NCSThread tSendRequestThread;		/**[06]**/

	char *pszIISDLLName;

	char *szUserName;
	char *szPassword;

	PCCERT_CONTEXT pCertContext;
	BOOLEAN bCertSuccess;
} NCScnetConnection;

typedef enum {
	NCS_HTTP_SEND_FAILED = 0,
	NCS_HTTP_SEND_SUCCESS = 1,
	NCS_HTTP_SEND_TIMEOUT = 2,
	NCS_HTTP_SEND_INVALID_CA = 3,
	NCS_HTTP_SEND_CLIENT_AUTH_CERT_NEEDED = 4
} NCScnetHttpSendRequestResult;			/**[03]**/

typedef struct {
	HINTERNET	hRequest;
    LPCTSTR		lpszHeaders;
    DWORD		dwHeadersLength;
    LPVOID		lpOptional;
    DWORD		dwOptionalLength;
	NCScnetHttpSendRequestResult RVal;
	BOOL		bCancel;
	NCSEvent	event;
} NCScnetHttpSendRequestInfo;			/**[03]**/

/* 
** Defines
*/
#ifdef _DEBUG
	//#define NO_TIMEOUT
#endif

#ifdef NO_TIMEOUT
	#define NCS_CONNECTION_ATTEMP_TIMEOUT_MS	0
#else
	#define NCS_CONNECTION_ATTEMP_TIMEOUT_MS	(20 * 1000)
#endif

#define NCS_CONNECTION_RECEIVE_TIMEOUT_MS	0
#define NCS_DEFAULT_HTTP_PORT				80
#define NCS_PACKET_CHUNK_SIZE				(8 * 1024)
#define NCS_MIN_NR_INET_CONNECTIONS			10000
#define NCS_PROXY_SHOW_MESSAGE_KEY			"IWS Client Network Proxy Message Shown"
#define NCS_POLLING_SHOW_MESSAGE_KEY		"IWS Client Network Streaming Message Shown"
#define NCS_WININET_SHOW_MESSAGE_KEY		"IWS Client Network wininet.dll Version Message Shown"
#define NCS_MESSAGE_BOX_TITLE				"Image Web Server Network v" NCS_VERSION_STRING_NO_NULL
#define NCS_HTTP_AGENT						"NCS/1.0"
#define NCS_WININET_DLL_NAME				"wininet.dll"
#define NCS_WININET_CE_DLL_NAME				"Windows\\wininet.dll"
#define NCS_CLIENTBASEDIR_REG_KEY			"ClientBaseDir"
#define NCS_CLIENT_DIR_NAME					"Program Files\\Earth Resource Mapping\\Image Web Server\\Client"

/*
** Macros
*/
#define NCScnetConnected(pConn, bSendSocket) (bSendSocket ? (pConn->hSendConn != NULL) : (pConn->hRecvConn != NULL))


/*
** Extern globals
*/
extern PCCERT_CONTEXT g_pCertContext;
extern HCERTSTORE g_hCertStore;
extern NCSMutex g_mCertMutex;

extern HINTERNET hInternet;
extern HINSTANCE hWininetDLL;
//extern char *pWininetDLLName;

extern BOOL bUseWinHTTP;

extern INT32 nClientCount;

//extern NCSnetProxyInfo ProxyInfo;

extern BOOL (CALLBACK *pHttpSendRequest)(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength);
extern BOOL (CALLBACK *pHttpAddRequestHeaders)(HINTERNET hConnect, LPCSTR lpszHeaders, DWORD dwHeadersLength, DWORD dwModifiers);  /** [04] **/
extern DWORD (CALLBACK *pInternetErrorDlg)(HWND hWnd, HINTERNET hRequest, DWORD dwError, DWORD dwFlags, LPVOID *lppvData);
extern BOOL (CALLBACK *pInternetSetOption)(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength);
extern HINTERNET (CALLBACK *pHttpOpenRequest)(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR FAR *lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext);
extern HINTERNET (CALLBACK *pInternetConnect)(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext);
extern BOOL (CALLBACK *pHttpQueryInfo)(HINTERNET hRequest, DWORD dwInfoLevel, LPVOID lpBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex);
extern BOOL (CALLBACK *pInternetCloseHandle)(HINTERNET hInternet);
extern BOOL (CALLBACK *pInternetWriteFile)(HINTERNET hFile, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite, LPDWORD lpdwNumberOfBytesWritten);
extern BOOL (CALLBACK *pInternetReadFile)(HINTERNET hFile, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead);
extern BOOL (CALLBACK *pInternetReadFileEx)(IN HINTERNET hFile, OUT LPINTERNET_BUFFERS lpBuffersOut, IN DWORD dwFlags, IN DWORD dwContext);
extern BOOL (CALLBACK *pInternetQueryOption)(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer, LPDWORD lpdwBufferLength);
extern HINTERNET (CALLBACK *pInternetOpen)(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags);

extern NCSMutex mSocketStartup;

/*
** Ptotos
*/

/* NCScnet.c */
NCSError NCScnetInitWininet(); //[08]

/* connect.c */
NCSError NCScnetConnectToServer(NCScnetConnection *pConn, BOOLEAN bSendSocket, void *pData, INT32 nDataLength);
void NCScnetDisconnectFromServer(NCScnetConnection *pConn);
void NCScnetDisconnect(NCScnetConnection *pConn, BOOLEAN bSendSocket);
INT32 NCScnetSendData(NCScnetConnection *pConn, BOOLEAN bSendSocket, void *pData, INT32 nBytesToSend, BOOLEAN *pbCancelSend);
INT32 NCScnetRecvData(NCScnetConnection *pConn, BOOLEAN bSendSocket, void *pData, INT32 nBytesToRead, BOOLEAN *pbCancelRecv);
#ifdef ECWP_POLLING_SUPPORT
NCSError NCScnetPollServer(NCScnetConnection *pConn);
#endif

/* packet.c */
NCSError NCScnetSendRawPacket(NCScnetConnection *pConn, void *pData, INT32 nDataLength, BOOLEAN *pbCancelSend);
NCSError NCScnetRecvRawPacket(NCScnetConnection *pConn, void **ppData, INT32 *pDataLength, BOOLEAN *pbCancelRecv);

/* util.c */
PCCERT_CONTEXT NCSCertificateChooser(HCERTSTORE hCertStore);
void NCScnetGetNetscapeProxyInfo(char *pProxyName, char *pBypassList);
BOOLEAN NCScnetSetID(NCScnetConnection *pConn);
void NCScnetCleanupGlobals(void);
NCScnetHttpSendRequestResult NCScnetHttpSendRequest(NCSThread *tThread, HINTERNET hRequest, LPCTSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength, NCSTimeStampMs tsTimeOut);		/**[06]**/
extern void NCScnetCheckIsPollingServer(NCScnetConnection *pConn);
extern void NCScnetAddPollingServer(NCScnetConnection *pConn);
BOOLEAN NCSRunningInService();

NCSError NCSMessageBoxNoWait( HWND hWnd, char *pText, char *pCaption, UINT uType );

#ifdef WIN32
void NCSWinInetAddClient();
void NCSWinInetRemoveClient();
BOOL NCSWinInetHttpSendRequest(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength);
BOOL NCSWinInetHttpAddRequestHeaders(HINTERNET hConnect, LPCSTR lpszHeaders, DWORD dwHeadersLength, DWORD dwModifiers);  /** [04] **/
DWORD NCSWinInetInternetErrorDlg(HWND hWnd, HINTERNET hRequest, DWORD dwError, DWORD dwFlags, LPVOID *lppvData);
BOOL NCSWinInetInternetSetOption(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength);
HINTERNET NCSWinInetHttpOpenRequest(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR FAR *lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext);
HINTERNET NCSWinInetInternetConnect(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext);
BOOL NCSWinInetHttpQueryInfo(HINTERNET hRequest, DWORD dwInfoLevel, LPVOID lpBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex);
BOOL NCSWinInetInternetCloseHandle(HINTERNET hInternet);
BOOL NCSWinInetInternetWriteFile(HINTERNET hFile, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite, LPDWORD lpdwNumberOfBytesWritten);
BOOL NCSWinInetInternetReadFile(HINTERNET hFile, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead);
BOOL NCSWinInetInternetReadFileEx(IN HINTERNET hFile, OUT LPINTERNET_BUFFERS lpBuffersOut, IN DWORD dwFlags, IN DWORD dwContext);
BOOL NCSWinInetInternetQueryOption(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer, LPDWORD lpdwBufferLength);
HINTERNET NCSWinInetInternetOpen(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags);
#endif //WIN32

#endif /* CNET_H */
