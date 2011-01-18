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
** FILE:     $Archive: /NCS/Source/include/NCSJPCPOCMarker.h $
** CREATED:  18/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCPOCMarker class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCPOCMARKER_H
#define NCSJPCPOCMARKER_H

#include "NCSJPCMarker.h"
#include "NCSJPCProgressionOrderType.h"

	/**
	 * CNCSJPCPOCMarker class - the JPC POC marker.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.7 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCPOCMarker: public CNCSJPCMarker {
public:


		/** Information for each progression order change */
	typedef struct {
			/** Resolution level index (inclusive) for progression start */
		UINT8		m_nRSpoc;	
			/** Component index (inclusive) for progression start */
		UINT16		m_nCSpoc;
			/** Layer index (exclusive) for the end of progression  */
		UINT16		m_nLYEpoc;
			/** Resolution Level index (exclusive) for the end of progression */
		UINT8		m_nREpoc;
			/**	Component index (exclusive) for the end of progression */
		UINT16		m_nCEpoc;
			/** Progression order */
		CNCSJPCProgressionOrderType	m_Ppoc;
	} ProgressionOrder;

		/** The progression order changes */
	std::vector<ProgressionOrder> m_Progressions;

		/** Current active Progression index */
	UINT16	m_nCurProgression;

		/** Default constructor, initialises members */
	CNCSJPCPOCMarker();
		/** Virtual destructor */
	virtual ~CNCSJPCPOCMarker();

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


#endif // !NCSJPCPOCMARKER_H
