/********************************************************
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
** FILE:    NCSExtent.cpp
** CREATED: Thu Feb 25 09:19:00 WST 1999
** AUTHOR:  Matthew Bardsley
** PURPOSE: Implementation of the NCSExtent class
**			The CNCSCoordinateConverter class encapsulates all extents related conversion and
**			co-ordinate routines.  This class can be used to store co-ordinates in
**			three formats these are :
**				World:		Is the World Value in the Units specified
**				Dataset:	Is the Dataset value in Dataset Pixels
**				Screen:		Is the Screen values in screen pixels
** EDITS:
**	
*******************************************************/

#include "NCSCoordinateConverter.h"

CNCSCoordinateConverter::CNCSCoordinateConverter()
{
	m_pWorldExtent = new CNCSExtents;

	if(m_pWorldExtent)
	{
		m_pWorldExtent->m_pTL->m_dX = 0.0L;
		m_pWorldExtent->m_pTL->m_dY = 0.0L;
		m_pWorldExtent->m_pBR->m_dX = 1.0L;
		m_pWorldExtent->m_pBR->m_dY = 1.0L;
	}

	m_pScreenExtent = new CNCSExtents;

	if(m_pScreenExtent)
	{
		m_pScreenExtent->m_pTL->m_dX = 0;
		m_pScreenExtent->m_pTL->m_dY = 0;
		m_pScreenExtent->m_pBR->m_dX = 1;
		m_pScreenExtent->m_pBR->m_dY = 1;
	}

	m_pWorldCoordSys = new CNCSCoordinateSystem;

}

CNCSCoordinateConverter::~CNCSCoordinateConverter()
{
	if(m_pWorldCoordSys)
		delete m_pWorldCoordSys;

	if(m_pScreenExtent)
		delete m_pScreenExtent;

	if(m_pWorldExtent)
		delete m_pWorldExtent;

}

CNCSCoordinateConverter &CNCSCoordinateConverter::operator=(CNCSCoordinateConverter &pExt)
{
	m_pWorldExtent->m_pTL->m_dX = pExt.m_pWorldExtent->m_pTL->m_dX;
	m_pWorldExtent->m_pTL->m_dY = pExt.m_pWorldExtent->m_pTL->m_dY;
	m_pWorldExtent->m_pBR->m_dX = pExt.m_pWorldExtent->m_pBR->m_dX;
	m_pWorldExtent->m_pBR->m_dY = pExt.m_pWorldExtent->m_pBR->m_dY;

	m_pScreenExtent->m_pTL->m_dX = pExt.m_pScreenExtent->m_pTL->m_dX;
	m_pScreenExtent->m_pTL->m_dY = pExt.m_pScreenExtent->m_pTL->m_dY;
	m_pScreenExtent->m_pBR->m_dX = pExt.m_pScreenExtent->m_pBR->m_dX;
	m_pScreenExtent->m_pBR->m_dY = pExt.m_pScreenExtent->m_pBR->m_dY;

	*m_pWorldCoordSys = *pExt.m_pWorldCoordSys;

	return *this;
}


NCSError CNCSCoordinateConverter::ConvertAllFromScreen(void)
{
	ScreenToWorld(m_pScreenExtent->m_pTL->m_dX,m_pScreenExtent->m_pTL->m_dY,&m_pWorldExtent->m_pTL->m_dX,&m_pWorldExtent->m_pTL->m_dY);
	ScreenToWorld(m_pScreenExtent->m_pBR->m_dX,m_pScreenExtent->m_pBR->m_dY,&m_pWorldExtent->m_pBR->m_dX,&m_pWorldExtent->m_pBR->m_dY);
	
	return(NCS_SUCCESS);
}

