//*****************************************************************************
// File name: vsol_line_3d_sptr.h
// Description: A smart pointer on a vsol_line_3d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/03| François BERTEL          |Creation
//*****************************************************************************
#ifndef VSOL_LINE_3D_REF_H
#define VSOL_LINE_3D_REF_H

class vsol_line_3d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vsol_line_3d> vsol_line_3d_sptr;

#endif // #ifndef VSOL_LINE_3D_REF_H
