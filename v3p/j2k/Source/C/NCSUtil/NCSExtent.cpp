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
**			The CNCSExtent class encapsulates all extents related conversion and
**			co-ordinate routines.  This class can be used to store co-ordinates in
**			three formats these are :
**				World:		Is the World Value in the Units specified
**				Dataset:	Is the Dataset value in Dataset Pixels
**				Screen:		Is the Screen values in screen pixels
** EDITS:
**	19Nov99	mjs	Changed class name, made into a common class that is inherited by
**				the COM class CComNCSExtent and moved into NCSUtil library.
*******************************************************/

#include "NCSExtent.h"

CNCSExtent::CNCSExtent()
{
	//Set Extents Defaults
	m_dDatasetTLX = 0;
	m_dDatasetTLY = 0;
	m_dDatasetBRX = 1;
	m_dDatasetBRY = 1;

	m_dWorldTLX = 0.0L;
	m_dWorldTLY = 0.0L;
	m_dWorldBRX = 0.0L;
	m_dWorldBRY = 0.0L;

	m_dScreenTLX = 0;
	m_dScreenTLY = 0;
	m_dScreenBRX = 1;
	m_dScreenBRY = 1;

	//Set Scale Defaults
	m_dScaleX = 1.0L;
	m_dScaleY = 1.0L;
	m_dTransX = 0.0L;
	m_dTransY = 0.0L;
}

CNCSExtent &CNCSExtent::operator=(CNCSExtent &pExt)
{
	m_dDatasetTLX = pExt.m_dDatasetTLX;
	m_dDatasetTLY = pExt.m_dDatasetTLY;
	m_dDatasetBRX = pExt.m_dDatasetBRX;
	m_dDatasetBRY = pExt.m_dDatasetBRY;

	m_dWorldTLX = pExt.m_dWorldTLX;
	m_dWorldTLY = pExt.m_dWorldTLY;
	m_dWorldBRX = pExt.m_dWorldBRX;
	m_dWorldBRY = pExt.m_dWorldBRY;

	m_dScreenTLX = pExt.m_dScreenTLX;
	m_dScreenTLY = pExt.m_dScreenTLY;
	m_dScreenBRX = pExt.m_dScreenBRX;
	m_dScreenBRY = pExt.m_dScreenBRY;

	m_dScaleX = pExt.m_dScaleX;
	m_dScaleY = pExt.m_dScaleY;
	m_dTransX = pExt.m_dTransX;
	m_dTransY = pExt.m_dTransY;
	
	m_dwUnits = pExt.m_dwUnits;
	m_dwFlags = pExt.m_dwFlags;

	return *this;
}

CNCSExtent::~CNCSExtent()
{
}

NCSError CNCSExtent::ConvertAllFromScreen(void)
{
	ScreenToDataset(m_dScreenTLX,m_dScreenTLY,&m_dDatasetTLX,&m_dDatasetTLY);
	ScreenToDataset(m_dScreenBRX,m_dScreenBRY,&m_dDatasetBRX,&m_dDatasetBRY);

	ScreenToWorld(m_dScreenTLX,m_dScreenTLY,&m_dWorldTLX,&m_dWorldTLY);
	ScreenToWorld(m_dScreenBRX,m_dScreenBRY,&m_dWorldBRX,&m_dWorldBRY);
	
	return(NCS_SUCCESS);
}

NCSError CNCSExtent::SetScreenExtents(double dSTLX, double dSTLY, double dSBRX,double dSBRY)
{
	m_dScreenTLX = dSTLX;
	m_dScreenTLY = dSTLY;
	m_dScreenBRX = dSBRX;
	m_dScreenBRY = dSBRY;

	return(NCS_SUCCESS);
}

NCSError CNCSExtent::SetDatasetExtents(double dDTLX,double dDTLY, double dDBRX,double dDBRY)
{
	m_dDatasetTLX = dDTLX;
	m_dDatasetTLY = dDTLY;
	m_dDatasetBRX = dDBRX;
	m_dDatasetBRY = dDBRY;
	return(NCS_SUCCESS);
}

NCSError CNCSExtent::SetWorldExtents(double dWTLX,double dWTLY, double dWBRX,double dWBRY)
{
	m_dWorldTLX = dWTLX;
	m_dWorldTLY = dWTLY;
	m_dWorldBRX = dWBRX;
	m_dWorldBRY = dWBRY;

	return(NCS_SUCCESS);
}

NCSError CNCSExtent::SetAllExtents(double dSTLX,double dSTLY,
								  double dSBRX,double dSBRY,
								  double dDTLX,double dDTLY,
								  double dDBRX,double dDBRY,
								  double dWTLX,double dWTLY,
								  double dWBRX,double dWBRY)
{
	SetScreenExtents(dSTLX,dSTLY,dSBRX,dSBRY);
	SetDatasetExtents(dDTLX,dDTLY,dDBRX,dDBRY);
	SetWorldExtents(dWTLX,dWTLY,dWBRX,dWBRY);
	return(NCS_SUCCESS);
}

