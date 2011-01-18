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
** FILE:     $Archive: /NCS/Source/include/NCSJPCSOTMarker.h $
** CREATED:  11/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCSOTMarker class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCSOTMARKER_H
#define NCSJPCSOTMARKER_H

#include "NCSJPCMarker.h"

	/**
	 * CNCSJPCSOTMarker class - the JPC SOT marker.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.6 $ $Author: simon $ $Date: 2005/01/17 09:11:24 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCSOTMarker: public CNCSJPCMarker {
public:

		/** Tile Index, >= 0 */
	UINT16		m_nIsot;
		/** Length of SOT + tile part bitstream */
	UINT32		m_nPsot;
		/** Tile part index, first = 0 */
	UINT8		m_nTPsot;
		/** Number of tile-parts of a tile in the codestream, 0 == not defined in this tile part */
	UINT8		m_nTNsot;

		/** Default constructor, initialises members */
	CNCSJPCSOTMarker();
		/** Virtual destructor */
	virtual ~CNCSJPCSOTMarker();

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
};


#endif // !NCSJPCSOTMARKER_H
