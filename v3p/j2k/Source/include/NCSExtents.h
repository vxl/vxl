/***************************************************************************
** Copyright 1989-2000 - Earth Resource Mapping Ltd.
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
** CLASS(ES):	CNCSExtent
** FILE:		NCSExtent.h
** CREATED:		18 Jan 2000
** AUTHOR:		Matthew Bardsley
** PURPOSE:		Holds definition of NCSExtent Class
** EDITS:
**
** CLASS NOTES:
***************************************************************************/
#if !defined(NCSEXTENT_H)
#define NCSEXTENT_H

#include "NCSPoint.h"

class NCS_EXPORT CNCSExtents  
{
public:
	CNCSExtents(IEEE8 tlx = 0.0,
			   IEEE8 tly = 0.0,
			   IEEE8 brx = 0.0,
			   IEEE8 bry = 0.0);
	virtual ~CNCSExtents();

	CNCSPoint *m_pTL;
	CNCSPoint *m_pBR;
};

#endif // !defined(NCSEXTENT_H)
