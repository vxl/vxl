
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__36B13A15_E22F_424F_83F4_EA98355A64D2__INCLUDED_)
#define AFX_STDAFX_H__36B13A15_E22F_424F_83F4_EA98355A64D2__INCLUDED_


#ifdef WIN32

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning (disable : 4786)
#pragma warning (disable : 4251)

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

#endif

// TODO: reference additional headers your program requires here
//GDT Specific
#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>

#include "NCSTypes.h"
#include "NCSUtil.h"

#ifdef PI
	#undef PI
#endif

#define    PI       3.14159265358979323846

#ifdef HALF_PI
	#undef HALF_PI
#endif

#define    HALF_PI     (PI/2.0)
#define    TWO_PI      (PI * 2.0)
#define    RAD_DEGREE       (PI/180.0)
#define    RAD_MINUTE       (RAD_DEGREE / 60.0)
#define    RAD_SECOND       (RAD_DEGREE / 3600.0)

#define	   LATITUDE_MIN -PI*0.5;
#define	   LATITUDE_MAX PI*0.5;

#define NEAR_ZERO    1.0e-7
#define NEAR_ZERO_10    1.0e-10
#define RADIAN_TOL   1.0e-12
#define METRE_TOL   1.0e-4
#define MAX_ITERS    100

#define NEAR_POLE(x)    ((fabs(PI/2.0 - fabs((x))) > 1.0e-10) ? 0 : 1)
#define NEAR_EQUATOR(x) ((fabs(0.0 - fabs((x))) > 1.0e-10) ? 0 : 1)

typedef std::vector<char*> gdtLine;
typedef std::vector<gdtLine*> gdtTable;

	//used in both NCSGDTCoordSystem and NCSGDTTransformation
	enum CoordType{COORD_TYPE_PROJECTION,COORD_TYPE_FIELD,COORD_TYPE_GEODETIC,COORD_TYPE_GEOCENTRIC};

//GDT SPecific
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#ifdef MACINTOSH
#	define NOEXCEPTIONS
#endif //MACINTOSH

#endif // !defined(AFX_STDAFX_H__36B13A15_E22F_424F_83F4_EA98355A64D2__INCLUDED_)

