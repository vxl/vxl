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
** FILE:     $Archive: /NCS/Source/include/NCSJPCProgressionOrderType.h $
** CREATED:  18/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCProgressionOrderType class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCPROGRESSIONORDERTYPE_H
#define NCSJPCPROGRESSIONORDERTYPE_H

#include "NCSJPCMarker.h"

	/**
	 * CNCSJPCProgressionOrderType class - the JPC Progression Order Type.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.6 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCProgressionOrderType {
public:
				/** Progression Order Type */
	typedef enum {
			/** Layer-Resolution-Component-Position */
		LRCP		 = 0,
			/** Resolution-Layer-Component-Position */
		RLCP		 = 1,
			/** Resolution-Position-Component-Layer */
		RPCL		 = 2,
			/** Position-Component-Resolution-Layer */
		PCRL		 = 3,
			/** Component-Position-Resolution-Layer */
		CPRL		 = 4
	} Type;

		/** The type */
	Type m_eType;

		/** Default constructor, initialises members */
	CNCSJPCProgressionOrderType();
		/** Virtual destructor */
	virtual ~CNCSJPCProgressionOrderType();

		/** 
		 * Parse the fields from the JPC codestream.
		 * @param		JPC			CNCSJPC to use to parse file.
		 * @param		Stream		IOStream to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError Parse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream);
		/** 
		 * UnParse the fields to the JPC codestream.
		 * @param		Stream		IOStream to use to parse file.
		 * @param		JPC			CNCSJPC to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError UnParse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream);
};


#endif // !NCSJPCPROGRESSIONORDERTYPE_H
