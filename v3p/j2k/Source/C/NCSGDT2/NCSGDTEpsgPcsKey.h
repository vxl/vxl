// NCSGDTEpsgPcsKey.h: interface for the NCSGDTEpsgPcsKey class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NCSGDTEPSGPCSKEY_H__E43A4013_1DCE_442A_899D_70510A3D2B2E__INCLUDED_)
#define AFX_NCSGDTEPSGPCSKEY_H__E43A4013_1DCE_442A_899D_70510A3D2B2E__INCLUDED_

#include <string>

using std::string;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class NCSGDTEpsgPcsKey  
{
public:
	NCSGDTEpsgPcsKey();
	virtual ~NCSGDTEpsgPcsKey();
	NCSGDTEpsgPcsKey(string & str1);

	int GetPcsKey() const {return itsPcsKey;}
	string GetProjection() const {return itsProjection;}
	string GetDatum() const {return itsDatum;}


private:
	string itsProjection;
	string itsDatum;
	int    itsPcsKey;

};

#endif // !defined(AFX_NCSGDTEPSGPCSKEY_H__E43A4013_1DCE_442A_899D_70510A3D2B2E__INCLUDED_)
