/********************************************************
** Copyright 1999-2004 Earth Resource Mapping Ltd.
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
** CREATED:  16 Jan 2004
** AUTHOR:   Gary Noel
** PURPOSE:  Class to hold Epsg PCSkey,projection,datum,name
**			 
** EDITS:    [xx] ddMmmyy NAME COMMENTS
**				  06Jan05 tfl  Changed parsing logic to be more robust, changed class name
** 
*******************************************************/
#include "stdafx.h"
#include "NCSGDTEPSGKey.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSGDTEPSGKey::CNCSGDTEPSGKey(){}

CNCSGDTEPSGKey::~CNCSGDTEPSGKey(){}

CNCSGDTEPSGKey::CNCSGDTEPSGKey(string &Line)
{
	string Tokens[10];
	INT32 nTokenCount = 0;
	const string Delimiters = ",\r\n";
	string::size_type nStartToken, nEndToken;
	string Token;
	string::size_type nLength = Line.length(); 

	nStartToken = Line.find_first_not_of(Delimiters);
	nEndToken = nStartToken;

	//while beginning of a word found
	while(nEndToken <(nLength -1) && (nTokenCount <=3))
	{                     
		//search end of the actual word
		//Ignore spaces if at the beginning
		nStartToken = Line.find_first_not_of(" ",nStartToken);
		nEndToken = Line.find_first_of(Delimiters, nStartToken);

		Token.assign(Line,nStartToken,(nEndToken-nStartToken));
		Tokens[nTokenCount] = Token;
		nStartToken = nEndToken;
		nStartToken = Line.find_first_not_of(Delimiters,nStartToken);

		nTokenCount++;          
	}

	//Set members
	m_nEPSG = atoi(Tokens[0].c_str());
	m_Projection = Tokens[1];
	m_Datum = Tokens[2];	
}