NCSError CNCSExtent::WorldToDataset(double dWX, double dWY, double *dDX, double *dDY)
{
	//check divisor for zero case
	if((m_dWorldBRX - m_dWorldTLX) == 0.0)
		return NCS_EXTENT_ERROR;

	dWX -= m_dWorldTLX;
	dWY -= m_dWorldTLY;

	//perform conversion
	*dDX = m_dDatasetTLX + ((m_dDatasetBRX - m_dDatasetTLX)/(m_dWorldBRX - m_dWorldTLX) * dWX);
	*dDY = m_dDatasetTLY + ((m_dDatasetBRY - m_dDatasetTLY)/(m_dWorldBRY - m_dWorldTLY) * dWY);

	return(NCS_SUCCESS);
}

NCSError CNCSExtent::DatasetToWorld(double dDX, double dDY, double *dWX, double *dWY)
{
	//check divisor for zero case
	if((m_dDatasetBRX - m_dDatasetTLX) == 0.0)
		return NCS_EXTENT_ERROR;

	dDX -= m_dDatasetTLX;
	dDY -= m_dDatasetTLY;

	//perform conversion
	*dWX = m_dWorldTLX + ((m_dWorldBRX - m_dWorldTLX)/(m_dDatasetBRX - m_dDatasetTLX) * dDX);
	*dWY = m_dWorldTLY + ((m_dWorldBRY - m_dWorldTLY)/(m_dDatasetBRY - m_dDatasetTLY) * dDY);

	return(NCS_SUCCESS);
}

NCSError CNCSExtent::DatasetToScreen(double dX, double dY, double *sX, double *sY)
{
	//check divisor for zero case
	if((m_dDatasetBRX - m_dDatasetTLX) == 0.0)
		return NCS_EXTENT_ERROR;

	dX -= m_dDatasetTLX;
	dY -= m_dDatasetTLY;

	//perform conversion
	*sX = m_dScreenTLX + (((double)(m_dScreenBRX - m_dScreenTLX)/(m_dDatasetBRX - m_dDatasetTLX)) * dX);
	*sY = m_dScreenTLY + (((double)(m_dScreenBRY - m_dScreenTLY)/(m_dDatasetBRY - m_dDatasetTLY)) * dY);

	return(NCS_SUCCESS);
}

NCSError CNCSExtent::ScreenToDataset( double dX, double dY, double *pdX, double *pdY)
{
	//check divisor for zero case
	if((m_dScreenBRX - m_dScreenTLX) == 0.0)
		return NCS_EXTENT_ERROR;

	dX -= m_dScreenTLX;
	dY -= m_dScreenTLY;

	*pdX = m_dDatasetTLX + ((m_dDatasetBRX - m_dDatasetTLX)/(m_dScreenBRX - m_dScreenTLX)) * dX;
	*pdY = m_dDatasetTLY + ((m_dDatasetBRY - m_dDatasetTLY)/(m_dScreenBRY - m_dScreenTLY)) * dY;

	return(NCS_SUCCESS);
}

NCSError CNCSExtent::ScreenToWorld(double dX, double dY, double *pdX, double *pdY)
{
	//check divisor for zero case.
	if((m_dScreenBRX - m_dScreenTLX) == 0.0)
		return NCS_EXTENT_ERROR;

	dX -= m_dScreenTLX;
	dY -= m_dScreenTLY;

	//perform conversion
	*pdX = m_dWorldTLX + ((m_dWorldBRX - m_dWorldTLX)/(m_dScreenBRX - m_dScreenTLX)) * dX;
	*pdY = m_dWorldTLY + ((m_dWorldBRY - m_dWorldTLY)/(m_dScreenBRY - m_dScreenTLY)) * dY;

	return(NCS_SUCCESS);
}

NCSError CNCSExtent::WorldToScreen( double dX, double dY, double *pdX, double *pdY)
{
	//check divisor for zero case
	if((m_dWorldBRX - m_dWorldTLX) == 0.0)
		return NCS_EXTENT_ERROR;

	dX -= m_dWorldTLX;
	dY -= m_dWorldTLY;

	//perform conversion
	*pdX = m_dScreenTLX + ((m_dScreenBRX - m_dScreenTLX)/(m_dWorldBRX - m_dWorldTLX)) * dX;
	*pdY = m_dScreenTLY + ((m_dScreenBRY - m_dScreenTLY)/(m_dWorldBRY - m_dWorldTLY)) * dY;

	return(NCS_SUCCESS);
}

