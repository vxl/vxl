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
** FILE:     $Archive: /NCS/Source/include/NCSJPCMarker.h $
** CREATED:  11/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCMarker class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCMARKER_H
#define NCSJPCMARKER_H

// Our own 64bit IOSTREAM class
#ifndef NCSJPCIOSTREAM_H
#include "NCSJPCIOStream.h"
#endif // NCSJPCIOSTREAM_H



	/**
	 * CNCSJPCMarker class - the base JPC codestream marker.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.8 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCMarker {
public:
	typedef enum {
		UNKNOWN		= 0x0000,
		SOC			= 0xFF4F,
		SIZ			= 0xFF51,
		COD			= 0xFF52,
		COC			= 0xFF53,
		TLM			= 0xFF55,
		PLM			= 0xFF57,
		PLT			= 0xFF58,
		QCD			= 0xFF5C,
		QCC			= 0xFF5D,
		RGN			= 0xFF5E,
		POC			= 0xFF5F,
		PPM			= 0xFF60,
		PPT			= 0xFF61,
		CRG			= 0xFF63,
		COM			= 0xFF64,
		SOT			= 0xFF90,
		SOP			= 0xFF91,
		EPH			= 0xFF92,
		SOD			= 0xFF93,
		EOC			= 0xFFD9
	} Type;

		/** Marker type */ 
	Type	m_eMarker;

		/** Marker Offset into stream */
	UINT64  m_nOffset;
		/** Marker Length */
	UINT16  m_nLength;
		/** Do we have this marker */
	bool	m_bHaveMarker;
		/** Is this marker valid */
	bool	m_bValid;

		/** Default constructor, initialises members */
	CNCSJPCMarker();
		/** Virtual destructor */
	virtual ~CNCSJPCMarker();

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
		 * Get length of a packet in the file.
		 * @param		src			Copy source
		 * @return      void
		 */
	void operator=( const class CNCSJPCMarker& src );
};

#endif // !NCSJPCMARKER_H
