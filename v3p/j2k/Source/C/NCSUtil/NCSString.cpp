// NCSString.cpp: implementation of the CNCSString class.
//
//////////////////////////////////////////////////////////////////////

#include "NCSString.h"
#include "NCSUtil.h"
#include <algorithm>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#ifndef WIN32
#define _vsnwprintf vswprintf
#define _vsnprintf vsnprintf
#endif

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSString::CNCSString()
{
	
}

CNCSString::~CNCSString()
{
	
}

//////////////////////////////////////////////////////////////////////
// Members
//////////////////////////////////////////////////////////////////////

// the essential MFC member functions
int CNCSString::Format(const NCSTChar* szFormat,...)
{
	std::vector<NCSTChar> _buffer(_MAX_CHARS);
	va_list argList;
	va_start(argList,szFormat);
#ifdef NCS_BUILD_UNICODE
	int ret = _vsnwprintf(&_buffer[0],_MAX_CHARS,szFormat,argList);
#else
	int ret = _vsnprintf(&_buffer[0],_MAX_CHARS,szFormat,argList);
#endif
	va_end(argList);
	assign(&_buffer[0],ret);
	return ret;
}

// this returns a reference so you can do things like this:
// CNCSString query = "abc";
// query += CNCSString().Format("%d %s",1,"abc");
CNCSString& CNCSString::FormatEx(const NCSTChar* szFormat,...)
{
	std::vector<NCSTChar> _buffer(_MAX_CHARS);
	va_list argList;
	va_start(argList,szFormat);
#ifdef NCS_BUILD_UNICODE
	int ret = _vsnwprintf(&_buffer[0],_MAX_CHARS,szFormat,argList);
#else
	int ret = _vsnprintf(&_buffer[0],_MAX_CHARS,szFormat,argList);
#endif
	va_end(argList);
	assign(&_buffer[0],ret);
	return (*this);
}
// trim the right hand of the string of whitespace characters
CNCSString& CNCSString::TrimRight(const NCSTChar* pszDelimiters)
{
	size_type idx = find_last_not_of(pszDelimiters);
	if (idx != std::string::npos)
	{
		erase(++idx);
	}
	return *this;
}

// trim the right hand of the string of whitespace characters
CNCSString& CNCSString::TrimLeft(const NCSTChar* pszDelimiters)
{
	size_type idx = find_first_not_of(pszDelimiters);
	if (idx != std::string::npos)
	{
		erase(0,idx);
	}
	else
	{
		erase();
	}
	return *this;
}

// trim leading and trailing whitespace
CNCSString& CNCSString::TrimAll(const NCSTChar* pszDelimiters)
{
	return TrimLeft(pszDelimiters).TrimRight(pszDelimiters);
}

// convert to upper and lower case
CNCSString& CNCSString::MakeUpper()	{
#ifdef POSIX
	//Not sure why the transform method does not work on linux/GCC
	CNCSString::iterator itCurr = begin();
	CNCSString::iterator itEnd = end();
	while( itCurr != itEnd ) {
		(*itCurr) = toupper((*itCurr));
		itCurr++;
	}
#else
	transform(begin(),end(),begin(),toupper);
#endif
	return (*this);
}
CNCSString& CNCSString::MakeLower()	{
#ifdef POSIX
	//Not sure why the transform algorithm doesn't work on linux/GCC
	CNCSString::iterator itCurr = begin();
	CNCSString::iterator itEnd = end();
	while( itCurr != itEnd ) {
		(*itCurr) = tolower((*itCurr));
		itCurr++;
	}
#else
	transform(begin(),end(),begin(),tolower);
#endif
	return (*this);
}

int CNCSString::Split(const CNCSString& delimiter, std::vector<CNCSString>& results)
{
	return Split( *this, delimiter, results );
}

bool CNCSString::CompareNoCase( const CNCSString& input )
{
	if( CompareNoCase( *this, input ) == 0 ) return true;
	else return false;
}

bool CNCSString::CompareNoCase( const CNCSString& input, int nChars )
{
	if( CompareNoCase( *this, input, nChars ) == 0 ) return true;
	else return false;
}

//////////////////////////////////////////////////////////////////////
// Static
//////////////////////////////////////////////////////////////////////

