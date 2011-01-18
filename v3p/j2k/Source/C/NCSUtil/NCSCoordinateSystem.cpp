/****************************************************************************
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
** CLASS(ES):	CNCSCoordinateSystem
** FILE:		NCSCoordinateSystem.cpp
** CREATED:		18 Jan 2000
** AUTHOR:		Matthew Bardsley
** PURPOSE:		Holds implementation of NCSCoordinateSystem Class
** EDITS:
** [01]	 ny	31Oct00	Merge WinCE/PALM changes
**
** CLASS NOTES:
***************************************************************************/

#include "NCSCoordinateSystem.h"
#include "NCSUtil.h"
#ifdef MACINTOSH
#include <string.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSCoordinateSystem::CNCSCoordinateSystem(char *pCoordinateSystemName,
                         char *pDatumName,
                         char *pProjName,
                         CellSizeUnits nUnits,
                         NCSCoordSys nType,
						 IEEE8 dCellX,
						 IEEE8 dCellY,
                         IEEE8 dOriginX,
                         IEEE8 dOriginY)

{

	if(pCoordinateSystemName)
	{
 	    m_pCoordinateSystemName = NCSStrDup(pCoordinateSystemName);
	}
	else
		m_pCoordinateSystemName = NULL;

	if(pDatumName)
	{
		m_pDatumName = NCSStrDup(pDatumName);
	}
	else
		m_pDatumName = NULL;

	if(pProjName)
	{
		m_pProjName = NCSStrDup(pProjName);
	}
	else
		m_pProjName = NULL;

	m_nUnits   = nUnits;
	m_nType    = nType;
	m_dCellX   = dCellX;
	m_dCellY   = dCellY;
	m_dOriginX = dOriginX;
	m_dOriginY = dOriginY;

}

CNCSCoordinateSystem::~CNCSCoordinateSystem()
{
	if(m_pCoordinateSystemName)
		NCSFree(m_pCoordinateSystemName);

	if(m_pDatumName)
		NCSFree(m_pDatumName);

	if(m_pProjName)
		NCSFree(m_pProjName);
}

//***************************************************************************
// METHOD:		operator=(CNCSCoordinateSystem &pCoordSys)
// PURPOSE:		Overloaded operator to perform assignments
// PARAMETERS:	CNCSCoordinateSystem &pCoordSys  
//
// RETURNS:		CNCSCoordinateSystem &
//
// NOTES:
//***************************************************************************
CNCSCoordinateSystem &CNCSCoordinateSystem::operator=(CNCSCoordinateSystem &pCoordSys)
{
	//free old values
	if(m_pCoordinateSystemName)
	{
		NCSFree(m_pCoordinateSystemName);
		m_pCoordinateSystemName = NULL;
	}

	if(m_pDatumName)
	{
		NCSFree(m_pDatumName);
		m_pDatumName = NULL;
	}

	if(m_pProjName)
	{
		NCSFree(m_pProjName);
		m_pProjName = NULL;
	}

	//assign new
	if(pCoordSys.m_pCoordinateSystemName)
	{
 	    m_pCoordinateSystemName = NCSStrDup(pCoordSys.m_pCoordinateSystemName);
	}

	if(pCoordSys.m_pDatumName)
	{
		m_pDatumName = NCSStrDup(pCoordSys.m_pDatumName);
	}

	if(pCoordSys.m_pProjName)
	{
		m_pProjName = NCSStrDup(pCoordSys.m_pProjName);
	}

	m_nUnits   = pCoordSys.m_nUnits;
	m_nType    = pCoordSys.m_nType;
	m_dCellX   = pCoordSys.m_dCellX;
	m_dCellY   = pCoordSys.m_dCellY;
	m_dOriginX = pCoordSys.m_dOriginX;
	m_dOriginY = pCoordSys.m_dOriginY;

	return *this;
}

//***************************************************************************
// METHOD:		operator=(CNCSCoordinateSystem &pCoordSys)
// PURPOSE:		Overloaded operator to perform assignments
// PARAMETERS:	CNCSCoordinateSystem &pCoordSys  
//
// RETURNS:		CNCSCoordinateSystem &
//
// NOTES:
//***************************************************************************
BOOLEAN CNCSCoordinateSystem::operator==(CNCSCoordinateSystem &pCoordSys)
{
	NCSError nRet = IsCompatible(&pCoordSys);
	BOOLEAN bRet;

	if(nRet == NCS_SUCCESS)
		bRet = TRUE;
	else
		bRet = FALSE;

	return bRet;
}

//***************************************************************************
// METHOD:		IsCompatible(CNCSCoordinateSystem *pCoordSys)
// PURPOSE:		To test if the coordinate system is compatible with itself.
// PARAMETERS:	CNCSCoordinateSystem *pCoordSys  
//
// RETURNS:		NCS_SUCCESS if compatible
//
// NOTES:		
//***************************************************************************
NCSError CNCSCoordinateSystem::IsCompatible(CNCSCoordinateSystem *pCoordSys)
{
	NCSError nRet = NCS_SUCCESS;

	//first test to see if the coord sys names are identical
	if(m_pCoordinateSystemName && pCoordSys->m_pCoordinateSystemName)
	{
		if(!strcmp(m_pCoordinateSystemName,pCoordSys->m_pCoordinateSystemName))
		{
			if(strcmp(m_pDatumName,pCoordSys->m_pDatumName))
				nRet = NCS_INCOMPATIBLE_COORDINATE_DATUM;

			if(strcmp(m_pProjName,pCoordSys->m_pProjName))
				nRet = NCS_INCOMPATIBLE_COORDINATE_PROJECTION;

			switch(m_nUnits)
			{
			case ECW_CELL_UNITS_METERS:
				if(pCoordSys->m_nUnits == ECW_CELL_UNITS_DEGREES)
					nRet = NCS_INCOMPATIBLE_COORDINATE_UNITS;
				break;
			case ECW_CELL_UNITS_DEGREES:
				if(pCoordSys->m_nUnits != ECW_CELL_UNITS_DEGREES)
					nRet = NCS_INCOMPATIBLE_COORDINATE_UNITS;
				break;
			case ECW_CELL_UNITS_FEET:
				if(pCoordSys->m_nUnits == ECW_CELL_UNITS_DEGREES)
					nRet = NCS_INCOMPATIBLE_COORDINATE_UNITS;
				break;
			}
		}
		else
			nRet = NCS_INCOMPATIBLE_COORDINATE_SYSTEMS;
	}
	else
		nRet = NCS_INCOMPATIBLE_COORDINATE_SYSTEMS;

	return nRet; 
}

