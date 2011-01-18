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
** FILE:     $Archive: /NCS/Source/include/NCSJPCQCCMarker.h $
** CREATED:  12/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCQCCMarker class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCQCCMARKER_H
#define NCSJPCQCCMARKER_H

#include "NCSJPCMarker.h"
#include "NCSJPCQuantizationParameter.h"
	/**
	 * CNCSJPCQCCMarker class - the JPC QCC marker.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.6 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCQCCMarker: public CNCSJPCMarker, public CNCSJPCQuantizationParameter {
public:
		/** Component index for this QCC */
	UINT16	m_nCqcc;

		/** Default constructor, initialises members */
	CNCSJPCQCCMarker();
		/** Copy constructor */
	CNCSJPCQCCMarker(const class CNCSJPCQCDMarker& src);
		/** Virtual destructor */
	virtual ~CNCSJPCQCCMarker();

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
		 * @param		src		QCD Marker.
		 * @return      CNCSJPCQCCMarker	Updated QCCMarker.
		 */
	class CNCSJPCQCCMarker& operator=( const class CNCSJPCQCDMarker& src );
};


#endif // !NCSJPCQCCMARKER_H
