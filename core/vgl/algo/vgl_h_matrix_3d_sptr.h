//*****************************************************************************
// File name: vgl_h_matrix_3d_sptr.h
// Description: Smart pointer on a h_matrix_3d 
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2003/03/27| Kongbin Kang (kk@Brown)  |Creation
//*****************************************************************************
#ifndef VGL_H_MATRIX_3D_REF_H
#define VGL_H_MATRIX_3D_REF_H

class vgl_h_matrix_3d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vgl_h_matrix_3d> vgl_h_matrix_3d_sptr;

#endif // #ifndef VGL_H_MATRIX_3D_REF_H
