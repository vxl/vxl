//*****************************************************************************
// File name: vsol_curve_3d_ref.h
// Description: A smart pointer on a vsol_curve_3d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/03| François BERTEL          |Creation
//*****************************************************************************
#ifndef VSOL_CURVE_3D_REF_H
#define VSOL_CURVE_3D_REF_H

class vsol_curve_3d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vsol_curve_3d> vsol_curve_3d_ref;

#endif // #ifndef VSOL_CURVE_3D_REF_H