int CNCSString::CompareNoCase( const CNCSString& first, const CNCSString& second )
{
#ifdef NCS_BUILD_UNICODE
	return wcsicmp( first.c_str(), second.c_str() );
#else
	return stricmp( first.c_str(), second.c_str() );
#endif
}

int CNCSString::CompareNoCase( const CNCSString& first, const CNCSString& second, int nChars )
{
#ifdef NCS_BUILD_UNICODE
	return wcsnicmp( first.c_str(), second.c_str(), nChars );
#else
	return strnicmp( first.c_str(), second.c_str(), nChars );
#endif
}

int CNCSString::Split(const CNCSString& input, const CNCSString& delimiter, vector<CNCSString>& results)
{
	int iPos = -1;
	int newPos = -1;
	int sizeS2 = (int)delimiter.size();
	int isize = (int)input.size();

	vector<int> positions;

	newPos = (int)input.find (delimiter, 0);

	if( newPos < 0 ) {
		// None found, just return full string in result
		results.push_back(input);
		return 0;
	}

	int numFound = 0;

	while( newPos > iPos )
	{
		numFound++;
		positions.push_back(newPos);
		iPos = newPos;
		newPos = (int)input.find (delimiter, iPos+sizeS2+1);
	}

	for( unsigned int i=0; i <= positions.size(); i++ )
	{
		CNCSString s;
		if( i == 0 ) {
			s = input.substr( i, positions[i] );
		} else {
			int offset = positions[i-1] + sizeS2;
			if( offset < isize )
			{
				if( i == positions.size() )
				{
					s = input.substr(offset);
				}
				else if( i > 0 )
				{
					s = input.substr( positions[i-1] + sizeS2, positions[i] - positions[i-1] - sizeS2 );
				}
			}
		}
		if( s.size() > 0 )
		{
			results.push_back(s);
		}
	}
	return numFound;
}

INT32 CNCSString::ToINT32()
{
	INT32 nResult=0;
#ifdef NCS_BUILD_UNICODE
	swscanf( c_str(), NCS_T("%d"), &nResult );
#else
	sscanf( c_str(), "%d", &nResult );
#endif
	return nResult;
}

IEEE8 CNCSString::ToIEEE8()
{
	IEEE8 dResult=0;
#ifdef NCS_BUILD_UNICODE
	swscanf( c_str(), NCS_T("%lf"), &dResult );
#else
	sscanf( c_str(), "%lf", &dResult );
#endif
	return dResult;
}

#ifdef NCS_BUILD_UNICODE

CNCSString CNCSString::Substr(size_type pos, size_type n) const
{
	CNCSString sSubStr = this->substr( pos, n );
	return sSubStr;
}

CNCSString& CNCSString::operator=(const char* pArg)
{
	erase();
	if( pArg ) {
		UINT32 nStrLen = strlen( pArg );
		reserve( nStrLen+1 );
		NCSTChar wideChars[2] = NCS_T("");
		for( size_t i=0; i < nStrLen; i++ ) {
			wideChars[0] = (NCSTChar)(pArg[i]);
			append( wideChars );
		}
	}
	return (*this);
}

CNCSString& CNCSString::operator+=(const char* pArg)
{
	if( pArg ) {
		CNCSString sAppend = pArg;
		append( sAppend );
	}
	return (*this);
}

CNCSString::CNCSString(const char* pArg)
{
	(*this) = pArg?pArg:"";
}

const char *CNCSString::a_str()
{
	m_sAsciiString.erase();
	int nSize = (int)this->size();
	m_sAsciiString.reserve( nSize+1 );
	const NCSTChar *pSrc = c_str();
	char szValue[2] = "a";
	for( int i=0; i < nSize; i++ ) {
		szValue[0] = (char)pSrc[i];
		m_sAsciiString.append( szValue );
	}

	return m_sAsciiString.c_str();
}

#endif

int CNCSString::ReplaceAll( const CNCSString& match, const CNCSString& value )
{
	int nCount = 0;
	int nMatchPos = find( match );
	while( nMatchPos != CNCSString::npos ) {
		nCount++;
		replace( nMatchPos, match.size(), value );
		
		nMatchPos = find( match );
	}

	return nCount;
}
