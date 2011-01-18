/********************************************************
** Copyright 2002 Earth Resource Mapping Ltd.
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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCEcwpIOStream.cpp $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCEcwpIOStream class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCEcwpIOStream.h"
#include "NCSEcw.h"
#include "NCSJPC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCEcwpIOStream::CNCSJPCEcwpIOStream(CNCSJPC *pJPC, bool bSeekable)
{
	m_pJPC = pJPC;
	// Initialise the members
	m_ClientNetID = NULL;
	m_ClientUID = 0;

	m_bSendInProgress = false;
	m_nClientSequence = 0;
	m_pSendPacket = NULL;
	m_bIsConnected = false;

	m_pMemory = (void*)NULL;
	m_nMemoryLength = 0;
	m_bSeekable = bSeekable;


}

// Destructor
CNCSJPCEcwpIOStream::~CNCSJPCEcwpIOStream()
{
	// Close the file, Close() will clean up.
	Close();
}

void CNCSJPCEcwpIOStream::Lock(bool bLockJPC)
{
	if(bLockJPC) {
		CNCSJPCGlobalLock _Lock;
		_Lock.Lock();
	}
	CNCSMutex::Lock();
}

void CNCSJPCEcwpIOStream::UnLock(bool bUnLockJPC)
{
	if(bUnLockJPC) {
		CNCSJPCGlobalLock _Lock;
		_Lock.UnLock();
	}
	CNCSMutex::UnLock();
}

// Open the stream on the specified memory.
CNCSError CNCSJPCEcwpIOStream::Open(char *pEcwpURL)
{
	Lock();
	Close();
	*(CNCSError*)this = NCScbmNetFileOpenInternal((UINT8**)&m_pMemory, 
													&m_nMemoryLength, 
													&m_ClientNetID, 
													&m_ClientUID,
													(NCSnetPacketSentCB*)PacketSentCB, 
													(NCSnetPacketRecvCB*)PacketRecvCB, 
													(void*)this, 
													pEcwpURL);

	if(GetErrorNumber() == NCS_SUCCESS) {
		*(CNCSError*)this = CNCSJPCIOStream::Open(pEcwpURL, false);
		m_bIsConnected = true;
		if(!m_pSendPacket) {
			m_pSendPacket = (NCSPacket*)NCSMalloc( NCSECW_MAX_SEND_PACKET_SIZE + 1 , FALSE);
		}
	}
	UnLock();
	return(*(CNCSError*)this);
}

// Close the stream, cleans up.
CNCSError CNCSJPCEcwpIOStream::Close()
{
	Lock();
	NCScnetDestroy(m_ClientNetID);
	m_ClientNetID = NULL;
	m_ClientUID = 0;

	NCSFree((void*)m_pMemory);
	m_pMemory = (void*)NULL;
	m_nMemoryLength = 0;
	*(CNCSError*)this = CNCSJPCMemoryIOStream::Close();
	NCSFree((void*)m_pSendPacket);
	m_pSendPacket = (NCSPacket*)NULL;

	while(m_ReceivedPackets.size()) {
		ReceivedPacket *pRP = *(m_ReceivedPackets.begin());
		m_ReceivedPackets.remove(pRP);
		NCSFree(pRP->pPacket);
		NCSFree(pRP);
	}
	UnLock();
	return(*(CNCSError*)this);
}

bool CNCSJPCEcwpIOStream::IsPacketStream()
{
	return(true);
}

void CNCSJPCEcwpIOStream::RequestPrecinct(CNCSJPCPrecinct *pPrecinct)
{
	Lock();
	std::vector<CNCSJPCPrecinct*>::iterator pCur = m_RequestPrecincts.begin();
	std::vector<CNCSJPCPrecinct*>::iterator pEnd = m_RequestPrecincts.end();

	while(pCur != pEnd) {
		if(*pCur == pPrecinct) {
			// Already in request list
			UnLock();
			return;
		}	
		pCur++;
	}
	pCur = m_CancelPrecincts.begin();
	pEnd = m_CancelPrecincts.end();
	while(pCur != pEnd) {
		if(*pCur == pPrecinct) {
			// Remove from cancel list
			m_CancelPrecincts.erase(pCur);
			break;
		}	
		pCur++;
	}
	// Add to request list
	m_RequestPrecincts.push_back(pPrecinct);
	UnLock();
}

void CNCSJPCEcwpIOStream::CancelPrecinct(CNCSJPCPrecinct *pPrecinct)
{
	Lock();
	std::vector<CNCSJPCPrecinct*>::iterator pCur = m_RequestPrecincts.begin();
	std::vector<CNCSJPCPrecinct*>::iterator pEnd = m_RequestPrecincts.end();

	while(pCur != pEnd) {
		if(*pCur == pPrecinct) {
			// Remove from request list
			m_RequestPrecincts.erase(pCur);
			break;
		}	
		pCur++;
	}
	pCur = m_CancelPrecincts.begin();
	pEnd = m_CancelPrecincts.end();
	while(pCur != pEnd) {
		if(*pCur == pPrecinct) {
			// Already in cancel list
			UnLock();
			return;
		}	
		pCur++;
	}
	m_CancelPrecincts.push_back(pPrecinct);
	UnLock();
}

CNCSError CNCSJPCEcwpIOStream::SendRequests()
{
	Lock();
	if(m_bSendInProgress == false) {
		PacketSentCB(NCS_SUCCESS, (UINT8*)NULL, this);
	}
	UnLock();
	return(NCS_SUCCESS);
}

void CNCSJPCEcwpIOStream::ProcessReceivedPackets()
{
	Lock();
	while(m_ReceivedPackets.size()) {
		ReceivedPacket *pRP = *(m_ReceivedPackets.begin());
		NCSPacket *pPacket = pRP->pPacket;
		INT32 nLength = pRP->nLength;

		m_ReceivedPackets.remove(pRP);
		NCSFree(pRP);

		if( pPacket ) {
			UINT32		iPacketSize;
			NCSPacketType ptType;
			NCSClientUID	nClientUID;

			// have a valid file, so now unpack the blocks read
			NCS_PACKET_UNPACK_BEGIN(pPacket);
			NCS_PACKET_UNPACK_ELEMENT(iPacketSize);
			NCS_PACKET_UNPACK_ELEMENT(nClientUID);
			NCS_PACKET_UNPACK_ELEMENT(ptType);

			switch( ptType ) {
				case NCSPT_BLOCKS: 
					{
						UINT16 nBlocks;
						UINT32 i;

						NCS_PACKET_UNPACK_ELEMENT(nBlocks);
						/*
						** Unpack block data
						*/
						for(i = 0; i < nBlocks; i++) {
							NCSBlockId nBlock;
							UINT32 nBlockLength;
							UINT8 *pImage;
							NCS_PACKET_UNPACK_ELEMENT(nBlock);
							NCS_PACKET_UNPACK_ELEMENT(nBlockLength);
							pImage = (UINT8*)NCSMalloc(nBlockLength, FALSE);
							NCS_PACKET_UNPACK_ELEMENT_SIZE(pImage[0], nBlockLength);
//char buf[1024];
//sprintf(buf, "Packet %ld size %ld (", nBlock, nBlockLength);
//for(int v = 0; v < NCSMin(20, nBlockLength); v++) {
//	sprintf(buf + strlen(buf), "0x%lx,", pImage[v]);
//}
//sprintf(buf + strlen(buf), ")\r\n");
//OutputDebugStringA(buf);
	//						m_pJPC->Lock();
							if(GetPacketStatus(nBlock) == CNCSJPCPacketStatus::REQUESTED) {
								SetPacketStatus(nBlock, CNCSJPCPacketStatus::RECEIVED);
								CNCSJPCProgression p;
								p.m_nCurPacket = nBlock;
								CNCSJPCPacket *pHeader = m_pJPC->GetPacketHeader(nBlock);

								if(pHeader && m_pJPC->FindPacketRCPL(nBlock, p.m_nCurTile, p.m_nCurResolution, p.m_nCurComponent, p.m_nCurPrecinctX, p.m_nCurPrecinctY, p.m_nCurLayer)) {
									CNCSJPCEcwpIOStream Stream(m_pJPC, true);
									if(((CNCSJPCMemoryIOStream&)Stream).Open(pImage, nBlockLength) == NCS_SUCCESS) {
										pImage = NULL;
										pHeader->ParseHeader(*m_pJPC, Stream, &p);
										Stream.Close();
									}
								}
								delete pHeader;
							}
	//						m_pJPC->UnLock();
							NCSFree(pImage);
						}
					}
					break;

				case NCSPT_FLOW_CONTROL:
						// NOTUSED
					break;

				case NCSPT_SYNCHRONISE: /* [03] */
						{
	//						m_pJPC->Lock();
							if(m_bSendInProgress == false) {
								CNCSJPCPacketStatusIterator pCur = m_Packets.begin();
								CNCSJPCPacketStatusIterator pEnd = m_Packets.end();

								while(pCur != pEnd) {
									UINT32 nBlock = (*pCur).second.m_nPacket; 
									if((*pCur).second.m_eStatus == CNCSJPCPacketStatus::REQUESTED) {
										SetPacketStatus(nBlock, CNCSJPCPacketStatus::NONE);
										pCur = m_Packets.begin();
										CNCSJPCProgression p;
										p.m_nCurPacket = nBlock;
										if(m_pJPC->FindPacketRCPL(nBlock, p.m_nCurTile, p.m_nCurResolution, p.m_nCurComponent, p.m_nCurPrecinctX, p.m_nCurPrecinctY, p.m_nCurLayer)) {
											CNCSJPCTilePartHeader *pMainTP = m_pJPC->GetTile(p.m_nCurTile);
											RequestPrecinct((CNCSJPCPrecinct*)pMainTP->m_Components[p.m_nCurComponent]->m_Resolutions[p.m_nCurResolution]->m_Precincts.find(p.m_nCurPrecinctX, p.m_nCurPrecinctY));
										}
									}
									pCur++;
								}
							}
							SendRequests();
		//					m_pJPC->UnLock();
						}
					break;

				default:
						// ERROR unknown packet - ignore it
					break;
			}

			NCS_PACKET_UNPACK_END(pPacket);
			NCSFree(pPacket);
		}
	}
	UnLock();
}

