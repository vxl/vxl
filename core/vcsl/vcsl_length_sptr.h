//*****************************************************************************
// File name: vcsl_length_sptr.h
// Description: Smart pointer on a vcsl_length
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_LENGTH_REF_H
#define VCSL_LENGTH_REF_H

class vcsl_length;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_length> vcsl_length_sptr;

#endif // #ifndef VCSL_LENGTH_REF_H
