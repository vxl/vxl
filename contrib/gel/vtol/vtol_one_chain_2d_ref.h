//*****************************************************************************
// File name: vtol_one_chain_2d_ref.h
// Description: A smart pointer on a vtol_one_chain_2d
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/16| François BERTEL          |Creation
//*****************************************************************************
#ifndef VTOL_ONE_CHAIN_2D_REF_H
#define VTOL_ONE_CHAIN_2D_REF_H

class vtol_one_chain_2d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<vtol_one_chain_2d> vtol_one_chain_2d_ref;

#endif // #ifndef VTOL_ONE_CHAIN_2D_REF_H
