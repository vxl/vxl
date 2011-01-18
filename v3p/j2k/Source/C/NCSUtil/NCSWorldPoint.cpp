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
** FILE:   	CNCSWorldPoint.cpp
** CREATED:	Wed Oct 18 2000
** AUTHOR: 	Jeff Pudwell
** PURPOSE:	
**		
** EDITS:
*******************************************************/


#include "NCSUtil.h"
#include "NCSWorldPoint.h"


CNCSWorldPoint::CNCSWorldPoint()
{
	x = y = z = 0.0;//NCSNanVal();
}

CNCSWorldPoint::CNCSWorldPoint(IEEE8 dWorldX, IEEE8 dWorldY)
{
	x = dWorldX;
	y = dWorldY;
	z = 0.0;
}

CNCSWorldPoint::CNCSWorldPoint(IEEE8 dWorldX, IEEE8 dWorldY, IEEE8 dWorldZ)
{
	x = dWorldX;
	y = dWorldY;
	z = dWorldZ;
}

IEEE8 CNCSWorldPoint::Width(CNCSWorldPoint dPoint)
{
	IEEE8 width;

	width = (x - dPoint.x);
	if (width < 0)
		width *= -1.0;

	return width;
}

IEEE8 CNCSWorldPoint::Height(CNCSWorldPoint dPoint)
{
	IEEE8 height;

	height = (y - dPoint.y);
	if (height < 0)
		height *= -1.0;

	return height;
}
