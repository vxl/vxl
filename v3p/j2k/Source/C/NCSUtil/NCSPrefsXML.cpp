// NCSPrefs.cpp: implementation of the CNCSPrefsXML class.
//
//////////////////////////////////////////////////////////////////////

#include "NCSPrefs.h"
#include "NCSPrefsXML.h"

#include "NCSString.h"

//////////////////////////////////////////////////////////////////////
// Statics
//////////////////////////////////////////////////////////////////////


void CNCSPrefsXML::Init()
{
#ifdef WIN32
	CNCSString sConfigDir = "C:\\Program Files\\Earth Resource Mapping\\Image Web Server\\Config\\";
	CNCSString sUserPrefsFile = sConfigDir;
	sUserPrefsFile += NCS_T("ncsuserprefs.xml");
	if( s_pUserPrefs == NULL ) s_pUserPrefs = new CNCSPrefsXML(sUserPrefsFile);

	CNCSString sMachinePrefsFile = sConfigDir;
	sMachinePrefsFile += NCS_T("prefs.xml");
	if( s_pMachinePrefs == NULL ) s_pMachinePrefs = new CNCSPrefsXML(sMachinePrefsFile);
#else
	// Setup the users prefs file
	CNCSString sUserPrefsFile;
	char *pUserPrefsFile = getenv("NCS_USER_PREFS");
	if( pUserPrefsFile ) {
		sUserPrefsFile = pUserPrefsFile;
	} else {
		char *pHomeDir = getenv("HOME");
		if( pHomeDir ) {
			sUserPrefsFile.Format( NCS_T("%s%s"), pHomeDir, NCS_T("/.erm/ncsuserprefs.xml") );
		} else {
			sUserPrefsFile = NCS_T("/etc/erm/ncsuserprefs.xml");
		}
	}
	if( s_pUserPrefs == NULL ) s_pUserPrefs = new CNCSPrefsXML(sUserPrefsFile.c_str());

	// Setup the machine prefs file
	CNCSString sMachinePrefsFile;
	
	char *pConfigFile = getenv("NCS_MACHINE_PREFS");
	if( pConfigFile ) {
		sMachinePrefsFile = pConfigFile;
	} else {
		if( NCSIsIWS() ) {
			char *pIWSConfigFile = getenv("IWS_PREFS");
			if( pIWSConfigFile ) {
				sMachinePrefsFile = pIWSConfigFile;
			} else {			
				sMachinePrefsFile = "/usr/local/erm/ImageWebServer/conf/prefs.xml";

				if( NCSFileSizeBytes( OS_STRING(sMachinePrefsFile.a_str()) ) >= 0 ) {
				} else if( NCSFileSizeBytes( OS_STRING("/etc/erm/ncsprefs.xml") ) >= 0 ) {
					sMachinePrefsFile = "/etc/erm/ncsprefs.xml";
				}
			}
		} else {
			sMachinePrefsFile = "/etc/erm/ncsprefs.xml";
		}
	}
	if( s_pMachinePrefs == NULL ) s_pMachinePrefs = new CNCSPrefsXML(sMachinePrefsFile.c_str());
#endif
}

//////////////////////////////////////////////////////////////////////
// CNCSPrefsXML, Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSPrefsXML::CNCSPrefsXML( CNCSString sFilename )
{
	m_bUpdated = false;
	m_sFilename = sFilename;

	if( !m_tixDoc.LoadFile( sFilename.a_str() ) ) {
		//Log error
	}

	TiXmlElement *pRootElement = m_tixDoc.RootElement();
	if( pRootElement ) {
		if( strcmp( pRootElement->Value(), "prefs" ) == 0 ) {
			//Root node exists and is correct type
		} else {
			pRootElement = NULL;
		}
	}

	if( !pRootElement ) {
		const char *pTemplate = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			"<prefs></prefs>\n";

		m_tixDoc.Clear();
		m_tixDoc.Parse(pTemplate);
	}

	//Create default key
	CNCSPrefsKey *pSoftwareKey = OpenKey( NCSPREF_DEFAULT_BASE_KEY_T, true );
	if( pSoftwareKey ) {
		// Save the file
		m_tixDoc.SaveFile( m_sFilename.a_str() );

		delete pSoftwareKey;
	}

	Spawn();
}

CNCSPrefsXML::~CNCSPrefsXML()
{
	m_tixDoc.SaveFile( m_sFilename.a_str() );
}

//////////////////////////////////////////////////////////////////////
// Members
//////////////////////////////////////////////////////////////////////

CNCSPrefs::CNCSPrefsKey *CNCSPrefsXML::OpenKey( CNCSString sBaseKey, bool bCreate=false )
{
	Lock();

	CNCSPrefsKeyXML *pKey = CNCSPrefsKeyXML::OpenKey( this, m_tixDoc.RootElement(), sBaseKey, bCreate );

	if( pKey ) {
		return pKey;
	}

	UnLock();
	return NULL;
}

