//*****************************************************************************
// File name: vtol_chain_2d_ref.h
// Description: A smart pointer on a vtol_chain_2d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/30| François BERTEL          |Creation
//*****************************************************************************
#ifndef VTOL_CHAIN_2D_REF_H
#define VTOL_CHAIN_2D_REF_H

class vtol_chain_2d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vtol_chain_2d> vtol_chain_2d_ref;

#endif // #ifndef VTOL_CHAIN_2D_REF_H
