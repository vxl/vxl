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
** FILE:     $Archive: /NCS/Source/include/NCSJPCPPTMarker.h $
** CREATED:  19/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCPPTMarker class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCPPTMARKER_H
#define NCSJPCPPTMARKER_H

#include "NCSJPCMarker.h"
#include "NCSJPCPacket.h"

	/**
	 * CNCSJPCPPTMarker class - the JPC PPT marker.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.9 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCPPTMarker: public CNCSJPCMarker {
public:
		/** Index of this marker WRT other PPT markers */
	UINT8		m_nZppt;
		/** Packet header(s) for tile-part */
//	CNCSJPCPacketList		m_Headers;

		/** Default constructor, initialises members */
	CNCSJPCPPTMarker();
		/** Virtual destructor */
	virtual ~CNCSJPCPPTMarker();

		/** 
		 * Parse the marker from the JPC codestream.
		 * @param		JPC			CNCSJPC to use to parse file.
		 * @param		Stream		IOStream to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError Parse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream);
#ifndef NCSJPC_LEAN_AND_MEAN
		/** 
		 * UnParse the marker to the JPC codestream.
		 * @param		Stream		IOStream to use to parse file.
		 * @param		JPC			CNCSJPC to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError UnParse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream);
#endif //!NCSJPC_LEAN_AND_MEAN
};

#endif // !NCSJPCPPTMARKER_H
