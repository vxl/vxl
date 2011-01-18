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
** CLASS(ES):	CNCSCoordinateSystem
** FILE:		NCSCoordinateSystem.h
** CREATED:		18 Jan 2000
** AUTHOR:		Matthew Bardsley
** PURPOSE:		Holds definition of NCSCoordinateSystem Class
** EDITS:
**
** CLASS NOTES:
***************************************************************************/
#if !defined(NCSCOORDINATESYSTEM_H)
#define NCSCOORDINATESYSTEM_H

#include "NCSUtil.h"
#include "NCSEcwclient.h"

class NCS_EXPORT CNCSCoordinateSystem  
{

public:
	CNCSCoordinateSystem(char *pCoordinateSystemName = (char *)NULL,
                         char *pDatumName  = (char *)NULL,
                         char *pProjName = (char *)NULL,
                         CellSizeUnits nUnits = ECW_CELL_UNITS_METERS,
                         NCSCoordSys nType = NCSCS_UTM,
						 IEEE8 dCellX = 0.0,
						 IEEE8 dCellY = 0.0,
                         IEEE8 dOriginX = 0.0,
                         IEEE8 dOriginY = 0.0);
	virtual ~CNCSCoordinateSystem();

	//methods
	virtual NCSError IsCompatible(CNCSCoordinateSystem *);

	//overloaded operators
	virtual CNCSCoordinateSystem &operator=(CNCSCoordinateSystem &);
	virtual BOOLEAN operator==(CNCSCoordinateSystem &);


	//properties
	char			*m_pCoordinateSystemName;
	char			*m_pDatumName;
	char			*m_pProjName;
	CellSizeUnits	m_nUnits;
	NCSCoordSys		m_nType;
	IEEE8			m_dCellX;
	IEEE8			m_dCellY;
	IEEE8			m_dOriginX;
	IEEE8			m_dOriginY;
	
};

#endif // !defined(NCSCOORDINATESYSTEM_H)
