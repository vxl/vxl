// NCSPrefs.cpp: implementation of the CNCSPrefsWin class.
//
//////////////////////////////////////////////////////////////////////

#include "NCSPrefs.h"
#include "NCSPrefsWin.h"

//////////////////////////////////////////////////////////////////////
// Statics
//////////////////////////////////////////////////////////////////////


void CNCSPrefsWin::Init()
{
	s_pMachinePrefs = new CNCSPrefsWin(false);
	s_pUserPrefs = new CNCSPrefsWin(true);
}

//////////////////////////////////////////////////////////////////////
// CNCSPrefsWin, Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSPrefsWin::CNCSPrefsWin( bool bUserPref )
{
	m_bUserPref = bUserPref;

	//Create default key
	CNCSPrefsKey *pSoftwareKey = OpenKey( "Software\\Earth Resource Mapping\\Image Web Server", true );
	if( pSoftwareKey ) {
		delete pSoftwareKey;
	}
}

CNCSPrefsWin::~CNCSPrefsWin()
{

}

//////////////////////////////////////////////////////////////////////
// Members
//////////////////////////////////////////////////////////////////////

CNCSPrefs::CNCSPrefsKey *CNCSPrefsWin::OpenKey( CNCSString sBaseKey, bool bCreate )
{
	Lock();

	CNCSPrefsKeyWin *pKey = CNCSPrefsKeyWin::OpenKey( this, (m_bUserPref?HKEY_CURRENT_USER:HKEY_LOCAL_MACHINE), sBaseKey, bCreate );

	if( pKey ) {
		return pKey;
	}

	UnLock();
	return NULL;
}

bool CNCSPrefsWin::DeleteKey( CNCSString sSubKey )
{
	bool bResult = false;
	Lock();

	bResult = CNCSPrefsKeyWin::DeleteKey( (m_bUserPref?HKEY_CURRENT_USER:HKEY_LOCAL_MACHINE), sSubKey );

	UnLock();
	return bResult;
}

//////////////////////////////////////////////////////////////////////
// CNCSPrefsWin::CNCSPrefsKeyWin
//////////////////////////////////////////////////////////////////////
CNCSPrefsWin::CNCSPrefsKeyWin::CNCSPrefsKeyWin( CNCSMutex *pmMutex, CNCSString sName, HKEY hKey )
: CNCSPrefs::CNCSPrefsKey(pmMutex, sName)
{
	m_pmMutex = pmMutex;
	m_hBaseKey = hKey;
	m_nEnumKeyIndex = -1;
}

CNCSPrefsWin::CNCSPrefsKeyWin::~CNCSPrefsKeyWin()
{
	RegCloseKey(m_hBaseKey);
}

// Utility function
CNCSString ExtractKeyName( CNCSString sFullPath )
{
	CNCSString sName;

	// Remove any trailing '\\'
	if( sFullPath[ sFullPath.size()-1 ] == '\\' ) {
		sFullPath.erase( sFullPath.size()-1, 1 );
	}

	int nStart = (int)sFullPath.find_last_of(NCS_T("\\"));
	if( nStart == CNCSString::npos ) {
		nStart = 0;
	} else {
		nStart++;
	}
	sName = sFullPath.substr( nStart );

	return sName;
}

CNCSPrefsWin::CNCSPrefsKeyWin *CNCSPrefsWin::CNCSPrefsKeyWin::OpenKey( CNCSMutex *pmMutex, HKEY hBaseKey, CNCSString sSubKey, bool bCreate )
{
	HKEY hKey = NULL;
	DWORD disp;

	if(RegOpenKeyEx(hBaseKey,
					 sSubKey.c_str(),
					 0,	
					 KEY_ALL_ACCESS,
					 &hKey) == ERROR_SUCCESS) {
		return new CNCSPrefsKeyWin( pmMutex, ExtractKeyName(sSubKey), hKey );
	} else if( bCreate ) {
		if (RegCreateKeyEx(	hBaseKey, 
							sSubKey.c_str(),
							(DWORD)0,
							(NCSTChar*)NULL, 
							REG_OPTION_NON_VOLATILE, 
							KEY_ALL_ACCESS,
							NULL, 
							&hKey, 
							&disp ) == ERROR_SUCCESS)
		{
			return new CNCSPrefsKeyWin( pmMutex, ExtractKeyName(sSubKey), hKey );
		}

	}
	return NULL;
}

bool CNCSPrefsWin::CNCSPrefsKeyWin::DeleteKey( HKEY hBaseKey, CNCSString sSubKey )
{
	LONG nResult = RegDeleteKey(hBaseKey, sSubKey.c_str() );

	if( nResult == ERROR_SUCCESS ) return true;
	else return false;
}

CNCSPrefs::CNCSPrefsKey *CNCSPrefsWin::CNCSPrefsKeyWin::GetFirstSubKey()
{
	m_nEnumKeyIndex = -1;

	return GetNextSubKey();
}

CNCSPrefs::CNCSPrefsKey *CNCSPrefsWin::CNCSPrefsKeyWin::GetNextSubKey()
{
	m_nEnumKeyIndex++;

    NCSTChar achKey[MAX_PATH];
	FILETIME ftLastWriteTime;
	unsigned long nKeyNameLength = MAX_PATH;

	DWORD retCode = RegEnumKeyEx(m_hBaseKey, 
                 m_nEnumKeyIndex, 
                 achKey, 
                 &nKeyNameLength, 
                 NULL, 
                 NULL, 
                 NULL, 
                 &ftLastWriteTime); 

	if( retCode == ERROR_SUCCESS ) {
		return OpenSubKey( achKey );
	}

	return NULL;
}

