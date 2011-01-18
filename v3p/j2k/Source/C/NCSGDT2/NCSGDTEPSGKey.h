/****************************************************
** Copyright 1998 Earth Resource Mapping Ltd.
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
****************************************************/
// NCSGDTEPSGKey.h: interface for the CNCSGDTEPSGKey class.
//
//////////////////////////////////////////////////////////////////////

#ifndef NCSGDTEPSGKEY_H
#define NCSGDTEPSGKEY_H

#include <string>

using std::string;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNCSGDTEPSGKey  
{
public:
	CNCSGDTEPSGKey();
	virtual ~CNCSGDTEPSGKey();
	CNCSGDTEPSGKey(string &Line);

	INT32 GetEPSG() const {return m_nEPSG;}
	string GetProjection() const {return m_Projection;}
	string GetDatum() const {return m_Datum;}


private:
	string m_Projection;
	string m_Datum;
	INT32  m_nEPSG;

};

#endif //NCSGDTEPSGKEY_H
