//*****************************************************************************
// File name: vsol_line_2d_ref.h
// Description: A smart pointer on a vsol_line_2d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/04/28| François BERTEL          |Creation
//*****************************************************************************
#ifndef VSOL_LINE_2D_REF_H
#define VSOL_LINE_2D_REF_H

class vsol_line_2d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vsol_line_2d> vsol_line_2d_ref;

#endif // #ifndef VSOL_LINE_2D_REF_H
