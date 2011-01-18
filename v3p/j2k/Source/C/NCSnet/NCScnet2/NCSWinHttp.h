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
** FILE:   	NCSWinHttp.h
** CREATED: 28 Jan 2001
** AUTHOR: 	Mark Sheridan
** PURPOSE:	Wrapper for the WinHttp library. Had to 
**			separate it from the main code because we 
**			cant include both wininet and winhttp headers 
**			in the same code, they clash. The wrappers 
**			also convert between chars and wide chars.
** EDITS:
**
*******************************************************/

#ifndef NCSWINHTTP_H
#define NCSWINHTTP_H

#include <Windows.h>

extern LPVOID NCSWinHttpLoad(BOOL copy);
extern void   NCSWinHttpUnload();
extern BOOL   NCSWinHttpSendRequest(LPVOID hRequest, LPCSTR  pwszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength, DWORD dwTotalLength, DWORD_PTR dwContext);
extern BOOL   NCSWinHttpAddRequestHeaders(LPVOID hRequest, LPCSTR  pwszHeaders, DWORD dwHeadersLength, DWORD dwModifiers);
extern BOOL   NCSWinHttpSetOption(LPVOID hInternet, DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength);
extern LPVOID NCSWinHttpOpenRequest(LPVOID hConnect, LPCSTR  pwszVerb, LPCSTR  pwszObjectName, LPCSTR  pwszVersion, LPCSTR  pwszReferrer, LPCSTR * ppwszAcceptTypes, DWORD dwFlags);
extern LPVOID NCSWinHttpConnect(LPVOID hSession, LPCSTR  pswzServerName, WORD nServerPort, DWORD dwReserved);
extern BOOL   NCSWinHttpQueryHeaders(LPVOID hRequest, DWORD dwInfoLevel, LPCSTR  pwszName, LPVOID lpBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex);
extern BOOL   NCSWinHttpCloseHandle(LPVOID hInternet);
extern BOOL   NCSWinHttpWriteData(LPVOID hRequest, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite, LPDWORD lpdwNumberOfBytesWritten);
extern BOOL   NCSWinHttpReadData(LPVOID hRequest, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead);
extern BOOL   NCSWinHttpQueryOption(LPVOID hInternet, DWORD dwOption, LPVOID lpBuffer, LPDWORD lpdwBufferLength);
extern LPVOID NCSWinHttpOpen(LPCSTR  pwszUserAgent, DWORD dwAccessType, LPCSTR  pwszProxyName, LPCSTR  pwszProxyBypass, DWORD dwFlags);
extern BOOL   NCSWinHttpIsService();
extern BOOL   NCSWinHttpSetMaxConns(LPVOID hInternet, int nConns);
extern BOOL   NCSWinHttpIsConnectionViaProxy(LPVOID hInternet);
extern BOOL   NCSWinHttpGetConnectionStatusCode(LPVOID hInstance, LPDWORD nErrorCode);
extern BOOL   NCSWinHttpGetConnectionStatusText(LPVOID hInstance, LPCSTR * ppwszStatus);

#endif