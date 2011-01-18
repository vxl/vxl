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
** CLASS(ES):	CNCSPoint
** FILE:		NCSPoint.h
** CREATED:		18 Jan 2000
** AUTHOR:		Matthew Bardsley
** PURPOSE:		Holds definition of NCSPoint Class
** EDITS:
**
** CLASS NOTES:
***************************************************************************/
#if !defined(NCSPOINT_H)
#define NCSPOINT_H

#include "NCSTypes.h"
#include "NCSDefs.h"

class NCS_EXPORT CNCSPoint  
{
public:
	CNCSPoint(IEEE8 dX = 0.0,IEEE8 dY = 0.0,IEEE8 dZ = NCS_FQNAN);
	virtual ~CNCSPoint();

	CNCSPoint &operator=(CNCSPoint &Point);
	CNCSPoint &operator+(CNCSPoint &Point);
	CNCSPoint &operator-(CNCSPoint &Point);
	CNCSPoint &operator*(CNCSPoint &Point);
	CNCSPoint &operator/(CNCSPoint &Point);

	IEEE8 m_dX;
	IEEE8 m_dY;
	IEEE8 m_dZ;

};

#endif // !defined(NCSPOINT_H)
