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
** FILE:		NCSExtent.cpp
** CREATED:		18 Jan 2000
** AUTHOR:		Matthew Bardsley
** PURPOSE:		Holds implementation of NCSExtent Class
** EDITS:
**
** CLASS NOTES:
***************************************************************************/

#include "NCSUtil.h"
#include "NCSExtents.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSExtents::CNCSExtents(IEEE8 dTLX,IEEE8 dTLY,IEEE8 dBRX, IEEE8 dBRY)
{
	m_pTL = new(CNCSPoint);
	m_pBR = new(CNCSPoint);

	m_pTL->m_dX = dTLX;
	m_pTL->m_dY = dTLY;

	m_pBR->m_dX = dBRX;
	m_pBR->m_dY = dBRY;
}

CNCSExtents::~CNCSExtents()
{
	if(m_pTL)
		delete(m_pTL);
	if(m_pBR)
		delete(m_pBR);
}
