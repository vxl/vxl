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
** FILE:   	 NCSGDTLocation.cpp
** CREATED:  06 Jan 2005
** AUTHOR:   Tom Lynch
** PURPOSE:  CNCSGDTLocation implementation (control location of GDT data)
**
** EDITS:    [xx] ddMmmyy NAME COMMENTS
**			 [01] 06Jan05 tfl  File first committed
** 
*******************************************************/

#include "NCSDefs.h"
#include "NCSUtil.h"
#include "NCSGDTLocation.h"

#ifndef MAX_ERMAPPER_MAJOR_VERSION
#define MAX_ERMAPPER_MAJOR_VERSION 10
#endif

#ifndef MIN_ERMAPPER_MAJOR_VERSION
#define MIN_ERMAPPER_MAJOR_VERSION 7
#endif

char CNCSGDTLocation::sm_szGDTPath[MAX_PATH] = {'\0'};
bool CNCSGDTLocation::sm_bGuessPath = true;
CNCSMutex CNCSGDTLocation::sm_PathMutex;

void CNCSGDTLocation::SetPath(const char *szPath)
{
	if(strlen(szPath) <= 1)	//JX
		return;//JX
	//Strip final '/' or '\' characters
	char *szCopy = (char *)NCSMalloc(sizeof(char)*strlen(szPath)+1,true);
	strcpy(szCopy,szPath);
	char cLastChar = szCopy[strlen(szCopy)-1];
	while(cLastChar == '\\' || cLastChar == '/') 
	{
		szCopy[strlen(szCopy)-1] = '\0';
		cLastChar = szCopy[strlen(szCopy)-1];
	}	
	sm_PathMutex.Lock();
	strcpy(sm_szGDTPath,szCopy);
	sm_PathMutex.UnLock();
	NCSFree(szCopy);
}

char *CNCSGDTLocation::GetPath()
{
	sm_PathMutex.Lock();
	char *szPath = NCSStrDup(sm_szGDTPath);
	sm_PathMutex.UnLock();
	return szPath;
}

void CNCSGDTLocation::DetectPath()
{
	if (sm_bGuessPath) 
	{
		char *szPath = GuessPath();
		CNCSGDTLocation::SetPath(szPath);
		NCSFree(szPath);
	}
}

