/*******************************************************
** Copyright 1999 Earth Resource Mapping Ltd.
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
** FILE:   	CNCSWorldPoint.h
** CREATED:	Wed Oct 18 2000
** AUTHOR: 	Jeff Pudwell
** PURPOSE:	
**		
** EDITS:
*******************************************************/


#ifndef NCSWORLDPOINT_H_
#define NCSWORLDPOINT_H_

#include "NCSTypes.h"
#include "NCSDefs.h"


class NCS_EXPORT CNCSWorldPoint
{
	public:
		IEEE8 x;
		IEEE8 y;
		IEEE8 z;

		CNCSWorldPoint();
		CNCSWorldPoint(IEEE8 dWorldX, IEEE8 dWorldY);
		CNCSWorldPoint(IEEE8 dWorldX, IEEE8 dWorldY, IEEE8 dWorldZ);

		IEEE8 Width(CNCSWorldPoint dPoint);
		IEEE8 Height(CNCSWorldPoint dPoint);

		bool operator==(CNCSWorldPoint &pPoint)
		{
			if (x == pPoint.x && y == pPoint.y && z == pPoint.z)
				return true;
			else
				return false;
		}

		bool operator!=(CNCSWorldPoint &pPoint)
		{
			if (x == pPoint.x && y == pPoint.y && z == pPoint.z)
				return false;
			else
				return true;
		}
};

#endif


