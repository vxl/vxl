/********************************************************
** Copyright 2001 Earth Resource Mapping Ltd.
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
** FILE:	NCSWinHttp.c
** CREATED: 28 Jan 2001
** AUTHOR:	Mark Sheridan
** PURPOSE: Wrapper for the WinHttp library. Had to 
**			separate it from the main code because we 
**			cant include both wininet and winhttp headers 
**			in the same code, they clash. The wrappers 
**			also convert between chars and wide chars.
** EDITS:
**
** Including winhttp.h requires the large Microsoft SDK under windows (1Gbyte!)
** By eliminating all the net file support the need for the SDK can be
** removed. Of course support for decoding J2K files over the web is also
** disabled. JLM April 11, 2009
*******************************************************/
#define _WIN32_WINNT 0x400



#include "NCSDefs.h"
#include "NCSWinHttp.h"
#include "NCSPrefs.h"
#include "NCSLog.h"
//#if !defined(_WIN32_WCE)
#if 0
	#include "winhttp.h"
#else
	#include <wininet.h>
#endif
#include "NCSDefs.h"

static HINSTANCE hWinHttpDLL = NULL;

BOOL bUseWinHTTP = FALSE;

BOOL	  (CALLBACK *pWinHttpSendRequest)(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength, DWORD dw, DWORD_PTR ptr);
BOOL	  (CALLBACK *pWinHttpAddRequestHeaders)(HINTERNET hConnect, LPCWSTR lpszHeaders, DWORD dwHeadersLength, DWORD dwModifiers);
BOOL	  (CALLBACK *pWinHttpSetOption)(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength);
HINTERNET (CALLBACK *pWinHttpOpenRequest)(HINTERNET hConnect, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR FAR *lplpszAcceptTypes, DWORD dwFlags);
HINTERNET (CALLBACK *pWinHttpConnect)(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwFlags);
BOOL	  (CALLBACK *pWinHttpQueryHeaders)(HINTERNET hRequest, DWORD dwInfoLevel, LPCWSTR  str, LPVOID lpBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex);
BOOL	  (CALLBACK *pWinHttpCloseHandle)(HINTERNET hInternet);
BOOL	  (CALLBACK *pWinHttpWriteData)(HINTERNET hFile, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite, LPDWORD lpdwNumberOfBytesWritten);
BOOL	  (CALLBACK *pWinHttpReadData)(HINTERNET hFile, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead);
BOOL	  (CALLBACK *pWinHttpQueryOption)(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer, LPDWORD lpdwBufferLength);
HINTERNET (CALLBACK *pWinHttpOpen)(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);
BOOL	  (CALLBACK *pWinHttpReceiveResponse)(HINTERNET hInst, LPVOID res);

#define NCS_MAX_WIDE_CHAR_LEN 1024

#define NCSCharToWideChar(src, dest) MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, -1, dest, NCS_MAX_WIDE_CHAR_LEN)
#define NCSWideCharToChar(src, dest) WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, src, -1, dest, NCS_MAX_WIDE_CHAR_LEN, NULL, NULL)

