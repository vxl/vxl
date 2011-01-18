/******************************************************
** Copyright 1998 Earth Resource Mapping Ltd.
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
** CREATED:  16 Jan 2004
** AUTHOR:   Gary Noel
** PURPOSE:  Enable users to define own Epsg Pcs code projection/Datum Mappings
**			 Used in conjunction with file "PcsKeyProjDatum.dat" 
** EDITS:    [xx] ddMmmyy NAME COMMENTS
**			 [01] 03Aug04 tfl  Added GetGCSGeoKey fn along similar lines
**			 [02] 06Jan05 tfl  Major changes to fix GDT path location issues
**							   (made class singleton to improve efficiency of use, subclassed
**								class with path as static member, changed method names)
** 
** 
*******************************************************/
#ifndef NCSGDTEPSG_H
#define NCSGDTEPSG_H

#include "NCSGDTEPSGKey.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NCSTypes.h"
#include "NCSError.h"
#include "NCSGDTLocation.h"

#include <vector>

class CNCSGDTEPSG: public CNCSGDTLocation  
{
public:
	static CNCSGDTEPSG *Instance();
	static void Release();

	CNCSError GetProjectionAndDatum(const INT32 nEPSG, char **pszProjection, char **pszDatum);
	INT32 GetEPSG(const char *szProjection, const char *szDatum);

protected:
	CNCSGDTEPSG();
	virtual ~CNCSGDTEPSG() {}

	//Inherited and overridden from CNCSGDTLocation
	virtual bool IsValidPath(const char *szPath);

private:
	static CNCSGDTEPSG *sm_pInstance;
	CNCSMutex m_InstanceMutex;
	bool m_bCustomKeysLoaded;
	NCSError LoadCustomKeys(const char *szPath);
	std::vector<CNCSGDTEPSGKey> m_Keys;

};

#endif //NCSGDTEPSG_H
