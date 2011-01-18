// NCSPrefs.cpp: implementation of the CNCSPrefsXML class.
//
//////////////////////////////////////////////////////////////////////

#include "NCSPrefs.h"

#ifdef NO_REGISTRY
#	include "NCSPrefsXML.h"
#else
#	include "NCSPrefsWin.h"
#endif


static CNCSPrefs::CNCSPrefsKey *pMachineKey = NULL;
static CNCSPrefs::CNCSPrefsKey *pUserKey = NULL;

static bool bHaveInit = false;


//////////////////////////////////////////////////////////////////////
// CNCSPrefs class
//////////////////////////////////////////////////////////////////////

CNCSPrefs *CNCSPrefs::s_pMachinePrefs = NULL;
CNCSPrefs *CNCSPrefs::s_pUserPrefs = NULL;

CNCSPrefs* CNCSPrefs::GetMachinePrefs()
{
	if( s_pMachinePrefs ) {
		return s_pMachinePrefs;
	}
	return NULL;
}

CNCSPrefs* CNCSPrefs::GetUserPrefs()
{
	if( s_pUserPrefs ) {
		return s_pUserPrefs;
	}
	return NULL;
}

void CNCSPrefs::Fini()
{
	if( s_pMachinePrefs ) delete s_pMachinePrefs;
	if( s_pUserPrefs ) delete s_pUserPrefs;
	s_pMachinePrefs = NULL;
	s_pUserPrefs = NULL;
}

CNCSPrefs::CNCSPrefsKey::CNCSPrefsKey(CNCSMutex *pmMutex, CNCSString sName)
{
	// The mutex "pmMutex" MUST be already locked
	// This mutex is used to only allow one thread access to the
	//  preferences at a time.  This is not needed for registry access 
	//  under windows but is done anyway so that the behavious is the same
	//  as the XML registry implamentation.
	m_pmMutex = pmMutex;
	m_sName = sName;
}

CNCSPrefs::CNCSPrefsKey::~CNCSPrefsKey()
{
	if( m_pmMutex ) {
		m_pmMutex->UnLock();
	}
}

CNCSString &CNCSPrefs::CNCSPrefsKey::GetKeyName()
{
	return m_sName;
}

//////////////////////////////////////////////////////////////////////
// C wrapper functins
//////////////////////////////////////////////////////////////////////

void NCSPrefInit(void)
{
	if( !bHaveInit ) {
		bHaveInit = true;

#ifdef NO_REGISTRY
		CNCSPrefsXML::Init();
#else
		CNCSPrefsWin::Init();
#endif
	}
}

void NCSPrefFini(void)
{
	if( bHaveInit ) {
		bHaveInit = false;
		CNCSPrefs::Fini();
	}
}

NCSError NCSPrefCreateMachineKeyLock(char *pKeyName )
{
	if( !bHaveInit || pMachineKey ) return NCS_UNKNOWN_ERROR;

	if( !pKeyName ) {
		pKeyName = NCSPREF_DEFAULT_BASE_KEY;
	}

	CNCSPrefs *pMachinePrefs = CNCSPrefs::GetMachinePrefs();
	if( pMachinePrefs ) {

		CNCSMutexLock lock(pMachinePrefs);

		pMachineKey = pMachinePrefs->OpenKey( pKeyName, true );
		if( pMachineKey ) {
			return NCS_SUCCESS;
		} else {
			return NCS_PREF_INVALID_MACHINE_KEY;
		}
	}

	return NCS_INVALID_REG_TYPE;
}

NCSError NCSPrefSetMachineKeyLock(char *pKeyName )
{
	if( !bHaveInit || pMachineKey ) return NCS_UNKNOWN_ERROR;

	if( !pKeyName ) {
		pKeyName = NCSPREF_DEFAULT_BASE_KEY;
	}

	CNCSPrefs *pMachinePrefs = CNCSPrefs::GetMachinePrefs();
	if( pMachinePrefs ) {

		CNCSMutexLock lock(pMachinePrefs);

		pMachineKey = pMachinePrefs->OpenKey( pKeyName );
		if( pMachineKey ) {
			return NCS_SUCCESS;
		} else {
			return NCS_PREF_INVALID_MACHINE_KEY;
		}
	}

	return NCS_INVALID_REG_TYPE;
}