void CNCSJPCEcwpIOStream::PacketRecvCB(NCSPacket *pPacket, 
									   INT32 nLength, 
									   pNCSnetClient pClientNetID, 
									   CNCSJPCEcwpIOStream *pStream, 
									   NCSError eError)
{
	// Check returned error code
	if( eError != NCS_SUCCESS ) {		/**[11]**/
		pStream->Lock();
		pStream->m_bIsConnected = false;
		pStream->UnLock();
		return;
	}

	pStream->Lock(false);

	ReceivedPacket *pRP = (ReceivedPacket*)NCSMalloc(sizeof(ReceivedPacket), FALSE);
	if(pRP) {
		pRP->pPacket = pPacket;
		pRP->nLength = nLength;

		pStream->m_ReceivedPackets.push_back(pRP);
	}
	pStream->UnLock(false);
}

void CNCSJPCEcwpIOStream::PacketSentCB(NCSError eError, UINT8 *pLastPacketSent, CNCSJPCEcwpIOStream *pStream)
{
	UINT32		iPacketSize;
	NCSPacketType ptType = NCSPT_CONNECTION_DATA;
	UINT32		iPacketSpaceLeft = NCSECW_MAX_SEND_PACKET_SIZE - NCS_DATA_PACKET_BASE_SIZE;
	UINT16		*pCancelCount, *pRequestCount;
	UINT16		nCancelCount, nRequestCount;
	NCSPacket	*pPacket;
	NCSConnectionDataRequestType	ptBlockRange = NCSRT_SINGLE_BLOCKS;

//	pJPC->Lock();
//		CNCSJPCEcwpIOStream *pStream = (CNCSJPCEcwpIOStream*)pJPC->m_pStream;
	pStream->Lock();
	CNCSJPC *pJPC = pStream->m_pJPC;
//	pJPC->Lock();
	pStream->m_bSendInProgress = false;

	if( pStream->m_RequestPrecincts.size() == 0 && pStream->m_CancelPrecincts.size() == 0) {
//		pJPC->UnLock();
		pStream->UnLock();
		return;				// nothing more to send for the file
	}

	pPacket = (NCSPacket*)pStream->m_pSendPacket;

	// Now construct the packet. Insert fake values for Packet Size, # of Cancels and # of requests
	// and then come back and update them before sending the packet

	iPacketSize = 0;			// Inserted after packet is constructed
	nCancelCount = 0;			// Inserted after packet is constructed
	nRequestCount = 0;			// Inserted after packet is constructed


	NCS_PACKET_PACK_BEGIN(pPacket);
	NCS_PACKET_PACK_ELEMENT(iPacketSize);
	NCS_PACKET_PACK_ELEMENT(pStream->m_ClientUID);
	NCS_PACKET_PACK_ELEMENT(ptType);
	NCS_PACKET_PACK_ELEMENT(pStream->m_nClientSequence);
	NCS_PACKET_PACK_ELEMENT(ptBlockRange);

	// Point to Cancel Count location, and insert fake value for now
	pCancelCount = (UINT16 *) pLocalData;
	NCS_PACKET_PACK_ELEMENT(nCancelCount);
	// Insert Cancels into the packet
	
	while(pStream->m_CancelPrecincts.size() > 0 && iPacketSpaceLeft >= pStream->m_CancelPrecincts[0]->m_Packets.size() * sizeof(NCSBlockId)) {
		for(UINT32 l = 0; l < pStream->m_CancelPrecincts[0]->m_Packets.size(); l++) {
			NCSJPCPacketId nPacket = pStream->m_CancelPrecincts[0]->m_Packets[l];
			if(pStream->GetPacketStatus(nPacket) == CNCSJPCPacketStatus::REQUESTED) {
				NCS_PACKET_PACK_ELEMENT(nPacket);
				iPacketSpaceLeft -= sizeof(NCSBlockId);
				nCancelCount++;
				pStream->SetPacketStatus(nPacket, CNCSJPCPacketStatus::NONE);
			}
		}
		pStream->m_CancelPrecincts.erase(pStream->m_CancelPrecincts.begin());
	}

	// Point to Request Count location, and insert fake value for now
	pRequestCount = (UINT16 *) pLocalData;
	NCS_PACKET_PACK_ELEMENT(nRequestCount);
	// Insert Requests into the packet
	
	while(pStream->m_RequestPrecincts.size() > 0 && iPacketSpaceLeft >= pStream->m_RequestPrecincts[0]->m_Packets.size() * sizeof(NCSBlockId)) {
		for(UINT32 l = 0; l < pStream->m_RequestPrecincts[0]->m_Packets.size(); l++) {
//			pJPC->Lock();
			NCSJPCPacketId nPacket = pStream->m_RequestPrecincts[0]->m_Packets[l];
			if(pStream->GetPacketStatus(nPacket) == CNCSJPCPacketStatus::NONE) {
				NCS_PACKET_PACK_ELEMENT(nPacket);
				iPacketSpaceLeft -= sizeof(NCSBlockId);
				nRequestCount++;
				pStream->SetPacketStatus(nPacket, CNCSJPCPacketStatus::REQUESTED);
			}
//			pJPC->UnLock();
		}
		pStream->m_RequestPrecincts.erase(pStream->m_RequestPrecincts.begin());
	}
	// Update size and counts
#ifdef NCSBO_MSBFIRST		
	iPacketSize = ((UINT8 *) pLocalData) - ((UINT8 *) pPacket);
	pPacket->iPacketSize = NCSByteSwap32(iPacketSize);
	*pCancelCount = NCSByteSwap16(nCancelCount);
	*pRequestCount = NCSByteSwap16(nRequestCount);
#else
	iPacketSize = (UINT32)(((UINT8 *) pLocalData) - ((UINT8 *) pPacket));
	pPacket->iPacketSize = iPacketSize;
	*pCancelCount = nCancelCount;
	*pRequestCount = nRequestCount;
#endif //MACINTOSH

	NCS_PACKET_PACK_END(pPacket);

	// Send the packet if something to send
	if( nCancelCount || nRequestCount ) {
		BOOLEAN bIsConnected;																			
		if( NCScnetSendPacket(pStream->m_ClientNetID, pPacket, iPacketSize, pStream, &bIsConnected)) { 
			pStream->m_bSendInProgress = TRUE;
			pStream->m_nClientSequence += 1;
		} else																							
			pStream->m_bIsConnected = false;
	}

//	pJPC->UnLock();
	pStream->UnLock();
}

