//*****************************************************************************
// File name: vsol_box_2d_ref.h
// Description: Smart pointer on a vsol_box_2d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/15| François BERTEL          |Creation
//*****************************************************************************
#ifndef VSOL_BOX_2D_REF_H
#define VSOL_BOX_2D_REF_H

class vsol_box_2d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vsol_box_2d> vsol_box_2d_ref;

#endif // #ifndef VSOL_BOX_2D_REF_H
