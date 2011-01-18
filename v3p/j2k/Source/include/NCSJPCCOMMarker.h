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
** FILE:     $Archive: /NCS/Source/include/NCSJPCCOMMarker.h $
** CREATED:  19/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCCOMMarker class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCCOMMARKER_H
#define NCSJPCCOMMARKER_H

#ifndef NCSJPCMARKER_H
#include "NCSJPCMarker.h"
#endif // NCSJPCMARKER_H


	/**
	 * CNCSJPCCOMMarker class - the JPC COM marker.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.8 $ $Author: simon $ $Date: 2005/01/17 09:11:17 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCCOMMarker: public CNCSJPCMarker {
public:
	typedef enum {
		BINARY			= 0,
		IS_8859_LATIN	= 1
	} Registration;

	Registration	m_eRegistration;
	void			*m_pComment;

		/** Default constructor, initialises members */
	CNCSJPCCOMMarker();
		/** copy constructor */
	CNCSJPCCOMMarker(const CNCSJPCCOMMarker &s);
		/** Virtual destructor */
	virtual ~CNCSJPCCOMMarker();

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

};


#endif // !NCSJPCCOMMARKER_H
