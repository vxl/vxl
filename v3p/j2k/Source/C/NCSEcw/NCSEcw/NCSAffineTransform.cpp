/********************************************************
** Copyright 2004 Earth Resource Mapping Ltd.
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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSEcw/NCSAffineTransform.cpp$
** CREATED:  14/07/2004 3:27:34 PM
** AUTHOR:   Tom Lynch
** PURPOSE:  Implementation of utility CNCSAffineTransform class
** EDITS:    [xx]	ddMmmyy	NAME	COMMENTS
 *******************************************************/

#include "NCSAffineTransform.h"

CNCSAffineTransform::CNCSAffineTransform() 
{
	//Just set some defaults
	fScaleX = 1.0;
	fScaleY = 1.0;
	fRotX = 0.0;
	fRotY = 0.0;
	fTransX = 0.0;
	fTransY = 0.0;
}

CNCSAffineTransform::CNCSAffineTransform(IEEE4 fPrecision)
{
	//Just set some defaults
	fScaleX = 1.0;
	fScaleY = 1.0;
	fRotX = 0.0;
	fRotY = 0.0;
	fTransX = 0.0;
	fTransY = 0.0;

	m_fPrecision = fPrecision;
}

CNCSAffineTransform::~CNCSAffineTransform() {}

bool CNCSAffineTransform::GetFromGeoTIFFModelTransformationTag(CNCSJP2File::CNCSJP2PCSBox::TIFFTag *pTag)
{
	//Get matrix values from TIFF tag
	if ((pTag != NULL) && (pTag->Values.size() == 16))
	{
		fScaleX = pTag->Values[0];
		fRotY = pTag->Values[1];
		fTransX = pTag->Values[3];
		fRotX = pTag->Values[4];
		fScaleY = pTag->Values[5];
		fTransY = pTag->Values[7];

		Standardize();

		return IsValid();
	}
	else return false;
}

bool CNCSAffineTransform::GetFromFileInfoEx(NCSFileViewFileInfoEx &FileInfo, bool bRasterPixelIsArea)
{
	//Get some values
	IEEE8 fFactorX = FileInfo.fCellIncrementX;
	IEEE8 fFactorY = -FileInfo.fCellIncrementY;
	IEEE8 fACWRotation = -FileInfo.fCWRotationDegrees;
	IEEE8 fOriginX = FileInfo.fOriginX;
	IEEE8 fOriginY = FileInfo.fOriginY;

	/**Calculate matrix values from file info
	fScaleX = cos(NCS_DEGREES_TO_RADIANS*FileInfo.fCWRotationDegrees)*FileInfo.fCellIncrementX;
	fScaleY = -cos(NCS_DEGREES_TO_RADIANS*FileInfo.fCWRotationDegrees)*FileInfo.fCellIncrementY;
	fRotX = sin(NCS_DEGREES_TO_RADIANS*FileInfo.fCWRotationDegrees)*FileInfo.fCellIncrementY;
	fRotY = sin(NCS_DEGREES_TO_RADIANS*FileInfo.fCWRotationDegrees)*FileInfo.fCellIncrementX;
	fTransX = FileInfo.fOriginX;
	fTransY = FileInfo.fOriginY;*/
	fScaleX = cos(NCS_DEGREES_TO_RADIANS*fACWRotation)*fFactorX;
	fRotY = sin(NCS_DEGREES_TO_RADIANS*fACWRotation)*fFactorX;
	fRotX = -sin(NCS_DEGREES_TO_RADIANS*fACWRotation)*fFactorY;
	fScaleY = cos(NCS_DEGREES_TO_RADIANS*fACWRotation)*fFactorY;
	fTransX = FileInfo.fOriginX;
	fTransY = FileInfo.fOriginY;
	if (!bRasterPixelIsArea)
	{
		fTransX += 0.5*fFactorX;
		fTransY += 0.5*fFactorY;
	}

	Standardize();

	return IsValid();
}

