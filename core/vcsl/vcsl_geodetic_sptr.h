//*****************************************************************************
// File name: vcsl_geodetic_sptr.h
// Description: Smart pointer on a vcsl_geodetic
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/29| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_GEODETIC_REF_H
#define VCSL_GEODETIC_REF_H

class vcsl_geodetic;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_geodetic> vcsl_geodetic_sptr;

#endif // #ifndef VCSL_GEODETIC_REF_H
