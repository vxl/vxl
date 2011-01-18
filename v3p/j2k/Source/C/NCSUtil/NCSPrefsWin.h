#ifndef NCS_PREFS_WIN_H
#define NCS_PREFS_WIN_H

using namespace std;

class NCS_EXPORT CNCSPrefsWin : public CNCSPrefs
{
protected:
	CNCSPrefsWin( bool bUserPref );
	virtual ~CNCSPrefsWin();

public:
	class NCS_EXPORT CNCSPrefsKeyWin : public CNCSPrefs::CNCSPrefsKey
	{
	protected:
		CNCSPrefsKeyWin( CNCSMutex *pmMutex, CNCSString sName, HKEY hKey );

	public:
		virtual ~CNCSPrefsKeyWin();

		bool Get( CNCSString sName, CNCSString &sValue, CNCSString sDefault=NCS_T("") );
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
			return OpenKey( NULL, m_hBaseKey, sSubKey, bCreate );
		}

		static CNCSPrefsKeyWin *OpenKey( CNCSMutex *pmMutex, HKEY hBaseKey, CNCSString sSubKey, bool bCreate );
		static bool DeleteKey( HKEY hBaseKey, CNCSString sSubKey );

	protected:
		int m_nEnumKeyIndex;

	private:
		HKEY m_hBaseKey;
	};

	CNCSPrefsKey *OpenKey( CNCSString sBaseKey, bool bCreate );
	bool DeleteKey( CNCSString sKey );

	static void Init();

protected:

	bool m_bUserPref;

};

#endif // NCS_PREFS_WIN_H