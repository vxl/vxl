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
** FILE:   	NCSPrefs.h
** CREATED:	Fri Jun  4 18:05:04 WST 1999
** AUTHOR: 	Mark Sheridan
** PURPOSE:	NCS Preferences header.
** EDITS:
** [01]	 ny	31Oct00	Merge WinCE/PALM SDK changes
** [02] rar 10Jun04 Re-written using C++ and to use an XML file on non-windows platforms
 *******************************************************/

#ifndef NCSPREFS_H
#define NCSPREFS_H

#ifdef POSIX
	#define NO_REGISTRY
#endif

#include "NCSUtil.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "NCSTypes.h"
#include "NCSErrors.h"
#include "NCSMutex.h"
#include "NCSMalloc.h"

#ifdef POSIX
#	define NCSPREF_DEFAULT_BASE_KEY "Image Web Server"
#	define NCSPREF_DEFAULT_BASE_KEY_T NCS_T("Image Web Server")
#else
#	define NCSPREF_DEFAULT_BASE_KEY "Software\\Earth Resource Mapping\\Image Web Server"
#	define NCSPREF_DEFAULT_BASE_KEY_T NCS_T("Software\\Earth Resource Mapping\\Image Web Server")
#endif

/* 
**
** Utility functions - create delete machine or user level keys
**
*/

// These initialization function are called by NCSUtilInit and NCSUtilFini repectively.
// They most only be called once.
void NCSPrefInit(void);
void NCSPrefFini(void);

// The return value from these function must always be checked.
// If NCS_SUCCESS is returned than there must be a subsequent
// call to the UnLock() once access to the key has finnished.
// If any of the Get/Set functions are used without first setting
// the Key than the default key (NCSPREF_DEFAULT_BASE_KEY) will
// be used.
NCSError NCSPrefCreateUserKeyLock(char *pKeyName );
NCSError NCSPrefCreateMachineKeyLock(char *pKeyName );
NCSError NCSPrefSetMachineKeyLock(char *pKeyName );
NCSError NCSPrefSetUserKeyLock(char *pKeyName );

void NCSPrefMachineUnLock();
void NCSPrefUserUnLock();

/* 
**
** Machine/Global level preference functions
** Base key is HKEY_LOCAL_MACHINE
**
*/
NCSError NCSPrefGetString(char *pKeyName, char **pString );
NCSError NCSPrefSetString(char *pKeyName, char *pString );
NCSError NCSPrefGetDouble(char *pKeyName, IEEE8 *pValue );
NCSError NCSPrefSetDouble(char *pKeyName, IEEE8 dValue );
NCSError NCSPrefGetInt(char *pKeyName, INT32 *pValue );
NCSError NCSPrefSetInt(char *pKeyName, INT32 nValue);
NCSError NCSPrefSetBoolean(char *pKeyname, BOOLEAN bValue );
NCSError NCSPrefGetBoolean(char *pKeyName, BOOLEAN *pValue );
NCSError NCSPrefDeleteValue(char *pKeyName);

/* 
**
** User level preference functions
** Base key is HKEY_CURRENT_USER
**
*/
NCSError NCSPrefGetUserString(char *pKeyName, char **pString );
NCSError NCSPrefSetUserString(char *pKeyName, char *pString );
NCSError NCSPrefGetUserDouble(char *pKeyName, IEEE8 *pValue );
NCSError NCSPrefSetUserDouble(char *pKeyName, IEEE8 dValue );
NCSError NCSPrefGetUserInt(char *pKeyName, INT32 *pValue );
NCSError NCSPrefSetUserInt(char *pKeyName, INT32 nValue );
NCSError NCSPrefSetUserBoolean(char *pKeyname, BOOLEAN bValue );
NCSError NCSPrefGetUserBoolean(char *pKeyName, BOOLEAN *pValue );
NCSError NCSPrefDeleteValueUser(char *pKeyName);

/* 
**
** New preference functions
**
*/
#define MACHINE_KEY TRUE
#define USER_KEY FALSE

NCSError NCSPrefGetStringEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName, char **pString );
NCSError NCSPrefSetStringEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName, char *pString );
NCSError NCSPrefGetDoubleEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName, IEEE8 *pValue );
NCSError NCSPrefSetDoubleEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName, IEEE8 dValue );
NCSError NCSPrefGetIntEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName, INT32 *pValue );
NCSError NCSPrefSetIntEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName, INT32 nValue );
NCSError NCSPrefSetBooleanEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName, BOOLEAN bValue );
NCSError NCSPrefGetBooleanEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName, BOOLEAN *pValue );
NCSError NCSPrefDeleteValueEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName );

#ifdef __cplusplus
}

#ifndef NCSUTIL_MIN_LINK

#include <iostream>
#include "NCSString.h"
#include "NCSError.h"

/*
 * A C++ interface for NCSPrefs
 * The C functions above are wrappers around this class.
 * 
 */
class NCS_EXPORT CNCSPrefs : public CNCSMutex
{
protected:
	CNCSPrefs(){};
	virtual ~CNCSPrefs(){};

public:
	class NCS_EXPORT CNCSPrefsKey
	{
	protected:
		CNCSPrefsKey(CNCSMutex *pmMutex, CNCSString sName);
	public:
		virtual ~CNCSPrefsKey();

		virtual bool Get( CNCSString sName, CNCSString &sValue,CNCSString sDefault=NCS_T("") ) = 0;
		virtual bool Set( CNCSString sName, CNCSString sValue ) = 0;
		virtual bool Get( CNCSString sName, int &nValue, int nDefault=0 ) = 0;
		virtual bool Set( CNCSString sName, int nValue ) = 0;
		virtual bool Get( CNCSString sName, double &dValue, double dDefault=0.0 ) = 0;
		virtual bool Set( CNCSString sName, double dValue ) = 0;
		virtual bool Get( CNCSString sName, bool &bValue, bool bDefault=false ) = 0;
		virtual bool Set( CNCSString sName, bool bValue ) = 0;

		virtual bool DeleteValue( CNCSString sName ) = 0;
		virtual bool DeleteSubKey( CNCSString sSubKey, bool bSubKeys=false ) = 0;

		virtual CNCSPrefsKey *OpenSubKey( CNCSString sSubKey, bool bCreate = false ) = 0;

		virtual CNCSPrefsKey *GetFirstSubKey() = 0;
		virtual CNCSPrefsKey *GetNextSubKey() = 0;

		virtual CNCSString &GetKeyName();

	protected:
		CNCSMutex *m_pmMutex;
		CNCSString m_sName;
	};

	// The user is responsible for deleting the CNCSPrefsKey instance returned by OpenKey.
	// Also access to the Prefs will be Lock to the current thread untill it is deleted.
	virtual CNCSPrefsKey *OpenKey( CNCSString sBaseKey=NCSPREF_DEFAULT_BASE_KEY, bool bCreate=false ) = 0;

	virtual bool DeleteKey( CNCSString sKey ) = 0;

	static CNCSPrefs* GetMachinePrefs();
	static CNCSPrefs* GetUserPrefs();

	static void Fini();

protected:
	static CNCSPrefs *s_pMachinePrefs;
	static CNCSPrefs *s_pUserPrefs;

};

#endif //NCSUTIL_MIN_LINK

#endif //__cplusplus

#endif /* NCSPREFS_H */

