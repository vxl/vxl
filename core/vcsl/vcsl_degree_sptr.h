//*****************************************************************************
// File name: vcsl_degree_sptr.h
// Description: Smart pointer on a vcsl_degree
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/28| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_DEGREE_REF_H
#define VCSL_DEGREE_REF_H

class vcsl_degree;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_degree> vcsl_degree_sptr;

#endif // #ifndef VCSL_DEGREE_REF_H
