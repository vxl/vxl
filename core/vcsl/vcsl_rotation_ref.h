//*****************************************************************************
// File name: vcsl_rotation_ref.h
// Description: Smart pointer on a vcsl_rotation
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/08/04| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_ROTATION_REF_H
#define VCSL_ROTATION_REF_H

class vcsl_rotation;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_rotation> vcsl_rotation_ref;

#endif // #ifndef VCSL_ROTATION_REF_H