bool CNCSPrefsWin::CNCSPrefsKeyWin::DeleteValue( CNCSString sName )
{
	if(RegDeleteValue(m_hBaseKey, sName.c_str()) ){
		//error = NCS_REGQUERY_VALUE_FAILED;
		return false;
	}
	else {
		return true;
	}
}

bool CNCSPrefsWin::CNCSPrefsKeyWin::Get( CNCSString sName, CNCSString &sValue, CNCSString sDefault )
{
	DWORD dwType;
	NCSTChar szValueBuffer[MAX_PATH * 10];
#ifdef NCS_BUILD_UNICODE
	DWORD dwBufferSize = MAX_PATH * 10 * 2;
#else
	DWORD dwBufferSize = MAX_PATH * 10;
#endif

	szValueBuffer[0] = '\0'; //[05]

	if (RegQueryValueEx(m_hBaseKey, 
						sName.c_str(), 
						NULL, 
						&dwType, 
						(LPBYTE)szValueBuffer, 
						&dwBufferSize) != ERROR_SUCCESS) {
		szValueBuffer[0] = '\0';
		//error = NCS_REGQUERY_VALUE_FAILED;
		sValue = sDefault;
		return false;
	} else {
		if (dwType == REG_SZ) {
			sValue = szValueBuffer;
			return true;
		} else {
			sValue = sDefault;
			return false;
			//error = NCS_INVALID_REG_TYPE;
		}
	}
}
bool CNCSPrefsWin::CNCSPrefsKeyWin::Set( CNCSString sName, CNCSString sValue )
{
	if (RegSetValueEx(m_hBaseKey, 
					  sName.c_str(), 
					  (DWORD)NULL, 
					  REG_SZ,
#ifdef NCS_BUILD_UNICODE
					  (BYTE *)sValue.c_str(), 
					  (DWORD)sValue.size()*2) != ERROR_SUCCESS) {
#else
					  (const unsigned char *)sValue.c_str(), 
					  sValue.size()) != ERROR_SUCCESS) {
#endif
		return false;
	} else {
		return true;
	}
}
bool CNCSPrefsWin::CNCSPrefsKeyWin::Get( CNCSString sName, int &nValue, int nDefault )
{
	DWORD dwType;
	DWORD buffer;
	DWORD dwBufferSize;

	buffer = 0;
	dwBufferSize = sizeof(DWORD);
	dwType = REG_DWORD;

	if (RegQueryValueEx(m_hBaseKey, 
						sName.c_str(), 
						NULL, 
						&dwType, 
						(LPBYTE) &buffer,
						&dwBufferSize) != ERROR_SUCCESS) {
		//error = NCS_REGQUERY_VALUE_FAILED;
		nValue = nDefault;
		return false;
	}
	else {
		if (dwType == REG_DWORD) {
			nValue = (INT32)buffer;
			return true;
		} else {
			//error =  NCS_INVALID_REG_TYPE;
			nValue = nDefault;
			return false;
		}
	}
}
bool CNCSPrefsWin::CNCSPrefsKeyWin::Set( CNCSString sName, int nValue )
{
	if (RegSetValueEx(m_hBaseKey, 
					  sName.c_str(), 
					  (DWORD)NULL, 
					  REG_DWORD, 
					  (UINT8 *)&nValue,	/**[04]**/
					  sizeof(INT32) ) != ERROR_SUCCESS) {
		//return NCS_PREF_INVALID_MACHINE_KEY;
		return false;
	} else {
		return true;
	}
}
bool CNCSPrefsWin::CNCSPrefsKeyWin::Get( CNCSString sName, double &dValue, double dDefault )
{
	CNCSString sValue;
	if( Get( sName, sValue ) ) {
#ifdef NCS_BUILD_UNICODE
		if( swscanf(sValue.c_str(), NCS_T("%lf"), &dValue) == 1 ) {
#else
		if( sscanf(sValue.c_str(), "%lf", &dValue) == 1 ) {
#endif
			return true;
		}
	}
	dValue = dDefault;
	return false;
}
bool CNCSPrefsWin::CNCSPrefsKeyWin::Set( CNCSString sName, double dValue )
{
	char szString[256];
	sprintf(szString, "%lf", dValue);

	return Set( sName, CNCSString(szString) );
}
bool CNCSPrefsWin::CNCSPrefsKeyWin::Get( CNCSString sName, bool &bValue, bool bDefault )
{
	int nValue=0;
	if( Get(sName, nValue) ) {
		bValue = (nValue?true:false);
		return true;
	}

	bValue = bDefault;
	return false;
}
bool CNCSPrefsWin::CNCSPrefsKeyWin::Set( CNCSString sName, bool bValue )
{
	return Set( sName, (bValue?1:0) );
}

bool CNCSPrefsWin::CNCSPrefsKeyWin::DeleteSubKey( CNCSString sSubKey, bool bSubKeys )
{
	if( bSubKeys ) {
		// Open Key to be deleted
		CNCSPrefsKey *pKey = OpenSubKey( sSubKey );
		if( pKey ) {
			// enumerate subkeys and delete them
			CNCSPrefsKey *pSubKey = pKey->GetFirstSubKey();
			while( pSubKey ) {
				pKey->DeleteSubKey( pSubKey->GetKeyName(), true );

				pSubKey = pKey->GetNextSubKey();
			}
		}
	}

	return DeleteKey( m_hBaseKey, sSubKey );
}
