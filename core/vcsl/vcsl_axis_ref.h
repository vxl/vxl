//*****************************************************************************
// File name: vcsl_axis_ref.h
// Description: Smart pointer on a vcsl_axis
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_AXIS_REF_H
#define VCSL_AXIS_REF_H

class vcsl_axis;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_axis> vcsl_axis_ref;

#endif // #ifndef VCSL_AXIS_REF_H
