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
** CLASS(ES):	CNCSCoordinateTransform
** FILE:		NCSCoordinateTransform.h
** CREATED:		18 Jan 2000
** AUTHOR:		Matthew Bardsley
** PURPOSE:		Holds definition of NCSCoordinateTransform Class
** EDITS:
**
** CLASS NOTES:
***************************************************************************/
#if !defined(NCSCOORDINATETRANSFORM_H)
#define NCSCOORDINATETRANSFORM_H

#include "NCSPoint.h"
#include "NCSCoordinateSystem.h"

class NCS_EXPORT CNCSCoordinateTransform  
{
public:
	//constructors
	CNCSCoordinateTransform(CNCSCoordinateSystem *pSrc = NULL,
							CNCSCoordinateSystem *pDest = NULL);
	virtual ~CNCSCoordinateTransform ();

	//access methods to private members
	NCSError GetSrcCoordSys( CNCSCoordinateSystem *pCoord);
	NCSError SetSrcCoordSys( CNCSCoordinateSystem *pCoord);
	NCSError GetDestCoordSys( CNCSCoordinateSystem *pCoord);
	NCSError SetDestCoordSys( CNCSCoordinateSystem *pCoord);

	//translation routines
	virtual NCSError Transform(CNCSPoint *pSrc,CNCSPoint *pDest);

private: 
	CNCSCoordinateSystem *m_pSrcCoordSys;
	CNCSCoordinateSystem *m_pDestCoordSys;

};

#endif // !defined(NCSCOORDINATETRANSFORM_H)
