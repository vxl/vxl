//*****************************************************************************
// File name: vsol_triangle_3d_sptr.h
// Description: Smart pointer on a vsol_triangle_3d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/04| François BERTEL          |Creation
//*****************************************************************************
#ifndef VSOL_TRIANGLE_3D_REF_H
#define VSOL_TRIANGLE_3D_REF_H

class vsol_triangle_3d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>
typedef vbl_smart_ptr<vsol_triangle_3d> vsol_triangle_3d_sptr;

#endif // #ifndef VSOL_TRIANGLE_3D_REF_H
