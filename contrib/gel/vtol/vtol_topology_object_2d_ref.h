//*****************************************************************************
// File name: vtol_topology_object_2d_ref.h
// Description: A smart pointer on a vtol_topology_object_2d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/16| François BERTEL          |Creation
//*****************************************************************************
#ifndef VTOL_TOPOLOGY_OBJECT_2D_REF_H
#define VTOL_TOPOLOGY_OBJECT_2D_REF_H

class vtol_topology_object_2d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vtol_topology_object_2d> vtol_topology_object_2d_ref;

#endif // #ifndef VTOL_TOPOLOGY_OBJECT_2D_REF_H
