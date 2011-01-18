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
** FILE:     $Archive: /NCS/Source/include/NCSJPCSIZMarker.h $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCSIZMarker class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCSIZMARKER_H
#define NCSJPCSIZMARKER_H

#include "NCSJPCMarker.h"
#include "NCSJPCComponentDepthType.h"

	/**
	 * CNCSJPCSIZMarker class - the JPC SIZ marker.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.8 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCSIZMarker: public CNCSJPCMarker {
public:
		/** Information for each component in the image */
	class NCSJPC_EXPORT_ALL ComponentInfo: public CNCSJPCComponentDepthType {
	public:
			/** Horizontal separation of a sample of component WRT reference grid */
		UINT8		m_nXRsiz;
			/** Vertical separation of a sample of component WRT reference grid */
		UINT8		m_nYRsiz;

			/** Default constructor */
		ComponentInfo();
			/** virtual destructor */
		virtual ~ComponentInfo();
			/** 
			 * Parse from the JPC codestream.
			 * @param		JPC			CNCSJPC to use to parse file.
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError Parse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream);
			/** 
			 * UnParse to the JPC codestream.
			 * @param		Stream		IOStream to use to parse file.
			 * @param		JPC			CNCSJPC to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError UnParse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream);
	};

		/** Capabilities required to read this codestream */
	UINT16		m_nRsiz;
		/** Width of the reference grid */
	UINT32		m_nXsiz;
		/** Height of the reference grid */
	UINT32		m_nYsiz;
		/** Horizontal offset to left of image within the grid */
	UINT32		m_nXOsiz;
		/** Vertical offset to top of image within the grid */
	UINT32		m_nYOsiz;
		/** Width of one reference tile wrt the reference grid */
	UINT32		m_nXTsiz;
		/** Height of one reference tile wrt the reference grid */
	UINT32		m_nYTsiz;
		/** Horizontal offset to the left of first tile within the grid */
	UINT32		m_nXTOsiz;
		/** Vertical offset to the top of first tile within the grid */
	UINT32		m_nYTOsiz;
		/** Number of components in the image */
	UINT16		m_nCsiz;
		/** Component info */
	std::vector<ComponentInfo> m_Components;

		/** Default constructor, initialises members */
	CNCSJPCSIZMarker();
		/** Virtual destructor */
	virtual ~CNCSJPCSIZMarker();

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
		 * @param		INT32		Tile Index
		 * @return		INT32		Tile P Index
		 */
	INT32 GetTilePFromIndex(INT32 iIndex);
		/**
		 * Get the Tile Q index from the tile index
		 * @param		INT32		Tile Index
		 * @return		INT32		Tile Q Index
		 */
	INT32 GetTileQFromIndex(INT32 iIndex);
		/**
		 * Get the Tile Index from the tile PQ index
		 * @param		INT32		Tile P Index
		 * @param		INT32		Tile Q Index
		 * @return		INT32		Tile Index
		 */
	INT32 GetTileIndexFromPQ(INT32 iPIndex, INT32 iQIndex);
};


#endif // !NCSJPCSIZMARKER_H