//
// Load the wininet dll. Currently always pass in false, dll copying is not implemented.
//
LPVOID NCSWinHttpLoad(BOOL copy) {
#if 0
	char *pBaseDir = NULL;

	NCSLogSetServer(TRUE); // We should be running in a server, so log it to the server file.

	if (hWinHttpDLL) {
		return hWinHttpDLL;
	}

	// Get a handle to the one in memory (if it is there) if they dont want a copy
	if (copy == FALSE) {
		hWinHttpDLL = GetModuleHandle(NCS_T("winhttp5.dll"));
	}
	else {
		// Copy logic not implemented yet.!!! Not sure if we actually need it.
	}
	// Look for it in the standard paths, it should be in %WINDIR%\SYSTEM32
	if (hWinHttpDLL == NULL) {
		hWinHttpDLL = LoadLibrary(NCS_T("winhttp5.dll"));

		if (hWinHttpDLL == NULL) {
			// Look for the path in the local machine registry under "HKLM\SOFTWARE\Earth Resource Mapping\Image Web Server\Server"
			if( NCSPrefSetMachineKeyLock("HKLM\\SOFTWARE\\Earth Resource Mapping\\Image Web Server\\Server") == NCS_SUCCESS ) {
				if(NCSPrefGetString("ServerConsoleBaseDir", &pBaseDir) == NCS_SUCCESS) {
					char szLibPath[MAX_PATH];
					if (strlen(pBaseDir) + strlen("\\winhttp5.dll\0") + 1 < MAX_PATH) {
						strcpy(szLibPath, pBaseDir);
						strcpy(szLibPath + strlen(pBaseDir), "\\winhttp5.dll\0");
					hWinHttpDLL = LoadLibrary(OS_STRING(szLibPath));
					}
					NCSFree(pBaseDir);
				}
				NCSPrefMachineUnLock();
			}
		}
		// Try the earth resource mapping dir in the program files directory. Dont have client reg access, so just try it here.
		if (hWinHttpDLL == NULL) {
			if (hWinHttpDLL == NULL) {
				hWinHttpDLL = LoadLibrary(NCS_T("%PROGRAMFILES%\\Earth Resource Mapping\\Image Web Server\\Server\\winhttp5.dll"));
			}
		}
	}
	


	// Load the functions we need
	if (hWinHttpDLL != NULL) {
		pWinHttpSendRequest = (BOOL(CALLBACK *)(HINTERNET, LPCWSTR, DWORD, LPVOID, DWORD, DWORD, DWORD_PTR))
											GetProcAddress(hWinHttpDLL, "WinHttpSendRequest");
		pWinHttpAddRequestHeaders = (BOOL(CALLBACK *)(HINTERNET, LPCWSTR, DWORD, DWORD))
											GetProcAddress(hWinHttpDLL, "WinHttpAddRequestHeaders");
		pWinHttpSetOption = (BOOL(CALLBACK *)(HINTERNET, DWORD, LPVOID, DWORD))
											GetProcAddress(hWinHttpDLL, "WinHttpSetOption");
		pWinHttpOpenRequest = (HINTERNET(CALLBACK *)(HINTERNET, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR FAR *, DWORD))
											GetProcAddress(hWinHttpDLL, "WinHttpOpenRequest");
		pWinHttpConnect = (HINTERNET(CALLBACK *)(HINTERNET, LPCWSTR, INTERNET_PORT, DWORD))
											GetProcAddress(hWinHttpDLL, "WinHttpConnect"); 
		pWinHttpQueryHeaders = (BOOL(CALLBACK *)(HINTERNET, DWORD, LPCWSTR, LPVOID, LPDWORD, LPDWORD))
											GetProcAddress(hWinHttpDLL, "WinHttpQueryHeaders"); 
		pWinHttpCloseHandle = (BOOL(CALLBACK *)(HINTERNET))
											GetProcAddress(hWinHttpDLL, "WinHttpCloseHandle"); 
		pWinHttpWriteData = (BOOL(CALLBACK *)(HINTERNET, LPCVOID, DWORD, LPDWORD))
											GetProcAddress(hWinHttpDLL, "WinHttpWriteData"); 
		pWinHttpReadData = (BOOL(CALLBACK *)(HINTERNET, LPVOID, DWORD, LPDWORD))
											GetProcAddress(hWinHttpDLL, "WinHttpReadData"); 
		pWinHttpQueryOption = (BOOL(CALLBACK *)(HINTERNET, DWORD, LPVOID, LPDWORD))
											GetProcAddress(hWinHttpDLL, "WinHttpQueryOption");
		pWinHttpOpen = (HINTERNET(CALLBACK *)(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, DWORD))
											GetProcAddress(hWinHttpDLL, "WinHttpOpen");
		pWinHttpReceiveResponse = (BOOL(CALLBACK *)(HINTERNET, LPVOID))
											GetProcAddress(hWinHttpDLL, "WinHttpReceiveResponse");

		// Make sure the entry points could be located. This should really log an error or something.
		if (pWinHttpSendRequest == NULL ||
			pWinHttpAddRequestHeaders == NULL ||
			pWinHttpSetOption == NULL ||
			pWinHttpOpenRequest == NULL ||
			pWinHttpConnect == NULL ||
			pWinHttpQueryHeaders == NULL ||
			pWinHttpCloseHandle == NULL ||
			pWinHttpWriteData == NULL ||
			pWinHttpReadData == NULL ||
			pWinHttpQueryOption == NULL ||
			pWinHttpOpen == NULL ||
			pWinHttpReceiveResponse == NULL) {
			NCSWinHttpUnload();
			return NULL;
		}

	}
	return hWinHttpDLL;
#endif
  return NULL;
}

