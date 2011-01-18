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
** FILE:     $Archive: /NCS/Source/include/NCSJPCSODMarker.h $
** CREATED:  11/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCSODMarker class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCSODMARKER_H
#define NCSJPCSODMARKER_H

#ifndef NCSJPCMARKER_H
#include "NCSJPCMarker.h"
#endif // NCSJPCMARKER_H


	/**
	 * CNCSJPCSODMarker class - the JPC SOD marker.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.7 $ $Author: simon $ $Date: 2005/01/17 09:11:24 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCSODMarker: public CNCSJPCMarker {
public:
		/** Byte offset of this tile-part's data in the codestream */
	INT64	m_nDataOffset;

		/** Default constructor, initialises members */
	CNCSJPCSODMarker();
		/** Virtual destructor */
	virtual ~CNCSJPCSODMarker();

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

private:
	INT64 FindSOP(CNCSJPCIOStream &Stream, UINT64 &nBytesLeft);

};


#endif // !NCSJPCSODMARKER_H
