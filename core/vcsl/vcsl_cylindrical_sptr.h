//*****************************************************************************
// File name: vcsl_cylindrical_sptr.h
// Description: Smart pointer on a vcsl_cylindrical
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_CYLINDRICAL_REF_H
#define VCSL_CYLINDRICAL_REF_H

class vcsl_cylindrical;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_cylindrical> vcsl_cylindrical_sptr;

#endif // #ifndef VCSL_CYLINDRICAL_REF_H
