//*****************************************************************************
// File name: vsol_point_2d_ref.h
// Description: Smart pointer on a vsol_point_2d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/04/25| François BERTEL          |Creation
//*****************************************************************************
#ifndef VSOL_POINT_2D_REF_H
#define VSOL_POINT_2D_REF_H

class vsol_point_2d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vsol_point_2d> vsol_point_2d_ref;

#endif // #ifndef VSOL_POINT_2D_REF_H
