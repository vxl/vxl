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
** FILE:     $Archive: /NCS/Source/C/NCSServerUtil/NCSObject.cpp $
** CREATED:  3/09/2001 11:53:54 AM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSObject implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

// Includes
#include "NCSUtil.h"
#include "NCSObject.h"

#ifndef NCSLOG_H
#include "NCSLog.h"
#endif // NCSLOG_H

#ifndef NCSTIMESTAMP_H
#include "NCSTimeStamp.h"
#endif // NCSTIMESTAMP_H

//
// Default constructor
//
CNCSObject::CNCSObject()
{
	m_tsCreate = NCSGetTimeStampMs();
	//m_pLife = new CNCSObjectLife(m_tsCreate);
#ifdef _DEBUG
	// Log
	CNCSLog::Log(CNCSLog::LOG_LEVEL3, "Object 0x%lx Created", this);
#endif // _DEBUG
}

//
// Destructor
//
CNCSObject::~CNCSObject()
{
	//delete m_pLife;
#ifdef _DEBUG
	// Log
	CNCSLog::Log(CNCSLog::LOG_LEVEL3, "Object 0x%lx Destroyed, Lifetime %I64us", this, TimeToNow());//m_tsCreate);
#endif // _DEBUG
}

//
// Get the objects construction timestamp
//
NCSTimeStampMs CNCSObject::CreatedAt(void)
{
	// Return construction timestamp
	//return(m_pLife->CreatedAt());
	return m_tsCreate;
}

//
// Get the objects current lifetime in ms.
//
NCSTimeStampMs CNCSObject::TimeToNow(void)
{
	//return(m_pLife->TimeToNow());
	return(NCSGetTimeStampMs() - m_tsCreate);
}

