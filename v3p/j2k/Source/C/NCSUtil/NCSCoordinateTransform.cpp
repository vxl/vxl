/***************************************************************************
**
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
** CLASS(ES):	CNCSCoordinateTramsform
** FILE:		NCSCoordinateTransform.cpp
** CREATED:		18 Jan 2000
** AUTHOR:		Matthew Bardsley
** PURPOSE:		Holds definition of NCSCoordinateTransform Class
** EDITS:
** [01]	 ny	31Oct00	Merge WinCE/PALM SDK changes
**
** CLASS NOTES:
***************************************************************************/

#include "NCSUtil.h"
#include "NCSCoordinateTransform.h"
#ifdef MACINTOSH
#include <string.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSCoordinateTransform::CNCSCoordinateTransform(CNCSCoordinateSystem *pSrc,
												 CNCSCoordinateSystem *pDest)
{
	m_pSrcCoordSys = new(CNCSCoordinateSystem);
	m_pDestCoordSys = new(CNCSCoordinateSystem);

	if(pSrc)
		*m_pSrcCoordSys = *pSrc;
	if(pDest)
		*m_pDestCoordSys = *pDest;

}

CNCSCoordinateTransform::~CNCSCoordinateTransform()
{
	if(m_pSrcCoordSys)
		delete(m_pSrcCoordSys);

	if(m_pDestCoordSys)
		delete(m_pDestCoordSys);
}


//***************************************************************************
// METHOD:		GetSrcCoordSys(CNCSCoordinateSystem *pCoord)
// PURPOSE:		To set the pCoord class with the same values as the 
//				m_pSrcCoordSys
// PARAMETERS:	CNCSCoordinateSystem *pCoord  //Pointer to class to recieve
//
// RETURNS:		Always returns NCS_SUCCESS
// NOTES:
//***************************************************************************
NCSError CNCSCoordinateTransform::GetSrcCoordSys(CNCSCoordinateSystem *pCoord)
{
	NCSError nRet= NCS_SUCCESS;

	*pCoord = *m_pSrcCoordSys;

	return nRet;
}

//***************************************************************************
// METHOD:		SetSrcCoordSys(CNCSCoordinateSystem *pCoord)
// PURPOSE:		To set the m_pSrcCoord class with the same values as the 
//				pCoord
// PARAMETERS:	CNCSCoordinateSystem *pCoord  //Pointer to class to copy
//
// RETURNS:		Always returns NCS_SUCCESS
// NOTES:
//***************************************************************************
NCSError CNCSCoordinateTransform::SetSrcCoordSys(CNCSCoordinateSystem *pCoord)
{
	NCSError nRet= NCS_SUCCESS;

	*m_pSrcCoordSys = *pCoord;

	return nRet;

}

//***************************************************************************
// METHOD:		GetDestCoordSys(CNCSCoordinateSystem *pCoord)
// PURPOSE:		To set the pCoord class with the same values as the 
//				m_pDestCoordSys
// PARAMETERS:	CNCSCoordinateSystem *pCoord  //Pointer to class to recieve
//
// RETURNS:		Always returns NCS_SUCCESS
// NOTES:
//***************************************************************************
NCSError CNCSCoordinateTransform::GetDestCoordSys( CNCSCoordinateSystem *pCoord)
{
	NCSError nRet= NCS_SUCCESS;

	*pCoord = *m_pDestCoordSys;

	return nRet;
}

//***************************************************************************
// METHOD:		SetDestCoordSys(CNCSCoordinateSystem *pCoord)
// PURPOSE:		To set the m_pDestCoord class with the same values as the 
//				pCoord
// PARAMETERS:	CNCSCoordinateSystem *pCoord  //Pointer to class to copy
//
// RETURNS:		Always returns NCS_SUCCESS
// NOTES:
//***************************************************************************
NCSError CNCSCoordinateTransform::SetDestCoordSys( CNCSCoordinateSystem *pCoord)
{
	NCSError nRet= NCS_SUCCESS;

	*m_pDestCoordSys = *pCoord;

	return nRet;
}

//***************************************************************************
// METHOD:		Transform(CNCSPoint *pSrc,CNCSPoint *pDest)
// PURPOSE:		To transform a point PSrc from the Source Projection to the
//				destination projection.
// PARAMETERS:	CNCSPoint *pSrc	 //pointer to the source point
//				CNCSPoint *pDest //pointer to the destination point
//
// RETURNS:		NCS_SUCCESS if transformation successful otherwise an NCS error
//				explaining why it was not successful.
//
// NOTES:		Classes derived from this class will perform coodinate
//				transformations from this function.  As we currently do not 
//				support projection mappings, this function currently scales
//				dependant on unit type.
//***************************************************************************
NCSError CNCSCoordinateTransform::Transform(CNCSPoint *pSrc,CNCSPoint *pDest)
{
	NCSError nRet= NCS_SUCCESS;
	IEEE8 dConversionFactor;

	//test to see if the two coordinate systems are compatible
	nRet = m_pSrcCoordSys->IsCompatible(m_pDestCoordSys);
	if(m_pSrcCoordSys->m_pCoordinateSystemName && m_pDestCoordSys->m_pCoordinateSystemName)
	{
		if(!strcmp(m_pSrcCoordSys->m_pCoordinateSystemName,NCS_LINEAR_COORD_SYS) &&
		   !strcmp(m_pDestCoordSys->m_pCoordinateSystemName,NCS_LINEAR_COORD_SYS))
		{
			if(nRet == NCS_SUCCESS)
			{
				//scale cell size to output cell size...
				//this is the only valid conversion allowed without
				//performing a coordinate system transformation :-(.
				switch(m_pDestCoordSys->m_nUnits)
				{
				case ECW_CELL_UNITS_METERS:
					if(m_pSrcCoordSys->m_nUnits == ECW_CELL_UNITS_FEET)
						dConversionFactor = NCS_FEET_FACTOR;
					else
						dConversionFactor = 1.0;
					break;

				case ECW_CELL_UNITS_FEET:
					if(m_pSrcCoordSys->m_nUnits == ECW_CELL_UNITS_METERS)
						dConversionFactor = 1.0 / NCS_FEET_FACTOR;
					else
						dConversionFactor = 1.0;
					break;

				default:
					dConversionFactor = 1.0;
					break;
				}

				pDest->m_dX = (pSrc->m_dX ) * dConversionFactor;
				pDest->m_dY = (pSrc->m_dY ) * dConversionFactor;
				
			}
		}
		else
		{
			nRet = NCS_COORDINATE_CANNOT_BE_TRANSFORMED;
		}
	}
	else
	{
		//TODO ADD NON-LINEAR TRANSFORMATIONS HERE.......
		nRet = NCS_COORDINATE_CANNOT_BE_TRANSFORMED;
	}

	return nRet;
}
