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
** FILE:   	NCScnet.h
** CREATED:	Thu Mar 4 09:19:00 WST 1999
** AUTHOR: 	Doug Mansell
** PURPOSE:	IWS client side networking - public interface
** EDITS:
** [01] sjc 01Oct99 Protocol version define
** [02] sjc 25Feb00 Chenged NCS_PROTOCOL_VERSION to 3
** [03] ny  12May00 Added bIsConnected parameter to NCScnetSendPacket
**					to enable the propagation of loss of connection
**					error to the upper level
** [04] jmp 06Mar01 Added generic NCScnetPostURL
** [05] rar 14May01 Added NCSUrlStream.
** [06] rar 17-May-01 Modified NCSnetPacketRecvCB() to include a returned error code.
** [07] rar 28May01 Added sending of ping packet by client,
**					incremented version to 4.
 *******************************************************/

#ifndef NCSCNET_H
#define NCSCNET_H


#include "NCSTypes.h"
#include "NCSErrors.h"

#ifdef NCSCNET3
	#ifdef __cplusplus
	#include "NCSUrlStream.h"
	#endif
#endif

#ifdef WIN32
typedef SIZE_T DWORD_PTR, *PDWORD_PTR;
#include "WinInet.h"
#endif //WIN32


#ifndef NCS_MAX_PACKET_SIZE
#define NCS_MAX_PACKET_SIZE (1024*1024)
#endif
#ifndef NCS_PROTOCOL_VERSION
/*
** Version of the wire protocol.  keep in sync with NCSsnet.h [01]
*/
#define NCS_PROTOCOL_VERSION	4 /**[07]**/
#endif

typedef void *pNCSnetClient;
typedef void NCSnetPacketRecvCB(void *, INT32, pNCSnetClient, void *, NCSError eError);	 /**[06]**/	// pPacket, nLength, pClient, pUserData, eError
typedef void NCSnetPacketSentCB(NCSError, void *, void *);					// eError, pPacket, pUserData


/////

#ifdef __cplusplus
extern "C" {
#endif

BOOLEAN NCScnetVerifyURL(char *szURL);

void NCScnetSetIISDLLName(char *szDLLname);
NCSError NCScnetCreate(char *szURL, void **ppClient, void *pPacketIn, int nLengthIn, void **ppPacketOut, INT32 *pLengthOut, NCSnetPacketRecvCB *pRecvCB, void *pRecvUserdata);
NCSError NCScnetCreateEx(char *szURL, void **ppClient, void *pPacketIn, int nLengthIn, void **ppPacketOut, INT32 *pLengthOut, NCSnetPacketRecvCB *pRecvCB, void *pRecvUserdata, char *pszIISDLLName);
NCSError NCScnetCreateProxy(char *szURL, void **ppClient, void *pPacketIn, int nLengthIn, void **ppPacketOut, INT32 *pLengthOut, NCSnetPacketRecvCB *pRecvCB, void *pRecvUserdata, char *pszIISDLLName, char *pszUsername, char *pszPassword );
void NCScnetSetSendBlocking(void *pConnection, BOOLEAN bOn);
void NCScnetSetRecvCB(pNCSnetClient pClient, NCSnetPacketRecvCB *pRecvCB, void *pUserdata);
void NCScnetSetSentCB(pNCSnetClient pClient, NCSnetPacketSentCB *pSentCB);
BOOLEAN NCScnetSendPacket(pNCSnetClient pClient, void *pPacket, int nLength, void *pUserdata, BOOLEAN *bIsConnected);
//#ifdef NCS_POST_VERSION2
NCSError NCScnetPostURL(char *szURL, char *szBody, char *szHeaders, char **ppAccept,
							   BOOLEAN bIsPost, BOOLEAN bUseCache, UINT8**szResponse, int *nRespLength,
							   UINT32 *pnStatusCode, UINT32 *pnContentLength);		/**[04]**/
//#endif
void NCScnetDestroy(pNCSnetClient pClient);
void NCScnetInit();
void NCScnetShutdown();

void NCScnetSetProxy( char *pProxy, INT32 nPort, char *pProxyBypass );

//
// Function specifically required for use by the IWS Proxy
//
#ifdef PROXYSERVER
void NCSUseWinHTTP( BOOL bValue );
#endif //PROXYSERVER

#ifdef _DEBUG
__declspec(dllexport) void NCScnetSetTimeout(BOOLEAN bTimeout);
#endif

#ifdef __cplusplus
};
#endif

#endif
