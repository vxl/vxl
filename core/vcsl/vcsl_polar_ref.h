//*****************************************************************************
// File name: vcsl_polar_ref.h
// Description: Smart pointer on a vcsl_polar
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_POLAR_REF_H
#define VCSL_POLAR_REF_H

class vcsl_polar;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_polar> vcsl_polar_ref;

#endif // #ifndef VCSL_POLAR_REF_H
