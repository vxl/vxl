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
** FILE:     $Archive: /NCS/Source/include/NCSJP2IOStream.h $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCIOStream class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCECWPIOSTREAM_H
#define NCSJPCECWPIOSTREAM_H

#ifndef NCSJPCMEMORYIOSTREAM_H
#include "NCSJPCMemoryIOStream.h"
#endif // NCSJPCMEMORYIOSTREAM_H
#ifndef NCSJPCPRECINCT_H
#include "NCSJPCPrecinct.h"
#endif // NCSJPCPRECINCT_H
#ifndef NCSUTIL_H
#include "NCSUtil.h"
#endif // NCSUTIL_H

#include "NCScnet.h"

/**
 * CNCSJPCEcwpIOStream class - 64bit ECWP IO "Stream" class.
 * This class is the IO mechanism used to access JP2 files via ECWP.
 * << and >> Operators are not implemented as we can't use exception handling for portability reasons.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.10 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
 */
class NCSJPC_EXPORT_ALL CNCSJPCEcwpIOStream: public CNCSJPCMemoryIOStream, private CNCSMutex {
public:

	class NCSJPC_EXPORT_ALL CNCSJPCPacketStatus {
	public:
		/** Packet status enum */
		typedef enum {
			NONE		= 0,
			REQUESTED,
			RECEIVED,
			DECODED
		} Status;	

		NCSJPCPacketId	m_nPacket;
		Status			m_eStatus;
	};	
	typedef std::map<NCSJPCPacketId, CNCSJPCPacketStatus>::iterator CNCSJPCPacketStatusIterator;

		/** Constructor, initialises members */
	CNCSJPCEcwpIOStream(CNCSJPC *pJPC, bool bSeekable = true);
		/** Virtual destructor, releases members */
	virtual ~CNCSJPCEcwpIOStream();

	virtual void Lock(bool bLockJPC = true);
	virtual void UnLock(bool bUnLockJPC = true);
		/** 
		 * Open the stream on the specified file.
		 * @param		pEcwpURL		ECWP URL
		 * @return      CNCSError	NCS_SUCCESS, or error code on failure.
		 */
	virtual CNCSError Open(char *pEcwpURL);
		/** 
		 * Close the stream.
		 * @return      CNCSError	NCS_SUCCESS, or error code on failure.
		 */
	virtual CNCSError Close();

		/** Is the stream a packet stream?  This means the SOD segment will be missing from
		 * the stream as parsed, and must be requested separately (ie, ECWP, JPIP)
		 * @return		bool		true this is a packet stream, otherwise false
		 */
	virtual bool NCS_FASTCALL IsPacketStream();
	
	virtual void RequestPrecinct(CNCSJPCPrecinct *pPrecinct);
	virtual void CancelPrecinct(CNCSJPCPrecinct *pPrecinct);
	virtual CNCSError SendRequests();

	virtual CNCSJPCPacketStatus::Status GetPacketStatus(NCSJPCPacketId nPacket);
	virtual void SetPacketStatus(NCSJPCPacketId nPacket, CNCSJPCPacketStatus::Status eStatus);

	virtual void ProcessReceivedPackets();
protected:
	CNCSJPC			*m_pJPC;
	pNCSnetClient	m_ClientNetID;
	NCSClientUID	m_ClientUID;
	bool			m_bIsConnected;
	bool			m_bSendInProgress;
	NCSSequenceNr	m_nClientSequence;
	NCSPacket		*m_pSendPacket;

	std::map<NCSJPCPacketId, CNCSJPCPacketStatus> m_Packets;

	typedef struct {
		NCSPacket *pPacket;
		INT32 nLength;
	} ReceivedPacket;
	std::list<ReceivedPacket*> m_ReceivedPackets;

		/** Precincts we need to request packets for */
	std::vector<CNCSJPCPrecinct*> m_RequestPrecincts;
		/** Precincts we need to cancel packets for */
	std::vector<CNCSJPCPrecinct*> m_CancelPrecincts;
		
	static void PacketRecvCB(NCSPacket *pPacket, INT32 nLength, pNCSnetClient pClientNetID, CNCSJPCEcwpIOStream *pJPC, NCSError eError);	 
	static void PacketSentCB(NCSError eError, UINT8 *pLastPacketSent, CNCSJPCEcwpIOStream *pJPC);			
};

#endif // !NCSJPCECWPIOSTREAM_H
