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
** FILE:   	CNCSScreenPoint.h
** CREATED:	Wed Oct 18 2000
** AUTHOR: 	Jeff Pudwell
** PURPOSE:	
**		
** EDITS:
*******************************************************/


#ifndef NCSSCREENPOINT_H_
#define NCSSCREENPOINT_H_

#include "NCSTypes.h"
#include "NCSDefs.h"


class NCS_EXPORT CNCSScreenPoint
{
	public:
		INT32 x;
		INT32 y;

		CNCSScreenPoint();
		CNCSScreenPoint(INT32 nScreenX, INT32 nScreenY);

		INT32 Width(CNCSScreenPoint nPoint);
		INT32 Height(CNCSScreenPoint nPoint);

		bool operator==(CNCSScreenPoint &pPoint)
		{
			if (x == pPoint.x && y == pPoint.y)
				return true;
			else
				return false;
		}

		bool operator!=(CNCSScreenPoint &pPoint)
		{
			if (x == pPoint.x && y == pPoint.y)
				return false;
			else
				return true;
		}
};

#endif
