/********************************************************
** Copyright 1999 Earth Resource Mapping Ltd.
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
** FILE:   	NCSUtil\prefs.c
** CREATED:	Fri Jun  4 18:05:04 WST 1999
** AUTHOR: 	Mark Sheridan
** PURPOSE:	NCSRegistry routines
**			There are two types of keys, per user and
**			global machine prefs. Use the NCSPref*User* calls
**			to set/get user prefs.
** EDITS:	
** [01] sjc 05Jul00 Fixed handle leaks - RegCloseKey() was after return
** [02] sjc 12Sep00 Changed access to KEY_WRITE, so non-admin accoutns can create keys properly.
** [03]  ny 31Oct00 Merge WinCE/PALM SDK changes
** [04]  ny 03Nov00 Mac port changes
** [05] rar 15Nov01 Changed NCSPrefGetStringInternal to initialise szValueBuffer to a null string
 *******************************************************/

#include "NCSPrefs.h"

#ifdef WIN32

static char *pBaseMachineKeyName = NULL;
static char *pBaseUserKeyName = NULL;
static NCSMutex MachineMutex;
static NCSMutex UserMutex;
static BOOLEAN bHaveInit = FALSE;

/*
** Local prototypes
*/
static NCSError NCSPrefGetStringInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName, char **pString );
static NCSError NCSPrefSetStringInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName, char *pString );
static NCSError NCSPrefGetDoubleInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName, IEEE8 *pValue );
static NCSError NCSPrefSetDoubleInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName, IEEE8 dValue );
static NCSError NCSPrefGetIntInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName, INT32 *pValue );
static NCSError NCSPrefSetIntInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName, INT32 nValue);
static NCSError NCSPrefSetBooleanInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName, BOOLEAN bValue);
static NCSError NCSPrefGetBooleanInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName, BOOLEAN *pValue);
static NCSError NCSPrefDeleteValueInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName);

/* 
**
** General preference functions for initialisation 
** and setting/getting base keys.
**
*/

void NCSPrefMachineUnLock()
{
	NCSMutexEnd(&MachineMutex);
}

void NCSPrefUserUnLock()
{
	NCSMutexEnd(&UserMutex);
}

void NCSPrefInit()
{
	if (!bHaveInit) {
		pBaseMachineKeyName = NCSStrDup(NCSPREF_DEFAULT_BASE_KEY);
		pBaseUserKeyName = NCSStrDup(NCSPREF_DEFAULT_BASE_KEY);
		NCSMutexInit(&MachineMutex);
		NCSMutexInit(&UserMutex);
		bHaveInit = TRUE;
	}
}

void NCSPrefFini()
{
	if (bHaveInit) {
		if (pBaseMachineKeyName) {
			NCSFree(pBaseMachineKeyName);
			pBaseMachineKeyName = (char*)NULL;
		}
		if (pBaseUserKeyName) {
			NCSFree(pBaseUserKeyName);
			pBaseUserKeyName = (char*)NULL;
		}
		NCSMutexFini(&UserMutex);
		NCSMutexFini(&MachineMutex);
		bHaveInit = FALSE;
	}
}
	
NCSError NCSPrefCreateUserKey(char *pKeyName )
{
	HKEY hKey;
	DWORD disp;

	NCSMutexBegin(&UserMutex);

    if (RegCreateKeyEx(	HKEY_CURRENT_USER, 
						OS_STRING(pKeyName ? pKeyName : NCSPREF_DEFAULT_BASE_KEY), // create default key if pKeyName is null
						(DWORD)0,
						OS_STRING((char*)NULL), 
						REG_OPTION_NON_VOLATILE, 
						KEY_ALL_ACCESS,
						NULL, 
						&hKey, 
						&disp ) != ERROR_SUCCESS)
	{
		NCSMutexEnd(&UserMutex);
		return NCS_PREF_INVALID_USER_KEY;
	}
	else {
		RegCloseKey(hKey);

		NCSMutexEnd(&UserMutex);
		return NCS_SUCCESS;
	}
}

