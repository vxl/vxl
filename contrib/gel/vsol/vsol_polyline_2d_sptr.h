//*****************************************************************************
// File name: vsol_polyline_2d_sptr.h
// Description: A smart pointer on a vsol_polyline_2d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2002/04/22| Amir Tamrakar            |Creation
//*****************************************************************************
#ifndef VSOL_POLYLINE_2D_REF_H
#define VSOL_POLYLINE_2D_REF_H

class vsol_polyline_2d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vsol_polyline_2d> vsol_polyline_2d_sptr;

#endif // #ifndef VSOL_POLYLINE_2D_REF_H
