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
** FILE:     $Archive: /NCS/Source/include/NCSJPC.h $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPC class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPC_H
#define NCSJPC_H

#ifndef NCSJPCMAINHEADER_H
#include "NCSJPCMainHeader.h"
#endif // NCSJPCMAINHEADER_H
#ifndef NCSJPCTILEPARTHEADER_H
#include "NCSJPCTilePartHeader.h"
#endif // NCSJPCTILEPARTHEADER_H

#include <map>

	/**
	 * CNCSJPC class - the JPC codestream class.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.24 $ $Author: russell $ $Date: 2005/03/04 05:02:48 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPC: public CNCSJPCMainHeader {
public:
	typedef enum {
			/** Profile 0 */
		BASELINE_0		= 0,
			/** Profile 1 */
		BASELINE_1		= 1,
			/** Profile 2 - unrestricted */
		BASELINE_2		= 2,
			/** NITF NPJE profile */
		NITF_BIIF_NPJE	= 3,
			/** NITF EPJE Profile */
		NITF_BIIF_EPJE	= 4
	} Profile;

		/** Pointer to IO Stream instance for this codestream */
	CNCSJPCIOStream *m_pStream;

	/** A vector of the first Tile Part for each tile */
	std::vector<CNCSJPCTilePartHeader *> m_Tiles;

	/** Current Tile-Part being decoded */
	CNCSJPCTilePartHeader	*m_pCurrentTilePart;
		/** Next tilepart index */
	UINT32	m_nNextZtp;

		/** performing low-memory compression */
	bool m_bLowMemCompression;
		/** Reconstruction parameter */
	IEEE4 m_fReconstructionParameter;

		/** Default constructor, initialises members */
	CNCSJPC();
		/** Virtual destructor */
	virtual ~CNCSJPC();

		/** 
		 * Parse the JPC codestream.
		 * @param		Stream		IOStream to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError Parse(CNCSJPCIOStream &Stream);
		/** 
		 * UnParse the JPC codestream.
		 * @param		Stream		IOStream to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError UnParse(CNCSJPCIOStream &Stream);

		/**
		 * Get the Tile from the index and part index
		 * @param		INT32		Tile Index, >= 0, -1 == current tile main tile header
		 * @param		INT32		Tile Part Index, >= 0
		 * @return		CNCSJPCTilePartHeader*		Pointer to tile part
		 */
	CNCSJPCTilePartHeader *GetTile(INT32 iIndex = -1, INT32 iPart = 0);
		/** 
		 * Get length of a packet in the file.
		 * @param		nPacket		Absolute Packet number to determine length for
		 * @return      UINT32		Packet length or 0 on failure.
		 */
	UINT32 GetPacketLength(UINT32 nPacket);
		/** 
		 * Read a packet from the file.
		 * @param		nPacket		Absolute Packet number to determine length for
		 * @param		pLength		Returned length of packet (header+data)
		 * @return      void *		NCSMalloc()'d pointer to read packet (header+data).
		 */
	void *GetPacket(UINT32 nPacket, UINT32 *pLength);
		/** 
		 * Get packet header for the specified absolute packet index.
		 * @param		nPacket		Absolute Packet number 
		 * @return      class CNCSJPCPacket*		Packet header
		 */
	class CNCSJPCPacket *GetPacketHeader(UINT32 nPacket);
		/** 
		 * Fnid the RCPL for the specified packet index 
		 * @param		nPacket		Absolute Packet number 
		 * @param		nTile		Tile index 
		 * @param		nResolution Resolution number
		 * @param		nComponent	Component index 
		 * @param		nPrecinctX	Precinct X index 
		 * @param		nPrecinctY  Precinct Y index 
		 * @param		nLayer		Quality Layer
		 * @return      bool 		true, or false on failure
		 */
	bool FindPacketRCPL(UINT32 nPacket, UINT16 &nTile, UINT8 &nResolution, UINT16 &nComponent, UINT32 &nPrecinctX, UINT32 &nPrecinctY, UINT16 &nLayer);
private:
};

#endif // !NCSJPC_H