NCSError NCSPrefCreateMachineKey(char *pKeyName )
{
	HKEY hKey;
	DWORD disp;

	NCSMutexBegin(&MachineMutex);
    if (RegCreateKeyEx(	HKEY_LOCAL_MACHINE, 
						OS_STRING(pKeyName ? pKeyName : NCSPREF_DEFAULT_BASE_KEY), // create default key if pKeyName is null
						(DWORD)0,
						OS_STRING((char*)NULL), 
						REG_OPTION_NON_VOLATILE, 
						KEY_ALL_ACCESS,
						NULL, 
						&hKey, 
						&disp ) != ERROR_SUCCESS)
	{
		NCSMutexEnd(&MachineMutex);
		return NCS_PREF_INVALID_MACHINE_KEY;
	}
	else {
		RegCloseKey(hKey);

		NCSMutexEnd(&MachineMutex);
		return NCS_SUCCESS;
	}
}

NCSError NCSPrefSetUserKeyLock(char *pKeyName )
{
	if (pKeyName) {
		NCSMutexBegin(&UserMutex);

		if (pBaseUserKeyName) {
			NCSFree(pBaseUserKeyName);
		}
		pBaseUserKeyName = NCSStrDup(pKeyName);
		return NCS_SUCCESS;
	}
	else {
		return NCS_PREF_INVALID_USER_KEY;
	}
}

NCSError NCSPrefSetMachineKeyLock(char *pKeyName )
{
	if (pKeyName) {
		NCSMutexBegin(&MachineMutex);
		if (pBaseMachineKeyName) {
			NCSFree(pBaseMachineKeyName);
		}
		pBaseMachineKeyName = NCSStrDup(pKeyName);
		return NCS_SUCCESS;
	}
	else {
		return NCS_PREF_INVALID_MACHINE_KEY;
	}
}

char *NCSPrefGetUserKey(void)
{
	char *pTmp;

	NCSMutexBegin(&UserMutex);
	pTmp = NCSStrDup(pBaseUserKeyName);
	NCSMutexEnd(&UserMutex);

	return(pTmp);
}

char *NCSPrefGetMachineKey(void)
{
	char *pTmp;
	
	NCSMutexBegin(&MachineMutex);
	pTmp = NCSStrDup(pBaseMachineKeyName);
	NCSMutexEnd(&MachineMutex);

	return(pTmp);
}

/* 
**
** Machine/Global/AllUsers level preference functions
** Base key is HKEY_LOCAL_MACHINE under WIN32
**
*/

NCSError NCSPrefGetString(char *pKeyName, char **pString )
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&MachineMutex);
	error = NCSPrefGetStringInternal(FALSE, pBaseMachineKeyName, pKeyName, pString);
	NCSMutexEnd(&MachineMutex);

	return error;
}

NCSError NCSPrefSetString(char *pKeyName, char *pString )
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&MachineMutex);
	error = NCSPrefSetStringInternal(FALSE, pBaseMachineKeyName, pKeyName, pString);
	NCSMutexEnd(&MachineMutex);
	return error;
}

NCSError NCSPrefGetDouble(char *pKeyName, IEEE8 *pValue )
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&MachineMutex);
	error = NCSPrefGetDoubleInternal(FALSE, pBaseMachineKeyName, pKeyName, pValue);
	NCSMutexEnd(&MachineMutex);
	return error;
}

NCSError NCSPrefSetDouble(char *pKeyName, IEEE8 fValue )
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&MachineMutex);
	error = NCSPrefSetDoubleInternal(FALSE, pBaseMachineKeyName, pKeyName, fValue);
	NCSMutexEnd(&MachineMutex);
	return error;
}

NCSError NCSPrefGetInt(char *pKeyName, INT32 *pInt )
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&MachineMutex);
	error = NCSPrefGetIntInternal(FALSE, pBaseMachineKeyName, pKeyName, pInt);
	NCSMutexEnd(&MachineMutex);
	return error;
}

