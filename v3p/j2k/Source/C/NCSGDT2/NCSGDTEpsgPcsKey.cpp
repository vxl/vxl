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
** 
*******************************************************/
#include "stdafx.h"
#include "NCSGDTEpsgPcsKey.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NCSGDTEpsgPcsKey::NCSGDTEpsgPcsKey()
{

}

NCSGDTEpsgPcsKey::~NCSGDTEpsgPcsKey()
{

}

NCSGDTEpsgPcsKey::NCSGDTEpsgPcsKey(string & itsString)
{
   string theArray[10];
   int stringCounter = 0;
   const string delims = ",\r\n";
   string::size_type begIdx, endIdx;
   string tempStr;
   int lengthOfString = itsString.length(); 
   
   begIdx = itsString.find_first_not_of(delims);
   endIdx = begIdx;

   int counter = 0;

// while beginning of a word found
    while(endIdx <(lengthOfString -1) && (counter <=3))
    {                     
 // search end of the actual word
			//Igore spaces if at the beginning
		begIdx = itsString.find_first_not_of(" ",begIdx);
		endIdx = itsString.find_first_of(delims, begIdx);
           
		tempStr.assign(itsString,begIdx,(endIdx-begIdx));
		theArray[counter] = tempStr;
		counter++;
		begIdx = endIdx;
        begIdx = itsString.find_first_not_of(delims,begIdx);

		stringCounter++;          
     }

	//Set the Names
	itsPcsKey = atoi(theArray[0].c_str());
	itsProjection = theArray[1];
	itsDatum = theArray[2];	
}
