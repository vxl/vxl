#ifndef NCSSTRING_H
#define NCSSTRING_H

// shut compiler up!
#ifdef WIN32
	#pragma warning(disable:4786)
	#pragma warning(disable:4251)
	#pragma warning(disable:4275)
#endif

#ifdef __cplusplus

#include "NCSDefs.h"
#include "NCSMisc.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

#ifdef NCS_BUILD_UNICODE
class NCS_EXPORT CNCSString : public std::wstring
#else
class NCS_EXPORT CNCSString : public std::string
#endif
{
	// size of Format() buffer.
	enum { _MAX_CHARS = 8096 };

public:

	CNCSString();
	~CNCSString();

	// various constructor flavours
	CNCSString(const CNCSString& arg)	{ assign(arg.c_str()); }
#ifdef NCS_BUILD_UNICODE
	CNCSString(const std::wstring& arg)	{ assign(arg); }
	CNCSString(const char* pArg);
	CNCSString(const std::string& arg)	{ (*this)=arg.c_str(); }
#else
	CNCSString(const std::string& arg)	{ assign(arg); }
#endif
	CNCSString(const NCSTChar* pArg)		{ assign(pArg?pArg:NCS_T("")); }

	// various assignment operator flavours
	CNCSString& operator=(const NCSTChar* pArg)		{ assign(pArg?pArg:NCS_T("")); return (*this); }
#ifdef NCS_BUILD_UNICODE
	CNCSString& operator=(const char* pArg);
	CNCSString& operator=(const std::wstring& arg)	{ assign(arg); return (*this); }
	CNCSString& operator=(const std::string& arg)	{ (*this)=arg.c_str(); return (*this); }
#else
	CNCSString& operator=(const std::string& arg)	{ assign(arg); return (*this); }
#endif
	CNCSString& operator=(const CNCSString& arg)	{ assign(arg.c_str()); return (*this); }

	CNCSString& operator+=(const NCSTChar* pArg)	{ append(pArg?pArg:NCS_T("")); return (*this); }
#ifdef NCS_BUILD_UNICODE
	CNCSString& operator+=(const char* pArg);
	CNCSString& operator+=(const std::wstring& arg)	{ append(arg); return (*this); }
	CNCSString& operator+=(const std::string& arg)	{ (*this)+=arg.c_str(); return (*this); }
#else
	CNCSString& operator+=(const std::string& arg)	{ assign(arg); return (*this); }
#endif

	// the essential MFC member functions
	int Format(const NCSTChar* szFormat,...);

	// this returns a reference so you can do things like this:
	// CNCSString query = "abc";
	// query += CNCSString().Format("%d %s",1,"abc");
	CNCSString& FormatEx(const NCSTChar* szFormat,...);

		// trim the right hand of the string of whitespace characters
	CNCSString& TrimRight(const NCSTChar* pszDelimiters = NCS_T(" \t\r\n") );

    // trim the right hand of the string of whitespace characters
    CNCSString& TrimLeft(const NCSTChar* pszDelimiters = NCS_T(" \t\r\n") );

    // trim leading and trailing whitespace
    CNCSString& TrimAll(const NCSTChar* pszDelimiters = NCS_T(" \t\r\n") );

	// convert to upper and lower case
	CNCSString& MakeUpper();
	CNCSString& MakeLower();

	INT32 ToINT32();
	IEEE8 ToIEEE8();

	bool CompareNoCase( const CNCSString& input );
	bool CompareNoCase( const CNCSString& input, int nChars );

	int Split(const CNCSString& delimiter, std::vector<CNCSString>& results);

	static int Split(const CNCSString& input, const CNCSString& delimiter, std::vector<CNCSString>& results);
	static int CompareNoCase( const CNCSString& first, const CNCSString& second );
	static int CompareNoCase( const CNCSString& first, const CNCSString& second, int nChars );

#ifdef NCS_BUILD_UNICODE
	CNCSString Substr(size_type pos = 0, size_type n = npos) const;

	const char *a_str();
	std::string m_sAsciiString;
#else
	inline CNCSString Substr(size_type pos = 0, size_type n = npos) const
	{
		return substr( pos, n );
	}

	const char *a_str() { return c_str(); };
#endif

	int ReplaceAll( const CNCSString& match, const CNCSString& value );

};

#endif //__cplusplus

#endif //NCSSTRING_H