NCSError NCSPrefSetInt(char *pKeyName, INT32 nValue)
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&MachineMutex);
	error = NCSPrefSetIntInternal(FALSE, pBaseMachineKeyName, pKeyName, nValue);
	NCSMutexEnd(&MachineMutex);
	return error;
}

NCSError NCSPrefSetBoolean(char *pKeyName, BOOLEAN bValue )
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&MachineMutex);
	error = NCSPrefSetBooleanInternal(FALSE, pBaseMachineKeyName, pKeyName, bValue);
	NCSMutexEnd(&MachineMutex);
	return error;
}

NCSError NCSPrefGetBoolean(char *pKeyName, BOOLEAN *pValue )
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&MachineMutex);
	error = NCSPrefGetBooleanInternal(FALSE, pBaseMachineKeyName, pKeyName, pValue);
	NCSMutexEnd(&MachineMutex);
	return error;
}

NCSError NCSPrefDeleteValue(char *pKeyName)
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&MachineMutex);
	error = NCSPrefDeleteValueInternal(FALSE, pBaseMachineKeyName, pKeyName);
	NCSMutexEnd(&MachineMutex);
	return error;
}


/* 
**
** User level preference functions
** Base key is HKEY_CURRENT_USER
**
*/

NCSError NCSPrefGetUserString(char *pKeyName, char **pString )
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&UserMutex);
	error = NCSPrefGetStringInternal(TRUE, pBaseUserKeyName, pKeyName, pString);
	NCSMutexEnd(&UserMutex);
	return error;
}

NCSError NCSPrefSetUserString(char *pKeyName, char *pString )
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&UserMutex);
	error = NCSPrefSetStringInternal(TRUE, pBaseUserKeyName, pKeyName, pString);
	NCSMutexEnd(&UserMutex);
	return error;
}

NCSError NCSPrefGetUserDouble(char *pKeyName, IEEE8 *pValue )
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&UserMutex);
	error = NCSPrefGetDoubleInternal(TRUE, pBaseUserKeyName, pKeyName, pValue );
	NCSMutexEnd(&UserMutex);
	return error;
}

NCSError NCSPrefSetUserDouble(char *pKeyName, IEEE8 dValue )
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&UserMutex);
	error = NCSPrefSetDoubleInternal(TRUE, pBaseUserKeyName, pKeyName, dValue );
	NCSMutexEnd(&UserMutex);
	return error;
}

NCSError NCSPrefGetUserInt(char *pKeyName, INT32 *pValue )
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&UserMutex);
	error = NCSPrefGetIntInternal(TRUE, pBaseUserKeyName, pKeyName, pValue );
	NCSMutexEnd(&UserMutex);
	return error;
}

NCSError NCSPrefSetUserInt(char *pKeyName, INT32 nValue)
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&UserMutex);
	error = NCSPrefSetIntInternal(TRUE, pBaseUserKeyName, pKeyName, nValue );
	NCSMutexEnd(&UserMutex);
	return error;
}

NCSError NCSPrefSetUserBoolean(char *pKeyName, BOOLEAN bValue )
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&UserMutex);
	error = NCSPrefSetBooleanInternal(TRUE, pBaseUserKeyName, pKeyName, bValue);
	NCSMutexEnd(&UserMutex);
	return error;
}

NCSError NCSPrefGetUserBoolean(char *pKeyName, BOOLEAN *pValue )
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&UserMutex);
	error = NCSPrefGetBooleanInternal(TRUE, pBaseUserKeyName, pKeyName, pValue);
	NCSMutexEnd(&UserMutex);
	return error;
}

NCSError NCSPrefDeleteValueUser(char *pKeyName)
{
	NCSError error = NCS_SUCCESS;

	NCSMutexBegin(&UserMutex);
	error = NCSPrefDeleteValueInternal(TRUE, pBaseUserKeyName, pKeyName);
	NCSMutexEnd(&UserMutex);
	return error;
}

