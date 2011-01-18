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
** FILE:     $Archive: /NCS/Source/include/NCSJPCTilePartHeader.h $
** CREATED:  13/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCTilePartHeader class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCTILEPARTHEADER_H
#define NCSJPCTILEPARTHEADER_H

#include "NCSJPCMarker.h"
#include "NCSJPCSOTMarker.h"
#include "NCSJPCCODMarker.h"
#include "NCSJPCCOCMarker.h"
#include "NCSJPCQCDMarker.h"
#include "NCSJPCQCCMarker.h"
#include "NCSJPCRGNMarker.h"
#include "NCSJPCPOCMarker.h"
#include "NCSJPCSODMarker.h"
#include "NCSJPCPLTMarker.h"
#include "NCSJPCPPTMarker.h"
#include "NCSJPCCOMMarker.h"
#include "NCSJPCComponent.h"
#include "NCSJPCProgression.h"
#include "NCSJPCTypes.h"
#include "NCSJPC.h"

	/**
	 * CNCSJPCTilePartHeader class - the JPC codestream Tile-Part Header.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.40 $ $Author: russell $ $Date: 2005/03/04 05:02:48 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCTilePartHeader: public CNCSJPCNode, public CNCSJPCProgression {
public:
		// Tile-Part absolute index within codestream
	UINT32	m_nZtp;
		/** pointer to main JPC */
	CNCSJPC *m_pJPC;

		/** Tile-part header is valid */
	bool	m_bValid;

		/** SOC start of codestream Marker - required */
	CNCSJPCSOTMarker	m_SOT;
		/** COD coding style default Marker - optional */
	CNCSJPCCODMarker	m_COD;
		/** QCD quantization default Marker - optional */
	CNCSJPCQCDMarker	m_QCD;
		/** RGN markers */
	std::vector<CNCSJPCRGNMarker> m_Regions;

		/** POC progression order change marker - optional
		 * NOTE: Over-rides any main-header POC and any COD Marker 
		 */
	CNCSJPCPOCMarker	m_POC;

		/** SOD start of data */
	CNCSJPCSODMarker	m_SOD;

		/** Packet length(s) for this tile-part */
	std::vector<CNCSJPCPLTMarker> m_PLTs;
		/** Hashed index to PLTs, in mod 32768 */
	std::vector<UINT32> m_PLTIndex;

		/** Packet header(s) for this tile-part */
	std::vector<CNCSJPCPPTMarker> m_PPTs;
		/** Are the PPT's from the file or dynamically created? */
	bool				m_bFilePPTs;

		/** If the tile has more than 1 tile part and this is tile part 0, this vectors
			contains all the other tile parts */
	std::vector<CNCSJPCTilePartHeader *> m_TileParts;

		/** Tile Components */
	std::vector<CNCSJPCComponent*> m_Components;

		/** Encoder PLT Stream */
