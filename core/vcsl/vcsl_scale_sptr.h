//*****************************************************************************
// File name: vcsl_scale_sptr.h
// Description: Smart pointer on a vcsl_scale
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/07/14| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_SCALE_REF_H
#define VCSL_SCALE_REF_H

class vcsl_scale;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_scale> vcsl_scale_sptr;

#endif // #ifndef VCSL_SCALE_REF_H
