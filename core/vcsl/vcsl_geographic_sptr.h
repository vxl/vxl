//*****************************************************************************
// File name: vcsl_geographic_sptr.h
// Description: Smart pointer on a vcsl_geographic
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/29| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_GEOGRAPHIC_REF_H
#define VCSL_GEOGRAPHIC_REF_H

class vcsl_geographic;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_geographic> vcsl_geographic_sptr;

#endif // #ifndef VCSL_GEOGRAPHIC_REF_H
