//*****************************************************************************
// File name: vsol_conic_2d_ref.h
// Description: A smart pointer on a vsol_conic_2d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/01| François BERTEL          |Creation
//*****************************************************************************
#ifndef VSOL_CONIC_2D_REF_H
#define VSOL_CONIC_2D_REF_H

class vsol_conic_2d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vsol_conic_2d> vsol_conic_2d_ref;

#endif // #ifndef VSOL_CONIC_2D_REF_H
