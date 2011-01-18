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
** FILE:     $Archive: /NCS/Source/include/NCSJPCSIZMarker.h $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCSIZMarker class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCCOMPONENTDEPTHTYPE_H
#define NCSJPCCOMPONENTDEPTHTYPE_H

#include "NCSJPCMarker.h"

	/**
	 * CNCSJPCComponentDepthType class - component depth/sign type
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.7 $ $Author: simon $ $Date: 2005/01/17 09:11:17 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCComponentDepthType {
public:
		/** Component bit depth */
	UINT8		m_nBits;	
		/** Component values are signed */
	bool		m_bSigned;

		/** Default constructor, initialises members */
	CNCSJPCComponentDepthType();
		/** Virtual destructor */
	virtual ~CNCSJPCComponentDepthType();

		/** 
		 * Parse from the JPC codestream.
		 * @param		JPC			CNCSJPC to use to parse file.
		 * @param		Stream		IOStream to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError Parse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream);
		/** 
		 * UnParse to the JPC codestream.
		 * @param		Stream		IOStream to use to parse file.
		 * @param		JPC			CNCSJPC to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError UnParse(class CNCSJPC &JPC, CNCSJPCIOStream &Stream);

	INT32 GetMinValue() { return(m_bSigned ? (-1 * (signed)GetMaxValue() - 1) : 0); }; 
	INT32 GetMaxValue() { return(m_bSigned ? ((1 << (m_nBits - 1)) - 1) : ((1 << m_nBits) - 1)); };
	INT32 GetDCShift() { return(m_bSigned ? 0 : (INT32)((UINT32)(1 << (m_nBits - 1)))); }; 
};


#endif // !NCSJPCCOMPONENTDEPTHTYPE_H
