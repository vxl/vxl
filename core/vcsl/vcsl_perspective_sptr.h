//*****************************************************************************
// File name: vcsl_perspective_sptr.h
// Description: Smart pointer on a vcsl_perspective
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/08/23| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_PERSPECTIVE_REF_H
#define VCSL_PERSPECTIVE_REF_H

class vcsl_perspective;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_perspective> vcsl_perspective_sptr;

#endif // #ifndef VCSL_PERSPECTIVE_REF_H
