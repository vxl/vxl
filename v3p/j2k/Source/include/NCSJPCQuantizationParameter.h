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
** FILE:     $Archive: /NCS/Source/include/NCSJPCQuantizationParameter.h $
** CREATED:  13/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCQuantizationParameter class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCQUANTIZATIONPARAMETER_H
#define NCSJPCQUANTIZATIONPARAMETER_H

#include "NCSJPCMarker.h"

	/**
	 * CNCSJPCQuantizationParameter class - the JPC Quantization Parameters.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.6 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCQuantizationParameter {
public:
	typedef enum {
		NONE				= 0x0,
		SCALAR_DERIVED		= 0x1,
		SCALAR_EXPOUNDED	= 0x2
	} Style;

	typedef struct {
		Style	m_eStyle;	
		UINT8				m_nGuardBits;
	} Values;

	typedef struct {
		UINT16		m_nMantissa;
		UINT8		m_nExponent;
	} Irreversible;

	typedef union {
		UINT8			m_nReversible;
		Irreversible	m_Irreversible;
	} Parameters;

	Values		m_Sqc;
	std::vector<Parameters>	m_SPqc;

		/** Default constructor, initialises members */
	CNCSJPCQuantizationParameter();
		/** Virtual destructor */
	virtual ~CNCSJPCQuantizationParameter();

		/** 
		 * Parse the fields from the JPC codestream.
		 * @param		JPC			CNCSJPC to use to parse file.
		 * @param		Stream		IOStream to use to parse file.
		 * @param		nLength		# of bytes of the marker to parse
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError Parse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream, INT32 nLength);
		/** 
		 * UnParse the fields to the JPC codestream.
		 * @param		Stream		IOStream to use to parse file.
		 * @param		JPC			CNCSJPC to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError UnParse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream);
};


#endif // !NCSJPCQUANTIZATIONPARAMETER_H
