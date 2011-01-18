/********************************************************
** Copyright 2003 Earth Resource Mapping Ltd.
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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCDump.cpp $
** CREATED:  12/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCDump class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifdef _DEBUG
#include "NCSJPCDump.h"
#include "NCSUtil.h"

#ifdef WIN32
#include <tchar.h>
#endif // WIN32


// Default constructor, initialises members
CNCSJPCDump::CNCSJPCDump()
{
}

// Virtual destructor
CNCSJPCDump::~CNCSJPCDump()
{
}

// Dump the formated varargs to stderr & log.
CNCSError CNCSJPCDump::Dump(CNCSLog::NCSLogLevel eLevel, NCSTChar *pFormat, va_list va)
{
	NCSTChar buf[4096];

	_vstprintf(buf, pFormat, va);

#ifdef WIN32
	OutputDebugString(buf);
#else
	_ftprintf(stderr, buf);
	fflush(stderr);
#endif
	va_end(va);

	if(eLevel <= CNCSLog::GetLogLevel()) {	
		CNCSLog::Log(CHAR_STRING(buf));
	}
	return(NCS_SUCCESS);
}

#ifdef NCS_BUILD_UNICODE
// Dump the formated varargs to stderr & log.
CNCSError CNCSJPCDump::Dump(CNCSLog::NCSLogLevel eLevel, wchar_t *pFormat, ...)
{
	va_list va;
	va_start(va, pFormat);
	return(Dump(eLevel, CHAR_STRING(pFormat), va));
}
#endif

// Dump the formated varargs to stderr & log.
CNCSError CNCSJPCDump::Dump(CNCSLog::NCSLogLevel eLevel, char *pFormat, ...)
{
	va_list va;
	va_start(va, pFormat);
	return(Dump(eLevel, OS_STRING(pFormat), va));
}

#endif //_DEBUG
