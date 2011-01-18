/******************************************************
** Copyright 2005 Earth Resource Mapping Ltd.
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
** FILE:   	 NCSGDTLocation.h
** CREATED:  06 Jan 2005
** AUTHOR:   Tom Lynch
** PURPOSE:  Establish an application wide way of controlling the location of GDT data
**
** EDITS:    [xx] ddMmmyy NAME COMMENTS
**			 [01] 06Jan05 tfl  File first committed
** 
*******************************************************/

#ifndef NCSGDTLOCATION_H
#define NCSGDTLOCATION_H

#include "NCSMutex.h"

class  CNCSGDTLocation  //JX	   NCS_EXPORT
{
public:
	CNCSGDTLocation() {}
	virtual ~CNCSGDTLocation() {}
	static void SetPath(const char *szPath);
	static char *GetPath();
	static void SetGuessPath(bool bGuessPath) {sm_bGuessPath = bGuessPath;}
	static bool GetGuessPath() {return sm_bGuessPath;}
	virtual void DetectPath();

protected:
	virtual bool IsValidPath(const char *szPath) = 0;
	virtual char *GuessPath();

private:
	static char sm_szGDTPath[MAX_PATH];
	static CNCSMutex sm_PathMutex;
	static bool sm_bGuessPath;
};

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * Set the custom GDT data path (for EPSG<-->ER Mapper projection/datum mapping)
	 * @param szPath the new GDT data path
	 */
void	NCSSetGDTPath2(char *szPath);
	/**
	 * Get the custom GDT data path (for EPSG<-->ER Mapper projection/datum mapping)
	 * @return the custom path being used, or NULL
	 */
char *NCSGetGDTPath2();

#ifdef __cplusplus
}
#endif


#endif
