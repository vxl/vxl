//*****************************************************************************
// File name: vcsl_geocentric_sptr.h
// Description: Smart pointer on a vcsl_geocentric
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/29| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_GEOCENTRIC_REF_H
#define VCSL_GEOCENTRIC_REF_H

class vcsl_geocentric;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_geocentric> vcsl_geocentric_sptr;

#endif // #ifndef VCSL_GEOCENTRIC_REF_H
