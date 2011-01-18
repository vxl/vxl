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
** FILE:     $Archive: /NCS/Source/include/NCSJPCDump.h $
** CREATED:  12/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCDump class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCDUMP_H
#define NCSJPCDUMP_H
#ifdef _DEBUG
#ifndef NCSJPCTYPES_H
#include "NCSJPCTypes.h"
#endif // NCSJPCTYPES_H

#ifndef NCSLOG_H
#include "NCSLog.h"
#endif // NCSLOG_H
#ifndef NCSERROR_H
#include "NCSError.h"
#endif // NCSERROR_H

	/**
	 * CNCSJPCDump class - the JPC dumper class.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.9 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCDump: public CNCSLog {
public:
		/** Default constructor, initialises members */
	CNCSJPCDump();
		/** Virtual destructor */
	virtual ~CNCSJPCDump();

#ifdef NCS_BUILD_UNICODE
		/** 
		 * Dump the formated varargs to stderr & log.
		 * @param		eLevel		LogLevel for dump
		 * @param		pFormat		Format string
		 * @param		...			Varargs for format string
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError Dump(CNCSLog::NCSLogLevel eLevel, wchar_t *pFormat, ...);
#endif
	/** 
		 * Dump the formated varargs to stderr & log.
		 * @param		eLevel		LogLevel for dump
		 * @param		pFormat		Format string
		 * @param		...			Varargs for format string
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError Dump(CNCSLog::NCSLogLevel eLevel, char *pFormat, ...);
private:
		/** 
		 * Dump the formated varargs to stderr & log.
		 * @param		eLevel		LogLevel for dump
		 * @param		pFormat		Format string
		 * @param		va			Varargs list
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError Dump(CNCSLog::NCSLogLevel eLevel, NCSTChar *pFormat, va_list va);
};

#endif // _DEBUG
#endif // !NCSJPCDUMP_H
