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
** FILE:     $Archive: /NCS/Source/C/NCSServerUtil/NCSError.h $
** CREATED:  28/08/2001 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSError class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSERROR_H
#define NCSERROR_H

#ifndef NCSLOG_H
#include "NCSLog.h"
#endif // NCSLOG_H

#ifndef NCSERRORS_H
#include "NCSErrors.h"
#endif // NCSERRORS_H

#ifndef NCSTYPES_H
#include "NCSTypes.h"
#endif // NCSTYPES_H

/**
 * NCSError wrapper class.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.8 $ $Author: russell $ $Date: 2006/01/05 09:15:15 $ 
 */
class NCS_EXPORT CNCSError {
public:
	/**
	 * Overloaded constructor with a heap of parameters (with defaults).
	 * 
	 * @param        eError		NCSError enum value
	 * @param		 pFile		Source file name where object is constructed (usually __FILE__ is used)
	 * @param		 nLine		Source file line # where object is constructed (usually __LINE__ is used)
	 * @param		 eLevel		Logging level to log error to log file
	 * @param		 pText		Optional text to append to error & log message
	 */
	CNCSError(const NCSError eError = NCS_SUCCESS, char *pFile = "" /*__FILE__*/, int nLine = 0 /*__LINE__*/, CNCSLog::NCSLogLevel eLevel = CNCSLog::LOG_LEVEL1, const char *pText = (char*)NULL);
	/**
	 * Copy constructor.
	 * 
	 * @param        Error		Reference to error to construct from
	 */
	CNCSError(const CNCSError &Error);
	/**
	 * Destructor.
	 */
	~CNCSError();

	bool Success() {
		return (m_eError == NCS_SUCCESS);
	}

	/**
	 * Get an error message for the error object, optionally with a formatted error string included..
	 * 
	 * @param        pFormat	Optional printf() style format string
	 * @param		 ...		Optional args to match pFormat
	 * @return       char *		Formatted error string.  Should be freed with NCSFree().
	 * @see          #GetErrorNumber()
	 */
	char *GetErrorMessage(char *pFormat = NULL, ...);
	/**
	 * Get the NCSError enum for this error object.
	 * 
	 * @return       NCSError	Enum value of error
	 * @see          #GetErrorMessage(char *pFormat = NULL, ...)
	 */
	NCSError GetErrorNumber(void) { return(m_eError); }
	/**
	 * Log the error to the log file, if logging is >= the specified log level.
	 * 
	 * @param        eLevel		Log level required before the error should be logged.
	 */
	void Log(CNCSLog::NCSLogLevel eLevel);

	/**
	 * Assign an error to the error object.
	 * 
	 * @param        Error		Reference to error enum to assign to object
	 * @return					Reference to the error object
	 */
	CNCSError& operator =(const CNCSError &Error);
	/**
	 * Compare the errors.
	 * 
	 * @param        Error		Error object to compare to.  Comparison is on error enum alone.
	 * @return					Boolean value informing whether the two errors are the same type
	 */
	inline bool operator ==( const CNCSError &Error ) {
			return(m_eError == Error.m_eError);
		};
	/**
	 * Compare the errors.
	 * 
	 * @param        eError		Error enum value to compare to.  Comparison is on error enum alone.
	 * @return					Boolean value informing whether the two errors are the same type
	 */
	inline bool operator ==( const NCSError eError ) {
			return(m_eError == eError);
		};
	/**
	 * Compare the errors.
	 * 
	 * @param        Error		Error object to compare to.  Comparison is on error enum alone.
	 * @return					Boolean value informing whether the two errors are not the same type
	 */
	inline bool operator !=( const CNCSError &Error ) {
			return(m_eError != Error.m_eError);
		};
	/**
	 * Compare the errors.
	 * 
	 * @param        eError		Error enum value to compare to.  Comparison is on error enum alone.
	 * @return					Boolean value informing whether the two errors are not the same type
	 */
	inline bool operator !=( const NCSError eError ) {
			return(m_eError != eError);
		};
private:
	/**
	 * NCSError enum value for this error.
	 */
	NCSError	m_eError;
	/**
	 * Optional formatted test message for this error.
	 */
	char		*m_pText;
	/**
	 * File this object was created in (overloaded constructor only).
	 * 
	 * @see     #CNCSError(NCSError eError = NCS_SUCCESS, char *pFile = __FILE__, int nLine = __LINE__, CNCSLog::NCSLogLevel eLevel = CNCSLog::LOG_LEVEL1, char *pText = (char*)NULL)
	 */
	const char	*m_pFile;
	/**
	 * Line number this object was created in (overloaded constructor only).
	 * 
	 * @see     #CNCSError(NCSError eError = NCS_SUCCESS, char *pFile = __FILE__, int nLine = __LINE__, CNCSLog::NCSLogLevel eLevel = CNCSLog::LOG_LEVEL1, char *pText = (char*)NULL)
	 */
	int			m_nLine;
};

/**
 * Create a CNCSError instance for the specified error enum, setting the file & line values.
 * 
 * @param        e				NCSError enum value
 */
#define NCSERROR(e)				CNCSError(e, __FILE__, __LINE__)
/**
 * Create a CNCSError instance for the specified error enum, setting the file & line values, and write it to the log.
 * 
 * @param        e				NCSError enum value
 */
#define NCSERRORLOG(e)			CNCSError(e, __FILE__, __LINE__, CNCSLog::LOG_LEVEL0)
/**
 * Create a CNCSError instance for the specified error enum, setting the file & line values, with some additional text.
 * 
 * @param        e				NCSError enum value
 */
#define NCSERRORTXT(e, t)		CNCSError(e, __FILE__, __LINE__, CNCSLog::LOG_LEVEL1, t)
/**
 * Create a CNCSError instance for the specified error enum, setting the file & line values, with some additional text, and write it to the log.
 * 
 * @param        e				NCSError enum value
 */
#define NCSERRORLOGTXT(e, t)	CNCSError(e, __FILE__, __LINE__, CNCSLog::LOG_LEVEL0, t)

#endif // NCSERROR_H