//
// Unload the wininet dll.
//
void NCSWinHttpUnload() {
#if 0
	if (hWinHttpDLL) {
		FreeLibrary(hWinHttpDLL);
		hWinHttpDLL = NULL;
		pWinHttpSendRequest = NULL;
		pWinHttpAddRequestHeaders = NULL;
		pWinHttpSetOption = NULL;
		pWinHttpOpenRequest = NULL;
		pWinHttpConnect = NULL;
		pWinHttpQueryHeaders = NULL;
		pWinHttpCloseHandle = NULL;
		pWinHttpWriteData = NULL;
		pWinHttpReadData = NULL;
		pWinHttpQueryOption = NULL;
		pWinHttpOpen = NULL;
		pWinHttpReceiveResponse = NULL;
	}
#endif
}

//
// Get the acces type for this connection. We have to pass in our own enumeration because we cant include winhttp headers directly in the cnet code.
//
static _inline DWORD NCSWinHttpGetAccessType(int aType) {
#if 0
	DWORD type = WINHTTP_ACCESS_TYPE_NO_PROXY;
	switch (aType) {
	case 0:
		type = WINHTTP_ACCESS_TYPE_NO_PROXY;
		break;
	case 1:
	default:
		type = WINHTTP_ACCESS_TYPE_DEFAULT_PROXY;
		break;
	case 2:
		type = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
		break;
	}
	return type;
#endif
  return 0;
}

//
// Send a request to the connection. A wrapper around WinHttpSendRequest/WinHttpReceiveResponse
//
BOOL NCSWinHttpSendRequest(LPVOID hRequest, LPCSTR pwszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength, DWORD dwTotalLength, DWORD_PTR dwContext) {
#if 0
	if (pWinHttpSendRequest) {

		BOOL  bValue   = FALSE;
		WCHAR  szHeaders[NCS_MAX_WIDE_CHAR_LEN];
		unsigned long hType;
		DWORD len = sizeof(hType);
		
		// pWinHttpSendRequest tends to crash when an invalid handle is passed in, so as a hack, try to get the handle type.....
		if (!NCSWinHttpQueryOption((HINTERNET)hRequest, WINHTTP_OPTION_HANDLE_TYPE, (LPVOID) &hType, &len)) {
			//WINHTTP_HANDLE_TYPE_SESSION, WINHTTP_HANDLE_TYPE_CONNECT, WINHTTP_HANDLE_TYPE_REQUEST 
			NCSLog(LOG_LOW, "NCScnet::NCSWinHttpSendRequest() FAILED : HINTERNET handle passed is not a valid handle.");
			return FALSE;
		}
		
		NCSCharToWideChar(pwszHeaders, szHeaders);

		bValue = pWinHttpSendRequest((HINTERNET)hRequest, 
						szHeaders, 
						dwHeadersLength, 
						(lpOptional== NULL ? WINHTTP_NO_REQUEST_DATA : lpOptional), 
						dwOptionalLength, 
						dwTotalLength, 
						dwContext);
		if (bValue) {
			bValue = pWinHttpReceiveResponse(hRequest, NULL);
		}
		return bValue;
	}
	else {
		return FALSE;
	}										
#endif
  return FALSE;
}

