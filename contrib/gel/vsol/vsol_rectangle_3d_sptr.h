//*****************************************************************************
// File name: vsol_rectangle_3d_sptr.h
// Description: A smart pointer on a vsol_rectangle_3d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/08| François BERTEL          |Creation
//*****************************************************************************
#ifndef VSOL_RECTANGLE_3D_REF_H
#define VSOL_RECTANGLE_3D_REF_H

class vsol_rectangle_3d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vsol_rectangle_3d> vsol_rectangle_3d_sptr;

#endif // #ifndef VSOL_RECTANGLE_3D_REF_H
