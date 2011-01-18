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
** FILE:     $Archive: /NCS/Source/include/NCSJPCPacketHeader.h $
** CREATED:  18/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCPacket class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCPACKETHEADER_H
#define NCSJPCPACKETHEADER_H

#include "NCSJPCMarker.h"

	/**
	 * CNCSJPCPacket class - the JPC Packet Header Type.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.9 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCPacket {
public:
		/** Packet Number */
	NCSJPCPacketId	m_nPacket;
		/** Packet length */
	INT32	m_nLength;
		/** Total Packet data length */
	INT32	m_nDataLength;
		/** Packet offset */
	UINT64	m_nOffset;
		/** Absolute Packet Data offset in codestream */
	UINT64	m_nDataOffset;

		/** Default constructor, initialises members */
	CNCSJPCPacket();

		/** 
		 * Parse the fields from the JPC codestream.
		 * @param		JPC			CNCSJPC to use to parse file.
		 * @param		Stream		IOStream to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError ParseHeader(class CNCSJPC &JPC, 
								  CNCSJPCIOStream &Stream, 
								  class CNCSJPCProgression *pProgression,
								  bool bParseData = true,
								  bool *pbNonZeroLength = NULL);
	virtual CNCSError UnParseHeader(CNCSJPC &JPC, 
									CNCSJPCIOStream &Stream, 
									CNCSJPCProgression *pProgression, 
									bool bParseData);
	virtual CNCSError ParseBody(class CNCSJPC &JPC, 
								CNCSJPCIOStream &Stream, 
								class CNCSJPCProgression *pProgression,
								bool bNonZeroLength);	
	virtual CNCSError UnParseBody(class CNCSJPC &JPC, 
								  CNCSJPCIOStream &Stream, 
								  class CNCSJPCProgression *pProgression,
								  bool bNonZeroLength);
protected:
		/** 
		 * Parse A segment length from the JPC codestream.
		 * @param		Stream		IOStream to use to parse file.
		 * @param		pCB			CodeBlock this segment belongs too
		 * @param		nPassesIncluded Number of passes included in this segment
		 * @param		Length		Length of segment returned
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	CNCSError ParseSegment(CNCSJPCIOStream &Stream, class CNCSJPCCodeBlock *pCB, UINT32 nPassesIncluded, UINT32 &Length);
	CNCSError UnParseSegment(CNCSJPCIOStream &Stream, class CNCSJPCCodeBlock *pCB, UINT32 nPassesIncluded, UINT32 &Length);
};


//typedef std::list<CNCSJPCPacket> CNCSJPCPacketListVector;

	/**
	 * CNCSJPCPacketList class - self parsing list of JPC Packet Headers.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.9 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCPacketList {
public:
		/** Do we have offsets yet */
	bool	m_bHaveOffsets;
		/** Default constructor, initialises members */
	CNCSJPCPacketList();
		/** Virtual destructor */
	virtual ~CNCSJPCPacketList();

		/** 
		 * Parse the headers from the JPC codestream.
		 * @param		JPC			CNCSJPC to use to parse file.
		 * @param		Stream		IOStream to use to parse file.
		 * @param		bComplete   Progression order is complete
		 * @param		nBytes		Number of bytes of headers to parse
		 * @param		nHeaders	Total number of headers to parse, -1 == ignore
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError Parse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream, bool &bComplete, UINT64 nBytes, INT32 nHeaders = -1);

		/** 
		 * Parse a header from the JPC codestream.
		 * @param		bComplete   Progression order is complete
		 * @param		nCurPacket	Current packet #
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError ParseHeader(bool &bComplete, UINT32 nCurPacket);
	virtual CNCSError GenDynamicPLTs(class CNCSJPCTilePartHeader *pMainTP);
		/** 
		 * UnParse the headers to the JPC codestream.
		 * @param		Stream		IOStream to use to parse file.
		 * @param		JPC			CNCSJPC to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError UnParse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream);

	void clear() { m_Packets.clear(); };
	void push_back(CNCSJPCPacket &packet) { m_Packets.push_back(packet); };
protected:
	std::vector<CNCSJPCPacket> m_Packets;
		/** Codestream class pointer */
	class CNCSJPC *m_pJPC;
		/** Pointer to IOStream */
	class CNCSJPCIOStream *m_pStream;
		/** Number of bytes of headers to read */
	UINT64 m_nBytes;
		/** Number of headers to read */
	INT32 m_nHeaders;
};

#endif // !NCSJPCPACKETHEADER_H