//
// NCSWinHttpAddRequestHeaders: wrapper around WinHttpAddRequestHeaders
//
BOOL NCSWinHttpAddRequestHeaders(LPVOID hRequest, LPCSTR pwszHeaders, DWORD dwHeadersLength, DWORD dwModifiers) {
#if 0
	if (pWinHttpAddRequestHeaders) {
		WCHAR  szHeaders[NCS_MAX_WIDE_CHAR_LEN];
		NCSCharToWideChar(pwszHeaders, szHeaders);
		return pWinHttpAddRequestHeaders((HINTERNET)hRequest, szHeaders, dwHeadersLength, dwModifiers);
	}
	else {
		return FALSE;
	}
#endif
  return FALSE;
}

//
// NCSWinHttpSetOption: wrapper around WinHttpSetOption
//
BOOL NCSWinHttpSetOption(LPVOID hInternet, DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength) {
#if 0
	if (pWinHttpSetOption) {
		return pWinHttpSetOption((HINTERNET)hInternet, dwOption, lpBuffer, dwBufferLength);
	}
	else {
		return FALSE;
	}		
#endif
  return FALSE;
}

//
// NCSWinHttpOpenRequest: wrapper around WinHttpOpenRequest
//
LPVOID NCSWinHttpOpenRequest(LPVOID hConnect, LPCSTR pwszVerb, LPCSTR pwszObjectName, LPCSTR pwszVersion, LPCSTR pwszReferrer, LPCSTR* ppwszAcceptTypes, DWORD dwFlags) {
#if 0
	ppwszAcceptTypes;//Keep compiler happy
	if (pWinHttpOpenRequest) {
		WCHAR  szVerb[NCS_MAX_WIDE_CHAR_LEN];
		WCHAR  szObjectName[NCS_MAX_WIDE_CHAR_LEN];
		WCHAR  szVersion[NCS_MAX_WIDE_CHAR_LEN];
		WCHAR  szFeferrer[NCS_MAX_WIDE_CHAR_LEN];

		NCSCharToWideChar(pwszVerb, szVerb);
		NCSCharToWideChar(pwszObjectName, szObjectName);
		NCSCharToWideChar(pwszVersion, szVersion);
		NCSCharToWideChar(pwszReferrer, szFeferrer);

		return pWinHttpOpenRequest((HINTERNET)hConnect, 
					szVerb, 
					szObjectName, 
					szVersion, 
					pwszReferrer == NULL ? WINHTTP_NO_REFERER : szFeferrer, 
					WINHTTP_DEFAULT_ACCEPT_TYPES, 
					dwFlags == 0 ? WINHTTP_FLAG_REFRESH : dwFlags);
	}
	else {
		return NULL;
	}
#endif
  return NULL;
}

//
// NCSWinHttpConnect: wrapper around WinHttpConnect
//
LPVOID NCSWinHttpConnect(LPVOID hSession, LPCSTR pswzServerName, WORD nServerPort, DWORD dwReserved) {
#if 0
	if (pWinHttpConnect) {
		WCHAR  szServerName[NCS_MAX_WIDE_CHAR_LEN];
		NCSCharToWideChar(pswzServerName, szServerName);

		return pWinHttpConnect((HINTERNET)hSession, szServerName, (INTERNET_PORT)nServerPort, dwReserved);
	} else {
		return NULL;
	}
#endif
  return NULL;
}

//
// NCSWinHttpQueryHeaders: wrapper around WinHttpQueryHeaders
//
BOOL NCSWinHttpQueryHeaders(LPVOID hRequest, DWORD dwInfoLevel, LPCSTR pwszName, LPVOID lpBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex) {
#if 0
	if (pWinHttpQueryHeaders) {
		WCHAR  szName[NCS_MAX_WIDE_CHAR_LEN];
		NCSCharToWideChar(pwszName, szName);

		return pWinHttpQueryHeaders((HINTERNET)hRequest, dwInfoLevel, pwszName ? szName : WINHTTP_HEADER_NAME_BY_INDEX, lpBuffer, lpdwBufferLength, lpdwIndex);
	}
	else {
		return FALSE;
	}
#endif
  return FALSE;
}

