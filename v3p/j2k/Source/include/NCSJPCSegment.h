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
** FILE:     $Archive: /NCS/Source/include/NCSJPCSegment.h $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPC Segment class
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCSEGMENT_H
#define NCSJPCSEGMENT_H

#ifndef NCSJPCTYPES_H
#include "NCSJPCTypes.h"
#endif // NCSJPCTYPES_H

// Segment class
class NCSJPC_EXPORT_ALL CNCSJPCSegment {
public:
		/** Segment length */
	UINT32	m_nLength;
		/** Segment Index */
	UINT16	m_nIndex;
		/** Number of passes in this segment */
	UINT8	m_nPasses;
		/** Segment data */
	UINT8	*m_pData;
	
		/** Default constructor */
	CNCSJPCSegment();
		/** Copy constructor */
	CNCSJPCSegment(const CNCSJPCSegment &s);
		/** Virtual destructor */
	virtual ~CNCSJPCSegment();
};

#endif // NCSJPCSEGMENT_H