char *CNCSGDTLocation::GuessPath()
{
	//Overall we check five possible locations for the GDT data
	//1.  Location of any path value in sm_szGDTPath
	//2.  In GDT_Data subdir of NCSEcw.dll location
	//3.  Location of Image Web Server client directory
	//4.  Location of ER Mapper GDT_Data (based on cycling through versions of ER Mapper)

	//1.  Location of any path value in sm_szGDTPath
	char *szFileLocation = (char *)NULL;
	szFileLocation = CNCSGDTLocation::GetPath();
	if (IsValidPath(szFileLocation))
	{
		return szFileLocation;
	}
	else {
		NCSFree(szFileLocation);
		szFileLocation = NULL;
	}

#ifdef WIN32

	//2.  In GDT_Data subdir of NCSEcw.dll location
	/////////////////////////////////////////////////////////////
	//get dll location
	/////////////////////////////////////////////////////////////
	char *szClientDllDir;
	
	//find client dir setup by cab install
	NCSTChar FileName[1024] = { '\0' };
	INT32 i = 0;

#ifdef _DEBUG
	//HMODULE hModule = GetModuleHandle( "NCSGdtd.dll" );
	HMODULE hModule = GetModuleHandleW(L"ncsecwd.dll");
#else
	//HMODULE hModule = GetModuleHandle( "NCSGdt.dll" );
	HMODULE hModule = GetModuleHandleW(L"ncsecw.dll");
#endif
	// Check if the NCSGdtLib is in the same dir as the view manager module
#ifdef NCS_BUILD_UNICODE
	DWORD nResult = GetModuleFileNameW(hModule,  FileName, 1024);
#else
	DWORD nResult = GetModuleFileNameA(hModule,  FileName, 1024);
#endif
	if (nResult)
	{
		for (i=nResult; i>0; i--)
		{
			if (FileName[i] == '\\')
			{
				//i++;
				break;
			}
		}
	}
	if(i!=0)
	{
		FileName[i]='\0';
		szClientDllDir=(char*)NCSMalloc(MAX_PATH * sizeof(char), false);

		strcpy(szClientDllDir, CHAR_STRING(FileName));
		strcat(szClientDllDir, "\\GDT_Data");

		if (IsValidPath(szClientDllDir))
		{
			return szClientDllDir;
		}
		else NCSFree(szClientDllDir);
	}

	char *szClientBaseDir;
	//3.  Location of Image Web Server client directory
	if( NCS_SUCCEEDED( NCSPrefSetMachineKeyLock(NCSPREF_DEFAULT_BASE_KEY) ) ) {
		if (NCSPrefGetString("ClientBaseDir", &szFileLocation) != NCS_SUCCESS)
			NCSPrefGetUserString("ClientBaseDir", &szFileLocation);
		//the registry key ClientBaseDir under the current user contains the word "test"
		//dont know why but its not a legitimate directory.
		if (szFileLocation != NULL)
			if(_stricmp(szFileLocation, "test")==0)
			{
				NCSFree(szFileLocation);
				szFileLocation = NULL;
			}
		if (szFileLocation != NULL)
		{
			szClientBaseDir=(char*)NCSMalloc(MAX_PATH * sizeof(char), false);
			strcpy(szClientBaseDir, szFileLocation);
			strcat(szClientBaseDir, "\\GDT_Data");
			NCSFree(szFileLocation);
			szFileLocation = NULL;
			if (IsValidPath(szClientBaseDir))
			{
				NCSPrefMachineUnLock();
				return szClientBaseDir;
			}
			else NCSFree (szClientBaseDir);
		}
		NCSPrefMachineUnLock();
	}
	//4.  Location of ER Mapper GDT_Data (based on cycling through versions of ER Mapper)
	char *szERMapperBaseDir=(char *)NULL;
	char *szERMapperLibVersionKey=(char*)NCSMalloc(MAX_PATH * sizeof(char), false);
	int nMajorVersion, nMinorVersion;
	for (nMajorVersion = MAX_ERMAPPER_MAJOR_VERSION; nMajorVersion >= MIN_ERMAPPER_MAJOR_VERSION; nMajorVersion--)
	{
		for (nMinorVersion = 9; nMinorVersion >= 0; nMinorVersion--)
		{
			sprintf(szERMapperLibVersionKey,"Software\\Earth Resource Mapping\\ERMAPPER(libversion%d.%d)",nMajorVersion,nMinorVersion);
			if( NCS_SUCCEEDED( NCSPrefSetMachineKeyLock(szERMapperLibVersionKey) ) ) {
				if (NCSPrefGetString("BASE_PATH", &szERMapperBaseDir) != NCS_SUCCESS)
					NCSPrefGetUserString("ClientBaseDir", &szERMapperBaseDir);
				if (szERMapperBaseDir != NULL)
				{
					char *szERMapperGDTDir=(char*)NCSMalloc(MAX_PATH * sizeof(char), false);
					strcpy(szERMapperGDTDir,szERMapperBaseDir);
					strcat(szERMapperGDTDir,"\\GDT_DATA");
					NCSFree(szERMapperBaseDir);
					if (IsValidPath(szERMapperGDTDir))
					{
						NCSFree(szERMapperLibVersionKey);
						NCSPrefMachineUnLock();
						return szERMapperGDTDir;
					}
					else 
						NCSFree(szERMapperGDTDir);
				}
				NCSPrefMachineUnLock();
			}
		}
	}
	NCSFree(szERMapperLibVersionKey);

	if( szFileLocation ) {
		NCSFree(szFileLocation);
		szFileLocation = NULL;
	}
#endif //win32
	return (NCSStrDup(""));	
}

extern "C" void NCSSetGDTPath2(char *szPath)
{
	CNCSGDTLocation::SetPath(szPath);
}

extern "C" char *NCSGetGDTPath2()
{
	return CNCSGDTLocation::GetPath();
}
