//*****************************************************************************
// File name: vcsl_radian_ref.h
// Description: Smart pointer on a vcsl_radian
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_RADIAN_REF_H
#define VCSL_RADIAN_REF_H

class vcsl_radian;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_radian> vcsl_radian_ref;

#endif // #ifndef VCSL_RADIAN_REF_H
