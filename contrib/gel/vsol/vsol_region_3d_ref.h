//*****************************************************************************
// File name: vsol_region_3d_ref.h
// Description: A smart pointer on a vsol_region_3d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/04| François BERTEL          |Creation
//*****************************************************************************
#ifndef VSOL_REGION_3D_REF_H
#define VSOL_REGION_3D_REF_H

class vsol_region_3d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vsol_region_3d> vsol_region_3d_ref;

#endif // #ifndef VSOL_REGION_3D_REF_H
