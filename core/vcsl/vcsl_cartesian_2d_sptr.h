//*****************************************************************************
// File name: vcsl_cartesian_2d_sptr.h
// Description: Smart pointer on a vcsl_cartesian_2d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_CARTESIAN_2D_REF_H
#define VCSL_CARTESIAN_2D_REF_H

class vcsl_cartesian_2d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_cartesian_2d> vcsl_cartesian_2d_sptr;

#endif // #ifndef VCSL_CARTESIAN_2D_REF_H
