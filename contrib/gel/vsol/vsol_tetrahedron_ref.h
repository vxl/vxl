#ifndef vsol_tetrahedron_ref_h_
#define vsol_tetrahedron_ref_h_
//*****************************************************************************
// File name: vsol_tetrahedron_ref.h
// Description: Smart pointer on a vsol_tetrahedron
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/04| François BERTEL          |Creation
//*****************************************************************************

class vsol_tetrahedron;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>
typedef vbl_smart_ptr<vsol_tetrahedron> vsol_tetrahedron_ref;

#endif // vsol_tetrahedron_ref_h_
