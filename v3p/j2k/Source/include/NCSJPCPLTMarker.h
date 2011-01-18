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
** FILE:     $Archive: /NCS/Source/include/NCSJPCPLTMarker.h $
** CREATED:  19/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCPLTMarker class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCPLTMARKER_H
#define NCSJPCPLTMARKER_H

#include "NCSJPCMarker.h"
#include "NCSJPCPacketLengthType.h"
#include "NCSJPCNode.h"

	/**
	 * CNCSJPCPLTMarker class - the JPC PLT marker.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.12 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCPLTMarker: public CNCSJPCMarker {
public:
		/** Index of this marker WRT other PLT markers */
	UINT8		m_nZplt;

		/** Number of packet lengths in this marker */
	UINT16		m_nLengths;
		/** Lengths in this PLT are cumulative */
	bool	m_bCumulativeLengths;

		/** PLT's were dynamically generated */
	bool		m_bDynamic;

		/** Length offsets when hashing index */
	UINT32 m_nFirstPacket;
	UINT64 m_nHeaderLengthOffset;
	UINT64 m_nDataLengthOffset;

		/** Default constructor, initialises members */
	CNCSJPCPLTMarker();
		/** Virtual destructor */
	virtual ~CNCSJPCPLTMarker();

		/** 
		 * Parse the marker from the JPC codestream.
		 * @param		JPC			CNCSJPC to use to parse file.
		 * @param		Stream		IOStream to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError Parse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream);
		/** 
		 * UnParse the marker to the JPC codestream.
		 * @param		Stream		IOStream to use to parse file.
		 * @param		JPC			CNCSJPC to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError UnParse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream);
		/** 
		 * Parse the marker from the JPC codestream.
		 * @param		JPC			CNCSJPC to use to parse file.
		 * @param		Stream		IOStream to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual bool ParseLength(class CNCSJPC &JPC, CNCSJPCIOStream &Stream, UINT16 iLength);

		/** 
		 * Assignment operator.
		 * @param		src			Copy source.
		 * @return      void
		 */
	void operator=( const class CNCSJPCPLTMarker& src );
		/** 
		 * Allocate the lengths array vector.
		 */
	void AllocLengths(void);
		/** 
		 * Free the lengths array vector.
		 */
	void FreeLengths(void);
		/** 
		 * Get a pointer to the lengths array vector.
		 */
	std::vector<CNCSJPCPacketLengthType> *GetLengths();
		/** 
		 * Add a length to the lengths array.
		 */
	void AddLength(CNCSJPCPacketLengthType &Len);

protected:
	static CNCSJPCNode::CNCSJPCNodeTracker sm_Tracker;

		/** Packet length(s) for tile-part */
	std::vector<CNCSJPCPacketLengthType>	*m_pLengths;
};

#endif // !NCSJPCPLTMARKER_H
