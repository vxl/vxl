/********************************************************
** Copyright 1999 Earth Resource Mapping Ltd.
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
** FILE:   	NCScnet3\NCSException.h
** CREATED:	12-4-2001
** AUTHOR: 	Russell Rogers
** PURPOSE:	
** EDITS:
** [01]
*******************************************************/

// NCSExceptions.h: interface for the CNCSExceptions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(NCSEXCEPTION_H)
#define NCSEXCEPTION_H

#include <string>
#include "NCSErrors.h"

using namespace std;

/** @class CNCSException
 *  @brief throws a NCSError exception
 *
 *  Exception class that can be used to throw a NCSError as an exception
 *  and to get the corresponding error text associated with that error.
 *
 *  @author	Russell Rogers
 *  @date	10 April 2001
 *  @par Edits:
 *
 */

class CNCSException : public exception
{
public:

	string m_sMessage;
	string m_sErrorText;
	string m_sAllText;
	NCSError m_eError;

	CNCSException( string sMessage, NCSError eError )
	{
		m_sMessage = sMessage;
		m_eError = eError;
		if( eError < NCS_MAX_ERROR_NUMBER )
			m_sErrorText = NCSGetErrorText(eError);
		else
			m_sErrorText = "invalid error code!";
		
		m_sAllText = m_sMessage;
		m_sAllText += " ";
		m_sAllText += m_sErrorText;
	}

	CNCSException( NCSError eError )
	{
		m_sMessage = "";
		m_eError = eError;
		if( eError < NCS_MAX_ERROR_NUMBER )
			m_sErrorText = NCSGetErrorText(eError);
		else
			m_sErrorText = "invalid error code!";

		m_sAllText = m_sErrorText;
	}
	virtual ~CNCSException() throw() { }
};

#endif // !defined(NCSEXCEPTION_H)
