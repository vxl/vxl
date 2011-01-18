/***************************************************************************
// Copyright 1989-2000 - Earth Resource Mapping Ltd.
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
** CLASS(ES):	CNCSRasterCoordinateConverter
** FILE:		NCSSystemcoords.h
** CREATED:		18 Jan 2000
** AUTHOR:		Matthew Bardsley
** PURPOSE:		Holds definition of CNCSRasterCoordinateConverter Class
** EDITS:
**
** CLASS NOTES:
***************************************************************************/
#if !defined(NCSSYSTEMCOORDS_H)
#define NCSSYSTEMCOORDS_H

#include "NCSUtil.h"
#include "NCSExtents.h"
#include "NCSCoordinateSystem.h"

class NCS_EXPORT CNCSCoordinateConverter
{
public:
	CNCSCoordinateConverter();
	 ~CNCSCoordinateConverter();

	CNCSCoordinateConverter &operator=(CNCSCoordinateConverter &pExt);

	NCSError ConvertAllFromScreen(void);
	NCSError SetScreenExtents(IEEE8 dSTLX, IEEE8 dSTLY, IEEE8 dSBRX,IEEE8 dSBRY);
	NCSError SetWorldExtents(IEEE8 dWTLX,IEEE8 dWTLY, IEEE8 dWBRX,IEEE8 dWBRY);

	NCSError SetAllExtents(IEEE8 dSTLX,IEEE8 dSTLY,
									  IEEE8 dSBRX,IEEE8 dSBRY,
									  IEEE8 dWTLX,IEEE8 dWTLY,
									  IEEE8 dWBRX,IEEE8 dWBRY);

	NCSError ScreenToWorld(IEEE8 dX, IEEE8 dY, IEEE8 *pdX, IEEE8 *pdY);
	NCSError WorldToScreen( IEEE8 dX, IEEE8 dY, IEEE8 *pdX, IEEE8 *pdY);

	NCSError SetWorldCoordSys( CNCSCoordinateSystem *pCoordSys);

	CNCSExtents *m_pWorldExtent;
	CNCSExtents *m_pScreenExtent;
	
	CNCSCoordinateSystem *m_pWorldCoordSys;

};

class NCS_EXPORT CNCSRasterCoordinateConverter : public CNCSCoordinateConverter
{

public:
	CNCSRasterCoordinateConverter();
	 ~CNCSRasterCoordinateConverter();

	CNCSRasterCoordinateConverter &operator=(CNCSRasterCoordinateConverter &pExt);
	NCSError ConvertAllFromScreen(void);

	NCSError SetAllExtents(IEEE8 dSTLX,IEEE8 dSTLY,
									  IEEE8 dSBRX,IEEE8 dSBRY,
									  IEEE8 dDTLX,IEEE8 dTLY,
									  IEEE8 dDBRX,IEEE8 dBRY,
									  IEEE8 dWTLX,IEEE8 dWTLY,
									  IEEE8 dWBRX,IEEE8 dWBRY);

	NCSError SetDatasetExtents(IEEE8 dDTLX,IEEE8 dDTLY, IEEE8 dDBRX,IEEE8 dDBRY);

	NCSError WorldToDataset(IEEE8 dWX, IEEE8 dWY, IEEE8 *dDX, IEEE8 *dDY);
	NCSError DatasetToWorld(IEEE8 dDX, IEEE8 dDY, IEEE8 *dWX, IEEE8 *dWY);
	NCSError DatasetToScreen(IEEE8 dX, IEEE8 dY, IEEE8 *sX, IEEE8 *sY);
	NCSError ScreenToDataset(IEEE8 dX, IEEE8 dY, IEEE8 *pdX, IEEE8 *pdY);

	NCSError SetWorldCoordSys( CNCSCoordinateSystem *pCoordSys);

	CNCSExtents *m_pDatasetExtent;

};

#endif // !defined(NCSSYSTEMCOORDS_H)
