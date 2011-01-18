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
** FILE:     $Archive: /NCS/Source/include/NCSJPCQCDMarker.h $
** CREATED:  12/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCQCDMarker class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCQCDMARKER_H
#define NCSJPCQCDMARKER_H

#include "NCSJPCMarker.h"
#include "NCSJPCQuantizationParameter.h"
	/**
	 * CNCSJPCQCDMarker class - the JPC QCD marker.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.5 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCQCDMarker: public CNCSJPCMarker, public CNCSJPCQuantizationParameter {
public:
		/** Default constructor, initialises members */
	CNCSJPCQCDMarker();
		/** Virtual destructor */
	virtual ~CNCSJPCQCDMarker();

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
		 * @param		src		QCC Marker.
		 * @return      CNCSJPCQCDMarker	Updated QCDMarker.
		 */
	class CNCSJPCQCDMarker& operator=( const class CNCSJPCQCCMarker& src );
};


#endif // !NCSJPCQCDMARKER_H
