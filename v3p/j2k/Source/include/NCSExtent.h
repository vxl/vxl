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
** FILE:    NCSExtent.h
** CREATED: Thu Feb 25 09:19:00 WST 1999
** AUTHOR:  Mark Sheridan
** PURPOSE: Generic extent class
** EDITS:
 *******************************************************/

#ifndef NCSEXTENT_H
#define NCSEXTENT_H

#include "NCSUtil.h"
#include "NCSTypes.h"
#include "NCSErrors.h"
#include "NCSDefs.h"

#if (defined(WIN32) && !defined(_WIN32_WCE))
#ifdef _DEBUG
#include <crtdbg.h>
#endif
#endif

class __declspec( dllexport ) CNCSExtent 
{
public:
	CNCSExtent();
	virtual ~CNCSExtent();

	NCSError ConvertAllFromScreen(void);

	NCSError SetScreenExtents(double dSTLX, double dSTLY, double dSBRX,double dSBRY);
	NCSError SetDatasetExtents(double dDTLX, double dDTLY, double dDBRX, double dDBRY);
	NCSError SetWorldExtents(double dRTLX, double dRTLY, double dRBRX, double dRBRY);
	NCSError SetAllExtents(double dSTLX, double dSTLY, double dSBRX, double dSBRY, 
							double dDTLX, double dDTLY, double dDBRX, double dDBRY,
							double dRTLX, double dRTLY, double dRBRX, double dRBRY);
	NCSError WorldToDataset( double dRX, double dRY, double *dDX, double *dDY);
	NCSError DatasetToWorld( double dDX, double dDY, double *dRX, double *dRY);
	NCSError DatasetToScreen(double dX, double dY, double *pX, double *pY);
	NCSError ScreenToDataset(double x, double y, double *dX, double *dY);
	NCSError ScreenToWorld(  double dX, double dY, double *pdX, double *pdY);
	NCSError WorldToScreen(  double dX, double dY, double *pdX, double *pdY);

	CNCSExtent &operator=(CNCSExtent &pExt);

	UINT32 m_dwFlags;
	UINT32 m_dwUnits;

	double m_dDatasetTLX;
	double m_dDatasetTLY;
	double m_dDatasetBRX;
	double m_dDatasetBRY;

	double m_dWorldTLX;
	double m_dWorldTLY;
	double m_dWorldBRX;
	double m_dWorldBRY;

	double m_dScreenTLX;
	double m_dScreenTLY;
	double m_dScreenBRX;
	double m_dScreenBRY;

	/*Scale values for transformations*/
	double m_dScaleX;
	double m_dScaleY;
	double m_dTransX;
	double m_dTransY;
};

#endif