//
// NCSWinHttpCloseHandle: wrapper around WinHttpCloseHandle
//
BOOL NCSWinHttpCloseHandle(LPVOID hInternet) {
#if 0
	if (pWinHttpCloseHandle) {
		return pWinHttpCloseHandle((HINTERNET)hInternet);
	} else {
		return FALSE;
	}
#endif
  return FALSE;
}

//
// NCSWinHttpWriteData: wrapper around WinHttpWriteData
//
BOOL NCSWinHttpWriteData(LPVOID hRequest, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite, LPDWORD lpdwNumberOfBytesWritten) {
#if 0
	if (pWinHttpWriteData) {
		return pWinHttpWriteData((HINTERNET)hRequest, lpBuffer, dwNumberOfBytesToWrite, lpdwNumberOfBytesWritten);
	}
	else {
		return FALSE;
	}
#endif
  return FALSE;
}

//
// NCSWinHttpReadData: wrapper around WinHttpReadData
//
BOOL NCSWinHttpReadData(LPVOID hRequest, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead) {
#if 0
	if (pWinHttpReadData) {
		return pWinHttpReadData((HINTERNET)hRequest, lpBuffer, dwNumberOfBytesToRead, lpdwNumberOfBytesRead);
	}
	else {
		return FALSE;
	}
#endif
  return FALSE;
}

//
// NCSWinHttpQueryOption: wrapper around WinHttpQueryOption
//
BOOL NCSWinHttpQueryOption(LPVOID hInternet, DWORD dwOption, LPVOID lpBuffer, LPDWORD lpdwBufferLength) {
#if 0
	if (pWinHttpQueryOption) {
		return pWinHttpQueryOption((HINTERNET)hInternet, dwOption, lpBuffer, lpdwBufferLength);
	}
	else {
		return FALSE;
	}
#endif
  return FALSE;
}

//
// NCSWinHttpOpen: wrapper around WinHttpOpen
//
LPVOID NCSWinHttpOpen(LPCSTR pUserAgent, DWORD dwAccessType, LPCSTR pwszProxyName, LPCSTR pwszProxyBypass, DWORD dwFlags) {
#if 0	
	if (pWinHttpOpen) {

		LPVOID nResult = NULL;
		DWORD dwType = NCSWinHttpGetAccessType(dwAccessType);
		WCHAR  wszUserAgent[NCS_MAX_WIDE_CHAR_LEN];
		WCHAR  wszProxyName[NCS_MAX_WIDE_CHAR_LEN];
		WCHAR  wszProxyBypass[NCS_MAX_WIDE_CHAR_LEN];

		NCSCharToWideChar(pUserAgent, wszUserAgent);
		NCSCharToWideChar(pwszProxyName, wszProxyName);
		NCSCharToWideChar(pwszProxyBypass, wszProxyBypass);

		nResult = pWinHttpOpen(wszUserAgent,
							dwType,
							pwszProxyName != NULL ? wszProxyName : WINHTTP_NO_PROXY_NAME,
							pwszProxyBypass != NULL ? wszProxyBypass : WINHTTP_NO_PROXY_BYPASS,
							dwFlags);

			if (nResult == NULL) {
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
					NULL);
				NCSLog(LOG_LOW, "NCScnet::NCSWinHttpOpen() FAILED : %s", (LPCTSTR)lpMsgBuf);
				LocalFree( lpMsgBuf );
			}
		return nResult;
	}
	else {
		return NULL;
	}
#endif
  return FALSE;
}

//
// NCSWinHttpIsService: determines if this process is running inside IIS, if so check the reg key.
//
BOOL NCSWinHttpIsService(){
#if 0	
	NCSTChar pName[MAX_PATH];
	GetModuleFileName(NULL, pName, MAX_PATH);
	NCSTLwr(pName);
	if(NCSIsService() || (GetModuleFileName(NULL, pName, MAX_PATH) && NCSTLwr(pName) &&
		(NCSTStr(pName, NCS_T("inetinfo.exe")) || NCSTStr(pName, NCS_T("dllhost.exe")) || NCSTStr(pName, NCS_T("mtx.exe"))))) {

		BOOLEAN bUseWinhttpDll = FALSE;
		if( (NCSPrefGetBoolean("ECWImage Use Winhttp", &bUseWinhttpDll) == NCS_SUCCESS) ){
			NCSPrefMachineUnLock();
			return (BOOL)bUseWinhttpDll;
		}
		return (BOOL)bUseWinHTTP;
	}
	return (BOOL)bUseWinHTTP;
#endif
  return FALSE;
}