bool CNCSPrefsXML::DeleteKey( CNCSString sKey )
{
	return false;
}

void CNCSPrefsXML::Work( void *pData )
{
	while( Run() ) {
		Lock();
		if( m_bUpdated ) {
			m_tixDoc.SaveFile( m_sFilename.a_str() );
			m_bUpdated = false;
		}
		UnLock();

		NCSSleep( 60*1000 );
	}
}

//////////////////////////////////////////////////////////////////////
// CNCSPrefsXML::CNCSPrefsKeyXML
//////////////////////////////////////////////////////////////////////
CNCSPrefsXML::CNCSPrefsKeyXML::CNCSPrefsKeyXML( CNCSPrefsXML *pPrefsXML, CNCSString sName, TiXmlElement *tixBaseElement )
: CNCSPrefs::CNCSPrefsKey(pPrefsXML, sName)
{
	m_pPrefsXML = pPrefsXML;
	m_pBaseElement = tixBaseElement;
	m_pEnumItem = NULL;
	m_bUpdated = false;
}

CNCSPrefsXML::CNCSPrefsKeyXML::~CNCSPrefsKeyXML()
{
	if( m_bUpdated ) {
		m_pPrefsXML->Invalidate();
	}
}

CNCSPrefsXML::CNCSPrefsKeyXML *CNCSPrefsXML::CNCSPrefsKeyXML::OpenKey( CNCSPrefsXML *pPrefsXML, TiXmlElement *tixBaseElement, CNCSString sBaseKey, bool bCreate )
{
	TiXmlElement *tixElement = OpenKey( sBaseKey, tixBaseElement, bCreate );

	//if it exists
	if( tixElement ) {
		// Get the name
		const char *pName = tixElement->Attribute("name");
		if( pName ) {
			return new CNCSPrefsKeyXML( pPrefsXML, pName, tixElement );
		}
	}

	return NULL;
}

TiXmlElement *CNCSPrefsXML::CNCSPrefsKeyXML::OpenKey( CNCSString sKey, TiXmlElement *tixBaseElement, bool bCreate )
{
	bool bRecurse=false;
	CNCSString sNode;

	//Find the first '\'
	CNCSString::size_type nFPos = sKey.find(NCS_T("\\"));
	if( nFPos != CNCSString::npos ) {
		sNode = sKey.substr( 0, nFPos );
		sKey.erase(0, nFPos+1);
		bRecurse = true;
	} else {
		sNode = sKey;
		bRecurse = false;
	}

	//Open node
	TiXmlElement* nextElement = tixBaseElement->FirstChildElement("k");
	while( nextElement ) {
		const char *pAttrName = nextElement->Attribute("name");
		if( pAttrName && ( sNode.CompareNoCase( pAttrName ) ) ) {
			if( bRecurse ) {
				return OpenKey( sKey, nextElement, bCreate );
			} else {
				return nextElement;
			}
		}
		nextElement = nextElement->NextSiblingElement("k");
	}

	//Else node does not exist
	if( bCreate ) {
		TiXmlElement key( "k" );
		key.SetAttribute( "name", sNode.a_str() );
		
		TiXmlNode* newNode = tixBaseElement->InsertEndChild( key );
		if( newNode ) {
			nextElement = newNode->ToElement();
			if( nextElement ) {
				if( bRecurse ) {
					return OpenKey( sKey, nextElement, bCreate );
				} else {
					return nextElement;
				}
			}
		}
	}

	return NULL;
}


CNCSPrefs::CNCSPrefsKey *CNCSPrefsXML::CNCSPrefsKeyXML::GetFirstSubKey()
{
	m_pEnumItem = NULL;

	return GetNextSubKey();
}

CNCSPrefs::CNCSPrefsKey *CNCSPrefsXML::CNCSPrefsKeyXML::GetNextSubKey()
{
	if( m_pEnumItem == NULL ) {
		m_pEnumItem = m_pBaseElement->FirstChildElement("k");
	} else {
		m_pEnumItem = m_pEnumItem->NextSiblingElement("k");
	}
	if( m_pEnumItem ) {
		const char *pName = m_pEnumItem->Attribute("name");
		if( pName ) {
			return new CNCSPrefsKeyXML( NULL, pName, m_pEnumItem );
		}
	}
	return NULL;
}

