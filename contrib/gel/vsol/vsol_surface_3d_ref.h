//*****************************************************************************
// File name: vsol_surface_3d_ref.h
// Description: Smart pointer on a vsol_surface_3d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/04| François BERTEL          |Creation
//*****************************************************************************
#ifndef VSOL_SURFACE_3D_REF_H
#define VSOL_SURFACE_3D_REF_H

class vsol_surface_3d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>
typedef vbl_smart_ptr<vsol_surface_3d> vsol_surface_3d_ref;

#endif // #ifndef VSOL_SURFACE_3D_REF_H