bool CNCSAffineTransform::PopulateFileInfoEx(NCSFileViewFileInfoEx &FileInfo, bool bRasterPixelIsArea)
{
	if (IsValid())
	{
		//Rotation
		IEEE8 fCWRotation = 0.0;
		IEEE8 fFactorX = 1.0, fFactorY = 1.0;

		//Boundary cases
		if (fRotY == 0 && fScaleX > 0) 
		{
			fCWRotation = 0.0;	//CHECKED
			fFactorX = fScaleX;
			fFactorY = fScaleY;
		}
		else if (fRotY == 0 && fScaleX < 0) 
		{
			fCWRotation = 180.0;	//CHECKED
			fFactorX = -fScaleX;
			fFactorY = -fScaleY;
		}
		else if (fRotY < 0 && fScaleX == 0) 
		{
			fCWRotation = 90.0;	//CHECKED
			fFactorX = fRotY;
			fFactorY = -fRotX;
		}
		else if (fRotY > 0 && fScaleX == 0) 
		{
			fCWRotation = -90.0;	//CHECKED
			fFactorX = -fRotY;
			fFactorY = fRotX;
		}
		else
		{
			//fRotY == sinCWrot * fFactorX
			//fScaleX == cosCWrot * fFactorX
			//--> tanCWrot = fRotY/fScaleX
			//--> ACrot = atan(fRotY/fScaleX)
			fCWRotation = -NCS_RADIANS_TO_DEGREES * atan(fRotY/fScaleX);
			//This is ambiguous (returns an answer in (-PI/2,PI/2) i.e. QI/IV,
			//but desired answer could be in (-PI,-PI/2) or (PI/2,PI) i.e. QII/III).
			//Use signs of fRotY and fScaleX to disambiguate
			//QII --> sin +ve, cos -ve --> fRotY -ve, fScaleX -ve	
			if (fRotY > 0 && fScaleX < 0)
				fCWRotation += 180;
			//QIII --> sin -ve, cos -ve --> fRotY +ve, fScaleX -ve
			if (fRotY < 0 && fScaleX < 0)
				fCWRotation -= 180;

			fFactorX = fScaleX/cos(NCS_DEGREES_TO_RADIANS*fCWRotation);
			fFactorY = fScaleY/cos(NCS_DEGREES_TO_RADIANS*fCWRotation);
		}

		FileInfo.fCellIncrementX = fFactorX;
		FileInfo.fCellIncrementY = -fFactorY;
		FileInfo.fCWRotationDegrees = fCWRotation;
		FileInfo.fOriginX = fTransX;
		FileInfo.fOriginY = fTransY;
		if (!bRasterPixelIsArea)
		{
			FileInfo.fOriginX -= 0.5*fFactorX;
			FileInfo.fOriginY -= 0.5*fFactorY;
		}

		NCSStandardizeFileInfoEx(&FileInfo);

		return true;
	}
	else return false;
}

bool CNCSAffineTransform::GetFromFileInfo(NCSFileViewFileInfo &FileInfo,bool bRasterPixelIsArea)
{
		//Calculate matrix values from file info
	fScaleX = FileInfo.fCellIncrementX;
	fScaleY = FileInfo.fCellIncrementY;
	fRotX = 0.0;
	fRotY = 0.0;
	fTransX = FileInfo.fOriginX;
	fTransY = FileInfo.fOriginY;
	
	if (!bRasterPixelIsArea)
	{
		fTransX += 0.5*fScaleX;
		fTransY += 0.5*fScaleY;
	}

	Standardize();

	return IsValid();
}

bool CNCSAffineTransform::PopulateFileInfo(NCSFileViewFileInfo &FileInfo)
{
	FileInfo.fOriginY = fTransX;
	FileInfo.fOriginX = fTransY;
	FileInfo.fCellIncrementX = fScaleX;
	FileInfo.fCellIncrementY = fScaleY;

	return IsValid();
}

#define SHEAR_CHECK_EPSILON	0.00001L	//to counteract rounding error
bool CNCSAffineTransform::IsValid()
{
	bool bValid = true;

	//Check validity of transform values - we don't handle shearing
	if ((fScaleX == 0 && fRotX == 0) || 
		(fScaleY == 0 && fRotY == 0))
		bValid = false; //invalid
	if ((fScaleX == 0 && fScaleY != 0) || 
		(fScaleX != 0 && fScaleY == 0) || 
		(fRotX == 0 && fRotY != 0) || 
		(fRotX != 0 && fRotY == 0))
		bValid = false; //sheared
	if (!(fScaleX == 0 || fScaleY == 0 || 
		fRotX == 0 || fRotY == 0))
	{
		if (fabs(fScaleX/fScaleY) - fabs(fRotY/fRotX) > SHEAR_CHECK_EPSILON)
			bValid = false; //sheared
	}

	return bValid;
}

void CNCSAffineTransform::Standardize()
{
	if (fabs(fScaleX) < m_fPrecision) fScaleX = 0.0;
	if (fabs(fRotX) < m_fPrecision) fRotX = 0.0;
	if (fabs(fScaleY) < m_fPrecision) fScaleY = 0.0;
	if (fabs(fRotY) < m_fPrecision) fRotY = 0.0;
}



