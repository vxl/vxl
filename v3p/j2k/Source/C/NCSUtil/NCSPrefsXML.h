#include "../tinyxml/tinyxml.h"

using namespace std;

class NCS_EXPORT CNCSPrefsXML : public CNCSThread, public CNCSPrefs
{
protected:
	CNCSPrefsXML( CNCSString sFilename );
	virtual ~CNCSPrefsXML();

public:
	class NCS_EXPORT CNCSPrefsKeyXML : public CNCSPrefs::CNCSPrefsKey
	{
	protected:
		CNCSPrefsKeyXML( CNCSPrefsXML *pPrefsXML, CNCSString sName, TiXmlElement *tixBaseElement );

	public:
		virtual ~CNCSPrefsKeyXML();
		bool Get( CNCSString sName, CNCSString &sValue, CNCSString sDefault="" );
		bool Set( CNCSString sName, CNCSString sValue );
		bool Get( CNCSString sName, int &nValue, int nDefault=0 );
		bool Set( CNCSString sName, int nValue );
		bool Get( CNCSString sName, double &dValue, double dDefault=0.0 );
		bool Set( CNCSString sName, double dValue );
		bool Get( CNCSString sName, bool &bValue, bool bDefault=false );
		bool Set( CNCSString sName, bool bValue );

		bool DeleteValue( CNCSString sName );
		bool DeleteSubKey( CNCSString sSubKey, bool bSubKeys=false );

		CNCSPrefsKey *GetFirstSubKey();
		CNCSPrefsKey *GetNextSubKey();

		CNCSPrefsKey *OpenSubKey( CNCSString sSubKey, bool bCreate=false )
		{
			return OpenKey( NULL, m_pBaseElement, sSubKey, bCreate );
		}

		static CNCSPrefsKeyXML *OpenKey( CNCSPrefsXML *pPrefsXML, TiXmlElement *tixBaseElement, CNCSString sBaseKey, bool bCreate );

	private:
		static TiXmlElement *OpenKey( CNCSString sKey, TiXmlElement *tixBaseElement, bool bCreate );
		TiXmlElement *GetElement( CNCSString sName, CNCSString sType, bool bCreate=false );

		TiXmlElement *m_pBaseElement;
		TiXmlElement *m_pEnumItem;
		CNCSPrefsXML *m_pPrefsXML;
		bool m_bUpdated;
	};

	CNCSPrefsKey *OpenKey( CNCSString sBaseKey, bool bCreate );
	bool DeleteKey( CNCSString sKey );

	/*
	 * This is the worker thread, it does periodical checks to see if the file
	 * has been updated on disk and also writes changes to disk, if there are any.
	 */
	void Work( void *pData );

	static void Init();

	void Invalidate()
	{
		m_bUpdated = true;
	}

protected:
	bool m_bUpdated;
	TiXmlDocument m_tixDoc;
	CNCSString m_sFilename;

};
