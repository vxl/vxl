/********************************************************
** Copyright 2000 Earth Resource Mapping Ltd.
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
** FILE:   	NCSUtil\NCSServerState.cpp
** CREATED:	06Jul00
** AUTHOR: 	Nicholas Yue
** PURPOSE:	Determines if the web server is running
**			Currently assumes that we are interogating IIS
** EDITS:
** [01] 07Dec00	 ny	Replace ptr with direct address of
**					the dServerState variable
 *******************************************************/

#ifndef _WIN32_WCE
#include <string.h>
#include <iiscnfg.h>
#include "NCSUtil.h"
#include "CNCSMetabaseEdit.h"
#else // !_WIN32_WCE
#include "NCSError.h"
#endif // !_WIN32_WCE
#include "NCSServerState.h"

NCSError NCSIsWebServerRunning(DWORD nServerID,BOOLEAN *bServerRunning)
#ifdef _WIN32_WCE
{
	return(NCS_METABASE_OPEN_FAILED);
}
#else
{
	CNCSMetabaseEdit me;
	char path[1024];
	HRESULT hRes;
	sprintf(path,"/LM/W3SVC/%d",nServerID);
	hRes = me.open(path,METADATA_PERMISSION_READ,5000);

	if (SUCCEEDED(hRes)) {
		DWORD MDRecordLen;
		DWORD dServerState;
		
		// Get the Access Flag for the SampleIWS diretory in the metabase
		METADATA_RECORD MDRecord = {MD_SERVER_STATE,		// Path ID
									METADATA_INHERIT,		// Is inherited
									IIS_MD_UT_SERVER,		// User Type
									DWORD_METADATA,			// Data Type
									sizeof(dServerState),	// Size of data
									(PBYTE)&dServerState,	// Data(our string)
									0};						// reserved
		hRes = me.get("/",&MDRecord,&MDRecordLen);

		if (SUCCEEDED(hRes)) {
			if (dServerState==MD_SERVER_STATE_STOPPING||dServerState==MD_SERVER_STATE_STOPPED)
				*bServerRunning = FALSE;
			else
				*bServerRunning = TRUE;
		} else
			return NCS_METABASE_GET_FAILED;
		me.close();
		return NCS_SUCCESS;
	} else
		return NCS_METABASE_OPEN_FAILED;
}
#endif