TiXmlElement *CNCSPrefsXML::CNCSPrefsKeyXML::GetElement( CNCSString sName, CNCSString sType, bool bCreate )
{
	TiXmlElement* nextElement = m_pBaseElement->FirstChildElement("v");
	while( nextElement ) {
		const char *pName = nextElement->Attribute("name");
		if( pName && ( sName.CompareNoCase( pName ) ) ) {
			const char *pType = nextElement->Attribute("type");
			if( (sType == NCS_T("")) || (pType && (sType.CompareNoCase( pType ))) ) {
				return nextElement;
			}
		}
		nextElement = nextElement->NextSiblingElement("v");
	}

	// The value does not exist
	if( bCreate ) {
		DeleteValue(sName);  //This is to prevent multiple values with the same name but differnt types

		TiXmlElement value( "v" );
		value.SetAttribute( "name", sName.a_str() );
		value.SetAttribute( "value", "" );
		value.SetAttribute( "type", sType.a_str() );
		
		TiXmlNode* newNode = m_pBaseElement->InsertEndChild( value );
		if( newNode ) {
			nextElement = newNode->ToElement();
			if( nextElement ) {
				return nextElement;
			}
		}

	}


	return NULL;
}

bool CNCSPrefsXML::CNCSPrefsKeyXML::Get( CNCSString sName, CNCSString &sValue, CNCSString sDefault )
{
	TiXmlElement* pElement = GetElement( sName, "string" );
	if( pElement ) {
		const char *pValue = pElement->Attribute("value");
		//const char *pValue = pElement->Value();
		if( pValue ) {
			sValue = pValue;
			return true;
		}
	}
	sValue = sDefault;
	return false;
}

bool CNCSPrefsXML::CNCSPrefsKeyXML::Set( CNCSString sName, CNCSString sValue )
{
	TiXmlElement* pElement = GetElement( sName, "string", true );
	if( pElement ) {
		//pElement->SetValue(sValue);
		pElement->SetAttribute("value", sValue.a_str() );
		m_bUpdated = true;
		return true;
	}

	return false;
}

bool CNCSPrefsXML::CNCSPrefsKeyXML::Get( CNCSString sName, int &nValue, int nDefault )
{
	TiXmlElement* pElement = GetElement( sName, "int" );
	if( pElement ) {
		const char *pValue = pElement->Attribute("value", &nValue);
		if( pValue ) {
			return true;
		}
	}
	nValue = nDefault;
	return false;
}

bool CNCSPrefsXML::CNCSPrefsKeyXML::Set( CNCSString sName, int nValue )
{
	TiXmlElement* pElement = GetElement( sName, "int", true );
	if( pElement ) {
		pElement->SetAttribute("value", nValue);
		m_bUpdated = true;
		return true;
	}

	return false;
}


bool CNCSPrefsXML::CNCSPrefsKeyXML::Get( CNCSString sName, double &dValue, double dDefault )
{
	TiXmlElement* pElement = GetElement( sName, "double" );
	if( pElement ) {
		const char *pValue = pElement->Attribute("value", &dValue);
		if( pValue ) {
			return true;
		}
	}

	dValue = dDefault;
	return false;
}

bool CNCSPrefsXML::CNCSPrefsKeyXML::Set( CNCSString sName, double dValue )
{
	TiXmlElement* pElement = GetElement( sName, "double", true );
	if( pElement ) {
		pElement->SetDoubleAttribute("value", dValue);
		m_bUpdated = true;
		return true;
	}

	return false;
}


bool CNCSPrefsXML::CNCSPrefsKeyXML::Get( CNCSString sName, bool &bValue, bool bDefault )
{
	TiXmlElement* pElement = GetElement( sName, "boolean" );
	if( pElement ) {
		const char *pValue = pElement->Attribute("value");
		if( pValue ) {
			if( stricmp( pValue, "true" ) == 0 ||
				strcmp( pValue, "1" ) == 0 )
			{
				bValue = true;
			} else {
				bValue = false;
			}
			return true;
		}
	}

	bValue = bDefault;
	return false;
}

bool CNCSPrefsXML::CNCSPrefsKeyXML::Set( CNCSString sName, bool bValue )
{
	TiXmlElement* pElement = GetElement( sName, "boolean", true );
	if( pElement ) {
		pElement->SetAttribute("value", (bValue?"true":"false"));
		m_bUpdated = true;
		return true;
	}

	return false;
}

bool CNCSPrefsXML::CNCSPrefsKeyXML::DeleteValue( CNCSString sName )
{
	int nCount=0;
	bool bResult = false;

	do {
		TiXmlElement* pElement = GetElement( sName, "" );
		if( pElement ) {
			nCount++;
			bResult = m_pBaseElement->RemoveChild(pElement); 
		} else {
			bResult = false;
		}
	} while( bResult );

	if( nCount > 0 ) {
		m_bUpdated = true;
		return true;
	}

	return false;
}

bool CNCSPrefsXML::CNCSPrefsKeyXML::DeleteSubKey( CNCSString sSubKey, bool bSubKeys )
{
	return false;
}
