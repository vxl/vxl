//*****************************************************************************
// File name: vsol_triangle_2d_ref.h
// Description: A smart pointer on a vsol_triangle_2d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/02| François BERTEL          |Creation
//*****************************************************************************
#ifndef VSOL_TRIANGLE_2D_REF_H
#define VSOL_TRIANGLE_2D_REF_H

class vsol_triangle_2d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vsol_triangle_2d> vsol_triangle_2d_ref;

#endif // #ifndef VSOL_TRIANGLE_2D_REF_H
