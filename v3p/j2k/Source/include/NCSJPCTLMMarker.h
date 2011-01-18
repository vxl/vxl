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
** FILE:     $Archive: /NCS/Source/include/NCSJPCTLMMarker.h $
** CREATED:  19/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCTLMMarker class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCTLMMARKER_H
#define NCSJPCTLMMARKER_H

#include "NCSJPCMarker.h"

	/**
	 * CNCSJPCTLMMarker class - the JPC TLM marker.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.7 $ $Author: simon $ $Date: 2005/01/17 09:11:24 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCTLMMarker: public CNCSJPCMarker {
public:
		/** Pointer segments */
	typedef struct {
			/** Tile index of the ith tile part */
		UINT16		m_nTtlm;	
			/** Length in bytes of the tile part from the SOT to the last byte of the tile-pert datastream */
		UINT32		m_nPtlm;
	} PointerSegment;

	typedef struct {
		UINT8	m_nST : 2;
		UINT8	m_nSP : 2;
	} SizeofTP;

		/** Index of this marker WRT other TLM markers */
	UINT8		m_nZtlm;
		/** Sizeof Ttlm and Ptlm */
	SizeofTP	m_Stlm;
		/** Pointer Segments */
	std::vector<PointerSegment> m_Pointers;

		/** Default constructor, initialises members */
	CNCSJPCTLMMarker();
		/** Virtual destructor */
	virtual ~CNCSJPCTLMMarker();

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


#endif // !NCSJPCTLMMARKER_H
