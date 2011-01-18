/********************************************************
** Copyright 1999-2004 Earth Resource Mapping Ltd
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
** PURPOSE:  Simple encapsulation of the GDT library
**			 
** EDITS:    [xx] ddMmmyy NAME COMMENTS
**			 [01] 03Aug04 tfl  Added GetGCSGeoKey fn along similar lines
**			 [02] 10Nov04 tfl  Fixed integer initialisationg problem in dat file search
**			 [03] 06Jan05 tfl  Major changes to fix issues with GDT path location
*******************************************************/

#include "stdafx.h"
#include "NCSGDTEpsg.h"
#ifndef _WIN32_WCE
#include <fstream>
#endif

# ifdef ValuePair
# undef ValuePair
#endif
#define ValuePair(name,value)    name = value,

typedef enum {
#include "epsg_pcs.inc"
	pcstype_end
} pcstype_t;

typedef enum {
#include "epsg_gcs.inc"
	gcstype_end
} gcstype_t;

typedef enum {
#  include "epsg_datum.inc"
	geodeticdatum_end
} geodeticdatum_t;

#include "pcs_erm.h"
#include "gcs_erm.h"
#ifndef _WIN32_WCE
using std::getline;
#endif

#ifndef _MAX_PATH
#	define _MAX_PATH 255
#endif

CNCSGDTEPSG* CNCSGDTEPSG::sm_pInstance = (CNCSGDTEPSG *)NULL;

//Singleton acquisition and release methods
CNCSGDTEPSG *CNCSGDTEPSG::Instance()
{
	if (sm_pInstance == (CNCSGDTEPSG *)NULL) 
	{
		sm_pInstance = new CNCSGDTEPSG;
	}
	return sm_pInstance;
}

void CNCSGDTEPSG::Release()
{
	if (sm_pInstance != (CNCSGDTEPSG *)NULL)
	{
		delete sm_pInstance;
		sm_pInstance = (CNCSGDTEPSG *)NULL;
	}
}

//Ctor
CNCSGDTEPSG::CNCSGDTEPSG()
{
	m_bCustomKeysLoaded = false;
	//DetectPath();
}

//EPSG <--> ER Mapper projection and datum conversion methods
CNCSError CNCSGDTEPSG::GetProjectionAndDatum(const INT32 nEPSG, char **projection, char **datum) 
{
	INT32 arrayIndex = 0;
	string projStr, datumStr;

	if (!m_bCustomKeysLoaded) DetectPath();

	m_InstanceMutex.Lock();

	//First check the User defined Epsg codes taken from the file PcsKeyProjDatum.dat	
	for (UINT32 k = 0; k < m_Keys.size(); k++)
	{
		if(m_Keys[k].GetEPSG() == nEPSG)
		{
			projStr = m_Keys[k].GetProjection();
			datumStr = m_Keys[k].GetDatum();

			*projection = NCSStrDup((char *)projStr.c_str());
			*datum = NCSStrDup((char *)datumStr.c_str());
			m_InstanceMutex.UnLock();
			return NCS_SUCCESS;
		}
	}

	//Then Check through the epsg_pcs codes defined in epsg_pcs.inc and pcs_erm.h 
	struct pcs_erm_mapping* array = arrayOfArrays[arrayIndex];
	while (array != NULL)
	{
		int j = 0;
		while (array[j].pcs_code != 0)
		{
			if (nEPSG == array[j].pcs_code) 
			{
				if (array[j].projection != (char *)NULL)
					*projection=(char *)NCSStrDup(array[j].projection);

				else				
					*projection = (char *)NULL;

				if (array[j].datum != (char *)NULL)
					*datum=(char *)NCSStrDup(array[j].datum);

				else
					*datum=(char *)NULL;

				m_InstanceMutex.UnLock();
				return NCS_SUCCESS;
			}
			j++;    // next record in this array
		}
		arrayIndex++;    // next array
		array = arrayOfArrays[arrayIndex];
	}

	m_InstanceMutex.UnLock();
	return NCS_INCOMPATIBLE_COORDINATE_SYSTEMS;
}

INT32 CNCSGDTEPSG::GetEPSG(const char *projection, const char *datum)
{
	int match = 0;
	int arrayIndex = 0;
	int i=0;
	string projStr, datumStr;
	bool bFound = false;

	if (!m_bCustomKeysLoaded) DetectPath();

	m_InstanceMutex.Lock();
	//Check the User Defined Codes
	for (UINT32 k = 0; k < m_Keys.size(); k++)
	{
		projStr = m_Keys[k].GetProjection();
		datumStr = m_Keys[k].GetDatum();

		if(((char *)projStr.c_str()) && ((char *)datumStr.c_str()) &&
			!stricmp(((char *)projStr.c_str()), projection) &&
			!stricmp(((char *)datumStr.c_str()), datum)) 
		{
			match = m_Keys[k].GetEPSG();
			bFound = true;
			break;
		}
	}

	if(!bFound)
	{
		struct pcs_erm_mapping* array = arrayOfArrays[arrayIndex];

		while (array != NULL)
		{
			i=0;
			// Check all projection lists //
			if (!match) 
			{
				while (array[i].pcs_code != 0) 
				{
					if(array[i].projection && array[i].datum &&
						!strcmp(array[i].projection, projection) &&
						!strcmp(array[i].datum, datum)) 
					{
						match = array[i].pcs_code;
						bFound = true;
						break;
					}
					i++;
				}

				if(bFound)
				{
					break;
				}
				arrayIndex++;    // next array
				array = arrayOfArrays[arrayIndex];
			}
		}

	}
	m_InstanceMutex.UnLock();
	return match;
}

bool CNCSGDTEPSG::IsValidPath(const char *szPath)
{
	return (NCS_SUCCESS == LoadCustomKeys(szPath));
}

//Attempts to load the user defined GDT data from the specified filename.
//If successful, NCS_SUCCESS is returned.
NCSError CNCSGDTEPSG::LoadCustomKeys(const char *szPath)
{
	NCSError Error = NCS_FILE_IO_ERROR;
#ifndef _WIN32_WCE
	//Open the  PcskeyProjDatum.dat file to see if anything has been added
	//by the user
	std::ifstream File;
	std::string Line;
	std::string DatLocation = szPath;

	m_InstanceMutex.Lock();
	//Read the file line by line
	if (DatLocation.length() != 0) {
		DatLocation += NCS_FILE_SEP;
		DatLocation += "PcskeyProjDatum.dat";

		File.open(DatLocation.c_str());	
		int a = 0;

		if (File)
		{
			while (getline(File,Line))
			{
				if (!Line.find_first_not_of("*")) //Ignore the line if it starts with a "*"	
				{
					CNCSGDTEPSGKey  EPSGPCSEntry(Line);
					m_Keys.push_back(EPSGPCSEntry);
				}
			}
			m_bCustomKeysLoaded = true;
			Error = NCS_SUCCESS;
		}
		else
		{
			Error = NCS_FILE_NOT_FOUND;
		}

		File.close();
	}
	else Error = NCS_INVALID_PARAMETER;

	m_InstanceMutex.UnLock();
#endif // _WIN32_WCE
	return Error;
}
