//*****************************************************************************
// File name: vcsl_coordinate_system_ref.h
// Description: Smart pointer on a vcsl_coordinate_system
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_COORDINATE_SYSTEM_REF_H
#define VCSL_COORDINATE_SYSTEM_REF_H

class vcsl_coordinate_system;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_coordinate_system> vcsl_coordinate_system_ref;

#endif // #ifndef VCSL_COORDINATE_SYSTEM_REF_H
