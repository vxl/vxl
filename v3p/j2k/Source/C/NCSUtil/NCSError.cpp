/********************************************************
** Copyright 2001 Earth Resource Mapping Ltd.
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
** FILE:     $Archive: /NCS/Source/C/NCSServerUtil/NCSError.cpp $
** CREATED:  28/08/2001 3:43:12 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSError class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

// Includes
#include <stdio.h>
#include <stdarg.h>

#ifndef NCSUTIL_H
#include "NCSUtil.h"
#endif //NCSUTIL_H

#include "NCSError.h"

// 
// Overloaded constructor with lotsa (default) parameters
//
CNCSError::CNCSError(const NCSError eType, char *pFile, int nLine, CNCSLog::NCSLogLevel eLevel, const char *pText)
{
	m_eError = eType;
	m_pText = pText ? NCSStrDup((char*)pText) : (char*)NULL;
	m_pFile = pFile;
	m_nLine = nLine;

	if(m_eError != NCS_SUCCESS) {
		Log(eLevel);
	}
}

//
// Copy Constructor
//
CNCSError::CNCSError(const CNCSError &Error)
{
	m_eError = Error.m_eError;
	m_pText = Error.m_pText ? NCSStrDup(Error.m_pText) : (char*)NULL;
	m_pFile = Error.m_pFile;
	m_nLine = Error.m_nLine;
}

//
// Destructor
//
CNCSError::~CNCSError()
{
	if(m_pText) {
		NCSFree(m_pText);
	}
}

//
// Return a formatted error message, possibly with additional formatted text as specified
//
char *CNCSError::GetErrorMessage(char *pFormat, ...) 
{
	char buf[1024];

	snprintf(buf, sizeof(buf), "An error has occurred: Error %ld \"%s\" %s file \"%s\" line %ld", m_eError, NCSGetErrorText(m_eError), m_pText ? m_pText : "", m_pFile, m_nLine);
	
	if(pFormat) {
		char buf2[1024];

		va_list va;

		va_start(va, pFormat);
		vsnprintf(buf2, sizeof(buf2), pFormat, va);
		va_end(va);

		strncat(buf, buf2, sizeof(buf) - strlen(buf) - 1);
	}
	return(NCSStrDup(buf));
}

//
// Write the error to the log file.
//
void CNCSError::Log(CNCSLog::NCSLogLevel eLevel)
{
	CNCSLog::Log((char*)m_pFile, m_nLine, eLevel, "CNCSError(%ld:%s, %s);", m_eError, NCSGetErrorText(m_eError), m_pText ? m_pText : "");
}

//
// Assignment operator
//
CNCSError& CNCSError::operator =(const CNCSError &Error)
{
	m_eError = Error.m_eError;
	if(m_pText) {
		NCSFree(m_pText);
	}
	m_pText = Error.m_pText ? NCSStrDup(Error.m_pText) : (char*)NULL;
	m_pFile = Error.m_pFile;
	m_nLine = Error.m_nLine;
	return(*this);
}

#ifdef NOTUSED
//
// Eq operator
//
bool CNCSError::operator ==( const CNCSError &Error )
{
	return(m_eError == Error.m_eError);
}

//
// Eq operator
//
bool CNCSError::operator ==( const NCSError eError )
{
	return(m_eError == eError);
}

//
// NEq operator
//
bool CNCSError::operator !=( const CNCSError &Error )
{
	return(m_eError != Error.m_eError);
}

//
// NEq operator
//
bool CNCSError::operator !=( const NCSError eError )
{
	return(m_eError != eError);
}
#endif // NOTUSED