NCSError NCSPrefCreateUserKeyLock(char *pKeyName )
{
	if( !bHaveInit || pUserKey ) return NCS_UNKNOWN_ERROR;

	if( !pKeyName ) {
		pKeyName = NCSPREF_DEFAULT_BASE_KEY;
	}

	CNCSPrefs *pUserPrefs = CNCSPrefs::GetUserPrefs();
	if(pUserPrefs) {
		CNCSMutexLock lock(pUserPrefs);

		pUserKey = pUserPrefs->OpenKey( pKeyName, true );
		if( pUserKey ) {
			return NCS_SUCCESS;
		} else {
			return NCS_PREF_INVALID_USER_KEY;
		}
	}

	return NCS_INVALID_REG_TYPE;
}

NCSError NCSPrefSetUserKeyLock(char *pKeyName )
{
	if( !bHaveInit || pUserKey ) return NCS_UNKNOWN_ERROR;

	if( !pKeyName ) {
		pKeyName = NCSPREF_DEFAULT_BASE_KEY;
	}

	CNCSPrefs *pUserPrefs = CNCSPrefs::GetUserPrefs();
	if(pUserPrefs) {
		CNCSMutexLock lock(pUserPrefs);

		pUserKey = pUserPrefs->OpenKey( pKeyName );
		if( pUserKey ) {
			return NCS_SUCCESS;
		} else {
			return NCS_PREF_INVALID_USER_KEY;
		}
	}

	return NCS_INVALID_REG_TYPE;
}

void NCSPrefMachineUnLock()
{
	CNCSPrefs *pMachinePrefs = CNCSPrefs::GetMachinePrefs();
	if( pMachinePrefs ) {
		CNCSMutexLock lock(pMachinePrefs);
		if( pMachineKey ) {
			delete pMachineKey;
			pMachineKey = NULL;
		}
	}
}

void NCSPrefUserUnLock()
{
	CNCSPrefs *pUserPrefs = CNCSPrefs::GetUserPrefs();
	if(pUserPrefs) {
		CNCSMutexLock lock(pUserPrefs);
		if( pUserKey ) {
			delete pUserKey;
			pUserKey = NULL;
		}
	}
}

//
// Machine
//

#define MACHINE_START \
	if( !bHaveInit ) return NCS_UNKNOWN_ERROR; \
	NCSError eError = NCS_REGQUERY_VALUE_FAILED; \
	CNCSPrefs *pMachinePrefs = CNCSPrefs::GetMachinePrefs(); \
	if(pMachinePrefs && pKeyName) { \
		bool bUseDefaultKey = false; \
		eError = NCS_SUCCESS; \
		CNCSMutexLock lock(pMachinePrefs); \
		if( !pMachineKey ) { \
			bUseDefaultKey = true; \
			eError = NCSPrefSetMachineKeyLock(NCSPREF_DEFAULT_BASE_KEY); \
		} \
		if( (eError == NCS_SUCCESS) && pMachineKey ) {

#define MACHINE_END \
			if( bUseDefaultKey ) { \
				NCSPrefMachineUnLock(); \
			} \
		} \
	} \
	return eError;


