//*****************************************************************************
// File name: vsol_group_2d_ref.h
// Description: A smart pointer on a vsol_group_2d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/04/25| François BERTEL          |Creation
//*****************************************************************************
#ifndef VSOL_GROUP_2D_REF_H
#define VSOL_GROUP_2D_REF_H

class vsol_group_2d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vsol_group_2d> vsol_group_2d_ref;

#endif // #ifndef VSOL_GROUP_2D_REF_H
