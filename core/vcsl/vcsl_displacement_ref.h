//*****************************************************************************
// File name: vcsl_displacement_ref.h
// Description: Smart pointer on a vcsl_displacement
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/08/05| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_DISPLACEMENT_REF_H
#define VCSL_DISPLACEMENT_REF_H

class vcsl_displacement;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_displacement> vcsl_displacement_ref;

#endif // #ifndef VCSL_DISPLACEMENT_REF_H