NCSError NCSPrefGetString(char *pKeyName, char **pString )
{
	MACHINE_START;

	CNCSString sValue;
	if( pString && pMachineKey->Get( pKeyName, sValue ) ) {
		*pString = NCSStrDup( (char *)sValue.a_str() );
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	MACHINE_END;
}
NCSError NCSPrefSetString(char *pKeyName, char *pString )
{
	MACHINE_START;

	if( pString && pMachineKey->Set( pKeyName, CNCSString(pString) ) ) {
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	MACHINE_END;
}
NCSError NCSPrefGetDouble(char *pKeyName, IEEE8 *pValue )
{
	MACHINE_START;

	IEEE8 dValue;
	if( pValue && pMachineKey->Get( pKeyName, dValue ) ) {
		*pValue = dValue;
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	MACHINE_END;
}
NCSError NCSPrefSetDouble(char *pKeyName, IEEE8 dValue )
{
	MACHINE_START;

	if( pMachineKey->Set( pKeyName, dValue ) ) {
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	MACHINE_END;
}
NCSError NCSPrefGetInt(char *pKeyName, INT32 *pValue )
{
	MACHINE_START;

	INT32 nValue;
	if( pValue && pMachineKey->Get( pKeyName, nValue ) ) {
		*pValue = nValue;
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	MACHINE_END;
}
NCSError NCSPrefSetInt(char *pKeyName, INT32 nValue)
{
	MACHINE_START;

	if( pMachineKey->Set( pKeyName, nValue ) ) {
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	MACHINE_END;
}
NCSError NCSPrefGetBoolean(char *pKeyName, BOOLEAN *pValue )
{
	MACHINE_START;

	bool bValue;
	if( pValue && pMachineKey->Get( pKeyName, bValue ) ) {
		*pValue = bValue;
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	MACHINE_END;
}
NCSError NCSPrefSetBoolean(char *pKeyName, BOOLEAN bValue )
{
	MACHINE_START;

	if( pMachineKey->Set( pKeyName, (bValue?true:false) ) ) {
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	MACHINE_END;
}
NCSError NCSPrefDeleteValue(char *pKeyName)
{
	MACHINE_START;

	if( pMachineKey->DeleteValue( pKeyName ) ) {
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	MACHINE_END;
}

//
// User
//
#define USER_START \
	if( !bHaveInit ) return NCS_UNKNOWN_ERROR; \
	NCSError eError = NCS_REGQUERY_VALUE_FAILED; \
	CNCSPrefs *pUserPrefs = CNCSPrefs::GetUserPrefs(); \
	if(pUserPrefs && pKeyName) { \
		bool bUseDefaultKey = false; \
		eError = NCS_SUCCESS; \
		CNCSMutexLock lock(pUserPrefs); \
		if( !pUserKey ) { \
			bUseDefaultKey = true; \
			eError = NCSPrefSetUserKeyLock(NCSPREF_DEFAULT_BASE_KEY); \
		} \
		if( (eError == NCS_SUCCESS) && pUserKey ) {

#define USER_END \
			if( bUseDefaultKey ) { \
				NCSPrefUserUnLock(); \
			} \
		} \
	} \
	return eError;


NCSError NCSPrefGetUserString(char *pKeyName, char **pString )
{
	USER_START;

	CNCSString sValue;
	if( pString && pUserKey->Get( pKeyName, sValue ) ) {
		*pString = NCSStrDup( (char *)sValue.a_str() );
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	USER_END;
}
NCSError NCSPrefSetUserString(char *pKeyName, char *pString )
{
	USER_START;

	if( pString && pUserKey->Set( pKeyName, CNCSString(pString) ) ) {
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	USER_END;
}
NCSError NCSPrefGetUserDouble(char *pKeyName, IEEE8 *pValue )
{
	USER_START;

	IEEE8 dValue;
	if( pValue && pUserKey->Get( pKeyName, dValue ) ) {
		*pValue = dValue;
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	USER_END;
}
NCSError NCSPrefSetUserDouble(char *pKeyName, IEEE8 dValue )
{
	USER_START;

	if( pUserKey->Set( pKeyName, dValue ) ) {
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	USER_END;
}
NCSError NCSPrefGetUserInt(char *pKeyName, INT32 *pValue )
{
	USER_START;

	INT32 nValue;
	if( pValue && pUserKey->Get( pKeyName, nValue ) ) {
		*pValue = nValue;
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	USER_END;
}
NCSError NCSPrefSetUserInt(char *pKeyName, INT32 nValue)
{
	USER_START;

	if( pUserKey->Set( pKeyName, nValue ) ) {
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	USER_END;
}
NCSError NCSPrefGetUserBoolean(char *pKeyName, BOOLEAN *pValue )
{
	USER_START;

	bool bValue;
	if( pValue && pUserKey->Get( pKeyName, bValue ) ) {
		*pValue = bValue;
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	USER_END;
}
NCSError NCSPrefSetUserBoolean(char *pKeyName, BOOLEAN bValue )
{
	USER_START;

	if( pUserKey->Set( pKeyName, (bValue?true:false) ) ) {
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	USER_END;
}
NCSError NCSPrefDeleteValueUser(char *pKeyName)
{
	USER_START;

	if( pUserKey->DeleteValue( pKeyName ) ) {
		eError = NCS_SUCCESS;
	} else {
		eError = NCS_REGQUERY_VALUE_FAILED;
	}

	USER_END;
}

/* 
**
** New preference functions
**
*/
NCSError NCSPrefGetStringEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName, char **pString )
{
	NCSError eError = NCS_SUCCESS;
	if( bIsMachine ) { //MACHINE_KEY
		eError = NCSPrefSetMachineKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefGetString( pValueName, pString );
			NCSPrefMachineUnLock();
		}
	} else {
		eError = NCSPrefSetUserKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefGetUserString( pValueName, pString );
			NCSPrefUserUnLock();
		}
	}

	return eError;
}

NCSError NCSPrefSetStringEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName, char *pString )
{
	NCSError eError = NCS_SUCCESS;
	if( bIsMachine ) { //MACHINE_KEY
		eError = NCSPrefSetMachineKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefSetString( pValueName, pString );
			NCSPrefMachineUnLock();
		}
	} else {
		eError = NCSPrefSetUserKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefSetUserString( pValueName, pString );
			NCSPrefUserUnLock();
		}
	}

	return eError;
}

NCSError NCSPrefGetDoubleEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName, IEEE8 *pValue )
{
	NCSError eError = NCS_SUCCESS;
	if( bIsMachine ) { //MACHINE_KEY
		eError = NCSPrefSetMachineKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefGetDouble( pValueName, pValue );
			NCSPrefMachineUnLock();
		}
	} else {
		eError = NCSPrefSetUserKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefGetUserDouble( pValueName, pValue );
			NCSPrefUserUnLock();
		}
	}

	return eError;
}

NCSError NCSPrefSetDoubleEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName, IEEE8 dValue )
{
	NCSError eError = NCS_SUCCESS;
	if( bIsMachine ) { //MACHINE_KEY
		eError = NCSPrefSetMachineKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefSetDouble( pValueName, dValue );
			NCSPrefMachineUnLock();
		}
	} else {
		eError = NCSPrefSetUserKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefSetUserDouble( pValueName, dValue );
			NCSPrefUserUnLock();
		}
	}

	return eError;
}

NCSError NCSPrefGetIntEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName, INT32 *pValue )
{
	NCSError eError = NCS_SUCCESS;
	if( bIsMachine ) { //MACHINE_KEY
		eError = NCSPrefSetMachineKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefGetInt( pValueName, pValue );
			NCSPrefMachineUnLock();
		}
	} else {
		eError = NCSPrefSetUserKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefGetUserInt( pValueName, pValue );
			NCSPrefUserUnLock();
		}
	}

	return eError;
}

NCSError NCSPrefSetIntEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName, INT32 nValue )
{
	NCSError eError = NCS_SUCCESS;
	if( bIsMachine ) { //MACHINE_KEY
		eError = NCSPrefSetMachineKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefSetInt( pValueName, nValue );
			NCSPrefMachineUnLock();
		}
	} else {
		eError = NCSPrefSetUserKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefSetUserInt( pValueName, nValue );
			NCSPrefUserUnLock();
		}
	}

	return eError;
}

NCSError NCSPrefSetBooleanEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName, BOOLEAN bValue )
{
	NCSError eError = NCS_SUCCESS;
	if( bIsMachine ) { //MACHINE_KEY
		eError = NCSPrefSetMachineKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefSetBoolean( pValueName, bValue );
			NCSPrefMachineUnLock();
		}
	} else {
		eError = NCSPrefSetUserKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefSetUserBoolean( pValueName, bValue );
			NCSPrefUserUnLock();
		}
	}

	return eError;
}

NCSError NCSPrefGetBooleanEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName, BOOLEAN *pValue )
{
	NCSError eError = NCS_SUCCESS;
	if( bIsMachine ) { //MACHINE_KEY
		eError = NCSPrefSetMachineKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefGetBoolean( pValueName, pValue );
			NCSPrefMachineUnLock();
		}
	} else {
		eError = NCSPrefSetUserKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefGetUserBoolean( pValueName, pValue );
			NCSPrefUserUnLock();
		}
	}

	return eError;
}

NCSError NCSPrefDeleteValueEx(BOOLEAN bIsMachine, char *pKeyName, char *pValueName )
{
	NCSError eError = NCS_SUCCESS;
	if( bIsMachine ) { //MACHINE_KEY
		eError = NCSPrefSetMachineKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefDeleteValue( pValueName );
			NCSPrefMachineUnLock();
		}
	} else {
		eError = NCSPrefSetUserKeyLock(pKeyName );
		if( eError == NCS_SUCCESS ) {
			eError = NCSPrefDeleteValueUser( pValueName );
			NCSPrefUserUnLock();
		}
	}

	return eError;
}

