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
**			The CNCSRasterCoordinateConverter class encapsulates all extents related conversion and
**			co-ordinate routines.  This class can be used to store co-ordinates in
**			three formats these are :
**				World:		Is the World Value in the Units specified
**				Dataset:	Is the Dataset value in Dataset Pixels
**				Screen:		Is the Screen values in screen pixels
** EDITS:
**	
*******************************************************/

#include "NCSCoordinateConverter.h"

CNCSRasterCoordinateConverter::CNCSRasterCoordinateConverter()
{
	m_pDatasetExtent = new CNCSExtents;

	if(m_pDatasetExtent)
	{
		m_pDatasetExtent->m_pTL->m_dX = 0;
		m_pDatasetExtent->m_pTL->m_dY = 0;
		m_pDatasetExtent->m_pBR->m_dX = 1;
		m_pDatasetExtent->m_pBR->m_dY = 1;
	}
}

CNCSRasterCoordinateConverter::~CNCSRasterCoordinateConverter()
{

	if(m_pDatasetExtent)
		delete m_pDatasetExtent;
}

CNCSRasterCoordinateConverter &CNCSRasterCoordinateConverter::operator=(CNCSRasterCoordinateConverter &pExt)
{
	CNCSCoordinateConverter::operator =(pExt);

	m_pDatasetExtent->m_pTL->m_dX = pExt.m_pDatasetExtent->m_pTL->m_dX;
	m_pDatasetExtent->m_pTL->m_dY = pExt.m_pDatasetExtent->m_pTL->m_dY;
	m_pDatasetExtent->m_pBR->m_dX = pExt.m_pDatasetExtent->m_pBR->m_dX;
	m_pDatasetExtent->m_pBR->m_dY = pExt.m_pDatasetExtent->m_pBR->m_dY;

	return *this;
}


NCSError CNCSRasterCoordinateConverter::ConvertAllFromScreen(void)
{

	CNCSCoordinateConverter::ConvertAllFromScreen();

	ScreenToDataset(m_pScreenExtent->m_pTL->m_dX,m_pScreenExtent->m_pTL->m_dY,&m_pDatasetExtent->m_pTL->m_dX,&m_pDatasetExtent->m_pTL->m_dY);
	ScreenToDataset(m_pScreenExtent->m_pBR->m_dX,m_pScreenExtent->m_pBR->m_dY,&m_pDatasetExtent->m_pBR->m_dX,&m_pDatasetExtent->m_pBR->m_dY);
	
	return(NCS_SUCCESS);
}


NCSError CNCSRasterCoordinateConverter::SetDatasetExtents(IEEE8 dDTLX,IEEE8 dDTLY, IEEE8 dDBRX,IEEE8 dDBRY)
{
	m_pDatasetExtent->m_pTL->m_dX = dDTLX;
	m_pDatasetExtent->m_pTL->m_dY = dDTLY;
	m_pDatasetExtent->m_pBR->m_dX = dDBRX;
	m_pDatasetExtent->m_pBR->m_dY = dDBRY;
	return(NCS_SUCCESS);
}


NCSError CNCSRasterCoordinateConverter::SetAllExtents(IEEE8 dSTLX,IEEE8 dSTLY,
								  IEEE8 dSBRX,IEEE8 dSBRY,
								  IEEE8 dDTLX,IEEE8 dDTLY,
								  IEEE8 dDBRX,IEEE8 dDBRY,
								  IEEE8 dWTLX,IEEE8 dWTLY,
								  IEEE8 dWBRX,IEEE8 dWBRY)
{
	//do base class then ours.
	CNCSCoordinateConverter::SetAllExtents(dSTLX,dSTLY,dSBRX, dSBRY,dWTLX,dWTLY,dWBRX,dWBRY);

	SetDatasetExtents(dDTLX,dDTLY,dDBRX,dDBRY);
	return(NCS_SUCCESS);
}

NCSError CNCSRasterCoordinateConverter::SetWorldCoordSys( CNCSCoordinateSystem *pCoordSys)
{
	*m_pWorldCoordSys = *pCoordSys;

	return NCS_SUCCESS;
}