NCSError CNCSCoordinateConverter::SetScreenExtents(IEEE8 dSTLX, IEEE8 dSTLY, IEEE8 dSBRX,IEEE8 dSBRY)
{
	m_pScreenExtent->m_pTL->m_dX = dSTLX;
	m_pScreenExtent->m_pTL->m_dY = dSTLY;
	m_pScreenExtent->m_pBR->m_dX = dSBRX;
	m_pScreenExtent->m_pBR->m_dY = dSBRY;

	return(NCS_SUCCESS);
}

NCSError CNCSCoordinateConverter::SetWorldExtents(IEEE8 dWTLX,IEEE8 dWTLY, IEEE8 dWBRX,IEEE8 dWBRY)
{
	m_pWorldExtent->m_pTL->m_dX = dWTLX;
	m_pWorldExtent->m_pTL->m_dY = dWTLY;
	m_pWorldExtent->m_pBR->m_dX = dWBRX;
	m_pWorldExtent->m_pBR->m_dY = dWBRY;

	return(NCS_SUCCESS);
}

NCSError CNCSCoordinateConverter::SetAllExtents(IEEE8 dSTLX,IEEE8 dSTLY,
								  IEEE8 dSBRX,IEEE8 dSBRY,
								  IEEE8 dWTLX,IEEE8 dWTLY,
								  IEEE8 dWBRX,IEEE8 dWBRY)
{
	SetScreenExtents(dSTLX,dSTLY,dSBRX,dSBRY);
	SetWorldExtents(dWTLX,dWTLY,dWBRX,dWBRY);
	return(NCS_SUCCESS);
}

NCSError CNCSCoordinateConverter::SetWorldCoordSys( CNCSCoordinateSystem *pCoordSys)
{
	*m_pWorldCoordSys = *pCoordSys;

	return NCS_SUCCESS;
}


NCSError CNCSCoordinateConverter::ScreenToWorld(IEEE8 dX, IEEE8 dY, IEEE8 *pdX, IEEE8 *pdY)
{
	//check divisor for zero case.
	if((m_pScreenExtent->m_pBR->m_dX - m_pScreenExtent->m_pTL->m_dX) == 0.0)
		return NCS_EXTENT_ERROR;

	dX -= m_pScreenExtent->m_pTL->m_dX;
	dY -= m_pScreenExtent->m_pTL->m_dY;

	//perform conversion
	*pdX = m_pWorldExtent->m_pTL->m_dX + ((m_pWorldExtent->m_pBR->m_dX - m_pWorldExtent->m_pTL->m_dX)/(m_pScreenExtent->m_pBR->m_dX - m_pScreenExtent->m_pTL->m_dX)) * dX;
	*pdY = m_pWorldExtent->m_pTL->m_dY + ((m_pWorldExtent->m_pBR->m_dY - m_pWorldExtent->m_pTL->m_dY)/(m_pScreenExtent->m_pBR->m_dY - m_pScreenExtent->m_pTL->m_dY)) * dY;

	return(NCS_SUCCESS);
}

NCSError CNCSCoordinateConverter::WorldToScreen( IEEE8 dX, IEEE8 dY, IEEE8 *pdX, IEEE8 *pdY)
{
	//check divisor for zero case
	if((m_pWorldExtent->m_pBR->m_dX - m_pWorldExtent->m_pTL->m_dX) == 0.0)
		return NCS_EXTENT_ERROR;

	dX -= m_pWorldExtent->m_pTL->m_dX;
	dY -= m_pWorldExtent->m_pTL->m_dY;

	//perform conversion
	*pdX = m_pScreenExtent->m_pTL->m_dX + ((m_pScreenExtent->m_pBR->m_dX - m_pScreenExtent->m_pTL->m_dX)/(m_pWorldExtent->m_pBR->m_dX - m_pWorldExtent->m_pTL->m_dX)) * dX;
	*pdY = m_pScreenExtent->m_pTL->m_dY + ((m_pScreenExtent->m_pBR->m_dY - m_pScreenExtent->m_pTL->m_dY)/(m_pWorldExtent->m_pBR->m_dY - m_pWorldExtent->m_pTL->m_dY)) * dY;

	return(NCS_SUCCESS);
}

