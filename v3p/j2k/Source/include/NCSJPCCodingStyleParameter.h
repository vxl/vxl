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
** FILE:     $Archive: /NCS/Source/include/NCSJPCCodingStyleParameter.h $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCCodingStyleParameter class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCCODINGSTYLEPARAMETERS_H
#define NCSJPCCODINGSTYLEPARAMETERS_H

#include "NCSJPCIOStream.h"

	/**
	 * CNCSJPCCODMarker class - the JPC COD marker.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.8 $ $Author: simon $ $Date: 2005/01/17 09:11:17 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCCodingStyleParameter {
public:

		/** SP Transformation Type */
	typedef enum {
			/** 9x7 irreversible filter */
		IRREVERSIBLE_9x7	= 0,
			/** 5x3 reversible filter */
		REVERSIBLE_5x3		= 1
	} TransformationType;

		/** SP Precinct Size */
	typedef struct {
			/** Precinct width exponent */
		UINT8		m_nPPx: 4;
			/** Precinct height exponent */
		UINT8		m_nPPy: 4;
	} PrecinctSize;

		/** Number of decomposition levels */
	UINT8						m_nLevels;
		/** Code block width exponent offset value */
	UINT8						m_nXcb;
		/** Code block height exponent offset value */
	UINT8						m_nYcb;
		/** CodeBlock coding pass style */
	UINT8						m_Scb;
		/** Wavelet transformation used */
	TransformationType			m_eTransformation;
		/** Precinct sizes */
	std::vector<PrecinctSize>	m_PrecinctSizes;

		/** Default constructor, initialises members */
	CNCSJPCCodingStyleParameter();
		/** Virtual destructor */
	virtual ~CNCSJPCCodingStyleParameter();

		/** 
		 * Parse the fields from the JPC codestream.
		 * @param		JPC			CNCSJPC to use to parse file.
		 * @param		Stream		IOStream to use to parse file.
		 * @param		bDefinedPrecincts	Precincts are explicitly defined in the CodingStyleParameter
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError Parse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream, bool bDefinedPrecincts);
		/** 
		 * UnParse the fields to the JPC codestream.
		 * @param		Stream		IOStream to use to parse file.
		 * @param		JPC			CNCSJPC to use to parse file.
		 * @param		bDefinedPrecincts	Precincts are explicitly defined in the CodingStyleParameter
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError UnParse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream, bool bDefinedPrecincts);
};


#endif // !NCSJPCCODINGSTYLEPARAMETERS_H
