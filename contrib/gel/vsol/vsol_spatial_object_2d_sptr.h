//*****************************************************************************
// File name: vsol_spatial_object_2d_sptr.h
// Description: Smart pointer to an vsol_spatial_object_2d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/04/25| François BERTEL          |Creation
//*****************************************************************************
#ifndef VSOL_SPATIAL_OBJECT_2D_REF_H
#define VSOL_SPATIAL_OBJECT_2D_REF_H

class vsol_spatial_object_2d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vsol_spatial_object_2d> vsol_spatial_object_2d_sptr;

#endif // #ifndef VSOL_SPATIAL_OBJECT_2D_REF_H
