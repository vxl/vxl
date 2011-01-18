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
** FILE:   	NCSPackets.h
** CREATED:	Wed Mar 3 09:19:00 WST 1999
** AUTHOR: 	Simon Cope
** PURPOSE:	NCS Packet Structure header.
** EDITS:
 *******************************************************/

#ifndef NCSPACKETS_H
#define NCSPACKETS_H

#ifndef NCSTYPES_H
#include "NCSTypes.h"
#endif

#ifdef MACINTOSH
#include "NCSMalloc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef UINT8 NCSBlock;
typedef UINT8 NCSFileHeader;

/*
** Increment this if the packet structure changes.
*/
#define NCS_PACKET_VERSION	1

typedef enum {
	NCSPT_CONNECTION_OPEN	= 0,
	NCSPT_CONNECTION_CLOSE,
	NCSPT_CONNECTION_DATA,
	NCSPT_CONNECTION_OPEN_INFO,
	NCSPT_BLOCKS,
	NCSPT_FLOW_CONTROL,
	NCSPT_SYNCHRONISE,
	NCSPT_CONNECTION_OPEN_FAIL
} NCSPacketType;



typedef enum {
	NCSRT_SINGLE_BLOCKS		= 0,		/* Individual block IDS */
	NCSRT_RANGE_BLOCKS		= 1,		/* Paired block IDS, each pair specify a range */
	NCSRT_RECT_BLOCKS		= 2			/* Paired block IDS, each pair specify TL/BR   */
										/* (inclusive) block rectangle */
} NCSConnectionDataRequestType;

typedef struct {
	NCSSequenceNr					nSequence;
	NCSConnectionDataRequestType	cdrtType;
	
	UINT16							nCancelBlocks;
	NCSBlockId						bidCancelBlocks[1];

	UINT16							nRequestBlocks;
	NCSBlockId						bidRequestBlocks[1];
} NCSConnectionDataPacket;

typedef struct {
	UINT8					nClientVersion;		/* MUST be first element of Open packet */
	char					szOpenFileName[1];
} NCSConnectionOpenPacket;

typedef struct {
	UINT8			nServerVersion;				/* MUST be first element of OpenInfo packet */
	UINT16			iHeaderSize;
	NCSFileHeader	Header[1];
} NCSConnectionOpenInfoPacket;


typedef struct {
	NCSBlockId				bidBlock;
	UINT32					nBlockLength;
	NCSBlock				blkBlock[1];
} NCSBlockPacketData;

typedef struct {
	UINT16					nBlocks;
	NCSBlockPacketData		bdData[1];
} NCSBlockPacket;


typedef enum {
	NCSFT_FASTER			= 0,
	NCSFT_SLOWER
} NCSBlockFlowType;


typedef enum {
	NCSOF_SERVER_FAILURE = 0,				/* generic/unknown/catastrophic failure */
	NCSOF_FILE_NOT_FOUND,
	NCSOF_FILE_LIMIT_REACHED,
	NCSOF_FILE_SIZE_LIMIT_REACHED,
	NCSOF_FILE_CLIENT_LIMIT_REACHED,
	NCSOF_INCOMPATIBLE_PROTOCOL_VERSION,
	/* modify enum only by inserting values here (because packets can move between different builds) */
	NCSOF_END_MARKER
} NCSConnectionOpenFailType;

typedef struct {
	UINT8	nFailType;						/* NCSConnectionOpenFailType */
	UINT32	nLimit;							/* meaning varies depending on nFailType */
	char	szError[1];						/* optional error message */
} NCSConnectionOpenFailPacket;

/*
** NCSPacket
**
** Basic NCS Packet structure
**
*/
typedef struct {
		/* these 3 elements MUST not change */
	UINT32					iPacketSize;			/* Total Size of this packet								*/
	NCSClientUID			cuidClient;				/* Server generated UID of this client (0 on Open Packet)	*/
	NCSPacketType			ptType;					/* Type of packet											*/
	
		/* this union MUST be next */
	union {
		NCSConnectionOpenPacket		coOpen;
		NCSConnectionOpenInfoPacket	coiOpenInfo;
		NCSConnectionDataPacket		cdpData;
		NCSBlockPacket				bpBlocks;
		NCSBlockFlowType			bftFlow;
		NCSSequenceNr				nLastSequence;
		NCSConnectionOpenFailPacket coFail;
	} u;

} NCSPacket;

#define NCS_PACKET_BASE_SIZE			(sizeof(UINT32) + sizeof(NCSClientUID) + sizeof(NCSPacketType))
#define NCS_DATA_PACKET_BASE_SIZE		(NCS_PACKET_BASE_SIZE + sizeof(NCSSequenceNr) + sizeof(NCSConnectionDataRequestType) + sizeof(UINT16) + sizeof(UINT16))
#define NCS_BLOCK_PACKET_DATA_BASE_SIZE (sizeof(NCSBlockId) + sizeof(UINT32))
#define NCS_BLOCK_PACKET_BASE_SIZE		(NCS_PACKET_BASE_SIZE + sizeof(UINT16))
#define NCS_OPEN_PACKET_BASE_SIZE		(NCS_PACKET_BASE_SIZE + sizeof(UINT8))
#define NCS_OPENINFO_PACKET_BASE_SIZE	(NCS_PACKET_BASE_SIZE + sizeof(UINT8) + sizeof(UINT16))
#define NCS_OPENFAIL_PACKET_BASE_SIZE	(NCS_PACKET_BASE_SIZE + sizeof(UINT8) + sizeof(UINT32))

