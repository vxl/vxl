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
** FILE:   	NCScnet3\cnetdefs.h
** CREATED:	12-4-2001
** AUTHOR: 	Russell Rogers
** PURPOSE:	
** EDITS:
** [01]
*******************************************************/

#include "NCSBuildNumber.h"

#if defined WIN32 && defined NCS_BUILD_WITH_STDERR_DEBUG_INFO
#define DEBUGPRINT(x) _RPT0(_CRT_WARN, x)
#elif defined NCS_BUILD_WITH_STDERR_DEBUG_INFO
#define DEBUGPRINT(x) fprintf(stderr,x)
#else
#define DEBUGPRINT(x)
#endif // WIN32 && NCS_BUILD_WITH_STDERR_DEBUG_INFO

/* 
** Defines
*/
#define NCS_DEFAULT_HTTP_PORT				80
#define NCS_CONNECTION_ATTEMP_TIMEOUT_MS	(20 * 1000)
#define NCS_CONNECTION_RECEIVE_TIMEOUT_MS	(60 * 60 * 1000)
#define NCS_PACKET_CHUNK_SIZE				(8 * 1024)
#define NCS_MIN_NR_INET_CONNECTIONS			10000
#define NCS_POLLING_SHOW_MESSAGE_KEY		"IWS Client Network Streaming Message Shown"
#define NCS_WININET_SHOW_MESSAGE_KEY		"IWS Client Network wininet.dll Version Message Shown"
#define NCS_HTTP_AGENT						"NCS/1.0"
#define NCS_WININET_DLL_NAME				"wininet.dll"
#define NCS_CLIENTBASEDIR_REG_KEY			"ClientBaseDir"
#define NCS_CLIENT_DIR_NAME					"Program Files\\Earth Resource Mapping\\Image Web Server\\Client"
#define NCS_PROXY_SHOW_MESSAGE_KEY			"IWS Client Network Proxy Message Shown"
#define NCS_MESSAGE_BOX_TITLE				"Image Web Server Network v" NCS_VERSION_STRING_NO_NULL
