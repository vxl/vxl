//*****************************************************************************
// File name: vcsl_spheroid_ref.h
// Description: Smart pointer on a vcsl_spheroid
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/29| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_SPHEROID_REF_H
#define VCSL_SPHEROID_REF_H

class vcsl_spheroid;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_spheroid> vcsl_spheroid_ref;

#endif // #ifndef VCSL_SPHEROID_REF_H
