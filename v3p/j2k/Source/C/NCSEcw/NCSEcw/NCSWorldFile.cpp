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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSWorldFile.cpp $
** CREATED:  02/09/2004 3:27:34 PM
** AUTHOR:   Tom Lynch
** PURPOSE:  Utility class for I/O with six-value world files
** EDITS:    [xx]	ddMmmyy	NAME	COMMENTS
 *******************************************************/
#include "NCSDefs.h"
#include "NCSWorldFile.h"
#include "NCSMalloc.h"

CNCSWorldFile::CNCSWorldFile() {}
CNCSWorldFile::CNCSWorldFile(IEEE8 fPrecision) {m_fPrecision = fPrecision;}
CNCSWorldFile::~CNCSWorldFile() {}

bool CNCSWorldFile::ScanValue(FILE *pFile, IEEE8 *pfValue)
{
	if (fscanf(pFile,"%lf", pfValue) == 1) return true;
	else return false;
}

bool CNCSWorldFile::Read(char *pWorldFilename)
{
	bool bReturn = true;
	FILE *pWorldFile = (FILE *)NULL;
	INT32 nItemsRead = 0;

	pWorldFile = fopen(pWorldFilename,"r");

	if (pWorldFile != NULL)
	{
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
		fprintf(stderr,"World file %s successfully opened for reading\n",pWorldFilename);
#endif
		bReturn = ScanValue(pWorldFile, &fScaleX);
		if (bReturn) bReturn = ScanValue(pWorldFile, &fRotY);
		if (bReturn) bReturn = ScanValue(pWorldFile, &fRotX);
		if (bReturn) bReturn = ScanValue(pWorldFile, &fScaleY);
		if (bReturn) bReturn = ScanValue(pWorldFile, &fTransX);
		if (bReturn) bReturn = ScanValue(pWorldFile, &fTransY);

		fclose(pWorldFile);
	}
	else
	{
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
		fprintf(stderr,"World file %s couldn't be opened for reading\n",pWorldFilename);
#endif
		bReturn = false;
	}
	return bReturn;
}

bool CNCSWorldFile::Write(char *pWorldFilename)
{
	bool bReturn = true;
	FILE *pWorldFile = (FILE *)NULL;

	pWorldFile = fopen(pWorldFilename,"w");

	if (pWorldFile != NULL)
	{
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
		fprintf(stderr,"World file %s successfully opened for writing\n",pWorldFilename);
#endif
		fprintf(pWorldFile,"%lf\n",fScaleX);
		fprintf(pWorldFile,"%lf\n",fRotY);
		fprintf(pWorldFile,"%lf\n",fRotX);
		fprintf(pWorldFile,"%lf\n",fScaleY);
		fprintf(pWorldFile,"%lf\n",fTransX);
		fprintf(pWorldFile,"%lf\n",fTransY);

		fclose(pWorldFile);
	}
	else
	{
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
		fprintf(stderr,"World file %s couldn't be opened for writing\n",pWorldFilename);
#endif
		bReturn = false;
	}
	return bReturn;
}

bool CNCSWorldFile::ConstructWorldFilename(char *szRasterFilename, char **pszWorldFilename)
{
	char *pLast = szRasterFilename;
	char *pNext = szRasterFilename;
	while (pNext != NULL)
	{
		pNext = strstr(pLast,"\\");
		if (pNext != NULL) pLast = pNext+1;
	}
	pNext = strstr(pLast,".");
	char *szWorldFilename = (char *)NCSMalloc((UINT32)(strlen(szRasterFilename)+5)*sizeof(char), true);
	*szWorldFilename = '\0';
	size_t nChars = (pNext == NULL)?(strlen(szRasterFilename)):(pNext - szRasterFilename);
	strncpy(szWorldFilename, szRasterFilename, nChars);
	strcat(szWorldFilename,".j2w");

	*pszWorldFilename = szWorldFilename;

	return true;
}