//
// NCSWinHttpSetMaxConns: Sets the maximum connections per server.
//
BOOL NCSWinHttpSetMaxConns(LPVOID hInternet, int nConns) {
#if 0
	BOOL a = NCSWinHttpSetOption(hInternet, WINHTTP_OPTION_MAX_CONNS_PER_SERVER, (LPVOID)&nConns, sizeof(nConns));
	BOOL b = NCSWinHttpSetOption(hInternet, WINHTTP_OPTION_MAX_CONNS_PER_1_0_SERVER, (LPVOID)&nConns, sizeof(nConns));
	return a && b;
#endif
  return FALSE;
}

//
// NCSWinHttpIsConnectionViaProxy: determines if this connection is via a proxy
//
BOOL NCSWinHttpIsConnectionViaProxy(LPVOID hInternet) {
#if 0
	BOOLEAN bIsConnIsViaProxy = FALSE;
	DWORD nLen = sizeof(WINHTTP_PROXY_INFO);
	WINHTTP_PROXY_INFO *pProxyInfo = (WINHTTP_PROXY_INFO *)NULL;
	
	// Get the size of the structure it will return by passing in a null pointer.
	NCSWinHttpQueryOption((HINTERNET)hInternet, WINHTTP_OPTION_PROXY,  (void *)NULL, &nLen);
	pProxyInfo = NCSMalloc(nLen, TRUE);

	// Now get the actual structure.
	if (NCSWinHttpQueryOption((HINTERNET)hInternet, WINHTTP_OPTION_PROXY,  pProxyInfo, &nLen)) {
		if (pProxyInfo->dwAccessType == WINHTTP_ACCESS_TYPE_NAMED_PROXY) {
			bIsConnIsViaProxy = TRUE;
		}
		else {
			bIsConnIsViaProxy = FALSE;
		}
	}
	if (pProxyInfo) NCSFree(pProxyInfo);
	return bIsConnIsViaProxy;
#endif
  return FALSE;
}

//
// NCSWinHttpGetConnectionStatusCode: gets the status code on the connection
//
BOOL NCSWinHttpGetConnectionStatusCode(LPVOID hInstance, LPDWORD nErrorNum) {
#if 0
	DWORD dwIndex = 0;
	DWORD dwLen = sizeof(DWORD);
	return NCSWinHttpQueryHeaders((HINTERNET)hInstance, 
				WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,
				WINHTTP_HEADER_NAME_BY_INDEX,
				nErrorNum,
				&dwLen,
				&dwIndex);
#endif
  return FALSE;
}

//
// NCSWinHttpGetConnectionStatusText: gets the status text on the connection
//
BOOL NCSWinHttpGetConnectionStatusText(LPVOID hInstance, LPCSTR* ppwszStatus){
#if 0
	LPVOID lpBuffer = NULL;
	DWORD lpdwBufferLength;
	DWORD dwIndex;
	BOOL  bValue = FALSE;
	DWORD dwLen = 0;

	// Get the length of the text buffer, by passing a null buffer
	NCSWinHttpQueryHeaders((HINTERNET)hInstance, 
		WINHTTP_QUERY_STATUS_TEXT,
		WINHTTP_HEADER_NAME_BY_INDEX,
		NULL,
		&lpdwBufferLength,
		&dwIndex);

	lpBuffer = NCSMalloc(lpdwBufferLength, TRUE);

	// Get the actual buffer
	bValue = NCSWinHttpQueryHeaders((HINTERNET)hInstance, 
				WINHTTP_QUERY_STATUS_TEXT,
				WINHTTP_HEADER_NAME_BY_INDEX,
				lpBuffer,
				&dwLen,
				&dwIndex);

	ppwszStatus = lpBuffer;
	return bValue;
#endif
  return FALSE;
}

