//*****************************************************************************
// File name: vtol_vertex_ref.h
// Description: A smart pointer on a vtol_vertex
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/16| François BERTEL          |Creation
//*****************************************************************************
#ifndef VTOL_VERTEX_REF_H
#define VTOL_VERTEX_REF_H

class vtol_vertex;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vtol_vertex> vtol_vertex_ref;

#endif // #ifndef VTOL_VERTEX_REF_H
