//*****************************************************************************
// File name: vcsl_length_unit_ref.h
// Description: Smart pointer on a vcsl_length_unit
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_LENGTH_UNIT_REF_H
#define VCSL_LENGTH_UNIT_REF_H

class vcsl_length_unit;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_length_unit> vcsl_length_unit_ref;

#endif // #ifndef VCSL_LENGTH_UNIT_REF_H
