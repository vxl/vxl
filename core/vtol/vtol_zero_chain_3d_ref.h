//*****************************************************************************
// File name: vtol_zero_chain_3d_ref.h
// Description: A smart pointer on a vtol_zero_chain_3d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/16| François BERTEL          |Creation
//*****************************************************************************
#ifndef VTOL_ZERO_CHAIN_3D_REF_H
#define VTOL_ZERO_CHAIN_3D_REF_H

class vtol_zero_chain_3d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vtol_zero_chain_3d> vtol_zero_chain_3d_ref;

#endif // #ifndef VTOL_ZERO_CHAIN_3D_REF_H
