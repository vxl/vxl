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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSWorldFile.h $
** CREATED:  02/09/2004 3:27:34 PM
** AUTHOR:   Tom Lynch
** PURPOSE:  Utility class for reading six-value world files
** EDITS:    [xx]	ddMmmyy	NAME	COMMENTS
 *******************************************************/

#include "NCSAffineTransform.h"
#ifndef ERS_JP2_WORLD_FILE_EXTS
#define ERS_JP2_WORLD_FILE_EXTS	{ ".j2w" , ".jxw" , ".jfw" }  
#endif

class CNCSWorldFile: public CNCSAffineTransform
{
public:
	CNCSWorldFile();
	CNCSWorldFile(IEEE8 fPrecision);
	~CNCSWorldFile();

	bool Read(char *pWorldFilename);
	bool Write(char *pWorldFilename);
	bool ConstructWorldFilename(char *szRasterFilename, char **szWorldFilename);
private:
	static inline bool ScanValue(FILE *pFile, IEEE8 *pValue);
};

