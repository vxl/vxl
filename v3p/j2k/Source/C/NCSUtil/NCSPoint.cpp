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
** FILE:		NCSPoint.cpp
** CREATED:		18 Jan 2000
** AUTHOR:		Matthew Bardsley
** PURPOSE:		Holds implementation of NCSPoint Class
** EDITS:
**
** CLASS NOTES:
***************************************************************************/

#include "NCSUtil.h"
#include "NCSPoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSPoint::	CNCSPoint(IEEE8 dX,IEEE8 dY,IEEE8 dZ)
{
	m_dX = dX;
	m_dY = dY;
	m_dZ = dZ;
}

CNCSPoint::~CNCSPoint()
{

}

//////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////

CNCSPoint &CNCSPoint::operator=(CNCSPoint &Point)
{
	m_dX = Point.m_dX;
	m_dY = Point.m_dY;
	m_dZ = Point.m_dZ;

	return *this;
}

CNCSPoint &CNCSPoint::operator+(CNCSPoint &Point)
{
	m_dX += Point.m_dX;
	m_dY += Point.m_dY;
	m_dZ += Point.m_dZ;

	return *this;
}

CNCSPoint &CNCSPoint::operator-(CNCSPoint &Point)
{
	m_dX -= Point.m_dX;
	m_dY -= Point.m_dY;
	m_dZ -= Point.m_dZ;

	return *this;
}

CNCSPoint &CNCSPoint::operator*(CNCSPoint &Point)
{
	m_dX *= Point.m_dX;
	m_dY *= Point.m_dY;
	m_dZ *= Point.m_dZ;

	return *this;
}

CNCSPoint &CNCSPoint::operator/(CNCSPoint &Point)
{
	m_dX /= Point.m_dX;
	m_dY /= Point.m_dY;
	m_dZ /= Point.m_dZ;

	return *this;
}
