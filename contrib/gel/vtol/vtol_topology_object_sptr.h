//*****************************************************************************
// File name: vtol_topology_object_sptr.h
// Description: A smart pointer on a vtol_topology_object
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/16| François BERTEL          |Creation
//*****************************************************************************
#ifndef VTOL_TOPOLOGY_OBJECT_REF_H
#define VTOL_TOPOLOGY_OBJECT_REF_H

class vtol_topology_object;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vtol_topology_object> vtol_topology_object_sptr;

#endif // #ifndef VTOL_TOPOLOGY_OBJECT_REF_H
