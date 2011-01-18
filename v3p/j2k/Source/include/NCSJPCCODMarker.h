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
** FILE:     $Archive: /NCS/Source/include/NCSJPCCODMarker.h $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCCODMarker class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCCODMARKER_H
#define NCSJPCCODMARKER_H

#include "NCSJPCMarker.h"
#include "NCSJPCCOCMarker.h"
#include "NCSJPCCodingStyleParameter.h"
#include "NCSJPCProgressionOrderType.h"

	/**
	 * CNCSJPCCODMarker class - the JPC COD marker.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.7 $ $Author: simon $ $Date: 2005/01/17 09:11:17 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCCODMarker: public CNCSJPCMarker {
public:
		/** Coding Style flags for Scod field */
	typedef struct {
		bool	bDefinedPrecincts: 1;
		bool	bSOPMarkers: 1;
		bool	bEPHMarkers: 1;
	} CodingStyle;

		/** SG Parameter values */
	typedef struct {
			/** Progression Order */
		CNCSJPCProgressionOrderType	m_ProgressionOrder;
			/** Number of layers, 1-65535 */
		UINT16					m_nLayers;
			/** Multi-Component Transform */
		bool					m_bMCT;
	} StyleParameters;

		/** Coding style for all components */
	CodingStyle			m_Scod;
		/** SG Coding style parameters */
	StyleParameters		m_SGcod;
		/** SP Coding style parameters */
	CNCSJPCCodingStyleParameter	m_SPcod;

		/** Default constructor, initialises members */
	CNCSJPCCODMarker();
		/** Virtual destructor */
	virtual ~CNCSJPCCODMarker();

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
		 * @param		src		COC Marker.
		 * @return      CNCSJPCCODMarker	Updated CODMarker.
		 */
	class CNCSJPCCODMarker& operator=( const class CNCSJPCCOCMarker& src );
};


#endif // !NCSJPCCODMARKER_H
