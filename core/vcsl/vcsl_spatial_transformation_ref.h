//*****************************************************************************
// File name: vcsl_spatial_transformation_ref.h
// Description: Smart pointer on a vcsl_spatial_transformation
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/07/17| François BERTEL          |Creation
//*****************************************************************************
#ifndef VCSL_SPATIAL_TRANSFORMATION_REF_H
#define VCSL_SPATIAL_TRANSFORMATION_REF_H

class vcsl_spatial_transformation;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vcsl_spatial_transformation> vcsl_spatial_transformation_ref;

#endif // #ifndef VCSL_SPATIAL_TRANSFORMATION_REF_H
