//*****************************************************************************
// File name: vcsl_spatial_sptr.h
// Description: Smart pointer on a vcsl_spatial
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/07/10| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_SPATIAL_REF_H
#define VCSL_SPATIAL_REF_H

class vcsl_spatial;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_spatial> vcsl_spatial_sptr;

#endif // #ifndef VCSL_SPATIAL_REF_H