void CNCSJPCEcwpIOStream::SetPacketStatus(NCSJPCPacketId nPacket, 
										  CNCSJPCEcwpIOStream::CNCSJPCPacketStatus::Status eStatus)
{
	CNCSJPCPacketStatusIterator pCur = m_Packets.find(nPacket);
	CNCSJPCPacketStatusIterator pEnd = m_Packets.end();

	if(pCur != pEnd) {
		if(eStatus == CNCSJPCPacketStatus::NONE) {
			m_Packets.erase(pCur);
		} else {
			(*pCur).second.m_eStatus = eStatus;
		}
		return;
	}
	CNCSJPCPacketStatus status;
	status.m_nPacket = nPacket;
	status.m_eStatus = eStatus;
	m_Packets.insert((const std::pair<const NCSJPCPacketId, CNCSJPCPacketStatus>) std::pair<const NCSJPCPacketId, CNCSJPCPacketStatus> ((const NCSJPCPacketId)nPacket, status));
//	m_Packets.resize(m_Packets.size() + 1);
//	m_Packets.back().m_nPacket = nPacket;
//	m_Packets.back().m_eStatus = eStatus;
}

CNCSJPCEcwpIOStream::CNCSJPCPacketStatus::Status CNCSJPCEcwpIOStream::GetPacketStatus(NCSJPCPacketId nPacket)
{
	CNCSJPCPacketStatusIterator pCur = m_Packets.find(nPacket);
	CNCSJPCPacketStatusIterator pEnd = m_Packets.end();

	if(pCur != pEnd) {
		return((*pCur).second.m_eStatus);
	}
	return(CNCSJPCPacketStatus::NONE);
}