NCSError CNCSRasterCoordinateConverter::WorldToDataset(IEEE8 dWX, IEEE8 dWY, IEEE8 *dDX, IEEE8 *dDY)
{
	//check divisor for zero case
	if((m_pWorldExtent->m_pBR->m_dX - m_pWorldExtent->m_pTL->m_dX) == 0.0)
		return NCS_EXTENT_ERROR;

	dWX -= m_pWorldExtent->m_pTL->m_dX;
	dWY -= m_pWorldExtent->m_pTL->m_dY;

	//perform conversion
	*dDX = m_pDatasetExtent->m_pTL->m_dX + ((m_pDatasetExtent->m_pBR->m_dX - m_pDatasetExtent->m_pTL->m_dX)/(m_pWorldExtent->m_pBR->m_dX - m_pWorldExtent->m_pTL->m_dX) * dWX);
	*dDY = m_pDatasetExtent->m_pTL->m_dY + ((m_pDatasetExtent->m_pBR->m_dY - m_pDatasetExtent->m_pTL->m_dY)/(m_pWorldExtent->m_pBR->m_dY - m_pWorldExtent->m_pTL->m_dY) * dWY);

	return(NCS_SUCCESS);
}

NCSError CNCSRasterCoordinateConverter::DatasetToWorld(IEEE8 dDX, IEEE8 dDY, IEEE8 *dWX, IEEE8 *dWY)
{
	//check divisor for zero case
	if((m_pDatasetExtent->m_pBR->m_dX - m_pDatasetExtent->m_pTL->m_dX) == 0.0)
		return NCS_EXTENT_ERROR;

	dDX -= m_pDatasetExtent->m_pTL->m_dX;
	dDY -= m_pDatasetExtent->m_pTL->m_dY;

	//perform conversion
	*dWX = m_pWorldExtent->m_pTL->m_dX + ((m_pWorldExtent->m_pBR->m_dX - m_pWorldExtent->m_pTL->m_dX)/(m_pDatasetExtent->m_pBR->m_dX - m_pDatasetExtent->m_pTL->m_dX) * dDX);
	*dWY = m_pWorldExtent->m_pTL->m_dY + ((m_pWorldExtent->m_pBR->m_dY - m_pWorldExtent->m_pTL->m_dY)/(m_pDatasetExtent->m_pBR->m_dY - m_pDatasetExtent->m_pTL->m_dY) * dDY);

	return(NCS_SUCCESS);
}

NCSError CNCSRasterCoordinateConverter::DatasetToScreen(IEEE8 dX, IEEE8 dY, IEEE8 *sX, IEEE8 *sY)
{
	//check divisor for zero case
	if((m_pDatasetExtent->m_pBR->m_dX - m_pDatasetExtent->m_pTL->m_dX) == 0.0)
		return NCS_EXTENT_ERROR;

	dX -= m_pDatasetExtent->m_pTL->m_dX;
	dY -= m_pDatasetExtent->m_pTL->m_dY;

	//perform conversion
	*sX = m_pScreenExtent->m_pTL->m_dX + (((IEEE8)(m_pScreenExtent->m_pBR->m_dX - m_pScreenExtent->m_pTL->m_dX)/(m_pDatasetExtent->m_pBR->m_dX - m_pDatasetExtent->m_pTL->m_dX)) * dX);
	*sY = m_pScreenExtent->m_pTL->m_dY + (((IEEE8)(m_pScreenExtent->m_pBR->m_dY - m_pScreenExtent->m_pTL->m_dY)/(m_pDatasetExtent->m_pBR->m_dY - m_pDatasetExtent->m_pTL->m_dY)) * dY);

	return(NCS_SUCCESS);
}

NCSError CNCSRasterCoordinateConverter::ScreenToDataset( IEEE8 dX, IEEE8 dY, IEEE8 *pdX, IEEE8 *pdY)
{
	//check divisor for zero case
	if((m_pScreenExtent->m_pBR->m_dX - m_pScreenExtent->m_pTL->m_dX) == 0.0)
		return NCS_EXTENT_ERROR;

	dX -= m_pScreenExtent->m_pTL->m_dX;
	dY -= m_pScreenExtent->m_pTL->m_dY;

	*pdX = m_pDatasetExtent->m_pTL->m_dX + ((m_pDatasetExtent->m_pBR->m_dX - m_pDatasetExtent->m_pTL->m_dX)/(m_pScreenExtent->m_pBR->m_dX - m_pScreenExtent->m_pTL->m_dX)) * dX;
	*pdY = m_pDatasetExtent->m_pTL->m_dY + ((m_pDatasetExtent->m_pBR->m_dY - m_pDatasetExtent->m_pTL->m_dY)/(m_pScreenExtent->m_pBR->m_dY - m_pScreenExtent->m_pTL->m_dY)) * dY;

	return(NCS_SUCCESS);
}

