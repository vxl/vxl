//*****************************************************************************
// File name: vcsl_lambertian_sptr.h
// Description: Smart pointer on a vcsl_lambertian
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/30| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_LAMBERTIAN_REF_H
#define VCSL_LAMBERTIAN_REF_H

class vcsl_lambertian;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_lambertian> vcsl_lambertian_sptr;

#endif // #ifndef VCSL_LAMBERTIAN_REF_H
