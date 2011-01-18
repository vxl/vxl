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
** FILE:     $Archive: /NCS/Source/include/NCSJPCMainHeader.h $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCMainHeader class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCMAINHEADER_H
#define NCSJPCMAINHEADER_H

#include "NCSJPCTypes.h"
#include "NCSJPCMarker.h"
#include "NCSJPCSOCMarker.h"
#include "NCSJPCSIZMarker.h"
#include "NCSJPCCODMarker.h"
#include "NCSJPCCOCMarker.h"
#include "NCSJPCEOCMarker.h"
#include "NCSJPCQCDMarker.h"
#include "NCSJPCQCCMarker.h"
#include "NCSJPCRGNMarker.h"
#include "NCSJPCPOCMarker.h"
#include "NCSJPCTLMMarker.h"
#include "NCSJPCPLMMarker.h"
#include "NCSJPCPPMMarker.h"
#include "NCSJPCCRGMarker.h"
#include "NCSJPCCOMMarker.h"
#include "NCSJPCTypes.h"

	/**
	 * CNCSJPCMainHeader class - the JPC codestream Main Header.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.17 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCMainHeader {
public:
		/** Main header is valid */
	bool	m_bValid;

		/** SOC start of codestream Marker - required */
	CNCSJPCSOCMarker	m_SOC;
		/** SIZ image size Marker - required */
	CNCSJPCSIZMarker	m_SIZ;
		/** COD coding style default Marker - required */
	CNCSJPCCODMarker	m_COD;
		/** QCD quantization default Marker - required */
	CNCSJPCQCDMarker	m_QCD;
		/** EOC end of codestream Marker - required */
	CNCSJPCEOCMarker	m_EOC;
		/** RGN markers */
	std::vector<CNCSJPCRGNMarker> m_RGNs;

		/** Any progression order changes 
		 * NOTE: this over-rides any progression in a tile-part COD marker
		 */
	CNCSJPCPOCMarker	m_POC;
		/** TLM markers */
	std::vector<CNCSJPCTLMMarker> m_TLMs;
		/** PLM markers */
	std::vector<CNCSJPCPLMMarker> m_PLMs;
		/** PPM markers */
	std::vector<CNCSJPCPPMMarker> m_PPMs;
		/** Are the PPM's from the file or dynamically created? */
	bool				m_bFilePPMs;

		/** CRG component registration - optional */
	CNCSJPCCRGMarker	m_CRG;
		/** COM Marker - compression only */
	CNCSJPCCOMMarker	m_COM;

		/** Default constructor, initialises members */
	CNCSJPCMainHeader();
		/** Virtual destructor */
	virtual ~CNCSJPCMainHeader();

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
		 * Get X0, the component top left in reference grid
		 * @param		INT32		Component Index
		 * @return		INT32		Component coordinate
		 */
	INT32 GetComponentX0(INT32 iComponent);
		/**
		 * Get X1, the component bottom right in reference grid
		 * @param		INT32		Component Index
		 * @return		INT32		Component coordinate
		 */
	INT32 GetComponentX1(INT32 iComponent);
		/**
		 * Get Y0, the component top left in reference grid
		 * @param		INT32		Component Index
		 * @return		INT32		Component coordinate
		 */
	INT32 GetComponentY0(INT32 iComponent);
		/**
		 * Get Y1, the component bottom right in reference grid
		 * @param		INT32		Component Index
		 * @return		INT32		Component coordinate
		 */
	INT32 GetComponentY1(INT32 iComponent);
		/**
		 * Get the component width in reference grid
		 * @param		INT32		Component Index
		 * @return		INT32		Component width
		 */
	INT32 GetComponentWidth(INT32 iComponent);
		/**
		 * Get the component height in reference grid
		 * @param		INT32		Component Index
		 * @return		INT32		Component height
		 */
	INT32 GetComponentHeight(INT32 iComponent);

		/**
		 * Get the number of tile across the image
		 * @return		INT32		Number of tiles across image
		 */
	INT32 GetNumXTiles();
		/**
		 * Get the number of tile down the image
		 * @return		INT32		Number of tiles down image
		 */
	INT32 GetNumYTiles();
		/**
		 * Get the Tile P index from the tile index
		 * @param		INT32		Tile Index, >= 0
		 * @return		INT32		X of the tile, >= 0
		 */
	INT32 GetTilePFromIndex(INT32 iIndex);
		/**
		 * Get the Tile Q index from the tile index
		 * @param		INT32		Tile Index, >= 0
		 * @return		INT32		Y of the tile, >= 0
		 */
	INT32 GetTileQFromIndex(INT32 iIndex);
		/**
		 * Get the Tile Index from the tile PQ index
		 * @param		INT32		Tile P Index
		 * @param		INT32		Tile Q Index
		 * @return		INT32		Tile Index
		 */
	INT32 GetTileIndexFromPQ(INT32 iPIndex, INT32 iQIndex);

		/** Tmp directory for encoding */
	char				*m_pTmpDir;

		/** Encoder PLT Stream - holds all packet lengths during encoding */
	CNCSJPCIOStream		*m_pEncoderPLTFile;
		/** Encoder Resolution Packet Offset temp files */
	CNCSJPCIOStream		*m_pEncoderOffsetFile;
		/** Encoder Resolution Packet temp files */
	CNCSJPCIOStreamVector m_EncoderFiles;

	bool OpenEncoderFiles(bool bWrite);
	bool CloseEncoderFiles(bool bDelete);

protected:
	friend class CNCSJPCTilePartHeader;
	friend class CNCSJP2File;
			/** CodingStyle for each component */
	std::vector<CNCSJPCCODMarker> m_CodingStyles;
		/** QuantizationStyle for each component */
	std::vector<CNCSJPCQCDMarker> m_QuantizationStyles;
};



#endif // !NCSJPCMAINHEADER_H
