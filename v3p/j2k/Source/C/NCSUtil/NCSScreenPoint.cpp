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
** FILE:   	CNCSScreenPoint.cpp
** CREATED:	Wed Oct 18 2000
** AUTHOR: 	Jeff Pudwell
** PURPOSE:	
**		
** EDITS:
*******************************************************/


#include "NCSUtil.h"
#include "NCSScreenPoint.h"


CNCSScreenPoint::CNCSScreenPoint()
{
	x = y = 0;
}

CNCSScreenPoint::CNCSScreenPoint(INT32 nScreenX, INT32 nScreenY)
{
	x = nScreenX;
	y = nScreenY;
}

INT32 CNCSScreenPoint::Width(CNCSScreenPoint nPoint)
{
	INT32 width;

	width = (x - nPoint.x);
	if (width < 0)
		width *= -1;

	return width;
}

INT32 CNCSScreenPoint::Height(CNCSScreenPoint nPoint)
{
	INT32 height;

	height = (y - nPoint.y);
	if (height < 0)
		height *= -1;

	return height;
}