#ifndef NCS_MAX_PACKET_SIZE
#define NCS_MAX_PACKET_SIZE				(512 * 1024)
#endif
#define NCS_PREFERED_DATA_PACKET_SIZE	(16 * 1024)

#ifdef NCSBO_MSBFIRST

#define NCS_PACKET_PACK_BEGIN(v)				{ UINT8 *pLocalData = (UINT8*)(v)

#define NCS_PACKET_PACK_ELEMENT(v)				if(sizeof(v) == sizeof(INT64)) NCS_PACKET_PACK_ELEMENT64(v) else if(sizeof(v) == sizeof(INT32)) NCS_PACKET_PACK_ELEMENT32(v) else if(sizeof(v) == sizeof(INT16)) NCS_PACKET_PACK_ELEMENT16(v) else NCS_PACKET_PACK_ELEMENT8(v)
#define NCS_PACKET_PACK_ELEMENT64(v)			{ INT64 n=0; n=NCSByteSwap64(v); memcpy(pLocalData, &(n), sizeof(v)); pLocalData += sizeof(v); }
#define NCS_PACKET_PACK_ELEMENT32(v)			{ INT32 n=0; n=NCSByteSwap32(v); memcpy(pLocalData, &(n), sizeof(v)); pLocalData += sizeof(v); }
#define NCS_PACKET_PACK_ELEMENT16(v)			{ INT16 n=0; n=NCSByteSwap16(v); memcpy(pLocalData, &(n), sizeof(v)); pLocalData += sizeof(v); }
#define NCS_PACKET_PACK_ELEMENT8(v)				{ memcpy(pLocalData, &(v), sizeof(v)); pLocalData += sizeof(v); }

#define NCS_PACKET_PACK_ELEMENT_SIZE(v, s)		memcpy(pLocalData, &(v), (s)); pLocalData += (s)
#define NCS_PACKET_PACK_END(v)					}

#define NCS_PACKET_UNPACK_BEGIN(v)				{ UINT8 *pLocalData = (UINT8*)(v)

#define NCS_PACKET_UNPACK_ELEMENT(v)			if(sizeof(v) == sizeof(INT64)) NCS_PACKET_UNPACK_ELEMENT64(v) else if(sizeof(v) == sizeof(INT32)) NCS_PACKET_UNPACK_ELEMENT32(v) else if(sizeof(v) == sizeof(INT16)) NCS_PACKET_UNPACK_ELEMENT16(v) else NCS_PACKET_UNPACK_ELEMENT8(v)
#define NCS_PACKET_UNPACK_ELEMENT64(v)			{ INT64 n=0; memcpy(&(n), pLocalData, sizeof(v)); n=NCSByteSwap64(n); memcpy(&(v), &(n), sizeof(v)); pLocalData += sizeof(v); }
#define NCS_PACKET_UNPACK_ELEMENT32(v)			{ INT32 n=0; memcpy(&(n), pLocalData, sizeof(v)); n=NCSByteSwap32(n); memcpy(&(v), &(n), sizeof(v)); pLocalData += sizeof(v); }
#define NCS_PACKET_UNPACK_ELEMENT16(v)			{ INT16 n=0; memcpy(&(n), pLocalData, sizeof(v)); n=NCSByteSwap16(n); memcpy(&(v), &(n), sizeof(v)); pLocalData += sizeof(v); }
#define NCS_PACKET_UNPACK_ELEMENT8(v)				{ memcpy(&(v), pLocalData, sizeof(v)); pLocalData += sizeof(v); }

#define NCS_PACKET_UNPACK_ELEMENT_SIZE(v, s)	memcpy(&(v), pLocalData, (s)); pLocalData += (s)
#define NCS_PACKET_UNPACK_END(v)				}

#else

#define NCS_PACKET_PACK_BEGIN(v)				{ UINT8 *pLocalData = (UINT8*)(v)
#define NCS_PACKET_PACK_ELEMENT(v)				memcpy(pLocalData, &(v), sizeof(v)); pLocalData += sizeof(v)
#define NCS_PACKET_PACK_ELEMENT_SIZE(v, s)		memcpy(pLocalData, &(v), (s)); pLocalData += (s)
#define NCS_PACKET_PACK_END(v)					}

#define NCS_PACKET_UNPACK_BEGIN(v)				{ UINT8 *pLocalData = (UINT8*)(v)
#define NCS_PACKET_UNPACK_ELEMENT(v)			memcpy(&(v), pLocalData, sizeof(v)); pLocalData += sizeof(v)
#define NCS_PACKET_UNPACK_ELEMENT_SIZE(v, s)	memcpy(&(v), pLocalData, (s)); pLocalData += (s)
#define NCS_PACKET_UNPACK_END(v)				}

#endif // NCSBO_MSBFIRST

#ifdef __cplusplus
}
#endif

#endif /* NCSPACKETS_H */
