/********************************************************
** Copyright 2002 Earth Resource Mapping Ltd.
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
** FILE:     $Archive: /NCS/Source/include/NCSJP2File.h $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJP2File class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
**			 [01] 03Aug04 tfl  Added integer member to NCSJP2PCSBox
**			 [02] 07Dec04 tfl  Moved GeodataUsage enum declaration to NCSECWClient.h
**			 [03] 17Dec04 tfl  Added static CNCSJP2File::Shutdown() function called by CNCSJP2FileView::Shutdown()
 *******************************************************/

#ifndef NCSAFFINETRANSFORM_H
#define NCSAFFINETRANSFORM_H

#include "NCSECWClient.h"
#include "NCSJP2File.h" //for TIFF tag

class CNCSAffineTransform 
{

public:
	IEEE8 fScaleX;
	IEEE8 fScaleY;
	IEEE8 fRotX;
	IEEE8 fRotY;
	IEEE8 fTransX;
	IEEE8 fTransY;
	
	CNCSAffineTransform();
	CNCSAffineTransform(IEEE4 fPrecision);
	~CNCSAffineTransform();

	bool GetFromFileInfoEx(NCSFileViewFileInfoEx &FileInfo, bool bRasterPixelIsArea);
	bool GetFromFileInfo(NCSFileViewFileInfo &FileInfo, bool bRasterPixelIsArea);
	bool GetFromGeoTIFFModelTransformationTag(CNCSJP2File::CNCSJP2PCSBox::TIFFTag *pTransformTag);
	bool PopulateFileInfoEx(NCSFileViewFileInfoEx &FileInfo, bool bRasterPixelIsArea);
	bool PopulateFileInfo(NCSFileViewFileInfo &FileInfo);
	bool IsValid();
protected:
	void Standardize();
	IEEE8 m_fPrecision;
};

#endif
