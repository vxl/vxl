//*****************************************************************************
// File name: vcsl_angle_ref.h
// Description: Smart pointer on a vcsl_angle
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_ANGLE_REF_H
#define VCSL_ANGLE_REF_H

class vcsl_angle;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_angle> vcsl_angle_ref;

#endif // #ifndef VCSL_ANGLE_REF_H