/* 
**
** Internal functions for key access
**
*/

static NCSError NCSPrefGetStringInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName, char **pString )
{
	NCSError error;
	HKEY hKey;

	if (!pString || !pKeyName || !pBaseKey)
		return NCS_INVALID_ARGUMENTS;

	if (RegOpenKeyEx((bUserPref ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE),
					 OS_STRING(pBaseKey),
					 0,	
					 KEY_QUERY_VALUE,
					 &hKey) == ERROR_SUCCESS) {
		
		DWORD dwType;
		LPBYTE szValueBuffer[MAX_PATH * 10];
		DWORD dwBufferSize = MAX_PATH * 10;

		szValueBuffer[0] = '\0'; //[05]

		if (pKeyName) {
			if (RegQueryValueEx(hKey, 
								OS_STRING(pKeyName), 
								NULL, 
								&dwType, 
								(unsigned char *) szValueBuffer, 
								&dwBufferSize) != ERROR_SUCCESS) {
				szValueBuffer[0] = '\0';
				error = NCS_REGQUERY_VALUE_FAILED;
			} else {
				if (dwType == REG_SZ) {
					*pString = NCSStrDup(CHAR_STRING((NCSTChar *)szValueBuffer));
					error = NCS_SUCCESS;
				} else { 
					*pString = NULL;
					error = NCS_INVALID_REG_TYPE;
				}
			}
		}
		else {
			*pString = NULL;
			error = NCS_REGQUERY_VALUE_FAILED;	
		}
		RegCloseKey(hKey);
	}
	else {
		*pString = NULL;
		 error = NCS_REGKEY_OPENEX_FAILED;
	}
	return error;
}

static NCSError NCSPrefSetStringInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName, char *pString )
{
	HKEY hKey;

	if (!pString || !pKeyName || !pBaseKey)
		return NCS_INVALID_ARGUMENTS;

	if (RegOpenKeyEx((bUserPref ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE),
					 OS_STRING(pBaseKey),
					 0,	
					 KEY_WRITE,//[02] KEY_SET_VALUE ,
					 &hKey) == ERROR_SUCCESS) {
		
		if (RegSetValueEx(hKey, 
						  OS_STRING(pKeyName), 
						  (DWORD)0, 
						  REG_SZ, 
						  (unsigned char *) OS_STRING(pString), 
						  (DWORD)strlen(pString)) != ERROR_SUCCESS) {
			RegCloseKey(hKey);	/**[01]**/
			return NCS_PREF_INVALID_MACHINE_KEY;
		} else {
			RegCloseKey(hKey);	/**[01]**/
			return NCS_SUCCESS;
		}
	}
	else {
		return NCS_REGKEY_OPENEX_FAILED;
	}
}

static NCSError NCSPrefGetDoubleInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName, IEEE8 *pValue )
{
	NCSError error;
	char *pString;

	error = NCSPrefGetStringInternal(bUserPref, pBaseKey, pKeyName, &pString);
	if(error == NCS_SUCCESS) {
#if defined(_WIN32_WCE) && (_WIN32_WCE <= 211)
		sscanf(pString, "%f", pValue);
#else
		*pValue = atof( pString );
#endif
		NCSFree(pString);
	}
	return(error);
}

static NCSError NCSPrefSetDoubleInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName, IEEE8 dValue )
{
	char String[256];
	sprintf(String, "%lf", dValue);
	return (NCSPrefSetStringInternal(bUserPref, pBaseKey, pKeyName, String));
}

static NCSError NCSPrefSetBooleanInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName, BOOLEAN bValue)
{
	return(NCSPrefSetIntInternal(bUserPref, pBaseKey, pKeyName, (INT32)bValue ));
}

static NCSError NCSPrefGetBooleanInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName, BOOLEAN *pValue)
{
	NCSError error;
	INT32 value;
	error = NCSPrefGetIntInternal(bUserPref, pBaseKey, pKeyName, &value );
	*pValue = (BOOLEAN)value;
	return error;
}