//	CNCSJPCIOStream		*m_pEncoderPLTFile;

		/** Default constructor, initialises members */
	CNCSJPCTilePartHeader(CNCSJPC *pJPC);
		/** Virtual destructor */
	virtual ~CNCSJPCTilePartHeader();

		/** 
		 * Recalc the structure sizes required based on the current decoding info.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError RecalcSizes();
	
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
		 * Get the Tile Index
		 * @return		INT32		Index of the tile, >= 0
		 */
	INT32 GetIndex();
		/**
		 * Get the Tile P index
		 * @return		INT32		P index of the tile, >= 0
		 */
	INT32 GetP();
		/**
		 * Get the Tile Q index
		 * @return		INT32		Q of the tile, >= 0
		 */
	INT32 GetQ();

		/**
		 * Get the Tile TX0 coordinate in reference grid
		 * @return		INT32		X of the tile, >= 0
		 */
	INT32 GetX0();
		/**
		 * Get the Tile TY0 coordinate in reference grid
		 * @return		INT32		Y of the tile, >= 0
		 */
	INT32 GetY0();
		/**
		 * Get the Tile TX1 coordinate in reference grid
		 * @return		INT32		X of the tile, >= 0
		 */
	INT32 GetX1();
		/**
		 * Get the Tile TY1 coordinate in reference grid
		 * @return		INT32		Y of the tile, >= 0
		 */
	INT32 GetY1();

		/**
		 * Get the PPx from the component resolution
		 * @param		INT32		Component Index
		 * @param		INT32		Resolution Level
		 * @return		INT32		PPx
		 */
	UINT32 GetPPx(UINT16 iComponent, UINT8 nResolution);
		/**
		 * Get the PPy from the component resolution
		 * @param		INT32		Component Index
		 * @param		INT32		Resolution Level
		 * @return		INT32		PPy
		 */
	UINT32 GetPPy(UINT16 iComponent, UINT8 nResolution);

		/**
		 * Get the xcb' from the resolution
		 * @param		INT32		Component Index
		 * @param		INT32		Resolution Level
		 * @return		INT32		xcb'
		 */
	UINT32 GetXCBPrime(UINT16 iComponent, UINT8 nResolution);
		/**
		 * Get the ycb' from the resolution
		 * @param		INT32		Component Index
		 * @param		INT32		Resolution Level
		 * @return		INT32		ycb'
		 */
	UINT32 GetYCBPrime(UINT16 iComponent, UINT8 nResolution);
		/**
		 * Get the CB Width from the resolution
		 * @param		INT32		Component Index
		 * @param		INT32		Resolution Level
		 * @return		INT32		xcb'
		 */
	UINT32 GetCBWidth(UINT16 iComponent, UINT8 nResolution);
		/**
		 * Get the CB Height from the resolution
		 * @param		INT32		Component Index
		 * @param		INT32		Resolution Level
		 * @return		INT32		ycb'
		 */
	UINT32 GetCBHeight(UINT16 iComponent, UINT8 nResolution);

		/** 
		 * Increment the progression order from the current location, parsing packet headers in the process.
		 * @param		pData		Data for Step function
		 * @param		bComplete	Progression order is complete.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError Iterate(CNCSJPCTilePartHeader *pMainTP, void *pData, bool &bComplete);
		/** 
		 * Perform the "Step" for this progression.
		 * @return      bool		true, or false on error & Error is set to actual error code.
		 */
	virtual bool Step(CNCSJPCComponent *pComponent,
					  CNCSJPCResolution *pResolution,
					  CNCSJPCPrecinct *pPrecinct);

	virtual bool StepRow(class CNCSJPCComponent *pComponent,
						 class CNCSJPCResolution *pResolution);

		/**
		 * Get the Packet Header for the specified packet number
		 * @param		nPacket		Packet number
		 * @return		CNCSJPCPacket	Packet Header struct (may be unparsed)
		 */
	CNCSJPCPacket *GetPacketHeader(UINT32 nPacket);
		/**
		 * Get the first packet number for this tile
		 * @return		UINT32	First packet number
		 */
	UINT32 GetFirstPacketNr();
		/**
		 * Get the number of packets for this tile
		 * @return		UINT32	Packet count
		 */
	UINT32 GetNrPackets();

		/** 
		 * Write a BufferType line to the output.
		 * @param		nCtx		Context
		 * @param		pSrc		Source buffer.
		 * @param		iComponent	Output Component
		 * @return      bool		true on succes, else false.
		 */
	virtual bool WriteLine(ContextID nCtx, CNCSJPCBuffer *pSrc, UINT16 iComponent);	

//	bool OpenEncoderPLTFile(bool bWrite);
//	bool CloseEncoderPLTFile(bool bDelete);

	bool SimpleStructure() {
		return(m_bSimpleStructure);
	}

protected:
	static CNCSJPCNodeTracker	sm_Tracker;

	TNCSCachedValue<UINT32> m_FirstPacketNr;
	TNCSCachedValue<UINT32> m_NrPackets;

		// Dynamic precincts in progression Step()
	bool m_bDynPrecinct;
		// Stream is seekable
	bool m_bSeekable;
		// Parsing Dynamic precincts in progression Step()
	bool m_bParsingHeaders;
		// Simple structure, R/P/L equal in all C and no POC
	bool m_bSimpleStructure;
};



#endif // !NCSJPCTILEPARTHEADER_H
