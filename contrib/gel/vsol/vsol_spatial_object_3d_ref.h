//*****************************************************************************
// File name: vsol_spatial_object_3d_ref.h
// Description: Smart pointer to an vsol_spatial_object_3d
//              DUMMY CLASS: THE REAL VERSION IS WRITTEN BY LUIS AND PETER
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/03| François BERTEL          |Creation
//*****************************************************************************
#ifndef VSOL_SPATIAL_OBJECT_3D_REF_H
#define VSOL_SPATIAL_OBJECT_3D_REF_H

class vsol_spatial_object_3d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vsol_spatial_object_3d> vsol_spatial_object_3d_ref;

#endif // #ifndef VSOL_SPATIAL_OBJECT_3D_REF_H
