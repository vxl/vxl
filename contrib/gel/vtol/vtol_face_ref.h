//*****************************************************************************
// File name: vtol_face_ref.h
// Description: A smart pointer on a vtol_face
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/15| François BERTEL          |Creation
//*****************************************************************************
#ifndef VTOL_FACE_REF_H
#define VTOL_FACE_REF_H

class vtol_face;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vtol_face> vtol_face_ref;

#endif // #ifndef VTOL_FACE_REF_H
