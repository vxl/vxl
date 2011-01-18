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
** FILE:     $Archive: /NCS/Source/include/NCSJPCCOCMarker.h $
** CREATED:  11/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCCOCMarker class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCCOCMARKER_H
#define NCSJPCCOCMARKER_H

#include "NCSJPCMarker.h"
#include "NCSJPCCodingStyleParameter.h"

	/**
	 * CNCSJPCCOCMarker class - the JPC COD marker.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.7 $ $Author: simon $ $Date: 2005/01/17 09:11:17 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCCOCMarker: public CNCSJPCMarker {
public:

		/** Index of component which the marker relates to */
	UINT16			m_nCcoc;
		/** Coding style for this component */
	bool			m_bScoc;
		/** SP Coding style parameters */
	CNCSJPCCodingStyleParameter	m_SPcoc;

		/** Default constructor, initialises members */
	CNCSJPCCOCMarker();
		/** Copy constructor */
	CNCSJPCCOCMarker(const class CNCSJPCCODMarker &src);
		/** Virtual destructor */
	virtual ~CNCSJPCCOCMarker();

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
		 * Assignment operator
		 * @param		src		COD Marker.
		 * @return      CNCSJPCCOCMarker	Updated COCMarker.
		 */
	class CNCSJPCCOCMarker &operator=( const class CNCSJPCCODMarker &src );
};


#endif // !NCSJPCCODMARKER_H
