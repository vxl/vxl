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
** FILE:     $Archive: /NCS/Source/include/NCSJPCrGNMarker.h $
** CREATED:  12/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCRGNMarker class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCRGNMARKER_H
#define NCSJPCRGNMARKER_H

#include "NCSJPCMarker.h"

	/**
	 * CNCSJPCRGNMarker class - the JPC RGN marker.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.6 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCRGNMarker: public CNCSJPCMarker {
public:
	typedef enum {
		IMPLICIT	= 0
	} Style;
		/** Index of component this region is for */
	UINT16		m_nCrgn;
		/** Style of the region */
	Style		m_eSrgn;
		/** Resion style parameter */
	UINT8		m_nSPrgn;

		/** Default constructor, initialises members */
	CNCSJPCRGNMarker();
		/** Virtual destructor */
	virtual ~CNCSJPCRGNMarker();

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


#endif // !NCSJPCRGNMARKER_H