static NCSError NCSPrefGetIntInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName, INT32 *pValue )
{
	NCSError error = NCS_SUCCESS;
	HKEY hKey;

	if (!pKeyName || !pBaseKey || !pValue)
		return NCS_INVALID_ARGUMENTS;

	if (RegOpenKeyEx((bUserPref ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE),
					 OS_STRING(pBaseKey),
					 0,	
					 KEY_QUERY_VALUE,
					 &hKey) == ERROR_SUCCESS) {
		
		DWORD dwType;
		DWORD buffer;
		DWORD dwBufferSize;

		buffer = 0;
		dwBufferSize = sizeof(DWORD);
		dwType = REG_DWORD;

		if (pKeyName) {
			if (RegQueryValueEx(hKey, 
								OS_STRING(pKeyName), 
								NULL, 
								&dwType, 
								(LPBYTE) &buffer,
								&dwBufferSize) != ERROR_SUCCESS) {
				*pValue = 0;
				error = NCS_REGQUERY_VALUE_FAILED;
			}
			else {
				if (dwType == REG_DWORD) {
					*pValue = (INT32)buffer;	
				} else {
					*pValue = 0;
					error =  NCS_INVALID_REG_TYPE;
				}
			}
		}
		else {
			*pValue = 0;
			error = NCS_REGQUERY_VALUE_FAILED;	
		}
		RegCloseKey(hKey);
	}
	else {
		*pValue = 0;
		 error = NCS_REGKEY_OPENEX_FAILED;
	}
	return error;
}

static NCSError NCSPrefDeleteValueInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName)
{
	NCSError error = NCS_SUCCESS;
	HKEY hKey;

	if (!pKeyName || !pBaseKey)
		return NCS_INVALID_ARGUMENTS;

	if (RegOpenKeyEx((bUserPref ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE),
					 OS_STRING(pBaseKey),
					 0,	
					 KEY_SET_VALUE,
					 &hKey) == ERROR_SUCCESS) {
		
		DWORD dwType;
		DWORD buffer;
		DWORD dwBufferSize;

		buffer = 0;
		dwBufferSize = sizeof(DWORD);
		dwType = REG_DWORD;

		if (pKeyName) {
			if(RegDeleteValue(hKey, OS_STRING(pKeyName)) ){
				error = NCS_REGQUERY_VALUE_FAILED;
			}
			else {
				error = NCS_SUCCESS;
			}
		}
		else {
			error = NCS_REGQUERY_VALUE_FAILED;	
		}
		RegCloseKey(hKey);
	}
	else {
		 error = NCS_REGKEY_OPENEX_FAILED;
	}
	return error;
}


static NCSError NCSPrefSetIntInternal(BOOLEAN bUserPref, char *pBaseKey, char *pKeyName, INT32 nValue)
{
	HKEY hKey;

	if (!pKeyName || !pBaseKey)
		return NCS_INVALID_ARGUMENTS;

	if (RegOpenKeyEx((bUserPref ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE),
					 OS_STRING(pBaseKey),
					 0,	
					 KEY_WRITE,//[02] KEY_ALL_ACCESS ,	/* KEY_SET_VALUE  */
					 &hKey) == ERROR_SUCCESS) {
		
		if (RegSetValueEx(hKey, 
						  OS_STRING(pKeyName), 
						  (DWORD)0, 
						  REG_DWORD, 
						  (UINT8 *)&nValue,	/**[04]**/
						  sizeof(INT32) ) != ERROR_SUCCESS) {
			RegCloseKey(hKey);	/**[01]**/
			return NCS_PREF_INVALID_MACHINE_KEY;
		} else {
			RegCloseKey(hKey);	/**[01]**/
			return NCS_SUCCESS;
		}
	}
	else {
		return NCS_REGKEY_OPENEX_FAILED;
	}
}

#endif
